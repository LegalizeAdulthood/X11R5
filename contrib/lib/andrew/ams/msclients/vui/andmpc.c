/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/vui/RCS/andmpc.c,v 2.11 1991/09/12 15:52:06 bobg Exp $ */
/* $ACIS: $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/vui/RCS/andmpc.c,v 2.11 1991/09/12 15:52:06 bobg Exp $ ";
#endif /* lint */

/* 
 * This package contains more C library routines either 1.missing from those
 * supplied with the MSC/IBM compilers, or 2. included in libmail or libitc.
 *
 *  Andrew Version
 *
 */

#include <andrewos.h>
#include <ctype.h>
#include <util.h>
ULsubstr(s1, s2)
char *s1, *s2;
{
    while (*s2 && *s1 &&
	   ((isupper(*s1) ? tolower(*s1) : *s1) ==
	    (isupper(*s2) ? tolower(*s2) : *s2))) {
	++s1; ++s2;
    }
    if (!*s2) return(0);
    return((isupper(*s1) ? tolower(*s1) : *s1) - (isupper(*s2) ? tolower(*s2) : *s2));
}

char *memcpy_preserve_overlap(dest, source, len)
char *dest, *source;
int len;
{
#ifndef sun
    bcopy(source, dest, len);
#else /* sun */
    memcpy(dest, source, len);
#endif /* sun */

    return dest;
}

