/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*********************\
* 		      *
*  WOOL_OBJECT  List  *
*  DEFINITIONS	      *
* 		      *
\*********************/

#ifndef INCLUDE_WL_LIST_H
#define INCLUDE_WL_LIST_H

/* type */

typedef struct _WOOL_List {
    WOOL_HEADER;
    int             size;	/* size of list  ( # of elements ) */
    WOOL_OBJECT     list[1];	/* array of elements. Note that you */
}              *WOOL_List;	/* can't expand a list ! (use colls.) */

/* exported functions */

EXT WOOL_List   WLList_make();
EXT WOOL_List   wool_list_make();
EXT WOOL_List   wool_list_make_from_evaluated_array();
EXT WOOL_OBJECT WLList_eval();
EXT WOOL_OBJECT WLList_print();
EXT WOOL_OBJECT WLList_free();
EXT WOOL_OBJECT WLList_sub();
EXT WOOL_OBJECT WLList_nth();
EXT WOOL_OBJECT WLList_replace_nth();
EXT WOOL_OBJECT WLList_delete_nth();
EXT WOOL_OBJECT WLList_concat();
EXT WOOL_OBJECT WLList_equal();
EXT WOOL_OBJECT WLList_execute();
EXT int		WLList_length();
EXT WOOL_OBJECT *wool_flatten_pairlist();
EXT WOOL_OBJECT WLList_qsort();
EXT WOOL_OBJECT wool_list_make_from_wool();
EXT WOOL_OBJECT WLList_copy();

/* methods */

EXT WOOL_METHOD WLList[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLList_eval,			/* WOOL_eval 1 */
   WLList_print,		/* WOOL_print 2 */
   WLList_free,			/* WOOL_free 3 */
   WLList_execute,		/* WOOL_execute 4 */
   wool_undefined_method_2,	/* WOOL_set 5 */
   wool_undefined_method_1,	/* WOOL_get_C_value 6 */
   wool_undefined_method_1,	/* WOOL_open 7 */
   wool_undefined_method_1,	/* WOOL_close 8 */
   wool_undefined_method_2,	/* WOOL_process_event 9 */
   WLList_copy,			/* WOOL_copy 10 */
   wool_undefined_method_2,	/* WOOL_get_dimensions 11 */
   wool_undefined_method_2,	/* WOOL_draw 12 */
   WLList_equal,		/* WOOL_equal 13 */
   wool_undefined_method_2,
   wool_undefined_method_2,	/* WOOL_setq 15 */
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1
}
#endif /* DO_INIT */
               ;
#endif /* INCLUDE_WL_LIST_H */
