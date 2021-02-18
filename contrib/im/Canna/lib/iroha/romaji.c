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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/romaji.c,v 3.9 91/09/05 20:32:11 kon Exp $";
     
#include "iroha.h"
#include <ctype.h>
#ifdef MEASURE_TIME
#include <sys/types.h>
#include <sys/times.h>
#endif

static int KanaYomiInsert(), clearYomiContext();
extern int forceKana;
extern InhibitHankakuKana;

/*
 * int d->rStartp;     ro shu c|h    shi f   ローマ字 スタート インデックス
 * int d->rEndp;       ro shu ch     shi f|  ローマ字 バッファ インデックス
 * int d->rCurs;       ro shu ch|    shi f   ローマ字 エンド   インデックス
 * int d->rAttr[1024]; 10 100 11     100 1   ローマ字かな変換先頭フラグバッファ
 * int d->kEndp;       ろ し  ch  ゅ し  f|  かな     バッファ インデックス
 * int d->kRStartp;    ろ し  c|h ゅ し  f   カーソル スタート インデックス
 * int d->kCurs;      ろ し  ch| ゅ し  f   カーソル エンド   インデックス
 * int d->kAttr[1024]; 11 11  00  11 11  0   カナ変換したフラグバッファ
 * int d->nrf;                1              ローマ字変換しませんフラグ
 */

/*
 * フラグやポインタの動き
 *
 *           ひゃく           hyaku
 *  先       100010           10010
 *  変       111111
 *  禁       000000
 * rStartp                         1
 * rCurs                           1
 * rEndp                           1
 * kRstartp        1
 * kCurs           1
 * kEndp           1
 *
 * ←
 *           ひゃく           hyaku
 *  先       100010           10010
 *  変       111111
 *  禁       000000
 * rStartp                       1
 * rCurs                         1
 * rEndp                           1
 * kRstartp      1
 * kCurs         1
 * kEndp           1
 *
 * ←
 *           ひゃく           hyaku
 *  先       100010           10010
 *  変       111111
 *  禁       000000
 * rStartp                       1
 * rCurs                         1
 * rEndp                           1
 * kRstartp    1
 * kCurs       1
 * kEndp           1
 *
 * ←
 *           ひゃく           hyaku
 *  先       100010           10010
 *  変       111111
 *  禁       000000
 * rStartp                    1
 * rCurs                      1
 * rEndp                           1
 * kRstartp  1
 * kCurs     1
 * kEndp           1
 *
 * →
 *           ひゃく           hyaku
 *  先       100010           10010
 *  変       111111
 *  禁       000000
 * rStartp                       1
 * rCurs                         1
 * rEndp                           1
 * kRstartp    1
 * kCurs       1
 * kEndp           1
 *
 * 'k'
 *           ひkゃく           hyakku
 *  先       1010010           100110
 *  変       1101111
 *  禁       0010000
 * rStartp                        1
 * rCurs                           1
 * rEndp                             1
 * kRstartp    1
 * kCurs        1
 * kEndp            1
 *
 * 'i'
 *           ひきゃく           hyakiku
 *  先       10100010           1001010
 *  変       11111111
 *  禁       00110000
 * rStartp                           1
 * rCurs                             1
 * rEndp                               1
 * kRstartp      1
 * kCurs         1
 * kEndp             1
 */

#define  ROMAJILIMIT   250

#define  SENTOU        1
#define  HENKANSUMI    2
#define  WARIKOMIMOJI  4
#define  SHIRIKIRE     8
#define  ATAMAKIRE    16

#define  doubleByteP(x) ((x) & 0x80)

#ifndef CALLBACK
#define kanaReplace(where, insert, insertlen, mask) \
kanaRepl(d, where, insert, insertlen, mask)

static
kanaRepl(d, where, insert, insertlen, mask)
uiContext d;
int where, insertlen, mask;
char *insert;
{
  yomiContext yc = (yomiContext)d->modec;

  generalReplace(yc->kana_buffer, yc->kAttr, &yc->kRStartp,
		 &yc->kCurs, &yc->kEndp,
		 where, insert, insertlen, mask);
}
#else /* CALLBACK */
#define kanaReplace(where, insert, insertlen, mask) \
kanaRepl(d, where, insert, insertlen, mask)

static
kanaRepl(d, where, insert, insertlen, mask)
uiContext d;
int where, insertlen, mask;
char *insert;
{
  char buf[256];
  yomiContext yc = (yomiContext)d->modec;

  strncpy(buf, insert, insertlen);
  buf[insertlen] = '\0';
  printf("かーそるの%s %d バイトを \"%s\" に変えました。\n",
	 where > 0 ? "右" : "左",  where > 0 ? where : -where, buf);

  generalReplace(yc->kana_buffer, yc->kAttr, &yc->kRStartp, 
		 &yc->kCurs, &yc->kEndp,
		 where, insert, insertlen, mask);
}
#endif /* CALLBACK */

#define  romajiReplace(where, insert, insertlen, mask) \
romajiRepl(d, where, insert, insertlen, mask)

static
romajiRepl(d, where, insert, insertlen, mask)
uiContext d;
int where, insertlen, mask;
char *insert;
{
  yomiContext yc = (yomiContext)d->modec;

  generalReplace(yc->romaji_buffer, yc->rAttr,
		 &yc->rStartp, &yc->rCurs, &yc->rEndp,
		 where, insert, insertlen, mask);
}

static int ichitaroMove = 0;
static int ReverseOnlyCursor = 1;

void
setIchitaroMove(x)
int x;
{
  ichitaroMove = x;
}

void
setReverseOnlyCursor(x)
int x;
{
  ReverseOnlyCursor = x;
}

/*
  makeYomiReturnStruct -- 読みをアプリケーションに返す時の構造体を作る関数

  makeYomiReturnStruct は kana_buffer を調べて適当な値を組み立てる。そ
  の時にリバースの領域も設定するが、リバースをどのくらいするかは、
  ReverseOnlyCursorという変数を見て決定する。

  */

void
makeYomiReturnStruct(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  if (ReverseOnlyCursor) {
    if (yc->kCurs == yc->kEndp) {
      d->kanji_status_return->revPos = 0;
      d->kanji_status_return->revLen = 0;
    }
    else {
      d->kanji_status_return->revPos  = yc->kCurs;
      d->kanji_status_return->revLen = 
	doubleByteP(yc->kana_buffer[yc->kCurs]) ? 2 : 1;
    }
  }
  else {
    d->kanji_status_return->revPos = 0;
    d->kanji_status_return->revLen = yc->kCurs;
  }
  d->kanji_status_return->echoStr = yc->kana_buffer;
  d->kanji_status_return->length = yc->kEndp;
}

extern ckverbose;

RomkanaInit(romajifile)
char *romajifile;
{
  extern char *RomkanaTable;

  /* ローマ字かな変換辞書のオープン */
  if (RomkanaTable) {
    romajidic = RkOpenRoma(RomkanaTable);

#ifdef DEBUG
  if (iroha_debug) {
    fprintf(stderr, "ローマ字かな変換辞書名(%s)\n", RomkanaTable);
  }
#endif /* DEBUG */

    if (ckverbose == CANNA_FULL_VERBOSE) {
      if (romajidic != NULL) { /* 辞書がオープンできた */
	printf("ローマ字かな変換辞書は \"%s\" を用います。\n", RomkanaTable);
      }
    }

    if (romajidic == NULL) { /* もし辞書がオープンできなければエラー */
      char rdic[1024], *p, *getenv();

      p = getenv("HOME");
      strcpy(rdic, p);
      strcat(rdic, "/");
      strcat(rdic, RomkanaTable);
      romajidic = RkOpenRoma(rdic);

#ifdef DEBUG
  if (iroha_debug) {
    fprintf(stderr, "上の辞書がオープンできないので(%s)\n", rdic);
  }
#endif /* DEBUG */

      if (ckverbose == CANNA_FULL_VERBOSE) {
	if (romajidic != NULL) {
	  printf("ローマ字かな変換辞書は \"%s\" を用います。\n", rdic);
	}
      }

      if (romajidic == NULL) { /* これもオープンできない */
	strcpy(rdic, LIBDIR);
	strcat(rdic, "/dic/");
	strcat(rdic, RomkanaTable);
	romajidic = RkOpenRoma(rdic);

#ifdef DEBUG
  if (iroha_debug) {
    fprintf(stderr, "上の辞書がオープンできないので(%s)\n", rdic);
  }
#endif /* DEBUG */

	if (ckverbose) {
	  if (romajidic != NULL) {
	    if (ckverbose == CANNA_FULL_VERBOSE) {
	      printf("ローマ字かな変換辞書は \"%s\" を用います。\n", rdic);
	    }
	  }
	  else {
	    printf("ローマ字かな変換辞書 \"%s\" がオープンできません。\n",
		   RomkanaTable);
	  }
	}

	if (romajidic == NULL) { /* 全部オープンできない */
	  sprintf(romajifile, 
		  "ローマ字かな変換テーブル(%s)がオープンできません。",
		  RomkanaTable);
	  necKanjiError = romajifile;
	  return -1;
	}
      }
    }
  }
  else {
    strcpy(romajifile, LIBDIR);
    strcat(romajifile, "/dic/default.rdic");
    romajidic = RkOpenRoma(romajifile);

    if (ckverbose) {
      if (romajidic != NULL) { /* 辞書がオープンできた */
	if (ckverbose == CANNA_FULL_VERBOSE)
	  printf("ローマ字かな変換辞書は \"%s\" を用います。\n", romajifile);
      }
      else {
	printf("ローマ字かな変換辞書 \"%s\" がオープンできません。\n",
	       romajifile);
      }
    }

    if (romajidic == NULL) { /* もし辞書がオープンできなければエラー */
      sprintf(romajifile, 
	      "システムのローマ字かな変換テーブルがオープンできません。");
      necKanjiError = romajifile;
      return -1;
    }
  }
  return 0;
}

/* ローマ字かな変換辞書のクローズ */

RomkanaFin()
{
  extern char *RomkanaTable;

  /* ローマ字かな変換辞書のクローズ */
  if (romajidic != NULL) {
    RkCloseRoma(romajidic);
  }
  if (RomkanaTable) {
    free(RomkanaTable);
  }
}

/* cfunc yomiContext

  yomiContext 構造体を一つ作り返す。

 */

yomiContext
newYomiContext(buf, bufsize, allowedc, chmodinhibit,
	       quitTiming, hinhibit, base, hcb)
     char *buf;
     int bufsize;
     char allowedc, chmodinhibit, quitTiming, hinhibit, base;
     int (*hcb)();
{
  yomiContext ycxt;

  ycxt = (yomiContext)malloc(sizeof(yomiContextRec));
  if (ycxt) {
    ycxt->id = YOMI_CONTEXT;
    clearYomiContext(ycxt);
    ycxt->allowedChars = allowedc;
    ycxt->generalFlags = chmodinhibit ? IROHA_YOMI_CHGMODE_INHIBITTED : 0;
    ycxt->generalFlags |= quitTiming ? IROHA_YOMI_END_IF_KAKUTEI : 0;
    ycxt->henkanInhibition = hinhibit;
    ycxt->baseChar = base;
    ycxt->retbufp = ycxt->retbuf = buf;
    ycxt->henkanCallback = hcb;
    if ((ycxt->retbufsize = bufsize) == 0) {
      ycxt->retbufp = 0;
    }
    ycxt->next = 0;
    ycxt->prevMode = 0;
  }
  return ycxt;
}

void
freeYomiContext(yc)
yomiContext yc;
{
  free(yc);
}

/*

  GetKanjiString は漢字かな混じり文を取ってくる関数である。実際には 
  empty モードを設定するだけでリターンする。最終的な結果が buf で指定
  されたバッファに格納され exitCallback が呼び出されることによって呼び
  出し側は漢字かな混じり文字を得ることができる。

  第２引数の ycxt は通常は０を指定する。アルファベットモードから日本語
  モードへの切り替えに際してのみは uiContext の底に保存してあるコンテ
  キストを用いる。アルファベットモードと日本語モードとの切り替えはスタッ
  ク上に積み込まれたモードの push/pop 操作ではなく、スワップ上のモード
  の一番上の要素の入れ替えになる。

  ３つの Callback のうち、exitCallback はひょっとしたら使われないで、
  everyTimeCallback と quitCallback しか用いないかも知れない。

 */

void popCallback();

yomiContext
GetKanjiString(d, buf, bufsize, allowedc, chmodinhibit,
	       quitTiming, hinhibit, base,
	       henkanCallback,
	       everyTimeCallback, exitCallback, quitCallback)
     uiContext d;
     char *buf;
     int bufsize;
     char allowedc, chmodinhibit, quitTiming, hinhibit, base;
     int (*henkanCallback)();
     int (*everyTimeCallback)(), (*exitCallback)(), (*quitCallback)();
{
  extern KanjiModeRec empty_mode;
  struct callback *pushCallback();
  yomiContext yc;

  if ((pushCallback(d, d->modec, everyTimeCallback, exitCallback, quitCallback,
		    (int (*)()) NULL)) == (struct callback *)0) {
    return (yomiContext)0;
  }

  yc = newYomiContext(buf, bufsize, allowedc, chmodinhibit,
		      quitTiming, hinhibit, base, henkanCallback);
  if (yc == (yomiContext)0) {
    popCallback(d);
    return (yomiContext)0;
  }
  yc->majorMode = d->majorMode;
  yc->minorMode = IROHA_MODE_HenkanMode;
  yc->next = d->modec;
  d->modec = (mode_context)yc;
  /* 前のモードの保存 */
  yc->prevMode = d->current_mode;
  /* モード変更 */
  d->current_mode = &empty_mode;
  return yc;
}

/* cfuncdef

   popYomiMode -- 読みモードをポップアップする。

 */

void
popYomiMode(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  d->modec = yc->next;
  d->current_mode = yc->prevMode;
  freeYomiContext(yc);
}

/* cfuncdef

  checkIfYomiExit -- 読みモードが終了かどうかを調べて値を返すフィルタ

  このフィルタは読みモードの各関数で値を返そうとする時に呼ぶ。読みモー
  ドでの処理が終了するところであれば、読みモードを終了し、uiContext に
  プッシュされていたローカルデータやモード構造体がポップされる。

  ローカルデータに exitCallback が定義されていなければいかなる場合にも
  構造体のポップアップは行われない。

  今のところ、読みモードの終了は次のような場合が考えられる。

  (1) C-m が確定読みの最後の文字として返された時。(変換許可の時)

  (2) 確定文字列が存在する場合。(変換禁止の時)

  quit で読みモードを終了する時は?他の関数?を呼ぶ。

 */

static
checkIfYomiExit(d, retval)
uiContext d;
int retval;
{
  yomiContext yc = (yomiContext)d->modec;

  if (retval <= 0) {
    /* 確定文字列がないかエラーの場合 ≡ exit ではない */
    return retval;
  }
  if (yc->retbufp && yc->retbufsize - (yc->retbufp - yc->retbuf) > retval) {
    /* 文字列格納バッファがあって、確定した文字列よりもあまっている領
       域が長いのであれば格納バッファに確定した文字列をコピーする */
    strncpy(yc->retbufp, d->buffer_return, retval);
    yc->retbufp[retval] = '\0';
    yc->retbufp += retval;
  }
  if (yc->generalFlags & IROHA_YOMI_END_IF_KAKUTEI
      || d->buffer_return[retval - 1] == '\n') {
    /* 変換が禁止されているとしたら exit */
    /* そうでない場合は、\n が入っていたら exit */
    d->status = EXIT_CALLBACK;
    if (d->cb->func[EXIT_CALLBACK] == NO_CALLBACK) {
      /* コールバックがない場合

	 こんなチェックを親切に行うのは、読みモードが非常に基本的なモード
	 であり、完全に抜けるときにわざわざポップアップしてもすぐにプッシュ
	 する場合が多いと考えられて処理が無駄だからである。

       */
    }
    else {
      d->status = EXIT_CALLBACK;
      popYomiMode(d);
    }
  }
  return retval;
}

static
checkIfYomiQuit(d, retval)
uiContext d;
int retval;
{
#ifdef QUIT_IN_YOMI /* コメントアウトする目的の ifdef */
  yomiContext yc = (yomiContext)d->modec;

  if (d->cb->func[QUIT_CALLBACK] == NO_CALLBACK) {
    /* コールバックがない場合

       こんなチェックを親切に行うのは、読みモードが非常に基本的なモード
       であり、完全に抜けるときにわざわざポップアップしてもすぐにプッシュ
       する場合が多いと考えられて処理が無駄だからである。

     */
  }
  else {
    d->status = QUIT_CALLBACK;
    popYomiMode(d);
  }
#endif /* QUIT_IN_YOMI */
  return retval;
}


/* tabledef

 charKind -- キャラクタの種類のテーブル

 0x20 から 0x7f までのキャラクタの種類を表すテーブルである。

 3: 数字
 2: １６進数として用いられる英字
 1: それ以外の英字
 0: その他

 となる。

 */

static char charKind[] = {
/*sp !  "  #  $  %  &  '  (  )  *  +  ,  -  .  / */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ? */
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1,
/*@  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O */
  1, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2,
/*P  Q  R  S  T  U  V  W  X  Y  X  [  \  ]  ^  _ */
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
/*`  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o */
  1, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2,
/*p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~  DEL */
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
};

static
getCurrentJishu(yc)
yomiContext yc;
{
  switch (yc->baseChar) {
  case JISHU_ZEN_KATA:
    return RK_SOKON | RK_KFER;
  case JISHU_HAN_KATA:
    return RK_SOKON | RK_HFER;
  default:
    return RK_SOKON | RK_NFER;
  }
}

/*
  YomiInsert -- ローマ字を１文字挿入する関数

  */

YomiInsert(d)
uiContext d;
{
  char roma_char[1024]; /* 変換されるローマ字 */
  char kana_char[1024]; /* 変換したかな */
  char xxxx[1024];
  int i, n, m, idou;
  int henkanflag, jishuflag;
  extern BreakIntoRoman;
  yomiContext yc = (yomiContext)d->modec;
  
  if (yc->allowedChars == IROHA_NOTHING_ALLOWED/* どのキーも受付けない */
      || yc->rEndp > ROMAJILIMIT) { /* または、制限以上に入れようとした */
    return NothingChangedWithBeep(d);
  }

  if (d->ch > 127) {
    if (yc->allowedChars == IROHA_NOTHING_RESTRICTED) {
      return KanaYomiInsert(d); /* callback のチェックは KanaYomiInsert で! */
    }
    else {
      return NothingChangedWithBeep(d);
    }
  }

  if (d->ch < 0x20 && yc->allowedChars != IROHA_NOTHING_RESTRICTED
      || charKind[d->ch - 0x20] < yc->allowedChars) {
    /* 0x20 はコントロールキャラクタの分 */
    return NothingChangedWithBeep(d);
  }

  if (yc->allowedChars != IROHA_NOTHING_RESTRICTED ||
    /* allowed all 以外ではローマ字かな変換を行わない */
      yc->baseChar == JISHU_ZEN_ALPHA || yc->baseChar == JISHU_HAN_ALPHA) {
    romajiReplace(0, &d->ch, 1, 0);

    if (yc->baseChar == JISHU_HAN_KATA || yc->baseChar == JISHU_HAN_ALPHA) {
      kanaReplace(0, &d->ch, 1, HENKANSUMI);
    }
    else {
      unsigned char romanBuf[4]; /* ２バイトで十分だと思うけどね */
      int len;

      len = RkCvtZen(romanBuf, 4, &d->ch, 1);
      kanaReplace(0, romanBuf, len, HENKANSUMI);
    }
    yc->rStartp = yc->rCurs;
    yc->kRStartp = yc->kCurs;
  }
  else { /* ローマ字カナ変換する場合 */
    if (BreakIntoRoman)
      yc->generalFlags |= IROHA_YOMI_BREAK_ROMAN;

    /* まずカーソル部分にローマ字を１文字入れる */

    romajiReplace(0, &d->ch, 1, 0);

    /* 直前に未変換文字列がないかどうか確認 */

    if (yc->kCurs == yc->kRStartp) {
      do { 
	yc->kRStartp--;
	yc->rStartp--;
      } while ( yc->kRStartp >= 0 
	       && !(yc->kAttr[yc->kRStartp] & HENKANSUMI) );
      yc->kRStartp++;
      yc->rStartp++;
    }
    
    jishuflag = getCurrentJishu(yc);

    /* 未変換ローマ文字列のかな変換 */
    if (romajidic != NULL) {
      n = RkMapRoma(romajidic, kana_char, 1024, 
		    yc->romaji_buffer + yc->rStartp, yc->rCurs - yc->rStartp,
		    jishuflag, &m);
    }
    else {
      m = -(n = (yc->rCurs - yc->rStartp) ? 1 : 0);
      strncpy(kana_char, yc->romaji_buffer + yc->rStartp, n);
    }
    
    /* ローマ字のうち n 文字分カナに変換された */
    
    while (n > 0) {
      if (m < 0) {
	henkanflag = 0;
	m = -m; /* |m| は変換された文字列の長さを示す */
      }
      else {
	henkanflag = HENKANSUMI;
      }
      
      for (i = 1 ; i < n ; i++) {
	yc->rAttr[yc->rStartp + i] &= ~SENTOU;      /* 先頭以外＝０ */
      }
      yc->rStartp += n;      /* n 文字分後ろへ */
      
      /* ローマ字かな変換の結果をカナバッファに入れる。 */
      
      kanaReplace(yc->kRStartp - yc->kCurs/* 注: 負になります */, 
		  kana_char, m,
		  henkanflag);
      yc->kRStartp = yc->kCurs;
      
      /* 未変換ローマ文字列のかな変換 */
      if (romajidic != NULL) {
	n = RkMapRoma(romajidic, kana_char, 1024, 
		      yc->romaji_buffer + yc->rStartp, yc->rCurs - yc->rStartp,
		      jishuflag, &m);
      }
      else {
	m = -(n = (yc->rCurs - yc->rStartp) ? 1 : 0);
	strncpy(kana_char, yc->romaji_buffer + yc->rStartp, n);
      }
      
      /* ローマ字のうち n 文字分カナに変換された */
    }
    
    /* ローマ字かな変換に使われなかったローマ字を後ろに付け足す */
    if (yc->rCurs - yc->rStartp) {
      n = 0; /* 一応確認しておく */
      if (forceKana) { /* 余ったローマ字を無理矢理かなにする */
	if (romajidic != NULL) {
	  n = RkMapRoma(romajidic, kana_char, 1024,
			yc->romaji_buffer + yc->rStartp,
			yc->rCurs - yc->rStartp,
			jishuflag | RK_FLUSH, &m);
	}
	else {
	  m = -(n = (yc->rCurs - yc->rStartp) ? 1 : 0);
	  strncpy(kana_char, yc->romaji_buffer + yc->rStartp, n);
	}

	if (n > 0) {
	  if (m < 0) {
	    henkanflag = 0;
	    m = -m; /* |m| は変換された文字列の長さを示す */
	  }
	  else {
	    henkanflag = HENKANSUMI;
	  }
      
	  for (i = 1 ; i < n ; i++) {
	    yc->rAttr[yc->rStartp + i] &= ~SENTOU;      /* 先頭以外＝０ */
	  }
	  yc->rStartp += n;      /* n 文字分後ろへ */

	  kanaReplace(yc->kRStartp - yc->kCurs/* 注: 負になります */, 
		      kana_char, m, henkanflag);
	  yc->kRStartp = yc->kCurs;
	}
      }
      if (yc->rCurs - yc->rStartp) {
	kanaReplace(yc->kRStartp - yc->kCurs/* 注: 負になります */, 
		    yc->romaji_buffer + yc->rStartp,
		    yc->rCurs - yc->rStartp,
		    SENTOU);
      }
      if (n > 0) { /* forceKana でかながあった場合ここを通る */
	yc->rStartp -= n;
	yc->kRStartp -= m;
      }
    }
  }
    
#ifdef DEBUG
  if (iroha_debug) {
    fprintf(stderr, "kCurs(%d), kEndp(%d)\n", yc->kCurs, yc->kEndp);
    /*
      fprintf(stderr, "romab(%s)", yc->romaji_buffer);
      fprintf(stderr, "romac(%s)\n", roma_char);
      fprintf(stderr, "kanab(%s)", yc->kana_buffer);
      fprintf(stderr, "kanac(%s)\n", kana_char);
      */
    fprintf(stderr, "length(%d)\n", d->kanji_status_return->length);
    fprintf(stderr, "revPos(%d)", d->kanji_status_return->revPos);
    fprintf(stderr, "revLen(%d)\n", d->kanji_status_return->revLen);
  }
#endif

  makeYomiReturnStruct(d);
  return 0;
}

#define KANAYOMIINSERT_BUFLEN 10
#define DAKUON		0xde
#define HANDAKUON	0xdf

static
KanaYomiInsert(d)
uiContext d;
{
  static unsigned char kana[] = {' ', ' ', 0x8e, ' '}, *kanap;
  unsigned char buf1[KANAYOMIINSERT_BUFLEN], buf2[KANAYOMIINSERT_BUFLEN];
  int len, replacelen, dakuten = 0;
  yomiContext yc = (yomiContext)d->modec;

  yc->generalFlags &= ~IROHA_YOMI_BREAK_ROMAN;
  kanap = kana + 2; len = 2; replacelen = 0;
  kana[3] = d->ch;
  romajiReplace(0, kanap, len, 0);
  yc->rStartp = yc->rCurs;
  if (d->ch == DAKUON || d->ch == HANDAKUON) { /* 濁音記号だったりしたら */
    if (yc->rCurs >= 4) { /* 前に文字がある場合には次の処理をする */
      kana[0] = yc->romaji_buffer[yc->rCurs - 4];
      kana[1] = yc->romaji_buffer[yc->rCurs - 3];
      if ((kana[0] & 0x80) && (kana[1] & 0x80)
	  && (kana[1] != DAKUON) && (kana[1] != HANDAKUON)
	  ) { /* 前の文字がカナで、かつ濁音類ではない時に次をする */
	kanap = kana; len = 4; replacelen = -2;
	dakuten = 1;
      }
    }
  }
#ifdef DEBUG
  if (iroha_debug) {
    char aho[200];

    strncpy(aho, kanap, len);
    aho[len] = 0;
    fprintf(stderr, "変換前(%s)", aho);
  }
#endif
  switch (yc->baseChar) {
  case JISHU_HIRA:
  case JISHU_ZEN_KATA:
  case JISHU_ZEN_ALPHA:
    len = RkCvtZen(buf1, KANAYOMIINSERT_BUFLEN, kanap, len);
#ifdef DEBUG
    if (iroha_debug) {
      char aho[200];

      strncpy(aho, buf1, len);
      aho[len] = 0;
      fprintf(stderr, "→中(%s)", aho);
    }
#endif
    if (yc->baseChar == JISHU_HIRA) {
      len = RkCvtHira(buf2, KANAYOMIINSERT_BUFLEN, buf1, len);
#ifdef DEBUG
      if (iroha_debug) {
	char aho[200];

	strncpy(aho, buf2, len);
	aho[len] = 0;
	fprintf(stderr, "→後(%s)\n", aho);
      }
#endif
    }
    else {
      len = RkCvtNone(buf2, KANAYOMIINSERT_BUFLEN, buf1, len);
    }
    break;
  default:
    len = RkCvtNone(buf2,  KANAYOMIINSERT_BUFLEN, kanap, len);
    break;
  }
  if (dakuten && len == 2) { /* 濁点を先頭でなくす */
    yc->rAttr[yc->rCurs - 2] &= ~SENTOU;
  }
  kanaReplace(replacelen, buf2, len, HENKANSUMI);
  if (dakuten && len > 2) { 
    /* "あ゜" のような文字列が一度に返ってしまったときに、"゜"
       に先頭印を付ける処理をする。ローマ字バッファに入っている濁音
       にも先頭印が付いているので、これで矛盾なく処理が行えるようになる。
       ローマ字バッファの濁音の先頭印を取り去ることは良くない。というのは、
       かなバッファにて濁音だけを削除したあとの処理がうまくいかなくなるから
       である。 */
    yc->kAttr[yc->kCurs - 2] |= SENTOU;
  }
  yc->kRStartp = yc->kCurs;

  makeYomiReturnStruct(d);
  return 0;
}

#undef KANAYOMIINSERT_BUFLEN

static
moveStrings(str, attr, start, end, distance)
char *str, *attr;
int  start, end, distance;
{     
  int i;

  if (distance == 0)
    ;
  else if (distance > 0) { /* 後ろにずれれば */
    for (i = end ; start <= i ; i--) { /* 後ろからずらす */
      str[i + distance]  = str[i];
      attr[i + distance] = attr[i];
    }
  }
  else /* if (distance < 0) */ { /* 前にずれれば */
    for (i = start ; i <= end ; i++) {     /* 前からずらす */
      str[i + distance]  = str[i];
      attr[i + distance] = attr[i];
    }
  }
}

static
howFarToGoBackward(d, chars)
uiContext d;
int *chars;
{
  yomiContext yc = (yomiContext)d->modec;

  *chars = 0;
  if (yc->kCurs == 0)
    return 0;
  else if (ichitaroMove) {
    int i;
    
    for (i = yc->kCurs - 1 ; i > 0 ; i--) {
      if (yc->kana_buffer[i] & 0x80)
	i--;
      ++*chars;
      if (yc->kAttr[i] & SENTOU)
	break;
    }
    return (yc->kCurs - i);
  }
  else {
    *chars = 1;
    return doubleByteP(yc->kana_buffer[yc->kCurs - 1]) ? 2 : 1;
  }
}

static
howFarToGoForward(d, chars)
uiContext d;
int *chars;
{
  yomiContext yc = (yomiContext)d->modec;

  *chars = 0;
  if (yc->kCurs == yc->kEndp)
    return 0;
  else if (ichitaroMove) {
    int i;
    
    i = yc->kCurs;
    i += (yc->kana_buffer[i] & 0x80) ? 2 : 1;
    *chars = 1;
    for (; i < yc->kEndp ; i++) {
      if (yc->kAttr[i] & SENTOU)
	break;
      if (yc->kana_buffer[i] & 0x80)
	i++;
      ++*chars;
    }
    return (i - yc->kCurs);
  }
  else {
    *chars = 1;
    return doubleByteP(yc->kana_buffer[yc->kCurs]) ? 2 : 1;
  }
}

static
YomiBackward(d) /* カーソルの左移動 */
uiContext d;
{
  int i, howManyMove, howManyChars;
  yomiContext yc = (yomiContext)d->modec;

  if (yc->kCurs != yc->kRStartp) {
    RomajiFlushYomi(d, NULL, 0);
  }

  howManyMove = howFarToGoBackward(d, &howManyChars);

  if (howManyMove == 0) { /* これが０ということは左端だということだろう */
    extern int CursorWrap;

    if (CursorWrap) {
      yc->kCurs = yc->kRStartp = yc->kEndp;
      yc->rCurs = yc->rStartp = yc->rEndp;
#ifdef CALLBACK
      printf("一番後に移動する\n");
#endif /* CALLBACK */
    }
    else {
      d->kanji_status_return->length = -1;
      return 0;
    }
  }
  else {  /* 文字列(yc->kana_buffer)の左端でなければ */
    yc->kCurs -= howManyMove;

#ifdef DEBUG
  if (iroha_debug) {
    fprintf(stderr, "backward(%d), kCurs(%d)\n", howManyMove, yc->kCurs);
  }
#endif /* DEBUG */

    if (yc->kCurs < yc->kRStartp)
      yc->kRStartp = yc->kCurs;   /* 未確定ローマ字カーソルもずらす */


    /* かなのポインタが変換されたときの途中のデータでない場合
       (つまり変換の時に先頭のデータだった場合)にはローマ字の
       カーソルもずらす */

    if (yc->kAttr[yc->kCurs] & SENTOU) {
      while ( yc->rCurs > 0 && !(yc->rAttr[--yc->rCurs] & SENTOU) )
	;
      if (yc->rCurs < yc->rStartp)
	yc->rStartp = yc->rCurs;
    }

#ifdef CALLBACK
    printf("%d 文字左に移動\n", howManyChars);
#endif /* CALLBACK */
  }
  makeYomiReturnStruct(d);

  return 0;
}

static
YomiForward(d) /* カーソルの右移動 */
uiContext d;
{
  int howManyMove, prevAttr, howManyChars;
  int i, f;
  yomiContext yc = (yomiContext)d->modec;

  if (yc->kCurs != yc->kRStartp) {
    RomajiFlushYomi(d, NULL, 0);
  }

  howManyMove = howFarToGoForward(d, &howManyChars);
  if (howManyMove == 0) { /* これが０だということは右端ということなので */
    extern int CursorWrap;

    if (CursorWrap) {
      yc->kRStartp = yc->kCurs = 0;
      yc->rStartp = yc->rCurs = 0;
#ifdef CALLBACK
      printf("左端に移動\n");
#endif /* CALLBACK */
    }
    else {
      d->kanji_status_return->length = -1;
      return 0;
    }
  }
  else { /* 右に動かせる場合 */
    if (yc->kAttr[yc->kCurs] & SENTOU) { /* ローマ字かな変換時先頭だった */
      while ( !yc->rAttr[++yc->rCurs] )
	; /* 次の先頭までずらす */
      yc->rStartp = yc->rCurs;
    }

#ifdef CALLBACK
    printf("%d 文字右に移動する\n", howManyChars);
#endif /* CALLBACK */

    yc->kCurs += howManyMove;   /* 画面の入力位置 カーソルを右にずらす */
    yc->kRStartp = yc->kCurs;
  }
  makeYomiReturnStruct(d);
  return 0;
}

static
YomiBeginningOfLine(d) /* カーソルの左端移動 */
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  if (yc->kCurs != yc->kRStartp) {
    RomajiFlushYomi(d, NULL, 0);
  }
  yc->kRStartp = yc-> kCurs = 0;
  yc->rStartp = yc-> rCurs = 0;
#ifdef CALLBACK
  printf("左端移動\n");
#endif /* CALLBACK */
  makeYomiReturnStruct(d);
  return 0;
}

static
YomiEndOfLine(d) /* カーソルの右端移動 */
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  if (yc->kCurs != yc->kRStartp) {
    RomajiFlushYomi(d, NULL, 0);
  }
  yc->kRStartp = yc-> kCurs = yc->kEndp;
  yc->rStartp = yc-> rCurs = yc->rEndp;
#ifdef CALLBACK
  printf("右端移動\n");
#endif /* CALLBACK */
  makeYomiReturnStruct(d);
  return 0;
}

/* RomajiFlushYomi(d, buffer, bufsize) ユーティリティ関数
 *
 * この関数は、(uiContext)d に蓄えられている読みの情報 
 * (yc->romaji_buffer と yc->kana_buffer)を用いて、buffer にその読みをフ
 * ラッシュした結果を返す関数である。フラッシュした結果の文字列の長さ
 * はこの関数の返り値として返される。
 *
 * buffer として NULL が指定された時は、バッファに対する格納は行わない
 *
 * 【作用】   
 *
 *    読みを確定する
 *
 * 【引数】
 *
 *    d  (uiContext)  カナ漢字変換構造体
 *    buffer (char *)    読みを返すためのバッファ (NULL 可)
 *
 * 【戻り値】
 *
 *    buffer に格納した文字列の長さ(バイト長)
 *
 * 【副作用】
 *
 */

RomajiFlushYomi(d, b, bsize)
uiContext d;
char *b;
int bsize;
{
  int n, m, ret, idou, henkanflag;
  char kana_char[1024];
  yomiContext yc = (yomiContext)d->modec;

  yc->generalFlags &= ~IROHA_YOMI_BREAK_ROMAN;

  while (yc->rStartp < yc->rCurs) {
    if (romajidic != NULL) {
      n = RkMapRoma(romajidic, kana_char, 1024, 
		    yc->romaji_buffer + yc->rStartp, yc->rCurs - yc->rStartp,
		    getCurrentJishu(yc) | RK_FLUSH, &m); 
                   /* ローマ字をカナに変換し切っていない部分が
                      あったら、それを変換してやれ */
    }
    else {
      m = -(n = (yc->rCurs - yc->rStartp) ? 1 : 0);
      strncpy(kana_char, yc->romaji_buffer + yc->rStartp, n);
    }

    if (n < 0) {
      necKanjiError = "RkMapRoma returns -1";
      return -1;
    }

    yc->rStartp += n;
  
    if (m < 0) {
      henkanflag = 0;
      m = -m;
    }
    else {
      henkanflag = HENKANSUMI;
    }

    kanaReplace(yc->kRStartp - yc->kCurs/* 注: 負になります */,
		/* カーソルの回りの○○文字を .. */
		kana_char, m,
		/* kana_char m文字に置き換えて .. */
		henkanflag);
    yc->kRStartp += m;
  }

  ret    = yc->kEndp;    /* その結果がこの関数の返り値になる */
  if (b) {
    if (bsize > ret) {
      strncpy(b, yc->kana_buffer, ret);
      b[ret] = '\0';
    }
    else {
      strncpy(b, yc->kana_buffer, bsize);
      ret = bsize;
    }
  }
  return ret;
}

#ifdef YOMIKAKUTEIORRETURN
/* 新しいロジックとあんまり合わないような気がしたのでとりあえずはずす。
   1990.9.21 kon */

static
YomiKakuteiOrReturn(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  if (d->ch != '\n' && d->ch != '\r') {
    return YomiKakutei(d);
  }
  else {
    int i;

    RomajiFlushYomi(d, NULL, 0);
    for (i = 0 ; i < yc->kEndp ; i++) {
      if ( !(yc->kAttr[i] & HENKANSUMI) ) {
        int ret;

        if (yc->rEndp < d->bytes_buffer) {
          strncpy(d->buffer_return, yc->romaji_buffer, yc->rEndp);
          d->buffer_return[yc->rEndp] = d->ch;
          ret = yc->rEndp + 1;
        }
        else {
          strncpy(d->buffer_return, yc->romaji_buffer, d->bytes_buffer);
          ret = d->bytes_buffer;
        }
        RomajiClearYomi(d);
        d->kanji_status_return->length = 0;
	d->current_mode = &empty_mode;
        AlphaMode(d);
        return ret;
      }
    }
    return YomiKakutei(d);
  }
}
#endif /* YOMIKAKUTEIORRETURN */

static
YomiKakutei(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  d->kanji_status_return->length = 0;
  d->nbytes = RomajiFlushYomi(d, d->buffer_return, d->bytes_buffer);
  { /* 前の読みを一緒に返す準備をする */
    extern yomiInfoLevel;
    int n;

    n = d->nbytes;
    if (yomiInfoLevel > 0 && n < d->bytes_buffer) {
      d->kanji_status_return->info |= KanjiYomiInfo;
      d->buffer_return[n++] = '\0';
      if (d->bytes_buffer - n < yc->kEndp) {
	strncpy(d->buffer_return + n, yc->kana_buffer, d->bytes_buffer - n);
      }
      else {
	strncpy(d->buffer_return + n, yc->kana_buffer, yc->kEndp);
	n += yc->kEndp;
	d->buffer_return[n++] = '\0';

	if (yomiInfoLevel > 1) {
	  if (d->bytes_buffer - n < yc->rEndp) {
	    strncpy(d->buffer_return + n, yc->romaji_buffer, 
		    d->bytes_buffer - n);
	  }
	  else {
	    strncpy(d->buffer_return + n, yc->romaji_buffer, yc->rEndp);
	    n += yc->rEndp;
	    d->buffer_return[n++] = '\0';
	  }
	}
      }
    }
  }
  return YomiExit(d, d->nbytes);
}

YomiExit(d, retval)
uiContext d;
int retval;
{
  extern KanjiModeRec empty_mode;

  RomajiClearYomi(d);

  /* 確定してしまったら、読みがなくなるのでφモードに遷移する。 */
  d->current_mode = &empty_mode;
  d->kanji_status_return->info |= KanjiEmptyInfo;

  return checkIfYomiExit(d, retval);
}

static
clearYomiContext(yc)
yomiContext yc;
{
  yc->rStartp = 0;
  yc->rCurs = 0;
  yc->rEndp = 0;
  yc->romaji_buffer[0] = (unsigned char)NULL;
  yc->rAttr[0] = SENTOU;
  yc->kRStartp = 0;
  yc->kCurs = 0;
  yc->kEndp = 0;
  yc->kana_buffer[0] = (unsigned char)NULL;
  yc->kAttr[0] = SENTOU;
}

/* RomajiClearYomi(d) ユーティリティ関数
 *
 * この関数は、(uiContext)d に蓄えられている読みの情報 
 * をクリアする。
 *
 * 【作用】   
 *
 *    読みをクリアする。
 *
 * 【引数】
 *
 *    d  (uiContext)  カナ漢字変換構造体
 *
 * 【戻り値】
 *
 *    なし。
 *
 * 【副作用】
 *
 *    yc->rEndp = 0;
 *    yc->kEndp = 0; 等
 */

RomajiClearYomi(d)
uiContext d;
{
  clearYomiContext((yomiContext)d->modec);
#ifdef CALLBACK
  printf("プリエディットをクリア\n");
#endif /* CALLBACK */
}

/* RomajiStoreYomi(d, kana) ユーティリティ関数
 *
 * この関数は、(uiContext)d に読みの情報をストアする。
 *
 * 【作用】   
 *
 *    読みを格納する。
 *
 * 【引数】
 *
 *    d    (uiContext)  カナ漢字変換構造体
 *    kana (unsigned char *) かな文字列
 *    roma (unsigned char *) ローマ字文字列
 * 【戻り値】
 *
 *    なし。
 *
 * 【副作用】
 *
 *    yc->rEndp = strlen(kana);
 *    yc->kEndp = strlen(kana); 等
 */

RomajiStoreYomi(d, kana, roma)
uiContext d;
unsigned char *kana, *roma;
{
  int i, ylen, rlen;
  yomiContext yc = (yomiContext)d->modec; /*???ちょっと疑問あり */

  ylen = strlen(kana);

  if (roma) {
    rlen = strlen(roma);
    strcpy(yc->romaji_buffer, roma);
    yc->rStartp = rlen;
    yc->rCurs = rlen;
    yc->rEndp = rlen;
  }
  else {
    strcpy(yc->romaji_buffer, kana);
    rlen = ylen;
    yc->rStartp = rlen;
    yc->rCurs = rlen;
    yc->rEndp = rlen;
  }
  strcpy(yc->kana_buffer, kana);
  yc->kRStartp = ylen;
  yc->kCurs = ylen;
  yc->kEndp = ylen;
  for (i = 0 ; i < rlen ; i++) {
    yc->rAttr[i] = 0;
  }
  yc->rAttr[0] |= SENTOU;
  yc->rAttr[i] = SENTOU;
  for (i = 0 ; i < ylen ; i++) {
    yc->kAttr[i] = HENKANSUMI;
  }
  yc->kAttr[0] |= SENTOU;
  yc->kAttr[i] = SENTOU;
}

/*
  KanaDeletePrevious -- 色々なところから使われる。

*/

KanaDeletePrevious(d)/* カーソルの左の文字の削除 */
uiContext d;
{
  int howManyDelete, howManyChars;
  int h;
  int i;
  yomiContext yc = (yomiContext)d->modec;

  /* カーソルの左側を削除するのだが、カーソルの左側が

    (1) 日本語文字列の時。
    (2) １バイト文字列の時。
    (3) ローマ字かな変換の途中の状態であり、アルファベットになっている時、
    (4) 先頭であるとき

    などが考えられる。(要は整理されていないのでもっとありそう)
   */

  if (yc->kCurs == 0) { /* 左端のとき */
    d->kanji_status_return->length = -1;
    return 0;
  }
  howManyDelete = howFarToGoBackward(d, &howManyChars);
  if (howManyDelete > 1 && (yc->generalFlags & IROHA_YOMI_BREAK_ROMAN)) {
    yc->generalFlags &= ~IROHA_YOMI_BREAK_ROMAN;
    yc->rStartp--;
    while ( yc->rStartp > 0 && !(yc->rAttr[yc->rStartp] & SENTOU) ) {
      yc->rAttr[yc->rStartp] |= SENTOU;
      yc->rStartp--;
    }
    romajiReplace (-1, NULL, 0, 0);
    yc->kRStartp--;
    while ( yc->kRStartp > 0 && !(yc->kAttr[yc->kRStartp] & SENTOU) )
      yc->kRStartp--;
    kanaReplace(yc->kRStartp - yc->kCurs, 
		yc->romaji_buffer + yc->rStartp,
		yc->rCurs - yc->rStartp,
		SENTOU/* 未変換に戻しているわけ */);
    if (forceKana) { /* 余ったローマ字を無理矢理かなにする */
      char kana_char[1024];
      int n, m, henkanflag, jishuflag;

      jishuflag = getCurrentJishu(yc);

      if (romajidic != NULL) {
	n = RkMapRoma(romajidic, kana_char, 1024,
		      yc->romaji_buffer + yc->rStartp,
		      yc->rCurs - yc->rStartp,
		      jishuflag | RK_FLUSH, &m);
      }
      else {
	m = -(n = (yc->rCurs - yc->rStartp) ? 1 : 0);
	strncpy(kana_char, yc->romaji_buffer + yc->rStartp, n);
      }

      if (n > 0) {
	if (m < 0) {
	  henkanflag = 0;
	  m = -m; /* |m| は変換された文字列の長さを示す */
	}
	else {
	  henkanflag = HENKANSUMI;
	}
      
	for (i = 1 ; i < n ; i++) {
	  yc->rAttr[yc->rStartp + i] &= ~SENTOU;      /* 先頭以外＝０ */
	}
	yc->rStartp += n;      /* n 文字分後ろへ */

	kanaReplace(yc->kRStartp - yc->kCurs/* 注: 負になります */, 
		    kana_char, m, henkanflag);
	yc->kRStartp = yc->kCurs;
	if (yc->rCurs - yc->rStartp) {
	  kanaReplace(yc->kRStartp - yc->kCurs/* 注: 負になります */, 
		      yc->romaji_buffer + yc->rStartp,
		      yc->rCurs - yc->rStartp,
		      SENTOU);
	}
	yc->rStartp -= n;
	yc->kRStartp -= m;
      }
    }
  }
  else {
    if (yc->kAttr[yc->kCurs - howManyDelete] & SENTOU) { 
      /* ローマ字かな変換の先頭だったら */
      int n;

      /* 先頭だったらローマ字も先頭マークが立っているところまで戻す */

      for (n = 1 ; yc->rCurs > 0 && !(yc->rAttr[--yc->rCurs] & SENTOU) ; n++)
	;
      moveStrings(yc->romaji_buffer, yc->rAttr, yc->rCurs + n, yc->rEndp, -n);
      if (yc->rCurs < yc->rStartp) {
	yc->rStartp = yc->rCurs;
      }
      yc->rEndp -= n;
    }
    kanaReplace(-howManyDelete, NULL, 0, 0);
  }
}

static
YomiDeletePrevious(d)
uiContext d;
{
  extern KanjiModeRec empty_mode;
  yomiContext yc = (yomiContext)d->modec;

  KanaDeletePrevious(d);
  makeYomiReturnStruct(d);

  if (yc->kEndp == 0) {
    /* 未確定文字列が全くなくなったのなら、φモードに遷移する */
    d->current_mode = &empty_mode;
    d->kanji_status_return->info |= KanjiEmptyInfo;
  }
  return 0;
}

static
YomiDeleteNext(d)/* カーソル上の文字の削除 */
uiContext d;
{
  extern KanjiModeRec empty_mode;
  int howManyDelete, howManyChars;
  int f;
  int h;
  int i;
  yomiContext yc = (yomiContext)d->modec;

  if (yc->kCurs == yc->kEndp) {
    /* 右端だからなにもしないのでしょうねぇ */
    d->kanji_status_return->length = -1;
    return 0;
  }

  howManyDelete = howFarToGoForward(d, &howManyChars);

  if (yc->kAttr[yc->kCurs] & SENTOU) {
    int n = 1;
    while ( !(yc->rAttr[++yc->rCurs] & SENTOU) )
      n++;
    moveStrings(yc->romaji_buffer, yc->rAttr, yc->rCurs, yc->rEndp, -n);
    yc->rCurs -= n;
    yc->rEndp -= n;
  }
  kanaReplace(howManyDelete, NULL, 0, 0);

  makeYomiReturnStruct(d);

  if (yc->kEndp == 0) {
    /* 未確定文字列が全くなくなったのなら、φモードに遷移する */
    d->current_mode = &empty_mode;
    d->kanji_status_return->info |= KanjiEmptyInfo;
  }
  return 0;
}

static
YomiKillToEndOfLine(d)  /* カーソルから右のすべての文字の削除 */
uiContext d;
{
  extern KanjiModeRec empty_mode;
  yomiContext yc = (yomiContext)d->modec;

  romajiReplace (yc->rEndp - yc->rCurs, NULL, 0, 0);
  kanaReplace   (yc->kEndp - yc->kCurs, NULL, 0, 0);

  makeYomiReturnStruct(d);

  if (yc->kEndp == 0) {
    /* 未確定文字列が全くなくなったのなら、φモードに遷移する */
    d->current_mode = &empty_mode;
    d->kanji_status_return->info |= KanjiEmptyInfo;
  }
  return 0;
}

static
YomiQuit(d)/* 読みの取り消し */
uiContext d;
{
  extern KanjiModeRec empty_mode;

  /* 未確定文字列を削除する */
  RomajiClearYomi(d);

  makeYomiReturnStruct(d);

  /* 未確定文字列が全くなくなったので、φモードに遷移する */
  d->current_mode = &empty_mode;
  d->kanji_status_return->info |= KanjiEmptyInfo;

  return checkIfYomiQuit(d, 0);
}

RomajiCursorFlush(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  yc->kRStartp = yc->kCurs = yc->kEndp;
  yc->rStartp  = yc->rCurs = yc->rEndp;
}

/* 
 generalReplace -- カナバッファにもローマ字バッファにも使える置換ルーチン

  この置換ルーチンは文字列のメモリ上の置換を行うためのライブラリルーチ
  ンである。メモリ上に文字列を保持しておく仕組みは次のようになっている
  ものとする。

    ・文字列用のバッファ
    ・文字の属性用のバッファ
    ・カーソル(インデックス(ポインタではない))
    ・文字列の終わりを指すインデックス
    ・必ずカーソルより左に存在するインデックス(未変換文字へのインデッ
      クスに使ったりする)

  上記に示されるバッファ上のカーソルの前か後ろの指定された長さの文字列
  を別に指定される文字列で置き換える処理をする。

  トータルのバイト数が変化する場合は文字列の終わりを指すインデックスの
  値も変化させる。また、カーソルの前の部分に対して文字列の置換を行う場
  合にはカーソルポジションの値も変化させる。カーソルを変化させた結果、
  未変換文字等へのインデックスよりも小さくなった場合には、未変換文字等
  へのインデックスの値をカーソルの値に合わせて小さくする。

  この関数の最終引数には新たに挿入する文字列の属性に関するヒントが記述
  できる。新たに挿入される文字列に対して、先頭文字には SENTOU と与えら
  れたヒントのＯＲを取った値が与えられ、その他の文字にはヒントで与えら
  れた値が格納される。

  【引数】
     buf      バッファへのポインタ
     attr     属性バッファへのポインタ
     startp   バッファの未確定文字列などへのインデックスを収めている変
              数へのポインタ
     cursor   カーソル位置を収めている変数へのポインタ
     endp     文字列の最終位置を指し示している変数へのポインタ

     bytes    何バイト置換するか？負の数が指定されるとカーソルの前の部
              分の |bytes| 分の文字列が置換の対象となり、正の数が指定
              されるとカーソルの後ろの部分の bytes 分の文字列が対象と
              なる。
     rplastr  新しく置く文字列へのポインタ
     len      新しく置く文字列の長さ
     attrmask 新しく置く文字列の属性のヒント

  実際にはこの関数を直接に使わずに、bytes, rplastr, len, attrmask だけ
  を与えるだけですむマクロ、kanaReplace, romajiReplace を使うのが良い。
*/

generalReplace(buf, attr, startp, cursor, endp, bytes, rplastr, len, attrmask)
char *buf, *attr, *rplastr;
int *startp, *cursor, *endp,  bytes, len, attrmask; 
{ 
  int idou, begin, end, i; 
  int cursorMove;

  if (bytes > 0) {
    cursorMove = 0;
    begin = *cursor;
    end = *endp;
  }
  else {
    bytes = -bytes;
    cursorMove = 1;
    begin = *cursor - bytes;
    end = *endp;
  }

  idou = len - bytes;

  moveStrings(buf, attr, begin + bytes, end, idou);
  *endp += idou;
  if (cursorMove) {
    *cursor += idou;
    if (*cursor < *startp)
      *startp = *cursor;
  }

  strncpy(buf + begin, rplastr, len);
  for (i = 0 ; i < len ; i++) {
    attr[begin + i] = attrmask;
  }
  if (len)
    attr[begin] |= SENTOU;
}

YomiQuotedInsert(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec, GetKanjiString();

  if (yc->generalFlags & IROHA_YOMI_CHGMODE_INHIBITTED) {
    return NothingChangedWithBeep(d);
  }    

  RomajiFlushYomi(d, NULL, 0);
  yomiQuotedInsertMode(d);
  d->kanji_status_return->length = -1;
  return 0;
}

static
YomiInsertQuoted(d)
uiContext d;
{
  char ch;
  coreContext cc = (coreContext)d->modec;
  yomiContext yc;

  ch = d->buffer_return[0];

  if (IrohaFunctionKey(ch)) {
    d->kanji_status_return->length = -1;
    d->kanji_status_return->info = 0;
    return 0;
  }
  else {
    d->current_mode = cc->prevMode;
    d->modec = cc->next;
    free(cc);

    yc = (yomiContext)d->modec;
    if (ch & 0x80) {
      char *p = (char *)d->buffer_return;
      int i;

      for (i = d->nbytes ; i >= 0 ; i--) {
	p[i + 1] = p[i];
      }
      p[0] = 0x8e; /* SS2 */
      d->nbytes += 1;
    }

    romajiReplace (0, d->buffer_return, d->nbytes, 0);
    kanaReplace   (0, d->buffer_return, d->nbytes, HENKANSUMI);
    yc->rStartp = yc->rCurs;
    yc->kRStartp = yc->kCurs;
    makeYomiReturnStruct(d);
    currentModeInfo(d);
    d->status = EXIT_CALLBACK;
    return 0;
  }
}

/* ConvertAsHex -- １６進とみなしての変換 

  ローマ字入力されて反転表示されている文字列を１６進で表示されているコードと
  みなして変換する。

  (MSBは０でも１でも良い)

  */

static
ConvertAsHex(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  if (yc->henkanInhibition & IROHA_YOMI_INHIBIT_ASHEX) {
    return NothingChangedWithBeep(d);
  }
  if (convertAsHex(d)) {
    d->kanji_status_return->length = 0;
    return YomiExit(d, 2);
  }
  else {
    return NothingChangedWithBeep(d);
  }
}

/*
  convertAsHex  １６進の数字を漢字文字に変換

  これは内部的に使用するためのルーチンである。d->romaji_buffer に含ま
  れる文字列を１６進で表された漢字コードであるとみなして、そのコードに
  よって表現される漢字文字に変換する。変換した文字列は buffer_return 
  に格納する。リターン値はエラーがなければ buffer_return に格納した文
  字列の長さである(通常は２である)。エラーが発生している時は−１が格納
  される。

  モードの変更等の処理はこの関数では行われない。

  またバッファのクリアなども行わないので注意するべきである。

  <戻り値>
    正しく１６進に変換できた場合は１そうでない時は０が返る。
*/

convertAsHex(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  return cvtAsHex(d, d->buffer_return, yc->romaji_buffer, yc->rEndp);
}

cvtAsHex(d, buf, hexbuf, hexlen)
uiContext d;     
unsigned char *buf, *hexbuf;
int hexlen;
{
  int i;
  unsigned char rch;
  
  if (hexlen != 4) { /* 入力された文字列の長さが４文字でないのであれば変換
			してあげない */
    d->kanji_status_return->length = -1;
    return 0;
  }
  for (i = 0 ; i < 4 ; i++) {
    rch = hexbuf[i]; /* まず一文字取り出し、１６進の数字にする。 */
    if ('0' <= rch && rch <= '9') {
      rch -= '0';
    }
    else if ('A' <= rch && rch <= 'F') {
      rch -= 'A' - 10;
    }
    else if ('a' <= rch && rch <= 'f') {
      rch -= 'a' - 10;
    }
    else {
      d->kanji_status_return->length = -1;
      return 0;
    }
    buf[i] = rch; /* 取り敢えず保存しておく */
  }
  buf[0] = 0x80 | (buf[0] * 16 + buf[1]); /* 保存してある値を元に漢字にする */
  buf[1] = 0x80 | (buf[2] * 16 + buf[3]);
  if (buf[0] < 0x00a1 || 0x00fe < buf[0]
      || buf[1] < 0x00a1 || 0x00fe < buf[1]) {
    return 0;
  }
  else {
    return 1;
  }
}

/*
 */

static
YomiHenkanOrSpace(d)
uiContext d;
{
  extern KanjiModeRec empty_mode;
  yomiContext yc = (yomiContext)d->modec;

  if (d->ch != ' ') {
    return YomiHenkan(d);
  }
  else {
    int i;

    RomajiFlushYomi(d, NULL, 0);
    for (i = 0 ; i < yc->kEndp ; i++) {
      if ( !(yc->kAttr[i] & HENKANSUMI) ) {
        int ret;

        if (yc->rEndp < d->bytes_buffer) {
          strncpy(d->buffer_return, yc->romaji_buffer, yc->rEndp);
          d->buffer_return[yc->rEndp] = ' ';
          ret = yc->rEndp + 1;
        }
        else {
          strncpy(d->buffer_return, yc->romaji_buffer, d->bytes_buffer);
          ret = d->bytes_buffer;
        }
        RomajiClearYomi(d);
        d->kanji_status_return->length = 0;
	d->current_mode = &empty_mode;
        AlphaMode(d);
        return ret;
      }
    }
    return YomiHenkan(d);
  }
}

static
_yomiHenkanExit(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  d->nbytes = retval;
  retval = YomiExit(d, retval);
  currentModeInfo(d);
  return retval;
}

static
_yomiHenkanQuit(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  makeYomiReturnStruct(d);
  currentModeInfo(d);
  return retval;
}

/*
 * かな漢字変換を行い(変換キーが初めて押された)、TanKouhoModeに移行する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
YomiHenkan(d)
uiContext	d;
{
  yomiContext yc = (yomiContext)d->modec;
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

  len = RomajiFlushYomi(d, d->genbuf, ROMEBUFSIZE);
  RomajiCursorFlush(d);

  if (yc->henkanCallback) {
    return yc->henkanCallback(d, IROHA_FN_Henkan, d->genbuf, len);
  }

  /* TanKouhoMode(単候補表示モード)に移行する */
  hc = pushHenkanMode(d, d->genbuf, len, yc,
		      0, _yomiHenkanExit, _yomiHenkanQuit, 0);
  if (hc == 0) {
    return -1;
  }
  hc->minorMode = IROHA_MODE_TankouhoMode;
  hc->kouhoCount = 1;

  if (henkan(d, 0) < 0) {
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

static
yomiChangeMode(d, mode)
uiContext d;
int mode;
{
  yomiContext yc = (yomiContext)d->modec;

  if (yc->generalFlags & IROHA_YOMI_CHGMODE_INHIBITTED) {
    return NothingChangedWithBeep(d);
  }

  RomajiFlushYomi(d, NULL, 0);
  yc->baseChar = mode;
  if (mode == JISHU_HIRA) {
    yc->majorMode = yc->minorMode = IROHA_MODE_HenkanMode;
  }
  else {
    yc->majorMode = yc->minorMode = IROHA_MODE_ZenKataHenkanMode + mode - 1;
  }
  
  makeYomiReturnStruct(d);
  currentModeInfo(d);
  return 0;
}

static
YomiHankakuRomajiMode(d)
uiContext d;
{
  return yomiChangeMode(d, JISHU_HAN_ALPHA);
}

static
YomiZenkakuRomajiMode(d)
uiContext d;
{
  return yomiChangeMode(d, JISHU_ZEN_ALPHA);
}

static
YomiHankakuKatakanaMode(d)
uiContext d;
{
  if (InhibitHankakuKana)
    return NothingChangedWithBeep(d);
  else
    return yomiChangeMode(d, JISHU_HAN_KATA);
}

static
YomiZenkakuKatakanaMode(d)
uiContext d;
{
  return yomiChangeMode(d, JISHU_ZEN_KATA);
}

static
YomiHiraganaMode(d)
uiContext d;
{
  return yomiChangeMode(d, JISHU_HIRA);
}

static
YomiNextMode(d)
uiContext d;
{
  int base;
  yomiContext yc = (yomiContext)d->modec;

  base = yc->baseChar;
  do {
    base++;
    if (base >= MAX_JISHU) {
      base = JISHU_HIRA;
    }
  } while (base == JISHU_HAN_KATA && InhibitHankakuKana);
  return yomiChangeMode(d, base);
}

static
YomiPrevMode(d)
uiContext d;
{
  int base;
  yomiContext yc = (yomiContext)d->modec;

  base = yc->baseChar;
  do {
    base--;
    if (base < 0) {
      base = MAX_JISHU - 1;
    }
  } while (base == JISHU_HAN_KATA && InhibitHankakuKana);
  return yomiChangeMode(d, base);
}
  

#include "yomimap.c"
