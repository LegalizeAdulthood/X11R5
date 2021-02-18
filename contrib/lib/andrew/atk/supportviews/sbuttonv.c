/* ********************************************************************* *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/sbuttonv.c,v 1.11 1991/09/29 20:24:03 rr2b Exp $ */
/* $ACIS:sbuttonv.c 1.6$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/sbuttonv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/sbuttonv.c,v 1.11 1991/09/29 20:24:03 rr2b Exp $";
#endif /* lint */

#include <stdio.h>
#include <class.h>
#include <andrewos.h>
#include <environ.ih>
#include <fontdesc.ih>
#include <graphic.ih>
#include <im.ih>
#include <observe.ih>
#include <cursor.ih>

#include <view.ih>

#include "sbutton.ih"
#include "sbuttonv.eh"

/* Defined constants and macros */
#if 0
#define DEBUG 1			/* turn on debugging */
#endif
#ifdef DEBUG
#define DBG(x) fprintf(stderr, "\nDebug: %s.", x);fflush(stderr);
#else
#define DBG(x) ;
#endif

#define NO_MSG "Push Me"

#define PROMPTFONT "andysans12b"
#define FONT "andysans"
#define FONTTYPE fontdesc_Bold
#define FONTSIZE 12
#define BUTTONDEPTH 4
#define ULSHADE 0.25 /* upper & left sides */
#define LRSHADE 0.75 /* lower & right sides */
#define TOPSHADE 0.50 /* face of button */
#define MOTIFBUTTONDEPTH 2
#define MOTIFULSHADE 0.10
#define MOTIFLRSHADE 0.50
#define MOTIFTOPSHADE 0.25
#define BUTTONPRESSDEPTH 2
#define TEXTPAD 2
#define EXTRATEXTPAD 3

#define TEXTINMIDDLE (graphic_BETWEENLEFTANDRIGHT | graphic_BETWEENTOPANDBOTTOM)

#define DEFAULTSTYLE(style) ((style<0 || style>sbutton_MOTIF)?sbutton_MOTIF:(style))

#define BUTTONS(self) (sbuttonv_ButtonData(self)->buttons)

/* External Declarations */

/* Global Variables */
static struct atom *buttonpushed=NULL;

static void InitFGBG(self, prefs, fg, bg)
struct view *self;
struct sbutton_prefs *prefs;
double *fg, *bg;
{

    char *bgcolor=sbutton_GetBackground(prefs);
    char *fgcolor=sbutton_GetForeground(prefs);
    char *deffg, *defbg;

    graphic_GetDefaultColors(&deffg, &defbg);

    if(bgcolor==NULL) bgcolor=defbg?defbg:"white";
    if(fgcolor==NULL) fgcolor=deffg?deffg:"black";

    /* This is necessary for consistent results on monochrome machines, the color setting functions try to pick a stipple on monochrome screens and they determine which stipple to use based on the fg (or bg for setfg) color in effect */ 
    view_SetBackgroundColor(self, bgcolor, 0, 0, 0);
    view_SetForegroundColor(self, fgcolor, 0, 0, 0);
    view_SetBackgroundColor(self, bgcolor, 0, 0, 0);

    view_GetBGColor(self, &bg[0], &bg[1], &bg[2]);
    view_GetFGColor(self, &fg[0], &fg[1], &fg[2]);
}

static void MySetShade(self, prefs, foreground, background, color)
struct view *self;
struct sbutton_prefs *prefs;
double *foreground, *background;
int color;
{
    double pct;

    int style=DEFAULTSTYLE(prefs->style);
    
    if(prefs->colors[color]) view_SetForegroundColor(self, prefs->colors[color], 0, 0, 0);
    else {
	switch(color) {
	    case sbutton_LABELBG:
	    case sbutton_BACKGROUND:
		pct=0.0;
		break;
	    case sbutton_TOPSHADOW:
		if(style==sbutton_MOTIF) pct=MOTIFULSHADE;
		else pct=ULSHADE;
		break;
	    case sbutton_TOP:
		switch(style) {
		    case sbutton_MOTIF:
			pct=MOTIFTOPSHADE;
			break;
		    case sbutton_THREEDEE:
			pct=TOPSHADE;
			break;
		    default:
			pct=0.0;
			break;
		}
		break;
	    case sbutton_BOTTOMSHADOW:
		if(style==sbutton_MOTIF) pct=MOTIFLRSHADE;
		else pct=LRSHADE;
		break;
	    case sbutton_LABELFG:
	    case sbutton_FOREGROUND:
		pct=1.0;
	}

	view_SetFGColor(self, foreground[0]*pct 
			+ background[0]*(1.0-pct), 
			foreground[1]*pct 
			+ background[1]*(1.0-pct), 
			foreground[2]*pct 
			+ background[2]*(1.0-pct));
    }
}

void sbuttonv__SaveViewState(classID, self, vi)
struct classheader *classID;
struct view *self;
struct sbuttonv_view_info *vi;
{
    vi->transfermode=view_GetTransferMode(self);
    view_GetFGColor(self, &vi->fgr, &vi->fgg, &vi->fgb);
    view_GetBGColor(self, &vi->bgr, &vi->bgg, &vi->bgb);
    vi->font=view_GetFont(self);
}

void sbuttonv__RestoreViewState(classID, self, vi)
struct classheader *classID;
struct view *self;
struct sbuttonv_view_info *vi;
{
    view_SetTransferMode(self, vi->transfermode);
    view_SetBGColor(self, vi->bgr, vi->bgg, vi->bgb);
    view_SetFGColor(self, vi->fgr, vi->fgg, vi->fgb);
    view_SetBGColor(self, vi->bgr, vi->bgg, vi->bgb);
    view_SetFont(self, vi->font);
}

void sbuttonv__SafeDrawButton(classID, self, si, r)
struct classheader *classID;
struct view *self;
struct sbutton_info *si;
struct rectangle *r;
{
    struct sbuttonv_view_info vi;
    sbuttonv_SaveViewState(self, &vi);
    sbuttonv_DrawButton(self, si, r);
    sbuttonv_RestoreViewState(self, &vi);
}

void sbuttonv__SizeForBorder(classID, self, dir,  style, lit, w, h, rw, rh)
struct classheader *classID;
struct view *self;
enum sbuttonv_conv dir;
int style;
boolean lit;
long w, h;
long *rw, *rh;
{
    style=DEFAULTSTYLE(style);
    
    switch(style) {
	case sbutton_BOXEDRECT:
	    if(dir==sbuttonv_Enclosing) {
		*rw=w + 2*BUTTONDEPTH + 3;
		*rh=h + 2*BUTTONDEPTH + 3;
	    } else {
		*rw=w - (2*BUTTONDEPTH + 3);
		*rh=h - (2*BUTTONDEPTH + 3);
	    }
	    break;
	case sbutton_THREEDEE:
	    if(dir==sbuttonv_Enclosing) {
		*rw=w + 2*BUTTONDEPTH;
		*rh=h + 2*BUTTONDEPTH;
	    } else {
		*rw=w - 2*BUTTONDEPTH;
		*rh=h - 2*BUTTONDEPTH;
	    }
	    break;
	case sbutton_MOTIF:
	    if(dir==sbuttonv_Enclosing) {
		*rw=w + 2*MOTIFBUTTONDEPTH +4;
		*rh=h + 2*MOTIFBUTTONDEPTH +4;
	    } else {
		*rw=w - 2*MOTIFBUTTONDEPTH -4;
		*rh=h - 2*MOTIFBUTTONDEPTH -4;
	    }
	    break;
	case sbutton_PLAINBOX:
	    if(dir==sbuttonv_Enclosing) {
		*rw=w + 2;
		*rh=h + 2;
	    } else {
		*rw=w - 2;
		*rh=h - 2;
	    }
	    break;
	default:
	    *rw=w;
	    *rh=h;
	    break;
    }
}

void sbuttonv__DrawRectBorder(classID, self, enclosing, prefs, inout, draw, interior)
struct classheader *classID;
struct view *self;
struct rectangle *enclosing;
struct sbutton_prefs *prefs;
boolean inout, draw;
struct rectangle *interior;
{
    sbuttonv_DrawBorder(self, enclosing->left, enclosing->top, enclosing->width, enclosing->height, prefs, inout, draw, interior);
}

void sbuttonv__DrawBorder(classID, self, x, y, w, h, prefs, inout, draw, interior)
struct classheader *classID;
struct view *self;
long x, y, w, h;
struct sbutton_prefs *prefs;
boolean inout;
struct rectangle *interior;
boolean draw;
{
    struct rectangle Rect2;
    int bdepth, r_bot, r2_bot;
    struct rectangle r;
    int style;
    double foreground[3], background[3];
    int ts, bs;

    style=DEFAULTSTYLE(prefs->style);
    
    r.left=x;
    r.top=y;
    r.width=w;
    r.height=h;

    if(interior!=NULL) *interior=r;

    if(draw) {
	InitFGBG(self, prefs, &foreground[0], &background[0]);
	view_SetTransferMode(self, graphic_SOURCE);
	if ((style != sbutton_THREEDEE) && (style != sbutton_MOTIF)) {
	    /* Erase with TOP color, only if style is not 3-D (3-D draws all bits) */
	    MySetShade(self, prefs,  foreground, background, sbutton_TOP);
	    view_FillRectSize(self, r.left, r.top, r.width, r.height, NULL);
	}
	MySetShade(self, prefs,  foreground, background, sbutton_FOREGROUND);
    }
    
    switch (style) {
	case sbutton_BOXEDRECT:
	    /* Rect2 is the inner rect */
	    Rect2.top = r.top + BUTTONDEPTH;
	    Rect2.left = r.left + BUTTONDEPTH;
	    Rect2.width = r.width - 2*BUTTONDEPTH - 1;
	    Rect2.height = r.height - 2*BUTTONDEPTH - 1;
	    if(draw) {
		view_DrawRectSize(self, r.left, r.top, r.width -1, r.height - 1);
		view_DrawRect(self, &Rect2);
	    }
	    if(interior) {
		interior->top=Rect2.top+1;
		interior->left=Rect2.left+1;
		interior->width=Rect2.width-2;
		interior->height=Rect2.height-2;
	    }
	    if(inout && draw) {
		view_SetTransferMode(self, graphic_INVERT);
		view_FillRect(self, &r, view_BlackPattern(self));
		view_FillRect(self, &Rect2, view_BlackPattern(self));
	    }

	    break;

	case sbutton_THREEDEE:
	case sbutton_MOTIF:
	    ts=sbutton_TOPSHADOW;
	    bs=sbutton_BOTTOMSHADOW;
	    if (style == sbutton_MOTIF) {
		bdepth = MOTIFBUTTONDEPTH;
		if(inout) {
		    ts=sbutton_BOTTOMSHADOW;
		    bs=sbutton_TOPSHADOW;
		}
	    } else bdepth = BUTTONDEPTH;
	    /* Rect2 is the inner (Text) region */
	    Rect2.top = r.top + bdepth;
	    Rect2.left = r.left + bdepth;
	    Rect2.width = r.width - 2*bdepth;
	    Rect2.height = r.height - 2*bdepth;
	    if(interior) *interior=Rect2;
	    r2_bot = (Rect2.top)+(Rect2.height);
	    r_bot = (r.top)+(r.height);

	    if(draw) {
		{
		    view_SetTransferMode(self, graphic_SOURCE);
		    MySetShade(self, prefs, foreground, background, ts);
		    view_FillRectSize(self, r.left, r.top, bdepth, r.height, NULL);	/* left bar */

		    MySetShade(self, prefs, foreground, background, bs);
		    view_FillRectSize(self, r.left + r.width - bdepth, r.top, bdepth, r.height, NULL); /* right bar */
		    view_FillTrapezoid(self, Rect2.left, r2_bot, Rect2.width, r.left, r_bot, r.width, NULL); /* lower trapz */

		    MySetShade(self, prefs,  foreground, background, ts);
		    view_FillTrapezoid(self, r.left, r.top, r.width, Rect2.left, Rect2.top, Rect2.width, NULL); /* upper trapz */
	
		    MySetShade(self, prefs,  foreground, background, sbutton_TOP);
		    view_FillRect(self, &Rect2, NULL); /* the middle box */
		}
	    }
	    break;

	case sbutton_PLAINBOX:
	    if(draw) view_DrawRectSize(self, r.left, r.top, r.width-1, r.height-1);
	    if(interior) {
		interior->left=r.left+1;
		interior->width=r.width-2;
		interior->top=r.top+1;
		interior->height=r.height-2;
	    }
	    /* FALLTHROUGH */
	default: ;
    } /* switch (style) */
    if(interior && interior->width<=0) interior->width=1;
    if(interior && interior->height<=0) interior->height=1;
}

void sbuttonv__InteriorBGColor(classID, self, prefs, lit,  result)
struct classheader *classID;
struct view *self;
struct sbutton_prefs *prefs;
boolean lit;
double *result;
{
    double topshade;
    double fore[3], back[3];
    struct sbuttonv_view_info vi;
    int style;
    
    sbuttonv_SaveViewState(self, &vi);
    
    InitFGBG(self, prefs, &fore[0], &back[0]);

    if(prefs->colors[sbutton_TOP]) {
	view_SetForegroundColor(self, prefs->colors[sbutton_TOP], 0, 0, 0);
	view_GetFGColor(self, result, result+1, result+2);
    } else {

	style=DEFAULTSTYLE(prefs->style);
	switch(style) {
	    case sbutton_THREEDEE:
		topshade = 0;
		break;
	    case sbutton_MOTIF:
		topshade = MOTIFTOPSHADE;
		break;
	    default:
		topshade=0;
		break;
	}
	result[0]= fore[0]*topshade + back[0]*(1.0-topshade);
	result[1]= fore[1]*topshade + back[1]*(1.0-topshade);
	result[2]= fore[2]*topshade + back[2]*(1.0-topshade);
    }
    sbuttonv_RestoreViewState(self, &vi);
}

static void DrawText(self, font, x, y, text, len, flags)
struct view *self;
struct fontdesc *font;
long x, y;
char *text;
int len;
long flags;
{
    if(flags==TEXTINMIDDLE && len==1 && font!=NULL) {
	long tx, ty;
	struct fontdesc_charInfo ci;
	fontdesc_CharSummary(font, view_GetDrawable(self), *text, &ci);
	tx = ci.width + 4;
	ty = ci.height + 4;
	view_MoveTo(self, x - tx/2 + ci.xOriginOffset + 2, y - ty/2 + ci.yOriginOffset +2);
	view_DrawText(self, text, len, 0);
    } else {
	view_MoveTo(self, x, y);
	view_DrawText(self, text, len, flags);
    }
}

static void DrawLabel(self, text, lit, prefs, fg, bg, x, y, flags)
struct view *self;
char *text;
boolean lit;
struct sbutton_prefs *prefs;
double *fg, *bg;
long x, y;
long flags;
{
    int style;
    int len;
    int slb, slf;
    struct fontdesc *my_fontdesc;

    if(*text=='\0') return;

    len=strlen(text);
    
    style = DEFAULTSTYLE(sbutton_GetStyle(prefs));

    if (!(my_fontdesc = sbutton_GetFont(prefs))) {
	my_fontdesc= fontdesc_Create(FONT, FONTTYPE, FONTSIZE);
    }
    if (my_fontdesc) {
	view_SetFont(self, my_fontdesc);
    }

    slb=sbutton_LABELBG;
    slf=sbutton_LABELFG;
    view_SetTransferMode(self, graphic_BLACK);
    switch(style) {
	case sbutton_THREEDEE:
	    if(lit) {
		slb=sbutton_LABELFG;
		slf=sbutton_LABELBG;
	    }
	    /* FALLTHROUGH */
	case sbutton_MOTIF:
	    MySetShade(self, prefs, fg, bg, slb);
	    DrawText(self, my_fontdesc, x+1, y, text, len, flags);
	    DrawText(self, my_fontdesc, x, y+1, text, len, flags);
	    DrawText(self, my_fontdesc, x+1, y+1, text, len, flags);

	    MySetShade(self, prefs, fg, bg,  slf);
	    DrawText(self, my_fontdesc, x, y, text, len, flags);
	    break;
	case sbutton_PLAINBOX:
	case sbutton_BOXEDRECT:
	default:
	    MySetShade(self, prefs, fg, bg, sbutton_LABELFG);
	    DrawText(self, my_fontdesc, x, y, text, len, flags);
    }
}
    
void sbuttonv__DrawLabel(classID, self, text,  x, y, prefs, lit, flags)
struct classheader *classID;
struct view *self;
char *text;
long x, y;
struct sbutton_prefs *prefs;
boolean lit;
long flags;
{
    
    double fg[3], bg[3];

    if(text==NULL || *text=='\0') return;
    
    InitFGBG(self, prefs, fg, bg);

    if(flags<0) flags=TEXTINMIDDLE;
    DrawLabel(self, text, lit, prefs, fg, bg, x, y, flags);
}

void sbuttonv__DrawButtonLabel(classID, self, text, interior, prefs, lit)
struct classheader *classID;
struct view *self;
char *text;
struct rectangle *interior;
struct sbutton_prefs *prefs;
boolean lit;
{
    long tx, ty;
    int style=DEFAULTSTYLE(sbutton_GetStyle(prefs));
    if(style==sbutton_BOXEDRECT) {
	tx = TEXTPAD + interior->left + ((interior->width-1) / 2);
	ty = TEXTPAD + interior->top + ((interior->height-1) /2);
    } else {
	tx = interior->left + ((interior->width-1) / 2);
	ty = interior->top + ((interior->height-1) / 2);
    }
    sbuttonv_DrawLabel(self, text, tx, ty, prefs, lit, -1);
}

void sbuttonv__DrawButton(classID, self, si, r)
struct classheader *classID;
struct view *self;
struct sbutton_info *si;
struct rectangle *r;
{
    int style;
    struct rectangle Rect2;
    int tx, ty;
    int bdepth, r_bot, r2_bot;
    char *text=si->label?si->label:NO_MSG;
    double fg[3], bg[3];
    
    style = DEFAULTSTYLE(sbutton_GetStyle(si->prefs));
    
    InitFGBG(self, si->prefs, fg, bg);
       
    view_SetTransferMode(self, graphic_SOURCE);
    if ((style != sbutton_THREEDEE) && (style != sbutton_MOTIF)) {
	/* Erase with top color, only if style is not 3-D (3-D draws all bits) */
	MySetShade(self, si->prefs,  fg, bg, sbutton_TOP);
	view_FillRectSize(self, r->left, r->top, r->width, r->height, NULL);
    }
    MySetShade(self, si->prefs,  fg, bg, sbutton_FOREGROUND);

    switch (style) {
	case sbutton_BOXEDRECT:
	    /* Rect2 is the inner rect */
	    Rect2.top = r->top + BUTTONDEPTH;
	    Rect2.left = r->left + BUTTONDEPTH;
	    Rect2.width = r->width - 2*BUTTONDEPTH - 1;
	    Rect2.height = r->height - 2*BUTTONDEPTH - 1;
	    tx = TEXTPAD + Rect2.left + ((Rect2.width-1) / 2);
	    ty = TEXTPAD + Rect2.top + ((Rect2.height-1) /2);
	    view_SetTransferMode(self, graphic_COPY);
	    view_DrawRectSize(self, r->left, r->top, r->width -1, r->height - 1);
	    view_DrawRect(self, &Rect2);
	    break;

	case sbutton_MOTIF:
	case sbutton_THREEDEE:
	    if (style == sbutton_MOTIF) {
		bdepth = MOTIFBUTTONDEPTH;
	    } else {
		bdepth = BUTTONDEPTH;
	    }


	    /* Rect2 is the inner (Text) region */
	    Rect2.top = r->top + bdepth;
	    Rect2.left = r->left + bdepth;
	    Rect2.width = r->width - 2*bdepth;
	    Rect2.height = r->height - 2*bdepth;

	    r2_bot = (Rect2.top)+(Rect2.height);
	    r_bot = (r->top)+(r->height);


	    view_SetTransferMode(self, graphic_COPY);
	    MySetShade(self, si->prefs,  fg, bg, sbutton_TOPSHADOW);
	    view_FillRectSize(self, r->left, r->top, bdepth, r->height, NULL);	/* left bar */

	    MySetShade(self, si->prefs,  fg, bg, sbutton_BOTTOMSHADOW);
	    view_FillRectSize(self, r->left + r->width - bdepth, r->top, bdepth, r->height, NULL); /* right bar */
	    view_FillTrapezoid(self, Rect2.left, r2_bot, Rect2.width, r->left, r_bot, r->width, NULL); /* lower trapz */

	    MySetShade(self, si->prefs,  fg, bg, sbutton_TOPSHADOW);
	    view_FillTrapezoid(self, r->left, r->top, r->width, Rect2.left, Rect2.top, Rect2.width, NULL); /* upper trapz */

	    MySetShade(self, si->prefs,  fg, bg, sbutton_TOP);
	    view_FillRect(self, &Rect2, NULL); /* the middle box */

	    MySetShade(self, si->prefs,  fg, bg, sbutton_FOREGROUND);

	    tx = r->left + ((r->width-1) / 2);
	    ty = r->top + ((r->height-1) / 2);
	    
	    break;

	case sbutton_PLAINBOX:
	    
	    view_SetTransferMode(self, graphic_BLACK);
	    view_DrawRectSize(self, r->left, r->top, r->width-1, r->height-1);

	    tx = r->left + ((r->width-1) / 2);
	    ty = r->top + ((r->height-1) / 2);

	    
	    break;

	default: /* PLAIN */
	    tx = r->left + ((r->width-1) / 2);
	    ty = r->top + ((r->height-1) / 2);
	    break;
    } /* switch (style) */
    DrawLabel(self, text, FALSE, si->prefs, fg, bg, tx, ty, TEXTINMIDDLE);
    if(si->lit) sbuttonv_HighlightButton(self, si, r);
}

boolean sbuttonv__InitializeClass(c)
struct classheader *c;
{
    buttonpushed=atom_Intern("buttonpushed");
    if(buttonpushed==NULL) return FALSE;
    return(TRUE);
}

boolean sbuttonv__InitializeObject(c, self)
struct classheader *c;
struct sbuttonv *self;
{
/*
  Set up the data for each instance of the object.
*/
    self->lwidth=self->lheight=(-1);
    
    self->bcount=0;
    
    self->info=NULL;
    
    self->dotriggers=TRUE;
    
    self->activebuttons=sbuttonv_LEFTBUTTON | sbuttonv_RIGHTBUTTON;
    
    self->vborder=environ_GetProfileInt("sbuttonvborder", 5);
    self->hborder=environ_GetProfileInt("sbuttonhborder", 5);

    self->hspacing=environ_GetProfileInt("sbuttonhspacing", 5);
    self->vspacing=environ_GetProfileInt("sbuttonvspacing", 5);

    self->needredraw=FALSE;
    self->lastbutton=0;
    self->forceupdate=FALSE;
    self->lasthighlight=(-1);
    self->awaitingUpdate = 0;
    observable_DefineTrigger(self, buttonpushed);
    return(TRUE);
}

unsigned char sbuttonv__SetActiveMouseButtons(self, active, deactive)
struct sbuttonv *self;
unsigned char active, deactive;
{
    unsigned char oldbs=self->activebuttons;
    self->activebuttons|=active;
    self->activebuttons&=~deactive;
    return oldbs;
}

void sbuttonv__LinkTree(self, parent)
struct sbuttonv *self;
struct view *parent;
{
    super_LinkTree(self, parent);

} /* sbuttonv__LinkTree */


void sbuttonv__FinalizeObject(c, self)
struct classheader *c;
struct sbuttonv *self;
{

    if(self->info) free(self->info);
}

static void EnsureInfo(self)
struct sbuttonv *self;
{
    struct sbutton *b=sbuttonv_ButtonData(self);
    long ocount=self->bcount;
    if(b->count!=self->bcount) {
	self->bcount=b->count;

	if(self->info) self->info=(struct sbuttonv_info *)realloc(self->info, self->bcount*sizeof(struct sbuttonv_info));
	else self->info=(struct sbuttonv_info *)malloc(self->bcount*sizeof(struct sbuttonv_info));
	if(!self->info) self->bcount=0;
	while(ocount<self->bcount) {
	    self->info[ocount].drawflag=sbutton_LABELCHANGED;
	    ocount++;
	}
    }
}

#define MAXWIDTH(col, max, left) ((col)<(left)?max+1:max)
#define MAXHEIGHT(row, max, left) ((row)<(left)?max+1:max)

void sbuttonv__FullUpdate(self, type, left, top, width, height)
struct sbuttonv *self;
enum view_UpdateType type;
long left, top, width, height;
{
/*
  Redisplay this object.  Specifically, set my font, and put my text label
  in the center of my display box. */
  struct rectangle vr;
  struct sbutton *b =  sbuttonv_ButtonData(self);
  int redraw;
  
  sbuttonv_GetLogicalBounds(self, &vr);

  self->lwidth=vr.width;
  self->lheight=vr.height;

  if(sbutton_GetMattePrefs(b)) sbuttonv_DrawRectBorder((struct view *)self, &vr, sbutton_GetMattePrefs(b), TRUE, TRUE, &vr);
  
  switch (type) {
    case view_FullRedraw:
    case view_LastPartialRedraw:
      redraw = 1;
      break;
    case view_MoveNoRedraw:
      redraw = 0;
      break;
  case view_PartialRedraw:
      return;
    case view_Remove:
     return;
    default:
      redraw = 1;
  }


  if (b) {
      int count=b->count;
      int maxwidth=(rectangle_Width(&vr) - 2*self->hborder + self->hspacing)/b->cols;
      int maxheight=(rectangle_Height(&vr) - 2*self->vborder + self->vspacing)/b->rows;
      int x=rectangle_Left(&vr) + self->hborder, y=rectangle_Top(&vr) + self->vborder;
      int vleftovers=(rectangle_Height(&vr)  - 2*self->vborder + self->vspacing) % b->rows;
      int hleftovers=(rectangle_Width(&vr) - 2*self->hborder + self->hspacing) % b->cols;
      int col=0;
      int row=0;
      struct sbuttonv_view_info v;
      
      self->maxwidth=self->specialwidth=maxwidth;
      self->maxheight=maxheight;

       
      if(redraw) sbuttonv_SaveViewState((struct view *)self, &v);
      EnsureInfo(self);
      
      while(--count>=0) {
	  rectangle_SetRectSize( &self->info[count].rect, x, y, MAXWIDTH(col, maxwidth, hleftovers) - self->hspacing, MAXHEIGHT(row, maxheight, vleftovers) - self->vspacing);
	  if(redraw) {
	      self->info[count].drawflag=0;
	      sbuttonv_DrawButton((struct view *)self, &BUTTONS(self)[count], &self->info[count].rect);
	  }
	  x+=MAXWIDTH(col, maxwidth, hleftovers);
	  col++;
	  if(x + MAXWIDTH(col, maxwidth, hleftovers) + self->hborder - self->hspacing> rectangle_Right(&vr)) {
	      col=0;
	      x=rectangle_Left(&vr) + self->hborder;
	      if(count>0 && count<b->cols) {
		  self->specialwidth = maxwidth = (rectangle_Width(&vr) - 2*self->hborder + self->hspacing)/count;
		  hleftovers=rectangle_Width(&vr)%maxwidth;
	      }
	      y+=MAXHEIGHT(row, maxheight, vleftovers);
	      row++;
	  }
      }
      if(redraw) sbuttonv_RestoreViewState((struct view *)self, &v);
  } /* if (b && redraw) */
}


void sbuttonv__Update(self)
struct sbuttonv *self;  
{

    struct sbutton *b=sbuttonv_ButtonData(self);
    struct rectangle r;
    int i;
    sbuttonv_GetLogicalBounds(self, &r);

    self->awaitingUpdate=FALSE;
    
    if(self->needredraw || self->bcount!=b->count || r.width!=self->lwidth || r.height!=self->lheight) {
	self->needredraw=FALSE;
	sbuttonv_FullUpdate(self, view_FullRedraw, r.left, r.top, r.width, r.height);
	return;
    }

    for(i=0;i<self->bcount;i++) {
	if(self->info[i].drawflag&sbutton_LABELCHANGED) sbuttonv_DrawButton((struct view *)self, &BUTTONS(self)[i], &self->info[i].rect);
	else if(self->info[i].drawflag&sbutton_ACTIVATIONCHANGED) {
	    if(sbutton_GetLit(b, i))
		sbuttonv_HighlightButton(self, &sbutton_GetButtons(b)[i], &self->info[i].rect);
	    else sbuttonv_UnHighlightButton(self, &sbutton_GetButtons(b)[i], &self->info[i].rect);
	}
	self->info[i].drawflag=0;
    }
    im_FlushGraphics(sbuttonv_GetIM(self));
}


static int RectEnclosesXY(r, x, y)
struct rectangle *r;
long x, y;
{
  return(   ( ((r->top)  <= y) && ((r->top + r->height) >= y) )
	 && ( ((r->left) <= x) && ((r->left + r->width) >= x) )
	 );
}


static void sbuttonv__HighlightButton(classID, self, si, r)
struct classheader *classID;
struct view *self;
struct sbutton_info *si;
struct rectangle *r;
{
    struct rectangle Rect2;
    struct fontdesc *my_fontdesc;
    int tx, ty;
    int bdepth, r2_bot, r_bot;
    struct sbuttonv_view_info vi;
    int style=DEFAULTSTYLE(sbutton_GetStyle(si->prefs));
    char *text=si->label?si->label:NO_MSG;
    double fg[3], bg[3];
    
    sbuttonv_SaveViewState(self, &vi);

    InitFGBG(self, si->prefs, fg, bg);

    switch (style) {
	case sbutton_PLAIN:
	case sbutton_PLAINBOX:
	    view_SetTransferMode(self, graphic_INVERT);
	    view_FillRect(self, r, view_BlackPattern(self));
	    break;

	case sbutton_BOXEDRECT:
	    /* Rect2 is the inner si->r */
	    Rect2.top = r->top + BUTTONDEPTH;
	    Rect2.left = r->left + BUTTONDEPTH;
	    Rect2.width = r->width - 2*BUTTONDEPTH;
	    Rect2.height = r->height - 2*BUTTONDEPTH;

	    view_SetTransferMode(self, graphic_INVERT);
	    view_FillRect(self, r, view_BlackPattern(self));
	    view_FillRect(self, &Rect2, view_BlackPattern(self));

	    break;

	case sbutton_MOTIF:
	case sbutton_THREEDEE:
	    if (style == sbutton_MOTIF) {
		bdepth = MOTIFBUTTONDEPTH;
	    } else {
		bdepth = BUTTONDEPTH;
	    }

	    /* Rect2 is the inner (Text) region */
	    Rect2.top = r->top + bdepth;
	    Rect2.left = r->left + bdepth;
	    Rect2.width = r->width - 2*bdepth;
	    Rect2.height = r->height - 2*bdepth;
	    r2_bot = (Rect2.top)+(Rect2.height);
	    r_bot = (r->top)+(r->height);

	    if (style == sbutton_MOTIF) {
		view_SetTransferMode(self, graphic_COPY);
		MySetShade(self, si->prefs,  fg, bg, sbutton_BOTTOMSHADOW);
		view_FillRectSize(self, r->left, r->top, bdepth, r->height, NULL);	/* left bar */

		MySetShade(self, si->prefs,  fg, bg, sbutton_TOPSHADOW);
		view_FillRectSize(self, r->left + r->width - bdepth, r->top, bdepth, r->height, NULL); /* right bar */
		view_FillTrapezoid(self, Rect2.left, r2_bot, Rect2.width, r->left, r_bot, r->width, NULL); /* lower trapz */

		MySetShade(self, si->prefs,  fg, bg, sbutton_BOTTOMSHADOW);
		view_FillTrapezoid(self, r->left, r->top, r->width, Rect2.left, Rect2.top, Rect2.width, NULL); /* upper trapz */

		MySetShade(self, si->prefs,  fg, bg, sbutton_FOREGROUND);

	    } else {
		if (!(my_fontdesc = sbutton_GetFont(si->prefs))) {
		    my_fontdesc= fontdesc_Create(FONT, FONTTYPE, FONTSIZE);
		}
		if (my_fontdesc) {
		    view_SetFont(self, my_fontdesc);
		}

		tx = r->left + ((r->width-1) / 2);
		ty = r->top + ((r->height-1) / 2);
		DrawLabel(self, text, TRUE, si->prefs, fg, bg, tx, ty, TEXTINMIDDLE);
	    } /* MOTIF? */

	    break;
    }
    sbuttonv_RestoreViewState(self, &vi);
    si->lit = 1;
}


static void sbuttonv__UnHighlightButton(classID, self, si, r)
struct classheader *classID;
struct view *self;
struct sbutton_info *si;
struct rectangle *r;
{
    struct rectangle Rect2;
    int tx, ty;
    int bdepth, r2_bot, r_bot;
    char *text=si->label?si->label:NO_MSG;
    double fg[3], bg[3];
    int style=DEFAULTSTYLE(sbutton_GetStyle(si->prefs));
    
    struct sbuttonv_view_info vi;
    sbuttonv_SaveViewState(self, &vi);
    InitFGBG(self, si->prefs, fg, bg);
    
    switch (style) {
	case sbutton_PLAIN:
	case sbutton_PLAINBOX:
	    view_SetTransferMode(self, graphic_INVERT);
	    view_FillRect(self, r, view_BlackPattern(self));
	    break;

	case sbutton_BOXEDRECT:
	    /* Rect2 is the inner rect */
	    Rect2.top = r->top + BUTTONDEPTH;
	    Rect2.left = r->left + BUTTONDEPTH;
	    Rect2.width = r->width - 2*BUTTONDEPTH;
	    Rect2.height = r->height - 2*BUTTONDEPTH;

	    view_SetTransferMode(self, graphic_INVERT);
	    view_FillRect(self, r, view_BlackPattern(self));
	    view_FillRect(self, &Rect2, view_BlackPattern(self));

	    break;

	case sbutton_MOTIF:
	case sbutton_THREEDEE:
	    if (style == sbutton_MOTIF) {
		bdepth = MOTIFBUTTONDEPTH;
	    } else {
		bdepth = BUTTONDEPTH;
	    }


	    /* Rect2 is the inner (Text) region */
	    Rect2.top = r->top + bdepth;
	    Rect2.left = r->left + bdepth;
	    Rect2.width = r->width - 2*bdepth;
	    Rect2.height = r->height - 2*bdepth;
	    r2_bot = (Rect2.top)+(Rect2.height);
	    r_bot = (r->top)+(r->height);

	    if (style == sbutton_MOTIF) {
		view_SetTransferMode(self, graphic_COPY);
		MySetShade(self, si->prefs,  fg, bg, sbutton_TOPSHADOW);
		view_FillRectSize(self, r->left, r->top, bdepth, r->height, NULL);	/* left bar */

		MySetShade(self, si->prefs,  fg, bg, sbutton_BOTTOMSHADOW);
		view_FillRectSize(self, r->left + r->width - bdepth, r->top, bdepth, r->height, NULL); /* right bar */
		view_FillTrapezoid(self, Rect2.left, r2_bot, Rect2.width, r->left, r_bot, r->width, NULL); /* lower trapz */

		MySetShade(self, si->prefs,  fg, bg, sbutton_TOPSHADOW);
		view_FillTrapezoid(self, r->left, r->top, r->width, Rect2.left, Rect2.top, Rect2.width, NULL); /* upper trapz */

		MySetShade(self, si->prefs,  fg, bg, sbutton_FOREGROUND);

	    } else {
		tx = r->left + ((r->width-1) / 2);
		ty = r->top + ((r->height-1) / 2);
		
		DrawLabel(self, text, FALSE, si->prefs, fg, bg, tx, ty, TEXTINMIDDLE);
	    } /* MOTIF? */

	    break;
    }
    sbuttonv_RestoreViewState(self, &vi);
    si->lit=FALSE;
}


int sbuttonv__WhichButton(self, x, y)
struct sbuttonv *self;
long x, y;
{
    int row, col, spill, button;
    int i;
    struct sbutton *b=sbuttonv_ButtonData(self);
    row=(y-self->vborder)/(self->maxheight);
    if(row<0 || row>=b->rows) return -1;
    spill=b->count-(row+1)*b->cols;
    if(spill<0) {
	col=(x-self->hborder)/self->specialwidth;
    } else col = (x - self->hborder)/self->maxwidth;
    if(col<0 ||col>=b->cols) return -1;
   
    button=b->count - row*b->cols - col -1;
   
    if(button>=0 && button<b->count) {
	if(x>=rectangle_Left(&self->info[button].rect) && x<rectangle_Right(&self->info[button].rect) && y>=rectangle_Top(&self->info[button].rect) && y<rectangle_Bottom(&self->info[button].rect)) return button;
	else for(i=0;i<b->count;i++) { if(x>=rectangle_Left(&self->info[i].rect) && x<rectangle_Right(&self->info[i].rect) && y>=rectangle_Top(&self->info[i].rect) && y<rectangle_Bottom(&self->info[i].rect)) return i;
	}
	return button;
    } else return -1;
}

boolean sbuttonv__Touch(self, ind, action)
struct sbuttonv *self;
int ind;
enum view_MouseAction action;
{
    struct cursor *wait_cursor;
    struct sbutton *b=sbuttonv_ButtonData(self);

    if(action!=view_LeftUp && action!=view_RightUp) return TRUE;
    if (ind>=0 && ind<b->count && BUTTONS(self)[ind].lit && (self->activebuttons&sbuttonv_LEFTBUTTON) || (action==view_RightUp && self->activebuttons&sbuttonv_RIGHTBUTTON)) {
	if (wait_cursor = cursor_Create(self)) {
	    cursor_SetStandard(wait_cursor, Cursor_Wait);
	    im_SetProcessCursor(wait_cursor);
	    sbutton_Actuate(sbuttonv_ButtonData(self), self->lastbutton);
	    if(self->dotriggers) {
		
		if(sbutton_GetTrigger(sbuttonv_ButtonData(self), ind)) {
		    sbuttonv_PullTrigger(self, sbutton_GetTrigger(sbuttonv_ButtonData(self), ind));
		} else {
		    sbuttonv_PullTrigger(self, buttonpushed);
		}
	    }
	    im_SetProcessCursor(NULL);
	    cursor_Destroy(wait_cursor);
	}
    }
    return TRUE;
}


struct view *sbuttonv__Hit(self, action, x, y, numclicks)
struct sbuttonv *self;
long x, y;
enum view_MouseAction action;
long numclicks;  
{
    /*
      Handle the button event.  Currently, semantics are:

      Left/Right Down  -- Draw button pressed
      Left/Right Up    -- draw button at rest, pull trigger
      Left/Right Movement     -- unhighlight if moved off, highlight if moved on
	  */
    struct sbutton *b=sbuttonv_ButtonData(self);
    struct rectangle r;
    sbuttonv_GetLogicalBounds(self, &r);

    if(!b->count) return((struct view *)self);

    if(!RectEnclosesXY(&r, x, y)) {
	if( self->lasthighlight>=0 && self->lasthighlight<b->count) {
	    
	    sbutton_DeActivateButton(b, self->lasthighlight);
	    self->lasthighlight=(-1);
	}
	return((struct view *)self);
    }
    self->lastbutton=sbuttonv_WhichButton(self, x, y);
    if(self->lastbutton<0) {
	if(self->lasthighlight>=0 && self->lasthighlight<b->count) { 
	    sbutton_DeActivateButton(b, self->lasthighlight);
	    
	    self->lasthighlight=(-1);
	}
	return((struct view *)self);
    }
    if(self->lastbutton>=self->bcount) {
	return ((struct view *)self);
    }
    if(sbuttonv_Touch(self, self->lastbutton, action)) {
	switch (action) {
	    case view_RightMovement:
	    case view_RightDown:
	    case view_LeftDown:
	    case view_LeftMovement: {

		if(self->lasthighlight!=self->lastbutton && self->lasthighlight>=0 && self->lasthighlight<b->count) { 
		    sbutton_DeActivateButton(b, self->lasthighlight);
		   
		    self->lasthighlight=(-1);
		}


		self->lasthighlight=self->lastbutton;

		if(!sbutton_GetLit(b, self->lastbutton)) sbutton_ActivateButton(b, self->lastbutton);
		}
		break;
	    case view_RightUp:
	    case view_LeftUp: {
		if(self->lasthighlight>=0 && self->lasthighlight<b->count) { 
		    sbutton_DeActivateButton(b, self->lasthighlight);
		    
		    self->lasthighlight=(-1);
		}
		}
		break;
	}
    }
    return((struct view *)self);
}


void sbuttonv__ObservedChanged(self, b, v)
struct sbuttonv *self;
struct sbutton *b;
long v;
{
    struct sbutton *b2=sbuttonv_ButtonData(self);
    long bchange=(v>=sbutton_CHANGEBASE && v<sbutton_CHANGEBASE+self->bcount)?v-sbutton_CHANGEBASE:-1;
    long change=sbutton_GetChangeType(b2);
    int i;
    
    if(b2!=b) return;

    self->needredraw=FALSE;
    
    if(self->bcount != b2->count) {
	self->needredraw=TRUE;
	sbuttonv_WantNewSize(self, self);
	super_ObservedChanged(self, b, observable_OBJECTCHANGED);
	return;
    }

    if(change&sbutton_ALLCHANGED) {
	for(i=0;i<self->bcount;i++) self->info[i].drawflag|=sbutton_LABELCHANGED;
    }
    
    if(change&(sbutton_LABELCHANGED | sbutton_FONTCHANGED | sbutton_SIZECHANGED | sbutton_STYLECHANGED)) sbuttonv_WantNewSize(self, self);

    if(change & sbutton_SIZECHANGED) self->needredraw=TRUE;
    
    if(bchange<0) {
	super_ObservedChanged(self, b, observable_OBJECTCHANGED);
	return;
    }
    
    if(change&sbutton_TRIGGERCHANGED) {
	if(sbutton_GetTrigger(b, bchange)) {
	    observable_DefineTrigger(self, sbutton_GetTrigger(b, bchange));
	    return;
	}
    }

    if(change&(sbutton_LABELCHANGED | sbutton_FONTCHANGED | sbutton_STYLECHANGED)) self->info[bchange].drawflag|=sbutton_LABELCHANGED;

    if(change&sbutton_ACTIVATIONCHANGED) {
	self->forceupdate=TRUE;
	self->info[bchange].drawflag|=sbutton_ACTIVATIONCHANGED;
    }
	
    super_ObservedChanged(self, b, observable_OBJECTCHANGED);
}


enum view_DSattributes sbuttonv__DesiredSize(self, width, height, pass, desired_width, desired_height)
struct sbuttonv *self;
long width;
long height;
enum view_DSpass pass;
long *desired_width;
long *desired_height;
{
    /* 
      Tell parent that this object  wants to be as big as the box around its
      text string.  For some reason IM allows resizing of this object. (BUG)
      */
    long maxheight=0;
    long maxwidth=0;
    struct fontdesc *my_fontdesc;
    struct FontSummary *my_FontSummary;
    struct graphic *my_graphic;
    struct sbutton *b = sbuttonv_ButtonData(self);
    int style;
    int count=b->count;
    if(!b) {
	*desired_width=256;
	*desired_height=256;
	return view_HeightFlexible|view_WidthFlexible;
    }
    my_graphic = (struct graphic *)sbuttonv_GetDrawable(self);
    
    while(--count>=0) {
	long lwidth, lheight=1;
	struct sbutton_prefs *prefs=sbutton_GetPrefs(b, count);
	style = DEFAULTSTYLE(sbutton_GetStyle(prefs));
	if (!(my_fontdesc = sbutton_GetFont(prefs))) {
	    my_fontdesc= fontdesc_Create(FONT, FONTTYPE, FONTSIZE);
	}
	if (my_fontdesc) {
	    char *label=sbutton_GetLabel(b, count) ? sbutton_GetLabel(b, count) : NO_MSG;
	    struct fontdesc_charInfo ci;
	    if(strlen(label)==1) {
		fontdesc_CharSummary(my_fontdesc, my_graphic, *label, &ci);
		lwidth=ci.width+4;
		lheight=ci.height+4;
	    } else {

		fontdesc_StringSize(my_fontdesc, my_graphic, label, &lwidth, &lheight);
		my_FontSummary =  fontdesc_FontSummary(my_fontdesc, my_graphic);
		if(my_FontSummary) lheight=my_FontSummary->maxHeight;
		
	    }
	}
	switch (style) {
	    case sbutton_PLAIN:
	    case sbutton_PLAINBOX:
		lwidth += 2*TEXTPAD + 2*EXTRATEXTPAD;
		if(lwidth>maxwidth) maxwidth=lwidth;
		if (my_FontSummary && lheight + 2*TEXTPAD > maxheight) maxheight = lheight + 2*TEXTPAD;
		break;
	    case sbutton_MOTIF:
		lwidth = lwidth + 2*TEXTPAD + 2*MOTIFBUTTONDEPTH;
		if(lwidth>maxwidth) maxwidth=lwidth;
		if (my_FontSummary) {
		    lheight = lheight + 2*TEXTPAD + 2*MOTIFBUTTONDEPTH;
		    if(lheight > maxheight) maxheight = lheight;
		}
		break;
	    case sbutton_BOXEDRECT:
	    case sbutton_THREEDEE:
		lwidth = lwidth + 2*TEXTPAD + 2*BUTTONDEPTH;
		if(lwidth>maxwidth) maxwidth=lwidth;
		if (my_FontSummary) {
		    lheight = lheight + 2*TEXTPAD + 2*BUTTONDEPTH;
		    if(lheight > maxheight) maxheight = lheight;
		}
		break;
	}
    }
   
    *desired_height=(maxheight+self->vspacing)*b->rows + 2*self->vborder - self->vspacing;
    *desired_width=(maxwidth+self->hspacing)*b->cols + 2*self->hborder - self->hspacing;
    return view_Fixed;
}

void sbuttonv__GetOrigin(self, width, height, originX, originY)
struct sbuttonv *self;
long width, height;
long *originX, *originY;
{
/*
  We want this object to sit in-line with text, not below the baseline.
  Simply, we could negate the height as the originX, but then our
  text would be too high.  So, instead, we use the height of
  our font above the baseline
*/

  struct FontSummary *my_FontSummary;
  struct fontdesc *my_fontdesc;
  struct graphic *my_graphic;
  struct sbutton *b = sbuttonv_ButtonData(self);
  int style;
  long maxheight, maxbelow=0;
  struct sbutton_prefs *prefs=b->count?sbutton_GetPrefs(b, b->count-1):NULL;

  /* This only really makes sense if there is only one button... should I check and do something different if there is more than one button?  this should at least still do the right thing for one button. */
  style = DEFAULTSTYLE(b->count ? sbutton_GetStyle(prefs) : sbutton_MOTIF);

  my_graphic = (struct graphic *)sbuttonv_GetDrawable(self);
  if (!(my_fontdesc = (prefs?sbutton_GetFont(prefs):NULL))) {
    my_fontdesc= fontdesc_Create(FONT, FONTTYPE, FONTSIZE);
  }
  if (my_fontdesc) {
      if(b->count && sbutton_GetLabel(b, b->count-1) && strlen(sbutton_GetLabel(b, b->count-1))==1) {
	  struct fontdesc_charInfo ci;
	  fontdesc_CharSummary(my_fontdesc, my_graphic, sbutton_GetLabel(b, b->count-1)[0], &ci);
	  maxheight=ci.height+4;
      } else {
	  my_FontSummary =  fontdesc_FontSummary(my_fontdesc, my_graphic);
	  maxheight=my_FontSummary->maxHeight;
	  maxbelow=my_FontSummary->maxBelow;
      }
  }

  *originX = 0;
  switch (style) {
    case sbutton_PLAIN:
    case sbutton_PLAINBOX:
      if (my_FontSummary)
	*originY = maxheight - maxbelow + 1 + sbuttonv_GetVBorder(self);
      break;
    case sbutton_MOTIF:
      if (my_FontSummary) 
	*originY = maxheight - maxbelow + 1 + TEXTPAD + MOTIFBUTTONDEPTH + sbuttonv_GetVBorder(self);
      break;
    case sbutton_BOXEDRECT:
    case sbutton_THREEDEE:
      if (my_FontSummary) 
	*originY = maxheight - maxbelow + 1 + TEXTPAD + BUTTONDEPTH + sbuttonv_GetVBorder(self);
      break;
  }
  return;
}

static boolean definetriggers(b, i, si, self)
struct sbutton *b;
int i;
struct sbutton_info *si;
struct sbuttonv *self;
{
    if(sbutton_GetTrigger(b, i)) {
	observable_DefineTrigger(self, sbutton_GetTrigger(b, i));
    }
    return FALSE;
}

void sbuttonv__SetDataObject(self, b)
struct sbuttonv *self;
struct sbutton *b;
{
     sbutton_Enumerate(b, definetriggers, self);
     super_SetDataObject(self, b);
}

void sbuttonv__WantUpdate(self, requestor)
struct sbuttonv *self;
struct view *requestor;
{
    if ((struct view *) self == requestor) {

	if(self->forceupdate && sbuttonv_GetIM(self)) {
	    self->awaitingUpdate = TRUE;
	    im_ForceUpdate();
	    self->forceupdate=FALSE;
	} else {
	    if (self->awaitingUpdate) return;
	    self->awaitingUpdate = TRUE;
	}
    }
    super_WantUpdate(self, requestor);
    /* if this is an activation change make it happen NOW! */
} /* sbuttonv__WantUpdate */

struct sbuttonv *sbuttonv__CreateFilledSButtonv(classID, defview, prefs, blist)
struct classheaded *classID;
char *defview;
struct sbuttonv *prefs;
struct sbutton_list *blist;
{
    struct sbutton *data=sbutton_CreateFilledSButton(prefs, blist);
    struct sbuttonv *self;
    
    if(!data) return NULL;

    if(defview) {
	if(!class_IsTypeByName(defview, "sbuttonv")) {
	    sbutton_Destroy(data);
	    return NULL;
	}
    } else defview=sbutton_ViewName(data);
    
    self=(struct sbuttonv *)class_NewObject(defview);
    if(!self) {
	sbutton_Destroy(data);
	return NULL;
    }
    sbuttonv_SetDataObject(self, data);
    return self;
}
