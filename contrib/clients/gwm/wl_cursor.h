/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*************************\
* 			  *
*  WOOL_OBJECT: WLCursor  *
*  DEFINITIONS		  *
* 			  *
\*************************/

#ifndef INCLUDE_WL_CURSOR_H
#define INCLUDE_WL_CURSOR_H

/* type */

typedef struct _WOOL_Cursor {
    WOOL_HEADER;
    Cursor	cursor;    
}              *WOOL_Cursor;

/* exported functions */

EXT WOOL_OBJECT wool_cursor_make();
EXT WOOL_OBJECT WLCursor_print();
EXT WOOL_OBJECT WLCursor_free();

/* methods */

EXT WOOL_METHOD WLCursor[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLCursor_print,		/* WOOL_print 2 */
   WLCursor_free,		/* WOOL_free 3 */
   wool_undefined_method_2,	/* WOOL_execute 4 */
   wool_undefined_method_2,	/* WOOL_set 5 */
   wool_undefined_method_1,	/* WOOL_get_C_value 6 */
   wool_undefined_method_1,	/* WOOL_open 7 */
   wool_undefined_method_1,	/* WOOL_close 8 */
   wool_undefined_method_2,	/* WOOL_process_event 9 */
   wool_undefined_method_1,	/* WOOL_copy 10 */
   wool_undefined_method_2,	/* WOOL_get_dimensions 11 */
   wool_undefined_method_2,	/* WOOL_draw 12 */
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
#endif /* INCLUDE_WL_CURSOR_H */
