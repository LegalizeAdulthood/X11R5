/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* ************************************************************ *\
	getvuid.c
	getvuid() (``get Vice UID'') gets the VUID associated with the ThisDomain authentication.
\* ************************************************************ */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/getvuid.c,v 2.7 1991/09/12 17:26:32 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/getvuid.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/getvuid.c,v 2.7 1991/09/12 17:26:32 bobg Exp $";
#endif /* lint */

#include <andyenv.h>
#include <stdio.h>
#include <errno.h>
#include <andrewos.h>	/* types.h */
#include <svcconf.h>
#include <util.h>

extern int errno;

#ifdef WHITEPAGES_ENV
int getvuid()
{/* Return the Vice UID of the current process, or -1 with errno set */
    struct CellAuth *ca;
    int rc;

    CheckServiceConfiguration();
    if (AMS_ViceIsRunning) {
	rc = FindCell(ThisDomain, &ca);
	if (rc == 0 && ca != NULL) {
	    if (ca->ViceID < 0) FillInCell(ca);
	    if (ca->ViceID >= 0) return ca->ViceID;	/* ViceId in the home cell */
	}
	errno = EINVAL;
	return -1;
    } else {
	return getuid();
    }
}
#endif /* WHITEPAGES_ENV */
