/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xbitmap5.c
 */
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/Form.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>

#include "BitmapEdit.h"

Dimension pixmap_width_in_cells, pixmap_height_in_cells;

/*
 * The Printout routine prints an array of 1s and 0s representing the
 * contents of the bitmap.  This data can be processed into any
 * desired form, including standard X11 bitmap file format.
 */
/* ARGSUSED */
static void 
Printout(widget, client_data, call_data)
Widget widget;
XtPointer client_data;	/* cast to bigBitmap */
XtPointer call_data;	/* unused */
{
	Widget bigBitmap = (Widget) client_data;
	int x, y;
	char *cell;
	cell = BitmapEditGetArrayString(bigBitmap);

	(void) putchar('\n');
	for (y = 0; y < pixmap_height_in_cells; y++) {
		for (x = 0; x < pixmap_width_in_cells; x++)
			(void) putchar(cell[x + y * pixmap_width_in_cells] ? '1' : '0');
		(void) putchar('\n');
	}
	(void) putchar('\n');
}

main(argc, argv)
int argc;
char *argv[];
{
	XtAppContext app_context;
	Widget topLevel, form, buttonbox, quit, output, viewport, bigBitmap;
	extern exit();

	static XrmOptionDescRec table[] = {
		{"-pw",            "*pixmapWidthInCells",        XrmoptionSepArg, NULL},
		{"-pixmapwidth",   "*pixmapWidthInCells",        XrmoptionSepArg, NULL},
		{"-ph",            "*pixmapHeightInCells",       XrmoptionSepArg, NULL},
		{"-pixmapheight",  "*pixmapHeightInCells",       XrmoptionSepArg, NULL},
		{"-cellsize",      "*cellSizeInPixels",           XrmoptionSepArg, NULL},
	};
    

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
        "XBitmap5",         /* Application class */
        table, XtNumber(table),   /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

	form = XtCreateManagedWidget("form", formWidgetClass, topLevel, NULL, 0);

	buttonbox = XtCreateManagedWidget("buttonbox", boxWidgetClass, form, NULL, 0);

	output = XtCreateManagedWidget("output", commandWidgetClass, buttonbox, NULL, 0);

	/* call to add callback below, after big bitmap created */

	quit = XtCreateManagedWidget("quit", commandWidgetClass, buttonbox, NULL, 0);

	XtAddCallback(quit, XtNcallback, exit, NULL);

	viewport = XtCreateManagedWidget("viewport", viewportWidgetClass, form, NULL, 0);

	bigBitmap = XtCreateManagedWidget("bigBitmap", bitmapEditWidgetClass, viewport, NULL, 0);

	XtAddCallback(output, XtNcallback, Printout, bigBitmap);

	/* need the following values for the Printout routine. */
	XtVaGetValues(bigBitmap, 
			XtNpixmapWidthInCells, &pixmap_width_in_cells,
			XtNpixmapHeightInCells, &pixmap_height_in_cells,
			NULL);

	XtRealizeWidget(topLevel);
	XtAppMainLoop(app_context);
}
