/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*************************\
* 			  *
*  WOOL_OBJECT: WLPixmap  *
*  DEFINITIONS		  *
* 			  *
\*************************/

#ifndef INCLUDE_WL_BITMAP_H
#define INCLUDE_WL_BITMAP_H

/* type */

typedef struct _WOOL_Pixmap {
    WOOL_HEADER;
    Pixmap	pixmap;    
    int x_hot, y_hot;
    unsigned int width, height;
#ifdef DEBUG
    int screen;
#endif
}              *WOOL_Pixmap;

/* useful construct */

typedef struct{
    int width, height;
    int is_bitmap;
} Graphic_desc;

#define BITMAP_EXTENSION ".xbm"
#define PIXMAP_EXTENSION ".xpm"

/* exported functions */

EXT WOOL_Pixmap WLPixmap_make();
EXT WOOL_Pixmap wool_layered_bitmap_make();
EXT WOOL_OBJECT wool_raw_bitmap_make();
EXT WOOL_OBJECT wool_stamp_make();
EXT WOOL_OBJECT WLPixmap_print();
EXT WOOL_OBJECT WLPixmap_free();
EXT WOOL_OBJECT WLPixmap_get_dimensions();
EXT WOOL_OBJECT WLPixmap_draw();
EXT WOOL_OBJECT WLPixmap_open();
EXT WOOL_Pixmap wool_make_blank_bitmap();

/* methods */

EXT WOOL_METHOD WLPixmap[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLPixmap_print,		/* WOOL_print 2 */
   WLPixmap_free,		/* WOOL_free 3 */
   wool_undefined_method_2,		/* WOOL_execute 4 */
   wool_undefined_method_2,		/* WOOL_set 5 */
   wool_undefined_method_1,	/* WOOL_get_C_value 6 */
   WLPixmap_open,		/* WOOL_open 7 */
   wool_undefined_method_1,	/* WOOL_close 8 */
   wool_undefined_method_2,	/* WOOL_process_event 9 */
   wool_undefined_method_1,	/* WOOL_copy 10 */
   WLPixmap_get_dimensions,	/* WOOL_get_dimensions 11 */
   WLPixmap_draw,		/* WOOL_draw 12 */
   wool_undefined_method_2,	/* WOOL_equal 13 */
   wool_undefined_method_2,
   wool_undefined_method_2,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1
}
#endif /* DO_INIT */
               ;
#endif /* INCLUDE_WL_BITMAP_H */
