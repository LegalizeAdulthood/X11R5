/*
 * Copyright 1991 Oki Technosystems Laboratory, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Oki Technosystems Laboratory not
 * be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * Oki Technosystems Laboratory makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Oki TECHNOSYSTEMS LABORATORY DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL OKI TECHNOSYSTEMS LABORATORY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Yasuhiro Kawai,	Oki Technosystems Laboratory, Inc.
 *				kawai@otsl.oki.co.jp
 */
#ifndef lint
static char sccsid[] = "@(#)Preedit.c	1.4";
#endif
/*
 * Preedit.c -
 *
 *     今のところは漢字を表示する機能だけ。
 *
 * 1.4 91/09/28 18:16:35
 */
#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "PreeditP.h"

#define DEFAULTFONT "-*-*-*-R-Normal--*-130-75-75-*-*"
#define WIDTH_SPECIFIED 0x01
#define HEIGHT_SPECIFIED 0x02

#define offset(field) XtOffset(PreeditWidget, preedit.field)

static XtResource resources[] = {
    {XtNcursor, XtCCursor, XtRCursor, sizeof(Cursor),
        offset(cursor), XtRImmediate, (caddr_t) None},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(foreground), XtRString, "XtDefaultForeground"},
    {XtNfontSet,  XtCFontSet, XtRFontSet, sizeof(XFontSet),
	offset(font_set), XtRString, DEFAULTFONT},
    {XtNspotOffsetX,  XtCX, XtRPosition, sizeof(Position),
	offset(offset_x), XtRPosition, 0},
    {XtNspotOffsetY,  XtCY, XtRPosition, sizeof(Position),
	offset(offset_y), XtRPosition, 0},
};

#undef offset

#define streq(a,b) (strcmp( (a), (b) ) == 0)

static void Initialize(), Realize(), Redisplay();
static Boolean SetValues();
static void SetValuesAlmost();
static void Destroy();
static void Draw();
static int  QueryWidth();
static void Caret();

PreeditClassRec preeditClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"Preedit",
    /* widget_size		*/	sizeof(PreeditRec),
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
    /* destroy			*/	Destroy,
    /* resize			*/	NULL,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	SetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	NULL,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* preedit fields */
    /* draw			*/	Draw,
    /* querywidth		*/	QueryWidth,
    /* caret			*/	Caret,
  }
};

WidgetClass preeditWidgetClass = (WidgetClass)&preeditClassRec;

static void
GetnormalGC(pw)
    PreeditWidget pw;
{
    XGCValues	values;
    
    values.foreground = pw->preedit.foreground;
    values.background = pw->core.background_pixel;
    
    pw->preedit.normal_GC = XtGetGC((Widget)pw,
				    (unsigned) GCForeground | GCBackground ,
				    &values);
}
static void
GetreverseGC(pw)
    PreeditWidget pw;
{
    XGCValues	values;
    
    values.foreground = pw->core.background_pixel;
    values.background = pw->preedit.foreground;
    
    pw->preedit.reverse_GC = XtGetGC((Widget)pw,
				     (unsigned) GCForeground | GCBackground ,
				     &values);
}

static void
SetWidthAndHeight(pw)
    PreeditWidget pw;
{
    XFontSet	fs = pw->preedit.font_set;
    XFontSetExtents *font_set_extents;
    
    font_set_extents = (XFontSetExtents *)XExtentsOfFontSet(fs);
    pw->preedit.preedit_height = font_set_extents->max_logical_extent.height;
    pw->preedit.offset_x = pw->core.border_width;
    pw->preedit.offset_y = -font_set_extents->max_logical_extent.y +
    pw->core.border_width;
    
    pw->preedit.caret_width = XmbTextEscapement(fs, " ", 1);
    pw->preedit.caret_height = pw->preedit.preedit_height;
    
    if (pw->preedit.wcs && pw->preedit.wcs_len) {
	pw->preedit.preedit_width = pw->preedit.offset_x + 
	XwcTextEscapement(fs,
			  pw->preedit.wcs,
			  pw->preedit.wcs_len);
    } else {
	pw->preedit.preedit_width = pw->preedit.offset_x;
    }
    if (pw->preedit.style != XIMIsInvisible 
	&& pw->preedit.wcs_len == pw->preedit.caret_position) {
	pw->preedit.preedit_width += pw->preedit.caret_width;
    }
    if (pw->preedit.preedit_width == 0) {
	pw->preedit.preedit_width = 1;
    }
}

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
    Widget request, new;
    Arg *args;
    Cardinal *num_args;
{
    int i;
    Dimension spot_x, spot_y;
    PreeditWidget pw = (PreeditWidget)new;
    
    pw->preedit.specified = 0;
    pw->preedit.wcs = (wchar_t *)NULL;
    pw->preedit.feedback = (XIMFeedback *)NULL;
    pw->preedit.wcs_len = 0;
    pw->preedit.caret_position = 0;
    pw->preedit.style = XIMIsInvisible;
    
    GetnormalGC(pw);
    GetreverseGC(pw);
    SetWidthAndHeight(pw);
    if (pw->core.width == 0) {
	pw->core.width = pw->preedit.preedit_width +
	2 * pw->core.border_width;
    } else {
	pw->preedit.specified |= WIDTH_SPECIFIED;
    }
    if (pw->core.height == 0) {
	pw->core.height = pw->preedit.preedit_height +
	2 * pw->core.border_width;
    } else {
	pw->preedit.specified |= HEIGHT_SPECIFIED;
    }
    for (i = 0; i < *num_args; i++) {
	if (streq(XtNspotX, args[i].name)) {
	    spot_x = (Dimension)args[i].value;
	    pw->core.x = spot_x - pw->preedit.offset_x;
	}
	if (streq(XtNspotY, args[i].name)) {
	    spot_y = (Dimension)args[i].value;
	    pw->core.y = spot_y - pw->preedit.offset_y;
	}
    }
} /* Initialize */

static void
Realize(w, valueMask, attributes)
    register Widget w;
    Mask *valueMask;
    XSetWindowAttributes *attributes;
{
    PreeditWidget pw = (PreeditWidget)w;
    
    if (pw->preedit.cursor != None) {
	*valueMask |= CWCursor;
	attributes->cursor = pw->preedit.cursor;
    }
    XtCreateWindow(w, (unsigned int)InputOutput, (Visual *)CopyFromParent,
		   *valueMask, attributes);
}


/* ARGSUSED */
static void
Destroy(w)
    Widget w;
{
    PreeditWidget pw = (PreeditWidget)w;
    
#ifdef DEBUG
    printf("Preedit: destroy\n");
#endif
    XtReleaseGC(w, pw->preedit.normal_GC);
}

static void
redraw(w, l)
    PreeditWidget w;
    int l;
{
#ifdef DEBUG
    printf("Preedit: expose\n");
#endif
    if (!w->preedit.feedback && w->preedit.style == XIMIsInvisible) {
	XwcDrawImageString(XtDisplay(w), XtWindow(w),
			   w->preedit.font_set, w->preedit.normal_GC,
			   w->preedit.offset_x, w->preedit.offset_y,
			   w->preedit.wcs,      l);
    } else {
	XIMFeedback attr, *feedback;
	int m, n;
	Position x, y;
	int dx;
	GC gc;
	
	feedback = w->preedit.feedback;
	x = w->preedit.offset_x;
	y = w->preedit.offset_y;
	attr = feedback?feedback[0]:0;
	m = n = 0;
	for (m = 0; m < l ; ) {
	    if (feedback) {
		if (m == w->preedit.caret_position) {
		    m++;
		} else {
	            for (; m < l
			 && attr == feedback[m]
			 && (m != w->preedit.caret_position);
			 m++){
			;
		    }
		}
	    } else {
		if (m < w->preedit.caret_position) {
		    m = w->preedit.caret_position;
		} else if (m == w->preedit.caret_position) {
		    m++;
		} else {
		    m = l;
		}
	    }
	    if (w->preedit.style != XIMIsInvisible &&
		n == w->preedit.caret_position) {
		attr |= XIMReverse;
	    }
	    if (attr & XIMReverse) {
		gc = w->preedit.reverse_GC;
	    } else {
		gc = w->preedit.normal_GC;
	    }
	    XwcDrawImageString(XtDisplay(w), XtWindow(w),
			       w->preedit.font_set, gc,
			       x, y, &w->preedit.wcs[n],  m - n);
	    
	    dx = XwcTextEscapement(w->preedit.font_set,
				   &w->preedit.wcs[n],
				   m - n);
	    if (attr & XIMUnderline) {
		XDrawLine(XtDisplay(w), XtWindow(w), gc,
			  x, y+1, x+dx, y+1);
	    }
	    x += dx;
	    attr = feedback?feedback[m]:0;
	    n = m;
	}
	if (l == w->preedit.caret_position &&
	    w->preedit.style != XIMIsInvisible) {
	    XFillRectangle(XtDisplay(w), XtWindow(w),
			   w->preedit.normal_GC, x, 0,
			   x + w->preedit.caret_width,
			   w->preedit.preedit_height);
	}
    }
}

static int
QueryWidth(w, s, l)
    PreeditWidget w;
    wchar_t *s;
    int l;
{
    XtGeometryResult result;
    Dimension width, height;
    Dimension width_return, height_return;
    Bool caret_flag;
    
    caret_flag = FALSE;
    w->preedit.wcs = s;
    w->preedit.wcs_len = l;
    SetWidthAndHeight(w);
    if ((w->preedit.specified & WIDTH_SPECIFIED) && (w->core.width != 0)) {
	width = w->core.width;
    } else {
	width = w->preedit.preedit_width;
    }
    if ((w->preedit.specified & HEIGHT_SPECIFIED) && (w->core.height != 0)) {
	height = w->core.height;
    } else {
	height = w->preedit.preedit_height;
    }
    if (height == 0) {
	height = 1;
    }
    result = XtMakeResizeRequest(w, width, height,
				 &width_return, &height_return);
    if (result == XtGeometryAlmost) {
	if (width != width_return) {
	    if (w->preedit.wcs_len == w->preedit.caret_position &&
	        w->preedit.style != XIMIsInvisible) {
		w->preedit.style = XIMIsInvisible;
		SetWidthAndHeight(w);
		if (w->preedit.preedit_width <= width_return) {
		    caret_flag = TRUE;
		}
	    }
	    while (w->preedit.preedit_width > width_return) {
		(w->preedit.wcs_len)--;
		SetWidthAndHeight(w);
	    }
	}
	width = w->preedit.preedit_width; height = height_return;
	(void)XtMakeResizeRequest(w, width, height,
				  &width_return, &height_return);
    }
    if (caret_flag) {
	return(-1);
    } else {
        return (w->preedit.wcs_len);
    }
}


static void
Draw(w, s, feedback, l)
    PreeditWidget w;
    wchar_t *s;
    XIMFeedback *feedback;
    int l;
{
    Bool changed;
    
    changed = FALSE;
    if (s) {
	w->preedit.wcs = s;
	w->preedit.wcs_len = l;
    }
    if (feedback) {
	if (!s) {
	    changed = TRUE;
	}
	if (l > w->preedit.wcs_len) {
	    l = w->preedit.wcs_len;
	}
	w->preedit.feedback = feedback;
    }
    if (XtIsRealized(w)) {
#ifdef DEBUG
	printf("Preedit: ClearArea\n");
#endif
	XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, TRUE);
    }
    return;
}

static void
Caret(w, position, style)
    PreeditWidget w;
    int position;
    XIMCaretStyle style;
{
#ifdef DEBUG
    printf("Preedit: caret position = %d, %d\n", position, style);
#endif
    w->preedit.caret_position = position;
    w->preedit.style = style;
}

/* ARGSUSED */
static void
Redisplay(w, event, region)
    Widget w;
    XEvent *event;
    Region region;
{
    PreeditWidget pw = (PreeditWidget) w;
    
    XFillRectangle(XtDisplay(pw), XtWindow(pw),
		   pw->preedit.reverse_GC, 0, 0,
		   pw->core.width,
		   pw->core.height);
    redraw(w, pw->preedit.wcs_len);
}

static Boolean
SetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
    PreeditWidget curpw = (PreeditWidget)current;
    PreeditWidget reqpw = (PreeditWidget)request;
    PreeditWidget newpw = (PreeditWidget)new;
    int i;
    Boolean resized = FALSE;
    Boolean redisplay = FALSE;
    Dimension spot_x, spot_y;
    
    for (i = 0; i < *num_args; i++) {
	if (streq(XtNwidth, args[i].name) && newpw->core.width != 0) {
	    curpw->preedit.specified |= WIDTH_SPECIFIED;
	}
	if (streq(XtNheight, args[i].name) && newpw->core.height != 0) {
	    curpw->preedit.specified |= HEIGHT_SPECIFIED;
	}
	if (streq(XtNspotX, args[i].name)) {
	    spot_x = (Dimension)args[i].value;
	    newpw->core.x = spot_x - newpw->preedit.offset_x;
	}
	if (streq(XtNspotY, args[i].name)) {
	    spot_y = (Dimension)args[i].value;
	    newpw->core.y = spot_y - newpw->preedit.offset_y;
	}
    }
    if (curpw->preedit.font_set != newpw->preedit.font_set) {
	SetWidthAndHeight(newpw);
	resized = True;
    }
    
    if (resized ||
        newpw->core.width != curpw->core.width ||
        newpw->core.height != curpw->core.height) {
	if (!(newpw->preedit.specified & WIDTH_SPECIFIED) ||
            newpw->core.width == 0) {
	    newpw->preedit.specified &= ~WIDTH_SPECIFIED;
	    newpw->core.width = newpw->preedit.preedit_width;
	}
	if (!(newpw->preedit.specified & HEIGHT_SPECIFIED) ||
	    newpw->core.height == 0) {
	    newpw->preedit.specified &= ~HEIGHT_SPECIFIED;
	    newpw->core.height = newpw->preedit.preedit_height;
	}
	resized = True;
    }
    if (curpw->preedit.foreground != newpw->preedit.foreground) {
	XtReleaseGC(new, curpw->preedit.normal_GC);
	GetnormalGC(newpw);
	XtReleaseGC(new, curpw->preedit.reverse_GC);
	GetreverseGC(newpw);
	redisplay = True;
    }
#ifdef DEBUG
    printf("preedit x=%d, y=%d\n", newpw->core.x, newpw->core.y);
#endif
    return(resized || redisplay);
}

static void
SetValuesAlmost(w, new, request, reply)
    Widget w;
    Widget new;
    XtWidgetGeometry *request;
    XtWidgetGeometry *reply;
{
    *request = *reply;
}

static XtGeometryResult
QueryGeometry(w, intended, preferred)
    Widget w;
    XtWidgetGeometry *intended, *preferred;
{
    PreeditWidget pw = (PreeditWidget)w;
    
    preferred->request_mode = CWWidth | CWHeight;
    preferred->width = pw->preedit.preedit_width;
    preferred->height = pw->preedit.preedit_height;
    if (((intended->request_mode & (CWWidth | CWHeight))
	 == (CWWidth | CWHeight)) &&
	intended->width == preferred->width &&
	intended->height == preferred->height) {
	return(XtGeometryYes);
    } else {
	return(XtGeometryAlmost);
    }
}

/*
 *
 */

void
PreeditDraw(w, s, feedback, l)
    PreeditWidget w;
    wchar_t *s;
    XIMFeedback *feedback;
    int l;
{
    PreeditWidgetClass class = (PreeditWidgetClass)w->core.widget_class;
    
    (*class->preedit_class.Draw)(w, s, feedback, l);
}

int
PreeditQueryWidth(w, s, l)
    PreeditWidget w;
    wchar_t *s;
    int l;
{
    PreeditWidgetClass class = (PreeditWidgetClass)w->core.widget_class;
    
    return((*class->preedit_class.QueryWidth)(w, s, l));
}

void
PreeditCaret(w, position, style)
    PreeditWidget w;
    int position;
    XIMCaretStyle style;
{
    PreeditWidgetClass class = (PreeditWidgetClass)w->core.widget_class;
#ifdef DEBUG
    printf("Preedit: Caret=%d,%d\n", position, style);
#endif
    
    (*class->preedit_class.Caret)(w, position, style);
}
