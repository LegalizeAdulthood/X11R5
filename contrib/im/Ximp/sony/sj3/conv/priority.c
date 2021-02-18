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
#include "sj_right.h"
#include "sj_priority.h"

Static	Int	isfukusi(hinsi)
TypeGram	hinsi;
{
	if (FUKUSI_1 <= hinsi && hinsi <= FUKUSI_7)
		return TRUE;
	else if (hinsi == MEISI_11 || hinsi == D_MEISI_6)
		return TRUE;
	return FALSE;
}

Static	Int	istaigen(right)
TypeCnct	right;
{
	if (right == R_MEISI || right == R_DMEISI || right == R_SMEISI1)
		return TRUE;
	if (right == R_WO_MEISI || right == R_DAIMEISI || right == R_JOSUUSI)
		return TRUE;
	if (right == R_RENYOU1 || (R_RENYOU3 <= right && right <= R_SA5RENYOU4))
		return TRUE;
	return FALSE;
}

Static	Int	taicnt(hinsi1, hinsi2)
TypeGram	hinsi1;
TypeGram	hinsi2;
{
	if (hinsi2 == TANKANJI) return 0;

	if (MYOUJI <= hinsi1 && hinsi1 <= KEN_KU) {
		if (MEISI_1 <= hinsi2 && hinsi2 <= SETUBI_9)
			return Taipri(hinsi1 - MYOUJI, hinsi2);
		else
			return 3;
	}
	else if ((MEISI_1 <= hinsi1 && hinsi1 <= MEISI_11) ||
	         hinsi1 == D_MEISI_2) {
		if (MEISI_1 <= hinsi2 && hinsi2 <= SETUBI_9) {
			return Taipri(5, hinsi2);
		}
		else
			return 5;
	}

	else {
		if (MEISI_1 <= hinsi2 && hinsi2 <= SETUBI_9)
			return Taipri(6, hinsi2);
		else
			return 4;
	}
}

Static	Int	sttcnt(hinsi1, hinsi2)
TypeGram	hinsi1;
TypeGram	hinsi2;
{
	if (SETTOU_1 <= hinsi1 && hinsi1 <= SETTOU_5) {
		if (MEISI_1 <= hinsi2 && hinsi2 <= SUUSI)
			return Sttpri(hinsi1 - SETTOU_1, hinsi2);
	}
	return 4;
}

Int	priority(clrec)
CLREC	*clrec;
{
	int		prty;
	CLREC		*cl2rec;
	Int		keeplen;
	TypeGram	hinsi1;
	Int		i;
	Int		prev = 0;

	if (clrec->cllen == clrec->cl2len) return 0xff;

	if (clrec->jnode->class == C_BUNSETU) {
		prty = Termtbl[clrec->right] & GETPRI;
		return(0xf0 + prty);
	}


	hinsi1 = clrec->jnode->hinsi;

	if (prevclrow == R_WO  || prevclrow == R_WO_S ||
	    prevclrow == R_NI1 || prevclrow == R_DRENYOU2) {
		if (clrec -> kubun == K_DOUSHI || isfukusi(hinsi1))
			prev = 5;
	}

	else if (istaigen(prevclrow)) {

		if (clrec -> kubun == K_TAIGEN)
			prev = taicnt(prevclgrm, hinsi1);

		else
			prev = 0;
	}

	else if (prevclrow == R_SETTOU) {
		if (clrec -> jnode -> sttofs)
			prty = 2;
		else
			prty = sttcnt(prevclgrm, hinsi1);
	}


	if (hinsi1 >= SETUBI_1 && hinsi1 <= SETUBI_9 && (!prev)) {
		prty = 1;
		goto finish;
	}

	cl2rec = maxclptr;
	keeplen = cl2rec -> cllen;

	if (clrec->right == R_WO  || clrec->right == R_WO_S ||
	    clrec->right == R_NI1 || clrec->right == R_DRENYOU2) {
		while (cl2rec && (Int)cl2rec->cllen == keeplen) {
			if (cl2rec->kubun == K_DOUSHI) {
				prty = 14;
				goto finish;
			}
			cl2rec = cl2rec->clsort;
		}
	}

	else if (clrec->right == R_SETTOU) {
		prty = 0;
		while (cl2rec && (Int)cl2rec -> cllen == keeplen) {
			if (cl2rec -> jnode -> sttofs )
				i = 2;
			else
				i = sttcnt(hinsi1, cl2rec->jnode->hinsi);

			if (i > prty)
				prty = i;
			cl2rec = cl2rec -> clsort;
		}
		goto finish;
	}

	else if (istaigen(clrec->right)) {
		prty = 0;
		while (cl2rec && (Int)cl2rec->cllen == keeplen) {
			i = taicnt(hinsi1, cl2rec->jnode->hinsi);
			if (i > prty)
				prty = i;
			cl2rec = cl2rec->clsort;
		}
		if (clrec->jnode->stbofs) {
			prty -= 3;
		}
		goto finish;
	}


	prty = Termtbl[clrec->right];


	if (prty & RENTAI) {
		while (cl2rec && (Int)cl2rec->cllen == keeplen) {
			if (cl2rec -> kubun == K_TAIGEN) {
				prty &= GETPRI;
				goto finish;
			}
			cl2rec = cl2rec->clsort;
		}
		prty = 4;
	}

	else {
		prty &= GETPRI;

		if (prty == P_RENYOU_J || prty == P_RENYOU_M ) {
			while (clrec->cllen != 1 && cl2rec &&
			       (Int)cl2rec->cllen == keeplen ) {
				if (cl2rec->kubun == K_DOUSHI) {
					prty = 6;
					goto finish;
				}
				else if (cl2rec->jnode->hinsi == SETUBI_1 ||
					 cl2rec->jnode->hinsi == SETUBI_2) {
					prty = 7;
					goto finish;
				}
				cl2rec = cl2rec->clsort;
			}
			prty = 4;
		}
	}


finish:
	cl2rec = maxclptr;

	if (hinsi1 == SUUSI) prty -= 4;

	if (cl2rec -> jnode -> hinsi == SUUSI) prty -= 3;

	if (cl2rec->jnode->class == C_DICT &&
	    cl2rec->jnode->jlen == 1 && (cl2rec->kubun != K_DOUSHI)) {
		prty -= 2;
	}

	if ((!istaigen(clrec->right)) && (cl2rec->jnode->hinsi == SETUBI_2)) {
		prty -= 4;
	}

	prty += 20 - clrec->cllen + clrec->jnode->jlen + clrec->gobiln;
	return(prty + prev);
}


Void	pritiny()
{
	CLREC	*clrec;
	Int	keeplen;
	int	i, prty = 0;

	clrec = selcl;
	keeplen = clrec -> cllen;

	if (istaigen(prevclrow)) {
		while (clrec && (Int)clrec -> cllen == keeplen) {
			i = taicnt(prevclgrm, clrec -> jnode -> hinsi);
			if (i > prty) {
				selcl = clrec;
				prty = i;
			}
			clrec = clrec -> clsort;
		}
	}

	else {

		while (clrec && (Int)clrec -> cllen == keeplen) {
			if (clrec -> jnode -> hinsi >= MYOUJI &&
			    clrec -> jnode -> hinsi <= KEN_KU)
				clrec = clrec -> clsort;

			else if (clrec -> jnode -> hinsi == TANKANJI)
				return;
			else {
				selcl = clrec;
				return;
			}
		}
	}
}

