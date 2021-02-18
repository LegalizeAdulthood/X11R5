/*
 * Copyright (C) 1990 Regents of the University of California.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of the University of
 * California not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  the University of California makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 */

# include <X11/Intrinsic.h>
# include <X11/StringDefs.h>
# include <X11/Shell.h>
# include <X11/Xaw/Form.h>

# include <stdio.h>

# include "cdrom_globs.h"
# include "debug.h"

XtAppContext	appc;
char		*file;
Boolean		debug		= False;
Boolean		display_timer	= True;
float		volbase		= 0.0;
float		volpcent	= 1.0;

static Widget	top_shell;
static Widget	top_form_widget;

static XrmOptionDescRec options[] = {
{ "-file",	".file",	XrmoptionSepArg,	(caddr_t) NULL },
{ "-debug",	".debug",	XrmoptionNoArg,		(caddr_t) "True" },
{ "-displayTimer", ".displayTimer", XrmoptionNoArg,	(caddr_t) "True" },
{ "-volBase",	".volBase",	XrmoptionSepArg,	(caddr_t) 0 },
{ "-volPcent",	".volPcent",	XrmoptionSepArg,	(caddr_t) 0 },
};

static XtResource	resources[] = {
	{ "file", "File", XtRString, sizeof(String), (Cardinal) &file,
		  XtRString, (caddr_t) NULL },
	{ "debug", "Debug", XtRBoolean, sizeof(Boolean), (Cardinal) &debug,
		  XtRBoolean, (caddr_t) &debug },
	{ "displayTimer", "DisplayTimer", XtRBoolean, sizeof(Boolean),
		  (Cardinal) &display_timer, XtRBoolean,
		  (caddr_t) &display_timer },
	{ "volBase", "VolBase", XtRFloat, sizeof(float),
		  (Cardinal) &volbase, XtRFloat, (caddr_t) &volbase },
	{ "volPcent", "VolPcent", XtRFloat, sizeof(float),
		  (Cardinal) &volpcent, XtRFloat, (caddr_t) &volpcent },
};

Widget
top_setup(argc, argv)
	int			argc;
	char			**argv;
{
	Display			*dpy;

	XtToolkitInitialize();

	appc = XtCreateApplicationContext();

	dpy = XtOpenDisplay(appc, NULL, "xcdplayer", "XCdplayer",
			    options, XtNumber(options),
			    (Cardinal *) &argc, argv);

	if (dpy == NULL) {
		fprintf(stderr, "can't open display\n");
		exit(1);
	}

	top_shell = XtAppCreateShell("xcdplayer", "XCdplayer",
				     applicationShellWidgetClass,
				     dpy, (ArgList) NULL, 0);

	(void) XtGetApplicationResources(top_shell, (caddr_t) NULL,
					 resources, XtNumber(resources),
					 (ArgList) NULL, 0);

	top_form_widget = XtCreateWidget("mainForm", formWidgetClass,
						 top_shell,
						 (ArgList) NULL, 0);

	return(top_form_widget);
}

void
top_start() {
	XtManageChild(top_form_widget);
	XtRealizeWidget(top_shell);
}
