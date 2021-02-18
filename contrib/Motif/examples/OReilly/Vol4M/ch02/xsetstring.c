/*
 * xsetstring.c - simple program to put up a banner on the display
 *                Demonstrates setting a compound string.
 */

/*
 * Header files required for all Toolkit programs
 */
#include <X11/Intrinsic.h>     /* Intrinsics definitions */
#include <Xm/Xm.h>    /* Standard Motif definitions */

/*
 * Public header file for widgets we actually use in this file.
 */
#include <Xm/PushB.h>     /* Motif PushButton Widget */


main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_context;
    Widget topLevel;
    Widget hello;
	static String greeting = "Hello, World!";

    topLevel = XtVaAppInitialize(
            &app_context,       /* Application context */
            "XHello",         /* Application class */
            NULL, 0,            /* command line option list */
            &argc, argv,        /* command line args */
            NULL,               /* for missing app-defaults file */
            NULL);              /* terminate varargs list */

    /* text = XmStringCreateLtoR("Hello World", XmSTRING_DEFAULT_CHARSET); */
    /* text = XmStringCreateSimple("Hello World"); */

	hello = XtVaCreateManagedWidget(
		    "hello",			/* arbitrary widget name */
		    xmPushButtonWidgetClass,	/* widget class from PushButton.h */
		    topLevel,			/* parent widget */
      XtVaTypedArg,     XmNlabelString,
                 XmRString, greeting, strlen(greeting)+1,

            /* XmNlabelString, text, */
            XmNalignment, XmALIGNMENT_END,
            NULL);              /* terminate varargs list */

	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	/*
	 *  Loop for events.
	 */
	XtAppMainLoop(app_context);
}
