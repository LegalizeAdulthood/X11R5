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

static char bunmap_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/bunmap.c,v 1.2 91/09/03 18:26:08 kon Exp $";

extern DoFuncSequence(),UseOtherKeymap();

#define NONE IROHA_FN_Undefined

static int (*bun_funcs[])() = {
/* ̤���                           */    NONE,
/* ��ʬ���Ȥ���������ؿ�           */    NONE,
/* �������ʤ�����������ؿ�         */    BunSelfInsert,
/* ��������			    */    BunQuotedInsert,
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
/* ����                             */    BunExtend,
/* ����                             */    BunShrink,
/* ���ι�                           */    NONE,
/* ���ι�                           */    NONE,
/* ��Ƭ                             */    BunFullShrink,
/* ����                             */    BunFullExtend,
/* ������                       */    BunQuit,
/* ���˺��                         */    BunQuit,
/* �����ޤǺ��                     */    BunKillToEOL,
/* �Ѵ�                             */    BunHenkan,
/* ����                             */    BunKakutei,
/* ���Ф�                           */    BunExtend,
/* �̤�                             */    BunShrink,
/* ʸ�῭�̥⡼�ɤ�                 */    NONE, 		   
/* �����                         */    BunQuit,
/* ��Ͽ                             */    NONE,
/* �����ʥ����ɤȤ����Ѵ�           */    NONE,
/* ����̾�Ȥ����Ѵ�                 */    NONE,
/* �������                         */    NONE,
/* ��ʬ̵�Ѵ�                       */    NONE,
/* ���Ѥؤ��Ѵ�                     */    NONE,
/* Ⱦ�Ѥؤ��Ѵ�                     */    NONE,
/* ��ʸ���ؤ��Ѵ�                   */    NONE,
/* ����ԥ��饤��                   */    NONE,
/* ��ʸ���ؤ��Ѵ�                   */    NONE,
/* �Ҥ餬���Ѵ�			    */	  NONE,
/* ���������Ѵ�			    */	  NONE,
/* ���޻��Ѵ�			    */	  NONE,
/* ʣ���Υ����γ������             */    DoFuncSequence,
/* �����������󥹤ؤ��б�           */    UseOtherKeymap,
};

KanjiModeRec bunsetsu_mode = {
  simplefunc,
  default_kmap,
  IROHA_KANJIMODE_TABLE_SHARED,
  bun_funcs,
};

