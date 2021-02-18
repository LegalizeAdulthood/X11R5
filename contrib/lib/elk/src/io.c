/* Ports
 */

#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>

#include "scheme.h"

#ifdef SYSCONF
#  include <unistd.h>
#endif

static Max_Open_Files;
static Object Open_Files[MAX_MAX_OPEN_FILES];

Object Curr_Input_Port, Curr_Output_Port;
Object Standard_Input_Port, Standard_Output_Port;

Object Make_Port();
void Close_Lost_Files();

Init_Io () {
    register Object *p;

#ifdef MAX_OFILES
    Max_Open_Files = MAX_OFILES;
#else
#ifdef SYSCONF
    Max_Open_Files = sysconf (_SC_OPEN_MAX);
#else
    Max_Open_Files = getdtablesize ();
#endif
#endif
    if (Max_Open_Files > MAX_MAX_OPEN_FILES)
	Max_Open_Files = MAX_MAX_OPEN_FILES;
    for (p = Open_Files; p < Open_Files+Max_Open_Files; p++)
	*p = Null;
    Standard_Input_Port = Make_Port (P_INPUT, stdin, Make_String ("stdin", 5));
    Standard_Output_Port = Make_Port (0, stdout, Make_String ("stdout", 6));
    Curr_Input_Port = Standard_Input_Port;
    Curr_Output_Port = Standard_Output_Port;
    Global_GC_Link (Standard_Input_Port);
    Global_GC_Link (Standard_Output_Port);
    Global_GC_Link (Curr_Input_Port);
    Global_GC_Link (Curr_Output_Port);
    Register_After_GC (Close_Lost_Files);
}

Reset_IO (destructive) {
    Discard_Input (Curr_Input_Port);
    if (destructive)
	Discard_Output (Curr_Output_Port);
    else
	Flush_Output (Curr_Output_Port);
    Curr_Input_Port = Standard_Input_Port;
    Curr_Output_Port = Standard_Output_Port;
}

Object Make_Port (flags, f, name) FILE *f; Object name; {
    Object port;
    register char *p;
    GC_Node;

    if (f && isatty (fileno (f)))
	flags |= P_TTY;
    GC_Link (name);
    p = Get_Bytes (sizeof (struct S_Port));
    SET(port, T_Port, (struct S_Port *)p);
    PORT(port)->flags = flags|P_OPEN;
    PORT(port)->file = f;
    PORT(port)->name = name;
    PORT(port)->ptr = 0;
    GC_Unlink;
    return port;
}

Object P_Port_File_Name (p) Object p; {
    Check_Type (p, T_Port);
    return (PORT(p)->flags & P_STRING) ? False : PORT(p)->name;
}

Object P_Eof_Objectp (x) Object x; {
    return TYPE(x) == T_End_Of_File ? True : False;
}

Object P_Curr_Input_Port () { return Curr_Input_Port; }

Object P_Curr_Output_Port () { return Curr_Output_Port; }

Object P_Input_Portp (x) Object x; {
    return TYPE(x) == T_Port && (PORT(x)->flags & P_INPUT) ? True : False;
}

Object P_Output_Portp (x) Object x; {
    return TYPE(x) == T_Port && !(PORT(x)->flags & P_INPUT) ? True : False;
}

void Close_Lost_Files () {
    register Object *p, *tag;

    for (p = Open_Files; p < Open_Files+Max_Open_Files; p++) {
	if (Nullp (*p)) continue;
	if (TYPE(*p) != T_Port)
	    Panic ("bad type in file table");
	tag = &PORT(*p)->name;
	if (TYPE(*tag) == T_Broken_Heart) {
	    SETPOINTER(*p, POINTER(*tag));
	} else {
	    fclose (PORT(*p)->file);
	    *p = Null;
	}
    }
}

Close_All_Files () {
    register Object *p;

    for (p = Open_Files; p < Open_Files+Max_Open_Files; p++) {
	if (Nullp (*p)) continue;
	fclose (PORT(*p)->file);
	PORT(*p)->flags &= ~P_OPEN;
	*p = Null;
    }
}

Register_File (port) Object port; {
    register Object *p;

    for (p = Open_Files; p < Open_Files+Max_Open_Files; p++)
	if (Nullp (*p)) break;
    if (p == Open_Files+Max_Open_Files)
	Primitive_Error ("no more slots for open files.\n");
    *p = port;
}

Object Get_File_Name (name) Object name; {
    register len;

    if (TYPE(name) == T_Symbol)
	name = SYMBOL(name)->name;
    else if (TYPE(name) != T_String)
	Wrong_Type_Combination (name, "string or symbol");
    if ((len = STRING(name)->size) > MAXPATHLEN || len == 0)
	Primitive_Error ("invalid file name");
    return name;
}

char *Internal_Tilde_Expand (s, dirp) register char *s, **dirp; {
    register char *p; 
    struct passwd *pw, *getpwnam();

    if (*s != '~')
	return 0;
    for (p = s+1; *p && *p != '/'; p++) ;
    *p = '\0';
    if (p == s+1) {
	if ((*dirp = getenv ("HOME")) == 0)
	    *dirp = "";
    } else {
	if ((pw = getpwnam (s+1)) == 0)
	    Primitive_Error ("unknown user: ~a", Make_String (s+1, p-s-1));
	*dirp = pw->pw_dir;
    } 
    return p;
}

Object General_File_Operation (s, op) Object s; register op; {
    register char *r;
    register n;
    Object ret, fn;
    Alloca_Begin;

    fn = Get_File_Name (s);
    n = STRING(fn)->size;
    Alloca (r, char*, n+1);
    bcopy (STRING(fn)->data, r, n);
    r[n] = '\0';
    switch (op) {
    case 0: {
	char *p, *dir;
	if ((p = Internal_Tilde_Expand (r, &dir)) == 0) {
	    Alloca_End;
	    return s;
	}
	Alloca (r, char*, strlen (dir) + 1 + strlen (p));
	sprintf (r, "%s/%s", dir, p+1);
	ret = Make_String (r, strlen (r));
	Alloca_End;
	return ret;
    }
    case 1: {
	struct stat st;
	/* Doesn't make much sense to check for errno != ENOENT here:
	 */
	ret = stat (r, &st) == 0 ? True : False;
	Alloca_End;
	return ret;
    }}
    /*NOTREACHED*/
}

Object P_Tilde_Expand (s) Object s; {
    return General_File_Operation (s, 0);
}

Object P_File_Existsp (s) Object s; {
    return General_File_Operation (s, 1);
}

Object Open_File (name, flags, err) register char *name; {
    register FILE *f;
    char *dir, *p;
    Object fn, port;
    struct stat st;
    Alloca_Begin;

    if (p = Internal_Tilde_Expand (name, &dir)) {
	Alloca (name, char*, strlen (dir) + 1 + strlen (p));
	sprintf (name, "%s/%s", dir, p+1);
    }
    if (!err && stat (name, &st) == -1 &&
	    (errno == ENOENT || errno == ENOTDIR)) {
	Alloca_End;
	return Null;
    }
    fn = Make_String (name, strlen (name));
    if ((f = fopen (name, (flags & P_INPUT) ? "r" : "w")) == NULL) {
	Saved_Errno = errno;  /* errno valid here? */
	Primitive_Error ("~s: ~E", fn);
    }
    port = Make_Port (flags, f, fn);
    Register_File (port);
    Alloca_End;
    return port;
}

Object General_Open_File (name, flags, path) Object name, path; {
    Object port, pref;
    register char *buf = 0, *fn;
    register plen, len, blen = 0, gotpath = 0;
    Alloca_Begin;

    name = Get_File_Name (name);
    len = STRING(name)->size;
    fn = STRING(name)->data;
    if (fn[0] != '/' && fn[0] != '~') {
	for ( ; TYPE(path) == T_Pair; path = Cdr (path)) {
	    pref = Car (path);
	    if (TYPE(pref) == T_Symbol)
		pref = SYMBOL(pref)->name;
	    if (TYPE(pref) != T_String)
		continue;
	    gotpath = 1;
	    if ((plen = STRING(pref)->size) > MAXPATHLEN || plen == 0)
		continue;
	    if (len + plen + 2 > blen) {
		blen = len + plen + 2;
		Alloca (buf, char*, blen);
	    }
	    bcopy (STRING(pref)->data, buf, plen);
	    if (buf[plen-1] != '/')
		buf[plen++] = '/';
	    bcopy (fn, buf+plen, len);
	    buf[len+plen] = '\0';
	    port = Open_File (buf, flags, 0);
	    /* No GC has been taken place in Open_File() if it returns Null.
	     */
	    if (!Nullp (port)) {
		Alloca_End;
		return port;
	    }
	}
    }
    if (gotpath)
	Primitive_Error ("file ~s not found", name);
    if (len + 1 > blen)
	Alloca (buf, char*, len + 1);
    bcopy (fn, buf, len);
    buf[len] = '\0';
    port = Open_File (buf, flags, 1);
    Alloca_End;
    return port;
}

Object P_Open_Input_File (name) Object name; {
    return General_Open_File (name, P_INPUT, Null);
}

Object P_Open_Output_File (name) Object name; {
    return General_Open_File (name, 0, Null);
}

Object P_Close_Port (port) Object port; {
    register Object *p;
    register flags;
    FILE *f;

    Check_Type (port, T_Port);
    flags = PORT(port)->flags;
    if (!(flags & P_OPEN) || (flags & P_STRING))
	return Void;
    f = PORT(port)->file;
    if (f == stdin || f == stdout)
	return Void;
    fclose (f);
    PORT(port)->flags &= ~P_OPEN;
    for (p = Open_Files; p < Open_Files+Max_Open_Files; p++)
	if (EQ(port,*p))
	    *p = Null;
    return Void;
}

#define General_With(prim,curr,flags) Object prim (name, thunk)\
	Object name, thunk; {\
    Object old, ret;\
    GC_Node2;\
\
    Check_Procedure (thunk);\
    old = curr;\
    GC_Link2 (thunk, old);\
    curr = General_Open_File (name, flags, Null);\
    ret = Funcall (thunk, Null, 0);\
    P_Close_Port (curr);\
    GC_Unlink;\
    curr = old;\
    return ret;\
}

General_With (P_With_Input, Curr_Input_Port, P_INPUT)
General_With (P_With_Output, Curr_Output_Port, 0)

Object General_Call_With (name, flags, proc) Object name, proc; {
    Object port, ret;
    GC_Node2;

    Check_Procedure (proc);
    GC_Link2 (proc, port);
    port = General_Open_File (name, flags, Null);
    port = Cons (port, Null);
    ret = Funcall (proc, port, 0);
    P_Close_Port (Car (port));
    GC_Unlink;
    return ret;
}

Object P_Call_With_Input (name, proc) Object name, proc; {
    return General_Call_With (name, P_INPUT, proc);
}

Object P_Call_With_Output (name, proc) Object name, proc; {
    return General_Call_With (name, 0, proc);
}

Object P_Open_Input_String (string) Object string; {
    Check_Type (string, T_String);
    return Make_Port (P_STRING|P_INPUT, (FILE *)0, string);
}

Object P_Open_Output_String () {
    return Make_Port (P_STRING, (FILE *)0, Make_String ((char *)0, 0));
}
