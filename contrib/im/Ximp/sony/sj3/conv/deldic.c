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

Uchar	*skipkstr(), *get_idxptr();
Int	sstrlen(), srchkana(), srchgram();
Int	srchkanji(), cvtknj();
Uint	addel_arg();
Void	mvmemi(), memset(), set_size(), chg_uidx();
Void	mkidxtbl();
TypeDicSeg	srchidx();
TypeIdxOfs	count_uidx();

Int	del_douon(), del_segment();
Void	del_uidx(), del_stdy();

Uint	deldic(yomi, kanji, hinsi)
Uchar		*yomi;
Uchar		*kanji;
TypeGram	hinsi;
{
	Uchar		yptr[MaxWdYomiLen + 1];
	Uchar		kptr[MaxWdKanjiLen + 1];
	Ushort		err;
	Int		knjlen;
	TypeDicSeg	useg;
	TypeDicOfs	ofs;
	Uchar		*p1, *p2, *p3;
	Int		dblknum;
	Int		hblknum;
	Int		kblknum;
	Int		samlen;
	Uchar		*stp, *edp;
	Int		size, len;

	if (err = addel_arg(yomi, kanji, hinsi, yptr, sizeof(yptr)))
		return err;

	if (!curdict -> maxunit) return AD_NotUserDict;

	knjlen = cvtknj(yomi, kanji, kptr);

	cnvstart = yptr;
	cnvlen = sstrlen(yptr);

	useg = srchidx((TypeDicSeg)DicSegBase, cnvlen);
	(*curdict->getdic)(curdict, useg);

	if (!(dblknum = srchkana(&p1, &samlen))) return AD_NoMidasi;

	if (!(hblknum = srchgram(p1, &p2, hinsi))) return AD_NoHinsi;

	p3 = p2;
	kblknum = srchkanji(&p3, kptr, knjlen);
	if (*p3 == HinsiBlkTerm) return AD_NoKanji;
	ofs = p3 - dicbuf;

	edp = skipkstr(p3);

	if (kblknum == 1) {
		if (hblknum == 1) {
			if (dblknum == 1) {
				return del_segment(useg);
			}
			else {
				return del_douon(useg, p1, ofs);
			}
		}
		else {
			stp = p2;
			edp++;
		}
	}
	else {
		stp = p3;
	}

	size = edp - stp;

	len = dicbuf + (curdict->seglen) - edp;
	mvmemi(edp, stp, len);

	edp += len;
	memset(edp, DicSegTerm, dicbuf + curdict->seglen - edp);

	set_size(p1, (Int)(getsize(p1) - size),
			(Int)getplen(p1), (Int)getnlen(p1));

	(*curdict->putdic)(curdict, useg);

	del_stdy(useg, ofs, size);

	return AD_Done;
}

Static	Int	del_douon(seg, ptr, ofs)
TypeDicSeg	seg;
Uchar		*ptr;
TypeDicOfs	ofs;
{
	Uchar	*nxt, *p1, *p2;
	Int	size;
	Int	nlen, plen, len;

	nxt = getntag(ptr);

	if (segend(nxt)) {
		size = nxt - ptr;

		memset(ptr, DicSegTerm, dicbuf + curdict->seglen - nxt);
	}

	else {
		size = getsize(nxt);
		nlen = getnlen(nxt);
		plen = getplen(nxt);

		if (ptr == segtop()) {
			if (nlen + plen > count_uidx()
				+ sstrlen(get_idxptr(seg)))
				return AD_OvflwIndex;
		}

		len = ((len = getplen(ptr)) < plen) ? plen - len : 0;

		set_size(ptr, size + len, plen - len, nlen + len);

		p1 = nxt + DouonBlkSizeNumber;
		p2 = ptr + DouonBlkSizeNumber + len;

		mvmemi(p1, p2, dicbuf + curdict->seglen - p1);

		size = p1 - p2;
		memset(dicbuf + curdict->seglen - size, DicSegTerm, size);

		if (ptr == segtop()) {
			chg_uidx(seg, ptr + DouonBlkSizeNumber, nlen + len);
		}
	}

	(*curdict->putdic)(curdict, seg);

	del_stdy(seg, ofs, size);

	return AD_Done;
}

Static	Int	del_segment(seg)
TypeDicSeg	seg;
{
	Int		i;
	STDYIN		*styp;
	TypeDicID	dicid;
	TypeStyNum	stdynum;
	TypeDicSeg	sg;

	if (curdict->segunit > 1) {
		for (sg = seg + 1 ; sg < curdict->segunit ; sg++) {
			(*curdict->getdic)(curdict, sg);
			(*curdict->putdic)(curdict, sg - 1);
		}
		curdict->segunit--;
	}
	else {
		memset(dicbuf, DicSegTerm, (Int)curdict->seglen);
		dicbuf[0] = 0;
		(*curdict->putdic)(curdict, DicSegBase);
	}

	(*curdict->rszdic)(curdict, curdict->segunit);

	del_uidx(seg);

	if (StudyExist()) {
		dicid = curdict->dicid;
		for (i = 0, styp = StudyDict ; i < StudyCount ; ) {
			if (styp -> dicid != dicid)
				;
			else if (styp -> seg > seg)
				styp -> seg -= 1;
			else if (styp -> seg < seg)
				;
			else {
				stdynum = styp -> styno;
				StudyCount--;
				mvmemi((Uchar *)(styp + 1), (Uchar *)styp,
					sizeof(*styp) * (StudyCount - i));
				continue;
			}

			i++;
			styp++;
		}
		for (i = 0, styp = StudyDict ; i < StudyCount ; i++, styp++)
			if (styp -> styno > stdynum) styp -> styno -= 1;

		putstydic();
	}

	return AD_Done;
}

Static	Void	del_uidx(seg)
TypeDicSeg	seg;
{
	Uchar	*p, *q;
	Int	len;

	p = get_idxptr(seg);
	for (q = p ; *q++; )
		;

	mvmemi(q, p, idxbuf + curdict->idxlen - q);

	len = q - p;
	memset(idxbuf + curdict->idxlen - len, 0, len);

	(*curdict->putidx)(curdict, 0);

	mkidxtbl(curdict);
}

Static	Void	del_stdy(seg, ofs, size)
TypeDicSeg	seg;
TypeDicOfs	ofs;
Int		size;
{
	Int		i;
	TypeStyNum	stdynum;
	STDYIN		*stdy;
	TypeDicID	dicid;

	if (StudyExist()) {

		stdynum = 0;
		dicid = curdict->dicid;
		for (i = 0, stdy = StudyDict ; i < StudyCount ; ) {
			if (stdy -> dicid != dicid)
				;
			else if (stdy -> seg != seg)
				;
			else if (stdy -> offset > ofs)
				stdy -> offset -= size;

			else if (stdy -> offset == ofs) {
				stdynum = stdy -> styno;
				StudyCount--;
				mvmemi((Uchar *)(stdy + 1), (Uchar *)stdy,
					sizeof(*stdy) * (StudyCount - i));
				continue;
			}

			i++;
			stdy++;
		}
		for (i = 0, stdy = StudyDict ; i < StudyCount ; i++, stdy++)
			if (stdy -> styno > stdynum) (stdy -> styno)--;

		putstydic();
	}
}

