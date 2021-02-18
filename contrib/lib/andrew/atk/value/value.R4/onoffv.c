/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/onoffv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/onoffv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/onoffv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ ";
#endif /* lint */

#include <class.h>
#include <onoffv.eh>
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
static struct atomlist *  AL_bottom_label;
static struct atomlist *  AL_top_label;

static struct atom *  A_long;
static struct atom *  A_string;

#define InternAtoms ( \
   AL_bodyfont = atomlist_StringToAtomlist("bodyfont") ,\
   AL_bodyfont_size = atomlist_StringToAtomlist("bodyfont-size") ,\
   AL_bottom_label = atomlist_StringToAtomlist("top label") ,\
   AL_top_label = atomlist_StringToAtomlist("bottom label") ,\
   A_long = atom_Intern("long") ,\
   A_string = atom_Intern("string") )


#define Min(X,Y) ((X) < (Y) ? (X) : (Y))
#define FUDGE 2
#define FUDGE2 4


/****************************************************************/
/*		private functions				*/
/****************************************************************/

static void CarveFonts(self)
struct onoffV * self;
{
    self->normalfont = fontdesc_Create( self->fontname, fontdesc_Plain, self->fontsize );
    self->boldfont   = fontdesc_Create( self->fontname, fontdesc_Bold,  self->fontsize );
    self->valuefont = fontdesc_Create( "values", fontdesc_Plain, 25);
    self->activefont = self->header.valueview.mouseIsOnTarget ? self->boldfont : self->normalfont;
}  

#define geticon(self) ((self->tmpval) ? 'a':'c')
static Drawonoff(self)
struct onoffV * self;
{
    char ch;
    onoffV_SetTransferMode( self, graphic_COPY );

    onoffV_EraseRectSize( self,self->x,self->y,self->width,self->height);
    onoffV_SetTransferMode( self, graphic_BLACK );
    onoffV_MoveTo(self,(self->width - self->x)/ 2, (self->height - self->y)/ 2);
    onoffV_SetFont(self,self->valuefont);
    ch = geticon(self);
    onoffV_DrawText(self,&ch,1, 0 );
    if(self->toplabel){
	onoffV_MoveTo(self,(self->width - self->x)/ 2, self->y + 1);
	onoffV_SetFont(self,(self->tmpval)? self->boldfont : self->normalfont);
	onoffV_DrawString(self,self->toplabel,graphic_BETWEENLEFTANDRIGHT | graphic_ATTOP);
    }
    if(self->bottomlabel){
	onoffV_MoveTo(self,(self->width - self->x)/ 2, self->y + self->height - 1);
	onoffV_SetFont(self,(self->tmpval)? self->normalfont : self->boldfont);
	onoffV_DrawString(self,self->bottomlabel,graphic_BETWEENLEFTANDRIGHT | graphic_ATBOTTOM);
    }
}



/****************************************************************/
/*		class procedures 				*/
/****************************************************************/




boolean onoffV__InitializeClass(classID)
struct classheader *classID;
{
    InternAtoms;
    return TRUE;
}




#define BADVAL -22222
/****************************************************************/
/*		instance methods				*/
/****************************************************************/
boolean onoffV__InitializeObject(classID, self )
struct classheader *classID;
struct onoffV * self;
{
    self->toplabel = NULL;
    self->bottomlabel = NULL;
    self->fontname = NULL;
    self->fontsize = 0;
    self->tmpval = BADVAL;
    return TRUE;
}


void onoffV__LookupParameters(self)
struct onoffV * self;
{
    char * fontname;
    long fontsize;
    struct resourceList parameters[7];

    parameters[0].name = AL_top_label;
    parameters[0].type = A_string;
    parameters[1].name = AL_bottom_label;
    parameters[1].type = A_string;
    parameters[2].name = AL_bodyfont;
    parameters[2].type = A_string;
    parameters[3].name = AL_bodyfont_size;
    parameters[3].type = A_long;
    parameters[4].name = NULL;
    parameters[4].type = NULL;

    onoffV_GetManyParameters(self, parameters, NULL, NULL);

    if (parameters[0].found)
	self->bottomlabel = (char *)parameters[0].data;
    else
	self->bottomlabel = NULL ;

    if (parameters[1].found)
	self->toplabel = (char *)parameters[1].data;
    else
	self->toplabel = NULL;

    if (parameters[2].found)
	fontname = (char *)parameters[2].data;
    else
	fontname = "andytype";

    if (parameters[3].found)
	fontsize = parameters[3].data;
    else
	fontsize = 10;

    if (fontsize != self->fontsize || fontname != self->fontname)
    {
	self->fontsize = fontsize;
	self->fontname = fontname;
	CarveFonts(self);
    }
}


void onoffV__DrawFromScratch(self,x,y,width,height)
struct onoffV * self;
long x,y,width,height;
{
    self->x = x;
    self->y = y;
    self->width = width;
    self->height = height;
    if (width > 0 && height > 0)
    {
	struct value *w = onoffV_Value(self);
	if(self->tmpval == BADVAL){
	    self->tmpval = value_GetValue(w);
	}
	self->activefont = self->header.valueview.mouseIsOnTarget ?
	  self->boldfont : self->normalfont;
	Drawonoff(self);	
    }
}


void onoffV__DrawDehighlight(self)
struct onoffV * self;
{

    struct value *w = onoffV_Value(self);
    self->activefont = self->normalfont;
    self->tmpval = value_GetValue(w);
    Drawonoff(self);	

}

void onoffV__DrawHighlight(self)
struct onoffV * self;
{
/*
    struct value *w = onoffV_Value(self);
    self->activefont = self->boldfont;
    self->tmpval = value_GetValue(w);
    Drawonoff(self);	
*/
}


void onoffV__DrawNewValue( self )
struct onoffV * self;
{
    struct value *w = onoffV_Value(self);
    self->tmpval = value_GetValue(w);
    Drawonoff(self);	
}



struct onoffV * onoffV__DoHit( self,type,x,y,hits )
struct onoffV * self;
enum view_MouseAction type;
long x,y,hits;
{
    struct value *tt = onoffV_Value(self);
    switch(type){
	case view_LeftDown:
	    self->tmpval = !value_GetValue(tt);
	    Drawonoff(self);
	    break;
	case view_LeftUp:
	    value_SetValue(tt,self->tmpval);
	    break;
    }  

    return self;
}





