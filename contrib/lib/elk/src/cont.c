/* Continuations, dynamic-wind
 */

#include "scheme.h"

WIND *First_Wind, *Last_Wind;

Object P_Reentrant_Continuationsp () {
#ifdef PORTABLE
    return False;
#else
    return True;
#endif
}

Object P_Control_Pointp (x) Object x; {
    return TYPE(x) == T_Control_Point ? True : False;
}

Object Make_Control_Point (size) {
    Object control;
    register struct S_Control *cp;
    register char *p;

#ifdef PORTABLE
    p = Get_Bytes (sizeof (struct S_Control));
#else
    p = Get_Bytes (size + sizeof (struct S_Control) - 1);
#endif
    cp = (struct S_Control *)p;
    SET(control, T_Control_Point, cp);
    cp->env = The_Environment;
    cp->gclist = GC_List;
    cp->firstwind = First_Wind;
    cp->lastwind = Last_Wind;
    cp->tailcall = Tail_Call;
#ifdef PORTABLE
    cp->callable = 1;
#else
    cp->size = size;
#endif
    return control;
}

#ifdef PORTABLE

jmp_buf last_jmpbuf;

static long last_id;
static long target_id;
static Object jmp_ret;

Object P_Call_CC (proc) Object proc; {
    register t;
    Object control, ret;
    jmp_buf last_last_jmpbuf;
    GC_Node2;

    t = TYPE(proc);
    if (t != T_Primitive && t != T_Compound && t != T_Control_Point)
	Wrong_Type_Combination (proc, "procedure");
    control = Null;
    GC_Link2 (proc, control);
    control = Make_Control_Point (0);
    CONTROL(control)->id = last_id++;
    if (setjmp (CONTROL(control)->jmpbuf)) {
	CONTROL(control)->callable = 0;
	if (--last_id < 0)
	    Panic ("last_id1");
	copy_jmpbuf (last_last_jmpbuf, last_jmpbuf);
	if (target_id == CONTROL(control)->id) {
	    Enable_Interrupts;
	    GC_Unlink;
	    return jmp_ret;
	}
	longjmp (last_jmpbuf, 1);
    }
    copy_jmpbuf (last_jmpbuf, last_last_jmpbuf);
    copy_jmpbuf (CONTROL(control)->jmpbuf, last_jmpbuf);
    ret = Cons (control, Null);
    ret = Funcall (proc, ret, 0);
    CONTROL(control)->callable = 0;
    if (--last_id < 0)
	Panic ("last_id2");
    copy_jmpbuf (last_last_jmpbuf, last_jmpbuf);
    GC_Unlink;
    return ret;
}
#else

Object P_Call_CC (proc) Object proc; {
    int size;
    register t;
    Object control, ret;
    GC_Node;

    t = TYPE(proc);
    if (t != T_Primitive && t != T_Compound && t != T_Control_Point)
	Wrong_Type_Combination (proc, "procedure");
    GC_Link (proc);
    size = stksize ();
    control = Make_Control_Point (size);
    SETFAST(ret,saveenv (CONTROL(control)->stack));
    if (TYPE(ret) != T_Special) {
	Enable_Interrupts;
	return ret;
    }
    control = Cons (control, Null);
    ret = Funcall (proc, control, 0);
    GC_Unlink;
    return ret;
}
#endif

Funcall_Control_Point (control, argl, eval) Object control, argl; {
    Object val, len, x;
    register struct S_Control *cp;
    register WIND *wp, *p;
    register delta = 0;
    GC_Node3;

#ifdef PORTABLE
    if (!CONTROL(control)->callable)
	Primitive_Error (
	    "re-entrant control points not supported in the portable version");
#endif
    if (GC_In_Progress)
	Fatal_Error ("jumping out of GC");
    val = Null;
    GC_Link3 (argl, control, val);
    len = P_Length (argl);
    if (FIXNUM(len) != 1)
	Primitive_Error ("control point expects one argument");
    val = Car (argl);
    if (eval)
	val = Eval (val);

#ifndef PORTABLE
    delta = *(int *)(CONTROL(control)->stack);
#endif
    wp = CONTROL(control)->lastwind;
    p = (WIND *)NORM(wp);
    x = wp ? p->inout : Null;
    for (wp = Last_Wind; wp && !EQ(wp->inout,x); wp = wp->prev)
	Do_Wind (Cdr (wp->inout));

    for (wp = CONTROL(control)->firstwind; wp; wp = p->next) {
	p = (WIND *)NORM(wp);
	if (First_Wind && EQ(p->inout,First_Wind->inout))
	    break;
	Do_Wind (Car (p->inout));
    }
    GC_Unlink;
    cp = CONTROL(control);
    Switch_Environment (cp->env);
    GC_List = cp->gclist;
    First_Wind = cp->firstwind;
    Last_Wind = cp->lastwind;
    Tail_Call = cp->tailcall;
#ifdef PORTABLE
    jmp_ret = val;
    target_id = CONTROL(control)->id;
    longjmp (last_jmpbuf, 1);
#else
    jmpenv (cp->stack, val);
#endif
    /*NOTREACHED*/
}

Do_Wind (w) Object w; {
    Object oldenv, b, tmp;

    if (TYPE(w) == T_Vector) {          /* fluid-let */
	oldenv = The_Environment;
	Switch_Environment (VECTOR(w)->data[1]);
	b = Lookup_Symbol (VECTOR(w)->data[0]);
	if (Nullp (b))
	    Panic ("fluid-let");
	tmp = VECTOR(w)->data[2];
	VECTOR(w)->data[2] = Cdr (b);
	Cdr (b) = tmp;
	SYMBOL(Car (b))->value = tmp;
	VECTOR(w)->data[1] = oldenv;
	Switch_Environment (oldenv);
    } else {                            /* dynamic-wind */
	(void)Funcall (w, Null, 0);
    }
}

Add_Wind (w, in, out) register WIND *w; Object in, out; {
    Object inout;
    GC_Node2;

    GC_Link2 (in, out);
    inout = Cons (in, out);
    w->inout = inout;
    w->next = 0;
    if (First_Wind == 0)
	First_Wind = w;
    else
	Last_Wind->next = w;
    w->prev = Last_Wind;
    Last_Wind = w;
    GC_Unlink;
}

Object P_Dynamic_Wind (in, body, out) Object in, body, out; {
    WIND w, *first = First_Wind;
    Object ret;
    GC_Node3;

    Check_Procedure (in);
    Check_Procedure (body);
    Check_Procedure (out);
    ret = Null;
    GC_Link3 (body, out, ret);
    Add_Wind (&w, in, out);
    (void)Funcall (in, Null, 0);
    ret = Funcall (body, Null, 0);
    (void)Funcall (out, Null, 0);
    if (Last_Wind = w.prev)
	Last_Wind->next = 0;
    First_Wind = first;
    GC_Unlink;
    return ret;
}

Object P_Control_Point_Env (c) Object c; {
    Check_Type (c, T_Control_Point);
    return CONTROL(c)->env;
}
