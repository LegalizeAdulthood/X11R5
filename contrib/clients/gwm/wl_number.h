/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/************************\
* 			 *
*  WOOL_OBJECT: Number 	 *
*  DEFINITIONS		 *
* 			 *
\************************/

#ifndef INCLUDE_WL_NUMBER_H
#define INCLUDE_WL_NUMBER_H

/* type */

typedef struct _WOOL_Number {
    WOOL_HEADER;
    Num            number;
}              *WOOL_Number;

/* exported functions */

EXT WOOL_Number WLNumber_make();
EXT WOOL_OBJECT WLNumber_eval();
EXT WOOL_OBJECT WLNumber_print();
EXT WOOL_OBJECT WLNumber_free();
EXT long *	WLNumber_get_C_value();
EXT WOOL_OBJECT WLNumber_equal();
EXT WOOL_OBJECT add_numbers();

#define must_be_number(obj, n) must_be(WLNumber, obj, n)

/* methods */

EXT WOOL_METHOD WLNumber[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLNumber_print,		/* WOOL_print 2 */
   WLNumber_free,		/* WOOL_free 3 */
   wool_undefined_method_2,		/* WOOL_execute 4 */
   wool_undefined_method_2,		/* WOOL_set 5 */
   (WOOL_METHOD) WLNumber_get_C_value,/* WOOL_get_C_value 6 */
   wool_undefined_method_1,	/* WOOL_open 7 */
   wool_undefined_method_1,	/* WOOL_close 8 */
   wool_undefined_method_2,	/* WOOL_process_event 9 */
   wool_undefined_method_1,	/* WOOL_copy 10 */
   wool_undefined_method_2,	/* WOOL_get_dimensions 11 */
   wool_undefined_method_2,	/* WOOL_draw 12 */
   WLNumber_equal,		/* WOOL_equal 13 */
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

#endif /* INCLUDE_WL_NUMBER_H */
