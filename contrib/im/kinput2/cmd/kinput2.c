/*
 *	kinput2
 */

/*
 * Copyright (C) 1991 by Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 *		ishisone@sra.co.jp
 */

#ifndef lint
static char	*rcsid = "$Id: kinput2.c,v 1.12 1991/09/30 01:23:01 ishisone Rel $";
#endif

#include <stdio.h>
#include <signal.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include "MyDispatch.h"
#include "ConvMgr.h"
#include "KIProto.h"
#include "XimpProto.h"

#if !defined(USE_WNN) && !defined(USE_IROHA) && !defined(USE_SJ3)
#define USE_WNN			/* default */
#endif

#ifdef USE_WNN
#include "CcWnn.h"
#include "WcharDisp.h"
#endif
#ifdef USE_IROHA
#include "Iroha.h"
#include "WcharDisp.h"
#endif
#ifdef USE_SJ3
#include "sj3def.h"
#include "Sj3.h"
#include "WcharDisp.h"
#endif

#include "DebugPrint.h"

int	debug_all;

/* application resource */
typedef struct {
    char *conversionEngine;
} AppResRec, *AppResP;

static AppResRec appres;

static XtResource app_resources[] = {
    { "conversionEngine", "ConversionEngine", XtRString, sizeof(String),
      XtOffset(AppResP, conversionEngine), XtRString, (XtPointer)"" },
};

static XrmOptionDescRec	options[] = {
    {"-bc",		"*KinputProtocol.backwardCompatible", XrmoptionNoArg,	"True"},
    {"-font",		"*JpWcharDisplay.font",	XrmoptionSepArg,	NULL},
    {"-kanjifont",	"*JpWcharDisplay.kanjiFont", XrmoptionSepArg,	NULL},
    {"-kanafont",	"*JpWcharDisplay.kanaFont", XrmoptionSepArg,	NULL},
#ifdef USE_WNN
    {"-wnn",		".conversionEngine",	XrmoptionNoArg,		"wnn"},
    {"-jserver",	"*CcWnn.jserver",	XrmoptionSepArg,	NULL},
    {"-ccdef",		"*CcWnn.ccdef",		XrmoptionSepArg,	NULL},
    {"-wnnenvname",	"*CcWnn.wnnEnvname",	XrmoptionSepArg,	NULL},
    {"-wnnenvrc",	"*CcWnn.wnnEnvrc",	XrmoptionSepArg,	NULL},
#endif
#ifdef USE_IROHA
    {"-canna",		".conversionEngine",	XrmoptionNoArg,		"canna"},
    {"-irohaserver",	"*Iroha.irohahost",	XrmoptionSepArg,	NULL},
    {"-cannaserver",	"*Iroha.irohahost",	XrmoptionSepArg,	NULL},
    {"-is",		"*Iroha.irohahost",	XrmoptionSepArg,	NULL},
    {"-irohafile",	"*Iroha.irohafile",	XrmoptionSepArg,	NULL},
    {"-cannafile",	"*Iroha.irohafile",	XrmoptionSepArg,	NULL},
#endif
#ifdef USE_SJ3
    {"-sj3",        ".conversionEngine",    XrmoptionNoArg,     "sj3"},
    {"-sj3serv",    "*Sj3.sj3serv",     XrmoptionSepArg,    NULL},
    {"-sj3serv2",   "*Sj3.sj3serv2",    XrmoptionSepArg,    NULL},
    {"-sj3user",    "*Sj3.sj3user",     XrmoptionSepArg,    NULL},
    {"-rcfile",     "*Sj3.rcfile",      XrmoptionSepArg,    NULL},
    {"-sbfile",     "*Sj3.sbfile",      XrmoptionSepArg,    NULL},
    {"-rkfile",     "*Sj3.rkfile",      XrmoptionSepArg,    NULL},
#ifndef USE_HANTOZEN
    {"-hkfile",     "*Sj3.hkfile",  XrmoptionSepArg,    NULL},
    {"-zhfile",     "*Sj3.zhfile",  XrmoptionSepArg,    NULL},
#endif
#endif
};

XtAppContext	apc;
Widget		toplevel;

static int	(*DefaultErrorHandler)();

static WidgetClass	getInputObjClass();
static int	IgnoreBadWindow();
static int	Exit();
static void	Destroyed();
static void	usage();

void
main(ac, av)
int ac;
char **av;
{
    Widget manager, protocol1, protocol2;
    int i;
    Display *dpy;
    WidgetClass inputobjclass;

    toplevel = XtAppInitialize(&apc, "Kinput2",
			       options, XtNumber(options),
			       (Cardinal *)&ac, av,
			       (String *)NULL, (ArgList)NULL, 0);

    /* check invalid argument */ 
    if (ac > 1) {
	int do_usage = 0;
	for (i = 1; i < ac; i++) {
	    if (!strcmp(av[i], "-debug")) {
		debug_all = 1;
	    } else {
		fprintf(stderr, "unknown argument: %s\n", av[i]);
		do_usage = 1;
	    }
	}
	if (do_usage) usage();
    }

    dpy = XtDisplay(toplevel);

#ifdef RANDOM_ID
    /*
     * one nasty hack here:
     *
     * kinput clients often use server's window ID for the only key
     * value to identify their conversion server (kinput), and they
     * think it is dead and take appropriate action (eg connecting to
     * the new server) when they notice the ID has changed.
     *
     * but it is likely that another kinput has the same resource ID
     * base (because X servers always choose the smallest unused ID
     * base for new clients). and if it is the same, so is the owner
     * window ID, and the clients don't notice the change.
     *
     * to get rid of the problem, we add some small random offset to
     * the resource ID so that every time we get different owner ID
     * even if the resource ID base is the same.
     *
     * of course it heavily depends on the current implementaion of
     * the resource ID allocation in Xlib, so I call it 'nasty'.
     */
    dpy->resource_id += getpid() % 1024;
#endif

    inputobjclass = getInputObjClass();

    manager = XtVaCreateManagedWidget("convmanager",
				      conversionManagerWidgetClass,
				      toplevel,
				      XtNwidth, 1,
				      XtNheight, 1,
				      NULL);

    protocol1 = XtVaCreateWidget("kinputprotocol",
				 kinputProtocolWidgetClass,
				 manager,
				 XtNlanguage, "JAPANESE",
				 XtNinputObjectClass, inputobjclass,
				 XtNdisplayObjectClass, jpWcharDisplayObjectClass,
				 XtNwidth, 1,
				 XtNheight, 1,
				 NULL);

    protocol2 = XtVaCreateWidget("ximpprotocol",
				 ximpProtocolWidgetClass,
				 manager,
				 XtNlocaleName, "ja_JP",
				 XtNinputObjectClass, inputobjclass,
				 XtNdisplayObjectClass, jpWcharDisplayObjectClass,
				 XtNwidth, 1,
				 XtNheight, 1,
				 NULL);

    XtAddCallback(protocol1, XtNdestroyCallback, Destroyed, (XtPointer)NULL);
    XtAddCallback(protocol2, XtNdestroyCallback, Destroyed, (XtPointer)NULL);

    /* set signal handler */
    if (signal(SIGINT, SIG_IGN) != SIG_IGN) signal(SIGINT, Exit);
    signal(SIGTERM, Exit);
#ifdef USE_WNN
    signal(SIGPIPE, SIG_IGN);
#endif

    /* set my error handler */
    DefaultErrorHandler = XSetErrorHandler(IgnoreBadWindow);

    XtRealizeWidget(toplevel);

    for (;;) {
	XEvent event;

	XtAppNextEvent(apc, &event);
	XtDispatchEvent(&event);
	MyDispatchEvent(&event); /* additional dispatcher */
    }
    /* NOTREARCHED */
}

static WidgetClass
getInputObjClass()
{
    WidgetClass class;

    XtGetApplicationResources(toplevel, &appres,
			      app_resources, XtNumber(app_resources),
			      NULL, 0);
#ifdef USE_WNN
    if (!strcmp(appres.conversionEngine, "wnn")) {
	class = ccWnnObjectClass;
    } else
#endif
#ifdef USE_IROHA
    if (!strcmp(appres.conversionEngine, "canna") ||
	!strcmp(appres.conversionEngine, "iroha")) {
	class = irohaObjectClass;
    } else
#endif
#ifdef USE_SJ3
    if (!strcmp(appres.conversionEngine, "sj3")) {
	class = sj3ObjectClass;
    } else
#endif
#ifdef USE_WNN
    class = ccWnnObjectClass;
#else
#  ifdef USE_IROHA
    class = irohaObjectClass;
#  endif
#  ifdef USE_SJ3
    class = sj3ObjectClass;
#  endif
#endif

    return class;
}

static int
IgnoreBadWindow(dpy, error)
Display *dpy;
XErrorEvent *error;
{
    /*
     * BadWindow events will be sent if any of the active clients die
     * during conversion.  Although I select DestroyNotify event on the
     * client windows to detect their destruction and take appropriate
     * actions, and I'm careful when doing critical operations, but still
     * there is a chance of getting unexpecting BadWindow error caused by
     * client death.
     * So I set the error handler to ignore BadWindow errors. Of course
     * I'd better check if the resourceid field of the error event is the
     * window ID of a client, but I'm too lazy to do that...
     */
    if (error->error_code == BadWindow) {
	return;
    } else {
	/* invoke default error handler */
	(*DefaultErrorHandler)(dpy, error);
    }
}

static int
Exit()
{
    XtDestroyWidget(toplevel);
    exit(0);
}

/* ARGSUSED */
static void
Destroyed(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
    static int cnt = 0;

    /*
     * if both of the protocol widgets (ie KinputProtocol and XimpProtocol),
     * kill myself.
     */
    if (++cnt > 1) Exit();
}

static void
usage()
{
    char **p;
    static char *syntaxtable[] = {
#ifdef USE_WNN
	"-wnn",			"use Wnn as the conversion engine",
	"-jserver <hostname>",	"specifies jserver host",
	"-ccdef <ccdeffile>",	"specifies character conversion def. file",
	"-wnnenvname <name>",	"specifies Wnn environment name",
	"-wnnenvrc <wnnenvrcfile>",	"specifies Wnn environment file",
#endif
#ifdef USE_IROHA
	"-canna",		"use Canna (Iroha) as the conversion engine",
	"{-irohaserver|-is} <hostname>[:n]", "specifies irohaserver host",
	"-irohafile <irohafile>", "specifies iroha customize file",
#endif
#ifdef USE_SJ3
	"-sj3",         	"use SJ3 as the conversion engine",
	"-sj3serv <hostname>",  "specifies first sj3serv host",
	"-sj3serv2 <hostname>", "specifies second sj3serv host",
	"-sj3user <user>",      "specifies user name connect to sj3serv",
	"-rcfile <file>",       "specifies resource definition file",
	"-sbfile <file>",       "specifies symbol table file",
	"-rkfile <file>",       "specifies roma-kana coversion definition file",
#ifndef USE_HANTOZEN
	"-hkfile <file>",       "specifies hira-kana coversion definition file",
	"-zhfile <file>",       "specifies zen/han coversion definition file",
#endif
#endif
	"-bc",			"backward compatible mode",
	"-font <font>",		"ASCII font to be used",
	"-kanjifont <font>",	"KANJI font to be used",
	"-kanafont <font>",	"KANA font to be used",
	"-background <color>",	"specifies background color",
	"-foreground <color>",	"specifies foreground color",
	"-geometry <geometry>",	"specifies geometry",
	"-rv",			"reverse video mode",
	"-display <display>",	"specifies display",
	NULL, NULL,
    };

    fprintf(stderr, "options are:\n");
    for (p = syntaxtable; *p != NULL; p += 2) {
	fprintf(stderr, "    %-30s %s\n", *p, *(p + 1));
    }
    exit(1);
}

#if defined(USE_WNN) && defined(NEED_Strlen)
/*
 * Wnn/jlib/js.c should have this function...
 */
int
Strlen(s)
unsigned short *s;
{
    int n = 0;

    while (*s++) n++;
    return n;
}
#endif
