/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* ************************************************************ *\
	wpcbase.c
	Compatibility routines that choose a cell to use.
	Include file ``wp.h'' declares the procedures for clients.
\* ************************************************************ */

static char rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/wpcbase.c,v 2.11 1991/09/12 17:30:52 bobg Exp $";

#include <andyenv.h>
#include <stdio.h>
#include <andrewos.h>		/* sys/file.h sys/types.h sys/time.h strings.h */
#include <ctype.h>
#include <errno.h>
#include <truth.h> /* itc.h -> truth.h DMT */
#include <sys/stat.h>
#include <pwd.h>
#include <util.h>
#ifdef WHITEPAGES_ENV  /* avoid makedepend "errors" */
#include <bt.h>
#include <wp.h>
#include <btwp.h>
#endif /* WHITEPAGES_ENV   */
#include <svcconf.h>

extern int errno;
#ifndef _IBMR2
extern char *malloc();
extern char *realloc();
#endif /* _IBMR2 */

/* Compatibility routines for non-cellular use */
struct wp_CD *ThisCellDir = NULL;

wp_ErrorCode wp_Initialize()
{
    wp_ErrorCode wpErr;
    /* char DomainName[200]; */
    int rc;

    if (ThisCellDir != NULL && ThisCellDir->Tag != wpcdTag) ThisCellDir = NULL;
    if (ThisCellDir == NULL) {	/* Need to default this from somewhere. */
	CheckServiceConfiguration();
	/* rc = GetCurrentWSCell(DomainName, sizeof(DomainName)); */
	/* if (rc != 0) return (rc + wperr_FileSystemErrorBegin); */
	/* Try to get the WP for this domain. */
	/* wpErr = wp_InitializeCell(DomainName, (struct wp_cd **) &ThisCellDir); */
	wpErr = wp_InitializeCell(ThisDomain, (struct wp_cd **) &ThisCellDir);
	return wpErr;	/* First initialization: Done as much as possible. */
    }

    /* Subsequent times: just bump the use count. */
    ++(ThisCellDir->TimesInited);
    return wperr_NoError;
}

wp_ErrorCode wp_Terminate()
{
    int OldInited;
    wp_ErrorCode res;

    if (ThisCellDir == NULL) return wperr_NotInited;
    OldInited = ThisCellDir->TimesInited;
    res = cwp_Terminate((struct wp_cd *) ThisCellDir);
    if (OldInited <= 1) ThisCellDir = NULL;	/* Was freed */
    return res;
}

wp_ErrorCode wp_GetNIDOnly(NID, PKPtr)
int NID; wp_PrimeKey *PKPtr;
{
    if (ThisCellDir == NULL) return wperr_NotInited;
    return cwp_GetNIDOnly((struct wp_cd *) ThisCellDir, NID, PKPtr);
}

wp_ErrorCode wp_GetUIDOnly(UID, PKPtr)
char *UID; wp_PrimeKey *PKPtr;
{
    if (ThisCellDir == NULL) return wperr_NotInited;
    return cwp_GetUIDOnly((struct wp_cd *) ThisCellDir, UID, PKPtr);
}

wp_ErrorCode wp_Read(PKey, FieldIx, FValPtr)
wp_PrimeKey PKey; wp_FieldIndex FieldIx; char **FValPtr;
{
    if (ThisCellDir == NULL) return wperr_NotInited;
    return cwp_Read((struct wp_cd *) ThisCellDir, PKey, FieldIx, FValPtr);
}
