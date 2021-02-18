#include "xlib.h"

static Object P_List_Extensions (d) Object d; {
    Object ret;
    int n;
    register i;
    register char **p;
    GC_Node;

    Check_Type (d, T_Display);
    Disable_Interrupts;
    p = XListExtensions (DISPLAY(d)->dpy, &n);
    Enable_Interrupts;
    ret = Make_Vector (n, Null);
    GC_Link (ret);
    for (i = 0; i < n; i++) {
	Object e = Make_String (p[i], strlen (p[i]));
	VECTOR(ret)->data[i] = e;
    }
    GC_Unlink;
    XFreeExtensionList (p);
    return ret;
}

static Object P_Query_Extension (d, name) Object d, name; {
    char *s;
    int opcode, event, error;
    Object ret, t;
    GC_Node2;
    Declare_C_Strings;

    Check_Type (d, T_Display);
    Make_C_String (name, s);
    if (!XQueryExtension (DISPLAY(d)->dpy, s, &opcode, &event, &error)) {
	Dispose_C_Strings;
	return False;
    }
    t = ret = P_Make_List (Make_Fixnum (3), Null);
    GC_Link2 (ret, t);
    Car (t) = (opcode ? Make_Integer (opcode) : False); t = Cdr (t);
    Car (t) = (event ? Make_Integer (event) : False); t = Cdr (t);
    Car (t) = (error ? Make_Integer (error) : False);
    GC_Unlink;
    Dispose_C_Strings;
    return ret;
}

init_xlib_extension () {
    Define_Primitive (P_List_Extensions,    "list-extensions",   1, 1, EVAL);
    Define_Primitive (P_Query_Extension,    "query-extension",   2, 2, EVAL);
}
