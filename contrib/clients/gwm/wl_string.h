/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/***********************\
* 		        *
*  WOOL_OBJECT  String  *
*  DEFINITIONS	        *
* 		        *
\***********************/

#ifndef INCLUDE_WL_STRING_H
#define INCLUDE_WL_STRING_H

/* type */

/* Note: the placeholder is there so that the string field of a WLString
 * begins at the same place as the p_name field of a WLAtom, so that a 
 * WLAtom can be handled to any function expecting an WLString as
 * argument. An WLAtom is less memory_consuming for often used symbols
 * ( "foo""foo" allocates two strings, 'foo 'foo only one atom), but cannot
 * be released.
 */

typedef struct _WOOL_String {
    WOOL_HEADER;
    WOOL_OBJECT     place_holder;
    char            string[1];
}              *WOOL_String;

/* exported functions */

EXT WOOL_String WLString_make();
EXT WOOL_String WLString_n_make();
EXT WOOL_OBJECT WLString_print();
EXT WOOL_OBJECT WLString_equal();
EXT WOOL_OBJECT add_strings();
EXT WOOL_String WLString_match();

/* exported objects */

EXT WOOL_String NIL_STRING;

/* methods */

EXT WOOL_METHOD WLString[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLString_print,		/* WOOL_print 2 */
   WLNumber_free,		/* WOOL_free 3 */
   wool_undefined_method_2,		/* WOOL_execute 4 */
   wool_undefined_method_2,		/* WOOL_set 5 */
   wool_undefined_method_1,	/* WOOL_get_C_value 6 */
   wool_undefined_method_1,	/* WOOL_open 7 */
   wool_undefined_method_1,	/* WOOL_close 8 */
   wool_undefined_method_2,	/* WOOL_process_event 9 */
   wool_undefined_method_1,	/* WOOL_copy 10 */
   wool_undefined_method_2,	/* WOOL_get_dimensions 11 */
   wool_undefined_method_2,	/* WOOL_draw 12 */
   WLString_equal,		/* WOOL_equal 13 */
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
#endif /* INCLUDE_WL_STRING_H */
