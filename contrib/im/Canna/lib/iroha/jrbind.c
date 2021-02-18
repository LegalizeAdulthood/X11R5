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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/jrbind.c,v 3.2 91/06/28 18:08:47 kon Exp $";

#include "iroha.h"
#include <iroha/mfdef.h>

#define ACTHASHTABLESIZE 64
#define KEYHASHTABLESIZE 16
   
/*

  jrKanjiString は TTY のキー入力を受け取り、そのキーにしたがって必要
  ならカナ漢字変換を行い、そのキー入力の結果として得られる文字列を 
  buffer_return で返す。buffer_return はアプリケーション側に用意するバッ
  ファであり、アプリケーションはそのバッファの長さを bytes_buffer で渡
  す。

  kanji_status_return は確定していない入力文字列を表示するためのデータ
  であり、未確定の読みや候補漢字などが返される。kanji_status_returnの
  メンバには、 echoStr, length, revPos, revLen がありそれぞれ、未確定
  文字列へのポインタ、その長さ、未確定文字列のうち、強調する部分へのオ
  フセット、強調する部分の長さを返す。未確定文字列を格納する領域は 
  jrKanjiString で自動的に用意される。

 */

extern int FirstTime;
extern int iroha_reset;

extern unsigned char *actFromHash();

int
jrKanjiString(context_id, ch, buffer_return, bytes_buffer,
	      kanji_status_return)
int	       ch;
unsigned char  *buffer_return;
int            bytes_buffer;
jrKanjiStatus  *kanji_status_return;
{
  int res;

  *buffer_return = ch;

  res = XLookupKanji2((unsigned int)0, (unsigned int)context_id,
		      buffer_return, bytes_buffer,
		      1/* byte */, 1/* functional char*/,
		      kanji_status_return);
  return res;
}

/* jrKanjiControl -- カナ漢字変換の制御を行う */

int
jrKanjiControl(context, request, arg)
     int context;
     int request;
     char *arg;
{
  return XKanjiControl2((unsigned int)0, (unsigned int)context, request, arg);
}

static uiContext
newUiContext(dpy, win)
unsigned int dpy, win;
{
  uiContext d;

  d = (uiContext)malloc(sizeof(uiContextRec));
  if (d == (uiContext)0) {
    return (uiContext)0;
  }
  if (initRomeStruct(d) < 0) {
    free(d);
    return (uiContext)0;
  }
  if (internContext(dpy, win, d) == 0) {
    freeRomeStruct(d);
    return (uiContext)0;
  }
  return d;
}

static serverInitDo();

XLookupKanji2(dpy, win, buffer_return, bytes_buffer, nbytes, functionalChar,
	      kanji_status_return)
unsigned int dpy, win;
int functionalChar, nbytes;
unsigned char *buffer_return;
int bytes_buffer;
jrKanjiStatus *kanji_status_return;
{
  uiContext d, keyToContext();
  int retval = 0;

  /* 初めて XLookupKanjiString が呼ばれた時は辞書の初期化などの処理が
     行われる。 */

  if (FirstTime) {
    int retval;

    retval = kanjiControl(KC_INITIALIZE, NULL, NULL);
    /* NULL はダミー引数 */
    if (retval == -1)
      return -1;

    d = keyToContext(dpy, win);

    if (d == NULL) {
      d = newUiContext(dpy, win);
      if (d == (uiContext)0) {
	return NoMoreMemory();
      }
    }
    
    FirstTime = 0;
  }

  d = keyToContext(dpy, win);

  if (d == NULL) {
    /* このウィンドウからイベントが来たのが始めてだったりするわけよ */
    d = newUiContext(dpy, win);
    if (d == (uiContext)0) {
      return NoMoreMemory();
    }
  }


  bzero(kanji_status_return, sizeof(jrKanjiStatus));
  
  d->ch = *buffer_return;
  d->buffer_return = buffer_return;
  d->bytes_buffer = bytes_buffer;
  d->kanji_status_return = kanji_status_return;

debug_message("current_mode(0x%x)\n", d->current_mode,0,0);

  d->nbytes = nbytes;

  if ( d->nbytes || functionalChar ) { /* キャラクタコードがとれた場合 */
#ifndef DEBUG
    retval =  doFunc(d, 0);
#else /* DEBUG */
    retval = doFunc(d, 0);
    checkModec(d);
#endif /* DEBUG */
    if(iroha_reset)
      retval = serverInitDo(d, dpy, win);
    return(retval);
  }
  else { /* キャラクタコードがとれなかった場合（シフトキーなど）... */
    d->kanji_status_return->length = -1;
    return 0;
  }
}

uiContext keyToContext();

int
XKanjiControl2(display, window, request, arg)
unsigned int display, window, request;
char *arg;
{
  if (request == KC_INITIALIZE || request == KC_FINALIZE ||
      request == KC_SETSERVERNAME || request == KC_SETINITFILENAME) {
    return kanjiControl(request, NULL, arg);
  }
  else if (0 <= request && request < MAX_KC_REQUEST) {
    uiContext d;

    d = keyToContext((unsigned int)display, (unsigned int)window);

    if (d == NULL) {
      d = newUiContext(display, window);
      if (d == (uiContext)0) {
	return NoMoreMemory();
      }
    }

    if (request == KC_CLOSEUICONTEXT) {
      rmContext(display, window);
    }
    return kanjiControl(request, d, arg);
  }
  else {
    return -1;
  }
}

_DoFuncSequence(d, keytbl, key) /* 複数の機能の割り当て */
uiContext d;
unsigned char *keytbl;
unsigned char key;
{
  int res, total_res, prevEchoLen, prevRevPos, prevRevLen;
  unsigned char *p, *prevEcho;

  if (key == NULL) {
    key = d->ch;
  }
  if (keytbl == NULL)
    keytbl = d->current_mode->keytbl;

  p = actFromHash(keytbl, key);

  if(p == NULL) {
    return 0;
  }
    
  total_res = 0;
  d->kanji_status_return->length = -1;
  for(p ; *p ; p++) {
    prevEcho = d->kanji_status_return->echoStr;
    prevEchoLen = d->kanji_status_return->length;
    prevRevPos = d->kanji_status_return->revPos;
    prevRevLen = d->kanji_status_return->revLen;
    d->kanji_status_return->length =
      d->kanji_status_return->revPos =
	d->kanji_status_return->revLen = 0;
    /* ２回目以降に以下のデータが失われている場合があるので入れ直す。 */
    d->ch = *(d->buffer_return) = key;
    d->nbytes = 1;

    res = _doFunc(d, (int)*p); /* 生の doFunc を呼ぶ。 */
    if (d->kanji_status_return->length < 0) {
      d->kanji_status_return->length = prevEchoLen;
      d->kanji_status_return->echoStr = prevEcho;
      d->kanji_status_return->revPos = prevRevPos;
      d->kanji_status_return->revLen = prevRevLen;
    }
    if (res < 0) {
      break;
    }
    if (res > 0) {
      total_res += res;
      d->buffer_return += res;
      d->bytes_buffer -= res;
    }
  }
  total_res = _afterDoFunc(d, total_res);
  d->flags |= MULTI_SEQUENCE_EXECUTED;
  return total_res;
}

DoFuncSequence(d) /* 複数の機能の割り当て */
uiContext d;
{
  return _DoFuncSequence(d, NULL, NULL);
}

struct map {
  KanjiMode tbl;
  unsigned char key;
  KanjiMode mode;
  struct map *next;
} *mapFromHash();

extern unsigned char *keyHistory;

UseOtherKeymap(d)
uiContext d;
{
  struct map *p;
  unsigned char showKey[10];

  strcpy(showKey,showChar(d->ch));
  p = mapFromHash(d->current_mode->keytbl, d->ch, 0);
  if (p == NULL) 
    return NothingChangedWithBeep(d);
  p->mode->ftbl = (int (**)())d->current_mode;
  keyHistory = (unsigned char *)malloc(strlen(showKey)+1);
  if (keyHistory) {
    strcpy(keyHistory,showKey);
    makeGLineMessage(d, keyHistory, strlen(keyHistory));
    if (p->mode->keytbl == NULL) {
      free(keyHistory);
      return NothingChangedWithBeep(d);
    }
    d->current_mode = p->mode;
  }
  return NothingForGLine(d);
}

/* cfuncdef

  pushCallback -- コールバックの集合をプッシュする。

  コールバックの集合を格納する配列が malloc されて、それが uiContext に
  プッシュされる。

  malloc された配列が戻り値として返る。

 */

struct callback *
pushCallback(d, environ, ev, ex, qu, au)
uiContext d;
mode_context environ;
int (*ev)(), (*ex)(), (*qu)(), (*au)();
{
  struct callback *newCB;

  newCB = (struct callback *)malloc(sizeof(struct callback));
  if (newCB) {
    newCB->func[0] = ev;
    newCB->func[1] = ex;
    newCB->func[2] = qu;
    newCB->func[3] = au;
    newCB->env = environ;
    newCB->next = d->cb;
    d->cb = newCB;
  }
  return newCB;
}

void
popCallback(d)
uiContext d;
{
  struct callback *oldCB;

  oldCB = d->cb;
  d->cb = oldCB->next;
  free(oldCB);
}

static
serverInitDo(d, dpy, win)
uiContext d;
int dpy, win;
{
  char **warning;
  jrKanjiStatus *ks;
  unsigned char *br, ch;
  int bb;
  extern struct ModeNameRecs ModeNames[];

  /* 前の情報を取っておく */
  ks = d->kanji_status_return;
  br = d->buffer_return;
  ch = d->ch;
  bb = d->bytes_buffer;

  iroha_reset = 0;

  KC_finalize(0/* dummy */, &warning);
  KC_initialize(0/* dummy */, &warning); 

  d = keyToContext(dpy, win);
  if (d == NULL) {
    /* このウィンドウからイベントが来たのが始めてだったりするわけよ */
    /* 初めてに決まっているけど */
    d = newUiContext(dpy, win);
    if (d == (uiContext)0) {
      return NoMoreMemory();
    }
  }

  /* 取っておいた情報を戻す */
  d->ch = ch;
  d->buffer_return = br;
  d->bytes_buffer = bb;
  d->kanji_status_return = ks;

  if(warning)
    strcpy(d->genbuf, *warning);
  else 
    strcpy(d->genbuf, "日本語入力システムの初期化をしました");

  makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
  d->kanji_status_return->info |= KanjiModeInfo;
  d->kanji_status_return->mode = ModeNames[getMinorMode(d)].name;

  return(0);
}
