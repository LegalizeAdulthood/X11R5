/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*********************\
* 		      *
*  WOOL_OBJECT  Atom  *
*  DEFINITIONS	      *
* 		      *
\*********************/

#ifndef INCLUDE_WL_ATOM_H
#define INCLUDE_WL_ATOM_H

/* type */

typedef struct _WOOL_Atom {
    WOOL_HEADER;
    WOOL_OBJECT     c_val;
    char            p_name[1];
}              *WOOL_Atom;

typedef struct _WOOL_Quark {
    WOOL_OBJECT	    pre_ptr;
    WOOL_HEADER;
    WOOL_OBJECT     c_val;
    char            p_name[1];
}              *WOOL_Quark;

/* exported functions */

EXT WOOL_Atom   wool_atom();
EXT WOOL_OBJECT WLAtom_eval();
EXT WOOL_OBJECT WLAtom_print();
EXT WOOL_OBJECT WLAtom_free();
EXT WOOL_OBJECT WLAtom_execute();
EXT WOOL_OBJECT WLAtom_set();
EXT WOOL_OBJECT WLAtom_setq();
EXT WOOL_OBJECT WLAtom_equal();
EXT WOOL_OBJECT WLAtom_silent_eval();
EXT int		WLAtom_get_C_value();
EXT void	must_be_atom();
EXT WOOL_Atom   find_next_prefixed_atom();
EXT WOOL_OBJECT WLAtom_unbind();
/* methods */

EXT WOOL_METHOD WLAtom[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLAtom_eval,			/* WOOL_eval 1 */
   WLAtom_print,		/* WOOL_print 2 */
   WLAtom_free,			/* WOOL_free 3 */
   WLAtom_execute,		/* WOOL_execute 4 */
   WLAtom_set,			/* WOOL_set 5 */
   (WOOL_METHOD) WLAtom_get_C_value,/* WOOL_get_C_value 6 */
   wool_undefined_method_1,	/* WOOL_open 7 */
   wool_undefined_method_1,	/* WOOL_close 8 */
   wool_undefined_method_2,	/* WOOL_process_event 9 */
   wool_undefined_method_1,	/* WOOL_copy 10 */
   wool_undefined_method_2,	/* WOOL_get_dimensions 11 */
   wool_undefined_method_2,	/* WOOL_draw 12 */
   WLAtom_equal,		/* WOOL_equal 13 */
   wool_undefined_method_2,
   WLAtom_setq,			/* WOOL_setq 15 */
   wool_undefined_method_1,
   WLAtom_silent_eval,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1
}
#endif /* DO_INIT */
               ;

#ifdef STATS
EXT WOOL_OBJECT hashstats();
EXT WOOL_OBJECT mstats();
EXT WOOL_OBJECT oblist();

#endif /* STATS */

#endif /* INCLUDE_WL_ATOM_H */
