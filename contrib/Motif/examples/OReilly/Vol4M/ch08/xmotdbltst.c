/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xmotdbltst.c - simple program to test getting motion events and 
 * double clicks in the same widget.
 */

#include <stdio.h>
/*
 * Include files required for all Toolkit programs
 */
#include <Xm/Xm.h>	/* Intrinsics and Motif Definitions */

/*
 * Public include file for widgets we actually use in this file.
 */
#include <Xm/PushB.h>		/* Athena Label Widget */

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
		"XMotdbltst",       /* Application class */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

	trial = XtCreateManagedWidget(
		"trial",		/* arbitrary widget name */
		xmPushButtonWidgetClass,	/* widget class from Label.h */
		topLevel,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

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
