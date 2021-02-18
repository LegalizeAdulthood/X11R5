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
 * Slide.c - Slider widget for X11 Toolkit
 * 
 * Author:	Mike Yang (mikey@sgi.com)
 *		Silicon Graphics, Inc.
 * Date:	Mon Aug 9 1988, fix for R3 Wed Mar 8 1989
 *		  fix for R4 Fri Jul 13 1990
 *		  update to R5 Tue May 28 1991
 *
 * A slight modification of code taken from the X11R2 Scrollbar widget
 *
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/XawInit.h>
#include "BargraphP.h"
#include "SlideP.h"
#include <X11/Xmu/Drawing.h>

/* Private definitions. */

static char defaultTranslations[] =
    "<Btn1Down>:   StartScroll(Forward) \n\
     <Btn2Down>:   StartScroll(Continuous) MoveThumb() NotifyThumb() \n\
     <Btn3Down>:   StartScroll(Backward) \n\
     <Btn2Motion>: MoveThumb() NotifyThumb() \n\
     <BtnUp>:      NotifyScroll(Proportional) EndScroll()";

#define Offset(field) XtOffset(SlideWidget, field)

static XtResource resources[] = {
  {XtNscrollProc, XtCCallback, XtRCallback, sizeof(caddr_t),
	     Offset(slide.scrollProc), XtRCallback, NULL},
  {XtNthumbProc, XtCCallback, XtRCallback, sizeof(caddr_t),
	     Offset(slide.thumbProc), XtRCallback, NULL},
  {XtNjumpProc, XtCCallback, XtRCallback, sizeof(XtPointer),
             Offset(slide.jumpProc), XtRCallback, NULL},
  {XtNscrollVCursor, XtCScrollVCursor, XtRCursor, sizeof(Cursor),
	     Offset(slide.verCursor), XtRString, "sb_v_double_arrow"},
  {XtNscrollHCursor, XtCScrollHCursor, XtRCursor, sizeof(Cursor),
	     Offset(slide.horCursor), XtRString, "sb_h_double_arrow"},
  {XtNscrollUCursor, XtCScrollUCursor, XtRCursor, sizeof(Cursor),
	     Offset(slide.upCursor), XtRString, "sb_up_arrow"},
  {XtNscrollDCursor, XtCScrollDCursor, XtRCursor, sizeof(Cursor),
	     Offset(slide.downCursor), XtRString, "sb_down_arrow"},
  {XtNscrollLCursor, XtCScrollLCursor, XtRCursor, sizeof(Cursor),
	     Offset(slide.leftCursor), XtRString, "sb_left_arrow"},
  {XtNscrollRCursor, XtCScrollRCursor, XtRCursor, sizeof(Cursor),
	     Offset(slide.rightCursor), XtRString, "sb_right_arrow"},
};
#undef Offset

static void Initialize();
static void Realize();

static void StartScroll();
static void MoveThumb();
static void NotifyThumb();
static void NotifyScroll();
static void EndScroll();

static XtActionsRec actions[] = {
	{"StartScroll",		StartScroll},
	{"MoveThumb",		MoveThumb},
	{"NotifyThumb",		NotifyThumb},
	{"NotifyScroll",	NotifyScroll},
	{"EndScroll",		EndScroll},
};


static void Notify(), Destroy();

SlideClassRec slideClassRec = {
  { /* core fields */
    (WidgetClass) &bargraphClassRec,	/* superclass		  */	
    "Slide",				/* class_name		  */
    sizeof(SlideRec),			/* size			  */
    NULL,				/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    Realize,				/* realize		  */
    actions,				/* actions		  */
    XtNumber(actions),			/* num_actions		  */
    resources,				/* resources		  */
    XtNumber(resources),		/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    TRUE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    Destroy,				/* destroy		  */
    XtInheritResize,			/* resize		  */
    XtInheritExpose,			/* expose		  */
    NULL,				/* set_values		  */
    NULL,				/* set_values_hook	  */
    XtInheritSetValuesAlmost,		/* set_values_almost	  */
    NULL,				/* get_values_hook	  */
    NULL,				/* accept_focus		  */
    XtVersion,				/* version		  */
    NULL,				/* callback_private	  */
    defaultTranslations,		/* tm_table		  */
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
  { /* slide fields */
    0,					/* ignore		  */
  },
};

static void Initialize( request, new )
   Widget request;		/* what the client asked for */
   Widget new;			/* what we're going to give him */
{

  SlideWidget cw = (SlideWidget) new;

  cw->slide.direction = 0;

}

static void Destroy(w)
     Widget w;
{

}

WidgetClass slideWidgetClass = (WidgetClass)&slideClassRec;

#define MINBARHEIGHT	0     /* How many pixels of slide to always show */
#define NoButton -1
#define PICKLENGTH(widget, x, y) \
    ((widget->bargraph.orientation == XtorientHorizontal) ? x : y)
#define MIN(x,y)	((x) < (y) ? (x) : (y))
#define MAX(x,y)	((x) > (y) ? (x) : (y))


/* Orientation enumeration constants */

static	XrmQuark  XtQEhorizontal;
static	XrmQuark  XtQEvertical;

/*ARGSUSED*/
#define	done(address, type) \
	{ (*toVal).size = sizeof(type); (*toVal).addr = (caddr_t) address; }

extern void LowerCase();



/*
 * Make sure the first number is within the range specified by the other
 * two numbers.
 */

static int InRange(num, small, big)
int num, small, big;
{
    return (num < small) ? small : ((num > big) ? big : num);
}

/*
 * Same as above, but for floating numbers. 
 */

static float FloatInRange(num, small, big)
float num, small, big;
{
    return (num < small) ? small : ((num > big) ? big : num);
}


/* Fill the area specified by top and bottom with the given pattern. */
static float FractionLoc(w, x, y)
  SlideWidget w;
  int x, y;
{
    float   result;

    result = PICKLENGTH(w, x / (float)w->core.width,
			y / (float)w->core.height);
    return FloatInRange(result, 0.0, 1.0);
}


static void Realize( gw, valueMask, attributes )
   Widget gw;
   Mask *valueMask;
   XSetWindowAttributes *attributes;
{
    SlideWidget w = (SlideWidget) gw;

    w->slide.inactiveCursor =
      (w->bargraph.orientation == XtorientVertical)
	? w->slide.verCursor
	: w->slide.horCursor;

    XtVaSetValues(gw, XtNcursor, w->slide.inactiveCursor, NULL);

    /* 
     * The Simple widget actually stuffs the value in the valuemask. 
     */
    
    (*slideWidgetClass->core_class.superclass->core_class.realize)
	(gw, valueMask, attributes);
}



/* ARGSUSED */
static void StartScroll( gw, event, params, num_params )
  Widget gw;
  XEvent *event;
  String *params;		/* direction: Back|Forward|Smooth */
  Cardinal *num_params;		/* we only support 1 */
{
    SlideWidget w = (SlideWidget) gw;
    Cursor cursor;
    char direction;

    if (w->slide.direction != 0) return; /* if we're already scrolling */

    if (*num_params > 0) direction = *params[0];
    else		 direction = 'C';

    w->slide.direction = direction;

    switch( direction ) {
	case 'B':
	case 'b':	cursor = (w->bargraph.orientation == XtorientVertical)
				   ? w->slide.downCursor
				   : w->slide.rightCursor; break;

	case 'F':
	case 'f':	cursor = (w->bargraph.orientation == XtorientVertical)
				   ? w->slide.upCursor
				   : w->slide.leftCursor; break;

	case 'C':
	case 'c':	cursor = (w->bargraph.orientation == XtorientVertical)
				   ? w->slide.rightCursor
				   : w->slide.upCursor; break;

	default:	return; /* invalid invocation */
    }

    XtVaSetValues(gw, XtNcursor, cursor, NULL);

    XFlush(XtDisplay(w));
}


static Boolean CompareEvents( oldEvent, newEvent )
    XEvent *oldEvent, *newEvent;
{
#define Check(field) if (newEvent->field != oldEvent->field) return False;

    Check(xany.display);
    Check(xany.type);
    Check(xany.window);

    switch( newEvent->type ) {
      case MotionNotify:
		Check(xmotion.state); break;
      case ButtonPress:
      case ButtonRelease:
		Check(xbutton.state);
		Check(xbutton.button); break;
      case KeyPress:
      case KeyRelease:
		Check(xkey.state);
		Check(xkey.keycode); break;
      case EnterNotify:
      case LeaveNotify:
		Check(xcrossing.mode);
		Check(xcrossing.detail);
		Check(xcrossing.state); break;
    }
#undef Check

    return True;
}

struct EventData {
	XEvent *oldEvent;
	int count;
};

static Bool PeekNotifyEvent( dpy, event, args )
    Display *dpy;
    XEvent *event;
    char *args;
{
    struct EventData *eventData = (struct EventData*)args;

    return ((++eventData->count == QLength(dpy)) /* since PeekIf blocks */
	    || CompareEvents(event, eventData->oldEvent));
}


static Boolean LookAhead( w, event )
    Widget w;
    XEvent *event;
{
    XEvent newEvent;
    struct EventData eventData;

    if (QLength(XtDisplay(w)) == 0) return False;

    eventData.count = 0;
    eventData.oldEvent = event;

    XPeekIfEvent(XtDisplay(w), &newEvent, PeekNotifyEvent, (char*)&eventData);

    if (CompareEvents(event, &newEvent))
	return True;
    else
	return False;
}


static void ExtractPosition( event, x, y )
    XEvent *event;
    Position *x, *y;			/* RETURN */
{
    switch( event->type ) {
      case MotionNotify:
		*x = event->xmotion.x;	 *y = event->xmotion.y;	  break;
      case ButtonPress:
      case ButtonRelease:
		*x = event->xbutton.x;   *y = event->xbutton.y;   break;
      case KeyPress:
      case KeyRelease:
		*x = event->xkey.x;      *y = event->xkey.y;	  break;
      case EnterNotify:
      case LeaveNotify:
		*x = event->xcrossing.x; *y = event->xcrossing.y; break;
      default:
		*x = 0; *y = 0;
    }
}

static void NotifyScroll( gw, event, params, num_params   )
   Widget gw;
   XEvent *event;
   String *params;		/* style: Proportional|FullLength */
   Cardinal *num_params;	/* we only support 1 */
{
    SlideWidget w = (SlideWidget) gw;
    int call_data;
    char style;
    Position x, y;

    if (w->slide.direction == 0) return; /* if no StartScroll */

    if (LookAhead(gw, event)) return;

    if (*num_params > 0) style = *params[0];
    else		 style = 'P';

    switch( style ) {
        case 'P':    /* Proportional */
        case 'p':    ExtractPosition( event, &x, &y );
		     call_data = InRange( PICKLENGTH( w, x, y ),
					  0,
					  (int) w->bargraph.length); break;

        case 'F':    /* FullLength */
        case 'f':    call_data = w->bargraph.length; break;
    }

    switch( w->slide.direction ) {
        case 'B':
        case 'b':    call_data = -call_data;
	  	     /* fall through */
        case 'F':
	case 'f':    XtCallCallbacks( gw, XtNscrollProc, (XtPointer)call_data );
	             break;

        case 'C':
	case 'c':    /* NotifyThumb has already called the thumbProc(s) */
		     break;
    }
}

/* ARGSUSED */
static void EndScroll(gw, event, params, num_params )
   Widget gw;
   XEvent *event;		/* unused */
   String *params;		/* unused */
   Cardinal *num_params;	/* unused */
{
    SlideWidget w = (SlideWidget) gw;

    XtVaSetValues(gw, XtNcursor, w->slide.inactiveCursor, NULL);
    XFlush(XtDisplay(w));	/* make sure it get propogated. */

    w->slide.direction = 0;
}


/* ARGSUSED */
static void MoveThumb( gw, event, params, num_params )
   Widget gw;
   XEvent *event;
   String *params;		/* unused */
   Cardinal *num_params;	/* unused */
{
    SlideWidget w = (SlideWidget) gw;
    Position x, y;

 /* if no StartScroll */
    if (w->slide.direction == 0) return;

    if (LookAhead(gw, event)) return;

    if (!event->xmotion.same_screen) return;

    ExtractPosition( event, &x, &y );
    XtBargraphSetValue( gw, FractionLoc(w, x, y));
    XFlush(XtDisplay(w));	/* re-draw it before Notifying */
}

/* ARGSUSED */
static void NotifyThumb( gw, event, params, num_params )
   Widget gw;
   XEvent *event;
   String *params;		/* unused */
   Cardinal *num_params;	/* unused */
{
    register SlideWidget w = (SlideWidget) gw;

    if (w->slide.direction == 0) return; /* if no StartScroll */

    if (LookAhead(gw, event)) return;

    /* thumbProc is not pretty, but is necessary for backwards
       compatibility on those architectures for which it work{s,ed};
       the intent is to pass a (truncated) float by value. */
    XtCallCallbacks( gw, XtNthumbProc, *(XtPointer*)&w->bargraph.top);
    XtCallCallbacks( gw, XtNjumpProc, (XtPointer)&w->bargraph.top);
}

