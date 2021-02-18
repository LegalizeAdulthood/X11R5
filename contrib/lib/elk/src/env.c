/* Environments, define, set!, etc.
 */

#include "scheme.h"

Object The_Environment;
Object Global_Environment;

Object List_To_Env(), General_Define();

Init_Env () {
    Global_Environment = The_Environment = List_To_Env (Cons (Null, Null));
    Global_GC_Link (Global_Environment);
    Global_GC_Link (The_Environment);
}

Object Env_To_List (env) Object env; {
    Object p;

    p = env;
    SETTYPE(p, T_Pair);
    return p;
}

Object List_To_Env (list) Object list; {
    Object e;

    e = list;
    SETTYPE(e, T_Environment);
    return e;
}

Object P_Env_List (env) Object env; {
    Check_Type (env, T_Environment);
    return Copy_List (Env_To_List (env));
}

Object P_Environmentp (x) Object x; {
    return TYPE(x) == T_Environment ? True : False;
}

Push_Frame (frame) Object frame; {
    Object e;

    Memoize_Frame (frame);
    e = Env_To_List (The_Environment);
    The_Environment = List_To_Env (Cons (frame, e));
}

Pop_Frame () {
    Object e;
    
    e = Env_To_List (The_Environment);
    The_Environment = List_To_Env (Cdr (e));
    Forget_Frame (Car (e));
}

Switch_Environment (to) Object to; {
    Object old, new;

    if (EQ(The_Environment,to))
	return;
    old = Env_To_List (The_Environment);
    new = Env_To_List (to);
    for ( ; !Nullp (old); old = Cdr (old)) {
	for (new = Env_To_List (to); !Nullp (new) && !EQ(new,old);
		new = Cdr (new))
	    ;
	if (EQ(new,old))
	    break;
	Forget_Frame (Car (old));
    }
    Memoize_Frames (Env_To_List (to), new);
    The_Environment = to;
}

Memoize_Frames (this, last) Object this, last; {
    if (Nullp (this) || EQ(this,last))
	return;
    Memoize_Frames (Cdr (this), last);
    Memoize_Frame (Car (this));
}

Memoize_Frame (frame) Object frame; {
    Object tail, binding;

    for (tail = frame; !Nullp (tail); tail = Cdr (tail)) {
	binding = Car (tail);
	SYMBOL(Car (binding))->value = Cdr (binding);
    }
}

Forget_Frame (frame) Object frame; {
    Object tail;

    for (tail = frame; !Nullp (tail); tail = Cdr (tail))
	SYMBOL(Car (Car (tail)))->value = Unbound;
}

Object Add_Binding (frame, sym, val) Object frame, sym, val; {
    Object b;
    GC_Node;

    GC_Link (frame);
    b = Cons (sym, val);
    frame = Cons (b, frame);
    GC_Unlink;
    return frame;
}

Object Lookup_Symbol (sym, err) Object sym; {
    Object p, b;

    for (p = Env_To_List (The_Environment); !Nullp (p); p = Cdr (p)) {
	b = Assq (sym, Car (p));
	if (!EQ(b, False))
	    return b;
    }
    if (err)
	Primitive_Error ("unbound variable: ~s", sym);
    return Null;
}

Object P_The_Environment () { return The_Environment; }

Object P_Global_Environment () { return Global_Environment; }

Object Define_Procedure (form, body, sym) Object form, body, sym; {
    Object ret;
    GC_Node3;

    GC_Link3 (form, body, sym);
    body = Cons (Cdr (form), body);
    body = Cons (sym, body);
    body = Cons (body, Null);
    body = Cons (Car (form), body);
    ret = General_Define (body, sym);
    GC_Unlink;
    return ret;
}

Object General_Define (argl, sym) Object argl, sym; {
    Object val, var, frame, binding;
    GC_Node3;

    var = Car (argl);
    val = Cdr (argl);
    if (TYPE(var) == T_Symbol) {
	frame = Null;
	GC_Link3 (var, val, frame);
	if (Nullp (val))
	    val = Void;
	else
	    val = Eval (Car (val));
	Set_Name (var, val);
	frame = Car (The_Environment);
	binding = Assq (var, frame);
	if (EQ(binding, False)) {
	    frame = Add_Binding (frame, var, val);
	    Car (The_Environment) = frame;
	} else
	    Cdr (binding) = val;
	SYMBOL(var)->value = val;
	GC_Unlink;
	return var;
    } else if (TYPE(var) == T_Pair) {
	if (Nullp (val))
	    Primitive_Error ("no sub-forms in compound: ~s", var);
	return Define_Procedure (var, val, sym);
    } else Wrong_Type_Combination (var, "symbol or pair");
    /*NOTREACHED*/
}

Object P_Define (argl) Object argl; {
    return General_Define (argl, Sym_Lambda);
}

Object P_Define_Macro (argl) Object argl; {
    return General_Define (argl, Sym_Macro);
}

Object P_Set (argl) Object argl; {
    Object val, var, binding, old;
    GC_Node3;

    var = Car (argl);
    val = Car (Cdr (argl));
    Check_Type (var, T_Symbol);
    binding = Lookup_Symbol (var, 1);
    old = Cdr (binding);
    GC_Link3 (var, binding, old);
    val = Eval (val);
    Set_Name (var, val);
    Cdr (binding) = val;
    SYMBOL(var)->value = val;
    GC_Unlink;
    return old;
}

Set_Name (var, val) Object var, val; {
    register t;

    t = TYPE(val);
    if (t == T_Compound) {
	if (Nullp (COMPOUND(val)->name))
	    COMPOUND(val)->name = var;
    } else if (t == T_Macro) {
	if (Nullp (MACRO(val)->name))
	    MACRO(val)->name = var;
    }
}

Object P_Boundp (x) Object x; {
    Check_Type (x, T_Symbol);
    return Nullp (Lookup_Symbol (x, 0)) ? False : True;
}
