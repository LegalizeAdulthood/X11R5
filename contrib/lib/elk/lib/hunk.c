#include <scheme.h>

#define T_Hunk3  (T_Last+1)

#define HUNK3(x)  ((struct S_Hunk3 *)POINTER(x))

struct S_Hunk3 {
    Object first, second, third;
};

static Object P_Hunk3_Cons (a, b, c) Object a, b, c; {
    register char *p;
    Object h;
    GC_Node3;

    GC_Link3 (a, b, c);
    p = Get_Bytes (sizeof (struct S_Hunk3));
    SET(h, T_Hunk3, (struct S_Hunk3 *)p);
    HUNK3(h)->first = a; HUNK3(h)->second = b; HUNK3(h)->third = c;
    GC_Unlink;
    return h;
}

static Object P_Hunk3p (x) Object x; {
    return TYPE(x) == T_Hunk3 ? True : False;
}

static Object P_Hunk3_Cxr (h, n) Object h, n; {
    Check_Type (h, T_Hunk3);
    switch (Get_Integer (n)) {
    case 0: return HUNK3(h)->first;
    case 1: return HUNK3(h)->second;
    case 2: return HUNK3(h)->third;
    default: Range_Error (n);
    }
}

static Object P_Hunk3_Set_Cxr (h, n, val) Object h, n, val; {
    Check_Type (h, T_Hunk3);
    switch (Get_Integer (n)) {
    case 0: HUNK3(h)->first  = val; break;
    case 1: HUNK3(h)->second = val; break;
    case 2: HUNK3(h)->third  = val; break;
    default: Range_Error (n);
    }
    return h;
}

static Hunk3_Eqv (a, b) Object a, b; { return EQ(a,b); }

static Hunk3_Equal (a, b) Object a, b; {
    return Equal (HUNK3(a)->first,  HUNK3(b)->first)  &&
           Equal (HUNK3(a)->second, HUNK3(b)->second) &&
           Equal (HUNK3(a)->third,  HUNK3(b)->third);
}

static Hunk3_Print (h, port, raw, depth, length) Object h, port; {
    Printf (port, "#[hunk3 %u]", POINTER(h));
}

static Hunk3_Visit (hp, f) Object *hp; int (*f)(); {
    (*f)(&HUNK3(*hp)->first);
    (*f)(&HUNK3(*hp)->second);
    (*f)(&HUNK3(*hp)->third);
}

init_lib_hunk () {
    Define_Type (T_Hunk3, "hunk3", NOFUNC, sizeof (struct S_Hunk3),
	Hunk3_Eqv, Hunk3_Equal, Hunk3_Print, Hunk3_Visit);
    Define_Primitive (P_Hunk3_Cons,    "hunk3-cons",     3, 3, EVAL);
    Define_Primitive (P_Hunk3p,        "hunk3?",         1, 1, EVAL);
    Define_Primitive (P_Hunk3_Cxr,     "hunk3-cxr",      2, 2, EVAL);
    Define_Primitive (P_Hunk3_Set_Cxr, "hunk3-set-cxr!", 3, 3, EVAL);
}
