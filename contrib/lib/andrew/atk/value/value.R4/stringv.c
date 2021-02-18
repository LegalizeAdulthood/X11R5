/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/stringv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/stringv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/stringv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ ";
#endif /* lint */

#include <class.h>
#include <stringv.eh>
#include <fontdesc.ih>
#include <rect.h>
#include <buffer.ih>
#include <proctbl.ih>
#include <atom.ih>
#include <atomlist.ih>
#include <graphic.ih>
#include <rm.ih>
#include <view.ih>
#include <value.ih>
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
#define FUDGE 2
#define FUDGE2 4

/****************************************************************/
/*		private functions				*/
/****************************************************************/

static void CarveFonts(self)
struct stringV * self;
{
    self->normalfont = fontdesc_Create( self->fontname, fontdesc_Plain, self->fontsize );
    self->boldfont   = fontdesc_Create( self->fontname, fontdesc_Bold,  self->fontsize );
    self->activefont = self->header.valueview.mouseIsOnTarget ? self->boldfont : self->normalfont;
}  


static void DrawLabel(self)
struct stringV * self;
{
    if(self->label){	
	stringV_SetTransferMode( self, graphic_COPY);
	stringV_EraseRectSize( self,self->x,self->y, self->width,self->height);

	stringV_SetTransferMode( self, graphic_BLACK );
	stringV_MoveTo( self, self->width / 2 + self->x,self->height/2 + self->y);
	stringV_SetFont( self, self->activefont );
	stringV_DrawString ( self, self->label,
				   graphic_BETWEENLEFTANDRIGHT | graphic_BETWEENTOPANDBOTTOM);
    }

}

static char *GetString(self)
struct stringV * self;
{
    char *str;
    struct value *w = stringV_Value(self);
    long len,val;
    char **arr;
    str = value_GetString(w);
    if(str == NULL && 
	((len = value_GetArraySize(w)) != 0) && 
	((arr = value_GetStringArray(w)) != NULL) && 
	  ((val = value_GetValue(w))>= 0) && 
	  val < len)
	str = arr[val];
    return str;
}
/****************************************************************/
/*		class procedures 				*/
/****************************************************************/




boolean stringV__InitializeClass(classID)
struct classheader *classID;
{
    InternAtoms;
    return TRUE;
}




#define BADVAL -22222
/****************************************************************/
/*		instance methods				*/
/****************************************************************/
boolean stringV__InitializeObject(classID, self )
struct classheader *classID;
struct stringV * self;
{
    self->plabel = NULL;
    self->label = NULL;
    self->fontname = NULL;
    self->fontsize = 0;
    self->UseAlt = TRUE;
    return TRUE;
}


void stringV__LookupParameters(self)
struct stringV * self;
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

    stringV_GetManyParameters(self, parameters, NULL, NULL);

    if (parameters[0].found)
	self->plabel = (char *)parameters[0].data;
    else
	self->plabel = NULL;
    self->label = self->plabel;
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


void stringV__DrawFromScratch(self,x,y,width,height)
struct stringV * self;
long x,y,width,height;
{
    char *str;
    self->x = x; self->y = y;
    self->height = height; self->width = width;
    if (width > 0 && height > 0)
    {
	if(self->UseAlt) str = GetString(self);
	else str = NULL;
	self->activefont = self->header.valueview.mouseIsOnTarget ?
	  self->boldfont : self->normalfont;
	if(str != NULL && *str) self->label = str;
	else self->label = self->plabel;
	if (self->label != NULL)
	    DrawLabel(self);
    }
}


void stringV__DrawDehighlight(self)
struct stringV * self;
{
    self->activefont = self->normalfont;
    DrawLabel( self );
}

void stringV__DrawHighlight(self)
struct stringV * self;
{
    self->activefont = self->boldfont;
    DrawLabel( self );
}
 
void stringV__DrawNewValue( self )
struct stringV * self;
{
    char *str;
    if(self->UseAlt) str = GetString(self);
    else str = NULL;
    if(str != NULL && *str) self->label = str;
    else self->label = self->plabel;
    if (self->label != NULL)
	DrawLabel(self);
}
