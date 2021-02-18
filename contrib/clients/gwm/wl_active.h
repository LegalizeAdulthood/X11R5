/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/************************\
* 			 *
*  WOOL_OBJECT  Active	 *
*  DEFINITIONS		 *
* 			 *
\************************/

#ifndef INCLUDE_WL_ACTIVE_H
#define INCLUDE_WL_ACTIVE_H

/* type */

typedef struct _WOOL_Active {
    WOOL_HEADER;
    WOOL_METHOD  get;		/* function called when got */
    char            p_name[1];	/* same as for atoms */
}              *WOOL_Active;

/* we need to store the set object without perturbing the name homogeneity */

typedef struct _WOOL_Active_internal {
    WOOL_METHOD  set;		/* function called when set */
    WOOL_HEADER;
    WOOL_METHOD  get;		/* function called when got */
    char            p_name[1];	/* same as for atoms */
}              *WOOL_Active_internal;

/* macro to access the set field */

#define ActiveSet(a) \
    (((WOOL_Active_internal) (((char *) a) - \
	(sizeof(struct _WOOL_Active_internal) - \
	sizeof(struct _WOOL_Active)))) -> set)

/* exported functions */

EXT WOOL_Active wool_active_make();
EXT WOOL_Active wool_func_active_make();
EXT WOOL_OBJECT  WLActive_eval();
EXT WOOL_OBJECT  WLActive_execute();
EXT WOOL_OBJECT  WLActive_set();
EXT WOOL_OBJECT  WLActive_setq();
EXT WOOL_OBJECT  WLActive_get_C_value();

/* methods */

EXT WOOL_METHOD WLActive[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLActive_eval,		/* WOOL_eval 1 */
   WLAtom_print,		/* WOOL_print 2 */
   WLAtom_free,			/* same error as to free an atom */
   WLActive_execute,		/* WOOL_execute 4 */
   WLActive_set,		/* WOOL_set 5 */
   WLActive_get_C_value,	/* WOOL_get_C_value 6 */
   wool_undefined_method_1,	/* WOOL_open 7 */
   wool_undefined_method_1,	/* WOOL_close 8 */
   wool_undefined_method_2,	/* WOOL_process_event 9 */
   wool_undefined_method_1,	/* WOOL_copy 10 */
   wool_undefined_method_2,	/* WOOL_get_dimensions 11 */
   wool_undefined_method_2,	/* WOOL_draw 12 */
   WLAtom_equal,		/* WOOL_equal 13 */
   wool_undefined_method_2,
   WLActive_setq,               /* WOOL_setq 15 */
   wool_undefined_method_1,
   WLActive_eval,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1
}
#endif /* DO_INIT */
               ;

#endif /* INCLUDE_WL_ACTIVE_H */
