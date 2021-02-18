/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */

/*
 * xwhichbutton.c - simple program to provide a Command widget that
 * 		performs a different action for each button.
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

#define NUM_ACTIONS 3

/*ARGSUSED*/
static void ActionA(w, event, params, num_params)
Widget w;
XButtonEvent *event;
String *params; 
Cardinal *num_params;
{
	printf("action A executed\n");
}

/*ARGSUSED*/
static void ActionB(w, event, params, num_params)
Widget w;
XButtonEvent *event;
String *params; 
Cardinal *num_params;
{
	printf("action B executed\n");
}

/*ARGSUSED*/
static void ActionC(w, event, params, num_params)
Widget w;
XButtonEvent *event;
String *params; 
Cardinal *num_params;
{
	printf("action C executed\n");
}

main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_context;
	Widget topLevel, trial;

	static XtActionsRec trial_actions[] = {
		{"actionA", ActionA},
		{"actionB", ActionB},
		{"actionC", ActionC},
	};

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
		"XWhichbutton",     /* Application class */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

	trial = XtVaCreateManagedWidget(
		"trial",		/* arbitrary widget name */
		commandWidgetClass,	/* widget class from Label.h */
		topLevel,	/* parent widget*/
		NULL);		/* terminate varargs list */
		
	XtAppAddActions(app_context, trial_actions, NUM_ACTIONS);

	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	/*
	 *  Loop for events.
	 */
	XtAppMainLoop(app_context);
}
