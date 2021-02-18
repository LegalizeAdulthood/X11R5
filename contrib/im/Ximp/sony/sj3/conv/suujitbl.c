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

#include "sj_const.h"
#include "sj_rename.h"
#include "sj_typedef.h"
#include "sj_yomicode.h"
#include "sj_suuji.h"

#define	NA	SjTblNArabiaF
#define	NY	SjTblNYomiF
#define	NN	SjTblNNarabiF
#define	NS	SjTblNNanSuuF

Uchar	suuji_tbl[] = {
	N_0,_NIL,_NIL,	_Num0  | NA,		0x00, 0x00,
	N_1,_NIL,_NIL,	_Num1  | NA,		0xec, 0x07,
	N_2,_NIL,_NIL,	_Num2  | NA,		0xec, 0x07,
	N_3,_NIL,_NIL,	_Num3  | NA,		0xff, 0x07,
	N_4,_NIL,_NIL,	_Num4  | NA,		0xec, 0x07,
	N_5,_NIL,_NIL,	_Num5  | NA,		0xec, 0x07,
	N_6,_NIL,_NIL,	_Num6  | NA,		0xec, 0xc7,
	N_7,_NIL,_NIL,	_Num7  | NA,		0xec, 0x07,
	N_8,_NIL,_NIL,	_Num8  | NA,		0xec, 0xc7,
	N_9,_NIL,_NIL,	_Num9  | NA,		0xec, 0x07,
	_I, _TI, _NIL,	_Num1,			0x00, 0x05,
	_I, _XTU,_NIL,	_Num1S | NY,		0x20, 0x02,
	_KI,_XYU,_U,  	_Num9,			0xec, 0x07,
	_KU,_NIL,_NIL,	_Num9  | NY,		0x00, 0x00,
	_GO,_NIL,_NIL,	_Num5,			0xec, 0x07,
	_GO,_U,  _NIL,	_Num5  | NN,		0x00, 0x00,
	_SA,_NN, _NIL,	_Num3,			0xd3, 0x07,
	_SI,_NIL,_NIL,	_Num4  | NY,		0x80, 0x00,
	_SI,_I,  _NIL,	_Num4  | NN,		0x00, 0x00,
	_SI,_TI, _NIL,	_Num7,			0xec, 0x07,
	_SU,_U,  _NIL,  _NumS  | NY | NS,	0xec, 0x07,
	_ZE,_RO, _NIL,	_Num0  | NN,		0x00, 0x00,
	_NA,_NA, _NIL,	_Num7,			0xec, 0x07,
	_NA,_NN, _NIL,	_NumN  | NY | NS,	0xf3, 0x07,
	_NI,_NIL,_NIL,	_Num2,			0xec, 0x07,
	_NI,_I,  _NIL,	_Num2  | NN,		0x00, 0x00,
	_HA,_TI, _NIL,	_Num8,			0xc0, 0x05,
	_HA,_XTU,_NIL,	_Num8  | NY,		0x20, 0xc2,
	_MA,_RU, _NIL,	_Num0  | NN,		0x00, 0x00,
	_YO,_NIL,_NIL,	_Num4  | NY,		0x00, 0x00,
	_YO,_NN, _NIL,	_Num4,			0xec, 0x07,
	_RE,_I,  _NIL,	_Num0  | NN,		0xec, 0x00,
	_RO,_KU, _NIL,	_Num6,			0xe0, 0x07,
	_RO,_XTU,_NIL,	_Num6  | NY,		0x00, 0xc2,
		0
};

Uchar	kurai1_tbl[] = {
	_ZI,_XYU,_U,  	(1<<4)+0+Kr1TblTopF,	0x05,
	_ZI,_XYU,_XTU,	(1<<4)+1+Kr1TblTopF,	0x02,
	_SE,_NN, _NIL,	(3<<4)+2+Kr1TblTopF,	0x07,
	_ZE,_NN, _NIL,	(3<<4)+3,		0x05,
	_HI,_XYA,_KU, 	(2<<4)+4+Kr1TblTopF,	0x07,
	_HI,_XYA,_XTU,	(2<<4)+5+Kr1TblTopF,	0x02,
	_BI,_XYA,_KU, 	(2<<4)+6,		0x07,
	_BI,_XYA,_XTU,	(2<<4)+7,		0x00,
	_PI,_XYA,_KU, 	(2<<4)+8,		0x07,
	_PI,_XYA,_XTU,	(2<<4)+9,		0x00,
		0
};

Uchar	kurai2_tbl[] = {
	_O, _KU, _NIL,	 (2<<4)+13,
	_TI,_XYO,_U,  	 (3<<4)+14,
	_MA,_NN, _NIL,	 (1<<4)+15,
		0
};

Uchar	num1tbl[]  = {
	0x82, 0x4f,
	0x82, 0x50,
	0x82, 0x51,
	0x82, 0x52,
	0x82, 0x53,
	0x82, 0x54,
	0x82, 0x55,
	0x82, 0x56,
	0x82, 0x57,
	0x82, 0x58,
	0x82, 0x50,
	0x90, 0x94,
	0x89, 0xbd
};

Uchar	num2tbl[]  = {
	0x81, 0x5a,
	0x88, 0xea,
	0x93, 0xf1,
	0x8e, 0x4f,
	0x8e, 0x6c,
	0x8c, 0xdc,
	0x98, 0x5a,
	0x8e, 0xb5,
	0x94, 0xaa,
	0x8b, 0xe3,
	0x88, 0xea,
	0x90, 0x94,
	0x89, 0xbd
};

Uchar	num3tbl[]  = {
	0x97, 0xeb,
	0x88, 0xeb,
	0x93, 0xf3,
	0x8e, 0x51,
	0x8e, 0x6c,
	0x8c, 0xdc,
	0x98, 0x5a,
	0x8e, 0xb5,
	0x94, 0xaa,
	0x8b, 0xe3,
	0x88, 0xeb,
	0x90, 0x94,
	0x89, 0xbd
};

Uchar	num4tbl[] = {
	0x90, 0xe7,
	0x95, 0x53,
	0x8f, 0x5c,
	0
};

Uchar	num5tbl[] = {
	0x90, 0xe7,
	0x95, 0x53,
	0x8f, 0x45,
	0
};

Uchar	num6tbl[] = {
	0x92, 0x9b,
	0x89, 0xad,
	0x96, 0x9c,
	0
};
