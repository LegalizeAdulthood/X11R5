#include <scheme.h>

/* (when condition form1 form2 ...)
 */
static Object P_When (argl) Object argl; {
    Object cond;
    GC_Node;
    TC_Prolog;

    GC_Link (argl);
    TC_Disable;
    cond = Eval (Car (argl));
    TC_Enable;
    GC_Unlink;
    return Truep (cond) ? Begin (Cdr (argl)) : False;
}

init_lib_when () {
    Define_Primitive (P_When, "when", 2, MANY, NOEVAL);
}
