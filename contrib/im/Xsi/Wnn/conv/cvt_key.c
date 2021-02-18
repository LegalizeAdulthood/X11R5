/*
 * $Id: cvt_key.c,v 1.2 1991/09/16 21:28:57 ohm Exp $
 */
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/***********************************************************************
			convert_key.c			 ＿＿＿
						87/12/19｜訂補｜
							 ￣￣￣
	タイムアウト処理を伴う、ファンクションキーコンバート処理。
***********************************************************************/
/*	Version 4.0
 */
#include <signal.h>
#include <setjmp.h>
#include "cvt_head.h"

/*
	keyin1()を呼び出すためには、次の手続きが必要です。

     １ tgetent()（4.2BSD）もしくはsetupterm()（System V）によって、
	キーボードのterm情報が入っているエントリを読み込みます。
     ２ convert_getstrs() によって、読んだ情報を専用のエリアにコピーします。
	これによって、他のエントリのterm情報を読み込んでも、１で得た情報が
	失われなくなります。
     ３ convert_key_setup()を呼び、変換の初期設定をします。

	convert_getterm()によって、１と２が一度にできます。
	また、convert_key_init()によって２と３が、
	convert_getterm_init()によって１から３までが、それぞれ一度にできます。

	なお、これらの convert_ で始まるライブラリ関数は、convert_read.c にて
	定義されています。

	4.2BSDの場合に旧版で呼ぶ必要のあったgetterm_fkeydata()は、
	不必要になったため廃止されました。
*/

extern	struct	CONVCODE tbl[];
extern	int	cnv_tbl_cnt;	/* convert table count */


 /** intの配列 h をビットベクタとみなし、第iビットをチェックあるいは立てる */
#define BITONP(h, i) (h[i / BITSIZ] &  (1 << (i % BITSIZ)))
#define BITOFP(h, i) (!BITONP(h, i))
#define BIT_UP(h, i) (h[i / BITSIZ] |= (1 << (i % BITSIZ)))
#define BITDWN(h, i) (h[i / BITSIZ] &= ~(1 << (i % BITSIZ)))

 /** 変換コードのチェックとコード変換 */
int
key_check(inbuf, conv_tbl, tbl_cnt, check_flg)
int	inbuf[];		/* ソースストリング */
struct	CONVCODE conv_tbl[];	/* コード変換テーブル */
int	tbl_cnt;
int	check_flg[];
{
	register int	dist, base;
	register char	*code_p;
	int	i;

	for(base = 0; inbuf[base] != -1; base++){
		for(dist = 0; dist < tbl_cnt; dist++){
			if(BITONP(check_flg, dist) &&
			   conv_tbl[dist] . fromkey != 0){
				code_p = conv_tbl[dist] . fromkey + base;
				if(*code_p == (char)inbuf[base]){
					if(*(code_p + 1) == (char)0){
						 /* マッチした */
						for(i = 0, base++;
						    (inbuf[i] = inbuf[base])
							!= -1;
						    i++, base++);
						return(conv_tbl[dist]. tokey);
					}
					/* まだマッチしていない */
				} else {
					BITDWN(check_flg, dist); /* 無効 */
				}
			}
		}
	}

	 /* ビットベクタ check_flg[] の第0〜tblcntビットに立ったまま残っている
	    ものがあるか調べる。*/
	for(i = 0; i < tbl_cnt / BITSIZ; i++){
		if(check_flg[i]) return(-1);
	}
	if((tbl_cnt %= BITSIZ) && (check_flg[i]& ~(~0 << tbl_cnt)))return(-1);
	 /* return -1 … まだ未決定の物がある */

	return(-2);	/* 変換対象となる物はない */
}

 /** 指定された変換テーブルに従ってコード変換する。*/
int
convert_key(inkey, conv_tbl, tbl_cnt, matching_flg)
int	(*inkey)();		/* キー入力関数 */
struct	CONVCODE conv_tbl[];	/* 変換テーブル */
int	tbl_cnt;		/* conv_tbl[] の個数 */
int	matching_flg;		/* マッチングしなかったストリングの処理指定
					0 : 返値として返す
					1 : そのストリングは捨てる	*/
{
#define MAX	20		/* キー入力バッファの最大値 */

	static	int	inbuf[MAX];	/* キー入力バッファ */
		/* バッファの終端は、-1 で示される。*/

	int	out_cnt;		/* 出力バッファの出力カウント */

	static	int	buf_cnt = 0;	/* inbuf の入力時のカウンタ   */

	int	check_flg[CHANGE_MAX];
	 /* ビットベクタとして扱われ、マッチング時に対象となっているconv_tbl[]
	    を示す。1の時対象となり、0で非対象 */

	int	i, c, flg = 0;		/* work */

	for(i = 0; i < div_up(tbl_cnt, BITSIZ); check_flg[i++] = ~0);
     /* 配列check_flgをビットベクタ扱いし、その第0〜tbl_cnt ビットを立てる。
	但し、実際はその少し先まで立つ */

	for(;;){
	    if(flg != 0 || buf_cnt == 0){
		if(buf_cnt > 0){
		    if ((inbuf[buf_cnt] = (*inkey)()) == -1) { /* 一文字入力 */
			c = -2;	/* タイムアウト */
			goto LABEL;
		    } else {
			inbuf[++buf_cnt] = -1; /* ターミネータ */
		    }
		} else {
		    if ((inbuf[buf_cnt] = (*inkey)()) == -1) { /* 一文字入力 */
			continue;
		    } else {
			inbuf[++buf_cnt] = -1; /* ターミネータ */
		    }
		}
	    }
	    flg++;

	    if(buf_cnt >= MAX - 1) return(-1);	/* ERROR */

	    c = key_check(inbuf, conv_tbl, tbl_cnt, check_flg);
LABEL:
	    switch(c){
		    case -1:	/* 未決定 */
			    continue;

		    case -2:	/* 変換対象でないことが決定した */
			    buf_cnt--;
			    out_cnt = 0;
			    c = inbuf[out_cnt++];
			    for(i = 0; inbuf[i] != -1;
				inbuf[i++] = inbuf[out_cnt++]);
			    if(matching_flg != 0){
				    flg = 0;
				    continue;
			    }
			    return(c);

		    default:	/* 変換されたコード */
			    buf_cnt = 0;
			    return(c);
	    }
	}
}

 /** コード変換を伴うキー入力関数 */
int
keyin1(get_ch)
int	(*get_ch)();	/* getchar() と同様の関数 */
{
	int ret;

	for ( ; ; ) {
	    if(cnv_tbl_cnt == 0) {
		ret = (*get_ch)();
		if (ret >= 0) 
		    return(ret);
	    } else {
		return(convert_key(get_ch, tbl, cnv_tbl_cnt, 0));
	    }
	}
}
