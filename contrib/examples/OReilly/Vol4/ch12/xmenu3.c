/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/* 
 *  xmenu3.c - pulldown menu invoked with MenuButton widget
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
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>

Widget pshell;

/*
 * quit button callback function
 */
/* ARGSUSED */
void Quit(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{ 
    exit(0); 
}

/*
 * menu pane button callback function
 */
/* ARGSUSED */
void PaneChosen(w, client_data, call_data)
Widget w;
XtPointer client_data;  /* cast to pane_number */
XtPointer call_data;    /* unused */
{ 
    int pane_number = (int) client_data;
    printf("Pane %d chosen.\n", pane_number);
    /* popdown the parent of the parent (pshell) */
    XtPopdown(pshell);
}

main(argc, argv)
int argc;
char **argv;
{
    XtAppContext app_context;
    Widget topLevel, box, pressme, quit, menulabel, menubox, menupane[10];
    int i;
    String buf[50];

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
        "XMenu3",   /* application class name */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

    box = XtCreateManagedWidget(
        "box",  /* widget name */
        boxWidgetClass, /* widget class */
        topLevel,   /* parent widget*/
        NULL,   /* argument list*/
        0   /* arglist size */
        );

    quit = XtCreateManagedWidget(
        "quit", /* widget name */
        commandWidgetClass, /* widget class */
        box,    /* parent widget*/
        NULL,   /* argument list*/
        0   /* arglist size */
        );

    pressme = XtVaCreateManagedWidget(
        "pressme",  /* widget name   */
        menuButtonWidgetClass,  /* widget class */
        box,    /* parent widget*/
        XtNmenuName, "pshell",
        NULL);

    pshell = XtCreatePopupShell(
        "pshell",
        transientShellWidgetClass,
        pressme,
        NULL,
        0
        );

    menubox = XtCreateManagedWidget(
        "menubox",               /* widget name   */
        boxWidgetClass,              /* widget class */
        pshell,                         /* parent widget*/
        NULL,                    /* argument list*/
        0           /* arglist size */
    );
    
    menulabel = XtCreateManagedWidget(
        "menulabel",               /* widget name   */
        labelWidgetClass,              /* widget class */
        menubox,                         /* parent widget*/
        NULL,                    /* argument list*/
        0           /* arglist size */
        );

    for (i = 0; i < 10; i++) {
        sprintf(buf, "menupane%d", i);
            menupane[i] = XtCreateManagedWidget(buf,   /* widget name   */
                    commandWidgetClass, menubox, NULL, 0);

        XtAddCallback(menupane[i], XtNcallback, PaneChosen, i);
    }

    XtAddCallback(quit, XtNcallback, Quit, 0);

    XtRealizeWidget(topLevel);

    XtAppMainLoop(app_context);
}
