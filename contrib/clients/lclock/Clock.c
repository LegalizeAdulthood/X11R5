/*
 * Clock.c
 *
 * a NeWS clone clock
 *
 * Dave Lemke
 * lemke@ncd.com
 * Wed Feb 28 16:42:39 PST 1990
 *
 * idea from the NeWS sunclock demo by Stuart Marks and Greg McLaughlin.
 * code is whacked over 'oclock' by Keith Packard
 */

#include <X11/Xos.h>
#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Converters.h>
#include "ClockP.h"
#include <math.h>
#include <X11/extensions/shape.h>

#define offset(field) XtOffset(ClockWidget,clock.field)
#define goffset(field) XtOffset(Widget,core.field)

static XtResource resources[] = {
    {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
    goffset(width), XtRString, "120"},
    {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
    goffset(height), XtRString, "120"},
    {XtNminute, XtCForeground, XtRPixel, sizeof(Pixel),
    offset(minute), XtRString, XtDefaultBackground},
    {XtNhour, XtCForeground, XtRPixel, sizeof(Pixel),
    offset(hour), XtRString, XtDefaultBackground},
    {XtNborder, XtCForeground, XtRPixel, sizeof(Pixel),
    offset(border), XtRString, XtDefaultBackground},
    {XtNreverseVideo, XtCReverseVideo, XtRBoolean, sizeof(Boolean),
    offset(reverse_video), XtRString, "FALSE"},
    {XtNbackingStore, XtCBackingStore, XtRBackingStore, sizeof(int),
    offset(backing_store), XtRString, "default"},
};

#undef offset
#undef goffset

static void Initialize(), Realize(), Destroy(), Resize(), Redisplay();

#define BORDER_WIDTH	(0.1)
#define WINDOW_WIDTH	(2.0 - BORDER_WIDTH*2)
#define WINDOW_HEIGHT	(2.0 - BORDER_WIDTH*2)
#define MINUTE_WIDTH	(BORDER_WIDTH / 2.0)
#define HOUR_WIDTH	(BORDER_WIDTH / 2.0)
#define JEWEL_SIZE	(BORDER_WIDTH * 0.75)
#define MINUTE_LENGTH	(0.8)
#define HOUR_LENGTH	(0.5)

#define HAND_BORDER_WIDTH	(0.02)

#define LOGO_BORDER_WIDTH	(w->core.width/20)
#define LOGO_BORDER_HEIGHT	(w->core.height/20)
#define LOGO_WINDOW_WIDTH	(w->core.width - LOGO_BORDER_WIDTH*2)
#define LOGO_WINDOW_HEIGHT	(w->core.height - LOGO_BORDER_HEIGHT*2)

#define	HOUR_HAND	1
#define	MINUTE_HAND	2


static void ClassInitialize();

ClockClassRec clockClassRec = {
    {				/* core fields */
	 /* superclass		 	 */ &widgetClassRec,
	 /* class_name			 */ "LogoClock",
	 /* size			 */ sizeof(ClockRec),
	 /* class_initialize		 */ ClassInitialize,
	 /* class_part_initialize	 */ NULL,
	 /* class_inited		 */ FALSE,
	 /* initialize			 */ Initialize,
	 /* initialize_hook		 */ NULL,
	 /* realize			 */ Realize,
	 /* actions			 */ NULL,
	 /* num_actions			 */ 0,
	 /* resources			 */ resources,
	 /* num_resources		 */ XtNumber(resources),
	 /* xrm_class			 */ NULL,
	 /* compress_motion		 */ TRUE,
	 /* compress_exposure		 */ TRUE,
	 /* compress_enterleave		 */ TRUE,
	 /* visible_interest		 */ FALSE,
	 /* destroy			 */ Destroy,
	 /* resize			 */ Resize,
	 /* expose			 */ Redisplay,
	 /* set_values			 */ NULL,
	 /* set_values_hook		 */ NULL,
	 /* set_values_almost		 */ NULL,
	 /* get_values_hook		 */ NULL,
	 /* accept_focus		 */ NULL,
	 /* version			 */ XtVersion,
	 /* callback_private		 */ NULL,
	 /* tm_table			 */ NULL,
	 /* query_geometry		 */ XtInheritQueryGeometry,
    }
};

static void
ClassInitialize()
{
    XtAddConverter(XtRString, XtRBackingStore, XmuCvtStringToBackingStore,
		   NULL, 0);
}

WidgetClass clockWidgetClass = (WidgetClass) & clockClassRec;

/* ARGSUSED */
static void
Initialize(greq, gnew)
    Widget      greq,
                gnew;
{
    ClockWidget w = (ClockWidget) gnew;
    int         shape_event_base,
                shape_error_base;

    /* wait for Realize to add the timeout */
    w->clock.interval_id = 0;

    if (!XShapeQueryExtension(XtDisplay(w), &shape_event_base,
			      &shape_error_base)) {
	fprintf(stderr, "sorry, lclock needs the SHAPE extension to run\n");
    }
    w->clock.shape_mask = 0;
    w->clock.shapeGC = 0;
    w->clock.shapebackGC = 0;
    w->clock.handGC = 0;
    w->clock.shape_width = 0;
    w->clock.shape_height = 0;
}

static void
Resize(w)
    ClockWidget w;
{
    int         face_width,
                face_height;

    if (!XtIsRealized(w))
	return;

    /*
     * compute desired border size
     */

    SetTransform(&w->clock.maskt,
		 0, w->core.width,
		 w->core.height, 0,
		 -1.0, 1.0,
		 -1.0, 1.0);

    face_width = abs(Xwidth(BORDER_WIDTH, BORDER_WIDTH, &w->clock.maskt));
    face_height = abs(Xheight(BORDER_WIDTH, BORDER_WIDTH, &w->clock.maskt));

    /*
     * shape the windows and borders
     */

    SetTransform(&w->clock.t,
		 face_width, w->core.width - face_width,
		 w->core.height - face_height, face_height,
		 -WINDOW_WIDTH / 2, WINDOW_WIDTH / 2,
		 -WINDOW_HEIGHT / 2, WINDOW_HEIGHT / 2);

    draw_clock(w);
}


draw_clock(w)
    ClockWidget w;
{
    XGCValues   xgcv;
    Widget      parent;
    int         x,
                y;
    Pixmap      shape_mask;

    /*
     * allocate a pixmap to draw shapes in
     */

    shape_mask = XCreatePixmap(XtDisplay(w), XtWindow(w),
			       w->core.width, w->core.height, 1);
    if (!w->clock.shapeGC) {
	w->clock.shapeGC = XCreateGC(XtDisplay(w), shape_mask, 0, &xgcv);
	xgcv.foreground = 0;
	w->clock.shapebackGC = XCreateGC(XtDisplay(w), shape_mask,
					 GCForeground, &xgcv);
    }
    /* erase the pixmap */
    XSetForeground(XtDisplay(w), w->clock.shapeGC, 0);
    XFillRectangle(XtDisplay(w), shape_mask, w->clock.shapeGC,
		   0, 0, w->core.width, w->core.height);
    XSetForeground(XtDisplay(w), w->clock.shapeGC, 1);

    /*
     * draw the bounding shape.  Doing this first eliminates extra exposure
     * events.
     */

    XmuDrawLogo(XtDisplay(w), shape_mask, w->clock.shapeGC,
		w->clock.shapebackGC,
		0, 0,
		w->core.width, w->core.height);

    draw_hands(w, shape_mask, True);
    /*
     * Find the highest enclosing widget and shape it
     */

    x = 0;
    y = 0;
    for (parent = (Widget) w; XtParent(parent); parent = XtParent(parent)) {
	x = x + parent->core.x + parent->core.border_width;
	y = y + parent->core.y + parent->core.border_width;
    }

    XShapeCombineMask(XtDisplay(parent), XtWindow(parent), ShapeBounding,
		      x, y, shape_mask, ShapeSet);

    /* erase the pixmap */
    XSetForeground(XtDisplay(w), w->clock.shapeGC, 0);
    XFillRectangle(XtDisplay(w), shape_mask, w->clock.shapeGC,
		   0, 0, w->core.width, w->core.height);
    XSetForeground(XtDisplay(w), w->clock.shapeGC, 1);

    /*
     * draw the clip shape
     */

    XmuDrawLogo(XtDisplay(w), shape_mask, w->clock.shapeGC,
		w->clock.shapebackGC,
		LOGO_BORDER_WIDTH, LOGO_BORDER_HEIGHT,
		LOGO_WINDOW_WIDTH, LOGO_WINDOW_HEIGHT);

    draw_hands(w, shape_mask, False);

    XShapeCombineMask(XtDisplay(w), XtWindow(w), ShapeClip,
		      0, 0, shape_mask, ShapeSet);

    XFreePixmap(XtDisplay(w), shape_mask);
}

draw_hands(w, mask, border)
    ClockWidget w;
    Pixmap      mask;
    Bool        border;
{
    draw_hand(w, mask, MINUTE_HAND, border, w->clock.shapeGC);
    draw_hand(w, mask, HOUR_HAND, border, w->clock.shapeGC);
}

draw_hand(w, drawable, hand, border, gc)
    ClockWidget w;
    Drawable    drawable;
    int         hand;
    Bool        border;
    GC          gc;
{
    TPoint      poly[POLY_SIZE];
    double      angle,
                width,
                length;

    switch (hand) {
    case HOUR_HAND:
	angle = w->clock.hour_angle;
	width = HOUR_WIDTH;
	length = HOUR_LENGTH;
	break;
    case MINUTE_HAND:
	angle = w->clock.minute_angle;
	width = MINUTE_WIDTH;
	length = MINUTE_LENGTH;
	break;
    }

    if (border)
	compute_hand(angle, length, width, poly);
    else
	compute_hand(angle,
		     length - 2 * HAND_BORDER_WIDTH,
		     width - 2 * HAND_BORDER_WIDTH,
		     poly);

    TFillPolygon(XtDisplay(w), drawable, gc,
		 &w->clock.t, poly, POLY_SIZE,
		 Convex, CoordModeOrigin);
}

/* ARGSUSED */
static void
Redisplay(gw, event, region)
    Widget gw;
    XEvent *event;
    Region region;
{
    ClockWidget w;

    w = (ClockWidget) gw;
    XClearWindow(XtDisplay(w), XtWindow(w));	/* erase the old hands */
    if (!w->clock.handGC)
	w->clock.handGC = XCreateGC(XtDisplay(w), XtWindow(w), 0, NULL);
    paint_hands(w);
}

static void
Realize(gw, valueMask, attrs)
    Widget      gw;
    XtValueMask *valueMask;
    XSetWindowAttributes *attrs;
{
    ClockWidget w = (ClockWidget) gw;
    static int  new_time();

    if (w->clock.backing_store != Always + WhenMapped + NotUseful) {
	attrs->backing_store = w->clock.backing_store;
	*valueMask |= CWBackingStore;
    }
    XtCreateWindow(gw, (unsigned) InputOutput, (Visual *) CopyFromParent,
		   *valueMask, attrs);

    Resize(w);
    new_time((caddr_t) gw, 0);
}

static void
Destroy(gw)
    Widget      gw;
{
    ClockWidget w = (ClockWidget) gw;

    if (w->clock.interval_id)
	XtRemoveTimeOut(w->clock.interval_id);
    if (w->clock.shapeGC)
	XtDestroyGC(w->clock.shapeGC);
    if (w->clock.shapebackGC)
	XtDestroyGC(w->clock.shapebackGC);
    if (w->clock.handGC)
	XtDestroyGC(w->clock.handGC);
}


/*
 * routines to draw the hands
 */

#define PI (3.14159265358979323846)

/*
 * converts a number from 0..1 representing a clockwise radial distance
 * from the 12 oclock position to a radian measure of the counter-clockwise
 * distance from the 3 oclock position
 */

static double
clock_to_angle(clock)
    double      clock;
{
    if (clock >= .75)
	clock -= 1.0;
    return -2.0 * PI * clock + PI / 2.0;
}

/* ARGSUSED */
static int
new_time(client_data, id)
    caddr_t     client_data;
    XtIntervalId id;		/* unused */
{
    ClockWidget w = (ClockWidget) client_data;
    long        now;
    struct tm  *localtime(),
               *tm;

    (void) time(&now);
    tm = localtime(&now);
    if (tm->tm_hour >= 12)
	tm->tm_hour -= 12;
    w->clock.hour_angle = clock_to_angle((((double) tm->tm_hour) +
				       ((double) tm->tm_min) / 60.0) / 12.0);
    w->clock.minute_angle =
	clock_to_angle(((double) tm->tm_min) / 60.0);
    /*
     * add the timeout before painting the hands, that may take a while and
     * we'd like the clock to keep up with time changes.
     */
    w->clock.interval_id = XtAddTimeOut((60 - tm->tm_sec) * 1000, new_time,
					client_data);
    draw_clock(w);
}				/* new_time */


/*
 * A hand is a rectangle with a triangular cap at the far end.
 * This is represented with a five sided polygon.
 */

compute_hand(a, l, width, poly)
    double      a,
                l,
                width;
    TPoint      poly[POLY_SIZE];
{
    double      c,
                s;

    c = cos(a);
    s = sin(a);
    poly[0].x = c * l;
    poly[0].y = s * l;
    poly[1].x = (l - width) * c - s * width;
    poly[1].y = (l - width) * s + c * width;
    poly[2].x = (-width) * c - s * width;
    poly[2].y = (-width) * s + c * width;
    poly[3].x = (-width) * c + s * width;
    poly[3].y = (-width) * s - c * width;
    poly[4].x = (l - width) * c + s * width;
    poly[4].y = (l - width) * s - c * width;
    poly[5].x = poly[0].x;
    poly[5].y = poly[0].y;
}

paint_hands(w)
    ClockWidget w;
{
    GC          gc = w->clock.handGC;
    Display    *dpy = XtDisplay(w);

    /* draw the hands, border first, hour hand on top */
    XSetForeground(dpy, gc, w->clock.border);
    draw_hand(w, XtWindow(w), MINUTE_HAND, True, gc);
    XSetForeground(dpy, gc, w->clock.minute);
    draw_hand(w, XtWindow(w), MINUTE_HAND, False, gc);

    XSetForeground(dpy, gc, w->clock.border);
    draw_hand(w, XtWindow(w), HOUR_HAND, True, gc);
    XSetForeground(dpy, gc, w->clock.hour);
    draw_hand(w, XtWindow(w), HOUR_HAND, False, gc);
}
