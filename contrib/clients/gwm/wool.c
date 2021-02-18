
/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/**************************************************************\
* 							       *
* WOOL.c:						       *
* main body of interpreter				       *
* 							       *
* Here are defined Wool-only functions.			       *
* To add a function:					       *
* 	- declare it (coded in or extern)		       *
* 	- add it to the declaration of predefined functions    *
* 	  in the wool_init function			       *
* 							       *
\**************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h> 
#include <sys/times.h>
#include <signal.h> 
#ifdef SYSV
#include <string.h>
#else /* SYSV */
#include <sys/wait.h>
#include <strings.h>
#include <sys/timeb.h>
#endif /* SYSV */
#if defined(IBM_RT) && defined(SYSV)
#include <unistd.h>
#endif /* IBM_RT && SYSV */
#ifdef i386
#include <unistd.h>
#endif /* i386 */

#include "INTERN.h"
#include "wool.h"
#include "EXTERN.h"
#include "wl_atom.h"
#include "wl_coll.h"
#include "wl_func.h"
#include "wl_list.h"
#include "wl_number.h"
#include "wl_string.h"
#include "wl_pointer.h"
#include "wl_active.h"
#include "wl_name.h"
#include "yacc.h"

#ifdef STATS
extern WOOL_OBJECT zrtstats();
extern WOOL_OBJECT wlcfstats();
#endif /* STATS */
extern WOOL_OBJECT progn();
extern WOOL_OBJECT wool_print_nary();
extern WOOL_OBJECT wool_with(), wool_with_eval(); 
extern int 	wool_error_in_profile;
extern char 	*yytext;
DECLARE_strchr;

#ifdef DEBUG
#ifdef GWM
extern WOOL_OBJECT WLFsm_fp();
extern WOOL_OBJECT WLState_fp();
extern WOOL_OBJECT WLArc_fp();
#endif GWM
#endif DEBUG

/* VARS */

#ifdef SECURE
/* some helps */
char *wool_methods_names[] = {
    "WOOL_type_name",
    "WOOL_eval",
    "WOOL_print",
    "WOOL_free",
    "WOOL_execute",
    "WOOL_set",
    "WOOL_get_C_value",
    "WOOL_open",
    "WOOL_close",
    "WOOL_process_event",
    "WOOL_copy",
    "WOOL_get_dimensions",
    "WOOL_draw",
    "WOOL_equal",
    "WOOL_reconfigure",
    "WOOL_setq",
    "WOOL_silent_eval",
    "WOOL_UNDEFINED",
    "WOOL_UNDEFINED",
    "WOOL_UNDEFINED",
    "WOOL_UNDEFINED"
};

#endif /* SECURE */

#ifdef DO_BUS_ERROR
#define DoABusError() *((int *)0) = 1
#else /* DO_BUS_ERROR */
#define DoABusError()
#endif /* DO_BUS_ERROR */

/*
 * all constructors sets reference count of the object to 0
 * You should call check_reference on this object if it was only
 * a temporary one!
 */

/*******************\
* 		    *
*  run-time errors  *
* 		    *
\*******************/

static int             wool_error_position;
static char 	      *wool_error_expecting;
static int	       wool_error_print_argc;
static WOOL_OBJECT    *wool_error_print_argv;

/* echoes 1st, 2nd, 3rd, nth */

char *
English_enumeration_postfix(n)
int n;
{
    switch (n) {
	case 1: return "st";
	case 2: return "nd";
	case 3: return "rd";
	default: return "th";
    }
}

#ifdef __STDC__
extern void wool_print_error_message(int error, const void *message, char *text);
#else
extern void wool_print_error_message();
#endif

/*
 * wool_error:
 * This is the standard error function. It is really brutal, as it wool_prints
 * an error message an then LONGJMPs to "wool_goes_here_on_error" with an
 * error value, for now 1.
 * You can feel free to redefine wool_goes_here_on_error.
 * 
 * WARNING: Don't forget to clean your structures before going here,
 * you won't return!
 */

WOOL_OBJECT
#ifdef __STDC__
_wool_error(int error, const void *message)
#else
_wool_error(error, message)
int             error;		/* error number (see wool.h) */
char           *message;	/* anything, in fact */
#endif
{
    static int      is_in_wool_error;

    if (is_in_wool_error) {
	stop_if_in_dbx();
	goto direct_longjmp;
    }
    is_in_wool_error = 1;
    if (wool_do_print_errors && error != SILENT_ERROR) {
	wool_print_error_message(error, message, "WOOL ERROR: ");
	wool_stack_dump(0);
	fflush(stdout);
	fflush(stderr);
    }
    if (!wool_continue_reading_on_error)
	wool_error_status = 1;
    wool_error_handler();		/* C package handler */
    DoABusError();			/* debug only */
    WLStackFrame_pop_to(wool_stackframe_on_error);
    calling_function_current = calling_function_stack +
	calling_function_on_error;
direct_longjmp:
    is_in_wool_error = 0;
    stop_if_in_dbx();
    longjmp(wool_goes_here_on_error, 1);
    /* NOTREACHED *//* for lint */
    return NIL;
}

void
#ifdef __STDC__
wool_print_error_message(int error, const void *message, char *text)
#else
wool_print_error_message(error, message, text)
int             error;		/* error number (see wool.h) */
char           *message;	/* anything, in fact */
char           *text;		/* WOOL ERROR: */
#endif
{
#ifndef DEBUG
    wool_newline();
#endif /* DEBUG */
    if (wool_is_reading_file) {
	wool_printf("\"%s\"", wool_is_reading_file);
	wool_printf(": line %d\n", yylineno);
    }
    wool_puts(text);
    switch (error) {
    case UNDEFINED_VARIABLE:
	wool_printf("undefined variable: %s", message);
	break;
    case BAD_NUMBER_OF_ARGS:
	wool_printf("bad number of arguments %d", message);
	break;
    case UNDEFINED_FUNCTION:
	wool_puts("undefined function: ");
	wool_print(message);
	break;
    case BAD_DEFUN:
	wool_printf("bad definition of function: %s", message);
	break;
    case BAD_ARG_TYPE:
	wool_printf("bad %d", wool_error_position + 1);
	wool_printf("%s argument: ",
		    English_enumeration_postfix(wool_error_position + 1));
	wool_print(message);
	if (wool_error_expecting) {
	    wool_puts(" (its type is \"");
	    wool_print(((WOOL_OBJECT) message) -> type[0]);
	    if (wool_error_expecting[0])
	      wool_printf("\", was expecting a \"%s",
			  wool_error_expecting);
	    wool_puts("\")");
	}
	break;
    case RELEASING_ATOM:
	wool_printf("Internal error: trying to free atom: %s", message);
	break;
    case NOT_AN_ATOM:
	wool_printf("Not an atom: %s", message);
	break;
    case BAD_LOCAL_SYNTAX:
	wool_printf("bad local variable declaration. %s", message);
	break;
    case SYNTAX_ERROR:
	wool_printf("%s", message);
	break;
    case INTERNAL_ERROR:
	wool_printf("Internal error: %s", message);
	break;
    case TIME_EXCEEDED:
	wool_printf("Evaluation took longer than %ld second(s) -- Aborted",
		    message);
	break;
    case CANNOT_SET:
	wool_puts("Cannot set a \"");
	wool_print_type(message);
	wool_puts("\": ");
	wool_print(message);
	break;
    case CANNOT_GET_C_VALUE:
	wool_puts("Cannot figure how to get a number from a \"");
	wool_print_type(message);
	wool_puts("\": ");
	wool_print(message);
	break;
    case TOO_MANY_PARAMETERS:
	wool_puts("Too many parameters for a C function call: ");
	wool_print(message);
	break;
    case NON_WOOL_OBJECT:
	wool_printf("%s on a non wool object.", message);
	break;
    case UNDEFINED_METHOD:
#ifdef DEBUG
	wool_printf("Undef WOOL method %s for type \"",
		    wool_methods_names[WOOL_current_method]);
#else /* DEBUG */
	wool_puts("Bad object type \"");
#endif /* DEBUG */
	WLAtom_print(((WOOL_OBJECT) message) -> type[0]);
	wool_puts("\" for object: ");
	wool_print(message);
	break;
    case NO_MEMORY:
	wool_puts("No more memory, bye...\n");
	wool_end(1);
	break;
    case NOT_REDEFINABLE:
	wool_puts("Cannot redefine object: ");
	wool_print(message);
	break;
    case NOT_MODIFIABLE:
	wool_puts("Cannot modify object: ");
	wool_print(message);
	break;
    case USER_ERROR:
	wool_print_nary(wool_error_print_argc, wool_error_print_argv);
	break;
    default:			/* suppose first arg was a string! */
	wool_printf(error, message);
    }
    wool_newline();
}

/* wool_trigger_error
 * triggers an error from wool
 */

WOOL_OBJECT
wool_trigger_error(argc, argv)
int argc;
WOOL_OBJECT	argv[];
{
    wool_error_print_argc = argc;
    wool_error_print_argv = argv;
    return wool_error(USER_ERROR, 0);
}

/* wool_error_has_occurred:
 * encapsulates statements to trap errors without printing them
 */

WOOL_OBJECT
wool_error_has_occurred(argc, argv)
int argc;
WOOL_OBJECT	argv[];
{
    int             we_got_an_error = 0;
    int old_wool_do_print_errors = wool_do_print_errors;

    save_wool_error_resume_point();
    wool_do_print_errors = 0;
    if (set_wool_error_resume_point()) {
	we_got_an_error = 1;
    } else {
	progn(argc, argv);
    }
    wool_do_print_errors = old_wool_do_print_errors;
    restore_wool_error_resume_point();
    return (we_got_an_error ? TRU : NIL);
}

/* bad argument call
 */

WOOL_OBJECT
bad_argument(argument, position, expecting)
WOOL_OBJECT	argument;
int		position;
char	       *expecting;
{
    wool_error_position = position;
    wool_error_expecting = expecting;
    return wool_error(BAD_ARG_TYPE, argument);
}

/* wool warning messages 
 * returns 1 if printed
 */

int
wool_warning(alarm_text)
char	*alarm_text;
{
    if(wool_do_print_errors){
	wool_puts(alarm_text);
	if (wool_is_reading_file) {
	    wool_printf("\"%s\"", wool_is_reading_file);
	    wool_printf(": line %d, ", yylineno);
	}
	wool_stack_dump(0);
	return 1;
    } else
    	return 0;
}


/* executes an expression, returns if error (do not jump to toplevel)
 * return eval if no error occurred, NULL otherwise
 */

WOOL_OBJECT
wool_eval_and_catch_errors(wool_expr)
WOOL_OBJECT wool_expr;
{
    WOOL_OBJECT	    result = 0;

    save_wool_error_resume_point();	/* contains decls */
    if (set_wool_error_resume_point()) {
	result = 0;
    } else {
	result = WOOL_send(WOOL_eval, wool_expr, (wool_expr));
    }
    restore_wool_error_resume_point();
    return  result;
}

/* 
 * undefined method, one for each number of parameters.
 */

WOOL_OBJECT
wool_undefined_method_1(object)
WOOL_OBJECT object;
{
    return wool_error(UNDEFINED_METHOD, object);
}

WOOL_OBJECT
wool_undefined_method_2(object, param1)
WOOL_OBJECT object;
WOOL_OBJECT param1;
{
    return wool_error(UNDEFINED_METHOD, object);
}

#ifdef DEBUG
#define RMAGIC		0x5555	/* magic # on range info (see malloc.c)*/

extern char *malloc_zone_begin, *malloc_zone_end;

wool_send_is_valid(message, object)
int message;
WOOL_OBJECT object;
{
    if (!object
	|| ((char *) object) < malloc_zone_begin
	|| ((char *) object) >= malloc_zone_end
	|| *((int *) object) == FREED_MAGIC
	|| ((((WOOL_Atom) object -> type[0]) -> type != WLAtom)
	    && (((WOOL_Atom) object -> type[0]) -> type != WLActive)
	    && (((WOOL_Atom) object -> type[0]) -> type != WLPointer)
	    && (((WOOL_Atom) object -> type[0]) -> type != WLName)))
	wool_error(NON_WOOL_OBJECT, wool_methods_names[message]);
}
#endif /* DEBUG */

/*********************************************************\
* 							  *
* The definition of C WOOL routines			  *
* These constitue the WOOL interface to the C programmer  *
* 							  *
\*********************************************************/

/*
 * wool_read:
 * reads an expression from the input (string or stream).
 * returns this expression or NULL if EOF reached
 * In case of syntax error, returns NIL
 * the read expression is in the global variable wool_read_expr,
 * if you need it. (this global is maintained for ref count purposes)
 * You don't need to free it since it's done at the beginning of this
 * routine.
 * Beware that it could be overwritten by a subsequent call to wool_eval
 * or wool_read !
 */

WOOL_OBJECT
wool_read()
{
    if (yyparse())
	return NULL;		/* returns NULL if error */
    else
	return wool_read_expr;	/* returns parsed expression */
}

/*
 * wool_pool:
 * this routine MUST be used when you want to make successive calls to
 * wool_read to parse an expression. It stores the string passed as argument
 * and returns the parenthese level. Thus a normal use would be to call
 * wool_pool with successive lines, while it returns a non-zero value,
 * and then call wool_read on the pooled buffer maintained by wool_poll,
 * whose address is stored in the global wool_pool_buffer.
 * (the parenthese level is in the int wool_pool_parenthese_level)
 * The buffer is reset by calling wool_pool with a NULL argument.
 */

int
wool_pool(s)
char           *s;		/* the string to be put in the pool */
{
    int             required_length;

    if (!wool_pool_buffer)
	wool_pool_buffer =
	    (char *) Malloc(wool_pool_buffer_size);
    if (!s) {
	*wool_pool_buffer = '\0';
	return wool_pool_parentheses_level = 0;
    } else {
	if ((strlen(wool_pool_buffer) + (required_length = strlen(s)))
	    >= wool_pool_buffer_size) {
	    wool_pool_buffer_size +=
		Max(wool_pool_buffer_size, required_length) + 4;
	    wool_pool_buffer = (char *)
		Realloc(wool_pool_buffer, wool_pool_buffer_size);
	}
	strcat(wool_pool_buffer, "\n");
	strcat(wool_pool_buffer, s);
	{
	    char           *oldstream, old_input_buf[AHEAD_BUF_SIZE];
	    int             old_type;

	    old_type = wool_input_redirect(1, s, &oldstream, old_input_buf);
	    while (yylex() != END_OF_FILE);
	    wool_input_redirect(old_type, oldstream, NULL, NULL);
	    wool_unput(old_input_buf);
	    return wool_pool_parentheses_level;
	}
    }
}

/*
 * wool_eval:
 * evals an expression given as argument;
 * returns the result of the evaluation
 * if you want to keep the result, increase its reference count!
 * In case of eval error, calls wool_error which returns NIL
 */

WOOL_OBJECT
wool_eval(read_expr)
WOOL_OBJECT read_expr;
{
    return WOOL_send(WOOL_eval, read_expr, (read_expr));
}

/***************************************************************************\
* 									    *
* WOOL USER routines:							    *
* here are the definition of the standard routines binded to wool atoms by  *
* wool_init								    *
* 									    *
\***************************************************************************/

/*
 * The NULL function is there only as a placeholder
 */

WOOL_OBJECT
NIL_FUNC()
{
    return NIL;
}

/*
 * quoting can be implemented as a function:
 * 'foo ==> (quote foo)
 */

WOOL_OBJECT
wool_quote(obj)
WOOL_OBJECT obj;
{
    return obj;
}

/*
 * eval is the opposite of quoting
 */

WOOL_OBJECT
eval(obj)
WOOL_OBJECT     obj;
{
    return WOOL_send(WOOL_eval, obj, (obj));
}

/* copy an object (useful for lists)
 */

WOOL_OBJECT
wool_copy(obj)
WOOL_OBJECT     obj;
{
    return WOOL_send(WOOL_copy, obj, (obj));
}

/*
 * Arithmetic functions, patterned along Le_Lisp ones
 */

WOOL_OBJECT
wool_divide(n1, n2)
WOOL_Number n1, n2;
{
    return (WOOL_OBJECT) WLNumber_make(n2 -> number ?
				       n1 -> number / n2 -> number : 0);
}

WOOL_OBJECT
wool_modulo(n1, n2)
WOOL_Number n1, n2;
{
    Num tmp = n2 -> number ? n1 -> number % n2 -> number : 0;
    
    if (tmp < 0)
	tmp = tmp + n2 -> number;
    
    return (WOOL_OBJECT) WLNumber_make(tmp);
}

WOOL_OBJECT
wool_multiply(n1, n2)
WOOL_Number n1, n2;
{
    return (WOOL_OBJECT) WLNumber_make(n1 -> number * n2 -> number);
}

WOOL_OBJECT
wool_add(argc,argv)
int		argc;
WOOL_Number     argv[];
{
    WOOL_TYPE       type;

    if (argc == 0)
	wool_error(BAD_NUMBER_OF_ARGS, (char *)argc);
    type = argv[0] -> type;
    if (type == WLList || argv[0] == (WOOL_Number) NIL)
	return WLList_concat(argc, argv);
    else if (type == WLNumber)
	return add_numbers(argc, argv);
    else
	must_be_string(argv[0], 0);
    return add_strings(argc, argv);
}

WOOL_OBJECT
wool_minus(argc, argv)
int		argc;
WOOL_Number     argv[];
{
    switch (argc) {
    case 1:
	return (WOOL_OBJECT) WLNumber_make(-(argv[0] -> number));
    case 2:
	return (WOOL_OBJECT) WLNumber_make(
				 (argv[0] -> number) - (argv[1] -> number));
    case 0:
	return wool_error(BAD_NUMBER_OF_ARGS, (char *)argc);
    default: {int i, result = argv[0] -> number;
	      for (i = 1; i < argc; i++)
		  result -= argv[i] -> number;
	      return (WOOL_OBJECT) WLNumber_make(result);
	  }	
    }
}

/* comparisons
 */

WOOL_OBJECT
wool_equal(o1, o2)
WOOL_OBJECT     o1, o2;
{
    return WOOL_send(WOOL_equal, o1, (o1, o2));
}

WOOL_OBJECT
wool_eq(o1, o2)
WOOL_OBJECT     o1, o2;
{
    if (o1 == o2)
	return TRU;
    else
	return NIL;
}

WOOL_OBJECT
greater_than(o1, o2)
WOOL_OBJECT     o1, o2;
{
    if (o1 -> type != o2 -> type)
	return NIL;
    if (((o1 -> type == WLNumber) &&
	 (((WOOL_Number) o1) -> number > ((WOOL_Number) o2) -> number))
	|| ((o1 -> type == WLString) &&
	    strcmp(((WOOL_String) o1) -> string,
		   ((WOOL_String) o2) -> string) == 1))
	return TRU;
    else
	return NIL;
}

WOOL_OBJECT
lesser_than(o1, o2)
WOOL_OBJECT     o1, o2;
{
    if (o1 -> type != o2 -> type)
	return NIL;
    if (((o1 -> type == WLNumber) &&
	 (((WOOL_Number) o1) -> number < ((WOOL_Number) o2) -> number))
	|| ((o1 -> type == WLString) &&
	    strcmp(((WOOL_String) o1) -> string,
		   ((WOOL_String) o2) -> string) == -1))
	return TRU;
    else
	return NIL;
}
    
/* wool_compare returns -1, 0, +1 if <, =, > and () if not comparable.
 */

WOOL_OBJECT
wool_compare(o1, o2)
WOOL_Number     o1, o2;
{
    if (o1 -> type != o2 -> type)
	return NIL;
    if (o1 -> type == WLNumber) {
	if (o1 -> number < o2 -> number)
	    return (WOOL_OBJECT) WLNumber_make(-1);
	else if (o1 -> number > o2 -> number)
	    return (WOOL_OBJECT) WLNumber_make(1);
	else
	    return (WOOL_OBJECT) WLNumber_make(0);
    } else {
	must_be_string(o1, 0);
	return (WOOL_OBJECT) WLNumber_make(
					strcmp(((WOOL_String) o1) -> string,
					     ((WOOL_String) o2) -> string));
    }
}

/* member of a list, or substring of a string:
 * returns position in list or string or NIL if not found
 */

WOOL_OBJECT
wool_member(obj, list)
WOOL_String	obj;
WOOL_List	list;
{
    if (list -> type == WLList) {	/* list */
	WOOL_OBJECT    *p = list -> list;
	WOOL_OBJECT    *last = p + list -> size;

	while (p < last) {
	    if (WOOL_send(WOOL_equal, obj, (obj, *p)) != NIL)
		return (WOOL_OBJECT) WLNumber_make(p - list -> list);
	    p++;
	}
	return NIL;
    } else if (list == (WOOL_List) NIL) {	/* () */
	return NIL;
    } else {				/* substring of a string */
	char           *p, *string;
	int             length;

	must_be_string(list, 1);
	must_be_string(obj, 0);
	string = ((WOOL_String) list) -> string;
	length = strlen(obj -> string);
	for (p = string; *p; p++)
	    if (!strncmp(obj -> string, p, length))
		return (WOOL_OBJECT) WLNumber_make(p - string);
	return NIL;
    }
}

/* logical operations
 */

WOOL_OBJECT
not(obj)
WOOL_OBJECT     obj;
{
    if (obj == NIL)
	return TRU;
    else
	return NIL;
}

WOOL_OBJECT
and(argc, argv)
int		argc;
WOOL_OBJECT     argv[];
{
    int    i;

    for (i = 0; i < argc; i++)
	if (WOOL_send(WOOL_eval, argv[i], (argv[i])) == NIL)
	    return NIL;
    return TRU;
}

WOOL_OBJECT
or(argc, argv)
int		argc;
WOOL_OBJECT     argv[];
{
    int    i;
    WOOL_OBJECT tmp;

    for (i = 0; i < argc; i++)
	if ((tmp = WOOL_send(WOOL_eval, argv[i], (argv[i]))) != NIL)
	    return tmp;
    return NIL;
}

/*
 * bitwise-operators
 */

WOOL_OBJECT
wool_bitwise_or(argc, argv)
int argc;
WOOL_Number argv[];
{
    int             num = 0;

    while (argc--)
	num |= argv[argc] -> number;
    return (WOOL_OBJECT) WLNumber_make(num);
}

WOOL_OBJECT
wool_bitwise_and(argc, argv)
int argc;
WOOL_Number argv[];
{
    int             num = argv[0] -> number;

    while (argc--)
	num &= argv[argc] -> number;
    return (WOOL_OBJECT) WLNumber_make(num);
}

WOOL_OBJECT
wool_bitwise_xor(argc, argv)
int argc;
WOOL_Number argv[];
{
    int             num = 0;

    while (argc--)
	num ^= argv[argc] -> number;
    return (WOOL_OBJECT) WLNumber_make(num);
}

/*
 * Setq, the most important function
 * implemented as a method
 */

WOOL_OBJECT
setq(atom, value)
WOOL_Atom atom;
WOOL_OBJECT value;
{
    return WOOL_send(WOOL_set, atom, (atom, value));
}

WOOL_OBJECT
set(atom, value)
WOOL_Atom atom;
WOOL_OBJECT value;
{
    return WOOL_send(WOOL_setq, atom, (atom, value));
}

/*
 * unbind: release storage of an atom
 */

WOOL_OBJECT
wool_unbind(atom)
WOOL_Atom atom;
{
    extern WOOL_OBJECT WLAtom_unbind(), WLName_unbind();

    if (atom -> type == WLAtom)
	return WLAtom_unbind(atom);
    else if (atom -> type == WLName)
	return WLName_unbind(atom);
    else
	return bad_argument(atom, 0, "atom or name");
}

/*
 * boundp: tests if atom has already be defined
 */

WOOL_OBJECT
wool_boundp(atom)
WOOL_OBJECT atom;
{
    WOOL_OBJECT	value = WOOL_send(WOOL_silent_eval, atom, (atom));

    if (value && value != UNDEFINED_WOOL_VALUE)
	return (atom == NIL ? TRU : atom);
    else
	return NIL;
}

/*
 * list: makes a list of its evaluated arguments
 */

WOOL_OBJECT
wool_list(argc, argv)
int argc;
WOOL_OBJECT     argv[];
{
    WOOL_List list = wool_list_make(argc);
    WOOL_OBJECT *q = list -> list, *last = argv + argc;

    while (argv < last)
        increase_reference(*q++ = *argv++);
    return (WOOL_OBJECT) list;
}

/*
 * length: of a string or list
 */

WOOL_Number
wool_length(obj)
WOOL_List     obj;
{
    if (obj -> type == WLList)
	return WLNumber_make(obj -> size);
    else if (obj == (WOOL_List) NIL)
	return WLNumber_make(0);
    else {
	must_be_string(obj, 0);
	return WLNumber_make(strlen(((WOOL_String) obj) -> string));
    }
}

/*
 * BEWARE: hacker's corner!
 * returns the object of the same type found at location number!
 * 	type		action
 * 	number		*int
 * 	string		*char
 * 	()		object
 * 	atom		adress of pointer object
 */

WOOL_OBJECT
wool_hack(type, pointer)
WOOL_OBJECT type;
WOOL_Number pointer;
{
    if (type -> type == WLNumber)
	return (WOOL_OBJECT) WLNumber_make(*((int *) pointer -> number));
    else if (type -> type == WLString)
	return (WOOL_OBJECT) WLString_make(pointer -> number);
    else if (type == NIL) {
	increase_reference(pointer -> number);
	return (WOOL_OBJECT) pointer -> number;
    } else if (type -> type == WLAtom)
	return (WOOL_OBJECT) WLNumber_make(pointer);
    else
	return NIL;
}

/* used time function expressed in milliseconds
 */

#define TIME_UNIT 60

WOOL_OBJECT
wool_used_time()
{
    long time;

#ifdef SYSV
    struct tms buffer;

    times(&buffer);
    time = ((buffer.tms_utime + buffer.tms_stime) * 1000 ) / TIME_UNIT;
#else /* SYSV */
    struct timeb time_bsd;

    ftime(&time_bsd);
    time = 1000 * time_bsd.time + time_bsd.millitm;
#endif /* SYSV */

    return (WOOL_OBJECT) WLNumber_make(time);
}

/*
 * atoi and itoa
 */

WOOL_OBJECT
wool_atoi(obj)
WOOL_String obj;
{
    must_be_string(obj, 0);
    return (WOOL_OBJECT) WLNumber_make(atoi(obj -> string));
}

WOOL_OBJECT
wool_itoa(obj)
WOOL_Number obj;
{
    char tmp_str[20];

    must_be_number(obj, 0);
    sprintf(tmp_str, "%d", obj -> number);
    return (WOOL_OBJECT) WLString_make(tmp_str);
}

/*
 * Shell escape: executes a SYSTEM of the string (or atom) argument
 */

WOOL_OBJECT
shell(argc, argv)
int		argc;
WOOL_String     argv[];
{
    int             i;
    char          **program_args = (char **) Malloc(sizeof(char *) * (argc + 1));

    if (!argc)
	wool_error(BAD_NUMBER_OF_ARGS, (char *)argc);
    for (i = 0; i < argc; i++) {
	must_be_string(argv[i], i);
	program_args[i] = argv[i] -> string;
    }
    program_args[argc] = NULL;
    if (!fork()) {
	wool_clean_before_exec();
	execvp(program_args[0], program_args);
	exit(127);
    }
    return NIL;
}

/* signals management avoiding defunct processes */

#ifdef SYSV
void
ChildDeathHandler(sig)
int sig;
{
    wait(0);
    signal(SIGCLD, ChildDeathHandler);
}

SignalsInit()
{
    signal(SIGCLD, ChildDeathHandler);
}
#else /* SYSV */
#include <sys/time.h>
#include <sys/resource.h>
void
ChildDeathHandler(sig)
int sig;
{
    union wait status;
    wait3(&status, WNOHANG, 0);
    signal(SIGCHLD, ChildDeathHandler);
}

SignalsInit()
{
    signal(SIGCHLD, ChildDeathHandler);
}
#endif /* SYSV */

/*
 *  print value of an object
 */

WOOL_OBJECT
wool_print(obj)
WOOL_OBJECT     obj;
{
    wool_print_level = 0;
    return WOOL_send(WOOL_print, obj, (obj));
}

WOOL_OBJECT
wool_print_nary(argc, argv)
int argc;
WOOL_OBJECT     argv[];
{
    int             i;
    WOOL_OBJECT	result = NIL;

    wool_print_level = 0;
    for (i = 0; i < argc; i++)
	result = WOOL_send(WOOL_print, argv[i], (argv[i]));
    yyoutflush();
    return result;
}

/*
 * (progn inst1 ... instn)
 * evals the n instructions then return the last one's result
 */

WOOL_OBJECT
progn(argc, argv)
int    argc;
WOOL_OBJECT *argv;
{
    if (argc) {
	int             local_zrt_size = zrt_size;

	while (--argc > 0) {
	    WOOL_send(WOOL_eval, *argv, (*argv));
	    zrt_gc(local_zrt_size);
	    argv++;
	}
	return WOOL_send(WOOL_eval, *argv, (*argv));
    } else {
	return NIL;
    }
}

/*
 * if "a la emacs"
 * if test thenclause [test thenclause]* [elseclause]
 * nearly a COND, in fact
 */

WOOL_OBJECT
wool_if(argc, argv)
int    argc;
WOOL_OBJECT *argv;
{
    while (argc > 1) {
	if (WOOL_send(WOOL_eval, *argv, (*argv)) != NIL) {
	    return WOOL_send(WOOL_eval, *(argv + 1), (*(argv + 1)));
	}
	argc -= 2;
	argv += 2;
	if (argc == 1) {
	    return WOOL_send(WOOL_eval, *argv, (*argv));
	}
    }
    return NIL;
}

/*
 * while cond inst1 ... instn
 * classical while
 */

WOOL_OBJECT
wool_while(argc, argv)
int    argc;
WOOL_OBJECT *argv;
{
    while (WOOL_send(WOOL_eval, *argv, (*argv)) != NIL) {
	progn(argc - 1, argv + 1);
    }
    return NIL;
}

/*
 * for:
 * (for var list-of-values instructions...)
 */

WOOL_OBJECT
wool_for_loop(argc, argv, map)
int    argc;
WOOL_List *argv;
int		map;
{
    WOOL_List       list, result_list;
    WOOL_OBJECT	    /* previous_value, */ result;
    int i;

    if (argc < 3)
	wool_error(BAD_NUMBER_OF_ARGS, (char *)argc);
    must_be_atom(argv[0], 0);
    list = (WOOL_List) WOOL_send(WOOL_eval, argv[1], (argv[1]));
    if (WLList_length(list) == 0)  /* (list == (WOOL_List) NIL) */
    	return NIL;

    WLStackFrame_push_value(argv[0]);

    if(map) {
    	result_list = wool_list_make(list -> size);
    	for (i = 0; i < list -> size; i++) {
	    WOOL_send(WOOL_setq, argv[0], (argv[0], list -> list[i]));
	    increase_reference(result_list -> list[i] =
		progn(argc - 2, argv + 2));
        }
    } else {
	for (i = 0; i < list -> size; i++) {
            WOOL_send(WOOL_setq, argv[0], (argv[0], list -> list[i]));
            result = progn(argc - 2, argv + 2);
	}
    }

    WLStackFrame_pop();

    return (map ? (WOOL_OBJECT) result_list : result);
}

WOOL_OBJECT
wool_for(argc, argv)
int    argc;
WOOL_List *argv;
{
    return wool_for_loop(argc, argv, 0);
}

WOOL_OBJECT
wool_mapfor(argc, argv)
int    argc;
WOOL_List *argv;
{
    return wool_for_loop(argc, argv, 1);
}



/*
 * TAG/EXIT:
 * (tag tag insts...)
 * (exit tag insts...)
 */

typedef struct _JumpingPoint {
    WOOL_StackFrame frame;
    int 	    level; 		/* in calling_function_stack */
    jmp_buf	    jump_buffer;
} *JumpingPoint;

WOOL_OBJECT
wool_tag(argc, argv)
int             argc;
WOOL_String    *argv;
{
    struct _JumpingPoint tag;
    WOOL_OBJECT     result;
    WOOL_Pointer    tag_name;

    if (argc < 2)
	wool_error(BAD_NUMBER_OF_ARGS, (char *)argc);
    must_be_string(argv[0], 0);
    wool_self_pointer_make(argv[0] -> string, '\024', &tag_name);
    *(tag_name -> ptr) = (long) &tag;
    tag.frame = wool_current_stackframe;
    tag.level = calling_function_current - calling_function_stack;
    if (!(result = (WOOL_OBJECT) setjmp(tag.jump_buffer))) {
	result = progn(argc - 1, argv + 1);	/* initialize tag */
    }
    *(tag_name -> ptr) = 0;
    return result;
}
	
WOOL_OBJECT
wool_exit(argc, argv)
int             argc;
WOOL_String    *argv;
{
    JumpingPoint    tag;
    WOOL_Pointer    tag_name;
    WOOL_OBJECT     result;

    if (argc < 1)
	wool_error(BAD_NUMBER_OF_ARGS, (char *)argc);
    must_be_string(argv[0], 0);
    if (wool_self_pointer_make(argv[0] -> string, '\024', &tag_name)
	&& *(tag_name -> ptr)) {
	tag = (JumpingPoint) * (tag_name -> ptr);
	result = argc > 1 ? progn(argc - 1, argv + 1) : NIL;
	WLStackFrame_pop_to(tag -> frame);
	calling_function_current = calling_function_stack + tag -> level;
	longjmp(tag -> jump_buffer, (int)result);
    } else
	wool_error(UNDEFINED_VARIABLE, argv[0] -> string);
    return NIL;
}

/* the host name as a string
 */

WOOL_OBJECT
wool_hostname_get()
{
    if (!wool_host_name) {
	char buf[256];
	int maxlen = 256;
	int len;
#ifdef SYSV
#include <sys/utsname.h>
	struct utsname name;
	
	uname (&name);
	len = strlen (name.nodename);
	if (len >= maxlen) len = maxlen - 1;
	strncpy (buf, name.nodename, len);
	buf[len] = '\0';
#else /* SYSV */
	buf[0] = '\0';
	(void) gethostname (buf, maxlen);
	buf [maxlen - 1] = '\0';
	len = strlen(buf);
#endif /* SYSV */
	increase_reference(wool_host_name = (WOOL_OBJECT) WLString_make(buf));
    }
    return  wool_host_name;
}

/*
 * very useful: de and df!
 * USAGE:
 * (de <func-name> (parameter-list) inst1 ... instn)
 * returns  the atom pointing to the subr
 */

WOOL_OBJECT
de(argc, argv)
int             argc;
WOOL_OBJECT    *argv;
{
    return defun(WLExpr, argc, argv);
}

WOOL_OBJECT
df(argc, argv)
int             argc;
WOOL_OBJECT    *argv;
{
    return defun(WLFExpr, argc, argv);
}

/*
 * wool_loadfile:
 * raw loadfile function: search EXACTLY for name in parameter
 */

WOOL_OBJECT
wool_loadfile(string)
char           *string;
{
    FILE           *fd, *oldinput;
    char            filename[MAX_TEMP_STRING_SIZE];
    int             local_zrt_size = zrt_size;

    if (!string)
	return NIL;
    strcpy(filename, string);
    fd = fopen(filename, "r");
    if (fd) {
	int             we_got_an_error = 0;
	char           *old_file = wool_is_reading_file;
	char            old_input_buffer[AHEAD_BUF_SIZE];
	int             oldtype = wool_input_redirect(0, fd, &oldinput,
						      old_input_buffer);
	int             old_yylineno = yylineno;

	save_wool_error_resume_point();	/* contains decls */
	yylineno = 1;
	wool_is_reading_file = filename;
	/* now, we must close the file and redirect input on error */
	if (wool_continue_reading_on_error) {
	    set_wool_error_resume_point();
	    while (wool_read()) {
		wool_eval(wool_read_expr);
		zrt_gc(local_zrt_size);
	    }
	} else {
	    if (set_wool_error_resume_point()) {
		we_got_an_error = 1;
	    } else {
		while (wool_read()) {
		    wool_eval(wool_read_expr);
		    zrt_gc(local_zrt_size);
		}
	    }
	}
	wool_input_redirect(oldtype, oldinput, 0, 0);
	wool_unput(old_input_buffer);
	fclose(fd);
	restore_wool_error_resume_point();
	wool_is_reading_file = old_file;
	yylineno = old_yylineno;
	if (we_got_an_error)
	    _wool_error(SILENT_ERROR, 0);
	return TRU;
    } else {
	return NIL;
    }
}

/* executes a given string */

WOOL_OBJECT
wool_execute_string(string)
char           *string;
{
    FILE           *oldinput;
    int             we_got_an_error = 0;
    char           *old_file = wool_is_reading_file;
    char            old_input_buffer[AHEAD_BUF_SIZE];
    int             oldtype = wool_input_redirect(1, string, &oldinput,
						  old_input_buffer);
    int             old_yylineno = yylineno;
    int             local_zrt_size = zrt_size;

    save_wool_error_resume_point();
    yylineno = 1;
    wool_is_reading_file = 0;
    /* now, we redirect input on error */
    if (set_wool_error_resume_point()) {
	we_got_an_error = 1;
    } else {
	while (wool_read()) {
	    wool_eval(wool_read_expr);
	    zrt_gc(local_zrt_size);
	}
    }
    wool_input_redirect(oldtype, oldinput, 0, 0);
    wool_unput(old_input_buffer);
    restore_wool_error_resume_point();
    wool_is_reading_file = old_file;
    yylineno = old_yylineno;
    if (we_got_an_error)
	return NIL;
    else
	return TRU;
}

/* same callable from wool */

WOOL_OBJECT
wool_execute_wool_string(string)
WOOL_String string;
{
    return wool_execute_string(string -> string);
}

/*
 * tests if file exists and is readable
 */

char *
file_exists(name)
char *name;
{
    if(access(name, R_OK)) {
	return 0;
    }else{
	return name;
    }
}

/*
 * file_with_optional_extension:
 * see if file exists with extension
 */

char *
file_with_optional_extension(filename, extension)
char           *filename;
char           *extension;
{
    static char     filename_wl[MAX_TEMP_STRING_SIZE];

    strcpy(filename_wl, filename);
    strcat(filename_wl, extension);
    if(file_exists(filename_wl))
    	return filename_wl;
    return file_exists(filename);
}

/*
 * file_in_path:
 * find file with using path, extensions, etc...
 * complete_filename is a pointer to temporary space
 */

char *
file_in_path(filename, extension, path, complete_filename)
char           *filename, *extension, *path, *complete_filename;
{
    char  *directory, *name;
    int 	    dirlen;

    if (strchr(filename, '/')) {	/* absolute pathname */
	return (file_with_optional_extension(filename, extension));
    } else {			/* relative pathname */
	while (*path) {
	    directory = path;
	    dirlen = 0;
	    while (*path && (*path != ':')) {
		path++;
		dirlen++;
	    }
	    if (*path)
		path++;
	    complete_filename[dirlen] = '\0';
	    if (dirlen) {
		strncpy(complete_filename, directory, dirlen);
		if (complete_filename[dirlen - 1] != '/')
		    strcat(complete_filename, "/");
	    }
	    strcat(complete_filename, filename);
	    if (name = file_with_optional_extension(complete_filename,
						    extension))
		return name;
	}
	return 0;
    }
}


/*
 * loading a file (callable from wool)
 */

WOOL_OBJECT
wool_loadfile_in_path(string)
WOOL_String     string;
{
    char            temp_filename[MAX_TEMP_STRING_SIZE];
    char	   *actual_pathname = file_in_path(string -> string,
    			wool_text_extension, wool_path, temp_filename);

    if(NIL == wool_loadfile(actual_pathname)) {
	wool_puts(wool_application_NAME);
    	wool_printf(": file not found: %s\n", string -> string);
	return NIL;
    } else {
    	return (WOOL_OBJECT) WLString_make(actual_pathname);
    }
}

/*
 * cond for compatibility
 */

WOOL_OBJECT 
wool_cond(argc, argv)
int    argc;
WOOL_OBJECT *argv;
{
    WOOL_OBJECT    *list;
    WOOL_OBJECT     result = wool_if(argc * 2,
				  list = wool_flatten_pairlist(argc, argv));

    Free(list);
    return result;
}

/*
 * A context is a list of pairs variable-name/variable-values (atom/object).
 * Context operations are:
 * 
 * 	context-save: archives in the context the current values of
 * 		variables (sets to () undefined ones...)
 * 	context-restore: sets the variables to their archived values
 */

must_be_context(context, n)
WOOL_List	context;
int		n;
{
    if (((context -> type != WLList)
	 && (context != (WOOL_List) NIL))
	|| (context -> size % 2))
	bad_argument(context, n, "even-sized list");
}

WOOL_OBJECT
wool_context_save(context)
WOOL_List	context;
{
    int             i;
    WOOL_List       new;

    must_be_context(context, 0);
    new = wool_list_make(context -> size);
    for (i = 0; i < context -> size; i += 2) {
#ifdef        STUPID
      WOOL_OBJECT tmp;
#endif        STUPID
      increase_reference(new -> list[i] = context -> list[i]);
#ifdef  STUPID
      tmp = context -> list[i];
      if (tmp -> type == WLAtom &&
#else
      if (context -> list[i] -> type == WLAtom &&
#endif
	    (!((WOOL_Atom) context -> list[i]) -> c_val)) 
	    /*
	     * if atom is undefined, take the following element of the list
	     * as a value of new context 
	     */
	    increase_reference(new -> list[i + 1] = context -> list[i + 1]);
	else
	    increase_reference(new -> list[i + 1] =
	    WOOL_send(WOOL_eval, context -> list[i], (context -> list[i])));
    }
    return (WOOL_OBJECT) new;
}

WOOL_OBJECT
wool_context_restore(context)
WOOL_List	context;
{
    int             i;

    must_be_context(context, 0);
    for (i = 0; i < context -> size; i += 2)
	WOOL_send(WOOL_setq, context -> list[i],
		  (context -> list[i], context -> list[i + 1]));
    return (WOOL_OBJECT) context;
}

/*
 * wool_getenv:
 * makes the WOOL_String out of getenv(wool_string)
 */

WOOL_OBJECT
wool_getenv(obj)
WOOL_String obj;
{
    char  *result;

    if (result = (char *) getenv(obj -> string))
	return (WOOL_OBJECT) WLString_make(result);
    else
	return (WOOL_OBJECT) NIL_STRING;
}

/*
 * makes an atom out of a string
 */

WOOL_OBJECT
wool_atom_of_string(s)
WOOL_String	s;
{
    return (WOOL_OBJECT) wool_atom(s -> string);
}

/*
 * if object is from type, ok.
 * if not, evaluates it and call wool_error if result is still not
 * YOU MUST check reference on result when no longer needed!
 */

WOOL_OBJECT
wool_type_or_evaluate(object, type)
WOOL_OBJECT object;
WOOL_TYPE   type;
{
    if ((object == NIL) || (object -> type == type))
	return object;
    if (((object = WOOL_send(WOOL_eval, object, (object))) -> type == type)
	|| (object == NIL))
	return object;
    if (object == UNDEFINED_WOOL_VALUE)
	return wool_error(UNDEFINED_VARIABLE, "");
    return bad_argument(object, 0, WOOL_TYPE_P_NAME(type));
}

wool_user_end()
{
    wool_puts("Bye.\n");
    wool_end(0);
}

#ifdef DEBUG 			/* some routines convenient for debugging: */

stop_if_in_dbx(){} /* used in dbx */

int		dbxi = 0; 
WOOL_Atom 	dbxa;
WOOL_OBJECT 	dbxo;

PO(n)
int n;
{
    wool_print(n);
    wool_newline();
    fflush(stdout);
}

/*
 * break function for gwm for debugging purposes
 */

WOOL_OBJECT
wool_break(){return NIL;}	/* WOOL user break! */

WOOL_OBJECT
wp(obj)
WOOL_OBJECT     obj;
{
    wool_print_level = 0;
    WOOL_send(WOOL_print, obj, (obj));
    wool_newline();
    yyoutflush();
}

/*
 * type(obj) prints its type (under dbx!)
 */

wt(obj)
WOOL_OBJECT obj;
{
    printf("%s\n", ((WOOL_Atom) obj -> type[0]) -> p_name);
}

char *
type(obj)
WOOL_OBJECT obj;
{
    return ((WOOL_Atom) obj -> type[0]) -> p_name;
}


WOOL_OBJECT
wool_print_newline(obj)
WOOL_OBJECT     obj;
{
    WOOL_send(WOOL_print, obj, (obj));
    putchar('\n');
    return obj;
}

struct _UniqId {
    int size;
    int last;
    int *list;
};

int
UniqId(UI, n)
struct _UniqId *UI;
int n;
{
    int i;
    if (!UI->size) {
	UI->size = 4000;
	UI->list = (int *) Malloc(UI->size);
    }
    for (i=0; i< UI->last; i++) {
	if (UI->list[i] == n) {
	    return i;
	}
    }
    UI->list[i] = n;
    return (UI->last)++;
}

/* checksum on jump-buffers */

int
jmpbuf_checksum(jmpbuf)
int *jmpbuf;
{
    int i, result = 0;
    static struct _UniqId UI;
    for(i=0; i < sizeof(jmp_buf)/sizeof(int); i++)
	result = (result << 3) + (result >> 28) + jmpbuf[i];
    return UniqId(&UI, result);
}

#endif /* DEBUG */

WOOL_OBJECT
wool_type(obj)
WOOL_OBJECT obj;
{
    return (WOOL_OBJECT) obj->type[0];
}

wool_print_type(obj)
WOOL_OBJECT obj;
{
    WLAtom_print(obj -> type[0]);
}

#ifdef MONITOR
WOOL_OBJECT
wool_moncontrol(num)
WOOL_Number	num;
{
    moncontrol(num ->number);
}
#endif /* MONITOR */

/* tracing info
 */

WOOL_OBJECT
wool_get_trace()
{
    return (WOOL_OBJECT) WLNumber_make(wool_tracing_on);
}

/* (trace obj)
 * obj = expr, evals expr at each eval of list
 * obj = 0/1 turns tracing on/off (without resetting expr)
 * obj = t resets expr
 */

WOOL_OBJECT
wool_set_trace(obj)
WOOL_OBJECT obj;
{

    if(obj -> type == WLNumber) {
	wool_tracing_on = ((WOOL_Number) obj) -> number;
    } else if(obj == NIL) {
	wool_tracing_on = 0;
    } else {
	wool_tracing_on = 1;
	decrease_reference(wool_tracing_on_EXPR);
	if (obj == TRU)
	    wool_tracing_on_EXPR = 0;
	else
	    increase_reference(wool_tracing_on_EXPR = obj);
    }
    wool_still_tracing = wool_tracing_on;
    return obj;
}

/***************************************************************************\
* 									    *
* add .:$HOME:$HOME/gwm: before built-in-path (INSTALL_DIR) and returns it  *
* (malloced)								    *
* 									    *
\***************************************************************************/

char *
wool_fix_path(built_in_path)
char *built_in_path;
{
    char           *home = (char *) getenv("HOME");
    char           *path =
    Malloc(strlen(built_in_path) + 9 + 2*(home ? strlen(home) : 0));

    strcpy(path, ".:");
    if (home) {
	strcat(path, home);
	strcat(path, ":");
	strcat(path, home);
	strcat(path, "/");
	strcat(path, WOOL_APP_name);
	strcat(path, ":");
    }
    strcat(path, built_in_path);
    return path;
}

/****************************************\
* 					 *
*  INITIALISATION:			 *
*  to be called before everything else 	 *
* 					 *
\****************************************/

/*
 * wool_init returns 0 if all is ok
 * It calls its parameter function if not NULL, just before reading user
 * profile. Used by GWM for setting default keywords
 */

int
wool_init(client_initialisation)
int	(*client_initialisation)();
{
    /* initialize tables */
    zrt_init();
    dft_init();
    WLNumber_init();
    HashTable_init();

    /* initialize signals */
    SignalsInit();

    /* initialize wool's objects */

    wool_atom_make(WOOL_OBJECT, NIL, "()", NIL);	/* atoms */
    increase_reference(wool_atom("nil") -> c_val = NIL);
    wool_atom_make(WOOL_OBJECT, TRU, "t", TRU);
    NIL_STRING_make();
    WA_progn = (WOOL_OBJECT) wool_atom("progn");

    /* init stack */
    WLStackFrame_init();
    calling_function_init();

    /* intitialise predefined functions (Subrs) */
    QUOTE = wool_subr_make(WLFSubr, wool_quote, "quote", 1);
    wool_subr_make(WLFSubr, setq, "setq", 2);
    wool_subr_make(WLFSubr, setq, ":", 2);
    wool_subr_make(WLSubr, set, "set", 2);
    wool_subr_make(WLSubr, wool_multiply, "*", 2);
    wool_subr_make(WLSubr, wool_divide, "/", 2);
    wool_subr_make(WLSubr, wool_modulo, "%", 2);
    wool_subr_make(WLSubr, wool_add, "+", NARY);
    wool_subr_make(WLSubr, wool_minus, "-", NARY);
    increase_reference(wool_atom("defun") -> c_val =
		       wool_subr_make(WLFSubr, de, "de", NARY));
    increase_reference(wool_atom("defunq") -> c_val =
		       wool_subr_make(WLFSubr, df, "df", NARY));
    wool_subr_make(WLFSubr, wool_lambda_make, "lambda", NARY);
    wool_subr_make(WLFSubr, wool_lambdaq_make, "lambdaq", NARY);
    wool_subr_make(WLSubr, wool_atom_of_string, "atom", 1);
    wool_subr_make(WLFSubr, progn, "progn", NARY);
    wool_subr_make(WLFSubr, wool_if, "if", NARY);
    wool_subr_make(WLFSubr, wool_cond, "cond", NARY);
    increase_reference(wool_atom("equal") -> c_val =
		       wool_subr_make(WLSubr, wool_equal, "=", 2));
    wool_subr_make(WLSubr, wool_eq, "eq", 2);
    wool_subr_make(WLSubr, greater_than, ">", 2);
    wool_subr_make(WLSubr, lesser_than, "<", 2);
    wool_subr_make(WLSubr, wool_compare, "compare", 2);
    wool_subr_make(WLSubr, shell, "!", NARY);
    increase_reference(wool_atom("print") -> c_val =
		       wool_subr_make(WLSubr, wool_print_nary, "?", NARY));
    wool_subr_make(WLSubr, not, "not", 1);
    wool_subr_make(WLFSubr, and, "and", NARY);
    wool_subr_make(WLFSubr, or, "or", NARY);
    increase_reference(wool_atom("together") -> c_val =
	       wool_subr_make(WLSubr, wool_bitwise_or, "bitwise-or", NARY));
    wool_subr_make(WLSubr, wool_bitwise_and, "bitwise-and", NARY);
    wool_subr_make(WLSubr, wool_bitwise_xor, "bitwise-xor", NARY);
    wool_subr_make(WLFSubr, wool_while, "while", NARY);
    wool_subr_make(WLFSubr, wool_for, "for", NARY);
    wool_subr_make(WLFSubr, wool_mapfor, "mapfor", NARY);
    wool_subr_make(WLFSubr, wool_with, "with", NARY);
    wool_subr_make(WLFSubr, wool_with_eval, "with-eval", NARY);
    wool_subr_make(WLSubr, wool_context_save, "context-save", 1);
    wool_subr_make(WLSubr, wool_context_restore, "context-restore", 1);
    wool_subr_make(WLSubr, wool_loadfile_in_path, "load", 1);
    wool_subr_make(WLSubr, wool_execute_wool_string, "execute-string", 1);
    wool_subr_make(WLSubr, eval, "eval", 1);
    wool_subr_make(WLSubr, wool_getenv, "getenv", 1);
    wool_subr_make(WLSubr, wool_unbind, "unbind", 1);
    wool_subr_make(WLSubr, wool_boundp, "boundp", 1);
    wool_subr_make(WLSubr, WLString_match, "match", NARY);
    wool_subr_make(WLSubr, wool_length, "length", 1);
    wool_subr_make(WLSubr, WLList_sub, "sublist", NARY);
    increase_reference(wool_atom("nth") -> c_val =
		       wool_subr_make(WLSubr, WLList_nth, "#", NARY));
    increase_reference(wool_atom("replace-nth") -> c_val =
		    wool_subr_make(WLSubr, WLList_replace_nth, "##", NARY));
    wool_subr_make(WLSubr, WLList_delete_nth, "delete-nth", 2);
    wool_subr_make(WLFSubr, wool_user_end, "end", 0);
    wool_subr_make(WLSubr, wool_atoi, "atoi", 1);
    wool_subr_make(WLSubr, wool_itoa, "itoa", 1);
    wool_subr_make(WLSubr, wool_hack, "hack", 2);
    wool_subr_make(WLSubr, wool_list, "list", NARY);
    wool_subr_make(WLFSubr, wool_tag, "tag", NARY);
    wool_subr_make(WLFSubr, wool_exit, "exit", NARY);
    wool_subr_make(WLSubr, wool_type, "type", 1);
    wool_subr_make(WLFSubr, wool_error_has_occurred, "error-occurred", NARY);
    wool_subr_make(WLSubr, wool_trigger_error, "trigger-error", NARY);
    wool_subr_make(WLSubr, wool_copy, "copy", 1);
    wool_subr_make(WLSubr, wool_used_time, "elapsed-time", 0);
    wool_subr_make(WLSubr, wool_member, "member", 2);
    wool_subr_make(WLSubr, WLNamespace_make, "namespace-make", 0);
    wool_subr_make(WLSubr, WLNamespace_add, "namespace-add", 1);
    wool_subr_make(WLSubr, WLNamespace_remove, "namespace-remove", 2);
    wool_subr_make(WLSubr, WLName_add, "defname", NARY);
    wool_subr_make(WLSubr, WLNamespace_set_current, "namespace", 2);
    wool_subr_make(WLSubr, WLName_namespace, "namespace-of", 1);
    wool_subr_make(WLSubr, WLNamespace_size, "namespace-size", 1);
    wool_subr_make(WLSubr, WLList_qsort, "sort", 2);
    wool_active_make("hostname", wool_hostname_get, NULL);

    /* --- */
#ifdef STATS
    wool_subr_make(WLFSubr, zrtstats, "gcinfo", 0);
    wool_subr_make(WLFSubr, wlcfstats, "wlcfinfo", 0);
    wool_subr_make(WLFSubr, mstats, "meminfo", 0);
    wool_subr_make(WLFSubr, hashstats, "hashinfo", 0);
    wool_subr_make(WLFSubr, oblist, "oblist", 0);
#else /* STATS */
    wool_subr_make(WLFSubr, NIL_FUNC, "gcinfo", 0);
    wool_subr_make(WLFSubr, NIL_FUNC, "wlcfinfo", 0);
    wool_subr_make(WLFSubr, NIL_FUNC, "meminfo", 0);
    wool_subr_make(WLFSubr, NIL_FUNC, "hashinfo", 0);
    wool_subr_make(WLFSubr, NIL_FUNC, "oblist", 0);
#endif /* STATS */
#ifdef DEBUG
    wool_subr_make(WLFSubr, wool_break, "break", 0);
#ifdef GWM
    wool_subr_make(WLSubr, WLFsm_fp, "print-fsm", 1);
    wool_subr_make(WLSubr, WLState_fp, "print-state", 1);
    wool_subr_make(WLSubr, WLArc_fp, "print-arc", 1);
#endif
#else /* DEBUG */
    wool_subr_make(WLFSubr, NIL_FUNC, "break", 0);
#endif /* DEBUG */
#ifdef MONITOR
    wool_subr_make(WLSubr, wool_moncontrol, "moncontrol", 1);
#endif /* MONITOR */
#ifdef USER_DEBUG
    wool_active_make("trace", wool_get_trace, wool_set_trace);
    wool_pointer_make("trace-level", &wool_tracing_level);
#endif /* USER_DEBUG */

    wool_pointer_make("print-level", &wool_max_print_level);
    wool_pointer_make("stack-print-level", &wool_max_stack_print_level);

    /* here do client inits before the profile is read */
    if (client_initialisation)
	(*client_initialisation) ();

    /* first time, load the user file */
    wool_error_status = 0;
    if (!set_wool_error_resume_point()) {
	zrt_gc(0);
	if (wool_loadfile_in_path(wool_atom(wool_user_profile_name)) == NIL) {
	    return 1;
	}
    }
    wlcf_flush();
    wool_error_in_profile = wool_error_status;
    set_wool_error_resume_point();
    dft_gc();

    return 0;
}

