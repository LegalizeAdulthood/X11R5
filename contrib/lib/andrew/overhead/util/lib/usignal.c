/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
	usignal.c -- Return a static string describing a signal value.
*/

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/usignal.c,v 2.7 1991/09/12 17:30:12 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/usignal.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/usignal.c,v 2.7 1991/09/12 17:30:12 bobg Exp $";
#endif /* lint */

#include <errno.h>
#include <signal.h>

char *UnixSignal(signalNumber)
int	signalNumber;
{
/* Returns a pointer to a static buffer containing English text describing the same signal condition that signalNumber describes (interpreted as a Unix signal number).  The text has no newlines in it.  We contend that this is what ``psignal'' should have been returning all along. */
    static char SigBuff[40];
#if (!defined(hpux)) && (!defined(AIX) && !defined(M_UNIX))
    extern char *sys_siglist[];

    if (signalNumber < NSIG && signalNumber > 0) return sys_siglist[signalNumber];
#endif /* #if (!defined(hpux)) && (!defined(AIX)) */
    sprintf(SigBuff, "Signal number %d", signalNumber);
    return SigBuff;
}
