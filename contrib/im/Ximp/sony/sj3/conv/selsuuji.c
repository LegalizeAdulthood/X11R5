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

#include "sj_rename.h"
#include "sj_typedef.h"
#include "sj_const.h"
#include "sj_suuji.h"

static	Ushort	selsjAR[] = {
	SelArb0,
	SelArbCma	| JFLAG_NK4 | JFLAG_NN0,
	SelKanSjYm	| JFLAG_NN0,
	SelKanSjYm2	| SelNum1_23_10_1 | JFLAG_NN0,
	SelKanSj0	| JFLAG_NK2,
	SelNumTerm
};

static	Ushort	selsjARCM[] = {
	SelArbCma	| JFLAG_NK4 | JFLAG_NN0,
	SelArb0,
	SelKanSjYm	| JFLAG_NN0,
	SelKanSjYm2	| SelNum1_23_10_1 | JFLAG_NN0,
	SelKanSj0	| JFLAG_NK2,
	SelNumTerm
};

static	Ushort	selsjKAZU[] = {
	SelKanSjYm	| JFLAG_NN0,
	SelKanSjYm2	| SelNum1_23_10_1 | JFLAG_NN0,
	SelKanSj0	| JFLAG_NK2,
	SelArb0,
	SelArbCma	| JFLAG_NK4 | JFLAG_NN0,
	SelNumTerm
};

static	Ushort	selsjSUUJI[] = {
	SelKanSj0,
	SelArb0,
	SelNumTerm
};

static	Ushort	selsjNANSUU[] = {
	SelKanSjYm,
	SelNumTerm
};

static	Ushort	selsjAR17[] = {
	SelLongArb,
	SelLongArbCma,
	SelNumTerm
};

static	Ushort	selsjAR17CM[] = {
	SelLongArbCma,
	SelLongArb,
	SelNumTerm
};

static	Ushort	selsjAR17SJ[] = {
	SelLongArb,
	SelLongSj,
	SelNumTerm
};

static	Ushort	selsjKANKURA[] = {
	SelKanSjKr,
	SelArbKr,
	SelNumTerm
};

static	Ushort	selsjARAKURA[] = {
	SelArbKr,
	SelKanSjKr	| JFLAG_NSN,
	SelNumTerm
};


Ushort	*selsjadrs[] = {
		selsjAR,
		selsjARCM,
			selsjKAZU,
			selsjSUUJI,
		selsjNANSUU,
		selsjKANKURA,
		selsjARAKURA,
		selsjAR17,
		selsjAR17CM,
		selsjAR17SJ
};
