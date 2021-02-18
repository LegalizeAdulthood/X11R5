/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
	tfail.c
	tfail() -- Return a Boolean indicating whether the errno value represents a temporary failure.
*/

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/tfail.c,v 2.8 1991/09/12 17:28:59 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/tfail.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/tfail.c,v 2.8 1991/09/12 17:28:59 bobg Exp $";
#endif /* lint */

#include <andrewos.h>
#include <stdio.h>
#include <errno.h>
#ifdef AFS_ENV
#include <afs/param.h>
#include <afs/errors.h>
#endif /* AFS_ENV */
#include <util.h>

int tfail(errorNumber)
int errorNumber;
{
/* Returns a Boolean indicating whether the errno value errorNumber is probably a temporary failure condition, i.e., one that might succeed if tried again later.  Returns 1 (true) on a temporary failure, 0 (false) on a permanent failure.
Admittedly, for most of the error conditions described, we can make only a guess about the temporary-ness of an error (EIO? EMFILE? EROFS? EMLINK?), but this is only a rough guess.
*/

    if (vdown(errorNumber)) return 1;
    switch (errorNumber) {
	case EINTR:
	case EIO:
	case EAGAIN:
	case ENOMEM:
	case ENODEV:
	case ENFILE:
	case ETXTBSY:
	case ENOSPC:
	case ENETDOWN:
	case ENETUNREACH:
	case ENETRESET:
	case ECONNABORTED:
	case ECONNRESET:
	case ENOBUFS:
	case ESHUTDOWN:
	case ETIMEDOUT:
	case ECONNREFUSED:
	case EHOSTDOWN:
	case EHOSTUNREACH:
#ifdef EDQUOT
	case EDQUOT:
#endif /* EDQUOT */
#ifdef AFS_ENV
	case VSALVAGE:
	case VOFFLINE:
	case VBUSY:
	case VMOVED:
#endif /* AFS_ENV */
	    return 1;		/* temporary failures */
	default:
	    return 0;		/* all others are permanent failures */
    }
}
