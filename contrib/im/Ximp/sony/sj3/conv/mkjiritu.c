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

Uchar	*srchdict();
Uchar	TFar	*getstb();
Int	srchhead(), sstrncmp();
JREC	*alloc_jrec();
Void	srchnum(), setubi(), setjrec();
Void	setcrec(), memcpy(), memset();

Void	dic_mu(), dic_cl();

Void	mkjiritu(mode)
Int	mode;
{
	Uchar	chkind1;
	Uchar	chkind2;
	JREC	*jrec;
	Uchar	TFar	*stb;

	headcode = headlen = 0;

	maxjptr = NULL;

	chkind1 = Chrtbl[*cnvstart];

	if (chkind1 & DICTOP) {
		dic_mu(mode);
	}

	if (mode & DO_CLSTUDY) dic_cl();

	srchnum();

	if (srchhead() && cnvlen != headlen) {
		cnvstart += headlen;
		cnvlen -= headlen;

		chkind2 = Chrtbl[*cnvstart];

		if (chkind2 & DICTOP) {
			dic_mu(mode);
		}

		if (headcode == SETTOU_DAI) {
			srchnum();
		}

		cnvstart -= headlen;
		cnvlen += headlen;
	}

	for (jrec = maxjptr ; jrec ; jrec = jrec -> jsort) {
		if (stb = getstb(jrec -> hinsi))
			setubi(jrec, stb);
	}
}

Static	Void	dic_mu(mode)
Int	mode;
{
	Uchar	*tagp;
	DICTL	*dp;

	for (dp = dictlist ; dp ; dp = dp -> next) {
		curdict = dp -> dict;
		dicinl  = 1;
		dicsaml = 0;
		prevseg = -1;

		while (tagp = srchdict(tagp)) setjrec(tagp, mode);
	}
}

JREC	*argjrec(len, rec)
Int	len;
JREC	*rec;
{
	Reg1	JREC	*ptr;
	Reg2	JREC	*jrec;
	Reg3	JREC	*child;

	jrec = alloc_jrec();

	if (!jrec) {
		if (!maxjptr) return NULL;

		ptr = NULL;
		jrec = maxjptr;
		while (child = jrec -> jsort) {
			ptr  = jrec;
			jrec = child;
		}

		if (len <= jrec -> jlen) return NULL;

		if (ptr)
			ptr -> jsort = NULL;
		else
			maxjptr = NULL;
	}

	if (rec)
		memcpy((Uchar *)rec, (Uchar *)jrec, sizeof(JREC));
	else
		memset((Uchar *)jrec, 0, sizeof(*jrec));
	jrec -> jlen = (Uchar)len;

	if (!maxjptr) {
		maxjptr = jrec;
		jrec -> jsort = NULL;
		return jrec;
	}

	ptr = maxjptr;

	if ((Int)ptr -> jlen < len) {
		jrec -> jsort = maxjptr;
		maxjptr = jrec;
		return jrec;
	}

	while (child = ptr -> jsort) {
		if ((Int)child -> jlen < len) break;

		ptr = child;
	}

	ptr -> jsort = jrec;
	jrec -> jsort = child;
	return jrec;
}

Static	Void	dic_cl()
{
	Uchar	*p;
	Ushort	pos;
	Int	len;
	Int	cmp;

	if (!StudyExist()) return;

	if ((pos = ClStudyIdx[*cnvstart / ClStudyStep]) != (Ushort)-1) {

		for (p = ClStudyDict + pos ; !iseocl(p) ; p = ClNextTag(p)) {
			cmp = sstrncmp(cnvstart, (Uchar TFar *)ClYomiPos(p),
					len = ClYomiLen(p));

			if (cmp == MATCH) {
				if (!isdpnd(*(cnvstart + len))) {
					setcrec(p);
				}
			}

			else if (cmp == OVER)
				break;
		}
	}
}

