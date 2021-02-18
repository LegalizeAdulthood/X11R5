#include <scheme.h>

extern char *getenv();
Object V_Home;

static Object P_Chdir (argc, argv) Object *argv; {
    Object dir;
    register n;
    register char *s;
    Alloca_Begin;

    dir = argc == 0 ? Val (V_Home) : argv[0];
    Check_Type (dir, T_String);
    n = STRING(dir)->size;
    Alloca (s, char*, n+1);
    bcopy (STRING(dir)->data, s, n);
    s[n] = '\0';
    if (chdir (s) < 0) {
	Saved_Errno = errno;
	Primitive_Error ("~s: ~E", dir);
    }
    Alloca_End;
    return Void;
}

init_lib_chdir () {
    register char *p = getenv ("HOME");

    if (p == 0)
	p = ".";
    Define_Variable (&V_Home, "home", Make_String (p, strlen (p)));
    Define_Primitive (P_Chdir, "chdir", 0, 1, VARARGS);
}
