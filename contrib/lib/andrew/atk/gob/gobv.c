/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/gob/RCS/gobv.c,v 1.8 1991/09/12 16:22:01 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/gob/RCS/gobv.c,v $ */

#ifndef lint
	char *gobview_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/gob/RCS/gobv.c,v 1.8 1991/09/12 16:22:01 bobg Exp $";
#endif /* lint */

/* gobv.c	

	The view module for the gob dataobject

*/


#include <andrewos.h>

#include <class.h>
#include <graphic.ih>
#include <view.ih>
#include <fontdesc.ih>

#include <menulist.ih>
#include <keymap.ih>
#include <keystate.ih>
#include <bind.ih>
#include <im.ih>
#include <rect.h>
#include <txttroff.ih>
#include <proctbl.ih>

/* $$$ include xxx.ih for any other file for which there is below a call to xxx_routine() */

/* include headers for the data object and THIS view */
#include "gob.ih"
#include "gobv.eh"
#define Gob(SS) ((struct gob *)SS->header.view.dataobject)

static char  debug = 0;      /* This debug switch is toggled with ESC-^D-D */
	/*	$$$ you could provide a method in this module which sets -debug-
		Then gobviewtest.c could call it to set this debug from the -d switch */
#define DEBUG(s) {if (debug) {printf s ; fflush(stdout);}}
#define ENTER(r) DEBUG(("Enter %s(0x%lx)\n", "r", self))
#define LEAVE(r) DEBUG(("Leave %s(0x%lx)\n", "r", self))

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *\
 *
 *	Supporting procedures
 *	
\* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* $$$ ComputeArea is one approach to a computation satisfying xxx_DesiredSize. 
	This code also illustrates how to find the size of a character or string.
	You can delete this function if size is computed entirely 
	within the xxx_DesiredSize routine. */

/* ComputeArea(self)
	set fields self->Area / Width / Height
	The width and height are 10 times the size of a 'W' and area is the product.

	The routine is more general, though; variable c can be set to any string and 
	ten times the size of that string is used.
*/
	static void
ComputeArea(self)
	register struct gobview *self;
{
	long minwidth, minheight;
	char *tail;
	char *c = "W";
	register struct graphic *g;
	struct fontdesc *fd;
	struct FontSummary *FS;
	long cnt;

	if (self->sizeknown)
		return;

	g = (struct graphic *)gobview_GetDrawable(self);
	fd = gobview_GetFont(self);
	FS = fontdesc_FontSummary(fd, g);

	minwidth = minheight = 0;
	for (cnt=0; cnt < 2; cnt++) {
		tail = c;
		/* if c is multiple lines, the size must be computed by
			adding the sizes of each line */
		while (tail) {
			register char *tend = index(tail, '\n');
			long w, h;
			if (tend) {
				fontdesc_TextSize(fd, g, tail, tend-tail, &w, &h);
				tail = tend+1;
			}
			else {
				fontdesc_StringSize(fd, g, tail, &w, &h);
				tail = NULL;
			}
			if (w > minwidth)  minwidth = w;
			minheight += FS->maxHeight;
		}
		if (minwidth != 0  &&  minheight != 0) 
			break;
		/* first tried current font: it had no 'W'.  Now try andy12 font */
		fd = fontdesc_Create("andy", 0, 12);
	}
	/* The string width is more or less minwidth.  
		(In fact minwidth is the distance
		from the origin of the first character of the string 
		to the origin of the next character after the string.)
	    The string height is no greater than minheight
		(We should really use FS->newlineHeight, 
		but that field is not implemented at this writing.)  */

	self->DesiredWidth = 10 * minwidth;
	self->DesiredHeight = 10 * minheight;
	self->DesiredArea = 100 * minwidth * minheight;

	self->sizeknown = TRUE;

	DEBUG(("ComputeArea: %d x %d   (cnt: %d)\n", 
			self->DesiredWidth, self->DesiredHeight, cnt));
}

#ifdef NOTUSED
/* 	$$$ This routine is not used.  It iilustrates using a graphic primitive */
/* InvertRectangle(self, left, top, width, height)
	Changes white to black and black to white within the rect. 
*/
	static void 
InvertRectangle(self, left, top, width, height)
	register struct gobview *self;
{
	/* this code shows how to invert rectangles */
	struct rectangle r;

	gobview_SetTransferMode(self, graphic_INVERT);
	rectangle_SetRectSize(&r, left, top, width, height);
	gobview_FillRect(self, &r, self->BlackPattern);
}

/* 	$$$ This routine is not used.  It iilustrates using a graphic primitive */
/* ShowString(self, x, y, string)
	Writes the text of 'string' centered at (x, y) in the logical rect.
*/
	static void
ShowString(self, x, y, string)
	register struct gobview *self;
	long x, y;
	char *string;
{
	gobview_MoveTo(self, x, y);
	gobview_DrawString(self, string,
			graphic_BETWEENLEFTANDRIGHT |
			    graphic_BETWEENTOPANDBASELINE);
}
#endif /* NOTUSED */

/* 	$$$ replace the following routine with one that redraws 
	whatever image your view is to provide 
		The algorithm here flashes the screen because it erases all the
		obs each time a new one is added.
		The correct approach is for the view to keep track of
		what it has on the screen and compare it with the data object
		each time there is to be a redraw.
		This will then continue to work if we add an operation to 
		remove obs.
*/
/* RedrawView(self, type, left, top, width, height)
	Redraws the view from the data structure.
	If the type specifies a partial redraw, we only really need to redraw the
	portion delimited by left, top, width, height. ($$$)
*/
#define xset(S,O,V) ((long)((float)(V - O->xmin) * S->xscale))
#define yset(S,O,V) ((long)(((float)(O->ymax - O->ymin) - (V - O->ymin)) * S->yscale))
makerec(rec,pt,sz)
struct rectangle *rec;
struct point *pt;
long sz;
{
    if(sz >= 2){
	rec->left = pt[0].x;
	rec->top = pt[0].y;
	rec->width = pt[1].x - pt[0].x;
	rec->height = pt[1].y - pt[0].y;
    }
}
	static 
RedrawView(self, full, left, top, width, height)
	register struct gobview *self;
	boolean full;
	long left, top, width, height;
{
	struct rectangle rec;
	struct rectangle r;
	register struct oblist *d;
	short lw,cw,cg,i;
	struct point *p,*pt,*o,stdpt[64];
	struct graphic *gray;
	register boolean upall;
	long size, type,pointstoscale;
	register struct gob *dobj 
		= Gob(self);
	upall = (full ||  timercmp(&(dobj->modtime),&(self->lasttime),>));
	gobview_GetVisualBounds(self,&r);
	rectangle_GetRectSize(&r, &left, &top, &width, &height);
		/* we shamelessly utilize the parameters as locals */
	DEBUG(("RedrawView(%d) in [%d, %d, %d, %d]\n", full, left, top, width, height));
		/* we have to subtract one from 
			right and bottom to get inside the rectangle */
	/* has input focus.  draw outline of rectangle */
	if(upall){
	    gobview_SetTransferMode(self, graphic_COPY);
	    gobview_GetLogicalBounds(self, &r);		/* find rectangle bounds */
	    gobview_FillRect(self, &r, self->WhitePattern);	/* clear the rectangle */
	}
	if(full) {
	    self->xscale = (float) width/((float)(dobj->xmax - dobj->xmin))  ;
	    self->yscale = (float) height /((float)(dobj->ymax - dobj->ymin))  ;
	    if ( self->HasInputFocus ) 
		gobview_DrawRectSize(self, left, top, width-1, height-1);
	}
	gobview_SetTransferMode(self, graphic_BLACK);
	if(upall){
	   gobview_SetLineWidth(self,1); gobview_MoveTo(self,xset(self,dobj,0),yset(self,dobj,dobj->ymin));
	    gobview_DrawLineTo(self,xset(self,dobj,0),yset(self,dobj,dobj->ymax));
	    gobview_MoveTo(self,xset(self,dobj,dobj->xmin),yset(self,dobj,0));
	    gobview_DrawLineTo(self,xset(self,dobj,dobj->xmax),yset(self,dobj,0));
	}
	cw= gobview_GetTransferMode(self);
	lw = gobview_GetLineWidth(self);
	cg = -1;
	gray = self->BlackPattern;
	for (d = gob_GetFirstOb(dobj);  d != NULL;
			d = gob_GetNextOb(dobj, d)) {
	    if(upall ||  timercmp(&(d->modtime),&(self->lasttime),>)){
		upall = TRUE; /* since objects may overlay, redraw subsequent objects */
		type = d->type & gob_OBMASK;
DEBUG(("GOT IN LOOP %d %d %d\n",d->type,d->size,type));
		if(cw != d->mode){
		    gobview_SetTransferMode(self,d->mode);
		    cw = d->mode;
		}
		if(d->type & (gob_OPEN | gob_CLOSED)){
		    /* gray indicates line width */
		    if(d->gray != lw && d->gray != 0){
			gobview_SetLineWidth(self,d->gray);
			lw = d->gray;
		    }
		}
		else if(d->type & gob_FILLED){
		    /* gray indicates gray pattern */
		    if(cg != d->gray){
			switch(d->gray){
			    case 0:
				gray = self->BlackPattern;
				break;
			    case 16:
				gray = self->WhitePattern;
				break;
			    default:
				if(d->gray > 16 || d->gray < 0)
				    gray = self->BlackPattern;
				else
				    gray = gobview_GrayPattern(self, d->gray,16);
				break;
			}
		    }
		}
		if(d->size > 1024 || d->size < 0) continue;
		if(d->size > 64){
			pt = (struct point *)malloc(sizeof(struct point ) * d->size);
		}
		else pt = stdpt;
		DEBUG(("scalex = %f, scaley = %f\n",self->xscale,self->yscale));
		if(type == gob_ARC)
		    pointstoscale = 2;
		else
		    pointstoscale = d->size;
		for(i = 0,p = pt,o = d->ptlist; i < pointstoscale; i++,p++,o++){
		    point_SetPt(p, xset(self,dobj,point_X(o)),yset(self,dobj,point_Y(o)));
		    DEBUG(("-- %d (%d,%d) (%d,%d) \n", i,point_X(o),point_Y(o),point_X(p),point_Y(p)));
		}
		size = d->size;
		switch(type){
		    case gob_PATH:
			DEBUG(("PATH %d %d %d %d %d\n",(d->type & gob_TYPEMASK),pt[0].x,pt[0].y,pt[1].x,pt[1].y));
			switch(d->type & gob_TYPEMASK){
			    case gob_OPEN:
				gobview_DrawPath(self,pt,size);
				break;
			    case gob_CLOSED:
				gobview_DrawPolygon(self,pt,size);
				break;
			    case gob_FILLED:
				gobview_FillPolygon(self,pt,size,gray);
				break;
			}
			break;
		    case gob_OVAL:
			DEBUG(("OVAL %d\n",(d->type & gob_TYPEMASK)));
			makerec(&rec,pt,size);
			DEBUG(("-- %d %d %d %d %d\n",(d->type & gob_TYPEMASK),pt[0].x,pt[0].y,pt[1].x,pt[1].y));
			DEBUG(("rec = %d,%d,%d,%d gray = %d\n",rec.left,rec.top,rec.width,rec.height,gray));
			switch(d->type & gob_TYPEMASK){
			    case gob_OPEN:
			    case gob_CLOSED:
				gobview_DrawOval(self,&rec);
				break;
			    case gob_FILLED:
				gobview_FillOval(self,&rec,gray);
				break;
			}
			break;
		    case gob_ARC:
			DEBUG(("ARC %d\n",(d->type & gob_TYPEMASK)));
			makerec(&rec,pt,size);
			
			DEBUG(("-- %d (%d %d) (%d %d) (%d,%d)\n",(d->type & gob_TYPEMASK),pt[0].x,pt[0].y,pt[1].x,pt[1].y, d->ptlist[2].x,d->ptlist[2].y));
			DEBUG(("rec = %d,%d,%d,%d gray = %d\n",rec.left,rec.top,rec.width,rec.height,gray));
			switch(d->type & gob_TYPEMASK){
			    case gob_OPEN:
			    case gob_CLOSED:
				gobview_DrawArc(self,&rec, d->ptlist[2].x,d->ptlist[2].y);
				break;
			    case gob_FILLED:
				gobview_FillArc(self,&rec, d->ptlist[2].x,d->ptlist[2].y,gray);
				break;
			}
			break;
		    case gob_LABEL:
			/* gray indicates text op */
			DEBUG(("draw string %s\n",d->label));
			gobview_MoveTo(self,pt[0].x,pt[0].y);
			gobview_DrawString(self,d->label,d->gray);
			break;
		}
		if(pt != stdpt) {
		    DEBUG(("FREEING PT\n"));
		    free(pt);
		}
	    }
	}
	gettime(&(self->lasttime));
}   
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *\
 *	User Interface 
 *	
 *	Routines called from keystrokes or menu
 *
 *	$$$ in this section put any operations that are to apply to your view
 *	The -rock- is the 3rd or 5th value from the bind description line (depending
 *	on whether the operation is invoked from menu or keyboard).
\* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

	static void
gobview_ClearRectangle(self, rock)
	register struct gobview *self;
	long rock;
{
	register struct gob *dobj = (struct gob *)self->header.view.dataobject;
	struct rectangle r;
	gobview_GetLogicalBounds(self, &r);
	gobview_FillRect(self, &r, self->WhitePattern);
	gob_ClearAllObs(dobj);
	gob_NotifyObservers(dobj, gob_DATACHANGED);
}

	static void
ToggleDebug(self, rock)
	register struct gobview *self;
	long rock;
{
	debug = ! debug;
	printf("debug is now %d\n", debug);  fflush (stdout);
}


/*  Menus and Keymaps
	The menus and keymaps are initialized to those of
	    EmbeddedMenus and EmbeddedKeymap in InitializeObject.
	The ApplicationMenus and ApplicationKeymap get
	    added to the menus and keymap in GetApplicationLayer

	Debugging can be toggled with the key sequence  ESC-^D -D

	$$$ Replace the initialization of the arrays with keybindings (2nd value)
	and/or menu entries (4th value) of your choice.  The function to be called
	is the 7th value in each array line.  The first value is a name and the 8th is 
	a comment.  The 6th controls non-constant menus and the 9th is ignored.
*/
static struct menulist  *EmbeddedMenus, *ApplicationMenus;
static struct keymap  *EmbeddedKeymap, *ApplicationKeymap;

static struct bind_Description EmbeddedBindings[]={
    {"gob-clear-rectangle", "\003", 0, "Clear~10", 0, 0, 
			gobview_ClearRectangle, "Clear the rectangle", NULL},
    {"gob-invert-debug", "\033\004D",		/* ESC - ^D - D */
			0, 0, 0, 0, ToggleDebug,
			"Toggle the gobview debug flag", NULL},
	NULL
};
static struct bind_Description ApplicationBindings[]={
	NULL
};


static void gobview_SetLogicalRect(self,x,y,width,height)
struct gobview *self;
long x,y,width,height;
{
    gob_SetLogicalRect(Gob(self),x,y,width,height);
}
static void gobview_ClearAllObs(self)
struct gobview *self;
{
    gob_ClearAllObs(Gob(self));
}
static void gobview_AddPathOb(self,pointlist,size,type,colorwid,mode)
struct gobview *self;
struct point *pointlist;
short size;
short type;
short colorwid;
short mode;
{
    gob_AddPathOb(Gob(self),pointlist,size,type,colorwid,mode);
}
static void gobview_AddOvalOb(self,x,y,width,height,type,colorwid,mode)
struct gobview *self;
long x, y, width, height;
short type, colorwid, mode;
{
    gob_AddOvalOb(Gob(self),x,y,width,height,type,colorwid,mode);
}
static void gobview_AddArcOb(self,x,y,width,height,StartAngle,OffsetAngle,type,colorwid,mode)
struct gobview *self;
long x; long y; long width;long height;
short StartAngle; short OffsetAngle; short type;short colorwid;short mode;
{
    gob_AddArcOb(Gob(self),x,y,width,height,StartAngle,OffsetAngle,type,colorwid,mode);
}
static void gobview_AddLabelOb(self,x,y,label,fontname,fontsize,fontstyle,op,mode)
struct gobview *self;
long x,y;
char *label;
char *fontname;
int fontsize,fontstyle,op,mode;
{
    gob_AddLabelOb(Gob(self),x,y,label, fontdesc_Create(fontname,fontstyle,fontsize),op,mode);
}


	boolean
gobview__InitializeClass(ClassID)
	struct classhdr *ClassID;
{
	EmbeddedMenus = menulist_New();
	ApplicationMenus = menulist_New();
	EmbeddedKeymap = keymap_New();
	ApplicationKeymap = keymap_New();
	bind_BindList(EmbeddedBindings, EmbeddedKeymap, EmbeddedMenus,
				&gobview_classinfo);
	bind_BindList(ApplicationBindings, ApplicationKeymap, ApplicationMenus,
				&gobview_classinfo);
	proctable_DefineProc("gobview-set-logical-rec", gobview_SetLogicalRect,&gobview_classinfo,NULL, "Set Logical Rectangle");
	proctable_DefineProc("gobview-Clear", gobview_ClearAllObs,&gobview_classinfo,NULL,"Clear Objects");
	proctable_DefineProc("gobview_add-path", gobview_AddPathOb,&gobview_classinfo,NULL, "Add Path");
	proctable_DefineProc("gobview-add-oval", gobview_AddOvalOb,&gobview_classinfo,NULL,"Add Oval");
	proctable_DefineProc("gobview_add-arc", gobview_AddArcOb,&gobview_classinfo,NULL, "Add Arc");
	proctable_DefineProc("gobview-add-label", gobview_AddLabelOb,&gobview_classinfo,NULL,"Add Label");
	return TRUE;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *\
 *	
 *	Override methods
 *	
\* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
	boolean
gobview__InitializeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct gobview  *self;
{
	self->Menus = menulist_DuplicateML(EmbeddedMenus, self);
	self->AppMenus = NULL;
	/* ApplicationMenus are added in GetApplicationLayer */
	self->Keystate = keystate_Create(self, EmbeddedKeymap);

	self->sizeknown = FALSE;
	self->OnScreen = FALSE;
	self->HasInputFocus = FALSE;
	self->embedded = TRUE;
	self->ignoreUp = FALSE;
	/* $$$ here initialize any variables added to the "data:" portion of 
		gobv.ch */

	return TRUE;
}

	void 
gobview__FinalizeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct gobview  *self;
{
	if(self->Menus) menulist_Destroy(self->Menus);
	if(self->AppMenus) menulist_Destroy(self->AppMenus);
	keystate_Destroy(self->Keystate);

	/* $$$ is variables in the data: of gobv.ch have been assigned
	pointers to malloc'ed storage, that memory should be free'd here.  */

}

	void
gobview__ObservedChanged(self, dobj, status)
	register struct gobview  *self;
	struct gob *dobj;
	long status;
{
	if (status == observable_OBJECTDESTROYED) 
		/* do not call wantupdate in this case,
			it will bomb the program */
		{}
	else if (status == gob_DATACHANGED) 
		/* $$$ it would be a good idea to make a note of which parts 
			of the data have changed, so only the corresponding parts 
			of the image need to be changed.  */ 
		gobview_WantUpdate(self, self);	/* request a chance to 
						update the image */
}

	struct gobview *
gobview__GetApplicationLayer(self)
	register struct gobview *self;
{
	self->embedded = FALSE;
	menulist_ChainBeforeML(self->Menus, self->AppMenus = menulist_DuplicateML(ApplicationMenus, self), NULL);
	keystate_AddBefore(keystate_Create(self, ApplicationKeymap), self->Keystate);
	return self;
}

	void 
gobview__ReceiveInputFocus(self)
	register struct gobview  *self;
{
	ENTER(gobview_ReceiveInputFocus);
	gobview_PostMenus(self, self->Menus);

	self->Keystate->next = NULL;
	gobview_PostKeyState(self, self->Keystate);

	self->HasInputFocus = TRUE;
	gobview_WantUpdate(self, self);   /* schedule an update to 
			provide an indication that we have the input focus */
	LEAVE(gobview_ReceiveInputFocus);
}


	void 
gobview__LoseInputFocus(self)
	register struct gobview  *self;
{
	ENTER(gobview_LoseInputFocus);
	self->HasInputFocus = FALSE;
	gobview_WantUpdate(self, self);  /* schedule an update to display 
			 indication that we no longer have the input focus */

	/* menus and keystate are deactivated by parent */

	LEAVE(gobview_LoseInputFocus);
}

/* gobview__FullUpdate(self, type, left, top, width, height)
	Redraws the entire image.  (The last four arguments are only used in 
	case the 'type' is a PartialRedraw;  they specify which part to update.)
*/
	void 
gobview__FullUpdate(self, type, left, top, width, height)
	register struct gobview  *self;
	register enum view_UpdateType  type;
	register long  left, top, width, height;
{
	DEBUG(("FullUpdate(%d, %d, %d, %d, %d)\n", type, left, top, width, height));
	if (type == view_Remove  
			||  gobview_GetLogicalWidth(self) == 0 
			|| gobview_GetLogicalHeight(self) == 0) {
		/* view_Remove means the view has left the screen.
			A zero dimension means the view is not visible */
		self->OnScreen = FALSE;
		return;
	}
	if (type != view_FullRedraw && type != view_LastPartialRedraw)
		return;
	/* we continue only for a FullRedraw or the last of a sequence of PartialRedraw
		requests.  */

	self->OnScreen = TRUE;
/* the following line is appropriate, but on 24 April 1988 it tickles a bug in ez 
	if (type == view_FullRedraw) 
*/
	{
		/* must recompute graphics info because image
			may be on different display hardware */

		self->WhitePattern = gobview_WhitePattern(self);
		self->BlackPattern = gobview_BlackPattern(self);
		DEBUG(("Drawable: 0x%lx   White: 0x%lx   Black: 0x%lx\n",
			gobview_GetDrawable(self), 
			self->WhitePattern, self->BlackPattern));
	}
	RedrawView(self, TRUE , left, top, width, height);
	LEAVE(gobview_FullUpdate);
}


	void 
gobview__Update(self)
	register struct gobview *self;
{
ENTER(gobview_Update);
	if (! self->OnScreen) return;

	/* $$$ the code in this example is highly bogus: it redraws the entire view
		every time there is any change.  Note that this leads to unacceptable
		flashing.  Instead the notes taken in ObservedChanged should be
		used here to update only the changed portion of the image */

	RedrawView(self,FALSE, 0, 0, 0, 0);

LEAVE(gobview_Update);
}
	struct view *
gobview__Hit(self, action, x, y, num_clicks)
	register struct gobview  *self;
	register enum view_MouseAction  action;
	register long  x, y, num_clicks;
{
#if 0
	register struct gob *dobj
		= (struct gob *)self->header.view.dataobject;
DEBUG(("Hit at (%d, %d) type %d\n", x, y, action));
	if (action == view_NoMouseEvent)
		return (struct view *)self;
	if (! self->OnScreen) return NULL;

	/* $$$ replace the following section with code to provide the
		desired response to mouse actions.  In this example, 
		a new ob is drawn and added to the data structure
		at the coordinates of the mouse hit.  It is a convention
		that permanent actions should take place on Up movement
		of a mouse button.   */
	if (action == view_LeftDown || action == view_RightDown) {
		if ( ! self->HasInputFocus) {
			gobview_WantInputFocus(self, self);
			self->ignoreUp = TRUE;
		}
	}
	else if (action == view_LeftUp || action == view_RightUp) {
		struct rectangle r;

		if (self->ignoreUp) {
			self->ignoreUp = FALSE;
			return NULL;
		}
		gobview_GetLogicalBounds(self, &r);	/* find rectangle bounds */
		if (x < rectangle_Left(&r)  ||  x >= rectangle_Right(&r)
				|| y < rectangle_Top(&r)
				|| y >= rectangle_Bottom(&r))
			/* if the mouse up is out of the rectangle, we do not
				want to add a ob.  Note the '=' signs;
				rectangles are not symmetrically bounded.  */
			return NULL;

		gob_AddOb(dobj, x, y);
		gob_NotifyObservers(dobj, gob_DATACHANGED);

		/* draw the image immediately
			(It will be redrawn by the Update routine
			called because of the NotifyObservers) */
		gobview_SetTransferMode(self, graphic_COPY);
		rectangle_SetRectSize(&r, x, y, 10, 7);
		gobview_FillRect(self, &r, self->BlackPattern);
	}

LEAVE(Hit);
#endif /* 0 */
	return (struct view *)self;		/* where to send subsequent hits */
}

/* 	$$$ replace with some routine that responds with a size for your view */
/* gobview__DesiredSize(self, width, height, pass, desiredWidth, desiredHeight) 
	The parent calls this to find out how big the view would like to be.
	This routine sets 'desiredWidth' and 'desiredHeight' and returns a
		code value indicating which is flexible for further negotiation.
	The 'width' and 'height' parameters are tentative values the parent is suggesting.
	'pass' indicates which of 'width' and 'height' should be considered fixed.
	If neither is fixed, they may be arbitrary values. 
*/
	enum view_DSattributes
gobview__DesiredSize(self, width, height, pass, desiredWidth, desiredHeight) 
	register struct gobview *self;
	long width;
	long height;
	enum view_DSpass pass;
	long *desiredWidth;
	long *desiredHeight;
{
	DEBUG(("DesiredSize(...%d, %d, %d...)\n", width, height, pass));

	ComputeArea(self);	/* set self->DesiredArea/Width/Height */

	if (pass == view_NoSet) {
		*desiredWidth = self->DesiredWidth;
		*desiredHeight = self->DesiredHeight;
	}
	else if (pass == view_WidthSet) {
		*desiredWidth = width;
		*desiredHeight = self->DesiredArea / width;
	}
	else /* pass == view_HeightSet */ {
		*desiredWidth = self->DesiredWidth / height;
		*desiredHeight = height;
	}

	DEBUG(("Leave DesiredSize: %d x %d\n", *desiredWidth, *desiredHeight));
	return view_HeightFlexible | view_WidthFlexible;
}
/* # # # # # # # # # # # # # # 
 *	PRINTING	
 *  # # # # # # # # # # # # #  */

	void
gobview__Print(self, file, processor, format, topLevel)
	register struct gobview 	*self;
	register FILE  *file;
	register char   *processor;
	register char   *format;
	register boolean   topLevel;
{
#ifdef texttroff_HASPBPE		/* use this until new texttroff is everywhere  */
	register struct gob *dobj = (struct gob *)self->header.view.dataobject;
	long wpts, hpts;  /* image dimensions in points */
	struct oblist *ob;
	char *prefix;

	/* $$$  If you want to describe the print image of your view in troff,
		replace the contents of this routine.  To use PostScript,
		replace the pieces noted below. */

	wpts = gobview_GetLogicalWidth(self);
	hpts = gobview_GetLogicalHeight(self);
	if (wpts == 0  ||  hpts == 0) {
		/* the parent has GOOFED and has not
			supplied a logical rectangle for printing */
		ComputeArea(self);
		wpts = self->DesiredWidth;
		hpts = self->DesiredHeight;
	}

	/* We should restrict wpts and hpts to (say) 576x720  (8x10 inches).
		We could do so by scaling the image */

	if (strcmp(processor, "troff") == 0) {
		/* output to troff */
		if (topLevel)
			/* take care of initial troff stream */
			texttroff_BeginDoc(file);
		/*  Put macro to interface to postscript */
		texttroff_BeginPS(file, wpts, hpts);
		prefix = "\\!  ";
	}
	else prefix = "";

	/* generate PostScript  */
	fprintf(file, "%s  /width %d def  /height %d def\n", prefix, wpts, hpts);
	fprintf(file, "%s  newpath 0 0 moveto 0 height lineto width height lineto\n", prefix);
	fprintf(file, "%s  \twidth 0 lineto clip    %% clip to assigned area\n", prefix);	

	/* $$$ if you want to describe the image of your inset in postscript, replace the 
	following fprints and the for-loop below with code to generate the PostScript. */

	/* define a postscript routine to display a ob */
	fprintf(file, "%s  /blob {\n", prefix);
	fprintf(file, "%s  \tneg height add    %% convert from screen to postscript coords\n",
			prefix);
	fprintf(file, "%s  \tmoveto	%% use coords from call\n", prefix);
	fprintf(file, "%s  \t10 0 rlineto\n", prefix);
	fprintf(file, "%s  \t0 -7 rlineto\n", prefix);
	fprintf(file, "%s  \t-10 0 rlineto\n", prefix);
	fprintf(file, "%s  \tfill\n", prefix);
	fprintf(file, "%s  } def\n", prefix);

	fprintf(file, "%s  newpath width 0 moveto 0 height lineto 0.5 setlinewidth stroke \t\n",
			prefix);		/* draw diagonal line */

	/* draw the obs */
	for (ob = gob_GetFirstOb(dobj); ob != NULL; 
			ob = gob_GetNextOb(dobj, ob))
		fprintf(file, "%s  %d %d blob\n", prefix,
			gob_GetObX(dobj, ob),
			gob_GetObY(dobj, ob));

	if (strcmp(processor, "troff") == 0){
		texttroff_EndPS(file, wpts, hpts);
		if (topLevel)
			texttroff_EndDoc(file);
	}
#endif /* texttroff_HASPBPE		 */
}
