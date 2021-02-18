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
#include "sj_right.h"
#include "sj_hinsi.h"
#include "sj_yomicode.h"

Static	Int	cnjstrcmp(yptr, cnjp, saml)
Uchar	*yptr;
Uchar	TFar	*cnjp;
Int	*saml;
{
	Int	asklen;
	Int	nkrlen;

	asklen = CnjAskLen(cnjp);

	if (*saml < asklen) return CONT;

	if (*saml > asklen) return OVER;

	nkrlen = CnjNkrLen(cnjp);

	yptr += asklen;
	cnjp ++;

	while (nkrlen--) {
		if (*yptr > *cnjp)
			return CONT;

		else if (*yptr < *cnjp) {
			if (!*yptr) return PARTLY;
			return OVER;
		}

		else{
			yptr++;
			cnjp++;
			(*saml)++;
		}
	}

	return MATCH;
}

Static	TypeCnct	cnvrow(rec, row)
JREC		*rec;
TypeCnct	row;
{
	TypeGram	hinsi;
	Int		stt;

	hinsi = rec -> hinsi;

	if ((stt = rec -> sttofs) == SETTOU_O) {

		if (R_RENYOU1 <= row && row <= R_SA5RENYOU1)
			row += 6;

		else if (R_RENYOU3 <= row && row <= R_SA5RENYOU3)
			row += 3;

		else if((hinsi>=KEIYOUSI_1 && hinsi<=KE_DOUSI_9)||
			(hinsi == MEISI_5) || (hinsi == MEISI_9))
			;

		else
			row = 0;
	}

	else if (stt == SETTOU_GO) {

		if (hinsi == MEISI_7 && row == R_SMEISI1)
			row++;
	}

	return row;
}

Int	setconj(hinsi, jrec, crec)
TypeGram	hinsi;
JREC		*jrec;
CREC		*crec;
{
	Uchar	*yptr;
	Int	cmp;
	TypeCnct	right;
	Int	saml;
	Int	ofslen;
	int	count = 0;
	Uchar	TFar	*cnj;

	if (cnj  = Conjadr(hinsi)) {
		saml = ofslen = 0;

		yptr = cnvstart + jrec -> jlen;

		if ((*cnj) & TYOUONFLG) {
			while (*yptr == _TYOUON) {
				yptr++;
				ofslen++;
			}
		}

		for ( ; *cnj != CNJEND ; cnj += CnjRecLen(cnj)) {
			cmp = cnjstrcmp(yptr, cnj, &saml);

			if (cmp == OVER) break;

			if (cmp == PARTLY) break;

			if (cmp != MATCH) continue;

			if (isdpnd(*(yptr + saml))) continue;

			if (right = cnvrow(jrec, (TypeCnct)CnjRight(cnj))) {
				crec -> len   = (Uchar)(ofslen + saml);
				crec -> right = right;
				count++;
				crec++;
			}
		}
	}

	return count;
}

