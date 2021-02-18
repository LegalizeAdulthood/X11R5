/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/* 
 *  Ex3-7.c
 */

/*
 *  So that we can use fprintf:
 */
#include <stdio.h>

/* 
 * Standard Toolkit include files:
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

/*
 * Public include files for widgets used in this file.
 */
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Box.h>

/*
 * quit button callback function
 */
/*ARGSUSED*/
void Quit(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{ 
	exit(0); 
}

typedef struct {
	String name;
	String street;
	String city;
} app_stuff;

/*
 * "Press me!" button callback function
 */
/*ARGSUSED*/
void PressMe(w, client_data, call_data)
Widget w;
XtPointer client_data; /* to be cast to app_stuff */
XtPointer call_data;
{ 
	app_stuff *address = (app_stuff *) client_data;

	/* 
	 * Alternately, the second argument can remain declared as
	 * client_data, and the following cast can be used:
	 *     app_stuff *address = (app_stuff *) client_data; 
	 */

	fprintf(stderr, "%s\n%s\n%s\n", address->name, address->street, address->city); 
}

main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_context;
	Widget box, quit, pressme, topLevel;

	static app_stuff stuff = {
		"John Doe",
		"1776 Constitution Way",
		"Phily, PA 01029"
	};

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
		"XBox1", 	/* application class name */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

	box = XtVaCreateManagedWidget(
		"box", 	/* widget name */
		boxWidgetClass,	/* widget class */
		topLevel,	/* parent widget*/
		NULL);	/* argument list*/

	quit = XtVaCreateManagedWidget(
		"quit",	/* widget name */
		commandWidgetClass,	/* widget class */
		box,	/* parent widget*/
		NULL);	/* argument list*/

	pressme = XtVaCreateManagedWidget(
		"pressme",	/* widget name	 */
		commandWidgetClass,	/* widget class */
		box,	/* parent widget*/
		NULL);	/* argument list*/

	XtAddCallback(quit, XtNcallback, Quit, 0);
	XtAddCallback(pressme, XtNcallback, PressMe, &stuff);

	XtRealizeWidget(topLevel);

	XtAppMainLoop(app_context);
}
