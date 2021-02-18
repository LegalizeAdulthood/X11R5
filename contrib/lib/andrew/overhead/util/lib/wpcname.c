/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* ************************************************************ *\
	wpcname.c
	Compatibility routines for non-cellular White Pages lookups.
	Include file ``wp.h'' declares the procedures for clients.
\* ************************************************************ */

static char rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/wpcname.c,v 2.6 1991/09/12 17:31:10 bobg Exp $";

#include <stdio.h>
#include <andyenv.h>
#include <andrewos.h>		/* sys/file.h sys/types.h strings.h*/
#include <ctype.h>
#include <errno.h>
#include <truth.h> /* itc.h -> truth.h DMT */
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

wp_ErrorCode wp_Search(SrchToken, MaxResults, MaxQuality, MatchQuality, PKPtr)
wp_SearchToken SrchToken; int MaxResults;
int MaxQuality, *MatchQuality; wp_PrimeKeySetPtr *PKPtr;
{
    if (ThisCellDir == NULL) return wperr_NotInited;
    return cwp_Search((struct wp_cd *) ThisCellDir, SrchToken, MaxResults,
		       MaxQuality, MatchQuality, PKPtr);
}

wp_ErrorCode wp_Lookup(SrchToken, MinMatchesFound, MaxQuality, MatchQuality, PKPtr)
wp_SearchToken SrchToken; int *MinMatchesFound;
int MaxQuality, *MatchQuality; wp_PrimeKey *PKPtr;
{
    if (ThisCellDir == NULL) return wperr_NotInited;
    return cwp_Lookup((struct wp_cd *) ThisCellDir, SrchToken, MinMatchesFound,
		       MaxQuality, MatchQuality, PKPtr);
}
