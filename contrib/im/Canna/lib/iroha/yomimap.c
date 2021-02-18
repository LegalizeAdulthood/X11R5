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

static char yomimap_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/yomimap.c,v 3.1 91/09/02 14:04:54 kon Exp $";

extern YomiNextJishu(), YomiPreviousJishu(), ConvertAsHex(), ConvertAsBushu();
extern YomiZenkaku(), YomiHankaku();
extern DoFuncSequence(),UseOtherKeymap();
extern YomiHiraganaJishu(), YomiKatakanaJishu(), YomiRomajiJishu();

static
yomiquotedfunc(d, mode, key, fnum)
uiContext d;
KanjiMode mode;
unsigned char key;
int fnum;
{
  return YomiInsertQuoted(d);
}

KanjiModeRec yomi_quoted_insert_mode = {
  yomiquotedfunc,
  0, 0, 0,
};

#define NONE IROHA_FN_Undefined

static int (*yomi_funcs[])() = {
/* 未定義                           */    NONE,
/* 自分自身を挿入する関数           */    NONE,
/* 修飾しながら挿入する関数         */    YomiInsert,
/* 引用挿入			    */    YomiQuotedInsert,
/* 日本語入力モード                 */    NONE,
/* アルファベット入力モード         */    NONE,
/* 変換入力モード                   */    NONE,
/* 全角ひらがな確定入力モード       */    YomiHiraganaMode,
/* 全角カタカナ確定入力モード       */    YomiZenkakuKatakanaMode,
/* 半角カタカナ確定入力モード       */    YomiHankakuKatakanaMode,
/* 全角アルファベット確定入力モード */    YomiZenkakuRomajiMode,
/* 半角アルファベット確定入力モード */    YomiHankakuRomajiMode,
/* １６進入力モード		    */    NONE,
/* 部首入力モード		    */    NONE,
/* 記号入力モード		    */    NONE,
/* 右へ                             */    YomiForward,
/* 左へ                             */    YomiBackward,
/* 次の行                           */    YomiNextJishu,
/* 前の行                           */    YomiPreviousJishu,
/* 行頭                             */    YomiBeginningOfLine,
/* 行末                             */    YomiEndOfLine,
/* 後ろを削除                       */    YomiDeleteNext,
/* 前に削除                         */    YomiDeletePrevious,
/* 行末まで削除                     */    YomiKillToEndOfLine,
#ifdef INTELLIGENTSPACE
/* 変換                             */    YomiHenkanOrSpace,
#else /* !INTELLIGENTSPACE */
/* 変換                             */    YomiHenkan,
#endif /* !INTELLIGENTSPACE */
/* 確定                             */    YomiKakutei,
/* 伸ばし                           */    YomiPrevMode,
/* 縮め                             */    YomiNextMode,
/* 文節伸縮モードへ                 */    NONE, 		   
/* 取りやめ                         */    YomiQuit,
/* 登録                             */    NONE,
/* １６進コードとして変換           */    ConvertAsHex,
/* 部首名として変換                 */    ConvertAsBushu,
/* 候補一覧                         */    ConvertAsBushu,
/* 部分無変換                       */    NONE,
/* 全角への変換                     */    YomiZenkaku,
/* 半角への変換                     */    YomiHankaku,
/* 大文字への変換                   */    NONE,
/* キャピタライズ                   */    NONE,
/* 小文字への変換                   */    NONE,
/* ひらがな変換			    */	  YomiHiraganaJishu,
/* カタカナ変換			    */	  YomiKatakanaJishu,
/* ローマ字変換			    */	  YomiRomajiJishu,
/* 複数のキーの割り当て             */    DoFuncSequence,
/* キーシーケンスへの対応           */    UseOtherKeymap,
};

KanjiModeRec yomi_mode = {
  simplefunc,
  default_kmap,
  IROHA_KANJIMODE_TABLE_SHARED,
  yomi_funcs,
};

