/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#ifndef	SYSTEM_H
#define	SYSTEM_H

/* Get common definitions */
#include <allsys.h>

#define	OPSYSNAME	"ULTRIX_4.3bsd"
#define	sys_pmax_ul4	1
#define	SYS_NAME	"pmax_ul4"
#define	PMAX_ENV

/* Here follow the overrides for this system. */
#undef	SY_B43
#define	SY_B43	1 /* This system is most like bsd 4.3 */

#ifndef In_Imake

/* Get major data types (esp. caddr_t) */
#include <sys/types.h>

#include <strings.h>

/* Get open(2) constants */
#include <sys/file.h>

/* Get struct timeval */
#ifdef SYSV
#include <time.h>
# ifdef mips
# include <bsd/sys/time.h>
# endif /* mips */
#else /* else not SYSV */
#include <sys/time.h>
#endif /* SYSV */

/* include path for syslog.h BSD vs SYSV */
#include <syslog.h>

/* VMUNIX vs. SY_B4x */
#ifndef VMUNIX
#define	VMUNIX	1
#endif VMUNIX

#define OSI_HAS_SYMLINKS 1
#define osi_readlink(PATH,BUF,SIZE) readlink((PATH),(BUF),(SIZE))

#define osi_ExclusiveLockNoBlock(fid)	flock((fid), LOCK_EX | LOCK_NB)
#define osi_UnLock(fid)			flock((fid), LOCK_UN)
#define osi_O_READLOCK			O_RDONLY
#define osi_F_READLOCK			"r"

#define	osi_vfork()			vfork()

#define	osi_setjmp  _setjmp
#define	osi_longjmp _longjmp

/* Make a time standard. */
struct osi_Times {unsigned long int Secs; unsigned long int USecs;};
/* Set one of the above with a call to osi_GetTimes(&foo) */
#define osi_GetSecs() time((long int *) 0)
extern void osi_SetZone();
extern char *osi_ZoneNames[];
extern long int osi_SecondsWest;
extern int osi_IsEverDaylight;

/*
 * Put system-specific definitions here
 */

#ifdef mips
# ifdef SYSTYPE_SYSV
# include <bsd/sys/ioctl.h>
# include <bsd/sys/file.h>
# endif /* SYSTYPE_SYSV */
#endif /* mips */

#define HAS_SYSEXITS 1

#endif /* !In_Imake */

/* Now follow the site-specific customizations. */
#include <site.h>

#endif	/* SYSTEM_H */
