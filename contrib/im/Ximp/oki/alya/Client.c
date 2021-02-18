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
static char sccsid[] = "@(#)Client.c	1.2";
#endif
/*
 * Client.c - 
 *
 * @(#)Client.c	1.2 91/09/28 17:11:42
 */
#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	"ClientP.h"
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

static char defaultTranslation[] = 
	"<Key>:    Key()\n\
	 <Destroy>: Destroy()\n\
	 <Configure>: Configure()";

/****************************************************************
 *
 * Client Resources
 *
 ****************************************************************/

static XtResource resources[] = {
    { XtNstatusActive, XtCStatusActive, XtRBoolean, sizeof(Boolean),
		XtOffsetOf(ClientWindowRec, clientWindow.status_active),
		XtRImmediate, FALSE },

    { XtNstatusAreaX, XtCX, XtRPosition, sizeof(Position),
		XtOffsetOf(ClientWindowRec, clientWindow.status.area.x),
		XtRImmediate, 0 },
    { XtNstatusAreaY, XtCY, XtRPosition, sizeof(Position),
		XtOffsetOf(ClientWindowRec, clientWindow.status.area.y),
		XtRImmediate, 0 },
    { XtNstatusAreaWidth, XtCWidth, XtRDimension, sizeof(Dimension), 
		XtOffsetOf(ClientWindowRec, clientWindow.status.area.width),
		XtRImmediate, 0 },
    { XtNstatusAreaHeight, XtCHeight, XtRDimension, sizeof(Dimension), 
		XtOffsetOf(ClientWindowRec, clientWindow.status.area.height),
		XtRImmediate, 0 },

    { XtNstatusForeground, XtCForeground, XtRPixel, sizeof(Pixel),
		XtOffsetOf(ClientWindowRec, clientWindow.status.foreground),
		XtRString, XtDefaultForeground },
    { XtNstatusBackground, XtCBackground, XtRPixel, sizeof(Pixel),
		XtOffsetOf(ClientWindowRec, clientWindow.status.background),
		XtRString, XtDefaultBackground },
    { XtNstatusColormap, XtCColormap, XtRColormap, sizeof(Colormap),
		XtOffsetOf(ClientWindowRec, clientWindow.status.colormap),
		XtRImmediate, 0 },
    { XtNstatusBgPixmap, XtCPixmap, XtRPixmap, sizeof(Pixmap),
		XtOffsetOf(ClientWindowRec, clientWindow.status.background_pixmap),
		XtRImmediate, (XtPointer)None },
    { XtNstatusLineSpacing, XtCSpace, XtRInt, sizeof(int),
		XtOffsetOf(ClientWindowRec, clientWindow.status.line_spacing),
		XtRImmediate, 0 },
    { XtNstatusCursor, XtCCursor, XtRCursor, sizeof(Cursor),
		XtOffsetOf(ClientWindowRec, clientWindow.status.cursor),
		XtRImmediate, (XtPointer)None },
    { XtNstatusNeededWidth, XtCWidth, XtRDimension, sizeof(Dimension), 
		XtOffsetOf(ClientWindowRec, clientWindow.status.areaneeded.width),
		XtRImmediate, 0 },
    { XtNstatusNeededHeight, XtCHeight, XtRDimension, sizeof(Dimension), 
		XtOffsetOf(ClientWindowRec, clientWindow.status.areaneeded.height),
		XtRImmediate, 0 },
    { XtNstatusFont, XtCFont, XtRString, sizeof(String),
		XtOffsetOf(ClientWindowRec, clientWindow.status_font),
		XtRString, DEFAULTFONT },

};

static void	destroy_handler();

static XtActionsRec actionList[] = {
	{"Destroy", destroy_handler},
};

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

static void Initialize();
static void ClassPartInitialize();
static void Destroy();
static Boolean SetValues();
static void GetValuesHook();
static XtGeometryResult GeometryManager();
static void ChangeManaged();

static void StatusDraw();

ClientWindowClassRec clientWindowClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &focusWindowClassRec,
    /* class_name         */    "ClientWindow",
    /* widget_size        */    sizeof(ClientWindowRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */	NULL,
    /* class_inited       */	FALSE,
    /* initialize         */    Initialize,
    /* initialize_hook    */	NULL,
    /* realize            */    XtInheritRealize,
    /* actions            */    actionList,
    /* num_actions	  */	XtNumber(actionList),
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/	TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    Destroy,
    /* resize             */    NULL,
    /* expose             */    NULL,
    /* set_values         */    SetValues,
    /* set_values_hook    */	NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	GetValuesHook,
    /* accept_focus       */    NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    defaultTranslation,
    /* query_geometry     */	NULL,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },{
/* composite_class fields */
    /* geometry_manager   */    XtInheritGeometryManager,
    /* change_managed     */    XtInheritChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
    /* extension          */	NULL
  },{
/* FocusWindow class fields */
    /* empty		  */	XtInheritDraw,
    /* empty		  */	XtInheritCaret,
  },{
/* ClientWindow class fields */
    /* empty		  */	StatusDraw,
  }
};

WidgetClass clientWindowWidgetClass = (WidgetClass)&clientWindowClassRec;


/****************************************************************
 *
 * Private Routines
 *
 ****************************************************************/

static Widget CreateStatus();

/* ARGSUSED */
static void
Initialize(request, new, args, num_args)
    Widget request;
    Widget new;
    ArgList args;
    Cardinal *num_args;
{
    ClientWindowWidget imnew;
    CorePart *core;
    ClientWindowPart *client;
    StatusInfo *cp;
    int i;
    unsigned long mask;

    imnew = (ClientWindowWidget)new;
    core = &imnew->core;
    client = &imnew->clientWindow;
    cp = &client->status;

    client->status_widget  = NULL;
    client->status_text = NULL;
#define streq(s, t) (*(s) == *(t) && !strcmp((s), (t)))
    mask = 0L;
    for (i=0; i < *num_args; i++) {
	if (streq(args[i].name, XtNstatusAreaX)) {
	    mask |= AREAX;
	}
	if (streq(args[i].name, XtNstatusAreaY)) {
	    mask |= AREAY;
	}
	if (streq(args[i].name, XtNstatusAreaWidth)) {
	    mask |= AREAW;
	}
	if (streq(args[i].name, XtNstatusAreaHeight)) {
	    mask |= AREAH;
	}
	if (streq(args[i].name, XtNstatusColormap)) {
	    mask |= CMAP;
	}
    }
    if (!(mask & (AREAX | AREAY | AREAW | AREAH))) {
	cp->area.x = 0;
	cp->area.y = 0;
	cp->area.width = core->width;
	cp->area.height = core->height;
    } else {
	if (cp->area.width == 0) {
	    cp->area.width = core->width;
	}
	if (cp->area.height == 0) {
	    cp->area.height = core->height;
	}
    }
    if (!(mask & CMAP)) {
	cp->colormap = core->colormap;
    }
    client->status_widget = CreateStatus(new);
}

/* ARGSUSED */
static Boolean
SetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
    ClientWindowWidget imnew = (ClientWindowWidget)new;
    ClientWindowWidget imcur = (ClientWindowWidget)current;
    int		     ac;
    Arg		     largs[20];
    Bool	     geom_changed;
    Bool	     redisplay;
    StatusInfo       *cp;

    cp = &imnew->clientWindow.status;
    geom_changed = FALSE;
    redisplay = FALSE;

    if (imnew->clientWindow.status_active) {
	XtMapWidget(imnew->clientWindow.status_widget);
    } else {
	XtUnmapWidget(imnew->clientWindow.status_widget);
    }
#define NEQ(a, b, c) (a->clientWindow.status.c  != b->clientWindow.status.c)
    ac = 0;
    if (NEQ(imnew, imcur, area.x)) {
	XtSetArg(largs[ac], XtNx, cp->area.x);
	ac++;
	geom_changed = TRUE;
    }
    if (NEQ(imnew, imcur, area.y)) {
	XtSetArg(largs[ac], XtNy, cp->area.y);
	ac++;
	geom_changed = TRUE;
    }
    if (NEQ(imnew, imcur, area.width)) {
	XtSetArg(largs[ac], XtNwidth, cp->area.width);
	ac++;
	geom_changed = TRUE;
    }
    if (NEQ(imnew, imcur, area.height)) {
	XtSetArg(largs[ac], XtNheight, cp->area.height);
	ac++;
	geom_changed = TRUE;
    }
    if (NEQ(imnew, imcur, colormap)) {
	XtSetArg(largs[ac], XtNcolormap, cp->colormap);
	ac++;
	redisplay = TRUE;
    }
    if (NEQ(imnew, imcur, foreground)) {
	XtSetArg(largs[ac], XtNforeground, cp->foreground);
	ac++;
	redisplay = TRUE;
    }
    if (NEQ(imnew, imcur, background)) {
	XtSetArg(largs[ac], XtNbackground, cp->background);
	ac++;
	redisplay = TRUE;
    }
    if (NEQ(imnew, imcur, background_pixmap)) {
	XtSetArg(largs[ac], XtNbackgroundPixmap, cp->background_pixmap);
	ac++;
	redisplay = TRUE;
    }
    if (NEQ(imnew, imcur, cursor)) {
	XtSetArg(largs[ac], XtNcursor, cp->cursor);
	ac++;
    }
    if (imnew->clientWindow.status_font != imcur->clientWindow.status_font) {
	XrmValue from, to;
	XFontSet fs;

	if (*imnew->clientWindow.status_font != '\0') {
	     from.addr = imnew->clientWindow.status_font;
	     from.size  = strlen(from.addr);
	     XtConvert(imnew, XtRString, &from, XtRFontSet, &to);
	     if (to.addr != NULL) {
	         fs = *(XFontSet *)to.addr;
	         XtSetArg(largs[ac], XtNfontSet, fs); ac++;
	     }
	     geom_changed = TRUE;
	}
    }
    if (ac > 0) {
	XtSetValues(imnew->clientWindow.status_widget, largs, ac);
    }
    if (cp->areaneeded.width == 0) {
	cp->areaneeded.width = cp->area.width;
    }
    if (cp->areaneeded.height == 0) {
	cp->areaneeded.height = cp->area.height;
    }
    if (NEQ(imnew, imcur, areaneeded.x)) {
	ac = 0;
    }
    if (NEQ(imnew, imcur, areaneeded.y)) {
	ac = 0;
    }
    return(redisplay || geom_changed);
}

/* ARGSUSED */
static void
GetValuesHook(w, args, num_args)
    Widget w;
    Arg *args;
    Cardinal *num_args;
{
    int i;
   ClientWindowWidget fw = (ClientWindowWidget)w;
	
    for (i = 0; i < *num_args; i++) {
	if (streq(args[i].name, XtNstatusInfo)) {
	    bcopy((char *)&fw->clientWindow.status,
		  (char *)args[i].value,
		  sizeof(StatusInfo));
	}
    }
}

static void
Destroy(w)
    Widget w;
{
    ClientWindowWidget cw = (ClientWindowWidget)w;

#ifdef DEBUG
    printf("Client: Destroy\n");
#endif
    XtDestroyWidget(cw->clientWindow.status_widget);
}
static Widget
CreateStatus(w)
    Widget w;
{
    int ac;
    Arg args[20];
    Widget widget;
    ClientWindowWidget clw;
    XrmValue from, to;
    XFontSet fs;

    clw = (ClientWindowWidget)w;
    ac = 0;
    XtSetArg(args[ac], XtNlabel, ""); ac++;
    XtSetArg(args[ac], XtNborderWidth, 0); ac++;
    XtSetArg(args[ac], XtNmappedWhenManaged, FALSE); ac++;

    XtSetArg(args[ac], XtNcolormap, clw->clientWindow.status.colormap); ac++;
    XtSetArg(args[ac], XtNx, clw->clientWindow.status.area.x); ac++;
    XtSetArg(args[ac], XtNy, clw->clientWindow.status.area.y); ac++;
    XtSetArg(args[ac], XtNwidth, clw->clientWindow.status.area.width); ac++;
    XtSetArg(args[ac], XtNheight, clw->clientWindow.status.area.height); ac++;
    XtSetArg(args[ac], XtNforeground, clw->clientWindow.status.foreground); ac++;
    XtSetArg(args[ac], XtNbackground, clw->clientWindow.status.background); ac++;
    XtSetArg(args[ac], XtNbackgroundPixmap, clw->clientWindow.status.background_pixmap); ac++;
    if (*clw->clientWindow.status_font != '\0') {
        from.addr = clw->clientWindow.status_font;
        from.size  = strlen(from.addr);
        XtConvert(w, XtRString, &from, XtRFontSet, &to);
        if (to.addr != NULL) {
	    fs = *(XFontSet *)to.addr;
	    XtSetArg(args[ac], XtNfontSet, fs); ac++;
        }
    }
    XtSetArg(args[ac], XtNcursor, clw->clientWindow.status.cursor); ac++;
    widget = XtCreateManagedWidget("status", preeditWidgetClass, w, args, ac);
    return(widget);
}

static void
StatusDraw(w, s)
    ClientWindowWidget w;
    XIMStatusDrawCallbackStruct *s;
{
    int ac;
    Arg args[10];
    ClientWindowWidget imw = (ClientWindowWidget)w;

    w->clientWindow.status_text = s->data.text;
    if (w->clientWindow.status_widget != (Widget)NULL) {
	PreeditDraw(w->clientWindow.status_widget,
		    s->data.text->string.wide_char,
		    s->data.text->feedback,
		    s->data.text->length);
    }
}

void
ClientStatusDraw(w, s)
    Widget w;
    XIMStatusDrawCallbackStruct *s;
{
    ClientWindowWidget imw = (ClientWindowWidget)w;
    ClientWindowWidgetClass class = (ClientWindowWidgetClass)imw->core.widget_class;
    
    if (!XtIsSubclass(imw, clientWindowWidgetClass)) {
	return;
    }
    (*class->clientWindow_class.status_draw)(imw, s);
}

/****************************************************************
 *
 * Action Routines
 *
 ****************************************************************/

/*ARGSUSED*/
static void
destroy_handler(w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    ClientWindowWidget fw = (ClientWindowWidget) w;
    
    if (event->type == DestroyNotify) {
#ifdef DEBUG
	printf("Client: destroy handler\n");
#endif
	_XtUnregisterWindow(XtWindow(fw), fw);
	fw->core.window = NULL;
	XtDestroyWidget(fw);
#ifdef DEBUG
	printf("Client: destroy handler end\n");
#endif
    }
}
