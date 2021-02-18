/*
 *     Copyright (c) 1991 Oki Techosystems Laboratory, Inc.
 *     Copyright (c) 1991 Oki Electric Industry Co., Ltd.
 */
#ifndef lint
static char sccsid[] = "@(#)StatusDisp.c	1.1";
#endif
/*
 * StatusDisp.c -
 *
 * 1.1 91/08/30 16:36:31
 */
#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "StatusDisP.h"

#define DEFAULTFONT "-*-*-*-R-Normal--*-130-75-75-*-*"

#define offset(field) XtOffset(StatusDispWidget, statusDisp.field)

static XtResource resources[] = {
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(foreground), XtRString, "XtDefaultForeground"},
    {XtNfontSet, XtCFontSet, XtRFontSet, sizeof(XFontSet),
	offset(font_set), XtRString, DEFAULTFONT},
    {XtNlabel,  XtCLabel, XtRString, sizeof(String),
	offset(status), XtRString, NULL},
};

#undef offset

#define streq(a,b) (strcmp( (a), (b) ) == 0)

static void Initialize(), Realize(), Redisplay();
static Boolean SetValues();

StatusDispClassRec statusDispClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"StatusDisp",
    /* widget_size		*/	sizeof(StatusDispRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	NULL,
    /* resize			*/	NULL,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	NULL,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* statusDisp fields */
    /* dummy			*/	NULL,
  }
};

WidgetClass statusDispWidgetClass = (WidgetClass)&statusDispClassRec;

static void
GetnormalGC(sw)
    StatusDispWidget sw;
{
    XGCValues	values;
    
    values.foreground = sw->statusDisp.foreground;
    values.background = sw->core.background_pixel;
    
    sw->statusDisp.normal_GC = XtGetGC((Widget)sw,
				    (unsigned) GCForeground | GCBackground ,
				    &values);
}

static void
SetWidthAndHeight(sw)
StatusDispWidget sw;
{
    XFontSet	fs = sw->statusDisp.font_set;
    XFontSetExtents *font_set_extents;
    
    font_set_extents = (XFontSetExtents *)XExtentsOfFontSet(fs);
    sw->statusDisp.status_height = font_set_extents->max_logical_extent.height
                                   + 2 * sw->core.border_width;
    sw->statusDisp.offset_x = sw->core.border_width;
    sw->statusDisp.offset_y = -font_set_extents->max_logical_extent.y +
                              sw->core.border_width;
    if (sw->statusDisp.status == NULL) {
	sw->statusDisp.status_len = 0;
	sw->statusDisp.status_width = 0;
    } else {
 	sw->statusDisp.status_width = 
	XmbTextEscapement(fs,
			  sw->statusDisp.status,
			  sw->statusDisp.status_len);
    }
}

/* ARGSUSED */
static void
Initialize(request, new)
    Widget request, new;
{
    StatusDispWidget sw = (StatusDispWidget)new;
/*
 *  デフォルトの表示内容を設定する。
 */

    if (sw->statusDisp.status == NULL) {
	sw->statusDisp.status = XtNewString("");
	sw->statusDisp.status_len = strlen(sw->statusDisp.status);
    } else {
        sw->statusDisp.status = XtNewString(sw->statusDisp.status);
	sw->statusDisp.status_len = strlen(sw->statusDisp.status);
    }
    GetnormalGC(sw);
    SetWidthAndHeight(sw);
    if (sw->core.width == 0) {
	if (sw->statusDisp.status_width == 0) {
	    sw->core.width = 1;
	} else {
	    sw->core.width = sw->statusDisp.status_width;
	}
    }
    if (sw->core.height == 0) {
	sw->core.height = sw->statusDisp.status_height;
    }
} /* Initialize */

static void
Realize(w, valueMask, attributes)
    register Widget w;
    Mask *valueMask;
    XSetWindowAttributes *attributes;
{
    XtCreateWindow(w, (unsigned int)InputOutput, (Visual *)CopyFromParent,
		   *valueMask, attributes);
}


/* ARGSUSED */
static void
Destroy(w)
    Widget w;
{
    StatusDispWidget sw = (StatusDispWidget)w;
    
    XtFree(sw->statusDisp.status);
    XtReleaseGC(w, sw->statusDisp.normal_GC);
}

/* ARGSUSED */
static void Redisplay(w, event, region)
    Widget w;
    XEvent *event;
    Region region;
{
    StatusDispWidget sw = (StatusDispWidget) w;

    if (sw->statusDisp.status_len) {
	XmbDrawString(XtDisplay(w), XtWindow(w),
		      sw->statusDisp.font_set, sw->statusDisp.normal_GC,
		      sw->statusDisp.offset_x, sw->statusDisp.offset_y,
		      sw->statusDisp.status, sw->statusDisp.status_len);
    }
}

static Boolean
SetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
    StatusDispWidget cursw = (StatusDispWidget)current;
    StatusDispWidget reqsw = (StatusDispWidget)request;
    StatusDispWidget newsw = (StatusDispWidget)new;
    int i;
    Boolean resized = FALSE;
    Boolean redisplay = FALSE;
    Boolean width_specified = FALSE;
    Boolean height_specified = FALSE;
    
    for (i = 0; i < *num_args; i++) {
	if (streq(XtNwidth, args[i].name)) {
	    width_specified = TRUE;
	}
	if (streq(XtNheight, args[i].name)) {
	    height_specified = TRUE;
	}
    }
    if (cursw->statusDisp.status != newsw->statusDisp.status) {
        XtFree((char *)cursw->statusDisp.status);
	newsw->statusDisp.status = XtNewString(newsw->statusDisp.status);
	newsw->statusDisp.status_len = strlen(newsw->statusDisp.status);
	resized = True;
    }
    if (resized ||
	(cursw->statusDisp.font_set != newsw->statusDisp.font_set)) {
	SetWidthAndHeight(newsw);
	resized = True;
    }
    if (resized) {
	if ((cursw->core.width == reqsw->core.width) && !width_specified) {
	    if (newsw->statusDisp.status_width == 0) {
	        newsw->core.width = 1;
	    } else {
	        newsw->core.width = newsw->statusDisp.status_width;
	    }
	}
	if ((cursw->core.height == reqsw->core.height) && !height_specified) {
	    newsw->core.height = newsw->statusDisp.status_height;
	}
    }
    if (cursw->statusDisp.foreground != newsw->statusDisp.foreground) {
	XtReleaseGC(new, cursw->statusDisp.normal_GC);
	GetnormalGC(newsw);
	redisplay = True;
    }
    return(resized || redisplay);
}

static XtGeometryResult
QueryGeometry(w, intended, preferred)
    Widget w;
    XtWidgetGeometry *intended, *preferred;
{
    StatusDispWidget sw = (StatusDispWidget)w;
    
    preferred->request_mode = CWWidth | CWHeight;
    preferred->width = sw->statusDisp.status_width;
    preferred->height = sw->statusDisp.status_height;
    if (((intended->request_mode & (CWWidth | CWHeight))
	 == (CWWidth | CWHeight)) &&
	intended->width == preferred->width &&
	intended->height == preferred->height) {
	return(XtGeometryYes);
    } else {
	return(XtGeometryAlmost);
    }
}
