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
#include "sj_yomicode.h"
#include "sj_hinsi.h"

static	Uchar	name_stb[] = {
	0x22, MEISI_1,	_KU, _NN,
			0x8C, 0x4E,
	0x22, MEISI_1,	_SA, _MA,
			0x97, 0x6C,
	0x24, MEISI_1,	_SA, _NN,
			0x82, 0xB3, 0x82, 0xF1,
	0x12, MEISI_1,	_SI,
			0x8E, 0x81,
	0x36, MEISI_1,	_TI, _XYA, _NN,
			0x82, 0xBF, 0x82, 0xE1, 0x82, 0xF1,
	0x22, MEISI_1,	_DO, _NO,
			0x93, 0x61,
	0x00
};

static	Uchar	timei_stb[] = {
	0x22, MEISI_1,	_E, _KI,
		 	0x89, 0x77,
	0x44, MEISI_1,	_E, _KI, _HA, _TU,
		 	0x89, 0x77, 0x94, 0xAD,
	0x44, MEISI_1,	_E, _KI, _YU, _KI,
		 	0x89, 0x77, 0x8D, 0x73,
	0x22, MEISI_1,	_GU, _NN,
	 		0x8C, 0x53,
	0x12, MEISI_1,	_SI,
			0x8E, 0x73,
	0x32, MEISI_1,	_TI, _XYO, _U,
			0x92, 0xAC,
	0x22, MEISI_1,	_HA, _TU,
			0x94, 0xAD,
	0x22, MEISI_1,	_MA, _TI,
	 		0x92, 0xAC,
	0x22, MEISI_1,	_MU, _RA,
			0x91, 0xBA,
	0x00
};

static	Uchar	meisi_stb[] = {
	0x44, MEISI_6,	_A, _TU, _KA, _I,
			0x88, 0xB5, 0x82, 0xA2,
	0x1a, MEISI_6,	_KA,
			0x89, 0xBB,
	0x22, MEISI_8,	_TE, _KI,
			0x93, 0x49,
	0x56, FUKUSI_1, _NI, _TO, _MO, _NA, _I,
			0x82, 0xc9, 0x94, 0xba, 0x82, 0xa2,
	0x56, FUKUSI_1, _NI, _TO, _MO, _NA, _U,
			0x82, 0xc9, 0x94, 0xba, 0x82, 0xa4,
	0x22, MEISI_8,	_HU, _U,
			0x95, 0x97,
	0x00
};

static	Uchar	sahen_stb[] = {
	0x44, MEISI_6,	_A, _TU, _KA, _I,
			0x88, 0xB5, 0x82, 0xA2,
	0x1a, MEISI_6,	_KA,
			0x89, 0xBB,
	0x12, MEISI_11,	_GO,
			0x8C, 0xE3,
	0x22, MEISI_11, _GO, _TO,
			0x96, 0x88,
	0x32, MEISI_11,	_ZI, _XYO, _U,
			0x8F, 0xE3,
	0x32, MEISI_11,	_TI, _XYU, _U,
			0x92, 0x86,
	0x22, MEISI_8,	_TE, _KI,
			0x93, 0x49,
	0x56, FUKUSI_1, _NI, _TO, _MO, _NA, _I,
			0x82, 0xc9, 0x94, 0xba, 0x82, 0xa2,
	0x56, FUKUSI_1, _NI, _TO, _MO, _NA, _U,
			0x82, 0xc9, 0x94, 0xba, 0x82, 0xa4,
	0x22, MEISI_8,	_HU, _U,
			0x95, 0x97,
	0x22, MEISI_11,	_MA, _E,
		 	0x91, 0x4F,
	0x00
};

static	Uchar	human_stb[] = {
	0x24, MEISI_1,	_SA, _NN,
			0x82, 0xB3, 0x82, 0xF1,
	0x00
};

static	Uchar	ippan_stb[] = {
	0x56, FUKUSI_1, _NI, _TO, _MO, _NA, _I,
			0x82, 0xc9, 0x94, 0xba, 0x82, 0xa2,
	0x56, FUKUSI_1, _NI, _TO, _MO, _NA, _U,
			0x82, 0xc9, 0x94, 0xba, 0x82, 0xa4,
	0x00
};

Uchar	*stbadr[] = {
	0,
	ippan_stb, ippan_stb, ippan_stb, meisi_stb, sahen_stb,
	sahen_stb, sahen_stb, meisi_stb, meisi_stb, meisi_stb,
	ippan_stb, ippan_stb, human_stb, ippan_stb, ippan_stb,
	ippan_stb, ippan_stb, ippan_stb, 	 0, 	    0,
	name_stb,  name_stb,  0, 	 timei_stb,
};
