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

Void	get_askknj()
{
	Reg1	Uchar	*p;
	Reg3	Uchar	*q;
	Reg2	Uchar	*r;
	Int	i = 0;
	Int	flg;

	p = q = dicbuf;

	q += *p++;

	while (p < q) {
		askknj[i] = p;

		r = NULL;
		flg = TRUE;
		do {
			if (*p & KanjiStrEnd) flg = FALSE;

			if (!r && ((*p & KanjiModeMask) != AiAttribute))
				r = p;

			p += codesize(*p);
		} while (flg);

		askknj_k[i++] = r;
	}
}

Int	seldict(id)
TypeDicID	id;
{
	DICTL	*dp;

	for (dp = dictlist ; dp ; dp = dp -> next) {
		if ((curdict = dp -> dict) -> dicid == id)
			return TRUE;
	}
	curdict = NULL;

	return FALSE;
}

Uchar	*get_idxptr(seg)
TypeDicSeg	seg;
{
	(*curdict->getofs)(curdict);
	(*curdict->getidx)(curdict);

	return idxbuf + idxofs[seg];
}

