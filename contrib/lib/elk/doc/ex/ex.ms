.RP
.fp 5 H
.fp 7 C
.de S
.\".ps 12
.ft 7
.if \\n(.$=1 \&\\$1
.if \\n(.$>1 \&\\$1\c
.ft
.\".ps
.if \\n(.$>1 \&\\$2
..
.de PG
.DS
.ft 5
..
.de SC
.DS
.ft 7
.\".ps 12
..
.de PE
.ft
.DE
.LP
..
.de C
.br
.ne 3c
.SH
\\$1 \\$2 \\$3 \\$4 \\$5 \\$6 \\$7 \\$8 \\$9
.XS
\\$1 \\$2 \\$3 \\$4 \\$5 \\$6 \\$7 \\$8 \\$9
.XE
..
.TL
Interfacing Scheme to the ``Real World''
.br
\(em a non-trivial example \(em
.AU
Oliver Laumann
.AB
This memo serves as a guide for implementors who want to interface the
Scheme [1] interpreter described in [2,3,4] to external applications
or libraries.
This is especially important in cases where
Scheme is used as the \f2extension language\fP of a larger
system.
The steps involved in such an integration are illustrated
in detail by a hypothetical but non-trivial example.
.AE
.C Introduction
.PP
This paper considers the situation where Scheme is not used in a
``stand-alone'' way, but as an integral part of a larger environment,
for instance, as the extension language of another application.
In such cases, the objects and functions provided by the environment
must be accessible from within Scheme.
.PP
Providing interfaces to the ``non-Scheme world'' usually involves
the creation of new abstract data types which hide irrelevant details
of the data types maintained by the environment and ``protect'' access to
them.
In addition, Scheme functions must finally call low-level functions
provided by the environment.
Since Scheme does not directly support
the definition of new data types and interfaces to foreign-language
functions, the implementor must provide the necessary ``glue''
between the Scheme interpreter and the environment; this ``glue''
must be written in a (comparatively) low-level language like ``C''
or ``C++'' [5].
.PP
Fortunately this is not as difficult as it may sound, since such
``interface code'' usually has a high degree of reusability
(which is exactly the reason why this memo was actually written).
However, an important decision facing the interface implementor is
where to draw the line between the low-level part of an interface,
that is, the part which is written in (say) ``C'' and therefore
``cast in stone'' from the Scheme programmer's point of view,
and the part that is written in Scheme.
Taking this decision partly involves a tradeoff between performance
on the one hand and flexibility and customizability on the other hand,
and partly it depends on whether something can be done easier (or less
costly) in a language like ``C'' or in Scheme.
.PP
The rest of this paper presents an example for such an interface
in a detailed way.
After starting with a few basic definitions,
the code is refined and extended in a number of steps, and possible
problems and improvements as well as the conventions used
(both coding and naming conventions) are pointed out.
The example is non-trivial in the sense that
problems that typically arise when such an interface is developed
are taken into account.
The example can be viewed as a prototype or a model for similar
integrations.
In fact, many Scheme data types have been defined by taking existing
code and only modifying the relevant parts.
.PP
On the other hand, the example is hypothetical; the ``graphics library''
or ``window system'' being discussed does not exist in reality.
The advantage of choosing a hypothetical environment is that many
details that would distract or are irrelevant for the example
can be omitted.
.QP
\s-1\f3Typographic Conventions:\fP Examples written in ``C'',
``C'' identifiers, names of ``C'' types etc. are typeset
in a sans-serif typeface; thus, \f5Object\fP is the name of a type,
while ``object'' is the generic term.
Scheme code and Scheme symbols are typeset using a fixed-width
typeface; thus,
.S equal?
is the name of a Scheme predicate, not a question.\s0
.sp .5
.C The Hypothetical Window System
.PP
Suppose that you are writing an application on a graphics workstation
under the control of a window system (or graphics system).
The window system provides a ``C''-library of functions that can
be used to place individual windows on the graphics display, to
query and modify the attributes (such as position and size)
of a window, to destroy a window, and others.
To begin with, let us assume that the window system library exports
the functions
.PG
Window Window_Create (int x, int y, int width, int height)
.PE
and
.PG
void Window_Destroy (Window w)
.PE
.PP
\f5Window_Create()\fP places a window with the given dimensions on the screen;
it returns a \f2handle\fP or \f2ID\fP for the newly-created
window that is used to reference the window in later calls to the library.
The type \f5Window\fP might be represented as a \f5long int\fP.
\f5Window_Destroy()\fP is the inverse operation; it instructs the window
system to free the resources associated with the window and remove it
from the screen.
.PP
Our goal is to be able to create and maintain windows from within
Scheme, this apparently involves the definition of a data type
.S window
and a number of \f2primitive procedures\fP that operate on
objects of type
.S window .
Ideally, we want to be able to write code like
.SC
(define width 400)
(define w
  (make-window 0 0 width (* 2 width)))
.sp .5
(window? w)  ; returns #t
.sp .5
;; [do something with the window]
.sp .5
(destroy-window w)
.PE
.PP
The new type
.S window
should be indistinguishable from other
``first-class'' types such as
.S integer
or
.S vector ;
for instance, the usual predicates should also work on objects
of this type.
.C The Anatomy of a Scheme Type
.PP
The Scheme interpreter maintains a \f2generic\fP type called \f5Object\fP.
Thus, all variables that can hold a Scheme object must be declared
as \f5Object\fP.
An \f5Object\fP is basically a 32-bit value (the actual size may
vary between different target machines) which is composed of
a \f2tag\fP part and a \f2pointer\fP part. \**
.FS
Some special types of objects, like small numbers, characters, or
booleans are directly stored in the \f2pointer\fP component of an
\f5Object\fP.
The definition of this kind of objects is not considered in this memo.
.FE
The \f2tag\fP part indicates the type of the object, and the remaining
bits hold the actual memory address of the object (that is, they
point into the heap).
The macros \f5TYPE()\fP and \f5POINTER()\fP are provided to extract
the fields of an \f5Object\fP.
Since objects are usually represented as a ``C''-structure, and since
the raw memory address of an object is seldom of any use, the result
of \f5POINTER()\fP is usually casted like this:
.PG
#define VECTOR(obj) ((struct S_Vector *)POINTER(obj))
.PE
where \f5S_Vector\fP is the actual representation of a
.S vector .
Applying \f5VECTOR\fP to an \f5Object\fP obviously only makes sense
when it is known or has been verified that the object is really
a vector:
.PG
 ...
if (TYPE(x) == T_Vector)
    num_items = VECTOR(x)\->size;
 ...
.PE
.PP
This enables us to begin with the definition of the new type
.S window ;
the representation of an object of this type and the macro to access
the individual components can be defined as
.PG
struct S_Window {
    Window handle;
};
#define WINDOW(obj) ((struct S_Window *)POINTER(obj))
.PE
where \f5handle\fP holds the value returned by a call to \f5Window_Create()\fP.
It is not really necessary to use a \f2struct\fP here, but it seems likely that
the type is going to be extended by additional components in the future.
.QP
\s-1\f3Naming Convention:\fP Type structures are usually given names
starting with \f5S_\fP followed by the capitalized type name.
The name of the access macro is obtained by capitalizing all letters of
the type name.
The variable holding the number identifying the
type itself is written as \f5T_\fP followed
by the type name capitalized.
.sp .5
Although these conventions are not enforced, they are recommended to
ease understanding of the code by other people.\s0
.C More About Types
.PP
The Scheme interpreter is instructed to register a new type by means
of the function \f5Define_Type()\fP.
It returns the ID of the new type, that is, the value that is
going to be returned by \f5TYPE(obj)\fP for all objects of that type.
If the first argument is zero, a new type ID is allocated by the
Scheme interpreter.
If, for some reason, the type ID must be a constant chosen by
the implementor, that constant is passed to \f5Define_Type()\fP
instead of the zero argument.
However, this is discouraged since a programmer-chosen type ID
is likely to conflict with types defined by other programmers
(\f5Define_Type\fP fails in this case).
That is, the typical usage is
.PG
T_Window = Define_Type (0, ...);
.PE
.PP
The second argument is the name of the newly defined type as a
null-terminated string (such as \f5"window"\fP).
The third argument is a pointer to a function that is called by
the interpreter whenever it must query the size (in bytes) of
an object of the type being defined.
It is called with an \f5Object\fP as its argument.
For vectors, the size function would be defined like this:
.PG
int Vector_Size (Object v) {
    return sizeof (struct S_Vector) +
        (VECTOR(v)\->size-1) * sizeof Object;
}
.PE
.PP
However, this does not seem reasonable for types where all objects
have the same size.
In this case, one would invoke \f5Define_Type()\fP with a size function
of \f5NOFUNC\fP and a fourth argument indicating the (constant) size.
Thus, in the case of the
.S window
type, one could write
.PG
T_Window = Define_Type (0, "window", NOFUNC, sizeof (struct S_Window), ...
.PE
.PP
The fifth and sixth argument are pointers to functions that are
called by the interpreter to query whether two objects of the
newly defined type are \f2operationally equivalent\fP or \f2equal\fP,
respectively, that is, whenever the Scheme predicate
.S eqv?
or
.S equal?
is invoked on objects of this type.
In cases where it is not useful to distinguish between equivalence
and equality, only an equality function is defined,
and this function is then used for both arguments.
In the
.S window
example, the equality function can be defined like
.PG
int Window_Equal (Object a, Object b) {
    return WINDOW(a)\->handle == WINDOW(b)\->handle;
}
.PE
.PP
When \f5NOFUNC\fP is passed to \f5Define_Type()\fP as the equivalence
or equality function, two objects are never regarded as being
equivalent or equal, respectively (that is,
.S eqv?
and
.S equal?
always return
.S #f ).
.PP
The seventh argument to \f5Define_Type()\fP is the \f2print function\fP;
this function is invoked by the interpreter when an object of the type
being defined is to be printed (usually when
.S display ,
.S write ,
or a similar function has been called).
The print function receives as arguments the object to be printed,
an output port, a flag indicating whether the object should be printed
in an unquoted representation (in the sense of
.S display ),
and the
currently remaining \f2print depth\fP and \f2print length\fP.
The simplest way to define a \f5Window_Print\fP function is like this:
.PG
/*ARGSUSED*/
void Window_Print (Object w, Object port, Bool raw, int depth, int len) {
    Printf (port, "#[window %lu]", WINDOW(w)\->handle);
}
.PE
where \f5Printf\fP (note the capital P)
is the Scheme version of the ``C''-library \f5printf\fP.
Since the window handle and the memory address of a window on the heap
are equally useless sources of information, one could also print
\f5POINTER(x)\fP (using the same format string).
.PP
The final argument of \f5Define_Type()\fP is the so-called \f2visit
function\fP.
This function is invoked by the interpreter during garbage collection
when it traverses objects in order to determine the set of currently
reachable (or accessible) objects.
This function is only required for types where the objects have
components of type \f5Object\fP, since in this case the reachability search of
the interpreter must be ``directed'' to the objects that can be
reached from the one being examined.
For other types, \f5NOFUNC\fP is used instead.
The visit function is called with the address of an \f5Object\fP
and a pointer to a function; this function must be called for each
component of the object that is of type \f5Object\fP itself.
Thus, for pairs (\f2cons cells\fP) one would define the visit function
like this:
.PG
void Pair_Visit (Object *x, void (*f)(Object *)) {
    (*f)(&PAIR(*x)\->car);
    (*f)(&PAIR(*x)\->cdr);
}
.PE
At this point we are able to provide the first workable version of
the
.S window
type:
.PG
\f2% cat window.c\fP
.sp .5
#include <scheme.h>
#include <window.h>

int T_Window;

struct S_Window {
    Window handle;
};
#define WINDOW(obj) ((struct S_Window *)POINTER(obj))

int Window_Equal (Object a, Object b) {
    return WINDOW(a)\->handle == WINDOW(b)\->handle;
}

/*ARGSUSED*/
void Window_Print (Object w, Object port, Bool raw, int depth, int len) {
    Printf (port, "#[window %lu]", WINDOW(w)\->handle);
}

void init_window () {
    T_Window = Define_Type (0, "window", NOFUNC, sizeof (struct S_Window),
        Window_Equal, Window_Equal, Window_Print, NOFUNC);
}
\f2%\fP
.PE
.C Loading a Type Definition
.PP
To be able to load a module containing a type definition into the
Scheme interpreter, it must be compiled and (if applicable) linked
together with the libraries used by the module:
.SC
cc -c \f2other-options\fP window.c
ld -r -x window.o \f2other_objects\fP -l\f2window_library\fP
mv a.out window.o
.PE
.PP
Of course, the exact sequence of commands may vary between different
machines and also depends on the programming language actually used
(this example assumes ``C'').
The resulting object file can then directly be loaded from within
Scheme using
.S "(load 'window.o)" .\**
.FS
Note that certain unusual machines and operating systems do not
support loading of object modules.
In these cases, other steps must be taken to register a new
type definition with the interpreter.
.FE
In addition, after having loaded one or more modules, one might
want to save an image of the interpreter to a file by means of
the
.S dump
primitive to avoid the (possibly time-consuming)
loading of the object modules in the future.
.PP
When an object file is loaded, the interpreter searches the file
for functions whose names begin with ``init_''; these
functions are called (without arguments) in the order in which
they appear in the object file.
Thus, by placing the call to \f5Define_Type()\fP into
the \f5init_window()\fP function in the example above, we
ensure that this call is executed at the time the file is
loaded by the interpreter.
Usual practice is to put one \f5init_\fP\f2module\fP function at
the end of each module.
.LP
In addition, it may be useful to place a line like
.PG
P_Provide (Intern ("window.o"));
.PE
into the \f5init_window()\fP function; this is the
``C'' counterpart of the Scheme expression
.SC
(provide 'window.o)
.PE
.PP
A Scheme program which makes use of the window type can
then conditionally load the corresponding object file by
evaluating
.SC
(require 'window.o)
.PE
.C Creating Scheme Objects
.PP
So far we have seen how a new data type
.S window
can be defined and made
known to the Scheme interpreter, but we are not yet able to create 
any objects of this type.
Creation of a Scheme object involves allocation and initialization
of a suitable number of bytes on the heap and allocation of an
\f5Object\fP ``handle''; the ID of the object's type must be written into
the tag field of the \f5Object\fP, and the pointer field must
be initialized to point to the starting address of the newly
allocated object on the heap.
.PP
Heap space is allocated with the function \f5Get_Bytes()\fP
which is similar to the ``C''-library \f5malloc()\fP except
that memory is allocated on the Scheme interpreter's heap instead
of the ``C'' heap.
In addition, it is not necessary to check the return value of
\f5Get_Bytes()\fP since (in contrast to \f5malloc()\fP) it either
succeeds or does not return at all.
Due to its nature, \f5Get_Bytes()\fP can trigger a garbage
collection; the implications following from this fact will be
discussed later.
.PP
The components of an \f5Object\fP handle can be initialized by
means of the macro \f5SET()\fP.
The first argument of \f5SET()\fP is a variable of type \f5Object\fP,
the second and third argument are the contents of the tag field
(that is, a type) and the contents of the pointer field (usually
a heap address).
Thus, the function to create a Scheme
.S window
can be
written like this:
.PG
Object Make_Window (Window id) {
    char *p;
    Object w;
.sp .5
    p = Get_Bytes (sizeof (struct S_Window));
    SET(w, T_Window, (struct S_Window *)p);
    WINDOW(w)\->handle = id;
    return w;
}
.PE
.QP
\s-1\f3Rule:\fP All components of a Scheme object that are of
the type \f5Object\fP themselves must be initialized with
a valid \f5Object\fP immediately after the object has been
allocated (typically, \f5Null\fP is used as a place-holder when the
``real'' values are not yet available).\s0
.sp .5
.PP
The Scheme interpreter already provides a number of similar \f5Make_\fP
functions the most interesting of which are
.PG
Object Make_Integer (int i)
Object Make_Fixnum (int i)  \f2[i must fit into a ``fixnum'']\fP
Object Make_Reduced_Flonum (double f)
Object Make_Char (unsigned char c)
Object Make_Vector (int length, Object init)
Object Make_String (char *string, int length)
.PE
.PP
The second argument of \f5Make_Vector()\fP is used to initialize
all elements of the vector.
The \f5length\fP argument of \f5Make_String()\fP may be larger than
the actual length of the (null-terminated) \f5string\fP argument.
Since the string passed to \f5Make_String()\fP must be null-terminated,
strings containing null bytes have to be created like this:
.PG
Object s = Make_String ("", length);
bcopy (\f2string_with_nulls\fP, STRING(s)\->data, length);
.PE
.C Defining Primitive Procedures
.PP
The \f5Make_Window()\fP function alone is not of much use; after all,
it is only a ``C''-function, not a primitive procedure that can be
called from within Scheme.
A primitive procedure that creates and returns an object of type
.S window
could be defined like this:
.PG
Object P_Make_Window (Object x, Object y, Object width, Object height) {
    Window id;
.sp .5
    id = Window_Create (Get_Integer (x), Get_Integer (y),
        Get_Integer (width), Get_Integer (height));
    return Make_Window (id);
}
.PE
.QP
\s-1\f3Naming Convention:\fP The names of primitive procedures start
with \f5P_\fP followed by the name of the procedure in Scheme with
all hyphens converted to underscores and all words capitalized.
For instance, the ``C''-function implementing the Scheme procedure
.S delete-all-buffers
would be called \f5P_Delete_All_Buffers()\fP.\s0
.LP
.QP
\s-1\f3Rule:\fP All primitive procedures must return an \f5Object\fP.
The non-printing object \f5Void\fP can be returned in absence of a
meaningful return value.
All arguments of a primitive procedure either are of type \f5Object\fP,
or the procedure has exactly two arguments, an integer and an array
of \f5Object\fPs.\s0
.sp .5
.PP
The function \f5Get_Integer()\fP converts a Scheme integer to a ``C''
\f5int\fP.
An error is signalled when \f5Get_Integer()\fP is called with an
\f5Object\fP other than an integer (thus, the arguments of
\f5P_Make_Window()\fP need not explicitly be type-checked)
or when the integer does not fit into an \f5int\fP.
.LP
Finally, we must arrange that when a Scheme expression like
.SC
(make-window 0 0 400 600)
.PE
is evaluated, the corresponding ``C''-function \f5P_Make_Window()\fP
is invoked by the interpreter.
More precisely, the Scheme symbol with the name \f5make-window\fP
must be bound to an object of type ``primitive procedure'' which
points to the function \f5P_Make_Window()\fP (just like the way
.S car
is bound to the primitive procedure ``car'' in the
interpreter's global environment).
In addition, we must define whether the arguments in a call to
.S make-window
must be evaluated (they must) and whether it
is acceptable to call this procedure with a variable or arbitrary
number of arguments (it is not acceptable), that is, the
\f2calling discipline\fP must be defined.
This is accomplished by the function \f5Define_Primitive()\fP:
.PG
Define_Primitive (P_Make_Window, "make-window", 4, 4, EVAL);
.PE
.PP
The first argument is a pointer to the ``C''-function that implements
the primitive procedure;
the seconds argument is the name of the primitive procedure (that is, 
the name of the symbol to which the procedure is to be bound);
the third and fourth argument are the minimum and maximum number
of arguments of the primitive procedure, and
the last argument is the calling discipline.
The natural place to go for the call to \f5Define_Primitive()\fP is
the \f5init_window()\fP function; this ensures that the call is
executed when the module is loaded.
.PP
Three calling disciplines are supported by \f5Define_Primitive()\fP:
\f5EVAL\fP, \f5VARARGS\fP, and \f5NOEVAL\fP.
Procedures of type \f5EVAL\fP are the most common ones; they are
declared with individual arguments like \f5P_Make_Window()\fP above.
\f5VARARGS\fP procedures receive two arguments, an \f5int\fP holding
the number of actual arguments, and an array of evaluated \f5Object\fPs.
For instance, the function implementing the primitive
.S list
could be declared as
.PG
Object P_List (int argc, Object *argv) {
    ...
}
.PE
and the corresponding call to \f5Define_Primitive()\fP would be
.PG
Define_Primitive (P_List, "list", 0, MANY, VARARGS);
.PE
The special constant \f5MANY\fP indicates that there is no upper bound
on the number of arguments.
.PP
\f5NOEVAL\fP is used for \f2special forms\fP; the function
implementing the special form receives one argument of type
\f5Object\fP, a list holding the unevaluated arguments
(thus, the argument of such a function has either the type
\f5T_Pair\fP, or the argument is \f5Null\fP when it has been called
with no arguments).
.QP
\s-1\f3Bug:\fP In the current version of the interpreter, primitive
procedures of type \f5EVAL\fP must have a fixed number of
arguments (that is, the minimum and maximum number of arguments
must be identical).
The type \f5VARARGS\fP must be used when a variable number of
arguments is desired.\s0
.sp .5
.PP
Scheme-conventions require a type predicate to be defined for
each new Scheme type.
This can be done quite easily like this:
.PG
Object P_Windowp (Object x) {
    return TYPE(x) == T_Window ? True : False;
}
\f2[``p''-suffix denotes a predicate; ``?'' in Scheme]\fP
.PE
The corresponding call to \f5Define_Primitive()\fP is
.PG
Define_Primitive (P_Windowp, "window?", 1, 1, EVAL);
.PE
\f5True\fP and \f5False\fP are pre-defined variables of type \f5Object\fP
representing the Scheme constants
.S #t
and
.S #f .
.LP
The primitive procedure to destroy a window could be written like this:
.PG
Object P_Destroy_Window (Object w) {
    Check_Type (w, T_Window);
    Window_Destroy (WINDOW(w)\->handle);
    return Void;
}
 ...
Define_Primitive (P_Destroy_Window, "destroy-window", 1, 1, EVAL);
.PE
.C Improving the Example
.PP
There are couple of problems with the first version of the
.S window
implementation.
One of them is obvious \(em it is possible that \f5Window_Destroy()\fP is
called with the handle of a window that has already been destroyed
by a previous call to \f5Window_Destroy()\fP.
Although it seems that this can be easily fixed, it turns out to be
the instance of a more fundamental problem.
Consider the following code fragment:
.SC
(define w (make-window 0 0 400 600))
(destroy-window w)
 ...
(define w2 (make-window 100 100 300 300))
.PE
.PP
Since the handle of the first window has been deallocated by the window
system due to the call to
.S destroy-window ,
it is perfectly
legal for the next call to \f5Window_Create()\fP to return the same
handle again.
Now we have the fatal situation that the object
.S w
points to
a window (since its handle is the same than that of
.S w2 ),
although it has actually been destroyed by a call to
.S destroy-window .
.S w
and
.S w2
are even
.S equal? ,
and a second call to
.S "(destroy-window w)"
causes
.S w2
(a completely unrelated window) to disappear.
.PP
The source of this problem is the fact that each Scheme object has a
potentially unlimited lifetime; one cannot ``force'' an object
to die (e.\|g. in the function
.S destroy-window )
as long as it is still accessible.
Thus, the
.S window
object
.S w
in the scenario above is in a special ``undead'' state \(em the ``Scheme-side''
of the object is still alive, while the resource in the window system
that the object is pointing to has already been deallocated.
.PP
One way to fix the
.S window
module (at least for the time being)
is to ``mark'' undead windows by overwriting their \f5handle\fP
component with a special value for which we can guarantee that
it is never returned by a call to \f5Window_Create()\fP.
Assuming that there is such a constant and that it is called \f5None\fP
(it might be defined as zero), we can rewrite \f5P_Destroy_Window()\fP
and \f5Window_Equal()\fP as follows:
.PG
Object P_Destroy_Window (Object w) {
    Check_Type (w, T_Window);
    if (WINDOW(w)\->handle != None) {
	Window_Destroy (WINDOW(w)\->handle);
	WINDOW(w)\->handle = None;
    }
    return Void;
}
.sp
int Window_Equal (Object a, Object b) {
    if (WINDOW(a)\->handle == None |\ | WINDOW(b)\->handle == None)
	return 0;
    return WINDOW(a)\->handle == WINDOW(b)\->handle;
}
.PE
.PP
According to the new version of the equality function, an undead
window is different from any other window (even from other undead
windows).
In addition to these changes, one might want to modify the
\f5Window_Print()\fP function to reflect the fact that a window being
printed has been destroyed.
.C More Improvements (and More Problems)
.PP
Assume that the window library provides a function
.PG
int List_Windows (Window **ret)
.PE
which returns (through the \f5ret\fP argument) a list of all windows
that are currently active on the display; the integer return value is the
number of windows stored into the \f5ret\fP array.
This allows us to write a primitive procedure
.S list-windows
that returns a vector of
.S window
objects like this:
.PG
Object P_List_Windows () {
    int n, i;
    Window *ret;
    Object v;
    GC_Node;
.sp .5
    n = List_Windows (&ret);
    v = Make_Vector (n, Null);
    GC_Link (v);
    for (i = 0; i < n; i++)
	VECTOR(v)\->data[i] = Make_Window (ret[i]);
    GC_Unlink;
    return v;
}
 ...
Define_Primitive (P_List_Windows, "list-windows", 0, 0, EVAL);
.PE
.PP
The function allocates a vector of the length returned by the
call to \f5List_Windows()\fP; the elements of the vector are
initialized with \f5Null\fP (any valid \f5Object\fP could be used
here, since the elements are overwritten by the immediately
following code anyway).
Then, each \f5Window\fP handle returned by \f5List_Windows()\fP is
converted into a Scheme object and stored into the corresponding
vector slot.
.PP
As mentioned above, the call to \f5Make_Window()\fP can cause
a garbage collection to be performed, since it involves a call to
\f5Get_Bytes()\fP.
The garbage collector basically traverses all Scheme objects that
are currently accessible and marks them as non-garbage.
Everything else is declared as garbage and overwritten by the
``good'' objects during the heap compaction.
Now imagine that one of the invocations of \f5Make_Window()\fP
in \f5P_List_Windows()\fP triggers the garbage collector, since
there is not enough space left on the heap to store one of the
.S window
objects.
The newly allocated vector is only accessible through
the local variable \f5v\fP (no other object is pointing to it),
thus the garbage collector is unable to find and mark the vector
as non-garbage, as it does not traverse the ``C'' runtime stack
during the accessibility search.
As the result, the vector as well as all windows stored in
the vector so far would be overwritten.
.PP
This can be avoided by temporarily storing a reference to the 
vector into a global list which is guaranteed to be visited
by the garbage collector.
When \f5P_List_Windows()\fP returns, it is no longer necessary
to ``protect'' the vector from being garbage-collected, since
it either becomes accessible, e.\|g. when
.S list-windows
is called like this:
.SC
(define v (list-windows))   ; the vector is stored into a variable
.PE
or
.SC
; the vector becomes accessible through another function's argument:
.sp .5
(frobnicate-windows (list-windows))
.PE
or the vector and its contents really become garbage, namely
when the primitive procedure is called like this:
.SC
(list-windows)   ; the return value is thrown away
.PE
.PP
An object can be ``linked'' into the global accessibility list by means
of the macro \f5GC_Link()\fP.
Since it is frequently necessary to protect several objects from
garbage collection (typically several local variables of a function) at
the same time, there are also macros called \f5GC_Link2()\fP,
\f5GC_Link3()\fP, etc. to facilitate protection of two or more
objects.
A call to the macro \f5GC_Link\fP\f2n\fP\f5()\fP must be preceded by the
declaration \f5GC_Node\fP\f2n\fP (as can be seen in the example
above).
The macro \f5GC_Unlink\fP removes all objects from the accessibility
list that have been added to the list by a preceding call to
\f5GC_Link()\fP.
Calls to \f5GC_Link()\fP and \f5GC_Unlink\fP cannot be nested.
.QP
\s-1\f3Rule:\fP It is imperative that an object has been fully
initialized before it is protected from garbage collection.\s0
.sp .5
.PP
When implementing \f5P_List_Windows()\fP, one may be tempted to
write the for-loop something like this:
.PG
Object *p;
 ...
for (p = VECTOR(v)\->data; n > 0; n\(mi\(mi)
    *p++ = Make_Window (*ret++);
.PE
.PP
That is, one might want to use a pointer to step through the
vector slots rather than use an index.
However, this would not work.
Since garbage collection involves moving the ``good'' objects
to a different location on the heap,
the contents of \f5v\fP changes during garbage collection
(the contents of the pointer field of \f5v\fP, to be precise).
Thus, the pointer \f5p\fP would suddenly point to an invalid
location (probably into a different Scheme object), although it
has been properly initialized.
Therefore it is essential that \f5VECTOR(v)\->data\fP is evaluated
again at each iteration.
.QP
\s-1\f3Rule:\fP Never keep ``C'' pointers to Scheme objects over
a function call possibly invoking a garbage collection.\s0
.sp .5
.PP
The fact that
.S list-windows
returns a vector of windows may have disadvantages.
It could be rewritten to return a list instead like this:
.PG
Object P_List_Windows () {
    int n;
    Window *ret;
    Object list, tail;
    GC_Node2;
.sp .5
    n = List_Windows (&ret);
    list = tail = P_Make_List (Make_Fixnum (n), Null);
    GC_Link2 (list, tail);
    for ( ; n > 0; n\(mi\(mi) {
	Car (tail) = Make_Window (*ret++);
	tail = Cdr (tail);
    }
    GC_Unlink;
    return list;
}
.PE
.PP
Note that there is no \f5Make_List()\fP function.
But there is a
.S make-list
primitive procedure which does what we want, so we can use that instead.
However, since it is a primitive procedure, all arguments must be
of type \f5Object\fP (which is the reason for the call to
\f5Make_Fixnum()\fP).
\f5Car()\fP and \f5Cdr()\fP are macros; they are defined as follows:
.PG
#define Car(x) PAIR(x)\->car
#define Cdr(x) PAIR(x)\->cdr
.PE
.PP
Although the version of
.S list-windows
using a list is slightly
more complex than the one using a vector, it has the advantage that
lists are ``favored'' by Scheme, that is, there are more primitive
procedures that work on lists than on vectors.
For instance, one can use
.S for-each
to apply a procedure to all elements of a list:
.SC
(for-each destroy-window (list-windows))
.PE
.PP
On the other hand, vectors consume less memory than lists and can
be manipulated more efficiently, since the elements of a vector
are stored contiguously and the length of a vector can be queried
in constant time.
.PP
Unfortunately, no matter whether vectors or lists are used,
there is a serious problem with
.S list-windows
(or rather with the entire
.S window
module).
Consider the following piece of code:
.SC
(define w (make-window 0 0 400 600))  ; the only call to make-window
(define v (list-windows))
(define w2 (vector-ref v 0))
(destroy-window w)
.PE
.PP
Before
.S destroy-window
is called,
.S w
and
.S w2
point to the same window (since only one window has been created).
The call to
.S destroy-window
instructs the window system to deallocate the window and marks
.S w
accordingly.
However,
.S w2
still contains the handle of the window that has
just been destroyed, so that \f5Window_Destroy()\fP can be called
again with the same window handle.
Even worse, another call to
.S make-window
can return a window with the same handle, so that the situation
gets completely inconsistent.
This looks like the mess which we thought we had fixed in the
previous chapter.
.PP
The problem seems to lie in the fact that it is possible at all
that two distinct Scheme objects can point to the same ``physical''
window.
The problem would go away if it could be ensured that there is
exactly one object of type
.S window
for each window handle.
In the scenario above, this would imply that
.S w
and the window returned by
.S list-windows
are not only
.S equal? ,
but also equal in the sense of
.S eq? .
If this could be ensured, it would, of course, no longer be necessary
to define a \f5Window_Equal()\fP function at all, since
.S "(equal? w1 w2)"
would imply
.S "(eq? w1 w2)"
for all windows
.S w1
and
.S w2
(and
.S "(eq? w1 w2)"
implies
.S "(equal? w1 w2)"
anyway).
.PP
The problem can be solved by modifying \f5Make_Window()\fP such that
it checks whether there is already a Scheme window with the given
window handle and, if this is true, returns this object rather than
allocating a new one.
To achieve this, we first define a function that stores a
.S window
object in a global list (a \f2pool\fP) of windows:
.PG
Object pool[MAX_WINDOWS];  \f2[MAX_WINDOWS is defined by the window library]\fP

void Register_Window (Object w) {
    Object *p;
.sp .5
    for (p = pool; p < pool+MAX_WINDOWS && !Nullp (*p); p++)
	;
    if (Nullp (*p))
	*p = w;
    else
	error ...
}
.PE
\f5pool\fP is initialized in \f5init_window\fP like this:
.PG
Object *p;
.sp .5
for (p = pool; p < pool+MAX_WINDOWS; p++) *p = Null;
.PE
.PP
\f5Nullp()\fP is a macro that simply tests whether its argument
is equal to \f5Null\fP.
In practice, it might be useful to employ a hash function or some
other algorithm that is faster than linear search (especially
if the number of different objects can get large).
.PP
In addition, we need a function that looks up an object with a
given window handle in the pool and, if one is found, returns it
(otherwise \f5Null\fP is returned):
.PG
Object Find_Window (Window handle) {
    Object *p;
.sp .5
    for (p = pool; p < pool+MAX_WINDOWS; p++)
	if (!Nullp (*p) && WINDOW(*p)\->handle == handle)
	    return *p;
    return Null;
}
.PE
Now \f5Make_Window()\fP can be modified like this:
.PG
Object Make_Window (Window id) {
    char *p;
    Object w;
.sp .5
    w = Find_Window (id);
    if (Nullp (w)) {
	p = Get_Bytes (sizeof (struct S_Window));
	SET(w, T_Window, (struct S_Window *)p);
	WINDOW(w)\->handle = id;
	Register_Window (w);
    }
    return w;
}
.PE
The last step is to remove an object from the pool when
.S destroy-window
is called:
.PG
void Deregister_Window (Object w) {
    Object *p;
.sp .5
    for (p = pool; p < pool+MAX_WINDOWS; p++)
	if (EQ(*p, w)) {
	    *p = Null;
	    break;
	}
}
.PE
.PP
\f5EQ\fP is a macro that checks whether two objects are equal
in the sense of the
.S eq?
predicate (that is, whether their pointer fields point to the same location).
The line
.PG
Deregister_Window (w);
.PE
must then be added to \f5P_Destroy_Window()\fP.
.C Garbage Collection Revisited
.LP
Consider the following code fragments:
.SC
(define w
  (make-window 0 0 400 600))
(set! w 'foo)
.PE
or
.SC
(make-window 0 0 400 600)
.PE
.PP
In the first scenario, a window is created and assigned to a variable;
immediately after that, the variable is overwritten by something else.
In the second example, the result of
.S make-window
is not even saved into a variable.
In both cases, the newly created window has become inaccessible to
the Scheme program; either by destroying the only reference to it
or by discarding the return value of
.S make-window .
Thus, it has become impossible to destroy the window from within
Scheme \(em it remains on the screen until it is removed by other means
(e.\|g., when the window system is terminated).
Of course, one could invoke
.S list-windows
to obtain a new 
reference to the window, but it seems desirable to have a general
solution to this problem that is independent of the existence of
.S list-windows .
.PP
For instance, it would be useful if the interpreter could
terminate a window (by a call to \f5Window_Destroy()\fP) automatically
when it can prove that all references to a window have been lost.
This can obviously be done with the help of the garbage collector,
since the garbage collector is able to determine the set of
all accessible objects anyway.
Thus, at the point the garbage collector is finished, we just need
to go through the set of active windows and destroy those windows
that have not been moved by the garbage collector (that is, the
windows that have not been found during the accessibility search).
Fortunately, the
.S window
code already maintains the set of
active windows (the \f5pool\fP array), so the function that
terminates all inaccessible windows can be written like this:
.PG
void Terminate_Windows () {
    Object *p, *tag;
.sp .5
    for (p = pool; p < pool+MAX_WINDOWS; p++) {
	if (Nullp (*p))
	    continue;
	tag = (Object *)POINTER(*p);
	if (TYPE(*tag) == T_Broken_Heart)
	    SETPOINTER(*p, POINTER(*tag));
	else
	    (void)P_Destroy_Window (*p);
    }
}
.PE
.PP
When the garbage collector ``visits'' an object, it first determines
the location of the object on the heap by looking at the pointer
field, then it copies the object to its new location.
The pointer field of the \f5Object\fP handle is updated so that
it points to the object's new location.
The memory that has been occupied by the object up to now is
overwritten by a special object called \f2broken heart\fP;
the pointer field of the broken heart is initialized to point
to the object's new location.
Thus, when the garbage collector encounters another \f5Object\fP
handle that points to the object just moved, it only needs to
update the pointer field with the new address stored in
the broken heart (the broken heart indicates that the object
has already been moved).
.PP
The \f5Terminate_Windows()\fP function simply looks
at the memory pointed to by each window's pointer field.
When a broken heart is stored in the window (that is, in the
\f5S_Window\fP structure), the window has obviously been moved
by the garbage collector.
This indicates that there must exist another reference to that window,
otherwise it had not been moved.
Thus, all we have to do is update the pointer field so that
it points to the window's new location
(the macro \f5SETPOINTER()\fP is used to store a value into
the pointer field of a variable of type \f5Object\fP).
Otherwise, when the window has not been overwritten by a broken heart,
it can be destroyed, since no more \f5Object\fP handles pointing to it
exist (except than the one in the \f5pool\fP).
Note that \f5P_Destroy_Window()\fP also removes the window from
the \f5pool\fP.
.PP
The first element of the underlying structure of a type must
obviously be of type \f5Object\fP, since the garbage collector
stores a broken heart at the beginning of an object (otherwise
it could be possible that the bit pattern at the beginning of
an object is accidentally interpreted as a broken heart).
Thus, \f5S_Window\fP must be redefined like this:
.PG
struct S_Window {
    Object tag;
    Window handle;
};
.PE
.PP
Since we currently do not have any use for a component of type
\f5Object\fP, we simply set it to \f5Null\fP in \f5Make_Window()\fP:
.PG
WINDOW(w)\->tag = Null;
.PE
.QP
\s-1\f3Rule:\fP All types must start with a component of
type \f5Object\fP, even if it is unused.
This component must be properly initialized immediately after creation of
an object (e.\|g. with \f5Null\fP).\s0
.sp .5
.PP
Finally we have to ensure that \f5Terminate_Windows()\fP is called
by the interpreter immediately after each garbage collection,
but before normal execution is resumed.
This can be done by registering a so-called \f2after-GC function\fP:
.PG
Register_After_GC (Terminate_Windows);
.PE
.PP
\f5Register_After_GC()\fP can be called from the window module's
initialization function.
An ``after-GC function'' must not cause a garbage collection and
it must return normally.
.PP
Note that the \f5pool\fP array is not ``GC-protected'' like the
local variables in the \f5P_List_Windows()\fP function above.
This is not necessary, because the pointer fields of the
\f5Object\fPs in the pool are updated ``by hand'' immediately
after each garbage collection.
If we had not written the \f5Terminate_Windows()\fP function,
we certainly had to make sure that the \f5pool\fP is known
to the garbage collector so that the pointer fields of the
.S window
objects are properly updated when a window has
been moved to a new location on the heap.
However, this cannot be done with \f5GC_Link()\fP and \f5GC_Unlink\fP,
since the scope of a \f5GC_Link()\fP is bounded by the body of the
function from which it is called.
While \f5GC_Link()\fP establishes a ``dynamic'' GC-protection and
is used to GC-protect local variables and function arguments,
we need a way to ``statically'' GC-protect an object (a global
variable).
This can be done with the macro
.PG
Global_GC_Link (obj)
.PE
where \f5obj\fP is a variable of type \f5Object\fP.
Unfortunately, the current version of the interpreter does not
support GC-protection of arrays of \f5Object\fPs (like the \f5pool\fP
array).
However, one could create a vector of \f5MAX_WINDOWS\fP elements
and use this vector to store the active windows rather than a
plain array of \f5Object\fPs.
The vector could then be GC-protected statically by a call to
\f5Global_GC_Link()\fP.
.QP
\s-1\f3Exercise:\fP Consider the following program fragment:
.SC
(list-windows)
(collect)   ; invoke the garbage collector
.PE
.QP
\s-1What happens?
How can this be fixed (which functions of the
.S window
module must be modified)?\s0
.sp .5
.C Defining Symbols
.PP
Suppose that the window system function \f5Window_Create()\fP has a
fifth argument that is used to determine the
color of the background of the newly created window.
The argument can be either \f5BgWhite\fP, \f5BgBlack\fP, or
\f5BgTransparent\fP; these symbols are defined as integer constants
in the window library's include file.
The primitive procedure
.S make-window
should be extended accordingly;
the additional argument can be one of the symbols
.S black ,
.S white ,
and
.S transparent .
This allows us to write expressions like
.SC
(define w (make-window 0 0 500 700 'white))
.PE
The new \f5P_Make_Window()\fP could be defined like this:
.PG
Object P_Make_Window (Object x, Object y, Object width, Object height,
	Object color) {
    Window id;
    int c;
.sp .5
    Check_Type (color, T_Symbol);
    if (EQ(color, Sym_Black))
	c = BgBlack;
    else if (EQ(color, Sym_White))
	c = BgWhite;
    else if (EQ(color, Sym_Transparent))
	c = BgTransparent;
    else
	Primitive_Error ("invalid background: ~s", color);
    id = Window_Create (Get_Integer (x), Get_Integer (y),
        Get_Integer (width), Get_Integer (height), c);
    return Make_Window (id);
}
.PE
.PP
\f5Primitive_Error()\fP invokes the Scheme error handler; it's arguments
are a format string (that can be passed to the primitive procedure
.S format )
and a variable number of \f5Object\fPs.
\f5Sym_Black\fP, \f5Sym_White\fP, and \f5Sym_Transparent\fP are of
type \f5Object\fP; they are initialized in \f5init_window()\fP like
this:
.PG
 ...
Define_Symbol (&Sym_Black, "black");
Define_Symbol (&Sym_White, "white");
Define_Symbol (&Transparent, "transparent");
.PE
.PP
The first argument to \f5Define_Symbol()\fP is a pointer to a
global variable of type \f5Object\fP, the second argument is
the \f2print name\fP of the newly created symbol.
The symbols created by the calls to \f5Define_Symbol()\fP are
properly GC-protected.
The above initializations could also be written like this:
.PG
 ...
Sym_Black = Intern ("black");
Global_GC_Link (Sym_Black);
\f2[etc.]\fP
.PE
.PP
\f5Intern()\fP is the ``Make'' function for symbols; it returns
an \f5Object\fP of type \f5T_Symbol\fP.
The reason why it does not follow the naming scheme of the other
``Make'' functions is that it works in a slightly different way.
Symbols are maintained by the interpreter similarly to windows
in our example; a call to \f5Intern()\fP does not necessarily create
a new object on the heap.
Scheme guarantees that two symbols with the same name are equal
in the sense of the
.S eq?
predicate (the analogous rule in the
.S window
example is that two windows with the
same handle are equal
.S eq? -wise
\(em which is the
reason why we introduced \f5Register_Window()\fP and \f5Find_Window()\fP).
The interpreter internally maintains a hashed symbol table (often called
\f2the obarray\fP or \f2the oblist\fP) similar to the window \f5pool\fP
in the example above.
\f5Intern()\fP essentially searches the oblist for a symbol with
the same name and, if it is found, simply returns a new \f5Object\fP
handle pointing to that symbol.
If no symbol with the given name is found, it creates one (this
involves a call to the \f5Get_Bytes()\fP function) and enters it into the
symbol table.
Certainly \f5Make_Window()\fP could also be written like this:
.PG
 ...
if (EQ(color, Intern ("black"))
    c = BgBlack;
else if (EQ(color, Intern ("white"))
    c = BgWhite;
\f2[etc.]\fP
.PE
.PP
The advantage over the first version is that there is no need for
the \f5Sym_\fP variables and the calls to \f5Define_Symbol()\fP.
On the other hand, it is less efficient, since \f5Intern\fP
must search the symbol each time it is called.
In addition, \f5Intern()\fP can trigger a garbage collection
(when a new symbol must be created on the heap), so that the arguments of
\f5P_Make_Window()\fP have to be GC-protected in the version using
\f5Intern()\fP.
.PP
When a primitive procedure maintains a large number of symbols
or when Scheme symbols must frequently be converted to ``C-symbols''
(integer constants), it might be useful to define a general conversion
like this:
.PG
struct symbol { char *name; int val; };

int Scheme_To_C_Symbol (Object sym, struct symbol *p) {
    Object name;
.sp .5
    Check_Type (sym, T_Symbol);
    name = SYMBOL(sym)\->name;
    while (p\->name != 0
	   && strncmp (p\->name, STRING(name)\->data, STRING(name)\->size) != 0)
	p++;
    if (p\->name == 0)
	Primitive_Error ("invalid symbol: ~s", sym);
    return p\->val;
}
.PE
The function could then be used like this:
.PG
struct symbol background\|[\|] = {
    "black",		BgBlack,
    "white",		BgWhite,
    "transparent",	BgTransparent,
    0, 0
};

Object P_Make_Window (..., Object color) {
    int c;
     ...
    c = Scheme_To_C_Symbol (color, background);
     ...
.PE
.C Defining Variables
.PP
Sometimes it is desirable to interact with the user of a module
not only through primitive procedures, but also through Scheme
variables defined by the module.
For instance, suppose that the hypothetical window system handles
errors by calling a user-supplied error function whenever an
error situation is encountered (such as ``out of window resources''
or ``out-of-bounds argument passed to a library function'').
The window system library could export a variable
.PG
void (*Error_Function)(int error);
.PE
that is initially zero and can be set by the user of the library.
In case of an error, when the variable is non-zero, the window system
invokes the \f5Error_Function\fP with an error number identifying the
error.
.PP
An obvious improvement would be to allow the user of the window
module to bind a Scheme function to the window system's error
handler.
This function could then invoke the standard Scheme error handler
(enabling the user to analyze the situation using the debugger)
or handle the error in an ``intelligent'' way.
This can be implemented by letting the window module declare
a variable
.S window-error-handler
and then define an
``interface'' function that acts as the ``glue'' between
the ``C'' error handler and the Scheme error handler.
The address of the interface function is assigned to
\f5Error_Function\fP at the point the window module is initialized.
When the function is called by the window system to report an
error, it checks whether the Scheme variable
.S window-error-handler
is currently bound to a function (an object of type \f5T_Compound\fP)
and, if this is the case, calls
the function with the error code argument.
Of course, the (numeric) argument provided by the window system
must be converted into a representation that is more useful to 
a Scheme error handler (e.\|g. a symbol or a string).
.LP
A variable can be defined by means of the function
.PG
void Define_Variable (Object *handle, char *name, Object init);
.PE
.PP
\f5Define_Variable\fP creates a symbol with the given name
(using \f5Intern()\fP) and binds the symbol to the object
specified by the third argument.
The binding is established in the current lexical environment,
that is, in the same environment into which the bindings for
the module's primitive procedures are entered.
The first argument is a pointer to an \f5Object\fP that can later
be used to retrieve the current value of the variable (the \f5Object\fP
does \f2not\fP hold the variable's value itself).
The value of a variable can be read and modified by applying
the macro \f5Val()\fP to the object that serves as the variable's
handle.
Thus, we can add declaration like
.PG
static Object V_Window_Error_Handler;
.PE
to
.S window.c
and insert the line
.PG
Define_Variable (&V_Window_Error_Handler, "window-error-handler", Null);
.PE
into the \f5init_window()\fP function.
The interface function could then be defined like this:
.PG
struct symbol errcodes\|[\|] = {
    "out-of-windows",	EWindow,
    "bad-argument",	EBadArg,
    \f2[etc.]\fP
    0, 0
};
 ...
void Window_Error_Handler (int err) {
    Object args, func;
    GC_Node;
.sp .5
    args = C_To_Scheme_Symbol (err, errcodes);
    GC_Link (args);
    args = Cons (args, Null);
    GC_Unlink;
    func = Val (V_Window_Error_Handler);
    if (TYPE(func) == T_Compound)
	(void)Funcall (func, args, 0);
    Primitive_Error ("bad window error handler");
    /*NOTREACHED*/
}
.PE
The last step is to call
.PG
Error_Function = Window_Error_Handler;
.PE
from \f5init_window()\fP.
.QP
\s-1\f3Naming Convention:\fP Names of variables holding symbols
start with \f5Sym_\fP; variables holding Scheme variables
have a \f5V_\fP prefix.\s0
.sp .5
.PP
The function \f5Window_Error_Handler()\fP essentially calls a
Scheme function; this is done by means of the function \f5Funcall()\fP.
The first argument to \f5Funcall()\fP must be either a primitive
procedure, a compound procedure, or a control-point (that is, an
\f5Object\fP of type \f5T_Primitive\fP, \f5T_Compound\fP, or
\f5T_Control_Point\fP, respectively).
The second argument is a list holding the arguments to the function
being called.
The third argument is a flag indicating whether the arguments must
be evaluated prior to calling the function.
\f5Funcall()\fP returns the value (an \f5Object\fP) returned by
the function it has called (or, in case it called a control-point,
it does not return at all).
Note that the return value of the user-supplied error handler in the
example above is ignored.
In fact, the error handler is expected to not return at all;
if it \f2does\fP return, the Scheme error function \f5Primitive_Error()\fP
is called which definitely does not return.
\f5Primitive_Error()\fP is also invoked when
.S window-error-handler
does not hold a compound procedure.
.PP
The function \f5Cons()\fP creates an \f5Object\fP of type \f5T_Pair\fP;
its arguments are the \f5Car\fP and the \f5Cdr\fP of the pair, respectively
(\f5Cons()\fP actually should have been called \f5Make_Pair()\fP).
Note that \f5args\fP must be GC-protected, since \f5Cons()\fP can
trigger a garbage collection.
\f5Funcall()\fP can cause a garbage collection, too, but this is
irrelevant in the example above, since no variables of type \f5Object\fP
are referenced after \f5Funcall()\fP has returned.
\f5C_To_Scheme_Symbol()\fP could be defined as the opposite
of \f5Scheme_To_C_Symbol()\fP; it receives as arguments an \f5int\fP
and a \f5struct symbol\fP array and returns the Scheme symbol
corresponding to the integer symbol (it must call \f5Intern()\fP
to create the symbol to be returned).
.LP
The error handling mechanism can now be used like this:
.SC
(set! window-error-handler          ; define a dumb error handler
  (lambda code
    (error 'window-error "~s" code)))
.PE
or
.SC
;; dynamically redefine error handler
.sp .5
(call-with-current-continuation
  (lambda (return)
    (fluid-let
      ((window-error-handler
        (lambda (code)
	  (display code)
	  (return #f))))
      (make-window 0 0 400 600))))   ; expression returns #f or a window

.PE
.C Primitive Procedures With ``Stringable'' Arguments
.PP
Consider the window system function
.PG
void Set_Window_Title (Window w, char *title);
.PE
which allows the user of the window library to define a title for
a window that has been created by a call to \f5Window_Create()\fP.
The title could be displayed by the window system in a title bar
on top of the window frame.
The corresponding primitive procedure could then be used like
.SC
(set-window-title! w "Debugger window #1")
.PE
.\"or
.\".SC
.\"(set-window-title! w "\f6doghijkao\fP")
.\".PE
or
.SC
(set-window-title! w '/etc/passwd)
.PE
.PP
Note that
.S set-window-title!
can be called with a string as well as with a symbol.
The primitive procedure could be defined as
.PG
Object P_Set_Window_Title (Object w, Object title) {
    int n;
    char *s;
.sp .5
    Check_Type (w, T_Window);
    if (TYPE(title) == T_Symbol)
	title = SYMBOL(title)\->name;
    else if (TYPE(title) != T_String)
	Wrong_Type_Combination (title, "string or symbol");
    n = STRING(title)\->size;
    s = alloca (n+1);
    bcopy (STRING(title)\->data, s, n);
    s[n] = '\\0';
    Set_Window_Title (WINDOW(w)\->handle, s);
    return Void;
}
.PE
.PP
The procedure first checks whether the \f5title\fP argument is
a symbol and, if this is true, sets the argument to the name of
the symbol.
The \f5name\fP component of a symbol is an \f5Object\fP of type
\f5T_String\fP.
If \f5title\fP is neither a symbol nor a string, the error
function \f5Wrong_Type_Combination()\fP is invoked with the offending
\f5Object\fP and a string indicating the expected argument types.
\f5Wrong_Type_Combination()\fP invokes the Scheme error handler
with a message like \f5``set-window-title!: wrong argument type
integer (expected string or symbol)''\fP.
This function is typically used in cases where an argument can be
of one of several types or when one wants to use a ``C'' string
to describe the expected type.
Otherwise either \f5Check_Type()\fP is used, or arguments are
type-checked explicitly, and, if the type is wrong,
the function \f5Wrong_Type()\fP is called with the wrongly-typed
object and the expected type (one of the ``\f5T_\fP'' constants)
as arguments.
Note that \f5Check_Type()\fP is a macro, therefore a function call
is involved only in the error case.
.PP
The main work done by \f5P_Set_Window_Title()\fP is to convert
the Scheme string into a (null-terminated) ``C'' string.
Unfortunately, the contents of a Scheme string (the \f5data\fP
component) cannot directly be used as an argument to the
``C'' function \f5Set_Window_Title()\fP, since the string is
not terminated by a null-byte.
Thus, the entire string has to be copied to a buffer that is one
byte larger than the length of the string, and a null-byte must
explicitly be appended.
Note that the buffer holding the ``C'' string is allocated on the
stack by a call to \f5alloca()\fP.
This has the advantage that the memory occupied by the buffer
need not be freed explicitly.
This is important, since primitive procedures do not necessarily
terminate normally.
For instance, an ``interrupt'' could occur
between the point the memory is allocated and the end of the
function, e.\|g. when the user presses the interrupt key;
the interrupt handler (as well as error handlers) never
returns to the point where the interrupt occurred.
The disadvantage is that typical implementations of \f5alloca()\fP
can cause the stack to overflow when called with a sufficiently
large size; thus, something like
.SC
(set-window-title! w (make-string 100000 #\ea))
.PE
would probably crash the interpreter or otherwise wreak havoc.
Note that \f5GC_Link()\fP and \f5GC_Unlink\fP are implemented
in a way so that an abnormal return from within a
\f5GC_Link\fP/\f5GC_Unlink\fP bracket does not cause any
inconsistencies.
.C Procedures With Keyword Arguments
.PP
In a ``real'' window system, the \f5Window_Create()\fP function
would probably have more arguments than just the location and
dimensions of the window to be created.
Suppose that it expects as additional arguments
the background color of the window, a flag indicating
whether a title-bar should be attached to the window, the width
of the window's border frame, the color of the border frame,
the handle of the ``parent window'' (assuming that there exists
a hierarchy among the active windows) or the constant \f5None\fP
if it has no parent, and a flag indicating whether the window
should be automatically raised to the top of all other windows
when the cursor enters a visible region of the window's surface.
Thus, the new \f5Window_Create()\fP is defined as
.PG
Window Window_Create (int x, int y, int width, int height, int background,
    Bool has_title, int border, int border_color, Window parent, Bool raise);
.PE
.PP
The corresponding primitive procedure can be modified accordingly
so that it accepts the new arguments.
Then one can write expressions like
.SC
(make-window 100 100 400 600 'white #f 2 'black 'none #t)
.PE
.PP
This is a loss.
How can one tell what an expression like this actually does without
either having memorized the calling interface or looking at the
function definition?
It would be much better if we were able to write something like
.SC
(make-window (x 100) (y 100) (width 400) (height 600) (background 'white)
  (border-color 'black) (border 2) (title #f) (raise #t) (parent 'none))
.PE
or
.SC
(make-window (width 400) (height 900) (x 0) (y 0))
.PE
.PP
Default values could be provided automatically for missing arguments.
Since keywords are used to identify the arguments, it would no
longer be necessary to specify the arguments in a pre-defined order.
Although it is feasible to modify \f5P_Make_Window()\fP so that it
accepts keyword arguments, one would not want to do this.
Note that the function must not evaluate its arguments; it does
not make sense to evaluate an expression like
.S "(x 100)" .
Thus, one would either have to declare the function as \f5NOEVAL\fP or
create a macro (which cannot be easily done from within ``C'').
.PP
On the other hand, in Scheme one could easily write a macro
that ``parses'' a list of keyword arguments and then calls the
simple
.S make-window
with the right number of arguments in the right order.
The macro could be defined like this:
.SC
(define-macro (better-make-window . args)
  (let ((x #f) (y #f) (width #f) (height #f) (background 'white)
        (border-color 'black) (border 2) (title #f) (raise #t)
        (parent 'none))
    (do ((a args (cdr a))) ((null? a))
      (cond
       ((not (and (pair? (car a)) (= (length (car a)) 2)))
        (error 'better-make-window "bad argument ~s" (car a)))
       ((memq (caar a) '(x y width height background border-color
                           border title raise parent))
        (eval `(set! ,(caar a) (cadar a))))
       (else
        (error 'better-make-window "unknown keyword: ~s" (car a)))))
    (if (not (and x y width height))
        (error 'better-make-window
               "you must specify location and size"))
    `(make-window ,x ,y ,width ,height ,background ,border-color
                  ,border ,title ,raise ,parent)))
.PE
.PP
Note that
.S x ,
.S y ,
.S width ,
and
.S height
are mandatory; default values are provided for the other arguments.
It should be clear that
.S better-make-window
could be improved, for instance, by mentioning the list of
permissible keywords only in one place (rather than in three
different places like in the definition above).
This could be done by placing all keywords into a list and use the
list in the call to
.S memq .
The actual arguments could also be stored in a list rather than
in individual variables, and this list could be ``spliced'' into
the call to
.S make-window
like so:
.SC
`(make-window ,@list-of-arguments)
.PE
.PP
Note that there is a naming conflict; if we called the macro
.S make-window ,
it would shadow the primitive procedure
.S make-window .
There are several ways to resolve the conflict;
for instance, the macro could be assigned a different name
(as has been done in the example above), or the primitive
procedures could be loaded into a separate environment
like this:
.SC
(define window-package
  (let ()
    (load 'window.o)
    (the-environment)))
.PE
and the call to
.S make-window
could then be changed to
.SC
`((access make-window window-package) ,x ,y ...)
.PE
where
.S access
is defined as
.SC
(define-macro (access sym env)
  `(eval ',sym ,env))
.PE
.SH
Other Argument Passing Techniques
.LP
[To be provided]
.bp
.C References
.LP
.nr PD .5v
.sp
.IP "1."
Jonathan Rees and William Clinger (editors):
.I
Revised\v'-.3m'\s-13\s0\v'.3m' Report on the Algorithmic
Language Scheme
.R
SIGPLAN Notices Vol. 21 Number 12 (December 1986).
.IP "2."
Bjarne Stroustrup:
.I
The C++ Programming Language
.R
Addison-Wesley, Reading, Massachusetts.  1986.
.IP "3."
Oliver Laumann:
.I
Reference Manual for the Elk Extension Language Interpreter
.R
.IP
.de PT
..
.bp
.PX
