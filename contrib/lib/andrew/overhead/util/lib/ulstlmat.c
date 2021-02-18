/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
	ulstlmat.c--match leftmost part of string, ignoring alphabetic case.
*/

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/ulstlmat.c,v 2.5 1991/09/12 17:29:49 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/ulstlmat.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/ulstlmat.c,v 2.5 1991/09/12 17:29:49 bobg Exp $";
#endif /* lint */

int ULstlmatch (big,small)
char *small, *big;
{
/*  stlmatch  --  match leftmost part of string
 *
 *  Usage:  i = stlmatch (big,small)
 *	int i;
 *	char *small, *big;
 *
 *  Returns 1 iff initial characters of big match small exactly, ignoring alphabetic case.
 *  else 0.
 */
    register char *s, *b, sc, bc;
    s = small;
    b = big;
    do {
	if (*s == '\0')  return (1);
	sc = *s++; if (sc <= 'Z') if (sc >= 'A') sc += ('a' - 'A');
	bc = *b++; if (bc <= 'Z') if (bc >= 'A') bc += ('a' - 'A');
    } 
    while (sc == bc);
    return (0);
}
