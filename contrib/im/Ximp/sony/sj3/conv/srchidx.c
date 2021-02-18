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
 * Author: Noriyuki Hosoe, Sony Corporation
 */

#include "sj_kcnv.h"

Uchar	*get_idxptr();
Int	sstrlen(), istrcmp();

TypeDicSeg	srchidx(low, len)
TypeDicSeg	low;
Int		len;
{
	TypeDicSeg	high;
	TypeDicSeg	mid;
	Int		cmp;
	Uchar		*target;

	if ((high = curdict->segunit - 1) < 1) return 0;

	if (low == (TypeDicSeg)-1) low = 0;

	while (low <= high) {

		mid = (low + high) >> 1;

		target = get_idxptr(mid);

		cmp = istrcmp(cnvstart, target, len, sstrlen(target));

		if (cmp == OVER) {
			high = mid - 1;
		}

		else if(cmp != MATCH){
			low = mid + 1;
		}

		else {
			return mid;
		}
	}

	return (cmp == OVER && mid != DicSegBase) ? mid - 1 : mid;
}

