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

Void	get_askknj();

Int	srchg_atrb(p, ofs)
Uchar		*p;
TypeDicOfs	*ofs;
{
	if (((*p & KanjiCodeMask) << 8) + *(p + 1) == grp_grp)
		*ofs = grp_ptr - dicbuf;

	return 2;
}

Int	srchg_ofs(p, ofs)
Uchar		*p;
TypeDicOfs	*ofs;
{
	Int	contf = TRUE;

	p = dicbuf + ((*p & KanjiCodeMask) << 8) + *(p + 1);
	do {
		if (*p & KanjiStrEnd) contf = FALSE;
		p += (*Srchg_func[*p >> 4])(p, ofs);
	} while (contf);

	return 2;
}

Int	srchg_knj(p, ofs)
Uchar		*p;
TypeDicOfs	*ofs;
{
	Int	contf = TRUE;

	p = askknj[*p & KanjiCodeMask];
	do {
		if (*p & KanjiStrEnd) contf = FALSE;
		p += (*Srchg_func[*p >> 4])(p, ofs);
	} while (contf);

	return 1;
}

Int	srchg_norm1(p, ofs)
Uchar		*p;
TypeDicOfs	*ofs;
{
	return 1;
}

Int	srchg_norm2(p, ofs)
Uchar		*p;
TypeDicOfs	*ofs;
{
	return 2;
}


TypeDicOfs	srchgrp2nd()
{
	Uchar		*p;
	TypeDicOfs	ofs = 0;
	Int		contf;

	(*curdict->getdic)(curdict, grp_seg);
	get_askknj();

	while (*grp_ptr != HinsiBlkTerm) {

		for (contf = TRUE, p = grp_ptr ; contf ; ) {
			if (*p & KanjiStrEnd) contf = FALSE;

			p += (*Srchg_func[*p >> 4])(p, &ofs);
		}

		grp_ptr = p;

		if (ofs) return ofs;
	}

	return 0;
}

TypeDicOfs	srchgrp1st(seg, ofs, grp)
TypeDicSeg	seg;
TypeDicOfs	ofs;
TypeGroup	grp;
{
	(*curdict->getdic)(curdict, grp_seg = seg);

	grp_ptr = dicbuf + ofs + 1;

	grp_grp = grp;

	return srchgrp2nd();
}

