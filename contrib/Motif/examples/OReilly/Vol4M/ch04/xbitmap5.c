/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xbitmap5.c
 */
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include <Xm/PanedW.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/DrawingA.h>
#include <Xm/ScrolledW.h>

#include "BitmapEdit.h"

#include <stdio.h>

#define DRAWN 1
#define UNDRAWN 0

/* 
 * Data structure for private data.
 * (This avoids lots of global variables.)
 */
typedef struct {
    GC draw_gc, undraw_gc, invert_gc;
    Pixmap normal_bitmap;
    Widget showNormalBitmap, showReverseBitmap;
    Dimension pixmap_width_in_cells, pixmap_height_in_cells;
} PrivateAppData;

static void CellToggled(), SetUpThings();

PrivateAppData private_data;
String filename;    /* filename to read and write */
static Boolean file_contained_good_data = False;

/* ARGSUSED */
static void 
PrintOut(widget, client_data, call_data)
Widget widget;
XtPointer client_data, call_data;   /* unused */
{
    XWriteBitmapFile(XtDisplay(widget), filename, private_data.normal_bitmap,
            private_data.pixmap_width_in_cells, private_data.pixmap_height_in_cells, 0, 0);
}

/*ARGSUSED*/
static void
RedrawSmallPicture(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
    GC gc;

    if (w == private_data.showNormalBitmap)
        gc = DefaultGCOfScreen(XtScreen(w));
    else
        gc = private_data.invert_gc;

    if (DefaultDepthOfScreen(XtScreen(w)) == 1)
        XCopyArea(XtDisplay(w), private_data.normal_bitmap, XtWindow(w),
                gc, 0, 0, private_data.pixmap_width_in_cells, 
                private_data.pixmap_height_in_cells, 0, 0);
    else
        XCopyPlane(XtDisplay(w), private_data.normal_bitmap, XtWindow(w),
                gc, 0, 0, private_data.pixmap_width_in_cells, 
                private_data.pixmap_height_in_cells, 0, 0, 1);
}

String
FillCell(w)
Widget w;
{
    String cell;
    int x, y;
    XImage *image;
    cell = XtCalloc(private_data.pixmap_width_in_cells * private_data.pixmap_height_in_cells, sizeof(char));
    /* Convert pixmap into image, so that we can 
     * read individual pixels */
    image = XGetImage(XtDisplay(w), private_data.normal_bitmap, 0, 0, 
        private_data.pixmap_width_in_cells, private_data.pixmap_height_in_cells,
        AllPlanes, XYPixmap);

    for (x = 0; x < private_data.pixmap_width_in_cells; x++) {
        for (y = 0; y < private_data.pixmap_height_in_cells; y++) {
            cell[x + (y * private_data.pixmap_width_in_cells)] = XGetPixel(image, x, y);
        }
    }
    return(cell);
}

main(argc, argv)
int argc;
char *argv[];
{
    XtAppContext app_context;
    Widget topLevel, form, scrolledW, bigBitmap, buttonbox, quit, output;
    Arg args[5];
    int i;
    extern exit();
    unsigned int width, height; /* NOT Dimension: used in Xlib calls */
    int junk;
    String cell;

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
            "XBitmap5",         /* Application class */
            table, XtNumber(table),  /* command line option list */
            &argc, argv,        /* command line args */
            NULL,               /* for missing app-defaults file */
            NULL);              /* terminate varargs list */

    if (argv[1] != NULL)
         filename = argv[1];
    else {
         fprintf(stderr, "xbitmap: must specify filename on command line.\n");
         exit(1);
    }

    form = XtCreateManagedWidget("form", xmPanedWindowWidgetClass, topLevel, NULL, 0);

    buttonbox = XtCreateManagedWidget("buttonbox", xmRowColumnWidgetClass, form, NULL, 0);

    output = XtCreateManagedWidget("output", xmPushButtonWidgetClass, buttonbox, NULL, 0);

    XtAddCallback(output, XmNactivateCallback, PrintOut, NULL);

    quit = XtCreateManagedWidget("quit", xmPushButtonWidgetClass, buttonbox, NULL, 0);

    XtAddCallback(quit, XmNactivateCallback, exit, NULL);

    switch (XReadBitmapFile(XtDisplay(quit),
            RootWindowOfScreen(XtScreen(quit)), filename,
            &width, &height, &private_data.normal_bitmap, &junk, &junk)) {
        case BitmapSuccess:
            file_contained_good_data = True;
            if ((private_data.pixmap_width_in_cells != width) || 
                    (private_data.pixmap_height_in_cells != height)) {
                i = 0;
                XtSetArg(args[i], XtNpixmapWidthInCells, width);   i++;
                XtSetArg(args[i], XtNpixmapHeightInCells, height);   i++;
                private_data.pixmap_width_in_cells = width;
                private_data.pixmap_height_in_cells = height;
                cell = FillCell(quit);
                XtSetArg(args[i], XtNcellArray, cell);   i++;
            }
            break;
        case BitmapOpenFailed:
            fprintf(stderr, "xbitmap: could not open bitmap file, using fresh bitmap.\n");
			i = 0;
            file_contained_good_data = False;
            break;
        case BitmapFileInvalid:
            fprintf(stderr, "xbitmap: bitmap file invalid.\n");
            exit(1);
        case BitmapNoMemory:
            fprintf(stderr, "xbitmap: insufficient server memory to create bitmap.\n");
            exit(1);
        default:
            fprintf(stderr, "xbitmap: programming error.\n");
            exit(1);
    }


    scrolledW = XtVaCreateManagedWidget("scrolledW", 
            xmScrolledWindowWidgetClass, form, NULL, 0);

    /* args are set above if file was read */
    bigBitmap = XtCreateManagedWidget("bigBitmap", 
            bitmapEditWidgetClass, scrolledW, args, i);

    XtAddCallback(bigBitmap, XtNtoggleCallback, CellToggled, NULL);

    if (!file_contained_good_data) {
        XtVaGetValues(bigBitmap, 
                XtNpixmapHeightInCells, &private_data.pixmap_height_in_cells,
                XtNpixmapWidthInCells, &private_data.pixmap_width_in_cells,
                NULL);

        private_data.normal_bitmap = XCreatePixmap(XtDisplay(quit), 
                RootWindowOfScreen(XtScreen(quit)),
                private_data.pixmap_width_in_cells, private_data.pixmap_height_in_cells, 1);
    }

    SetUpThings(topLevel);

    private_data.showNormalBitmap = XtVaCreateManagedWidget("showNormalBitmap", 
            xmDrawingAreaWidgetClass, buttonbox, 
            XmNwidth, private_data.pixmap_width_in_cells,
            XmNheight, private_data.pixmap_height_in_cells,
            NULL);

    private_data.showReverseBitmap = XtVaCreateManagedWidget("showReverseBitmap", 
            xmDrawingAreaWidgetClass, buttonbox, 
            XmNwidth, private_data.pixmap_width_in_cells,
            XmNheight, private_data.pixmap_height_in_cells,
            NULL);

    XtAddCallback(private_data.showReverseBitmap, XmNexposeCallback, CellToggled, NULL);
    XtAddCallback(private_data.showNormalBitmap, XmNexposeCallback, CellToggled, NULL);
    XtRealizeWidget(topLevel);
    XtAppMainLoop(app_context);
}

static void
SetUpThings(w)
Widget w;
{
    XGCValues values;

    values.foreground = 1;
    values.background = 0;

    /* note that normal_bitmap is used as the drawable because it
     * is one bit deep.  The root window may not be one bit deep. */
    private_data.draw_gc = XCreateGC(XtDisplay(w), private_data.normal_bitmap,
            GCForeground | GCBackground, &values);

    values.foreground = 0;
    values.background = 1;
    private_data.undraw_gc = XCreateGC(XtDisplay(w), private_data.normal_bitmap,
            GCForeground | GCBackground, &values);

    /* this GC is for copying from the bitmap
     * to the small reverse widget */
    values.foreground = WhitePixelOfScreen(XtScreen(w));
    values.background = BlackPixelOfScreen(XtScreen(w));
    private_data.invert_gc = XtGetGC(w, GCForeground | GCBackground, &values);
}

/* ARGSUSED */
static void
CellToggled(w, client_data, info)
Widget w;
XtPointer client_data;  /* unused */
XtPointer info; /* call_data (from widget) */
{
    BitmapEditPointInfo *cur_info = (BitmapEditPointInfo *) info;
    /* 
     * Note: BitmapEditPointInfo is defined in BitmapEdit.h 
     */

    XDrawPoint(XtDisplay(w), private_data.normal_bitmap, 
            ((cur_info->mode == DRAWN) ? private_data.draw_gc : 
            private_data.undraw_gc), cur_info->newx, cur_info->newy);

    RedrawSmallPicture(private_data.showNormalBitmap);
    RedrawSmallPicture(private_data.showReverseBitmap);
}
