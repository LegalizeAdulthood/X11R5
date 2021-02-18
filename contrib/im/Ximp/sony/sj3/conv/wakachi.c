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
#include "sj_right.h"
#include "sj_hinsi.h"

JREC	*argjrec();
CLREC	*argclrec();
Int	terminate(), setconj();
Void	setclrec(), srchfzk();

Void	wakachi()
{
	JREC		*jrec;
	CLREC		*clrec;

	CREC		crec[4];
	Int		count;
	Uchar		*next;
	TypeCnct	right;
	Int		i;
	Int		maxlen;

	free_jall(maxjptr);
	maxjptr = NULL;

	maxlen = (cnvlen > MaxClInputLen) ? MaxClInputLen : cnvlen;

	next = cnvstart;
	if (Chrtbl[*next] & (TAI_KGU | FZK_KGU)) {

		while (*next && (Chrtbl[*next] & (TAI_KGU | FZK_KGU)))
			next++;

		if ((i = next - cnvstart) > maxlen) {
			next = cnvstart + maxlen;
			i = maxlen;
		}

		if (!(jrec = argjrec(i, (JREC *)NULL))) return;

		jrec -> class  = C_WAKACHI;
		jrec -> hinsi  = D_MEISI_1;

		if (Chrtbl[*(next - 1)] & TAI_KGU)
			srchfzk(jrec, next, R_DAIMEISI, 0);
	}

	else {

		if (!(jrec = argjrec(0, (JREC *)NULL))) return;
		jrec -> class  = C_WAKACHI;
		jrec -> hinsi  = MEISI_6;

		while ((Int)jrec -> jlen < maxlen) {

			count = setconj((TypeGram)MEISI_6, jrec, crec);

			for (i = count ; i-- > 0 ; ) {

				right = crec[i].right;
				gobilen = crec[i].len;
				next = cnvstart + jrec->jlen + gobilen;

				if (gobilen) {
					if (terminate(right, next)) {
						fzk_ka_flg = 0;
						setclrec(jrec, next, right);
					}
				}
				else if ((Chrtbl[*next] & NUMBER)
						&& (jrec -> jlen)) {
					fzk_ka_flg = 0;
					setclrec(jrec, next, right);
					return;
				}

				srchfzk(jrec, next, right, 0);
			}

			if (maxclptr) break;

			(jrec -> jlen)++;
		}
	}

	if ((maxclptr) && (Chrtbl[*(cnvstart + jrec -> jlen)] & FZKTOP))
		return;

	if (jrec -> jlen) {
		if (clrec = argclrec((Int)jrec -> jlen)) {
			clrec -> jnode   = jrec;
			clrec -> right   = R_MEISI;
			clrec -> kubun   = K_TAIGEN;
			(jrec -> count)++;
		}
	}
}

