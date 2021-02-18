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

#ifndef	_SJ_CONST

#define	_SJ_CONST

#define	MATCH		0
#define	OVER		1
#define	PARTLY		2
#define	CONT		3
#define	UNDER		4


#define	FALSE		0
#define	TRUE		-1


#define	AD_Done		0x0000
#define	AD_NotUserDict	0x0001
#define	AD_BadYomi	0x0002
#define	AD_BadKanji	0x0004
#define	AD_BadHinsi	0x0008

#define	AD_ArExist	0x0010
#define	AD_OvflwDouBlk	0x0020
#define	AD_OvflwDouNum	0x0040
#define	AD_OvflwUsrDic	0x0080
#define	AD_OvflwIndex	0x0100

#define	AD_NoMidasi	0x0010
#define	AD_NoHinsi	0x0020
#define	AD_NoKanji	0x0040


#define	C_WAKACHI	0x00
#define	C_MINASI	0x01
#define	C_BUNSETU	0x02
#define	C_DICT		0x03
#define	C_N_ARABIA	0x04
#define	C_N_ARACMA	0x05
#define	C_N_KAZU	0x06
#define	C_N_SUUJI	0x07
#define	C_N_NANSUU	0x08
#define	C_N_KANKURA	0x09
#define	C_N_ARAKURA	0x0a
#define	C_N_KAZULONG	0x0b
#define	C_N_KAZULCMA	0x0c
#define	C_N_SUUJILONG	0x0d


#define	JFLAG_N10	(1 << 15)
#define	JFLAG_N_1	(1 << 14)
#define	JFLAG_NS1	(1 << 13)
#define	JFLAG_N01	(1 << 12)
#define	JFLAG_N23	(1 << 11)
#define	JFLAG_NK2	(1 << 10)
#define	JFLAG_NK4	(1 << 9)
#define	JFLAG_NN0	(1 << 8)
#define	JFLAG_NSN	(1 << 7)
#define	JFLAG_KA	(1 << 0)

#define	DO_CLSTUDY	0x01
#define	DO_IKKATU	0x02


#define	K_DOUSHI	1
#define	K_YOUGEN	2
#define	K_TAIGEN	3
#define	K_FUKUSI	4
#define	K_RENTAISI	5
#define	K_SONOTA	6



#define	SJIS_f1		(Uchar)0x81
#define	SJIS_f2		(Uchar)0x82
#define	SJIS_f3		(Uchar)0x83
#define	SJIS_f8		(Uchar)0x80
#define	SJIS_f9		(Uchar)0x90
#define	SJIS_fe		(Uchar)0xe0
#define	SJIS_ff		(Uchar)0xf0

#define	SJIS_HAJIME	(Uchar)0x40
#define	SJIS_YUUBIN	(Uchar)0xa7
#define	SJIS_KIGOUEND	(Uchar)0xac
#define	SJIS_0		(Uchar)0x4f
#define	SJIS_0END	(Uchar)0x58
#define	SJIS_A		(Uchar)0x60
#define	SJIS_AEND	(Uchar)0x79
#define	SJIS_a		(Uchar)0x81
#define	SJIS_aEND	(Uchar)0x9a
#define	SJIS_HIRA	(Uchar)0x9f
#define	SJIS_HIRAEND	(Uchar)0xf1
#define	SJIS_VU		(Uchar)0x94
#define	SJIS_XKA	(Uchar)0x95
#define	SJIS_XKE	(Uchar)0x96

#define	isknj1(c)	(((c)>=0x80 && (c)<=0x9f) || ((c)>=0xe0 &&(c)<= 0xff))


#define	SHU		0x01
#define	T_KANOU		0x80
#define	T_SHUUJO	0x40


#define	DICTOP  	0x01
#define	FZKTOP		0x02
#define	DEPEND		0x04
#define	NUMBER		0x08
#define	FZK_KGU  	0x10
#define	TAI_KGU		0x20
#define	BEFORNUM	0x40
#define	MIDNUM		0x80
#define	NOTNRM		0xFE


#define	CNJEND		0xff
#define	TYOUONFLG	0x80


#define	MAXFREC		32
#define	MAXFLVL		10
#define	FZKEND		0xff

#define	SETTOU_O	1
#define	SETTOU_GO	2
#define	SETTOU_DAI	3
#define	SETTOU_KIGOU	4


#define	NumKetaLength	16
#define	NumWordBuf	((NumKetaLength+3)/4)


#endif

