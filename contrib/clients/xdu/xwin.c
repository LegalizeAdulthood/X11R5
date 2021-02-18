/*
 * XDU - X Window System Interface.
 *
 * We hide all of the X hieroglyphics inside of this module.
 *
 * Phillip C. Dykstra
 * <phil@BRL.MIL>
 * 4 Sep 1991.
 * 
 * Copyright (C)	Phillip C. Dykstra	1991
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the authors name not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The author makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 */
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Core.h>

/* IMPORTS: routines that this module vectors out to */
extern int press();
extern int reset();
extern int repaint();
extern int ncols;

/* EXPORTS: routines that this module exports outside */
extern int xsetup();
extern int xmainloop();
extern int xrepaint();
extern int xdrawrect();

static String fallback_resources[] = {
"*window.width:		600",
"*window.height:	480",
NULL
};

typedef struct {
	Pixel	foreground;
	Pixel	background;
	XFontStruct	*font;
	int	ncol;
	Boolean	showsize;
} res_data, *res_data_ptr;
static res_data res;

/* Application Resources */
static XtResource application_resources[] = {
	{ XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
		XtOffset(res_data_ptr,foreground), XtRString, "Black"},
	{ XtNbackground, XtCBackground, XtRPixel, sizeof(Pixel),
		XtOffset(res_data_ptr,background), XtRString, "White"},
	{ XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
		XtOffset(res_data_ptr,font), XtRString, "Fixed" },
	{"ncol", "Ncol", XtRInt, sizeof(int),
		XtOffset(res_data_ptr,ncol), XtRString, "5"},
	{"showsize", "ShowSize", XtRBoolean, sizeof(Boolean),
		XtOffset(res_data_ptr,showsize), XtRString, "True"}
};

/* To Do... */
static XrmOptionDescRec options[] = {
	{"-ncol",	"*ncol",	XrmoptionSepArg,	NULL}
};

/* action routines */
static void a_goto();
static void a_reset();
static void a_quit();

static XtActionsRec actionsTable[] = {
	{ "reset",	a_reset },
	{ "goto",	a_goto },
	{ "quit",	a_quit },
};

static char defaultTranslations[] = "\
<Key>Q:	quit()\n\
<Key>R:	reset()\n\
<Btn1Down>: goto()\n\
<Btn2Down>: reset()\n\
<Btn3Down>: quit()\n\
";

/*  action routines  */

static void a_quit(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
int *num_params;
{
	XtDestroyApplicationContext(XtWidgetToApplicationContext(w));
	exit(0);
}

static void a_goto(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
int *num_params;
{
	press(event->xbutton.x, event->xbutton.y);
}

static void a_reset(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
int *num_params;
{
	reset();
}

/* callback routines */

static void c_repaint(w, data, event)
Widget w;
caddr_t data;
XEvent *event;
{
	xrepaint();
}

/* X Window related variables */
static Cursor WorkingCursor;
static Display *dpy;
static int screen;
static Visual *vis;
static Window win;
static GC gc;
static GC cleargc;
static XtAppContext app_con;

/*  External Functions  */

int
xsetup(argc, argv)
int argc;
char **argv;
{
	XtTranslations trans_table;
	Widget toplevel;
	Widget w;
	XGCValues gcv;

	/* Create the top level Widget */
	toplevel = XtAppInitialize(&app_con, "XDu",
			options, XtNumber(options),
			&argc, argv, fallback_resources, NULL, 0);

	if (argc != 1) {
		(void) fprintf(stderr, "Usage: %s [Xt options]\n", argv[0]);
		exit(1);
	}

	XtGetApplicationResources(toplevel, &res, application_resources,
		XtNumber(application_resources), NULL, 0 );

	XtAppAddActions(app_con, actionsTable, XtNumber(actionsTable));
	trans_table = XtParseTranslationTable(defaultTranslations);

	/* Create a simple Core class widget to draw in */
	w = XtCreateManagedWidget("window", coreWidgetClass, toplevel,
		NULL, 0);

	/* events */
	XtAddEventHandler(w, ExposureMask, FALSE, c_repaint, NULL);
	XtAddEventHandler(w, StructureNotifyMask, FALSE, c_repaint, NULL);
	XtAugmentTranslations(w, trans_table);

	XtRealizeWidget(toplevel);

	/* We need these for the raw Xlib calls */
	win = XtWindow(w);
	dpy = XtDisplay(w);
	screen = DefaultScreen(dpy);
	vis = DefaultVisual(dpy,screen);

	gcv.foreground = res.foreground;
	gcv.background = res.background;
	gcv.font = res.font->fid;
	gc = XCreateGC(dpy, win, (GCFont|GCForeground|GCBackground), &gcv);

	ncols = res.ncol;
}

xmainloop()
{
	XtAppMainLoop(app_con);
	return(0);
}

xrepaint()
{
	XWindowAttributes xwa;

	XClearWindow(dpy, win);
	XGetWindowAttributes(dpy, win, &xwa);
	repaint(xwa.width, xwa.height);
}

xdrawrect(name, size, x, y, width, height)
char *name;
int size;
int x, y, width, height;
{
	int	textx, texty;
	char	label[1024];
	XCharStruct overall;
	int	ascent, descent, direction;
	int	cheight;

	/*printf("draw(%d,%d,%d,%d)\n", x, y, width, height );*/
	XDrawRectangle(dpy, win, gc, x, y, width, height);

	if (res.showsize) {
		sprintf(label,"%s (%d)", name, size);
		name = label;
	}

	XTextExtents(res.font, name, strlen(name), &direction,
		&ascent, &descent, &overall);
	cheight = overall.ascent + overall.descent;
	if (height < (cheight + 2))
		return;

	/* print label */
	textx = x + 4;
	texty = y + height/2.0 + (overall.ascent - overall.descent)/2.0 + 1.5;
	XDrawString(dpy, win, gc, textx, texty, name, strlen(name));
}
