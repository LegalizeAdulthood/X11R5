/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/error.c,v 1.18 1991/09/12 16:25:20 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/error.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/error.c,v 1.18 1991/09/12 16:25:20 bobg Exp $";
#endif

/* error.c
	error processing for ness

	Entry points:

general:

freeze(string)		{in error.h}
codelocStore(nesssym)
codelocForget(nesssym)
codelocFind(loc)

compile time errors:

SetupErrorHandling(ness)
SaveError(msg, loc, len)
ReportError(msr, index)
ExprError(msg, expr)
yyerror(s)
errsynch()
isFuncStart(tok)

run time errors:

LocateErrorFunc(loc, base, msg, ness)
MapRunError(ness)

*/

/*
 * $Log: error.c,v $
 * Revision 1.18  1991/09/12  16:25:20  bobg
 * Update copyright notice and rcsid
 *
 * Revision 1.17  1990/10/12  21:08:45  wjh
 * revised yaccpar to eliminate saving parser state.
 * 	moved yycompile to gen.c
 * 	eliminated ErrorList
 * 	made the argument to MapRunError be a ness instead of an ErrorList
 *
 * Revision 1.16  89/09/19  19:57:41  wjh
 * fix precedence error
 * 
 * Revision 1.15  89/09/11  08:30:39  ghoti
 * fix illegal pointer combination
 * 
 * Revision 1.14  89/09/03  22:47:22  wjh
 * newness
 * 
 * Revision 1.13  89/06/23  17:19:39  wjh
 * (Items that should not be sent to downstairs are marked "R4".)
 * 
 * Added a call to CheckForInterrupt in the function calls and internal gotos.  Thus loops can be terminated with ^G.   (interp.c)  R4
 * 
 * Changed dokeys() so it uses im_DoKeySequence.  This means it is now possible to send a key sequence which involves the message line.  (interp.c)  R4
 * 
 * Implemented DoMenu(object, menustring).   This function causes the same behavior as if the user selected the menu option.  At present the menu string must be exactly as originally defined;  see the warning above for im_DoMenu().  (interp.c, call.c)  R4
 * 
 * Changed access to Ness library routines so they are always compiled.  (They used ot get the default of NoCompilation, so they were useless.)  (call.c)  
 * 
 * Removed a superflous {} pair.  {This is purely a cosmetic change.}  (nessmark.c) 
 * 
 * Fixed value_GetStringValue.  Formerly it was getting an invalid initial value.  {The fix was done by adding a call to nessmark_Initialize() in the stackString section.}  (nevent.c) 
 * 
 * Modified the data stream so errors will not occur when a ness object is the outermost object.  The fix was to add two bytes, "00", at the end of the origin string to prevent the former occurence of a spurious "\}".  (ness.c) 
 * 
 * Fixed menu handling so Ness and child menus get posted when there is a mouse click in an inset within the Ness.  Formerly neither set of menus was posted. (nessv.c) 
 * 
 * Fixed dostmt, the function called from ness-load, which is recommended to be bound to ESC-ESC.  It was using a NULL pointer, so people were getting core dumps if they typed ESC-ESC before doing a compile.  (ness.c) 
 * 
 * Avoided an infinite loop which occurred if a library function referred to a non-existent entry point within itself.  Did this by checking to see if the library function is already Compiling just before trying to compile it.  (call.c call.hn, call.h) 
 * 
 * Revised system marker allocation so the compilation will not get a subsequent error.  (gen.c)
 * 
 * Revised system marker allocation so it expands the space available if necessary. This will make it possible to compile larger programs.  (gen.c)
 * 
 * Changed the type of TType to long from struct object *.  This will allow compilation on stricter compilers.  (interp.h) 
 * 
 * Fixed nessmark_FinalizeObject so it would not reference a NULL pointer.  {Somehow the assembler noticed this bug!}  (nessmark.c) 
 * 
 * Changed functions which deal with constant strings to have (char *) as there argument type (SaveError, exprnode_Create, ReportError, ExprError, RunError, LocateErrorFunc, QueryReadOnly, makeConst, printallerrors) or return type (Freeze, argcounterr, argtypeerr).  This prevents compile errors on picky compilers.  (interp.c, error.c, call.c, ness.c, nessv.c, search.c, nevent.c, nessmark.c, nessrun.ci)  R4 
 * 
 * Changed Imakefile to store Imakefile in checkin rule.  (Imakefile)
 * 
 * 
 * Revision 1.12  89/06/01  15:59:27  wjh
 * campus release version
 * 
 * Revision 1.7  89/01/04  10:53:56  wjh
 * fixed various core dump bugs
 * 
 * Revision 1.6  88/12/20  19:46:51  wjh
 * fixed various bugs
 * 
 * Revision 1.5  88/12/07  22:42:31  wjh
 * 
 * 9 Nov
 * implemented access level control
 * skip first line of script if starts with # (for shell script)
 * changed so the name of all initialization functions is init()
 * added ness-load
 * moved execution of init() from compilation to first execution
 * deferred compilation to first FullUpdate time
 * 
 * 22 Nov
 * proc table calls now work correctly with type-free procs  (the first arg can be anything)
 * added "cheat_" functions which will remain undocumented
 * changed inset() to a function
 * fixed some bugs in initial access
 * 
 * 25 November
 * added long strings
 * added Next Error menu option
 * made replace() work correctly in all cases
 * added class() and new()
 * 
 * 29 Nov
 * added ^<upper-case> and \e as characters in strings
 * added nextn() and length()
 * 
 * 6 Dec
 * added functions: parseint(), parsereal(), firstobject(), whereitwas(), replacewithobject(), addstyles(), nextstylegroup(), enclosingstylegroup(), clearstyles(), hasstyles()
 * catch bus and segmentation errors
 * 
 * 
 * Revision 1.4  88/11/16  11:59:27  wjh
 * 
 * implemented access level control
 * skip first line of script if starts with # (for shell script)
 * changed so the name of all initialization functions is init()
 * added ness-load
 * moved execution of init() from compilation to first execution
 * deferred compilation to first FullUpdate time
 * 
 * Revision 1.3  88/11/01  17:42:05  wjh
 * 
 * an empty Ness is now given a template (ness.template or default.template)
 * implemented:  extend proctable on proc "name" ...
 * added currentmark()
 * fixed a core dump by not recompile during UnlinkTree()
 * 
 * Revision 1.2  88/10/26  11:51:42  wjh
 * fixed a malloc abort bug that happened when certain error messages were discarded
 * added call to init() when initial compilation is done
 * fixed so events are posted to views whenever the view name is registered with the arbiter
 * 
 * Revision 1.1  88/10/21  10:57:12  wjh
 * Initial revision
 * 
 * Creation 0.0  88/05/28 10:16:00  wjh
 * Initial creation by WJHansen
 * 
*/

/* the first byte of an error message indicates whether (:) it is in static memory
		(which means it never changes)
 	or (*) is the result of 'freeze' (which is used when the message is 
		newly generated for each occurrence)
*/


#include <class.h>		/* for NULL */
#include <nodeclss.h>
#include <lex.ih>
#include <ness.ih>
#include <nesssym.ih>
#include <nessmark.ih>
#include <compdefs.h>	/* for exprnode */
#include <envt.h>
#include <nevent.h>	/* for objnode */
#include <gen.h>		/* for curNess */

#include <error.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
	errornode functions
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct errornode *
errornode_New()
{
	return (struct errornode *)malloc(sizeof(struct errornode));
}

	struct errornode *
errornode_Create(ness, loc, len, execloc, msg, ownmsg, next)
	struct ness *ness;
	long loc, len, execloc;
	char *msg;
	boolean ownmsg;
	struct errornode *next;
{
	struct errornode *enode = (struct errornode *)malloc(sizeof(struct errornode));
	enode->where = ness_CreateMark(ness, loc, len);
	enode->execloc = execloc;
	enode->msg = (unsigned char *)msg;
	enode->ownmsg = ownmsg;
	enode->next = next;
	return enode;
}


	void 
errornode_Destroy(enode)
	struct errornode *enode;
{
	if (enode->ownmsg) free(enode->msg);
	ness_RemoveMark((struct ness *)mark_GetObject(enode->where), enode->where);
	mark_Destroy(enode->where);
	free(enode);
}



#if 0

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
	OBJECT CODE LOCATION ROUTINES
	Map from object code location to the nesssym
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static struct nesssym **codelocTable = NULL;
	/* this table is an array of nesssym's for the various ness functions that have
	have been compiled.  They are ordered by the code location so a binary search
	can be done.  */
static long codelocTableMax = 0;
	/* the number of entries available in the current table */
static long codelocTableInUse = 0;
	/* the number of entries currently in use */

/* codelocIndex(loc)
	searches the table for the highest entry less than 'loc'
	and returns a pointer to the corresponding element of the table.
	if 'loc' precedes the first entry, returns NULL
*/
	static struct nesssym **
codelocIndex(loc)
	long loc;
{
	long low, hi, mid;
	long midloc;	/* the object code location at mid */

	/* do a binary search */
	low = -1;
	hi = codelocTableInUse;

		/* each time before testing the predicate, we have
			-1 <= low < hi <= codelocTableInUse
			objloc@low <= loc < objloc@hi
				(where objloc@-1 is negative infinity
				and objloc@codelocTableInUse is +infinity)
		*/
	while (low < hi-1) {
		mid = low + (hi - low) / 2;	/* low <= mid < hi */
		midloc = nessmark_GetPos(&SysMarkLowEnd[
			nesssym_NGetINode(codelocTable[mid], funcnode)->SysMarkOffset   ]);	
		if (loc < midloc)		/* objloc@low <= loc < objloc@mid */
			hi = mid;
		else  /* (loc >= midloc)	/* objloc@mid <= loc < objloc@hi */
			low = mid;
	}
	return (low == -1) ? NULL : &codelocTable[low];
}

/* codelocStore(fness) 
	record 'fness' under location  fness->funcnode->SysMark->pos
	KLUDGE: the first six fields of funcnode are the same as event nodes
*/
	void
codelocStore(fness)
	struct nesssym *fness;
{
	long newloc;
	struct nesssym **prior;
	long size = sizeof (struct nesssym *);

	if (codelocTableInUse >= codelocTableMax) {
		/* make the table bigger */
		if (codelocTable == NULL)
			codelocTable = (struct nesssym **)malloc(
				size * (codelocTableMax = 50));
		else codelocTable= (struct nesssym **)realloc(codelocTable,
				size * (codelocTableMax += 50));
	}
	
	newloc = nessmark_GetPos(&SysMarkLowEnd[
			nesssym_NGetINode(fness, funcnode)->SysMarkOffset   ]);
	prior = codelocIndex(newloc);
	if (prior == NULL)
		prior = codelocTable - 1;
	bcopy(prior+1, prior+2, size * ((codelocTable + codelocTableInUse) - (prior + 1)));

	*(prior+1) = fness;
	codelocTableInUse++;
}

/* codelocForget(fness)
	forget the codeloc entry for fness
*/
	void
codelocForget(fness)
	struct nesssym *fness;
{
	struct nesssym **resinx = codelocIndex(nessmark_GetPos(&SysMarkLowEnd[
			nesssym_NGetINode(fness, funcnode)->SysMarkOffset   ]));
	if (fness != *resinx) 
		/* oops, not this one */
		return;
	
	bcopy(resinx+1, resinx, sizeof (struct nesssym *) 
				* ((codelocTable + codelocTableInUse) - (resinx+1)));
	codelocTableInUse--;
}

/* codelocFind(loc)
	find the nesssym corresponding to 'loc' in object code 
*/
	struct nesssym *
codelocFind(loc)
	long loc;
{
	struct nesssym **resinx;
	struct nessmark *resmark;

	resinx = codelocIndex(loc);
	if (resinx == NULL) return NULL;
	resmark = &SysMarkLowEnd[
			nesssym_NGetINode(*resinx, funcnode)->SysMarkOffset   ];
	if (loc < nessmark_GetPos(resmark)  
			||  loc >= nessmark_GetPos(resmark) 
					+ nessmark_GetLength(resmark))
		return NULL;
	return *resinx;
}

#else

	void
codelocStore(fness)
	struct nesssym *fness;
{}
	void
codelocForget(fness)
	struct nesssym *fness;
{}

	static boolean
HasLoc(s, loc)
	struct nesssym *s;
	long loc;
{
	struct nessmark *resmark = &SysMarkLowEnd [
			nesssym_NGetINode(s, funcnode)->SysMarkOffset   ];
	return (loc >= nessmark_GetPos(resmark)  
			&&  loc < nessmark_GetEndPos(resmark));
}

/* codelocFind(loc)
	find the nesssym corresponding to 'loc' in object code 

	scan NessList and scan globals for each Ness  
		(and scan children for each XObject)
*/
	struct nesssym *
codelocFind(loc)
	long loc;
{
	struct ness *ness;
	struct nesssym *g, *xg;

	for (ness = ness_GetList(); ness != NULL; ness = ness->next) 
	  if (ness->compiled)
	    for (g = ness->globals;  g != NULL;  g = g->next) 

		if (g->flags == (flag_function | flag_ness)) {
			if (HasLoc(g, loc)) return g;
		}
		else if (g->flags == flag_xobj)
			for (xg = nesssym_NGetINode(g, objnode)->attrs;
					xg != NULL;
					xg = xg->next) 
				if (xg->flags == (flag_function | flag_ness | flag_xfunc)
						||  xg->flags == flag_event)
					if (HasLoc(xg, loc)) return xg;

	return NULL;
}

#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
	COMPILE TIME ERRORS
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static long lastloc, lastlen;	/* loc/len of last ":. . . restart" */
static boolean Restarted;


	void
SetupErrorHandling(ness)
	struct ness *ness;
{
	lastloc = -1;
	Restarted = FALSE;
	while (ness->ErrorList != NULL) {
		struct errornode *t = ness->ErrorList;
		ness->ErrorList = t->next;
		errornode_Destroy(t);
	}
	ness->ErrorList = NULL;
}



/* ReportError(msg, index)
	calls SaveError after getting the loc and len
		from the token at 'index'
*/
	void
ReportError(msg, index)
	char *msg;
	long index;
{
	long loc, len;
	loc = lex_RecentPosition(index, &len);
	SaveError(msg, loc, len);
}

/* ExprError(msg, expr)
	calls SaveError after getting the loc and len from 'expr'
*/
	void
ExprError(msg, expr)
	char *msg;
	struct exprnode *expr;
{
	SaveError(msg, expr->loc, expr->len);
}

/* SaveError(msg, loc, len)
	adds the message to the list of errors
	list is maintained with earliest message first
*/
	void
SaveError(msg, loc, len)
	char *msg;
	long loc, len;
{
	struct errornode *err;

	err = errornode_Create(curNess, loc, len, 0, msg, (*msg == '*'), NULL);
	if (curNess->ErrorList == NULL)
		curNess->ErrorList = err;
	else {
		struct errornode *t;
		for (t = curNess->ErrorList; t->next != NULL; t = t->next) 
			{}
		t->next = err;
	}
}


	void
yyerror(s)
	unsigned char *s;
{
	/* struct toksym *token; */
	long loc, len;
	static char *syntaxerror = "syntax error";


	loc = lex_RecentPosition(0, &len);

	/* this is either a "syntax error" or a "yacc stack overflow" */
	/* don't report error if we have Restarted from a former error at the same loc/len */
	if (Restarted && loc == lastloc  &&  len ==lastlen)
		return;

	if (strcmp(s, syntaxerror) == 0)
		SaveError(":syntax error", loc, len);
	else {
		char buf[300];
		buf[0] = '*';
		strncpy(buf+1, s, 298);
		buf[299] = '\0';	/* truncate if needed */
		SaveError(freeze(buf), loc, len);
	}
	Restarted = FALSE;
}

/* errsynch(index, pyychar, pyyerrflag)
	generate an error message that parser will restart after error 
	However, if it is the same restart token as the last, skip it
	returns location of restart
*/
	long
errsynch(index, pyychar, pyyerrflag)
	long index;
	int *pyychar;
	short *pyyerrflag;
{
	long loc, len;

	loc = lex_RecentPosition(index, &len);

	if ( ! Restarted || loc > lastloc || len != lastlen) { 
		/* this is the first restart at this token */
		Restarted = TRUE;
		lastloc = loc;
		lastlen = len;
		*pyychar = -1;	/* yyclearin */
		*pyyerrflag = 0;	/* yyerrok */
		lex_Repeat(index);
		SaveError(": . . . restart with token", loc, len); 
	}
	return (loc);
}

/* isFuncStart(tok, maybeend, ncheck)
	check to see if this token may be an error restart point
	if 'maybeend' is TRUE, it is not a restart point when
			the preceding token is END or the next is ';'
	furthermore, in order to be a restart, one of the 'ncheck' tokens
			starting with this one must be unindented
	The position in the token stream is unaffected; the next lex_NextToken
	will get the token it would have returned had this function not been called
*/
	boolean
isFuncStart(tok, maybeend, ncheck)
	struct nesssym *tok;
	boolean maybeend;
	long ncheck;
{
	struct nesssym *ttok;
	long n;
	if (maybeend) {
		/* reject if predecessor is "end" or successor is ';' */
		lex_Repeat(-1);		/* go back to preceding token */
		lex_NextToken(&ttok);	/* read preceeding token */
		lex_NextToken(&tok);	/* reread current token */
		if (strcmp(ttok->header.sym.name, "end") == 0)
			return FALSE;
		lex_NextToken(&ttok);	/* read ahead one token */
		lex_Repeat(0);		/* reset to repeat the next token */
		if (ttok->header.toksym.toknum == 0)  /* EOF */
			return FALSE;
		if (*ttok->header.sym.name == ';')
			return FALSE;
	}
	/* here tok still has its original value and lex will return the next token */
	for (n = 0; n < ncheck; n++) {
		if (lex_RecentIndent(0) == 0) {
			/* succeed */
			if (n>0) lex_Repeat(1-n);	/*  n==1 ==> 0;  n==2 ==> -1 */
			return TRUE;
		}
		lex_NextToken(&ttok);
		if (ttok->header.toksym.toknum == 0)  /* EOF */
			return FALSE;
	}
	/* found none unindented: fail */
	lex_Repeat(2-ncheck);	/*  ncheck==2 ==> 0;   ncheck==3 ==> -1  */
	return FALSE;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
	RUNTIME ERRORS
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* MapRunError (ness)
	Modify the errornode ness->ErrorList to have the source code location 
	corresponding to the object code location.
*/
	void
MapRunError(ness)
	struct ness *ness;
{
	register struct funcnode *fnode;
	struct nesssym *errsym;
	register struct nessmark *objcode;
	struct errornode *err;
	long loc, len;

	err = ness->ErrorList;
	ness->ErrorList = err->next;	/* remove err from ErrorList
				before it is cleared by SetUpErrorHandling */
	if (err == NULL) 
		return;
	errsym = codelocFind(err->execloc);
	if (errsym == NULL) 
		return;

	fnode = nesssym_NGetINode(errsym, funcnode);	/* (might be
			an event node.  KLUDGE leading fields are the same) */
	objcode = &SysMarkLowEnd[fnode->SysMarkOffset];
	if (errsym->flags == (flag_function | flag_ness)) {
		/* top level function */
		if (ness != errsym->parent.ness)
			fprintf(stderr, "MapRunErr - ness mismatch 1\n");
	}
	else {
		/* event or function defined in an 'extend' */
		if (ness != errsym->parent.nesssym->parent.ness)
			fprintf(stderr, "MapRunErr - ness mismatch 2\n");
	}

	mark_SetPos(err->where, fnode->srcloc);
	mark_SetLength(err->where, fnode->srclen);
	genStartLocating(err->execloc - nessmark_GetPos(objcode),
		errsym == ness->InitFunc);

	lex_SetSource(ness_GetLex(ness), ness, fnode->srcloc, fnode->srclen,
		ness->outerScope, ness->constScope);

	yycompile(ness);

	if (genFinishLocating(&loc, &len)) {
		/* found the line */
		mark_SetPos(err->where, loc);
		mark_SetLength(err->where, len);
	}

	/* delete compilation error messages */
	while (ness->ErrorList != NULL) {
		struct errornode *t = ness->ErrorList;
		ness->ErrorList = t->next;
		errornode_Destroy(t);
	}
	ness->ErrorList = err;
}


/* LocateErrorFunc(loc, base, msg, ness)
	Generates an errornode of 'msg' for the point of error.
	The error occurred at object code location 'loc', where
	the start of the object code is 'base'.
	If the top stack frame is in the 'ness', return the generated message.
	If the top stack frame is not in the 'ness', the actual failure
		is attached to its own ness, which is then exposed.
		Then the stack frames are traversed looking for one that 
		is in the 'ness' and the returned error indicates a 
		failure in the library routine called at that point.
*/
	struct errornode *
LocateErrorFunc(loc, base, msg, ness)
	unsigned char *loc, *base;
	char *msg;
	struct ness *ness;
{
	struct nesssym *wheresym;
	struct ness *whereness;
	struct frameelt *frame;

	wheresym = codelocFind(loc - base);
	if (wheresym == NULL) 
		return errornode_Create(ness, 0, 0, loc - base, msg, (*msg == '*'), NULL);
	whereness = (wheresym->flags == (flag_function | flag_ness)) 
				? wheresym->parent.ness
				: wheresym->parent.nesssym->parent.ness;
	if (whereness == ness) 
		/* the error is in the ness itself */
		/* the msg will be MapRunError'ed in the ness */
		return errornode_Create(whereness, 0, 0, loc - base, msg, (*msg == '*'), NULL);

	/* the message is not in the root ness.
		attach an error message to the ness with the error */
	whereness->ErrorList = errornode_Create(whereness, 0, 0, loc - base, msg, (*msg == '*'),
				whereness->ErrorList);
	MapRunError(whereness);
	ness_Expose(whereness);

	/* go up stack looking for the root ness to give it a message */
	frame = FramePtr;
	while (TRUE) {
		if (frame->returnaddress == NULL) 
			/* this is the outermost frame, give up */
			return errornode_Create(ness, 0, 0, loc - base, 
				":error in subroutine called from unknown location", 
				FALSE, NULL);
		wheresym = codelocFind(frame->returnaddress - base - 2);
		whereness = (wheresym->flags == (flag_function | flag_ness)) 
				? wheresym->parent.ness
				: wheresym->parent.nesssym->parent.ness;
		if (whereness == ness) 
			return errornode_Create(whereness, 0, 0, frame->returnaddress - base - 2,
				":a library function had an error", 
				FALSE, NULL);
		frame = frame->prevframe;
	} 
}
