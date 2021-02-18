/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */

/* 
 *  xbox4.c
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

XtCallbackRec quit_callback_list[]={
	{Quit, 0},
	{(XtCallbackProc) NULL, (XtPointer) NULL},
};

/*
 * Note: callback list comes before argument list, because
 * callback list must be declared before it is used in the argument 
 * list.
 */
Arg quit_button_args[] = {
	XtNcallback, (XtArgVal) quit_callback_list,
};
/*
 * End of quit button stuff
 */

/*
 * "Press me!" button callback function
 */
/*ARGSUSED*/
void Pressme(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{ 
	fprintf(stderr, "Thankyou!\n"); 
}

XtCallbackRec pressme_callback_list[]={
	{Pressme, 0},
	{(XtCallbackProc) NULL, (XtPointer) NULL},
};

Arg pressme_args[] = {
	XtNcallback, (XtArgVal) pressme_callback_list,
};
/*
 * End of press me button stuff.
 */

main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_context;
	Widget box, quit, pressme, topLevel;

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
		"XBox4", 	/* application class name */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

	box = XtVaCreateManagedWidget(
		"box", 	/* widget name */
		boxWidgetClass,	/* widget class */
		topLevel,	/* parent widget*/
		NULL);

	quit = XtCreateManagedWidget(
		"quit",	/* widget name */
		commandWidgetClass,	/* widget class */
		box,	/* parent widget*/
		quit_button_args,	/* argument list*/
		XtNumber(quit_button_args)	/* arglist size */
		);

	pressme = XtCreateManagedWidget(
		"pressme",	/* widget name	 */
		commandWidgetClass,	/* widget class */
		box,	/* parent widget*/
		pressme_args,	/* argument list*/
		XtNumber(pressme_args)	/* arglist size */
		);

	XtRealizeWidget(topLevel);

	XtAppMainLoop(app_context);
}
