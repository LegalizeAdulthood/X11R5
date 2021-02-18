/*
 * xcomstring.c - simple test of compound strings, modifying fonts
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
    XmString      text;
    XmString      s1, s2, s3, s4;
    Widget hello;
	static String string1 = "Specify the ",
                  string2 = "character set ",
                  string3 = "in the code.";

    topLevel = XtVaAppInitialize(
            &app_context,       /* Application context */
            "XComstring",         /* Application class */
            NULL, 0,            /* command line option list */
            &argc, argv,        /* command line args */
            NULL,               /* for missing app-defaults file */
            NULL);              /* terminate varargs list */

    s1=XmStringSegmentCreate(string1,"chset1",XmSTRING_DIRECTION_L_TO_R,
            False);
    s2=XmStringSegmentCreate(string2,"chset2",XmSTRING_DIRECTION_L_TO_R,
            False);
    s3=XmStringSegmentCreate(string3,"chset1",XmSTRING_DIRECTION_L_TO_R,
            True);   /* True indicates add separator */

    s4 = XmStringConcat(s1, s2);
    XmStringFree(s1);
    XmStringFree(s2);

    text = XmStringConcat(s4, s3);
    XmStringFree(s3);
    XmStringFree(s4);

	hello = XtVaCreateManagedWidget(
		    "hello",			/* arbitrary widget name */
		    xmPushButtonWidgetClass,	/* widget class from PushButton.h */
		    topLevel,			/* parent widget */
            XmNlabelString, text,
            NULL);              /* terminate varargs list */

    XmStringFree(text);

	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	/*
	 *  Loop for events.
	 */
	XtAppMainLoop(app_context);
}
