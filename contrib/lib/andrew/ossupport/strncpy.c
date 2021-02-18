static char *strncpy_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ossupport/RCS/strncpy.c,v 1.2 1991/09/12 16:57:04 bobg Exp $";

/*
 *      Copyright BellSouth 1991 - All Rights Reserved
 *      For full copyright information see:'andrew/config/COPYRITE.bls'.
 */

/* this exists because of a bug in hp's strncpy 
 * (in that one, bad things can happen when s and t overlap)
 */

char *strncpy(s, t, n)
char *s, *t;
int n;
{
    char *v = s;

    while (n--) {
	*s++ = *t;
	if (*t++ == '\0') return v;
    }
    return v;
}
