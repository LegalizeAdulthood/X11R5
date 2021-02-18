/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *globals_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/class/machdep/sgi_mips/RCS/globals.c,v 1.2 1991/10/03 15:58:56 bobg Exp $";

/* 
 * array of global entry points for dynamic loader
 *
 * Defines a table of entry points and their names
 *
 */

/* macros which actually define entry points */

/*   globalref - direct reference to global symbol x */

#define globalref(x) extern long x[];
#define globalarray(x) extern long x[];
#define routine(x) extern void x();

        extern struct basicobject_methods class_RoutineStruct;
/*	globalref(class_RoutineStruct) */
	globalref(class_Error)

	routine(class_NewObject)
	routine(class_Load)
	routine(class_IsLoaded)
	routine(class_Lookup)
	routine(class_IsType)
	routine(class_IsTypeByName)
	routine(class_EnterInfo)
	routine(class_SetClassPath)
	routine(class_PrependClassPath)
	routine(class_GetEText)
	routine(class_GetTextBase)
	routine(class_GetTextLength)


/* common symbols referred to but not defined directly in libc.a */

	globalref(environ)		/* common symbol, defined nowhere */
	globalref(errno)		/* cerror */

	globalref(_sproced)
	globalref(_us_rsthread_stdio)

/* do not delete the following line - make depends on it */

#include	<globalrefs._h>

#undef globalref
#undef globalarray
#undef routine

#ifdef __STDC__
#define globalref(x) (long) &x, #x,
#define globalarray(x) (long) x, #x,

#define routine(x) (long) x, #x,
#else
#define globalref(x) (long) &x, "x",
#define globalarray(x) (long) x, "x",

#define routine(x) (long) x, "x",
#endif

struct globaltab {
    long entrypoint;	/* entry point value */
    char *entryname;	/* symbolic name */
} globals[] = {

/* from libclass */
	globalref(class_RoutineStruct)
	globalref(class_Error)

	routine(class_NewObject)
	routine(class_Load)
	routine(class_IsLoaded)
	routine(class_Lookup)
	routine(class_IsType)
	routine(class_IsTypeByName)
	routine(class_EnterInfo)
	routine(class_SetClassPath)
	routine(class_PrependClassPath)
	routine(class_GetEText)
	routine(class_GetTextBase)
	routine(class_GetTextLength)


/* common symbols referred to but not defined directly in libc.a */

	globalref(environ)		/* common symbol, defined nowhere */
	globalref(errno)		/* cerror */

	globalref(_sproced)
	globalref(_us_rsthread_stdio)

/* do not delete the following line - make depends on it */

#include	<globalrefs._h>

#if 0
	.text

	.globl	globalend
globalend:    .space 0
#else
};

long globalcount = sizeof(globals) / sizeof(struct globaltab);
#endif
