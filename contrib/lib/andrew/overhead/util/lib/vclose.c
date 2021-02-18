/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/vclose.c,v 2.10 1991/09/12 17:30:21 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/vclose.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */


/*
                vclose.c -- routines that are specific to Vice file system.
 
        vclose -- Do a close to & force it to wait until
                  the close completes.  This is for Vice,
                  which normally does not wait.
 
        vdown  -- Return non-zero if error number passed indicates that
                  Vice/Venus is down.
 
        IsOnVice -- return non-zero if we believe that the file behind
                the given file-descriptor is in Vice.
 
*/

#include <andyenv.h>
#include <errno.h>
extern int errno;
#include <stdio.h>
#include <ctype.h>
#include <andrewos.h>		/* sys/types.h */
#include <sys/ioctl.h>
#include <sys/stat.h>
#ifdef AFS_ENV
#include <afs/param.h>
#include <sys/signal.h>
#include <setjmp.h>
#include <afs/venus.h>
#include <afs/errors.h>
#endif /* AFS_ENV */

extern int errno;

#define ALLSIGS	    0xffffffff

#ifdef AFS_ENV
static int HasCheckedVice = 0, ViceRuns = 1;

static jmp_buf noVice;

static JumpNoVice() {
    longjmp(noVice, 1);
}

static CkVice()
{
    struct sigvec SOld, SNew;
    int GotSignal;
    struct ViceIoctl dummy;

    dummy.in_size = 0;	/* Prepare the trial pioctl() call */
    dummy.out_size = 0;

    GotSignal = 0;	/* nothing so far. */

    SNew.sv_handler = JumpNoVice;
    SNew.sv_mask = 0;	/* no additional sigs to mask */
    SNew.sv_onstack = 0;	/* on regular stack, please */
    if (sigvec(SIGSYS, &SNew, &SOld) != 0) return;

    if (setjmp(noVice) == 0) {	/* test whether Vice runs */
	(void) pioctl("/afs", VIOC_GET_WS_CELL, &dummy, 1);	/* Just see if this signals! */
    } else {			/* oops--caught SIGSYS. */
	GotSignal = 1;
    }
    sigvec(SIGSYS, &SOld, NULL);	/* Reset the old behavior. */

    ViceRuns = (GotSignal ? 0 : 1);	/* Remember whether this is a Vice kernel. */

    HasCheckedVice = 1;
}

static DoIoctl(fd)
int fd;
{
    struct ViceIoctl dummy;

    if (!HasCheckedVice) CkVice();

    if (ViceRuns) {
	dummy.in_size = 0;
	dummy.out_size = 0;
	ioctl(fd, VIOCCLOSEWAIT, &dummy);
    }
}
#endif /* AFS_ENV */

int ViceIsRunning() {
#ifdef AFS_ENV
    if (!HasCheckedVice) CkVice();
    return ViceRuns;
#else /* AFS_ENV */
    return 0;
#endif /* AFS_ENV */
}

int vclose(fd)
int fd;
{
#ifdef AFS_ENV
    register int oldmask;
#endif /* AFS_ENV */
    register int rc;

#ifdef AFS_ENV
    DoIoctl(fd);
    oldmask = sigsetmask(ALLSIGS);
#endif /* AFS_ENV */

    rc = close(fd);

#ifdef AFS_ENV
    sigsetmask(oldmask);
#endif /* AFS_ENV */

    return rc;
}

int vfclose(f)
FILE *f;
{
#ifdef AFS_ENV
    register int oldmask;
#endif /* AFS_ENV */
    register int rc;

    /* Barf immediately on NULL arg */
    if (f == NULL) {
	errno = EINVAL;
	return EOF;
    }

#ifdef AFS_ENV
    DoIoctl(fileno(f));
    oldmask = sigsetmask(ALLSIGS);
#endif /* AFS_ENV */

    rc = fclose(f);

#ifdef AFS_ENV
    sigsetmask(oldmask);
#endif /* AFS_ENV */

    return rc;
}

int vdown(err)
int err;
{
/*	ETIMEDOUT:	Venus has timed out the connection to the file server
      ENXIO:	the Venus process handling the kernel device has terminated
      ENOTTY:	Venus doesn't know about this file descriptor;
probably it's left over from a previous Venus run
      ENODEV:	Volume off line (Venus error code mapping)
      EIO:	AFS read-ahead has failed; file was openable but not fully readable.
    VOFFLINE:	Volume is off line, for reason given in offline message
     VBUSY:	Volume temporarily unavailable; try again; not usually
	      propagated to application level.
      255, -1:    Occasionally erroneously propagated from AFS cache managers to indicate connection failure (the ETIMEDOUT case).
*/
#ifdef AFS_ENV
    if (!HasCheckedVice) CkVice();
    if (ViceRuns) {
	return (err == ETIMEDOUT || err == ENXIO || err == ENOTTY || err == EIO
#ifdef EFAULT
		|| err == EFAULT
#endif /* EFAULT */
		|| err == ENODEV || err == VOFFLINE || err == VBUSY || err == 255 || err == -1);
    } else
#endif /* AFS_ENV */
	return (err == ENXIO
#ifdef ETIMEDOUT
		|| err == ETIMEDOUT
#endif /* ETIMEDOUT */
#ifdef EFAULT
		|| err == EFAULT
#endif /* EFAULT */
		);
}

int IsOnVice(fd)
int fd;
{
    /* Return TRUE if we believe that the file open on the given file descriptor is in Vice, and FALSE otherwise.
	*/
#ifdef AFS_ENV
    struct ViceIoctl dummy;
    auto char CellN[64];
    int Err;
    register int oldmask, rc;

    if (!HasCheckedVice) CkVice();

    if (ViceRuns) {
	dummy.in_size = 0;
	dummy.out_size = sizeof(CellN);
	dummy.out = CellN;
	oldmask = sigsetmask(ALLSIGS);

	rc = ioctl(fd, VIOCIGETCELL, &dummy);
	Err = errno;

	sigsetmask(oldmask);
	return (rc == 0 || (Err != ENOTTY && vdown(Err)));
    } else
#endif /* AFS_ENV */
	return 0;
}

#ifdef TESTINGONLYTESTING
main()
{
    printf("Vdown(etimedout) is %d.\n", vdown(ETIMEDOUT));
}
#endif /* TESTINGONLYTESTING */
