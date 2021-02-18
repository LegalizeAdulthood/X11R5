/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */

/*
 * xfarewell.c - simple program to provide a Command widget that
 * 		performs a different action in response to a
 *              click of the first and second pointer buttons.
 */

#include <stdio.h>
/*
 * Include files required for all Toolkit programs
 */
#include <X11/Intrinsic.h>          /* Intrinsics Definitions */
#include <X11/StringDefs.h>         /* Standard String definitions */

/*
 * Public include file for widgets we actually use in this file.
 */
#include <X11/Xaw/Command.h>        /* Athena Command Widget */

/* 
 *  Confirm action
 */
/*ARGSUSED*/
static void Confirm(w, event, params, num_params)
Widget w;
XButtonEvent *event;
String *params; 
Cardinal *num_params;
{
/*
 * Once we show how to do it, we could pop-up a dialog box to do this.
 * Since we haven't yet, simply print a message to stderr.
 */
	fprintf(stderr, "Are you sure you want to exit?\n\
                Click with the middle pointer button if you're sure.\n");
}

/* 
 *  Quit action
 */
/*ARGSUSED*/
static void Quit(w, event, params, num_params)
Widget w;
XButtonEvent *event;
String *params;
Cardinal *num_params;
{
	fprintf(stderr, "It was nice knowing you.\n");
	exit(0); 
}

main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_context;
	Widget topLevel, farewell;

	static XtActionsRec two_quits[] = {
		{"confirm", Confirm},
		{"quit", Quit},
	};

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
        "XFarewell",        /* Application class */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

	farewell = XtVaCreateManagedWidget(
		"farewell",         /* arbitrary widget name */
		commandWidgetClass, /* widget class from Command.h */
		topLevel,           /* parent widget*/
		NULL);              /* terminate varargs list */

	XtAppAddActions(app_context, two_quits, XtNumber(two_quits));

	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	/*
	 *  Loop for events.
	 */
	XtAppMainLoop(app_context);
}
