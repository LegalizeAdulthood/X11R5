/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
	casestrcpy.c -- functions to copy a source string to a dest buff. lcstrcpy converts the source chars to lowercase before copying. ucstrcpy converts source chars to uppercase.
*/

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/casestrcpy.c,v 1.2 1991/09/12 17:24:35 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/casestrcpy.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/casestrcpy.c,v 1.2 1991/09/12 17:24:35 bobg Exp $";
#endif /* lint */

#include <ctype.h>

#define	UPCASE(x)   (isascii(x) && isalpha(x) && isupper(x) ? (x) : (toupper(x)))
#define DOWNCASE(x) (isascii(x) && isupper(x) ? (tolower(x)) : (x) )

/* Just like strncpy but shift-case in transit and forces null termination */
/* Copied 8/24/89 from afs/util/casestrcpy.c to allow omission of lib/afs/util.a */

char *lcstring (d, s, n)
  char *d;				/* dest string */
  char *s;				/* source string */
  int   n;				/* max transfer size */
{   char *original_d = d;
    char  c;

    if ((s == 0) || (d == 0)) return 0;	/* just to be safe */
    while (n) {
	c = *s++;
	c = DOWNCASE(c);
	*d++ = c;
	if (c == 0) break;		/* quit after transferring null */
	if (--n == 0) *(d-1) = 0;	/* make sure null terminated */
    }
    return original_d;
}

#undef DOWNCASE

/* Copied to avoid linker errors when AFS and AMS libraries are linked
 * together.
 *
 * Just like strncpy but shift-case in transit and forces null termination.  
 * Copied 8/24/89 from afs/util/casestrcpy.c to allow omission of 
 * lib/afs/util.a.
 */

char *ucstring (d, s, n)
  char *d;
  char *s;
  int   n;
{   char *original_d = d;
    char  c;

    if ((s == 0) || (d == 0)) return 0;
    while (n) {
	c = *s++;
	c = UPCASE(c);
	*d++ = c;
	if (c == 0) break;
	if (--n == 0) *(d-1) = 0;	/* make sure null terminated */
    }
    return original_d;
}

#undef UPCASE
