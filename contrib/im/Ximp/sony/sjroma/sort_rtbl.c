/*
 * Copyright 1991 Sony Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Sony not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Sony makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SONY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SONY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Masaki Takeuchi, Sony Corporation
 */
/*
 *	sort_rtbl.c
 *	sort romaji tuduri table.
 *	Copyright (c) 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi
 *	Wed Mar  9 15:12:49 JST 1988
 */
/*
 * $Header: sort_rtbl.c,v 1.1 91/04/29 17:56:51 masaki Exp $ SONY;
 */

#include "sj3lib.h"
#include "rtbl.h"

sort_rtbl (tbl, num)
struct romaji	**tbl;
int	num;
{
	quick_rtbl (tbl, 0, num - 1);
}

quick_rtbl (tbl, i, j)		/* Hoare's quick sort */
struct romaji	**tbl;
register int	i, j;
{
	register int	k;

	if (i >= j)
		return;
	k = distribute (tbl, i, j);
	if ((j - k) > (k - i)) {
		quick_rtbl (tbl, i, k - 1);
		quick_rtbl (tbl, k + 1, j);
	}
	else {
		quick_rtbl (tbl, k + 1, j);
		quick_rtbl (tbl, i, k - 1);
	}
}

distribute (tbl, i, j)		/* distribute algorithm	*/
register struct romaji	**tbl;
register int		i, j;
{
	register struct romaji	*pivot;
	register int		last;

	--i;
	pivot = tbl[j];
	last = j;
	while (i < j) {
		do {
			++i;
		} while(small (tbl[i], pivot));
		do {
			--j;
		} while ((i < j) && small (pivot, tbl[j]));
		if(i < j)
			exchange (tbl, i, j);
	}
	exchange (tbl, i, last);
	return (i);
}

small (v, w)			/* compare algorithm	*/
register struct romaji	*v, *w;
{
	/*
	 *	If v is smaller than w, return 1,
	 *	else return (0).
	 */
	return (compare (v->tuduri, w->tuduri));
}

compare (v, w)
register char	*v, *w;
{
	if (*v < *w)
		return (1);
	else if (*v > *w)
		return (0);
	else if (*v == '\0' && *w == '\0')
		return (0);
	else
		return (compare (++v, ++w));
}

exchange (tbl, i, j)		/* exchange algorithm	*/
register struct romaji	**tbl;
register int		i, j;
{
	register struct romaji	*temp;

	temp = tbl[i];
	tbl[i] = tbl[j];
	tbl[j] = temp;
}

