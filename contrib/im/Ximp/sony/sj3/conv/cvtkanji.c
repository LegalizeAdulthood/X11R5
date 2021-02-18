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

Int	hzstrlen(), ph_khtbl();
Void	cvtdict(), cvtminasi(), cvtwakachi(), cvtkouho();
Void	ph_setkouho(), mvmemi(), free_clall(), setstyrec();
Void	free_jall(), memcpy(), cvtphknj();
Uchar	TFar	*getstb();
STDYIN	*srchstdy();

Void	cvtphknj()
{
	Int	i, j;
	CLREC	*clrec;
	CLREC	*clkp;

	if (!kanjilen) goto not_enough_memory;

	khcount = 0;
	clkp = NULL;
	stdytop = NULL;

		clrec = clt1st;
		while (clrec) {
			if (ph_khtbl(clrec)) clkp = clrec;
			clrec = clrec -> clsort;
		}

		if (clkp && stdytop) {
			if (stdytop -> nmflg)
				ph_setkouho(clkp, (TypeDicOfs)0, (STDYIN *)0);
		}

	kouhotbl[0].rank = 1;

	kanjitmp = kanjibuf + 1;

	setstyrec(kouhotbl);

	cvtkouho(kouhotbl);

	i = kanjitmp - kanjibuf;
	if (i < kanjilen) {
		kanjilen -= i;

		ystart += clt1st -> cllen;

		j = hzstrlen(inputyomi, (Int)clt1st -> cllen);
		kanjibuf[0] = (Uchar)j;

		inputyomi += j;

		memcpy(kanjibuf, kanjipos, i);
		kanjipos += i;
	}
	else
		kanjilen = 0;

not_enough_memory:

	free_clall(clt1st);
	free_jall (jrt1st);
}

Void	cvtkouho(krec)
KHREC	*krec;
{
	CLREC	*clrec;

	clrec = krec -> clrec;

	switch (clrec -> jnode -> class) {
	case C_DICT:
		cvtdict(krec, clrec, FALSE);
		break;

	case C_N_ARABIA:
	case C_N_ARACMA:
	case C_N_KAZU:
	case C_N_SUUJI:
	case C_N_NANSUU:
	case C_N_KANKURA:
	case C_N_ARAKURA:
	case C_N_KAZULONG:
	case C_N_KAZULCMA:
	case C_N_SUUJILONG:
		cvtdict(krec, clrec, TRUE);
		break;

	case C_BUNSETU :
	case C_MINASI  :
		cvtminasi((Int)clrec -> cllen);
		break;

	case C_WAKACHI :
		cvtwakachi(clrec);
		break;
	}

	*kanjitmp++ = 0;
}

Void	setstyrec(krec)
KHREC	*krec;
{
	JREC	*jrec;
	KHREC	*kptr;
	Int	ii;
	Uchar	TFar	*fptr;
	STDYOUT	stdy;

	jrec  = krec -> clrec -> jnode;

	if (krec -> mode && !krec -> offs) {
		for (ii = khcount, kptr = kouhotbl ; ii--; kptr++)
			if (kptr -> rank == 1) break;

		stdy.stdy1.offset = kptr -> offs;
		stdy.stdy1.seg    = kptr -> clrec -> jnode -> jseg;
		stdy.stdy1.styno  = kptr -> styno;
		stdy.stdy1.dicid  = kptr -> clrec -> jnode -> dicid;
		stdy.stdy1.nmflg  = 1;
	}
	else {
		stdy.stdy1.offset = krec -> offs;
		stdy.stdy1.seg    = jrec -> jseg;
		stdy.stdy1.styno  = krec -> styno;
		stdy.stdy1.dicid  = jrec -> dicid;
		stdy.stdy1.nmflg  = 0;
	}
	stdy.stdy1.sttkj  = krec -> sttkj;
	stdy.stdy1.ka_kj  = krec -> ka_kj;
	stdy.hinshi       = jrec -> hinsi;
	stdy.len          = jrec -> jlen;
	stdy.sttfg        = krec -> sttfg;
	stdy.ka_fg        = krec -> ka_fg;

	if (jrec -> stbofs && (fptr = getstb(jrec -> hinsi)))
		stdy.len -= StbYomiLen(fptr + jrec -> stbofs - 1);

	if (fptr = Settou_ptr(jrec -> sttofs))
		stdy.sttlen = SttYomiLen(fptr);
	else
		stdy.sttlen = 0;

	memcpy((Uchar *)&stdy, kanjitmp, sizeof(STDYOUT));
	kanjitmp += sizeof(STDYOUT);
}

