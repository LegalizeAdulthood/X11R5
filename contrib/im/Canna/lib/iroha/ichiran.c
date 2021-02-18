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

static	char	rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/ichiran.c,v 3.7 91/09/04 13:19:16 kon Exp $";

#include	<errno.h>
#include	"iroha.h"

extern SelectDirect;
extern BunsetsuKugiri;
extern QuitIchiranIfEnd;
extern stayAfterValidate;
extern HexkeySelect;
extern CursorWrap;

extern int kCount;

extern int TanNextKouho(), TanKakutei();

static int clearIchiranContext(), IchiranKakutei();

#define ICHISIZE 9
unsigned char *bango = (unsigned char *)
     "１　２　３　４　５　６　７　８　９　ａ　ｂ　ｃ　ｄ　ｅ　ｆ";
                                                /* 候補行の候補番号の文字列 */
static unsigned char *kuuhaku = (unsigned char *)"　";

/*
 * 一覧行表示中のカレント文節の候補を更新する
 *
 * ・すでにある d->echo_buffer のうちのカレント文節の部分を変更する
 * ・これにともない kugiri も更新される
 *
 * 引き数	uiContext
 *              henkanContext
 */
static void
makeIchiranEchoStrCurChange(d, hc)
uiContext	d;
henkanContext   hc;
{
  int  i, j, k, old_klen, new_klen;
  unsigned char *ptr;

  ptr = hc->echo_buffer;

  /* 旧カレント文節のバイト数 */
  old_klen = hc->kugiri[hc->curbun + 1] - hc->kugiri[hc->curbun];
  if(BunsetsuKugiri && (hc->curbun != (hc->nbunsetsu - 1)))
    old_klen -= 1;

  /* 新カレント文節のバイト数 */
  new_klen = strlen(hc->allkouho[hc->curIkouho]);

  /* カレント文節の長さ分空けるために、次の文節以降をずらす */
  /* ずらす文字列長 */ 
  i = hc->kugiri[hc->nbunsetsu] - hc->kugiri[hc->curbun + 1];

  if(new_klen > old_klen) { /* 右にずらす*/
    j = hc->kugiri[hc->nbunsetsu];
    k = new_klen - old_klen;
    for(; i>=0; i--, j--) /* NULL もコピーする*/
      hc->echo_buffer[j + k] = hc->echo_buffer[j];
  }else if(new_klen < old_klen) { /* 左にずらす*/
    j = hc->kugiri[hc->curbun + 1];
    k = old_klen - new_klen;
    for(; i>=0; i--, j++) /* NULL もコピーする*/
      hc->echo_buffer[j - k] = hc->echo_buffer[j];
  }
  /* カレント候補を空けたところに入れる */ 
  strncpy((ptr + hc->kugiri[hc->curbun]), hc->allkouho[hc->curIkouho], 
	  new_klen);
  if(BunsetsuKugiri && (hc->curbun != (hc->nbunsetsu - 1)))
    *(ptr + hc->kugiri[hc->curbun] + new_klen) = (unsigned char)' ';

  /* 各文節の区ぎりをとっておく(先頭からのバイト数) */
  /* カレント文節以降差分をたす */
  for(i=hc->curbun; i<hc->nbunsetsu; i++)
    hc->kugiri[i + 1] += new_klen - old_klen;
}

/*
 * かな漢字変換用の構造体の内容を更新する(その場のみ)
 *
 * ・d->echo_buffer をエコーし、カレント文節を反転する
 *
 * 引き数	uiContext
 *              henkanContext
 */
static
makeIchiranKanjiStatusReturn(d, hc)
uiContext	d;
henkanContext   hc;
{
  unsigned char tmp;

  d->kanji_status_return->echoStr = hc->echo_buffer;
  d->kanji_status_return->length = hc->kugiri[hc->nbunsetsu];
  d->kanji_status_return->revPos = hc->kugiri[hc->curbun];
  if(BunsetsuKugiri) {
    tmp = hc->echo_buffer[hc->kugiri[hc->curbun]];
    d->kanji_status_return->revLen =
     ((tmp & 0x80) && (tmp != 0x8e)) ? 2 : 1; 
  } else {
    d->kanji_status_return->revLen =
      hc->kugiri[hc->curbun + 1] - hc->kugiri[hc->curbun];
  }
}

/*
 * 候補行に関する構造体の内容を更新する
 *
 * ・glineinfo と kouhoinfo から候補行を作成し、カレント候補番号を反転する
 *
 * 引き数	uiContext
 * 戻り値	なし
 */
makeGlineStatus(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
  unsigned char tmp, pos;
  int kcoffset, cur;

  if (kCount) {
    cur = *(ic->curIkouho) + 1;
    kcoffset = (cur < 10) ? 2 : (cur < 100) ? 1 : 0;
  }

  d->kanji_status_return->info |= KanjiGLineInfo;
  d->kanji_status_return->gline.line =
    ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].gldata;
  d->kanji_status_return->gline.length = 
    ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].gllen;
  d->kanji_status_return->gline.revPos = pos = 
    ic->kouhoifp[*(ic->curIkouho)].khpoint;
  tmp = d->kanji_status_return->gline.line[pos];
  d->kanji_status_return->gline.revLen = 
    ((tmp & 0x80) && (tmp != 0x8e)) ? 2 : 1; 

  if (kCount) {
    sprintf(d->kanji_status_return->gline.line + d->ncolumns
	    - ICHISIZE + kcoffset,
	    " %d/%d",
	    cur, ic->nIkouho);
    d->kanji_status_return->gline.length
      = strlen(d->kanji_status_return->gline.line);
  }
}

static
ichiranEveryTimeCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  makeIchiranEchoStrCurChange(d, env);
  makeIchiranKanjiStatusReturn(d, env);

  return(retval);
}

static
ichiranExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  henkanContext hc = (henkanContext)env;

  hc->kouhoCount = 0;
  /* d->curIkouhoをカレント候補とする */
  if((retval = RkXfer(hc->context, hc->curIkouho)) == NG) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "カレント候補を取り出せませんでした";
    /* カレント候補が取り出せないくらいでは何ともないぞ */
  } else 
    retval = d->nbytes = 0;

  makeIchiranEchoStrCurChange(d, (henkanContext)env);
  makeIchiranKanjiStatusReturn(d, (henkanContext)env);

  freeGetIchiranList(((henkanContext)env)->allkouho);
  
  popCallback(d);

  if(!stayAfterValidate && !d->more.todo) {
    d->more.todo = 1;
    d->more.ch = 0;
    d->more.fnum = IROHA_FN_Forward;
  }
  currentModeInfo(d);

  return(retval);
}

static
ichiranQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  henkanContext hc = (henkanContext)env;

  hc->kouhoCount = 0;

  if((retval = RkXfer(hc->context, hc->curIkouho)) == NG) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "カレント候補を取り出せませんでした";
    /* カレント候補が取り出せないくらいでは何ともないぞ */
  } else 
    retval = d->nbytes = 0;

  makeIchiranEchoStrCurChange(d, env); 	/* RkGetKanji したのをとってくる */
  makeIchiranKanjiStatusReturn(d, env);

  freeGetIchiranList(((henkanContext)env)->allkouho);

  popCallback(d);
  currentModeInfo(d);
  return(retval);
}

void
freeIchiranContext(ic)
ichiranContext ic;
{
  free(ic);
}

void
freeIchiranBuf(ic)
ichiranContext ic;
{
  if(ic->glinebufp)
    free(ic->glinebufp);
  if(ic->kouhoifp)
    free(ic->kouhoifp);
  if(ic->glineifp)
    free(ic->glineifp);
}

freeGetIchiranList(buf)
unsigned char **buf;
{
  /* 候補一覧表示行用のエリアをフリーする */
  if(*buf)
    free(*buf);
  if(buf)
    free(buf);
}

static void
popIchiranMode(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;

  d->modec = ic->next;
  d->current_mode = ic->prevMode;
  freeIchiranContext(ic);
}

/*
 * すべての候補を取り出して、配列にする
 */
unsigned char **
getIchiranList(d, context, nelem, currentkouho)
uiContext d;
int context;
int *nelem, *currentkouho;
{
  unsigned char *work, *wptr, **bptr, **buf, **bwork;
  RkStat st;

  /* RkListで得る、すべての候補のための領域を得る */
  if((work = (unsigned char *)malloc(ROMEBUFSIZE)) == (unsigned char *)NULL) {
    necKanjiError = "malloc (getIchiranList) できませんでした";
    return(0);
  }

  /* すべての候補を得る。
     例: けいかん → 警官@景観@掛冠@@ (@はNULL) */
  if((*nelem = RkGetKanjiList(context, work, ROMEBUFSIZE)) < 0) {
    necKanjiError = "すべての候補の取り出しに失敗しました";
    free(work);
    return(0);
  }

  /* makeKouhoIchiran()に渡すデータ */
  if((buf = (unsigned char **)calloc
      (*nelem + 1, sizeof(unsigned char *))) == (unsigned char **)NULL) {
    necKanjiError = "malloc (getIchiranList) できませんでした";
    free(work);
    return(0);
  }
  for(wptr = work, bptr = buf; *wptr; bptr++) { /* buf を作る */
    *bptr = wptr;
    while(*wptr++);
  }
  *bptr = (unsigned char *)NULL;

  if(RkGetStat(context, &st) == -1) {
    necKanjiError = "ステイタスを取り出せませんでした";
    free(work);
    free(buf);
    return(0);
  }
  *currentkouho = st.candnum; /* カレント候補は何番目？ */

  return(buf);
}

/* cfunc ichiranContext
 *
 * ichiranContext 候補一覧用の構造体を作り初期化する
 *
 */
ichiranContext
newIchiranContext()
{
  ichiranContext icxt;

  if((icxt = (ichiranContext)malloc(sizeof(ichiranContextRec))) == 
     (ichiranContext)NULL) {
    necKanjiError = "malloc (newIchiranContext) できませんでした";
    return(0);
  }
  clearIchiranContext(icxt);

  return(icxt);
}

/*
 * 候補一覧行を作る
 */
selectOne(d, buf, ck, nelem, bangomax, inhibit, currentkouho, allowcallback,
	  everyTimeCallback, exitCallback, quitCallback, auxCallback)
uiContext d;
unsigned char **buf;
int *ck;
int nelem, bangomax;
unsigned char inhibit;
int currentkouho;
int allowcallback;
int (*everyTimeCallback)(), (*exitCallback)();
int (*quitCallback)(), (*auxCallback)();
{
  extern KanjiModeRec ichiran_mode;
  ichiranContext ic;
  int retval = 0;

  if (allowcallback != WITHOUT_LIST_CALLBACK &&
      d->list_func == (void (*)())0) {
    allowcallback = WITHOUT_LIST_CALLBACK;
  }

  if(pushCallback(d, d->modec,
	everyTimeCallback, exitCallback, quitCallback, auxCallback) == 0) {
    necKanjiError = "malloc (pushCallback) できませんでした";
    return(NG);
  }
  
  if((ic = newIchiranContext()) == (ichiranContext)NULL) {
    popCallback(d);
    return(NG);
  }
  ic->majorMode = d->majorMode;
  ic->next = d->modec;
  d->modec = (mode_context)ic;

  ic->prevMode = d->current_mode;
  d->current_mode = &ichiran_mode;

  ic->allkouho = buf;
  ic->curIkouho = ck;
  ic->inhibit = inhibit;
  ic->nIkouho = nelem;

  if (allowcallback != WITHOUT_LIST_CALLBACK) {
    ic->flags |= ICHIRAN_ALLOW_CALLBACK;
    ic->inhibit |= NUMBERING;
  }

  if (allowcallback == WITHOUT_LIST_CALLBACK) {
    if ((retval = makeKouhoIchiran(d, nelem, bangomax, inhibit,
				  currentkouho))   == NG) {
      popIchiranMode(d);
      popCallback(d);
      return(NG);
    }
  }
  else {
    d->list_func(d->client_data, CANNA_LIST_Start, buf, nelem, ck);
  }

  return(0);
}

/*
 * IchiranContext の初期化
 */
static
clearIchiranContext(p)
ichiranContext p;
{
  p->id = ICHIRAN_CONTEXT;
  p->svIkouho = 0;
  p->curIkouho = 0;
  p->nIkouho = 0;
  p->tooSmall = 0;
  p->curIchar = 0;
  p->allkouho = 0;
  p->glinebufp = 0;
  p->kouhoifp = (kouhoinfo *)0;
  p->glineifp = (glineinfo *)0;
  p->flags = (unsigned char)0;
}
  
/*
 * 候補一覧のデータ構造体を作るための領域を確保する
 */
allocIchiranBuf(d, inhibit, flag)
uiContext d;
unsigned char inhibit;
int flag;
{
  ichiranContext ic = (ichiranContext)d->modec;
  unsigned char **work;
  int length = 0, size;

  /* サイズの分と番号の分の領域を得る*/
  size = ic->nIkouho * (d->ncolumns + 1); /* えいやっ */
  if((ic->glinebufp = (unsigned char *)malloc(size)) == 
     (unsigned char *)NULL) {
    necKanjiError = "malloc (allocIchiranBuf) できませんでした";
    return(NG);
  }

  /* kouhoinfoの領域を得る */
  if((ic->kouhoifp = 
      (kouhoinfo *)malloc((ic->nIkouho + 1) * sizeof(kouhoinfo))) ==
     (kouhoinfo *)NULL) {
    necKanjiError = "malloc (allocIchiranBuf) できませんでした";
    free(ic->glinebufp);
    return(NG);
  }

  /* glineinfoの領域を得る */
  if((ic->glineifp = 
      (glineinfo *)malloc((ic->nIkouho + 1) * sizeof(glineinfo))) ==
     (glineinfo *)NULL) {
    necKanjiError = "malloc (allocIchiranBuf) できませんでした";
    free(ic->glinebufp);
    free(ic->kouhoifp);
    return(NG);
  }
}

/*
 * 候補一覧行を表示用のデータをテーブルに作成する
 *
 * ・glineinfo と kouhoinfoを作成する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
makeKouhoIchiran(d, nelem, bangomax, inhibit, currentkouho)
uiContext d;
int nelem, bangomax;
unsigned char inhibit;
int currentkouho;
{
  ichiranContext ic = (ichiranContext)d->modec;
  unsigned char **kkptr, *kptr, *gptr, *bptr, *svgptr, **work;
  int           i, ko, lnko, cn = 0, svcn, line = 0;
  RkStat	st;

  ic->nIkouho = nelem;	/* 候補の数 */

  /* カレント候補をセットする */
  ic->svIkouho = *(ic->curIkouho);
  *(ic->curIkouho) += currentkouho;
  if(*(ic->curIkouho) >= ic->nIkouho)
    ic->svIkouho = *(ic->curIkouho) = 0;

#ifdef DEBUG
  if(iroha_debug) {
    printf("<makeKouhoIchiran> nelem = %d, currentkouho = %d\n", nelem, 
	   currentkouho);
    printf("tooSmall = %d\n", ic->tooSmall);
    printf("<makeKouhoIchiran> buf\n");
    for(work = ic->allkouho; *work; work++)
      printf("-> %s ", *work);
  }
#endif

  if(allocIchiranBuf(d, inhibit, 1) == NG)
    return(NG);

  if(d->ncolumns < 2) {
    ic->tooSmall = 1;
    return(0);
  }

  /* glineinfoとkouhoinfoを作る */
  /* 
   ＊glineinfo＊
      int glkosu   : int glhead     : int gllen  : unsigned char *gldata
      １行の候補数 : 先頭候補が     : １行の長さ : 候補一覧行の文字列
                   : 何番目の候補か :
   -------------------------------------------------------------------------
   0 | 6           : 0              : 24         : １新２心３進４真５神６信
   1 | 4           : 6              : 16         : １臣２寝３伸４芯

    ＊kouhoinfo＊
      int khretsu  : int khpoint  : unsigned char *khdata
      なん列目に   : 行の先頭から : 候補の文字列
      ある候補か   : 何バイト目か :
   -------------------------------------------------------------------------
   0 | 0           : 0            : 新
   1 | 0           : 4            : 心
             :                :             :
   7 | 1           : 0            : 臣
   8 | 1           : 4            : 寝
  */

  kkptr = ic->allkouho;
  kptr = *(ic->allkouho);
  gptr = ic->glinebufp;

  /* line -- 何列目か
     ko   -- 全体の先頭から何番目の候補か
     lnko -- 列の先頭から何番目の候補か
     cn   -- 列の先頭から何バイト目か */

  for(line=0, ko=0; ko<ic->nIkouho; line++) {
    ic->glineifp[line].gldata = gptr; /* 候補行を表示するための文字列 */
    ic->glineifp[line].glhead = ko;   /* この行の先頭候補は、全体でのko番目 */

    for(lnko=0, cn=0;
	cn<d->ncolumns - (kCount ? ICHISIZE + 1 : 0) &&
	lnko<bangomax && ko<ic->nIkouho ; lnko++, ko++) {
      kptr = kkptr[ko];
      ic->kouhoifp[ko].khretsu = line; /* 何行目に存在するかを記録 */
      if(lnko == 0)
	ic->kouhoifp[ko].khpoint = cn;   /* その行で何バイト目かを記録 */
      else 
	ic->kouhoifp[ko].khpoint = cn + 2;   /* その行で何バイト目かを記録 */
      ic->kouhoifp[ko].khdata = kptr;  /* その文字列へのポインタ */
      svgptr = gptr;
      svcn = cn;

      if(!(inhibit & (unsigned char)NUMBERING)) {
	/* 番号をコピーする */
	if(lnko == 0) {
	  strncpy(gptr, bango, 2);
	  cn += 2; gptr += 2;
	} else {
	  strncpy(gptr, bango+(2 + BANGOSIZE * (lnko - 1)), BANGOSIZE);
	  cn += BANGOSIZE; gptr += BANGOSIZE;
	}
      } else {
	/* 空白をコピーする */
	if(lnko) {
	  strncpy(gptr, kuuhaku, KG_KUHAKUSIZE);
	  cn += KG_KUHAKUSIZE; gptr += KG_KUHAKUSIZE;
	}
      }
      /* 候補をコピーする */
      for(; *kptr && cn<d->ncolumns - (kCount ? ICHISIZE + 1 : 0);
	  gptr++, kptr++, cn++) {
	*gptr = *kptr;
      }

      /* カラム数よりはみだしてしまいそうになったので１つ戻す */
      if ((cn >= d->ncolumns - (kCount ? ICHISIZE + 1 : 0)) && *kptr) {
	if (lnko) {
	  gptr = svgptr;
	  cn = svcn;
	  break;
	}
	else {
	  ic->tooSmall = 1;
	  return(0);
	}
      }
    }
    if (kCount) {
      while (cn < d->ncolumns - 1) {
	*gptr++ = (unsigned char)' ';
	cn++;
      }
    }
    /* １行終わり */
    *gptr++ = (unsigned char)NULL;
    ic->glineifp[line].glkosu = lnko;
    ic->glineifp[line].gllen = strlen(ic->glineifp[line].gldata);
  }
  /* 最後にNULLを入れる */
  ic->kouhoifp[ko].khretsu = (int)NULL;
  ic->kouhoifp[ko].khpoint = (int)NULL;
  ic->kouhoifp[ko].khdata  = (unsigned char *)NULL;
  ic->glineifp[line].glkosu  = (int)NULL;
  ic->glineifp[line].glhead  = (int)NULL;
  ic->glineifp[line].gllen   = (int)NULL;
  ic->glineifp[line].gldata  = (unsigned char *)NULL;

#ifdef DEBUG
  if (iroha_debug) {
    for(i=0; ic->glineifp[i].glkosu; i++)
      printf("%d: %s\n", i, ic->glineifp[i].gldata);
  }
#endif

  return(0);
}

/*
 * 候補一覧行を表示する
 *
 * ・候補一覧表示のためのデータをテーブルに作成する
 * ・候補一覧表示行が狭いときは、一覧を表示しないで次候補をその場に表示する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */

TanKouhoIchiran(d)
uiContext d;
{
  return tanKouhoIchiran(d, 1);
}

tanKouhoIchiran(d, step)
uiContext d;
int step;
{
  henkanContext hc = (henkanContext)d->modec;
  ichiranContext ic;
  int nelem, currentkouho, retval = 0;
  unsigned char inhibit = 0;
  unsigned char listcallback = (unsigned char)(d->list_func ? 1 : 0);

  unsigned char **getIchiranList();

  /* 候補一覧行が狭くて候補一覧が出せない */
  if(listcallback == 0 && d->ncolumns - (kCount ? ICHISIZE + 1 : 0) < 4) {
    /* tooSmall */
    return(TanNextKouho(d));
  }

  /* すべての候補を取り出す */
  if((hc->allkouho = 
      getIchiranList(d, hc->context, &nelem, &currentkouho)) == 0) {
    necKanjiPipeError();
    strcpy(d->genbuf, necKanjiError);
    d->nbytes = TanKakutei(d);
    makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
    return d->nbytes;
  }

  if ( !HexkeySelect )
    inhibit |= (unsigned char)NUMBERING;

  hc->curIkouho = currentkouho;	/* 現在のカレント候補番号を保存する */
  currentkouho = step;	/* カレント候補から何番目をカレント候補とするか */

  /* 候補一覧に移行する */
  if((retval = selectOne(d, hc->allkouho, &hc->curIkouho, nelem, BANGOMAX,
               inhibit, currentkouho, WITH_LIST_CALLBACK,
	       ichiranEveryTimeCatch, ichiranExitCatch,
	       ichiranQuitCatch, 0)) == NG) {
    freeGetIchiranList(hc->allkouho);
    return(GLineNGReturn(d));
  }

  ic = (ichiranContext)d->modec;
  if(ic->tooSmall) {
    freeGetIchiranList(hc->allkouho);
    popIchiranMode(d);
    popCallback(d);
    return(TanNextKouho(d));
  }

  ic->minorMode = IROHA_MODE_IchiranMode;
  currentModeInfo(d);

  if ( !(ic->flags & ICHIRAN_ALLOW_CALLBACK) ) {
    makeGlineStatus(d);
  }
  /* d->status = EVERYTIME_CALLBACK; */

  return(retval);
}

/*
 * 候補一覧行の表示を強制終了する
 */
IchiranQuit(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
  int retval = 0;

  if (ic->flags & ICHIRAN_ALLOW_CALLBACK &&
      d->list_func) {
    d->list_func(d->client_data, CANNA_LIST_Quit, 0, 0, 0);
  }

  *(ic->curIkouho) = ic->nIkouho - 1; /* ひらがな候補にする */

  ichiranFin(d);

  d->status = QUIT_CALLBACK;

  return(retval);
}

/*
 * 次候補に移動する
 *
 * ・カレント候補が最終候補だったら先頭候補をカレント候補とする
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
IchiranForwardKouho(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
  int mode;

  if (ic->flags & ICHIRAN_ALLOW_CALLBACK &&
      d->list_func) {
    d->list_func(d->client_data, CANNA_LIST_Forward, 0, 0, 0);
    return 0;
  }

  /* 次候補にする (単語候補一覧状態で、最後の候補だったら一覧をやめる) */
  *(ic->curIkouho) += 1;
  if(*(ic->curIkouho) >= ic->nIkouho) {
    if(QuitIchiranIfEnd && (getMinorMode(d) == IROHA_MODE_IchiranMode)) {
      return(IchiranQuit(d));
    } else if(CursorWrap) {
      *(ic->curIkouho) = 0;
    } else {
      *(ic->curIkouho) -= 1;
      return NothingChangedWithBeep(d);
    }
  }

  if(ic->tooSmall) { /* for bushu */
    d->status = AUX_CALLBACK;
    return 0;
  }

  makeGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return 0;
}

/*
 * 前候補に移動する
 *
 * ・カレント候補が先頭候補だったら最終候補をカレント候補とする
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
IchiranBackwardKouho(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
  int mode;

  if (ic->flags & ICHIRAN_ALLOW_CALLBACK &&
      d->list_func) {
    d->list_func(d->client_data, CANNA_LIST_Backward, 0, 0, 0);
    return 0;
  }

  /* 現在のモードを求める */
  if(QuitIchiranIfEnd) {
    mode = getMinorMode(d);
  }

  /* 前候補にする (単語候補一覧状態で、最初の候補だったら一覧をやめる) */
  if(*(ic->curIkouho))
    *(ic->curIkouho) -= 1;
  else {
    if(QuitIchiranIfEnd && (mode == IROHA_MODE_IchiranMode)) {
      return(IchiranQuit(d));
    } else if(CursorWrap) {
      *(ic->curIkouho) = ic->nIkouho - 1;
    } else {
      *(ic->curIkouho) = 0;
      return NothingChangedWithBeep(d);
    }
  }

  if(ic->tooSmall) { /* for bushu */
    d->status = AUX_CALLBACK;
    return 0;
  }

  makeGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return 0;
}

/*
 * 前候補列に移動する
 *
 * ・カレント候補を求めて候補一覧とその場の候補を表示する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
IchiranPreviousKouhoretsu(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;

  if (ic->flags & ICHIRAN_ALLOW_CALLBACK &&
      d->list_func) {
    d->list_func(d->client_data, CANNA_LIST_Prev, 0, 0, 0);
    return 0;
  }

  if(ic->tooSmall) { /* for bushu */
    return(IchiranBackwardKouho(d));
  }

  /* 前候補列にする (*(ic->curIkouho)を求める)*/
  getIchiranPreviousKouhoretsu(d);

  makeGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return 0;
}

/*
 * 前候補列のカレント候補を求める
 *
 * ・前候補列中の同じ候補番号のものをカレント候補とする
 * ・候補番号の同じものがない時は、その候補中の最終候補をカレント候補とする
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
getIchiranPreviousKouhoretsu(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
  int index;
  int curretsu, nretsu;

  /* カレント候補行のなかで何番目の候補かなのかを得る */
  index = *(ic->curIkouho) - 
    ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].glhead;
  /* 前候補列を得る */
  curretsu = ic->kouhoifp[*(ic->curIkouho)].khretsu;
  nretsu = ic->kouhoifp[ic->nIkouho - 1].khretsu + 1;
  if(curretsu == 0) {
    if(CursorWrap)
      curretsu = nretsu;
    else
      return NothingChangedWithBeep(d);
  }
  curretsu -= 1;
  /* index がカレント候補列の候補数より大きくなってしまったら
     最右候補をカレント候補とする */
  if(ic->glineifp[curretsu].glkosu <= index) 
    index = ic->glineifp[curretsu].glkosu - 1;
  /* 前候補列の同じ番号に移動する */
  *(ic->curIkouho) = index + ic->glineifp[curretsu].glhead;
}

/*
 * 次候補列に移動する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
IchiranNextKouhoretsu(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;

  if (ic->flags & ICHIRAN_ALLOW_CALLBACK &&
      d->list_func) {
    d->list_func(d->client_data, CANNA_LIST_Next, 0, 0, 0);
    return 0;
  }

  if(ic->tooSmall) {
    return(IchiranForwardKouho(d));
  }

  /* 次候補列にする (*(ic->curIkouho) を求める) */
  getIchiranNextKouhoretsu(d);

  makeGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return 0;
}

/*
 * 次候補列に移動する
 *
 * ・次候補列中の同じ候補番号のものをカレント候補とする
 * ・候補番号の同じものがない時は、その候補中の最終候補をカレント候補とする
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
getIchiranNextKouhoretsu(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
  int index;
  int curretsu, nretsu;

  /* カレント候補行のなかで何番目の候補かなのかを得る */
  index = *(ic->curIkouho) - 
    ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].glhead;
  /* 次候補列を得る */
  curretsu = ic->kouhoifp[*(ic->curIkouho)].khretsu;
  nretsu = ic->kouhoifp[ic->nIkouho - 1].khretsu + 1;
  curretsu += 1;
  if(curretsu >= nretsu) {
    if(CursorWrap)
      curretsu = 0;
    else {
      return NothingChangedWithBeep(d);
    }
  }
  /* index がカレント候補列の候補数より大きくなってしまったら
     最右候補をカレント候補とする */
  if(ic->glineifp[curretsu].glkosu <= index) 
    index = ic->glineifp[curretsu].glkosu - 1;
  /* 前候補列の同じ番号に移動する */
  *(ic->curIkouho) = index + ic->glineifp[curretsu].glhead;
}

/*
 * 候補行中の先頭候補に移動する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
IchiranBeginningOfKouho(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;

  if (ic->flags & ICHIRAN_ALLOW_CALLBACK &&
      d->list_func) {
    d->list_func(d->client_data, CANNA_LIST_BeginningOfLine, 0, 0, 0);
    return 0;
  }

  if(ic->tooSmall) {
    d->status = AUX_CALLBACK;
    return 0;
  }

  /* 候補列の先頭候補をカレント候補にする */
  *(ic->curIkouho) = 
    ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].glhead;

  makeGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return 0;
}

/*
 * 候補行中の最右候補に移動する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
IchiranEndOfKouho(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;

  if (ic->flags & ICHIRAN_ALLOW_CALLBACK &&
      d->list_func) {
    d->list_func(d->client_data, CANNA_LIST_EndOfLine, 0, 0, 0);
    return 0;
  }

  if(ic->tooSmall) {
    d->status = AUX_CALLBACK;
    return 0;
  }

  /* 候補列の最右候補をカレント候補にする */
  *(ic->curIkouho) = 
    ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].glhead
    + ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].glkosu - 1;

  makeGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return 0;
}

/*
 * 候補行中の入力された番号の候補に移動する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
IchiranBangoKouho(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
  int zflag, retval = 0;

  if(ic->tooSmall) {
    d->status = AUX_CALLBACK;
    return(retval);
  }

  /* d->status = EVERYTIME_CALLBACK; */

  if (HexkeySelect && !(ic->inhibit & NUMBERING)) {
    /* 入力された番号の候補をカレント候補とする */
    if((zflag = getIchiranBangoKouho(d)) == NG)
      return NothingChangedWithBeep(d);

    /* SelectDirect のカスタマイズの処理 */
    if(SelectDirect) /* ON */ {
      if(zflag) /* ０が入力された */
	retval = IchiranQuit(d);
      else
	retval = IchiranKakutei(d);
    } else {          /* OFF */
      makeGlineStatus(d);
      /* d->status = EVERYTIME_CALLBACK; */
    }
    return(retval);
  }
  else {
    if(!(ic->inhibit & CHARINSERT)) {
      retval = IchiranKakutei(d);
      d->more.todo = 1;
      d->more.ch = d->ch;
      d->more.fnum = IROHA_FN_FunctionalInsert;
    } else {
      NothingChangedWithBeep(d);
    }
    return(retval);
  }
}

/*
 * 候補行中の入力された番号の候補に移動する
 *
 *
 * 引き数	uiContext
 * 戻り値	０が入力されたら              １を返す
 * 		１〜９、ａ〜ｆが入力されたら  ０を返す
 * 		エラーだったら              ー１を返す
 */
getIchiranBangoKouho(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
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
    return(NG);
  }
  /* 入力データは 候補行の中に存在する数か？ */
  if(num > ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].glkosu) {
    /* 入力された番号は正しくありません */
    return(NG);
  }

  /* 入力された数が０で SelectDirect が ON なら読みに戻して１を返す */
  if(num == 0) {
    if(SelectDirect)
      return(1);
    else {
      /* 入力された番号は正しくありません */
      return(NG);
    }  
  } else {
    /* 候補列の先頭候補を得る */
    index = ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].glhead;
    *(ic->curIkouho) = index + (num - 1);
  }

  return(0);
}

/*
 * 候補行中から選択された候補をカレント候補とする
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
IchiranKakutei(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
  int retval = 0;
  unsigned char *kakuteiStrings;

  if (ic->flags & ICHIRAN_ALLOW_CALLBACK &&
      d->list_func) {
    d->list_func(d->client_data, CANNA_LIST_Select, 0, 0, 0);
  }

  kakuteiStrings = ic->allkouho[*(ic->curIkouho)];
  retval = d->nbytes = strlen(kakuteiStrings);
  strcpy(d->buffer_return, kakuteiStrings);

  ichiranFin(d);

  d->status = EXIT_CALLBACK;

  return(retval);
}

/*
 * 候補行表示モードから抜ける
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
ichiranFin(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;

  /* 候補一覧表示行用のエリアをフリーする */
  freeIchiranBuf(ic);

  popIchiranMode(d);

  /* gline をクリアする */
  GlineClear(d);
}

#include	"ichiranmap.c"
