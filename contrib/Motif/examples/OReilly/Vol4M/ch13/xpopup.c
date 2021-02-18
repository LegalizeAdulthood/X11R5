/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/* 
 *  xmenu1.c - simple spring-loaded menu
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
#include <Xm/PushBG.h>
#include <Xm/Label.h>


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
 * menu pane button callback function
 */
/*ARGSUSED*/
void PaneChosen(w, client_data, call_data)
Widget w;
XtPointer client_data;   /* cast to pane_number */
XtPointer call_data;
{ 
    int pane_number = (int) client_data;
    printf("Pane %d chosen.\n", pane_number);
}

static void  
PostMenu (w, client_data, event)
Widget         w;
XtPointer      client_data;
XEvent  *event;
{
    Widget popup = (Widget) client_data;
    XButtonEvent *bevent = (XButtonEvent *) event;

/*     if (bevent->button != Button3)
        return;
*/

    XmMenuPosition(popup, event);
    XtManageChild (popup);
}

main(argc, argv)
int argc;
char **argv;
{
    XtAppContext app_context;
    Widget topLevel, box, quit, label, menulabel, menubox, entry[10];
    Widget menu;
    int i;
    String buf[50];

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
        "XMenu1",   /* application class name */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

    box = XtCreateManagedWidget(
        "box",  /* widget name */
        xmRowColumnWidgetClass, /* widget class */
        topLevel,   /* parent widget*/
        NULL,   /* argument list*/
        0   /* arglist size */
        );

    label = XtCreateManagedWidget(
        "label",    /* widget name */
        xmLabelWidgetClass,   /* widget class */
        box,    /* parent widget*/
        NULL,   /* argument list*/
        0   /* arglist size */
        );

    quit = XtCreateManagedWidget(
        "quit", /* widget name */
        xmPushButtonWidgetClass, /* widget class */
        box,    /* parent widget*/
        NULL,   /* argument list*/
        0   /* arglist size */
        );

    XtAddCallback(quit, XmNactivateCallback, Quit, 0);

    menu = XmCreatePopupMenu(label, "menu", NULL, 0);

    XtAddEventHandler(label, ButtonPressMask, False, PostMenu, menu);

    for (i = 0; i < 10; i++) {
        sprintf(buf, "entry%d", i);
            entry[i] = XtCreateManagedWidget(buf,   /* widget name   */
                    xmPushButtonGadgetClass, menu, NULL, 0);

        XtAddCallback(entry[i], XmNactivateCallback, PaneChosen, i);
    }


    XtRealizeWidget(topLevel);

    XtAppMainLoop(app_context);
}
