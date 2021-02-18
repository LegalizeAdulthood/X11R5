/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */

/* 
 *  xform.c
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
#include <Xm/Form.h>

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
    Widget form, quit, pressme, topLevel;

    topLevel = XtVaAppInitialize(
            &app_context,       /* Application context */
            "XForm",            /* application class name */
            NULL, 0,            /* command line option list */
            &argc, argv,        /* command line args */
            NULL,               /* for missing app-defaults file */
            NULL);              /* terminate varargs list */

    form = XtVaCreateManagedWidget(
            "form",     /* widget name */
            xmFormWidgetClass,  /* widget class */
            topLevel,   /* parent widget*/
            NULL);  /* argument list*/

    pressme = XtVaCreateManagedWidget(
            "pressme",  /* widget name   */
            xmPushButtonWidgetClass,    /* widget class */
            form,   /* parent widget*/
            NULL);  /* argument list*/

    quit = XtVaCreateManagedWidget(
            "quit", /* widget name */
            xmPushButtonWidgetClass,    /* widget class */
            form,   /* parent widget*/
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget, pressme,
            NULL);  /* argument list*/

    XtAddCallback(quit, XmNactivateCallback, Quit, 0);
    XtAddCallback(pressme, XmNactivateCallback, PressMe, 0);

    XtRealizeWidget(topLevel);

    XtAppMainLoop(app_context);
}
