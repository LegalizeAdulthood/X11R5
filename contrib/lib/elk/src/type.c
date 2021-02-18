#include "scheme.h"

/*ARGSUSED*/
Dummy_Visit (p, fp) Object *p, (*fp)(); {
    Panic ("Dummy_Visit");
}

/* User-defined types must be greater than T_Last and less than MAX_TYPE.
 */
TYPEDESCR Types[MAX_TYPE] = {
    { 0, "integer",		0, 0, 0, 0, 0, 0, },
    { 1, "integer", /*bignum*/	0, 0, 0, 0, 0, 0, }, 
    { 1, "real",		0, 0, 0, 0, 0, 0, },
    { 0, "null",		0, 0, 0, 0, 0, 0, },
    { 0, "boolean",		0, 0, 0, 0, 0, 0, },
    { 0, "void",		0, 0, 0, 0, 0, 0, },
    { 0, "unbound",		0, 0, 0, 0, 0, 0, },
    { 0, "special",		0, 0, 0, 0, 0, 0, },
    { 0, "character",		0, 0, 0, 0, 0, 0, },
    { 1, "symbol",		0, 0, 0, 0, 0, Dummy_Visit, },
    { 1, "pair",		0, 0, 0, 0, 0, Dummy_Visit, },
    { 1, "environment",		0, 0, 0, 0, 0, Dummy_Visit, },
    { 1, "string",		0, 0, 0, 0, 0, 0, },
    { 1, "vector",		0, 0, 0, 0, 0, Dummy_Visit, },
    { 1, "primitive",		0, 0, 0, 0, 0, 0, },
    { 1, "compound",		0, 0, 0, 0, 0, Dummy_Visit, },
    { 1, "control-point",	0, 0, 0, 0, 0, Dummy_Visit, },
    { 1, "promise",		0, 0, 0, 0, 0, Dummy_Visit, },
    { 1, "port",		0, 0, 0, 0, 0, Dummy_Visit, },
    { 0, "end-of-file",		0, 0, 0, 0, 0, 0, },
    { 1, "autoload",		0, 0, 0, 0, 0, Dummy_Visit, },
    { 1, "macro",		0, 0, 0, 0, 0, Dummy_Visit, },
    { 1, "!!broken-heart!!",	0, 0, 0, 0, 0, 0, },
};

Wrong_Type (x, t) Object x; register t; {
    Wrong_Type_Combination (x, Types[t].name);
}

Wrong_Type_Combination (x, name) Object x; register char *name; {
    register t = TYPE(x);
    register char *p;
    char buf[100];

    if (t < 0 || t >= MAX_TYPE || !(p = Types[t].name))
	Panic ("bad type");
    sprintf (buf, "wrong argument type %s (expected %s)", p, name);
    Primitive_Error (buf);
}

Object P_Type (x) Object x; {
    register t = TYPE(x);
    register char *p;

    if (t < 0 || t >= MAX_TYPE || !(p = Types[t].name))
	Panic ("bad type");
    return Intern (p);
}

Define_Type (t, name, size, const_size, eqv, equal, print, visit) register t;
	char *name;
	int (*size)(), (*eqv)(), (*equal)(), (*print)(), (*visit)(); {
    register TYPEDESCR *p;

    Error_Tag = "define-type";
    if (t == 0) {
	for (t = T_Last+1; t < MAX_TYPE && Types[t].name; t++)
	    ;
	if (t == MAX_TYPE)
	    Primitive_Error ("out of types");
    } else {
	if (t < 0 || t >= MAX_TYPE)
	    Primitive_Error ("bad type");
	if (Types[t].name)
	    Primitive_Error ("type already in use");
    }
    p = &Types[t];
    p->haspointer = 1;		/* Assumption */
    p->name = name;
    p->size = size;
    p->const_size = const_size;
    p->eqv = eqv;
    p->equal = equal;
    p->print = print;
    p->visit = visit;
    return t;
}

Object P_Voidp (x) Object x; {  /* Don't know a better place for this. */
    return TYPE(x) == T_Void ? True : False;
}
