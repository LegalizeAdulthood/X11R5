/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/fourwayv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/fourwayv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/fourwayv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ ";
#endif /* lint */

#include <class.h>
#include <fourwayv.eh>
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
#define rectangle_TempSet(X,Y,W,H,R) ((R)->top = (Y), (R)->left = (X), \
				      (R)->height = (H), (R)->width = (W), (R))

#define InternAtoms ( \
   AL_bodyfont = atomlist_StringToAtomlist("bodyfont") ,\
   AL_bodyfont_size = atomlist_StringToAtomlist("bodyfont-size") ,\
   AL_label = atomlist_StringToAtomlist("':' separated labels") ,\
   A_long = atom_Intern("long") ,\
   A_string = atom_Intern("string") )


#define Min(X,Y) ((X) < (Y) ? (X) : (Y))
#define FUDGE 2
#define FUDGE2 4


/****************************************************************/
/*		private functions				*/
/****************************************************************/

static  void db(self,foo,fo)
struct fourwayV * self;
struct rectangle *foo,*fo;
{
    fourwayV_DrawRect(self,foo);   
    fourwayV_MoveTo( self, fo->left, fo->top );
    fourwayV_DrawLineTo( self, foo->left, foo->top);
    fourwayV_MoveTo( self, fo->left + fo->width, fo->top );
    fourwayV_DrawLineTo( self, foo->left + foo->width, foo->top);
    fourwayV_MoveTo( self, fo->left , fo->top + fo->height);
    fourwayV_DrawLineTo( self, foo->left, foo->top + foo->height);
    fourwayV_MoveTo( self, fo->left + fo->width, fo->top + fo->height);
    fourwayV_DrawLineTo( self, foo->left + foo->width, foo->top + foo->height);

}
static void DrawButton(self,x,y,width,height,pct,drawborder)
     struct fourwayV * self;
     long x,y,width,height;
     boolean drawborder;
{
  struct rectangle foo,fo;
  long vcut,wcut;
  if(!drawborder && pct > 0)fourwayV_SetClippingRect( self, rectangle_TempSet(x,y,width + 2,height + 1,&fo)  ); 
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
      fourwayV_EraseRect(self,&foo);
  if(drawborder) fourwayV_DrawRect(self,&fo);   
  db(self,&foo,&fo);
}
static void CarveFonts(self)
struct fourwayV * self;
{
    self->normalfont = fontdesc_Create( self->fontname, fontdesc_Plain, self->fontsize );
    self->boldfont   = fontdesc_Create( self->fontname, fontdesc_Bold,  self->fontsize );
    self->valuefont = fontdesc_Create( "values", fontdesc_Plain, 25);
    self->activefont = self->header.valueview.mouseIsOnTarget ? self->boldfont : self->normalfont;
}  
locateHit(self,x,y)
struct fourwayV * self;
int x,y;
{
    int hx, hy;
    hx = (self->width + self->x) / 2;
    hy = (self->height + self->y) / 2;
    if(x < hx){
	if(y < hy) return 0;
	return 3;
    }
    if(y < hy) return 1;
    return 2;

}

geticon(self)
struct fourwayV * self;
{
    switch(self->tmpval){
	case 0:
	    return 'n';
	case 1:
	    return 'h';
	case 2:
	    return 'j';
	case 3:
	    return 'l';
    }
}
parselabels(self,chr)
struct fourwayV * self;
char *chr;
{ 
    int i,j;
    for(i = 0 ; i < fourwayv_NUMLABELS; i++){
	self->label[i] = NULL;
    }
    if(chr == NULL) return;
    i = 0;
    do{
	j = 0;
	self->label[i] = chr;
	while(*chr != ':' && *chr != '\0'){
	    chr++;
	    j++;
	}
	self->lsize[i] = j;
	if(*chr == '\0') break;
	chr++;
    }while (++i < fourwayv_NUMLABELS);
}

static Drawnway(self)
struct fourwayV * self;
{
    char ch;
    fourwayV_SetTransferMode( self, graphic_COPY );

    fourwayV_EraseRectSize( self,self->x,self->y,self->width,self->height);
    fourwayV_SetTransferMode( self, graphic_COPY );
    DrawButton(self,(self->width - self->x)/ 2 - 20, (self->height - self->y)/ 2 - 20,40,40,-4,TRUE);
    fourwayV_MoveTo(self,(self->width - self->x)/ 2, (self->height - self->y)/ 2);
    fourwayV_SetFont(self,self->valuefont);
    ch = geticon(self);
    fourwayV_DrawText(self,&ch,1, 0 );
    if(self->label[0]){
	fourwayV_MoveTo(self,self->x, self->y);
	fourwayV_SetFont(self,(self->tmpval == 0)? self->boldfont : self->normalfont);
	fourwayV_DrawText(self,self->label[0],self->lsize[0],graphic_ATLEFT | graphic_ATTOP);
    }
    if(self->label[1]){
	fourwayV_MoveTo(self,self->width + self->x, self->y);
	fourwayV_SetFont(self,(self->tmpval == 1)? self->boldfont : self->normalfont );
	fourwayV_DrawText(self,self->label[1],self->lsize[1],graphic_ATRIGHT| graphic_ATTOP);
    }
    if(self->label[2]){
	fourwayV_MoveTo(self,self->x + self->width, self->y + self->height);
	fourwayV_SetFont(self,(self->tmpval == 2)? self->boldfont : self->normalfont);
	fourwayV_DrawText(self,self->label[2],self->lsize[2],graphic_ATRIGHT | graphic_ATBOTTOM);
    }
    if(self->label[3]){
	fourwayV_MoveTo(self, self->x, self->y + self->height);
	fourwayV_SetFont(self,(self->tmpval == 3)?  self->boldfont : self->normalfont );
	fourwayV_DrawText(self,self->label[3],self->lsize[3],graphic_ATLEFT| graphic_ATBOTTOM);
    }
}



/****************************************************************/
/*		class procedures 				*/
/****************************************************************/




boolean fourwayV__InitializeClass(classID)
struct classheader *classID;
{
    InternAtoms;
    return TRUE;
}




#define BADVAL -22222
/****************************************************************/
/*		instance methods				*/
/****************************************************************/
boolean fourwayV__InitializeObject(classID, self )
struct classheader *classID;
struct fourwayV * self;
{   
    int i;
    for(i = 0 ; i < fourwayv_NUMLABELS; i++)
	self->label[i] = NULL;
    self->fontname = NULL;
    self->fontsize = 0;
    self->tmpval = 0;
    return TRUE;
}


void fourwayV__LookupParameters(self)
struct fourwayV * self;
{
    char * fontname;
    long fontsize;
    struct resourceList parameters[7];

    parameters[0].name = AL_label;
    parameters[0].type = A_string;
    parameters[1].name = AL_bodyfont;
    parameters[1].type = A_string;
    parameters[2].name = AL_bodyfont_size;
    parameters[2].type = A_long;
    parameters[3].name = NULL;
    parameters[3].type = NULL;

    fourwayV_GetManyParameters(self, parameters, NULL, NULL);

    if (parameters[0].found)
	parselabels(self,(char *)parameters[0].data);
    else
	parselabels(self,NULL);

    if (parameters[1].found)
	fontname = (char *)parameters[1].data;
    else
	fontname = "andytype";

    if (parameters[2].found)
	fontsize = parameters[2].data;
    else
	fontsize = 10;

    if (fontsize != self->fontsize || fontname != self->fontname)
    {
	self->fontsize = fontsize;
	self->fontname = fontname;
	CarveFonts(self);
    }
}


void fourwayV__DrawFromScratch(self,x,y,width,height)
struct fourwayV * self;
long x,y,width,height;
{
    self->x = x;
    self->y = y;
    self->width = width;
    self->height = height;
    if (width > 0 && height > 0)
    {
	struct value *w = fourwayV_Value(self);
	if(self->tmpval == BADVAL){
	    self->tmpval = value_GetValue(w);
	}
	self->activefont = self->header.valueview.mouseIsOnTarget ?
	  self->boldfont : self->normalfont;
	Drawnway(self);	
    }
}


void fourwayV__DrawDehighlight(self)
struct fourwayV * self;
{

    struct value *w = fourwayV_Value(self);
    self->activefont = self->normalfont;
    self->tmpval = value_GetValue(w);
    Drawnway(self);	

}

void fourwayV__DrawHighlight(self)
struct fourwayV * self;
{
/*
    struct value *w = fourwayV_Value(self);
    self->activefont = self->boldfont;
    self->tmpval = value_GetValue(w);
    Drawnway(self);	
*/
}


void fourwayV__DrawNewValue( self )
struct fourwayV * self;
{
    struct value *w = fourwayV_Value(self);
    self->tmpval = value_GetValue(w);
    Drawnway(self);	
}



struct fourwayV * fourwayV__DoHit( self,type,x,y,hits )
struct fourwayV * self;
enum view_MouseAction type;
long x,y,hits;
{
    struct value *tt = fourwayV_Value(self);
    int tmp;
    switch(type){
	case view_LeftDown:
	    self->tmpval = locateHit(self,x,y);
	    Drawnway(self);
	    break;
	case view_LeftMovement:
	    tmp =  locateHit(self,x,y);
	    if(self->tmpval != tmp){
		self->tmpval = tmp;
		Drawnway(self);
	    }
	    break;
	case view_LeftUp:
	    value_SetValue(tt,self->tmpval);
	    break;
    }  

    return self;
}





