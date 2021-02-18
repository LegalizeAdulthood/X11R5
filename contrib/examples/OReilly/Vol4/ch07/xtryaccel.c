/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xtryaccel.c
 */

#include <stdio.h>
/*
 * Include files required for all Toolkit programs
 */
#include <X11/Intrinsic.h>	/* Intrinsics Definitions */
#include <X11/StringDefs.h>	/* Standard Name-String definitions */

/*
 * Public include file for widgets we actually use in this file.
 */
#include <X11/Xaw/Command.h>		/* Athena Label Widget */
#include <X11/Xaw/Box.h>		/* Athena Label Widget */

/*
 * Goodbye button callback function
 */
/* ARGSUSED */
void Quit(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{ 
	exit(0);
}

/*
 * Hello button callback function
 */
/* ARGSUSED */
void Print(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{ 
	fprintf(stderr, "It was nice knowing you.\n");
}

main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_context;
	Widget topLevel, box, goodbye, hello;

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
		"XTryAccel",        /* Application class */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

	box = XtCreateManagedWidget(
		"box",		/* arbitrary widget name */
		boxWidgetClass,	/* widget class from Label.h */
		topLevel,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	goodbye = XtCreateManagedWidget(
		"goodbye",		/* arbitrary widget name */
		commandWidgetClass,	/* widget class from Label.h */
		box,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	hello = XtCreateManagedWidget(
		"hello",		/* arbitrary widget name */
		commandWidgetClass,	/* widget class from Label.h */
		box,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	XtAddCallback(goodbye, XtNcallback, Quit, 0);
	XtAddCallback(hello, XtNcallback, Print, 0);

	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	XtInstallAccelerators(box, goodbye);
	XtInstallAccelerators(box, hello);

	/*
	 *  Loop for events.
	 */
	XtAppMainLoop(app_context);
}
