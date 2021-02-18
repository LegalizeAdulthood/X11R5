#include "xt.h"

static Object P_Xt_Release_4_Or_Laterp () {
    return True;
}

static Object P_Xt_Release_5_Or_Laterp () {
#ifdef XT_RELEASE_5_OR_LATER
    return True;
#else
    return False;
#endif
}

init_xt_xt () {
    Define_Primitive (P_Xt_Release_4_Or_Laterp, "xt-release-4-or-later?",
	0, 0, EVAL);
    Define_Primitive (P_Xt_Release_5_Or_Laterp, "xt-release-5-or-later?",
	0, 0, EVAL);
    XtToolkitInitialize ();
    P_Provide (Intern ("xt.o"));
}
