/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
	newstr.c -- Return a dynamically-allocated copy of a string.
*/

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/newstr.c,v 2.6 1991/09/12 17:27:53 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/newstr.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/newstr.c,v 2.6 1991/09/12 17:27:53 bobg Exp $";
#endif /* lint */

#include <andrewos.h>		/* strings.h */

#ifndef _IBMR2
extern char *malloc();
#endif /* _IBMR2 */

char *NewString(srcptr)
char *srcptr;
{
    /* Allocate a buffer long enough to hold the argument string, copy the string to that buffer, and return the pointer to the new buffer. */
    char *NewP;
    NewP = malloc(strlen(srcptr)+1);
    if (NewP != 0) strcpy(NewP, srcptr);
    return NewP;
}
