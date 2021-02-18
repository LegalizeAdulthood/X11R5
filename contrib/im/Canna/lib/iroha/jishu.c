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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/jishu.c,v 3.3 91/09/05 20:29:53 kon Exp $";

#include "iroha.h"
#include <ctype.h>

static int setInhibitInformation(), JishuNextJishu(), JishuPreviousJishu();
static int jishuAdjustRome(), myjishuAdjustRome(), JishuZenkaku();
static int JishuHankaku(), YomiJishu();

/* jc->kc          何の文字種か
 * d->jishu_rEndp
 * d->jishu_kEndp
 * 【例】 
 *               あいしsh|
 * C-            アイシsh|
 * C-            アイシs|h
 * C-            アイシ|sh
 * C-            アイ|しsh
 *
 *               あいしsh|
 * C-            aishish|
 * C-            ａｉｓｈｉｓｈ|
 * C-            ａｉｓｈｉｓ|h
 * C-            ａｉｓｈｉ|sh
 * C-            ａｉｓｈ|いsh
 * C-            aish|いsh
 * C-            あいsh|いsh
 * C-            アイsh|sh
 * C-            アイs|ひsh
 * C-            アイ|しsh
 * C-
 * 
 */

#define	INHIBIT_HANKATA	01
#define	INHIBIT_KANA	02
#define INHIBIT_ALPHA	04

static jishuContext
pushJishuMode(d, yc, len, ev, ex, qu, au)
uiContext d;
yomiContext yc;
int len;
int (*ev)(), (*ex)(), (*qu)(), (*au)();
{
  struct callback *pushCallback();
  void popJishuMode();
  jishuContext jc;
  extern KanjiModeRec jishu_mode;

  jc = (jishuContext)malloc(sizeof(jishuContextRec));
  if (jc) {
    jc->id = JISHU_CONTEXT;
    jc->next = (mode_context)d->modec;
    jc->ycx = yc;
    jc->kc = JISHU_HIRA;/* 今はひらがなモードです */
    jc->jishu_case = 0; /* 小文字ベースのモードです */
    setInhibitInformation(jc);
    jc->jishu_rEndp = yc->rEndp;/* ローマ字バッファの長さをコピー */
    jc->jishu_kEndp = yc->kEndp;/* かなバッファの長さをコピー */
    jc->majorMode = d->majorMode;
    d->modec = (mode_context)jc;
    if (pushCallback(d, yc, ev, ex, qu, au) == (struct callback *)0) {
      popJishuMode(d);
      return 0;
    }
    jc->prevMode = d->current_mode;
    d->current_mode = &jishu_mode;
  }
  return jc;
}

void
freeJishuContext(jc)
jishuContext jc;
{
  free(jc);
}

void
popJishuMode(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;

  d->modec = jc->next;
  d->current_mode = jc->prevMode;
  freeJishuContext(jc);
}

static
setInhibitInformation(jc)
jishuContext jc;
{
  int i;
  extern InhibitHankakuKana;
  yomiContext yc = jc->ycx;

  jc->inhibition = InhibitHankakuKana ? INHIBIT_HANKATA : 0;
  for (i = 0 ; i < yc->kEndp ; i++) {
    if ( !(yc->kAttr[i] & HENKANSUMI) ) {
      jc->inhibition |= INHIBIT_KANA;
      break;
    }
  }
  for (i = 0 ; i < yc->rEndp ; i++) {
    if (yc->romaji_buffer[i] & 0x80) {
      jc->inhibition |= INHIBIT_ALPHA;
    }
  }
}

static
exitJishu(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  /* ここから下は完全な『読み』モード */
  retval = YomiExit(d, retval);
  currentModeInfo(d);
  return retval;
}

static
quitJishu(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  extern KanjiModeRec yomi_mode;

  popCallback(d);
  makeYomiReturnStruct(d);
/*  d->current_mode = &yomi_mode; */
  currentModeInfo(d);
  return retval;
}

YomiNextJishu(d) /* 読みモードからの順回り文字種変換 */
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;
  jishuContext jc;
  int ret;

  if (yc->henkanInhibition & IROHA_YOMI_INHIBIT_JISHU) {
    return NothingChangedWithBeep(d);
  }
  jc = pushJishuMode(d, yc, RomajiFlushYomi(d, yc->kana_buffer, ROMEBUFSIZE),
		     NO_CALLBACK, exitJishu, quitJishu, NO_CALLBACK);
  if (jc == (jishuContext)0) {
    return NoMoreMemory();
  }
  ret = JishuNextJishu(d);
  jc->minorMode = IROHA_MODE_JishuMode;
  currentModeInfo(d);
  return ret;
}

YomiPreviousJishu(d) /* 読みモードからの逆回り文字種変換 */
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;
  jishuContext jc;
  int ret;

  if (yc->henkanInhibition & IROHA_YOMI_INHIBIT_JISHU) {
    return NothingChangedWithBeep(d);
  }
  jc = pushJishuMode(d, yc, RomajiFlushYomi(d, yc->kana_buffer, ROMEBUFSIZE),
		     NO_CALLBACK, exitJishu, quitJishu, NO_CALLBACK);
  if (jc == (jishuContext)0) {
    return NoMoreMemory();
  }
  ret = JishuPreviousJishu(d);
  jc->minorMode = IROHA_MODE_JishuMode;
  currentModeInfo(d);
  return ret;
}

int YomiJishu();

YomiZenkaku(d) /* 読みモードからの全角変換 */
uiContext d;
{
  int JishuZenkaku();

  return YomiJishu(d, JishuZenkaku);
}

YomiHankaku(d) /* 読みモードからの半角変換 */
uiContext d;
{
  int JishuHankaku();
  extern InhibitHankakuKana;

  if (InhibitHankakuKana)
    return NothingChangedWithBeep(d);
  else
    return YomiJishu(d, JishuHankaku);
}

static
jishuHenkan(d) /* 引数の字種に字種を変換するだけ */
uiContext d;
{
  char xxxx[1024];
  int len, revlen;
  int n, xferflag;
  jishuContext jc = (jishuContext)d->modec;
  yomiContext yc = jc->ycx;

  switch (jc->kc)
    {
    case JISHU_ZEN_KATA: /* 全角カタカナに変換する */
      len = RkCvtZen(xxxx, 1024, yc->kana_buffer, jc->jishu_kEndp);
      revlen = RkCvtKana(d->genbuf, ROMEBUFSIZE, xxxx, len);
      break;

    case JISHU_HAN_KATA: /* 半角カタカナに変換する */
      len = RkCvtKana(xxxx, 1024, yc->kana_buffer, jc->jishu_kEndp);
      revlen = RkCvtHan(d->genbuf, ROMEBUFSIZE, xxxx, len);
      break;

    case JISHU_HIRA: /* ひらがなに変換する */
      len = RkCvtZen(xxxx, 1024, yc->kana_buffer, jc->jishu_kEndp);
      revlen = RkCvtHira(d->genbuf, ROMEBUFSIZE, xxxx, len);
      break;

    case JISHU_ZEN_ALPHA: /* 全角英数に変換する */
      if (jc->jishu_case == IROHA_JISHU_UPPER) {
	int i;
	unsigned char *p = yc->romaji_buffer;

	for (i = 0 ; i < jc->jishu_rEndp ; i++) {
	  if ('a' <= p[i] && p[i] <= 'z') {
	    xxxx[i] = toupper(p[i]);
	  }
	  else {
	    xxxx[i] = p[i];
	  }
	}
	revlen = RkCvtZen(d->genbuf, ROMEBUFSIZE, xxxx, jc->jishu_rEndp);
      }
      else if (jc->jishu_case == IROHA_JISHU_CAPITALIZE) {
	strncpy(xxxx, yc->romaji_buffer, jc->jishu_rEndp);
	if ('a' <= xxxx[0] && xxxx[0] <= 'z') {
	  xxxx[0] = toupper(xxxx[0]);
	}
	revlen = RkCvtZen(d->genbuf, ROMEBUFSIZE, xxxx, jc->jishu_rEndp);
      }
      else {
	revlen = RkCvtZen(d->genbuf, ROMEBUFSIZE,
			  yc->romaji_buffer, jc->jishu_rEndp);
      }
      break;

    case JISHU_HAN_ALPHA: /* 半角英数に変換する */
      revlen = jc->jishu_rEndp;
      if (jc->jishu_case == IROHA_JISHU_UPPER) {
	int i;
	unsigned char *p = yc->romaji_buffer;

	for (i = 0 ; i < revlen ; i++) {
	  if ('a' <= p[i] && p[i] <= 'z') {
	    d->genbuf[i] = toupper(p[i]);
	  }
	  else {
	    d->genbuf[i] = p[i];
	  }
	}
      }
      else if (jc->jishu_case == IROHA_JISHU_CAPITALIZE) {
	strncpy(d->genbuf, yc->romaji_buffer, revlen);
	d->genbuf[0] = toupper(yc->romaji_buffer[0]);
      }
      else {
	strncpy(d->genbuf, yc->romaji_buffer, revlen);
      }
      break;

    default:/* どれでもなかったら変換出来ないので何もしない */
      break;
    }

/* 文字種変換しない部分を付け加える */
  switch (jc->kc)
    {
    case JISHU_HIRA: /* ひらがななら */
    case JISHU_ZEN_KATA: /* 全角カタカナなら */
    case JISHU_HAN_KATA: /* 半角カタカナなら */
      strncpy(d->genbuf + revlen, &yc->kana_buffer[jc->jishu_kEndp],
	      yc->kEndp - jc->jishu_kEndp);
                                      /* かなバッファから文字列を取り出す */
      len = revlen + yc->kEndp - jc->jishu_kEndp;
      break;

    case JISHU_ZEN_ALPHA: /* 全角英数なら */
    case JISHU_HAN_ALPHA: /* 半角英数なら */
      xferflag = RK_XFER;
      switch (yc->baseChar) {
      case JISHU_ZEN_KATA:
	xferflag = RK_KFER;
      case JISHU_HIRA:
	len = RkCvtRoma(romajidic, d->genbuf + revlen, ROMEBUFSIZE - revlen,
			yc->romaji_buffer + jc->jishu_rEndp,
			yc->rEndp - jc->jishu_rEndp,
			RK_FLUSH | RK_SOKON | xferflag);
	break;
      case JISHU_HAN_KATA:
	len = RkCvtRoma(romajidic, xxxx, 1024,
			yc->romaji_buffer + jc->jishu_rEndp,
			yc->rEndp - jc->jishu_rEndp,
			RK_FLUSH | RK_SOKON | RK_KFER);
	len = RkCvtHan(d->genbuf + revlen, ROMEBUFSIZE - revlen,
		       xxxx, len);
	break;
      case JISHU_ZEN_ALPHA:
	len = RkCvtZen(d->genbuf + revlen, ROMEBUFSIZE - revlen,
		       yc->romaji_buffer + jc->jishu_rEndp,
		       yc->rEndp - jc->jishu_rEndp);
	break;
      case JISHU_HAN_ALPHA:
	len = RkCvtNone(d->genbuf + revlen, ROMEBUFSIZE - revlen,
			yc->romaji_buffer + jc->jishu_rEndp,
			yc->rEndp - jc->jishu_rEndp);
	break;
      }
      len += revlen;
      break;
    default:/* どれでもなかったら何もしない */
      break;
    }
  d->genbuf[len] = '\0';
  d->kanji_status_return->echoStr = (unsigned char *)d->genbuf;
  d->kanji_status_return->length  = len;
  d->kanji_status_return->revLen  = revlen;
  d->kanji_status_return->revPos  = 0;

  return revlen;
}

static
inhibittedJishu(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;

  return (((jc->kc == JISHU_ZEN_KATA || jc->kc == JISHU_HAN_KATA) &&
	   (jc->inhibition & INHIBIT_KANA)) ||
	  ((jc->kc == JISHU_ZEN_ALPHA || jc->kc == JISHU_HAN_ALPHA) &&
	   (jc->inhibition & INHIBIT_ALPHA)) ||
	  ((jc->kc == JISHU_HAN_KATA) && 
	   (jc->inhibition & INHIBIT_HANKATA))
	  );
}

static
void
nextJishu(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;

  do {
    jc->kc = (unsigned char)(((int)jc->kc + 1) % MAX_JISHU);
  } while (inhibittedJishu(d));
}

static
void
previousJishu(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;

  do {
    jc->kc = (unsigned char)(((int)jc->kc + MAX_JISHU - 1) % MAX_JISHU);
  } while (inhibittedJishu(d));
}
	    

static
JishuNextJishu(d) /* 字種モードの時に順回り文字種変換をする */
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;
  yomiContext yc = jc->ycx;

#ifdef DEBUG
    if (iroha_debug) {
      fprintf(stderr, "文字種変換の始め");
      fprintf(stderr, "jc->kc(%d)\n", jc->kc);
    }
#endif

/* 取り出した文字列を変換する */
    nextJishu(d);
    if (jc->kc == JISHU_HIRA) {
      if (jc->jishu_kEndp == yc->kEndp && jc->jishu_rEndp == yc->rEndp) {
	popJishuMode(d);
	d->status = QUIT_CALLBACK;
	return 0;
      }
    }

#ifdef DEBUG
    if (iroha_debug) {
      fprintf(stderr, "文字種変換の終わり");
      fprintf(stderr, "jc->kc(%d)\n", jc->kc);
    }
#endif

  jishuHenkan(d); /* 字種を変換 */
  return 0;
}

static
JishuPreviousJishu(d) /* 字種モードの時に逆回り文字種変換をする */
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;
  yomiContext yc = jc->ycx;

/* 取り出した文字列を変換する */
  previousJishu(d);
  if (jc->kc == JISHU_HIRA) {
    if (jc->jishu_kEndp == yc->kEndp && jc->jishu_rEndp == yc->rEndp) {
      popJishuMode(d);
      d->status = QUIT_CALLBACK;
      return 0;
    }
  }
  jishuHenkan(d);
  return 0;
}

static
JishuShrink(d) /* 文字種変換領域を縮める */
uiContext d;
{  
  int f;
  jishuContext jc = (jishuContext)d->modec;
  yomiContext yc = jc->ycx;

  f = (yc->kana_buffer[jc->jishu_kEndp - 1]&0x80)?2:1;
               /* カーソルの前の文字の最左ビットが立っていたら */
               /* jc->jishu_kEndp は、絶対０にならないから、できる！！ */ 

  /* 種々のポインタを戻す */
  switch (jc->kc)
    {
    case JISHU_ZEN_ALPHA:
    case JISHU_HAN_ALPHA: /* 全角英数字か半角英数字なら */
      myjishuAdjustRome(d);
      jc->jishu_rEndp--; /* 字種ローマ字バッファインデックスを１戻す */
      if (yc->rAttr[jc->jishu_rEndp] & SENTOU) {
	                       /* ローマ字かな変換先頭フラグバッファが
				* 立っていたら
			        */
	for (--jc->jishu_kEndp ; 
	     jc->jishu_kEndp > 0 && !(yc->kAttr[jc->jishu_kEndp] & SENTOU) ;
	     --jc->jishu_kEndp);
	                       /* かな変換したフラグバッファの先頭が
				* 立っている所まで
				* 字種かなバッファインデックスを
				* 戻す
			        */
      }
      break;
    case JISHU_HIRA:
    case JISHU_ZEN_KATA:
    case JISHU_HAN_KATA: /* ひらがなか全角カタカナか半角カタカナなら */
      jishuAdjustRome(d);
      jc->jishu_kEndp -= f; /* 字種かなバッファインデックスを
	                    * １文字分戻す */
      if (yc->kAttr[jc->jishu_kEndp] & SENTOU) {
                               /* かな変換したフラグバッファが
				* 立っていたら
			        */
	for (--jc->jishu_rEndp ; 
	     jc->jishu_rEndp > 0 && !(yc->rAttr[jc->jishu_rEndp] & SENTOU) ;
	     --jc->jishu_rEndp);
	                       /* ローマ字かな変換先頭フラグバッファが
				* 立っている所まで
				* 字種ローマ字バッファインデックスを
				* 戻す
			        */
      }
      break;
    }

  if(jc->jishu_rEndp <= 0) {/* １周したら字種バッファインデックスを
			    * 元の長さに戻す
			    */
    jc->jishu_kEndp = yc->kEndp;
    jc->jishu_rEndp = yc->rEndp;
  }
  jishuHenkan(d);
  return 0;
}

static
JishuExtend(d) /* 文字種変換領域を伸ばす */
uiContext d;
{
  int f;
  jishuContext jc = (jishuContext)d->modec;
  yomiContext yc = jc->ycx;

  /* 種々のポインタを増やす */
  switch (jc->kc)
    {
    case JISHU_ZEN_ALPHA:
    case JISHU_HAN_ALPHA: /* 全角英数字か半角英数字なら */
      myjishuAdjustRome(d);

      if(jc->jishu_rEndp >= yc->rEndp && jc->jishu_kEndp >= yc->kEndp ) {
                                    /* １周したら字種バッファインデックスを
				     * 一番前に戻す
				     */
	jc->jishu_rEndp = 0;
	jc->jishu_kEndp = 0;
      }

      f = (yc->kana_buffer[jc->jishu_kEndp]&0x80)?2:1; 
                           /* カーソル上の文字の最左ビットが立っていたら */

      if (yc->rAttr[jc->jishu_rEndp] & SENTOU) {
	                       /* ローマ字かな変換先頭フラグバッファが
				* 立っていたら
			        */

	for (jc->jishu_kEndp++ ; 
	     jc->jishu_kEndp > 0 && !(yc->kAttr[jc->jishu_kEndp] & SENTOU) ;
	     jc->jishu_kEndp++);
	                       /* かな変換したフラグバッファの先頭が
				* 立っている所まで
				* 字種かなバッファインデックスを増やす
			        */
      }
      jc->jishu_rEndp++; /* 字種ローマ字バッファインデックスを１増やす */
      break;
    case JISHU_HIRA:
    case JISHU_ZEN_KATA:
    case JISHU_HAN_KATA: /* ひらがなか全角カタカナか半角カタカナなら */
      jishuAdjustRome(d);

      if(jc->jishu_rEndp >= yc->rEndp && jc->jishu_kEndp >= yc->kEndp ) {
                                    /* １周したら字種バッファインデックスを
				     * 一番前に戻す
				     */
	jc->jishu_rEndp = 0;
	jc->jishu_kEndp = 0;
      }

      f = (yc->kana_buffer[jc->jishu_kEndp]&0x80)?2:1; 
                     /* カーソル上の文字の最左ビットが立っていたら */

      if (yc->kAttr[jc->jishu_kEndp] & SENTOU) {
                               /* かな変換したフラグバッファが
				* 立っていたら
			        */
	for (jc->jishu_rEndp++ ; 
	     jc->jishu_rEndp > 0 && !(yc->rAttr[jc->jishu_rEndp] & SENTOU) ;
	     jc->jishu_rEndp++);
	                       /* ローマ字かな変換先頭フラグバッファが
				* 立っている所まで
				* 字種ローマ字バッファインデックスを増やす
			        */
      }
      jc->jishu_kEndp += f; /* 字種かなバッファインデックスを
	                    * １文字分増やす
			    */
      break;
    }
  jishuHenkan(d);
  return 0;
}

static
jishuAdjustRome(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;
  yomiContext yc = jc->ycx;

  while (!(yc->rAttr[jc->jishu_rEndp] & SENTOU)) {
    ++jc->jishu_rEndp;
  }
}

static
myjishuAdjustRome(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;
  yomiContext yc = jc->ycx;

  while (!(yc->kAttr[jc->jishu_kEndp] & SENTOU)
	 && !(jc->jishu_kEndp == yc->kEndp)) {
    ++jc->jishu_kEndp;
  }
}

static
JishuZenkaku(d) /* 全角変換 */
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;

/* 取り出した文字列を変換する */
  switch(jc->kc)
    {
    case JISHU_HIRA: /* ひらがななら何もしない */
      break;
      
    case JISHU_HAN_ALPHA: /* 半角英数なら全角英数に変換する */
      jc->kc = JISHU_ZEN_ALPHA;
      break;
      
    case JISHU_ZEN_ALPHA: /* 全角英数なら何もしない */
      break;
      
    case JISHU_HAN_KATA: /* 半角カタカナなら全角カタカナに変換する */
      jc->kc = JISHU_ZEN_KATA;
      break;
      
    case JISHU_ZEN_KATA: /* 全角カタカナなら何もしない */
      break;
      
    default: /* どれでもなかったら変換出来ないので何もしない */
      break;
    }

  jishuHenkan(d);
  return 0;
}

static
JishuHankaku(d) /* 半角変換 */
     uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;
  extern InhibitHankakuKana;
  
  /* 取り出した文字列を変換する */
  switch(jc->kc)
    {
    case JISHU_HIRA: /* ひらがななら半角カタカナに変換する */
      jc->kc = JISHU_HAN_KATA;
      break;
      
    case JISHU_ZEN_KATA: /* 全角カタカナなら半角カタカナに変換する */
      jc->kc = JISHU_HAN_KATA;
      break;
      
    case JISHU_HAN_KATA: /* 半角カタカナなら何もしない */
      break;
      
    case JISHU_ZEN_ALPHA: /* 全角英数なら半角英数に変換する */
      jc->kc = JISHU_HAN_ALPHA;
      break;
      
    case JISHU_HAN_ALPHA: /* 半角英数なら何もしない */
      break;
      
    default: /* どれでもなかったら変換出来ないので何もしない */
      break;
    }

  if (jc->kc == JISHU_HAN_KATA && InhibitHankakuKana) {
    return NothingChangedWithBeep(d);
  }

  jishuHenkan(d);
  return 0;
}

static
JishuKakutei(d)
uiContext d;
{
  jishuHenkan(d);
  d->nbytes = strlen(d->genbuf);
  if (d->nbytes > d->bytes_buffer) { /* バッファが足りない */
    d->nbytes = ujisncpy(d->buffer_return, d->genbuf, d->bytes_buffer);
  }
  else {
    strcpy(d->buffer_return, d->genbuf);
  }
  d->kanji_status_return->revPos =
    d->kanji_status_return->revLen =
      d->kanji_status_return->length = 0;

  popJishuMode(d);
  d->status = EXIT_CALLBACK;
  return d->nbytes;
}

static
JishuYomiInsert(d)
uiContext d;
{
  int n;

  n = JishuKakutei(d); /* 字種モードのポップは JishuKakutei で行われる */
  d->more.todo = 1;
  d->more.ch = d->ch;
  d->more.fnum = 0;    /* 上の ch で示される処理をせよ */
  return n;
}

static
JishuQuit(d)
uiContext d;
{
  popJishuMode(d);
  d->status = QUIT_CALLBACK;
  return 0;
}

/* 大文字にする関数 */

static
JishuToUpper(d)
uiContext d;
{
  int len, revlen, i;
  unsigned char *p;
  jishuContext jc = (jishuContext)d->modec;

  if (jc->kc == JISHU_ZEN_ALPHA || jc->kc == JISHU_HAN_ALPHA) {
    jc->jishu_case = IROHA_JISHU_UPPER;
    jishuHenkan(d);
    return 0;
  }
  else {
    /* 前と何も変わりません */
    d->kanji_status_return->length = -1;
    return 0;
  }
}

static
JishuCapitalize(d)
uiContext d;
{
  int len, revlen, i;
  unsigned char *p;
  jishuContext jc = (jishuContext)d->modec;

  if (jc->kc == JISHU_ZEN_ALPHA || jc->kc == JISHU_HAN_ALPHA) {
    jc->jishu_case = IROHA_JISHU_CAPITALIZE;
    jishuHenkan(d);
    return 0;
  }
  else {
    /* 前と何も変わりません */
    d->kanji_status_return->length = -1;
    return 0;
  }
}

static
JishuToLower(d)
uiContext d;
{
  int len, revlen, i;
  unsigned char *p;
  jishuContext jc = (jishuContext)d->modec;

  if (jc->kc == JISHU_ZEN_ALPHA || jc->kc == JISHU_HAN_ALPHA) {
    jc->jishu_case = 0;
    jishuHenkan(d);
    return 0;
  }
  else {
    /* 前と何も変わりません */
    d->kanji_status_return->length = -1;
    return 0;
  }
}

static
JishuHiragana(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;

  jc->kc = JISHU_HIRA;
  jishuHenkan(d); /* 字種を変換 */
  return 0;
}

static
JishuKatakana(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;

  jc->kc = JISHU_ZEN_KATA;
  jishuHenkan(d); /* 字種を変換 */
  return 0;
}

static
JishuRomaji(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;

  if (jc->inhibition == INHIBIT_ALPHA) {
    return NothingChangedWithBeep(d);
  }
  jc->kc = JISHU_ZEN_ALPHA;
  jishuHenkan(d); /* 字種を変換 */
  return 0;
}

static
YomiJishu(d, fn) /* 読みモードから直接字種モードへ */
uiContext d;
int (*fn)();
{
  yomiContext yc = (yomiContext)d->modec;
  jishuContext jc;

  if (yc->henkanInhibition & IROHA_YOMI_INHIBIT_JISHU) {
    return NothingChangedWithBeep(d);
  }
  jc = pushJishuMode(d, yc, RomajiFlushYomi(d, yc->kana_buffer, ROMEBUFSIZE),
		     NO_CALLBACK, exitJishu, quitJishu, NO_CALLBACK);
  if (jc == (jishuContext)0) {
    return NoMoreMemory();
  }
  jc->minorMode = IROHA_MODE_JishuMode;
  currentModeInfo(d);
  return fn(d);
}

YomiHiraganaJishu(d) /* 読みモードから字種モードのひらがなへ */
uiContext d;
{
  return YomiJishu(d, JishuHiragana);
}

YomiKatakanaJishu(d) /* 読みモードから字種モードのひらがなへ */
uiContext d;
{
  return YomiJishu(d, JishuKatakana);
}

YomiRomajiJishu(d) /* 読みモードから字種モードのひらがなへ */
uiContext d;
{
  int res;

  res = YomiJishu(d, JishuRomaji);
  if (d->kanji_status_return->length < 0) {
    return JishuQuit(d);
  }
  return res;
}

static
_jishuHenkanExit(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  d->nbytes = retval;
  d->status = EXIT_CALLBACK;
  popJishuMode(d);
  currentModeInfo(d);
  return retval;
}

static
_jishuHenkanQuit(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  jishuHenkan(d);
  currentModeInfo(d);
  return 0;
}

/*
 * かな漢字変換を行い(変換キーが初めて押された)、TanKouhoModeに移行する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
JishuKanjiHenkan(d)
uiContext	d;
{
  jishuContext jc = (jishuContext)d->modec;
  yomiContext yc = jc->ycx;
  henkanContext hc, pushHenkanMode();
  int len;
  extern kouho_threshold;

#ifdef MEASURE_TIME
  struct tms timebuf;
  long   currenttime, times();

  currenttime = times(&timebuf);
#endif

  if (yc->henkanInhibition & IROHA_YOMI_INHIBIT_HENKAN) {
    return NothingChangedWithBeep(d);
  }

  len = jishuHenkan(d);
  d->nbytes = strlen(d->genbuf);
  if (d->nbytes == len) {
    len = 0;
  }
  if (yc->henkanCallback) {
    return yc->henkanCallback(d, IROHA_FN_Henkan, d->genbuf, d->nbytes);
  }

  /* TanKouhoMode(単候補表示モード)に移行する */
  hc = pushHenkanMode(d, d->genbuf, d->nbytes, yc,
		      0, _jishuHenkanExit, _jishuHenkanQuit, 0);
  if (hc == 0) {
    return -1;
  }
  hc->minorMode = IROHA_MODE_TankouhoMode;
  hc->kouhoCount = 1;

  if (henkan(d, len) < 0) {
    popHenkanMode(d);
    popCallback(d);
    return -1;
  }

  if (kouho_threshold > 0 && hc->kouhoCount >= kouho_threshold) {
    return tanKouhoIchiran(d, 0);
  }

  currentModeInfo(d);

#ifdef MEASURE_TIME
  hc->proctime = times(&timebuf);
  hc->proctime -= currenttime;
#endif

  return 0;
}

#include "jishumap.c"
