/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nevent.c,v 1.28 1991/10/03 15:56:15 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nevent.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nevent.c,v 1.28 1991/10/03 15:56:15 bobg Exp $";
#endif

/* nevent.c - object extension and event handling for Ness 
*/

#include <andrewos.h>	/* for index() */
#include <physical.h>
#include <util.h>		/* for FOLDEDEQ */

#include <im.ih>
#include <init.ih>
#include <view.ih>
#include <proctbl.ih>
#include <menulist.ih>
#include <keymap.ih>
#include <message.ih>
#include <scroll.ih>
#include <matte.ih>
#include <lpair.ih>
#include <arbiterv.ih>
#include <celv.ih>
#include <cel.ih>
#include <value.h>	/* for value_NEWVALUE */
#include <value.ih>
#include <valuev.ih>
#include <atom.ih>
#include <observe.ih>

/* for FrameMark */
#include <text.ih>
#include <textv.ih>
#include <frame.ih>
#include <buffer.ih>

#include <nodeclss.h>
#include <nesssym.ih>
#include <nessmark.ih>
#include <nevent.h>
#include <error.h>
#include <gen.h>
#include <envt.h>
#include <lex.ih>
#include <call.h>	/* for callInitAll */
#include <ness.ih>	/* for CurrNess */
#include <y.tab.h>

/* data for functions reporting event parameters */
static long MouseX = 0;
static long MouseY = 0;
static long MouseAction = -99;
static long SavedClickCount = 1;
static unsigned char *LastKeys = NULL;
static unsigned char *LastMenu = NULL;

static struct classinfo *scrollClass = NULL;
static struct classinfo *matteClass = NULL;
static struct classinfo *frameClass = NULL;
static struct classinfo *bufferClass = NULL;
static struct classinfo *arbiterviewClass = NULL;

static struct init *globalInit = NULL;

#define PTSTOMARK(arg,iar)  ((TType)((struct nessmark *)arg)->header.nessmark_methods \
		== nessmarkHdr) ? TRUE :   \
		RunError(":not a pointer to a mark (uninitialized variable?)", iar);


/* neventInit()
*/
	void
neventInit()
{
}

#define 		MAXNAME 100

	static unsigned char *
CelSymName(name)
	unsigned char *name;
{
	static unsigned char buf[MAXNAME+1];
	strncpy(buf+1, name, MAXNAME-1);
	buf[MAXNAME] = '\0';	/* truncate if too long */
	buf[0] = '&';
	return buf;
}

#undef 		MAXNAME

/* neventStartExtend(currobj, name)
	start compilation of an object extension
	check that there is no prior extension in progress (currobj == NULL)
	establish a new scope for global names
	uniquify the object name string to be different than a string constant
	and Return the unique object nesssym with its info field pointing at an objnode
*/
	struct nesssym *
neventStartExtend(currobj, name)
	struct nesssym *currobj;
	struct nesssym *name;
{
	struct nesssym *newobj;
	boolean new;
	struct classinfo *viewClass = NULL;
	char buf[100];
	boolean isView = FALSE;
	unsigned char *cname = CelSymName(nesssym_ToC(name, 
			&curNess->header.text, buf, sizeof(buf)));
	if (currobj != NULL) 
		ReportError(":'Extend' is not allowed inside an 'extend' block", -1);

	newobj = nesssym_NLocate(cname, name, nesssym_NGetScope(name), &new);
	if ( ! new) {
		genPushScope(nesssym_NGetINode(newobj, objnode)->scope);
		return newobj;
	}

	newobj->flags = flag_xobj;
	newobj->type = Tptr;
	if (strncmp(cname, "&view:", 6) == 0) {
		/* set viewClass from the rest of the name */
		isView = TRUE;
		viewClass = class_Load(cname+6);
		if (viewClass == NULL)
			ReportError(":unknown view name", 0);
	}
	nesssym_NSetINode(newobj, objnode,
			objnode_Create(NULL, genNewScope(), 
				lex_RecentPosition(-1, 0), 999,
				(struct celview *) viewClass,
				NULL, NULL,		/* pe's */
				NULL, NULL, FALSE, isView));	/* data */

	/* link this extend so codeloc entries from ON xxx are attached to the Ness */
	genLinkGlobal(newobj);
	return newobj;
}

/* neventFinishExtend(obj)
	complete processing of EXTEND ... END
	restore script scope
	return the obj
*/
	struct nesssym *
neventFinishExtend(obj)
	struct nesssym *obj;
{
	struct objnode *onode = nesssym_NGetINode(obj, objnode);
	long loc, len;
	loc = lex_RecentPosition(0, &len);
	onode->len = loc + len - onode->loc;
	genPopScope();
	return obj;
}


/* neventStartEvent(currobj, e, spec)
	start processing an ON ... END:
	check that 'currobj' is not null 
	uniqify the name, hang a new eventnode off it,
	save 'e' in the eventnode for check at finish
	start a function definition and put object code loc in event node
	Return the new symbol
*/
	struct nesssym *
neventStartEvent(currobj, e, spec)
	struct nesssym *currobj;
	struct nesssym *e;
	struct nesssym *spec;
{
	struct nesssym *newevent;
	boolean new;
	unsigned char buf[400];
	nesssym_scopeType oldscope = lex_GetScope(lex_GetCurrent()); /* save 
				before changed by genNewScope() */
	static long nextevent = 0;
	struct text *newt;
	struct nessmark *newm;
	char *cname;

	newt = text_New();
	text_AlwaysCopyText(newt, 0, curNess, spec->header.toksym.loc,
			spec->header.toksym.len);
	BackSlashReduce(newt);
	newm = nessmark_New();   
	nessmark_Set(newm, newt, 0, text_GetLength(newt));
	cname = nessmark_ToC(newm);
	nessmark_Destroy(newm);		/* also destroys newt */

	sprintf(buf, "\\ Event %d (%s)", nextevent++, cname);

	if (currobj == NULL) 
		ReportError(":'on' is not allowed outside an 'extend' block", -2);
	newevent = nesssym_NLocate(buf, spec, lex_GetScope(lex_GetCurrent()), &new);
	newevent->flags = flag_event;
	newevent->type = Tfunc;
	nesssym_NSetINode(newevent, eventnode, 
			eventnode_Create(genEnter(),
				oldscope, genNewScope(),
				lex_RecentPosition(-2, 0), 999,
				NULL, e, 
				cname,	/* pass ownership of cname */
				curNess,
				NULL,		/* no TriggerHolder */
				FALSE		/* not enabled yet */
	));
	return newevent;
}


/* neventFinishEvent(event, locals, e)
	complete processing ON ... END
	check 'e' versus value in event's eventnode
	finish the function and do fixups for it 
*/
	struct nesssym *
neventFinishEvent(event, locals, e)
	struct nesssym *event, *locals, *e;
{
	struct eventnode *enode = nesssym_NGetINode(event, eventnode);
	long loc, len;
	if (e != enode->varsym) {
		char buf[50];
		sprintf(buf, "*Should end \"%s\"",  nesssym_NGetName(enode->varsym));
		ReportError(freeze(buf), -1);
	}
	enode->locallist = locals;
	loc = lex_RecentPosition(0, &len);
	enode->srclen = loc + len - enode->srcloc;

	if ( ! genExit(NULL, locals))
		SaveError(":More than 10 arguments and locals", 
			enode->srcloc, enode->srclen);

	genPopScope();
	genLocStore(event);   genLinkGlobal(event);
	return event;
}


/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = *\

	Run Time
	Event information return

\* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

	static union stackelement *
unstackLong(NSP, iar, pLong)
	union stackelement *NSP;
	unsigned char *iar;
	long *pLong;
{
	if (NSP->l.hdr != longHdr)
		RunError(":arg should be an integer", iar);
	*pLong = NSP->l.v;
	return NSPopSpace(longstkelt); 
}


/* unstackObject(NSP, iar, pObj, type)
	pop an object value off the stack and into *pObj
	if 'type' is not NULL, check that *pObj is a non-NULL ptr to the given type
	Return the new value of NSP.
	Use iar for RunError.
	The types of pObj and type are specific to valueview, but the routine can 
	be used to pop any object by using casts on the arguments.
*/
	static union stackelement *
unstackObject(NSP, iar, pObj, type)
	union stackelement *NSP;
	unsigned char *iar;
	struct valueview **pObj;
	struct classinfo *type;
{
	if (NSP->p.hdr != ptrHdr)
		RunError(":arg should be an object", iar);
	if (type == NULL) {}
	else if (NSP->p.v == NULL)
		RunError(":ptr to object is NULL", iar);
	else if (class_IsType(NSP->p.v, type)) 
		*pObj = (struct valueview *)NSP->p.v;
	else if (class_IsType(NSP->p.v, celviewClass)  &&
			class_IsType(celview_GetApplication((struct celview *)NSP->p.v), type))
		*pObj = (struct valueview *)celview_GetApplication((struct celview *)NSP->p.v);
	else if (class_IsType(NSP->p.v, celviewClass)  &&
			class_IsType(celview_GetTrueChild((struct celview *)NSP->p.v), type))
		*pObj = (struct valueview *)celview_GetTrueChild((struct celview *)NSP->p.v);
	else {
		/* XXX duplicates code in call.c:callCFunc */
		char *buf;
		buf = malloc(60 + strlen(class_GetTypeName(NSP->p.v)) 
			+ strlen(class_GetTypeName(type)));
		sprintf(buf, 
			"*object argument is /%s/, but should be /%s/",
			class_GetTypeName(NSP->p.v),
			class_GetTypeName(type));
		RunError(buf, iar);
	}
	return NSPopSpace(ptrstkelt); 
}

/* ProperPtr(ptr, type)
	looks around in neighborhood of ptr for a pointer of the specified type
	If ptr is celview, the neighborhood includes child and TrueChild
	If ptr is view, the neighborhood includes its data object.
*/
	struct basicobject *
ProperPtr(ptr, type)
	struct basicobject *ptr;
	struct classinfo *type;
{
	struct dataobject *dobj;
	struct view *view;
	long i;
	boolean issame;

	if (ptr == NULL) return ptr;
	if (class_IsType(ptr, type)) return ptr;

	if ( ! class_IsType(ptr, viewClass)) {
		/* if ptr is not a view, maybe it is a cel.  
				Try its dataObject */
		if (class_IsType(ptr, celClass)) {
			dobj = cel_GetObject((struct cel *)ptr);
			if (class_IsType(dobj, type))
				return (struct basicobject *)dobj;
		}
		/* maybe it's a buffer */
		if (class_IsType(ptr, CLASS(buffer))) {
			dobj = buffer_GetData((struct buffer *)ptr);
			if (class_IsType(dobj, type))
				return (struct basicobject *)dobj;
		}
		
		return NULL;
	}
	/* it is a view.  If data object is wanted, check this one's */
	if (class_IsType(type, dataobjectClass)) {
		dobj = ((struct view *)ptr)->dataobject;
		if (class_IsType(dobj, type))
			return (struct basicobject *)dobj;
	}


	/* it is a view, try various kinds of containers */

	if (class_IsType(ptr, CLASS(arbiterview))) {
		/* sometimes an arbiter isn't really in the tree */
		/* sometimes it has a child pointer pointing to a bogus place */
		return NULL;
	}

#define NCLASSES 9
	for (i = 1; i <= NCLASSES; i++) {
	    view = NULL;
	    switch (i) {
	    case 1:   if ( (issame = class_IsType(ptr, celviewClass)) ) 
			view = celview_GetApplication((struct celview *)ptr);
		break;

	    case 2: if (issame) 
			view = celview_GetTrueChild((struct celview *)ptr);
		break;

	    case 3:   if ( (issame = class_IsType(ptr, lpairClass)) ) 
			view = lpair_GetNth((struct lpair *)ptr, 0);
		break;

	    case 4: if (issame) 
			view = lpair_GetNth((struct lpair *)ptr, 1);
		break;

	    case 5:   if ( (issame = class_IsType(ptr, CLASS(scroll))) ) 
			view = scroll_GetChild((struct scroll *)ptr);
		break;

	    case 6: if (issame) 
		view = scroll_GetScrollee((struct scroll *)ptr);
		break;

	    case 7: if (class_IsType(ptr, CLASS(matte))) 
			view = ((struct matte *)ptr)->child;
		break;

	    case 8: if (class_IsType(ptr, imClass)) 
			view = ((struct im *)ptr)->topLevel;
		break;

	    case 9: if (class_IsType(ptr, CLASS(frame))) 
			view = frame_GetChildView((struct frame *)ptr);
		break;

	    } /* end of switch(i) */

	    /* test the value of view left by the latest case */
	    if (view != NULL && 
		(view=(struct view *) ProperPtr((struct basicobject *)view, type)) != NULL)
			return (struct basicobject *)view;
	} /* end of for(i) */
#undef NCLASSES

	return NULL;
}

	static struct text *
FrameMark(ness, m, title, pgmnm, enablecommands, pPos, pLen)
	struct ness *ness;
	struct nessmark *m;
	char *pgmnm, *title;
	boolean enablecommands;
	long *pPos, *pLen;
{
	struct text *text;
	struct textview *textview;
	struct buffer *buffer;
	struct frame *frame;
	struct im *window;
	char *oldpgmnm;
	char *lastslash;

	text = (struct text *)nessmark_GetText(m);

	oldpgmnm = im_GetProgramName();
	if (pgmnm != NULL && *pgmnm != '\0')
		im_SetProgramName(pgmnm);

	lastslash = rindex(title, '/');
	if (lastslash == NULL)
		lastslash = title;
	else lastslash++;
	buffer = buffer_Create(lastslash, NULL, NULL, text);
	if ((frame = frame_Create(buffer)) == NULL)
		return NULL;
	if ((window = im_Create(NULL)) == NULL) 
		return NULL;
	im_SetView(window, frame);
	frame_PostDefaultHandler(frame, "message", 	frame_WantHandler(frame, "message"));
	message_DisplayString(frame, 0, "");
	frame_SetBuffer(frame, buffer, TRUE);
	frame_SetCommandEnable(frame, enablecommands);

	textview = (struct textview *)frame_GetView(frame);
	textview_SetDotPosition(textview, nessmark_GetPos(m));
	textview_SetDotLength(textview, nessmark_GetLength(m));
	textview_FrameDot(textview, nessmark_GetPos(m));
	textview_WantInputFocus(textview, textview);

	ness_SetDefaultText(ness, textview);
	ness_SetArbiter(ness, arbiterview_FindArb((struct view *)textview));
	neventPost (ness, FALSE);

	ness_AddObserver(ness, ness);

	im_KeyboardProcessor();	/* DO IT - wait while user interacts*/

	*pPos = textview_GetDotPosition(textview);
	*pLen = textview_GetDotLength(textview);
	textview_Destroy(textview);
	im_SetProgramName(oldpgmnm);
	return text;
}

/* neventInfo(op, iar, ness)
	return information about the most recent interactive input
*/
	void
neventInfo(op, iar, ness)
	unsigned char op;
	unsigned char *iar;
	struct ness *ness;
{
	union stackelement *NSP = NSPstore;
	long v;
	boolean bv;
	unsigned char *s;
	struct view *vptr;
	struct valueview *vvwobj;
	struct value *vobj;

	switch (op) {
	case 'a':
		v = (long)view_LeftDown;
		goto stackLong;
	case 'b':
		v = (long)view_LeftUp;
		goto stackLong;
	case 'c':
		v = (long)view_LeftMovement;
		goto stackLong;
	case 'd':
		v = (long)view_RightDown;
		goto stackLong;
	case 'e':
		v = (long)view_RightUp;
		goto stackLong;
	case 'f':
		v = (long)view_RightMovement;
		goto stackLong;
	case 'k':
		if (LastKeys == NULL)
			RunError(":no keys event in progress", iar);
		s = LastKeys;
		goto stackString;
	case 'm':
		if (LastMenu == NULL)
			RunError(":no menu event in progress", iar);
		s = LastMenu;
		goto stackString;

	case 'n':		/* value_getvalue(obj) => long */
		NSP = unstackObject(NSP, iar, &vvwobj, valueviewClass);
		vobj = (struct value *)vvwobj->header.view.dataobject;
		v = value_GetValue(vobj);
		goto stackLong;
	case 'o':		/* value_getarraysize(obj) => long  */
		NSP = unstackObject(NSP, iar, &vvwobj, valueviewClass);
		vobj = (struct value *)vvwobj->header.view.dataobject;
		v = value_GetArraySize(vobj);
		goto stackLong;
	case 'p':		/* value_getstring(obj) => string */
		NSP = unstackObject(NSP, iar, &vvwobj, valueviewClass);
		vobj = (struct value *)vvwobj->header.view.dataobject;
		s = (unsigned char *)value_GetString(vobj);
		goto stackString;
	case 'q':	{	/* value_getarrayelt(obj, index) => string */
		long index;
		unsigned char **sar;
		NSP = unstackLong(NSP, iar, &index);
		NSP = unstackObject(NSP, iar, &vvwobj, valueviewClass);
		vobj = (struct value *)vvwobj->header.view.dataobject;
		if (index <0  ||  index > value_GetArraySize(vobj))
			RunError(":index out of bounds", iar);
		sar = (unsigned char **)value_GetStringArray(vobj);
		s = sar[index];
	}	goto stackString;
	case 'r':		/* value_setvalue(obj, long) */
		NSP = unstackLong(NSP, iar, &v);
		NSP = unstackObject(NSP, iar, &vvwobj, valueviewClass);
		vobj = (struct value *)vvwobj->header.view.dataobject;
		value_SetValue(vobj, v);
		break;
	case 's':		/* value_setarraysize(obj, long)  */
		NSP = unstackLong(NSP, iar, &v);
		NSP = unstackObject(NSP, iar, &vvwobj, valueviewClass);
		vobj = (struct value *)vvwobj->header.view.dataobject;
		if (v != value_GetArraySize(vobj)) {
			/* create new array of the given size
				XXX never freed  */ 			value_SetStrArrayAndSize(vobj, 
				calloc(v, sizeof(unsigned char *)), v);
					/* init to NULL ptrs */
		}
		break;

	case 't':	{	/* value_setstring(obj, str) */
		PTSTOMARK(&NSP->m, iar);
		s = nessmark_ToC(&NSP->m);
		NSP = popValue(NSP);
		NSP = unstackObject(NSP, iar, &vvwobj, valueviewClass);
		vobj = (struct value *)vvwobj->header.view.dataobject;
		/* store a pointer to the malloced string returned by ToC
			XXX never freed */
		value_SetString(vobj, s);
	}	break;
	case 'u':	{	/* value_setarrayelt(obj, index, str) */
		long index;
		unsigned char **sar;
		PTSTOMARK(&NSP->m, iar);
		s = nessmark_ToC(&NSP->m);
		NSP = popValue(NSP);	/* discard the marker now */
		NSP = unstackLong(NSP, iar, &index);
		NSP = unstackObject(NSP, iar, &vvwobj, valueviewClass);
		vobj = (struct value *)vvwobj->header.view.dataobject;
		if (index <0  ||  index >= value_GetArraySize(vobj))
			RunError(":index out of bounds", iar);
		/* store the pointer to the malloced string into the malloced array
			XXX never freed */
		sar = (unsigned char **)value_GetStringArray(vobj);
		sar[index] = s;
		value_NotifyObservers(vobj, value_NEWVALUE);
	}	break;
	case 'v':		/* value_setnotify(obj, bool) */
		/* NSP = unstackBool(NSP, iar, &v); */
		if (NSP->b.hdr != boolHdr)
			RunError(":arg should be a boolean", iar);
		v = NSP->b.v;
		NSPopSpace(boolstkelt); 
		NSP = unstackObject(NSP, iar, &vvwobj, valueviewClass);
		vobj = (struct value *)vvwobj->header.view.dataobject;
		value_SetNotify(vobj, v);
		break;

	case 'w':		/* mouseaction */
		if (MouseAction == -99)
			RunError(":no mouse event in progress", iar);
		v = MouseAction;
		goto stackLong;
	case 'x':		/* mousex */
		if (MouseAction == -99)
			RunError(":no mouse event in progress", iar);
		v = MouseX;
		goto stackLong;
	case 'y':		/* mousey */
		if (MouseAction == -99)
			RunError(":no mouse event in progress", iar);
		v = MouseY;
		goto stackLong;
	case 'F':		/* currentinputfocus */
		vptr = (im_GetLastUsed() == NULL) ? NULL
				:  im_GetInputFocus(im_GetLastUsed());	
		goto stackPointer;
	case 'H':  {	/* DoHit(inset, action, x, y) */
		/* XXX ought to guarantee an up for every transmitted down */
		long x, y, act;
		struct view *inset;
		static struct view *hitee = NULL, *hiteesource = NULL;
		NSP = unstackLong(NSP, iar, &y);
		NSP = unstackLong(NSP, iar, &x);
		NSP = unstackLong(NSP, iar, &act);
		/* now inset is at top of stack */
		if (NSP->p.hdr != ptrHdr)
			RunError(":arg should be an object", iar);
		inset = (struct view *)NSP->p.v;
		NSPopSpace(ptrstkelt);
		if (inset == NULL  ||  !  class_IsType(inset, viewClass))
			RunError(":first arg should be a view", iar);
		if (ness->CurrentInset == inset  &&  class_IsType(inset, celviewClass)) {
			/* we short cut around the celview to avoid retrapping a mouse hit */
			if (celview_GetApplication((struct celview *)inset) != NULL)
				inset = celview_GetApplication((struct celview *)inset);
			else if (celview_GetTrueChild((struct celview *)inset) != NULL)
				inset = celview_GetTrueChild((struct celview *)inset);
		}

		if (hiteesource != inset  ||  act == (long)view_LeftDown  ||  act == (long)view_RightDown) {
			hiteesource = inset;
			hitee = view_Hit(inset, act, x, y, SavedClickCount);  /* XXX do we need a click count? */
		}
		else {
			/* move or up, with inset the same as where we sent the last hit:
				 send to hitee */
			hitee = view_Hit(hitee,
			    act,
			    physical_GlobalXToLogicalX(view_GetDrawable(hitee),
				physical_LogicalXToGlobalX(view_GetDrawable(inset), x)),
			    physical_GlobalYToLogicalY(view_GetDrawable(hitee),
				physical_LogicalYToGlobalY(view_GetDrawable(inset),  y)),
			    SavedClickCount);	/* XXX do we need a click count? */
		}
	}	break;	/* end case 'H' */
	case 'I':		/* currentinset */
		vptr = ness->CurrentInset;
		if (vptr == NULL) 
			vptr = (struct view *)ness->DefaultText;
		goto stackPointer;
	case 'J':		/* currentwindow */
		vptr = ness->CurrentInset;
		if (vptr == NULL) 
			vptr = (struct view *)ness->DefaultText;
		if (vptr == NULL) 
			goto stackPointer;
		vptr = (struct view *)view_GetIM(vptr);
		goto stackPointer;

 	/* launchApplication(marker, title, programname, enablecommands)
	{"launchApplication", "UP", {Tvoid, Tstr, Tstr, Tstr, Tbool, Tend}, ness_codeGreen}, */

	case 'P':	{
		boolean enablecommands;
		char *pgmnm, * title;
		struct text *text;
		long pos, len;

		if (NSP->l.hdr != boolHdr)
			RunError(":last argument should be a boolean value", iar);
		enablecommands = NSP->b.v;
		NSP = popValue(NSP);

		PTSTOMARK(&NSP->m, iar);
		pgmnm = nessmark_ToC(&NSP->m);
		NSP = popValue(NSP);

		PTSTOMARK(&NSP->m, iar);
		title = nessmark_ToC(&NSP->m);
		NSP = popValue(NSP);

		PTSTOMARK(&NSP->m, iar);
		text = FrameMark(ness, &NSP->m, 
			title, pgmnm, enablecommands, &pos, &len);

		free(pgmnm);
		free(title);
		nessmark_Set(&NSP->m, text, pos, len);
	}	break;


	case 'R':		/* isreadonly(string) */
		PTSTOMARK(&NSP->m, iar);
		bv = simpletext_GetReadOnly(nessmark_GetText(&NSP->m));
		NSP = popValue(NSP);
		goto stackBool;
	case 'T':	{	/* TellUser(msg) */
		unsigned char *buf;
		PTSTOMARK(&NSP->m, iar);
		buf = nessmark_ToC(&NSP->m);
		message_DisplayString(im_GetLastUsed(), 0, buf);
		im_ForceUpdate();
		free(buf);
		NSP = popValue(NSP);
		ness->ToldUser = TRUE;
	}	break;
	case 'U':	{	/* AskUser(prompt, default) */
		unsigned char *prompt, *defaultstring;
		unsigned char buf[301];

		PTSTOMARK(&NSP->m, iar);
		defaultstring = nessmark_ToC(&NSP->m);
		NSP = popValue(NSP);
		PTSTOMARK(&NSP->m, iar);
		prompt = nessmark_ToC(&NSP->m);
		NSP = popValue(NSP);
		if (message_AskForString(im_GetLastUsed(), 0,  prompt, 
				defaultstring, buf, 300) < 0) {
			message_DisplayString(im_GetLastUsed(), 0, "Cancelled.");
			strcpy(buf, "CANCEL");
		}
		s = buf;
		free(prompt);			
		free(defaultstring);
		ness->ToldUser = FALSE;
	}	goto stackString;


	stackBool:
		NSPushSpace(boolstkelt); 
		NSP->b.hdr = boolHdr;
		NSP->b.v = bv;
		break;
	stackLong:
		NSPushSpace(longstkelt); 
		NSP->l.hdr = longHdr;
		NSP->l.v = v;
		break;
	stackString:
		NSPushSpace(nessmark); 
		nessmark_Initialize(&NSP->m);
		nessmark_MakeConst(&NSP->m, s);
		break;
	stackPointer:
		NSPushSpace(ptrstkelt); 
		NSP->p.hdr = ptrHdr;
		NSP->p.v = (struct basicobject *)vptr;
		break;
	}		/* end switch(op) */
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = *\

	Run Time
	Event handlers

\* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

static boolean PostDebug = FALSE;	/* side arg to HandleNamedCell */

	void
InterpretEvent(obj, enode)
	struct view *obj;
	struct eventnode *enode;
{
	struct ness *ness = enode->parentness;
	struct view *RememberCurrentInset;
		/* The "CurrentInset" value in a normal Ness is its text 
			or maybe the inset active when ESC-ESC was picked
		   We save the currentinset value and replace it with the one
		   from the inset that had the event. */
	if ( ! enode->enabled) 
		return;
	ness_EstablishViews(ness, obj);
	RememberCurrentInset = ness->CurrentInset;
	ness->CurrentInset = obj;

	ness->ErrorList = callInitAll(ness);
	if (ness->ErrorList == NULL)
		ness->ErrorList = interpretNess(enode->SysMarkOffset, 
				NULL, ness);
	ness->CurrentInset = RememberCurrentInset;

	if (ness->ErrorList != NULL) {
		neventUnpost(ness, FALSE);	/* remove old postings */
		MapRunError(ness);
		ness_Expose(ness);
	}
}


/* MouseProcStub(obj, action, x, y, nclicks, enode)
	This procedure is called by a mouse event.
	From the xattr it gets everything it needs to initiate the proper Ness stmtList.
	 	these are the possible mouse actions:
			view_NoMouseEvent,
			view_LeftDown, view_LeftUp, view_LeftMovement,
			view_RightDown, view_RightUp, view_RightMovement
*/
	struct view *
MouseProcStub(obj, action, x, y, nclicks, enode)
	struct view *obj;	/* the celview */
	enum view_MouseAction action;
	long x, y, nclicks;
	struct eventnode *enode;
{
	/* XXX we need to scan xattr looking for other mouse event handlers */
	MouseX = x;
	MouseY = y;
	MouseAction = (long)action;
	SavedClickCount = nclicks;
	InterpretEvent(obj, enode);
	MouseAction = -99;
	SavedClickCount = 1;		/* yuck XXX should give this to client */
	return obj;
}

/* MenuProcStub(obj, enode)
	This procedure is called by a menu selection event.  
	From the xattr it gets everything it needs to initiate the proper Ness stmtList.
*/
	void
MenuProcStub(obj, enode)
	struct view *obj;	/* the celview, I think. I will ignore it */
	struct eventnode *enode;
{
	LastMenu = enode->spec;
	InterpretEvent(obj, enode);
	LastMenu = NULL;
}

/* KeyProcStub(obj, enode)
	This procedure is called by a key sequence event.
	From the enode it gets everything it needs to initiate the proper Ness stmtList.
*/
	void
KeyProcStub(obj, enode)
	struct view *obj;	/* the celview, I think. I will ignore it */
	struct eventnode *enode;
{
	LastKeys = enode->spec;
	InterpretEvent(obj, enode);
	LastKeys = NULL;
}


EventEventStub(ness, obj, enode)
	struct ness *ness;
	struct view *obj;
	struct eventnode *enode;
{
	InterpretEvent(obj, enode);
}


/* PostMenuEvent(ness, onode, enode)
	the script 'ness' has an extension for object described by objnode 'onode'
		and having a Menu event 'enode'
	post it to onode->obj
*/
	static void
PostMenuEvent(ness, onode, enode)
	struct ness *ness;
	struct objnode *onode;
	struct eventnode *enode;
{
	unsigned char name[400];
	if (onode->menupe == NULL) {
		/* create (or find) menu pe for this class */
		sprintf(name, "%s-NessMenuEvent",
				class_GetTypeName(onode->obj));
		onode->menupe = proctable_DefineProc(freeze(name),
				MenuProcStub, class_GetType(onode->obj), 
				class_GetTypeName(onode->obj), 
				"Call Ness for a menu event");
	}
	if (onode->menulist == NULL) {
		/* need to create and post the menulist */
		onode->menulist = menulist_Create(onode->obj);
		if (onode->ExtendAView) {
			if (globalInit != NULL)
				init_AddMenuBinding(globalInit, 
					class_GetTypeName(onode->obj), 
					FALSE, onode->menulist);
		}
		else celview_SetMenulist(onode->obj, onode->menulist);
	}
	/* add the key sequence to the keymap */
	menulist_AddToML(onode->menulist, enode->spec, onode->menupe, (long)enode, 0);
	enode->enabled = TRUE;
}

/* PostKeysEvent(ness, onode, enode)
	the script 'ness' has an extension for object described by objnode 'onode'
		and having a Keys event 'enode'
	post it to onode->obj
*/
	static void
PostKeysEvent(ness, onode, enode)
	struct ness *ness;
	struct objnode *onode;
	struct eventnode *enode;
{
	unsigned char name[400];
	if (onode->keype == NULL) {
		/* create (or find) keys pe for this class */
		sprintf(name, "%s-NessKeysEvent",
				class_GetTypeName(onode->obj));
		onode->keype = proctable_DefineProc(freeze(name), 
				KeyProcStub, 	class_GetType(onode->obj), 
				class_GetTypeName(onode->obj), 
				"Call Ness for a keys event");
	}
	if (onode->keymap == NULL) {
		/* need to create and post the keymap */
		onode->keymap = keymap_New();
		if (onode->ExtendAView) {
			if (globalInit != NULL)
				init_AddKeyBinding(globalInit, 
					class_GetTypeName(onode->obj),
					FALSE, onode->keymap);
		}
		else celview_SetKeymap(onode->obj, onode->keymap);
	}
	/* add the key sequence to the keymap */
	keymap_BindToKey(onode->keymap, enode->spec, onode->keype, enode);
	enode->enabled = TRUE;
}

/* PostMouseEvent(ness, onode, enode)
	the script 'ness' has an extension for object described by objnode 'onode'
		and having a Mouse event 'enode'
	post it to onode->obj
*/
	static void
PostMouseEvent(ness, onode, enode)
	struct ness *ness;
	struct objnode *onode;
	struct eventnode *enode;
{
	if (onode->MouseEnabled || onode->ExtendAView) 
		return;
	celview_SetHitfunc(onode->obj, MouseProcStub, enode);
	onode->MouseEnabled = TRUE;
	enode->enabled = TRUE;
}


/* TryTrigger(obj, trigger, ness, enode)
	Try AddRecipient for 'obj' to see if it has the trigger
	If successful set enode->TriggerHolder and enode->enable
	return TRUE for success and FALSE for failure
*/
	static boolean
TryTrigger(obj, trigger, ness, enode)
	struct observable *obj;
	struct atom *trigger;
	struct ness *ness;
	struct eventnode *enode;
{
	if (obj == NULL)
		return FALSE;
	if (observable_AddRecipient(obj, trigger, ness, 
				EventEventStub, enode))  {
		enode->TriggerHolder = obj;
		enode->enabled = TRUE;
		return TRUE;
	}
	if (class_IsType(obj, viewClass)) 
		return TryTrigger(&(((struct view *)obj)->dataobject->header.observable), 
				trigger, ness, enode);
	return FALSE;
}


/*  PostEventEvent(ness, onode, enode)
	post  the event 'enode' to the object given by 'onode', using the 'ness'

	This event derives from   --  on event "..." --  in the source;
	it corresponds to a trigger defined by the target object.

	To process triggers we just do observable_AddRecipient().
	If it returns FALSE, we try another portion of the cel.
	We look at the cel, the celview, the child, and the true child,
	the child's data object, and the true child's data object.
*/
PostEventEvent(ness, onode, enode)
	struct ness *ness;
	struct objnode *onode;
	struct eventnode *enode;
{
	struct atom *trigger;

	if (FOLDEDEQ("becamevisible", enode->spec)) {
		enode->enabled = TRUE;
		return;
	}

	if (onode->ExtendAView) 
		return;
	trigger = atom_Intern(enode->spec);

	if (TryTrigger(&(onode->obj->header.observable), trigger, ness, enode))  
		return;
	if (class_IsType(onode->obj, celviewClass)) {
		struct celview *cv = (struct celview *)onode->obj;
		if (TryTrigger(&(celview_GetTrueChild(cv)->header.observable),
				trigger, ness, enode))  
			return;
		if (celview_GetApplication(cv)  !=  celview_GetTrueChild(cv)
				&&  TryTrigger(&(celview_GetApplication(cv)
						->header.observable),
					trigger, ness, enode))
			return;
	}
	if (class_IsType(onode->obj, imClass)) {
		struct view *v = ((struct im *)onode->obj)->topLevel;
		if (TryTrigger(&(v->header.observable), trigger, ness, enode))
			return;
		if (class_IsType(v, lpairClass)  &&
				TryTrigger(&(lpair_GetNth((struct lpair *)v,
						 0)->header.observable), 
					trigger, ness, enode))
			return;
	}
	/* well, we didn't find it.  
	XXX we ought to post an error message */
}


/* postevents(ness, attr)
	the script 'ness' has an extension for object with name 'attr'
	post all events intended for this object
*/
	static void
postevents(ness, attr)
	struct ness *ness;
	struct nesssym *attr;
{
	boolean repost;
	struct nesssym *xattr;
	struct objnode *onode;
	char buf[100];

	repost = FALSE;

	if (PostDebug)
		printf("Posting events for %s\n",  
			nesssym_ToC(attr, &curNess->header.text, buf, sizeof(buf)));

	/* traverse xattr's, posting each event */
	onode = nesssym_NGetINode(attr, objnode);
	for (xattr = onode->attrs; xattr != NULL; xattr = xattr->next) 
		if (xattr->flags == flag_event) {
			struct eventnode *enode = nesssym_NGetINode(xattr, eventnode);
			if (PostDebug)
				printf("	%s event (%s)\n", 
					nesssym_NGetName(enode->varsym),
					enode->spec);
			xattr->parent.nesssym = attr;	/* ensure that parent is set */
			switch(enode->varsym->header.toksym.toknum) {
			case MENU:
				PostMenuEvent(ness, onode, enode);
				repost = TRUE;
				break;
			case KEYS:
				PostKeysEvent(ness, onode, enode);
				repost = TRUE;
				break;
			case MOUSE:
				PostMouseEvent(ness, onode, enode);
				break;
			case EVENT:
				PostEventEvent(ness, onode, enode);
				break;
			}
		} /* end if (and for) */

	if (repost  && ! onode->ExtendAView)  
		celview_Repost(onode->obj);
}

/* unpostevents(ness, attr, xattr)
	the script 'ness' has an extension for object with name 'attr'
	unpost all its events
*/
	static void
unpostevents(ness, attr, debug)
	struct ness *ness;
	struct nesssym *attr;
	boolean debug;
{
	register struct objnode *onode = nesssym_NGetINode(attr, objnode);
	struct nesssym *xattr;
	boolean repost;
	char buf[100];

	if (onode->ExtendAView) {
		if (onode->menulist != NULL && globalInit != NULL)
			init_DeleteMenuBinding(globalInit, onode->obj, FALSE, 
					onode->menulist);
		if (onode->keymap != NULL && globalInit != NULL)
			init_DeleteKeyBinding(globalInit, onode->obj, FALSE, 
					onode->keymap);
		return;
	}
	/* it is an extended object   (ExtendAView is false)*/

	/* set onode->obj or give error
		look up the object by name again in case it has been cut from the base text */
	if (ness_GetArbiter(ness) == NULL  ||  
			(onode->obj=arbiterview_GetNamedCelview(
					ness_GetArbiter(ness),
					nesssym_ToC(attr, &curNess->header.text, 
						buf, sizeof(buf)))) 
				== NULL) {
		if (debug) {
			printf("unpostevents could not find \"%s\" in arb at 0x%lx\n",
				nesssym_ToC(attr, &curNess->header.text, 
						buf, sizeof(buf)),
				ness_GetArbiter(ness));
		}
		return;
	}

	/* traverse xattr's, disabling each event */
	for (xattr = onode->attrs; xattr != NULL; xattr = xattr->next) {
		struct eventnode *enode 
				= nesssym_NGetINode(xattr, eventnode);
		if (xattr->flags == flag_event) 
			enode->enabled = FALSE;
		if (enode->TriggerHolder != NULL) {
			observable_DeleteRecipient(enode->TriggerHolder, 
				atom_Intern(enode->spec), ness);
			enode->TriggerHolder = NULL;
		}
	}

	repost = FALSE;

	if (onode->obj == NULL) 
		return;
	if (onode->menulist != NULL) {
		/* tell the object it has no menulist */
		celview_SetMenulist(onode->obj, NULL); 
		repost = TRUE;
	}
	if (onode->keymap != NULL)  {
		/* tell the object it has no keymap */
		celview_SetKeymap(onode->obj, NULL);
		repost = TRUE;
	}
	if (onode->MouseEnabled) {
		celview_SetHitfunc(onode->obj, NULL, 0);
		onode->MouseEnabled = FALSE;
	}

	if (repost)   celview_Repost(onode->obj);

	if (onode->keymap != NULL)  {
		keymap_Destroy(onode->keymap);
		onode->keymap = NULL;
	}
	if (onode->menulist != NULL) {
		menulist_Destroy(onode->menulist);
		onode->menulist = NULL;
	}
}

/* HandleNamedCel(cv, ness)
	This function is called each time an arbiter finds out about a named cel.
	It scans the NessList looking for a ness with an extend for a cel of that name.
	(The ness arg is ignored because arbiters may have multiple nesses.)
*/
	static void
HandleNamedCel(cv, null)
	struct celview *cv;
	struct ness *null;
{
	struct cel *c;
	unsigned char *tname;
	struct ness *n;
	struct nesssym *attr;
	struct objnode *onode;
	struct nesssym *xattr;

	c = (struct cel *)cv->header.view.dataobject;
	if (c == NULL  ||  cel_GetRefName(c) == NULL) return;

	tname = CelSymName((unsigned char *)cel_GetRefName(c));
	for (n = ness_GetList();  n != NULL;  n = n->next) {
		if ( ! n->compiled  ||  ness_GetArbiter(n) == NULL)  continue;
		if (cv->arb == NULL || ness_GetArbiter(n) != cv->arb) continue;
		if (strcmp(tname, "&defaulttext") == 0)
			/* We are being told of a child called "defaulttext" and n
			    is associated with cv->arb.  Set n->DefaultText.*/
			ness_SetDefaultText(n, (struct textview *)
					ProperPtr((struct basicobject *)cv, 
						textviewClass));
		attr = nesssym_NFind(tname, n->constScope);
		if (attr == NULL || attr->flags != flag_xobj) continue;

		/* 'attr' is the symbol entry for an EXTEND block for the given name */

		onode = nesssym_NGetINode(attr, objnode);
		onode->obj = cv;
		attr->parent.ness = n;	/* set parent */
		postevents(n, attr);

		/* check to see if it has an enode for 'becameVisible' */

		for (xattr = onode->attrs; xattr != NULL; xattr = xattr->next) 
		    if (xattr->flags == flag_event) {
			struct eventnode *enode = nesssym_NGetINode(xattr, eventnode);
			if (enode->varsym->header.toksym.toknum== EVENT &&
				    FOLDEDEQ ("becamevisible", enode->spec)) {
				/* it has "becameVisible": call it */
				EventEventStub(n, (struct view *)cv, enode);
				break;	/* from the for(xattr) */
			}
		    } /* end if (and for) */
		return;
	}
}



/* neventPost (ness)
	Post all events for the extended objects in ness->globals
*/
	void 
neventPost (ness, debug)
	struct ness *ness;
	boolean debug;
{
	struct arbiterview *arb = ness_GetArbiter(ness);
	struct nesssym *attr;

	PostDebug = debug;

	globalInit = im_GetGlobalInit();

	if (arb != NULL)
		arbiterview_AddHandler(arb, HandleNamedCel, NULL);

	/* process -extend "view:name"- */
	for (attr = ness->globals; attr != NULL; attr = attr->next) 
		/* visit each global entity in the script,
		   if it is an xobj with name beginning "&view:"
		   post the events */
		if (attr->flags == flag_xobj  
				&& nesssym_NGetINode(attr, objnode)->ExtendAView) {
			/* process its -on proc-s */
			attr->parent.ness = ness;	/* set parent */
			postevents(ness, attr); 
		}
}


/* neventUnpost (ness)
	Remove all events posted for the extended objects in ness->globals
*/
	void
neventUnpost (ness, debug)
	struct ness *ness;
	boolean debug;
{
	struct nesssym *attr;
	char buf[100];

	globalInit = im_GetGlobalInit();

	for (attr = ness->globals; attr != NULL; attr = attr->next) 
		/* visit each global entity in the script,
		   if it is an xobj, and has events, unpost them */
		if (attr->flags == flag_xobj) {
			if (debug)
				printf("Unposting events for %s\n",
					nesssym_ToC(attr, &curNess->header.text, 
							buf, sizeof(buf)));
			attr->parent.ness = ness;	/* set parent */
			unpostevents(ness, attr, debug);
		}
}
