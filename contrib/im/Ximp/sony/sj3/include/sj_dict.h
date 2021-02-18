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

#ifndef	_SJ_DICT

#define	_SJ_DICT

#define	DIC_MAIN		0
#define	DIC_USER		1


#define	DicSegBase		0
#define	MaxDouonBlk		0xfff
#define	DouonBlkSizeNumber	3
#define	DicSegTerm		0xff
#define	DouonBlkTerm		0xff
#define	HinsiBlkTerm		0xff
#define	MaxSegmentNumber	0xffff



#define	NormalKanji0		0x00
#define	NormalKanji1		0x10
#define	ZenHiraAssyuku		0x20
#define	ZenKataAssyuku		0x30
#define	KanjiAssyuku		0x40
#define	OffsetAssyuku		0x50
#define	NormalKanji2		0x60
#define	AiAttribute		0x70
#define	KanjiStrEnd		0x80
#define	KanjiModeMask		0x70
#define	KanjiCodeMask		0x0f
#define	LeadingHankaku		0x80
#define	AtrMsKanjiOfs		0xf000

#define	MaxKnjAskNumber		16
#define	MaxYomiAskNumber	16

#define	MaxWdYomiLen		31
#define	MaxWdKanjiLen		64

#define	AssyukuYomiTop		0x80
#define	NokoriYomiTop		0x40


#define getsize(p)	(((Int)(*(p) & 0x0f) * 0x100) + *((p) + 1))

#define	getntag(p)	((*(p) == DicSegTerm) ? (p) : (p) + getsize(p))

#define	getplen(p)	(((*(p) & AssyukuYomiTop)?0x10:0) + (*((p)+2) >> 4))

#define	getnlen(p)	(((*(p) & NokoriYomiTop)?0x10:0) + (*((p)+2) & 0x0f))

#define	segtop()	(dicbuf + *dicbuf + 1)

#define	segend(p)	(*(p) == DicSegTerm || (p) >= dicbuf + curdict->seglen)


#endif

