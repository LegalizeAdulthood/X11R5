/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/* 
 *  xmenu2.c - pop up using command (placement code).
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
#include <X11/Xaw/Label.h>

/*
 * The popup shell ID is global because both dialog and pshell
 * are needed in the dialogDone callback, and both can't be
 * passed in without creating a structure.
 */
Widget pshell, pressme;

/*ARGSUSED*/
void PlaceMenu(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
    Position x, y;
	Dimension height;

    /*
     * translate coordinates in application top-level window
     * into coordinates from root window origin.
     */
    XtTranslateCoords(pressme,                /* Widget */
            (Position) 0,        /* x */
            (Position) 0,       /* y */
            &x, &y);          /* coords on root window */

	/* get height of pressme so that menu is positioned below */
    XtVaGetValues(pressme, 
            XtNheight, &height,
            NULL);

    /* move popup shell one pixel above and left of this position 
     * (it's not visible yet) */
    XtVaSetValues(pshell, 
            XtNx, x - 1,   /* -1 accounts for border width */
            XtNy, y + height,
            NULL);
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

/*
 * menu pane button callback function
 */
/*ARGSUSED*/
void PaneChosen(w, client_data, call_data)
Widget w;
XtPointer client_data;  /* cast to pane_number */
XtPointer call_data;  /* unused */
{ 
    int pane_number = (int) client_data;
    printf("Pane %d chosen.\n", pane_number);
    XtPopdown(pshell);
}

main(argc, argv)
int argc;
char **argv;
{
    XtAppContext app_context;
    Widget topLevel, box, quit, menubox, menupane[10];
    int i;
    String buf[50];

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
        "XMenu2",   /* application class name */
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

    pressme = XtCreateManagedWidget(
        "pressme",  /* widget name   */
        commandWidgetClass, /* widget class */
        box,    /* parent widget*/
        NULL,   /* argument list*/
        0   /* arglist size */
        );

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
    
    for (i = 0; i < 10; i++) {
        sprintf(buf, "menupane%d", i);
            menupane[i] = XtCreateManagedWidget(buf,   /* widget name   */
                    commandWidgetClass, menubox, NULL, 0);

        XtAddCallback(menupane[i], XtNcallback, PaneChosen, i);
    }

    XtAddCallback(quit, XtNcallback, Quit, 0);

    XtAddCallback(pshell, XtNpopupCallback, PlaceMenu, NULL);

    XtRealizeWidget(topLevel);

    XtAppMainLoop(app_context);
}
