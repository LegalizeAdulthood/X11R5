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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/defaultmap.c,v 2.24 91/05/21 15:56:43 satoko Exp $";

#include "iroha.h"
#include <iroha/mfdef.h>

int howToBehaveInCaseOfUndefKey = kc_normal;

#define DEFAULTBEHAVIOR 0

simplefunc(d, mode, key, fnum)
uiContext d;
KanjiMode mode;
unsigned char key;
int fnum;
{
  int (*func)();
  
  if (fnum > 0) {
    func = mode->ftbl[fnum];
  }
  else {
    func = mode->ftbl[mode->keytbl[key]];
  }
  if (func || (func = mode->ftbl[DEFAULTBEHAVIOR])) {
    return func(d);
  }
  else {
    return simpleUndefBehavior(d);
  }
}

int (*jrBeepFunc)() = NULL;

simpleUndefBehavior(d)
uiContext d;
{
  switch (howToBehaveInCaseOfUndefKey)
    {
    case kc_through:
      d->nbytes = 0;
      if (d->bytes_buffer > 1) {
	if (d->ch & 0x80) {
	  d->buffer_return[d->nbytes++] = 0x8e;
	}
	d->buffer_return[d->nbytes++] = d->ch;
      }
      else {
	d->kanji_status_return->info |= KanjiThroughInfo;
      }
      d->kanji_status_return->length = -1;
      return d->nbytes;
    case kc_kakutei:
      d->nbytes = escapeToBasicStat(d, IROHA_FN_Kakutei);
      if (d->bytes_buffer > d->nbytes) {
	d->kanji_status_return->info |= KanjiThroughInfo;
	d->buffer_return[d->nbytes] = d->ch;
	d->nbytes++;
      }
      return d->nbytes;
    case kc_kill:
      d->nbytes = escapeToBasicStat(d, IROHA_FN_Quit);
      if (d->bytes_buffer > d->nbytes) {
	d->kanji_status_return->info |= KanjiThroughInfo;
	d->buffer_return[d->nbytes] = d->ch;
	d->nbytes++;
      }
      return d->nbytes;
    case kc_normal:
    default:
      if (jrBeepFunc) {
	jrBeepFunc();
      }
      d->kanji_status_return->length = -1;
      return 0;
    }
}

#define NONE IROHA_FN_Undefined

unsigned char default_kmap[256] =
{               
/* C-@ */       NONE,
/* C-a */       IROHA_FN_BeginningOfLine,
/* C-b */       IROHA_FN_Backward,
/* C-c */       IROHA_FN_BubunMuhenkan,
/* C-d */       IROHA_FN_DeleteNext,
/* C-e */       IROHA_FN_EndOfLine,
/* C-f */       IROHA_FN_Forward,
/* C-g */       IROHA_FN_Quit,
/* C-h */       IROHA_FN_DeletePrevious,
/* C-i */       IROHA_FN_Shrink,
/* C-j */       IROHA_FN_Kakutei,
/* C-k */       IROHA_FN_KillToEndOfLine,
/* C-l */       IROHA_FN_ToLower,
/* C-m */       IROHA_FN_Kakutei,
/* C-n */       IROHA_FN_Next,
/* C-o */       IROHA_FN_Extend,
/* C-p */       IROHA_FN_Prev,
/* C-q */       IROHA_FN_QuotedInsert,
/* C-r */       NONE,
/* C-s */       NONE,
/* C-t */       NONE,
/* C-u */       IROHA_FN_ToUpper,
/* C-v */       NONE,
/* C-w */       IROHA_FN_KouhoIchiran,
/* C-x */       NONE,
/* C-y */       IROHA_FN_ConvertAsHex,
/* C-z */       NONE,
/* C-[ */       NONE,
/* C-\ */       NONE,
/* C-] */       NONE,
/* C-^ */       NONE,
/* C-_ */       NONE,
/* space */     IROHA_FN_Henkan,
/* ! */         IROHA_FN_FunctionalInsert,
/* " */         IROHA_FN_FunctionalInsert,
/* # */         IROHA_FN_FunctionalInsert,
/* $ */         IROHA_FN_FunctionalInsert,
/* % */         IROHA_FN_FunctionalInsert,
/* & */         IROHA_FN_FunctionalInsert,
/* ' */         IROHA_FN_FunctionalInsert,
/* ( */         IROHA_FN_FunctionalInsert,
/* ) */         IROHA_FN_FunctionalInsert,
/* * */         IROHA_FN_FunctionalInsert,
/* + */         IROHA_FN_FunctionalInsert,
/* , */         IROHA_FN_FunctionalInsert,
/* - */         IROHA_FN_FunctionalInsert,
/* . */         IROHA_FN_FunctionalInsert,
/* / */         IROHA_FN_FunctionalInsert,
/* 0 */         IROHA_FN_FunctionalInsert,
/* 1 */         IROHA_FN_FunctionalInsert,
/* 2 */         IROHA_FN_FunctionalInsert,
/* 3 */         IROHA_FN_FunctionalInsert,
/* 4 */         IROHA_FN_FunctionalInsert,
/* 5 */         IROHA_FN_FunctionalInsert,
/* 6 */         IROHA_FN_FunctionalInsert,
/* 7 */         IROHA_FN_FunctionalInsert,
/* 8 */         IROHA_FN_FunctionalInsert,
/* 9 */         IROHA_FN_FunctionalInsert,
/* : */         IROHA_FN_FunctionalInsert,
/* ; */         IROHA_FN_FunctionalInsert,
/* < */         IROHA_FN_FunctionalInsert,
/* = */         IROHA_FN_FunctionalInsert,
/* > */         IROHA_FN_FunctionalInsert,
/* ? */         IROHA_FN_FunctionalInsert,
/* @ */         IROHA_FN_FunctionalInsert,
/* A */         IROHA_FN_FunctionalInsert,
/* B */         IROHA_FN_FunctionalInsert,
/* C */         IROHA_FN_FunctionalInsert,
/* D */         IROHA_FN_FunctionalInsert,
/* E */         IROHA_FN_FunctionalInsert,
/* F */         IROHA_FN_FunctionalInsert,
/* G */         IROHA_FN_FunctionalInsert,
/* H */         IROHA_FN_FunctionalInsert,
/* I */         IROHA_FN_FunctionalInsert,
/* J */         IROHA_FN_FunctionalInsert,
/* K */         IROHA_FN_FunctionalInsert,
/* L */         IROHA_FN_FunctionalInsert,
/* M */         IROHA_FN_FunctionalInsert,
/* N */         IROHA_FN_FunctionalInsert,
/* O */         IROHA_FN_FunctionalInsert,
/* P */         IROHA_FN_FunctionalInsert,
/* Q */         IROHA_FN_FunctionalInsert,
/* R */         IROHA_FN_FunctionalInsert,
/* S */         IROHA_FN_FunctionalInsert,
/* T */         IROHA_FN_FunctionalInsert,
/* U */         IROHA_FN_FunctionalInsert,
/* V */         IROHA_FN_FunctionalInsert,
/* W */         IROHA_FN_FunctionalInsert,
/* X */         IROHA_FN_FunctionalInsert,
/* Y */         IROHA_FN_FunctionalInsert,
/* Z */         IROHA_FN_FunctionalInsert,
/* [ */         IROHA_FN_FunctionalInsert,
/* \ */         IROHA_FN_FunctionalInsert,
/* ] */         IROHA_FN_FunctionalInsert,
/* ^ */         IROHA_FN_FunctionalInsert,
/* _ */         IROHA_FN_FunctionalInsert,
/* ` */         IROHA_FN_FunctionalInsert,
/* a */         IROHA_FN_FunctionalInsert,
/* b */         IROHA_FN_FunctionalInsert,
/* c */         IROHA_FN_FunctionalInsert,
/* d */         IROHA_FN_FunctionalInsert,
/* e */         IROHA_FN_FunctionalInsert,
/* f */         IROHA_FN_FunctionalInsert,
/* g */         IROHA_FN_FunctionalInsert,
/* h */         IROHA_FN_FunctionalInsert,
/* i */         IROHA_FN_FunctionalInsert,
/* j */         IROHA_FN_FunctionalInsert,
/* k */         IROHA_FN_FunctionalInsert,
/* l */         IROHA_FN_FunctionalInsert,
/* m */         IROHA_FN_FunctionalInsert,
/* n */         IROHA_FN_FunctionalInsert,
/* o */         IROHA_FN_FunctionalInsert,
/* p */         IROHA_FN_FunctionalInsert,
/* q */         IROHA_FN_FunctionalInsert,
/* r */         IROHA_FN_FunctionalInsert,
/* s */         IROHA_FN_FunctionalInsert,
/* t */         IROHA_FN_FunctionalInsert,
/* u */         IROHA_FN_FunctionalInsert,
/* v */         IROHA_FN_FunctionalInsert,
/* w */         IROHA_FN_FunctionalInsert,
/* x */         IROHA_FN_FunctionalInsert,
/* y */         IROHA_FN_FunctionalInsert,
/* z */         IROHA_FN_FunctionalInsert,
/* { */         IROHA_FN_FunctionalInsert,
/* | */         IROHA_FN_FunctionalInsert,
/* } */         IROHA_FN_FunctionalInsert,
/* ~ */         IROHA_FN_FunctionalInsert,
/* DEL */       NONE,
/* Nfer */      IROHA_FN_Kakutei,
/* Xfer */      IROHA_FN_Henkan,
/* Up */        IROHA_FN_Prev,
/* Left */      IROHA_FN_Backward,
/* Right */     IROHA_FN_Forward,
/* Down */      IROHA_FN_Next,
/* Insert */    IROHA_FN_KigouMode,
/* Rollup */    NONE,
/* Rolldown */  NONE,
/* Home */      NONE,
/* Help */      NONE,
/* KeyPad */	NONE,
/* 8c */        NONE,
/* 8d */        NONE,
/* 8e */        NONE,
/* 8f */        NONE,
/* S-nfer */    NONE,
/* S-xfer */    NONE,
/* S-up */      NONE,
/* S-left */    IROHA_FN_Shrink,
/* S-right */   IROHA_FN_Extend,
/* S-down */    NONE,
/* C-nfer */    NONE,
/* C-xfer */    NONE,
/* C-up */      NONE,
/* C-left */    IROHA_FN_Shrink,
/* C-right */   IROHA_FN_Extend,
/* C-down */    NONE,
/* 9c */        NONE,
/* 9d */        NONE,
/* 9e */        NONE,
/* 9f */        NONE,
/* KANASPACE */ NONE,
/* 。 */        IROHA_FN_FunctionalInsert,
/* 「 */        IROHA_FN_FunctionalInsert,
/* 」 */        IROHA_FN_FunctionalInsert,
/* 、 */        IROHA_FN_FunctionalInsert,
/* ・ */        IROHA_FN_FunctionalInsert,
/* ヲ */        IROHA_FN_FunctionalInsert,
/* ァ */        IROHA_FN_FunctionalInsert,
/* ィ */        IROHA_FN_FunctionalInsert,
/* ゥ */        IROHA_FN_FunctionalInsert,
/* ェ */        IROHA_FN_FunctionalInsert,
/* ォ */        IROHA_FN_FunctionalInsert,
/* ャ */        IROHA_FN_FunctionalInsert,
/* ュ */        IROHA_FN_FunctionalInsert,
/* ョ */        IROHA_FN_FunctionalInsert,
/* ッ */        IROHA_FN_FunctionalInsert,
/* ー */        IROHA_FN_FunctionalInsert,
/* ア */        IROHA_FN_FunctionalInsert,
/* イ */        IROHA_FN_FunctionalInsert,
/* ウ */        IROHA_FN_FunctionalInsert,
/* エ */        IROHA_FN_FunctionalInsert,
/* オ */        IROHA_FN_FunctionalInsert,
/* カ */        IROHA_FN_FunctionalInsert,
/* キ */        IROHA_FN_FunctionalInsert,
/* ク */        IROHA_FN_FunctionalInsert,
/* ケ */        IROHA_FN_FunctionalInsert,
/* コ */        IROHA_FN_FunctionalInsert,
/* サ */        IROHA_FN_FunctionalInsert,
/* シ */        IROHA_FN_FunctionalInsert,
/* ス */        IROHA_FN_FunctionalInsert,
/* セ */        IROHA_FN_FunctionalInsert,
/* ソ */        IROHA_FN_FunctionalInsert,
/* タ */        IROHA_FN_FunctionalInsert,
/* チ */        IROHA_FN_FunctionalInsert,
/* ツ */        IROHA_FN_FunctionalInsert,
/* テ */        IROHA_FN_FunctionalInsert,
/* ト */        IROHA_FN_FunctionalInsert,
/* ナ */        IROHA_FN_FunctionalInsert,
/* ニ */        IROHA_FN_FunctionalInsert,
/* ヌ */        IROHA_FN_FunctionalInsert,
/* ネ */        IROHA_FN_FunctionalInsert,
/* ノ */        IROHA_FN_FunctionalInsert,
/* ハ */        IROHA_FN_FunctionalInsert,
/* ヒ */        IROHA_FN_FunctionalInsert,
/* フ */        IROHA_FN_FunctionalInsert,
/* ヘ */        IROHA_FN_FunctionalInsert,
/* ホ */        IROHA_FN_FunctionalInsert,
/* マ */        IROHA_FN_FunctionalInsert,
/* ミ */        IROHA_FN_FunctionalInsert,
/* ム */        IROHA_FN_FunctionalInsert,
/* メ */        IROHA_FN_FunctionalInsert,
/* モ */        IROHA_FN_FunctionalInsert,
/* ヤ */        IROHA_FN_FunctionalInsert,
/* ユ */        IROHA_FN_FunctionalInsert,
/* ヨ */        IROHA_FN_FunctionalInsert,
/* ラ */        IROHA_FN_FunctionalInsert,
/* リ */        IROHA_FN_FunctionalInsert,
/* ル */        IROHA_FN_FunctionalInsert,
/* レ */        IROHA_FN_FunctionalInsert,
/* ロ */        IROHA_FN_FunctionalInsert,
/* ワ */        IROHA_FN_FunctionalInsert,
/* ン */        IROHA_FN_FunctionalInsert,
/* ゛ */        IROHA_FN_FunctionalInsert,
/* ゜ */        IROHA_FN_FunctionalInsert,
/* F1 */        NONE,
/* F2 */        NONE,
/* F3 */        NONE,
/* F4 */        NONE,
/* F5 */        NONE,
/* F6 */        NONE,
/* F7 */        NONE,
/* F8 */        NONE,
/* F9 */        NONE,
/* F10 */       NONE,
/* ea */        NONE,
/* eb */        NONE,
/* ec */        NONE,
/* ed */        NONE,
/* ee */        NONE,
/* ef */        NONE,
/* PF1 */       NONE,
/* PF2 */       NONE,
/* PF3 */       NONE,
/* PF4 */       NONE,
/* PF5 */       NONE,
/* PF6 */       NONE,
/* PF7 */       NONE,
/* PF8 */       NONE,
/* PF9 */       NONE,
/* PF10 */      NONE,
/* fa */        NONE,
/* fb */        NONE,
/* fc */        NONE,
/* fd */        NONE,
/* fe */        NONE,
/* ff */        NONE,
};
