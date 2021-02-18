/* 
 *  xbox1.c - simple button box
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

/*
 * "Press me!" button callback function
 */
/*ARGSUSED*/
void PressMe(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{ 
	fprintf(stderr, "Thankyou!\n"); 
}

main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_context;
	Widget box, quit, pressme, topLevel;

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
        "XBox1",            /* Application class */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

	box = XtVaCreateManagedWidget(
		"box",				/* widget name */
		boxWidgetClass,		/* widget class */
		topLevel,			/* parent widget*/
		NULL);				/* terminate varargs list */
		
	quit = XtVaCreateManagedWidget(
		"quit",				/* widget name */
		commandWidgetClass,	/* widget class */
		box,				/* parent widget*/
		NULL);				/* terminate varargs list */
		

	pressme = XtVaCreateManagedWidget(
		"pressme",			/* widget name */
		commandWidgetClass,	/* widget class */
		box,				/* parent widget*/
		NULL);				/* terminate varargs list */
		

	XtAddCallback(quit, XtNcallback, Quit, 0);
	XtAddCallback(pressme, XtNcallback, PressMe, 0);

	XtRealizeWidget(topLevel);

	XtAppMainLoop(app_context);
}
