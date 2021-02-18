/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* ************************************************************ *\
	wpcgen.c
	The wp_Generate function.
	Include file ``wp.h'' declares the procedures for clients.
\* ************************************************************ */

static char rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/wpcgen.c,v 2.6 1991/09/12 17:30:57 bobg Exp $";

#include <stdio.h>
#include <andyenv.h>
#include <andrewos.h>		/* sys/file.h sys/types.h sys/time.h strings.h*/
#include <ctype.h>
#include <errno.h>
#include <truth.h> /* itc.h -> truth.h DMT */
#include <sys/stat.h>
#include <pwd.h>
#ifdef WHITEPAGES_ENV  /* avoid makedepend "errors" */
#include <bt.h>
#include <wp.h>
#include <btwp.h>
#endif /* WHITEPAGES_ENV   */

extern int errno;
extern char *malloc();
extern char *realloc();

/* Compatibility routines for non-cellular use */
extern struct wp_CD *ThisCellDir;

wp_ErrorCode wp_Generate(PKPtr)
wp_PrimeKey *PKPtr;
{
    if (ThisCellDir == NULL) return wperr_NotInited;
    return cwp_Generate((struct wp_cd *) ThisCellDir, PKPtr);
}
