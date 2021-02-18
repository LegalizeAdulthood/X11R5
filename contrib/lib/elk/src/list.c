#include "scheme.h"

Object Const_Cons (car, cdr) Object car, cdr; {
    Object ret = P_Cons (car, cdr);
    SETCONST(ret);
    return ret;
}

Object P_Cons (car, cdr) Object car, cdr; {
    register char *p;
    register f = 0;
    Object cell;
    GC_Node2;

    p = Hp;
    ALIGN(p);
    if (p + sizeof (struct S_Pair) <= Heap_End) {
	Hp = p + sizeof (struct S_Pair);
    } else {
	GC_Link2 (car, cdr);
	p = Get_Bytes (sizeof (struct S_Pair));
	f++;
    }
    SET(cell, T_Pair, (struct S_Pair *)p);
    Car (cell) = car;
    Cdr (cell) = cdr;
    if (f)
	GC_Unlink;
    return cell;
}

Object P_Car (x) Object x; {
    if (Bug_Compat) {
	Check_List (x);
	return Nullp (x) ? Null : Car (x);
    } else {
	Check_Type (x, T_Pair);
	return Car (x);
    }
}

Object P_Cdr (x) Object x; {
    if (Bug_Compat) {
	Check_List (x);
	return Nullp (x) ? Null : Cdr (x);
    } else {
	Check_Type (x, T_Pair);
	return Cdr (x);
    }
}

Object Cxr (x, pat, len) Object x; register char *pat; register len; {
    Object ret;

    for (ret = x, pat += len; !Nullp (ret) && len > 0; len--)
	switch (*--pat) {
	case 'a': ret = P_Car (ret); break;
	case 'd': ret = P_Cdr (ret); break;
	default: Primitive_Error ("invalid pattern");
	}
    return ret;
}

Object P_Cddr  (x) Object x; { return Cxr (x,  "dd", 2); }
Object P_Cdar  (x) Object x; { return Cxr (x,  "da", 2); }
Object P_Cadr  (x) Object x; { return Cxr (x,  "ad", 2); }
Object P_Caar  (x) Object x; { return Cxr (x,  "aa", 2); }
Object P_Cdddr (x) Object x; { return Cxr (x, "ddd", 3); }
Object P_Cddar (x) Object x; { return Cxr (x, "dda", 3); }
Object P_Cdadr (x) Object x; { return Cxr (x, "dad", 3); }
Object P_Cdaar (x) Object x; { return Cxr (x, "daa", 3); }
Object P_Caddr (x) Object x; { return Cxr (x, "add", 3); }
Object P_Cadar (x) Object x; { return Cxr (x, "ada", 3); }
Object P_Caadr (x) Object x; { return Cxr (x, "aad", 3); }
Object P_Caaar (x) Object x; { return Cxr (x, "aaa", 3); }

Object P_Cxr (x, pat) Object x, pat; {
    Check_List (x);
    if (TYPE(pat) == T_Symbol)
	pat = SYMBOL(pat)->name;
    else if (TYPE(pat) != T_String)
	Wrong_Type_Combination (pat, "string or symbol");
    return Cxr (x, STRING(pat)->data, STRING(pat)->size);
}

Object P_Nullp (x) Object x; {
    return Nullp (x) ? True : False;
}

Object P_Pairp (x) Object x; {
    return TYPE(x) == T_Pair ? True : False;
}

Object P_Listp (x) Object x; {
    Object s;
    register f;

    for (s = x, f = 0; !Nullp (x); f ^= 1) {
	if (TYPE(x) != T_Pair)
	    return False;
	if ((x = Cdr (x)) == s)
	    return False;
	if (f) s = Cdr (s);
    }
    return True;
}

Object P_Setcar (x, new) Object x, new; {
    Check_Type (x, T_Pair);
    Check_Mutable (x);
    return Car (x) = new;
}

Object P_Setcdr (x, new) Object x, new; {
    Check_Type (x, T_Pair);
    Check_Mutable (x);
    return Cdr (x) = new;
}

Object General_Member (key, list, comp) Object key, list; register comp; {
    register r;

    for ( ; !Nullp (list); list = Cdr (list)) {
	Check_List (list);
	if (comp == 0)
	    r = EQ(Car (list), key);
	else if (comp == 1)
	    r = Eqv (Car (list), key);
	else
	    r = Equal (Car (list), key);
	if (r) return list;
    }
    return False;
}

Object P_Memq (key, list) Object key, list; {
    return General_Member (key, list, 0);
}

Object P_Memv (key, list) Object key, list; {
    return General_Member (key, list, 1);
}

Object P_Member (key, list) Object key, list; {
    return General_Member (key, list, 2);
}

Object General_Assoc (key, alist, comp) Object key, alist; register comp; {
    Object elem;
    register r;

    for ( ; !Nullp (alist); alist = Cdr (alist)) {
	Check_List (alist);
	elem = Car (alist);
	if (TYPE(elem) != T_Pair)
	    continue;
	if (comp == 0)
	    r = EQ(Car (elem), key);
	else if (comp == 1)
	    r = Eqv (Car (elem), key);
	else
	    r = Equal (Car (elem), key);
	if (r) return elem;
    }
    return False;
}

Object P_Assq (key, alist) Object key, alist; {
    return General_Assoc (key, alist, 0);
}

Object P_Assv (key, alist) Object key, alist; {
    return General_Assoc (key, alist, 1);
}

Object P_Assoc (key, alist) Object key, alist; {
    return General_Assoc (key, alist, 2);
}

Internal_Length (list) Object list; {
    Object tail;
    register i;

    for (i = 0, tail = list; TYPE(tail) == T_Pair; tail = Cdr (tail), i++)
	;
    return i;
}

Object P_Length (list) Object list; {
    Object tail;
    register i;

    for (i = 0, tail = list; !Nullp (tail); tail = Cdr (tail), i++)
	Check_List (tail);
    return Make_Integer (i);
}

Object P_Make_List (n, init) Object n, init; {
    register len;
    Object list;
    GC_Node;

    if ((len = Get_Integer (n)) < 0)
	Range_Error (n);
    list = Null;
    GC_Link (init);
    while (len-- > 0)
	list = Cons (init, list);
    GC_Unlink;
    return list;
}

Object P_List (argc, argv) Object *argv; {
    Object list, tail, cell;
    GC_Node2;

    GC_Link2 (list, tail);
    for (list = tail = Null; argc-- > 0; tail = cell) {
	cell = Cons (*argv++, Null);
	if (Nullp (list))
	    list = cell;
	else
	    P_Setcdr (tail, cell);
    }
    GC_Unlink;
    return list;
}

Object P_Last_Pair (x) Object x; {
    Check_Type (x, T_Pair);
    for ( ; TYPE(Cdr (x)) == T_Pair; x = Cdr (x)) ;
    return x;
}

Object P_Append (argc, argv) Object *argv; {
    Object list, last, tail, cell;
    register i;
    GC_Node3;

    list = last = Null;
    GC_Link3 (list, last, tail);
    for (i = 0; i < argc-1; i++) {
	for (tail = argv[i]; !Nullp (tail); tail = Cdr (tail)) {
	    Check_List (tail);
	    cell = Cons (Car (tail), Null);
	    if (Nullp (list))
		list = cell;
	    else
		P_Setcdr (last, cell);
	    last = cell;
	}
    }
    if (argc)
	if (Nullp (list))
	    list = argv[i];
	else
	    P_Setcdr (last, argv[i]);
    GC_Unlink;
    return list;
}

Object P_Append_Set (argc, argv) Object *argv; {
    register i, j;

    for (i = j = 0; i < argc; i++)
	if (!Nullp (argv[i]))
	    argv[j++] = argv[i];
    if (j == 0)
	return Null;
    for (i = 0; i < j-1; i++)
	P_Setcdr (P_Last_Pair (argv[i]), argv[i+1]);
    return *argv;
}

Object P_Reverse (x) Object x; {
    Object ret;
    GC_Node;

    GC_Link (x);
    for (ret = Null; !Nullp (x); x = Cdr (x)) {
	Check_List (x);
	ret = Cons (Car (x), ret);
    }
    GC_Unlink;
    return ret;
}

Object P_Reverse_Set (x) Object x; {
    Object prev, tail;

    for (prev = Null; !Nullp (x); prev = x, x = tail) {
	Check_List (x);
	tail = Cdr (x);
	P_Setcdr (x, prev);
    }
    return prev;
}

Object P_List_Tail (x, num) Object x, num; {
    register n;

    for (n = Get_Integer (num); n > 0 && !Nullp (x); n--, x = P_Cdr (x)) ;
    return x;
}

Object P_List_Ref (x, num) Object x, num; {
    return P_Car (P_List_Tail (x, num));
}

Object Copy_List (x) Object x; {
    Object car, cdr;
    GC_Node3;

    if (TYPE(x) == T_Pair) {
	if (stksize () > Max_Stack)
	    Uncatchable_Error ("Out of stack space");
	car = cdr = Null;
	GC_Link3 (x, car, cdr);
	car = Copy_List (Car (x));
	cdr = Copy_List (Cdr (x));
	x = Cons (car, cdr);
	GC_Unlink;
    }
    return x;
}
