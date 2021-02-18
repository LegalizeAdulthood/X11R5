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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/hex.c,v 3.1 91/06/28 18:10:54 kon Exp $";
     
#include "iroha.h"

#define USUAL     0
#define IMMEDIATE 1

#define HEXPROMPT "コード: "
#define HEXPROMPTLEN  8 /* "コード: " の長さは８バイト */

static int quitHex();

int hexCharacterDefinigStyle = USUAL;

/* cfuncdef

  hexEveryTimeCatch -- 読みを１６進入力モードで表示する関数

 */

static
hexEveryTimeCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  yomiContext yc = (yomiContext)d->modec;
  static unsigned char buf[256] = HEXPROMPT;
  /* ??? このようなバッファをいろいろな部分で持つのは好ましくないので、
     uiContext にまとめて持って共有して使った方が良い */

  int codelen = d->kanji_status_return->length;
  if (codelen >= 0) {
    strncpy(buf + HEXPROMPTLEN, d->kanji_status_return->echoStr, codelen);
    d->kanji_status_return->gline.line = buf;
    d->kanji_status_return->gline.length = codelen + HEXPROMPTLEN;
    d->kanji_status_return->gline.revPos =
      d->kanji_status_return->revPos + HEXPROMPTLEN;
    d->kanji_status_return->gline.revLen = d->kanji_status_return->revLen;
    d->kanji_status_return->info |= KanjiGLineInfo;
    echostrClear(d);
    if (codelen == 4) { /* ４文字になったときには.... */
      if (convertAsHex(d)) {
	yc->allowedChars = IROHA_NOTHING_ALLOWED;
	strncpy(yc->kana_buffer + yc->kEndp + 1, d->buffer_return, 2);
	d->kanji_status_return->echoStr = 
	  yc->kana_buffer + yc->kEndp + 1;
	d->kanji_status_return->revPos = d->kanji_status_return->revLen = 0;
	d->kanji_status_return->length = 2;
	retval = 0;
	if (hexCharacterDefinigStyle != USUAL) {
	  d->more.todo = 1;
	  d->more.ch = d->ch;
	  d->more.fnum = IROHA_FN_Kakutei;
	}
      }
      else {
	extern int (*jrBeepFunc)();

	if (jrBeepFunc) {
	  jrBeepFunc();
	}
	d->more.todo = 1;
	d->more.ch = d->ch;
	d->more.fnum = IROHA_FN_DeletePrevious;
      }
    }
    else {
      yc->allowedChars = IROHA_ONLY_HEX;
    }
  }
  checkGLineLen(d);
  return retval;
}

static
exitHex(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  unsigned char buf[4]; /* ４バイトも要らない。２バイトで十分 */

  if (cvtAsHex(d, buf, d->buffer_return, d->nbytes)) {
    strncpy(d->buffer_return, buf, 2);
    GlineClear(d);
    popCallback(d);
    retval = YomiExit(d, 2);
    currentModeInfo(d);
    return retval;
  }
  else {
    return quitHex(d, 0, env);
  }
}

static
quitHex(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  GlineClear(d);
  popCallback(d);
  currentModeInfo(d);
  return 0;
}

yomiContext GetKanjiString();

/* cfuncdef

  HexMode -- １６進入力モードになるときに呼ばれる。

 */

HexMode(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  if (yc->generalFlags & IROHA_YOMI_CHGMODE_INHIBITTED) {
    return NothingChangedWithBeep(d);
  }    

  return hexMode(d, 1, quitHex);
}

hexMode(d, flag, quitfunc)
uiContext d;
int flag;
int (*quitfunc)();
{
  yomiContext yc;

  yc = GetKanjiString(d, (unsigned char *)NULL, 0,
		      IROHA_ONLY_HEX,
		      IROHA_YOMI_CHGMODE_INHIBITTED,
		      IROHA_YOMI_END_IF_KAKUTEI,
		      IROHA_YOMI_INHIBIT_ALL,
		      JISHU_HAN_KATA,
		      0, /* 通常の変換をする */
		      hexEveryTimeCatch, exitHex, quitfunc);
  if (yc == (yomiContext)0) {
    return NoMoreMemory();
  }
  if(flag)
    yc->majorMode = yc->minorMode = IROHA_MODE_HexMode;
  else {
    yc->majorMode = IROHA_MODE_ExtendMode;
    yc->minorMode = IROHA_MODE_HexMode;
  }
  currentModeInfo(d);
  return 0;
}
