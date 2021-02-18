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

#ifndef	_SJ_TABLE

#define	_SJ_TABLE

#define	isdpnd(c)	(Chrtbl[c] & DEPEND)

#define	CnjAskLen(p)	((*(p) & 0x70) >> 4)

#define	CnjNkrLen(p)	(*(p) & 0x0f)

#define	CnjRight(p)	*(p + CnjNkrLen(p) + 1)

#define	CnjRecLen(p)	(CnjNkrLen(p) + 2)

#define	FzkAskLen(p)	(*(p) >> 5)

#define	FzkNkrLen(p)	(*(p) & 0x07)

#define	FzkStrKa(p)	((*(p) >> 3) & 1)

#define	FzkHasCode(p)	(*(p) & 0x10)

#define	FzkLeft(p)	(*((p) + nlen + 1))

#define	FzkRight(p)	(*((p) + nlen + 2))

#define	SttYomiLen(p)	(*(p) & 0x0f)

#define	SttHiraKnj(p)	(*(p) & 0x80)

#define	StbKnjLen(p)	(*(p) & 0x07)

#define	StbYomiLen(p)	(*(p) >> 4)

#define	StbYomiTop(p)	((p) + 2)

#define	StbBakeru(p)	(*(p) & 0x08)

#define	StbHinsi(p)	(*((p) + 1))


extern	Uchar	charsize[], chrtbl[], *conjadr[], connadr[];
extern  Uchar   *fzkadr[], kigou[], kurai1_tbl[], kurai2_tbl[];
extern	Uchar	num1tbl[], num2tbl[], num3tbl[], num4tbl[];
extern	Uchar	num5tbl[], num6tbl[], *rigtadr[], scncttbl[2][24];
extern	Uchar	*settou_ptr[], *stbadr[], sttpri[5][31];
extern  Uchar   suuji_tbl[], taipri[7][45], termtbl[];
extern	Ushort	*selsjadrs[];
extern	UCPFunc	getkan_func[], makekan_func[];
extern	VFunc	cvtnum_func[];
extern  IFunc	hiraknj_func[], setjrec_func[], srchg_func[];
#define	Charsize	charsize
#define	Chrtbl		chrtbl
#define	Conjadr(i)	conjadr[i]
#define	Connadr		connadr
#define	Cvtnum_func	cvtnum_func
#define	Fzkadr(i)	fzkadr[i]
#define	Getkan_func	getkan_func
#define	Hiraknj_func	hiraknj_func
#define	Kigou		kigou
#define	Kurai1_tbl	kurai1_tbl
#define	Kurai2_tbl	kurai2_tbl
#define	Makekan_func	makekan_func
#define	Num1tbl		num1tbl
#define	Num2tbl		num2tbl
#define	Num3tbl		num3tbl
#define	Num4tbl		num4tbl
#define	Num5tbl		num5tbl
#define	Num6tbl		num6tbl
#define	Rigtadr(i)	rigtadr[i]
#define	Scncttbl(i, j)	scncttbl[i][j]
#define	Selsjadrs(i)	selsjadrs[i]
#define	Setjrec_func	setjrec_func
#define	Settou_ptr(i)	settou_ptr[i]
#define	Srchg_func	srchg_func
#define	Stbadr(i)	stbadr[i]
#define	Sttpri(i, j)	sttpri[i][j]
#define	Suuji_tbl	suuji_tbl
#define	Taipri(i, j)	taipri[i][j]
#define	Termtbl		termtbl


#define	codesize(ch)	(Charsize[(ch & KanjiModeMask) >> 4])

#endif

