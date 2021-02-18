#include "xlib.h"

static Object P_Get_Default (d, program, option) Object d, program, option; {
    register char *ret, *p, *o;
    Object r;
    Declare_C_Strings;

    Check_Type (d, T_Display);
    Make_C_String (program, p);
    Make_C_String (option, o);
    if (!(ret = XGetDefault (DISPLAY(d)->dpy, p, o)))
	r =  False;
    else
	r = Make_String (ret, strlen (ret));
    Dispose_C_Strings;
    return r;
}

static Object P_Resource_Manager_String (d) Object d; {
    register char *ret;

    Check_Type (d, T_Display);
    ret = XResourceManagerString (DISPLAY(d)->dpy);
    return ret ? Make_String (ret, strlen (ret)) : False;
}

static Object P_Parse_Geometry (string) Object string; {
    register char *s;
    Object ret, t;
    register mask;
    int x, y, w, h;
    Declare_C_Strings;

    Make_C_String (string, s);
    mask = XParseGeometry (s, &x, &y, &w, &h);
    t = ret = P_Make_List (Make_Fixnum (6), False);
    if (mask & XNegative) Car (t) = True; t = Cdr (t);
    if (mask & YNegative) Car (t) = True; t = Cdr (t);
    if (mask & XValue) Car (t) = Make_Fixnum (x); t = Cdr (t);
    if (mask & YValue) Car (t) = Make_Fixnum (y); t = Cdr (t);
    if (mask & WidthValue) Car (t) = Make_Fixnum (w); t = Cdr (t);
    if (mask & HeightValue) Car (t) = Make_Fixnum (h);
    Dispose_C_Strings;
    return ret;
}

static Object P_Parse_Color (d, cmap, spec) Object d, cmap, spec; {
    register char *s;
    XColor ret;
    Object r;
    Declare_C_Strings;

    Check_Type (d, T_Display);
    Make_C_String (spec, s);
    if (!XParseColor (DISPLAY(d)->dpy, Get_Colormap (cmap), s, &ret))
	r = False;
    else
	r = Make_Color (ret.red, ret.green, ret.blue);
    Dispose_C_Strings;
    return r;
}

init_xlib_util () {
    Define_Primitive (P_Get_Default,       "get-default",         3, 3, EVAL);
    Define_Primitive (P_Resource_Manager_String,
			"resource-manager-string",                1, 1, EVAL);
    Define_Primitive (P_Parse_Geometry,    "parse-geometry",      1, 1, EVAL);
    Define_Primitive (P_Parse_Color,       "parse-color",         3, 3, EVAL);
}
