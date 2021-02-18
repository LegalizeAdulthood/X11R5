/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 * Bargraph.c - Bargraph widget for X11 Toolkit
 * 
 * Author:	Mike Yang (mikey@sgi.com)
 *		Silicon Graphics, Inc.
 * Date:	Mon Aug 9 1988, fix for R3 Wed Mar 8 1989
 *		  fix for R4 Fri Jul 13 1990
 *		  update to R5 Tue May 28 1991
 *
 * A slight modification of code taken from the X11R2 Scrollbar widget
 *
 * Copyright (c) 1988, 1991 Mike Yang
 *
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/XawInit.h>
#include "BargraphP.h"
#include <X11/Xmu/Drawing.h>

static float floatZero = 0.0;
static int DEFAULTVALUE = ~0;

#define Offset(field) XtOffset(BargraphWidget, field)

static XtResource resources[] = {
  {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
	     Offset(core.width), XtRInt, (XtPointer)&DEFAULTVALUE},
  {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
	     Offset(core.height), XtRInt, (XtPointer)&DEFAULTVALUE},
  {XtNlength, XtCLength, XtRInt, sizeof(int),
	     Offset(bargraph.length), XtRString, "1"},
  {XtNthickness, XtCThickness, XtRDimension, sizeof(Dimension),
	     Offset(bargraph.thickness), XtRString, "14"},
  {XtNorientation, XtCOrientation, XtROrientation, sizeof(XtOrientation),
	     Offset(bargraph.orientation), XtRString, "vertical"},
  {XtNthumb, XtCThumb, XtRPixmap, sizeof(Pixmap),
	     Offset(bargraph.thumb), XtRPixmap, NULL},
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	     Offset(bargraph.foreground), XtRString, "black"},
  {XtNshown, XtCShown, XtRFloat, sizeof(float),
	     Offset(bargraph.shown), XtRFloat, (XtPointer)&floatZero},
  {XtNtop, XtCTop, XtRFloat, sizeof(float),
	     Offset(bargraph.top), XtRFloat, (XtPointer)&floatZero},
  {XtNfillColor, XtCFillColor, XtRPixel, sizeof(Pixel),
     	     Offset(bargraph.fill_color), XtRString, "black"},
  {XtNfillPixmap, XtCFillPixmap, XtRPixmap, sizeof(Pixmap),
     	     Offset(bargraph.fill_pixmap), XtRPixmap, NULL},
};
#undef Offset

static void ClassInitialize();
static void Initialize();
static void Resize();
static void Redisplay();
static Boolean SetValues();

static void Notify(), Destroy();

BargraphClassRec bargraphClassRec = {
  { /* core fields */
    (WidgetClass) &simpleClassRec,	/* superclass		  */	
    "Bargraph",				/* class_name		  */
    sizeof(BargraphRec),			/* size			  */
    ClassInitialize,			/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    XtInheritRealize,			/* realize		  */
    NULL,				/* actions		  */
    0,					/* num_actions		  */
    resources,				/* resources		  */
    XtNumber(resources),		/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    TRUE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    Destroy,				/* destroy		  */
    Resize,				/* resize		  */
    Redisplay,				/* expose		  */
    SetValues,				/* set_values		  */
    NULL,				/* set_values_hook	  */
    XtInheritSetValuesAlmost,		/* set_values_almost	  */
    NULL,				/* get_values_hook	  */
    NULL,				/* accept_focus		  */
    XtVersion,				/* version		  */
    NULL,				/* callback_private	  */
    NULL,				/* tm_table		  */
    NULL,				/* query_geometry	  */
    NULL,				/* display_accelerator	  */
    NULL,				/* extension		  */
  },
  { /* simple fields */
    XtInheritChangeSensitive,		/* change_sensitive	  */
  },
  { /* bargraph fields */
    0,					/* ignore		  */
  },
};

static void Destroy(w)
     Widget w;
{

}

WidgetClass bargraphWidgetClass = (WidgetClass)&bargraphClassRec;

#define MINBARHEIGHT	0     /* How many pixels of bargraph to always show */
#define NoButton -1
#define MIN(x,y)	((x) < (y) ? (x) : (y))
#define MAX(x,y)	((x) > (y) ? (x) : (y))

static void ClassInitialize()
{
    XawInitializeWidgetSet();
    XtAddConverter( XtRString, XtROrientation, XmuCvtStringToOrientation,
		    NULL, (Cardinal)0 );
}

/*
 * Make sure the first number is within the range specified by the other
 * two numbers.
 */

static int InRange(num, small, big)
int num, small, big;
{
    return (num < small) ? small : ((num > big) ? big : num);
}


static FillArea(w, top, bottom, thumb)
  BargraphWidget w;
  Position top, bottom;
  int thumb;
{
    Dimension length = bottom-top;

    switch(thumb) {
	/* Fill the new Thumb location */
      case 1:
	if (w->bargraph.orientation == XtorientHorizontal) 
	    XFillRectangle(XtDisplay(w), XtWindow(w),
			   w->bargraph.gc, top, 1, length,
			   w->core.height-2);
	
	else XFillRectangle(XtDisplay(w), XtWindow(w), w->bargraph.gc,
			    1, top, w->core.width-2, length);

	break;
	/* Clear the old Thumb location */
      case 0:
	if (w->bargraph.orientation == XtorientHorizontal) 
	    XClearArea(XtDisplay(w), XtWindow(w), top, 1,
		       length, w->core.height-2, FALSE);
	
	else XClearArea(XtDisplay(w), XtWindow(w), 1,
			top, w->core.width-2, length, FALSE);
	break;
	/* Fill the area before the Thumb */
      case 2:
	if (w->bargraph.fill_pixmap == NULL) {
	  XSetForeground(XtDisplay(w), w->bargraph.gc, w->bargraph.fill_color);
	  XSetFillStyle(XtDisplay(w), w->bargraph.gc, FillSolid);
	} else {
	  XSetTile(XtDisplay(w), w->bargraph.gc, w->bargraph.fill_pixmap);
	}
	if (w->bargraph.orientation == XtorientHorizontal) 
	    XFillRectangle(XtDisplay(w), XtWindow(w),
			   w->bargraph.gc, top, 1, length,
			   w->core.height-2);
	
	else XFillRectangle(XtDisplay(w), XtWindow(w), w->bargraph.gc,
			    1, top, w->core.width-2, length);
	if (w->bargraph.fill_pixmap == NULL) {
	  XSetForeground(XtDisplay(w), w->bargraph.gc, w->bargraph.foreground);
	  XSetFillStyle(XtDisplay(w), w->bargraph.gc, FillTiled);
	} else {
	  XSetTile(XtDisplay(w), w->bargraph.gc, w->bargraph.thumb);
	}
	break;
    }  
}


/* Paint the thumb in the area specified by w->top and
   w->shown.  The old area is erased.  The painting and
   erasing is done cleverly so that no flickering will occur. */

static void PaintThumb( w )
  BargraphWidget w;
{
    int oldtop, oldbot, newtop, newbot;

    oldtop = w->bargraph.topLoc;
    oldbot = oldtop + w->bargraph.shownLength;
    newtop = w->bargraph.length * w->bargraph.top;
    newbot = newtop + (int)(w->bargraph.length * w->bargraph.shown);
    if (newbot < newtop + MINBARHEIGHT) newbot = newtop + MINBARHEIGHT;
    w->bargraph.topLoc = newtop;
    w->bargraph.shownLength = newbot - newtop;

    if (XtIsRealized((Widget)w)) {
	if (newtop < oldtop) FillArea(w, newtop, MIN(newbot, oldtop), 1);
	if (newtop > oldtop) FillArea(w, oldtop, newtop, 2);
	if (newbot < oldbot) FillArea(w, newbot, oldbot, 0);
	if (newbot > oldbot) FillArea(w, MAX(newtop, oldbot), newbot, 1);
    }
}


static void SetDimensions(w)
    BargraphWidget w;
{
    if (w->bargraph.orientation == XtorientVertical) {
	w->bargraph.length = w->core.height;
	w->bargraph.thickness = w->core.width;
    }
    else {
	w->bargraph.length = w->core.width;
	w->bargraph.thickness = w->core.height;
    }
}


/* ARGSUSED */
static void Initialize( request, new )
   Widget request;		/* what the client asked for */
   Widget new;			/* what we're going to give him */
{
    BargraphWidget w = (BargraphWidget) new;
    XGCValues gcValues;
    unsigned int depth = 1;

    if (w->bargraph.thumb == NULL) {
        w->bargraph.thumb = XmuCreateStippledPixmap( XtScreen(w),
						    w->bargraph.foreground,
						    w->core.background_pixel,
						    w->core.depth);
    }
    
    gcValues.foreground = w->bargraph.foreground;
    gcValues.fill_style = FillTiled;
    gcValues.tile = w->bargraph.thumb;
    w->bargraph.gc = XtGetGC( new, GCForeground | GCFillStyle | GCTile,
			     &gcValues);

    if (w->core.width == DEFAULTVALUE)
	w->core.width = (w->bargraph.orientation == XtorientVertical)
	    ? w->bargraph.thickness : w->bargraph.length;

    if (w->core.height == DEFAULTVALUE)
	w->core.height = (w->bargraph.orientation == XtorientHorizontal)
	    ? w->bargraph.thickness : w->bargraph.length;

    SetDimensions( w );
    w->bargraph.topLoc = 0;
    w->bargraph.shownLength = 0;
}


/* ARGSUSED */
static Boolean SetValues( current, request, desired )
   Widget current,		/* what I am */
          request,		/* what he wants me to be */
          desired;		/* what I will become */
{
    BargraphWidget w = (BargraphWidget) current;
    BargraphWidget rw = (BargraphWidget) request;
    BargraphWidget dw = (BargraphWidget) desired;
    Boolean redraw = FALSE;

    if (w->bargraph.foreground != rw->bargraph.foreground ||
	w->core.background_pixel != rw->core.background_pixel)
        redraw = TRUE;

    if (dw->bargraph.top < 0.0 || dw->bargraph.top > 1.0)
        dw->bargraph.top = w->bargraph.top;

    if (dw->bargraph.shown < 0.0 || dw->bargraph.shown > 1.0)
        dw->bargraph.shown = w->bargraph.shown;

    if (w->bargraph.top != dw->bargraph.top ||
        w->bargraph.shown != dw->bargraph.shown)
	redraw = TRUE;

    return( redraw );
}

/* ARGSUSED */
static void Resize( gw )
   Widget gw;
{
    /* ForgetGravity has taken care of background, but thumb may
     * have to move as a result of the new size. */
    SetDimensions( (BargraphWidget)gw );
    Redisplay( gw, (XEvent*)NULL, (Region)NULL );
}


/* ARGSUSED */
static void Redisplay( gw, event, region )
   Widget gw;
   XEvent *event;		/* unused, NULL if called from Resize() */
   Region region;		/* unused, NULL if called from Resize() */
{
    BargraphWidget w = (BargraphWidget) gw;

    w->bargraph.topLoc = -1000; /* Forces entire thumb to be painted. */
    PaintThumb( w ); 
}




/* Set the scroll bar to the given location. */
#if NeedFunctionPrototypes
void XtBargraphSetValue(Widget gw,
#if NeedWidePrototypes
			double top)
#else
			float top)
#endif
#else
void XtBargraphSetValue(Widget gw, float top)
Widget gw;
float top;
#endif
{
    BargraphWidget w = (BargraphWidget)gw;

/* if still thumbing */
/*
    if (w->bargraph.direction == 'c') return; 
*/

    w->bargraph.top = (top > 1.0) ? 1.0 :
		       (top >= 0.0) ? top :
			   w->bargraph.top;

/*
    w->bargraph.shown = (shown > 1.0) ? 1.0 :
			 (shown >= 0.0) ? shown :
			     w->bargraph.shown;
*/

    PaintThumb( w );
}

