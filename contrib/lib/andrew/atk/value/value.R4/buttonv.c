/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/buttonv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/buttonv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/buttonv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ ";
#endif /* lint */

#include <class.h>
#include <buttonv.eh>
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
static struct atom *  A_long;
static struct atom *  A_string;
#define InternAtoms ( \
   AL_bodyfont = atomlist_StringToAtomlist("bodyfont") ,\
   AL_bodyfont_size = atomlist_StringToAtomlist("bodyfont-size") ,\
   AL_label = atomlist_StringToAtomlist("label") ,\
   A_long = atom_Intern("long") ,\
   A_string = atom_Intern("string") )

#define Min(X,Y) ((X) < (Y) ? (X) : (Y))
#define Graphic(A) (((struct view *)(A))->drawable)
#define CLOSEBUTTON TRUE
/****************************************************************/
/*		private functions				*/
/****************************************************************/

static void CarveFonts(self)
     struct buttonV * self;
{
  self->normalfont = fontdesc_Create( self->fontname, fontdesc_Plain, self->fontsize );
  self->boldfont   = fontdesc_Create( self->fontname, fontdesc_Bold,  self->fontsize );
  self->activefont = self->header.valueview.mouseIsOnTarget ? self->boldfont : self->normalfont;
}  

#define rectangle_TempSet(X,Y,W,H,R) ((R)->top = (Y), (R)->left = (X), \
				      (R)->height = (H), (R)->width = (W), (R))
db(self,foo,fo)
struct buttonV * self;
struct rectangle *foo,*fo;
{
#if 0
    if(foo->top > fo->top){
	buttonV_FillTrapezoid(self,foo->left,foo->top,foo->width,fo->left,fo->top,fo->width,buttonV_GrayPattern(self,4,16));
	buttonV_FillTrapezoid(self,fo->left,fo->top + fo->height,fo->width,foo->left,foo->top + foo->height,foo->width,buttonV_GrayPattern(self,4,16));
    }
    else {
	buttonV_FillTrapezoid(self,fo->left,fo->top,fo->width,foo->left,foo->top,foo->width,buttonV_GrayPattern(self,4,16));
	buttonV_FillTrapezoid(self,foo->left,foo->top + foo->height,foo->width,fo->left,fo->top + fo->height,fo->width,buttonV_GrayPattern(self,4,16));
    }
#endif /* 0 */
    buttonV_DrawRect(self,foo);   
    buttonV_MoveTo( self, fo->left, fo->top );
    buttonV_DrawLineTo( self, foo->left, foo->top);
    buttonV_MoveTo( self, fo->left + fo->width, fo->top );
    buttonV_DrawLineTo( self, foo->left + foo->width, foo->top);
    buttonV_MoveTo( self, fo->left , fo->top + fo->height);
    buttonV_DrawLineTo( self, foo->left, foo->top + foo->height);
    buttonV_MoveTo( self, fo->left + fo->width, fo->top + fo->height);
    buttonV_DrawLineTo( self, foo->left + foo->width, foo->top + foo->height);

}
#define DEFAULTPCT 10
#if 0
struct fontdesc_charInfo {
    short width;			/* Width of bounding box */
    short height;			/* Height of bounding box */
    short xOriginOffset;	        /* X offset from left of bb */
    short yOriginOffset;		/* Y offset from top of bb */
    short xSpacing;		/* hor dist from this origin to next*/
    short ySpacing;		/* vert dist from this origin to next */
};
    CharSummary(struct graphic *gr, char LookUpChar, struct fontdesc_charInfo * returendInfo);
#endif /* 0 */
static void DrawButton(self,x,y,width,height,pct,drawborder)
     struct buttonV * self;
     long x,y,width,height;
     boolean drawborder;
{
  struct rectangle foo,fo;
  long vcut,wcut;
  if(!drawborder && pct > 0)buttonV_SetClippingRect( self, rectangle_TempSet(x,y,width + 2,height + 1,&fo)  ); 
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
      buttonV_EraseRect(self,&foo);
  if(drawborder) buttonV_DrawRect(self,&fo);   
  db(self,&foo,&fo);
}
#define MINSIZE 3

static boolean DrawLabel(self,x,y,width,height,full)
struct buttonV * self;
long x,y,width,height;
boolean full;
{
  struct rectangle fo;
  long TextWidth,TextHeight;
  boolean isicon = FALSE;
  TextHeight = 0;
  TextWidth = 0 ; 
  if(self->label){
      struct FontSummary *fs;
      if(self->label[1] == '\0') isicon = TRUE;
      else {
	  fontdesc_StringSize(self->activefont,Graphic(self), self->label, &TextWidth, &TextHeight) ;
	  fs = fontdesc_FontSummary(self->activefont,Graphic(self));
	  TextHeight = fs->maxHeight;
      }
  }
  buttonV_SetClippingRect( self, rectangle_TempSet(x ,y,width,height, &fo) );
  buttonV_SetTransferMode( self, graphic_COPY );
  buttonV_FillRectSize(self,x,y,width ,height ,buttonV_GrayPattern(self,4,16));
  buttonV_SetFont( self, self->activefont );
/*  buttonV_SetTransferMode( self, graphic_BLACK ); */
  if(isicon){
	  struct  fontdesc_charInfo ci;
	  fontdesc_CharSummary(self->activefont,Graphic(self),*(self->label),&ci);
	  TextWidth = ci.width + 4;
	  TextHeight = ci.height + 4;
	  DrawButton(self,(self->header.valueview.width >> 1) - (TextWidth / 2) - 2,(self->header.valueview.height >> 1) - (TextHeight / 2) - 4 ,TextWidth + 4,TextHeight + 5,-4,TRUE);
	  buttonV_MoveTo( self, (self->header.valueview.width >> 1) - (TextWidth / 2) + ci.xOriginOffset + 2,(self->header.valueview.height >> 1) - (TextHeight / 2) + ci.yOriginOffset + 2);
	  buttonV_DrawText(self,self->label,1, 0 );
      }
  else {
      long left,top;
      left = (self->header.valueview.x + ((self->header.valueview.width - TextWidth )>> 1)) - 3;
      top = (self->header.valueview.height >> 1) - (TextHeight / 2) - 4;
      DrawButton(self,left,top ,TextWidth + 7,TextHeight + 5,-4,TRUE);
#if 0 
     buttonV_MoveTo( self, left + 3,
			 self->header.valueview.y +(self->header.valueview.height >> 1) - 2);
     
      buttonV_DrawString ( self, self->label,
				graphic_ATLEFT | graphic_BETWEENTOPANDBOTTOM );
#endif /* 0  */
      buttonV_MoveTo( self,self->header.valueview.x + ((self->header.valueview.width)>> 1),
		      self->header.valueview.y +(self->header.valueview.height >> 1) - 2);

      buttonV_DrawString ( self, self->label,
			   graphic_BETWEENLEFTANDRIGHT | graphic_BETWEENTOPANDBOTTOM );
  }
  return TRUE;
}



/*
static void DrawNewLabel(self)
     struct buttonV * self;
{
     DrawLabel(self,self->header.valueview.x,self->header.valueview.y,
	    self->header.valueview.width,self->header.valueview.height,FALSE);
}

*/

/****************************************************************/
/*		class procedures 				*/
/****************************************************************/




boolean buttonV__InitializeClass(classID)
struct classheader *classID;
{
  InternAtoms;
  return TRUE;
}





/****************************************************************/
/*		instance methods				*/
/****************************************************************/
boolean buttonV__InitializeObject(classID, self )
struct classheader *classID;
     struct buttonV * self;
{
  self->label = NULL;
  self->fontname = NULL;
  self->fontsize = 0;
  self->buttontype = buttonV_EITHER;
  return TRUE;
}


void buttonV__LookupParameters(self)
     struct buttonV * self;
{
  char * fontname;
  long fontsize;
  struct resourceList parameters[4];

  parameters[0].name = AL_label;
  parameters[0].type = A_string;
  parameters[1].name = AL_bodyfont;
  parameters[1].type = A_string;
  parameters[2].name = AL_bodyfont_size;
  parameters[2].type = A_long;
  parameters[3].name = NULL;
  parameters[3].type = NULL;

  buttonV_GetManyParameters(self, parameters, NULL, NULL);

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

/*  if (fontsize != self->fontsize || fontname != self->fontname)
    { */
      self->fontsize = fontsize;
      self->fontname = fontname;
      CarveFonts(self);
/*     } */
}


void buttonV__DrawFromScratch(self,x,y,width,height)
struct buttonV * self;
long x,y,width,height;
{
    boolean drewbutton = FALSE;
    if (width > 0 && height > 0)
    {
	self->activefont = self->header.valueview.mouseIsOnTarget ?
	  self->boldfont : self->normalfont;
	if (self->label != NULL && *(self->label) != '\0')
	    drewbutton = DrawLabel(self,x,y,width,height,TRUE);
	if(!drewbutton && (self->buttontype & buttonV_SURROUND))DrawButton(self,x,y,width,height,DEFAULTPCT,FALSE);
    }
}


void buttonV__DrawDehighlight(self)
struct buttonV * self;
{
    self->activefont = self->normalfont;
    buttonV_DrawNewValue( self );
    /*  DrawLabel( self, self->header.valueview.x, self->header.valueview.y,
		   self->header.valueview.width, self->header.valueview.height,FALSE ); */
}

void buttonV__DrawHighlight(self)
struct buttonV * self;
{

    self->activefont = self->boldfont;
    buttonV_DrawNewValue( self );
    /*  DrawLabel( self, self->header.valueview.x, self->header.valueview.y,
		   self->header.valueview.width, self->header.valueview.height,FALSE );*/

}


void buttonV__DrawNewValue( self )
     struct buttonV * self;
{
#if 0 /* what the hell ? */
  int x;
  for (x = 0; x < 1; ++x)
    {
      buttonV__DrawHighlight( self );
      buttonV__DrawDehighlight( self );
    }
  self->activefont = self->header.valueview.mouseIsOnTarget ?
    self->boldfont : self->normalfont;
#endif /* 0  */
  if (self->label != NULL && *(self->label) != '\0')  buttonV_DrawFromScratch(self,self->header.valueview.x,self->header.valueview.y,
		   self->header.valueview.width,self->header.valueview.height);
/*  buttonV_FlushGraphics( self ); */
}



struct buttonV * buttonV__DoHit( self,type,x,y,hits )
     struct buttonV * self;
     enum view_MouseAction type;
     long x,y,hits;
{
  if (type == view_RightUp  || type == view_LeftUp )
      value_SetValue(buttonV_Value(self),value_GetValue(buttonV_Value(self)) + 1);
  return self;
}





