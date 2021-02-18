#include <scheme.h>

static Object P_String_Reverse (str) Object str; {
    register char c, *s, *t;

    Check_Type (str, T_String);
    for (s = STRING(str)->data, t = s+STRING(str)->size; --t > s; s++)
	c = *s, *s = *t, *t = c;
    return str;
}

init_lib_string () {
    Define_Primitive (P_String_Reverse, "string-reverse!", 1, 1, EVAL);
}
