/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */
/* "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/jptaiwa.h,v 3.1 91/07/01 15:09:40 kon Exp $" */
/* "@(#)jptaiwa.h	R14.2.0.0 88/02/19 09:23:58 " */
/************************************************************************/
/*	連文節カナ漢字変換　対話変換機能諸定数				*/
/************************************************************************/

/************************************************************************/
/* 	対話処理モード 							*/
/************************************************************************/

#define	DATAIN		0		/* カナ漢字変換			*/
#define	TAIWAINIT	1		/* 変換情報の初期化		*/
#define	TAIWAAUTO	2		/* 対話処理の開始		*/
#define	TAIWAEND	3		/* 対話処理の終了		*/

/************************************************************************/
/*	入力キ−コ−ド（制御キ−）					*/
/************************************************************************/

#define KANJI		0x01		/* 漢字入力終了			*/
#define HEXCODE		0x02		/* １６進コ−ド入力開始		*/
#define BUSHU		0x03		/* 部首入力変換開始／終了	*/
#define CHGROMA        	0x04		/* ロ−マ字／カナ入力切り替え	*/
#define CHGHENKAN	0x05		/* 変換モ−ド切り替え		*/
#define CHGKATAKANA    	0x06		/* カタカナ／ひらがな切り替え	*/
#define CHGJISYO	0x07		/* 辞書変更			*/
#define LEFT        	0x08		/* リバ−ス表示左移動（縮小）	*/
#define RIGHT       	0x09		/* リバ−ス表示右移動（拡大)	*/
#define REVLEFT		0x0a		/* リバ−ス文節左移動		*/
#define MUHENKAN	0x0b		/* リバ−ス文節右移動（無変換）	*/
#define HENKAN		0x0c		/* 漢字変換			*/
#define KAKUTEI    	0x0d		/* 全文節確定			*/
#define TORIKESI	0x0e		/* 取り消し			*/
#define TAB         	0x0f		/* タブ				*/
#define BACKSPACE   	0x10		/* バックスペ−ス		*/
#define REDRAW      	0x11
#define TOHIRAGANA	0x12		/* リバ−ス部分のひらがな変換	*/
#define TOKATAKANA	0x13		/* リバ−ス部分のカタカナ変換	*/

/************************************************************************/
/*	対話変換機能メインステ−タス（処理モ−ドがDATAINの時の戻り値）	*/
/************************************************************************/

#define	MODE_CHG	1		/* モ−ド変化			*/
#define	MIDASI		2		/* カナ見出し入力中		*/
#define RET_KEY		3		/* 特殊キ−入力			*/
#define	HEX_IN		4 		/* １６進コ−ド入力中		*/
#define	DOUON		5		/* 同音語選択処理		*/
#define	MUSI	        6 		/* 無効入力			*/
#define	BUSHU_IN	7		/* 部首変換入力中		*/
#define	KIGOU_IN	8		/* 記号一覧中			*/
#define	TAN_TOUROKU_IN	9		/* 単語登録中			*/
#define DIOERR          -1		/* 辞書アクセスエラ−		*/

/************************************************************************/
/* 	各メインステ−タスに対するサブステ−タス  			*/
/************************************************************************/

/*	メインステ−タス　＝　MODE_CHG					*/
#define	KANA_M		1		/* ひらがな／カタカナモ−ド変化	*/
#define	HENKAN_M	2		/* 漢字変換モ−ド変化		*/
#define	HEX_M		3		/* ＪＩＳコ−ド入力モ−ド	*/
#define	BUSHU_M		4		/* 部首変換入力モ−ド		*/
#define	KANJI_M		5		/* 日本語入力終了		*/
#define	ROMA_M		6		/* ロ−マ字／カナ入力モ−ド変化	*/
#define	JISHO_M		7		/* 辞書切り替え			*/
#define	ZENHIRAKAKU_M	8		/* 全角ひらがな確定入力モード   */
#define	ZENKATAKAKU_M	9		/* 全角カタカナ確定入力モード   */
#define	HANKATAKAKU_M	10		/* 半角カタカナ確定入力モード   */
#define	ZENALPHAKAKU_M	11		/* 全角アルファベット確定入力モード */
#define	HANALPHAKAKU_M	12		/* 半角アルファベット確定入力モード */
#define	KIGOU_M		13		/* 記号入力モード */
#define	TAN_TOUROKU_M	14		/* 単語登録モード */
#define	TAN_TOUROKU_HIN_M	15	/* 単語登録品詞選択モード */
#define	TAN_TOUROKU_DIC_M	16	/* 単語登録辞書選択モード */

/*	メインステ−タス　＝　RET_KEY					*/
#define	KEY_KAKUTEI	0		/* 確定キ−			*/
#define	KEY_TAB		1		/* タブキ−			*/
#define	KEY_BS		2		/* バックスペ−スキ−		*/
#define	KEY_HENKAN	3		/* 変換キ−			*/
#define KEY_REDRAW	4

/*	メインステ−タス　＝　HEX_IN					*/
#define	HEX_ERR		1		/* 入力エラ−，取り消し		*/
#define	HEX_POS		2		/* １６進数入力中		*/
#define	HEX_TRAN	3		/* 変換成功（終了）		*/
 
/*	メインステ−タス　＝　DOUON					*/
#define	D_HYOUJI	0		/* 同音語候補表示		*/
#define	D_END		1		/* 同音語選択終了		*/

/*	メインステ−タス　＝　BUSHU_IN					*/
#define	BU_MIDASI	1		/* 部首見出し入力中		*/
#define	BU_KOUHO	2		/* 候補表示			*/
#define	BU_END		3		/* 入力確定（終了）		*/

/*	メインステ−タス　＝　KIGOU_IN					*/
#define	KI_KOUHO	1		/* 記号候補表示中		*/
#define	KI_END		2		/* 入力確定（終了）		*/

/*	メインステ−タス　＝　TAN_TOUROKU_IN				*/
#define	ADD_INPUT	1		/* 入力中			*/
#define	ADD_END		2		/* 登録終了			*/

/************************************************************************/
/* 	カナ漢字変換モード 						*/
/************************************************************************/

#define	CHIKUJI		0		/* 逐次自動変換			*/
#define	RENBUN		1		/* 連文節変換			*/
#define	TANBUN		2		/* 単文節変換			*/

/************************************************************************/
/* 	ひらがな／カタカナモード					*/
/************************************************************************/

#define	IN_HIRAGANA	0		/* ひらがな入力(変換入力)	*/
#define	IN_KATAKANA	1		/* カタカナ入力(確定入力)	*/
#define	IN_ALPHA	2		/* アルファベット入力(確定入力)	*/

/************************************************************************/
/* 	ロ−マ字／カナ入力モ−ド					*/
/************************************************************************/

#define IN_ROMAJI	-1		/* ロ−マ字入力			*/
#define IN_KANA		0		/* カナ入力			*/

/************************************************************************/
/* 	辞書								*/
/************************************************************************/

#include <jpdef.h>

/************************************************************************/
/* 	その他								*/
/************************************************************************/

#define	KOUHO_SIZE	128 		/* 同音語表示域の大きさの最大	*/
#define MIN_KOUHO_SIZE  34		/* 同音語表示域の大きさの最小	*/
#define MIN_BUSHU_KOUHO_SIZE	4	/* 部首候補表示域の大きさの最小	*/

/************************************************************************/
/* 	連文節ライブラリ諸元						*/
/************************************************************************/

#include <japanren.h>

/************************************************************************/
/* 	対話処理制御テ−ブル						*/
/************************************************************************/

struct taiwaInf	{
	unsigned char	*message;	/* メッセ−ジバッファ		*/
	int		messageLength;	/* メッセ−ジ長			*/
	unsigned char	*kakutei;	/* 確定文字列バッファ		*/
	int		kakuteiLength;	/* 確定文字列長			*/
	int		reversePosition;/* メッセ−ジのリバ−ス表示位置	*/
	int		reverseLength;	/* メッセ−ジのリバ−ス表示長	*/
	int		subStatus;	/* サブステ−タス		*/
	int		henkanMode;	/* カナ漢字変換モ−ド		*/
	int		kanaMode;	/* ひらがな／カタカナ入力モ−ド	*/
	int		romajiMode;	/* ロ−マ字／カナ入力モ−ド	*/
	int		jisyo;		/* カレント辞書			*/
	int		kouhoSize;	/* 同音語候補表示域の大きさ	*/
	unsigned char	*jrdic;		/* 利用者指定のシステム辞書	*/
	unsigned char	*jgdic;		/* 利用者指定の専門語辞書	*/
	unsigned char	*judic;		/* 利用者指定の個人辞書		*/
};
