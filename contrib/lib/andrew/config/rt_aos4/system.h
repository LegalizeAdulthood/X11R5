/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#ifndef	SYSTEM_H
#define	SYSTEM_H

/* Get common definitions */
#include <allsys.h>

#define	OPSYSNAME	"ibm032_4.3bsd"
#define	sys_rt_aos4	1
#define	SYS_NAME	"rt_aos4"
#ifndef	IBM032_ENV
#define	IBM032_ENV
#endif /* IBM032_ENV */

/* Here follow the overrides for this system. */
#undef	SY_B43
#define	SY_B43	1 /* This system is most like bsd 4.3 */

#define	NDBM_ENV    1

#ifndef In_Imake

/* Get major data types (esp. caddr_t) */
#include <sys/types.h>

/* Get open(2) constants */
#include <sys/file.h>

#include <strings.h>

/* Get struct timeval */
#include <sys/time.h>

/* include path for syslog.h */
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

#endif /* In_Imake */

/*
 * Put system-specific definitions here
 */

#define HAS_SYSEXITS 1

/* Now follow the site-specific customizations. */
#include <site.h>

#endif	/* SYSTEM_H */
