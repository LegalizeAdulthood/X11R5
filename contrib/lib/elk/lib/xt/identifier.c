
#include "xt.h"

Generic_Predicate (Identifier);

static Identifier_Equal (x, y) Object x, y; {
    register struct S_Identifier *p = IDENTIFIER(x), *q = IDENTIFIER(y);
    return p->type == q->type && p->val == q->val && !p->free && !q->free;
}

Generic_Print (Identifier, "#[identifier %u]", POINTER(x));

Object Make_Id (type, val, num) XtPointer val; {
    register char *p;
    Object i;

    i = Find_Object (T_Identifier, (GENERIC)0, Match_Xt_Obj, type, val);
    if (Nullp (i)) {
	p = Get_Bytes (sizeof (struct S_Identifier));
	SET (i, T_Identifier, (struct S_Identifier *)p);
	IDENTIFIER(i)->tag = Null;
	IDENTIFIER(i)->type = type;
	IDENTIFIER(i)->val = val;
	IDENTIFIER(i)->num = num;
	IDENTIFIER(i)->free = 0;
	Register_Object (i, (GENERIC)0, (PFO)0, 0);
    }
    return i;
}

XtPointer Use_Id (x, type) Object x; {
    Check_Type (x, T_Identifier);
    if (IDENTIFIER(x)->type != type || IDENTIFIER(x)->free)
	Primitive_Error ("invalid identifier");
    IDENTIFIER(x)->free = 1;
    Deregister_Object (x);
    return IDENTIFIER(x)->val;
}

init_xt_identifier () {
    Generic_Define (Identifier, "identifier", "identifier?");
}
