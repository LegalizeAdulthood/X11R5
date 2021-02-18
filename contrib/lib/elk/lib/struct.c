#include <scheme.h>

#define STRUCT(x)  ((struct S_Struct *)POINTER(x))

struct S_Struct {
    Object name;
    Object slots;
    Object values;
};

int T_Struct;

static Object P_Structurep (x) Object x; {
    return TYPE(x) == T_Struct ? True : False;
}

static Object P_Structure_Name (x) Object x; {
    Check_Type (x, T_Struct);
    return STRUCT(x)->name;
}

static Object P_Structure_Slots (x) Object x; {
    Check_Type (x, T_Struct);
    return P_Vector_To_List (STRUCT(x)->slots);
}

static Object P_Structure_Values (x) Object x; {
    Check_Type (x, T_Struct);
    return P_Vector_To_List (STRUCT(x)->values);
}

static Check_Structure_Type (x, t) Object x, t; {
    Check_Type (x, T_Struct);
    Check_Type (t, T_Symbol);
    if (!EQ(STRUCT(x)->name, t))
	Primitive_Error ("wrong structure type ~s (expected ~s)",
	    STRUCT(x)->name, t);
}

static Object P_Structure_Ref (x, t, n) Object x, t, n; {
    Check_Structure_Type (x, t);
    return P_Vector_Ref (STRUCT(x)->values, n);
}

static Object P_Structure_Set (x, t, n, obj) Object x, t, n, obj; {
    Check_Structure_Type (x, t);
    return P_Vector_Set (STRUCT(x)->values, n, obj);
}

static Object P_Make_Structure (name, slots) Object name, slots; {
    register char *p;
    register n;
    Object s, vec, *vp;
    GC_Node3;

    Check_Type (name, T_Symbol);
    Check_List (slots);
    s = Null;
    GC_Link3 (s, name, slots);
    p = Get_Bytes (sizeof (struct S_Struct));
    SET(s, T_Struct, (struct S_Struct *)p);
    STRUCT(s)->name = name;
    n = Internal_Length (slots);
    vec = Make_Vector (n, Null);
    STRUCT(s)->values = vec;
    vec = Make_Vector (n, Null);
    STRUCT(s)->slots = vec;
    GC_Unlink;
    for (vp = VECTOR(vec)->data; n--; slots = Cdr (slots)) {
	Check_Type (Car (slots), T_Symbol);
	*vp++ = Car (slots);
    }
    return s;
}

static Structure_Eqv (a, b) Object a, b; { return EQ(a,b); }

static Structure_Equal (a, b) Object a, b; {
    return EQ(STRUCT(a)->name,STRUCT(b)->name) &&
	   Equal (STRUCT(a)->slots, STRUCT(b)->slots) &&
	   Equal (STRUCT(a)->values, STRUCT(b)->values);
}

static Structure_Print (x, port, raw, depth, length) Object x, port; {
    GC_Node2;

    GC_Link2 (port, x);
    Printf (port, "#[");
    Print_Object (STRUCT(x)->name, port, raw, depth, length);
    Printf (port, "-structure %u]", POINTER(x));
    GC_Unlink;
}

static Structure_Visit (sp, f) register Object *sp; register (*f)(); {
    (*f)(&STRUCT(*sp)->name);
    (*f)(&STRUCT(*sp)->slots);
    (*f)(&STRUCT(*sp)->values);
}

init_lib_struct () {
    T_Struct = Define_Type (0, "structure", NOFUNC, sizeof (struct S_Struct),
	Structure_Eqv, Structure_Equal, Structure_Print, Structure_Visit);
    Define_Primitive (P_Structurep,       "structure?",       1, 1, EVAL);
    Define_Primitive (P_Structure_Name,   "structure-name",   1, 1, EVAL);
    Define_Primitive (P_Structure_Slots,  "structure-slots",  1, 1, EVAL);
    Define_Primitive (P_Structure_Values, "structure-values", 1, 1, EVAL);
    Define_Primitive (P_Structure_Ref,    "structure-ref",    3, 3, EVAL);
    Define_Primitive (P_Structure_Set,    "structure-set!",   4, 4, EVAL);
    Define_Primitive (P_Make_Structure,   "make-structure",   2, 2, EVAL);
    P_Provide (Intern ("structures"));
}
