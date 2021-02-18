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
Int	yomicmp(), sstrlen();
Void	get_askknj();
TypeDicSeg	srchidx();

Int	yomicmp(ptr1, ptr2, saml)
Reg1	Uchar	*ptr1;
Reg2	Uchar	*ptr2;
Uchar	*saml;
{
	Int	i, j;
	Int	same;

	same = *saml;

	i = getplen(ptr2);
	j = getnlen(ptr2);

	if (j == 0) {
		ptr2 = get_idxptr(prevseg);

		for (i = same ; i ; i--)
			if (*ptr1++ != *ptr2++) return OVER;

		j = sstrlen(ptr2);
	}

	else if (i > same)
		return CONT;

	else {
		if (i < same) same = i;
		ptr1 += same;
		ptr2 += DouonBlkSizeNumber;
	}

	while (j-- > 0) {
		if (*ptr1 > *ptr2) {
			*saml = same;
			return CONT;
		}
		else if (*ptr1++ < *ptr2++) {
			return OVER;
		}
		same++;
	}

	*saml = same;
	return MATCH;
}


Uchar	*srchdict(tagp)
Uchar	*tagp;
{
	TypeDicSeg	segno;
	Int		cmp;
	Int		maxlen;

	maxlen = (cnvlen > MaxWdYomiLen) ? MaxWdYomiLen : cnvlen;

	while ((int)dicinl <= maxlen) {


		segno = srchidx(prevseg, (Int)dicinl);

		(*curdict->getdic)(curdict, segno);

		if (prevseg != segno) {

			prevseg = segno;

			tagp = dicbuf;
		}

		else {

			tagp = getntag(tagp);

			if (segend(tagp)) {

				prevseg = ++segno;

				if (segno >= curdict->segunit) return NULL;

				(*curdict->getdic)(curdict, segno);

				tagp = dicbuf;
			}
		}

		if (tagp == dicbuf) {
			get_askknj();
			tagp += *tagp + 1;
		}

		while (!segend(tagp)) {

			cmp = yomicmp(cnvstart, tagp, &dicsaml);

			if ((int)dicsaml > maxlen) return NULL;

			if (cmp == OVER)
				return NULL;

			else if (cmp == MATCH) {

				if (isdpnd(*(cnvstart + dicsaml))) {
					break;
				}

				else {
					dicinl = dicsaml + 1;
					return tagp;
				}
			}
			tagp = getntag(tagp);
		}
		dicinl = dicsaml + 1;
	}

	return NULL;
}

