/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* ************************************************************ *\
	wpgen.c
	The wp_Generate function.
	Include file ``wp.h'' declares the procedures for clients.
\* ************************************************************ */

static char rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/wpgen.c,v 2.7 1991/09/12 17:31:27 bobg Exp $";

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
extern int errno;
#ifndef _IBMR2
extern char *malloc();
extern char *realloc();
#endif /* _IBMR2 */

/*
To enumerate all the prime keys in the database, use wp_Generate(PKPtr).  Give this procedure a pointer to a null Prime Key and it will allocate the first PrimeKey and return it.  Give it back that PrimeKey and it will deallocate it, find the next one, and return the next one.  If there's no next one, it will return wperr_NoError and yet set your pointer to NULL.
*/
wp_ErrorCode cwp_Generate(cd, PKPtr)
struct wp_cd *cd; wp_PrimeKey *PKPtr;
{
    struct wp_CD *CD = (struct wp_CD *) cd;
    bt_ErrorCode BTErr;
    int LoopNum;
    char *EachKey, KeyBuf[PKLEN+3];
    static char PrimeTail[3] = {KeyTagSep, KeyTagR, '\0'};

    if (CD == NULL || CD->Tag != wpcdTag) return wperr_NotACD;
    for (LoopNum = 0; LoopNum < 5; ++LoopNum) {
	if (*PKPtr == NULL)
	    BTErr = bt_CursorToStart(CD->cursor);
	else if (strcmp(CD->PrevPK, *PKPtr) == 0) {
	    BTErr = bterr_NoError;
	} else {
	    strncpy(KeyBuf, *PKPtr, PKLEN);
	    strcpy(&KeyBuf[PKLEN], PrimeTail);
	    BTErr = bt_Search(CD->cursor, KeyBuf);
	}
	if (BTErr != bterr_NoError && wp_RetryThis(BTErr+wperr_BTreeBaseValue)) {
	    if (cwp_ReInitialize((struct wp_cd *) CD) != wperr_NoError) sleep(3);
	    continue;
	}
	if (BTErr != bterr_NoError) {CD->PrevPK[0] = '\0'; return w_BTreeErr(BTErr);}

	for (;;) {
	    BTErr = bt_NextCursor(CD->cursor);
	    if (BTErr == bterr_NoNextKey) {
		if (*PKPtr != NULL) {free(*PKPtr); *PKPtr = NULL;}
		CD->PrevPK[0] = '\0';
		return wperr_NoError;	/* end of the ride */
	    }
	    if (BTErr == bterr_NoError)
		BTErr = bt_GetCursorKey(CD->cursor, &EachKey);
	    if (BTErr != bterr_NoError) {
		CD->PrevPK[0] = '\0';
		if (wp_RetryThis(BTErr+wperr_BTreeBaseValue)) {
		    if (cwp_ReInitialize((struct wp_cd *) CD) != wperr_NoError) sleep(3);
		    goto MoreTries;
		}
		return w_BTreeErr(BTErr);
	    }
	    if (strcmp(&EachKey[strlen(EachKey) - 2], PrimeTail) == 0) break;
	    free(EachKey);
	}
	/* OK, we want to return this PK. */
	if (*PKPtr != NULL) {
	    if (strlen(*PKPtr) >= PKLEN) {	/* use the existing storage */
		strncpy(*PKPtr, EachKey, PKLEN);
		(*PKPtr)[PKLEN] = '\0';
		free(EachKey);
		strcpy(CD->PrevPK, *PKPtr);
		return wperr_NoError;
	    } else {free(*PKPtr); *PKPtr = NULL;}
	}
	EachKey[PKLEN] = '\0';	/* re-use stg from bt package */
	*PKPtr = EachKey;
	strcpy(CD->PrevPK, *PKPtr);
	return wperr_NoError;
      MoreTries:;
    }
    return w_BTreeErr(BTErr);
}
