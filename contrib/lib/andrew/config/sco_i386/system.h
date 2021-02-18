/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#ifndef	SYSTEM_H
#define	SYSTEM_H

/* Get common definitions */
#include <allsys.h>

#define	OPSYSNAME	"sco_i386"
#define	sys_sco_i386	1
#define	SYS_NAME	"sco_i386"
#define	i386_ENV	1

/* Here follow the overrides for this system. */
#undef	SY_U53
#define	SY_U53	1 /* This system is most like SVR3 */
#undef SY_U5x
#define SY_U5x 1

#ifndef In_Imake

#ifndef SYSV
#define SYSV	1
#endif /* SYSV */

#define select Xselect
#include <sys/select.h>

/* Get major data types (esp. caddr_t) */
#include <sys/types.h>

#define SYSV_STRINGS
#include <string.h>
#define index strchr
#define rindex strrchr

/* Get open(2) constants */
#include <fcntl.h>
#include <sys/file.h>

/* Get struct timeval */
#include <time.h>

/* More BSDisms */

#define SIGCHLD SIGCLD

/* getdtablesize() and an errno which does not seem to be defined for SYSV */
#define getdtablesize()			_NFILE
#define setpriority(which,who,prio) (nice((prio)-nice(0)))

#define OSI_HAS_SYMLINKS 0
/* If OSI_HAS_SYMLINKS is not defined, osi_readlink is present in libutil. */
extern int osi_readlink();

#include <unistd.h>
#define osi_ExclusiveLockNoBlock(fid)	lockf((fid), F_TLOCK, 0)
#define osi_UnLock(fid)			lockf((fid), F_ULOCK, 0)
#define osi_O_READLOCK			O_RDWR
#define osi_F_READLOCK			"r+"

#define	osi_vfork()			fork()

#define	osi_setjmp  _setjmp
#define	osi_longjmp _longjmp

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


#define sigvec sigaction
#define sv_handler sa_handler
#define sv_mask sa_mask
#define sv_flags sa_flags
#define sv_onstack sa_flags

#define random rand
#define srandom srand
#define initstate(a,b,c) srand(a)
/* bcopy defined below */
#define bzero(b, length) memset(b, 0, length)
#define bcmp(region1, region2, length) memcmp(region1, region2, length)
#define killpg(pgid, signal) kill(-(pgid), signal)

#include <sys/param.h>
#define MAXPATHLEN 512
#define getwd(pathname) getcwd(pathname, MAXPATHLEN)

#include <sysexits.h>

/*
 * Structure of the information in the first word returned by both
 * wait and wait3.  If w_stopval==WSTOPPED, then the second structure
 * describes the information returned, else the first.  See WUNTRACED below.
 */
   union wait	{
	int	w_status;		/* used in syscall */
	/*
	 * Terminated process status.
	 */
	struct {
		unsigned short	w_pad;		/* pad to low order 16 bits */
		unsigned short	w_Retcode:8;	/* exit code if w_termsig==0 */
		unsigned short	w_Coredump:1;	/* core dump indicator */
		unsigned short	w_Termsig:7;	/* termination signal */
	} w_T;
	/*
	 * Stopped process status.  Returned
	 * only for traced children unless requested
	 * with the WUNTRACED option bit.
	 */
	struct {
		unsigned short	w_pad;		/* pad to low order 16 bits */
		unsigned short	w_Stopsig:8;	/* signal that stopped us */
		unsigned short	w_Stopval:8;	/* == W_STOPPED if stopped */
	} w_S;
   };
#  define w_termsig	w_T.w_Termsig
#  define w_coredump	w_T.w_Coredump
#  define w_retcode	w_T.w_Retcode
#  define w_stopval	w_S.w_Stopval
#  define w_stopsig	w_S.w_Stopsig

#  define WSTOPPED	0177	/* value of s.stopval if process is stopped */

#define wait3(stat_loc,options,pid) waitpid(-1,(stat_loc),(options))

#define L_SET	SEEK_SET
#define L_INCR	SEEK_CUR
#define L_XTND	SEEK_END

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

#define lstat(a,b) stat((a),(b))

/* to work around a bug in gethostname */
#define gethostname(a,b) (((b)<127)?(gethostname((a),(b))):(gethostname((a),127)))

/* These are valid for 3.2v0 through 3.2v2
 */
#define USESHORTFILENAMES 1
#define NO_ITIMER_ENV 1

#endif /* !In_Imake */

#undef RESOLVER_ENV
#define PRE_X11R4_ENV 1
#define USE_MMDF_ENV 1
#ifndef POSIX_ENV
#define POSIX_ENV	1
#endif /* POSIX_ENV */

/* Now follow the site-specific customizations. */
#include <site.h>

#ifndef In_Imake
#ifndef MEMMOVE_IS_BROKEN
#define bcopy(src, dst, length) memmove(dst, src, length)
#endif
#ifdef RENAME_IS_BROKEN
#define rename sco_rename
#endif
#ifdef NO_ITIMER_ENV
struct itimerval {
	struct timeval it_interval;	/* timer interval */
	struct timeval it_value;	/* current value */
};
#define ITIMER_REAL	0		/* real time intervals */
#define ITIMER_VIRTUAL	1		/* virtual time intervals */
#define ITIMER_PROF	2		/* user and system virtual time */
#else
#include <sys/itimer.h>
#endif /* NO_ITIMER_ENV */
#endif /* !In_Imake */

#endif	/* SYSTEM_H */
