/* backtrace-list, etc.
 */

#include "scheme.h"

Object P_Backtrace_List (argc, argv) Object *argv; {
    register GCNODE *p, *gp = GC_List;
    register delta = 0;
    Object cp, list, tail, cell, vec;
    GC_Node3;

    if (argc > 0) {
	cp = argv[0];
	Check_Type (cp, T_Control_Point);
#ifndef PORTABLE
	delta = *(int *)(CONTROL(cp)->stack);
#endif
	gp = CONTROL(cp)->gclist;
    }
    vec = list = tail = Null;
    GC_Link3 (vec, list, tail);
    for ( ; gp; gp = p->next) {
	p = (GCNODE *)NORM(gp);
	switch (p->gclen) {
	case TAG_ENV:
	    vec = Make_Vector (3, Null);
	    VECTOR(vec)->data[2] = *(Object *)NORM(p->gcobj);
	    break;
	case TAG_FUN:
	    VECTOR(vec)->data[0] = *(Object *)NORM(p->gcobj);
	    break;
	case TAG_ARGS:
	    VECTOR(vec)->data[1] = *(Object *)NORM(p->gcobj);
	    cell = Cons (vec, Null);
	    if (Nullp (list))
		list = cell;
	    else
		P_Setcdr (tail, cell);
	    tail = cell;
	}
    }
    GC_Unlink;
    return list;
}
