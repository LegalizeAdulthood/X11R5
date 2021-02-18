#include <ctype.h>

#include "scheme.h"

Object Obarray;

Object Null,
       True,
       False,
       Unbound,
       Special,
       Void,
       Newline,
       Eof,
       Zero,
       One;

Init_Symbol () {
    SETTYPE(Null, T_Null);
    SETTYPE(True, T_Boolean); SETFIXNUM(True, 1);
    SETTYPE(False, T_Boolean); SETFIXNUM(False, 0);
    SETTYPE(Unbound, T_Unbound);
    SETTYPE(Special, T_Special);
    SETTYPE(Void, T_Void);
    SETTYPE(Eof, T_End_Of_File);
    Newline = Make_Char ('\n');
    Zero = Make_Fixnum (0);
    One = Make_Fixnum (1);
    Obarray = Make_Vector (OBARRAY_SIZE, Null);
    Global_GC_Link (Obarray);
}

Object Make_Symbol (name) Object name; {
    Object sym;
    register char *p;
    register struct S_Symbol *sp;
    GC_Node;

    GC_Link (name);
    p = Get_Bytes (sizeof (struct S_Symbol));
    SET(sym, T_Symbol, (struct S_Symbol *)p);
    sp = SYMBOL(sym);
    sp->name = name;
    sp->value = Unbound;
    sp->plist = Null;
    GC_Unlink;
    return sym;
}

Object P_Symbolp (x) Object x; {
    return TYPE(x) == T_Symbol ? True : False;
}

Object P_Symbol_To_String (x) Object x; {
    Check_Type (x, T_Symbol);
    return SYMBOL(x)->name;
}

Object Obarray_Lookup (str, len) register char *str; register len; {
    register h;
    register struct S_String *s;
    register struct S_Symbol *sym;
    Object p;

    h = Hash (str, len) % OBARRAY_SIZE;
    for (p = VECTOR(Obarray)->data[h]; !Nullp (p); p = sym->next) {
	sym = SYMBOL(p);
	s = STRING(sym->name);
	if (s->size == len && bcmp (s->data, str, len) == 0)
	    return p;
    }
    return Make_Fixnum (h);
}

Object Intern (str) char *str; {
    Object s, *p, sym, ostr;
    register i, len;
    register char *q;

    len = strlen (str);
    s = Obarray_Lookup (str, len);
    if (TYPE(s) != T_Fixnum)
	return s;
    ostr = Make_Const_String (str, len);
#ifdef CASE_INSENSITIVE
    for (q = STRING(ostr)->data, i = 0; i < len; i++, q++)
	if (isupper (*q))
	    *q = tolower (*q);
#endif
    sym = Make_Symbol (ostr);
    p = &VECTOR(Obarray)->data[FIXNUM(s)];
    SYMBOL(sym)->next = *p;
    return *p = sym;
}

Object P_String_To_Symbol (str) Object str; {
    Object s, *p, sym;

    Check_Type (str, T_String);
    s = Obarray_Lookup (STRING(str)->data, STRING(str)->size);
    if (TYPE(s) != T_Fixnum)
	return s;
    sym = Make_Symbol (str);
    p = &VECTOR(Obarray)->data[FIXNUM(s)];
    SYMBOL(sym)->next = *p;
    return *p = sym;
}

Object P_Oblist () {
    register i;
    Object p, list, bucket;
    GC_Node2;

    p = list = Null;
    GC_Link2 (p, list);
    for (i = 0; i < OBARRAY_SIZE; i++) {
	bucket = Null;
	for (p = VECTOR(Obarray)->data[i]; !Nullp (p); p = SYMBOL(p)->next)
	    bucket = Cons (p, bucket);
	if (!Nullp (bucket))
	    list = Cons (bucket, list);
    }
    GC_Unlink;
    return list;
}

Object P_Put (argc, argv) Object *argv; {
    Object sym, key, last, tail, prop;
    GC_Node3;

    sym = argv[0];
    key = argv[1];
    Check_Type (sym, T_Symbol);
    Check_Type (key, T_Symbol);
    last = Null;
    for (tail = SYMBOL(sym)->plist; !Nullp (tail); tail = Cdr (tail)) {
	prop = Car (tail);
	if (EQ(Car (prop), key)) {
	    if (argc == 3)
		Cdr (prop) = argv[2];
	    else if (Nullp (last))
		SYMBOL(sym)->plist = Cdr (tail);
	    else
		Cdr (last) = Cdr (tail);
	    return key;
	}
	last = tail;
    }
    if (argc == 2)
	return False;
    GC_Link3 (sym, last, key);
    tail = Cons (key, argv[2]);
    tail = Cons (tail, Null);
    if (Nullp (last))
	SYMBOL(sym)->plist = tail;
    else
	Cdr (last) = tail;
    GC_Unlink;
    return key;
}

Object P_Get (sym, key) Object sym, key; {
    Object prop;

    Check_Type (sym, T_Symbol);
    Check_Type (key, T_Symbol);
    prop = Assq (key, SYMBOL(sym)->plist);
    if (!Truep (prop))
	return False;
	/*
	 * Do we want to signal an error or return #f?
	 *
	 * Primitive_Error ("~s has no such property: ~s", sym, key);
	 */
    return Cdr (prop);
}

Object P_Symbol_Plist (sym) Object sym; {
    Check_Type (sym, T_Symbol);
    return Copy_List (SYMBOL(sym)->plist);
}

Hash (str, len) char *str; {
    register h;
    register char *p, *ep;

    h = 5 * len;
    if (len > 5)
	len = 5;
    for (p = str, ep = p+len; p < ep; ++p)
	h = (h << 2) ^ *p;
    return h & 017777777777;
}

Define_Symbol (sym, name) Object *sym; char *name; {
    *sym = Intern (name);
    _Global_GC_Link (sym);
}

Define_Variable (var, name, init) Object *var, init; char *name; {
    Object frame, sym;
    GC_Node;

    GC_Link (init);
    sym = Intern (name);
    SYMBOL(sym)->value = init;
    frame = Add_Binding (Car (The_Environment), sym, init);
    *var = Car (frame);
    Car (The_Environment) = frame;
    _Global_GC_Link (var);
    GC_Unlink;
}
