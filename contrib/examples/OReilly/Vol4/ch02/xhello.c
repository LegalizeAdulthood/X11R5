/*
 * xhello.c - simple program to put up a banner on the display
 */

/*
 * Include files required for all Toolkit programs
 */
#include <X11/Intrinsic.h>     /* Intrinsics Definitions*/
#include <X11/StringDefs.h>    /* Standard Name-String definitions*/

/*
 * Public include file for widgets we actually use in this file.
 */
#include <X11/Xaw/Label.h>     /* Athena Label Widget */

main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_context;
    Widget topLevel, hello;

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
        "XHello",         /* Application class */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

	hello = XtVaCreateManagedWidget(
		"hello",			/* arbitrary widget name */
		labelWidgetClass,	/* widget class from Label.h */
		topLevel,			/* parent widget */
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
