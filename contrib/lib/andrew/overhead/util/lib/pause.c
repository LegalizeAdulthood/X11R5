/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/pause.c,v 2.8 1991/09/12 17:28:16 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/pause.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

/*
		pause.c -- test and set the Computer Science pause-on-disk-full facility

	SetPauseOff() -- turn off disk-full-pausing facility
	SetPauseOn() -- turn on disk-full-pausing facility
	PauseP() -- return non-zero if the disk-full-pausing facility is enabled.
*/

#include <system.h>
#include <errno.h>
extern int errno;
#include <stdio.h>
#include <andrewos.h>		/* sys/types.h */
#include <sys/stat.h>
#include <signal.h>
/* #include <sys.h> DMT commented out */
extern int errno;

static int DoCall(Action, OldP)
int Action, *OldP;
{/* Do the Computer Science Department's ``rpause'' system call for ENOSPC. */
#define	RPAUSE_SAME	0		/* leave state unchanged */
#define	RPAUSE_DISABLE	1	/* disable pause on error */
#define	RPAUSE_ENABLE	2	/* enable pause on error */

#define	RPAUSE_ALL	0x7fffffff		/* all error number types */
#if defined(AIX) || defined(M_UNIX)
    return 0;
#else /* #ifdef AIX */
    struct sigvec OldS, NewS;
    int Err;

    NewS.sv_handler = SIG_IGN;
    NewS.sv_mask = 0;
    NewS.sv_onstack = 0;
    if (sigvec(SIGSYS, &NewS, &OldS) != 0) return -1;

    *OldP = syscall(-5, ENOSPC, RPAUSE_ALL, Action);
    Err = errno;

    sigvec(SIGSYS, &OldS, NULL);	/* Reset old; don't want prior state */
    errno = Err;
    return (*OldP >= 0);
#endif /* #ifdef AIX */
}

int SetPauseOff()
{/* Set the pause facility off. */
    int Dum;
    return DoCall(RPAUSE_DISABLE, &Dum);
}

int SetPauseOn()
{/* Set the pause facility on. */
    int Dum;
    return DoCall(RPAUSE_ENABLE, &Dum);
}

int PauseP()
{/* Tell if the pause facility is on. */
    int Old;

    Old = 0;	/* default: wasn't on. */
    DoCall(RPAUSE_SAME, &Old);
    return (Old == RPAUSE_ENABLE ? 1 : 0);
}

#ifdef TESTINGONLYTESTING
main ()
{
    printf("PauseP() returns %d/%d\n", PauseP(), errno);
    printf("SetPauseOn() returns %d/%d\n", SetPauseOn(), errno);
    printf("PauseP() returns %d/%d\n", PauseP(), errno);
    printf("SetPauseOff() returns %d/%d\n", SetPauseOff(), errno);
    printf("PauseP() returns %d/%d\n", PauseP(), errno);
}
#endif /* TESTINGONLYTESTING */
