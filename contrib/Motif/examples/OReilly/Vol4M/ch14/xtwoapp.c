/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */

/*
 * xtwoapp.c
 */

/*
 * Include files required for all Toolkit programs
 */
#include <Xm/Xm.h>	/* Intrinsics Definitions */

/*
 * Public include file for widgets we actually use in this file.
 */
#include <Xm/Label.h>		/* Motif Label Widget */


/* 
 * This works, but is not elegant, since it ruins Xlib's
 * network optimizations.
 */
void OurMainLoop(app1, app2, dis1, dis2)
    XtAppContext app1, app2;
	Display *dis1, *dis2;
{
    XEvent event;

    for (;;) {
		if (XtAppPeekEvent(app1, &event) == TRUE) {
			XtAppNextEvent(app1, &event);
			XtDispatchEvent(&event);
			if (XtAppPeekEvent(app2, &event) == TRUE) {
				XtAppNextEvent(app2, &event);
				XtDispatchEvent(&event);
			}
			else XFlush(dis2);
		}
		else {
			XFlush(dis1);
			if (XtAppPeekEvent(app2, &event) == TRUE) {
				XtAppNextEvent(app2, &event);
				XtDispatchEvent(&event);
			}
			else XFlush(dis2);
		}
    }
}

main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_con1, app_con2;
	Widget topLevel1, topLevel2, hello, goodbye;
	Display *display1, *display2;

	XtToolkitInitialize();

	app_con1 = XtCreateApplicationContext();
	app_con2 = XtCreateApplicationContext();

	display1 = XtOpenDisplay(app_con1, "spike:0", argv[0], "XHello", NULL, 0, &argc, argv);

	display2 = XtOpenDisplay(app_con2, "ncd:0", argv[0], "XGoodbye", NULL, 0, &argc, argv);

	topLevel1 = XtAppCreateShell(app_con1, "XHello", applicationShellWidgetClass, display1, NULL, 0);

	topLevel2 = XtAppCreateShell(app_con2, "XGoodbye", applicationShellWidgetClass, display2, NULL, 0);

	hello = XtCreateManagedWidget(
		"hello",		/* arbitrary widget name */
		xmLabelWidgetClass,	/* widget class from Label.h */
		topLevel1,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	goodbye = XtCreateManagedWidget(
		"goodbye",		/* arbitrary widget name */
		xmLabelWidgetClass,	/* widget class from Label.h */
		topLevel2,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);
	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel1);
	XtRealizeWidget(topLevel2);

	/*
	 *  Loop for events.
	 *  Notice that since XtAppMainLoop only handles on
     *  app context, we have to write our own juggling loop.
     *  This is not easy (just an attempt made here).
	 */
	OurMainLoop(app_con1, app_con2, display1, display2);
}
