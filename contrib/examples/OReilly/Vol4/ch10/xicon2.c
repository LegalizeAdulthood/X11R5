/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xicon2.c
 */

/*
 * Include files required for all Toolkit programs
 */
#include <X11/Intrinsic.h>	/* Intrinsics Definitions */
#include <X11/IntrinsicP.h>	/* For core structure in XtOffset */
#include <X11/StringDefs.h>	/* Standard Name-String definitions */

/*
 * Public include file for widgets we actually use in this file.
 */
#include <X11/Xaw/Label.h>		/* Athena Label Widget */

#include <X11/Shell.h>

#include <stdio.h>

main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_context;
	Widget topLevel, hello;
	XrmValue from, to;
	static char icon_file_name[] = "./icon.bit";
	Arg arg;

	static XtConvertArgRec screenConvertArg[] = {
		{
		XtBaseOffset,
		(XtPointer)XtOffset(Widget, core.screen),
		sizeof(Screen *)
		}
	};

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
		"XIcon2",           /* Application class */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

	XtAppAddConverter(app_context, XtRString, XtRBitmap, XmuCvtStringToBitmap,
		screenConvertArg, XtNumber(screenConvertArg));

	from.size = strlen(icon_file_name);
	from.addr = icon_file_name;

	XtConvert(topLevel, XtRString, &from, XtRBitmap, &to);

	if (to.addr != NULL) {
        	XtSetArg(arg, XtNiconPixmap, *(Pixmap *)to.addr);
        	XtSetValues(topLevel, &arg, 1);
	}
	else
		fprintf(stderr, "xhello: unable to find icon pixmap file.\n");

	hello = XtCreateManagedWidget(
		"hello",		/* arbitrary widget name */
		labelWidgetClass,	/* widget class from Label.h */
		topLevel,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	/*
	 *  Loop for events.
	 */
	XtAppMainLoop(app_context);
}

