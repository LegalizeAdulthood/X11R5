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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/bunsetsu.c,v 1.2 91/09/03 18:44:17 kon Exp $";

#include <errno.h>
#include "iroha.h"

extern int BunsetsuKugiri;

static void
makeAdjustReturnStruct(d, ac, obunlen)
uiContext d;
adjustContext ac;
{
  if (BunsetsuKugiri) {
    d->kanji_status_return->revPos = ac->kanjilen + ac->bunlen;
    d->kanji_status_return->revLen =
      (ac->genbuf[ac->kanjilen + ac->bunlen] & 0x80) ? 2 : 1;
    if (ac->kanjilen + ac->bunlen == ac->strlen) {
      d->kanji_status_return->revLen = 0;
    }
    d->kanji_status_return->echoStr = ac->genbuf;
    d->kanji_status_return->length = ac->strlen;
  }
  else {
    d->kanji_status_return->revPos = ac->kanjilen;
    d->kanji_status_return->revLen = ac->bunlen;
    d->kanji_status_return->echoStr = ac->genbuf;
    d->kanji_status_return->length = ac->strlen;
  }
}

static int
adjustError()
{
  necKanjiError = "文節伸ばし縮め操作に失敗しました。";
  return -1;
}

void
freeAdjustContext(ac)
adjustContext ac;
{
  free(ac);
}

static void
popAdjustMode(d)
uiContext d;
{
  adjustContext ac = (adjustContext)d->modec;

  d->modec = ac->next;
  d->current_mode = ac->prevMode;
  /* callback は callback 自身が pop する */
  freeAdjustContext(ac);
}

adjustContext
pushAdjustMode(d, kanji, kanjilen, yomi, yomilen, bunlen,
	       hc, yc, ev, ex, qu, au)
uiContext d;
unsigned char *kanji, *yomi;
int kanjilen, yomilen, bunlen;
henkanContext hc;
yomiContext yc;
int (*ev)(), (*ex)(), (*qu)(), (*au)();
{
  adjustContext ac;
  struct callback *pushCallback();
  extern KanjiModeRec bunsetsu_mode;

  ac = (adjustContext)malloc(sizeof(adjustContextRec));
  if (ac == 0) {
    (void)adjustError();
    return 0;
  }
  if (pushCallback(d, d->modec, ev, ex, qu, au) == (struct callback *)0) {
    freeAdjustContext(ac);
    (void)adjustError();
    return 0;
  }
  ac->id = ADJUST_CONTEXT;
  ac->majorMode = d->majorMode;
  ac->next = d->modec;
  d->modec = (mode_context)ac;
  ac->prevMode = d->current_mode;
  d->current_mode = &bunsetsu_mode;
  strncpy(ac->genbuf, kanji, kanjilen);
  strncpy(ac->genbuf + kanjilen, yomi, yomilen);
  ac->genbuf[kanjilen + yomilen] = '\0';
  ac->kanjilen = kanjilen;
  ac->strlen = kanjilen + yomilen;
  ac->bunlen = bunlen;
  ac->hcx = hc;
  ac->ycx = yc;

  makeAdjustReturnStruct(d, ac);

  return ac;
}

static
BunFullExtend(d)
uiContext d;
{
  adjustContext ac = (adjustContext)d->modec;

  ac->bunlen = ac->strlen - ac->kanjilen;
  makeAdjustReturnStruct(d, ac);
  return 0;
}

static
BunFullShrink(d)
uiContext d;
{
  adjustContext ac = (adjustContext)d->modec;
  int newlen;

  ac->bunlen = (ac->genbuf[ac->kanjilen] & 0x80) ? 2 : 1;
  makeAdjustReturnStruct(d, ac);
  return 0;
}

extern CursorWrap; /* 右端に行って右に行こうとしたとき左端に行くかどうか */

static
BunExtend(d)
uiContext d;
{
  adjustContext ac = (adjustContext)d->modec;

  if (ac->kanjilen + ac->bunlen < ac->strlen) {
    /* まだ伸ばせる */

    ac->bunlen += (ac->genbuf[ac->kanjilen + ac->bunlen] & 0x80) ? 2 : 1;
    makeAdjustReturnStruct(d, ac);
    return 0;
  }
  else if(CursorWrap) {
    return BunFullShrink(d);
  }
  (void)NothingChangedWithBeep(d);
  return 0;
}

static
BunShrink(d)
uiContext d;
{
  adjustContext ac = (adjustContext)d->modec;

  if (ac->bunlen > 0) {
    /* まだ縮まる */
    int newlen = ac->bunlen;

    newlen -= (ac->genbuf[ac->kanjilen + ac->bunlen - 1] & 0x80) ? 2 : 1;
    if (newlen > 0) {
      ac->bunlen = newlen;
      makeAdjustReturnStruct(d, ac);
      return 0;
    }
    else if(CursorWrap) {
      return BunFullExtend(d);
    }
  }
  (void)NothingChangedWithBeep(d);
  return 0;
}

static
BunHenkan(d)
uiContext d;
{
  adjustContext ac = (adjustContext)d->modec;
  henkanContext hc = ac->hcx;

  if ((hc->nbunsetsu = RkResize(hc->context, ac->bunlen)) == -1) {
    extern errno;

    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "かな漢字変換に失敗しました";
    return -1;
  }

  d->status = EXIT_CALLBACK;
  popAdjustMode(d);
  return 0;
}

static
BunQuit(d)
uiContext d;
{
  adjustContext ac = (adjustContext)d->modec;
  int bunlen = ac->bunlen;

  d->status = QUIT_CALLBACK;
  popAdjustMode(d);
  return bunlen;
}

static
BunSelfInsert(d)
uiContext d;
{
  d->nbytes = BunQuit(d);
  d->more.todo = 1;
  d->more.ch = d->ch;
  d->more.fnum = IROHA_FN_FunctionalInsert;
  return d->nbytes;
}

static
BunQuotedInsert(d)
uiContext d;
{
  d->nbytes = BunQuit(d);
  d->more.todo = 1;
  d->more.ch = d->ch;
  d->more.fnum = IROHA_FN_QuotedInsert;
  return d->nbytes;
}

static
BunKakutei(d)
uiContext d;
{
  adjustContext ac = (adjustContext)d->modec;

  if (BunsetsuKugiri) {
    d->nbytes = ujisncpy(d->buffer_return, ac->genbuf, ac->kanjilen - 1);
    d->nbytes += ujisncpy(d->buffer_return + d->nbytes,
			  ac->genbuf + ac->kanjilen,
			  ac->strlen - ac->kanjilen);
  }
  else {
    d->nbytes = ujisncpy(d->buffer_return, ac->genbuf, ac->strlen);
  }
  d->status = EXIT_CALLBACK;
  popAdjustMode(d);
  return d->nbytes;
}

static
BunKillToEOL(d)
uiContext d;
{
  d->nbytes = BunQuit(d);
  d->more.todo = 1;
  d->more.ch = d->ch;
  d->more.fnum = IROHA_FN_KillToEndOfLine;
  return d->nbytes;
}

#include "bunmap.c"
