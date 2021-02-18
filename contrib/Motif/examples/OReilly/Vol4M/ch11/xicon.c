/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */

/*
 * xicon.c
 */

/*
 * Include files required for all Toolkit programs
 */
#include <Xm/Xm.h>	/* Intrinsics Definitions */

/*
 * Public include file for widgets we actually use in this file.
 */
#include <Xm/Label.h>		/* Motif Label Widget */

#include <X11/Shell.h>

#include "icon.bit"

main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_context;
	Widget topLevel, hello;
	Pixmap icon_pixmap;

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
		"XIcon",            /* Application class */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

	icon_pixmap = XCreateBitmapFromData(XtDisplay(topLevel),
			RootWindowOfScreen(XtScreen(topLevel)),
			icon_bits,
			icon_width, icon_height );

	XtVaSetValues(topLevel, 
			XmNiconPixmap, icon_pixmap,
			NULL);

	hello = XtCreateManagedWidget(
		"hello",		/* arbitrary widget name */
		xmLabelWidgetClass,	/* widget class from Label.h */
		topLevel,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	/*
	 *  Loop for events.
	 */
	XtAppMainLoop(app_context);
}
