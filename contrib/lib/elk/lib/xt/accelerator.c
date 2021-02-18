#include "xt.h"

XtAccelerators Get_Accelerators (a) Object a; {
    register char *s;
    XtAccelerators ret;
    Declare_C_Strings;

    Make_C_String (a, s);
    if ((ret = XtParseAcceleratorTable (s)) == 0)
	Primitive_Error ("bad accelerator table: ~s", a);
    Dispose_C_Strings;
    return ret;
}

static Object P_Install_Accelerators (dst, src) Object dst, src; {
    Check_Widget (dst);
    Check_Widget (src);
    XtInstallAccelerators (WIDGET(dst)->widget, WIDGET(src)->widget);
    return Void;
}
    
static Object P_Install_All_Accelerators (dst, src) Object dst, src; {
    Check_Widget (dst);
    Check_Widget (src);
    XtInstallAllAccelerators (WIDGET(dst)->widget, WIDGET(src)->widget);
    return Void;

}

init_xt_accelerator () {
    Define_Primitive (P_Install_Accelerators,
			"install-accelerators",       2, 2, EVAL);
    Define_Primitive (P_Install_All_Accelerators,
			"install-all-accelerators",   2, 2, EVAL);
}
