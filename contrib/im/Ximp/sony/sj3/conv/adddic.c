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

Uchar	*srchdict(), *skipkstr(), *get_idxptr();
Int	sstrncmp(), study(), cmpstr();
Int	cvtknj();
Int	srchkana(), srchgram(), getkanji();
Int	sstrlen(), srchkanji(), usr_freelen();
Uint	addel_arg();
Void	memset(), mvmemi(), mkclidx(), mkidxtbl();
Void	mvmemd(), set_size(), delclsub(), apnd_uidx();
TypeDicSeg	srchidx();
TypeIdxOfs	count_uidx();

Void	sprt_seg(), chg_uidx();
Int	cal_nextym();
Uint	checkdict();

Uint	adddic(yomi, kanji, hinsi)
Uchar		*yomi;
Uchar		*kanji;
TypeGram	hinsi;
{
	Uint		err;
	Uchar		yptr[MaxWdYomiLen+1];
	Uchar		kptr[MaxWdKanjiLen+1];
	Int		klen;
	TypeDicSeg	segnum;
	Int		saml;
	TypeDicOfs	dstofs;
	TypeDicOfs	douofs;
	TypeDicOfs	hnsofs;
	TypeDicOfs	knjofs;
	TypeDicOfs	nxtask;
	TypeDicOfs	newsiz;
	TypeDicOfs	oldsiz;
	TypeDicOfs	fresiz;
	TypeIdxOfs	freidx;
	Uchar		*douptr;
	Uchar		*dstptr;
	TypeDicOfs	pos;
	Uchar		*p1;
	Int		i;
	Int		nlen, plen;
	Int		size;
	STDYIN	*stdy;
	STDYOUT	stdydat;

	if (err = addel_arg(yomi, kanji, hinsi, yptr, sizeof(yptr)))
		return err;

	inputyomi = yomi;
	cnvstart = ystart = yptr;
	cnvlen = sstrlen(yptr);

	if (err = checkdict(kanji, hinsi)) return err;

	if (!curdict -> maxunit) return AD_NotUserDict;

	klen = cvtknj(yomi, kanji, kptr);

	newsiz = DouonBlkSizeNumber + cnvlen + klen + 2;

	if (newsiz > MaxDouonBlk) return AD_OvflwDouBlk;
	if (newsiz > curdict->seglen) return AD_OvflwDouBlk;

	segnum = srchidx((TypeDicSeg)DicSegBase, cnvlen);

	for ( ; ; ) {
		(*curdict->getdic)(curdict, segnum);
		i = srchkana(&p1, &saml);
		dstofs = douofs = p1 - dicbuf;
		hnsofs = knjofs = 0;
		if (i) {
			i = srchgram(p1, &p1, hinsi);
			dstofs = hnsofs = p1 - dicbuf;
			if (i) {
				srchkanji(&p1, kptr, klen);
				if (*p1 != HinsiBlkTerm) return AD_ArExist;
				dstofs = knjofs = p1 - dicbuf;
			}
		}

		p1 = dicbuf + douofs;

		if (!hnsofs) {
			oldsiz = 0;
			newsiz = DouonBlkSizeNumber + cnvlen - saml;

			nxtask = cal_nextym(p1);
		}
		else {
			oldsiz = newsiz = getsize(p1);
			nxtask = 0;
		}
		if (!knjofs) newsiz += 2;
		newsiz += klen;

		fresiz = usr_freelen();

		if (newsiz - oldsiz - nxtask <= fresiz) break;

		if (segend(p1) && (segnum + 1 < curdict->segunit)) {
			segnum++;
			continue;
		}

		if (curdict->segunit >= curdict->maxunit)
			return AD_OvflwUsrDic;

		freidx = count_uidx();

		if (douofs + newsiz <= curdict->seglen) {
			if (hnsofs) p1 = getntag(p1);
			pos = p1 - dicbuf;

			if (freidx < getplen(p1) + getnlen(p1) + 1)
				return AD_OvflwIndex;

			if ((*curdict->putdic)(curdict, curdict->segunit))
				return AD_OvflwUsrDic;

			sprt_seg(segnum, pos);

			nxtask = 0;
			break;
		}

		if (segend(p1)) {
			if (freidx < cnvlen + 1) return AD_OvflwIndex;

			if ((*curdict->putdic)(curdict, curdict->segunit))
				return AD_OvflwUsrDic;
			curdict->segunit++;

			(*curdict->getdic)(curdict, curdict->segunit - 1);
			memset(dicbuf, DicSegTerm, (Int)curdict->seglen);
			dicbuf[0] = 0;
			(*curdict->putdic)(curdict, curdict->segunit - 1);
			(*curdict->rszdic)(curdict, curdict->segunit);
			mkidxtbl(curdict);

			segnum++;
		}

		else {
			if (freidx < getplen(p1) + getnlen(p1) + 1)
				return AD_OvflwIndex;

			if ((*curdict->putdic)(curdict, curdict->segunit))
				return AD_OvflwUsrDic;

			sprt_seg(segnum, douofs);

			segnum++;
		}
	}

	douptr = dicbuf + douofs;

	if (nxtask) {
		Uchar	*tp;

		size = getsize(douptr) - nxtask;
		plen = getplen(douptr) + nxtask;
		nlen = getnlen(douptr) - nxtask;
		*douptr = (size >> 8);
		if (nlen & 0x10) *douptr |= NokoriYomiTop;
		if (plen & 0x10) *douptr |= AssyukuYomiTop;
		*(douptr + 1) = size;
		*(douptr + 2) = ((nlen & 0x0f) | ((plen & 0x0f) << 4));

		p1 = douptr + DouonBlkSizeNumber;
		tp = p1 + nxtask;
		mvmemi(tp, p1, dicbuf + curdict->seglen - tp);
	}

	dstptr = dicbuf + dstofs;
	size = newsiz - oldsiz;
	p1 = dicbuf + curdict->seglen;
	mvmemd(p1 - size, p1, p1 - dstptr - size);

	if (!hnsofs) {
		if (douofs == segtop() - dicbuf) {
			chg_uidx(segnum, yptr, cnvlen);
		}

		*dstptr   = 0;
		if ((cnvlen - saml) & 0x10) *dstptr |= NokoriYomiTop;
		if (saml & 0x10) *dstptr |= AssyukuYomiTop;
		*dstptr++ |= (size >> 8);
		*dstptr++  = size;
		*dstptr++  = (((cnvlen-saml)&0x0f)|((saml&0x0f)<<4));

		while (saml < cnvlen) *dstptr++ = yptr[saml++];
	}
	else {
		plen = getplen(douptr);
		nlen = getnlen(douptr);
		*douptr = (newsiz >> 8);
		if (nlen & 0x10) *douptr |= NokoriYomiTop;
		if (plen & 0x10) *douptr |= AssyukuYomiTop;
		*(douptr + 1) = newsiz;
		*(douptr + 2) = ((nlen & 0x0f) | ((plen & 0x0f) << 4));
	}

	if (!knjofs) *dstptr++ = hinsi;

	stdydat.stdy1.offset = dstptr - dicbuf;
	for (saml = 0 ; saml < klen ; ) *dstptr++ = kptr[saml++];

	if (!knjofs) *dstptr++ = HinsiBlkTerm;

	(*curdict->putdic)(curdict, segnum);

	if (StudyExist()) {
		size -= nxtask;
		for (i = StudyCount, stdy = StudyDict ; i-- ; stdy++) {
			if (stdy -> dicid != curdict->dicid) continue;
			if (stdy -> seg != segnum) continue;
			if (stdy -> offset >= dstofs) stdy -> offset += size;
		}

		for (p1 = ClStudyDict ; !iseocl(p1) ; p1 = ClNextTag(p1)) {
			err = sstrncmp(yptr, (Uchar TFar *)ClYomiPos(p1),
					(Int)ClYomiLen(p1));
			if (err == MATCH) {
				delclsub(p1);
				mkclidx();
				putcldic();
				break;
			}
			else if (err == OVER)
				break;
		}
	}

	stdydat.stdy1.seg    = segnum;
	stdydat.stdy1.dicid  = curdict->dicid;
	stdydat.stdy1.styno  = 0;
	stdydat.stdy1.sttkj  = 0;
	stdydat.stdy1.ka_kj  = 0;
	stdydat.stdy1.nmflg  = 0;
	stdydat.hinshi       = hinsi;
	stdydat.len          = cnvlen;
	stdydat.sttlen       = 0;
	stdydat.sttfg        = 0;
	stdydat.ka_fg        = 0;
	(Void)study(&stdydat);

	return AD_Done;
}

Static	Int	checksub(kanji, grm)
Uchar		*kanji;
TypeGram	grm;
{
	Reg1	Uchar	*tagp;
	Reg2	Uchar	*ptr;
	Reg3	Uchar	*endp;
	Int	flg;
	Int	dounum;
	Uchar	buf[MaxWdKanjiLen + 1];

	dicinl = cnvlen;
	dicsaml = 0;
	prevseg = (TypeDicSeg)-1;
	for (;;) {
		if (!(tagp = srchdict(tagp))) return 0;
		if (cnvlen == getnlen(tagp) + getplen(tagp))
			break;
	}

	dounum = 0;

	ptr = tagp + DouonBlkSizeNumber + getnlen(tagp);

	endp = tagp + getsize(tagp);

	while (ptr < endp) {

		flg = (grm == *ptr++);
		while (*ptr != HinsiBlkTerm) {

			if (flg) {

				buf[getkanji(inputyomi, cnvlen, ptr, buf)] = 0;

				if (cmpstr(kanji, buf)) return -1;
			}

			dounum++;
			ptr = skipkstr(ptr);
		}

		ptr++;
	}

	return dounum;
}

Static	Uint	checkdict(kanji, grm)
Uchar		*kanji;
TypeGram	grm;
{
	Reg1	DICTL	*dp;
	Reg2	Int	dounum = 0;
	Reg3	Int	tmp;
	DICT	*tmpdict;


	tmpdict = curdict;
	for (dp = dictlist ; dp ; dp = dp -> next) {

		curdict = dp -> dict;

		if ((tmp = checksub(kanji, grm)) < 0) {
			curdict = tmpdict;
			return AD_ArExist;
		}

		dounum += tmp;
	}
	curdict = tmpdict;


	if (MaxKouhoNumber <= dounum) return AD_OvflwDouNum;

	return 0;
}

Static	Int	cal_nextym(ptr)
Reg1	Uchar	*ptr;
{
	Reg2	Int	count = 0;
	Reg3	Uchar	*src;
	Reg4	Int	len;

	if (!segend(ptr)) {

		src = cnvstart + getplen(ptr);

		len = getnlen(ptr);
		ptr += DouonBlkSizeNumber;

		while (len--) {
			if (*ptr++ == *src++)
				count++;
			else
				break;
		}
	}

	return count;
}

Static	Int	usr_freelen()
{
	Reg1	Uchar	*ptr;

	ptr = segtop();
	while (!segend(ptr)) ptr += getsize(ptr);

	return(curdict->seglen - (ptr - dicbuf));
}

Static	Void	sprt_seg(seg, ofs)
TypeDicSeg	seg;
TypeDicOfs	ofs;
{
	Reg3	TypeDicSeg	s;
	Uchar		*pos;
	Reg1	Uchar	*p;
	Reg2	Uchar	*q;
	Int		i;
	Int		j;
	Int		ylen;
	TypeDicID	dicid;
	Uchar		yomi[MaxWdYomiLen];
	STDYIN		*stdy;

	for (s = curdict->segunit - 1 ; s >= seg ; s--) {
		(*curdict->getdic)(curdict, s);
		(*curdict->putdic)(curdict, s + 1);
	}
	curdict->segunit++;
	(*curdict->rszdic)(curdict, curdict->segunit);

	(*curdict->getdic)(curdict, seg + 1);

	pos = dicbuf + ofs;

	for (p = segtop() ; (p <= pos) && !segend(p) ; p = q) {
		q = getntag(p);
		i = getplen(p);
		j = getnlen(p);
		p += DouonBlkSizeNumber;
		while (j-- > 0) yomi[i++] = *p++;
	}

	ylen = (i = getplen(pos)) + (j = getnlen(pos));

	apnd_uidx(seg, yomi, ylen);

	set_size(segtop(), getsize(pos) + i, 0, ylen);

	mvmemi(yomi, (p = segtop() + DouonBlkSizeNumber), ylen);

	q = pos + DouonBlkSizeNumber + j;
	i = dicbuf + curdict->seglen - q;
	mvmemi(q, p += ylen, i);
	j = q - p;

	p += i;
	memset(p, DicSegTerm, (dicbuf + curdict->seglen - p));

	(*curdict->putdic)(curdict, seg + 1);

	if (StudyExist()) {
		dicid = curdict -> dicid;
		for (i = StudyCount, stdy = StudyDict ; i-- > 0 ; stdy++) {
			if (stdy -> dicid != dicid)
				continue;
			else if (stdy -> seg < seg)
				continue;
			else if (stdy -> seg > seg)
				stdy -> seg += 1;
			else if (stdy -> offset >= ofs) {
				stdy -> seg += 1;
				stdy -> offset -= j;
			}
		}
	}

	(*curdict->getdic)(curdict, seg);
	memset(dicbuf + ofs, DicSegTerm, (Int)(curdict->seglen - ofs));
	(*curdict->putdic)(curdict, seg);
}

Static	Void	apnd_uidx(seg, yomi, len)
TypeDicSeg	seg;
Uchar		*yomi;
Int		len;
{
	Reg1	Uchar	*p;
	Reg1	Uchar	*q;

	p = get_idxptr(seg);
	while (*p++) ;

	q = idxbuf + curdict->idxlen;
	mvmemd(q - (len + 1), q, (Int)(q - (len + 1) - p));

	while (len--) *p++ = *yomi++;
	*p = 0;

	(*curdict->putidx)(curdict, 0);

	mkidxtbl(curdict);
}

