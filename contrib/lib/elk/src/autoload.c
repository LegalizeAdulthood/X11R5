#include "scheme.h"

Object V_Autoload_Notifyp;

Init_Auto () {
    Define_Variable (&V_Autoload_Notifyp, "autoload-notify?", True);
}

Object P_Autoload (sym, file) Object sym, file; {
    Object al, ret;
    register char *p;
    GC_Node3;

    al = Null;
    Check_Type (sym, T_Symbol);
    if (TYPE(file) != T_Symbol && TYPE(file) != T_String)
	Wrong_Type_Combination (file, "string or symbol");
    GC_Link3 (al, sym, file);
    p = Get_Bytes (sizeof (struct S_Autoload));
    SET(al, T_Autoload, (struct S_Autoload *)p);
    AUTOLOAD(al)->file = file;
    AUTOLOAD(al)->env = The_Environment;
    al = Cons (al, Null);
    al = Cons (sym, al);
    ret = P_Define (al);
    GC_Unlink;
    return ret;
}

Object Do_Autoload (sym, al) Object sym, al; {
    Object val, a[1];
    GC_Node;

    if (Truep (Val (V_Autoload_Notifyp))) {
	a[0] = AUTOLOAD(al)->file;
	Format (Standard_Output_Port, "[Autoloading ~s]~%", 18, 1, a);
    }
    GC_Link (sym);
    (void)General_Load (AUTOLOAD(al)->file, AUTOLOAD(al)->env);
    GC_Unlink;
    val = SYMBOL(sym)->value;
    if (TYPE(val) == T_Autoload)
	Primitive_Error ("autoloading failed to define ~s", sym);
    return val;
}
