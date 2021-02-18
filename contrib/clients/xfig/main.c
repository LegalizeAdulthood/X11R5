/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
 *
 * "Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty."
 *
 */

#include "fig.h"
#include "version.h"
#include "patchlevel.h"
#include "object.h"
#include "mode.h"
#include "resources.h"
#include "u_fonts.h"
#include "w_drawprim.h"
#include "w_mousefun.h"
#include "w_setup.h"
#include "w_util.h"

extern void     setup_cmd_panel();
extern          X_error_handler();
extern          error_handler();
extern int      ignore_exp_cnt;
extern char    *getenv();

#include "fig.icon.X"
Pixmap          fig_icon;

static char     tool_name[100];
static          sigwinched();

/************** FIG options ******************/

TOOL            tool;
static char    *filename = NULL;

static Boolean  true = True;
static Boolean  false = False;
static int      zero = 0;

static XtResource application_resources[] = {
    {"showallbuttons", "ShowAllButtons", XtRBoolean, sizeof(Boolean),
    XtOffset(appresPtr, ShowAllButtons), XtRBoolean, (caddr_t) & false},
    {XtNjustify, XtCJustify, XtRBoolean, sizeof(Boolean),
    XtOffset(appresPtr, RHS_PANEL), XtRBoolean, (caddr_t) & false},
    {"landscape", XtCOrientation, XtRBoolean, sizeof(Boolean),
    XtOffset(appresPtr, landscape), XtRBoolean, (caddr_t) & true},
    {"debug", "Debug", XtRBoolean, sizeof(Boolean),
    XtOffset(appresPtr, DEBUG), XtRBoolean, (caddr_t) & false},
    {"pwidth", XtCWidth, XtRFloat, sizeof(float),
    XtOffset(appresPtr, tmp_width), XtRInt, (caddr_t) & zero},
    {"pheight", XtCHeight, XtRFloat, sizeof(float),
    XtOffset(appresPtr, tmp_height), XtRInt, (caddr_t) & zero},
    {XtNreverseVideo, XtCReverseVideo, XtRBoolean, sizeof(Boolean),
    XtOffset(appresPtr, INVERSE), XtRBoolean, (caddr_t) & false},
    {"trackCursor", "Track", XtRBoolean, sizeof(Boolean),
    XtOffset(appresPtr, TRACKING), XtRBoolean, (caddr_t) & true},
    {"inches", "Inches", XtRBoolean, sizeof(Boolean),
    XtOffset(appresPtr, INCHES), XtRBoolean, (caddr_t) & true},
    {"boldFont", "BoldFont", XtRString, sizeof(char *),
    XtOffset(appresPtr, boldFont), XtRString, (caddr_t) NULL},
    {"normalFont", "NormalFont", XtRString, sizeof(char *),
    XtOffset(appresPtr, normalFont), XtRString, (caddr_t) NULL},
    {"buttonFont", "ButtonFont", XtRString, sizeof(char *),
    XtOffset(appresPtr, buttonFont), XtRString, (caddr_t) NULL},
    {"startfontsize", "StartFontSize", XtRFloat, sizeof(float),
    XtOffset(appresPtr, startfontsize), XtRInt, (caddr_t) & zero},
    {"internalborderwidth", "InternalBorderWidth", XtRFloat, sizeof(float),
    XtOffset(appresPtr, internalborderwidth), XtRInt, (caddr_t) & zero},
};

static XrmOptionDescRec options[] =
{
    {"-showallbuttons", ".showallbuttons", XrmoptionNoArg, "True"},
    {"-right", ".justify", XrmoptionNoArg, "True"},
    {"-left", ".justify", XrmoptionNoArg, "False"},
    {"-debug", ".debug", XrmoptionNoArg, "True"},
    {"-landscape", ".landscape", XrmoptionNoArg, "True"},
    {"-Landscape", ".landscape", XrmoptionNoArg, "True"},
    {"-portrait", ".landscape", XrmoptionNoArg, "False"},
    {"-Portrait", ".landscape", XrmoptionNoArg, "False"},
    {"-pwidth", ".pwidth", XrmoptionSepArg, 0},
    {"-pheight", ".pheight", XrmoptionSepArg, 0},
    {"-inverse", ".reverseVideo", XrmoptionNoArg, "True"},
    {"-notrack", ".trackCursor", XrmoptionNoArg, "False"},
    {"-track", ".trackCursor", XrmoptionNoArg, "True"},
    {"-inches", ".inches", XrmoptionNoArg, "True"},
    {"-imperial", ".inches", XrmoptionNoArg, "True"},
    {"-centimeters", ".inches", XrmoptionNoArg, "False"},
    {"-metric", ".inches", XrmoptionNoArg, "False"},
    {"-boldFont", ".boldFont", XrmoptionSepArg, 0},
    {"-normalFont", ".normalFont", XrmoptionSepArg, 0},
    {"-buttonFont", ".buttonFont", XrmoptionSepArg, 0},
    {"-startFontSize", ".startfontsize", XrmoptionSepArg, 0},
    {"-startfontsize", ".startfontsize", XrmoptionSepArg, 0},
    {"-internalBW", ".internalborderwidth", XrmoptionSepArg, 0},
    {"-internalBorderWidth", ".internalborderwidth", XrmoptionSepArg, 0},
};

static XtCallbackRec callbacks[] =
{
    {NULL, NULL},
};

static Arg      form_args[] =
{
    {XtNcallback, (XtArgVal) callbacks},
    {XtNinput, (XtArgVal) True},
    {XtNdefaultDistance, (XtArgVal) 0},
    {XtNresizable, (XtArgVal) False},
};

static void     check_for_resize();
XtActionsRec    form_actions[] =
{
    {"ResizeForm", (XtActionProc) check_for_resize},
};

static String   form_translations =
"<ConfigureNotify>:ResizeForm()\n";

#define	NCHILDREN	9
static TOOL     form;

main(argc, argv)
    int             argc;
    char           *argv[];

{
    TOOL            children[NCHILDREN];
    int             ichild;
    int             init_canv_wd, init_canv_ht;
    XWMHints       *wmhints;
    char            i;
    Dimension       w, h;

    DeclareArgs(5);

    (void) sprintf(tool_name, " XFIG %s.%s  (protocol: %s)",
		   FIG_VERSION, PATCHLEVEL, PROTOCOL_VERSION);
    (void) strcat(file_header, PROTOCOL_VERSION);
    tool = XtAppInitialize(&tool_app, "Fig", options, XtNumber(options),
			(Cardinal *) & argc, argv, NULL, NULL, 0);

    fix_converters();
    XtGetApplicationResources(tool, &appres, application_resources,
			      XtNumber(application_resources), NULL, 0);

    i = 1;
    while (argc-- > 1) {
	if (*argv[i] != '-') {	/* search for non - name */
	    filename = argv[i];
	    break;
	}
	i++;
    }

    print_landscape = appres.landscape;	/* match print and screen format to
					 * start */

    tool_d = XtDisplay(tool);
    tool_s = XtScreen(tool);
    tool_sn = DefaultScreen(tool_d);

    /* run synchronously for debugging */

    if (appres.DEBUG) {
	XSynchronize(tool_d, 1);
	fprintf(stderr, "Debug mode, running synchronously\n");
    }
    if (CellsOfScreen(tool_s) == 2 && appres.INVERSE) {
	XrmValue        value;
	XrmDatabase     newdb = (XrmDatabase) 0, old;

	value.size = sizeof("White");
	value.addr = "White";
	XrmPutResource(&newdb, "xfig*borderColor", "String",
		       &value);
	value.size = sizeof("White");
	value.addr = "White";
	XrmPutResource(&newdb, "xfig*foreground", "String",
		       &value);
	value.size = sizeof("Black");
	value.addr = "Black";
	XrmPutResource(&newdb, "xfig*background", "String",
		       &value);
	old = XtDatabase(tool_d);
	XrmMergeDatabases(newdb, &old);

	/* now set the tool part, since its already created */
	FirstArg(XtNborderColor, WhitePixelOfScreen(tool_s));
	NextArg(XtNforeground, WhitePixelOfScreen(tool_s));
	NextArg(XtNbackground, BlackPixelOfScreen(tool_s));
	SetValues(tool);
    }
    init_font();

    gc = DefaultGC(tool_d, tool_sn);
    bold_gc = DefaultGC(tool_d, tool_sn);
    button_gc = DefaultGC(tool_d, tool_sn);

    /* set the roman and bold fonts for the message windows */
    XSetFont(tool_d, gc, roman_font->fid);
    XSetFont(tool_d, bold_gc, bold_font->fid);
    XSetFont(tool_d, button_gc, button_font->fid);

    init_cursor();
    form = XtCreateManagedWidget("form", formWidgetClass, tool,
				 form_args, XtNumber(form_args));

    if (cur_fontsize == 0)
	cur_fontsize = (int) appres.startfontsize;
    if (cur_fontsize == 0)
	cur_fontsize = DEF_FONTSIZE;

    if (INTERNAL_BW == 0)
	INTERNAL_BW = (int) appres.internalborderwidth;
    if (INTERNAL_BW <= 0)
	INTERNAL_BW = DEF_INTERNAL_BW;

    SW_PER_ROW = SW_PER_ROW_PORT;
    SW_PER_COL = SW_PER_COL_PORT;
    init_canv_wd = appres.tmp_width *
	(appres.INCHES ? PIX_PER_INCH : PIX_PER_CM);
    init_canv_ht = appres.tmp_height *
	(appres.INCHES ? PIX_PER_INCH : PIX_PER_CM);

    if (init_canv_wd == 0)
	init_canv_wd = appres.landscape ? DEF_CANVAS_WD_LAND :
	    DEF_CANVAS_WD_PORT;

    if (init_canv_ht == 0)
	init_canv_ht = appres.landscape ? DEF_CANVAS_HT_LAND :
	    DEF_CANVAS_HT_PORT;

    if ((init_canv_ht < DEF_CANVAS_HT_PORT) ||
	(HeightOfScreen(tool_s) < DEF_CANVAS_HT_PORT)) {
	SW_PER_ROW = SW_PER_ROW_LAND;
	SW_PER_COL = SW_PER_COL_LAND;
    }
    setup_sizes(init_canv_wd, init_canv_ht);
    (void) init_cmd_panel(form);
    (void) init_msg(form);
    (void) init_mousefun(form);
    (void) init_mode_panel(form);
    (void) init_topruler(form);
    (void) init_canvas(form);
    (void) init_fontmenu(form);	/* printer font menu */
    (void) init_unitbox(form);
    (void) init_sideruler(form);
    (void) init_ind_panel(form);

    ichild = 0;
    children[ichild++] = cmd_panel;	/* command buttons */
    children[ichild++] = msg_panel;	/* message window */
    children[ichild++] = mousefun;	/* labels for mouse fns */
    children[ichild++] = mode_panel;	/* current mode */
    children[ichild++] = topruler_sw;	/* top ruler */
    children[ichild++] = unitbox_sw;	/* box containing units */
    children[ichild++] = sideruler_sw;	/* side ruler */
    children[ichild++] = canvas_sw;	/* main drawing canvas */
    children[ichild++] = ind_panel;	/* current settings indicators */

    /*
     * until the following XtRealizeWidget() is called, there are NO windows
     * in existence
     */

    XtManageChildren(children, NCHILDREN);
    XtRealizeWidget(tool);

    /* Set the input field to true to allow keyboard input */
    wmhints = XGetWMHints(tool_d, XtWindow(tool));
    wmhints->flags |= InputHint;/* add in input hint */
    wmhints->input = True;
    XSetWMHints(tool_d, XtWindow(tool), wmhints);
    XFree((char *) wmhints);

    if (appres.RHS_PANEL) {	/* side button panel is on right size */
	FirstArg(XtNfromHoriz, 0);
	NextArg(XtNhorizDistance, RULER_WD + INTERNAL_BW);
	SetValues(topruler_sw);

	FirstArg(XtNfromHoriz, 0);
	NextArg(XtNhorizDistance, 0);
	NextArg(XtNfromVert, topruler_sw);
	NextArg(XtNleft, XtChainLeft);	/* chain to left of form */
	NextArg(XtNright, XtChainLeft);
	SetValues(sideruler_sw);

	FirstArg(XtNfromHoriz, 0);
	NextArg(XtNhorizDistance, 0);
	NextArg(XtNfromVert, msg_panel);
	NextArg(XtNleft, XtChainLeft);	/* chain to left of form */
	NextArg(XtNright, XtChainLeft);
	SetValues(unitbox_sw);

	/* relocate the side button panel */
	XtUnmanageChild(mode_panel);
	XtUnmanageChild(canvas_sw);
	FirstArg(XtNfromHoriz, canvas_sw);	/* panel right of canvas */
	NextArg(XtNhorizDistance, -INTERNAL_BW);
	NextArg(XtNfromVert, mousefun);
	NextArg(XtNleft, XtChainRight);
	NextArg(XtNright, XtChainRight);
	SetValues(mode_panel);
	FirstArg(XtNfromHoriz, sideruler_sw);	/* panel right of canvas */
	SetValues(canvas_sw);
	XtManageChild(canvas_sw);
	XtManageChild(mode_panel);
    }
    fig_icon = XCreateBitmapFromData(tool_d, XtWindow(tool),
				     fig_bits, fig_width, fig_height);

    FirstArg(XtNtitle, tool_name);
    NextArg(XtNiconPixmap, fig_icon);
    SetValues(tool);

    init_gc();
    setup_cmd_panel();
    setup_msg();
    setup_canvas();
    setup_rulers();
    setup_mode_panel();
    setup_mousefun();
    setup_fontmenu();		/* setup bitmaps in printer font menu */
    setup_ind_panel();
    get_directory(cur_dir);

    FirstArg(XtNwidth, &w);
    NextArg(XtNheight, &h);
    GetValues(tool);
    TOOL_WD = (int) w;
    TOOL_HT = (int) h;
    XtAppAddActions(tool_app, form_actions, XtNumber(form_actions));
    XtOverrideTranslations(form, XtParseTranslationTable(form_translations));

    XSetErrorHandler(X_error_handler);
    XSetIOErrorHandler(X_error_handler);
    (void) signal(SIGHUP, error_handler);
    (void) signal(SIGFPE, error_handler);
    (void) signal(SIGBUS, error_handler);
    (void) signal(SIGSEGV, error_handler);
    (void) signal(SIGINT, SIG_IGN); /* in case user accidentally types ctrl-c */

    put_msg("READY, please select a mode or load a file");

    /*
     * decide on filename for cut buffer: first try users HOME directory to
     * allow cutting and pasting between sessions, if this fails create
     * unique filename in /tmp dir
     */

    if (*strcpy(cut_buf_name, getenv("HOME")) != NULL) {
	strcat(cut_buf_name, "/.xfig");
    } else {
	sprintf(cut_buf_name, "%s%06d", "/tmp/xfig", getpid());
    }

    while (XtAppPending(tool_app)) {
	XEvent          event;

	/* pass events to ensure we are completely initialised */
	XtAppNextEvent(tool_app, &event);
	XtDispatchEvent(&event);
    }

    if (filename == NULL)
	strcpy(cur_filename, DEF_NAME);
    else
	load_file(filename);

    XtAppMainLoop(tool_app);
}

static void
check_for_resize(tool, event, params, nparams)
    TOOL            tool;
    INPUTEVENT     *event;
    String         *params;
    Cardinal       *nparams;
{
    XConfigureEvent *xc = (XConfigureEvent *) event;
    Dimension       b;
    int             dx, dy;

    DeclareArgs(3);

    if (xc->width == TOOL_WD && xc->height == TOOL_HT)
	return;			/* no size change */
    dx = xc->width - TOOL_WD;
    dy = xc->height - TOOL_HT;
    TOOL_WD = xc->width;
    TOOL_HT = xc->height;
    setup_sizes(CANVAS_WD + dx, CANVAS_HT + dy);

    XawFormDoLayout(form, False);
    ignore_exp_cnt++;		/* canvas is resized twice - redraw only once */

    FirstArg(XtNborderWidth, &b);
    /* first redo the top panels */
    GetValues(cmd_panel);
    XtResizeWidget(cmd_panel, CMDPANEL_WD, CMDPANEL_HT, b);
    GetValues(msg_panel);
    XtResizeWidget(msg_panel, MSGPANEL_WD, MSGPANEL_HT, b);
    GetValues(mousefun);
    XtResizeWidget(mousefun, MOUSEFUN_WD, MOUSEFUN_HT, b);
    XtUnmanageChild(mousefun);
    resize_mousefun();
    XtManageChild(mousefun);	/* so that it shifts with msg_panel */

    /* now redo the center area */
    XtUnmanageChild(mode_panel);
    FirstArg(XtNheight, (MODEPANEL_SPACE + 1) / 2);
    SetValues(d_label);
    FirstArg(XtNheight, (MODEPANEL_SPACE) / 2);
    SetValues(e_label);
    XtManageChild(mode_panel);	/* so that it adjusts properly */

    FirstArg(XtNborderWidth, &b);
    GetValues(canvas_sw);
    XtResizeWidget(canvas_sw, CANVAS_WD, CANVAS_HT, b);
    GetValues(topruler_sw);
    XtResizeWidget(topruler_sw, TOPRULER_WD, TOPRULER_HT, b);
    resize_topruler();
    GetValues(sideruler_sw);
    XtResizeWidget(sideruler_sw, SIDERULER_WD, SIDERULER_HT, b);
    resize_sideruler();
    XtUnmanageChild(sideruler_sw);
    XtManageChild(sideruler_sw);/* so that it shifts with canvas */
    XtUnmanageChild(unitbox_sw);
    XtManageChild(unitbox_sw);	/* so that it shifts with canvas */

    XawFormDoLayout(form, True);
}
