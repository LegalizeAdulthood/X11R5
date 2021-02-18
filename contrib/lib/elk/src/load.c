/* ECOFF support written by George Hartzell
 * MACH-O support based on code by Chris Maeda (cmaeda@cs.cmu.edu)
 */

#include "scheme.h"

#ifdef CAN_LOAD_OBJ
#ifdef COFF
#  include <filehdr.h>
#  include <syms.h>
#  undef TYPE         /* ldfnc.h defines a TYPE macro. */
#  include <ldfcn.h>
#  undef TYPE
#  define TYPE(x) ((int)((x) >> TYPESHIFT))
#else
#ifdef ECOFF
#  include <filehdr.h>
#  include <aouthdr.h>
#  include <scnhdr.h>
#  include <syms.h>
#else
#ifdef MACH_O
#  include <rld.h>
#else
#  include <a.out.h>
#  include <sys/types.h>
#endif
#endif
#endif
#endif

static Object V_Load_Path, V_Load_Noisilyp, V_Load_Libraries;

#ifdef CAN_LOAD_OBJ
#  define Default_Load_Libraries "-lc"
#else
#  define Default_Load_Libraries ""
#endif

#if defined(CAN_DUMP) || defined(CAN_LOAD_OBJ)
char Loader_Input[20];
#endif

#ifdef CAN_LOAD_OBJ
static char Loader_Output[20];
#ifdef ECOFF
struct headers {
    struct filehdr fhdr;
    struct aouthdr aout;
    struct scnhdr section[3];    
};
#endif
#endif

Init_Load () {
    Define_Variable (&V_Load_Path, "load-path",
	Cons (Make_String (".", 1),
	Cons (Make_String (SCM_DIR, sizeof (SCM_DIR) - 1),
	Cons (Make_String (LIB_DIR, sizeof (LIB_DIR) - 1), Null))));
    Define_Variable (&V_Load_Noisilyp, "load-noisily?", False);
    Define_Variable (&V_Load_Libraries, "load-libraries", 
	Make_String (Default_Load_Libraries, sizeof Default_Load_Libraries-1));
}

Object General_Load (name, env) Object name, env; {
    register char *p;
    register struct S_String *str;
    Object oldenv, port;
    GC_Node2;

    Check_Type (env, T_Environment);
    oldenv = The_Environment;
    GC_Link2 (env, oldenv);
    port = General_Open_File (name, P_INPUT, Val (V_Load_Path));
    str = STRING(PORT(port)->name);
    Switch_Environment (env);
    p = str->data + str->size;
    if (str->size >= 2 && *--p == 'o' && *--p == '.') {
#ifdef CAN_LOAD_OBJ
	Load_Object (port, str);
#else
	;
#endif
    } else
	Load_Source (port);
    Switch_Environment (oldenv);
    GC_Unlink;
    return Void;
}

Object P_Load (argc, argv) register argc; register Object *argv; {
    return General_Load (argv[0], argc == 1 ? The_Environment : argv[1]);
}

Load_Source (port) Object port; {
    Object val;
    GC_Node;

    GC_Link (port);
    while (1) {
	val = General_Read (port, 1);
	if (TYPE(val) == T_End_Of_File)
	    break;
	val = Eval (val);
	if (Truep (Val (V_Load_Noisilyp))) {
	    Print (val);
	    P_Newline (0);
	}
    }
    P_Close_Port (port);
    GC_Unlink;
}

#ifdef CAN_LOAD_OBJ

#ifdef MACH_O
Load_Object (port, fn) Object port; register struct S_String *fn; {
    long retval;
    struct mach_header *hdr;
    char *object_filenames[2];
    NXStream *err_stream;

    P_Close_Port (port);

    Disable_Interrupts;
    /* Construct a stream for error logging:
     */
    if ((err_stream = NXOpenFile (fileno (stderr), NX_WRITEONLY)) == 0)
	Primitive_Error ("NXOpenFile failed");

    object_filenames[0] = fn->data;
    object_filenames[1] = 0;

    retval = rld_load (err_stream, /* report error messages here */
	&hdr,                      /* return header address here */
	object_filenames,          /* load these */
	"/dev/null");              /* doesn't work if NULL?! */
    NXClose (err_stream);
    if (retval != 1)
	Primitive_Error ("rld_load() failed");

    /* Grab the symbol table from the just-loaded file:
     */
    if (The_Symbols)
	Free_Symbols (The_Symbols);
    The_Symbols = Snarf_Symbols (hdr);
    if (!Call_Initializers (The_Symbols, 0))
	Primitive_Error ("no initializers");
    Enable_Interrupts;
}

#else /* MACH_O */

Load_Object (port, fn) Object port; register struct S_String *fn; {
#ifdef ECOFF
    struct headers hdr;
#else
    struct exec hdr;
#endif
    register char *brk, *obrk, *buf, *lp, *li;
    register n, f;
    Object libs;
    FILE *fp;
    Alloca_Begin;

    n = fread ((char *)&hdr, sizeof (hdr), 1, PORT(port)->file);
    P_Close_Port (port);
#ifdef ECOFF
    if (n == 0 ||
	(hdr.fhdr.f_magic != MIPSELMAGIC && hdr.fhdr.f_magic != MIPSEBMAGIC))
#else
#ifdef hp9000s300
    if (n == 0 || N_BADMAG(hdr))
#else
    if (n == 0 || hdr.a_magic != OMAGIC)
#endif
#endif
	Primitive_Error ("not a valid object file");
    strcpy (Loader_Output, "/tmp/ldXXXXXX");
    mktemp (Loader_Output);
    Alloca (buf, char*, fn->size + strlen (A_Out_Name) + 500);
    obrk = brk = sbrk (0);
    brk = (char *)((int)brk + 7 & ~7);
    libs = Val (V_Load_Libraries);
    if (TYPE(libs) == T_String) {
        if ((n = STRING(libs)->size) > 400)
	    Primitive_Error ("too many load libraries");
	lp = STRING(libs)->data;
    } else {
	lp = "-lc"; n = 3;
    }
    li = Loader_Input;
    if (li[0] == 0)
	li = A_Out_Name;
#ifdef XFLAG_BROKEN
    sprintf (buf, "/bin/ld -N -A %s -T %x %.*s -o %s %.*s",
#else
#ifdef hp9000s300
    sprintf (buf, "/bin/ld -N -x -A %s -R %x %.*s -o %s %.*s",
#else
#ifdef USE_GNULD
    sprintf (buf, "/usr/local/lib/gcc-ld -N -x -A %s -T %x %.*s -o %s %.*s",
#else
    sprintf (buf, "/bin/ld -N -x -A %s -T %x %.*s -o %s %.*s",
#endif
#endif
#endif
	li, brk, fn->size, fn->data, Loader_Output, n, lp);
    if (Verbose)
	printf ("[%s]\n", buf);
    if (system (buf) != 0) {
	(void)unlink (Loader_Output);
	Primitive_Error ("system linker failed");
    }
    Disable_Interrupts;               /* To ensure that f gets closed */
    if ((f = open (Loader_Output, 0)) == -1) {
	(void)unlink (Loader_Output);
	Primitive_Error ("cannot open tempfile");
    }
    if (Loader_Input[0])
	(void)unlink(Loader_Input);
    strcpy (Loader_Input, Loader_Output);
    if (read (f, (char *)&hdr, sizeof (hdr)) != sizeof (hdr)) {
err:
	close (f);
	Primitive_Error ("corrupt tempfile (/bin/ld is broken)");
    }
#ifdef ECOFF
    n = hdr.aout.tsize + hdr.aout.dsize + hdr.aout.bsize;
#else
    n = hdr.a_text + hdr.a_data + hdr.a_bss;
#endif
    n += brk - obrk;
    if (sbrk (n) == (char *)-1) {
	close (f);
	Primitive_Error ("not enough memory to load object file");
    }
    bzero (obrk, n);
#ifdef ECOFF
    n -= hdr.aout.bsize;
    (void)lseek (f, (long)hdr.section[0].s_scnptr, 0);
#else
    n -= hdr.a_bss;
#endif
    if (read (f, brk, n) != n)
	goto err;
    if ((fp = fdopen (f, "r")) == NULL) {
	close (f);
	Primitive_Error ("cannot fdopen object file");
    }
    if (The_Symbols)
	Free_Symbols (The_Symbols);
    The_Symbols = Snarf_Symbols (fp, &hdr);
    fclose (fp);
    Alloca (buf, char*, fn->size + 50);
    if (!Call_Initializers (The_Symbols, brk)) {
	sprintf (buf, "no initializers in %.*s", fn->size, fn->data);
	Primitive_Error (buf);
    }
    Alloca_End;
    Enable_Interrupts;
}

Finit_Load () {
    if (Loader_Input[0])
	(void)unlink (Loader_Input);
}
#endif /* MACH_O */

#endif /* CAN_LOAD_OBJ */
