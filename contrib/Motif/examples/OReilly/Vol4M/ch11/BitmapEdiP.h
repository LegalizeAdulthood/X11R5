/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/* 
 * BitmapEditP.h - Private definitions for BitmapEdit widget
 */

#ifndef _XtBitmapEditP_h
#define _XtBitmapEditP_h

/*
 * This include not needed unless the .c file includes IntrinsicP.h
 * after this file.   Anyway, it doesn't hurt.
 */
#include <X11/CoreP.h>

/*
 * This one is always needed!
 */
#include "BitmapEdit.h"

/* New fields for the BitmapEdit widget class record */

typedef struct {
	int dummy_field;	/* keep compiler happy */
} BitmapEditClassPart;

/* Full class record declaration */
typedef struct _BitmapEditClassRec {
    CoreClassPart	core_class;
    BitmapEditClassPart	bitmapEdit_class;
} BitmapEditClassRec;

extern BitmapEditClassRec bitmapEditClassRec;

/* New fields for the BitmapEdit widget record */
typedef struct {
    /* resources */
    Pixel	foreground;
    Pixel	selectionForeground;
    XtCallbackList callback;	/* application installed callback function(s) */
    Dimension	pixmap_width_in_cells;
    Dimension	pixmap_height_in_cells;
    int cell_size_in_pixels;

    /* private state */
    int cur_x, cur_y;  /* position of visible corner in big pixmap */
    Dimension	pixmap_width_in_pixels;
    Dimension	pixmap_height_in_pixels;
    Pixmap big_picture;
    GC		draw_gc;	/* one plane, for drawing into pixmap */
    GC		undraw_gc;	/* one plane, for drawing into pixmap */
    GC		deep_draw_gc;	/* defaultdepthofscreen, for copying pixmap into window */
    GC		deep_undraw_gc;	/* defaultdepthofscreen, for rubber banding selections */
    char *cell;	/* array for keeping track of array of bits */

    char *data;	/* selection data */
    int select_start_x, select_start_y;  /* for selections */
    int select_end_x, select_end_y;  /* for selections */
    Boolean first_box;
    Atom target_atom;
    /* these are the required target type atoms: */
} BitmapEditPart;

/*
 * Full instance record declaration
 */
typedef struct _BitmapEditRec {
    CorePart		core;
    BitmapEditPart	bitmapEdit;
} BitmapEditRec;

#endif /* _XtBitmapEditP_h */
