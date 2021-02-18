/*
 * xgetstring.c - simple program to put up a banner on the display
 *                Demonstrates querying a compound string.
 */

#include <stdio.h>
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
	String string;
    XmString      text;
    Widget hello;

    topLevel = XtVaAppInitialize(
            &app_context,       /* Application context */
            "XHello",         /* Application class */
            NULL, 0,            /* command line option list */
            &argc, argv,        /* command line args */
            NULL,               /* for missing app-defaults file */
            NULL);              /* terminate varargs list */

	hello = XtVaCreateManagedWidget(
		    "hello",			/* arbitrary widget name */
		    xmPushButtonWidgetClass,	/* widget class from PushButton.h */
		    topLevel,			/* parent widget */
            NULL);              /* terminate varargs list */

    XtVaGetValues(hello,
		    XmNlabelString, &text,
		    NULL);

    (void) XmStringGetLtoR(text, XmSTRING_DEFAULT_CHARSET, &string);
	printf("The string set in the app-defaults file is: %s\n", string);

	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	/*
	 *  Loop for events.
	 */
	XtAppMainLoop(app_context);
}
