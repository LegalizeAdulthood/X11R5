/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/* 
 * BitmapEditP.h - Private definitions for BitmapEdit widget
 */

#ifndef _ORABitmapEditP_h
#define _ORABitmapEditP_h

/*
 * Include private header file of superclass.
 * In this case, however, Primitive's header file is
 * thrown into XmP.h.
 */
#include <Xm/XmP.h>   

/*
 * Include public header file for this widget.
 */
#include "BitmapEdit.h"

/* New fields for the BitmapEdit widget class record */

typedef struct {
	int make_compiler_happy;	/* keep compiler happy */
} BitmapEditClassPart;

/* Full class record declaration */
typedef struct _BitmapEditClassRec {
    CoreClassPart	core_class;
    XmPrimitiveClassPart	primitive_class;
    BitmapEditClassPart	bitmapEdit_class;
} BitmapEditClassRec;

extern BitmapEditClassRec bitmapEditClassRec;

/* New fields for the BitmapEdit widget record */
typedef struct {
    /* resources */
    Pixel	foreground;
    XtCallbackList callback;	/* application installed callback function(s) */
    Dimension	pixmap_width_in_cells;
    Dimension 	pixmap_height_in_cells;
    int cell_size_in_pixels;
    char *cell;	/* array for keeping track of array of bits */
	Boolean show_all;  /* whether bitmap should display entire bitmap */

    /* private state */
    int cur_x, cur_y;  /* position of visible corner in big pixmap */
    Dimension	pixmap_width_in_pixels;
    Dimension	pixmap_height_in_pixels;
    Pixmap big_picture;
    GC		draw_gc;	/* one plane, for drawing into pixmap */
    GC		undraw_gc;	/* one plane, for drawing into pixmap */
    GC		copy_gc;	/* defaultdepthofscreen, for copying pixmap into window */
	Boolean user_allocated;  /* whether user allocated cell array */
} BitmapEditPart;

/*
 * Full instance record declaration
 */
typedef struct _BitmapEditRec {
    CorePart		    core;
    XmPrimitivePart		primitive;
    BitmapEditPart	    bitmapEdit;
} BitmapEditRec;

#endif /* _ORABitmapEditP_h */
