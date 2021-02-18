/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/********************************************\
* 					     *
*  WOOL_OBJECT  Subr, FSubr, Expr and FExpr  *
*  DEFINITIONS				     *
* 					     *
\********************************************/

#ifndef INCLUDE_WL_FUNC_H
#define INCLUDE_WL_FUNC_H

/* type */

typedef struct _WOOL_Subr {
    WOOL_HEADER;
    int             arity;
    WOOL_OBJECT(*body) ();
}              *WOOL_Subr;

typedef struct _WOOL_FSubr {
    WOOL_HEADER;
    int             arity;
    WOOL_OBJECT(*body) ();
}              *WOOL_FSubr;

typedef struct _WOOL_Expr {
    WOOL_HEADER;
    int             arity;
    WOOL_Atom      *parameters;
    int             body_size;
    WOOL_OBJECT    *body;
}              *WOOL_Expr;

typedef struct _WOOL_FExpr {
    WOOL_HEADER;
    int             arity;
    WOOL_Atom      *parameters;
    int             body_size;
    WOOL_OBJECT    *body;
}              *WOOL_FExpr;

/* exported functions */

EXT WOOL_Subr   WLSubr_make();
EXT WOOL_OBJECT wool_lambda_make();
EXT WOOL_OBJECT wool_lambdaq_make();
EXT WOOL_OBJECT defun();
EXT WOOL_OBJECT wool_subr_make();
EXT WOOL_OBJECT WLSubr_print();
EXT WOOL_OBJECT WLFSubr_print();
EXT WOOL_OBJECT WLExpr_print();
EXT WOOL_OBJECT WLFExpr_print();
EXT WOOL_OBJECT WLExpr_free();
EXT WOOL_OBJECT WLSubr_execute();
EXT WOOL_OBJECT WLFSubr_execute();
EXT WOOL_OBJECT WLExpr_execute();
EXT WOOL_OBJECT WLFExpr_execute();

EXT WOOL_OBJECT execute_local_code();
EXT WOOL_OBJECT local_variables();
EXT WOOL_OBJECT *map_eval();

/* methods */

EXT WOOL_METHOD WLSubr[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLSubr_print,		/* WOOL_print 2 */
   WLNumber_free,		/* WOOL_free 3 */
   WLSubr_execute,		/* WOOL_execute 4 */
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

EXT WOOL_METHOD WLFSubr[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLFSubr_print,		/* WOOL_print 2 */
   WLNumber_free,		/* WOOL_free 3 */
   WLFSubr_execute,		/* WOOL_execute 4 */
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

EXT WOOL_METHOD WLExpr[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLExpr_print,		/* WOOL_print 2 */
   WLExpr_free,			/* WOOL_free 3 */
   WLExpr_execute,		/* WOOL_execute 4 */
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

EXT WOOL_METHOD WLFExpr[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLFExpr_print,		/* WOOL_print 2 */
   WLExpr_free,			/* WOOL_free 3 */
   WLFExpr_execute,		/* WOOL_execute 4 */
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


/* A stack frame is:
 * - the (printable) expression which triggered the NEXT call
 * - a pointer to the previous stack frame
 * - size, the number of stored variables
 * - parameters, the list of formal (atoms, actives, pointers) parms
 * - new_values, the list of evaluated new values
 * - old_values, the list of previous values which will get restored
 *		 (internal space to the struct)
 */

typedef struct _WOOL_StackFrame {
    struct _WOOL_StackFrame *previous;	/* previous stack frame */
    int             size;		/* the number of stacked vars */
    WOOL_Atom      *parameters;		/* pointer to parameters */
    WOOL_OBJECT    *new_values;		/* pointer to new values */
    WOOL_OBJECT     old_values[1];	/* the old values to be restored */
}               *WOOL_StackFrame;

/* first frame and pointer to the last/current one */

EXT struct _WOOL_StackFrame wool_first_stackframe;
EXT WOOL_StackFrame wool_current_stackframe INIT(&wool_first_stackframe);
EXT WOOL_StackFrame wool_stackframe_on_error INIT(&wool_first_stackframe);

/* call stack */

EXT WOOL_OBJECT *calling_function_stack;
EXT WOOL_OBJECT *calling_function_current;
EXT WOOL_OBJECT *calling_function_end;
EXT int calling_function_on_error INIT(0);
EXT int wool_max_stack_print_level INIT(100);

#define calling_function_push(obj) \
    if (calling_function_current >= calling_function_end) { \
	int size = calling_function_end - calling_function_stack; \
	int ptr = calling_function_current - calling_function_stack; \
	calling_function_stack = (WOOL_OBJECT *) \
		Realloc(calling_function_stack, \
		    size * 2 * sizeof(WOOL_OBJECT) - 4); \
	calling_function_end = calling_function_stack +2*size -1; \
	calling_function_current = calling_function_stack + ptr; \
    } \
    *calling_function_current++ = (WOOL_OBJECT) (obj); \
    CheckLoopsPush()
#ifdef DEBUG
#define calling_function_pop() \
    if (calling_function_current == calling_function_stack) \
	wool_error("stack underflow%s", ""); \
    calling_function_current--; \
    CheckLoopsPop()
#else /* DEBUG */
#define calling_function_pop() \
    calling_function_current--; CheckLoopsPop()
#endif /* DEBUG */

/* debug */
#ifdef DEBUG
EXT int CheckLoopsN INIT(0);
#define CheckLoopsPop() CheckLoopsN--
#define CheckLoopsPush() if (CheckLoopsN++ > 1280) \
    wool_error("stack overflow%s", "")
#else
#define CheckLoopsPop()
#define CheckLoopsPush()
#endif

#endif /* INCLUDE_WL_FUNC_H */
