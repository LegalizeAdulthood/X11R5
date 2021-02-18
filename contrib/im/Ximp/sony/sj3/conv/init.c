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

Void	seg_count(dict)
DICT	*dict;
{
	Uchar	*p;
	Uchar	*q;
	TypeDicSeg	segcnt = 0;

	if (dict-> getidx) {
		(*dict->getidx)(dict);

		p = idxbuf;
		q = p + dict->idxlen;
		while (p < q && *p) {
			segcnt++;
			while (*p++) ;
		}
	}

	dict->segunit = (segcnt == 0) ? 1 : segcnt;
}

Void	mkidxtbl(dict)
DICT	*dict;
{
	Uchar	*p;
	TypeDicSeg	seg;

	if (!dict->getidx || !dict->getofs) return;

	seg = 0;

	(*dict->getidx)(dict);
	(*dict->getofs)(dict);

	idxofs[0] = 0;
	for (p = idxbuf ; p < idxbuf + dict->idxlen && seg < dict->segunit ; ) {
		idxofs[seg++] = p - idxbuf;
		while (*p++) ;
	}
}

Void	initwork()
{
	jrt1st = jrt2nd = maxjptr = (JREC *)0;
	clt1st = clt2nd = maxclptr = (CLREC *)0;
}

