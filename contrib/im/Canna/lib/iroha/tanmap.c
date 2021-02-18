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
/* ̤���                           */    NONE,
/* ��ʬ���Ȥ���������ؿ�           */    NONE,
/* �������ʤ�����������ؿ�         */    TanKakuteiYomiInsert,
/* ��������			    */    NONE,
/* ���ܸ����ϥ⡼��                 */    NONE,
/* ����ե��٥å����ϥ⡼��         */    NONE,
/* �Ѵ����ϥ⡼��                   */    NONE,
/* ���ѤҤ餬�ʳ������ϥ⡼��       */    NONE,
/* ���ѥ������ʳ������ϥ⡼��       */    NONE,
/* Ⱦ�ѥ������ʳ������ϥ⡼��       */    NONE,
/* ���ѥ���ե��٥åȳ������ϥ⡼�� */    NONE,
/* Ⱦ�ѥ���ե��٥åȳ������ϥ⡼�� */    NONE,
/* ���������ϥ⡼��		    */    NONE,
/* �������ϥ⡼��		    */    NONE,
/* �������ϥ⡼��		    */    NONE,
/* ����                             */    TanForwardBunsetsu,
/* ����                             */    TanBackwardBunsetsu,
/* ���ι�                           */    TanNextKouho,
/* ���ι�                           */    TanPreviousKouho,
/* ��Ƭ                             */    TanBeginningOfBunsetsu,
/* ����                             */    TanEndOfBunsetsu,
/* ������                       */    NONE,
/* ���˺��                         */    TanMuhenkan,
/* �����ޤǺ��                     */    TanBubunKakutei,
/* �Ѵ�                             */    TanHenkan,
/* ����                             */    TanKakutei,
/* ���Ф�                           */    TanExtendBunsetsu,
/* �̤�                             */    TanShrinkBunsetsu,
/* ʸ�῭�̥⡼�ɤ�                 */    TanBunsetsuMode,
/* �����                         */    TanMuhenkan,
/* ��Ͽ                             */    NONE,
#ifdef BUNPOU_DISPLAY
/* �����ʥ����ɤȤ����Ѵ�           */    TanPrintBunpou,
#else /* !BUNPOU_DISPLAY */
/* �����ʥ����ɤȤ����Ѵ�           */    NONE,
#endif /* !BUNPOU_DISPLAY */
/* ����̾�Ȥ����Ѵ�                 */    NONE,
/* �������                         */    TanKouhoIchiran,
/* ��ʬ̵�Ѵ�                       */    TanBubunMuhenkan,
/* ���Ѥؤ��Ѵ�                     */    TanZenkaku,
/* Ⱦ�Ѥؤ��Ѵ�                     */    TanHankaku,
/* ��ʸ���ؤ��Ѵ�                   */    NONE,
/* ����ԥ��饤��                   */    NONE,
#ifdef MEASURE_TIME
/* ��ʸ���ؤ��Ѵ�                   */    TanPrintTime,
#else /* !MEASURE_TIME */
/* ��ʸ���ؤ��Ѵ�                   */    NONE,
#endif /* !MEASURE_TIME */
/* �Ҥ餬���Ѵ�			    */	  TanHiragana,
/* ���������Ѵ�			    */	  TanKatakana,
/* ���޻��Ѵ�			    */	  TanRomaji,
/* ʣ���Υ����γ������             */    DoFuncSequence,
/* �����������󥹤ؤ��б�           */    UseOtherKeymap,
};

KanjiModeRec tankouho_mode = {
  simplefunc,
  default_kmap,
  IROHA_KANJIMODE_TABLE_SHARED,
  tan_funcs,
};
