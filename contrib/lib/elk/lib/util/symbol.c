#include <scheme.h>
#include "symbol.h"

unsigned long Symbols_To_Bits (x, mflag, stab) Object x; SYMDESCR *stab; {
    register SYMDESCR *syms;
    register unsigned long mask = 0;
    Object l, s;
    register char *p;
    register n;

    for (l = x; !Nullp (l); l = Cdr (l)) {
	if (mflag) {
	    Check_Type (l, T_Pair);
	    x = Car (l);
	}
	Check_Type (x, T_Symbol);
	s = SYMBOL(x)->name;
	p = STRING(s)->data;
	n = STRING(s)->size;
	for (syms = stab; syms->name; syms++)
	    if (n && strncmp (syms->name, p, n) == 0) break;
	if (syms->name == 0)
	    Primitive_Error ("invalid argument: ~s", x);
	mask |= syms->val;
	if (!mflag) break;
    }
    return mask;
}

Object Bits_To_Symbols (x, mflag, stab) unsigned long x; SYMDESCR *stab; {
    register SYMDESCR *syms;
    Object list, tail, cell;
    GC_Node2;

    if (mflag) {
	GC_Link2 (list, tail);
	for (list = tail = Null, syms = stab; syms->name; syms++)
	    if ((x & syms->val) && syms->val != ~0) {
		Object z = Intern (syms->name);
		cell = Cons (z, Null);
		if (Nullp (list))
		    list = cell;
		else
		    P_Setcdr (tail, cell);
		tail = cell;
	    }
	GC_Unlink;
	return list;
    }
    for (syms = stab; syms->name; syms++)
	if (syms->val == x)
	    return Intern (syms->name);
    return Null;
}
