#include "xt.h"

#define MAX_CONVERTER   512

typedef struct {
    char *name;
    int scheme_to_c;
    PFO to_scheme;
    PFX to_c;
} CONVERTER;

static CONVERTER ctab[MAX_CONVERTER], *clast = ctab;

Define_Converter_To_Scheme (name, c) char *name; PFO c; {
    Error_Tag = "c->scheme";
    if (clast == ctab+MAX_CONVERTER)
	Primitive_Error ("too many converters");
    clast->name = name;
    clast->scheme_to_c = 0;
    clast->to_scheme = c;
    clast++;
}

Define_Converter_To_C (name, c) char *name; PFX c; {
    Error_Tag = "scheme->c";
    if (clast == ctab+MAX_CONVERTER)
	Primitive_Error ("too many converters");
    clast->name = name;
    clast->scheme_to_c = 1;
    clast->to_c = c;
    clast++;
}

PFO Find_Converter_To_Scheme (name) char *name; {
    register CONVERTER *p;

    for (p = ctab; p < clast; p++)
	if (!p->scheme_to_c && streq (p->name, name))
	    return p->to_scheme;
    return 0;
}

PFX Find_Converter_To_C (name) char *name; {
    register CONVERTER *p;

    for (p = ctab; p < clast; p++)
	if (p->scheme_to_c && streq (p->name, name))
	    return p->to_c;
    return 0;
}
