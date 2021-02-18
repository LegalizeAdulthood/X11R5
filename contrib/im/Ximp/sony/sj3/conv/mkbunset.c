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

CLREC	*alloc_clrec();
Uchar	TFar	*getstb();
Int	terminate(), setconj();
Void	setclrec(), srchfzk(), memset();

Void	mkbunsetu()
{
	Reg1	JREC	*jrec;
	TypeGram	hinsi;
	Uchar	TFar	*cnj;
	CREC	crec[4];
	Int	count;
	TypeCnct	right;
	Uchar	*next;
	int	i;

	fzkcount = 0;

	maxclptr = NULL;

	jrec = maxjptr;

	while (jrec) {

		if (jrec -> stbofs && (cnj = getstb(jrec -> hinsi)))
			hinsi = StbHinsi(cnj + jrec -> stbofs - 1);
		else
			hinsi = jrec -> hinsi;

		if (right = Connadr[hinsi]) {
			gobilen = 0;
			next = cnvstart + jrec->jlen;

			if (terminate(right, next)) {
				fzk_ka_flg = 0;
				setclrec(jrec, next, right);
			}

			srchfzk(jrec, next, right, 0);
		}

		else if (count = setconj(hinsi, jrec, crec)) {

			for (i = count ; i-- > 0 ; ) {

				right = crec[i].right;
				gobilen = crec[i].len;
				next = cnvstart + jrec->jlen + gobilen;

				if (terminate(right, next)) {
					fzk_ka_flg = 0;
					setclrec(jrec, next, right);
				}

				srchfzk(jrec, next, right, 0);
			}
		}

		jrec = jrec -> jsort;
	}
}


CLREC	*argclrec(len)
Int	len;
{
	Reg1	CLREC	*ptr;
	Reg2	CLREC	*rec;
	Reg3	CLREC	*child;

	rec = alloc_clrec();

	if (!rec) {
		if (!maxclptr) return NULL;

		ptr = NULL;
		rec = maxclptr;
		while (child = rec -> clsort) {
			ptr = rec;
			rec = child;
		}

		if (len <= rec -> cllen) return NULL;

		if (ptr)
			ptr -> clsort = NULL;
		else
			maxclptr = NULL;
		if (rec -> jnode) (rec -> jnode)--;
	}

	memset((Uchar *)rec, 0, sizeof(*rec));
	rec -> cllen = rec -> cl2len = (Uchar)len;

	if (!maxclptr) {
		maxclptr = rec;
		return rec;
	}

	ptr = maxclptr;

	if (((Int)ptr -> cllen < len) ||
	    (((Int)ptr -> cllen == len) &&
	     (ptr -> jnode -> hinsi == TANKANJI))) {
		rec -> clsort = maxclptr;
		maxclptr = rec;
		return rec;
	}

	while (child = ptr -> clsort) {
		if ((Int)child -> cllen < len ||
		    ((Int)child -> cllen == len &&
		     child->jnode->hinsi == TANKANJI)){
			ptr -> clsort = rec;
			rec -> clsort = child;
			return rec;
		}
		else
			ptr = child;
	}


	ptr -> clsort = rec;
	return rec;
}

