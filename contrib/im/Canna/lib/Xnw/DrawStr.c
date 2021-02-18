/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of NEC
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  NEC Corporation makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */

/*

  This program is made by modifying Label.c.

 */

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
 * DrawStr.c - DrawStr widget
 *
 */

#define XtStrlen(s)		((s) ? strlen(s) : 0)

#include <stdio.h>
#include <ctype.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "XawInit.h"
#include "DrawStrP.h"
#include <X11/kanji.h>

#define streq(a,b) (strcmp( (a), (b) ) == 0)

#define MULTI_LINE_LABEL 32767

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffset(DrawStrWidget, field)
static XtResource resources[] = {
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(drawStr.foreground), XtRString, "XtDefaultForeground"},
    {XtNbackground, XtCBackground, XtRPixel, sizeof(Pixel),
	offset(drawStr.background), XtRString, "XtDefaultBackground"},
    {XtNfont,  XtCFont, XtRFontStruct, sizeof(XFontStruct *),
        offset(drawStr.font),XtRString, XnwDefaultFont},
    {XtNkanjiFont,  XtCKanjiFont, XtRKanjiFontStruct, sizeof(XFontStruct *),
        offset(drawStr.kanji_font),XtRString, XnwDefaultKanjiFont},
    {XtNkanaFont,  XtCKanaFont, XtRKanaFontStruct, sizeof(XFontStruct *),
        offset(drawStr.kana_font),XtRString, XnwDefaultKanaFont},
    {XtNlabel,  XtCLabel, XtRString, sizeof(String),
	offset(drawStr.label), XtRString, NULL},
    {XtNreverse, XtCReverse, XtRBoolean, sizeof(Boolean),
	offset(drawStr.reverse), XtRImmediate, (caddr_t)False},
};

static void Initialize();
static void Resize();
static void Redisplay();
static Boolean SetValues();
static void ClassInitialize();
static void Destroy();
static XtGeometryResult QueryGeometry();

DrawStrClassRec drawStrClassRec = {
  {
/* core_class fields */	
#define superclass		(&simpleClassRec)
    /* superclass	  	*/	(WidgetClass) superclass,
    /* class_name	  	*/	"DrawStr",
    /* widget_size	  	*/	sizeof(DrawStrRec),
    /* class_initialize   	*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize		  	*/	XtInheritRealize,
    /* actions		  	*/	NULL,
    /* num_actions	  	*/	0,
    /* resources	  	*/	resources,
    /* num_resources	  	*/	XtNumber(resources),
    /* xrm_class	  	*/	NULLQUARK,
    /* compress_motion	  	*/	TRUE,
    /* compress_exposure  	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest	  	*/	FALSE,
    /* destroy		  	*/	Destroy,
    /* resize		  	*/	Resize,
    /* expose		  	*/	Redisplay,
    /* set_values	  	*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus	 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* tm_table		   	*/	NULL,
    /* query_geometry		*/	QueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
/* Simple class fields initialization */
  {
    /* change_sensitive		*/	XtInheritChangeSensitive
  }
};
WidgetClass drawStrWidgetClass = (WidgetClass)&drawStrClassRec;
/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void ClassInitialize()
{
    extern void XmuCvtStringToJustify();
    extern void XmuCvtStringToBitmap();
    static XtConvertArgRec screenConvertArg[] = {
        {XtWidgetBaseOffset, (caddr_t) XtOffset(Widget, core.screen),
	     sizeof(Screen *)}
    };
    XawInitializeWidgetSet();
    XtAddConverter( XtRString, XtRJustify, XmuCvtStringToJustify, NULL, 0 );
    XtAddConverter("String", "Bitmap", XmuCvtStringToBitmap,
		   screenConvertArg, XtNumber(screenConvertArg));
    XtAddConverter( XtRString, XtRFontStruct,
                        XnwCvtStringToFontStruct,
                        screenConvertArg, XtNumber(screenConvertArg));
    XtAddConverter( XtRString, XtRKanjiFontStruct,
                        XnwCvtStringToKanjiFontStruct,
                        screenConvertArg, XtNumber(screenConvertArg));
    XtAddConverter( XtRString, XtRKanaFontStruct,
                        XnwCvtStringToKanaFontStruct,
                        screenConvertArg, XtNumber(screenConvertArg));
} /* ClassInitialize */

/*
 * Calculate width and height of displayed text in pixels
 */

static void SetTextWidthAndHeight(lw)
    DrawStrWidget lw;
{
    register XFontStruct	*fs = lw->drawStr.font;
    register XFontStruct	*kanji_fs = lw->drawStr.kanji_font;
    register XFontStruct	*kana_fs = lw->drawStr.kana_font;
    char *nl;

    lw->drawStr.max_ascent = XnwMax3(fs->max_bounds.ascent,
					kanji_fs->max_bounds.ascent,
					kana_fs->max_bounds.ascent);
    lw->drawStr.max_descent = XnwMax3(fs->max_bounds.descent,
					kanji_fs->max_bounds.descent,
					kana_fs->max_bounds.descent);
    lw->drawStr.label_height = lw->drawStr.max_ascent + lw->drawStr.max_descent;
    if (lw->drawStr.label == NULL) {
	lw->drawStr.label_len = 0;
	lw->drawStr.label_width = 0;
    }
    else if ((nl = strchr(lw->drawStr.label, '\n')) != NULL) {
	char *label;
	lw->drawStr.label_len = MULTI_LINE_LABEL;
	lw->drawStr.label_width = 0;
	for (label = lw->drawStr.label; nl != NULL; nl = strchr(label, '\n')) {
	    int width = XKanjiTextWidth(&(lw->drawStr.font), label,
					(int)(nl - label), fs_jis_euc3_mode);
	    if (width > lw->drawStr.label_width) lw->drawStr.label_width = width;
	    label = nl + 1;
	    if (*label)
		lw->drawStr.label_height +=
		    lw->drawStr.max_ascent + lw->drawStr.max_descent;
	}
	if (*label) {
	    int width = XKanjiTextWidth(&(lw->drawStr.font), label,
					strlen(label), fs_jis_euc3_mode);
	    if (width > lw->drawStr.label_width) lw->drawStr.label_width = width;
	}
    } else {
	lw->drawStr.label_len = strlen(lw->drawStr.label);
	lw->drawStr.label_width =
	    XKanjiTextWidth(&(lw->drawStr.font), lw->drawStr.label,
				(int) lw->drawStr.label_len, fs_jis_euc3_mode);
    }
}

static void GetnormalGC(lw)
    DrawStrWidget lw;
{
    XGCValues	values;

    values.foreground	= lw->drawStr.foreground;
    values.background	= lw->core.background_pixel;
    values.font		= lw->drawStr.font->fid;

    lw->drawStr.normal_GC = XtGetGC(
	(Widget)lw,
	(unsigned) GCForeground | GCBackground | GCFont,
	&values);
    values.font = lw->drawStr.kanji_font->fid;
    lw->drawStr.kanji_normal_GC = XtGetGC(
        (Widget)lw,
        (unsigned) GCForeground | GCBackground | GCFont,
        &values);

    values.font = lw->drawStr.kana_font->fid;
    lw->drawStr.kana_normal_GC = XtGetGC(
        (Widget)lw,
        (unsigned) GCForeground | GCBackground | GCFont,
        &values);
}

static void GetreverseGC(lw)
    DrawStrWidget lw;
{
    XGCValues	values;

    values.foreground	= lw->core.background_pixel;
    values.background	= lw->drawStr.foreground;
    values.font		= lw->drawStr.font->fid;

    lw->drawStr.reverse_GC = XtGetGC(
	(Widget)lw,
	(unsigned) GCForeground | GCBackground | GCFont,
	&values);
    values.font = lw->drawStr.kanji_font->fid;
    lw->drawStr.kanji_reverse_GC = XtGetGC(
        (Widget)lw,
        (unsigned) GCForeground | GCBackground | GCFont,
        &values);

    values.font = lw->drawStr.kana_font->fid;
    lw->drawStr.kana_reverse_GC = XtGetGC(
        (Widget)lw,
        (unsigned) GCForeground | GCBackground | GCFont,
        &values);
}

/* ARGSUSED */
static void Initialize(request, new)
 Widget request, new;
{
    DrawStrWidget lw = (DrawStrWidget) new;

    if (lw->drawStr.label == NULL) 
        lw->drawStr.label = XtNewString(lw->core.name);
    else {
        lw->drawStr.label = XtNewString(lw->drawStr.label);
    }

    GetnormalGC(lw);
    GetreverseGC(lw);

    SetTextWidthAndHeight(lw);

    if (lw->core.width == 0)
        lw->core.width = lw->drawStr.label_width + 1;
    if (lw->core.height == 0)
        lw->core.height = lw->drawStr.label_height;

    lw->drawStr.label_x = 1;
    lw->drawStr.label_y = 0;
    (*XtClass(new)->core_class.resize) ((Widget)lw);

} /* Initialize */

/*
 * Repaint the widget window
 */

/* ARGSUSED */
static void Redisplay(w, event, region)
    Widget w;
    XEvent *event;
    Region region;
{
   DrawStrWidget lw = (DrawStrWidget) w;
   GC *gc;

   if (region != NULL &&
       XRectInRegion(region, lw->drawStr.label_x, lw->drawStr.label_y,
		     lw->drawStr.label_width, lw->drawStr.label_height)
	     == RectangleOut)
       return;

   gc = lw->drawStr.reverse ? &(lw->drawStr.reverse_GC) : &(lw->drawStr.normal_GC);
    {
       int len = lw->drawStr.label_len;
       char *label = lw->drawStr.label;
       Position y;
       y = lw->drawStr.label_y + lw->drawStr.max_ascent;
       if (len == MULTI_LINE_LABEL) {
	   char *nl;
	   while ((nl = strchr(label, '\n')) != NULL) {
	       XDrawImageKanjiString(
			   XtDisplay(w), XtWindow(w), gc, &lw->drawStr.font,
			   lw->drawStr.label_x,
			   y, label, (int)(nl - label), fs_jis_euc3_mode);
	       y += lw->drawStr.max_ascent + lw->drawStr.max_descent;
	       label = nl + 1;
	   }
	   len = strlen(label);
       }
       if (len)
	   XDrawImageKanjiString(
		       XtDisplay(w), XtWindow(w), gc, &lw->drawStr.font,
			lw->drawStr.label_x,
		       y, label, len, fs_jis_euc3_mode);
    }
}

static void _Reposition(lw, width, height, dx, dy)
    register DrawStrWidget lw;
    Dimension width, height;
    Position *dx, *dy;
{
    *dx = *dy = 0;
    lw->drawStr.label_x = 1;
    lw->drawStr.label_y = 0;
    return;
}

static void Resize(w)
    Widget w;
{
    DrawStrWidget lw = (DrawStrWidget)w;
    Position dx, dy;
    _Reposition(lw, w->core.width, w->core.height, &dx, &dy);
}

/*
 * Set specified arguments into widget
 */

#define PIXMAP 0
#define WIDTH 1
#define HEIGHT 2
#define NUM_CHECKS 3

static Boolean SetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
    DrawStrWidget curlw = (DrawStrWidget) current;
    DrawStrWidget reqlw = (DrawStrWidget) request;
    DrawStrWidget newlw = (DrawStrWidget) new;
    int i;
    Boolean was_resized = False, redisplay = False, checks[NUM_CHECKS];

    for (i = 0; i < NUM_CHECKS; i++)
	checks[i] = FALSE;

    for (i = 0; i < *num_args; i++) {
	if (streq(XtNwidth, args[i].name))
	    checks[WIDTH] = TRUE;
	if (streq(XtNheight, args[i].name))
	    checks[HEIGHT] = TRUE;
    }

    if (newlw->drawStr.label == NULL) {
	newlw->drawStr.label = newlw->core.name;
    }

    if (curlw->drawStr.label != newlw->drawStr.label) {
        if (curlw->drawStr.label != curlw->core.name)
	    XtFree( (char *)curlw->drawStr.label );

	if (newlw->drawStr.label != newlw->core.name) {
	    newlw->drawStr.label = XtNewString( newlw->drawStr.label );
	}
	was_resized = True;
    }

    if (was_resized || (curlw->drawStr.font != newlw->drawStr.font) ||
        (curlw->drawStr.kanji_font != newlw->drawStr.kanji_font) ||
        (curlw->drawStr.kana_font != newlw->drawStr.kana_font) ||
	checks[PIXMAP]) {

	SetTextWidthAndHeight(newlw);
	was_resized = True;
    }

    /* recalculate the window size if something has changed. */
    if (was_resized) {
	if ((curlw->core.width == reqlw->core.width) && !checks[WIDTH])
	    newlw->core.width = newlw->drawStr.label_width + 1;

	if ((curlw->core.height == reqlw->core.height) && !checks[HEIGHT])
	    newlw->core.height = newlw->drawStr.label_height;
    }

    if (curlw->drawStr.foreground != newlw->drawStr.foreground
        || curlw->drawStr.font->fid != newlw->drawStr.font->fid
        || curlw->drawStr.kanji_font->fid != newlw->drawStr.kanji_font->fid
        || curlw->drawStr.kana_font->fid != newlw->drawStr.kana_font->fid) {

	XtReleaseGC(new, curlw->drawStr.normal_GC);
	XtReleaseGC(new, curlw->drawStr.reverse_GC);
        XtReleaseGC(new, curlw->drawStr.kanji_normal_GC);
        XtReleaseGC(new, curlw->drawStr.kana_normal_GC);
        XtReleaseGC(new, curlw->drawStr.kanji_reverse_GC);
        XtReleaseGC(new, curlw->drawStr.kana_reverse_GC);
	GetnormalGC(newlw);
	GetreverseGC(newlw);
	redisplay = True;
    }

    return was_resized || redisplay ||
	   curlw->drawStr.reverse != newlw->drawStr.reverse;
}

static void Destroy(w)
    Widget w;
{
    DrawStrWidget lw = (DrawStrWidget)w;

    XtFree( lw->drawStr.label );
    XtReleaseGC( w, lw->drawStr.normal_GC );
    XtReleaseGC( w, lw->drawStr.reverse_GC);
    XtReleaseGC( w, lw->drawStr.kanji_normal_GC);
    XtReleaseGC( w, lw->drawStr.kana_normal_GC);
    XtReleaseGC( w, lw->drawStr.kanji_reverse_GC);
    XtReleaseGC( w, lw->drawStr.kana_reverse_GC);
}


static XtGeometryResult QueryGeometry(w, intended, preferred)
    Widget w;
    XtWidgetGeometry *intended, *preferred;
{
    register DrawStrWidget lw = (DrawStrWidget)w;

    preferred->request_mode = CWWidth | CWHeight;
    preferred->width = lw->drawStr.label_width;
    preferred->height = lw->drawStr.label_height;
    if (  ((intended->request_mode & (CWWidth | CWHeight))
	   	== (CWWidth | CWHeight)) &&
	  intended->width == preferred->width &&
	  intended->height == preferred->height)
	return XtGeometryYes;
    else if (preferred->width == w->core.width &&
	     preferred->height == w->core.height)
	return XtGeometryNo;
    else
	return XtGeometryAlmost;
}
