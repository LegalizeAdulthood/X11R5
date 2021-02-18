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

Int	terminate(), priority();
Void	mkjiritu(), mkbunsetu(), wakachi(), pritiny();
Void	free_clall(), free_jall();

Static	Int	set2nd(clrec)
CLREC	*clrec;
{

	CLREC	*rec2;

	mkjiritu(DO_IKKATU | DO_CLSTUDY);

	mkbunsetu();

	if (!maxclptr) {
		wakachi();
		return clrec -> cllen;
	}

	for (rec2 = maxclptr ; rec2 ; rec2 = rec2 -> clsort) {
		if (terminate(rec2->right, cnvstart+rec2->cllen) != SHU)
			break;
	}

	return rec2 ? (clrec -> cllen + rec2 -> cllen) : (clrec -> cllen);
}

Void	mk2claus()
{
	Uchar	*keepptr;
	Int	keeplen;
	CLREC	*clrec;
	Int	len;
	Int	save2ln;
	Int	prty;
	Int	maxprty;
	Int	count;
	Int	prty2;

	prty = maxprty = prty2 = 0;

	clt2nd = maxclptr = NULL;
	jrt2nd = maxjptr  = NULL;

	selcl = clt1st;

	if (selcl -> right == R_FZKKGU) {
		pritiny();
		return;
	}

	if (cnvlen == selcl -> cllen) {
		pritiny();
		return;
	}

	keepptr = cnvstart;
	keeplen = cnvlen;

	len = count = 0;
	for (clrec = selcl ; clrec && count < 5 ; clrec = clrec -> clsort) {

		cnvstart = keepptr + clrec -> cllen;
		cnvlen = keeplen - clrec -> cllen;

		if (len != clrec -> cllen) {
			if (clt2nd != maxclptr) {
				free_clall(maxclptr);
				free_jall(maxjptr);
			}

			save2ln = set2nd(clrec);

			count++;
			len = clrec -> cllen;
			prty2 = 0;
		}

		if (Termtbl[clrec -> right] & T_SHUUJO)
			clrec -> cl2len = (Uchar)len;
		else
			clrec -> cl2len = (Uchar)save2ln;

		if (selcl -> cl2len > clrec -> cl2len) continue;

		prty = priority(clrec) - prty2;

		prty2++;

		if (selcl -> cl2len == clrec -> cl2len) {
			if (clrec -> cllen == nextcllen) {
				if (selcl -> cllen != nextcllen)
					;
				else if (prty <= maxprty)
					continue;
			}

			else if (prty <= maxprty)
				continue;
		}

		selcl = clrec;
		maxprty = prty;

		if (clt2nd == maxclptr) continue;

		free_clall(clt2nd);
		free_jall(jrt2nd);
		jrt2nd = maxjptr;
		clt2nd = maxclptr;
	}

	if (clt2nd != maxclptr) {
		free_clall(maxclptr);
		free_jall(maxjptr);
	}

	cnvstart = keepptr;
	cnvlen = keeplen;
}

