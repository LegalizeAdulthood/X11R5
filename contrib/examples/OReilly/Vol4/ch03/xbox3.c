/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/* 
 *  xbox3.c - simple button box
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

#include <X11/Shell.h>

/*
 * Public include files for widgets used in this file.
 */
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Dialog.h>

/*
 * The popup shell ID is global because both dialog and pshell
 * are needed in the dialogDone callback, and both can't be
 * passed in without creating a structure.
 */
Widget pshell, pressme, quit;

/*
 * dialog button
 */
/*ARGSUSED*/
void PopupDialog(w, client_data, call_data)
Widget w;
XtPointer client_data; /* cast to topLevel */
XtPointer call_data;
{
	Widget topLevel = (Widget) client_data;
	Position x, y;
	Dimension width, height;

	/*
 	 * get the coordinates of the middle of topLevel widget.
 	 */
	XtVaGetValues(topLevel, 
			XtNwidth, &width,
			XtNheight, &height,
			NULL);
	
	/*
	 * translate coordinates in application top-level window
	 * into coordinates from root window origin.
	 */
        XtTranslateCoords(topLevel,                /* Widget */
                (Position) width/2,        /* x */
                (Position) height/2,       /* y */
                &x, &y);          /* coords on root window */

	/* move popup shell to this position (it's not visible yet) */
	XtVaSetValues(pshell, 
			XtNx, x,
			XtNy, y,
			NULL);

	/*
	 * Indicate to user that no other application functions are
	 * valid while dialog is popped up...
	 */
	XtSetSensitive(pressme, FALSE);
	XtSetSensitive(quit, FALSE);

	XtPopup(pshell, XtGrabNonexclusive);
}

/*
 * dialog done button
 */
/*ARGSUSED*/
void DialogDone(w, client_data, call_data)
Widget w;
XtPointer client_data; /* cast to dialog */
XtPointer call_data;
{
	Widget dialog = (Widget) client_data;

	String string;

	XtPopdown(pshell);

	XtSetSensitive(pressme, TRUE);
	XtSetSensitive(quit, TRUE);

	string = XawDialogGetValueString(dialog);

	printf("User typed: %s\n", string);
}

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

main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_context;
	Widget box, topLevel, dialog, dialogDone;

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
		"XBox3", 	/* application class name */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

	box = XtVaCreateManagedWidget(
		"box", 	/* widget name */
		boxWidgetClass,	/* widget class */
		topLevel,	/* parent widget*/
		NULL);              /* terminate varargs list */

	quit = XtVaCreateManagedWidget(
		"quit",	/* widget name */
		commandWidgetClass,	/* widget class */
		box,	/* parent widget*/
		NULL);              /* terminate varargs list */

	pressme = XtVaCreateManagedWidget(
		"pressme",	/* widget name	 */
		commandWidgetClass,	/* widget class */
		box,	/* parent widget*/
		NULL);              /* terminate varargs list */

	pshell = XtVaCreatePopupShell(
		"pshell",
		transientShellWidgetClass,
		topLevel,
		NULL);              /* terminate varargs list */

	dialog = XtVaCreateManagedWidget(
		"dialog",               /* widget name   */
		dialogWidgetClass,              /* widget class */
		pshell,                         /* parent widget*/
		NULL);              /* terminate varargs list */

	dialogDone = XtVaCreateManagedWidget(
		"dialogDone",           /* widget name   */
		commandWidgetClass,             /* widget class */
		dialog,                         /* parent widget*/
		NULL);              /* terminate varargs list */

	XtAddCallback(quit, XtNcallback, Quit, 0);

	XtAddCallback(dialogDone, XtNcallback, DialogDone, dialog);

	XtAddCallback(pressme, XtNcallback, PopupDialog, topLevel);

	XtRealizeWidget(topLevel);

	XtAppMainLoop(app_context);
}
