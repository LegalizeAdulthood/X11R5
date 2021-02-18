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
#include "sj_yomicode.h"

Uchar	*get_idxptr();
Int	sj2cd_str(), yomicmp();
Int	sstrlen(), sstrcmp();
Void	mvmemi(), mvmemd(), mkidxtbl();

Void	set_size(p, size, plen, nlen)
Reg1	Uchar	*p;
Int	size;
Int	plen;
Int	nlen;
{
	*p = (Uchar)(size >> 8);
	if (nlen & 0x10) *p |= NokoriYomiTop;
	if (plen & 0x10) *p |= AssyukuYomiTop;
	*(p + 1) = (Uchar)size;
	*(p + 2) = (Uchar)((nlen & 0x0f) | ((plen & 0x0f) << 4));
}

Static	Int	isvyomi(yp)
Reg1	Uchar	*yp;
{
	Reg2	Int	len;

	if (!*yp) return FALSE;

	if (!(Chrtbl[*yp] & DICTOP)) return FALSE;

	for (len = 0 ; *yp ; yp++) {
		if (!(Chrtbl[*yp]) || (*yp >= Y_KUTEN)) return FALSE;

		if (++len > MaxWdYomiLen) return FALSE;
	}

	return TRUE;
}

Static	Int	isvknj(kanji)
Reg1	Uchar	*kanji;
{
	Reg2	Int	len;

	if (!*kanji) return FALSE;

	for (len = 0 ; *kanji ; ) {
		if (isknj1(*kanji)) {
			kanji += 2;
			len += 2;
		}
		else {
			kanji++;
			len += 2;
		}

		if (len > MaxWdKanjiLen) return FALSE;
	}

	return TRUE;
}

Static	Int	isgrm(gram)
TypeGram	gram;
{
	Reg1	Int	grm = gram;

	if (MEISI_1    <= grm && grm <= D_MEISI_6)  return TRUE;
	if (MYOUJI     <= grm && grm <= JOSUUSI2)   return TRUE;
	if (KEIYOUSI_1 <= grm && grm <= DO_ZAHEN)   return TRUE;
	if (DO_1DAN_1  <= grm && grm <= DO_WAGO_2)  return TRUE;
	if (DO_KAGO_5  <= grm && grm <= DO_WAGO_5)  return TRUE;
	if (DO_KAGO_6  <= grm && grm <= DO_WAGO_6)  return TRUE;
	if (DO_1DAN_3  <= grm && grm <= DO_WAGO_3)  return TRUE;
	if (DO_1DAN_4  <= grm && grm <= DO_WAGO_4)  return TRUE;
	if (DO_KAGO_7  <= grm && grm <= DO_WAGO_7)  return TRUE;
	if (DO_KAGO_8  <= grm && grm <= DO_WAGO_8)  return TRUE;
	if (SP_SA_MI1  <= grm && grm <= TANKANJI)   return TRUE;

	return FALSE;
}

Uint	addel_arg(yp, kp, grm, nyp, len)
Uchar	*yp;
Uchar	*kp;
TypeGram	grm;
Uchar	*nyp;
Int	len;
{
	Reg1	Ushort	err = 0;

	if (!sj2cd_str(yp, nyp, len)) err |= AD_BadYomi;

	if (!isvyomi(nyp)) err |= AD_BadYomi;

	if (!isvknj(kp)) err |= AD_BadKanji;

	if (!isgrm(grm)) err |= AD_BadHinsi;

	return err;
}

Static	Void	yomi2kata(src, dst)
Reg2	Uchar	*src;
Reg1	Uchar	*dst;
{
	Reg3	Int	first;
	Reg4	Int	second;

	while (first = *src++) {
		second = *src++;

		if ((first != 0x82) || (second < 0x9f)) {
			*dst++ = first;
			*dst++ = second;
		}

		else {
			*dst++ = 0x83;

			if (second < 0xde)
				*dst++ = (second - 0x9f + 0x40);

			else
				*dst++ = (second - 0xde + 0x80);
		}
	}

	*dst = 0;
}

Static	Int	top_strcmp(src, dst)
Reg1	Uchar	*src;
Reg3	Uchar	*dst;
{
	Reg2	Int	i;

	for (i = 0 ; *src && (*src == *dst) ; src++, dst++) i++;

	i /= 2;

	return (i > MaxYomiAskNumber) ? MaxYomiAskNumber : i;
}

Static	Int	last_strcmp(src, dst)
Uchar	*src;
Uchar	*dst;
{
	Int	slen;
	Reg1	Int	dlen;

	slen = sstrlen(src);
	dlen = sstrlen(dst);

	if (dlen > MaxYomiAskNumber * 2) return 0;

	if (sstrcmp(src + (slen - dlen), dst)) return 0;

	return (dlen / 2);
}

Int	cvtknj(yomi, knj, dst)
Uchar	*yomi;
Uchar	*knj;
Reg1	Uchar	*dst;
{
	Uchar	kana[MaxWdYomiLen * 2 + 1];
	Reg2	Int	len;
	Uchar	*keep;

	keep = dst;
	yomi2kata(yomi, kana);

	if (len = top_strcmp(yomi, knj))
		*dst = (Uchar)(ZenHiraAssyuku | (len - 1));

	else if (len = top_strcmp(kana, knj))
		*dst = (Uchar)(ZenKataAssyuku | (len - 1));

	if (len) {
		knj += len * 2;

		if (!*knj) *dst |= KanjiStrEnd;

		dst++;
	}

	if (*knj) {
		for ( ; ; ) {
			if (len = last_strcmp(yomi, knj)) {
				*dst++ = (Uchar)(ZenHiraAssyuku | (len - 1) |
							KanjiStrEnd);
				break;
			}

			else if (len = last_strcmp(kana, knj)) {
				*dst++ = (Uchar)(ZenKataAssyuku | (len - 1) |
							KanjiStrEnd);
				break;
			}

			else if (isknj1(*knj)) {
				*dst++ = (*knj++ & ~KanjiStrEnd);
				*dst++ = *knj++;
			}

			else {
				*dst++ = (LeadingHankaku & ~KanjiStrEnd);
				*dst++ = *knj++;
			}

			if (!*knj) {
				*(dst - 2) |= KanjiStrEnd;
				break;
			}
		}
	}

	*dst = 0;

	return (dst - keep);
}

Int	srchkana(ptr, saml)
Uchar	**ptr;
Int	*saml;
{
	Uchar	*p;
	Int	cmp;
	Int	cnt = 0;
	Uchar	same;
	Reg1	Int	flg = 0;

	same = 0;

	p = segtop();

	while (!segend(p)) {

		if (flg == 0) {

			cmp = yomicmp(cnvstart, p, &same);

			if (cmp == OVER) break;

			if (cmp == MATCH && *(cnvstart + same) == 0) {
				*ptr = p;
				*saml = same;
				flg = 1;
			}
		}

		p = getntag(p);
		cnt++;
	}

	if (flg != 0) return cnt;

	*ptr = p;
	*saml = same;
	return 0;
}

Int	srchgram(ptr, dst, hinsi)
Reg1	Uchar	*ptr;
Uchar	**dst;
TypeGram	hinsi;
{
	Uchar	*endp;
	Int	cnt = 0;
	Int	flg = FALSE;

	endp = getntag(ptr);

	ptr += DouonBlkSizeNumber + getnlen(ptr);
	while (ptr < endp) {
		if (*ptr == hinsi) {
			*dst = ptr;
			flg = TRUE;
		}

		do {
			ptr++;
		} while (*ptr != HinsiBlkTerm);
		ptr++;
		cnt++;
	}

	if (flg) return cnt;

	*dst = ptr;
	return 0;
}

Int	srchkanji(dst, knj, klen)
Uchar	**dst;
Uchar	*knj;
Int	klen;
{
	Reg1	Uchar	*ptr;
	Uchar	*p, *q;
	Int	len;
	Int	flg = FALSE;
	Int	cnt = 0;

	ptr = *dst + 1;

	while (*ptr != HinsiBlkTerm) {
		for (p = ptr, q = knj, len = klen ; len ; len--)
			if (*p++ != *q++) break;

		if (!len) {
			*dst = ptr;
			flg = TRUE;
		}

		while (!(*ptr & KanjiStrEnd)) {
			ptr += codesize(*ptr);
		}
		ptr += codesize(*ptr);
		cnt++;
	}

	if (!flg) *dst = ptr;

	return cnt;
}

TypeIdxOfs	count_uidx()
{
	Uchar	*p;

	p = get_idxptr(curdict->segunit - 1);
	while (*p++) ;

	return (curdict->idxlen - (p - idxbuf));
}

Void	chg_uidx(seg, yomi, len)
TypeDicSeg	seg;
Uchar	*yomi;
Int	len;
{
	Uchar		*p, *q;
	Int		olen;

	p = get_idxptr(seg);

	olen = sstrlen(p);

	q = idxbuf + curdict->idxlen;

	if (len > olen) {
		mvmemd(q - (len-olen), q, (Int)(q - (len-olen) - p));
	}
	else {
		mvmemi(p + (olen-len), p, (Int)(q - (p + (olen-len))));
	}

	while (len--) *p++ = *yomi++;

	(*curdict->putidx)(curdict, 0);

	mkidxtbl(curdict);
}

