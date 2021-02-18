/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/**************************\
* 			   *
*  WOOL_OBJECT  Namespace  *
*  DEFINITIONS		   *
* 			   *
\**************************/

#ifndef INCLUDE_WL_NAMESPACE_H
#define INCLUDE_WL_NAMESPACE_H

/* type */

/* a namespace is an array of values, a name is an offset in this namespace
 * 
 * a namespace type is an array (one per namespace in the type) of an
 * array to values of the names (names are offsets in this namespace
 */

typedef struct _WOOL_Namespace {
    WOOL_HEADER;
    int current;			/* current (active) namespace */
    int number;				/* number of namespaces */
    int size;				/* number of slots in a namespace */
    int limit;				/* number of allocated slots */
    void (*trigger) ();			/* callback on change of state */
    void (*save_state) ();		/* save state before scanning */
    void (*restore_state) ();		/* restore state after scanning */
    int first_deleted;			/* index of first vacant slot */
    WOOL_OBJECT **namespaces;		/* array of names */
}              *WOOL_Namespace;

/* A name is like an atom, but with value being an index in a namespace, and
 * the base pointer being the namespace
 */

typedef struct _WOOL_Name {
    WOOL_HEADER;
    Num  	index;          /* index in namespace */
    char        p_name[1];  	/* same as for atoms */
}              *WOOL_Name;

typedef struct _WOOL_Name_internal {
    WOOL_Namespace namespace;   /* the namespace */
    WOOL_HEADER;
    Num  	index;          /* index in namespace */
    char        p_name[1];  	/* same as for atoms */
}              *WOOL_Name_internal;

/* macro to access the namespace field */

#define NameNamespace(a) \
    (((WOOL_Name_internal) (((char *) a) - \
	(sizeof(struct _WOOL_Name_internal) - \
	sizeof(struct _WOOL_Name)))) -> namespace)

/* exported functions */

EXT WOOL_Namespace WLNamespace_make();
EXT WOOL_OBJECT  WLNamespace_eval();
EXT WOOL_OBJECT  WLNamespace_print();
EXT WOOL_OBJECT  WLNamespace_free();

EXT WOOL_OBJECT  WLName_eval();
EXT WOOL_OBJECT  WLName_silent_eval();
EXT WOOL_OBJECT  WLName_execute();
EXT WOOL_OBJECT  WLName_set();
EXT WOOL_OBJECT  WLName_setq();

EXT WOOL_OBJECT  WLNamespace_add();
EXT WOOL_OBJECT  WLNamespace_remove();
EXT WOOL_OBJECT  WLName_add();
EXT WOOL_OBJECT  WLNamespace_set_current();
EXT WOOL_OBJECT  WLName_namespace();
EXT WOOL_OBJECT  WLNamespace_size();
EXT WOOL_OBJECT *WLName_slot();

#define must_be_namespace(namespace, n) must_be(WLNamespace, namespace, n)
#define must_be_name(name, n) must_be(WLName, namespace, n)

/* methods */

EXT WOOL_METHOD WLName[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLName_eval,		/* WOOL_eval 1 */
   WLAtom_print,		/* WOOL_print 2 */
   WLAtom_free,			/* same error as to free an atom */
   WLName_execute,		/* WOOL_execute 4 */
   WLName_set,		/* WOOL_set 5 */
   wool_undefined_method_1,	/* WOOL_get_C_value 6 */
   wool_undefined_method_1,	/* WOOL_open 7 */
   wool_undefined_method_1,	/* WOOL_close 8 */
   wool_undefined_method_2,	/* WOOL_process_event 9 */
   wool_undefined_method_1,	/* WOOL_copy 10 */
   wool_undefined_method_2,	/* WOOL_get_dimensions 11 */
   wool_undefined_method_2,	/* WOOL_draw 12 */
   WLAtom_equal,		/* WOOL_equal 13 */
   wool_undefined_method_2,
   WLName_setq,               /* WOOL_setq 15 */
   wool_undefined_method_1,
   WLName_silent_eval,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1
}
#endif /* DO_INIT */
               ;

EXT WOOL_METHOD WLNamespace[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLNamespace_print,		/* WOOL_print 2 */
   WLNamespace_free,	/* same error as to free an atom */
   wool_undefined_method_2,		/* WOOL_execute 4 */
   wool_undefined_method_2,		/* WOOL_set 5 */
   wool_undefined_method_1,	/* WOOL_get_C_value 6 */
   wool_undefined_method_1,	/* WOOL_open 7 */
   wool_undefined_method_1,	/* WOOL_close 8 */
   wool_undefined_method_2,	/* WOOL_process_event 9 */
   wool_undefined_method_1,	/* WOOL_copy 10 */
   wool_undefined_method_2,	/* WOOL_get_dimensions 11 */
   wool_undefined_method_2,	/* WOOL_draw 12 */
   WLAtom_equal,		/* WOOL_equal 13 */
   wool_undefined_method_2,
   wool_undefined_method_1,               /* WOOL_setq 15 */
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1
}
#endif /* DO_INIT */
               ;

#endif /* INCLUDE_WL_NAMESPACE_H */
