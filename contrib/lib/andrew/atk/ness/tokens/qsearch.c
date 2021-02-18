/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/qsearch.c,v 1.3 1991/09/12 16:27:17 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/qsearch.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/qsearch.c,v 1.3 1991/09/12 16:27:17 bobg Exp $";
#endif

/* 
	qsearch.c  -  a reasonably fast search algorithm
		(as suggested by article in CACM)

	qsearch(char *srcloc, long srclen, char *patloc, long patlen) 
			returns char *;
		returns the location of the found string
			or NULL if not found
		its length will be patlen

	qmatch(char *srcloc, long srclen, char *patloc, long patlen) 
			returns long;
		returns a length indicating how much of pat is matched
		starting at beginning of src

		NB. If you just want to know if pat is a prefix of
		src, use bcmp.

*/
/*
 * $Log: qsearch.c,v $
 * Revision 1.3  1991/09/12  16:27:17  bobg
 * Update copyright notice and rcsid
 *
 * Revision 1.2  1990/11/28  14:39:46  susan
 * moved andrewos.h to be first include file
 *
 * Revision 1.1  90/10/12  20:54:33  wjh
 * Initial revision
 * 
 * Creation 0.0  90/10/5 12:35:00  wjh
 * Initial creation by WJHansen
 * 
*/

#include <andrewos.h>	/* bcmp() */
#include <class.h>	/* for NULL */
#include <ctype.h>	/* isspace() */

/* qsearch(srcloc, srclen, patloc, patlen) returns char *;
	returns the location of the found string
		or NULL if not found
	its length will be patlen
*/
	char *
qsearch(srcloc, srclen, patloc, patlen) 
	char *srcloc, *patloc;
	long srclen, patlen;
{
	long delta[256];
	register char *cx, *curend;
	register char target;
	register long i;

	/* for each character of pattern, compute a delta value
		such that is the pattern is aligned at cx
		it should next align at
		cx+delta[*(cx+patlen)]
		If there are multiple instances of a character, 
		use the rightmost because it advances cx the least
	*/
	for (i = 0; i < 256; i++)
		delta[i] = patlen+1;	/* delta for chars not in pat */
	curend = patloc + patlen;
	for (cx = patloc; cx < curend; cx++) 
		delta[*cx] = curend - cx;

	/* compute target character:  first non-white in pat 
		XXX ideally target would be that character in pat
		least likely to be found in the src */
	curend = patloc + patlen;
	target = *(curend-1);	/* use last char of pat if none other */
	i = patlen - 1;
	for (cx = patloc; cx < curend; cx++) 
		if ( ! isspace(*cx)) {
			target = *cx;
			i = cx - patloc;
			break;
		}

	cx = srcloc;
	curend = srcloc + srclen - patlen + 1;
	while (cx < curend) {
		if (*(cx+i) == target && bcmp(cx, patloc, patlen) == 0)
			return cx;
		cx += delta[*(cx+patlen)];
	}
	return NULL;
}

/* qmatch(srcloc, srclen, patloc, patlen) returns long;
	returns a length indicating how much of pat is matched
	starting at beginning of src

	NB. If you just want to know if pat is a prefix of
	src, use bcmp.
*/
	long
qmatch(srcloc, srclen, patloc, patlen) 
	char *srcloc, *patloc;
	long srclen, patlen;
{
	register long *lsx, *lpx, *lsend;
	register char *sx = srcloc, *px = patloc, *send;
	long len = (srclen > patlen) ? patlen : srclen;
	long oddcnt;
	send = srcloc + len;

	if (len < 30 || (((long)srcloc) & 3) != (((long)patloc) & 3) 
			/* now check hardware characteristics: */
			|| sizeof(char) * 4 != sizeof(long)
			|| sizeof(char *) != 4) {
		for ( ; sx < send && *sx == *px; sx++, px++) 
			{}
		return sx - srcloc;
	}

	/* len >= 30 and both aligned the same:  do by long words */
	/* align to long boundary */
	oddcnt = (4 - ((long)srcloc)) & 3;
	while (oddcnt > 0) {
		if (*sx != *px) return sx - srcloc;
		sx++, px++, oddcnt --;
	}

	/* check a sequence of long words */
	lsx = (long *)sx;  
	lpx = (long *)px;
	lsend = (long *)((long)send & ~3);
	for ( ; lsx < lsend && *lsx == *lpx; lsx++, lpx++) 
		{}
	
	sx = (char *)lsx;
	px = (char *)lpx;
	/* check last word (whether or not lsx == lsend) 
		assertion: this loop will execute no more than thrice */
	for ( ; sx < send && *sx == *px; sx++, px++)
		{}
	return sx - srcloc;
}
