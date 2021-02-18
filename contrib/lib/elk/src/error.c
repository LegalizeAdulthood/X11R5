#include <varargs.h>

#include "scheme.h"

Object Arg_True;

static Object V_Error_Handler,
              V_Interrupt_Handler,
              V_Top_Level_Control_Point;

Init_Error () {
    Arg_True = Cons (True, Null);
    Global_GC_Link (Arg_True);
    Define_Variable (&V_Error_Handler, "error-handler", Null);
    Define_Variable (&V_Interrupt_Handler, "interrupt-handler", Null);
    Define_Variable (&V_Top_Level_Control_Point, "top-level-control-point",
	Null);
}

#ifdef lint
/*VARARGS1*/
Fatal_Error (foo) char *foo; { foo = foo; }
#else
Fatal_Error (va_alist) va_dcl {
    va_list args;
    char *fmt;

    va_start (args);
    fmt = va_arg (args, char *);
    (void)fflush (stdout);
    fprintf (stderr, "\nFatal error: ");
    vfprintf (stderr, fmt, args);
    fprintf (stderr, ".\n");
    va_end (args);
    exit (1);
}
#endif

Panic (msg) char *msg; {
    (void)fflush (stdout);
    fprintf (stderr, "\nPanic: %s (dumping core).\n", msg);
    abort ();
}

Uncatchable_Error (errmsg) char *errmsg; {
    Reset_IO (0);
    Format (Curr_Output_Port, errmsg, strlen (errmsg), 0, (Object *)0);
    P_Newline (0);
    Reset ();
}

#ifdef lint
/*VARARGS1*/
Primitive_Error (foo) char *foo; { foo = foo; }
#else
Primitive_Error (va_alist) va_dcl {
    va_list args;
    register char *p, *fmt;
    register i, n;
    Object msg, sym, argv[10];
    GC_Node; GCNODE gcv;

    /* In case the error occurred after a Disable_Interrupts:
     */
    Enable_Interrupts;

    va_start (args);
    fmt = va_arg (args, char *);
    for (n = 0, p = fmt; *p; p++)
	if (*p == '~' && p[1] != '~' && p[1] != '%') n++;
    if (n > 10)
	Panic ("Primitive_Error args");
    for (i = 0; i < n; i++)
	argv[i] = va_arg (args, Object);
    sym = Null;
    GC_Link (sym);
    gcv.gclen = 1 + i; gcv.gcobj = argv; gcv.next = &gc1; GC_List = &gcv;
    sym = Intern (Error_Tag);
    msg = Make_String (fmt, p - fmt);
    Err_Handler (sym, msg, i, argv);
    /*NOTREACHED*/
}
#endif

Object P_Error (argc, argv) Object *argv; {
    Check_Type (argv[1], T_String);
    Err_Handler (argv[0], argv[1], argc-2, argv+2);
    /*NOTREACHED*/
}

Err_Handler (sym, fmt, argc, argv) Object sym, fmt, *argv; {
    Object fun, args, a[1];
    GC_Node3;

    Reset_IO (0);
    args = Null;
    GC_Link3 (args, sym, fmt);
    args = P_List (argc, argv);
    args = Cons (fmt, args);
    args = Cons (sym, args);
    fun = Val (V_Error_Handler);
    if (TYPE(fun) == T_Compound)
	(void)Funcall (fun, args, 0);
    a[0] = sym;
    Format (Curr_Output_Port, "~s: ", 4, 1, a);
    Format (Curr_Output_Port, STRING(fmt)->data, STRING(fmt)->size,
	argc, argv);
    P_Newline (0);
    GC_Unlink;
    Reset ();
    /*NOTREACHED*/
}

Intr_Handler () {
    Object fun;

#ifdef FORGETS_SIGNAL_HANDLER
    (void)signal (SIGINT, Intr_Handler);
#endif
    Error_Tag = "interrupt-handler";
    Reset_IO (1);
    fun = Val (V_Interrupt_Handler);
    if (TYPE(fun) == T_Compound)
	(void)Funcall (fun, Null, 0);
    Format (Curr_Output_Port, "~%\7Interrupt!~%", 15, 0, (Object *)0);
    Reset ();
    /*NOTREACHED*/
}

Reset () {
    Object cp;

    cp = Val (V_Top_Level_Control_Point);
    if (TYPE(cp) == T_Control_Point)
	(void)Funcall_Control_Point (cp, Arg_True, 0);
    (void)fflush (stdout);
    exit (1);
}

Object P_Reset () {
    Reset_IO (0);
    Reset ();
    /*NOTREACHED*/
}

Range_Error (i) Object i; {
    Primitive_Error ("argument out of range: ~s", i);
}
