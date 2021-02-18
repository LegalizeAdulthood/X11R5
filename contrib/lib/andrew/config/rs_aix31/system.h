/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#ifndef	SYSTEM_H
#define	SYSTEM_H
 
/* Get common definitions */
 
#include <allsys.h>
 
#define	OPSYSNAME	"risc_aix31"
#define	sys_rt_aix3	1
#define	SYS_NAME	"rs_aix31"
 
#ifndef _IBMR2
#define _IBMR2 1
#endif /* _IBMR2 */ 

/* These are here for AIX support. */
 
#undef SY_AIX31
#define SY_AIX31 1	/* define for AIX 3.1 */
#undef POSIX_ENV
#define POSIX_ENV 1	/* This is a Posix system. */

#ifndef In_Imake

#undef _ANSI_C_SOURCE
#include <stdlib.h>
#define _ANSI_C_SOURCE

#include <sys/types.h>

#undef _ANSI_C_SOURCE
#include <strings.h>
#define _ANSI_C_SOURCE

/* Get open(2) constants */
#include <sys/file.h>

/* Get struct timeval */
#include <time.h>
#include <sys/time.h>

/* include path for syslog.h */
#include <sys/syslog.h>

#define OSI_HAS_SYMLINKS 1
/* If OSI_HAS_SYMLINKS is not defined, osi_readlink is present in libutil. */
#define osi_readlink(PATH,BUF,SIZE) readlink((PATH),(BUF),(SIZE))
#include <sys/flock.h>
#define osi_ExclusiveLockNoBlock(fid)	flock((fid),LOCK_EX|LOCK_NB)
#define osi_UnLock(fid)			flock((fid),LOCK_UN)
#define osi_O_READLOCK			O_RDWR
#define osi_F_READLOCK			"r+"

/* handle (BSD) vfork for (AIX) which only knows fork */
#define	osi_vfork()			fork()

/* Handle the absence of _setjmp and _longjmp on AIX. */
#define	osi_setjmp  setjmp
#define	osi_longjmp longjmp

/* Make a time standard. */
struct osi_Times {unsigned long int Secs; unsigned long int USecs;};
/* Set one of the above with a call to osi_GetTimes(&foo) */
#define osi_GetSecs() time((long int *) 0)
#define osi_SetZone() tzset()
#define osi_ZoneNames tzname
#define osi_SecondsWest timezone
#define osi_IsEverDaylight daylight

/* More BSD-isms */
#define setreuid(r,e) setuid(r)
#define setlinebuf(file) setvbuf(file, NULL, _IOLBF, BUFSIZ)

/*
 * Put system-specific definitions here
 */
#define HAS_SYSEXITS 1
#define BIT_ZERO_ON_LEFT 1

#endif /* !In_Imake */

/* This system needs the customized ``install'' program. */
 
#define	BUILDANDREWINSTALL_ENV	1
#define GETDOMAIN_ENV 1
/* Things that aren't yet handled */
 
#ifdef ANDREW_MALLOC_ENV
#undef ANDREW_MALLOC_ENV
#endif /* ANDREW_MALLOC_ENV */

#ifdef DEBUG_MALLOC_ENV
#undef DEBUG_MALLOC_ENV
#endif /* DEBUG_MALLOC_ENV */

/* Now follow the site-specific customizations. */
 
#include <site.h>

#endif	/* SYSTEM_H */
 
