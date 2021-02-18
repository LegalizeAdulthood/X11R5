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
 * Author: Masaki Takeuchi, Sony Corporation
 */
/*
 * $Header: conversion.h,v 1.1 91/04/29 17:57:40 masaki Exp $ SONY;
 */

#include "sj2.h"
#include "sj3lib.h"
#include "henkan.h"

typedef struct bunsetu	Bunsetu;

typedef struct _Conversion {
	u_short	HAindex[BUFFLENGTH];	/* Halpha moji index		*/
	u_short	Plen[BUFFLENGTH];	/* bunsetu len			*/
	u_short	OrgPlen[BUFFLENGTH];	/* original bunsetu len		*/
	u_short	Bspan[BUFFLENGTH];	/* span number			*/
	u_short	Bpoint[BUFFLENGTH];	/* bunsetu number		*/
	u_short	Edit[BUFFLENGTH];	/* edit number buffer		*/
	u_short	Amojilen[BUFFLENGTH];	/* alphabet moji length		*/
	u_short	Dmojilen[BUFFLENGTH];	/* displaied moji length	*/
	u_short	span_l[BUFFLENGTH];	/* span length area		*/
	u_short	Elen[BUFFLENGTH];	/* edit str len			*/
	u_short	Displen[BUFFLENGTH];	/* display length		*/

	u_short Vindex[BUFFLENGTH * 2];

	short	ha_point;		/* hankaku alphabet pointer	*/
	short	moji_count;		/* moji counter			*/
	short	span_point;		/* span pointer			*/
	short	Bnum;			/* bunsetu number		*/
	short	Enum;			/* edit number			*/
	short	e_point;		/* Ebuff pointer		*/
	short	HAindex_p;
	short	alpha_point;

	u_short line;
	u_short column;

	u_short	PushCurrentVcol;
	u_short	Vlen;
	u_short	SavedRow;
	u_short	SavedCol;
	u_short	CurrentVcol;
	u_short	MaxVcol;

	short	CurBun;			/* bunsetu current num		*/
	short	PreBun;			/* prev bunsetu number		*/

	Bunsetu	Bun[BUFFLENGTH];	/* henkan kekka			*/

	u_char	*span_p[BUFFLENGTH];	/* span pointer area		*/
	u_char	*Ppoint[BUFFLENGTH];	/* bunsetu, a src ptr		*/
	u_char	*Epoint[BUFFLENGTH];	/* edit str pointer		*/
	u_char	Halpha[BUFFLENGTH];	/* hankaku alphabet buffer	*/
	u_char	Dmojimod[BUFFLENGTH];	/* the moji inputed mode	*/
	u_char	kettei[BUFFLENGTH];	/* span kettei buffer		*/
	u_char	Pkettei[BUFFLENGTH];	/* bunsetu mode			*/
	u_char	Pedited[BUFFLENGTH];	/* bunsetu edited		*/
	u_char	Ebuff[EDITLEN];		/* edit str buffer		*/
	u_char	Bkettei[BUFFLENGTH];	/* flag Bpoint/Bspan		*/
	u_char	Dispmod[BUFFLENGTH];	/* 0: hankaku, 1: zenkaku	*/
	u_char	ZZbuff[BUFFLENGTH * 2];
	u_char	yomiout[BUFFLENGTH * 2];
	u_char	kanjiout[BUFFLENGTH * 4];
	u_char	alpha_buff[SHORTBUFFSIZE];

	u_char	Imode;			/* input display mode		*/
	u_char	BuffMode;		/* buffering mode		*/
	u_char	Kanji;			/* henkan flag			*/
	u_char	Dflag;			/* douon flag			*/
	u_char	word_regist;
	u_char	Jmode;
	u_char	Cflag;
	u_char	henkanflag;
	u_char	o_mode;

	u_char	wrap;
} Conversion;

