/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/sliderv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/sliderv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/sliderv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ ";
#endif /* lint */

#include <class.h>
#include <sliderv.eh>
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
static struct atomlist *  AL_bodyfont;
static struct atomlist *  AL_bodyfont_size;
static struct atomlist *  AL_label;
static struct atomlist *AL_max_value;
static struct atomlist *AL_min_value;

static struct atom *  A_long;
static struct atom *  A_string;

#define InternAtoms ( \
   AL_bodyfont = atomlist_StringToAtomlist("bodyfont") ,\
   AL_bodyfont_size = atomlist_StringToAtomlist("bodyfont-size") ,\
   AL_label = atomlist_StringToAtomlist("label") ,\
   AL_max_value = atomlist_StringToAtomlist("max_value") ,\
   AL_min_value = atomlist_StringToAtomlist("min_value") ,\
   A_long = atom_Intern("long") ,\
   A_string = atom_Intern("string") )

#define rectangle_TempSet(X,Y,W,H,R) ((R)->top = (Y), (R)->left = (X), \
				      (R)->height = (H), (R)->width = (W), (R))

#define Min(X,Y) ((X) < (Y) ? (X) : (Y))
#define FUDGE 2
#define FUDGE2 4


/****************************************************************/
/*		private functions				*/
/****************************************************************/

static void CarveFonts(self)
struct sliderV * self;
{
    self->normalfont = fontdesc_Create( self->fontname, fontdesc_Plain, self->fontsize );
    self->boldfont   = fontdesc_Create( self->fontname, fontdesc_Bold,  self->fontsize );
    self->valuefont = fontdesc_Create( "values", fontdesc_Plain, 25);
    self->activefont = self->header.valueview.mouseIsOnTarget ? self->boldfont : self->normalfont;
}  
#define MAXWID 36
#define ADDTOP 3
#define SUBHEIGHT 7
static void getsizes(self)
struct sliderV * self;
{
    struct FontSummary *fs;
    long labelwidth, labelheight, valheight,junk,wp,ww;
    fs = fontdesc_FontSummary(self->boldfont,sliderV_GetDrawable(self));
/*
    sprintf(buf,"%ld",self->maxval);
    fontdesc_StringSize(self->boldfont,sliderV_GetDrawable(self), buf,&(valwidth),&(junk));
    sprintf(buf,"%ld",self->minval);
    fontdesc_StringSize(self->boldfont,sliderV_GetDrawable(self), buf,&(valwidth1),&(junk));
    if(valwidth1 > valwidth) valwidth = valwidth1;
*/
    valheight = ( fs->newlineHeight == 0) ? fs->maxHeight : fs->newlineHeight;
    if(self->label){
	fontdesc_StringSize(self->boldfont,sliderV_GetDrawable(self), self->label,&(labelwidth),&(junk));
	labelheight = valheight;
    }
    else{
	labelheight = labelwidth = 0;
    }
    ww = self->width - FUDGE2 - FUDGE2 - self->x - self->x;
    wp = self->x + FUDGE2;
    if(ww > MAXWID){
	ww = MAXWID;
	wp = self->x + (self->width - MAXWID) / 2;
    }
    rectangle_SetRectSize(&self->wheelrec,wp,valheight + FUDGE + self->y + ADDTOP,ww, self->height - labelheight - valheight - FUDGE2 - self->y - self->y - SUBHEIGHT);
    rectangle_SetRectSize(&self->labelrec,(sliderV_Width(self) - labelwidth) / 2,self->height - labelheight,labelwidth + FUDGE2,labelheight);
    rectangle_SetRectSize(&self->valrec,self->x + 6, self->y + FUDGE + 4 ,self->width -FUDGE - 12 ,valheight);
    rectangle_SetRectSize(&self->inwheelrec,rectangle_Left(&self->wheelrec) + 1, rectangle_Top(&self->wheelrec) + 1,rectangle_Width(&self->wheelrec) - FUDGE2, rectangle_Height(&self->wheelrec) - FUDGE2 - FUDGE2);
}
db(self,foo,fo)
struct sliderV * self;
struct rectangle *foo,*fo;
{
    sliderV_DrawRect(self,foo);   
    sliderV_MoveTo( self, fo->left, fo->top );
    sliderV_DrawLineTo( self, foo->left, foo->top);
    sliderV_MoveTo( self, fo->left + fo->width, fo->top );
    sliderV_DrawLineTo( self, foo->left + foo->width, foo->top);
    sliderV_MoveTo( self, fo->left , fo->top + fo->height);
    sliderV_DrawLineTo( self, foo->left, foo->top + foo->height);
    sliderV_MoveTo( self, fo->left + fo->width, fo->top + fo->height);
    sliderV_DrawLineTo( self, foo->left + foo->width, foo->top + foo->height);

}
static void DrawButton(self,x,y,width,height,pct,drawborder)
     struct sliderV * self;
     long x,y,width,height;
     boolean drawborder;
{
  struct rectangle foo,fo;
  long vcut,wcut;
  if(!drawborder && pct > 0)sliderV_SetClippingRect( self, rectangle_TempSet(x,y,width + 2,height + 1,&fo)  ); 
  rectangle_TempSet(x,y,width,height,&fo);
 /* fo.width--; */  fo.height--;

 if(pct > 0){
  vcut = height /pct ;
  wcut =  width /pct;
  rectangle_TempSet(x + wcut ,y + 1,width - wcut - wcut,height - 3 - vcut - vcut - vcut,&foo);
 }
  else{
      vcut = pct; wcut = pct;
      rectangle_TempSet(x + wcut ,y  - 1,width - wcut - wcut,height - 5 - vcut - vcut - vcut,&foo);
  }
  if(pct <= 0)
      sliderV_EraseRect(self,&foo);
  if(drawborder) sliderV_DrawRect(self,&fo);   
  db(self,&foo,&fo);
}
static void DrawLabel(self)
struct sliderV * self;
{
    if(self->label){	
	sliderV_SetTransferMode( self, graphic_COPY);
	sliderV_EraseRect( self,&self->labelrec);

	sliderV_SetTransferMode( self, graphic_COPY );
	sliderV_MoveTo( self,self->x + ( self->width / 2),self->y + self->height);
	sliderV_SetFont( self, self->activefont );
	sliderV_DrawString ( self, self->label,
				   graphic_BETWEENLEFTANDRIGHT | graphic_ATBOTTOM);
    }

}
char *sliderV__GetValueString(self)
struct sliderV * self;
{
    sprintf(self->buf,"%ld",sliderV_GetTmpVal(self));
    return self->buf;
}
static void DrawValue(self)
struct sliderV * self;
{
    char *buf;   
    buf = sliderV_GetValueString(self);
    sliderV_SetTransferMode( self, graphic_COPY);
    sliderV_EraseRect( self,&self->valrec);

    sliderV_SetTransferMode( self, graphic_COPY );
    sliderV_MoveTo( self, self->x + (self->width / 2), self->y + FUDGE + 4);
    sliderV_SetFont( self, self->activefont );
    sliderV_DrawString ( self, buf,
				graphic_BETWEENLEFTANDRIGHT | graphic_ATTOP);

}
#define HGH 3
#define SPACE 6
static DrawKnurl(self)
struct sliderV * self;
{
    int start,height,w;
    sliderV_SetTransferMode( self, graphic_COPY );
    height =  ((self->tmpval - self->minval )* (rectangle_Height(&self->inwheelrec) - FUDGE2)) / (self->maxval - self->minval);
    start = rectangle_Top(&self->inwheelrec) + rectangle_Height(&self->inwheelrec) - height - FUDGE2;
    w = self->x + (self->width / 2);
/* Flashes too much    ??*/
    sliderV_FillRectSize(self,w - 2, rectangle_Top(&self->inwheelrec),
			  4,rectangle_Height(&self->inwheelrec),sliderV_GrayPattern(self,8,16)); 

    sliderV_DrawRectSize(self,w - 2, rectangle_Top(&self->inwheelrec),
			     4,rectangle_Height(&self->inwheelrec));
    sliderV_SetFont(self,self->valuefont);
    sliderV_MoveTo(self,w,start);
    sliderV_DrawText(self,"B",1, 0 );
    
}

static Drawslider(self,DoAll)
struct sliderV * self;
{
    sliderV_SetTransferMode( self, graphic_COPY );
#if 0
    if(DoAll){
	sliderV_EraseRect( self,&self->wheelrec);
	sliderV_SetTransferMode( self, graphic_COPY );
	/*	sliderV_DrawRect( self,&self->wheelrec); 
	 sliderV_MoveTo(self,rectangle_Left(&self->wheelrec),rectangle_Top(&self->wheelrec));
	 sliderV_DrawLine(self,0,rectangle_Height(&self->wheelrec));
	 */
    }
    else {
    }
#endif /* 0 */
    sliderV_EraseRect( self,&self->wheelrec);
    DrawKnurl(self);
}



/****************************************************************/
/*		class procedures 				*/
/****************************************************************/




boolean sliderV__InitializeClass(classID)
struct classheader *classID;
{
    InternAtoms;
    return TRUE;
}




#define BADVAL -22222
/****************************************************************/
/*		instance methods				*/
/****************************************************************/
boolean sliderV__InitializeObject(classID, self )
struct classheader *classID;
struct sliderV * self;
{
    self->label = NULL;
    self->fontname = NULL;
    self->fontsize = 0;
    self->maxval = 100;
    self->minval = 0;
    self->increment = 1;
    self->tmpval = BADVAL;
    self->lasty = 0;
    self->rv = 1000000;
    self->granular = self->gran =  0;
    return TRUE;
}


void sliderV__LookupParameters(self)
struct sliderV * self;
{
    char * fontname;
    long fontsize,diff;
    struct resourceList parameters[7];

    parameters[0].name = AL_label;
    parameters[0].type = A_string;
    parameters[1].name = AL_bodyfont;
    parameters[1].type = A_string;
    parameters[2].name = AL_bodyfont_size;
    parameters[2].type = A_long;
    parameters[3].name = AL_max_value;
    parameters[3].type = A_long;
    parameters[4].name = AL_min_value;
    parameters[4].type = A_long;
    parameters[5].name = NULL;
    parameters[5].type = NULL;

    sliderV_GetManyParameters(self, parameters, NULL, NULL);

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

    if(parameters[3].found)
	self->maxval = parameters[3].data;
    else
	self->maxval = 100;

    if(parameters[4].found)
	self->minval = parameters[4].data;
    else
	self->minval = 0;

    diff = self->maxval - self->minval;
    if(diff == 0) (self->maxval)++;
    if(diff < 20) self->granular = 6;
    else if(diff < 50) self->granular = 4;
    else if(diff < 100) self->granular = 2;

    else self->granular = 0;

    if (fontsize != self->fontsize || fontname != self->fontname)
    {
	self->fontsize = fontsize;
	self->fontname = fontname;
	CarveFonts(self);
    }
}


void sliderV__DrawFromScratch(self,x,y,width,height)
struct sliderV * self;
long x,y,width,height;
{
    self->x = x;
    self->y = y;
    self->width = width;
    self->height = height;
    if (width > 0 && height > 0)
    {
	struct value *w = sliderV_Value(self);
	getsizes(self);
	if(self->tmpval == BADVAL){
	    self->tmpval = value_GetValue(w);
	    if(self->tmpval < self->minval || 
	       self->tmpval > self->maxval){
		value_SetValue(w,self->minval);
		self->tmpval = self->minval;
	    }
	}
	self->activefont = self->header.valueview.mouseIsOnTarget ?
	  self->boldfont : self->normalfont;
	DrawButton(self,self->x + 5,self->y + 3,self->width - 10,self->wheelrec.top + self->wheelrec.height - 3,-3,TRUE);
	DrawValue(self);	
	Drawslider(self,TRUE);
	if (self->label != NULL)
	    DrawLabel(self);
    }
}


void sliderV__DrawDehighlight(self)
struct sliderV * self;
{
    struct value *w = sliderV_Value(self);
    self->activefont = self->normalfont;
    self->tmpval = value_GetValue(w);
    DrawLabel( self );
    DrawValue(self);	
    Drawslider(self,TRUE);
}

void sliderV__DrawHighlight(self)
struct sliderV * self;
{
    struct value *w = sliderV_Value(self);
    self->activefont = self->boldfont;
    DrawLabel( self );
    self->tmpval = value_GetValue(w);
    DrawValue(self);
    Drawslider(self,TRUE);
}


void sliderV__DrawNewValue( self )
struct sliderV * self;
{
    struct value *w = sliderV_Value(self);
    self->tmpval = value_GetValue(w);
    DrawValue(self);
    Drawslider(self,TRUE);
}



struct sliderV * sliderV__DoHit( self,type,x,y,hits )
struct sliderV * self;
enum view_MouseAction type;
long x,y,hits;
{
    struct value *tt = sliderV_Value(self);
    long myval;
    static int moved;
    switch(type){
	case view_LeftDown:
	case view_RightDown:
	    self->tmpval = value_GetValue(tt);
	    self->lasty = y;
	    moved = 0;
	    break;
	case view_LeftMovement	:
	case view_RightMovement:
	    moved++;
	    if(self->granular){
		myval = self->tmpval;
		self->gran += (self->lasty - y);
		while(self->gran > self->granular){
		    myval++;
		    self->gran -= self->granular;
		}
		while(self->gran < -self->granular){
		    myval--;
		    self->gran += self->granular;
		}
	    }
	    else 
	    myval = self->tmpval + (( self->lasty - y) /* * self->increment */ );
	    
	    if(myval > self->maxval)  myval = self->maxval;
	    else if(myval < self->minval) myval = self->minval;
	    if(myval != self->tmpval){
		self->tmpval = myval;
		DrawValue(self);
		Drawslider(self,FALSE);
	    }

	    self->lasty = y;
	    break;
	case view_LeftUp:
	case view_RightUp:
	    if(moved == 0){
		myval = (type == view_RightUp)? self->tmpval - self->increment : self->tmpval + self->increment ;
		if(myval <= self->maxval &&  myval >= self->minval){
		    self->tmpval = myval;
		    DrawValue(self);
		    self->lasty = y;
		    DrawValue(self);
		    Drawslider(self,FALSE);
		}
	    }
	    tt->string = sliderV_GetValueString(self);
	    value_SetValue(tt,self->tmpval);
	    break;
    }  

    return self;
}





