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

Int	sj2cd_chr(), hzstrlen(), sstrlen();
Void	sstrncpy(), mkjiritu(), mkbunsetu(), wakachi();
Void	mkkouho(), getrank(), cvtclknj(), freework();
Void	memset();

CLREC	*nextrecblk(), *prevrecblk();
Void	initkbuf();

Int	cl2knj(yomi, len, kouho)
Uchar	*yomi;
Reg4	Int	len;
Uchar	*kouho;
{
	Reg1	Uchar	*ptr1;
	Reg2	Uchar	*ptr2;
	Reg3	Int	i;

	khcount = nkhcount = 0;

	initkbuf(kouho);

	if (len > MaxClInputLen * 2) len = MaxClInputLen * 2;

	ptr1 = yomi; ptr2 = hyomi;
	for (i = 0 ; i < len ; i++) {
		if (isknj1(*ptr1)) {
			if (++i >= len) break;
		}
		ptr1 += sj2cd_chr(ptr1, ptr2++);
	}
	*ptr2 = 0;

	sstrncpy(yomi, orgyomi, (Int)(ptr1 - yomi));

	if (!hyomi[0]) return 0;

	freework();

	inputyomi = orgyomi;
	cnvstart = ystart = hyomi;
	cnvlen = sstrlen(hyomi);

	mkjiritu(0);

	mkbunsetu();

	if (!maxclptr) wakachi();

	jrt1st = maxjptr;
	clt1st = maxclptr;

	mkkouho();

	getrank();

	selectid = 1;

	cvtclknj();

	return hzstrlen(inputyomi, (Int)clt1st -> cllen);
}

Int	nextcl(kouho, mode)
Uchar	*kouho;
Int	mode;
{
	CLREC	*clptr;

	initkbuf(kouho);

	if (!khcount) return 0;

	if (!mode && (selectid < khcount))
		selectid++;

	else if((mode < 2) && (clptr = nextrecblk())) {
		selectid = 1;
		clt1st = clptr;
		mkkouho();
		getrank();
	}

	else {
		cvtclknj();
		return 0;
	}

	cvtclknj();

	return hzstrlen(inputyomi, (Int)clt1st -> cllen);
}


Static	CLREC	*nextrecblk()
{
	Reg1	CLREC	*clptr;
	Reg2	Int	keeplen;

	keeplen = clt1st -> cllen;

	clptr =  clt1st -> clsort;

	while (clptr && (Int)clptr -> cllen == keeplen)
		clptr = clptr -> clsort;

	return clptr;
}


Int	prevcl(kouho, mode)
Uchar	*kouho;
Int	mode;
{
	CLREC	*clptr;

	initkbuf(kouho);

	if (!khcount) return 0;

	if (!mode && (selectid > 1))
		selectid--;

	else if ((mode < 2) && (clptr = prevrecblk())) {
		clt1st = clptr;
		mkkouho();
		getrank();
		selectid = khcount;
	}

	else {
		cvtclknj();
		return 0;
	}

	cvtclknj();

	return hzstrlen(inputyomi, (Int)clt1st -> cllen);
}

Static	CLREC	*prevrecblk()
{
	Reg1	CLREC	*clptr;
	CLREC	*keepptr;
	Reg2	Int	keeplen;
	Int	cllen;

	clptr = maxclptr;

	if ((cllen = clt1st -> cllen) == clptr -> cllen)
		return NULL;

	while ((Int)clptr -> cllen > cllen) {
		keeplen = clptr -> cllen;

		keepptr = clptr;

		while ((Int)clptr -> cllen == keeplen)
			clptr = clptr -> clsort;
	}

	return keepptr;
}

Static	Void	initkbuf(kouho)
Uchar	*kouho;
{
	kanjitmp = kouho;
	memset(kanjitmp, 0, sizeof(STDYOUT) + 1);
}

Int	selectnum()
{
	return selectid;
}

