/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/getmyhom.c,v 1.4 1991/09/12 17:26:14 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/getmyhom.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

#include <stdio.h>
#include <util.h>

#define NULL 0

static char myhome[100]="";

char *getMyHome()
/* Like gethome(NULL): a less general version that gets only my own home directory, not anybody's.  Will fail if the environment variable isn't set. */
{
    char *h;

    if (myhome[0] == '\0') {
	extern char *getenv();
	h = getenv("HOME");
	if (h != NULL) strncpy(myhome, h, sizeof(myhome));
    }
    return (myhome[0] == '\0' ? NULL : myhome);
}
#ifdef TESTINGONLYTESTING
main()
{
    printf("getMyHome() returns ``%s''.\n", getMyHome());
}
#endif /* TESTINGONLYTESTING */
