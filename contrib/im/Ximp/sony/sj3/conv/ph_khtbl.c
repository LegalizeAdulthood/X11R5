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

Uchar	*skipkstr();
STDYIN	*srchstdy();
Void	ph_setkouho();
Int	seldict();

Static	Void	ph_setsty(clrec)
CLREC	*clrec;
{
	JREC		*jrec;
	TypeDicOfs	offset;
	Uchar		*ptr;
	STDYIN		*sptr;

	if (!seldict((jrec = clrec -> jnode) -> dicid)) {
		if (!khcount)
			ph_setkouho(clrec, (TypeDicOfs)1, (STDYIN *)NULL);
		return;
	}

	(*curdict->getdic)(curdict, jrec -> jseg);
	ptr = dicbuf + jrec->jofsst + 1;

	for ( ; *ptr != HinsiBlkTerm ; ptr = skipkstr(ptr)) {

		offset = ptr - dicbuf;

		sptr = srchstdy(jrec -> jseg, offset, jrec -> dicid);

		if (!khcount){
			ph_setkouho(clrec, offset, sptr);
			continue;
		}

		if (!sptr) continue;

		if (sptr -> styno < kouhotbl[0].styno) {
			ph_setkouho(clrec, offset, sptr);
		}

		else if (sptr -> styno == kouhotbl -> styno) {
			if (!(kouhotbl[0].ka_fg &&
			   sptr -> ka_kj == kouhotbl[0].ka_kj)) {
				ph_setkouho(clrec, offset, sptr);
			}
		}
	}
}



Int	ph_khtbl(clrec)
CLREC	*clrec;
{
	JREC	*jrec;
	Int	flg = FALSE;

	switch((jrec = clrec -> jnode) -> class) {
	case C_DICT:
		ph_setsty(clrec);
		break;

	case C_N_ARABIA:
	case C_N_ARACMA:
	case C_N_KAZU:
	case C_N_SUUJI:
	case C_N_NANSUU:
	case C_N_KANKURA:
	case C_N_ARAKURA:
	case C_N_KAZULONG:
	case C_N_KAZULCMA:
	case C_N_SUUJILONG:
		if (jrec -> jofsst) {
			ph_setsty(clrec);
			break;
		}
		else
			flg = TRUE;
	case C_BUNSETU:
	case C_MINASI:
	case C_WAKACHI:
		if (!khcount) ph_setkouho(clrec, (TypeDicOfs)0, (STDYIN *)0);
		break;
	}

	return flg;
}

