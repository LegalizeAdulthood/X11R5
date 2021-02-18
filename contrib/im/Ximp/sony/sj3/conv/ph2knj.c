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

Int	sj2cd_chr(), sstrlen();
Void	mkjiritu(), mkbunsetu(), wakachi(), mk2claus();
Void	freework();
Void	selclrec();
CLREC	*free_clst();
JREC	*free_jlst();

Int	ph2knj(zyomi, kanji, knjlen)
Uchar	*zyomi;
Uchar	*kanji;
Int	knjlen;
{
	Uchar	*ptr;
	Uchar	*dst;
	Int	i;

	for (ptr = zyomi, dst = hyomi, i = 0 ; *ptr ; ) {
		if (i++ >= MaxPhInputLen) { *kanji = 0; return 0; }
		ptr += sj2cd_chr(ptr, dst++);
	}
	*dst = 0;

	freework();
	prevclgrm = 0;
	prevclrow = 0;

	khcount = 0;
	nextcllen = 0;

	inputyomi = zyomi;
	cnvstart = ystart = hyomi;
	cnvlen = sstrlen(hyomi);

	kanjipos = kanji;
	kanjilen = knjlen;

	while (cnvlen && kanjilen) {
		if (!clt1st) {
			mkjiritu(DO_CLSTUDY | DO_IKKATU);

			mkbunsetu();

			if (!maxclptr) wakachi();

			jrt1st = maxjptr;
			clt1st = maxclptr;
		}

		mk2claus();

		selclrec();

		prevclgrm = selcl -> jnode -> hinsi;
		prevclrow = selcl -> right;

		clt1st = free_clst(clt1st, (Int)selcl -> cllen);
		jrt1st = free_jlst(jrt1st);

		cnvstart += selcl -> cllen;
		cnvlen -= selcl -> cllen;

		cvtphknj();

		clt1st = clt2nd;
		jrt1st = jrt2nd;
	}

	if (!kanjilen) {
		free_clall(clt1st);
		free_jall (jrt1st);
	}

	*kanjipos = 0;

	return(inputyomi - zyomi);
}

