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
 *	sj_ktbl.c
 *	katakana/hiragana conversion table.
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Fri May  8 16:55:12 JST 1987
 *
 *	This file is included Shift-JIS code.
 */
/*
 * $Header: sj_ktbl.c,v 4.0 90/11/28 13:03:16 root Rel40 $ SONY;
 */

char *Ktable[][3] = {
	"ァ",	"ぁ",	"ｧ",
	"ア",	"あ",	"ｱ",
	"ィ",	"ぃ",	"ｨ",
	"イ",	"い",	"ｲ",
	"ゥ",	"ぅ",	"ｩ",
	"ウ",	"う",	"ｳ",
	"ェ",	"ぇ",	"ｪ",
	"エ",	"え",	"ｴ",
	"ォ",	"ぉ",	"ｫ",
	"オ",	"お",	"ｵ",
	"カ",	"か",	"ｶ",
	"ガ",	"が",	"ｶﾞ",
	"キ",	"き",	"ｷ",
	"ギ",	"ぎ",	"ｷﾞ",
	"ク",	"く",	"ｸ",
	"グ",	"ぐ",	"ｸﾞ",
	"ケ",	"け",	"ｹ",
	"ゲ",	"げ",	"ｹﾞ",
	"コ",	"こ",	"ｺ",
	"ゴ",	"ご",	"ｺﾞ",
	"サ",	"さ",	"ｻ",
	"ザ",	"ざ",	"ｻﾞ",
	"シ",	"し",	"ｼ",
	"ジ",	"じ",	"ｼﾞ",
	"ス",	"す",	"ｽ",
	"ズ",	"ず",	"ｽﾞ",
	"セ",	"せ",	"ｾ",
	"ゼ",	"ぜ",	"ｾﾞ",
	"ソ",	"そ",	"ｿ",
	"ゾ",	"ぞ",	"ｿﾞ",
	"タ",	"た",	"ﾀ",
	"ダ",	"だ",	"ﾀﾞ",
	"チ",	"ち",	"ﾁ",
	"ヂ",	"ぢ",	"ﾁﾞ",
	"ッ",	"っ",	"ｯ",
	"ツ",	"つ",	"ﾂ",
	"ヅ",	"づ",	"ﾂﾞ",
	"テ",	"て",	"ﾃ",
	"デ",	"で",	"ﾃﾞ",
	"ト",	"と",	"ﾄ",
	"ド",	"ど",	"ﾄﾞ",
	"ナ",	"な",	"ﾅ",
	"ニ",	"に",	"ﾆ",
	"ヌ",	"ぬ",	"ﾇ",
	"ネ",	"ね",	"ﾈ",
	"ノ",	"の",	"ﾉ",
	"ハ",	"は",	"ﾊ",
	"バ",	"ば",	"ﾊﾞ",
	"パ",	"ぱ",	"ﾊﾟ",
	"ヒ",	"ひ",	"ﾋ",
	"ビ",	"び",	"ﾋﾞ",
	"ピ",	"ぴ",	"ﾋﾟ",
	"フ",	"ふ",	"ﾌ",
	"ブ",	"ぶ",	"ﾌﾞ",
	"プ",	"ぷ",	"ﾌﾟ",
	"ヘ",	"へ",	"ﾍ",
	"ベ",	"べ",	"ﾍﾞ",
	"ペ",	"ぺ",	"ﾍﾟ",
	"ホ",	"ほ",	"ﾎ",
	"ボ",	"ぼ",	"ﾎﾞ",
	"ポ",	"ぽ",	"ﾎﾟ",
	"マ",	"ま",	"ﾏ",
	"ミ",	"み",	"ﾐ",
	"ミ",	"み",	"ﾐ",
	"ム",	"む",	"ﾑ",
	"メ",	"め",	"ﾒ",
	"モ",	"も",	"ﾓ",
	"ャ",	"ゃ",	"ｬ",
	"ヤ",	"や",	"ﾔ",
	"ュ",	"ゅ",	"ｭ",
	"ユ",	"ゆ",	"ﾕ",
	"ョ",	"ょ",	"ｮ",
	"ヨ",	"よ",	"ﾖ",
	"ラ",	"ら",	"ﾗ",
	"リ",	"り",	"ﾘ",
	"ル",	"る",	"ﾙ",
	"レ",	"れ",	"ﾚ",
	"ロ",	"ろ",	"ﾛ",
	"ヮ",	"ゎ",	"ﾜ",
	"ワ",	"わ",	"ﾜ",
	"ヰ",	"ゐ",	"ｨ",
	"ヱ",	"ゑ",	"ｴ",
	"ヲ",	"を",	"ｦ",
	"ン",	"ん",	"ﾝ",
	"ヴ",	"ぶ",	"ｳﾞ",
	"ヵ",	"か",	"ｶ",
	"ヶ",	"け",	"ｹ",
	"゛",	"゛",	"ﾞ",
	"゜",	"゜",	"ﾟ",
	"",	"",	""
};

