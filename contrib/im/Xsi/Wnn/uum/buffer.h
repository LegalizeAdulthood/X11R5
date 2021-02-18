/*
 * $Id: buffer.h,v 1.2 1991/09/16 21:33:25 ohm Exp $
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
/*	Version 4.0
 */
/* bufferの表示のされ方は、hantenによって決定される。
反転するか、下線状態にするかを各独立に、
バッファーの内で、文頭からmarkまで、markからcursorまで、
cursorからbufferの最後までの状態を独立に設定することができる。
	 when mark is set
        	hanten bit0(0x01),bit1(0x02):      --- mark
		bit2(0x04),bit3(0x08):        mark --- cursor
		bit4(0x10),bit5(0x20):      cursor --- 
	  when mark is not set(-1)
		bit4,bit5:      
		bit6(0x40):	t_b_st --- t_b_end ボールド表示
    first one....hanten
    second one...under_line
*/
struct buf{
  w_char  *buffer;	/* 登録用バッファ */
  char *buf_msg; /** 仮名を入力する時のバッファーの前に表示する文字列 */
  int start_col; /** 仮名入力時の画面上のコラムのスタート位置 */
  int t_c_p; /** 仮名入力用バッファー上でのカーソル位置 */
  int hanten; /** 仮名入力用バッファーを反転表示させているかどうか */
  int maxlen; /**仮名入力用バッファーの中にはいっている文字数 */
  int buflen; /** length of the buffer */
  int t_m_start;  	/* -1 : markが設定されていない。*/
			/* 0 以上 : マークの位置*/
  int t_b_st;	/**ボールド表示の開始位置 */
  int t_b_end;	/**ボールド表示の終了位置 */
  int vlen;    /*バッファー中でスクリーンに表示される幅*/
  int duplicate;/* スクリーンがスクロールする時に、重複して表示する文字数*/

  int vst;    /* bufferの中で画面に表示されている最初の位置*/
		/*これは、勝手に上のルーチンでいじらない方が望ましい。*/
  int (*key_in_fun)(); /*ここに関数が設定されていると、key_tableにバインドが
  ない文字の入力を
  受けた時にバッファーにその文字を入れずに代わりにその関数が呼ばれる*/
  int (*redraw_fun)();/* romkan kara redraw ga kaette kitatoki */
  int (*ctrl_code_fun)();/*ここに関数が設定されていると、key_tableにバインドが
  ないコントロール文字の入力を受けた時にベルを鳴らす代わりに
  その関数が呼ばれる*/
  int (**key_table)();	/*キーバインドのテーブル*/
  char *rk_clear_tbl;	/* romakn_clear が hituyouka */
};


extern int *bunsetsu;
extern int *bunsetsuend;
extern struct wnn_env **bunsetsu_env;	/* 文節の変換に使った env */
extern int *touroku_bnst;
/* extern int bunsetsucnt; */
extern int touroku_bnst_cnt;

extern struct buf *c_b; /* 現在用いられているbufを示すグローバル変数*/
extern w_char *kill_buffer; /* キルバッファー大きさは、maxchgとなっている*/
extern int kill_buffer_offset;/* キルバッファー使われている長さ*/
extern w_char *remember_buf; /* リメンバァバッファ大きさは、maxchgとなっている*/

