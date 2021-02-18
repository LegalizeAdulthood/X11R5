/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/************************\
* 			 *
*  WOOL_OBJECT: WLLabel	 *
*  DEFINITIONS		 *
* 			 *
\************************/

#ifndef INCLUDE_WL_LABEL_H
#define INCLUDE_WL_LABEL_H

/* type */

typedef struct _WOOL_Label {
    WOOL_HEADER;
    int x, y;
    long	    font;
    long	    color;
    WOOL_String	    label;
}              *WOOL_Label;

/* exported functions */

EXT WOOL_OBJECT wool_label_make();
EXT WOOL_Label WLLabel_make();
EXT WOOL_OBJECT WLLabel_print();
EXT WOOL_OBJECT WLLabel_free();
EXT char *	WLLabel_get_C_value();
EXT WOOL_OBJECT WLLabel_get_dimensions();
EXT WOOL_OBJECT WLLabel_draw();
EXT WOOL_OBJECT WLLabel_open();

/* methods */

EXT WOOL_METHOD WLLabel[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLLabel_print,		/* WOOL_print 2 */
   WLLabel_free,		/* WOOL_free 3 */
   wool_undefined_method_2,		/* WOOL_execute 4 */
   wool_undefined_method_2,		/* WOOL_set 5 */
   (WOOL_METHOD) WLLabel_get_C_value,/* WOOL_get_C_value 6 */
   WLLabel_open,		/* WOOL_open 7 */
   wool_undefined_method_1,	/* WOOL_close 8 */
   wool_undefined_method_2,	/* WOOL_process_event 9 */
   wool_undefined_method_1,	/* WOOL_copy 10 */
   WLLabel_get_dimensions,	/* WOOL_get_dimensions 11 */
   WLLabel_draw,		/* WOOL_draw 12 */
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

#endif /* INCLUDE_WL_LABEL_H */
