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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/empty.c,v 3.1 91/07/22 16:39:57 kon Exp $";

#include "iroha.h"

extern int simplefunc();

/* EmptySelfInsert -- ��ʬ���Ȥ����ʸ����Ȥ����֤��ؿ���
 * 
 */

EmptySelfInsert(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  d->kanji_status_return->info |= KanjiThroughInfo | KanjiEmptyInfo;
  if (yc->generalFlags & IROHA_YOMI_END_IF_KAKUTEI) {
    /* ����ǡ����������ԤäƤ���ͤˤϳ���ǡ������Ϥ��ʤ� */
    return 0;
  }
  else {
    return d->nbytes;
  }
}

/* EmptyYomiInsert -- ���⡼�ɤ˰ܹԤ����ɤߤ����Ϥ���ؿ�
 *
 */

EmptyYomiInsert(d)
uiContext d;
{
  extern KanjiModeRec yomi_mode;

  d->current_mode = &yomi_mode;
  RomajiClearYomi(d);
  return YomiInsert(d); /* ������Хå��Υ����å��� YomiInsert �Ǥ���� */
}

/* EmptyQuotedInsert -- ���ΰ�����ɤΤ褦��ʸ���Ǥ⥹�롼���̤��ؿ���
 *
 */

/* 
  Empty �⡼�ɤǤ� quotedInset �� ^Q �Τ褦��ʸ������� Emacs �ʤɤ���
  ���̤äƤ��ޤ��Хޥåפ��֤��Ƥ��ޤ��Τǡ����ʴ����Ѵ������ǲ�����
  ����ʤ�Ƥ��Ȥ�ɬ�פʤ��ΤǤϤʤ��Τ��ʤ���
 */

static
EmptyQuotedInsert(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec, GetKanjiString();
  extern KanjiModeRec yomi_mode;

  if (yc->generalFlags & IROHA_YOMI_CHGMODE_INHIBITTED) {
    return NothingChangedWithBeep(d);
  }    

  d->current_mode = &yomi_mode;
  return YomiQuotedInsert(d);
}

/* 
  AlphaSelfInsert -- ��ʬ���Ȥ����ʸ����Ȥ����֤��ؿ���
 */

AlphaSelfInsert(d)
uiContext d;
{
  unsigned char kanap = (unsigned char)*d->buffer_return;

  d->kanji_status_return->length = 0;
  if ( d->nbytes != 1 || kanap < 0xa0 || 0xdf < kanap ) {
    d->kanji_status_return->info |= KanjiThroughInfo | KanjiEmptyInfo;
    return d->nbytes;
  }
  else { /* ��̾�������Ϥξ�� */
    if (d->bytes_buffer > 1) {
      d->buffer_return[1] = d->buffer_return[0];
      d->buffer_return[0] = (unsigned char)0x8e; /* SS2 */
      return 2;
    }
    else {
      return 0;
    }
  }
}

static
EmptyQuit(d)
uiContext d;
{
  int res;

  res = EmptySelfInsert(d);
  d->status = QUIT_CALLBACK;
  if (d->cb->func[QUIT_CALLBACK] != NO_CALLBACK) {
    popYomiMode(d);
  }
  return res;
}

static
EmptyKakutei(d)
uiContext d;
{
  int res;

  res = EmptySelfInsert(d);
  d->status = EXIT_CALLBACK;
  if (d->cb->func[EXIT_CALLBACK] != NO_CALLBACK) {
    popYomiMode(d);
  }
  return res;
}

static
EmptyDeletePrevious(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  if (yc->generalFlags & IROHA_YOMI_DELETE_DONT_QUIT) {
    /* Delete �� QUIT ���ʤ��ΤǤ���С�selfInsert */
    return EmptySelfInsert(d);
  }
  else {
    return EmptyQuit(d);
  }
}

#include "emptymap.c"
