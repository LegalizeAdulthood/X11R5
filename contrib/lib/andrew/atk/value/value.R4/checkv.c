/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/checkv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/checkv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/checkv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ ";
#endif /* lint */

#include <class.h>
#include <checkv.eh>
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
static struct atomlist *  AL_checktype;

static struct atom *  A_long;
static struct atom *  A_string;

#define InternAtoms ( \
   AL_bodyfont = atomlist_StringToAtomlist("bodyfont") ,\
   AL_bodyfont_size = atomlist_StringToAtomlist("bodyfont-size") ,\
   AL_label = atomlist_StringToAtomlist("label") ,\
   AL_checktype = atomlist_StringToAtomlist("checktype") ,\
   A_long = atom_Intern("long") ,\
   A_string = atom_Intern("string") )


#define Min(X,Y) ((X) < (Y) ? (X) : (Y))
#define FUDGE 2


/****************************************************************/
/*		private functions				*/
/****************************************************************/

static void CarveFonts(self)
struct checkv * self;
{
    self->normalfont = fontdesc_Create( self->fontname, fontdesc_Plain, self->fontsize );
}  

static Drawcheck(self)
struct checkv * self;
{
    register int side;
    register int gap;
    struct rectangle r;
    char ch;
    checkv_SetTransferMode( self, graphic_COPY );

    if (!self->BlackPattern)
	self->BlackPattern = checkv_BlackPattern(self);
    side = Min(self->width, self->height);
    gap  = FUDGE;
    side = side - (gap*2) + 1;
    checkv_EraseRectSize( self,self->x,self->y,self->width,self->height);
    checkv_SetTransferMode( self, graphic_BLACK );
    checkv_MoveTo(self, self->x + gap, self->y + gap);
 
    /* Box */
    checkv_DrawLineTo(self, self->x + gap, self->y + side);
    checkv_DrawLineTo(self, self->x + side, self->y + side);
    checkv_DrawLineTo(self, self->x + side, self->y + gap);
    checkv_DrawLineTo(self, self->x + gap, self->y + gap);

    /* Check */
    if (self->tmpval)
	switch(self->checktype) {
	    case 2: /* Solid */
		r.top    = self->y + gap*2;
		r.left   = self->x + gap*2;
		r.width  = side - gap*2 - 1;
		r.height = side - gap*2 - 1;
		checkv_SetTransferMode( self, graphic_COPY );
		checkv_FillRect(self, &r, self->BlackPattern);
		break;
	    default: /* Cross */
		checkv_DrawLineTo(self, self->x + side, self->y + side);
		checkv_MoveTo(self, self->x + gap, self->y +side);
		checkv_DrawLineTo(self, self->x + side, self->y + gap);
		break;
	}

    if(self->label){
	checkv_MoveTo(self, self->x + side + gap*2, self->y + (side+gap)/2);
	checkv_SetFont(self, self->normalfont);
	checkv_DrawString(self,self->label,graphic_ATLEFT | graphic_BETWEENTOPANDBASELINE);
    }
}



/****************************************************************/
/*		class procedures 				*/
/****************************************************************/




boolean checkv__InitializeClass(classID)
struct classheader *classID;
{
    InternAtoms;
    return TRUE;
}


#define BADVAL -22222
/****************************************************************/
/*		instance methods				*/
/****************************************************************/
boolean checkv__InitializeObject(classID, self )
struct classheader *classID;
struct checkv * self;
{
    self->label = NULL;
    self->fontname = NULL;
    self->fontsize = 0;
    self->tmpval = BADVAL;
    self->BlackPattern = NULL;
    return TRUE;
}

void checkv__LookupParameters(self)
struct checkv * self;
{
    char * fontname;
    long fontsize;
    struct resourceList parameters[5];

    parameters[0].name = AL_label;
    parameters[0].type = A_string;
    parameters[1].name = AL_bodyfont;
    parameters[1].type = A_string;
    parameters[2].name = AL_bodyfont_size;
    parameters[2].type = A_long;
    parameters[3].name = AL_checktype;
    parameters[3].type = A_long;
    parameters[4].name = NULL;
    parameters[4].type = NULL;

    checkv_GetManyParameters(self, parameters, NULL, NULL);

    if (parameters[0].found)
	self->label = (char *)parameters[0].data;
    else
	self->label = NULL ;

    if (parameters[1].found)
	fontname = (char *)parameters[1].data;
    else
	fontname = "andysansb";

    if (parameters[2].found)
	fontsize = parameters[2].data;
    else
	fontsize = 10;

    if (parameters[3].found)
	self->checktype = (short) parameters[3].data;
    else
	self->checktype = 1;

    if (fontsize != self->fontsize || fontname != self->fontname)
    {
	self->fontsize = fontsize;
	self->fontname = fontname;
	CarveFonts(self);
    }
}


void checkv__DrawFromScratch(self,x,y,width,height)
struct checkv * self;
long x,y,width,height;
{
    self->x = x;
    self->y = y;
    self->width = width;
    self->height = height;
    if (width > 0 && height > 0)
    {
	struct value *w = checkv_Value(self);
	if(self->tmpval == BADVAL){
	    self->tmpval = value_GetValue(w);
	}
	Drawcheck(self);	
    }
}


void checkv__DrawDehighlight(self)
struct checkv * self;
{

    struct value *w = checkv_Value(self);
    self->tmpval = value_GetValue(w);
    Drawcheck(self);	

}

void checkv__DrawHighlight(self)
struct checkv * self;
{
}


void checkv__DrawNewValue( self )
struct checkv * self;
{
    struct value *w = checkv_Value(self);
    self->tmpval = value_GetValue(w);
    Drawcheck(self);	
}



struct checkv * checkv__DoHit( self,type,x,y,hits )
struct checkv * self;
enum view_MouseAction type;
long x,y,hits;
{
    struct value *tt = checkv_Value(self);
    switch(type){
	case view_LeftDown:
	    self->tmpval = !value_GetValue(tt);
	    Drawcheck(self);
	    break;
	case view_LeftUp:
	    value_SetValue(tt,self->tmpval);
	    break;
    }  

    return self;
}





