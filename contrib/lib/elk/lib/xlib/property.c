#include "xlib.h"

Object Sym_Now;

Generic_Predicate (Atom);

Generic_Simple_Equal (Atom, ATOM, atom);

Generic_Print (Atom, "#[atom %u]", ATOM(x)->atom);

Object Make_Atom (a) Atom a; {
    register char *p;
    Object atom;

    if (a == None)
	return Sym_None;
    atom = Find_Object (T_Atom, (GENERIC)0, Match_X_Obj, a);
    if (Nullp (atom)) {
	p = Get_Bytes (sizeof (struct S_Atom));
	SET (atom, T_Atom, (struct S_Atom *)p);
	ATOM(atom)->tag = Null;
	ATOM(atom)->atom = a;
	Register_Object (atom, (GENERIC)0, (PFO)0, 0);
    }
    return atom;
}

/* Should be used with care */
static Object P_Make_Atom (n) Object n; {
    return Make_Atom ((Atom)Get_Integer (n));
}

static Object Internal_Intern_Atom (d, name, flag) Object d, name; {
    register char *s;
    Object ret;
    Declare_C_Strings;

    Check_Type (d, T_Display);
    Make_C_String (name, s);
    ret = Make_Atom (XInternAtom (DISPLAY(d)->dpy, s, flag));
    Dispose_C_Strings;
    return ret;
}

static Object P_Intern_Atom (d, name) Object d, name; {
    return Internal_Intern_Atom (d, name, 0);
}

static Object P_Find_Atom (d, name) Object d, name; {
    return Internal_Intern_Atom (d, name, 1);
}

static Object P_Atom_Name (d, a) Object d, a; {
    register char *s;

    Check_Type (d, T_Display);
    Check_Type (a, T_Atom);
    Disable_Interrupts;
    s = XGetAtomName (DISPLAY(d)->dpy, ATOM(a)->atom);
    Enable_Interrupts;
    return Make_String (s, strlen (s));
}

static Object P_List_Properties (w) Object w; {
    register i;
    int n;
    register Atom *ap;
    Object v;
    GC_Node;

    Check_Type (w, T_Window);
    Disable_Interrupts;
    ap = XListProperties (WINDOW(w)->dpy, WINDOW(w)->win, &n);
    Enable_Interrupts;
    v = Make_Vector (n, Null);
    GC_Link (v);
    for (i = 0; i < n; i++) {
	Object x = Make_Atom (ap[i]);
	VECTOR(v)->data[i] = x;
    }
    GC_Unlink;
    XFree ((char *)ap);
    return v;
}

static Object P_Get_Property (w, prop, type, start, len, deletep)
	Object w, prop, type, start, len, deletep; {
    Atom req_type = AnyPropertyType, actual_type;
    int format;
    unsigned long nitems, bytes_left;
    unsigned char *data;
    Object ret, t, x;
    register i;
    GC_Node2;

    Check_Type (w, T_Window);
    Check_Type (prop, T_Atom);
    if (!EQ(type, False)) {
	Check_Type (type, T_Atom);
	req_type = ATOM(type)->atom;
    }
    Check_Type (deletep, T_Boolean);
    Disable_Interrupts;
    if (XGetWindowProperty (WINDOW(w)->dpy, WINDOW(w)->win, ATOM(prop)->atom,
	    (long)Get_Integer (start), (long)Get_Integer (len),
	    EQ(deletep, True), req_type, &actual_type, &format,
	    &nitems, &bytes_left, &data) != Success)
	Primitive_Error ("cannot get property");
    Enable_Interrupts;
    ret = t = P_Make_List (Make_Fixnum (4), Null);
    GC_Link2 (ret, t);
    x = Make_Atom (actual_type);
    Car (t) = x; t = Cdr (t);
    x = Make_Integer (format);
    Car (t) = x; t = Cdr (t);
    if (nitems) {
	if (format == 8) {
	    Object s;
	    x = Make_String ((char *)0, (int)nitems);
	    s = Car (t) = x;
	    bcopy ((char *)data, STRING(s)->data, (int)nitems);
	} else {
	    Object v = Make_Vector ((int)nitems, Null);
	    GC_Node;
	    /* Assumes short is 16 bits and int is 32 bits.
	     */
	    GC_Link (v);
	    for (i = 0; i < nitems; i++) {
		x = Make_Unsigned (format == 16 ?
		    *((short *)data + i) : *((int *)data + i));
		VECTOR(v)->data[i] = x;
	    }
	    Car (t) = v;
	    GC_Unlink;
	}
    }
    t = Cdr (t); 
    x = Make_Unsigned ((unsigned)bytes_left);
    Car (t) = x;
    GC_Unlink;
    return ret;
}

static Object P_Change_Property (w, prop, type, format, mode, data)
	Object w, prop, type, format, mode, data; {
    register i, m, x, nitems, f;
    char *buf;
    Alloca_Begin;

    Check_Type (w, T_Window);
    Check_Type (prop, T_Atom);
    Check_Type (type, T_Atom);
    m = Symbols_To_Bits (mode, 0, Propmode_Syms);
    switch (f = Get_Integer (format)) {
    case 8:
	Check_Type (data, T_String);
	buf = STRING(data)->data;
	nitems = STRING(data)->size;
	break;
    case 16: case 32:
	Check_Type (data, T_Vector);
	nitems = VECTOR(data)->size;
	Alloca (buf, char*, nitems * (f / sizeof (char)));
	for (i = 0; i < nitems; i++) {
	    x = Get_Integer (VECTOR(data)->data[i]);
	    if (f == 16) {
		if (x > 65535)
		    Primitive_Error ("format mismatch");
		*((short *)buf + i) = x;     /* Assumes short is 16 bits */
	    } else *((int *)buf + i) = x;    /*   and int is 32 bits. */
	}
	break;
    default:
	Primitive_Error ("invalid format: ~s", format);
    }
    XChangeProperty (WINDOW(w)->dpy, WINDOW(w)->win, ATOM(prop)->atom,
	ATOM(type)->atom, f, m, buf, nitems);
    Alloca_End;
    return Void;
}

static Object P_Delete_Property (w, prop) Object w, prop; {
    Check_Type (w, T_Window);
    Check_Type (prop, T_Atom);
    XDeleteProperty (WINDOW(w)->dpy, WINDOW(w)->win, ATOM(prop)->atom);
    return Void;
}

static Object P_Rotate_Properties (w, v, delta) Object w, v, delta; {
    Atom *p;
    register i, n;
    Alloca_Begin;

    Check_Type (w, T_Window);
    Check_Type (v, T_Vector);
    n = VECTOR(v)->size;
    Alloca (p, Atom*, n * sizeof (Atom));
    for (i = 0; i < n; i++) {
	Object a = VECTOR(v)->data[i];
	Check_Type (a, T_Atom);
	p[i] = ATOM(a)->atom;
    }
    XRotateWindowProperties (WINDOW(w)->dpy, WINDOW(w)->win, p, n,
	Get_Integer (delta));
    Alloca_End;
    return Void;
}

static Object P_Set_Selection_Owner (d, s, owner, time) Object d, s, owner,
	time; {
    Check_Type (d, T_Display);
    Check_Type (s, T_Atom);
    XSetSelectionOwner (DISPLAY(d)->dpy, ATOM(s)->atom, Get_Window (owner),
	Get_Time (time));
    return Void;
}

static Object P_Selection_Owner (d, s) Object d, s; {
    Check_Type (d, T_Display);
    Check_Type (s, T_Atom);
    return Make_Window (0, DISPLAY(d)->dpy,
	XGetSelectionOwner (DISPLAY(d)->dpy, ATOM(s)->atom));
}

static Object P_Convert_Selection (s, target, prop, w, time)
	Object s, target, prop, w, time; {
    Atom p = None;

    Check_Type (s, T_Atom);
    Check_Type (target, T_Atom);
    if (!EQ(prop, Sym_None)) {
	Check_Type (prop, T_Atom);
	p = ATOM(prop)->atom;
    }
    Check_Type (w, T_Window);
    XConvertSelection (WINDOW(w)->dpy, ATOM(s)->atom, ATOM(target)->atom,
	p, WINDOW(w)->win, Get_Time (time));
    return Void;
}

init_xlib_property () {
    Define_Symbol (&Sym_Now, "now");
    Generic_Define (Atom, "atom", "atom?");
    Define_Primitive (P_Make_Atom,         "make-atom",          1, 1, EVAL);
    Define_Primitive (P_Intern_Atom,       "intern-atom",        2, 2, EVAL);
    Define_Primitive (P_Find_Atom,         "find-atom",          2, 2, EVAL);
    Define_Primitive (P_Atom_Name,         "atom-name",          2, 2, EVAL);
    Define_Primitive (P_List_Properties,   "list-properties",    1, 1, EVAL);
    Define_Primitive (P_Get_Property,      "get-property",       6, 6, EVAL);
    Define_Primitive (P_Change_Property,   "change-property",    6, 6, EVAL);
    Define_Primitive (P_Delete_Property,   "delete-property",    2, 2, EVAL);
    Define_Primitive (P_Rotate_Properties, "rotate-properties",  3, 3, EVAL);
    Define_Primitive (P_Set_Selection_Owner, "set-selection-owner!",
								 4, 4, EVAL);
    Define_Primitive (P_Selection_Owner,   "selection-owner",    2, 2, EVAL);
    Define_Primitive (P_Convert_Selection, "convert-selection",  5, 5, EVAL);
}
