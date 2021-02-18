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
/* ̤���                           */    NONE,
/* ��ʬ���Ȥ���������ؿ�           */    NONE,
/* �������ʤ�����������ؿ�         */    YomiInsert,
/* ��������			    */    YomiQuotedInsert,
/* ���ܸ����ϥ⡼��                 */    NONE,
/* ����ե��٥å����ϥ⡼��         */    NONE,
/* �Ѵ����ϥ⡼��                   */    NONE,
/* ���ѤҤ餬�ʳ������ϥ⡼��       */    YomiHiraganaMode,
/* ���ѥ������ʳ������ϥ⡼��       */    YomiZenkakuKatakanaMode,
/* Ⱦ�ѥ������ʳ������ϥ⡼��       */    YomiHankakuKatakanaMode,
/* ���ѥ���ե��٥åȳ������ϥ⡼�� */    YomiZenkakuRomajiMode,
/* Ⱦ�ѥ���ե��٥åȳ������ϥ⡼�� */    YomiHankakuRomajiMode,
/* ���������ϥ⡼��		    */    NONE,
/* �������ϥ⡼��		    */    NONE,
/* �������ϥ⡼��		    */    NONE,
/* ����                             */    YomiForward,
/* ����                             */    YomiBackward,
/* ���ι�                           */    YomiNextJishu,
/* ���ι�                           */    YomiPreviousJishu,
/* ��Ƭ                             */    YomiBeginningOfLine,
/* ����                             */    YomiEndOfLine,
/* ������                       */    YomiDeleteNext,
/* ���˺��                         */    YomiDeletePrevious,
/* �����ޤǺ��                     */    YomiKillToEndOfLine,
#ifdef INTELLIGENTSPACE
/* �Ѵ�                             */    YomiHenkanOrSpace,
#else /* !INTELLIGENTSPACE */
/* �Ѵ�                             */    YomiHenkan,
#endif /* !INTELLIGENTSPACE */
/* ����                             */    YomiKakutei,
/* ���Ф�                           */    YomiPrevMode,
/* �̤�                             */    YomiNextMode,
/* ʸ�῭�̥⡼�ɤ�                 */    NONE, 		   
/* �����                         */    YomiQuit,
/* ��Ͽ                             */    NONE,
/* �����ʥ����ɤȤ����Ѵ�           */    ConvertAsHex,
/* ����̾�Ȥ����Ѵ�                 */    ConvertAsBushu,
/* �������                         */    ConvertAsBushu,
/* ��ʬ̵�Ѵ�                       */    NONE,
/* ���Ѥؤ��Ѵ�                     */    YomiZenkaku,
/* Ⱦ�Ѥؤ��Ѵ�                     */    YomiHankaku,
/* ��ʸ���ؤ��Ѵ�                   */    NONE,
/* ����ԥ��饤��                   */    NONE,
/* ��ʸ���ؤ��Ѵ�                   */    NONE,
/* �Ҥ餬���Ѵ�			    */	  YomiHiraganaJishu,
/* ���������Ѵ�			    */	  YomiKatakanaJishu,
/* ���޻��Ѵ�			    */	  YomiRomajiJishu,
/* ʣ���Υ����γ������             */    DoFuncSequence,
/* �����������󥹤ؤ��б�           */    UseOtherKeymap,
};

KanjiModeRec yomi_mode = {
  simplefunc,
  default_kmap,
  IROHA_KANJIMODE_TABLE_SHARED,
  yomi_funcs,
};

