#include "xt.h"

static Object V_Xt_Warning_Handler;

Xt_Warning (msg) char *msg; {
    Object args, fun;

    args = Cons (Make_String (msg, strlen (msg)), Null);
    fun = Val (V_Xt_Warning_Handler);
    if (TYPE(fun) == T_Compound)
	(void)Funcall (fun, args, 0);
    Format (Curr_Output_Port, msg, strlen (msg), 0, (Object *)0);
    P_Newline (0);
}

init_xt_error () {
    Define_Variable (&V_Xt_Warning_Handler, "xt-warning-handler", Null);
    XtSetWarningHandler (Xt_Warning);
}
