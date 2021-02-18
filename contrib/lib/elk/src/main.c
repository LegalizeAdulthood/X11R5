#include "scheme.h"

#ifdef UNISTD
#  include <unistd.h>
#endif
#include TIME_H
#ifndef STACK_SIZE
#  include <sys/resource.h>
#endif
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/file.h>

char *stkbase;
int Max_Stack;
int Interpreter_Initialized;
int GC_Debug = 0;
int Bug_Compat;
int Verbose;

char **Argv;
int Argc, First_Arg;

#if defined(CAN_LOAD_OBJ) || defined(CAN_DUMP) || defined(INIT_OBJECTS)
char *A_Out_Name;
char *Find_Executable();
#endif

#if defined(CAN_LOAD_OBJ) || defined(INIT_OBJECTS)
SYMTAB *The_Symbols;
#endif

#ifdef CAN_DUMP
static char *original_stkbase;
int Was_Dumped;

main (ac, av) char **av; {
    char foo;

    if (Was_Dumped) {
	(void)alloca (INITIAL_STK_OFFSET - (original_stkbase - &foo));
    } else {
	original_stkbase = &foo;
	(void)alloca (INITIAL_STK_OFFSET);
    }
    Main (ac, av);
    /*NOTREACHED*/
}

Main (ac, av) char **av; {
#else
main (ac, av) char **av; {
#endif
    register char *loadfile = 0;
    register debug = 0, heap = HEAP_SIZE;
    Object file;
    char foo;

    if (ac == 0)
	Usage ();
    Get_Stack_Limit ();

#if defined(CAN_LOAD_OBJ) || defined(CAN_DUMP) || defined(INIT_OBJECTS)
    A_Out_Name = Find_Executable (av[0]);
#endif

    Argc = ac; Argv = av;
    First_Arg = 1;
#ifdef CAN_DUMP
    if (Was_Dumped) {
	char *tmp = &foo;
	ALIGN(tmp);
	if (stkbase != tmp)
	    Panic ("stack base");
	Loader_Input[0] = '\0';
	(void)signal (SIGINT, Intr_Handler);
	(void)Funcall_Control_Point (Dump_Control_Point, Arg_True, 0);
	/*NOTREACHED*/
    }
#endif

    for ( ; First_Arg < ac; First_Arg++) {
	if (strcmp (av[First_Arg], "-g") == 0) {
	    debug = 1;
	} else if (strcmp (av[First_Arg], "-bc") == 0) {
	    Bug_Compat = 1;
	} else if (strcmp (av[First_Arg], "-v") == 0) {
	    Verbose = 1;
	} else if (strcmp (av[First_Arg], "-h") == 0) {
	    if (++First_Arg == ac)
		Usage ();
	    heap = atoi (av[First_Arg]);
	} else if (strcmp (av[First_Arg], "-l") == 0) {
	    if (++First_Arg == ac || loadfile)
		Usage ();
	    loadfile = av[First_Arg];
	} else if (strcmp (av[First_Arg], "--") == 0) {
	    First_Arg++;
	    break;
	} else if (av[First_Arg][0] == '-') {
	    Usage ();
	} else {
	    break;
	}
    }

    stkbase = &foo;
    ALIGN(stkbase);
    Make_Heap (heap);
    Init_Everything ();
#ifdef INIT_OBJECTS
    if (Should_Init_Objects ()) {
	Error_Tag = "init-objects";
	The_Symbols = Open_File_And_Snarf_Symbols (A_Out_Name);
	Call_Initializers (The_Symbols, (char *)0);
    }
#endif
    (void)signal (SIGINT, Intr_Handler);
    Error_Tag = "top-level";
    if (loadfile == 0)
	loadfile = "toplevel";
    file = Make_String (loadfile, strlen (loadfile));
    Interpreter_Initialized = 1;
    GC_Debug = debug;
    if (loadfile[0] == '-' && loadfile[1] == '\0')
	Load_Source (Standard_Input_Port);
    else
	(void)General_Load (file, The_Environment);
    exit (0);
}

Usage () {
    fprintf (stderr, 
	"Use: %s [-l -|file] [-h KBytes] [-g] [-v] [-bc] [[--] args]\n",
	Argv[0]);
    exit (1);
}

Init_Everything () {
    Init_String ();
    Init_Symbol ();
    Init_Env();
    Init_Error ();
    Init_Io ();
    Init_Prim();
    Init_Math ();
    Init_Print ();
    Init_Auto ();
    Init_Heap ();
    Init_Load ();
    Init_Proc ();
    Init_Special ();
    Init_Read ();
    Init_Features ();
#ifdef CAN_DUMP
    Init_Dump ();
#endif
}

Get_Stack_Limit () {
#ifdef STACK_SIZE
    Max_Stack = STACK_SIZE;
#else
    struct rlimit rl;

    if (getrlimit (RLIMIT_STACK, &rl) == -1) {
	perror ("getrlimit");
	exit (1);
    }
    Max_Stack = rl.rlim_cur;
#endif
    Max_Stack -= STACK_MARGIN;
}

#if defined(CAN_LOAD_OBJ) && !defined(MACH_O)
exit (n) {
    Finit_Load ();
    _cleanup ();
    _exit (n);
}
#endif

#if defined(CAN_LOAD_OBJ) || defined(CAN_DUMP) || defined(INIT_OBJECTS)
Executable (fn) char *fn; {
    struct stat s;

    return stat (fn, &s) != -1 && (s.st_mode & S_IFMT) == S_IFREG
	    && access (fn, X_OK) != -1;
}

char *Find_Executable (fn) char *fn; {
    char *path, *getenv();
    static char buf[MAXPATHLEN+1];
    register char *p;

    for (p = fn; *p; p++) {
	if (*p == '/') {
	    if (Executable (fn))
		return fn;
	    else
		Fatal_Error ("%s is not executable", fn);
	}
    }
    if ((path = getenv ("PATH")) == 0)
	path = ":/usr/ucb:/bin:/usr/bin";
    do {
	p = buf;
	while (*path && *path != ':')
	    *p++ = *path++;
	if (*path)
	    ++path;
	if (p > buf)
	    *p++ = '/';
	strcpy (p, fn);
	if (Executable (buf))
	    return buf;
    } while (*path);
    Fatal_Error ("cannot find pathname of %s", fn);
    /*NOTREACHED*/
}
#endif

Object P_Command_Line_Args () {
    Object ret, tail;
    register i;
    GC_Node2;

    ret = tail = P_Make_List (Make_Fixnum (Argc-First_Arg), Null);
    GC_Link2 (ret, tail);
    for (i = First_Arg; i < Argc; i++, tail = Cdr (tail)) {
	Object a = Make_String (Argv[i], strlen (Argv[i]));
	Car (tail) = a;
    }
    GC_Unlink;
    return ret;
}

Object P_Exit (argc, argv) Object *argv; {
    exit (argc == 0 ? 0 : Get_Integer (argv[0]));
    /*NOTREACHED*/
}

#ifdef INIT_OBJECTS
Should_Init_Objects () {
    register char *s, *p;

    for (p = A_Out_Name + strlen (A_Out_Name), s = "emehcs"; *s; )
	if (--p < A_Out_Name || *p != *s++) return 1;
    return !(--p < A_Out_Name || *p == '/');
}
#endif
