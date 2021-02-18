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
static char     sccsid[] = "@(#)Focus.c	1.2";
#endif
/*
 * Focus.c -
 * 
 * @(#)Focus.c	1.2 91/09/28 17:11:59
 */
#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	<X11/Xaw/Label.h>
#include	"FocusP.h"
#include	"Conversion.h"
#include	"Preedit.h"


#define AREAX	0x0001
#define AREAY	0x0002
#define AREAW	0x0004
#define AREAH	0x0008
#define CMAP	0x0010
#define FG	0x0020
#define BG	0x0040
#define BGPIX	0x0080
#define FONT	0x0100
#define LSPC	0x0200
#define CURSOR	0x0400
#define NEEDW   0x0800
#define NEEDH   0x1000

static char     defaultTranslation[] =
	"<Key>:       Key()\n\
	 <Destroy>:   Destroy()\n\
	 <Configure>: Configure()";

/****************************************************************
 *
 * Focus Resources
 *
 ****************************************************************/

static XtResource resources[] = {
	{XtNwindow, XtCWindow, XtRWindow, sizeof(Window),
		XtOffsetOf(FocusWindowRec, focusWindow.window),
	XtRImmediate, NULL},
	{XtNconvObject, XtCConvObject, XtRWidget, sizeof(Widget),
		XtOffsetOf(FocusWindowRec, focusWindow.conv_object),
	XtRImmediate, NULL},
	{XtNpreeditActive, XtCPreeditActive, XtRBoolean, sizeof(Boolean),
		XtOffsetOf(FocusWindowRec, focusWindow.preedit_active),
	XtRImmediate, FALSE},
	{XtNinputActive, XtCInputActive, XtRBoolean, sizeof(Boolean),
		XtOffsetOf(FocusWindowRec, focusWindow.input_active),
	XtRImmediate, FALSE},

	{XtNpreeditAreaX, XtCX, XtRPosition, sizeof(Position),
		XtOffsetOf(FocusWindowRec, focusWindow.preedit.area.x),
	XtRImmediate, 0},
	{XtNpreeditAreaY, XtCY, XtRPosition, sizeof(Position),
		XtOffsetOf(FocusWindowRec, focusWindow.preedit.area.y),
	XtRImmediate, 0},
	{XtNpreeditAreaWidth, XtCWidth, XtRDimension, sizeof(Dimension),
		XtOffsetOf(FocusWindowRec, focusWindow.preedit.area.width),
	XtRImmediate, 0},
	{XtNpreeditAreaHeight, XtCHeight, XtRDimension, sizeof(Dimension),
		XtOffsetOf(FocusWindowRec, focusWindow.preedit.area.height),
	XtRImmediate, 0},

	{XtNpreeditForeground, XtCForeground, XtRPixel, sizeof(Pixel),
		XtOffsetOf(FocusWindowRec, focusWindow.preedit.foreground),
	XtRImmediate, (XPointer)XtDefaultForeground},

	{XtNpreeditBackground, XtCBackground, XtRPixel, sizeof(Pixel),
		XtOffsetOf(FocusWindowRec, focusWindow.preedit.background),
	XtRImmediate, (XPointer)XtDefaultBackground},

	{XtNpreeditColormap, XtCColormap, XtRColormap, sizeof(Colormap),
		XtOffsetOf(FocusWindowRec, focusWindow.preedit.colormap),
	XtRImmediate, 0},

	{XtNpreeditBgPixmap, XtCPixmap, XtRPixmap, sizeof(Pixmap),
		XtOffsetOf(FocusWindowRec, focusWindow.preedit.background_pixmap),
	XtRImmediate, (XPointer)None},

	{XtNpreeditLineSpacing, XtCSpace, XtRInt, sizeof(int),
		XtOffsetOf(FocusWindowRec, focusWindow.preedit.line_spacing),
	XtRImmediate, (XPointer)16},

	{XtNpreeditCursor, XtCCursor, XtRCursor, sizeof(Cursor),
		XtOffsetOf(FocusWindowRec, focusWindow.preedit.cursor),
	XtRImmediate, (XPointer)None},

	{XtNpreeditNeededWidth, XtCWidth, XtRDimension, sizeof(Dimension),
		XtOffsetOf(FocusWindowRec, focusWindow.preedit.areaneeded.width),
	XtRImmediate, 0},

	{XtNpreeditNeededHeight, XtCHeight, XtRDimension, sizeof(Dimension),
		XtOffsetOf(FocusWindowRec, focusWindow.preedit.areaneeded.height),
	XtRImmediate, 0},

	{XtNpreeditSpotX, XtCX, XtRPosition, sizeof(Position),
		XtOffsetOf(FocusWindowRec, focusWindow.preedit.spot_location.x),
	XtRImmediate, 0},

	{XtNpreeditSpotY, XtCY, XtRPosition, sizeof(Position),
		XtOffsetOf(FocusWindowRec, focusWindow.preedit.spot_location.y),
	XtRImmediate, 0},

	{XtNpreeditFont, XtCFont, XtRString, sizeof(String),
		XtOffsetOf(FocusWindowRec, focusWindow.preedit_font),
	XtRString, DEFAULTFONT},
};

static void     key();
static void     destroy_handler();
static void     configure_handler();

static XtActionsRec actionList[] = {
	{"Key", key},
	{"Destroy", destroy_handler},
	{"Configure", configure_handler},
};

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

static void     Initialize();
static void     ClassPartInitialize();
static void     Realize();
static void     Resize();
static void     Destroy();
static Boolean  SetValues();
static void     GetValuesHook();
static XtGeometryResult GeometryManager();
static void     ChangeManaged();

static void     Draw();
static void     Caret();

static CompositeClassExtensionRec extension_rec = {
	 /* next_extension */ NULL,
	 /* record_type */ NULLQUARK,
	 /* version */ XtCompositeExtensionVersion,
	 /* record_size */ sizeof(CompositeClassExtensionRec),
	 /* accepts_objects */ TRUE,
};

FocusWindowClassRec focusWindowClassRec = {
	{
		/* core_class fields      */
		 /* superclass         */ (WidgetClass) & compositeClassRec,
		 /* class_name         */ "FocusWindow",
		 /* widget_size        */ sizeof(FocusWindowRec),
		 /* class_initialize   */ NULL,
		 /* class_part_init    */ ClassPartInitialize,
		 /* class_inited       */ FALSE,
		 /* initialize         */ Initialize,
		 /* initialize_hook    */ NULL,
		 /* realize            */ Realize,
		 /* actions            */ actionList,
		 /* num_actions	       */ XtNumber(actionList),
		 /* resources          */ resources,
		 /* num_resources      */ XtNumber(resources),
		 /* xrm_class          */ NULLQUARK,
		 /* compress_motion    */ TRUE,
		 /* compress_exposure  */ TRUE,
		 /* compress_enterleave */ TRUE,
		 /* visible_interest   */ FALSE,
		 /* destroy            */ Destroy,
		 /* resize             */ NULL,
		 /* expose             */ NULL,
		 /* set_values         */ SetValues,
		 /* set_values_hook    */ NULL,
		 /* set_values_almost  */ XtInheritSetValuesAlmost,
		 /* get_values_hook    */ GetValuesHook,
		 /* accept_focus       */ NULL,
		 /* version            */ XtVersion,
		 /* callback_private   */ NULL,
		 /* tm_table           */ defaultTranslation,
		 /* query_geometry     */ NULL,
		 /* display_accelerator */ XtInheritDisplayAccelerator,
		 /* extension          */ NULL
	}, {
		/* composite_class fields */
		 /* geometry_manager   */ GeometryManager,
		 /* change_managed     */ ChangeManaged,
		 /* insert_child	  */ XtInheritInsertChild,
		 /* delete_child	  */ XtInheritDeleteChild,
		 /* extension          */ NULL
	}, {
		/* FocusWindow class fields */
		 /* empty		  */ Draw,
		 /* empty		  */ Caret,
	}
};

WidgetClass     focusWindowWidgetClass = (WidgetClass) & focusWindowClassRec;


/****************************************************************
 *
 * Private Routines
 *
 ****************************************************************/

static Widget   CreatePreedit();

static void 
Resize(w)
    Widget          w;
{
}				/* Resize */

/* ARGSUSED */
static XtGeometryResult 
GeometryManager(w, request, reply)
    Widget          w;
    XtWidgetGeometry *request;
    XtWidgetGeometry *reply;/* RETURN */
    
{
    FocusWindowWidget fw;
    Bool almost;
    Position  request_x, request_y;
    Dimension request_width, request_height;
    Dimension request_border_width;
    Position  area_x, area_y;
    Dimension area_width, area_height;
    
    reply->request_mode = 0;
    almost = FALSE;
    request_x = (request->request_mode & CWX)? request->x:w->core.x;
    request_y = (request->request_mode & CWY)? request->y:w->core.y;
    request_width = (request->request_mode & CWWidth)?
		     request->width:w->core.width;
    request_height = (request->request_mode & CWHeight)?
		     request->height:w->core.height;
    request_border_width = (request->request_mode & CWBorderWidth)?
			   request->border_width:w->core.border_width;
    fw = (FocusWindowWidget)XtParent(w);
    
    if (w == fw->focusWindow.preedit_widget1 ||
        w == fw->focusWindow.preedit_widget2) {
	area_x      = fw->focusWindow.preedit.area.x;
	area_y      = fw->focusWindow.preedit.area.y;
	area_width  = fw->focusWindow.preedit.area.width;
	area_height = fw->focusWindow.preedit.area.height;
    } else {
	area_x      = 0;
	area_y      = 0;
	area_width  = fw->core.width;
	area_height = fw->core.height;
    }
    reply->x      = request_x;
    reply->y      = request_y;
    reply->width  = request_width;
    reply->height = request_height;
    reply->border_width = request_border_width;
    if (request_x < area_x) {
#ifdef DEBUG
	printf("Geometry manager: x req = %d,reply =%d\n", request_x, area_x);
#endif
	reply->x = request_x = area_x;
	reply->request_mode |= CWX;
	almost = TRUE;
    }
    if (request_y  < area_y) {
#ifdef DEBUG
	printf("Geometry manager: y req = %d,reply =%d\n", request_y, area_y);
#endif
	reply->y = request_y = area_y;
	reply->request_mode |= CWY;
	almost = TRUE;
    }
    if (area_x + area_width <
        request_x + request_width + 2 * request_border_width) {
	int width;

	width = area_x + area_width - (request_x + 2 * request_border_width);
        if (width < 0) {
	    return (XtGeometryNo);
	}
	reply->width = width;
	reply->request_mode |= CWWidth;
#ifdef DEBUG
	printf("Geometry manager: req= %d,almost width =%d\n", request_width, reply->width);
#endif
	almost = TRUE;
    } 
    if (!(request->request_mode & XtCWQueryOnly) && !almost) {
	if (request->request_mode & CWX) {
	    w->core.x = request->x;
#ifdef DEBUG
	    printf("Geometry manager: accepted x= %d\n", request->x);
#endif
	}
	if (request->request_mode & CWY) {
	    w->core.y = request->y;
	}
	if (request->request_mode & CWWidth) {
#ifdef DEBUG
	    printf("Geometry manager: accepted w= %d\n", request->width);
#endif
	    w->core.width = request->width;
	}
	if (request->request_mode & CWHeight) {
	    w->core.height = request->height;
	}
	if (request->request_mode & CWBorderWidth) {
	    w->core.border_width = request->border_width;
	}
    }
    if (almost) {
	reply->request_mode |= request->request_mode;
	return (XtGeometryAlmost);
    } else {
	return (XtGeometryYes);
    }
}

/* ARGSUSED */
static void 
ChangeManaged(w)
    Widget          w;
{
}

/* ARGSUSED */
static void
ClassPartInitialize(wc)
    WidgetClass     wc;
{
    FocusWindowWidgetClass fwc, sc;
    
    fwc = (FocusWindowWidgetClass) wc;
    sc = (FocusWindowWidgetClass) fwc->core_class.superclass;
    
    if (fwc->focusWindow_class.focus_draw == XtInheritDraw) {
	fwc->focusWindow_class.focus_draw = sc->focusWindow_class.focus_draw;
    }
    if (fwc->focusWindow_class.focus_caret == XtInheritCaret) {
	fwc->focusWindow_class.focus_caret = sc->focusWindow_class.focus_caret;
    }
    extension_rec.next_extension = fwc->composite_class.extension;
    fwc->composite_class.extension = (caddr_t) & extension_rec;
}

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
    Widget          request;
    Widget          new;
    ArgList         args;
    Cardinal       *num_args;
{
    FocusWindowWidget imnew;
    CorePart       *core;
    FocusWindowPart *focus;
    XWindowAttributes WinAttr;
    Window          root;
    int             x, y;
    unsigned int    width, height;
    unsigned int    border_width;
    unsigned int    depth;
    unsigned long   mask;
    Dimension lspc;
    PreeditInfo *fp;
    int i;
    Arg largs[2];
    int ac;
    
    imnew = (FocusWindowWidget) new;
    core = &imnew->core;
    focus = &imnew->focusWindow;
    fp = &focus->preedit;
    
    XGetWindowAttributes(XtDisplay(imnew), imnew->focusWindow.window, &WinAttr);
    
    core->x = (Position) WinAttr.x;
    core->y = (Position) WinAttr.y;
    core->width = (Dimension) WinAttr.width;
    core->height = (Dimension) WinAttr.height;
    core->border_width = (Dimension) WinAttr.border_width;
    core->depth = (Cardinal) WinAttr.depth;
    core->colormap = (Colormap) WinAttr.colormap;
    
    focus->mask = 0L;
    focus->preedit_text = NULL;
#define streq(s, t) (*(s) == *(t) && !strcmp((s), (t)))
    mask = 0L;
    for (i=0; i < *num_args; i++) {
	if (streq(args[i].name, XtNpreeditAreaX)) {
	    mask |= AREAX;
	}
	if (streq(args[i].name, XtNpreeditAreaY)) {
	    mask |= AREAY;
	}
	if (streq(args[i].name, XtNpreeditAreaWidth)) {
	    mask |= AREAW;
	}
	if (streq(args[i].name, XtNpreeditAreaHeight)) {
	    mask |= AREAH;
	}
	if (streq(args[i].name, XtNpreeditColormap)) {
	    mask |= CMAP;
	}
	if (streq(args[i].name, XtNpreeditLineSpacing)) {
	    mask |= LSPC;
	}
	if (streq(args[i].name, XtNpreeditNeededWidth)) {
	    mask |= NEEDW;
	}
	if (streq(args[i].name, XtNpreeditNeededHeight)) {
	    mask |= NEEDH;
	}
    }
    if (!(mask & (AREAX | AREAY | AREAH | AREAW))) {
	fp->area.x = 0;
	fp->area.y = 0;
	fp->area.width = core->width;
	fp->area.height = core->height;
    } else {
	if (fp->area.width == 0) {
	    fp->area.width = core->width;
	}
	if (fp->area.height == 0) {
	    fp->area.height = core->height;
	}
    }

    if (!(mask & CMAP)) {
	fp->colormap = core->colormap;
    }
    
    focus->preedit_widget1 = CreatePreedit(imnew, "preedit1");
    focus->preedit_widget2 = CreatePreedit(imnew, "preedit2");
    
    focus->wcs_len1 = 0;
    focus->wcs_len2 = 0;
    if (!(mask & LSPC)) {
	ac = 0;
	XtSetArg(largs[ac], XtNheight, &lspc); ac++;
        XtGetValues(focus->preedit_widget1, largs, ac);
	fp->line_spacing = lspc;
    }
    focus->position = 0;
    focus->style = XIMIsInvisible;
}

static void
Destroy(w)
    Widget w;
{
    FocusWindowWidget fw = (FocusWindowWidget)w;
#ifdef DEBUG
    printf("Focus Destoroy\n");
#endif
    XtDestroyWidget(fw->focusWindow.preedit_widget1);
    XtDestroyWidget(fw->focusWindow.preedit_widget2);
#ifdef DEBUG
    printf("Focus Destoroy end\n");
#endif
}

/* ARGSUSED */
static void 
Realize(w, valueMask, attributes)
    Widget          w;
    Mask           *valueMask;
    XSetWindowAttributes *attributes;
{
    FocusWindowWidget imw;
    
    imw = (FocusWindowWidget) w;
    imw->core.window = imw->focusWindow.window;
    imw->focusWindow.mask |= StructureNotifyMask;
    if (imw->focusWindow.preedit_active) {
	XtMapWidget(imw->focusWindow.preedit_widget1);
    }
    if (imw->focusWindow.input_active) {
	imw->focusWindow.mask |= KeyPressMask;
    }
    XSelectInput(XtDisplay(imw), imw->focusWindow.window, imw->focusWindow.mask);
    XFlush(XtDisplay(imw));
}

/* ARGSUSED */
static          Boolean
SetValues(current, request, new, args, num_args)
    Widget          current, request, new;
    Arg *args;
    Cardinal *num_args;
{
    FocusWindowWidget imnew = (FocusWindowWidget) new;
    FocusWindowWidget imcur = (FocusWindowWidget) current;
    int             ac;
    Arg             largs[10];
    Bool            geom_changed;
    Bool            redisplay;
    PreeditInfo     *fp;
    
    geom_changed = FALSE;
    redisplay = FALSE;
    fp = &imnew->focusWindow.preedit;

    if (imnew->focusWindow.preedit_active) {
	XtMapWidget(imnew->focusWindow.preedit_widget1);
    } else {
	XtUnmapWidget(imnew->focusWindow.preedit_widget1);
	XtUnmapWidget(imnew->focusWindow.preedit_widget2);
    }
    if (imnew->focusWindow.input_active) {
	imnew->focusWindow.mask |= KeyPressMask;
    } else {
	imnew->focusWindow.mask &= ~KeyPressMask;
    }
    if (imnew->focusWindow.mask != imcur->focusWindow.mask) {
	XSelectInput(XtDisplay(imnew), imnew->focusWindow.window,
		     imnew->focusWindow.mask);
	XFlush(XtDisplay(imnew));
    }
#define NEQ(a, b, c) (a->focusWindow.preedit.c  != b->focusWindow.preedit.c)
    ac = 0;
    if (NEQ(imnew, imcur, colormap)) {
	XtSetArg(largs[ac], XtNcolormap, fp->colormap);
	ac++;
	redisplay = TRUE;
    }
    if (NEQ(imnew, imcur, foreground)) {
	XtSetArg(largs[ac], XtNforeground, fp->foreground);
	ac++;
	redisplay = TRUE;
    }
    if (NEQ(imnew, imcur, background)) {
	XtSetArg(largs[ac], XtNbackground, fp->background);
	ac++;
	redisplay = TRUE;
    }
    if (NEQ(imnew, imcur, background_pixmap)) {
	XtSetArg(largs[ac], XtNbackgroundPixmap, fp->background_pixmap);
	ac++;
	redisplay = TRUE;
    }
    if (NEQ(imnew, imcur, cursor)) {
	XtSetArg(largs[ac], XtNcursor, fp->cursor);
	ac++;
    }
    if (imnew->focusWindow.preedit_font != imcur->focusWindow.preedit_font) {
	XrmValue from, to;
	XFontSet fs;

	if (*imnew->focusWindow.preedit_font != '\0') {
	    from.addr = imnew->focusWindow.preedit_font;
	    from.size  = strlen(from.addr);
	    XtConvert(imnew, XtRString, &from, XtRFontSet, &to);
	    if (to.addr != NULL) {
	        fs = *(XFontSet *)to.addr;
	        XtSetArg(largs[ac], XtNfontSet, fs); ac++;
	        geom_changed = TRUE;
	    }
	}
    }
    if (NEQ(imnew, imcur, spot_location.x) ||
	NEQ(imnew, imcur, spot_location.y)) {
	
	XtSetArg(largs[ac], XtNspotX, fp->spot_location.x); ac++;
	XtSetArg(largs[ac], XtNspotY, fp->spot_location.y); ac++;
#ifdef DEBUG
	printf("Focus Setvalues: spot location\n");
#endif
	geom_changed = TRUE;
    }
    if (ac > 0) {
	XtSetValues(imnew->focusWindow.preedit_widget1, largs, ac);
	XtSetValues(imnew->focusWindow.preedit_widget2, largs, ac);
    }
    if (fp->area.width == 0) {
        fp->area.width = fp->area.width;
    }
    if (fp->area.height == 0) {
        fp->area.height = fp->area.height;
    }
    if (geom_changed) {
	XIMText *s;

        s = imnew->focusWindow.preedit_text;
	if (s) {
	    FocusPreeditDraw(imnew, s, 0);
	}
    }
    return (redisplay);
}

/* ARGSUSED */
static void
GetValuesHook(w, args, num_args)
    Widget w;
    Arg *args;
    Cardinal *num_args;
{
    int i;
    FocusWindowWidget fw = (FocusWindowWidget)w;
	
    for (i = 0; i < *num_args; i++) {
	if (streq(args[i].name, XtNpreeditInfo)) {
	    bcopy((char *)&fw->focusWindow.preedit,
		  (char *)args[i].value,
		  sizeof(PreeditInfo));
	}
    }
}

static          Widget
CreatePreedit(w, name)
    FocusWindowWidget w;
    String name;
{
    int             ac;
    Arg             args[20];
    Widget          widget;
    XFontSet        fs;
    XrmValue        from, toval;
    
    ac = 0;
    XtSetArg(args[ac], XtNlabel, ""); ac++;
    XtSetArg(args[ac], XtNborderWidth, 0); ac++;
    XtSetArg(args[ac], XtNmappedWhenManaged, FALSE); ac++;
    
    XtSetArg(args[ac], XtNcolormap, w->focusWindow.preedit.colormap); ac++;
    XtSetArg(args[ac], XtNforeground, w->focusWindow.preedit.foreground); ac++;
    XtSetArg(args[ac], XtNbackground, w->focusWindow.preedit.background); ac++;
    XtSetArg(args[ac], XtNbackgroundPixmap, w->focusWindow.preedit.background_pixmap); ac++;
    if (*w->focusWindow.preedit_font != '\0') {
        from.addr = w->focusWindow.preedit_font;
        from.size  = strlen(from.addr);
        XtConvert(w, XtRString, &from, XtRFontSet, &toval);
        if (toval.addr != NULL) {
	    fs = *(XFontSet *)toval.addr;
	    XtSetArg(args[ac], XtNfontSet, fs); ac++;
        }
    }
    XtSetArg(args[ac], XtNcursor, w->focusWindow.preedit.cursor); ac++;
    XtSetArg(args[ac], XtNspotX, w->focusWindow.preedit.spot_location.x); ac++;
    XtSetArg(args[ac], XtNspotY, w->focusWindow.preedit.spot_location.y); ac++;
    widget = XtCreateManagedWidget(name, preeditWidgetClass, w, args, ac);
    return (widget);
}

static void
Draw(w, s, cp)
    FocusWindowWidget w;
    XIMText        *s;
    int            cp;
{
    int rc;
    int ac;
    Arg args[10];
    Position y;
    int n;
    Boolean changed;
    XIMFeedback *feedback;
    int position;
    XIMCaretStyle style;
    Bool caret_flag;
    
    if (s) {
	changed = s != w->focusWindow.preedit_text;
        w->focusWindow.preedit_text = s;
    } else {
	changed = TRUE;
	s = w->focusWindow.preedit_text;
    }
    
    position = w->focusWindow.position;
    style = w->focusWindow.style;

#ifdef DEBUG
    printf("Focus: caret=%d, %d\n", position, style);
#endif
    PreeditCaret(w->focusWindow.preedit_widget1, position, style);

    rc = PreeditQueryWidth(w->focusWindow.preedit_widget1,
		           s->string.wide_char,
		           s->length);
    n = 1;
    if (rc == -1) {	
	rc = w->focusWindow.wcs_len1 = s->length;
	caret_flag = TRUE;
    } else {
	w->focusWindow.wcs_len1 = rc;
	caret_flag = FALSE;
    }
    if (rc < s->length || caret_flag) {
        ac = 0;
	XtSetArg(args[ac], XtNy, &y); ac++;
	XtGetValues(w->focusWindow.preedit_widget1, args, ac);
        y += w->focusWindow.preedit.line_spacing;
        ac = 0;
        XtSetArg(args[ac], XtNx, 0); ac++;
        XtSetArg(args[ac], XtNy, y); ac++;
	XtSetValues(w->focusWindow.preedit_widget2, args, ac);
        rc = PreeditQueryWidth(w->focusWindow.preedit_widget2,
		               &s->string.wide_char[rc],
		               s->length - rc);
	if (rc == -1) {
	    rc = s->length - w->focusWindow.wcs_len1;
	}
	n++;
	w->focusWindow.wcs_len2 = rc;
    }

    if (position <= w->focusWindow.wcs_len1) {
	PreeditCaret(w->focusWindow.preedit_widget1, position, style);
    } else {
	position -= w->focusWindow.wcs_len1;
	PreeditCaret(w->focusWindow.preedit_widget2, position, style);
    }
    feedback = s->feedback;
    if (changed || cp < w->focusWindow.wcs_len1) {
        PreeditDraw(w->focusWindow.preedit_widget1,
		    s->string.wide_char,
	    	    feedback,
		    w->focusWindow.wcs_len1);
    }
    if (n > 1) {
       	if (changed || cp < w->focusWindow.wcs_len2) {
            PreeditDraw(w->focusWindow.preedit_widget2,
		        &s->string.wide_char[w->focusWindow.wcs_len1],
		        feedback?&feedback[w->focusWindow.wcs_len1]:0,
		        w->focusWindow.wcs_len2);
	}
	XtMapWidget(w->focusWindow.preedit_widget2);
    } else {
	XtUnmapWidget(w->focusWindow.preedit_widget2);
    }
}

static void
Caret(w, position, style)
    FocusWindowWidget          w;
    int position;
    XIMCaretStyle style;
{
    w->focusWindow.position = position;
    w->focusWindow.style = style;
}

/****************************************************************
 *
 * Action Routines
 *
 ****************************************************************/

/* ARGSUSED */
static void
key(w, event, params, num_params)
    Widget          w;
    XEvent         *event;
    String         *params;
    Cardinal       *num_params;
{
    FocusWindowWidget imw = (FocusWindowWidget) w;
    
    if (imw->focusWindow.conv_object != (Widget) NULL) {
	XimpKey(imw->focusWindow.conv_object, event);
    }
}

/* ARGSUSED */
static void
destroy_handler(w, event, params, num_params)
    Widget          w;
    XEvent         *event;
    String         *params;
    Cardinal       *num_params;
{
    FocusWindowWidget fw = (FocusWindowWidget) w;
    
    if (event->type == DestroyNotify) {
#ifdef DEBUG
	printf("Focus: destroy handler\n");
#endif
	_XtUnregisterWindow(XtWindow(fw), fw);
	fw->core.window = NULL;
	XtDestroyWidget(fw);
#ifdef DEBUG
	printf("Focus: destroy handler end \n");
#endif
    }
}

/* ARGSUSED */
static void
configure_handler(w, event, params, num_params)
    Widget          w;
    XEvent         *event;
    String         *params;
    Cardinal       *num_params;
{
    FocusWindowWidget imw = (FocusWindowWidget) w;
    
    if (event->type == ConfigureNotify) {
	imw->core.x = (Position) event->xconfigure.x;
	imw->core.y = (Position) event->xconfigure.y;
	imw->core.width = (Dimension) event->xconfigure.width;
	imw->core.height = (Dimension) event->xconfigure.height;
	imw->core.border_width = (Dimension) event->xconfigure.border_width;
    }
}

/*
 */

void
FocusPreeditDraw(w, s, cp)
    Widget          w;
    XIMText        *s;
    int             cp;
{
    FocusWindowWidget imw = (FocusWindowWidget) w;
    FocusWindowWidgetClass class = (FocusWindowWidgetClass) imw->core.widget_class;
    
    if (!XtIsSubclass(imw, focusWindowWidgetClass)) {
	return;
    }
    (*class->focusWindow_class.focus_draw) (imw, s, cp);
}

void
FocusPreeditCaret(w, position, style)
    Widget          w;
    int position;
    XIMCaretStyle style;
{
    FocusWindowWidget imw = (FocusWindowWidget) w;
    FocusWindowWidgetClass class = (FocusWindowWidgetClass) imw->core.widget_class;
    
    if (!XtIsSubclass(imw, focusWindowWidgetClass)) {
	return;
    }
    (*class->focusWindow_class.focus_caret)(imw, position, style);
}
