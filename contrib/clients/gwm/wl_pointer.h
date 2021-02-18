/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/************************\
* 			 *
*  WOOL_OBJECT  Pointer	 *
*  DEFINITIONS		 *
* 			 *
\************************/

#ifndef INCLUDE_WL_POINTER_H
#define INCLUDE_WL_POINTER_H

/* type */

typedef struct _WOOL_Pointer {
    WOOL_HEADER;
    long     	    *ptr; 	/* pointer to C object */
    char            p_name[1];	/* same as for atoms */
}              *WOOL_Pointer;

typedef struct _WOOL_Pointer_internal {
    char	    **base;	/* if non-null, ptr is an offset to it */
    WOOL_HEADER;
    long     	    ptr; 	/* offset to pointer to C object */
    char            p_name[1];	/* same as for atoms */
}              *WOOL_Pointer_internal;

/* macro to access the internal field */

#define WLPointerBase(wl_ptr) \
	(((WOOL_Pointer_internal) (((char *) (wl_ptr)) - \
	        (sizeof(struct _WOOL_Pointer_internal) - \
	        sizeof(struct _WOOL_Pointer)))) -> base)

#define WL_Pointer_internal(wl_ptr) \
    ((WOOL_Pointer_internal) (((char *) (wl_ptr)) - \
                (sizeof(struct _WOOL_Pointer_internal) - \
                sizeof(struct _WOOL_Pointer))))

/* exported functions */

EXT WOOL_Pointer wool_pointer_make();
EXT WOOL_Pointer wool_func_pointer_make();
EXT WOOL_Pointer WLPointer_make();
EXT WOOL_OBJECT  WLPointer_eval();
EXT WOOL_OBJECT  WLPointer_execute();
EXT WOOL_OBJECT  WLPointer_set();
EXT WOOL_OBJECT  WLPointer_setq();
EXT long         WLPointer_get_C_value();

/* methods */

EXT WOOL_METHOD WLPointer[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLPointer_eval,		/* WOOL_eval 1 */
   WLAtom_print,		/* WOOL_print 2 */
   WLAtom_free,			/* WOOL_free 3 */
   WLPointer_execute,		/* WOOL_execute 4 */
   WLPointer_set,		/* WOOL_set 5 */
   (WOOL_METHOD) WLPointer_get_C_value,/* WOOL_get_C_value 6 */
   wool_undefined_method_1,	/* WOOL_open 7 */
   wool_undefined_method_1,	/* WOOL_close 8 */
   wool_undefined_method_2,	/* WOOL_process_event 9 */
   wool_undefined_method_1,	/* WOOL_copy 10 */
   wool_undefined_method_2,	/* WOOL_get_dimensions 11 */
   wool_undefined_method_2,	/* WOOL_draw 12 */
   WLAtom_equal,		/* WOOL_equal 13 */
   wool_undefined_method_2,
   WLPointer_setq,              /* WOOL_setq 15 */
   wool_undefined_method_1,
   WLPointer_eval,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1
}
#endif /* DO_INIT */
               ;

#endif /* INCLUDE_WL_POINTER_H */
