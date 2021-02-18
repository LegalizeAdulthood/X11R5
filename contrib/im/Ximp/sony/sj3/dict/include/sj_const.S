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

#define	IllegalFormat		"フォーマットが異常です"
#define	TooLongYomi		"読み文字列が長すぎます"
#define	TooLongKanji		"漢字文字列が長すぎます"
#define	TooLongHinsi		"品詞文字列が長すぎます"
#define	TooLongGroup		"グループ名が異常です"
#define	TooLongJosi		"助詞文字列が異常です"
#define	BadHinsi		"登録されていない品詞です"
#define	BadGroup		"登録されていないグループです"
#define	BadJosi			"登録されていない助詞です"
#define	NoYomiString		"読み文字列が取得できません"
#define	NoKanjiString		"漢字文字列が取得できません"
#define	TooManyAtr		"属性の数が多すぎます"
#define	TooManyHinsi		"品詞の数が多すぎます"
#define	TooManyJosi		"助詞の数が多すぎます"
#define	NoDataInMain		"メイン辞書に存在しません"
#define	TooManyTarget		"品詞を１つ指定してください"

#define	HinsiTankan	cnvhinsi("単漢")

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
