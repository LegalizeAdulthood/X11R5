#include <scheme.h>

static Object New_Handler;

static void New_Handler_Proc () {
    (void)Funcall (New_Handler, Null, 0);
}

static Object P_Set_New_Handler (p) Object p; {
    Object old;

    Check_Procedure (p);
    old = New_Handler;
    New_Handler = p;
    return old;
}

init_lib_cplusplus () {
    New_Handler = Null;
    Global_GC_Link (New_Handler);
    set_new_handler (New_Handler_Proc);
    Define_Primitive (P_Set_New_Handler, "set-c++-new-handler!", 1, 1, EVAL);
}
