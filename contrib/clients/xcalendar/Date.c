

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

Portions Copyright (c) Dan Heller <island!argv@sun.com> 1988
         Copyright (c) Richard Bingle <bingle@cs.purdue.edu> 1990

******************************************************************/

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include <X11/Xaw/XawInit.h>
#include "DateP.h"

static char *months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
    "Aug", "Sep", "Oct", "Nov", "Dec"
};
static char *Months[] = {
    "January", "February", "March", "April", "May", "June", "July",
    "August", "September", "October", "November", "December"
};
static char *days[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
static char *Days[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday", "Sunday"
};

extern long time();
static void clock_tic();
static round();
	
/* Private Definitions */

#define PI			3.14159265358979
#define TWOPI			(2. * PI)

#define ANALOG_SIZE_DEFAULT	164

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define abs(a) ((a) < 0 ? -(a) : (a))


/* Initialization of defaults */

#define offset(field) XtOffset(DateWidget,date.field)
#define goffset(field) XtOffset(Widget,core.field)

static XtResource resources[] = {
    {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
	goffset(width), XtRImmediate, (caddr_t) 0},
    {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
	goffset(height), XtRImmediate, (caddr_t) 0},
    {XtNbackground, XtCBackground, XtRPixel, sizeof(Pixel),
	goffset(background_pixel), XtRString, "XtdefaultBackground"},
    {XtNupdate, XtCInterval, XtRInt, sizeof(int), 
        offset(update), XtRImmediate, (caddr_t) 60 },
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
        offset(fgpixel), XtRString, "XtdefaultForeground"},
    {XtNchime, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(chime), XtRImmediate, (caddr_t) FALSE },
    {XtNpadding, XtCMargin, XtRInt, sizeof(int),
        offset(padding), XtRImmediate, (caddr_t) 8},
    {XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
        offset(font), XtRString, "fixed"},
    {XtNreverseVideo, XtCReverseVideo, XtRBoolean, sizeof (Boolean),
	offset (reverse_video), XtRImmediate, (caddr_t) FALSE},
    {XtNbackingStore, XtCBackingStore, XtRBackingStore, sizeof (int),
    	offset (backing_store), XtRString, "default"},
    {XtNdate, XtCString, XtRString, sizeof (String),
	offset (date), XtRString, "%W, %M %d"},
};

#undef offset
#undef goffset

static void ClassInitialize();
static void Initialize(), Realize(), Destroy(), Resize(), Redisplay();
static Boolean SetValues();

DateClassRec dateClassRec = {
    { /* core fields */
    /* superclass		*/	&widgetClassRec,
    /* class_name		*/	"Date",
    /* widget_size		*/	sizeof(DateRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	resources,
    /* resource_count		*/	XtNumber(resources),
    /* xrm_class		*/	NULL,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	NULL,
    /* query_geometry           */	XtInheritQueryGeometry,
    /* display_accelerator      */	XtInheritDisplayAccelerator,
    /* extension                */	NULL
    }
};

WidgetClass dateWidgetClass = (WidgetClass) &dateClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void ClassInitialize()
{
    XawInitializeWidgetSet();
    XtAddConverter( XtRString, XtRBackingStore, XmuCvtStringToBackingStore,
		    NULL, 0 );
}

/* ARGSUSED */
static void Initialize (request, new)
    Widget request, new;
{
    DateWidget w = (DateWidget)new;
    XtGCMask		valuemask;
    XGCValues	myXGCV;
    int min_height, min_width;

    valuemask = GCForeground | GCBackground | GCFont | GCLineWidth;
    if (w->date.font != NULL)
        myXGCV.font = w->date.font->fid;
    else
        valuemask &= ~GCFont;	/* use server default font */

    {
       char *str;
       struct tm tm, *localtime();
       long time_value;
       (void) time(&time_value);
       tm = *localtime(&time_value);
       str = asctime(&tm);
       if (w->date.font == NULL)
          w->date.font = XQueryFont( XtDisplay(w),
				      XGContextFromGC(
					   DefaultGCOfScreen(XtScreen(w))) );
       min_width = XTextWidth(w->date.font, str, strlen(str)) +
	  2 * w->date.padding;
       min_height = w->date.font->ascent +
	  w->date.font->descent + 2 * w->date.padding;
    }
    if (w->core.width == 0)
	w->core.width = min_width;
    if (w->core.height == 0)
	w->core.height = min_height;

    myXGCV.foreground = w->date.fgpixel;
    myXGCV.background = w->core.background_pixel;
    if (w->date.font != NULL)
        myXGCV.font = w->date.font->fid;
    else
        valuemask &= ~GCFont;	/* use server default font */
    myXGCV.line_width = 0;
    w->date.myGC = XtGetGC((Widget)w, valuemask, &myXGCV);

    valuemask = GCForeground | GCLineWidth ;
    myXGCV.foreground = w->core.background_pixel;
    w->date.EraseGC = XtGetGC((Widget)w, valuemask, &myXGCV);

    if (w->date.update <= 0)
	w->date.update = 60;	/* make invalid update's use a default */
#ifdef BINGLE
    if (w->date.update < 60)
	w->date.update = 60;
#endif BINGLE
    w->date.numseg = 0;
    w->date.interval_id = 0;
}

static void Realize (gw, valueMask, attrs)
     Widget gw;
     XtValueMask *valueMask;
     XSetWindowAttributes *attrs;
{
     DateWidget	w = (DateWidget) gw;
#ifdef notdef
     *valueMask |= CWBitGravity;
     attrs->bit_gravity = ForgetGravity;
#endif
     switch (w->date.backing_store) {
     case Always:
     case NotUseful:
     case WhenMapped:
     	*valueMask |=CWBackingStore;
	attrs->backing_store = w->date.backing_store;
	break;
     }
     XtCreateWindow( gw, InputOutput, (Visual *)CopyFromParent,
		     *valueMask, attrs);
     Resize(gw);
}

static void Destroy (gw)
     Widget gw;
{
     DateWidget w = (DateWidget) gw;
     if (w->date.interval_id) XtRemoveTimeOut (w->date.interval_id);
     XtDestroyGC (w->date.myGC);
     XtDestroyGC (w->date.EraseGC);
}

static void Resize (gw) 
    Widget gw;
{
    DateWidget w = (DateWidget) gw;
    /* don't do this computation if window hasn't been realized yet. */
}

/* ARGSUSED */
static void Redisplay (gw, event, region)
    Widget gw;
    XEvent *event;		/* unused */
    Region region;		/* unused */
{
    DateWidget w = (DateWidget) gw;
    w->date.prev_time_string[0] = '\0';
    clock_tic((caddr_t)w, (XtIntervalId)0);
}

/* ARGSUSED */
static void clock_tic(client_data, id)
        caddr_t client_data;
        XtIntervalId *id;
{
        DateWidget w = (DateWidget)client_data;    
	struct tm *localtime();
	struct tm tm; 
	long	time_value;
	char	*time_ptr;
        register Display *dpy = XtDisplay(w);
        register Window win = XtWindow(w);

	if (id || !w->date.interval_id)
	    w->date.interval_id =
		XtAppAddTimeOut( XtWidgetToApplicationContext( (Widget) w),
				w->date.update*1000, clock_tic, (caddr_t)w );
	(void) time(&time_value);
	tm = *localtime(&time_value);
	/*
	 * Beep on the half hour; double-beep on the hour.
	 */
	if (w->date.chime == TRUE) {
	    if (w->date.beeped && (tm.tm_min != 30) &&
		(tm.tm_min != 0))
	      w->date.beeped = FALSE;
	    if (((tm.tm_min == 30) || (tm.tm_min == 0)) 
		&& (!w->date.beeped)) {
		w->date.beeped = TRUE;
		XBell(dpy, 50);	
		if (tm.tm_min == 0)
		  XBell(dpy, 50);
	    }
	}
	{
	    int i, len, prev_len;
	    char *datep, *startp, *p;

	    char *date_fmt = (char *) w->date.date;

	    time_ptr = asctime(&tm);
	    datep = (char *) malloc(40);
	    startp = datep;
	    for (p=date_fmt; p && *p; p++) {
		if (*p != '%')
           	    *datep++ = *p;
        	else switch (*++p) {
            	    case 'M':
                	    datep += strlen(strcpy(datep, Months[tm.tm_mon]));
			    break;
            	    case 'm':
                	    datep += strlen(strcpy(datep, months[tm.tm_mon]));
			    break;
            	    case 'W':
                	    datep += strlen(strcpy(datep, Days[tm.tm_wday]));
			    break;
            	    case 'w':
                	    datep += strlen(strcpy(datep, days[tm.tm_wday]));
			    break;
            	    case 'd':
                	    if (tm.tm_mday >= 10)
                    	    *datep++ = (tm.tm_mday / 10 + '0');
                	    *datep++ = tm.tm_mday % 10 + '0';
			    break;
            	    case 'Y':
                	    *datep++ = '1', *datep++ = '9';
                	    /* fall thru */
            	    case 'y':
                	    *datep++ = tm.tm_year / 10 + '0';
                	    *datep++ = tm.tm_year % 10 + '0';
			    break;
            	    case '%':
                	    *datep++ = *p;
            	    otherwise: ; /* nothing */
       	        }
    	    }
    	    *datep = '\0';

	    if (strcmp(startp, w->date.prev_time_string)) {
	    	XFillRectangle (dpy, win, w->date.EraseGC,
	    		0, 0, w->core.width, w->core.height);

	    	XDrawImageString (dpy, win, w->date.myGC,
	        	(w->core.width - w->date.padding -
			XTextWidth(w->date.font, startp,
			(int) (datep-startp)))/2, 
			2+w->date.font->ascent+w->date.padding,
			startp, (int)(datep - startp));
	    }
	    len = strlen (startp);
	    if (startp[len - 1] == '\n') startp[--len] = '\0';
	    prev_len = strlen (w->date.prev_time_string);
	    for (i = 0; ((i < len) && (i < prev_len) && 
	    		 (w->date.prev_time_string[i] == startp[i])); i++);
	    strcpy (w->date.prev_time_string+i, startp+i);
	    free(startp);
    	}
}
	
static int round(x)
double x;
{
	return(x >= 0.0 ? (int)(x + .5) : (int)(x - .5));
}

/* ARGSUSED */
static Boolean SetValues (gcurrent, grequest, gnew)
    Widget gcurrent, grequest, gnew;
{
      DateWidget current = (DateWidget) gcurrent;
      DateWidget new = (DateWidget) gnew;
      Boolean redisplay = FALSE;
      XtGCMask valuemask;
      XGCValues	myXGCV;

      /* first check for changes to date-specific resources.  We'll accept all
         the changes, but may need to do some computations first. */

      if (new->date.update != current->date.update) {
	  if (current->date.interval_id)
	      XtRemoveTimeOut (current->date.interval_id);
	  if (XtIsRealized(new))
	      new->date.interval_id = XtAppAddTimeOut( 
                                         XtWidgetToApplicationContext(gnew),
					 new->date.update*1000,
				         clock_tic, (caddr_t)gnew);

      }

      if (new->date.padding != current->date.padding)
	   redisplay = TRUE;

       if (new->date.font != current->date.font)
	   redisplay = TRUE;

      if ((new->date.fgpixel != current->date.fgpixel)
          || (new->core.background_pixel != current->core.background_pixel)) {
          valuemask = GCForeground | GCBackground | GCFont | GCLineWidth;
	  myXGCV.foreground = new->date.fgpixel;
	  myXGCV.background = new->core.background_pixel;
          myXGCV.font = new->date.font->fid;
	  myXGCV.line_width = 0;
	  XtDestroyGC (current->date.myGC);
	  new->date.myGC = XtGetGC(gcurrent, valuemask, &myXGCV);
	  redisplay = TRUE;
          }

      if (new->core.background_pixel != current->core.background_pixel) {
          valuemask = GCForeground | GCLineWidth;
	  myXGCV.foreground = new->core.background_pixel;
	  myXGCV.line_width = 0;
	  XtDestroyGC (current->date.EraseGC);
	  new->date.EraseGC = XtGetGC((Widget)gcurrent, valuemask, &myXGCV);
	  redisplay = TRUE;
	  }
     
     return (redisplay);

}
