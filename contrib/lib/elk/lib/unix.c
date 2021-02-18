#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>

#include <scheme.h>
#include "util/string.h"

#ifdef DIRENT
#  include <dirent.h>
#else
#  include <sys/dir.h>
#endif

#ifdef SYSCONF
#  include <unistd.h>
#endif

extern char *getenv();

static Object P_Read_Directory (name) Object name; {
    register char *s;
    register DIR *d;
#ifdef DIRENT
    register struct dirent *dp;
#else
    register struct direct *dp;
#endif
    Object ret;
    GC_Node;
    Declare_C_Strings;

    ret = Null;
    GC_Link (ret);
    Make_C_String (name, s);
    Disable_Interrupts;
    if ((d = opendir (s)) == NULL)
	Primitive_Error ("cannot open directory ~s", name);
    while ((dp = readdir (d)) != NULL) {
	Object x = Make_String (dp->d_name, strlen (dp->d_name));
	ret = Cons (x, ret);
    }
    closedir (d);
    Enable_Interrupts;
    GC_Unlink;
    Dispose_C_Strings;
    return ret;
}

static Object P_File_Status (name) Object name; {
    register char *s;
    Object ret;
    struct stat st;
    Declare_C_Strings;

    Make_C_String (name, s);
    if (stat (s, &st) == -1) {
	switch (errno) {
	case ENOTDIR:
	case EINVAL:
	case ENOENT:
	case EACCES:
#ifdef ENAMETOOLONG
	case ENAMETOOLONG:
#endif
#ifdef ELOOP
	case ELOOP:
#endif
	    s = "non-existent"; break;
	default:
	    Saved_Errno = errno;
	    Primitive_Error ("cannot stat ~s: ~E", name);
	}
    } else {
	switch (st.st_mode & S_IFMT) {
	case S_IFDIR: s = "directory"; break;
	case S_IFCHR: s = "character-special"; break;
	case S_IFBLK: s = "block-special"; break;
	case S_IFREG: s = "regular"; break;
#ifdef S_IFSOCK
	case S_IFSOCK: s = "socket"; break;
#endif
#ifdef S_IFFIFO
	case S_IFFIFO: s = "fifo"; break;
#endif
	default: s = "unknown"; break;
	}
    }
    ret = Intern (s);
    Dispose_C_Strings;
    return ret;
}

static Object P_System (cmd) Object cmd; {
    register char *s;
    register i, n, pid;
    int status;
    Declare_C_Strings;

    Make_C_String (cmd, s);
#ifdef VFORK
    switch (pid = vfork ()) {
#else
    switch (pid = fork ()) {
#endif
    case -1:
	Saved_Errno = errno;
	Primitive_Error ("cannot fork: ~E");
    case 0:
#ifdef MAX_OFILES
	n = MAX_OFILES;
#else
#ifdef SYSCONF
	n = sysconf (_SC_OPEN_MAX);
#else
	n = getdtablesize ();
#endif
#endif
	for (i = 3; i < n; i++)
	    (void)close (i);
	execl ("/bin/sh", "sh", "-c", s, (char *)0);
	_exit (127);
    default:
	Disable_Interrupts;
	while ((i = wait (&status)) != pid && i != -1)
		;
	Enable_Interrupts;
    }
    Dispose_C_Strings;
    if (i == -1)
	return False;
    if (n = (status & 0377))
	return Cons (Make_Fixnum (n), Null);
    return Make_Fixnum ((status >> 8) & 0377);
}

static Object P_Getenv (e) Object e; {
    register char *s;
    Object ret;
    Declare_C_Strings;

    Make_C_String (e, s);
    ret = (s = getenv (s)) ? Make_String (s, strlen (s)) : False;
    Dispose_C_Strings;
    return ret;
}

init_lib_unix () {
    Define_Primitive (P_Read_Directory, "read-directory", 1, 1, EVAL);
    Define_Primitive (P_File_Status,    "file-status",    1, 1, EVAL);
    Define_Primitive (P_System,         "system",         1, 1, EVAL);
    Define_Primitive (P_Getenv,         "getenv",         1, 1, EVAL);
    P_Provide (Intern ("unix"));
}
