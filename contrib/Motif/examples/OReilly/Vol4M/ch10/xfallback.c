/*
 * xfallback.c - simple program to demonstrate fallback resources
 */

/*
 * Include files required for all Toolkit programs
 */
#include <Xm/Xm.h>     /* Intrinsics Definitions*/

/*
 * Public include file for widgets we actually use in this file.
 */
#include <Xm/Label.h>     /* Motif Label Widget */

main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_context;
    Widget topLevel, hello;

	static String fallback_resources[] = {
		"*hello.labelString: App defaults file not installed",
		"*hello.fontList: *courier-bold*18*iso8859-1",
		NULL,		/* MUST BE NULL TERMINATED */
	};

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
        "NoFile",         /* Application class */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        fallback_resources,  /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

	hello = XtVaCreateManagedWidget(
		"hello",			/* arbitrary widget name */
		xmLabelWidgetClass,	/* widget class from Label.h */
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
