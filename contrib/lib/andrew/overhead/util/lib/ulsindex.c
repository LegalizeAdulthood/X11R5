/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
	ulsindex.c--find index of one string within another, ignoring alphabetic case.
*/

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/ulsindex.c,v 2.5 1991/09/12 17:29:43 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/ulsindex.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/ulsindex.c,v 2.5 1991/09/12 17:29:43 bobg Exp $";
#endif /* lint */

char *ULsindex(big, small)
char *big, *small;
/*  ULsindex  --  find index of one string within another, ignoring alphabetic case.
 *
 *  Usage:  p = ULsindex (big,small)
 *	char *p,*big,*small;
 *
 *  ULsindex searches for a substring of big which matches small,
 *  and returns a pointer to this substr.  If no matching
 *  substring is found, 0 is returned.
 *
 */
{
    register char *bp, *bp1, *sp, bc, sc;
    register char c = *small;

    if (c==0) return(0);
    if (c <= 'Z') if (c >= 'A') c += ('a' - 'A');
    for (bp=big;  *bp;  bp++) {
	bc = *bp; if (bc <= 'Z') if (bc >= 'A') bc += ('a' - 'A');
	if (bc == c) {
	    sp = small;
	    bp1 = bp;
	    do {
		sc = *++sp; if (sc == '\0') {
		    return(bp);
		}
		if (sc <= 'Z') if (sc >= 'A') sc += ('a' - 'A');
		bc = *++bp1; if (bc <= 'Z') if (bc >= 'A') bc += ('a' - 'A');
	    } while (sc == bc);
	}
    }
    return 0;
}
