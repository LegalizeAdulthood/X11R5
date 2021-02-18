/* Special forms
 */

#include "scheme.h"

static Object Sym_Else;

Init_Special () {
    Define_Symbol (&Sym_Else, "else");
}

Object P_Quote (argl) Object argl; {
    return Car (argl);
}

Object Quasiquote (x, level) Object x; {
    Object form, list, tail, cell, qcar, qcdr, ret;
    TC_Prolog;

    if (TYPE(x) != T_Pair)
	return x;
    if (EQ(Car (x), Sym_Unquote)) {
	x = Cdr (x);
	if (TYPE(x) != T_Pair)
	    Primitive_Error ("bad unquote form: ~s", x);
	if (level) {
	    ret = Cons (Car (x), Null);
	    ret = Quasiquote(ret, level-1);
	    ret = Cons (Sym_Unquote, ret);
	} else {
	    TC_Disable;
	    ret = Eval (Car (x));
	    TC_Enable;
	}
	return ret;
    } else if (TYPE(Car (x)) == T_Pair
	    && EQ(Car (Car (x)), Sym_Unquote_Splicing)) {
	GC_Node6;

	qcdr = Cdr (x);
	form = list = tail = cell = Null;
	x = Car (x);
	if (TYPE(Cdr (x)) != T_Pair)
	    Primitive_Error ("bad unquote-splicing form: ~s", x);
	if (level) {
	    GC_Link2 (list, qcdr);
	    list = Quasiquote(Cdr (x), level-1);
	    list = Cons (Sym_Unquote_Splicing, list);
	    qcdr = Quasiquote(qcdr, level);
	    list = Cons (list, qcdr);
	    GC_Unlink;
	    return list;
	}
	GC_Link6 (x, qcdr, form, list, tail, cell);
	TC_Disable;
	form = Eval (Car (Cdr (x)));
	TC_Enable;
	for ( ; TYPE(form) == T_Pair; tail = cell, form = Cdr (form)) {
	    cell = Cons (Car (form), Null);
	    if (Nullp (list))
		list = cell;
	    else
		P_Setcdr (tail, cell);
	}
	qcdr = Quasiquote (qcdr, level);
	GC_Unlink;
	if (Nullp (list))
	    return qcdr;
	P_Setcdr (tail, qcdr);
	return list;
    } else {
	GC_Node3;

	qcar = qcdr = Null;
	GC_Link3 (x, qcar, qcdr);
	if (EQ(Car (x), Sym_Quasiquote))   /* hack! */
	    ++level;
	qcar = Quasiquote (Car (x), level);
	qcdr = Quasiquote (Cdr (x), level);
	list = Cons (qcar, qcdr);
	GC_Unlink;
	return list;
    }
}

Object P_Quasiquote (argl) Object argl; {
    return Quasiquote (Car (argl), 0);
}

Object P_Begin (forms) Object forms; {
    GC_Node;
    TC_Prolog;

    if (Nullp (forms))
	return Null;
    GC_Link (forms);
    TC_Disable;
    for ( ; !Nullp (Cdr (forms)); forms = Cdr (forms))
	(void)Eval (Car (forms));
    GC_Unlink;
    TC_Enable;
    return Eval (Car (forms));
}

Object P_Begin1 (forms) Object forms; {
    register n;
    Object r, ret;
    GC_Node;
    TC_Prolog;

    GC_Link (forms);
    TC_Disable;
    for (n = 1; !Nullp (Cdr (forms)); n = 0, forms = Cdr (forms)) {
	r = Eval (Car (forms));
	if (n)
	    ret = r;
    }
    GC_Unlink;
    TC_Enable;
    r = Eval (Car (forms));
    return n ? r : ret;
}

Object P_If (argl) Object argl; {
    Object cond, ret;
    GC_Node;
    TC_Prolog;

    GC_Link (argl);
    TC_Disable;
    cond = Eval (Car (argl));
    TC_Enable;
    if (Truep(cond))
	ret = Eval (Car (Cdr (argl)));
    else
	ret = Begin (Cdr (Cdr (argl)));
    GC_Unlink;
    return ret;
}

Object P_Case (argl) Object argl; {
    Object ret, key, clause, select;
    GC_Node;
    TC_Prolog;

    GC_Link (argl);
    ret = False;
    TC_Disable;
    key = Eval (Car (argl));
    for (argl = Cdr (argl); !Nullp (argl); argl = Cdr (argl)) {
	clause = Car (argl);
	Check_List (clause);
	if (Nullp (clause))
	    Primitive_Error ("empty clause");
	select = Car (clause);
	if (EQ(select, Sym_Else)) {
	    if (!Nullp (Cdr (argl)))
		Primitive_Error ("`else' not in last clause");
	    if (Nullp (Cdr (clause)))
		Primitive_Error ("no forms in `else' clause");
	} else if (TYPE(select) == T_Pair) {
	    select = P_Memv (key, select);
	} else
	    select = P_Eqv (key, select);
	if (Truep (select)) {
	    clause = Cdr (clause);
	    TC_Enable;
	    ret = Nullp (clause) ? True : Begin (clause);
	    break;
	}
    }
    TC_Enable;
    GC_Unlink;
    return ret;
}

Object P_Cond (argl) Object argl; {
    Object ret, clause, guard;
    int else_clause = 0;
    GC_Node3;
    TC_Prolog;

    ret = False;
    clause = guard = Null;
    GC_Link3 (argl, clause, guard);
    TC_Disable;
    for ( ; !Nullp (argl); argl = Cdr (argl)) {
	clause = Car (argl);
	Check_List (clause);
	if (Nullp (clause))
	    Primitive_Error ("empty clause");
	guard = Car (clause);
	if (EQ(guard, Sym_Else)) {
	    if (!Nullp (Cdr (argl)))
		Primitive_Error ("`else' not in last clause");
	    if (Nullp (Cdr (clause)))
		Primitive_Error ("no forms in `else' clause");
	    else_clause++;
	} else
	    guard = Eval (Car (clause));
	if (Truep (guard)) {
	    clause = Cdr (clause);
	    if (!else_clause && !Nullp (clause) &&
		    EQ(Car (clause), Intern ("=>"))) {
		clause = Cdr (clause);
		if (Nullp (clause) || !Nullp (Cdr (clause)))
		    Primitive_Error ("syntax error after =>");
		clause = Eval (Car (clause));
		Check_Procedure (clause);
		guard = Cons (guard, Null);
		TC_Enable;
		ret = Funcall (clause, guard, 0);
	    } else {
		TC_Enable;
		ret = Nullp (clause) ? guard : Begin (clause);
	    }
	    break;
	}
    }
    TC_Enable;
    GC_Unlink;
    return ret;
}

Object General_Junction (argl, and) Object argl; register and; {
    Object ret;
    GC_Node;
    TC_Prolog;

    ret = and ? True : False;
    if (Nullp (argl))
	return ret;
    GC_Link (argl);
    TC_Disable;
    for ( ; !Nullp (Cdr (argl)); argl = Cdr (argl)) {
	ret = Eval (Car (argl));
	if (and != Truep (ret))
	    break;
    }
    TC_Enable;
    if (Nullp (Cdr (argl)))
	ret = Eval (Car (argl));
    GC_Unlink;
    return ret;
}

Object P_And (argl) Object argl; {
    return General_Junction (argl, 1);
}

Object P_Or (argl) Object argl; {
    return General_Junction (argl, 0);
}

Object P_Do (argl) Object argl; {
    Object tail, b, val, test, frame, newframe, len, ret;
    register local_vars;
    GC_Node6;
    TC_Prolog;

    b = test = frame = newframe = Null;
    GC_Link6 (argl, tail, b, test, frame, newframe);
    TC_Disable;
    for (tail = Car (argl); !Nullp (tail); tail = Cdr (tail)) {
	Check_List (tail);
	b = Car (tail);
	if (Nullp (b))
	    Primitive_Error ("bad initialization form");
	val = P_Cdr (b);
	Check_List (val);
	b = Car (b);
	Check_Type (b, T_Symbol);
	if (!Nullp (val))
	    val = Eval (Car (val));
	if (!EQ(Assq (b, frame), False))
	    Primitive_Error ("~s: duplicate variable binding", b);
	frame = Add_Binding (frame, b, val);
    }
    if (local_vars = !Nullp (frame))
	Push_Frame (frame);
    test = Car (Cdr (argl));
    Check_Type (test, T_Pair);
    while (1) {
	b = Eval (Car (test));
	if (Truep (b))
	    break;
	(void)Begin (Cdr (Cdr (argl)));
	if (!local_vars)
	    continue;
	newframe = Null;
	for (tail = Car (argl); !Nullp (tail); tail = Cdr (tail)) {
	    b = Car (tail);
	    /*                This could be done much more
	     *                efficiently, but I'm too lazy...
	     */
	    val = Cdr (b);
	    len = P_Length (val);
	    val = FIXNUM(len) > 1 ? Car (Cdr (val)) : Car (b);
	    newframe = Add_Binding (newframe, Car (b), Eval (val));
	}
	Pop_Frame ();
	Push_Frame (newframe);
    }
    Check_List (Cdr (test));
    TC_Enable;
    ret = Begin (Cdr (test));
    if (local_vars)
	Pop_Frame ();
    GC_Unlink;
    return ret;
}

Object General_Let (argl, disc) Object argl; {
    Object frame, b, binding, val, tail, ret;
    GC_Node5;
    TC_Prolog;

    frame = b = val = Null;
    GC_Link5 (argl, frame, b, val, tail);
    TC_Disable;
    for (tail = Car (argl); !Nullp (tail); tail = Cdr (tail)) {
	Check_List (tail);
	b = Car (tail);
	if (Nullp (b))
	    Primitive_Error ("bad binding form");
	val = P_Cdr (b);
	Check_List (val);
	if (!Nullp (val) && !Nullp (Cdr (val)))
	    Primitive_Error ("bad binding form");
	b = Car (b);
	Check_Type (b, T_Symbol);
	if (!Nullp (val))
	    val = Car (val);
	if (disc == 0) {
	    if (!Nullp (val))
		val = Eval (val);
	} else if (disc == 1) {
	    Push_Frame (frame);
	    if (!Nullp (val))
		val = Eval (val);
	    Pop_Frame ();
	} else if (disc == 2)
	    val = Null;
	binding = Assq (b, frame);
	if (disc != 1 && !EQ(binding, False))
	    Primitive_Error ("~s: duplicate variable binding", b);
	if (disc == 1 && !EQ(binding, False))
	    Cdr (binding) = val;
	else
	    frame = Add_Binding (frame, b, val);
    }
    Push_Frame (frame);
    if (disc == 2) {
	for (tail = Car (argl); !Nullp (tail); tail = Cdr (tail)) {
	    b = Car (tail);
	    val = Cdr (b);
	    if (Nullp (val))
		continue;
	    val = Car (val);
	    b = Lookup_Symbol (Car (b), 1);
	    val = Eval (val);
	    Cdr (b) = val;
	    SYMBOL(Car (b))->value = val;
	}
    }
    TC_Enable;
    ret = Begin (Cdr (argl));
    Pop_Frame ();
    GC_Unlink;
    return ret;
}

Object Named_Let (argl) Object argl; {
    Object b, val, tail, vlist, vtail, flist, ftail, cell;
    GC_Node6;
    TC_Prolog;

    tail = vlist = vtail = flist = ftail = Null;
    GC_Link6 (argl, tail, vlist, vtail, flist, ftail);
    TC_Disable;
    for (tail = Car (Cdr (argl)); !Nullp (tail); tail = Cdr (tail)) {
	Check_List (tail);
	b = Car (tail);
	if (Nullp (b))
	    Primitive_Error ("bad binding form");
	val = P_Cdr (b);
	Check_List (val);
	if (Nullp (val) || !Nullp (Cdr (val)))
	    Primitive_Error ("bad binding form");
	Check_Type (Car (b), T_Symbol);
	if (!Nullp (val))
	    val = Car (val);
	cell = Cons (val, Null);
	if (Nullp (flist))
	    flist = cell;
	else
	    P_Setcdr (ftail, cell);
	ftail = cell;
	cell = Cons (Car (Car (tail)), Null);
	if (Nullp (vlist))
	    vlist = cell;
	else
	    P_Setcdr (vtail, cell);
	vtail = cell;
    }
    Push_Frame (Add_Binding (Null, Car (argl), Null));
    tail = Cons (vlist, Cdr (Cdr (argl)));
    tail = P_Lambda (tail);
    COMPOUND(tail)->name = Car (argl);
    b = Lookup_Symbol (Car (argl), 1);
    Cdr (b) = tail;
    SYMBOL(Car (argl))->value = tail;
    TC_Enable;
    tail = Funcall (tail, flist, 1);
    Pop_Frame ();
    GC_Unlink;
    return tail;
}

Object P_Let (argl) Object argl; {
    if (TYPE(Car (argl)) == T_Symbol)
	return Named_Let (argl);
    else 
	return General_Let (argl, 0);
}

Object P_Letseq (argl) Object argl; {
    return General_Let (argl, 1);
}

Object P_Letrec (argl) Object argl; {
    return General_Let (argl, 2);
}

Object P_Fluid_Let (argl) Object argl; {
    Object b, sym, val, tail, ret;
    register WIND *w, *first = First_Wind, *last = Last_Wind;
    GC_Node5;
    TC_Prolog;
    Alloca_Begin;

    sym = b = val = Null;
    GC_Link5 (argl, sym, b, val, tail);
    TC_Disable;
    for (tail = Car (argl); !Nullp (tail); tail = Cdr (tail)) {
	Check_List (tail);
	b = Car (tail);
	if (Nullp (b))
	    Primitive_Error ("bad binding form");
	sym = Car (b);
	val = P_Cdr (b);
	Check_List (val);
	Check_Type (sym, T_Symbol);
	if (!Nullp (val))
	    val = Car (val);
	if (!Nullp (val))
	    val = Eval (val);
	b = Lookup_Symbol (sym, 1);
	Alloca (w, WIND*, sizeof (WIND));
	{
	    Object vec;

	    vec = Make_Vector (3, Null);
	    VECTOR(vec)->data[0] = sym;
	    VECTOR(vec)->data[1] = The_Environment;
	    VECTOR(vec)->data[2] = Cdr (b);
	    Add_Wind (w, vec, vec);
	}
	Cdr (b) = val;
	SYMBOL(sym)->value = val;
    }
    ret = Begin (Cdr (argl));
    for (w = Last_Wind; w != last; w = w->prev)
	Do_Wind (Car (w->inout));
    if (Last_Wind = last)
	last->next = 0;
    First_Wind = first;
    GC_Unlink;
    TC_Enable;
    Alloca_End;
    return ret;
}
