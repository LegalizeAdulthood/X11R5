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

Int	sj2cd_str(), sstrlen();
Void	memcpy(), memset(), mvmemd(), mvmemi();

Uchar	*delcldata(), *srchclpos();
Void	delclsub(), mkclidx();
Int	delclold();

Int	clstudy(yomi1, yomi2, stdy)
Uchar	*yomi1;
Uchar	*yomi2;
STDYOUT	*stdy;
{
	Uchar	ytmp[CLSTDY_TMPLEN];
	Uchar	*dst, *ptr;
	Int	ret;
	Int	len1, len2;
	Int	alen, blen;
	Int	i;

	if (!StudyExist()) return ClstudyNoDict;

	if (!yomi1 || !*yomi1) return ClstudyParaErr;

	if (!yomi2 || !*yomi2) {

		if (!sj2cd_str(yomi1, ytmp, CLSTDY_TMPLEN))
			return ClstudyYomiErr;

		(Void)delcldata(ytmp);

		ret = ClstudyNormEnd;
	}

	else if (!stdy)
		return ClstudyParaErr;

	else {
		if (!sj2cd_str(yomi1, ytmp, CLSTDY_TMPLEN))
			return ClstudyYomiErr;
		len1 = sstrlen(ytmp);

		if (!sj2cd_str(yomi2, ytmp + len1, CLSTDY_TMPLEN - len1))
			return ClstudyYomiErr;
		len2 = len1 + stdy -> len;

		ytmp[len2] = 0;

		alen = len2 + CL_HDLEN;

		if (alen + 1 >= ClStudyLen) return ClstudyNoArea;

		blen = ClStudyTail - delcldata(ytmp) - 1;

		while (blen < alen) {
			if (!(i = delclold())) break;
			blen += i;
		}

		if (blen >= alen) {
			dst = srchclpos(ytmp);

			ptr = ClStudyTail - alen;
			mvmemd(ptr, ClStudyTail, (ptr - dst));
			ClYomiLen(dst)  = (Uchar)len2;
			ClGramCode(dst) = stdy -> hinshi;
			ClYomi1Len(dst) = (Uchar)len1;
			ClSetNum(dst, 1);
			memcpy(ytmp, ClYomiPos(dst), len2);

			ret = ClstudyNormEnd;
		}
		else
			ret = ClstudyNoArea;
	}

	mkclidx();

	putcldic();

	return ret;
}

Static	Uchar	*delcldata(ycode)
Uchar	*ycode;
{
	Reg1	Uchar	*tagptr;
	Reg2	Uchar	*ptr1;
	Reg3	Uchar	*ptr2;
	Int	len1;

	tagptr = ClStudyDict;
	while (!iseocl(tagptr)) {
		ptr1 = ycode;
		ptr2 = ClYomiPos(tagptr);
		len1 = ClYomiLen(tagptr);

		while (*ptr1 && len1) {
			if (*ptr1 != *ptr2) break;
			ptr1++;
			ptr2++;
			len1--;
		}

		if (!*ptr1 || !len1) {
			delclsub(tagptr);

		}
		else {
			tagptr = ClNextTag(tagptr);
		}
	}

	return tagptr;
}

Void	delclsub(target)
Uchar	*target;
{
	Reg1	Uchar	*src;
	Int	num;
	Int	tmp;
	Int	len;

	num = ClGetNum(target);

	len = ClBlkLen(target);

	src = target + len;

	mvmemi(src, target, (ClStudyTail - src));

	memset(ClStudyTail - len, 0, len);

	src = ClStudyDict;
	while (!iseocl(src)) {
		tmp = ClGetNum(src);

		if (tmp > num) {
			tmp -= 1;
			ClSetNum(src, tmp);
		}

		src = ClNextTag(src);
	}
}

Static	Int	delclold()
{
	Reg1	Uchar	*tagptr;
	Reg2	Uchar	*target;
	Int	num;
	Int	tmp;

	target = NULL;
	num = 0;

	tagptr = ClStudyDict;
	while (!iseocl(tagptr)) {

		tmp = ClGetNum(tagptr);

		if (tmp >= num) {
			num = tmp;
			target = tagptr;
		}

		tagptr = ClNextTag(tagptr);
	}

	if (target) {
		num = ClBlkLen(tagptr);

		delclsub(target);
	}

	return num;
}

Static	Uchar	*srchclpos(ycode)
Uchar	*ycode;
{
	Reg1	Uchar	*tagptr;
	Reg3	Uchar	*target;
	Reg2	Uchar	*ptr1;
	Reg4	Uchar	*ptr2;
	Reg5	Int	len;
	Int	tmp;

	target = NULL;
	tagptr = ClStudyDict;
	while (!iseocl(tagptr)) {
		tmp = ClGetNum(tagptr);

		tmp++;
		ClSetNum(tagptr, tmp);

		ptr1 = ycode;
		ptr2 = ClYomiPos(tagptr);
		len  = ClYomiLen(tagptr);
		while (*ptr1 && len) {
			if (*ptr1 != *ptr2) break;
			ptr1++;
			ptr2++;
			len--;
		}

		if (!target) {
			if (!len)
				;

			else if (!*ptr1)
				target = tagptr;

			else if (*ptr1 < *ptr2)
				target = tagptr;
		}

		tagptr = ClNextTag(tagptr);
	}

	return target ? target : tagptr;
}

Void	mkclidx()
{
	Reg1	Uchar	*ptr;
	Reg2	Int	num;
	Int	st, ed;

	if (!StudyExist()) return;

	ptr = ClStudyDict;
	for (num = st = 0 ; st < 0x100 ; num++, st = ed) {
		ed = st + ClStudyStep;

		while (!iseocl(ptr)) {
			if (*ClYomiPos(ptr) >= st) break;
			ptr = ClNextTag(ptr);
		}

		if (ClYomiLen(ptr) && ((Int)*ClYomiPos(ptr) < ed))
			ClStudyIdx[num] = ptr - ClStudyDict;
		else
			ClStudyIdx[num] = (Ushort)-1;
	}
}

