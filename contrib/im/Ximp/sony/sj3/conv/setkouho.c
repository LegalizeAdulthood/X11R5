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

Int	sel_sjmode();

Int	hiraknj();

Void 	setkouho(clrec, offs, mode)
CLREC	 	*clrec;
TypeDicOfs	offs;
Int		mode;
{
	JREC	*jrec;
	KHREC	*kptr;
	Uchar	TFar	*fptr;
	Int	flg;
	Int	i;

	if (khcount >= MaxKouhoNumber) return;

	jrec = clrec -> jnode;

	kptr = &kouhotbl[khcount++];

	if (mode && !offs) nkhcount++;

	kptr -> clrec  = clrec;
	kptr -> offs   = offs;
	kptr -> rank   = 0;
	kptr -> styno  = 0;
	kptr -> mode   = (Uchar)mode;

	if (jrec -> flags & JFLAG_KA) {
		kptr -> ka_fg = 1;
		kptr -> ka_kj = 1;
		flg = 1;
	}
	else if (clrec -> fzk_ka) {
		kptr -> ka_fg = 1;
		kptr -> ka_kj = 0;
		flg = 2;
	}
	else {
		kptr -> ka_fg = 0;
		kptr -> ka_kj = 0;
		flg = 0;
	}

	kptr -> sttfg = kptr -> sttkj = 0;

	if (!(fptr = Settou_ptr(jrec -> sttofs))) return;

	if (!SttHiraKnj(fptr)) return;

	kptr -> sttfg = 1;
	kptr -> sttkj = 0;

	i = hiraknj(dicbuf + offs);
	if (offs > 1 && (i == 2 || i == 3)) return;


	if (khcount >= MaxKouhoNumber) return;

	kptr++;
	khcount++;

	if (mode && !offs) nkhcount++;

	kptr -> clrec  = clrec;
	kptr -> offs   = offs;
	kptr -> rank   = 0;
	kptr -> styno  = 0;
	kptr -> mode   = (Uchar)mode;

	kptr -> sttfg  = 1;
	kptr -> sttkj  = 1;

	switch (flg) {
	case 1:		kptr -> ka_fg = kptr -> ka_kj = 1; break;
	case 2:		kptr -> ka_fg = 1; kptr -> ka_kj = 0; break;
	default:	kptr -> ka_fg = kptr -> ka_kj = 0; break;
	}
}

Void	ph_setkouho(clrec, offs, sptr)
CLREC	 	*clrec;
TypeDicOfs	offs;
STDYIN		*sptr;
{
	JREC	*jrec;
	Uchar	TFar	*p;

	if (sptr) {
		stdytop = sptr;
		kouhotbl[0].styno = sptr -> styno;
		kouhotbl[0].sttkj = sptr -> sttkj;
	}
	else {
		kouhotbl[0].styno = -1;
		kouhotbl[0].sttkj = 0;
	}

	kouhotbl[0].clrec = clrec;
	kouhotbl[0].offs  = offs;
	kouhotbl[0].rank  = 0;

	kouhotbl[0].mode = sel_sjmode(jrec = clrec -> jnode);

	kouhotbl[0].sttfg = (Uchar)
		((p = Settou_ptr(jrec->sttofs)) ? (SttHiraKnj(p)?1:0) : 0);

	if (jrec -> flags & JFLAG_KA) {
		kouhotbl[0].ka_fg =
		kouhotbl[0].ka_kj = 1;
	}
	else if (clrec -> fzk_ka) {
		kouhotbl[0].ka_fg = 1;
		kouhotbl[0].ka_kj = 0;
	}
	else {
		kouhotbl[0].ka_fg =
		kouhotbl[0].ka_kj = 0;
	}

	khcount = 1;
}

Static	Int	hiraknj(p)
Reg1	Uchar	*p;
{
	Uchar	flg = TRUE;
	Int	i;
	Int	result;

	do {
		if (*p & KanjiStrEnd) flg = FALSE;

		if (result = (*Hiraknj_func[*p >> 4])(p, &i))
			return result;

		p += i;
	} while (flg);

	return 0;
}

Int	hiraknj_atrb(p, len)
Uchar	*p;
Int	*len;
{
	*len = 2;
	return 0;
}

Int	hiraknj_ofs(p, len)
Uchar	*p;
Int	*len;
{
	*len = 2;
	return hiraknj(dicbuf + ((*p & KanjiCodeMask) << 8) + *(p + 1));
}

Int	hiraknj_knj(p, len)
Uchar	*p;
Int	*len;
{
	*len = 1;
	return hiraknj(askknj[*p & KanjiCodeMask]);
}

Int	hiraknj_hask(p, len)
Uchar	*p;
Int	*len;
{
	*len = 1;
	return 2;
}

Int	hiraknj_kask(p, len)
Uchar	*p;
Int	*len;
{
	*len = 1;
	return 3;
}

Int	hiraknj_norm(p, len)
Uchar	*p;
Int	*len;
{
	*len = 2;
	return 1;
}

Int	hiraknj_hira(p, len)
Uchar	*p;
Int	*len;
{
	Uchar	ch;

	*len = 2;
	ch = *(p + 1);

	switch (*p & KanjiCodeMask) {
	case 0x02:
		if (ch >= 0x9f && ch <= 0xf1) return 2;
		return 1;

	case 0x03:
		if (ch >= 0x40 && ch <= 0x96) return 3;
		return 1;

	default:
		break;
	}
	return 1;
}

