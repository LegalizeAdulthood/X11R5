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

/* filedef

  util.c -- ユーティリティ関数を集めた。

 */

static char sccs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/util.c,v 2.27 91/05/21 16:03:59 satoko Exp $";

#include "iroha.h"

/* arraydef

  tmpbuf -- ちょっと仮に使われるバッファ

 */

/*
 * Gline をクリアする
 *
 * 引き数	uiContext
 * 戻り値	なし
 */
GlineClear(d)
uiContext d;
{
  d->kanji_status_return->info |= KanjiGLineInfo;
  d->kanji_status_return->gline.line = (unsigned char *)NULL;
  d->kanji_status_return->gline.length = 0;
  d->kanji_status_return->gline.revPos = 0;
  d->kanji_status_return->gline.revLen = 0;
}

/* cfuncdef

  Gline2echostr -- 一覧行の内容をその場に移動

 */

static
Gline2echostr(d)
uiContext d;
{
  d->kanji_status_return->echoStr =
    d->kanji_status_return->gline.line;
  d->kanji_status_return->length =
    d->kanji_status_return->gline.length;
  d->kanji_status_return->revPos =
    d->kanji_status_return->gline.revPos;
  d->kanji_status_return->revLen =
    d->kanji_status_return->gline.revLen;
  GlineClear(d);
}

static
echostr2Gline(d)
uiContext d;
{
  d->kanji_status_return->gline.line =
    d->kanji_status_return->echoStr;
  d->kanji_status_return->gline.length =
    d->kanji_status_return->length;
  d->kanji_status_return->gline.revPos =
    d->kanji_status_return->revPos;
  d->kanji_status_return->gline.revLen =
    d->kanji_status_return->revLen;
  d->kanji_status_return->info |= KanjiGLineInfo;
}

echostrClear(d)
uiContext d;
{
  d->kanji_status_return->echoStr = (unsigned char *)NULL;
  d->kanji_status_return->length =
    d->kanji_status_return->revPos = d->kanji_status_return->revLen = 0;
}

/* cfuncdef

  checkGLineLen -- 一覧行に表示できる長さを越えているかをチェック

  長さが越えていたら、カーソル部分に表示されるようにする。

 */

checkGLineLen(d)
uiContext d;
{
  if (d->kanji_status_return->info & KanjiGLineInfo) {
    if (d->kanji_status_return->gline.length > d->ncolumns) {
      Gline2echostr(d);
    }
  }
}

/* cfuncdef

  NothingChanged -- 読みについては何も変えないようにする

 */

static
NothingChanged(d)
uiContext d;
{
  d->kanji_status_return->length = -1; /* 変わらない。 */
  d->kanji_status_return->revPos 
    = d->kanji_status_return->revLen = 0;
  d->kanji_status_return->info = 0;
  return 0;
}

NothingForGLine(d)
uiContext d;
{
  d->kanji_status_return->length = -1; /* 変わらない。 */
  d->kanji_status_return->revPos 
    = d->kanji_status_return->revLen = 0;
  return 0;
}

NothingChangedWithBeep(d)
uiContext d;
{
  extern int (*jrBeepFunc)();

  if (jrBeepFunc) {
    jrBeepFunc();
  }
  return NothingChanged(d);
}

NothingForGLineWithBeep(d)
uiContext d;
{
  extern int (*jrBeepFunc)();

  if (jrBeepFunc) {
    jrBeepFunc();
  }
  return NothingForGLine(d);
}

#ifdef SOMEONE_USE_THIS
/* 誰も使っていないみたい。 */
Insertable(ch)
unsigned char ch;
{
  if ((0x20 <= ch && ch <= 0x7f) || (0xa0 <= ch && ch <= 0xff)) {
    return 1;
  }
  else {
    return 0;
  }
}
#endif /* SOMEONE_USE_THIS */

/* 
 ujisncpy -- ２バイト文字の途中で切れないようにコピーする関数

  実際にコピーするバイト数は n で指定されたバイト数かあるいはそれより
  １小さい値になる。実際にコピーしたバイト数を返す。

  SS3 には対応していない。
 */

int
ujisncpy(dest, src, n)
unsigned char *dest, *src;
int n;
{
  int i = 0;
  unsigned char c;

  while (i < n) {
    c = src[i];
    if ( ! (c & 0x80) ) { /* ASCII の場合 */
      dest[i++] = c;
    }
    else if (i + 1 < n) { /* 漢字かカナの場合、しかもコピーできる場合 */
      dest[i++] = c;
      dest[i] = src[i];
      i++;
    }
    else { /* 漢字で１バイトしかコピーできない場合 */
      return i; /* コピーしきれず、n より１小さい値で終わった。 */
    }
  }
  return i; /* n バイトコピーしきれた */
}

/*
 * リバースなしのメッセージをガイドラインに表示する
 * 次の入力があったときに消えるようにフラグを設定する
 */
makeGLineMessage(d, msg, sz)
uiContext d;
unsigned char *msg;
int sz;
{
  d->kanji_status_return->gline.line = msg;
  d->kanji_status_return->gline.length = sz;
  d->kanji_status_return->gline.revPos = 0;
  d->kanji_status_return->gline.revLen = 0;
  d->kanji_status_return->info |= KanjiGLineInfo;

  d->flags &= ~PCG_RECOGNIZED;
  d->flags |= PLEASE_CLEAR_GLINE;
  checkGLineLen(d);
}

/* 

  ある読みコンテクストから他の読みコンテクストに対して読みデータをコピーする
  カーソルは先頭に来る。

 */

copyYomiBuffers(destycx, srcycx, skana, ekana, sromaji, eromaji)
yomiContext destycx, srcycx;
int skana, ekana, sromaji, eromaji;
{
  int i;

  for (i = skana ; i < ekana ; i++) {
    destycx->kana_buffer[i - skana] = srcycx->kana_buffer[i];
    destycx->kAttr[i - skana] = srcycx->kAttr[i];
  }
  destycx->kAttr[i] = SENTOU;
  destycx->kEndp = ekana - skana;
  destycx->kana_buffer[ekana - skana] = (unsigned char)'\0';
  destycx->kRStartp = destycx->kCurs = 0;

  for (i = sromaji ; i < eromaji ; i++) {
    destycx->romaji_buffer[i - sromaji] = srcycx->romaji_buffer[i];
    destycx->rAttr[i - sromaji] = srcycx->rAttr[i];
  }
  destycx->rAttr[i] = SENTOU;
  destycx->rEndp = eromaji - sromaji;
  destycx->romaji_buffer[eromaji - sromaji] = (unsigned char)'\0';
  destycx->rStartp = destycx->rCurs = 0;
}

#ifdef DEBUG
dbg_msg(fmt, x, y, z)
char *fmt;
int x, y, z;
{
  if (iroha_debug) {
    fprintf(stderr, fmt, x, y, z);
  }
}

checkModec(d)
uiContext d;
{
  coreContext c;
  struct callback *cb;
  int depth = 0, cbDepth = 0;
  int callbacks = 0;

  for (c = (coreContext)d->modec ; c ; c = (coreContext)c->next)
    depth++;
  for (cb = d->cb ; cb ; cb = cb->next) {
    int i;

    cbDepth++;
    for (i = 0 ; i < 4 ; i++) {
      callbacks <<= 1;
      if (cb->func[i]) {
	callbacks++;
      }
    }
  }
  if (depth != cbDepth) {
    fprintf(stderr, "■■■■■！！！深さが違うぞ！！！■■■■■\n");
  }
  debug_message("■ 深さ: d->modec:%d d->cb:%d callbacks:0x%08x ", 
		depth, cbDepth, callbacks);
  debug_message("EXIT_CALLBACK = 0x%x\n", d->cb->func[EXIT_CALLBACK],0,0);
  {
    extern KanjiModeRec yomi_mode;
    if (d->current_mode == &yomi_mode) {
      yomiContext yc = (yomiContext)d->modec;
      if (yc->kana_buffer[yc->kEndp]) {
	fprintf(stderr, "■■■■■ カナバッファにゴミが入っているぞ！\n");
      }
    }
  }
}

static char pbufstr[] = " o|do?b%";

showRomeStruct(dpy, win)
unsigned int dpy, win;
{
  char buf[1024];
  uiContext d, keyToContext();
  static int n = 0;
  int i;
  
  n++;
  fprintf(stderr, "\n【デバグメッセージ(%d)】\n", n);
  d = keyToContext((unsigned int)dpy, (unsigned int)win);
  fprintf(stderr, "buffer(0x%x), bytes(%d)\n",
	  d->buffer_return, d->bytes_buffer);
  fprintf(stderr, "nbytes(%d), ch(0x%x)\n", d->nbytes, d->ch);
  fprintf(stderr, "モード: %d\n", ((coreContext)d->modec)->minorMode);

  /* コンテクスト */
  fprintf(stderr, "コンテクスト(%d)\n", d->contextCache);
  fprintf(stderr, "デフォルトコンテクスト(%d), 部首コンテクスト(%d)\n",
	  defaultContext, defaultBushuContext);

  /* ローマ字かな関連 */
  if (((coreContext)d->modec)->id == YOMI_CONTEXT) {
    yomiContext yc = (yomiContext)d->modec;

    fprintf(stderr, "r:       Start(%d), Cursor(%d), End(%d)\n",
	    yc->rStartp, yc->rCurs, yc->rEndp);
    fprintf(stderr, "k: 未変換Start(%d), Cursor(%d), End(%d)\n",
	    yc->kRStartp, yc->kCurs, yc->kEndp);
    strncpy(buf, yc->romaji_buffer, yc->rEndp);
    buf[yc->rEndp] = NULL;
    fprintf(stderr, "romaji_buffer(%s)\n", buf);
    fprintf(stderr, "romaji_attrib(");
    for (i = 0 ; i <= yc->rEndp ; i++) {
      fprintf(stderr, "%1x", yc->rAttr[i]);
    }
    fprintf(stderr, ")\n");
    fprintf(stderr, "romaji_pointr(");
    for (i = 0 ; i <= yc->rEndp ; i++) {
      int n = 0;
      if (i == yc->rStartp)
	n |= 1;
      if (i == yc->rCurs)
	n |= 2;
      if (i == yc->rEndp)
	n |= 4;
      fprintf(stderr, "%c", pbufstr[n]);
    }
    fprintf(stderr, ")\n");
    strncpy(buf, yc->kana_buffer, yc->kEndp);
    buf[yc->kEndp] = NULL;
    fprintf(stderr, "kana_buffer(%s)\n", buf);
    fprintf(stderr, "kana_attrib(");
    for (i = 0 ; i <= yc->kEndp ; i++) {
      fprintf(stderr, "%1x", yc->kAttr[i]);
    }
    fprintf(stderr, ")\n");
    fprintf(stderr, "kana_pointr(");
    for (i = 0 ; i <= yc->kEndp ; i++) {
      int n = 0;
      if (i == yc->kRStartp)
	n |= 1;
      if (i == yc->kCurs)
	n |= 2;
      if (i == yc->kEndp)
	n |= 4;
      fprintf(stderr, "%c", pbufstr[n]);
    }
    fprintf(stderr, ")\n");
    fprintf(stderr, "\n");
  }
/*  RkPrintDic(0, "kon"); */
}
#endif /* DEBUG */

NoMoreMemory()
{
  extern char *necKanjiError;

  necKanjiError = "malloc に失敗しました。";
  return NG;
}

GLineNGReturn(d)
uiContext d;
{
  strcpy(d->genbuf, necKanjiError);
  makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
  currentModeInfo(d);

  return(0);
}

GLineNGReturnFI(d)
uiContext d;
{
  popForIchiranMode(d);
  popCallback(d);

  strcpy(d->genbuf, necKanjiError);
  makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
  currentModeInfo(d);

  return(0);
}

GLineNGReturnTK(d)
uiContext d;
{
  popTourokuMode(d);
  popCallback(d);

  strcpy(d->genbuf, necKanjiError);
  makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
  currentModeInfo(d);

  return(0);
}

static
afterBgnBunError(d, popfunc)
uiContext d;
int (*popfunc)();
{
  popfunc();

  escapeToBasicStat(d, IROHA_FN_Quit);

  strcpy(d->genbuf, necKanjiError);
  makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
  currentModeInfo(d);

  return(0);
}

#ifdef DEBUG_ALLOC
int fail_malloc = 0;

#undef malloc

char *
debug_malloc(n)
int n;
{
  char *malloc();

  if (fail_malloc)
    return (char *)0;
  else
    return malloc(n);
}
#endif /* DEBUG_ALLOC */
