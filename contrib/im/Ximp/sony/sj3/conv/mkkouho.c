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
#include "sj_hinsi.h"
#include "sj_suuji.h"

Uchar	*skipkstr();
Void	setkouho(), setwdnum(), get_askknj();
Int	seldict();

Void	getkhtbl(), cl_kanji();
Int	diffknj();
Void	cl_numcmn();
Uchar	*makekan();

Void	mkkouho()
{
	CLREC	*clrec;
	Int	keeplen;

	khcount = nkhcount = 0;

	keeplen = clt1st -> cllen;

	clrec = clt1st;

	do {
		getkhtbl(clrec);
		clrec = clrec -> clsort;
	} while (clrec && (keeplen == clrec -> cllen));
}

Static	Void	getkhtbl(clrec)
CLREC	*clrec;
{
	JREC	*jrec;

	jrec = clrec -> jnode;

	switch (jrec -> class) {
	case C_DICT:
		cl_kanji(jrec, clrec);
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
		cl_numcmn(jrec, clrec);
		break;

	case C_BUNSETU:
		break;

	case C_MINASI:
	case C_WAKACHI:
		setkouho(clrec, (TypeDicOfs)0, 0);
		break;
	}
}

Static	Void 	cl_kanji(jrec, clrec)
JREC  	*jrec;
CLREC 	*clrec;
{
	Uchar	*ptr;
	Int	kcount = khcount;

	if (seldict(jrec -> dicid)) {
		(*curdict->getdic)(curdict, jrec -> jseg);
		ptr = dicbuf + jrec->jofsst + 1;

		get_askknj();

		while (*ptr != HinsiBlkTerm) {

			if (diffknj(jrec, ptr, kcount)) {
				setkouho(clrec, (TypeDicOfs)(ptr - dicbuf), 0);
			}

			ptr = skipkstr(ptr);
		}
	}
	else {
		setkouho(clrec, (TypeDicOfs)1, 0);
	}
}

Uchar	*makekan_none(s, d, flg)
Uchar	*s;
Uchar	*d;
Int	flg;
{
	return d;
}

Uchar	*makekan_1byte(s, d, flg)
Uchar	*s;
Uchar	*d;
Int	flg;
{
	*d++ = flg ? *s : (*s & ~KanjiStrEnd);
	return d;
}

Uchar	*makekan_knj(s, d, flg)
Uchar	*s;
Uchar	*d;
Int	flg;
{
	return makekan(askknj[*s & KanjiCodeMask], d, flg);
}

Uchar	*makekan_ofs(s, d, flg)
Uchar	*s;
Uchar	*d;
Int	flg;
{
	return makekan(dicbuf + ((*s & KanjiCodeMask) << 8) + *(s + 1),
				d, flg);
}

Uchar	*makekan_norm(s, d, flg)
Uchar	*s;
Uchar	*d;
Int	flg;
{
	*d++ = flg ? *s : (*s & ~KanjiStrEnd);
	*d++ = *++s;

	return d;
}

Static	Uchar	*makekan(s, d, flg)
Uchar	*s;
Uchar	*d;
Int	flg;
{
	for ( ; ; ) {
		if (*s & KanjiStrEnd) {
			d = (*Makekan_func[*s >> 4])(s, d, flg);
			break;
		}
		else
			d = (*Makekan_func[*s >> 4])(s, d, FALSE);

		s += codesize(*s);
	}

	return d;
}

#define	PEND	2
#define	QEND	1
Static	Int	sameknj(p, plen, q, qlen)
Reg1	Uchar	*p;
Uchar	plen;
Reg2	Uchar	*q;
Uchar	qlen;
{
	Int	i;
	Int	j;
	Int	endf = 0;

	do {
		if ((i = codesize(*p)) != codesize(*q)) return FALSE;

		if ((*p & ~KanjiStrEnd) != (*q & ~KanjiStrEnd)) break;
		if ((i == 2) && (*(p + 1) != *(q + 1))) break;

		if (*p & KanjiStrEnd) endf += PEND;
		if (*q & KanjiStrEnd) endf += QEND;

		p += i;
		q += i;
	} while (!endf);

	switch (endf) {
	case (PEND | QEND):
		if (plen == qlen) return TRUE;
		break;

	case PEND:
		i = plen;
		plen = qlen;
		qlen = i;
		p = q;

	case QEND:
		if (!((j = *p) & KanjiStrEnd)) break;
		if ((j & KanjiModeMask) != ZenHiraAssyuku) break;
		if (qlen != plen - (j & KanjiCodeMask) - 1) break;
		return TRUE;

	default:
		if (!((i = *q) & KanjiStrEnd)) break;
		if (!((j = *p) & KanjiStrEnd)) break;
		if ((i & KanjiModeMask) != ZenHiraAssyuku) break;
		if ((j & KanjiModeMask) != ZenHiraAssyuku) break;
		if (plen - (j & KanjiCodeMask) != qlen - (i & KanjiCodeMask))
			break;
		return TRUE;
	}

	return FALSE;
}
#undef	PEND
#undef	QEND

Static	Int	diffknj(jrec, ptr, num)
JREC	*jrec;
Uchar	*ptr;
Int	num;
{
	KHREC	*kptr;
	JREC	*jptr;
	Int	i;
	Uchar	kbuf1[MaxWdKanjiLen];
	Uchar	kbuf2[MaxWdKanjiLen];

	if (jrec -> hinsi == TANKANJI) return TRUE;

	makekan(ptr, kbuf1, TRUE);

	for (i = 0, kptr = kouhotbl ; i < num ; i++, kptr++) {
		if ((jptr = kptr -> clrec -> jnode) == jrec)
			return FALSE;

		if (jptr -> hinsi == TANKANJI) continue;

		if (jptr -> class != jrec -> class) continue;

		if (jptr -> dicid != jrec -> dicid) continue;

		if (jptr -> jseg != jrec -> jseg) continue;

		if (jptr -> sttofs != jrec -> sttofs) continue;

		if (jptr -> stbofs != jrec -> stbofs) continue;

		makekan(dicbuf + kptr -> offs, kbuf2, TRUE);

		if (sameknj(kbuf1, jrec -> jlen, kbuf2, jptr -> jlen))
			return FALSE;
	}

	return TRUE;
}

Static	Int	chrck_numtbl(flg, cond)
Ushort	flg;
Ushort	cond;
{
	Ushort	must;

	if (cond = SelNumCond(cond)) {
		if (must = (cond & SelNumMust)) {
			if ((flg & must) != must) return FALSE;
			cond &= ~must;
		}
		if (cond && !(cond & flg)) return FALSE;
	}

	return TRUE;
}

Int	sel_sjmode(jrec)
JREC	*jrec;
{
	Ushort	i;
	Ushort	TFar	*p;
	Ushort	TFar	*q;

	switch (jrec -> class) {
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
		p = q = Selsjadrs(jrec->class - C_N_ARABIA);
		break;

	default:
		return SelArb;
	}

	while ((i = *p++) != SelNumTerm) {
		if (chrck_numtbl(jrec -> flags, i)) break;
	}

	if (i == SelNumTerm) i = *q;

	return SelNumFunc(i);
}

Static	Void	cl_numcmn(jrec, clrec)
JREC	*jrec;
CLREC	*clrec;
{
	Uchar	*p;
	Int	i;
	Ushort	j;
	Ushort	TFar	*tbl;

	p = cnvstart;
	if ((i = jrec -> sttofs) == SETTOU_DAI)
		p += SttYomiLen(Settou_ptr(i));
	else if (i == SETTOU_KIGOU)
		p++;

	if (jrec -> jofsst) {
		i = sel_sjmode(jrec);

		if (seldict(jrec -> dicid)) {
			(*curdict->getdic)(curdict, jrec -> jseg);

			p = dicbuf + jrec->jofsst + 1;

			while (*p != HinsiBlkTerm) {
				setkouho(clrec, (TypeDicOfs)(p - dicbuf), i);

				p = skipkstr(p);
			}
		}
		else {
			setkouho(clrec, (TypeDicOfs)1, i);
		}
	}
	else {
		tbl = Selsjadrs(jrec->class - C_N_ARABIA);
		while ((j = *tbl++) != SelNumTerm) {
			if (!chrck_numtbl(jrec -> flags, j)) continue;

			setkouho(clrec, (TypeDicOfs)0, (Int)SelNumFunc(j));
		}
	}
}

