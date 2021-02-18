/* The `Object' abstraction
 */

#ifndef OBJECT_H
#define OBJECT_H

#define VALBITS         24
#define VALMASK         ((1 << VALBITS) - 1)
#define SIGNBIT         (1 << (VALBITS-1))
#define SIGNMASK        ~(SIGNBIT-1)
#define FIXNUM_FITS(x)  (((x) & SIGNMASK) == 0 || ((x) & SIGNMASK) == SIGNMASK)
#define FIXNUM_FITS_UNSIGNED(x)     (((x) & SIGNMASK) == 0)

#define CONSTBIT        (1 << VALBITS)

#define TYPESHIFT       (VALBITS+1)

typedef unsigned Object;

/* Extract/Set/Compare the type and val components of Objects */

#define TYPE(x) ((int)((x) >> TYPESHIFT))
#define SETTYPE(x,t) ((x) = ((x) & VALMASK) | ((x) & CONSTBIT) | \
			    ((int)(t) << TYPESHIFT))

#define FIXNUM(x) (((int)(x) << (32-VALBITS)) >> (32-VALBITS))

#define SETFIXNUM(x,i) ((x) = ((x) & ~VALMASK) | ((i) & VALMASK))

#define CHAR(x) ((x) & VALMASK)

#ifndef POINTER_CONSTANT_HIGH_BITS
#  define POINTER(x) ((x) & VALMASK)
#else
#  define POINTER(x) (((x) & VALMASK) | POINTER_CONSTANT_HIGH_BITS)
#endif
#define SETPOINTER(x,p) SETFIXNUM(x,(int)(p))

#define ISCONST(x) ((x) & CONSTBIT)
#define SETCONST(x) ((x) |= CONSTBIT)

#define SET(x,t,p) ((x) = ((int)(t) << TYPESHIFT) | ((int)(p) & VALMASK))

#define EQ(x,y) ((x) == (y))

#define SETFAST(x,y) ((x) = (y))

/* Fixed types.  Cannot use enum, because the set of types is extensible */

#define T_Fixnum          0      /* Must be 0 */
#define T_Bignum          1
#define T_Flonum          2
#define T_Null            3      /* empty list */
#define T_Boolean         4      /* #t (1) and #f (0) */
#define T_Void            5      /* doesn't print */
#define T_Unbound         6      /* only used internally */
#define T_Special         7      /* only used internally */
#define T_Character       8
#define T_Symbol          9
#define T_Pair           10
#define T_Environment    11      /* A pair */
#define T_String         12
#define T_Vector         13
#define T_Primitive      14      /* Primitive procedure */
#define T_Compound       15      /* Compound procedure */
#define T_Control_Point  16
#define T_Promise        17      /* Result of (delay expression) */
#define T_Port           18
#define T_End_Of_File    19
#define T_Autoload       20
#define T_Macro          21
#define T_Broken_Heart   22      /* only used internally */

#define T_Last T_Broken_Heart

#define BIGNUM(x)   ((struct S_Bignum *)POINTER(x))
#define FLONUM(x)   ((struct S_Flonum *)POINTER(x))
#define STRING(x)   ((struct S_String *)POINTER(x))
#define VECTOR(x)   ((struct S_Vector *)POINTER(x))
#define SYMBOL(x)   ((struct S_Symbol *)POINTER(x))
#define PAIR(x)     ((struct S_Pair *)POINTER(x))
#define PRIM(x)     ((struct S_Primitive *)POINTER(x))
#define COMPOUND(x) ((struct S_Compound *)POINTER(x))
#define CONTROL(x)  ((struct S_Control *)POINTER(x))
#define PROMISE(x)  ((struct S_Promise *)POINTER(x))
#define PORT(x)     ((struct S_Port *)POINTER(x))
#define AUTOLOAD(x) ((struct S_Autoload *)POINTER(x))
#define MACRO(x)    ((struct S_Macro *)POINTER(x))

typedef unsigned short gran_t;	/* Granularity of bignums */

struct S_Bignum {
    Object minusp;
    unsigned size;		/* Number of ushorts allocated */
    unsigned usize;		/* Number of ushorts actually used */
    gran_t data[1];		/* Data, lsw first */
};

struct S_Flonum {
    Object tag;               /* Each S_Foo must start with an Object */
    double val;
};

struct S_Symbol {
    Object next;
    Object name;               /* A string */
    Object value;
    Object plist;
};

struct S_Pair {
    Object car, cdr;
};

struct S_String {
    Object tag;
    int size;
    char data[1];
};

struct S_Vector {
    Object tag;
    int size;
    Object data[1];
};

enum discipline { EVAL, NOEVAL, VARARGS };
struct S_Primitive {
    Object tag;
    Object (*fun)();
    char *name;
    int minargs;
    int maxargs;    /* Or MANY */
    enum discipline disc;
};
#define MANY    100

struct S_Compound {
    Object closure;     /* (lambda (args) form ...) */
    Object env;         /* Procedure's environment */
    int min_args, max_args;
    Object name;
};

typedef struct wind {
    struct wind *next, *prev;
    Object inout;                  /* Pair of thunks */
} WIND;

typedef struct gcnode {
    struct gcnode *next;
    int gclen;
    Object *gcobj;
} GCNODE;

struct S_Control {
    Object env;
    GCNODE *gclist;
    WIND *firstwind, *lastwind;
    int tailcall;
#ifdef PORTABLE
    jmp_buf jmpbuf;
    int callable;
    long id;
#else
    int size;
    char stack[1];
#endif
};

struct S_Promise {
    Object env;
    Object thunk;
    int done;
};

struct S_Port {
    Object name;    /* string */
    short flags;
    char unread;
    int ptr;
    FILE *file;
};
#define P_OPEN    1 /* flags */
#define P_INPUT   2
#define P_STRING  4
#define P_UNREAD  8
#define P_TTY    16

struct S_Autoload {
    Object file;
    Object env;
};

struct S_Macro {
    Object body;
    int min_args, max_args;
    Object name;
};


/* "size" is called with one object and returns the size of the object.
 *    If "size" is NOFUNC, then "const_size" is taken instead.
 * "eqv" and "equal" are called with two objects and return 0 or 1.
 *    NOFUN may be passed instead (than eqv and equal always return #f).
 * "print" is called with an object, a port, a flag indicating whether
 *    the object is to be printed "raw" (a la display), the print-depth,
 *    and the print-length.
 * "visit" is called with a pointer to an object and a function.
 *    For each component of the object, the function must be called with
 *    a pointer to the component.  NOFUNC may be supplied.
 */
typedef struct {
    int haspointer;
    char *name;
    int (*size)();
    int const_size;
    int (*eqv)();
    int (*equal)();
    int (*print)();
    int (*visit)();
} TYPEDESCR;
#define NOFUNC ((int (*)())0)


typedef struct sym {
    struct sym *next;
    char *name;
    unsigned long value;
} SYM;

typedef struct {
    SYM *first;
    char *strings;
} SYMTAB;

#endif
