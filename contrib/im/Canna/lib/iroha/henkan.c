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

static	char	rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/henkan.c,v 3.7 91/09/05 20:28:19 kon Exp $";

#include	<errno.h>
#include	<fcntl.h>
#include	"iroha.h"
#include	"RKintern.h"
#include	"ncommon.h"
#ifdef MEASURE_TIME
#include <sys/types.h>
#include <sys/times.h>
#endif

extern ckverbose;

dicMesg(s, d, f)
char *s, *d;
int f;
{
  if (ckverbose == CANNA_FULL_VERBOSE) {
    char buf[128];

    sprintf(buf, "\"%s\"", d);
    printf("%14s %-20s を指定しています。\n", s, buf);
  }
}

extern Gakushu;
extern CursorWrap;
extern char *RengoGakushu, *KatakanaGakushu;
int BunsetsuKugiri = 0;
int stayAfterValidate = 1;
int kakuteiIfEndOfBunsetsu = 0;

#define DICERRORMESGLEN 78
static char dictmp[DICERRORMESGLEN];
static char *mountErrorMessage = "をマウントできませんでした";

int kouho_threshold = 2;

static int changeCurBunsetsuString(), doYomiHenkan(), kanakanError();
static int bubunEveryTimeCatch(), quitBubun(), exitBubun();

#define  SENTOU        1

/*
 * BunsetsuKugiri (文節の区ぎりを半角空白にするフラグ) を操作する
 *
 * 引き数	なし
 * 戻り値	なし
 */
SetBunsetsuKugiri(x)
int x;
{

  BunsetsuKugiri = x;
}

#define IROHA_SERVER_NAME_LEN 128
static char iroha_server_name[IROHA_SERVER_NAME_LEN] = {0, 0};

RkSetServerName(s)
char *s;
{
  if (s)
    strncpy(iroha_server_name, s, IROHA_SERVER_NAME_LEN);
  else
    iroha_server_name[0] = '\0';
}

static
RkInitError()
{
  extern errno;

  if (errno == EPIPE) {
    necKanjiError = "irohaserverにコネクトできませんでした";
  }
  else {
    necKanjiError = "かな漢字変換辞書の初期化に失敗しました";
  }
  addWarningMesg(necKanjiError);
  RkFinalize();
}

static void
mountError(dic)
char *dic;
{
  int mnterrlen;
  if (DICERRORMESGLEN < 
      strlen(dic) +
      (mnterrlen = strlen(mountErrorMessage)) + 1) {
    strncpy(dictmp, dic, DICERRORMESGLEN - mnterrlen - 3/* ... */ - 1);
    strcpy(dictmp + DICERRORMESGLEN - mnterrlen - 3 - 1, "...");
    strcpy(dictmp + DICERRORMESGLEN - mnterrlen - 1, mountErrorMessage);
  }
  else {
    sprintf(dictmp, "%s%s", dic, mountErrorMessage);
  }
  necKanjiError = dictmp;
  addWarningMesg(dictmp);
}

/*
 * かな漢字変換のための初期処理
 *
 * ・RkInitializeを呼んで、defaultContext を作成する
 * ・defaultBushuContext を作成する
 * ・辞書のサーチパスを設定する
 * ・システム辞書、部首用辞書、ユーザ辞書をマウントする
 *
 * 引き数	なし
 * 戻り値	0:まあ正常、 -1:とことん不良
 */
KanjiInit()
{
  int i;
  char **p;
  char *ptr, *getenv();

  /* 連文節ライブラリを初期化する */

  if (iroha_server_name[0]) {
    if ((defaultContext =RkInitialize(iroha_server_name)) == -1) {
      RkInitError();
      return -1;
    }
  }
  else if ((ptr = getenv("IROHADICDIR")) == (char *)NULL) {
    if((defaultContext = RkInitialize("/usr/lib/iroha/dic")) == -1) {
      RkInitError();
      return -1;
    }
  }
  else {
    if((defaultContext = RkInitialize(ptr)) == -1) {
      RkInitError();
      return -1;
    }
  }
  if(defaultContext != -1) {
    if((defaultBushuContext = RkCreateContext()) == -1) {
      necKanjiError = "部首用のコンテクストを作成できませんでした";
      addWarningMesg(necKanjiError);
      defaultContext = -1;
      RkFinalize();
      return -1;
    }
  } else {
    defaultBushuContext = -1;
  }

  debug_message("デフォルトコンテキスト(%d), 部首コンテキスト(%d)\n",
		defaultContext, defaultBushuContext, 0);

  if (defaultContext != -1) {
    if((ptr = getenv("IROHADICPATH")) == (char *)NULL) {
      if((RkSetDicPath(defaultContext, "iroha")) == -1) {
	necKanjiError = "辞書のディレクトリを設定できませんでした";
	RkFinalize();
	return(NG);
      }
      if((RkSetDicPath(defaultBushuContext, "iroha")) == -1) {
	necKanjiError = "辞書のディレクトリを設定できませんでした";
	RkFinalize();
	return(NG);
      }
    }
    else {
      if((RkSetDicPath(defaultContext, ptr)) == -1) {
	necKanjiError = "辞書のディレクトリを設定できませんでした";
	RkFinalize();
	return(NG);
      }
      if((RkSetDicPath(defaultBushuContext, ptr)) == -1) {
	necKanjiError = "辞書のディレクトリを設定できませんでした";
	RkFinalize();
	return(NG);
      }
    }

    /* システム辞書のマウント */
    for (i=0, p=kanjidicname ; i<nkanjidics ; i++, p++) {
#ifdef DEBUG
      if (iroha_debug) {
	fprintf(stderr, "use dictionary(%s)\n", *p);
      }
#endif
      if(RkMountDic(defaultContext, *p, 0) == -1) {
#ifdef DEBUG
	if (iroha_debug) {
	  fprintf(stderr, "kanjidicname = %s\n", *p);
	}
#endif
	mountError(*p);
      }
      dicMesg("システム辞書", *p, 1);
    }
    
    /* 部首用辞書のマウント */
    for (i=0, p=bushudicname; i<nbushudics; i++, p++) {
#ifdef DEBUG
      if (iroha_debug) {
	fprintf(stderr, "use bushudic(%s)\n", *p);
      }
#endif
      if(RkMountDic(defaultBushuContext, *p, 0) == -1) {
#ifdef DEBUG
	if (iroha_debug) {
	  fprintf(stderr, "bushudicname = %s\n", *p);
	}
#endif
	mountError(*p);
      }
      dicMesg("部首辞書", *p, 1);
    }
    
    /* ユーザ辞書のマウント */
    for (p = userdicname ; *p ; p++) {
#ifdef DEBUG
      if (iroha_debug) {
	fprintf(stderr, "use userdic(%s)\n", *p);
      }
#endif
      if(RkMountDic(defaultContext, *p, 0) == -1) {
#ifdef DEBUG
	if (iroha_debug) {
	  fprintf(stderr, "userdicname = %s\n", *p);
	}
#endif
	if(strcmp(*p, "user"))
	  mountError(*p);
      }
      dicMesg("ユーザ辞書", *p, 0);
    }

    /* 連語辞書のマウント */
    if (RengoGakushu) {
      if(RkMountDic(defaultContext, RengoGakushu, 0) == -1) {
	mountError(RengoGakushu);
      }
      dicMesg("連語辞書", RengoGakushu, 0);
    }
  }
  return(0);
}

/*
 * かな漢字変換のための後処理
 *
 * ・システム辞書、部首用辞書、ユーザ辞書をアンマウントする
 * ・RkFinalizeを呼ぶ
 *
 * 引き数	なし
 * 戻り値	なし
 */
KanjiFin()
{
  int i;
  char buf[256];

  for (i = 0 ; i < nkanjidics ; i++) {
    /*
    if(RkUnmountDic(defaultContext, kanjidicname[i]) == -1) {
      sprintf(buf, "%s をアンマウントできませんでした。", kanjidicname[i]);
      addWarningMesg(buf);
    }
    */
    free(kanjidicname[i]);
  }

  for (i = 0 ; i < nbushudics ; i++) {
    /*
    if(RkUnmountDic(defaultBushuContext, bushudicname[i]) == -1) {
      sprintf(buf, "%s をアンマウントできませんでした。", bushudicname[i]);
      addWarningMesg(buf);
    }
    */
    free(bushudicname[i]);
  }

  for (i = 0 ; i < nuserdics ; i++) {
    /*
    if(RkUnmountDic(defaultContext, userdicname[i]) == -1) {
      printf(buf, "%s をアンマウントできませんでした。", userdicname[i]);
      addWarningMesg(buf);
    }
    */
    free(userdicname[i]);
  }

  for (i = 0 ; i < nlocaldics ; i++) {
    /*
    if(RkUnmountDic(defaultContext, localdicname[i]) == -1) {
      printf(buf, "%s をアンマウントできませんでした。", localdicname[i]);
      addWarningMesg(buf);
    }
    */
    free(localdicname[i]);
  }

  if(RengoGakushu)
    free(RengoGakushu);

  if(KatakanaGakushu)
    free(KatakanaGakushu);

  /* 連文節ライブラリを終了させる */
  RkFinalize();

  return(0);
}

/*
 * エコー用文字列の作成
 *
 * ・各文節の先頭からの位置を kugiri にセーブしながら
 *   d->echo_buffer にエコー用の文字列を作る
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
makeEchoStr(d)
uiContext        d;
{
  int  i, index;
  unsigned char *ptr;
  henkanContext hc = (henkanContext)d->modec;

  ptr = hc->echo_buffer;
  /* 先頭の文節から順に変換結果を得て、エコー用のバッファに入れる */
  /* 各文節の区ぎりをとっておく(先頭からのバイト数) */
  for(i=0, index=0; i < hc->nbunsetsu; i++) {
    if(RkGoTo(hc->context, i) == -1) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "文節の移動に失敗しました";
      return(NG);
    }
    hc->kugiri[i] = index;
    if(RkGetKanji(hc->context, (ptr + index), (ROMEBUFSIZE - index)) < 0) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "カレント候補を取り出せませんでした";
      return(NG);
    }
    index += strlen(ptr + index);
    if(BunsetsuKugiri && ((i + 1) < hc->nbunsetsu)) {
      *(ptr + index) = (unsigned char)' ';
      index++;
    }
  }  
  hc->kugiri[i] = index; /* 終わりの位置 */
  hc->kugiri[i+1] = (int)NULL;

  /* カレント文節に戻す */
  if(RkGoTo(hc->context, hc->curbun) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "カレント文節への移動に失敗しました";
    return(NG);
  }

  return(0);
}

/*
 * カレント文節の候補を更新する
 *
 * ・すでにある d->echo_buffer のうちのカレント文節の部分を変更する
 * ・これにともない kugiri も更新される
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
makeEchoStrCurChange(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;
  unsigned char tmpbuf[ROMEBUFSIZE];

  /* カレント候補を得る */
  if(RkGetKanji(hc->context, tmpbuf, ROMEBUFSIZE) < 0) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "カレント候補を取り出せませんでした";
    return(NG);
  }
  changeCurBunsetsuString(hc, tmpbuf, strlen(tmpbuf));
  return 0;
}

/*
 * カレント文節を指定された文字列に更新する
 *
 * ・すでにある d->echo_buffer のうちのカレント文節の部分を変更する
 * ・これにともない kugiri も更新される
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1(これはない)
 */
static
changeCurBunsetsuString(hc, str, len)
henkanContext	hc;
unsigned char   *str;
int             len;
{
  int  i, j, k, old_klen, ret;
  unsigned char *ptr;

  ptr = hc->echo_buffer;

  /* 旧カレント文節のバイト数 */
  old_klen = hc->kugiri[hc->curbun + 1] - hc->kugiri[hc->curbun];
  if(BunsetsuKugiri && (hc->curbun != (hc->nbunsetsu - 1)))
    old_klen -= 1;

  /* カレント候補を得る */
  /* カレント文節の長さ分空けるために、
     カレント文節の次の文節以降をずらす */
  i = hc->kugiri[hc->nbunsetsu] - hc->kugiri[hc->curbun + 1];
  /* ずらす文字列長 */ 

  if(len > old_klen) { /* 右にずらす*/
    j = hc->kugiri[hc->nbunsetsu]; /* NULL もコピーする*/
    k = len - old_klen;
    for(; i>=0; i--, j--) /* NULL もコピーする*/
      hc->echo_buffer[j + k] = hc->echo_buffer[j];
  }else if(len < old_klen) { /* 左にずらす*/
    j = hc->kugiri[hc->curbun + 1];
    k = old_klen - len;
    for(; i>=0; i--, j++) /* NULL もコピーする*/
      hc->echo_buffer[j - k] = hc->echo_buffer[j];
  }
  /* カレント候補を空けたところに入れる */ 
  ret = hc->kugiri[hc->curbun];
  strncpy(ptr + ret, str, len);
  if(BunsetsuKugiri && (hc->curbun != (hc->nbunsetsu - 1)))
    *(ptr + hc->kugiri[hc->curbun] + len) = (unsigned char)' ';

  /* 各文節の区ぎりをとっておく(先頭からのバイト数) */
  /* カレント文節以降差分をたす */
  for(i=hc->curbun; i<hc->nbunsetsu; i++)
    hc->kugiri[i + 1] += len - old_klen;

  return ret;
}

/*
 * カレント文節以降を更新する
 * ・すでにある d->echo_buffer のうちのカレント文節以降を変更する
 * ・これにともない kugiri も更新される
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
makeEchoStrCurUnderChange(d)
uiContext	d;
{
  int  i, index;
  unsigned char *ptr;
  henkanContext hc = (henkanContext)d->modec;

  ptr = hc->echo_buffer;
  /* カレント文節から順に変換結果を得て、エコー用のバッファに入れる */
  /* 各文節の区ぎりをとっておく(先頭からのバイト数) */
  for(i=hc->curbun, index=hc->kugiri[hc->curbun]; i < hc->nbunsetsu; i++) {
    if(RkGoTo(hc->context, i) == -1) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "文節の移動に失敗しました";
      return(NG);
    }
    hc->kugiri[i] = index;
    if(RkGetKanji(hc->context, (ptr + index), (ROMEBUFSIZE - index)) < 0) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "カレント候補を取り出せませんでした";
      return(NG);
    }
    index += strlen(ptr + index);
    if(BunsetsuKugiri && ((i + 1) < hc->nbunsetsu)) {
      *(ptr + index) = (unsigned char)' ';
      index++;
    }
  }  
  hc->kugiri[i] = index; /* 終わりの位置 */
  hc->kugiri[i+1] = (int)NULL;

  /* カレント文節に戻す */
  if(RkGoTo(hc->context, hc->curbun) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "カレント文節への移動に失敗しました";
    return(NG);
  }

  return(0);
}

static
confirmAllocJishubun(hc)
henkanContext hc;
{
  int i;

  if (hc->jishubun == 0) {
    hc->jishubun = (int *)calloc(MAXNBUNSETSU, sizeof(int));
    if (hc->jishubun == 0) {
      necKanjiError = "メモリが足りず字種変換できませんでした";
      return -1;
    }
    for (i = 0 ; i < MAXNBUNSETSU ; i++) {
      hc->jishubun[i] = 0;
    }
  }
  return 0;
}

int RkCvtHira(), RkCvtKana(), RkCvtHan();

static int (*kanaCnv[3])() = {
  RkCvtHira, /* JISHU_HIRA */
  RkCvtKana, /* JISHU_ZEN_KATA */
  RkCvtHan,  /* JISHU_HAN_KATA */
};
/* JISHU_HIRA、などのマクロの値が変わったときには上の配列を変えなくては
   ならない */

/*
 * カレント文節の候補をひらがな/カタカナにする
 *
 * ・すでにある d->echo_buffer のうちのカレント文節の部分を変更する
 * ・これにともない kugiri も更新される
 *
 * 引き数	uiContext, which は字種を表す。JISHU_HIRA, JISHU_ZEN_KATA
 *                                             JISHU_HAN_KATA のうちの
 *                          どれかでなければならない。範囲のチェックなし。
 * 戻り値	正常終了時 0	異常終了時 -1
 *
 */

static
makeEchoStrCurKana(d, which)
uiContext	d;
int which;
{
  henkanContext hc = (henkanContext)d->modec;
  unsigned char tmpbuf[ROMEBUFSIZE], tmpbuf2[ROMEBUFSIZE];
  int yspos, yepos;

  /* ひらがなをカレント候補とする */
  if (hc->kanaKugiri
      && ((yepos = hc->kanaKugiri[hc->curbun + 1]) >= 0)) {
    /* 前に RkGetYomi をしていたらその結果を使う */
    yspos = hc->kanaKugiri[hc->curbun];
    strncpy(tmpbuf2, hc->ycx->kana_buffer + yspos, yepos - yspos);
    tmpbuf2[yepos - yspos] = '\0';
  }
  else if (RkGetYomi(hc->context, tmpbuf2, ROMEBUFSIZE) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "読みを取り出せませんでした";
    return(NG);
  }
  if (confirmAllocJishubun(hc) < 0) {
    return -1;
  }
  hc->jishubun[hc->curbun] = which;
  RkNfer(hc->context);
  kanaCnv[which](tmpbuf, ROMEBUFSIZE, tmpbuf2, strlen(tmpbuf2));
  changeCurBunsetsuString(hc, tmpbuf, strlen(tmpbuf));
  return 0;
}

/*
  読みの長さをカウントして配列に入れておく

  配列はかな用のものとローマ字用のものがあり、それぞれ、

  １文節目の開始位置、２文節目の開始位置、３文節目の開始位置.....

  のようになっている。

  配列の各要素は最初 -1 に初期化される。すなわち、-1 になっている部分は
  読みの長さがまだカウントされていない部分である。

  引数: d                uiContext
        start_pos_return d->curbun の読みが始まる位置

  リターン値
        ０以外           読みの長さ
        ０               エラー

  エラーは次の場合考えられる。

   ・yc->kana_buffer の読みと hc->yomi_buffer が等しくない(字種漢字変換時など)
   ・区切りを入れておく配列が alloc できなかった。
   ・RkGoTo や RkGetYomi がエラーを返した

 */

static
countYomi(d, start_pos_return)
uiContext d;
int *start_pos_return;
{
  henkanContext hc = (henkanContext)d->modec;
  yomiContext yc = hc->ycx;
  int i, kp, rp, current;
  RkStat st;

  if (strcmp(hc->yomi_buffer, yc->kana_buffer)) {
    /* 読みとして与えられたものとカナバッファの内容が違う時は
       読みのカウントができません。 */
    return 0;
  }
  if (hc->kanaKugiri == 0) {
    hc->kanaKugiri = (int *)calloc(MAXNBUNSETSU, sizeof(int));
    if (hc->kanaKugiri == 0) {
      return 0;
    }
    hc->romajiKugiri = (int *)calloc(MAXNBUNSETSU, sizeof(int));
    if (hc->romajiKugiri == 0) {
      free(hc->kanaKugiri);
      return 0;
    }
    for (i = 0 ; i < MAXNBUNSETSU ; i++) {
      hc->kanaKugiri[i] = hc->romajiKugiri[i] = -1;
    }
    hc->kanaKugiri[0] = hc->romajiKugiri[0] = 0;
  }
  current = hc->curbun;
  if (hc->romajiKugiri[current] < 0
      || hc->romajiKugiri[current + 1] < 0) {
    /* 切り方がわからないので調べる。 */
    for (i = 0 ; i <= current ; i++) {
      if (hc->kanaKugiri[i + 1] < 0) {
	if (RkGoTo(hc->context, i) < 0) {
	  return 0;
	}
	if (RkGetStat(hc->context, &st) < 0) {
	  return 0;
	}
	hc->kanaKugiri[i + 1] = hc->kanaKugiri[i] + st.ylen;
      }
    }
    RkGoTo(hc->context, current);
    for (i = 0, kp = rp = 0 ; i <= current ;) {
      i++;
      do {
	if (yc->kAttr[++kp] & SENTOU) {
	  for (rp++ ; rp < yc->rEndp && !(yc->rAttr[rp] & SENTOU); rp++)
	    ;
	}
      } while (kp < hc->kanaKugiri[i]);
      hc->romajiKugiri[i] = rp;
    }
  }
  *start_pos_return = hc->romajiKugiri[current];
  return hc->romajiKugiri[current + 1] - *start_pos_return;
}

/*

  changeCurYomiLen -- 変換モードでしか呼んでは行けない

 */

static
changeCurYomiLen(d, kanalen, romajilen)
uiContext d;
int kanalen, romajilen;
{
  int dummy, koff, roff, i;
  henkanContext hc = (henkanContext)d->modec;

  countYomi(d, &dummy);

  koff = kanalen -
    (hc->kanaKugiri[hc->curbun + 1] - hc->kanaKugiri[hc->curbun]);
  roff = romajilen -
    (hc->romajiKugiri[hc->curbun + 1] - hc->romajiKugiri[hc->curbun]);

  for (i = hc->curbun + 1 ; i < MAXNBUNSETSU && hc->kanaKugiri[i] >= 0 ; i++) {
    hc->kanaKugiri[i] += koff;
    hc->romajiKugiri[i] += roff;
  }
}

static
makeEchoStrCurRomaji(d, zen)
uiContext	d;
int zen; /* 全角なら１、半角なら０ */
{
  henkanContext hc = (henkanContext)d->modec;
  yomiContext yc = hc->ycx;
  unsigned char tmpbuf[ROMEBUFSIZE], tmpbuf2[ROMEBUFSIZE];
  int len, pos;

  if ((len = countYomi(d, &pos)) == 0) {
    return NothingChangedWithBeep(d);
  }
  if (confirmAllocJishubun(hc) < 0) {
    return -1;
  }
  RkNfer(hc->context);
  strncpy(tmpbuf2, yc->romaji_buffer + pos, len);
  if (zen) {
    hc->jishubun[hc->curbun] = JISHU_ZEN_ALPHA;
    len = RkCvtZen(tmpbuf, ROMEBUFSIZE, tmpbuf2, len);
    changeCurBunsetsuString(hc, tmpbuf, len);
    return 0;
  }
  else {
    hc->jishubun[hc->curbun] = JISHU_HAN_ALPHA;
    changeCurBunsetsuString(hc, tmpbuf2, len);
    return 0;
  }
}

/*
 * かな漢字変換用の構造体の内容を更新する(その場のみ)
 *
 * ・d->echo_buffer をエコーし、カレント文節を反転する
 *
 * 引き数	uiContext
 * 戻り値	なし
 */
makeKanjiStatusReturn(d, hc)
uiContext	d;
henkanContext   hc;
{
  unsigned char tmp;

/*  d->kanji_status_return->info    = 0;*/
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

static
henkanError()
{
  necKanjiError = "変換に失敗しました。";
  return -1;
}

henkanContext
pushHenkanMode(d, yomi, yomilen, yc, ev, ex, qu, au)
uiContext d;
unsigned char *yomi;
int yomilen;
yomiContext yc;
int (*ev)(), (*ex)(), (*qu)(), (*au)();
{
  henkanContext hc;
  struct callback *pushCallback();
  extern KanjiModeRec tankouho_mode;

  hc = (henkanContext)malloc(sizeof(henkanContextRec));
  if (hc == 0) {
    henkanError();
    return 0;
  }
  if (pushCallback(d, d->modec, ev, ex, qu, au) == (struct callback *)0) {
    freeHenkanContext(hc);
    henkanError();
    return 0;
  }
  hc->id = HENKAN_CONTEXT;
  hc->majorMode = d->majorMode;
  hc->next = d->modec;
  d->modec = (mode_context)hc;
  hc->prevMode = d->current_mode;
  d->current_mode = &tankouho_mode;
  strncpy(hc->yomi_buffer, yomi, yomilen);
  hc->yomi_buffer[yomilen] = '\0';
  hc->yomilen = yomilen;
  hc->ycx = yc;
  hc->kanaKugiri = hc->romajiKugiri = hc->jishubun = 0;
  hc->context = -1; /* popHenkanModeなどのためとりあえずナシにしておく。 */
  if (d->contextCache >= 0) {
    hc->context = d->contextCache;
    d->contextCache = -1;
    debug_message("△キャッシュのを使う\n",0,0,0);
  }
  else {
    if (defaultContext == -1) { /* 接続が切れている場合 */
      debug_message("△接続に行く\n",0,0,0);
      if (KanjiInit() < 0) {
	popHenkanMode(d);
	popCallback(d);
	necKanjiError = "irohaserverにコネクトできません";
	return 0;
      }
    }
    debug_message("△デュプリケートする\n",0,0,0);
    hc->context = RkDuplicateContext(defaultContext);
    if (hc->context < 0) {
      popHenkanMode(d);
      popCallback(d);
      henkanError();
      return 0;
    }
  }
  return hc;
}

popHenkanMode(d)
uiContext d;
{
  henkanContext hc = (henkanContext)d->modec;

  d->modec = hc->next;
  d->current_mode = hc->prevMode;
  /* callback は callback 自身が pop する */
  if (d->contextCache >= 0) {
    RkCloseContext(hc->context);
  }
  else {
    d->contextCache = hc->context;
  }
  freeHenkanContext(hc);
}

freeHenkanContext(hc)
henkanContext hc;
{
  if (hc->kanaKugiri) {
    free(hc->kanaKugiri);
  }
  if (hc->romajiKugiri) {
    free(hc->romajiKugiri);
  }
  if (hc->jishubun) {
    free(hc->jishubun);
  }
  free(hc);
}

henkan(d, len)
uiContext d;
int len;
{
  /* よみを漢字に変換する */
  if(doYomiHenkan(d, len) == NG) {
    popHenkanMode(d);
    popCallback(d);
    strcpy(d->genbuf, necKanjiError);
    makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
    NothingForGLine(d);
    return 0;
  }

  /* kanji_status_returnを作る */
  makeKanjiStatusReturn(d, (henkanContext)d->modec);
  return 0;
}


/*
 * かな漢字変換を行う
 * ・d->yomi_bufferによみを取り出し、RkBgnBunを呼んでかな漢字変換を開始する
 * ・カレント文節を先頭文節にして、エコー文字列を作る
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
doYomiHenkan(d, len)
uiContext	d;
int len;
{
  unsigned int mode;
  henkanContext hc = (henkanContext)d->modec;

#ifdef DEBUG
  if (iroha_debug) {
    printf("yomi     => %s\n", hc->yomi_buffer);
    printf("yomi len => %d\n", hc->yomilen);
  }
#endif

  /* 連文節変換を開始する *//* 辞書にない カタカナ ひらがな を付加する */
  mode = 0;
  mode = (RK_XFER<<RK_XFERBITS) | RK_KFER;
  
  if(hc->context == -1) {
    int tmp;
    if(defaultContext == -1) {
      if(((tmp = KanjiInit()) != 0) || (defaultContext == -1)) {
	necKanjiError = "irohaserverにコネクトできません";
	return(NG);
      } else {
	d->contextCache = RkDuplicateContext(defaultContext);
      }
    }
  }

#ifdef MEASURE_TIME
  {
    struct tms timebuf;
    long RkTime, times();

    RkTime = times(&timebuf);
#endif /* MEASURE_TIME */

  if((hc->nbunsetsu =
      RkBgnBun(hc->context, hc->yomi_buffer, hc->yomilen, mode))
     == -1) {
    return kanakanError();
  }
  if (len > 0) {
    if ((hc->nbunsetsu = RkResize(hc->context, len)) == -1) {
      return kanakanError();
    }
  }

#ifdef MEASURE_TIME
    hc->rktime = times(&timebuf);
    hc->rktime -= RkTime;
  }
#endif /* MEASURE_TIME */

  /* カレント文節は先頭文節 */
  hc->curbun = 0;

  /* エコーストリングを作る */
  if(makeEchoStr(d) == NG)
    return(NG);

  return(0);
}

static
kanakanError()
{
  extern errno;

  if(errno == EPIPE)
    necKanjiPipeError();
  necKanjiError = "かな漢字変換に失敗しました";
  return(NG);
}

/*
 * 次文節に移動する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
TanForwardBunsetsu(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;

  hc->curbun += 1;
  if(hc->curbun >= hc->nbunsetsu) {
    if(kakuteiIfEndOfBunsetsu) {
      d->nbytes = TanKakutei(d);
      d->kanji_status_return->length = 0;
      return(d->nbytes);
    } else if(CursorWrap) {
      hc->curbun = 0;
    } else {
      hc->curbun -= 1;
      return NothingForGLine(d);
    }
  }

  /* カレント文節を１つ右に移す */
  /* カレント文節が最右だったら、
     最左をカレント文節にする   */
  if(RkRight(hc->context) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "文節の移動に失敗しました";
    return(NG);
  }

  /* kanji_status_returnを作る */
  makeKanjiStatusReturn(d, hc);
  return(0);
}

/*
 * 前文節に移動する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
TanBackwardBunsetsu(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;

  if(hc->curbun)
    hc->curbun -= 1;
  else {
    if(CursorWrap)
      hc->curbun = hc->nbunsetsu - 1;
    else 
      return NothingForGLine(d);
  }

  /* カレント文節を１つ左に移す */
  /* カレント文節が最左だったら、
     最右をカレント文節にする   */
  if(RkLeft(hc->context) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "文節の移動に失敗しました";
    return(NG);
  }

  /* kanji_status_returnを作る */
  makeKanjiStatusReturn(d, hc);

  return(0);
}

/*
 * 次候補をカレント候補にする
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */

static
tanNextKouho(d, hc)
uiContext	d;
henkanContext   hc;
{
#ifdef MEASURE_TIME
  struct tms timebuf;
  long proctime, times();

  proctime = times(&timebuf);
#endif /* MEASURE_TIME */

  /* 次の候補をカレント候補とする */
  if(RkNext(hc->context) == -1) {
    necKanjiError = "カレント候補を取り出せませんでした";
    if(errno == EPIPE)
      necKanjiPipeError();
    return(NG);
  }

#ifdef MEASURE_TIME
  hc->rktime = times(&timebuf);
  hc->rktime -= proctime;
#endif /* MEASURE_TIME */

  /* エコーストリングを作る */
  if(makeEchoStrCurChange(d) == NG)
    return(NG);

  /* kanji_status_returnを作る */
  makeKanjiStatusReturn(d, hc);

#ifdef MEASURE_TIME
  hc->proctime = times(&timebuf);
  hc->proctime -= proctime;
#endif /* MEASURE_TIME */

  return(0);
}

TanNextKouho(d)
uiContext d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;
  return tanNextKouho(d, hc);
}

/*

  TanHenkan -- 回数をチェックする意外は TanNextKouho とほぼ同じ

 */

static
TanHenkan(d)
uiContext d;
{
  henkanContext hc = (henkanContext)d->modec;

  if (kouho_threshold && ++hc->kouhoCount >= kouho_threshold) {
    return TanKouhoIchiran(d);
  }
  else {
    return tanNextKouho(d, hc);
  }
}

/*
 * 前候補をカレント候補にする
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
TanPreviousKouho(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;
  /* 前の候補をカレント候補とする */
  if(RkPrev(hc->context) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "カレント候補を取り出せませんでした";
    return(NG);
  }

  /* エコーストリングを作る */
  if(makeEchoStrCurChange(d) == NG)
    return(NG);

  /* kanji_status_returnを作る */
  makeKanjiStatusReturn(d, hc);

  return(0);
}

/*
 * ひらがなをカレント候補にする
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
TanHiragana(d)
uiContext	d;
{
  /* エコーストリングを作る */
  ((henkanContext)d->modec)->kouhoCount = 0;
  if(makeEchoStrCurKana(d, JISHU_HIRA) == NG)
    return(NG);

  /* kanji_status_returnを作る */
  makeKanjiStatusReturn(d, (henkanContext)d->modec);
  return(0);
}

/*
 * カタカナをカレント候補にする
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
TanKatakana(d)
uiContext	d;
{
  ((henkanContext)d->modec)->kouhoCount = 0;
  /* エコーストリングを作る */
  if(makeEchoStrCurKana(d, JISHU_ZEN_KATA) == NG)
    return(NG);

  /* kanji_status_returnを作る */
  makeKanjiStatusReturn(d, (henkanContext)d->modec);
  return(0);
}

static
TanHankakuKatakana(d)
uiContext	d;
{
  extern InhibitHankakuKana;

  if (InhibitHankakuKana)
    return NothingChangedWithBeep(d);

  /* エコーストリングを作る */
  if(makeEchoStrCurKana(d, JISHU_HAN_KATA) == NG)
    return(NG);

  /* kanji_status_returnを作る */
  makeKanjiStatusReturn(d, (henkanContext)d->modec);
  return(0);
}

/*
 * ローマ字をカレント候補にする
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
TanRomaji(d)
uiContext	d;
{
  ((henkanContext)d->modec)->kouhoCount = 0;
  /* エコーストリングを作る */
  if(makeEchoStrCurRomaji(d, 1/*全角*/) == NG)
    return(NG);

  /* kanji_status_returnを作る */
  makeKanjiStatusReturn(d, (henkanContext)d->modec);
  return(0);
}

static
TanHankakuRomaji(d)
uiContext	d;
{
  /* エコーストリングを作る */
  if(makeEchoStrCurRomaji(d, 0/*半角*/) == NG)
    return(NG);

  /* kanji_status_returnを作る */
  makeKanjiStatusReturn(d, (henkanContext)d->modec);
  return(0);
}

static
TanZenkaku(d)
uiContext d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;
  if (hc->jishubun == 0) { /* 字種変換されたことがない */
    return TanHiragana(d);
  }
  switch (hc->jishubun[hc->curbun])
    {
    case JISHU_HIRA:
      return TanHiragana(d);
    case JISHU_ZEN_KATA:
    case JISHU_HAN_KATA:
      return TanKatakana(d);
    case JISHU_ZEN_ALPHA:
    case JISHU_HAN_ALPHA:
      return TanRomaji(d);
    default:
      return NothingChangedWithBeep(d); /* ありえないのでは？ */
    }
}

static
TanHankaku(d)
uiContext d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;
  if (hc->jishubun == 0) { /* 字種変換されたことがない */
    return TanHankakuKatakana(d);
  }
  switch (hc->jishubun[hc->curbun])
    {
    case JISHU_HIRA:
    case JISHU_ZEN_KATA:
    case JISHU_HAN_KATA:
      return TanHankakuKatakana(d);
    case JISHU_ZEN_ALPHA:
    case JISHU_HAN_ALPHA:
      return TanHankakuRomaji(d);
    default:
      return NothingChangedWithBeep(d); /* ありえないのでは？ */
    }
}

/*
 * 最左文節に移動する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
TanBeginningOfBunsetsu(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;
  /* カレント文節を左端にする */
  if(RkGoTo(hc->context, 0) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "文節の移動に失敗しました";
    return(NG);
  }
  hc->curbun = 0;

  /* kanji_status_returnを作る */
  makeKanjiStatusReturn(d, hc);

  return(0);
}

/*
 * 最右文節に移動する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
TanEndOfBunsetsu(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;
  /* カレント文節を右端にする */
  if(RkGoTo(hc->context, (hc->nbunsetsu - 1)) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "文節の移動に失敗しました";
    return(NG);
  }
  hc->curbun = hc->nbunsetsu - 1;

  /* kanji_status_returnを作る */
  makeKanjiStatusReturn(d, hc);

  return(0);
}

#ifdef SATOKO
TanMuhenkan(d)	/* 部分的に読みに戻す */
uiContext	d;
{
  int  i, index;
  unsigned char *ptr;


  /* 読みをカレント候補とする(無変換) */
  if(RkNfer(hc->context) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "読みを取り出せませんでした";
    return(NG);
  }
  /* エコーストリングを作る */
  ptr = d->echo_buffer;
  /* 先頭の文節から順に無変換にして、エコー用のバッファに入れる */
  /* 各文節の区ぎりをとっておく(先頭からのバイト数) */
  for(i=0, index=0; i < d->nbunsetsu; i++) {
    RkGoTo(hc->context, i);
    if(RkNfer(hc->context) == -1) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "読みを取り出せませんでした";
      return(NG);
    }
    d->kugiri[i] = index;
    if(RkGetKanji(hc->context, (ptr + index), (ROMEBUFSIZE - index)) < 0) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "カレント候補を取り出せませんでした";
      return(NG);
    }
    index += strlen(ptr + index);
  }  
  d->kugiri[i] = index; /* 終わりの位置 */
  d->kugiri[i+1] = (int)NULL;

  /* カレント文節を先頭にする */
  RkGoTo(hc->context, 0);

  /* kanji_status_returnを作る */
  makeKanjiStatusReturn(d, (henkanContext)d->modec);

  return(0);
}
#endif

/*
 * カレント文節の前まで確定し、カレント以降の文節を読みに戻す
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
TanBubunKakutei(d)
uiContext	d;
{
  int i, j, n, len = 0;
  RkStat st;
  unsigned char tmpbuf[ROMEBUFSIZE];
  unsigned char *ptr;
  henkanContext hc = (henkanContext)d->modec;
  yomiContext yc = hc->ycx;

  /* 確定文字列 を作る */
  if(BunsetsuKugiri) {
    ptr = tmpbuf;
    for(i=0; i<hc->curbun; i++) {
	n = hc->kugiri[i+1] - hc->kugiri[i];
      if(i != (hc->nbunsetsu - 1))
	n--;
      strncpy(ptr, &hc->echo_buffer[hc->kugiri[i]], n);
      ptr += n;
    }
    d->nbytes = ((ptr - tmpbuf) > d->bytes_buffer) ? 
      d->bytes_buffer : (ptr - tmpbuf);
    d->nbytes = ujisncpy(d->buffer_return, tmpbuf, d->nbytes);
  } else {
    d->nbytes = (hc->kugiri[hc->curbun] > d->bytes_buffer) ? 
      d->bytes_buffer : hc->kugiri[hc->curbun];
    d->nbytes = ujisncpy(d->buffer_return, hc->echo_buffer, d->nbytes);
  }

  /* 未確定文節を読みに戻す */
  for(i=0; i<hc->curbun; i++) { /* 確定された文字列の読みの長さを得る */ 
    if(RkGoTo(hc->context, i) == -1) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "文節の移動に失敗しました";
      return(NG);
    }
    if(RkGetStat(hc->context, &st) == -1) {
      necKanjiError = "ステイタスを取り出せませんでした";
      if(errno == EPIPE) {
	necKanjiPipeError();
	d->nbytes = TanKakutei(d);
	strcpy(d->genbuf, necKanjiError);
	makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
	return d->nbytes;
      }
      return(NG);
    }
    len += st.ylen;
  }

#define  kanaReplace(where, insert, insertlen, mask) \
generalReplace(yc->kana_buffer, yc->kAttr, &yc->kRStartp,\
&yc->kCurs, &yc->kEndp,\
where, insert, insertlen, mask)

#define  romajiReplace(where, insert, insertlen, mask) \
generalReplace(yc->romaji_buffer, yc->rAttr, &yc->rStartp,\
&yc->rCurs, &yc->rEndp,\
where, insert, insertlen, mask)

  j = 0;
  for (i = 0 ; i < len ; i++) {
    if (yc->kAttr[i] & SENTOU) {
      do {
	j++;
      } while (!(yc->rAttr[j] & SENTOU));
    }
  }
  yc->rStartp = yc->rCurs = j;
  romajiReplace(-j, (unsigned char *)NULL, 0, 0);
  yc->kRStartp = yc->kCurs = len;
  kanaReplace(-len, (unsigned char *)NULL, 0, 0);

  /* 変換を取り止める */
  /* 学習のカスタマイズは？ */
  if(Gakushu) { /* ON */
    if(RkEndBun(hc->context, 1) == -1) {	/* 1:学習する */
      necKanjiError = "かな漢字変換の終了に失敗しました";
      if(errno == EPIPE)
	necKanjiPipeError();
    }
  } else { /* OFF */
    if(RkEndBun(hc->context, 0) == -1) {	/* 0:学習しない */
      necKanjiError = "かな漢字変換の終了に失敗しました";
      if(errno == EPIPE)
	necKanjiPipeError();
    }
  }

  d->status = QUIT_CALLBACK;
  if (hc->next == (mode_context)yc) {
    popHenkanMode(d);
  }
  else { /* すぐ下が読みコンテクスト出ないときは QUIT_CALLBACK がうまく
	    働かない。すぐ下が読み出ないときは字種だと思われるのでポップ
	    してしまう */
    popHenkanMode(d);
    popCallback(d);
    popJishuMode(d);
  }
    
  return(d->nbytes);
}

/*
 * 全ての文節を読みに戻し、YomiInputMode に戻る
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
TanMuhenkan(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;

  /* 変換を取り止める */
  if(RkEndBun(hc->context, 0) == -1) {	/* 0:学習しない */
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "かな漢字変換の終了に失敗しました";
  }

  d->status = QUIT_CALLBACK;
  popHenkanMode(d);
  return(0);
}

/*
 * 全ての文節を確定する
 *
 * ・d->buffer_return に d->echo_buffer をコピーし、確定文字列とする
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
TanKakutei(d)
uiContext	d;
{
  int i, n;
  unsigned char *ptr;
  unsigned char tmpbuf[ROMEBUFSIZE];
  char word[1024], *w; /* 自動学習 */
  henkanContext hc = (henkanContext)d->modec;
  yomiContext yc = hc->ycx;
#ifdef KAKUTEI_MEASURE_TIME
  struct tms timebuf;
  long proctime, rktime, times();

  proctime = times(&timebuf);
#endif /* KAKUTEI_MEASURE_TIME */


  /* エコーストリングを確定文字列とする */
  if(BunsetsuKugiri) {
    ptr = tmpbuf;
    for(i=0; i<hc->nbunsetsu; i++) {
      n = hc->kugiri[i+1] - hc->kugiri[i];
      if(i != (hc->nbunsetsu - 1))
	n--;
      strncpy(ptr, &hc->echo_buffer[hc->kugiri[i]], n);
      ptr += n;
    }
    d->nbytes = ((ptr - tmpbuf) > d->bytes_buffer) ? 
      d->bytes_buffer : (ptr - tmpbuf);
    d->nbytes = ujisncpy(d->buffer_return, tmpbuf, d->nbytes);
  } else {
    d->nbytes = (hc->kugiri[hc->nbunsetsu] > d->bytes_buffer) ? 
      d->bytes_buffer : hc->kugiri[hc->nbunsetsu];
    d->nbytes = ujisncpy(d->buffer_return, hc->echo_buffer, d->nbytes);
  }

#define RENGOBUFSIZE 256

  if (RengoGakushu && hc->nbunsetsu > 1) { /* 連語学習をしようかなぁ */
    int    i;
    RkLex  lex[2][RENGOBUFSIZE];
    char   yomi[2][RENGOBUFSIZE];
    char   kanji[2][RENGOBUFSIZE];
    int    nword[2];

    w = word;
    *w = '\0';

    RkGoTo(hc->context, 0);
    nword[0] = RkGetLex(hc->context, lex[0], RENGOBUFSIZE);
    yomi[0][0] = '\0'; /* yomi[current][0]の真理値 ≡ RkGetYomiしたか */

    for (i = 1 ; i < hc->nbunsetsu ; i++) {
      int current, previous, mighter;

      current = i % 2;
      previous = 1 - current;

      nword[current] = 0;
      if ( !nword[previous] ) {
	nword[previous] = RkGetLex(hc->context, lex[previous], RENGOBUFSIZE);
      }
      RkRight(hc->context);

      if (nword[previous] == 1) {
	nword[current] = RkGetLex(hc->context, lex[current], RENGOBUFSIZE);
	yomi[current][0] = '\0';
	if (((lex[previous][0].ylen <= 6 && lex[previous][0].klen == 2) ||
	     (lex[current][0].ylen <= 6 && lex[current][0].klen == 2)) &&
	    (lex[current][0].rownum < R_K5 ||
	     R_NZX < lex[current][0].rownum)) {
	  if ( !yomi[previous][0] ) {
	    RkLeft(hc->context);
	    RkGetYomi(hc->context, yomi[previous], RENGOBUFSIZE);
	    RkGetKanji(hc->context, kanji[previous], RENGOBUFSIZE);
	    RkRight(hc->context);
	  }
	  RkGetYomi(hc->context, yomi[current], RENGOBUFSIZE);
	  RkGetKanji(hc->context, kanji[current], RENGOBUFSIZE);

	  strncpy(yomi[previous] + lex[previous][0].ylen,
		  yomi[current], lex[current][0].ylen);
	  yomi[previous][lex[previous][0].ylen + lex[current][0].ylen] = '\0';

	  strncpy(kanji[previous] + lex[previous][0].klen,
		  kanji[current], lex[current][0].klen);
	  kanji[previous][lex[previous][0].klen + lex[current][0].klen] = '\0';

#ifdef NAGASADEBUNPOUWOKIMEYOU
	  if (lex[previous][0].klen >= lex[current][0].klen) {
	    /* 前の漢字の長さ       >=    後ろの漢字の長さ */
	    mighter = previous;
	  }
	  else {
	    mighter = current;
	  }
#else
	  mighter = current;
#endif
	  sprintf(w, "%s #%d#%d %s",
		  yomi[previous],
		  lex[mighter][0].rownum, lex[mighter][0].colnum,
		  kanji[previous]);
	  w += strlen(w) + 1; *w = '\0';
	}
      }
    }
  }

#ifdef KAKUTEI_MEASURE_TIME
  rktime = times(&timebuf);
#endif /* KAKUTEI_MEASURE_TIME */

  /* 変換を終了する */
  /* 学習のカスタマイズは？ */
  if(RkEndBun(hc->context, Gakushu ? 1 : 0) == -1) { /* 1:学習する/0:しない */
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "かな漢字変換の終了に失敗しました";
  }

#ifdef KAKUTEI_MEASURE_TIME
  d->rktime = times(&timebuf);
  d->rktime -= rktime;
#endif /* KAKUTEI_MEASURE_TIME */

  if (RengoGakushu && hc->nbunsetsu > 1) { /* 連語学習をしようかなぁ */
    for (w = word ; *w ; w += strlen(w) + 1) {
      RkDefineDic(hc->context, RengoGakushu, w);
    }
  }

  /* 読みをクリアする */
  { /* その前に、前の読みを一緒に返す準備をする */
    extern yomiInfoLevel;
    int n;

    n = d->nbytes;
    if (yomiInfoLevel > 0) {
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

#ifdef KAKUTEI_MEASURE_TIME
  d->proctime = times(&timebuf);
  d->proctime -= proctime;

  TanPrintTime(d);
#endif /* KAKUTEI_MEASURE_TIME */

  d->status = EXIT_CALLBACK;
  popHenkanMode(d);
  return(d->nbytes);
}


/*
 * 漢字候補を確定させ、ローマ字をインサートする
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
TanKakuteiYomiInsert(d)
uiContext d;
{
  d->nbytes = TanKakutei(d);
  d->more.todo = 1;
  d->more.ch = d->ch;
  d->more.fnum = 0;    /* 上の ch で示される処理をせよ */
  return(d->nbytes);
}


/* cfuncdef

  pos で指定された文節およびそれより後の文節の字種変換情報を
  クリアする。
*/

static
clearBunsetsuInfo(hc, pos)
henkanContext hc;
int pos;
{
  int i;
  if (hc->kanaKugiri) {
    for (i = pos ; i < MAXNBUNSETSU ; i++) {
      hc->kanaKugiri[i] = -1;
    }
    hc->kanaKugiri[0] = 0;
  }
  if (hc->romajiKugiri) {
    for (i = pos ; i < MAXNBUNSETSU ; i++) {
      hc->romajiKugiri[i] = -1;
    }
    hc->romajiKugiri[0] = 0;
  }
  if (hc->jishubun) {
    for (i = pos ; i < MAXNBUNSETSU ; i++) {
      hc->jishubun[i] = 0;
    }
  }
}

/*
 * 文節を伸ばす
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
TanExtendBunsetsu(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;
  /* 文節を伸ばす */
  if((hc->nbunsetsu = RkEnlarge(hc->context)) == -1) {
    necKanjiError = "文節の拡大に失敗しました";
    if(errno == EPIPE) {
      necKanjiPipeError();
      strcpy(d->genbuf, necKanjiError);
      d->nbytes = TanKakutei(d);
      makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
      return d->nbytes;
    }
    return(NG);
  }

  clearBunsetsuInfo(hc, hc->curbun);

  /* エコーストリングを作る */
  if(makeEchoStrCurUnderChange(d) == NG)
    return(NG);

  /* kanji_status_returnを作る */
  makeKanjiStatusReturn(d, hc);

  return(0);
}

/*
 * 文節を縮める
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
TanShrinkBunsetsu(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;
  /* 文節を縮める */
  if((hc->nbunsetsu = RkShorten(hc->context)) == -1) {
    necKanjiError = "文節の縮小に失敗しました";
    if(errno == EPIPE) {
      necKanjiPipeError();
      d->nbytes = TanKakutei(d);
      strcpy(d->genbuf, necKanjiError);
      makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
      return d->nbytes;
    }
    return(NG);
  }

  clearBunsetsuInfo(hc, hc->curbun);

  /* エコーストリングを作る */
  if(makeEchoStrCurUnderChange(d) == NG)
    return(NG);

  /* kanji_status_returnを作る */
  makeKanjiStatusReturn(d, hc);

  return(0);
}

/*
 * 部分無変換する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
TanBubunMuhenkan(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;
  yomiContext yc = hc->ycx, newyc, GetKanjiString();
  int len, pos, current;
  int bubunEveryTimeCatch(), exitBubun(), quitBubun();
  extern KanjiModeRec yomi_mode;

  hc->kouhoCount = 0;
  current = hc->curbun;
  if ((len = countYomi(d, &pos)) == 0) {
    /* なんらかの事情で読みの長さを確認できなかったなら終わり */
    if(errno == EPIPE) {
      necKanjiPipeError();
      d->nbytes = TanKakutei(d);
      strcpy(d->genbuf, "かな漢字変換サーバとの接続が切れました");
      makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
      return d->nbytes;
    }
    return NothingChangedWithBeep(d);
  }
  newyc = GetKanjiString(d, (unsigned char *)NULL, 0,
			 IROHA_NOTHING_RESTRICTED,
			 !IROHA_YOMI_CHGMODE_INHIBITTED,
			 IROHA_YOMI_END_IF_KAKUTEI,
			 IROHA_YOMI_INHIBIT_NONE,
			 JISHU_HIRA,
			 0, /* 通常の変換をする */
			 bubunEveryTimeCatch, exitBubun, quitBubun);
  if (newyc == (yomiContext)0) {
    return NoMoreMemory();
  }
  newyc->minorMode = IROHA_MODE_BubunMuhenkanMode;
  /* 読みバッファにあらかじめ読みを入れておく。 */
  copyYomiBuffers(newyc, yc,
		  hc->kanaKugiri[current], hc->kanaKugiri[current + 1],
		  hc->romajiKugiri[current], hc->romajiKugiri[current + 1]);
  d->current_mode = &yomi_mode;
  d->kanji_status_return->echoStr = newyc->kana_buffer;
  d->kanji_status_return->length = newyc->kEndp;
  d->kanji_status_return->revPos = 0;
  d->kanji_status_return->revLen = newyc->kana_buffer[0] & 0x80 ? 2 : 1;
  if(RkNfer(hc->context) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    popYomiMode(d);
    popCallback(d);
    necKanjiError = "変換に失敗しました";
    return(NG);
  }
  bubunEveryTimeCatch(d, 0, hc);
  d->minorMode = IROHA_MODE_HenkanMode;
  /* モードが以下の currentModeInfo で確実に出力されるようにする */
  currentModeInfo(d);
  d->status = AUX_CALLBACK; /* prevent EverytimeCatch */
  return 0;
}

static
bubunEveryTimeCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  unsigned char xxxx[ROMEBUFSIZE];
  int len, revPos, revLen;
  coreContext cc = (coreContext)d->modec;

  if ((len = d->kanji_status_return->length) < 0) {
    return retval;
  }
  strncpy(xxxx, d->kanji_status_return->echoStr, len);
  xxxx[len] = '\0';
  revPos = changeCurBunsetsuString(env, xxxx, len);
  revPos += d->kanji_status_return->revPos;
  revLen = d->kanji_status_return->revLen;
  makeKanjiStatusReturn(d, (henkanContext)env);
  if (revLen) {
    d->kanji_status_return->revPos = revPos;
    d->kanji_status_return->revLen = revLen;
  }
  /* モードを強制的に変える */
  d->kanji_status_return->info &= ~KanjiModeInfo;
  cc->minorMode = IROHA_MODE_BubunMuhenkanMode;
  d->minorMode = IROHA_MODE_BubunMuhenkanMode;
  return retval;
}

static
quitBubun(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  henkanContext hc = (henkanContext)d->modec;
  unsigned char xxxx[ROMEBUFSIZE];
  int len;

  popCallback(d);
  ((henkanContext)env)->kouhoCount = 0;
  len = RkGetYomi(hc->context, xxxx, ROMEBUFSIZE);
  if (len < 0) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "変換に失敗しました";
    return(NG);
  }
  changeCurBunsetsuString(env, xxxx, len);
  makeKanjiStatusReturn(d, (henkanContext)env);
  currentModeInfo(d);
  return 0;
}

static
exitBubun(d, retval, hc)
uiContext d;
int retval;
henkanContext hc;
{
  yomiContext yc = hc->ycx;
  RkStat st;
  int i, yplen = 0, yclen = 0, rplen = 0, rclen = 0, startpos;
  popCallback(d);

  /* 変わった部分の読みを変更する */
  if ((rclen = countYomi(d, &startpos)) == 0) {
  }
  yc->rStartp = yc->rCurs = startpos;
  yc->kRStartp = yc->kCurs = hc->kanaKugiri[hc->curbun];
  generalReplace(yc->romaji_buffer, yc->rAttr, &yc->rStartp,
		 &yc->rCurs, &yc->rEndp,
		 hc->romajiKugiri[hc->curbun + 1] -
		 hc->romajiKugiri[hc->curbun],
		 d->buffer_return, retval, 0);
  generalReplace(yc->kana_buffer, yc->kAttr, &yc->kRStartp,
		 &yc->kCurs, &yc->kEndp,
		 hc->kanaKugiri[hc->curbun + 1] - hc->kanaKugiri[hc->curbun],
		 d->buffer_return, retval, HENKANSUMI);
  changeCurYomiLen(d, retval, retval);
  
  strcpy(hc->yomi_buffer, yc->kana_buffer);

  /* カレント候補の表示を変える */
  hc->kouhoCount = 0;
  changeCurBunsetsuString(hc, d->buffer_return, retval);
  makeKanjiStatusReturn(d, hc);
  currentModeInfo(d);
  return 0;
}

#ifdef BUNPOU_DISPLAY
/*
 * 文法情報をプリントする
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
TanPrintBunpou(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;
  static unsigned char buf[256], tmpbuf[64];
  static unsigned char mesg[512];
  RkLex	lex[32];
  int i, n;
  unsigned char *p, *q;
#define LOCAL_RK /* ローカルの RK ライブラリを文法表示に使う */
#ifdef LOCAL_RK
#define GRAMMER_DIC	"/usr/lib/iroha/dic/hyoujun.gram"

  struct RkKxGram	*gram = (struct RkKxGram *)-1;

  if (gram == (struct RkKxGram *)-1) {
    gram = (struct RkKxGram *)0;

    gram = RkOpenGram(GRAMMER_DIC, 0);
  }
#endif /* LOCAL_RK */

  hc->kouhoCount = 0;

#ifdef DO_GETYOMI
  if (RkGetYomi(hc->context, buf, 256) == -1) {
    if (errno == EPIPE) {
      necKanjiPipeError();
    }
    fprintf(stderr, "カレント候補の読みを取り出せませんでした。\n");
  }
  fprintf(stderr, "%s\n", buf);
#endif /* DO_GETYOMI */

  if(RkGetKanji(hc->context, buf, 256) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "カレント候補を取り出せませんでした";
    return(NG);
  }
  if((n = RkGetLex(hc->context, lex, 32)) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "形態素情報を取り出せませんでした";
    return(NG);
  }

  mesg[0] = '\0';
  p = buf;
  q = mesg;
  for (i = 0 ; i < n ; i++) {
#ifdef LOCAL_RK
    if (gram) {
      strncpy(q, p, lex[i].klen);
      p += lex[i].klen;
      RkUparseGramNum(gram, lex[i].rownum, lex[i].colnum, q + lex[i].klen);
      q[strlen(q) + 1] = '\0';
      q[strlen(q)] = ' ';
    }
    else {
#endif /* LOCAL_RK */
      strncpy(tmpbuf, p, lex[i].klen);
      p += lex[i].klen;
      tmpbuf[lex[i].klen] = 0;
      sprintf(q, "%s#%d#%d ", tmpbuf, lex[i].rownum, lex[i].colnum);
#ifdef LOCAL_RK
    }
#endif /* LOCAL_RK */
    q += strlen(q);
  }
  if (q != mesg) {
    q--;
  } else {
    strcpy(mesg, "形態素が取れません。");
    q = mesg + strlen(mesg);
  }
  d->kanji_status_return->info |= KanjiGLineInfo;
  d->kanji_status_return->gline.line = mesg;
  d->kanji_status_return->gline.length = q - mesg;
  d->kanji_status_return->gline.revPos = 0;
  d->kanji_status_return->gline.revLen = 0;
  d->kanji_status_return->length = -1;
  d->flags |= PLEASE_CLEAR_GLINE;
  return(0);
}
#endif /* BUNPOU_DISPLAY */

#ifdef MEASURE_TIME
static
TanPrintTime(d)
uiContext	d;
{
  static char buf[256];
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;
  sprintf(buf, "変換時間 %d [ms]、うち UI 部は %d [ms]",
	  (hc->proctime) * 50 / 3,
	  (hc->proctime - hc->rktime) * 50 / 3);
  d->kanji_status_return->info |= KanjiGLineInfo;
  d->kanji_status_return->gline.line = (unsigned char *)buf;
  d->kanji_status_return->gline.length = strlen(buf);
  d->kanji_status_return->gline.revPos = 0;
  d->kanji_status_return->gline.revLen = 0;
  d->kanji_status_return->length = -1;
  d->flags |= PLEASE_CLEAR_GLINE;
  return(0);
}
#endif /* MEASURE_TIME */

necKanjiPipeError()
{
  debug_message("まっさらにしちゃいます\n",0,0,0);

  defaultContext = -1;
  defaultBushuContext = -1;

  makeAllContextToBeClosed(0);

  RkFinalize();
}

/* cfuncdef

  getKanjiPart -- カレント文節の直前までの、
                  漢字表記上の長さと読みの長さを得る。
                  ついでにカレント文節の読みの長さも返す。
 */

static int
getKanjiPart(d, hc, klp, ylp, curylp)
uiContext d;
henkanContext hc;
int *klp, *ylp, *curylp;
{
  int i, j, kl = 0, yl = 0, curyl = 0, nlex;
  unsigned char xxxx[256];
  RkLex	lex[32];

  for (i = 0 ; i < hc->curbun ; i++) {
    if (RkGoTo(hc->context, i) == -1) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "文節の移動に失敗しました";
      return(NG);
    }
    if ((nlex = RkGetLex(hc->context, lex, 32)) == -1) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "形態素情報を取り出せませんでした";
      return(NG);
    }
    for (j = 0 ; j < nlex ; j++) {
      kl += lex[j].klen;
      yl += lex[j].ylen;
    }
    if (BunsetsuKugiri) kl++;
  }
  if (RkGoTo(hc->context, hc->curbun) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "文節の移動に失敗しました";
    return(NG);
  }
  if ((nlex = RkGetLex(hc->context, lex, 32)) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "形態素情報を取り出せませんでした";
    return(NG);
  }
  for (j = 0 ; j < nlex ; j++) {
    curyl += lex[j].ylen;
  }
  if (klp) *klp = kl;
  if (ylp) *ylp = yl;
  if (curylp) *curylp = curyl;
  return 0;
}

static
_tanAdjustExit(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  henkanContext hc;

  popCallback(d);
  d->nbytes = retval;

  if (retval > 0) { /* 確定を意味する */
    TanMuhenkan(d); /* 単候補モードから読みモードに戻る */
    /* さらに読みモードを抜けるように設定 */
    d->more.todo = 1;
    d->more.ch = d->ch;
    d->more.fnum = IROHA_FN_Quit;
  }
  else {
    hc = (henkanContext)d->modec;
    clearBunsetsuInfo(hc, hc->curbun);

    /* エコーストリングを作る */
    if(makeEchoStrCurUnderChange(d) == NG)
      return NG;

    /* kanji_status_returnを作る */
    makeKanjiStatusReturn(d, hc);
    currentModeInfo(d);
  }

  return retval;
}

static
_tanAdjustQuit(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  int curpos = retval, i, j;
  yomiContext yc;

  popCallback(d);

  yc = (yomiContext)((henkanContext)d->modec)->ycx;

  retval = TanBubunKakutei(d);

  j = 0;
  for (i = 0 ; i < curpos ; i++) {
    if (yc->kAttr[i] & SENTOU) {
      do {
	j++;
      } while (!(yc->rAttr[j] & SENTOU));
    }
  }
  yc->rCurs = j;
  yc->kCurs = curpos;

  return retval;
}

/* cfuncdef

  TanBunsetsuMode -- 単候補モードから文節伸ばし縮めモードへ移行する

 */

static
TanBunsetsuMode(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;
  yomiContext yc = (yomiContext)hc->ycx;
  adjustContext ac, pushAdjustMode();
  int  kanlen, yomlen, curyomlen;

  if (getKanjiPart(d, hc, &kanlen, &yomlen, &curyomlen) < 0) {
    return -1;
  }
  /* AdjustBunsetsuMode(文節伸ばし縮めモード)に移行する */
  ac = pushAdjustMode(d, hc->echo_buffer, kanlen,
		      hc->yomi_buffer + yomlen,
		      hc->yomilen - yomlen,
		      curyomlen, hc, yc,
		      0, _tanAdjustExit, _tanAdjustQuit, 0);
  if (ac == 0) {
    return -1;
  }
  ac->minorMode = IROHA_MODE_AdjustBunsetsuMode;

  currentModeInfo(d);
  return 0;
}

#include	"tanmap.c"
