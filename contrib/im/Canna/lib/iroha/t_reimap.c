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

static char t_reimap_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/t_reimap.c,v 2.11 91/09/02 14:04:23 kon Exp $";

extern DoFuncSequence(),UseOtherKeymap();

#define NONE IROHA_FN_Undefined

static int (*tourokureibun_funcs[])() = {
/* 未定義                           */    NONE,
/* 自分自身を挿入する関数           */    NONE,
/* 修飾しながら挿入する関数         */    YesNo,
/* 引用挿入			    */    NONE,
/* 日本語入力モード                 */    NONE,
/* アルファベット入力モード         */    NONE,
/* 変換入力モード                   */    NONE,
/* 全角ひらがな確定入力モード       */    NONE,
/* 全角カタカナ確定入力モード       */    NONE,
/* 半角カタカナ確定入力モード       */    NONE,
/* 全角アルファベット確定入力モード */    NONE,
/* 半角アルファベット確定入力モード */    NONE,
/* １６進入力モード		    */    NONE,
/* 部首入力モード		    */    NONE,
/* 記号入力モード		    */    NONE,
/* 右へ                             */    NONE,
/* 左へ                             */    NONE,
/* 次の行                           */    NONE,
/* 前の行                           */    NONE,
/* 行頭                             */    NONE,
/* 行末                             */    NONE,
/* 削除                             */    NONE,
/* 削除                             */    NONE,
/* 行末まで削除                     */    NONE,
/* 変換                             */    NONE,
/* 確定                             */    NONE,
/* 伸ばし                           */    NONE,
/* 縮め                             */    NONE,
/* 文節伸縮モードへ                 */    NONE, 		   
/* 取りやめ                         */    YesNoQuit,
/* 登録                             */    NONE,
/* １６進コードとして変換           */    NONE,
/* 部首名として変換                 */    NONE,
/* 候補一覧                         */    NONE,
/* 部分無変換                       */    NONE,
/* 全角への変換                     */    NONE,
/* 半角への変換                     */    NONE,
/* 大文字への変換                   */    NONE,
/* キャピタライズ                   */    NONE,
/* 小文字への変換                   */    NONE,
/* ひらがな変換			    */	  NONE,
/* カタカナ変換			    */	  NONE,
/* ローマ字変換			    */	  NONE,
/* 複数のキーの割り当て             */    DoFuncSequence,
/* キーシーケンスへの対応           */    UseOtherKeymap,
};

KanjiModeRec tourokureibun_mode = {
  simplefunc,
  default_kmap,
  IROHA_KANJIMODE_TABLE_SHARED,
  tourokureibun_funcs,
};
