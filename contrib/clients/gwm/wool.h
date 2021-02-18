/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/***************************\
* 			    *
*  various defs for wool   *
* 			    *
\***************************/

#ifndef INCLUDE_WOOL_H
#define INCLUDE_WOOL_H
#include <setjmp.h>
#ifndef EXT
#include "EXTERN.h"
#endif /* EXT */
#include "machine.h"

/* various parameters */

/*
 *  Maximum size for:
 *      length of name of CACHED objets (fonts, etc...)
 * 	output lines of wool_printf (not wool_puts)
 * 	file names (path look-up)
 * 	X properties (only "machine name" for now)
 *
 *  Should alway be >= 256, 1024 is fine.
 *  Used only for speed concerns to allocate temp strings in the stack
 */

#define MAX_TEMP_STRING_SIZE 1024

/* default WLPATH for searching wool files */

#ifdef INSTALL_PATH
#define DEFAULT_WLPATH INSTALL_PATH
#else /* INSTALL_PATH */
# ifdef INSTALL_DIR
#define DEFAULT_WLPATH INSTALL_DIR
# else 
#define DEFAULT_WLPATH DEFAULT_DEFAULT_WLPATH
# endif INSTALL_DIR
#endif /* INSTALL_PATH */

/* path */

EXT	char *wool_path;
EXT     char *wool_user_profile_name, *wool_text_extension;

/* shell variables */

#define DEFAULT_FONT "fixed"		/* MUST exist! */

/* application_name */

EXT char *wool_application_NAME INIT(WOOL_APP_NAME);
EXT char *wool_application_name INIT(WOOL_APP_name);

/* END OF INSTALLATION PARAMETERS */

#ifndef Max
#define Max(x,y)		(((x)<(y))?(y):(x))
#define Min(x,y)		(((x)<(y))?(x):(y))
#ifndef Abs
#define Abs(x)			(((x)>0)?(x):-(x))
#endif
#define FlagOn(mask,flag) ((mask)&(flag))
#endif

/* Now, programmer's section... */

/* the WOOL_OBJECT type:
 * Each object handled by WOOL is a pointer to a structure including at least:
 *  - a pointer to a list of methods, common to all objects of this type.
 *  - a reference count, integer telling how many objects points to this
 *    one. The object should be freed as soon as this counter goes down to
 *    zero. (first bit is to see if it is not in the zrt, rest is for count)
 */

#ifndef DEBUG
#define WOOL_HEADER \
	struct _WOOL_OBJECT *(**type)(); \
	unsigned int reference_count
#else /* DEBUG */
#define WOOL_HEADER \
	struct _WOOL_OBJECT *(**type)(); \
	int reference_count
#endif /* DEBUG */

typedef struct _WOOL_OBJECT {
    WOOL_HEADER;
}               *WOOL_OBJECT;

typedef 
WOOL_OBJECT(*WOOL_METHOD) ();

typedef WOOL_METHOD *WOOL_TYPE;

/* wool_error prints the file and linenum of error... */

#ifdef DEBUG
#define wool_error(ERRstring,ERRmess) \
    (WOOL_OBJECT) (wool_printf("\nC source file \"%s\"", __FILE__), \
    wool_printf(" line %d\n", __LINE__), \
    _wool_error(ERRstring,ERRmess)) 
#if __STDC__
#define	ASSERT(p)   if (!(p)) wool_error("assertion failed: %s", #p)
#else /* __STDC__ */
#define	ASSERT(p)   if (!(p)) wool_error("assertion failed: %s", "p")
#endif /* __STDC__ */
#else
#define wool_error(s,m)	_wool_error(s,m)
#define	ASSERT(p)
#endif

/* global vars */

EXT int wool_error_status 		INIT(0);/* set to 1 by a wool_error */
EXT int wool_do_print_errors 		INIT(1);/* shall we print errors? */
EXT int wool_continue_reading_on_error 	INIT(0);/* in files */
EXT int wool_error_in_profile 		INIT(0);/* do we abort? */
EXT int wool_max_print_level 		INIT(9);/* max list imbrication */
EXT int wool_print_level 		INIT(0);/* current value */
EXT char *wool_shell_name		INIT(0);/* var name of forked shell */
EXT WOOL_OBJECT wool_host_name		INIT(0);/* host name running wool */

/* first include standards file if missing */

#ifndef NULL
#define NULL 0
#endif /* NULL */

/* to access easily any object's reference count */

#define REF(obj) (((WOOL_OBJECT) (obj))->reference_count)

/* Macros to ease initializations */

/* macro to define and initialize wool atoms in the code: 
 * 	type is WOOL_Atom or WOOL_OBJECT
 *	C_name is the name in the C code (i.e NIL)
 *	wool_value is the string which will represent the atom
 *	value is an object taken as the c_val
 */

#define wool_atom_make(type, C_name, wool_name, value) \
	C_name = (type) wool_atom(wool_name); \
	increase_reference(((WOOL_Atom) C_name)->c_val = (WOOL_OBJECT) value)

#define wool_atom_with_numeric_value_make(name, value) \
	WLAtom_set(wool_atom(name), WLNumber_make(value))

/* type-checking macros */

#define must_be_or_nil(typename, arg, n) \
    if (((WOOL_OBJECT) arg) != NIL && (arg) -> type != typename) 	\
    	bad_argument(arg, n, WOOL_TYPE_P_NAME(typename))

#define must_be(typename, object, n) \
    if (object -> type != typename) \
            bad_argument(object, n, WOOL_TYPE_P_NAME(typename))

/* get from context */

#define get_C_from_object(atom) \
	(long) WOOL_send(WOOL_get_C_value, (atom), ((atom)))

#define get_val_from_context(field, atom) \
	increase_reference((WOOL_OBJECT) (field = (void*) \
		WOOL_send(WOOL_eval, atom, (atom))))

/* generic numerical type */
#ifdef PTR_TYPE
typedef PTR_TYPE Num;
#else /* PTR_TYPE */
typedef long Num;
#endif

#ifdef CARD32
typedef CARD32 Card32;
#else /* CARD32 */
typedef unsigned long	Card32;
#endif /* CARD32 */

/* for alignement problems, gives next valid pointer */
#define ALIGN(ptr) ((((ptr) >> 2) + 1) << 2)

#ifndef DO_NOT_REDEFINE_MALLOC
#define Malloc(bytes) malloc(bytes)
#define Free(bytes) free(bytes)
#define Realloc(ptr, bytes) realloc(ptr, bytes)
#define Calloc(ptr, bytes) calloc(ptr, bytes)
extern char *malloc(), *realloc(), *calloc();
#else /* DO_NOT_REDEFINE_MALLOC */
#define Malloc(bytes) wool_malloc(bytes)
#define Free(bytes) wool_free(bytes)
#define Realloc(ptr, bytes) wool_realloc(ptr, bytes)
#define Calloc(ptr, bytes) wool_calloc(ptr, bytes)
extern char *wool_malloc(), *wool_realloc(), *wool_calloc();
#endif /* DO_NOT_REDEFINE_MALLOC */

#ifndef DEBUG
#define increase_reference(obj) REF(obj) += 2
#define decrease_reference(obj) {if(obj && (REF(obj)-=2)==1)zrt_put(obj);}
#define decrease_reference_non_null(obj) {if((REF(obj)-=2)==1)zrt_put(obj);}
#endif /* DEBUG */

#define dft_gc() while (dft_last > dft) Free(*(--dft_last))

/* exported functions */

EXT int		wool_init();
EXT WOOL_OBJECT _wool_error();
EXT WOOL_OBJECT wool_print();
EXT WOOL_OBJECT wool_subr_make();
EXT WOOL_OBJECT wool_eval();
EXT WOOL_OBJECT wool_read();
EXT int         wool_pool();
EXT WOOL_OBJECT NIL_FUNC();
EXT WOOL_OBJECT wool_type_or_evaluate();
EXT WOOL_OBJECT zrt_put();
EXT WOOL_OBJECT bad_argument();

/* exported objects */

EXT WOOL_OBJECT wool_read_expr INIT(NULL);
EXT WOOL_OBJECT temp_obj;

EXT WOOL_OBJECT  *zrt, *zrt_last;
EXT int     zrt_size, zrt_limit;

EXT char **dft, **dft_last, **dft_last_allocated;

EXT jmp_buf     wool_error_jmpbuf;
typedef int    *jmp_buf_ptr;	/* this works if jmp_buf is an int[] */
EXT jmp_buf_ptr wool_goes_here_on_error 
INIT(wool_error_jmpbuf);

EXT WOOL_OBJECT NIL;		/* NIL is just a predefined atom */
EXT WOOL_OBJECT TRU;		/* t is just a predefined atom */
EXT WOOL_OBJECT QUOTE;		/* Function needed for parsing */
EXT WOOL_OBJECT UNDEFINED_WOOL_VALUE; /* for undefined value */
EXT WOOL_OBJECT WA_progn;	/* the progn atom */

#ifndef NULL
#define NULL 0
#endif /* NULL */

#define UNDEFINED (WOOL_OBJECT) 0
#define NARY	-1
#define ANY	0xFFFFFFFF

/*
 *  this macro is used to say where to go after an wool_error
 *  NOTE: be sure the function this macros is in won't have returned
 *  before the call to wool_error!
 */

#define set_wool_error_resume_point() (\
	calling_function_on_error = calling_function_current \
		- calling_function_stack, \
	wool_stackframe_on_error = wool_current_stackframe, \
	setjmp(wool_goes_here_on_error))

#define save_wool_error_resume_point() \
    int old_calling_function_on_error = calling_function_on_error; \
    WOOL_StackFrame old_wool_stackframe_on_error = wool_stackframe_on_error;\
    jmp_buf_ptr old_wool_goes_here_on_error = wool_goes_here_on_error;\
    jmp_buf         current_jmpbuf;\
    Trace('l',("Saving at %s:%d: buf [%d]\n",__FILE__,__LINE__,jmpbuf_checksum(wool_goes_here_on_error)));\
    wool_goes_here_on_error = current_jmpbuf


#define restore_wool_error_resume_point() \
    calling_function_on_error = old_calling_function_on_error; \
    wool_stackframe_on_error = old_wool_stackframe_on_error; \
    wool_goes_here_on_error = old_wool_goes_here_on_error ;\
    Trace('l',("Restoring at %s:%d: buf [%d]\n",__FILE__,__LINE__,jmpbuf_checksum(wool_goes_here_on_error)))

/*
 * error messages
 */

#define UNDEFINED_VARIABLE   0
#define BAD_NUMBER_OF_ARGS   1
#define UNDEFINED_FUNCTION   2
#define BAD_DEFUN 	     3
#define BAD_ARG_TYPE	     4
#define RELEASING_ATOM       5
#define BAD_LOCAL_SYNTAX     6
#define NOT_AN_ATOM	     7
#define SYNTAX_ERROR	     8
#define INTERNAL_ERROR	     9
#define TIME_EXCEEDED	    10
#define CANNOT_SET	    11
#define CANNOT_GET_C_VALUE  12
#define TOO_MANY_PARAMETERS 13
#define NON_WOOL_OBJECT	    14
#define UNDEFINED_METHOD    15
#define NO_MEMORY	    16
#define NOT_REDEFINABLE     17
#define SILENT_ERROR	    18
#define NOT_MODIFIABLE	    19
#define USER_ERROR          20    

/* the send define:
 * called by WOOL_send(WOOL_MethodName,
 *		       object,
 *		       (object, parm1, parm2, ... )); NEED parenthesises !!!
 */

#ifndef DEBUG
#define WOOL_send(message,object,parms) (*(((object)->type)[message]))parms
#else /* DEBUG */
#define WOOL_send(message,object,parms) \
    (WOOL_current_method = message, \
     wool_send_is_valid(message,object), \
	(WOOL_OBJECT) (*(((object)->type)[message]))parms)
#endif

#ifdef SECURE
extern char *wool_methods_names[];
#endif /* SECURE */

#define WOOL_TYPE_P_NAME(type) ((WOOL_Atom) (type)[0]) -> p_name

/* the methods for all WOOL types */

/*	method name          N arity desc
#define WOOL_type_name	     0	/* 0 not a method but the atom of type name */
#define WOOL_eval 	     1	/* 1 evaluates object */
#define WOOL_print 	     2	/* 1 print object */
#define WOOL_free	     3	/* 1 release storage of object */
#define WOOL_execute 	     4	/* 2 execute (as CAR of list) */
#define WOOL_set	     5	/* 2 set value (called by setq) */
#define WOOL_get_C_value     6	/* 1 extract the raw pointed obj */
#define WOOL_open	     7	/* 1 create the X Window */
#define WOOL_close	     8	/* 1 destroy it */
#define WOOL_process_event   9	/* 2 trigger the event handler */
#define WOOL_copy	    10	/* 1 make a copy of itself */
#define WOOL_get_dimensions 11	/* 2 adjust and get dimensions */
#define WOOL_redraw	    12	/* 2 adjust and redraw an object */
#define WOOL_equal	    13	/* 2 a == b ? */
#define WOOL_reconfigure    14	/* 2 move after WOOL_redraw */
#define WOOL_setq	    15	/* 2 set but do not eval value */
#define WOOL_make	    16  /* 1 create a new object */
#define WOOL_silent_eval    17  /* 1 eval but do not break on undefined */

/* the undefined methods (aborts!), one per arity */

EXT WOOL_OBJECT wool_undefined_method_1();
EXT WOOL_OBJECT wool_undefined_method_2();

#ifdef DEBUG
EXT int WOOL_current_method;
#define FREED_MAGIC 0x31416	/* block has been freed */
#else
#define stop_if_in_dbx()
#endif /* DEBUG */

#define UndefinedPos	-1

/* the user-level debugging functions */

EXT int wool_tracing_on INIT(0);
EXT WOOL_OBJECT wool_tracing_on_EXPR INIT(0);
EXT int wool_tracing_level INIT(0);
EXT int wool_still_tracing INIT(0);

#if defined(__HIGHC__)
pragma on(POINTERS_COMPATIBLE);
#endif

/* debugging-only traces */

#ifdef DEBUG
EXT char * WlTraceFlags INIT("");
#define Trace(flag, args) if(strchr(WlTraceFlags, flag)) printf args
#define TraceDo(flag, inst) if(strchr(WlTraceFlags, flag)) inst
#else /* DEBUG */
#define Trace(flag, args)
#define TraceDo(flag, inst) 
#endif /* DEBUG */

#endif /* INCLUDE_WOOL_H */
