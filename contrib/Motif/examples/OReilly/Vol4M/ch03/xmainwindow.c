/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */

/* 
 *  xmainwindow.c - main window with help and quit
 */

/*
 *  So that we can use fprintf:
 */
#include <stdio.h>

/* 
 * Standard Toolkit include files:
 */
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

/*
 * Public include files for widgets used in this file.
 */
#include <Xm/PushB.h>
#include <Xm/MainW.h>
/* #include <Xm/MessageB.h> */
#include <Xm/SelectioB.h>
#include <Xm/CascadeB.h>
#include <Xm/Frame.h>

/* 
 * callback to pop up help dialog widget 
 */
/*ARGSUSED*/
void ShowHelp(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
    Widget dialog = (Widget) client_data;
    XtManageChild(dialog);
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
    Widget topLevel, mainWindow, menuBar, frame;
    Widget fileButton, fileMenu, quit, getHelp, helpBox;
    Widget temp;
	int i;
	static char *items[] =  {
		"Beginner 1",
		"Beginner 2",
		"Beginner 3",
		"Intermediate 1",
		"Intermediate 2",
		"Intermediate 3",
		"Advanced 1",
		"Advanced 2",
		"Advanced 3",
	};

	static XmString xmstring[XtNumber(items)];

	for (i = 0; i < XtNumber(items); i++)
		xmstring[i] = XmStringCreateSimple(items[i]);

    topLevel = XtVaAppInitialize(
            &app_context,       /* Application context */
            "XMainWindow",      /* application class name */
            NULL, 0,            /* command line option list */
            &argc, argv,        /* command line args */
            NULL,               /* for missing app-defaults file */
            NULL);              /* terminate varargs list */

    /* create main window */
    mainWindow = XtVaCreateManagedWidget(
            "mainWindow",   /* widget name */
            xmMainWindowWidgetClass,    /* widget class */
            topLevel,   /* parent widget*/
            NULL);              /* terminate varargs list */

    /* create menu bar along top inside of main window */
    menuBar = XmCreateMenuBar(
            mainWindow, /* parent widget*/
            "menuBar",  /* widget name */
            NULL,       /* no arguments needed */
            0);         /* no arguments needed */
    XtManageChild(menuBar);

    frame = XtVaCreateManagedWidget(
            "frame",    /* widget name */
            xmFrameWidgetClass, /* widget class */
            mainWindow, /* parent widget*/
            NULL);              /* terminate varargs list */

    /*  Set MainWindow areas and add tab groups */
    XmMainWindowSetAreas (mainWindow, menuBar, NULL, NULL, NULL,
            frame);

    /*
     *  CREATE FILE MENU AND CHILDREN
     */

    /* create button that will pop up the menu */
    fileButton = XtVaCreateManagedWidget(
            "fileButton",   /* widget name */
            xmCascadeButtonWidgetClass, /* widget class */
            menuBar,    /* parent widget*/
            NULL);              /* terminate varargs list */

    /* create menu (really a Shell widget and RowColumn widget combo) */
    fileMenu = XmCreatePulldownMenu(
            menuBar,    /* parent widget*/
            "fileMenu", /* widget name */
            NULL,       /* no argument list needed */
            0);         /* no argument list needed */

    /* create the quit button up in the menu */
    quit = XtVaCreateManagedWidget(
            "quit", /* widget name */
            xmPushButtonWidgetClass,    /* widget class */
            fileMenu,   /* parent widget*/
            NULL);              /* terminate varargs list */

    /* 
     * Specify which menu fileButton will pop up.
     */
    XtVaSetValues(fileButton,
            XmNsubMenuId, fileMenu,
            NULL);

    /* arrange for quit button to call function that exits. */
    XtAddCallback(quit, XmNactivateCallback, Quit, 0);

    /*
     *  CREATE HELP BUTTON AND BOX
     */

    /* create button that will bring up help popup */
    getHelp = XtVaCreateManagedWidget(
            "getHelp",  /* widget name */
            xmCascadeButtonWidgetClass, /* widget class */
            menuBar,    /* parent widget*/
            NULL);              /* terminate varargs list */

    /* tell menuBar which is the help button 
     * (will be specially positioned. */
    XtVaSetValues(menuBar,
            XmNmenuHelpWidget, getHelp,
            NULL);

    /* create popup that will contain help */
    helpBox = XmCreateSelectionDialog(
            getHelp,    /* parent widget*/
            "helpBox",  /* widget name   */
            NULL,       /* no arguments needed */
            0);         /* no arguments needed */

	XtVaSetValues(helpBox,
		XmNlistItems, xmstring,
		XmNlistItemCount, XtNumber(xmstring),
		NULL);

    /* temp = XmMessageBoxGetChild (helpBox, XmDIALOG_CANCEL_BUTTON);
    XtUnmanageChild (temp);
    temp = XmMessageBoxGetChild (helpBox, XmDIALOG_HELP_BUTTON);
    XtUnmanageChild (temp);
	*/


    /* arrange for getHelp button to pop up helpBox */
    XtAddCallback(getHelp, XmNactivateCallback, ShowHelp, helpBox);


    XtRealizeWidget(topLevel);

    XtAppMainLoop(app_context);
}
