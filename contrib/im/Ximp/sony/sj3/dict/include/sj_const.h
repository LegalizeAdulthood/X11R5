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

#ifndef	_SJ_CONST_
#define	_SJ_CONST_

#define	MainSegmentLength	2048
#define	MainIndexLength		2048
#define	MainIndexNumber		512

#define	MaxYomiLength		32
#define	MaxKanjiLength		32
#define	MaxHinsiNumber		16
#define	MaxAtrNumber		16
#define	MaxJosiNumber		16

#define	MaxKnjAskNumber		16
#define	MaxYomiAskNumber	16

#define	DouonBlkSizeNumber	3

#define	DouonBlkTerm		0xff
#define	HinsiBlkTerm		0xff

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

#define	MaxHindoNumber		2000
#define	MaxOffsetNumber		1000

#define	IllegalFormat		"\203\164\203\110\201\133\203\175\203\142\203\147\202\252\210\331\217\355\202\305\202\267"
#define	TooLongYomi		"\223\307\202\335\225\266\216\232\227\361\202\252\222\267\202\267\202\254\202\334\202\267"
#define	TooLongKanji		"\212\277\216\232\225\266\216\232\227\361\202\252\222\267\202\267\202\254\202\334\202\267"
#define	TooLongHinsi		"\225\151\216\214\225\266\216\232\227\361\202\252\222\267\202\267\202\254\202\334\202\267"
#define	TooLongGroup		"\203\117\203\213\201\133\203\166\226\274\202\252\210\331\217\355\202\305\202\267"
#define	TooLongJosi		"\217\225\216\214\225\266\216\232\227\361\202\252\210\331\217\355\202\305\202\267"
#define	BadHinsi		"\223\157\230\136\202\263\202\352\202\304\202\242\202\310\202\242\225\151\216\214\202\305\202\267"
#define	BadGroup		"\223\157\230\136\202\263\202\352\202\304\202\242\202\310\202\242\203\117\203\213\201\133\203\166\202\305\202\267"
#define	BadJosi			"\223\157\230\136\202\263\202\352\202\304\202\242\202\310\202\242\217\225\216\214\202\305\202\267"
#define	NoYomiString		"\223\307\202\335\225\266\216\232\227\361\202\252\216\346\223\276\202\305\202\253\202\334\202\271\202\361"
#define	NoKanjiString		"\212\277\216\232\225\266\216\232\227\361\202\252\216\346\223\276\202\305\202\253\202\334\202\271\202\361"
#define	TooManyAtr		"\221\256\220\253\202\314\220\224\202\252\221\275\202\267\202\254\202\334\202\267"
#define	TooManyHinsi		"\225\151\216\214\202\314\220\224\202\252\221\275\202\267\202\254\202\334\202\267"
#define	TooManyJosi		"\217\225\216\214\202\314\220\224\202\252\221\275\202\267\202\254\202\334\202\267"
#define	NoDataInMain		"\203\201\203\103\203\223\216\253\217\221\202\311\221\266\215\335\202\265\202\334\202\271\202\361"
#define	TooManyTarget		"\225\151\216\214\202\360\202\120\202\302\216\167\222\350\202\265\202\304\202\255\202\276\202\263\202\242"

#define	HinsiTankan	cnvhinsi("\222\120\212\277")

#define	FALSE			0
#define	TRUE			(!FALSE)

#define	DIVLIST		1
#define	ASSYUKUREC	2
#define	STRING		3
#define	HINDOREC	4
#define	OFFSETREC	5
#define	KANJIREC	6
#define	HINSIREC	7
#define	DOUONREC	8

#endif
