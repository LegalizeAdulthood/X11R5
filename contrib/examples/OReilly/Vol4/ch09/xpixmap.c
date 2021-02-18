/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xpixmap.c - simple program to test invoking a converter directly
 */

#include <X11/IntrinsicP.h>	/* temporary, until we find out how
				 * to register a type converter without
				 * referencing the core structure */

/*
 * Include files required for all Toolkit programs
 */
#include <X11/Intrinsic.h>	/* Intrinsics Definitions */
#include <X11/StringDefs.h>	/* Standard Name-String definitions */

/*
 * Public include file for widgets we actually use in this file.
 */
#include <X11/Xaw/Label.h>		/* Athena Label Widget */

#define OURPIXMAPNAME "xlogo64"

main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_context;
	Widget topLevel, hello;
	XrmValue from, to;
	Pixmap pixmap;
	void XmuCvtStringToBitmap();

	static XtConvertArgRec screenConvertArg[] = {
		{
		XtBaseOffset, 
		(XtPointer) XtOffset(Widget, core.screen), 
		sizeof(Screen *)
		}
	};


    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
        "XHello",         /* Application class */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

	XtAddConverter(XtRString, XtRBitmap, XmuCvtStringToBitmap,
			screenConvertArg, XtNumber(screenConvertArg));

    from.addr = OURPIXMAPNAME;
	from.size = strlen(OURPIXMAPNAME);

	XtConvert(topLevel, XtRString, &from, XtRBitmap, &to);

	if (to.addr == NULL)
		exit(1);
	
	pixmap = *(Pixmap *) to.addr;


	hello = XtVaCreateManagedWidget(
		"hello",		/* arbitrary widget name */
		labelWidgetClass,	/* widget class from Label.h */
		topLevel,	/* parent widget*/
		XtNbitmap, pixmap,
		NULL);

	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	/*
	 *  Loop for events.
	 */
	XtAppMainLoop(app_context);
}
