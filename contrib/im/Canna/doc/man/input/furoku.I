付録Ｉ　カスタマイズファイルの例

　以下にカスタマイズファイルの例として jdaemon.iroha および just.iroha 
を示します。それぞれのファイルは /usr/lib/iroha/sample の下に存在しま
すので実際に環境変数 IROHAFILE で指定するなどして試してみて下さい。

(1) jdaemon.iroha

　EWS-UX/V R6.1 における日本語入力システムとほぼ同じユーザインタフェー
スを提供します。

# Copyright 1991 NEC Corporation, Tokyo, Japan.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation for any purpose and without fee is hereby granted,
# provided that the above copyright notice appear in all copies and that
# both that copyright notice and this permission notice appear in
# supporting documentation, and that the name of NEC Corporation
# not be used in advertising or publicity pertaining to distribution
# of the software without specific, written prior permission.  NEC 
# Corporation makes no representations about the suitability of this
# software for any purpose.  It is provided "as is" without express
# or implied warranty.
#
# NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
# NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
# USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
# OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
# PERFORMANCE OF THIS SOFTWARE. 

# Using this customize file gives you a Japanese Input Interface
# like EWS-UX/v R6.1 does.

# @(#) 102.1 $Header: /work/nk.R3.1/doc/nyuryoku/RCS/furoku.I,v 1.1 91/06/14 10:12:01 satoko Exp $


InitialMode	hanAlphaKakuteiMode

romkanaTable	"jdaemon.rdic"

dictionary	"iroha"
dictionary	"fuzokugo"
dictionary	"hojomwd"
dictionary	"hojoswd"

bushudic	"bushu"

userdic		"user"

cursorWrap	       	on	# default on
selectDirect	       	on	# default on
numericalkeySelect     	on	# default on
characterBasedMove     	on	# default on
reverseWidely	       	on	# default off
breakIntoRoman		off	# default off
quitIfEndOfIchiran    	on	# default off
kakuteiIfEndOfBunsetsu 	on	# default off
stayAfterValidate      	off	# default on

# setting for mode string

modeString(alpha)		NULL
modeString(yomiganai)		"Ｒ全角逐次かなシ"
modeString(hex)		       	"Ｒ全角１６進　シ"
modeString(bushu)		"Ｒ全角部首"
modeString(henkanNyuryoku)	"Ｒ全角逐次かなシ"
modeString(zenHiraKakutei)	NULL
modeString(zenKataKakutei)	NULL
modeString(hanKataKakutei)	"Ｒ"
modeString(zenAlphaKakutei)     "　全角逐次かなシ"
modeString(hanAlphaKakutei)     ""
modeString(kigo)		"Ｒ全角記号　　シ"
modeString(mojishu)		NULL
modeString(tankouho)		NULL
modeString(ichiran)		NULL
modeString(yesno)		NULL
modeString(extend)		NULL
modeString(russian)		"Ｒ全角ロシア　シ"
modeString(greek)		"Ｒ全角ギリシャシ"
modeString(line)		"Ｒ全角罫線　　シ"
modeString(changingServer)	NULL
modeString(henkanMethod)	NULL
modeString(deleteDic)		NULL
modeString(touroku)		"Ｒ全角登録　　シ"
modeString(tourokuHinshi)	"Ｒ全角登録品詞シ"
modeString(tourokuDic)		"Ｒ全角登録辞書シ"
modeString(quotedInsert)	"Ｒ全角ｑ　　　シ"
modeString(bubunMuhenkan)	"Ｒ全角逐次編集シ"
modeString(zenKataHenkan)	NULL
modeString(hanKataHenkan)	NULL
modeString(zenAlphaHenkan)	NULL
modeString(hanAlphaHenkan)	NULL
modeString(mountDic)		NULL

# setting for keymapping

quotedInsert		undefine
extend			undefine
shrink			undefine
alphaMode		undefine
quotedInsert		undefine
forward			undefine
backward		undefine
previous		undefine
next			undefine
beginningOfLine		undefine
endOfLine		undefine
deleteNext		undefine
killToEndOfLine		undefine
henkan			undefine
quit			undefine
japaneseMode		undefine
selfInsert		space

henkan			^x
henkan			down
henkan			xfer
Forward			^o
Backward		^n
quit			^c

hanAlphaKakutei {
	hanKataKakuteiMode	^r
	zenAlphaKakuteiMode	^e
}

yomiganai {
	hanKataKakuteiMode	^e
	zenAlphaKakuteiMode	^r
	BushuMode		^b
	HexMode			^w
	ZenKataKakuteiMode	^k
}

ZenKataKakutei {
	hanKataKakuteiMode	^e
	zenAlphaKakuteiMode	^r
	BushuMode		^b
	HexMode			^w
	HenkanNyuryokuMode	^k
}

HanKataKakutei {
	hanAlphaKakuteiMode	^r
	henkanNyuryokuMode	^e
	BushuMode		^b
	HexMode			^w
}

ZenAlphaKakutei {
	hanAlphaKakuteiMode	^e
	henkanNyuryokuMode	^r
	BushuMode		^b
	HexMode			^w
}

yomi {
	hiragana		^a
	katakana		^z
 	selfInsert,henkan	.
 	selfInsert,henkan	\,
	selfInsert,henkan	>
	selfInsert,henkan	<
	selfInsert,henkan	!
	selfInsert,henkan	?
}

mojishu {
	extend			right
	shrink			left
	hiragana		^a
	katakana		^z
}

tankouho {
	extend			right
	shrink			left
	hiragana		^a
	katakana		^z
	KouhoIchiran		xfer
	KouhoIchiran		down
	prevKouho		backspace
}

ichiran 
{
	Quit			0
	prevKouho		backspace
	nextKouhoRetsu		down
	prevKouhoRetsu		up
}

(2) just.iroha

　一太郎における日本語入力操作をシミュレートするカスタマイズファイルで
す。ローマ字かな変換テーブルに専用のテーブルを使っています。また、主に
以下のような操作法を提供しています。

		キー	機能
	      ────────────
		space	変換
		return	確定
		C-u	ひらがな変換
		C-i	カタカナ変換
		C-o	半角変換
		C-p	ローマ字変換
		C-n	右文節移動
		→、C-l	右移動
		←、C-k	左移動
		↓	確定
		

# Copyright 1991 NEC Corporation, Tokyo, Japan.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation for any purpose and without fee is hereby granted,
# provided that the above copyright notice appear in all copies and that
# both that copyright notice and this permission notice appear in
# supporting documentation, and that the name of NEC Corporation
# not be used in advertising or publicity pertaining to distribution
# of the software without specific, written prior permission.  NEC 
# Corporation makes no representations about the suitability of this
# software for any purpose.  It is provided "as is" without express
# or implied warranty.
#
# NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
# NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
# USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
# OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
# PERFORMANCE OF THIS SOFTWARE. 

# Using this customize file gives you a Japanese Input Interface
# like ichitaro does.

# @(#) 102.1 $Header: /work/nk.R3.1/doc/nyuryoku/RCS/furoku.I,v 1.1 91/06/14 10:12:01 satoko Exp $

#InitialMode		hanAlphaKakuteiMode

romkanaTable	"just.rdic"

dictionary	"iroha"
dictionary	"fuzokugo"
dictionary	"hojomwd"
dictionary	"hojoswd"

bushudic	"bushu"

userdic		"user"

cursorWrap		off	# default on
selectDirect		on	# default on
numericalkeySelect	on	# default on
characterBasedMove	off	# default on
reverseWidely		off	# default off
breakIntoRoman		off	# default off
stayAfterValidate	off	# default on
quitIfEndOfIchiran	on	# default off
kakuteiIfEndOfBunsetsu	on	# default off
nHenkanForIchiran	2	# default 2

# setting for keymapping

yomi {
	Hiragana		C-u
	Katakana		C-i
	Hankaku			C-o
	Romaji			C-p
	Forward			C-l
	Backward		C-k
	Kakutei			C-n
	Forward			right
	Backward		left
	Kakutei			down
}


tankouho {
	Hiragana		C-u
	Katakana		C-i
	Hankaku			C-o
	Romaji			C-p
	Forward			C-n
	Extend			C-l
	Shrink			C-k
	Extend			right
	Shrink			left
	Kakutei			down
}

mojishu {
	Hiragana		C-u
	Katakana		C-i
	Hankaku			C-o
	Romaji			C-p
	Extend			C-l
	Shrink			C-k
	Kakutei			C-n
	Extend			right
	Shrink			left
	Kakutei			down
}

ichiran {
	Kakutei			C-n
	Extend			right
	Extend			C-l
	Shrink			left
	Shrink			C-k
	nextKouhoRetsu		xfer
}
