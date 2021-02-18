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

STDYIN	*srchstdy();

Void	setstynum(), setnspr(), regetrank();

Void	getrank()
{
	trank = nrank = 1;

	setstynum();

	regetrank();
}

Static	Void	setstynum()
{
	KHREC	*kptr;
	STDYIN	*sptr;
	Int	count;

	for (kptr = kouhotbl, count = khcount ; count-- ; kptr++) {
		sptr = srchstdy(kptr -> clrec -> jnode -> jseg, kptr -> offs,
				kptr -> clrec -> jnode -> dicid);

		if (!sptr) continue;

		setnspr(kptr, sptr);
	}
}

Static	Void	setnspr(kptr, sptr)
KHREC	*kptr;
STDYIN	*sptr;
{
	KHREC		*ptr;
	Int		keepnm;
	TypeStyNum	styno;

	styno = kptr -> styno = sptr -> styno;

	keepnm = trank;

	for (ptr = kouhotbl ; ptr < kptr ; ptr++) {

		if (styno < ptr -> styno) {
			ptr -> rank++;

			keepnm--;
		}

		else if (styno == ptr -> styno) {

			if (kptr -> sttfg && kptr -> ka_fg) {
				if ((sptr -> sttkj == kptr -> sttkj) &&
				    (sptr -> ka_kj == kptr -> ka_kj)) {
					ptr -> rank++;
					keepnm--;
				}
			}
			else if (kptr -> sttfg) {
				if (sptr -> sttkj == kptr -> sttkj) {
					ptr -> rank++;
					keepnm--;
				}
			}
			else if (kptr -> ka_fg) {
				if (sptr -> ka_kj == kptr -> ka_kj) {
					ptr -> rank++;
					keepnm--;
				}
			}
		}
	}

	kptr -> rank = (Uchar)keepnm;
	trank++;

	if (keepnm <= nrank) nrank++;

	if (sptr -> nmflg && (nrank > keepnm)) nrank = keepnm;
}

Static	Void	regetrank()
{
	KHREC	*kptr;
	Int	count;
	Int	tmp;

	if (nrank < trank) {
		tmp = nrank;

		trank += nkhcount;
	}
	else {
		nkhcount = tmp = 0;
	}

	for (count = khcount, kptr = kouhotbl ; count-- ; kptr++) {
		if (!(kptr -> rank)) {
			if (tmp && kptr -> mode && !(kptr -> offs)) {
				kptr -> rank = (Uchar)tmp++;
			}
			else
				kptr -> rank = (Uchar)trank++;
		}

		else if ((Short)kptr -> rank >= nrank)
			kptr -> rank += (Uchar)nkhcount;
	}
}

