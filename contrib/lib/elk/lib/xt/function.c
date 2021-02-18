#include "xt.h"

#define MAX_FUNCTIONS            512

static Object Functions;

Register_Function (x) Object x; {
    register i;

    for (i = 0; i < MAX_FUNCTIONS; i++)
	if (Nullp (VECTOR(Functions)->data[i])) break;
    if (i == MAX_FUNCTIONS)
	Primitive_Error ("too many registered functions");
    VECTOR(Functions)->data[i] = x;
    return i;
}

Object Get_Function (i) int i; {
    return VECTOR(Functions)->data[i];
}

Deregister_Function (i) int i; {
    VECTOR(Functions)->data[i] = Null;
}

init_xt_function () {
    Functions = Make_Vector (MAX_FUNCTIONS, Null);
    Global_GC_Link (Functions);
}
