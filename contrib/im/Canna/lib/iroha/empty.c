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

/* EmptySelfInsert -- 自分自身を確定文字列として返す関数。
 * 
 */

EmptySelfInsert(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  d->kanji_status_return->info |= KanjiThroughInfo | KanjiEmptyInfo;
  if (yc->generalFlags & IROHA_YOMI_END_IF_KAKUTEI) {
    /* 確定データだけを待っている人には確定データを渡さない */
    return 0;
  }
  else {
    return d->nbytes;
  }
}

/* EmptyYomiInsert -- ○モードに移行し、読みを入力する関数
 *
 */

EmptyYomiInsert(d)
uiContext d;
{
  extern KanjiModeRec yomi_mode;

  d->current_mode = &yomi_mode;
  RomajiClearYomi(d);
  return YomiInsert(d); /* コールバックのチェックは YomiInsert でされる */
}

/* EmptyQuotedInsert -- 次の一字がどのような文字でもスルーで通す関数。
 *
 */

/* 
  Empty モードでの quotedInset は ^Q のような文字が一回 Emacs などの方
  に通ってしまえばマップが返られてしまうので、カナ漢字変換の方で何かを
  するなんてことは必要ないのではないのかなぁ。
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
  AlphaSelfInsert -- 自分自身を確定文字列として返す関数。
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
  else { /* 仮名キー入力の場合 */
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
    /* Delete で QUIT しないのであれば、selfInsert */
    return EmptySelfInsert(d);
  }
  else {
    return EmptyQuit(d);
  }
}

#include "emptymap.c"
