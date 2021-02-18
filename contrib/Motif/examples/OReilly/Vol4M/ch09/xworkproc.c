/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/* 
 *  xworkproc.c
 */

#include <stdio.h>

/* 
 * Standard Toolkit include files:
 */
#include <Xm/Xm.h>

/*
 * Public include files for widgets used in this file.
 */
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/BulletinB.h>

Widget getHelp;

/*
 * popup dialog
 */
/*ARGSUSED*/
void PopupDialog(w, client_data, call_data)
Widget w;
XtPointer client_data;  /* cast to topLevel */
XtPointer call_data;
{
	Widget popup = (Widget) client_data;

	XtManageChild(popup);
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

/* work procedure */
Boolean
create_popup(client_data)
XtPointer client_data;
{
	Widget parent = (Widget) client_data;
	Widget helpBox;
	Widget temp;

	helpBox = XmCreateMessageDialog(parent, "message", NULL, 0);

    temp = XmMessageBoxGetChild (helpBox, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild (temp);
    temp = XmMessageBoxGetChild (helpBox, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild (temp);

    /* arrange for getHelp button to pop up helpBox */
    XtAddCallback(getHelp, XmNactivateCallback, PopupDialog, helpBox);

	return(True);	/* makes Xt remove this work proc automatically */ 
}

main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_context;
	Widget rowColumn, topLevel;
    Widget pshell, quit;

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
		"XWorkproc",        /* application class name */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

	rowColumn = XtVaCreateManagedWidget(
		"rowColumn", 	/* widget name */
		xmRowColumnWidgetClass,	/* widget class */
		topLevel,	/* parent widget*/
		NULL	/* terminate argument list*/
		);

	quit = XtVaCreateManagedWidget(
		"quit",	/* widget name */
		xmPushButtonWidgetClass,	/* widget class */
		rowColumn,	/* parent widget*/
		NULL	/* terminate argument list*/
		);

	getHelp = XtVaCreateManagedWidget(
		"getHelp",	/* widget name	 */
		xmPushButtonWidgetClass,	/* widget class */
		rowColumn,	/* parent widget*/
		NULL	/* terminate argument list*/
		);

	(void) XtAppAddWorkProc(app_context, create_popup, topLevel);

	XtAddCallback(quit, XmNactivateCallback, Quit, 0);

	XtRealizeWidget(topLevel);

	XtAppMainLoop(app_context);
}
