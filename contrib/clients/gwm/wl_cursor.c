/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/***********************\
* 		        *
*  WOOL_OBJECT: Cursor  *
*  BODY		        *
* 		        *
\***********************/

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_string.h"
#include "wl_number.h"
#include "gwm.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "wl_pixmap.h"
#include "INTERN.h"
#include "wl_cursor.h"

/*
 * Constructor: wool_cursor_make
 *   arguments: bitmap & mask
 * font cursor code contributed by Bill Trost <trost@scott.labs.tek.com>
 */

WOOL_OBJECT
wool_cursor_make(argc, argv)
int argc;
WOOL_String	argv[];
{
    WOOL_Cursor     object = (WOOL_Cursor)
    Malloc(sizeof(struct _WOOL_Cursor));
    XColor          colors[2];
    WOOL_Pixmap     bitmap, mask;
    WOOL_String     temp_string;
    int             cursor_number;
    int 	    use_cursor_font = 0;

    switch (argc) {
    case 2:
	bitmap = (WOOL_Pixmap) wool_raw_bitmap_make(argv[0]);
	mask = (WOOL_Pixmap) wool_raw_bitmap_make(argv[1]);
	break;
    case 1:
	if (argv[0] -> type == WLNumber) {
	    use_cursor_font = 1;
	    cursor_number = ((WOOL_Number) argv[0]) -> number;
	    break;
	} else {
	    must_be_string(argv[0], 0);
	    temp_string = WLString_n_make(strlen(argv[0] -> string) + 2);
	    strcpy(temp_string -> string, argv[0] -> string);
	    strcat(temp_string -> string, "-f");
	    bitmap = (WOOL_Pixmap) wool_raw_bitmap_make(temp_string);
	    strcpy(temp_string -> string, argv[0] -> string);
	    strcat(temp_string -> string, "-m");
	    mask = (WOOL_Pixmap) wool_raw_bitmap_make(temp_string);
	    break;
	}
    default:
	return wool_error(BAD_NUMBER_OF_ARGS, argc);
    }
    zrt_put(object);
    object -> type = WLCursor;
    colors[0].pixel = Context -> pixel.Fore;
    colors[1].pixel = Context -> pixel.Back;
    XQueryColors(dpy, DefaultColormap(dpy, Context -> screen), colors, 2);

    if (use_cursor_font) {
	object -> cursor = XCreateFontCursor(dpy, cursor_number);
	XRecolorCursor(dpy, object -> cursor, &colors[0], &colors[1]);
	return (WOOL_OBJECT) object;
    } else {
	if ((bitmap -> x_hot < 0) || (bitmap -> x_hot >= bitmap -> width))
	    bitmap -> x_hot = 0;
	if ((bitmap -> y_hot < 0) || (bitmap -> y_hot >= bitmap -> height))
	    bitmap -> y_hot = 0;
	if (!GWM_InvertCursors) {
	    XFillRectangle(dpy, bitmap -> pixmap, Context -> gc.Bitmap,
			   0, 0, bitmap -> width, bitmap -> height);
	    XFillRectangle(dpy, mask -> pixmap, Context -> gc.Bitmap,
			   0, 0, mask -> width, mask -> height);
	}
	object -> cursor = XCreatePixmapCursor(dpy, bitmap -> pixmap,
					       mask -> pixmap,
					       &colors[0], &colors[1],
					  bitmap -> x_hot, bitmap -> y_hot);
	return (WOOL_OBJECT) object;
    }
}


/*
 * WLCursor_print:
 * a cursor prints as [CURSOR widthxheight]
 */

WOOL_OBJECT 
WLCursor_print(obj)
WOOL_Cursor     obj;
{
    wool_printf("{CURSOR %x}", obj);
    return (WOOL_OBJECT) obj;
}

/*
 * WLCursor_free:
 * The structure is just freed, and string released.
 */

WOOL_OBJECT 
WLCursor_free(obj)
WOOL_Cursor     obj;
{
    XFreeCursor(dpy, obj -> cursor);
    Free(obj);
    return NULL;
}
