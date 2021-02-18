/* 
 *  xrowcolumn.c - simple button box
 */

/*
 *  So that we can use fprintf:
 */
#include <stdio.h>

/* 
 * Standard Toolkit include files:
 */
#include <Xm/Xm.h>

/*
 * Public include files for widgets used in this file.
 */
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>

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
    Widget rowColumn, quit, pressme, topLevel;

    topLevel = XtVaAppInitialize(
            &app_context,       /* Application context */
            "XTryAccel",            /* Application class */
            NULL, 0,            /* command line option list */
            &argc, argv,        /* command line args */
            NULL,               /* for missing app-defaults file */
            NULL);              /* terminate varargs list */

    rowColumn = XtVaCreateManagedWidget(
            "rowColumn",                /* widget name */
            xmRowColumnWidgetClass,     /* widget class */
            topLevel,           /* parent widget*/
            NULL);              /* terminate varargs list */
        
    quit = XtVaCreateManagedWidget(
            "quit",             /* widget name */
            xmPushButtonWidgetClass,    /* widget class */
            rowColumn,              /* parent widget*/
            NULL);              /* terminate varargs list */
        

    pressme = XtVaCreateManagedWidget(
            "pressme",          /* widget name */
            xmPushButtonWidgetClass,    /* widget class */
            rowColumn,              /* parent widget*/
            NULL);              /* terminate varargs list */

	XtInstallAccelerators(rowColumn, quit);
	XtInstallAccelerators(rowColumn, pressme);

    XtAddCallback(quit, XmNactivateCallback, Quit, 0);
    XtAddCallback(pressme, XmNactivateCallback, PressMe, 0);

    XtRealizeWidget(topLevel);

    XtAppMainLoop(app_context);
}
