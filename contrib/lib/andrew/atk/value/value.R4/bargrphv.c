/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/bargrphv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/bargrphv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/bargrphv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ ";
#endif /* lint */

#include <class.h>
#include <bargrphv.eh>
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


#define Min(X,Y) ((X) < (Y) ? (X) : (Y))
#define FUDGE 2
#define FUDGE2 4


/****************************************************************/
/*		private functions				*/
/****************************************************************/

static void CarveFonts(self)
struct bargraphV * self;
{
    self->normalfont = fontdesc_Create( self->fontname, fontdesc_Plain, self->fontsize );
    self->boldfont   = fontdesc_Create( self->fontname, fontdesc_Bold,  self->fontsize );
    self->valuefont = fontdesc_Create( "values", fontdesc_Plain, 25);
    self->activefont = self->header.valueview.mouseIsOnTarget ? self->boldfont : self->normalfont;
}  
#define MAXWID 36
static void getsizes(self)
struct bargraphV * self;
{
    struct FontSummary *fs;
    long labelwidth, labelheight, valheight,junk,wp,ww;
    fs = fontdesc_FontSummary(self->boldfont,bargraphV_GetDrawable(self));
/*
    sprintf(buf,"%ld",self->maxval);
    fontdesc_StringSize(self->boldfont,bargraphV_GetDrawable(self), buf,&(valwidth),&(junk));
    sprintf(buf,"%ld",self->minval);
    fontdesc_StringSize(self->boldfont,bargraphV_GetDrawable(self), buf,&(valwidth1),&(junk));
    if(valwidth1 > valwidth) valwidth = valwidth1;
*/
    valheight = ( fs->newlineHeight == 0) ? fs->maxHeight : fs->newlineHeight;
    if(self->label){
	fontdesc_StringSize(self->boldfont,bargraphV_GetDrawable(self), self->label,&(labelwidth),&(junk));
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
    rectangle_SetRectSize(&self->wheelrec,wp,valheight + FUDGE + self->y ,ww, self->height - labelheight - valheight - FUDGE2 - self->y - self->y );
    rectangle_SetRectSize(&self->labelrec,(bargraphV_Width(self) - labelwidth) / 2,self->height - labelheight,labelwidth + FUDGE2,labelheight);
    rectangle_SetRectSize(&self->valrec,self->x , self->y + FUDGE ,self->width ,valheight);
    rectangle_SetRectSize(&self->inwheelrec,rectangle_Left(&self->wheelrec) + 1, rectangle_Top(&self->wheelrec) + 1,rectangle_Width(&self->wheelrec) - FUDGE2, rectangle_Height(&self->wheelrec) - 2);
}


static void DrawLabel(self)
struct bargraphV * self;
{
    if(self->label){	
	bargraphV_SetTransferMode( self, graphic_COPY);
	bargraphV_EraseRect( self,&self->labelrec);

	bargraphV_SetTransferMode( self, graphic_BLACK );
	bargraphV_MoveTo( self,self->x + ( self->width / 2),self->y + self->height);
	bargraphV_SetFont( self, self->activefont );
	bargraphV_DrawString ( self, self->label,
				   graphic_BETWEENLEFTANDRIGHT | graphic_ATBOTTOM);
    }

}
char *bargraphV__GetValueString(self)
struct bargraphV * self;
{
    static char buf[64];
    sprintf(buf,"%ld",bargraphV_GetTmpVal(self));
    return buf;
}
static void DrawValue(self)
struct bargraphV * self;
{
    char *buf;   
    buf = bargraphV_GetValueString(self);
    bargraphV_SetTransferMode( self, graphic_COPY);
    bargraphV_EraseRect( self,&self->valrec);

    bargraphV_SetTransferMode( self, graphic_BLACK );
    bargraphV_MoveTo( self, self->x + self->width / 2, self->y + FUDGE);
    bargraphV_SetFont( self, self->activefont );
    bargraphV_DrawString ( self, buf,
				graphic_BETWEENLEFTANDRIGHT | graphic_ATTOP);

}
#define HGH 3
#define SPACE 6
static DrawKnurl(self)
struct bargraphV * self;
{
    int start,height;
    struct rectangle r;
    int offset = FUDGE2;
    height =  ((self->tmpval - self->minval)* (rectangle_Height(&self->inwheelrec) - FUDGE2)) /(self->maxval - self->minval) ;
    start = rectangle_Height(&self->inwheelrec) - height - FUDGE2;
    rectangle_SetRectSize(&r,rectangle_Left(&self->inwheelrec) + FUDGE,
			  rectangle_Top(&self->inwheelrec) +  start, 
			  rectangle_Width(&self->inwheelrec) - FUDGE2,height);
#if 0
    bargraphV_FillRect(self,&r, bargraphV_GrayPattern(self,12,16));
    bargraphV_FillTrapezoid(self,rectangle_Left(&self->inwheelrec) + FUDGE + FUDGE2,
			   rectangle_Top(&self->inwheelrec) + start - FUDGE2,
			   rectangle_Width(&self->inwheelrec) - FUDGE2,
			    rectangle_Left(&self->inwheelrec) + FUDGE,
			   rectangle_Top(&self->inwheelrec) +  start, 
			   rectangle_Width(&self->inwheelrec) - FUDGE2,
			  bargraphV_GrayPattern(self,8,16));
    bargraphV_FillTrapezoid(self,
			   rectangle_Left(&self->inwheelrec) +  rectangle_Width(&self->inwheelrec) - FUDGE,
			   
			   
#endif /* 0 */
    bargraphV_FillTrapezoid(self,r.left,r.top + r.height, r.width,
			 r.left	+ offset, r.top	+ r.height + offset, r.width,
			 bargraphV_GrayPattern(self,8,16));
    bargraphV_FillTrapezoid(self,r.left + r.width ,r.top , 0,
			 r.left + r.width,  r.top + offset,offset,
			 bargraphV_GrayPattern(self,12,16));
    bargraphV_FillTrapezoid(self,r.left + r.width ,r.top + r.height , offset,
			 r.left + r.width +offset , r.top + r.height + offset,0,
			 bargraphV_GrayPattern(self,12,16));
    bargraphV_FillRectSize(self,r.left + r.width ,r.top + offset,
			offset,r.height - offset,bargraphV_GrayPattern(self,12,16));
    bargraphV_MoveTo(self,r.left + r.width,r.top );
    bargraphV_DrawLine(self,offset,offset);bargraphV_DrawLine(self,0,r.height);
    bargraphV_DrawLine(self,-r.width,0);bargraphV_DrawLine(self,-offset,-offset);
    bargraphV_MoveTo(self,r.left + r.width,r.top + r.height);
    bargraphV_DrawLine(self,offset,offset);
    bargraphV_DrawRect(self, &r);
}

static Drawbar(self,DoAll)
struct bargraphV * self;
{
    bargraphV_SetTransferMode( self, graphic_COPY );
#if 0
    if(DoAll){
	bargraphV_EraseRect( self,&self->wheelrec);
	bargraphV_SetTransferMode( self, graphic_BLACK );
	/*	bargraphV_DrawRect( self,&self->wheelrec); 
	 bargraphV_MoveTo(self,rectangle_Left(&self->wheelrec),rectangle_Top(&self->wheelrec));
	 bargraphV_DrawLine(self,0,rectangle_Height(&self->wheelrec));
	 */
    }
    else {
    }
#endif /* 0 */
    bargraphV_EraseRect( self,&self->wheelrec);
    DrawKnurl(self);
}



/****************************************************************/
/*		class procedures 				*/
/****************************************************************/




boolean bargraphV__InitializeClass(classID)
struct classheader *classID;
{
    InternAtoms;
    return TRUE;
}




#define BADVAL -22222
/****************************************************************/
/*		instance methods				*/
/****************************************************************/
boolean bargraphV__InitializeObject(classID, self )
struct classheader *classID;
struct bargraphV * self;
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


void bargraphV__LookupParameters(self)
struct bargraphV * self;
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

    bargraphV_GetManyParameters(self, parameters, NULL, NULL);

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


void bargraphV__DrawFromScratch(self,x,y,width,height)
struct bargraphV * self;
long x,y,width,height;
{
    self->x = x;
    self->y = y;
    self->width = width;
    self->height = height;
    if (width > 0 && height > 0)
    {
	struct value *w = bargraphV_Value(self);
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
	DrawValue(self);	
	Drawbar(self,TRUE);
	if (self->label != NULL)
	    DrawLabel(self);
    }
}


void bargraphV__DrawDehighlight(self)
struct bargraphV * self;
{
    struct value *w = bargraphV_Value(self);
    self->activefont = self->normalfont;
    self->tmpval = value_GetValue(w);
    DrawLabel( self );
    DrawValue(self);	
}

void bargraphV__DrawHighlight(self)
struct bargraphV * self;
{
    struct value *w = bargraphV_Value(self);
    self->activefont = self->boldfont;
    DrawLabel( self );
    self->tmpval = value_GetValue(w);
    DrawValue(self);
}


void bargraphV__DrawNewValue( self )
struct bargraphV * self;
{
    struct value *w = bargraphV_Value(self);
    self->tmpval = value_GetValue(w);
    DrawValue(self);
    Drawbar(self,TRUE);
}

#if 0

struct bargraphV * bargraphV__DoHit( self,type,x,y,hits )
struct bargraphV * self;
enum view_MouseAction type;
long x,y,hits;
{
    struct value *tt = bargraphV_Value(self);
    long myval;
    switch(type){
	case view_LeftDown:
	    self->tmpval = value_GetValue(tt);
	    self->lasty = y;
	    break;
	case view_LeftMovement	:
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
	    else myval = self->tmpval + (( self->lasty - y) * self->increment);
	    if(myval != self->tmpval){
		if(myval > self->maxval) self->tmpval = self->minval;
		else if(myval < self->minval) self->tmpval = self->maxval;
		else self->tmpval = myval;
		DrawValue(self);
	    }
	    self->lasty = y;
	    Drawbar(self,FALSE);
	    break;
	case view_LeftUp:
	    value_SetValue(tt,self->tmpval);
	    break;
    }  

    return self;
}


#endif /* 0 */
