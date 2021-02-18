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

#include <stdio.h>
#include <sys/types.h>
#include "sj_struct.h"

extern	OffsetRec *ofsrec[];
extern	int	ofsrec_num;

static	OffsetRec *makeoffset(ptr, len, ofs)
u_char	*ptr;
int	len;
int	ofs;
{
	OffsetRec *orec;
	u_char	*p;

	orec = (OffsetRec *)Malloc(sizeof(OffsetRec));

	p = (u_char *)Malloc(len);
	bcopy(ptr, p, len);

	orec -> kptr   = p;
	orec -> klen   = len;
	if (ofs)
		orec -> offset = ofs;
	else
		orec -> offset = ofsrec_num + 1;

	return orec;
}

set_ofsrec(ptr, len, ofs)
u_char	*ptr;
int	len;
int	ofs;
{
	OffsetRec *orec;
	int	low, high, mid;
	int	i;

	if (ofsrec_num <= 0) {
		orec = makeoffset(ptr, len, ofs);
		ofsrec[0]= orec;
		ofsrec_num++;

		return;
	}

	low = 0; high = ofsrec_num - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		i = string_cmp(ofsrec[mid]->kptr, ofsrec[mid]->klen, ptr, len);
		if (i > 0)
			high = mid - 1;
		else if (i < 0)
			low = ++mid;
		else
			break;
	}

	if (i) {
		if (ofsrec_num >= MaxOffsetNumber) {
			fprintf(stderr, "\203\111\203\164\203\132\203\142\203\147\201\105\203\145\201\133\203\165\203\213\202\252\202\240\202\323\202\352\202\334\202\265\202\275\n");
			exit(1);
		}

		orec = makeoffset(ptr, len, ofs);

		for (i = ofsrec_num ; i > mid ; i--) ofsrec[i] = ofsrec[i - 1];
		ofsrec[mid] = orec;
		ofsrec_num++;
	}
}

isknjexist(knj, len)
u_char	*knj;
int	len;
{
	int	low, high, mid;
	int	i;

	if (ofsrec_num <= 0) return 0;

	low = 0; high = ofsrec_num - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		i = string_cmp(ofsrec[mid]->kptr, ofsrec[mid]->klen, knj, len);
		if (i > 0)
			high = mid - 1;
		else if (i < 0)
			low = ++mid;
		else
			return ofsrec[mid] -> offset;
	}

	return 0;
}

clear_ofsrec()
{
	int	i;

	for (i = 0 ; i < ofsrec_num ; i++) {
		Free(ofsrec[i] -> kptr);
		Free(ofsrec[i]);
	}

	ofsrec_num = 0;
}

OffsetRec *real_ofsrec(ptr)
u_char	*ptr;
{
	int	ofs;
	int	i;
	OffsetRec **p;

	ofs = (*ptr++ & ~(KanjiStrEnd | KanjiModeMask));
	ofs <<= 8;
	ofs += *ptr;

	for (i = ofsrec_num, p = ofsrec ; i-- > 0 ; p++)
		if ((*p) -> offset == ofs)
			return *p;

	fprintf(stderr, "real_ofsrec() \202\305\210\331\217\355\202\310\217\363\221\324 %d\n", ofs);
	exit(1);
}
