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

Void	free_clall(), free_jall(), mkjiritu(), mkbunsetu();
Void	wakachi();

Void	selclrec()
{
	Int	len;
	CLREC	*rec;

	nextcllen = 0;

	len = selcl -> cllen;
	rec = clt1st;
	while (rec) {
		if (len >= rec -> cllen) break;
		rec = rec -> clsort;
	}

	while (rec) {
		if (len > rec -> cllen) return;
		if (rec -> jnode -> class == C_BUNSETU) break;
		rec = rec -> clsort;
	}
	if (!rec) return;


	len = ClYomi1Len(ClStudyDict + rec->jnode->jofsst);

	nextcllen =  rec -> cllen - len;

	while (rec) {
		if (len == rec -> cllen) {
			selcl = rec;
			break;
		}
		else if (len > rec -> cllen) {
			rec = NULL;
			break;
		}
		rec = rec -> clsort;
	}

	if (rec == NULL) {
		Int	klen;
		Uchar	ch;

		free_clall(clt1st);
		free_jall(jrt1st);

		klen = cnvlen;
		cnvlen = len;
		ch = *(cnvstart + len);
		*(cnvstart + len) = 0;

		mkjiritu(DO_IKKATU);

		mkbunsetu();
		if (!maxclptr) wakachi();

		jrt1st = maxjptr;
		selcl = clt1st = maxclptr;

		cnvlen = klen;
		*(cnvstart + len) = ch;
	}

	free_clall(clt2nd); clt2nd = NULL;
	free_jall(jrt2nd); jrt2nd = NULL;
}

