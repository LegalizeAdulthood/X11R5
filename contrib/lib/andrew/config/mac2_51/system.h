/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#ifndef	SYSTEM_H
#define	SYSTEM_H

/* Get common definitions */
#include <allsys.h>

#define	OPSYSNAME	"MAC2_mach"
#define	sys_mac2_51	1
#define	SYS_NAME	"mac2_51"
#define	MACH_ENV

/* Here follow the overrides for this system. */
/* for MACH systems */
#undef	SY_MCH
#define	SY_MCH	1

#ifndef In_Imake

/* Get major data types (esp. caddr_t) */
#include <sys/types.h>

#include <string.h>
#define index strchr
#define rindex strrchr

/* Get open(2) constants */
#include <sys/file.h>

/* Get struct timeval */
#include <time.h>		/* need this as well as sys/time.h */
#include <sys/time.h>

/* include path for syslog.h BSD vs SYSV */
#include <syslog.h>

#define OSI_HAS_SYMLINKS 0

/* If OSI_HAS_SYMLINKS is not defined, osi_readlink is present in libutil. */
extern int osi_readlink();

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

#define HAS_SYSEXITS 1

#endif /* !In_Imake */

/* Now follow the site-specific customizations. */
#include <site.h>

#endif	/* SYSTEM_H */
