/* Delay and force
 */

#include "scheme.h"

Object P_Promisep (x) Object x; {
    return TYPE(x) == T_Promise ? True : False;
}

Object P_Delay (argl) Object argl; {
    Object d;
    register char *p;
    GC_Node;

    GC_Link (argl);
    p = Get_Bytes (sizeof (struct S_Promise));
    GC_Unlink;
    SET(d, T_Promise, (struct S_Promise *)p);
    PROMISE(d)->done = 0;
    PROMISE(d)->env = The_Environment;
    PROMISE(d)->thunk = Car (argl);
    return d;
}

Object P_Force (d) Object d; {
    Object ret, a[2];
    GC_Node;

    Check_Type (d, T_Promise);
    if (PROMISE(d)->done)
	return PROMISE(d)->thunk;
    GC_Link (d);
    a[0] = PROMISE(d)->thunk; a[1] = PROMISE(d)->env;
    ret = P_Eval (2, a);
    GC_Unlink;
    PROMISE(d)->done = 1;
    return PROMISE(d)->thunk = ret;
}

Object P_Promise_Env (p) Object p; {
    Check_Type (p, T_Promise);
    return PROMISE(p)->env;
}
