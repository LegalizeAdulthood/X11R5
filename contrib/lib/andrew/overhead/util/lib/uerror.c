/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
	uerror.c -- Return a static string describing an errno value.
*/

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/uerror.c,v 2.7 1991/09/12 17:29:39 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/uerror.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/uerror.c,v 2.7 1991/09/12 17:29:39 bobg Exp $";
#endif /* lint */

#include <andrewos.h>
#include <errno.h>
#ifdef AFS_ENV
#include <afs/param.h>
#include <afs/errors.h>
#endif /* AFS_ENV */
#define NULL (char *) 0

char *UnixError(errorNumber)
int	errorNumber;
{
/* Returns a pointer to a static buffer containing English text describing the same error condition that errorNumber describes (interpreted as a Unix error number).  The text has no newlines in it.  We contend that this is what ``perror'' should have been returning all along. */
    static char ErrBuff[40];
    extern int sys_nerr;
    extern char *sys_errlist[];
#ifdef AFS_ENV
    static char *vice_errlist[] = {
	/* 101: VSALVAGE */		"Volume needs salvage",
	/* 102: VNOVNODE */		"Bad vnode number quoted",
	/* 103: VNOVOL */		"Volume not attached, doesn't exist, not created, or not online",
	/* 104: VVOLEXISTS */		"Volume already exists",
	/* 105: VNOSERVICE */		"Volume is not in service",
	/* 106: VOFFLINE */		"Volume is off line",
	/* 107: VONLINE */		"Volume is already on line",
	/* 108: VDISKFULL */		NULL,	/* mapped to ENOSPC */
	/* 109: VOVERQUOTA */		NULL,	/* mapped to EDQUOT */
	/* 110: VBUSY */			"Volume temporarily unavailable; try again",
	/* 111: VMOVED */		"Volume has moved to another server"
    };
#define	Vice_Errlist_Size	(sizeof(vice_errlist) / sizeof(vice_errlist[0]))
#endif /* AFS_ENV */

    if (errorNumber < sys_nerr && errorNumber > 0) return sys_errlist[errorNumber];
#ifdef AFS_ENV
    if (errorNumber >= VICE_SPECIAL_ERRORS
	 && errorNumber < (VICE_SPECIAL_ERRORS + Vice_Errlist_Size))
	if (vice_errlist[errorNumber - VICE_SPECIAL_ERRORS] != NULL)
	    return vice_errlist[errorNumber - VICE_SPECIAL_ERRORS];
#endif /* AFS_ENV */
    sprintf(ErrBuff, "Error number %d", errorNumber);
    return ErrBuff;
}
