/* provide, require, feature?
 */

#include "scheme.h"

static Object Features;

Init_Features () {
    Features = Null;
    Global_GC_Link (Features);
}

Object P_Featurep (sym) Object sym; {
    Check_Type (sym, T_Symbol);
    return Truep (P_Memq (sym, Features)) ? True : False;
}

Object P_Provide (sym) Object sym; {
    Check_Type (sym, T_Symbol);
    Features = Cons (sym, Features);
    return Void;
}

Object P_Require (argc, argv) Object *argv; {
    Object sym, a[1];
    GC_Node;

    sym = argv[0];
    GC_Link (sym);
    if (!Truep (P_Featurep (sym))) {
	a[0] = argc == 1 ? sym : argv[1];
	if (argc == 3)
	    Check_Type (argv[2], T_Environment);
	if (Truep (Val (V_Autoload_Notifyp)))
	    Format (Standard_Output_Port, "[Autoloading ~s]~%", 18, 1, a);
	(void)General_Load (a[0], argc == 3 ? argv[2] : The_Environment);
	if (!Truep (P_Featurep (sym)))
	    Primitive_Error ("feature ~s was not provided", sym);
    }
    GC_Unlink;
    return Void;
}
