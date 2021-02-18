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

static char tanmap_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/tanmap.c,v 3.2 91/09/03 08:55:05 kon Exp $";

extern TanKouhoIchiran(), DoFuncSequence(),UseOtherKeymap();

#define NONE IROHA_FN_Undefined

static int (*tan_funcs[])() = {
/* 未定義                           */    NONE,
/* 自分自身を挿入する関数           */    NONE,
/* 修飾しながら挿入する関数         */    TanKakuteiYomiInsert,
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
/* 右へ                             */    TanForwardBunsetsu,
/* 左へ                             */    TanBackwardBunsetsu,
/* 次の行                           */    TanNextKouho,
/* 前の行                           */    TanPreviousKouho,
/* 行頭                             */    TanBeginningOfBunsetsu,
/* 行末                             */    TanEndOfBunsetsu,
/* 後ろを削除                       */    NONE,
/* 前に削除                         */    TanMuhenkan,
/* 行末まで削除                     */    TanBubunKakutei,
/* 変換                             */    TanHenkan,
/* 確定                             */    TanKakutei,
/* 伸ばし                           */    TanExtendBunsetsu,
/* 縮め                             */    TanShrinkBunsetsu,
/* 文節伸縮モードへ                 */    TanBunsetsuMode,
/* 取りやめ                         */    TanMuhenkan,
/* 登録                             */    NONE,
#ifdef BUNPOU_DISPLAY
/* １６進コードとして変換           */    TanPrintBunpou,
#else /* !BUNPOU_DISPLAY */
/* １６進コードとして変換           */    NONE,
#endif /* !BUNPOU_DISPLAY */
/* 部首名として変換                 */    NONE,
/* 候補一覧                         */    TanKouhoIchiran,
/* 部分無変換                       */    TanBubunMuhenkan,
/* 全角への変換                     */    TanZenkaku,
/* 半角への変換                     */    TanHankaku,
/* 大文字への変換                   */    NONE,
/* キャピタライズ                   */    NONE,
#ifdef MEASURE_TIME
/* 小文字への変換                   */    TanPrintTime,
#else /* !MEASURE_TIME */
/* 小文字への変換                   */    NONE,
#endif /* !MEASURE_TIME */
/* ひらがな変換			    */	  TanHiragana,
/* カタカナ変換			    */	  TanKatakana,
/* ローマ字変換			    */	  TanRomaji,
/* 複数のキーの割り当て             */    DoFuncSequence,
/* キーシーケンスへの対応           */    UseOtherKeymap,
};

KanjiModeRec tankouho_mode = {
  simplefunc,
  default_kmap,
  IROHA_KANJIMODE_TABLE_SHARED,
  tan_funcs,
};
