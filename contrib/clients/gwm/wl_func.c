/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/********************************************\
* 					     *
*  WOOL_OBJECT  Subr, FSubr, Expr and FExpr  *
*  BODY					     *
* 					     *
\********************************************/

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_atom.h"
#include "wl_number.h"
#include "wl_list.h"
#include "INTERN.h"
#include "wl_func.h"

/* Constructors:
 * For Subr and FSubr: wool_subr_make
 * takes as arguments:
 *  1-  the type (WLSubr or WLFSubr)
 *  2-  the (pointer to) C function associated
 *  3-  the string which will be its WOOL name
 *  4-  the number of arguments: 0,1,2 or NARY
 * Returns the (F)Subr.
 * 
 * For the Expr and FExpr: defun
 * takes as arguments:
 *  1-  the type (WLExpr or WLFExpr)
 *  2-  the argc,
 *  3-  the argv
 *  	of the list (funcname (list of args) stat1 stat2 ... statn)
 *  	corresponding to the WOOL definition.
 * Returns the (F)Expr.
 */

WOOL_OBJECT 
defun(type, argc, argv)
WOOL_TYPE       type;		/* WLExpr or WLFExpr */
int             argc;		/* the list without the "de" or "df" */
WOOL_Atom      *argv;
{
    WOOL_OBJECT func;

    if(argc < 2)
    	return wool_error(BAD_DEFUN, ((WOOL_Atom) (*(argv))) -> p_name);
    func = wool_lambda_make(argc -1, argv +1);
    func -> type = type;
    return WOOL_send(WOOL_setq, argv[0], (argv[0], func));
}

WOOL_OBJECT 
wool_subr_make(type, C_function, wool_name, arity)
WOOL_TYPE       type;
WOOL_OBJECT(*C_function) ();
char           *wool_name;
int             arity;
{
    WOOL_OBJECT     func = (WOOL_OBJECT)
    WLSubr_make(type, C_function, arity);
    WOOL_Atom       object = wool_atom(wool_name);

    increase_reference(object -> c_val = func);
    return (WOOL_OBJECT) func;
}

/*
 * WLSubr_make:
 * makes a (F)Subr (without knowing its name)
 */

WOOL_Subr 
WLSubr_make(type, C_function, arity)
WOOL_TYPE       type;

WOOL_OBJECT(*C_function) ();
int             arity;

{
    WOOL_Subr       object = (WOOL_Subr) Malloc(sizeof(struct _WOOL_Subr));

    object -> type = type;
    zrt_put(object);
    object -> arity = arity;
    object -> body = C_function;
    return object;
}

/*
 * the real creator of Exprs, lambda
 */

WOOL_OBJECT
wool_lambda_make(argc, argv)
int             argc;           /* the list without the "de" or "df" */
WOOL_OBJECT    *argv;
{
    WOOL_Expr       object;
    int             i;
    WOOL_List       parameters = (WOOL_List) argv[0];

    if (argc < 1)
	wool_error(BAD_DEFUN, ((WOOL_Atom) (*(argv))) -> p_name);
    if ((parameters -> type != WLList) && (argv[0] != NIL)) {
	if (parameters -> type == WLAtom) {	/* NARY */
	    parameters = (WOOL_List) wool_list(1, &(argv[0]));
	} else
	    wool_error(BAD_DEFUN, ((WOOL_Atom) (*(argv))) -> p_name);
    }
    if (parameters != (WOOL_List) NIL)
	for (i = 0; i < parameters -> size; i++) {
#ifdef	STUPID
	    WOOL_OBJECT tmp;
	    tmp = parameters -> list[i];
	    if (tmp -> type != WLAtom)
#else
	    if (parameters -> list[i] -> type != WLAtom)
#endif
		wool_error(NOT_REDEFINABLE, parameters -> list[i]);
	}
    object = (WOOL_Expr) Malloc(sizeof(struct _WOOL_Expr));
    object -> type = WLExpr;
    zrt_put(object);
    increase_reference(object);
    if (parameters == (WOOL_List) NIL)
	object -> arity = 0;
    else
	object -> arity = parameters -> size;
    if (object -> arity) {
	duplicate_n_objects(parameters -> list, &(object -> parameters),
			    object -> arity);
    }
    if (object -> body_size = argc - 1) {
	duplicate_n_objects(argv + 1, &(object -> body), object -> body_size);
    }
    if (argv[0] -> type == WLAtom && argv[0] != NIL)
    	object -> arity = NARY;
    return (WOOL_OBJECT) object;
}

WOOL_OBJECT
wool_lambdaq_make(argc, argv)
int             argc;           /* the list without the "de" or "df" */
WOOL_OBJECT    *argv;
{
    WOOL_OBJECT lambda = wool_lambda_make(argc, argv);
    lambda -> type = WLFExpr;
    return lambda;
}

/*
 * printing a function is pretty-printing its definition.
 */

WOOL_OBJECT 
WLFSubr_print(obj)
WOOL_FSubr      obj;
{
    wool_putchar('F');
    return WLSubr_print(obj);
}

WOOL_OBJECT 
WLSubr_print(obj)
WOOL_Subr       obj;
{
    if (obj -> arity == NARY) {
	wool_puts("SUBR n-ary: ");
    } else {
	wool_printf("SUBR with %d parameters: ", obj -> arity);
    }
    print_atom_pointing_to(obj);
    return (WOOL_OBJECT) obj;
}

WOOL_OBJECT 
WLExpr_print(obj)
WOOL_Expr       obj;
{
    int             i = 0;

    if (obj -> arity == NARY)
	wool_puts("EXPR n-ary: ");
    else
	wool_printf("EXPR with %d parameters: ", obj -> arity);
    print_atom_pointing_to(obj);
    for (; i < (obj -> arity != NARY ? obj -> arity : 1); i++) {
	WOOL_send(WOOL_print, *(obj -> parameters + i),
		  (*(obj -> parameters + i)));
	wool_putchar(' ');
    }
    for (i = 0; i < obj -> body_size; i++) {
	printf("\n    ");
	WOOL_send(WOOL_print, *(obj -> body + i), (*(obj -> body + i)));
    }
    return (WOOL_OBJECT) obj;
}

WOOL_OBJECT 
WLFExpr_print(obj)
WOOL_FExpr      obj;
{
    wool_putchar('F');
    return (WLExpr_print(obj));
}

/*
 * freeing:
 */

WOOL_OBJECT 
WLExpr_free(obj)
WOOL_Expr       obj;
{
    if (obj -> body_size) {
	decrease_reference_in_list(obj -> body_size, obj -> body);
	DelayedFree(obj -> body);
    }
    if (obj -> arity) {
	decrease_reference_in_list(obj -> arity, obj -> parameters);
	DelayedFree(obj -> parameters);
    }
    DelayedFree(obj);
    return NULL;
}

/*
 * main routines: execution!
 */

/*
 * Note: for NARY function, a list of evaluated args is created.
 */

WOOL_OBJECT 
WLSubr_execute(obj, list)
WOOL_Subr obj;		/* the function */
WOOL_List	   list;
{
    WOOL_OBJECT result;
    int    argc = list -> size - 1;
    WOOL_OBJECT *argv = list -> list + 1;

    /* NARY FUNCTIONS */
    if (obj -> arity == NARY) {
	if (argc) {
	    WOOL_OBJECT    *eval_list =
	    (WOOL_OBJECT *) Malloc(sizeof(WOOL_OBJECT) * argc);

	    result = (*(obj -> body)) (argc,
				       map_eval(argc, argv, eval_list));
	    Free(eval_list);
	    return result;
	} else {
	    return (*(obj -> body)) (0, 0);
	}
    } else {
	/* FIXED ARITY */
	if (obj -> arity != argc) {
	    return wool_error(BAD_NUMBER_OF_ARGS, argc);
	} else {
	    switch (obj -> arity) {
	    case 0:
		return (*(obj -> body)) ();
	    case 1:
		return (*(obj -> body)) (
				      WOOL_send(WOOL_eval, *argv, (*argv)));
	    case 2:
#ifdef STUPID
		{
		    WOOL_OBJECT     arg1 = WOOL_send(WOOL_eval, *argv,
						     (*argv));
		    WOOL_OBJECT     arg2 = WOOL_send(WOOL_eval, *(argv + 1),
						     (*(argv + 1)));

		    return (*(obj -> body)) (arg1, arg2);
		}
#else /* STUPID */
		return (*(obj -> body)) (
				       WOOL_send(WOOL_eval, *argv, (*argv)),
			  WOOL_send(WOOL_eval, *(argv + 1), (*(argv + 1))));
#endif /* STUPID */
	    default:
		return NIL;	/* should not be reached */
	    }
	}
    }
}

WOOL_OBJECT 
WLFSubr_execute(obj, list)
WOOL_FSubr obj;	/* the function */
WOOL_List          list;
{
    int    argc = list -> size - 1;
    WOOL_OBJECT *argv = list -> list + 1;

    if (obj -> arity == NARY) {
	return (*(obj -> body)) (argc, argv);
    } else {
	if (obj -> arity != argc) {
	    return wool_error(BAD_NUMBER_OF_ARGS, argc);
	} else {
	    switch (obj -> arity) {
	    case 0:
		return (*(obj -> body)) ();
	    case 1:
		return (*(obj -> body)) (*argv);
	    case 2:
		return (*(obj -> body)) (*argv, *(argv + 1));
	    default:
		return NIL;	/* should not be reached */
	    }
	}
    }
}

WOOL_OBJECT 
WLExpr_execute(obj, list)
WOOL_Expr obj;
WOOL_List          list;
{
    int    argc = list -> size - 1;
    WOOL_OBJECT *argv = list -> list + 1;

    if (obj -> arity != NARY) {
	if (!argc) {
	    return execute_local_code(0, 0, 0, 0,
				      obj -> body_size, obj -> body);
	} else {
	    WOOL_OBJECT result, *local_list;

	    local_list = (WOOL_OBJECT *)
		Malloc(sizeof(WOOL_OBJECT) * argc);
	    map_eval(argc, argv, local_list);
	    result = execute_local_code(argc, local_list,
					obj -> arity, obj -> parameters,
					obj -> body_size, obj -> body);
	    Free(local_list);
	    return (result);
	}
    } else {
	if (!argc) {
	    return execute_local_code(1, &NIL, 1, obj -> parameters,
				      obj -> body_size, obj -> body);
	} else {
	    WOOL_List       parameters_list = (WOOL_List)
	    wool_list_make_from_evaluated_array(argc, argv);

	    return execute_local_code(1, &parameters_list, 1,
			  obj -> parameters, obj -> body_size, obj -> body);
	}
    }
}

WOOL_OBJECT 
WLFExpr_execute(obj, list)
WOOL_FExpr obj;
WOOL_List          list;
{
    if (obj -> arity != NARY) {
	return execute_local_code(list -> size - 1, list -> list + 1,
				  obj -> arity, obj -> parameters,
				  obj -> body_size, obj -> body);
    } else {
	if (!(list -> size - 1)) {
	    return execute_local_code(1, &NIL, 1, obj -> parameters,
				      obj -> body_size, obj -> body);
	} else {
	    WOOL_List       parameters_list = (WOOL_List)
	    wool_list(list -> size - 1, list -> list + 1);

	    return execute_local_code(1, &parameters_list, 1,
			  obj -> parameters, obj -> body_size, obj -> body);
	}
    }

}

/***********************************************\
* 					        *
*  Accessory functions for evaluation purposes  *
* 					        *
\***********************************************/

/*
 * here goes all the stuff really needed to operate the evaluation
 * mecanism of WOOL.
 */

/*
 *  map_eval evaluates "a la APL"
 */

WOOL_OBJECT    *
map_eval(size, source, dest)
int    size;
WOOL_OBJECT *source, *dest;
{
    WOOL_OBJECT *last = source + size;

    while (source < last) {
	*dest = WOOL_send(WOOL_eval, *source, (*source));
	dest++, source++;
    }
    return dest - size;
}

/* 
 * execute_local_code:
 * main program for all function calls.
 * (note: values = NULL ==> alls params set to NIL) (handy hack)
 * Values will be check_referenced
 */

WOOL_OBJECT 
execute_local_code(values_size, values,
		   parameters_size, parameters,
		   body_size, body)
int             values_size, parameters_size, body_size;
WOOL_OBJECT *values, *parameters, *body;
{
    WOOL_OBJECT     result;

    if (values_size != parameters_size)
	wool_error(BAD_NUMBER_OF_ARGS, values_size);
    if (parameters_size) {
	WLStackFrame_push(parameters_size, parameters, values);
	result = (WOOL_OBJECT) progn(body_size, body);
	WLStackFrame_pop_for_function_calls();
	return result;
    } else {
	return (WOOL_OBJECT) progn(body_size, body);
    }
}

/*****************************\
* 			      *
* Local variables management  *
* 			      *
\*****************************/

/*
 * the simpler local variable declarations "WITH"
 * used as in (with (x 1 y 2) ...insts...)
 */

WOOL_OBJECT 
wool_with(argc, argv)
int    argc;
WOOL_OBJECT *argv;
{
    WOOL_OBJECT     result;
    WOOL_List       vars = (WOOL_List) argv[0];

    if ((argc < 2) || (vars -> size % 2 && vars -> size > 2))
	return wool_error(BAD_LOCAL_SYNTAX, "");
    if (((vars -> type != WLList) && (vars != (WOOL_List) NIL))
	|| (vars -> size % 2)) {
	vars = (WOOL_List) WOOL_send(WOOL_eval, vars, (vars));
	must_be_context(vars, 0);
    }
    if (vars == (WOOL_List) NIL) {
	return (WOOL_OBJECT) progn(argc - 1, argv + 1);
    }

    WLStackFrame_push_spaced_values(vars -> size / 2, vars->list);
    result = (WOOL_OBJECT) progn(argc - 1, argv + 1);
    WLStackFrame_pop();
    return result;
}

/*
 * with_eval evaluates first its first argument	before calling wool_with
 */

WOOL_OBJECT
wool_with_eval(argc, argv)
int    argc;
WOOL_OBJECT *argv;
{
    WOOL_OBJECT	*eval_args, result;
    int i;

    if (argc == 0)
	return wool_error(BAD_LOCAL_SYNTAX, "");
    eval_args = (WOOL_OBJECT *) Malloc(sizeof(WOOL_OBJECT) * argc);
    eval_args[0] = WOOL_send(WOOL_eval, argv[0], (argv[0]));
    for (i = 1; i <argc; i++){
	eval_args[i] = argv[i];
    }
    result = wool_with(argc, eval_args);
    Free(eval_args);
    return result;
}

/**************************\
* 			   *
* Stack frames management  *
* 			   *
\**************************/

/* initialize first stack frame */

WLStackFrame_init()
{
    wool_first_stackframe.previous = NULL;
    wool_first_stackframe.size = 0;
}

/* pushes arguments on stack (functional call) */

WLStackFrame_push(size, parameters, new_values)
int 		size;
WOOL_Atom      *parameters;
WOOL_OBJECT    *new_values;
{
    WOOL_StackFrame frame = (WOOL_StackFrame) Malloc(
	sizeof(struct _WOOL_StackFrame) + (size - 1) * sizeof(WOOL_OBJECT));
    WOOL_OBJECT    *old_values = frame -> old_values;
    WOOL_Atom      *end_param = parameters + size;

    CheckLoopsPush();
    frame -> previous = wool_current_stackframe;
    frame -> size = size;
    frame -> parameters = parameters;
    frame -> new_values = new_values;

    while (parameters < end_param) {
	*old_values++ = (*parameters) -> c_val;
	increase_reference((*parameters++) -> c_val = *new_values++);
    }
    wool_current_stackframe = frame;	/* last in case of errors in eval */
}

/* pushes arguments on stack (declarative (with) call)
 * parameters and new_values are set to point in the old_value space
 */

WLStackFrame_push_spaced_values(size, param_and_values)
int             size;
WOOL_Atom      *param_and_values;
{
    WOOL_StackFrame frame = (WOOL_StackFrame) Malloc(
    sizeof(struct _WOOL_StackFrame) + (size * 3 - 1) * sizeof(WOOL_OBJECT));
    WOOL_OBJECT    *old_values = frame -> old_values;
    WOOL_Atom      *parameters = (WOOL_Atom *) old_values + size;
    WOOL_OBJECT    *new_values = (WOOL_OBJECT *) parameters + size;
    WOOL_Atom      *end_param = (WOOL_Atom *) new_values;

    CheckLoopsPush();
    frame -> previous = wool_current_stackframe;
    frame -> size = size;
    frame -> parameters = parameters;
    frame -> new_values = new_values;

    while (parameters < end_param) {
	*parameters = *param_and_values++;
	*old_values =
	    WOOL_send(WOOL_silent_eval, *parameters, (*parameters));
	if (*old_values) increase_reference (*old_values);
	*new_values = WOOL_send(WOOL_eval,
				*param_and_values, (*param_and_values));
	WOOL_send(WOOL_setq, *parameters, (*parameters, *new_values));
	old_values++;
	new_values++;
	param_and_values++;
	parameters++;
    }
    wool_current_stackframe = frame;	/* last in case of errors in eval */
}

/* pushes one value on stack without affecting it
 */

WLStackFrame_push_value(variable)
WOOL_Atom       variable;
{
    WOOL_StackFrame frame = (WOOL_StackFrame)
	Malloc(sizeof(struct _WOOL_StackFrame) +
	       (1 * 3 - 1) * sizeof(WOOL_OBJECT));
    WOOL_OBJECT    *old_values = frame -> old_values;
    WOOL_Atom      *parameters = (WOOL_Atom *) old_values + 1;
    WOOL_OBJECT    *new_values = (WOOL_OBJECT *) parameters + 1;
    WOOL_Atom      *end_param = (WOOL_Atom *) new_values;

    CheckLoopsPush();
    frame -> previous = wool_current_stackframe;
    frame -> size = 1;
    frame -> parameters = parameters;
    frame -> new_values = new_values;

    *parameters = variable;
    *old_values =
	WOOL_send(WOOL_silent_eval, *parameters, (*parameters));
    if (*old_values) increase_reference (*old_values);
    wool_current_stackframe = frame; /* last in case of errors in eval */
}

/* restores old parameters values and de-pop frame stack */

WLStackFrame_pop()
{
    WOOL_Atom      *parameters = wool_current_stackframe -> parameters;
    WOOL_OBJECT    *old_values = wool_current_stackframe -> old_values;
    WOOL_Atom      *end_param = parameters + wool_current_stackframe -> size;
    WOOL_StackFrame previous_frame = wool_current_stackframe -> previous;

    CheckLoopsPop();
    while (parameters < end_param) {
	if (*old_values) {
	    WOOL_send(WOOL_setq, *parameters, (*parameters, *old_values));
	    decrease_reference(*old_values);
	} else {
	    decrease_reference((*parameters) -> c_val);
	    (*parameters) -> c_val = 0;
	}
	parameters++, old_values++;
    }
    Free(wool_current_stackframe);
    wool_current_stackframe = previous_frame;
}

/* special version for functions (only atoms there) */

WLStackFrame_pop_for_function_calls()
{
    WOOL_Atom      *parameters = wool_current_stackframe -> parameters;
    WOOL_OBJECT    *old_values = wool_current_stackframe -> old_values;
    WOOL_Atom      *end_param = parameters + wool_current_stackframe -> size;
    WOOL_StackFrame previous_frame = wool_current_stackframe -> previous;

    CheckLoopsPop();
    while (parameters < end_param) {
	if (*old_values) {
	    WOOL_send(WOOL_setq, *parameters, (*parameters, *old_values));
	    decrease_reference(*old_values);
	} else {
	    decrease_reference((*parameters) -> c_val);
	    (*parameters) -> c_val = UNDEFINED;
	}
	parameters++, old_values++;
    }
    Free(wool_current_stackframe);
    wool_current_stackframe = previous_frame;
}

/* pop all the frames from current to given one */

WLStackFrame_pop_to(to_frame)
WOOL_StackFrame to_frame;
{
    while (wool_current_stackframe != to_frame) {
	ASSERT(wool_current_stackframe -> previous);
	WLStackFrame_pop();
    }
}

/**************\
* 	       *
*  stack dump  *
* 	       *
\**************/

#define calling_function_initial_size 63

calling_function_init()
{
    calling_function_stack = (WOOL_OBJECT *) Malloc(sizeof(WOOL_OBJECT) *
					     calling_function_initial_size);
    calling_function_end = calling_function_stack
	+ calling_function_initial_size;
    calling_function_current = calling_function_stack;
}

/* prints the calling function */

wool_stack_dump(where)
int where;				/* 0 normal, 1 stderr */
{
    int level = calling_function_current - calling_function_stack;
    int last_printed = Max(0, level - wool_max_stack_print_level);

    if (wool_max_stack_print_level < 0)
	last_printed = 0;
    while (--level >= last_printed)
	calling_function_print(calling_function_stack[level], where, level);
}

/* prints one frame */

calling_function_print(calling_list, where, level)
WOOL_List	calling_list;
int where;                              /* 0 normal, 1 stderr */
int level;
{
    if (calling_list && calling_list != (WOOL_List) NIL) {
	if (where) {
	    WOOL_Atom       atom = (WOOL_Atom) calling_list -> list[0];

	    if (atom -> type == WLAtom)
		fprintf(stderr, "In function %s\n", atom -> p_name);
	} else {
	    wool_printf("[%d] ", level);
	    wool_print(calling_list);
	    wool_newline();
	}
    }
}
