/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/mbuttonv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/mbuttonv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/mbuttonv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ ";
#endif /* lint */

#include <class.h>
#include <mbuttonv.eh>
#include <fontdesc.ih>
#include <rect.h>
#include <value.ih>
#include <buffer.ih>
#include <proctbl.ih>
#include <atom.ih>
#include <atomlist.ih>
#include <graphic.ih>
#include <rm.ih>
#include <view.ih>
#include <point.h>
#include <list.ih>
static struct atomlist *  AL_bodyfont;
static struct atomlist *  AL_bodyfont_size;
static struct atomlist *  AL_label;
static struct atomlist *  AL_placement;
static struct atom *  A_long;
static struct atom *  A_string;
#define InternAtoms ( \
AL_bodyfont = atomlist_StringToAtomlist("bodyfont") ,\
AL_bodyfont_size = atomlist_StringToAtomlist("bodyfont-size") ,\
AL_label = atomlist_StringToAtomlist("label") ,\
AL_placement = atomlist_StringToAtomlist("placement") ,\
A_long = atom_Intern("long") ,\
A_string = atom_Intern("string") )
struct mbuttonv_rl {
    struct rectangle rect;
    char *string;
    boolean pushed;
    long key,len;
    long width;
};
#define mbuttonv_SetUpperShade(SELF,P){\
if(P) mbuttonv_setShade(SELF,200);\
else mbuttonv_setShade(SELF,(self->mono ?25:45));\
}
#define mbuttonv_SetLowerShade(SELF,P){\
if(!P) mbuttonv_setShade(SELF,200);\
else mbuttonv_setShade(SELF,(self->mono ?25:45));\
} 
#define FORESHADE 30

#define Min(X,Y) ((X) < (Y) ? (X) : (Y))
#define Graphic(A) (((struct view *)(A))->drawable)
#define CLOSEBUTTON TRUE
/****************************************************************/
/*		private functions				*/
/****************************************************************

#define DEFAULTPCT 10

#define MINSIZE 3
/* Rect2 is the inner (Text) region */

#define PROMPTFONT "andysans12b"
#define FONT "andysans"
#define FONTTYPE fontdesc_Bold
#define FONTSIZE 12
#define BUTTONDEPTH 3
#define BUTTONPRESSDEPTH 2
#define TEXTPAD 2
#define mbuttonv_STRING_END ':'
#define mbuttonv_STRING_ESCAPE '\\'

#define FGCOLOR "white"
#define BGCOLOR "blue"
/* #define TEXTCOLOR "black" */

#define CFGCOLOR "white"
#define CBGCOLOR "blue"

#define BWFGCOLOR "black"
#define BWBGCOLOR "white"


static void mbuttonv_setShade(self, val)
struct mbuttonv *self;
int val;			/* 0 - 200*/
{
    double pct;
    if(val > 100){
/*	mbuttonv_SetFGColor(self,.95,.95,.95); */
	if (self->mono) pct = .75;
	else {
	    /*	mbuttonv_SetFGColor(self,.95,.95,.95); */
	    mbuttonv_SetForegroundColor(self,"Dark Gray",0,0,0);
	    return;
	}
    }
    else if(val == 100){
	mbuttonv_SetFGColor(self, 
			    self->foreground_color[0], 
			    self->foreground_color[1], 
			    self->foreground_color[2]);
	return ;
    }
    else if(val == 0){
	mbuttonv_SetFGColor(self, 
			    self->background_color[0], 
			    self->background_color[1], 
			    self->background_color[2]);
	return;
    }
			    
    else pct = (double)val/ 100.;
    mbuttonv_SetFGColor(self, 
			 self->foreground_color[0]* pct 
			 + self->background_color[0]*(1.0-pct), 
			 self->foreground_color[1]*pct 
			 + self->background_color[1]*(1.0-pct), 
			 self->foreground_color[2]*pct 
			 + self->background_color[2]*(1.0-pct));

} /* mbuttonv_setShade */


static boolean clearrl(rl,self)
struct mbuttonv_rl *rl;
struct mbuttonv *self;
{
    if(rl->len > 0) free(rl->string);
    free(rl);
    return TRUE;
}
static vsetrec(rl,self)
struct mbuttonv_rl *rl;
struct mbuttonv *self;
{
    self->rtl += -self->rhw;
    rectangle_SetRectSize(&(rl->rect),self->header.valueview.x,self->rtl,
		  self->header.valueview.width, self->rhw);
}
static wsetrec(rl,self)
struct mbuttonv_rl *rl;
struct mbuttonv *self;
{
    rectangle_SetRectSize(&(rl->rect),self->rtl,self->header.valueview.y,
		   self->rhw,self->header.valueview.height);
    self->rtl += self->rhw;
}
static calcRec(self)
struct mbuttonv * self;
{
    if(self->vertical){
	self->rtl = self->header.valueview.height;
	self->rhw = self->header.valueview.height / self->count;
	list_Enumerate(self->list,vsetrec,self);
    }
    else {
	self->rtl = 0;
	self->rhw = self->header.valueview.width / self->count;
	list_Enumerate(self->list,wsetrec,self);
    }
}

static mbuttonv_CacheSettings(self)
struct mbuttonv *self;
{
    char *fgcolor, *bgcolor,tmp[256],*t,*chr;
    unsigned char fg_rgb[3], bg_rgb[3];
    struct graphic *my_graphic;
    struct mbuttonv_rl *rl;
    long i,j;
    long max = 0l;
    self->activefont = fontdesc_Create( self->fontname, fontdesc_Plain, self->fontsize );
    my_graphic = (struct graphic *)mbuttonv_GetDrawable(self);

    fgcolor = FGCOLOR;
    bgcolor = BGCOLOR;
    self->FontSummary =  fontdesc_FontSummary(self->activefont, my_graphic);
    list_Enumerate(self->list,clearrl,self) ;
    list_Clear(self->list);
    i = 0;
    chr = self->label;
    self->mono = (mbuttonv_DisplayClass(self ) & graphic_Monochrome);
    do{
	j = 0;
	rl = (struct mbuttonv_rl *)
	  malloc(sizeof(struct mbuttonv_rl));
	rl->string = "";
	t = tmp;
	if(chr != NULL){
	    while(*chr != mbuttonv_STRING_END && *chr != '\0'){
	    if(*chr == mbuttonv_STRING_ESCAPE && *(chr + 1) == mbuttonv_STRING_END){
		    chr++;
		}
		*t++ = *chr++;
		j++;
	    }
	}
	rl->len = j;
	*t = '\0';
	if(j > 0){
	    rl->string = malloc(j + 1);
	    strcpy(rl->string,tmp);
	}
	list_InsertEnd(self->list,(char *)rl);
	rl->width = fontdesc_StringSize(self->activefont,my_graphic, tmp,NULL,NULL);
	if(rl->width > max) max = rl->width;
	rl->pushed = FALSE;
	rl->key = i;
	if(chr == NULL || *chr == '\0') break;
	chr++;
    }while (++i < 128);
    self->current = NULL;
    self->count = i + 1;
    self->max = max;
    if ( self->mono ){
	fgcolor = BWFGCOLOR;
	bgcolor = BWBGCOLOR;
    }
    else {  
	fgcolor = CFGCOLOR;
	bgcolor = CBGCOLOR;
    }

    if (mbuttonv_GetIM(self) != NULL) {
	/* these calls don't make sense if there is no IM,
	    (in fact they seg fault!) */

	mbuttonv_SetForegroundColor(self, fgcolor, fg_rgb[0]*256L, fg_rgb[1]*256L, fg_rgb[2]*256L);
	mbuttonv_SetBackgroundColor(self, bgcolor, bg_rgb[0]*256L, bg_rgb[1]*256L, bg_rgb[2]*256L);

	mbuttonv_GetFGColor(self, 
			    &(self->foreground_color[0]), 
			    &(self->foreground_color[1]), 
			    &(self->foreground_color[2]));
	mbuttonv_GetBGColor(self, 
			    &(self->background_color[0]), 
			    &(self->background_color[1]), 
			    &(self->background_color[2]));
    } 
}
static void drawRecButton(self,rect,text,len,pushed,borderonly,blit)
struct mbuttonv * self;
struct rectangle *rect;
char *text;
long len;
boolean pushed;
boolean borderonly;
boolean blit;
{

    struct rectangle Rect2;
    int r2_bot, r_bot;
    int tx = 0, ty = 0;
    short t_op;
    long offset;
    offset = 0;
    mbuttonv_SetFont(self, self->activefont); 

    mbuttonv_SetTransferMode(self, graphic_SOURCE);
    t_op = graphic_BETWEENLEFTANDRIGHT | graphic_BETWEENTOPANDBOTTOM;
    Rect2.top = rect->top + BUTTONDEPTH + offset;
    Rect2.left = rect->left + BUTTONDEPTH  + offset;
    Rect2.width = rect->width - 2*BUTTONDEPTH ;
    Rect2.height = rect->height - 2*BUTTONDEPTH ;
    r2_bot = (Rect2.top)+(Rect2.height);
    r_bot = (rect->top)+(rect->height);

    mbuttonv_SetTransferMode(self, graphic_COPY);
    if(!borderonly && (!self->mono || !blit)){
	/*	mbuttonv_setShade(self, ((self->mono) ? .0:((pushed)? .0: FORESHADE))); */
	if(self->mono){
	    mbuttonv_setShade(self, 0);
	}
	else mbuttonv_setShade(self, ((pushed)? 0: FORESHADE));
	mbuttonv_FillRect(self, &Rect2, NULL); /* the middle box */
	if(text && *text){
	    mbuttonv_setShade(self, 100);
	    mbuttonv_SetTransferMode(self, graphic_COPY); 
	    if(len == 1){
		/* special case drawing an icon */
		struct  fontdesc_charInfo ci;
		fontdesc_CharSummary(self->activefont,Graphic(self),*text,&ci);
		tx = ci.width + 4;
		ty = ci.height + 4;
		mbuttonv_MoveTo( self, rect->left + (rect->width >> 1) - (tx / 2) + ci.xOriginOffset + 2,rect->top + (rect->height >> 1) - (ty / 2) + ci.yOriginOffset + 2);
		mbuttonv_DrawText(self,text,len,0  );
	    }
	    else 
	    {
		tx = Rect2.left + (Rect2.width/ 2);
		ty = Rect2.top + ( Rect2.height/ 2);
		mbuttonv_MoveTo(self, tx, ty);
		mbuttonv_DrawText(self, text, len,t_op);
	    }
	    mbuttonv_SetTransferMode(self, graphic_COPY);
	}
    }
    if(self->mono && (blit || ((!blit) && pushed))){
	mbuttonv_SetTransferMode(self, graphic_INVERT);
	mbuttonv_FillRect(self, &Rect2, NULL); /* the middle box */
	mbuttonv_SetTransferMode(self, graphic_COPY);
    }
    mbuttonv_SetUpperShade(self,pushed) ;
    mbuttonv_FillRectSize(self, rect->left, rect->top, BUTTONDEPTH + offset, rect->height, NULL);	/* left bar */

    mbuttonv_SetLowerShade(self,pushed) ;
    mbuttonv_FillRectSize(self, rect->left + rect->width - BUTTONDEPTH + offset, rect->top, BUTTONDEPTH - offset, rect->height, NULL); /* right bar */
    mbuttonv_FillTrapezoid(self, Rect2.left, r2_bot, Rect2.width, rect->left, r_bot, rect->width, NULL); /* lower trapz */

    mbuttonv_SetUpperShade(self,pushed) ;

    mbuttonv_FillTrapezoid(self, rect->left, rect->top, rect->width, Rect2.left, Rect2.top, Rect2.width, NULL); /* upper trapz */
    mbuttonv_SetTransferMode(self, graphic_COPY);
    if(self->mono){
	mbuttonv_setShade(self, 100);
	mbuttonv_DrawRect(self,rect); 
	/*	mbuttonv_DrawRect(self,&Rect2); */
    }
#if 0
    else {
	mbuttonv_setShade(self, ((self->mono) ? 0:50));
	mbuttonv_FillRectSize(self, rect->left, rect->top, BUTTONDEPTH + offset, rect->height, NULL);	/* left bar */

	mbuttonv_FillRectSize(self, rect->left + rect->width - BUTTONDEPTH + offset, rect->top, BUTTONDEPTH - offset, rect->height, NULL); /* right bar */
	mbuttonv_FillTrapezoid(self, Rect2.left, r2_bot, Rect2.width, rect->left, r_bot, rect->width, NULL); /* lower trapz */

	mbuttonv_FillTrapezoid(self, rect->left, rect->top, rect->width, Rect2.left, Rect2.top, Rect2.width, NULL); /* upper trapz */
	if(self->mono) {
	    mbuttonv_setShade(self,100);
	    mbuttonv_DrawRect(self, rect);
	}
    }
    mbuttonv_SetTransferMode(self, graphic_BLACK);
#endif
}

static void DrawButton(self,rl,left,top,width,height,borderonly,blit)
struct mbuttonv * self;
struct mbuttonv_rl *rl;
long left,top,width,height;
boolean borderonly,blit;
{
    struct rectangle Rect,*rect;
    char *text ;
    boolean ped ;
    long textlen;

    if(rl == NULL) {
	rect = &Rect;
	rectangle_SetRectSize(rect,left,top,width,height);
	ped =  (self->pushed);
	text = "";
	textlen = 0l;
    }
    else{
	rect = &(rl->rect);
	ped = (rl->pushed);
	text = rl->string;
	textlen = rl->len;
    }
    drawRecButton(self,rect,text,textlen,ped,borderonly,blit);
}
static boolean enclosed(rl,x,y)
struct mbuttonv_rl *rl;
long x,y;
{
    if(x < rl->rect.left ||
	y < rl->rect.top ||
	x >= rl->rect.left + rl->rect.width ||
	y >= rl->rect.top + rl->rect.height)
	return FALSE;
    return TRUE;
}

static drl(rl,self)
struct mbuttonv_rl *rl;
struct mbuttonv *self;
{
    DrawButton(self,rl,0,0,0,0,FALSE,FALSE);
    return TRUE;
}
static findcurrent(rl,self)
struct mbuttonv_rl *rl;
struct mbuttonv *self;
{
    return !enclosed(rl,self->x,self->y);
}
static findkey(rl,i)
struct mbuttonv_rl *rl;
long i;
{
    return !(i == rl->key);
}
static DrawAllButtons(self)
struct mbuttonv * self;
{
    list_Enumerate(self->list,drl,self);
}

/****************************************************************/
/*		class procedures 				*/
/****************************************************************/




boolean mbuttonv__InitializeClass(classID)
struct classheader *classID;
{
  InternAtoms;
  return TRUE;
}





/****************************************************************/
/*		instance methods				*/
/****************************************************************/
boolean mbuttonv__InitializeObject(classID, self )
struct classheader *classID;
     struct mbuttonv * self;
{
  self->label = NULL;
  self->fontname = NULL;
  self->fontsize = 0;
  self->buttontype = mbuttonv_EITHER;
  self->pushed = FALSE;
  self->list = list_New();
  self->current = NULL;
  return TRUE;
     }


void mbuttonv__LookupParameters(self)
     struct mbuttonv * self;
{
  char * fontname;
  long fontsize;
  struct resourceList parameters[5];
  char *plc;
  parameters[0].name = AL_label;
  parameters[0].type = A_string;
  parameters[1].name = AL_bodyfont;
  parameters[1].type = A_string;
  parameters[2].name = AL_bodyfont_size;
  parameters[2].type = A_long;
  parameters[3].name = AL_placement;
  parameters[3].type = A_string;
  parameters[4].name = NULL;
  parameters[4].type = NULL;

  mbuttonv_GetManyParameters(self, parameters, NULL, NULL);

  if (parameters[0].found)
    self->label = (char *)parameters[0].data;
  else
    self->label = NULL;

  if (parameters[1].found)
    fontname = (char *)parameters[1].data;
  else
    fontname = "andytype";

  if (parameters[2].found)
    fontsize = parameters[2].data;
  else
      fontsize = 10;

  if (parameters[3].found)
      plc = (char *)parameters[3].data;
  else
      plc = NULL;

  if(plc != NULL && (*plc == 'h' || *plc == 'H')) self->vertical = FALSE;
  else self->vertical = TRUE;
  self->fontsize = fontsize;
  self->fontname = fontname;
  mbuttonv_CacheSettings(self);

}


void mbuttonv__DrawFromScratch(self,x,y,width,height)
struct mbuttonv * self;
long x,y,width,height;
{
    if (width > 0 && height > 0)
    {
	calcRec(self);
	DrawAllButtons(self);
	mbuttonv_DrawNewValue( self );
    }
}


void mbuttonv__DrawDehighlight(self)
struct mbuttonv * self;
{
}

void mbuttonv__DrawHighlight(self)
struct mbuttonv * self;
{

}


void mbuttonv__DrawNewValue( self )
     struct mbuttonv * self;
{
    long i= value_GetValue(mbuttonv_Value(self));
    struct mbuttonv_rl *rl = self->current;
    if(self->count > 1){
	if(rl == NULL ||  i  != rl->key){
	    if(rl){  
		rl->pushed = FALSE;
		DrawButton(self,rl,0,0,0,0,FALSE,TRUE);
	    }
	    rl = (struct mbuttonv_rl *) list_Enumerate(self->list,findkey,i);
	    if(rl){
		rl->pushed = TRUE;
		DrawButton(self,rl,0,0,0,0,FALSE,TRUE);
	    }
	    self->current = rl;
	}
    }
    self->valueset = TRUE;
}

struct mbuttonv * mbuttonv__DoHit( self,type,x,y,hits )
struct mbuttonv * self;
enum view_MouseAction type;
long x,y,hits;
{
    struct mbuttonv_rl *rl;
    long v;
    rl = self->current;
    if(self->count == 1){
	struct valueview *vself = (struct valueview *) self;
	switch(type){
	    case view_RightUp:
	    case view_LeftUp:
		if(rl){
		    rl->pushed = FALSE;
		    DrawButton(self,rl,0,0,0,0,FALSE,TRUE); 
		}
		value_SetValue(mbuttonv_Value(self),value_GetValue(mbuttonv_Value(self)) + 1);
		self->current = NULL;
		return self;
	    case view_LeftDown:
	    case view_RightDown:
		break;
	    case view_LeftMovement:
	    case view_RightMovement:
		if(rl && !vself->mouseIsOnTarget){
		    rl->pushed = FALSE;
		    DrawButton(self,rl,0,0,0,0,FALSE,TRUE); 
		    self->current = NULL;
		}
		return self;
	}
    }
    switch(type){
	case view_RightUp:
	case view_LeftUp:
	    if(rl) v = rl->key;
	    else v = -1;
	    if(value_GetValue(mbuttonv_Value(self)) != v)
		value_SetValue(mbuttonv_Value(self),v);
	    break;
	case view_LeftMovement:
	case view_RightMovement:
	case view_LeftDown:
	case view_RightDown:
	    if(rl == NULL || !enclosed(rl,x,y)){
		self->x = x;
		self->y = y;
		if(rl){  
		    rl->pushed = FALSE;
		    DrawButton(self,rl,0,0,0,0,FALSE,TRUE);
		}
		self->valueset = FALSE;
		rl = (struct mbuttonv_rl *) list_Enumerate(self->list,findcurrent,self);
		if(rl == NULL){
		    self->current = NULL;
		    mbuttonv_DrawNewValue( self );
		    break;
		}
		if(rl){
		    rl->pushed = TRUE;
		    DrawButton(self,rl,0,0,0,0,FALSE,TRUE);
		}
		self->current = rl;
	    }
	    break;
    } 
    return self;
}

struct view * mbuttonv__Hit(self, type, x, y, numberOfClicks)
     struct mbuttonv * self;
     enum view_MouseAction type;
     long x, y, numberOfClicks;
     {/* should probably just restore this functionality to valueview,
	with a way to optionly set it */
	 register short sendEvent;
	 struct valueview *vself = (struct valueview *) self;
	 if (vself->active)
	 {
	     switch (type)
	     {
		 case view_NoMouseEvent:
		     sendEvent = FALSE;
		     break;
		 case view_LeftDown:
		     vself->mouseState = valueview_LeftIsDown;
		     mbuttonv_Highlight(self);
		     sendEvent = TRUE;
		     self->movedoff = FALSE;
		     break;
		 case view_RightDown:
		     vself->mouseState = valueview_RightIsDown;
		     mbuttonv_Highlight(self);
		     sendEvent = TRUE;
		     self->movedoff = FALSE;
		     break;
		 case view_LeftUp:
		 case view_RightUp:
		     if(self->movedoff) sendEvent = FALSE;
		     else if (vself->mouseIsOnTarget)
		     {
			 mbuttonv_Dehighlight(self);
			 sendEvent = TRUE;
		     }
		     else sendEvent = TRUE;
		     break;
		 case view_LeftMovement:
		 case view_RightMovement:
		     if(self->movedoff) sendEvent = FALSE;
		     else  if(mbuttonv_OnTarget(self,x,y))
		     {
			 mbuttonv_Highlight(self);
			 sendEvent = TRUE;
		     }
		     else
		     {
			 mbuttonv_Dehighlight(self);
			 self->movedoff = TRUE;
			 if(self->valueset) sendEvent = FALSE;
			 else 
			     sendEvent = TRUE;

		     }
		     break;
	     }
	 }
	 else
	     sendEvent = FALSE;
	 if (sendEvent){
	     return (struct view *) mbuttonv_DoHit(self, type, x, y, numberOfClicks);
	 }
	 else
	     return (struct view *) self;
     }
