/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/***********************\
* 		        *
*  WOOL_OBJECT: Pixmap  *
*  BODY		        *
* 		        *
\***********************/

#include "EXTERN.h"
#include <stdio.h>
#include "xpm/xpm.h"
#include "wool.h"
#include "wl_string.h"
#include "wl_atom.h"
#include "wl_pointer.h"
#include "wl_active.h"
#include "wl_number.h"
#include "wl_label.h"
#include "wl_list.h"
#include "gwm.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "INTERN.h"
#include "wl_pixmap.h"
#include "def_bitmap.h"

/* local prototyping */

WOOL_OBJECT 	wool_default_bitmap_make();
WOOL_OBJECT 	wool_default_pixmap_make();

/* when debugging, maintain the screen to which belongs the pixmap */

#ifdef DEBUG
#define StorePixmapScreen(pixmap, root) \
	pixmap -> screen = ScreenOfRoot(root)
#else
#define StorePixmapScreen(pixmap, root)
#endif /* DEBUG */

/*
 * makes a WOOL pixmap out of a X11 bitmap (or appropriate depth pixmap)
 */

WOOL_Pixmap
WLPixmap_make(pixmap)
Pixmap	pixmap;
{
    WOOL_Pixmap     object = (WOOL_Pixmap)
    Malloc(sizeof(struct _WOOL_Pixmap));
    unsigned int    bw, depth;
    Window          root;

    zrt_put(object);
    object -> type = WLPixmap;
    XGetGeometry(dpy, pixmap, &root, &object -> x_hot, &object -> y_hot,
		 &object -> width, &object -> height, &bw, &depth);

    if (!object -> width || !object -> height || !depth) {	/* ERROR */
	wool_default_pixmap_make(object, "invalid pixmap %s", "size");
    } else if (depth == 1) {	/* BITMAP */
	object -> pixmap = XCreatePixmap(dpy, Context->root, object -> width,
				       object -> height, Context->depth);
	XSetForeground(dpy, Context->gc.Work, Context -> pixel.Back);
	XFillRectangle(dpy, object -> pixmap, Context->gc.Work, 0, 0,
		       object -> width, object -> height);
	XSetForeground(dpy, Context->gc.Set, Context -> pixel.Fore);
	XSetStipple(dpy, Context->gc.Set, pixmap);
	XFillRectangle(dpy, object -> pixmap, Context->gc.Set, 0, 0,
		       object -> width, object -> height);
    } else if (depth == Context->depth) {	/* PIXMAP */
 	object -> pixmap = XCreatePixmap(dpy, Context->root, object -> width,
 				       object -> height, Context->depth);
 	XCopyArea(dpy, pixmap, object->pixmap, Context->gc.Work, 0, 0,
 		  object->width, object->height, 0, 0);
    } else {			/* ERROR */
	wool_default_pixmap_make(object, "Pixmap is of depth %d", depth);
    }
    StorePixmapScreen(object, root);
    return object;
}

/*
 * the physical reading of a bitmap. Used for bitmaps & cursors
 * if parameter is a label returns it!
 */

WOOL_OBJECT
wool_raw_bitmap_make(filename)
WOOL_String	filename;
{
    char           *name;
    extern char    *GWM_WLBITPATH;
    WOOL_Pixmap     object;
    char            temp_filename[MAX_TEMP_STRING_SIZE];

    must_be_string(filename, 0);
    object = (WOOL_Pixmap) Malloc(sizeof(struct _WOOL_Pixmap));
    zrt_put(object);
    object -> type = WLPixmap;
    name = (char *) file_in_path(filename -> string,
				 BITMAP_EXTENSION, wool_path, temp_filename);
    if (!name) {
	return wool_default_bitmap_make(object,
			  "GWM: Cannot find Bitmap %s", filename -> string);
    }
    switch (XReadBitmapFile(dpy, Context->root, name,
		     &object -> width, &object -> height, &object -> pixmap,
			    &object -> x_hot, &object -> y_hot)) {
    case BitmapOpenFailed:
	return wool_default_bitmap_make(object,
					"Cannot open file %s", name);
    case BitmapFileInvalid:
	return wool_default_bitmap_make(object,
					"File %s is not a bitmap", name);
    case BitmapNoMemory:
	wool_error("No memory to load file %s", name);
	break;
    }
    return (WOOL_OBJECT) object;
}

/* Given a pre-allocated WOOL_Pixmap, fill it with  the default
 * bitmap or pixmap, rexpectively
 */

WOOL_OBJECT
wool_default_bitmap_make(object, reason_format, reason)
WOOL_Pixmap object;			/* empty wool structure */
char *reason_format;			/* text of the error message or 0 */
char *reason;				/* data for %s in reason format */
{
    if (reason_format) {
	wool_printf(reason_format, reason);
	wool_puts(", using default instead\n");
    }
    object -> width = def_bitmap_width;
    object -> height = def_bitmap_height;
    object -> x_hot = def_bitmap_x_hot;
    object -> y_hot = def_bitmap_y_hot;
    object -> pixmap = Context -> DefaultBitmap;
    return (WOOL_OBJECT) object;
}

WOOL_OBJECT
wool_default_pixmap_make(object, reason_format, reason)
WOOL_Pixmap object;                     /* empty wool structure */
char *reason_format;                    /* text of the error message or 0 */
char *reason;                           /* data for %s in reason format */
{
    if (reason_format) {
        wool_printf(reason_format, reason);
        wool_puts(", using default instead\n");
    }
    object -> width = def_bitmap_width;
    object -> height = def_bitmap_height;

    XSetForeground(dpy, Context->gc.Work, Context -> pixel.Back);
    object -> pixmap = XCreatePixmap(dpy, Context->root,
				     object -> width, object -> height,
				     Context->depth);
    XFillRectangle(dpy, object -> pixmap, Context->gc.Work, 0, 0,
		   object -> width, object -> height);
    XSetForeground(dpy, Context->gc.Set, Context -> pixel.Fore);
    XSetStipple(dpy, Context->gc.Set, Context -> DefaultBitmap);
    XFillRectangle(dpy, object -> pixmap, Context->gc.Set, 0, 0,
		   object -> width, object -> height);
    StorePixmapScreen(object, Context -> root);
    return (WOOL_OBJECT) object;
}

/*
 * to overlay bitmaps:
 * (layered-bitmap-make back "file1" color1 "file2" color2 ...)
 * filenames can be active-labels
 */

WOOL_Pixmap
wool_layered_bitmap_make(argc, argv)
int argc;
WOOL_Pixmap argv[];
{
    Pixmap          pix;
    int             i;
    unsigned int    width = 0, height = 0;
    WOOL_Pixmap     object;
    struct _Box     box;
    WOOL_Label      label;
    Graphic_desc   *graphics;

    if ((argc == 0) || (argc > 2 && (argc % 2 == 0)))
	wool_error(BAD_NUMBER_OF_ARGS, argc);
    if (argc == 1) {		/* one args re-calls with default colors */
	WOOL_Number     new_argv[3];

	new_argv[0] = WLNumber_make(Context -> pixel.Back);
	new_argv[2] = WLNumber_make(Context -> pixel.Fore);
	new_argv[1] = (WOOL_Number) argv[0];
	return (WOOL_Pixmap) wool_layered_bitmap_make(3, new_argv);
    }
    if (argc == 2) {		/* two args make a blank bitmaps */
	return wool_make_blank_bitmap(argv[0], argv[1]);
    }
    /* multi-args form */
    must_be_number(argv[0], 0);
    must_be_number(argv[2], 2);
    graphics = (Graphic_desc *) Malloc(sizeof(Graphic_desc) * argc);
    
    /* first determine the size of the final pixmap and load the bitmaps */
    for (i = 1; i < argc; i += 2) {
	if (is_a_string(argv[i])) {
	    graphics[i].is_bitmap = 1;
	    argv[i] = (WOOL_Pixmap) wool_raw_bitmap_make(argv[i]);
	} else {
	    graphics[i].is_bitmap = 0;
	}
	WOOL_send(WOOL_get_dimensions, argv[i], (argv[i], &box));
	graphics[i].width = box.width;
	graphics[i].height = box.height;
	width = Max(width, box.width);
	height = Max(height, box.height);
    }

    /* then lay the graphics one on top of another */
    XSetForeground(dpy, Context->gc.Work, ((WOOL_Number) argv[0]) -> number);
    pix = XCreatePixmap(dpy, Context->root, width, height, Context->depth);
    XFillRectangle(dpy, pix, Context->gc.Work, 0, 0, width, height);
    for (i = 1; i < argc; i += 2) {
	must_be_number(argv[i + 1], i + 1);
	if (graphics[i].is_bitmap) {
	    XSetForeground(dpy, Context->gc.Set,
	    	((WOOL_Number) argv[i + 1]) -> number);
	    XSetStipple(dpy, Context->gc.Set, argv[i] -> pixmap);
	    XSetTSOrigin(dpy, Context->gc.Set, (width - argv[i] -> width) / 2, (height - argv[i] -> height) / 2);
	    XFillRectangle(dpy, pix, Context->gc.Set,
	    (width - argv[i] -> width) / 2, (height - argv[i] -> height) / 2,
			   argv[i] -> width, argv[i] -> height);
	    XSetTSOrigin(dpy, Context->gc.Set, 0, 0);
	} else if (argv[i] -> type == WLPixmap) {
	    XCopyArea(dpy, argv[i] -> pixmap, pix, Context->gc.Work, 0, 0,
		      argv[i] -> width, argv[i] -> height,
		      (width - argv[i] -> width) / 2,
		      (height - argv[i] -> height) / 2);
	} else if (argv[i] -> type == WLLabel) {	/* label */
	    label = (WOOL_Label) argv[i];
	    XSetFont(dpy, Context->gc.Work, label -> font);
	    XSetForeground(dpy, Context->gc.Work,
			   ((WOOL_Number) argv[i + 1]) -> number);
	    XDrawString(dpy, pix, Context->gc.Work,
			label -> x, label -> y, label -> label -> string,
			strlen(label -> label -> string));
	}
    }

    /* then creates the WOOL_Pixmap */
    object = (WOOL_Pixmap) Malloc(sizeof(struct _WOOL_Pixmap));
    zrt_put(object);
    object -> type = WLPixmap;
    object -> pixmap = pix;
    object -> x_hot = object -> y_hot = 0;
    object -> width = width;
    object -> height = height;
    Free(graphics);
    StorePixmapScreen(object, Context -> root);
    return object;
}

/*
 * makes a pixmap of "foreground" color of a given size
 */

WOOL_Pixmap
wool_make_blank_bitmap(width, height)
WOOL_Number width;
WOOL_Number height;
{
    WOOL_Pixmap     object;
    Pixmap          pix;

    XSetForeground(dpy, Context->gc.Work, Context -> pixel.Fore);
    pix = XCreatePixmap(dpy, Context->root,
		      width -> number, height -> number, Context->depth);
    XFillRectangle(dpy, pix, Context->gc.Work, 0, 0, width -> number, height -> number);
    object = (WOOL_Pixmap) Malloc(sizeof(struct _WOOL_Pixmap));
    zrt_put(object);
    object -> type = WLPixmap;
    object -> pixmap = pix;
    object -> x_hot = object -> y_hot = 0;
    object -> width = width -> number;
    object -> height = height -> number;
    StorePixmapScreen(object, Context -> root);
    return object;
}

/*
 * A stamp is a string "stamped" on the background to yield a bitmap
 */

WOOL_OBJECT
wool_stamp_make(argc, argv)
int             argc;
WOOL_String     argv[];
{
    int             up, down, dir, x, y, width, height, font;
    XCharStruct     extent;
    WOOL_Pixmap     object;

    if (argc == 0 || argc > 2)
        return wool_error(BAD_NUMBER_OF_ARGS, argc);
    must_be_string(argv[0], 0);
    if (argc == 2) {
        must_be_number(argv[1], 1);
        font = ((WOOL_Number) argv[1]) -> number;
    } else
        font = DefaultFont;
    object  = (WOOL_Pixmap) Malloc(sizeof(struct _WOOL_Pixmap));
    XQueryTextExtents(dpy, font,
		      argv[0] -> string, strlen(argv[0] -> string),
		      &dir, &up, &down, &extent);
    x = DefaultLabelHorizontalMargin - extent.lbearing;
    y = DefaultLabelVerticalMargin + up;
    width = Max(1, extent.width + 2 * DefaultLabelHorizontalMargin);
    height = Max(1, up + down + 2 * DefaultLabelVerticalMargin);

    zrt_put(object);
    object -> type = WLPixmap;
    object -> pixmap = XCreatePixmap(dpy, Context->root, width, height, 
	Context->depth);
    object -> x_hot = object -> y_hot = 0;
    object -> width = width;
    object -> height = height;

    XSetForeground(dpy, Context->gc.Work, Context -> pixel.Back);
    XFillRectangle(dpy, object -> pixmap, Context->gc.Work,
		   0, 0, width, height);
    XSetFont(dpy, Context->gc.Work, font);
    XSetForeground(dpy, Context->gc.Work, Context -> pixel.Fore);
    XDrawString(dpy, object -> pixmap, Context->gc.Work,
		x, y, argv[0] -> string,
		strlen(argv[0] -> string));
    StorePixmapScreen(object, Context -> root);
    return (WOOL_OBJECT) object;
}

/*
 * WLPixmap_print:
 * a bitmap prints as [PIXMAP widthxheight id]
 */

WOOL_OBJECT 
WLPixmap_print(obj)
WOOL_Pixmap     obj;
{
    wool_printf("{PIXMAP %dx", obj -> width);
    wool_printf("%d", obj -> height);
    wool_printf(" 0x%x", obj -> pixmap);
#ifdef DEBUG
    wool_printf(", on screen %d", obj -> screen);
#endif /* DEBUG */
    wool_puts("}");
    return (WOOL_OBJECT) obj;
}

/*
 * WLPixmap_free:
 * The structure is just freed, and string released.
 */

WOOL_OBJECT 
WLPixmap_free(obj)
WOOL_Pixmap     obj;
{
	if (obj->pixmap != Context -> DefaultBitmap)
		XFreePixmap(dpy, obj->pixmap);
	Free(obj);
	return NULL;
}

/*
 * returns the dimensions of the strings in the font + margins
 * x,y is the start of the string (baseline) in the box.
 */

WOOL_OBJECT
WLPixmap_get_dimensions(bitmap, box)
WOOL_Pixmap bitmap;
Box	box;	     		/* RETURN the dimensions */
{
    box -> x = 0;
    box -> y = 0;
    box -> width = bitmap -> width;
    box -> height = bitmap -> height;
    return NULL;
}

/*
 * opening a bitmap is putting it in backgroud
 */

WOOL_OBJECT
WLPixmap_open(bitmap, wob)
WOOL_Pixmap	bitmap;
Wob wob;
{
    XSetWindowBackgroundPixmap(dpy, wob -> hook, bitmap -> pixmap);
    XClearWindow(dpy,  wob -> hook);
    return NULL;		/* 0 means no exposure necessary */
}

/*
 * drawing a bitmap  (not needed ?)
 */

WOOL_OBJECT
WLPixmap_draw(bitmap, wob)
WOOL_Pixmap	bitmap;
Wob		wob;
{
    XClearWindow(dpy, wob -> hook);
    return NULL;
}

/*
 * Reading of a Pixmap file (xpm v2 format)
 */

WOOL_OBJECT
wool_xpm_pixmap_make(argc, argv)
int 	argc;
WOOL_String argv[];
{
    char           *name;
    extern char    *GWM_WLBITPATH;
    WOOL_Pixmap     object;
    char            temp_filename[MAX_TEMP_STRING_SIZE];
    unsigned int    npixels_return;
    unsigned long   *pixels_return;
    WOOL_String	filename;
    XpmAttributes xpmatt;
    XpmColorSymbol     *colorsymbols = 0;
    int             ncolorsymbols = 0;
    Pixmap pixmapshape;
    int i;

    if ((argc == 0) || (argc > 2 && (argc % 2 == 0)))
	wool_error(BAD_NUMBER_OF_ARGS, argc);
    filename = argv[0];
    must_be_string(filename, 0);
    
    if (argc > 1) {
	ncolorsymbols = (argc-1)/2;
	colorsymbols = (XpmColorSymbol *) Malloc(sizeof(XpmColorSymbol)
					      * ncolorsymbols);
	for (i=0; i<ncolorsymbols; i++) {
	    must_be_string(argv[i*2+1], i*2+1);
	    colorsymbols[i].name = argv[i*2+1] -> string;
	    if (argv[i*2+2]->type == WLNumber) {
		colorsymbols[i].value = 0;
		colorsymbols[i].pixel = ((WOOL_Number) argv[i*2+2])->number;
	    } else {
		must_be_string(argv[i*2+2], i*2+2);
		colorsymbols[i].value = argv[i*2+2] -> string;
	    }
	}
    }
    object = (WOOL_Pixmap) Malloc(sizeof(struct _WOOL_Pixmap));
    zrt_put(object);
    object -> type = WLPixmap;
    object -> x_hot = 0;
    object -> y_hot = 0;
    name = (char *) file_in_path(filename -> string,
				 PIXMAP_EXTENSION, wool_path, temp_filename);
    if (!name) {
	return wool_default_pixmap_make(object,
			  "GWM: Cannot find Pixmap %s", filename -> string);
    }

    xpmatt.valuemask = XpmVisual|XpmColormap|XpmDepth|XpmColorSymbols;
    xpmatt.visual = DefaultVisual(dpy, Context->screen);
    xpmatt.colormap = DefaultColormap(dpy, Context->screen);
    xpmatt.depth = Context->depth;
    xpmatt.colorsymbols = colorsymbols;
    xpmatt.numsymbols = ncolorsymbols;

    switch (XReadPixmapFile(dpy,
			    Context->root,
			    name,
			    &object -> pixmap,
			    &pixmapshape,
			    &xpmatt
			    )
	    ) {
    case PixmapSuccess:
	break;
    case PixmapOpenFailed:
	return wool_default_pixmap_make(object,
					"Cannot open file %s", name);
    case PixmapFileInvalid:
	return wool_default_pixmap_make(object,
					"File %s is not a XPM pixmap", name);
    case PixmapNoMemory:
	wool_error("No memory to load file %s", name);
	break;
    default:
	wool_error("Error while reading pixmap %s", name);
	break;
    }
    object->width = xpmatt.width;
    object->height = xpmatt.height;
    return (WOOL_OBJECT) object;
}

MakeDefaultBitmap()
{
    if (!(Context -> DefaultBitmap = XCreateBitmapFromData(dpy, Context->root,
		     def_bitmap_bits, def_bitmap_width, def_bitmap_height)))
	wool_error(NO_MEMORY, "");
}

/* Graphical primitives
 */

/* draws a line in a pixmap:
 * syntax (draw-line pixmap x1 y1 x2 y2)
 * uses "foreground" color
 */

WOOL_OBJECT
wool_draw_line(argc, argv)
int 	argc;
WOOL_Number argv[];
{
    int             i;

    if (argc != 5)
	wool_error(BAD_NUMBER_OF_ARGS, argc);
    must_be(WLPixmap, argv[0], 0);
    for (i = 1; i < 5; i++)
	must_be(WLNumber, argv[i], i);

    XSetForeground(dpy, Context->gc.Work, Context -> pixel.Fore);
    XDrawLine(dpy, ((WOOL_Pixmap) argv[0]) -> pixmap, Context->gc.Work,
	      argv[1] -> number, argv[2] -> number,
	      argv[3] -> number, argv[4] -> number);
    return (WOOL_OBJECT) argv[0];
}

/* draws a filled rectangle in a pixmap:
 * syntax (draw-rectangle pixmap x1 y1 width height border style)
 * uses "foreground" color for border
 * uses "background" color for rectangle
 * style: 0 = nothing, 1 = outline, 2 = filled, 3 = outline+filled 
 */

WOOL_OBJECT
wool_draw_rectangle(argc, argv)
int 	argc;
WOOL_Number argv[];
{
    int             i;

    if (argc != 7)
	wool_error(BAD_NUMBER_OF_ARGS, argc);
    must_be(WLPixmap, argv[0], 0);
    for (i = 1; i < 7; i++)
	must_be(WLNumber, argv[i], i);

    if (argv[6] -> number & 2) {	/* inside rectangle */
	XSetForeground(dpy, Context->gc.Work, Context -> pixel.Back);
	XFillRectangle(dpy, ((WOOL_Pixmap) argv[0]) -> pixmap,
		       Context->gc.Work,
		       argv[1] -> number, argv[2] -> number,
		       argv[3] -> number, argv[4] -> number);
    }
    if (argv[6] -> number & 1) {	/* ouline rectangle */
	int linewidth = argv[5] -> number ? argv[5] -> number : 1;
	XPoint points[5];

	points[0].x = argv[1] -> number - 1;
	points[0].y = argv[2] -> number - 1;
	points[1].x = points[0].x + argv[3] -> number + 1;
	points[1].y = points[0].y;
	points[2].x = points[1].x;
	points[2].y = points[0].y + argv[4] -> number + 1;
	points[3].x = points[0].x;
	points[3].y = points[2].y;
	points[4].x = points[0].x;
	points[4].y = points[0].y;

	XSetForeground(dpy, Context->gc.Work, Context -> pixel.Fore);
	XDrawLines(dpy, ((WOOL_Pixmap) argv[0]) -> pixmap,
		   Context->gc.Work, points, 5, CoordModeOrigin);
	for (i = 1; i < linewidth; i++) {
	    points[0].x -= 1; points[0].y -= 1;
	    points[1].x += 1; points[1].y -= 1;
	    points[2].x += 1; points[2].y += 1;
	    points[3].x -= 1; points[3].y += 1;
	    points[4].x -= 1; points[4].y -= 1;
	    XDrawLines(dpy, ((WOOL_Pixmap) argv[0]) -> pixmap,
		       Context->gc.Work, points, 5, CoordModeOrigin);
	}
    }
    return (WOOL_OBJECT) argv[0];
}

/* prints a text in a pixmap:
 * syntax (draw-text pixmap x1 y1 font text)
 * uses "foreground" color for text color
 */

WOOL_OBJECT
wool_draw_text(argc, argv)
int 	argc;
WOOL_Number argv[];
{
    int             i;

    if (argc != 5)
	wool_error(BAD_NUMBER_OF_ARGS, argc);
    must_be(WLPixmap, argv[0], 0);
    for (i = 1; i < 4; i++)
	must_be(WLNumber, argv[i], i);
    must_be_string(argv[4], 4);

    XSetForeground(dpy, Context->gc.Work, Context -> pixel.Fore);
    XSetFont(dpy, Context->gc.Work, argv[3]->number);
    XDrawString(dpy,  ((WOOL_Pixmap) argv[0]) -> pixmap, Context->gc.Work,
		argv[1] -> number, argv[2] -> number,
		((WOOL_String) argv[4]) -> string,
		strlen(((WOOL_String) argv[4]) -> string));
    return (WOOL_OBJECT) argv[0];
}
