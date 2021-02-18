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

static	char	rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/onoff.c,v 3.3 91/07/30 13:28:19 kon Exp $";

#include	<errno.h>
#include	"iroha.h"

#define ICHISIZE 9

extern int kCount;

static unsigned char *black = (unsigned char *)"◎";
static unsigned char *white = (unsigned char *)"○";
static unsigned char *space = (unsigned char *)"　";

static int makeOnOffIchiran();

static void
popOnOffMode(d)
uiContext d;
{
  ichiranContext oc = (ichiranContext)d->modec;

  d->modec = oc->next;
  d->current_mode = oc->prevMode;
  freeIchiranContext(oc);
}

/*
 * 候補一覧行を作る
 */
selectOnOff(d, buf, ck, nelem, bangomax, currentkouho, status,
	  everyTimeCallback, exitCallback, quitCallback, auxCallback)
uiContext d;
unsigned char **buf;
int *ck;
int nelem, bangomax;
int currentkouho;
unsigned char *status;
int (*everyTimeCallback)(), (*exitCallback)();
int (*quitCallback)(), (*auxCallback)();
{
  extern KanjiModeRec onoff_mode;
  ichiranContext oc;
  int retval = 0;
  ichiranContext newIchiranCotext();

  if(pushCallback(d, d->modec,
	everyTimeCallback, exitCallback, quitCallback, auxCallback) == 0) {
    necKanjiError = "malloc (pushCallback) できませんでした";
    return(NG);
  }
  
  if((oc = (ichiranContext)newIchiranContext()) == NULL) {
    popCallback(d);
    return(NG);
  }
  oc->next = d->modec;
  d->modec = (mode_context)oc;

  oc->prevMode = d->current_mode;
  d->current_mode = &onoff_mode;

  oc->allkouho = buf;
  oc->curIkouho = ck;

  if((retval = makeOnOffIchiran(d, nelem, bangomax,
			currentkouho, status))   == NG) {
    popOnOffMode(d);
    popCallback(d);
    return(NG);
  }
  return(retval);
}

/*
 * 候補一覧行を表示用のデータをテーブルに作成する
 *
 * ・glineinfo と kouhoinfoを作成する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
makeOnOffIchiran(d, nelem, bangomax, currentkouho, status)
uiContext d;
int nelem, bangomax;
int currentkouho;
unsigned char *status;
{
  ichiranContext oc = (ichiranContext)d->modec;
  unsigned char **kkptr, *kptr, *gptr, *bptr, *svgptr, **work;
  int           i, ko, lnko, cn = 0, svcn, line = 0;
  RkStat	st;

  oc->nIkouho = nelem;	/* 候補の数 */

  /* カレント候補をセットする */
  oc->svIkouho = *(oc->curIkouho);
  *(oc->curIkouho) += currentkouho;
  if(*(oc->curIkouho) >= oc->nIkouho)
    oc->svIkouho = *(oc->curIkouho) = 0;

#ifdef DEBUG
  if(iroha_debug) {
    printf("<makeKouhoIchiran> nelem = %d, currentkouho = %d\n", nelem, 
	   currentkouho);
    printf("tooSmall = %d\n", oc->tooSmall);
    printf("<makeKouhoIchiran> buf\n");
    for(work = oc->allkouho; *work; work++)
      printf("-> %s ", *work);
  }
#endif

  if(allocIchiranBuf(d, 0, 1) == NG)
    return(NG);

  if(d->ncolumns < 2) {
    oc->tooSmall = 1;
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

  kkptr = oc->allkouho;
  kptr = *(oc->allkouho);
  gptr = oc->glinebufp;

  /* line -- 何列目か
     ko   -- 全体の先頭から何番目の候補か
     lnko -- 列の先頭から何番目の候補か
     cn   -- 列の先頭から何バイト目か */

  for(line=0, ko=0; ko<oc->nIkouho; line++) {
    oc->glineifp[line].gldata = gptr; /* 候補行を表示するための文字列 */
    oc->glineifp[line].glhead = ko;   /* この行の先頭候補は、全体でのko番目 */

    for(lnko=0, cn=0;
	cn<d->ncolumns - (kCount ? ICHISIZE + 1: 0) &&
	lnko<bangomax && ko<oc->nIkouho ; lnko++, ko++) {
      kptr = kkptr[ko];
      oc->kouhoifp[ko].khretsu = line; /* 何行目に存在するかを記録 */
      if(lnko == 0)
	oc->kouhoifp[ko].khpoint = cn;   /* その行で何バイト目かを記録 */
      else 
	oc->kouhoifp[ko].khpoint = cn + 2;   /* その行で何バイト目かを記録 */
      oc->kouhoifp[ko].khdata = kptr;  /* その文字列へのポインタ */
      svgptr = gptr;
      svcn = cn;

      /* ◎か○をコピーする */
      if(lnko) {
	strcpy(gptr, space, 2);
	cn += 2; gptr += 2;
      }
      if(status[ko] == (unsigned char)1)
	strncpy(gptr, black, 2);
      else
	strncpy(gptr, white, 2);
      cn += 2; gptr += 2;
      /* 候補をコピーする */
      for(; *kptr && cn<d->ncolumns - (kCount ? ICHISIZE + 1: 0);
	  gptr++, kptr++, cn++) {
	*gptr = *kptr;
      }

      /* カラム数よりはみだしてしまいそうになったので１つ戻す */
      if ((cn >= d->ncolumns - (kCount ? ICHISIZE + 1: 0)) && *kptr) {
	if (lnko) {
	  gptr = svgptr;
	  cn = svcn;
	  break;
	}
	else {
	  oc->tooSmall = 1;
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
    oc->glineifp[line].glkosu = lnko;
    oc->glineifp[line].gllen = strlen(oc->glineifp[line].gldata);
  }
  /* 最後にNULLを入れる */
  oc->kouhoifp[ko].khretsu = (int)NULL;
  oc->kouhoifp[ko].khpoint = (int)NULL;
  oc->kouhoifp[ko].khdata  = (unsigned char *)NULL;
  oc->glineifp[line].glkosu  = (int)NULL;
  oc->glineifp[line].glhead  = (int)NULL;
  oc->glineifp[line].gllen   = (int)NULL;
  oc->glineifp[line].gldata  = (unsigned char *)NULL;

#ifdef DEBUG
  if (iroha_debug) {
    for(i=0; oc->glineifp[i].glkosu; i++)
      printf("%d: %s\n", i, oc->glineifp[i].gldata);
  }
#endif

  return(0);
}

/*
 * カレント候補を現在と反対にする(ON→OFF, OFF→ON)
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
OnOffSelect(d)
uiContext d;
{
  ichiranContext oc = (ichiranContext)d->modec;
  mountContext mc = (mountContext)oc->next;
  int point, retval = 0;
  unsigned char *gline;

  /* mountNewStatus を変更する (1→0, 0→1) */
  if(mc->mountNewStatus[*(oc->curIkouho)])
    mc->mountNewStatus[*(oc->curIkouho)] = 0;
  else
    mc->mountNewStatus[*(oc->curIkouho)] = 1;

  /* gline用のデータを書き換える (◎→○, ○→◎) */
  gline = oc->glineifp[oc->kouhoifp[*(oc->curIkouho)].khretsu].gldata;
  point = oc->kouhoifp[*(oc->curIkouho)].khpoint;

  if(mc->mountNewStatus[*(oc->curIkouho)])
    strncpy(gline + point, black, 2);
  else
    strncpy(gline + point, white, 2);

  makeGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return(retval);
}

/*
 * status をそのまま返し、OnOffモードをPOPする (EXIT_CALLBACK)
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
OnOffKakutei(d)
uiContext d;
{
  ichiranContext oc = (ichiranContext)d->modec;
  int retval = 0;
  unsigned char *kakuteiStrings;

  /* 候補一覧表示行用のエリアをフリーする */
  freeIchiranBuf(oc);

  popOnOffMode(d);

#ifdef DEBUG
  if(iroha_debug) {
    mountContext mc = (mountContext)d->modec;
    int i;

    printf("<★mount>\n");
    for(i= 0; mc->mountList[i]; i++)
      printf("[%s][%x][%x]\n", mc->mountList[i],
	     mc->mountOldStatus[i], mc->mountNewStatus[i]);
    printf("\n");
  }
#endif

  /* gline をクリアする */
  GlineClear(d);

  d->status = EXIT_CALLBACK;

  return(retval);
}

#include	"onoffmap.c"
