/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/**************************\
* 			   *
*  WOOL_OBJECT Collection  *
*  DEFINITIONS		   *
* 			   *
\**************************/

#ifndef INCLUDE_WL_COLL_H
#define INCLUDE_WL_COLL_H

/* type */

typedef struct _WOOL_Collection {
    WOOL_HEADER;
    int             size;	/* number of elements */
    int             limit;	/* elements allowable in the malloced aera */
    WOOL_OBJECT    *list;	/* malloced region to store elements */
}              *WOOL_Collection;

typedef struct _WOOL_QuotedExpr {	/* 'foo */
    WOOL_HEADER;
    WOOL_OBJECT	expr;			/* foo */
}	       *WOOL_QuotedExpr;

/* exported functions */

EXT WOOL_Collection WLCollection_make();
EXT WOOL_OBJECT WLCollection_print();
EXT WOOL_OBJECT WLCollection_free();
EXT WOOL_Collection WLCollection_add();
EXT WOOL_OBJECT WLCollection_progn();

EXT WOOL_QuotedExpr WLQuotedExpr_make();
EXT WOOL_OBJECT WLQuotedExpr_eval();
EXT WOOL_OBJECT WLQuotedExpr_print();
EXT WOOL_OBJECT WLQuotedExpr_free();
EXT WOOL_OBJECT WLQuotedExpr_equal();

/* methods */

EXT WOOL_METHOD WLCollection[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLCollection_print,		/* WOOL_print 2 */
   WLCollection_free,		/* WOOL_free 3 */
   wool_undefined_method_2,		/* WOOL_execute 4 */
   wool_undefined_method_2,		/* WOOL_set 5 */
   (WOOL_METHOD) wool_undefined_method_1,/* WOOL_get_C_value 6 */
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

EXT WOOL_METHOD WLQuotedExpr[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLQuotedExpr_eval,		/* WOOL_eval 1 */
   WLQuotedExpr_print,		/* WOOL_print 2 */
   WLQuotedExpr_free,		/* WOOL_free 3 */
   wool_undefined_method_2,		/* WOOL_execute 4 */
   wool_undefined_method_2,		/* WOOL_set 5 */
   (WOOL_METHOD) wool_undefined_method_1,/* WOOL_get_C_value 6 */
   wool_undefined_method_1,	/* WOOL_open 7 */
   wool_undefined_method_1,	/* WOOL_close 8 */
   wool_undefined_method_2,	/* WOOL_process_event 9 */
   wool_undefined_method_1,	/* WOOL_copy 10 */
   wool_undefined_method_2,	/* WOOL_get_dimensions 11 */
   wool_undefined_method_2,	/* WOOL_draw 12 */
   WLQuotedExpr_equal,		/* WOOL_equal 13 */
   wool_undefined_method_2,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1
}
#endif /* DO_INIT */
               ;

/* Initialisations */

#ifdef DO_INIT
#define INITIAL_COLLECTION_SIZE 7	/* initial size allocated in
					 * WLCollection_make */
#endif /* DO_INIT */
#endif /* INCLUDE_WL_COLL_H */
