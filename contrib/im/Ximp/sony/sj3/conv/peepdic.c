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
#include "sj_yomicode.h"

Uchar	*skipkstr(), *skiphblk(), *get_idxptr();
Int	getkanji();

Int	prev_kanji(), prev_hinsi(), prev_douon();
Int	next_kanji(), next_hinsi(), next_douon();
Void	set_kanji(), set_buf(), add_yomi(), cd2sjh_chr();
Void	set_idxyomi();

Int	getusr(buf)
Uchar	*buf;
{
	peepyomi[0] = peepknj[0] = peepgrm = 0;

	(*curdict->getdic)(curdict, peepidx = DicSegBase);
	get_askknj();

	set_idxyomi();

	peepdptr = segtop();

	if (segend(peepdptr)) return 0;

	add_yomi();

	peephptr = peepdptr + DouonBlkSizeNumber + getnlen(peepdptr);

	peepgrm = *peephptr;

	peepkptr = peephptr + 1;

	set_kanji();

	set_buf(buf);

	return -1;
}

Int	nextusr(buf)
Uchar	*buf;
{
	(*curdict->getdic)(curdict, peepidx);
	get_askknj();

	if (next_kanji()) {
		set_kanji();

		set_buf(buf);

		return -1;
	}

	return 0;
}

Int	prevusr(buf)
Uchar	*buf;
{
	(*curdict->getdic)(curdict, peepidx);
	get_askknj();

	if (prev_kanji()) {
		set_kanji();

		set_buf(buf);

		return -1;
	}

	return 0;
}

Static	Void	set_kanji()
{
	Int	len;

	len = getkanji(peepyomi, getnlen(peepdptr) + getplen(peepdptr),
			peepkptr, peepknj);
	*(peepknj + len) = 0;
}

Static	Void	set_buf(buf)
Reg2	Uchar	*buf;
{
	Reg1	Uchar	*p;

	for (p = peepyomi ; *p ; ) *buf++ = *p++;
	*buf++ = 0;

	for (p = peepknj ; *p ; ) *buf++ = *p++;
	*buf++ = 0;

	*buf++ = peepgrm;
	*buf++ = 0;
}

Static	Int	prev_kanji()
{
	Uchar	*p1;
	Uchar	*p2;

	p1 = peephptr + 1;
	if (peepkptr <= p1) return prev_hinsi();

	p2 = peepkptr;
	while (p1 < p2) {
		peepkptr = p1;
		p1 = skipkstr(p1);
	}

	return -1;
}

Static	Int	prev_hinsi()
{
	Uchar	*p1;
	Uchar	*p2;

	p1 = peepdptr + DouonBlkSizeNumber + getnlen(peepdptr);
	if (peephptr <= p1) return prev_douon();

	p2 = peephptr;
	while (p1 < p2) {
		peephptr = p1;
		p1 = skiphblk(p1);
	}

	peepgrm = *peephptr;

	p1 = peephptr + 1;
	while (*p1 != HinsiBlkTerm) {
		peepkptr = p1;

		p1 = skipkstr(p1);
	}

	return -1;
}

Static	Int	prev_douon()
{
	Uchar	*p1;
	Uchar	*p2;

	if (peepdptr <= segtop()) {
		if (peepidx <= DicSegBase) return 0;

		(*curdict->getdic)(curdict, --peepidx);
		get_askknj();

		set_idxyomi();

		p1 = segtop();
		do {
			peepdptr = p1;

			add_yomi();

			p1 = getntag(p1);
		} while (!segend(p1));
	}
	else {
		set_idxyomi();

		p1 = segtop();
		p2 = peepdptr;
		do {
			peepdptr = p1;

			add_yomi();

			p1 = getntag(p1);
		} while (p1 < p2);
	}

	p1 = peepdptr + DouonBlkSizeNumber + getnlen(peepdptr);

	p2 = getntag(peepdptr);
	do {
		peephptr = p1;
		p1 = skiphblk(p1);
	} while (p1 < p2);

	peepgrm = *peephptr;

	p1 = peephptr + 1;
	do {
		peepkptr = p1;

		p1 = skipkstr(p1);
	} while (*p1 != HinsiBlkTerm);

	return -1;
}

Static	Int	next_kanji()
{
	Uchar	*p1;

	p1 = skipkstr(peepkptr);

	if (*p1 == HinsiBlkTerm) return next_hinsi();

	peepkptr = p1;

	return -1;
}

Static	Int	next_hinsi()
{
	Uchar	*p1;

	p1 = skiphblk(peephptr);

	if (p1 >= getntag(peepdptr)) return next_douon();

	peephptr = p1;

	peepgrm = *p1;

	peepkptr = p1 + 1;

	return -1;
}

Static	Int	next_douon()
{
	Uchar	*p1;

	p1 = getntag(peepdptr);

	if (segend(p1)) {
		if (peepidx + 1 < curdict->segunit) {
			(*curdict->getdic)(curdict, ++peepidx);
			get_askknj();
			peepdptr = segtop();

			set_idxyomi();
		}
		else
			return 0;
	}
	else {
		peepdptr = p1;
	}

	add_yomi();

	peephptr = peepdptr + DouonBlkSizeNumber + getnlen(peepdptr);

	peepgrm = *peephptr;

	peepkptr = peephptr + 1;

	return -1;
}

Static	Void	set_idxyomi()
{
	Uchar	*p1, *p2;

	if (p2 = get_idxptr(peepidx)) {
		p1 = peepyomi;
		while (*p2) {
			cd2sjh_chr(*p2++, p1);
			p1 += 2;
		}
		*p1 = 0;
	}
}

Static	Void	add_yomi()
{
	Int	nlen;
	Uchar	*p1, *p2;

	nlen = getnlen(peepdptr);

	p1 = peepyomi + getplen(peepdptr) * 2;

	p2 = peepdptr + DouonBlkSizeNumber;

	while (nlen--) {
		cd2sjh_chr(*p2++, p1);
		p1 += 2;
	}

	*p1 = 0;
}

Static	Void	cd2sjh_chr(ch, dst)
Uchar	ch;
Reg1	Uchar	*dst;
{
	if (ch == _TYOUON) {
		*dst++ = 0x81;
		*dst++ = 0x5b;
	}
	else if (ch == _IGETA) {
		*dst++ = 0x81;
		*dst++ = 0x94;
	}
	else if (ch == _ATTO)  {
		*dst++ = 0x81;
		*dst++ = 0x97;
	}
	else if (ch == _YUUBIN) {
		*dst++ = 0x81;
		*dst++ = 0xa7;
	}
	else if (ch < N_0) {
	}
	else if (ch <= N_9) {
		*dst++ = 0x82;
		*dst++ = ch + 0x3F;
	}
	else if (ch <= A_Z) {
		*dst++ = 0x82;
		*dst++ = ch + 0x46;
	}
	else if (ch <= A_z) {
		*dst++ = 0x82;
		*dst++ = ch + 0x4D;
	}
	else if (ch <= _NN) {
		*dst++ = 0x82;
		*dst++ = ch + 0x51;
	}
	else if (ch <= _XKE) {
		*dst++ = 0x83;
		*dst++ = ch - 0x0d;
	}
	else {
	}
}

