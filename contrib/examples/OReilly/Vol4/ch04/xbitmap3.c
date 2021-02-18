/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xbitmap3.c
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/Form.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>

#include "BitmapEdit.h"

#include <stdio.h>

#define DRAWN 1
#define UNDRAWN 0

GC draw_gc, undraw_gc;
Pixmap normal_bitmap, reverse_bitmap;
Widget bigBitmap, showNormalBitmap, showReverseBitmap;
Dimension pixmap_width_in_cells, pixmap_height_in_cells;

static void Cell_toggled();

String filename;	/* filename to read and write */

/* ARGSUSED */
static void 
Printout(widget, client_data, call_data)
Widget widget;
XtPointer client_data, call_data;	/* unused */
{
    XWriteBitmapFile(XtDisplay(widget), filename, normal_bitmap,
	    pixmap_width_in_cells, pixmap_height_in_cells, 0, 0);
}

/*ARGSUSED*/
static void
Redraw_small_picture(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
	Pixmap pixmap;

	if (w == showNormalBitmap)
		pixmap = normal_bitmap;
	else
		pixmap = reverse_bitmap;

        if (DefaultDepthOfScreen(XtScreen(w)) == 1)
 	    XCopyArea(XtDisplay(w), pixmap, XtWindow(w),
			DefaultGCOfScreen(XtScreen(w)), 0, 0, 
			pixmap_width_in_cells, pixmap_height_in_cells, 0, 0);
        else
 	    XCopyPlane(XtDisplay(w), pixmap, XtWindow(w),
     			DefaultGCOfScreen(XtScreen(w)), 0, 0, 
			pixmap_width_in_cells, pixmap_height_in_cells, 0, 0, 1);
}

main(argc, argv)
int argc;
char *argv[];
{
	XtAppContext app_context;
    Widget topLevel, form, buttonbox, quit, output;
    extern exit();
    static XtActionsRec window_actions[] = {
	{"redraw_small_picture", Redraw_small_picture}
    };

    String trans =
	"<Expose>:	redraw_small_picture()";

    static XrmOptionDescRec table[] = 
    {
        {"-pw",            "*pixmapWidthInCells",        XrmoptionSepArg, NULL},
        {"-pixmapwidth",   "*pixmapWidthInCells",        XrmoptionSepArg, NULL},
        {"-ph",            "*pixmapHeightInCells",       XrmoptionSepArg, NULL},
        {"-pixmapheight",  "*pixmapHeightInCells",       XrmoptionSepArg, NULL},
        {"-cellsize",      "*cellSizeInPixels",           XrmoptionSepArg, NULL},
    };
    
    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
        "XBitmap3",         /* Application class */
        table, XtNumber(table),   /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

    if (argv[1] != NULL)
         filename = argv[1];
    else {
         fprintf(stderr, "xbitmap: must specify filename on command line\n");
         exit(1);
    }

    form = XtCreateManagedWidget("form", formWidgetClass, topLevel, NULL, 0);

    buttonbox = XtCreateManagedWidget("buttonbox", boxWidgetClass, form, NULL, 0);

    output = XtCreateManagedWidget("output", commandWidgetClass, buttonbox, NULL, 0);

    XtAddCallback(output, XtNcallback, Printout, NULL);

    quit = XtCreateManagedWidget("quit", commandWidgetClass, buttonbox, NULL, 0);

    XtAddCallback(quit, XtNcallback, exit, NULL);

    XtAppAddActions(app_context, window_actions, XtNumber(window_actions));

    bigBitmap = XtCreateManagedWidget("bigBitmap", bitmapEditWidgetClass, 
			form, NULL, 0);

    XtAddCallback(bigBitmap, XtNcallback, Cell_toggled, NULL);

    XtVaGetValues(bigBitmap, 
		XtNpixmapHeightInCells, &pixmap_height_in_cells,
		XtNpixmapWidthInCells, &pixmap_width_in_cells,
		NULL);

    set_up_things(topLevel);

    showNormalBitmap = XtVaCreateManagedWidget("showNormalBitmap", 
		widgetClass, buttonbox, 
		XtNwidth, pixmap_width_in_cells,
		XtNheight, pixmap_height_in_cells,
		XtNtranslations, XtParseTranslationTable(trans),
		NULL);

    showReverseBitmap = XtVaCreateManagedWidget("showReverseBitmap", 
		widgetClass, buttonbox, 
		XtNwidth, pixmap_width_in_cells,
		XtNheight, pixmap_height_in_cells,
		XtNtranslations, XtParseTranslationTable(trans),
		NULL);

    XtRealizeWidget(topLevel);
	XtAppMainLoop(app_context);
}

set_up_things(w)
Widget w;
{
	XGCValues values;


	normal_bitmap = XCreatePixmap(XtDisplay(w), 
	    RootWindowOfScreen(XtScreen(w)),
	    pixmap_width_in_cells, pixmap_height_in_cells, 1);

	reverse_bitmap = XCreatePixmap(XtDisplay(w), 
	    RootWindowOfScreen(XtScreen(w)),
	    pixmap_width_in_cells, pixmap_height_in_cells, 1);

	values.foreground = 1;
	values.background = 0;
	/* note that normal_bitmap is used as the drawable because it
	 * is one bit deep.  The root window may not be one bit deep */
	draw_gc = XCreateGC(XtDisplay(w),  normal_bitmap,
	        GCForeground | GCBackground, &values);

	values.foreground = 0;
	values.background = 1;
	undraw_gc = XCreateGC(XtDisplay(w), normal_bitmap,
	        GCForeground | GCBackground, &values);

	XFillRectangle(XtDisplay(w), reverse_bitmap, draw_gc,
   	    	0, 0, pixmap_width_in_cells + 1, pixmap_height_in_cells + 1);
	XFillRectangle(XtDisplay(w), normal_bitmap, undraw_gc,
   	    	0, 0, pixmap_width_in_cells + 1, pixmap_height_in_cells + 1);
}

/* ARGSUSED */
static void
Cell_toggled(w, client_data, call_data)
Widget w;
XtPointer client_data;	/* unused */
XtPointer call_data;	/* will be cast to cur_info */
{
    /* cast pointer to needed type: */
    BitmapEditPointInfo *cur_info = (BitmapEditPointInfo *) call_data;
    /* 
     * Note, BitmapEditPointInfo is defined in BitmapEdit.h 
     */

    XDrawPoint(XtDisplay(w), normal_bitmap, ((cur_info->mode == DRAWN) ? draw_gc : undraw_gc), cur_info->newx, cur_info->newy);
    XDrawPoint(XtDisplay(w), reverse_bitmap, ((cur_info->mode == DRAWN) ? undraw_gc : draw_gc), cur_info->newx, cur_info->newy); 

    Redraw_small_picture(showNormalBitmap);
    Redraw_small_picture(showReverseBitmap);
}
