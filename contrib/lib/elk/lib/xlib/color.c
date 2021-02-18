#include "xlib.h"

Generic_Predicate (Color);

static Color_Equal (x, y) Object x, y; {
    register XColor *p = &COLOR(x)->c, *q = &COLOR(y)->c;
    return p->red == q->red && p->green == q->green && p->blue == q->blue;
}

Generic_Print (Color, "#[color %u]", POINTER(x));

Object Make_Color (r, g, b) unsigned short r, g, b; {
    register char *p;
    Object c;

    c = Find_Object (T_Color, (GENERIC)0, Match_X_Obj, r, g, b);
    if (Nullp (c)) {
	p = Get_Bytes (sizeof (struct S_Color));
	SET (c, T_Color, (struct S_Color *)p);
	COLOR(c)->tag = Null;
	COLOR(c)->c.red = r;
	COLOR(c)->c.green = g;
	COLOR(c)->c.blue = b;
	Register_Object (c, (GENERIC)0, (PFO)0, 0);
    }
    return c;
}

XColor *Get_Color (c) Object c; {
    Check_Type (c, T_Color);
    return &COLOR(c)->c;
}

static unsigned short Get_RGB_Value (x) Object x; {
    double d;

    d = Get_Double (x);
    if (d < 0.0 || d > 1.0)
	Primitive_Error ("bad RGB value: ~s", x);
    return (unsigned short)(d * 65535);
}

static Object P_Make_Color (r, g, b) Object r, g, b; {
    return Make_Color (Get_RGB_Value (r), Get_RGB_Value (g), Get_RGB_Value (b));
}

static Object P_Color_Rgb_Values (c) Object c; {
    Object ret, t, x;
    GC_Node3;

    Check_Type (c, T_Color);
    ret = t = Null;
    GC_Link3 (c, ret, t);
    t = ret = P_Make_List (Make_Fixnum (3), Null);
    GC_Unlink;
    x = Make_Reduced_Flonum (COLOR(c)->c.red / 65535.0);
    Car (t) = x; t = Cdr (t);
    x = Make_Reduced_Flonum (COLOR(c)->c.green / 65535.0);
    Car (t) = x; t = Cdr (t);
    x = Make_Reduced_Flonum (COLOR(c)->c.blue / 65535.0);
    Car (t) = x;
    return ret;
}

static Object P_Query_Color (cmap, p) Object cmap, p; {
    XColor c;
    Colormap cm = Get_Colormap (cmap);

    c.pixel = Get_Pixel (p);
    Disable_Interrupts;
    XQueryColor (COLORMAP(cmap)->dpy, cm, &c);
    Enable_Interrupts;
    return Make_Color (c.red, c.green, c.blue);
}

static Object P_Query_Colors (cmap, v) Object cmap, v; {
    Colormap cm = Get_Colormap (cmap);
    register i, n;
    Object ret;
    register XColor *p;
    GC_Node;
    Alloca_Begin;

    Check_Type (v, T_Vector);
    n = VECTOR(v)->size;
    Alloca (p, XColor*, n * sizeof (XColor));
    for (i = 0; i < n; i++)
	p[i].pixel = Get_Pixel (VECTOR(v)->data[i]);
    Disable_Interrupts;
    XQueryColors (COLORMAP(cmap)->dpy, cm, p, n);
    Enable_Interrupts;
    ret = Make_Vector (n, Null);
    GC_Link (ret);
    for (i = 0; i < n; i++, p++) {
	Object x = Make_Color (p->red, p->green, p->blue);
	VECTOR(ret)->data[i] = x;
    }
    GC_Unlink;
    Alloca_End;
    return ret;
}

static Object P_Lookup_Color (cmap, name) Object cmap, name; {
    register char *s;
    XColor visual, exact;
    Colormap cm = Get_Colormap (cmap);
    Object ret, x;
    GC_Node;
    Declare_C_Strings;

    Make_C_String (name, s);
    if (!XLookupColor (COLORMAP(cmap)->dpy, cm, s, &visual, &exact))
	Primitive_Error ("no such color: ~s", name);
    ret = Cons (Null, Null);
    GC_Link (ret);
    x = Make_Color (visual.red, visual.green, visual.blue);
    Car (ret) = x;
    x = Make_Color (exact.red, exact.green, exact.blue);
    Cdr (ret) = x;
    GC_Unlink;
    Dispose_C_Strings;
    return ret;
}

init_xlib_color () {
    Generic_Define (Color, "color", "color?");
    Define_Primitive (P_Make_Color,       "make-color",       3, 3, EVAL);
    Define_Primitive (P_Color_Rgb_Values, "color-rgb-values", 1, 1, EVAL);
    Define_Primitive (P_Query_Color,      "query-color",      2, 2, EVAL);
    Define_Primitive (P_Query_Colors,     "query-colors",     2, 2, EVAL);
    Define_Primitive (P_Lookup_Color,     "lookup-color",     2, 2, EVAL);
}
