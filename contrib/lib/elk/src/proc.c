/* Eval, funcall, apply, map, lambda, macro, etc.
 */

#include "scheme.h"

#ifdef PORTABLE
#  define MAX_ARGS_ON_STACK  8
#else
#  define MAX_ARGS_ON_STACK  4
#endif

char *Error_Tag;

/* "Tail_Call" indicates whether we are executing the last form in a
 * sequence of forms.  If it is true and we are about to call a compound
 * procedure, we are allowed to check whether a tail-call can be
 * performed instead.
 */
int Tail_Call = 0;

Object Sym_Lambda,
       Sym_Macro;

Object Macro_Expand();

Init_Proc () {
    Define_Symbol (&Sym_Lambda, "lambda");
    Define_Symbol (&Sym_Macro, "macro");
}

Check_Procedure (x) Object x; {
    register t = TYPE(x);

    if (t != T_Primitive && t != T_Compound)
	Wrong_Type_Combination (x, "procedure");
    if (t == T_Primitive && PRIM(x)->disc == NOEVAL)
	Primitive_Error ("invalid procedure: ~s", x);
}

Object P_Procedurep (x) Object x; {
    register t = TYPE(x);
    return t == T_Primitive || t == T_Compound || t == T_Control_Point
	 ? True : False;
}

Object P_Primitivep (x) Object x; {
    return TYPE(x) == T_Primitive ? True : False;
}

Object P_Compoundp (x) Object x; {
    return TYPE(x) == T_Compound ? True : False;
}

Object P_Macrop (x) Object x; {
    return TYPE(x) == T_Macro ? True : False;
}

Object Make_Compound () {
    Object proc;
    register char *p;

    p = Get_Bytes (sizeof (struct S_Compound));
    SET(proc, T_Compound, (struct S_Compound *)p);
    COMPOUND(proc)->closure = COMPOUND(proc)->env = COMPOUND(proc)->name = Null;
    return proc;
}

Object Make_Primitive (fun, name, min, max, disc) Object (*fun)(); char *name;
	enum discipline disc; {
    Object prim;
    register char *p;
    register struct S_Primitive *pr;

    p = Get_Bytes (sizeof (struct S_Primitive));
    SET(prim, T_Primitive, (struct S_Primitive *)p);
    pr = PRIM(prim);
    pr->tag = Null;
    pr->fun = fun;
    pr->name = name;
    pr->minargs = min;
    pr->maxargs = max;
    pr->disc = disc;
    return prim;
}

Object Eval (form) Object form; {
    register t;
    register struct S_Symbol *sym;
    Object fun, binding, args, ret;
    GC_Node;

again:
    t = TYPE(form);
    if (t == T_Symbol) {
	sym = SYMBOL(form);
	if (EQ(sym->value,Unbound)) {
	    binding = Lookup_Symbol (form, 1);
	    sym->value = Cdr (binding);
	}
	ret = sym->value;
	if (TYPE(ret) == T_Autoload)
	    ret = Do_Autoload (form, ret);
	return ret;
    }
    if (t != T_Pair) {
	if (t == T_Null && !Bug_Compat)
	    Primitive_Error ("no subexpression in procedure call");
	if (t == T_Vector && !Bug_Compat)
	    Primitive_Error ("unevaluable object: ~s", form);
	return form;
    }
    if (stksize () > Max_Stack)
	Uncatchable_Error ("Out of stack space");
    GC_Link (form);
    fun = Eval (Car (form));
    args = Cdr (form);
    Check_List (args);
    if (TYPE(fun) == T_Macro) {
	form = Macro_Expand (fun, args);
	GC_Unlink;
	goto again;
    }
    ret = Funcall (fun, args, 1);
    GC_Unlink;
    return ret;
}

Object P_Eval (argc, argv) Object *argv; {
    Object ret, oldenv;
    GC_Node;

    if (argc == 1)
	return Eval (argv[0]);
    Check_Type (argv[1], T_Environment);
    oldenv = The_Environment;
    GC_Link (oldenv);
    Switch_Environment (argv[1]);
    ret = Eval (argv[0]);
    Switch_Environment (oldenv);
    GC_Unlink;
    return ret;
}

Object P_Apply (argc, argv) Object *argv; {
    Object ret, list, tail, cell, last;
    register i;
    GC_Node3;

    Check_Procedure (argv[0]);
    /* Make a list of all args but the last, then append the
     * last arg (which must be a proper list) to this list.
     */
    list = tail = last = Null;
    GC_Link3 (list, tail, last);
    for (i = 1; i < argc-1; i++, tail = cell) {
	cell = Cons (argv[i], Null);
	if (Nullp (list))
	    list = cell;
	else
	    P_Setcdr (tail, cell);
    }
    for (last = argv[argc-1]; !Nullp (last); last = Cdr (last), tail = cell) {
	cell = Cons (P_Car (last), Null);
	if (Nullp (list))
	    list = cell;
	else
	    P_Setcdr (tail, cell);
    }
    ret = Funcall (argv[0], list, 0);
    GC_Unlink;
    return ret;
}

Arglist_Length (list) Object list; {
    Object tail;
    register i;

    for (i = 0, tail = list; TYPE(tail) == T_Pair; tail = Cdr (tail), i++)
	;
    if (Nullp (tail))
	return i;
    Primitive_Error ("argument list is improper");
    /*NOTREACHED*/
}

Object Funcall_Primitive (fun, argl, eval) Object fun, argl; {
    register struct S_Primitive *prim;
    register argc, i;
    char *last;
    register Object *argv;
    Object abuf[MAX_ARGS_ON_STACK], ret;
    GC_Node2; GCNODE gcv;
    TC_Prolog;
    Alloca_Begin;

    prim = PRIM(fun);
    last = Error_Tag;
    Error_Tag = prim->name;
    argc = Arglist_Length (argl);
    if (argc < prim->minargs
	    || (prim->maxargs != MANY && argc > prim->maxargs))
	Primitive_Error ("wrong number of arguments");
    if (prim->disc == NOEVAL) {
	ret = (prim->fun)(argl);
    } else {
	/* Tail recursion is not possible while evaluating the arguments
	 * of a primitive procedure.
	 */
	TC_Disable;
	if (argc <= MAX_ARGS_ON_STACK)
	    argv = abuf;
	else
	    Alloca (argv, Object*, argc * sizeof (Object));
	GC_Link2 (argl, fun);
	gcv.gclen = 1; gcv.gcobj = argv; gcv.next = &gc2; GC_List = &gcv;
	for (i = 0; i < argc; i++, argl = Cdr (argl)) {
	    argv[i] = eval ? Eval (Car (argl)) : Car (argl);
	    gcv.gclen++;
	}
	TC_Enable;
	prim = PRIM(fun);   /* fun has possibly been moved during gc */
	if (prim->disc == VARARGS) {
	    ret = (prim->fun)(argc, argv);
	} else {
	    switch (argc) {
	    case 0:
		ret = (prim->fun)(); break;
	    case 1:
		ret = (prim->fun)(argv[0]); break;
	    case 2:
		ret = (prim->fun)(argv[0], argv[1]); break;
	    case 3:
		ret = (prim->fun)(argv[0], argv[1], argv[2]); break;
	    case 4:
		ret = (prim->fun)(argv[0], argv[1], argv[2], argv[3]); break;
	    case 5:
		ret = (prim->fun)(argv[0], argv[1], argv[2], argv[3], argv[4]);
		break;
	    case 6:
		ret = (prim->fun)(argv[0], argv[1], argv[2], argv[3], argv[4],
		                  argv[5]); break;
	    case 7:
		ret = (prim->fun)(argv[0], argv[1], argv[2], argv[3], argv[4],
		                  argv[5], argv[6]); break;
	    case 8:
		ret = (prim->fun)(argv[0], argv[1], argv[2], argv[3], argv[4],
		                  argv[5], argv[6], argv[7]); break;
	    case 9:
		ret = (prim->fun)(argv[0], argv[1], argv[2], argv[3], argv[4],
		                  argv[5], argv[6], argv[7], argv[8]); break;
	    case 10:
		ret = (prim->fun)(argv[0], argv[1], argv[2], argv[3], argv[4],
		                  argv[5], argv[6], argv[7], argv[8], argv[9]);
		break;
	    default:
		Panic ("too many args for primitive");
	    }
	}
	GC_Unlink;
	Alloca_End;
    }
    Error_Tag = last;
    return ret;
}

/* If we are in a tail recursion, we are reusing the old procedure
 * frame; we just assign new values to the formal parameters:
 */
#define Lambda_Bind(var,val)\
if (tail_calling) {\
    newframe = Add_Binding (newframe, var, val);\
} else {\
    frame = Add_Binding (frame, var, val);\
}

Object Funcall_Compound (fun, argl, eval) Object fun, argl; {
    register argc, min, max, i, tail_calling = 0;
    Object *argv, abuf[MAX_ARGS_ON_STACK], rest, ret, frame,
	tail, tail_call_env, oldenv, newframe;
    GC_Node6; GCNODE gcv;
    TC_Prolog;
    Alloca_Begin;

#ifdef lint
    tail_call_env = Null;
#endif
    frame = oldenv = tail = newframe = Null;
    GC_Link6 (argl, oldenv, frame, tail, fun, newframe);
again:
    argc = Arglist_Length (argl);
    min = COMPOUND(fun)->min_args;
    max = COMPOUND(fun)->max_args;
    if (argc < min)
	Primitive_Error ("too few arguments for ~s", fun);
    if (max >= 0 && argc > max)
	Primitive_Error ("too many arguments for ~s", fun);
    if (tail_calling) {
	tail = The_Environment;
	Switch_Environment (tail_call_env);
    } else {
	if (argc <= MAX_ARGS_ON_STACK)
	    argv = abuf;
	else
	    Alloca (argv, Object*, argc * sizeof (Object));
    }
    TC_Disable;
    gcv.gclen = 1; gcv.gcobj = argv; gcv.next = &gc6; GC_List = &gcv;
    for (i = 0; i < argc; i++, argl = Cdr (argl)) {
	argv[i] = eval ? Eval (Car (argl)) : Car (argl);
	gcv.gclen++;
    }
    TC_Enable;
    if (tail_calling)
	Switch_Environment (tail);
    tail = Car (Cdr (COMPOUND(fun)->closure));
    for (i = 0; i < min; i++, tail = Cdr (tail))
	Lambda_Bind (Car (tail), argv[i]);
    if (max == -1) {
	rest = P_List (argc-i, argv+i);
	Lambda_Bind (tail, rest);
    }
    if (tail_calling) {
	Pop_Frame ();
	Push_Frame (newframe);
    } else {
	oldenv = The_Environment;
	Switch_Environment (COMPOUND(fun)->env);
        Push_Frame (frame);
    }

    Tail_Call = 1;
    ret = Begin (Cdr (Cdr (COMPOUND(fun)->closure)));
    if (TYPE(ret) == T_Special) {
	argl = Car (ret);
	tail_call_env = Cdr (ret);
	tail_calling = 1;
	eval = 1;
	newframe = Null;
	goto again;
    }
    Tail_Call = 0;
    Pop_Frame ();
    Switch_Environment (oldenv);
    GC_Unlink;
    Alloca_End;
    return ret;
}

Object Funcall (fun, argl, eval) Object fun, argl; {
    register t;
    static struct S_Pair tail_call_info;
    Object ret, env;
    Tag_Node;

    t = TYPE(fun);
    if (Tail_Call && eval && t == T_Compound) {
	register GCNODE *p;
	Object f;

	for (p = GC_List; p; p = p->next) {
	    f = *(p->gcobj);
	    if (p->gclen == TAG_FUN && TYPE(f) == T_Compound) {
		if (EQ(f,fun)) {
		    SET(ret, T_Special, &tail_call_info);
		    Car (ret) = argl;
		    Cdr (ret) = The_Environment;
		    return ret;
		}
		break;
	    }
	}
    }
    env = The_Environment;
    Tag_Link (argl, fun, env);
    if (t == T_Primitive) {
	ret = Funcall_Primitive (fun, argl, eval);
    } else if (t == T_Compound) {
	ret = Funcall_Compound (fun, argl, eval);
    } else if (t == T_Control_Point) {
	Funcall_Control_Point (fun, argl, eval);
	/*NOTREACHED*/
    } else Primitive_Error ("application of non-procedure: ~s", fun);
    GC_Unlink;
    return ret;
}

Check_Formals (x, min, max) Object x; int *min, *max; {
    Object s, t1, t2;

    *min = *max = 0;
    for (t1 = Car (x); !Nullp (t1); t1 = Cdr (t1)) {
        s = TYPE(t1) == T_Pair ? Car (t1) : t1;
	Check_Type (s, T_Symbol);
	for (t2 = Car (x); t2 != t1; t2 = Cdr (t2))
	    if (EQ(s, Car (t2)))
		Primitive_Error ("~s: duplicate variable binding", s);
	if (TYPE(t1) != T_Pair)
	    break;
	(*min)++; (*max)++;
    }
    if (TYPE(t1) == T_Symbol)
	*max = -1;
    else if (!Nullp (t1))
	Wrong_Type_Combination (t1, "list or symbol");
}

Object P_Lambda (argl) Object argl; {
    Object proc, closure;
    GC_Node2;

    proc = Null;
    GC_Link2 (argl, proc);
    proc = Make_Compound ();
    closure = Cons (Sym_Lambda, argl);
    COMPOUND(proc)->closure = closure;
    COMPOUND(proc)->env = The_Environment;
    Check_Formals (argl, &COMPOUND(proc)->min_args,
	&COMPOUND(proc)->max_args);
    GC_Unlink;
    return proc;
}

Object P_Procedure_Lambda (p) Object p; {
    Check_Type (p, T_Compound);
    return Copy_List (COMPOUND(p)->closure);
}

Object P_Procedure_Env (p) Object p; {
    Check_Type (p, T_Compound);
    return COMPOUND(p)->env;
}

Object General_Map (argc, argv, accum) Object *argv; register accum; {
    register i;
    Object *args;
    Object head, list, tail, cell, arglist, val;
    GC_Node2; GCNODE gcv;
    Alloca_Begin;

    Check_Procedure (argv[0]);
    Alloca (args, Object*, (argc-1) * sizeof (Object));
    list = tail = Null;
    GC_Link2 (list, tail);
    gcv.gclen = argc; gcv.gcobj = args; gcv.next = &gc2; GC_List = &gcv;
    while (1) {
	for (i = 1; i < argc; i++) {
	    head = argv[i];
	    if (Nullp (head)) {
		GC_Unlink;
		Alloca_End;
		return list;
	    }
	    Check_Type (head, T_Pair);
	    args[i-1] = Car (head);
	    argv[i] = Cdr (head);
	}
	arglist = P_List (argc-1, args);
	val = Funcall (argv[0], arglist, 0);
	if (!accum)
	    continue;
	cell = Cons (val, Null);
	if (Nullp (list))
	    list = cell;
	else
	    P_Setcdr (tail, cell);
	tail = cell;
    }
    /*NOTREACHED*/
}

Object P_Map (argc, argv) Object *argv; {
    return General_Map (argc, argv, 1);
}

Object P_For_Each (argc, argv) Object *argv; {
    return General_Map (argc, argv, 0);
}

Object Make_Macro () {
    Object mac;
    register char *p;

    p = Get_Bytes (sizeof (struct S_Macro));
    SET(mac, T_Macro, (struct S_Macro *)p);
    MACRO(mac)->body = MACRO(mac)->name = Null;
    return mac;
}

Object P_Macro (argl) Object argl; {
    Object mac, body;
    GC_Node2;

    mac = Null;
    GC_Link2 (argl, mac);
    mac = Make_Macro ();
    body = Cons (Sym_Macro, argl);
    MACRO(mac)->body = body;
    Check_Formals (argl, &MACRO(mac)->min_args, &MACRO(mac)->max_args);
    GC_Unlink;
    return mac;
}

Object P_Macro_Body (m) Object m; {
    Check_Type (m, T_Macro);
    return Copy_List (MACRO(m)->body);
}

Object Macro_Expand (mac, argl) Object mac, argl; {
    register argc, min, max, i, tail_calling = 0;
    Object frame, ret, tail;
    Object newframe; /* not used; see Lambda_Bind() */
    GC_Node4;
    TC_Prolog;

    frame = tail = Null;
    GC_Link4 (argl, frame, tail, mac);
    argc = Arglist_Length (argl);
    min = MACRO(mac)->min_args;
    max = MACRO(mac)->max_args;
    if (argc < min)
	Primitive_Error ("too few arguments for ~s", mac);
    if (max >= 0 && argc > max)
	Primitive_Error ("too many arguments for ~s", mac);
    tail = Car (Cdr (MACRO(mac)->body));
    for (i = 0; i < min; i++, tail = Cdr (tail), argl = Cdr (argl))
	Lambda_Bind (Car (tail), Car (argl));
    if (max == -1)
	Lambda_Bind (tail, argl);
    Push_Frame (frame);
    TC_Disable;
    ret = Begin (Cdr (Cdr (MACRO(mac)->body)));
    TC_Enable;
    Pop_Frame ();
    GC_Unlink;
    return ret;
}

Object P_Macro_Expand (form) Object form; {
    Object ret, mac;
    GC_Node;

    Check_Type (form, T_Pair);
    GC_Link (form);
    mac = Eval (Car (form));
    if (TYPE(mac) != T_Macro)
	ret = form;
    else
	ret = Macro_Expand (mac, Cdr (form));
    GC_Unlink;
    return ret;
}
