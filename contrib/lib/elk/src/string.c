#include <ctype.h>

#include "scheme.h"

char Char_Map[256];

Init_String () {
    register i;

    for (i = 0; i < 256; i++)
	Char_Map[i] = i;
    for (i = 'A'; i <= 'Z'; i++)
	Char_Map[i] = tolower (i);
}

Object General_Make_String (s, len, konst) char *s; {
    Object str;
    register char *p;

    if (konst)
	p = Get_Const_Bytes (len + sizeof (struct S_String) - 1);
    else
	p = Get_Bytes (len + sizeof (struct S_String) - 1);
    SET(str, T_String, (struct S_String *)p);
    if (konst)
	SETCONST(str);
    STRING(str)->tag = Null;
    STRING(str)->size = len;
    if (s)
	bcopy (s, STRING(str)->data, len);
    return str;
}

Object Make_String (s, len) char *s; {
    return General_Make_String (s, len, 0);
}

Object Make_Const_String (s, len) char *s; {
    return General_Make_String (s, len, 1);
}

Object P_Stringp (s) Object s; {
    return TYPE(s) == T_String ? True : False;
}

Object P_Make_String (argc, argv) Object *argv; {
    register len, c = ' ';
    Object str;
    register char *p;

    if ((len = Get_Integer (argv[0])) < 0)
	Range_Error (argv[0]);
    if (argc == 2) {
	Check_Type (argv[1], T_Character);
	c = CHAR(argv[1]);
    }
    str = Make_String ((char *)0, len);
    for (p = STRING(str)->data; len; len--) *p++ = c;
    return str;
}

Object P_String (argc, argv) Object *argv; {
    Object str;
    register i;

    str = Make_String ((char *)0, argc);
    for (i = 0; i < argc; i++) {
	Check_Type (argv[i], T_Character);
	STRING(str)->data[i] = CHAR(argv[i]);
    }
    return str;
}

Object P_String_To_Number (argc, argv) Object *argv; {
    Object ret;
    register char *b;
    register struct S_String *p;
    int radix = 10;
    Alloca_Begin;

    Check_Type (argv[0], T_String);
    if (argc == 2) {
	radix = Get_Integer (argv[1]);
	switch (radix) {
	case 2: case 8: case 10: case 16:
	    break;
	default:
	    Primitive_Error ("invalid radix: ~s", argv[1]);
	}
    }
    p = STRING(argv[0]);
    Alloca (b, char*, p->size+1);
    bcopy (p->data, b, p->size);
    b[p->size] = '\0';
    ret = Parse_Number (b, radix);
    Alloca_End;
    return Nullp (ret) ? False : ret;
}

Object P_String_Length (s) Object s; {
    Check_Type (s, T_String);
    return Make_Integer (STRING(s)->size);
}

Object P_String_Ref (s, n) Object s, n; {
    Check_Type (s, T_String);
    return Make_Char (STRING(s)->data[Get_Index (n, s)]);
}

Object P_String_Set (s, n, new) Object s, n, new; {
    register i, old;

    Check_Type (s, T_String);
    Check_Mutable (s);
    Check_Type (new, T_Character);
    old = STRING(s)->data[i = Get_Index (n, s)];
    STRING(s)->data[i] = CHAR(new);
    return Make_Char (old);
}

Object P_Substring (s, a, b) Object s, a, b; {
    register i, j;

    Check_Type (s, T_String);
    if ((i = Get_Integer (a)) < 0 || i > STRING(s)->size)
	Range_Error (a);
    if ((j = Get_Integer (b)) < 0 || j > STRING(s)->size)
	Range_Error (b);
    if (i > j)
	Primitive_Error ("`end' less than `start'");
    return Make_String (&STRING(s)->data[i], j-i);
}

Object P_String_Copy (s) Object s; {
    Check_Type (s, T_String);
    return Make_String (STRING(s)->data, STRING(s)->size);
}

Object P_String_Append (argc, argv) Object *argv; {
    register i, len;
    Object s, str;

    for (len = i = 0; i < argc; i++) {
	Check_Type (argv[i], T_String);
	len += STRING(argv[i])->size;
    }
    str = Make_String ((char *)0, len);
    for (len = i = 0; i < argc; i++) {
	s = argv[i];
	bcopy (STRING(s)->data, &STRING(str)->data[len], STRING(s)->size);
	len += STRING(s)->size;
    }
    return str;
}

Object P_List_To_String (list) Object list; {
    Object str, len;
    register i;
    GC_Node;

    GC_Link (list);
    len = P_Length (list);
    str = Make_String ((char *)0, FIXNUM(len));
    for (i = 0; i < FIXNUM(len); i++, list = Cdr (list)) {
	Check_Type (Car (list), T_Character);
	STRING(str)->data[i] = CHAR(Car (list));
    }
    GC_Unlink;
    return str;
}

Object P_String_To_List (s) Object s; {
    register i;
    Object list, tail, cell;
    GC_Node3;

    Check_Type (s, T_String);
    list = tail = Null;
    GC_Link3 (s, list, tail);
    for (i = 0; i < STRING(s)->size; i++, tail = cell) {
	cell = Cons (Make_Char (STRING(s)->data[i]), Null);
	if (Nullp (list))
	    list = cell;
	else
	    P_Setcdr (tail, cell);
    }
    GC_Unlink;
    return list;
}

Object P_Substring_Fill (s, a, b, c) Object s, a, b, c; {
    register i, j;

    Check_Type (s, T_String);
    Check_Mutable (s);
    Check_Type (c, T_Character);
    i = Get_Index (a, s);
    if ((j = Get_Integer (b)) < 0 || j > STRING(s)->size)
	Range_Error (b);
    if (i > j)
	Primitive_Error ("`end' less than `start'");
    while (i < j)
	STRING(s)->data[i++] = CHAR(c);
    return s;
}

Object P_String_Fill (s, c) Object s, c; {
    Object ret;
    GC_Node2;

    Check_Type (s, T_String);
    Check_Mutable (s);
    GC_Link2 (s, c);
    ret = P_Substring_Fill (s, Make_Fixnum (0), 
	Make_Integer (STRING(s)->size), c);
    GC_Unlink;
    return ret;
}

Object General_Substringp (s1, s2, ci) Object s1, s2; register ci; {
    register n, l1, l2;
    register char *p1, *p2, *p3, *map;

    Check_Type (s1, T_String);
    Check_Type (s2, T_String);
    l1 = STRING(s1)->size;
    l2 = STRING(s2)->size;
    map = Char_Map;
    for (p2 = STRING(s2)->data; l2 >= l1; p2++, l2--) {
	for (p1 = STRING(s1)->data, p3 = p2, n = l1; n; n--, p1++, p3++) {
	    if (ci) {
		if (map[*p1] != map[*p3]) goto fail;
	    } else
		if (*p1 != *p3) goto fail;
	}
	return Make_Integer (STRING(s2)->size - l2);
fail:   ;
    }
    return False;
}

Object P_Substringp (s1, s2) Object s1, s2; {
    return General_Substringp (s1, s2, 0);
}

Object P_CI_Substringp (s1, s2) Object s1, s2; {
    return General_Substringp (s1, s2, 1);
}

General_Strcmp (s1, s2, ci) Object s1, s2; register ci; {
    register n, l1, l2;
    register char *p1, *p2, *map;

    Check_Type (s1, T_String);
    Check_Type (s2, T_String);
    l1 = STRING(s1)->size; l2 = STRING(s2)->size;
    n = l1 > l2 ? l2 : l1;
    p1 = STRING(s1)->data; p2 = STRING(s2)->data;
    for (map = Char_Map; --n >= 0; p1++, p2++) {
	if (ci) {
	    if (map[*p1] != map[*p2]) break;
	} else
	    if (*p1 != *p2) break;
    }
    if (n < 0)
	return l1 - l2;
    return *p1 - *p2;
}

Object P_Str_Eq (s1, s2) Object s1, s2; {
    return General_Strcmp (s1, s2, 0) ? False : True;
}

Object P_Str_Less (s1, s2) Object s1, s2; {
    return General_Strcmp (s1, s2, 0) < 0 ? True : False;
}

Object P_Str_Greater (s1, s2) Object s1, s2; {
    return General_Strcmp (s1, s2, 0) > 0 ? True : False;
}

Object P_Str_Eq_Less (s1, s2) Object s1, s2; {
    return General_Strcmp (s1, s2, 0) <= 0 ? True : False;
}

Object P_Str_Eq_Greater (s1, s2) Object s1, s2; {
    return General_Strcmp (s1, s2, 0) >= 0 ? True : False;
}

Object P_Str_CI_Eq (s1, s2) Object s1, s2; {
    return General_Strcmp (s1, s2, 1) ? False : True;
}

Object P_Str_CI_Less (s1, s2) Object s1, s2; {
    return General_Strcmp (s1, s2, 1) < 0 ? True : False;
}

Object P_Str_CI_Greater (s1, s2) Object s1, s2; {
    return General_Strcmp (s1, s2, 1) > 0 ? True : False;
}

Object P_Str_CI_Eq_Less (s1, s2) Object s1, s2; {
    return General_Strcmp (s1, s2, 1) <= 0 ? True : False;
}

Object P_Str_CI_Eq_Greater (s1, s2) Object s1, s2; {
    return General_Strcmp (s1, s2, 1) >= 0 ? True : False;
}
