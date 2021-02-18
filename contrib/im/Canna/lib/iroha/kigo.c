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

static	char	rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/kigo.c,v 3.2 91/06/28 18:14:13 kon Exp $";

#include	"iroha.h"

extern SelectDirect;

static unsigned char *kuhaku = (unsigned char *)"　";   /* 記号一覧行の空白 */
static unsigned char *kakko = (unsigned char *)"［］";  /* 記号括弧の文字列 */

#define BYTE1		84	/* JISコード表の第一バイトの数 */
#define BYTE2		94	/* JISコード表の第二バイトの数 */
#define KIGOSU		(((BYTE1 - 1) * BYTE2) + 4)
    				/* 記号の総数 */
#define KAKKOSIZE	2	/* JISコード表示用括弧のバイト数 */
#define KCODESIZE	4	/* JISコード表示そのもののバイト数 */
#define KCODEALLSIZE	((KAKKOSIZE * 2) + KCODESIZE)
     				/* JISコード表示全体のバイト数 8 */

static int kigo_curIkouho, KigoKakutei();

/* cfunc ichiranContext
 *
 * ichiranContext
 *
 */
static ichiranContext
newKigoContext()
{
  ichiranContext kcxt;

  if((kcxt = (ichiranContext)malloc(sizeof(ichiranContextRec))) == NULL) {
    necKanjiError = "malloc (newKigoContext) できませんでした";
    return(0);
  }
  kcxt->id = ICHIRAN_CONTEXT;

  return kcxt;
}

static void
freeKigoContext(kc)
ichiranContext kc;
{
  free(kc);
}

/*
 * 記号一覧行を作る
 */
static
getKigoContext(d,
	  everyTimeCallback, exitCallback, quitCallback, auxCallback)
uiContext d;
int (*everyTimeCallback)(), (*exitCallback)();
int (*quitCallback)(), (*auxCallback)();
{
  extern KanjiModeRec kigo_mode;
  ichiranContext kc;
  int retval = 0;

  if(pushCallback(d, d->modec,
	everyTimeCallback, exitCallback, quitCallback, auxCallback) == 0) {
    necKanjiError = "malloc (pushCallback) できませんでした";
    return(NG);
  }
  
  if((kc = newKigoContext()) == NULL) {
    popCallback(d);
    return(NG);
  }
  kc->next = d->modec;
  d->modec = (mode_context)kc;

  kc->prevMode = d->current_mode;
  d->current_mode = &kigo_mode;

  return(retval);
}

static void
popKigoMode(d)
uiContext d;
{
  ichiranContext kc = (ichiranContext)d->modec;

  d->modec = kc->next;
  d->current_mode = kc->prevMode;
  freeIchiranContext(kc);
}

/*
 * 記号一覧行に関する構造体の内容を更新する
 *
 * ・カレント候補によって kouhoinfo と glineinfo から候補一覧行を作る
 * ・カレント候補のコードをキャラクタに変換する
 *
 * 引き数	RomeStruct
 * 戻り値	なし
 */
static
makeKigoGlineStatus(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;
  unsigned char *kptr, *gptr;
  int  i, b1, b2;

  kptr = kc->kouhoifp[*(kc->curIkouho)].khdata;
  gptr = kc->glineifp->gldata + KAKKOSIZE;
  
  /* カレント記号のJISコードを一覧行の中のカッコ内に入れる */
  for(i=0; i<KIGOSIZE; i++, kptr++) {
    b1 = (int)((unsigned int)(*kptr & 0x7f) >> 4); /* １バイト目 */
    b2 = *kptr & 0x0f;			/* ２バイト目 */
    b1 += ((b1 > 0x09) ? ('a' - 10) : '0');	/* 文字にする */
    b2 += ((b2 > 0x09) ? ('a' - 10) : '0');	/* 文字にする */
    *gptr++ = b1;
    *gptr++ = b2;
  }

  d->kanji_status_return->info |= KanjiGLineInfo;
  d->kanji_status_return->gline.line = kc->glineifp->gldata;
  d->kanji_status_return->gline.length = kc->glineifp->gllen;
  d->kanji_status_return->gline.revPos =
    kc->kouhoifp[*(kc->curIkouho)].khpoint;
  d->kanji_status_return->gline.revLen = KIGOSIZE;

}

/* 記号一覧用のglineinfoとkouhoinfoを作る
 *
 * ＊glineinfo＊
 *    int glkosu   : int glhead     : int gllen  : unsigned char *gldata
 *    １行の候補数 : 先頭記号が     : １行の長さ : 記号一覧行の文字列
 *                 : 何番目の記号か :
 * -------------------------------------------------------------------------
 * 0 | 6           : 0              : 24         : １☆２★３○４●５◎６□
 *
 *  ＊kouhoinfo＊
 *    int khretsu  : int khpoint  : unsigned char *khdata
 *    未使用       : 行の先頭から : 記号の文字
 *                 : 何バイト目か :
 * -------------------------------------------------------------------------
 * 0 | 0           : 0            : ☆
 * 1 | 0           : 4            : ★
 * 2 | 0           : 8            : ○
 *          :               :
 *
 * 引き数	headkouho	カレント記号候補行の先頭候補の位置
 *					(2121から何個目か(2121は０番目))
 *		uiContext
 * 戻り値	正常終了時 0
 */
static
makeKigoInfo(d, headkouho)
uiContext	d;
int		headkouho;
{
  ichiranContext kc = (ichiranContext)d->modec;
  unsigned char *gptr;
  int  i, b1, b2, lnko, cn;
  unsigned char byte1hex = 0xa1;
  unsigned char byte2hex = 0xa1;

  b2 = headkouho % BYTE2;	/* JISコード表中(Ｘ軸)の位置 (点-1) */
  b1 = headkouho / BYTE2;	/* JISコード表中(Ｙ軸)の位置 (区-1) */

#ifdef DEBUG
  if (iroha_debug) {
    printf("kigoinfo = bangomax %d, b1 %d, b2 %d\n", kc->nIkouho, b1, b2);
    printf("kigoinfo = headkouho %d, curIkouho %d\n",
	   headkouho, *(kc->curIkouho));
  }
#endif

  /* 記号一覧用のglineinfoとkouhoinfoを作る */
  gptr = kc->glinebufp;

  kc->glineifp->glhead = headkouho;
  kc->glineifp->gldata = gptr;

  /* JISコードの表示領域を一覧行中に作る */
  strncpy(gptr, kakko, KAKKOSIZE);
  gptr += KAKKOSIZE;
  for(i=0; i<KCODESIZE; i++)
    *gptr++ = ' ';		/* ダミーを入れる */
  strncpy(gptr, (kakko + KAKKOSIZE), KAKKOSIZE);
  gptr += KAKKOSIZE;

  for(cn=KCODEALLSIZE, lnko=0;
      b1<BYTE1 && lnko<kc->nIkouho && (headkouho+lnko)<KIGOSU ; b1++) {
    for(; b2<BYTE2 && lnko<kc->nIkouho && (headkouho+lnko)<KIGOSU; b2++, lnko++) {
      
      /* 区切りになる空白をコピーする */
      if(lnko != 0) {
	strncpy(gptr, kuhaku, KG_KUHAKUSIZE);
	gptr += KG_KUHAKUSIZE;
	cn += KG_KUHAKUSIZE;
      }

      kc->kouhoifp[lnko].khpoint = cn;
      kc->kouhoifp[lnko].khdata = gptr;
      
      /* 候補をコピーする */
      *gptr++ = byte1hex + (unsigned char)b1;
      *gptr++ = byte2hex + (unsigned char)b2;

      cn += KIGOSIZE;
    }
    b2 = 0;
  }
  *gptr = (unsigned char)NULL;
  kc->glineifp->glkosu = lnko;
  kc->glineifp->gllen = strlen(kc->glineifp->gldata);

  return(0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 記号一覧                                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
kigoIchiranExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  retval = YomiExit(d, retval);
  currentModeInfo(d);

  return(retval);
}

static
kigoIchiranQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  currentModeInfo(d);

  return(0);
}

static
uuKigoIchiranQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  currentModeInfo(d);

  return(uiUtil2Mode(d, 0));
}

KigoIchiran(d)
uiContext	d;
{
  yomiContext yc = (yomiContext)d->modec;

  if (yc->generalFlags & IROHA_YOMI_CHGMODE_INHIBITTED) {
    return NothingChangedWithBeep(d);
  }    
  
  if(makeKigoIchiran(d, 1) == NG)
    return(GLineNGReturn(d));
  else
    return(0);
}

/*
 * 記号一覧行を表示する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
makeKigoIchiran(d, flag)
uiContext	d;
int             flag;
{
  ichiranContext kc;
  int            headkouho, retval = 0;
  unsigned char  inhibit = 0;

  if(d->ncolumns < (KCODEALLSIZE + KIGOSIZE)) {
    NothingChangedWithBeep(d);
    necKanjiError = "候補一覧用の幅が狭いので記号一覧できません";
    return(NG);
  }

  if(flag) { /* 記号一覧 */
    if((retval = getKigoContext(d,
		0, kigoIchiranExitCatch, kigoIchiranQuitCatch, 0)) == NG) {
      return(NG);
    }
    kc = (ichiranContext)d->modec;
    kc->majorMode = kc->minorMode = IROHA_MODE_KigoMode;
  } else { /* 拡張の記号一覧 */
    if((retval = getKigoContext(d,
		0, kigoIchiranExitCatch, uuKigoIchiranQuitCatch, 0)) == NG) {
      return(NG);
    }
    kc = (ichiranContext)d->modec;
    kc->majorMode = IROHA_MODE_ExtendMode;
    kc->minorMode = IROHA_MODE_KigoMode;
  }
  currentModeInfo(d);

  /* 最大記号表示数のセット */
  /* 総カラム数から "［JIS ］" 分を差し引いて計算する */
  if((kc->nIkouho =
      (((d->ncolumns - KCODEALLSIZE - KIGOSIZE) / KIGOCOL) + 1))
                                                  > KIGOBANGOMAX) {
    kc->nIkouho = KIGOBANGOMAX;
  }

  kc->curIkouho = &kigo_curIkouho;
  inhibit |= (unsigned char)NUMBERING;

  if(allocIchiranBuf(d, inhibit, 0) == NG) { /* 記号一覧モード */
    popKigoMode(d);
    popCallback(d);
    return(NG);
  }

  /* カレント候補のある記号一覧行の先頭候補と、
     一覧行中のカレント候補の位置を求める */
  if(d->curkigo) {		/* a1a1から何番目の記号か */
    headkouho = (d->curkigo / kc->nIkouho) * kc->nIkouho;
    *(kc->curIkouho) = d->curkigo % kc->nIkouho;
  } else {
    d->curkigo = 0;
    headkouho = 0;
    *(kc->curIkouho) = 0;
  }

  /* glineinfoとkouhoinfoを作る */
  makeKigoInfo(d, headkouho);

  /* kanji_status_returnを作る */
  makeKigoGlineStatus(d);

  return(0);
}

/*
 * 記号一覧行中の次の記号に移動する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0
 */
static
KigoForwardKouho(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;
  int  headkouho;

  /* 次の記号にする */
  ++*(kc->curIkouho);
  
  /* 一覧表示の最後の記号だったら、次の一覧行の先頭記号をカレント記号とする */
  if((*(kc->curIkouho) >= kc->nIkouho) ||
     (kc->glineifp->glhead + *(kc->curIkouho) >= KIGOSU)) {
    headkouho  = kc->glineifp->glhead + kc->nIkouho;
    if(headkouho >= KIGOSU)
      headkouho = 0;
    *(kc->curIkouho) = 0;
    makeKigoInfo(d, headkouho);
  }

  /* kanji_status_retusrn を作る */
  makeKigoGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return(0);
}

/*
 * 記号一覧行中の前の記号に移動する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0
 */
static
KigoBackwardKouho(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;
  int  headkouho;

  /* 前の記号にする */
  --*(kc->curIkouho);

  /* 一覧表示の先頭の記号だったら、前の一覧行の最終記号をカレント記号とする */
  if(*(kc->curIkouho) < 0) {
    headkouho  = kc->glineifp->glhead - kc->nIkouho;
    if(headkouho < 0)
      headkouho = ((KIGOSU - 1) / kc->nIkouho) * kc->nIkouho;
    makeKigoInfo(d, headkouho);
    *(kc->curIkouho) = kc->glineifp->glkosu - 1;
  }

  /* kanji_status_retusrn を作る */
  makeKigoGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return(0);
}

/*
 * 前記号一覧列に移動する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0
 */
static
KigoPreviousKouhoretsu(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;
  int headkouho;

  /** 前候補列にする **/
  headkouho  = kc->glineifp->glhead - kc->nIkouho;
  if(headkouho < 0)
    headkouho = ((KIGOSU -1) / kc->nIkouho) * kc->nIkouho;
  makeKigoInfo(d, headkouho);

  /* *(kc->curIkouho) がカレント記号一覧の記号数より大きくなってしまったら
     最右記号をカレント候補とする */
  if(*(kc->curIkouho) >= kc->glineifp->glkosu)
    *(kc->curIkouho) = kc->glineifp->glkosu - 1;

  /* kanji_status_retusrn を作る */
  makeKigoGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return(0);
}

/*
 * 次記号一覧列に移動する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0
 */
static
KigoNextKouhoretsu(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;
  int headkouho;

  /** 次候補列にする **/
  headkouho = kc->glineifp->glhead + kc->nIkouho;
  if(headkouho >= KIGOSU)
    headkouho = 0;
  makeKigoInfo(d, headkouho);

  /* *(kc->curIkouho) がカレント記号一覧の記号数より大きくなってしまったら
     最右記号をカレント候補とする */
  if(*(kc->curIkouho) >= kc->glineifp->glkosu)
    *(kc->curIkouho) = kc->glineifp->glkosu - 1;

  /* kanji_status_retusrn を作る */
  makeKigoGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return(0);
}

/*
 * 記号一覧行中の先頭の記号に移動する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0
 */
static
KigoBeginningOfKouho(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;

  /* 候補列の先頭候補をカレント候補にする */
  *(kc->curIkouho) = 0;

  /* kanji_status_retusrn を作る */
  makeKigoGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return(0);
}

/*
 * 記号一覧行中の最右の記号に移動する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0
 */
static
KigoEndOfKouho(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;

  /** 候補列の最右候補をカレント候補にする **/
  *(kc->curIkouho) = kc->glineifp->glkosu - 1;

  /* kanji_status_retusrn を作る */
  makeKigoGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return(0);
}

/*
 * 記号一覧行中の入力された番号の記号に移動する  【未使用】
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0
 */
static
KigoBangoKouho(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;
  int num, index;

  /* 入力データは ０〜９ ａ〜ｆ か？ */
  if(((0x30 <= d->ch) && (d->ch <= 0x39))
     || ((0x61 <= d->ch) && (d->ch <= 0x66))) {
    if((0x30 <= d->ch) && (d->ch <= 0x39))
      num = (int)(d->ch & 0x0f);
    else if((0x61 <= d->ch) && (d->ch <= 0x66))
      num = (int)(d->ch - 0x57);
  } 
  else {
    /* 入力された番号は正しくありません */
    return NothingChangedWithBeep(d);
  }
  /* 入力データは 候補行の中に存在する数か？ */
  if(num >= kc->glineifp->glkosu) {
    /* 入力された番号は正しくありません */
    return NothingChangedWithBeep(d);
  }

  /* 候補列の先頭候補を得る */
  *(kc->curIkouho) = num;

  /* SelectDirect のカスタマイズの処理 */
  if(SelectDirect) /* ON */ {

    return(KigoKakutei(d));
  } else           /* OFF */ {
    /* kanji_status_retusrn を作る */
    makeKigoGlineStatus(d);

    return(0);
  }
}

/*
 * 記号一覧行中から選択された記号を確定する
 *
 * ・次に記号一覧した時に前回確定した記号がカレント候補となるように、
 *   確定した候補をセーブしておく
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0
 */
static
KigoKakutei(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;

  /* カレント記号をセーブする */
  d->curkigo = kc->glineifp->glhead + *(kc->curIkouho);

  /* エコーストリングを確定文字列とする */
  if (d->bytes_buffer >= KIGOSIZE) {
    d->nbytes = KIGOSIZE;
    strncpy(d->buffer_return, kc->kouhoifp[*(kc->curIkouho)].khdata, 
	    d->nbytes);
    d->buffer_return[KIGOSIZE] = (unsigned char)NULL;
  }
  else {
    d->nbytes = 0;
  }

  popKigoMode(d);
  GlineClear(d);

  d->status = EXIT_CALLBACK;

  return(d->nbytes);
}

/*
 * 記号一覧行を消去する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0
 */
static
KigoQuit(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;

  popKigoMode(d);

  /* gline をクリアする */
  GlineClear(d);

  d->status = QUIT_CALLBACK;

  return(0);
}

#include	"kigomap.c"
