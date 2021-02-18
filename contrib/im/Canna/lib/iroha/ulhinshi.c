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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/ulhinshi.c,v 3.3 91/08/28 22:01:21 kon Exp $";

#include <errno.h>
#include "iroha.h"

int gramaticalQuestion = 1;

extern HexkeySelect;

extern int uiUtilIchiranTooSmall();
extern int uuTTangoQuitCatch();

static int tourokuYes(), tourokuNo(), makeDoushi(), uuTDicExitCatch();
static int uuTDicQuitCatch(), tangoTouroku();

static
unsigned char *gyouA[] = { (unsigned char *)"か", (unsigned char *)"が",
			   (unsigned char *)"さ", (unsigned char *)"た",
			   (unsigned char *)"な", (unsigned char *)"ば",
			   (unsigned char *)"ま", (unsigned char *)"ら",
			   (unsigned char *)"わ",};

static
unsigned char *gyouI[] = { (unsigned char *)"き", (unsigned char *)"ぎ",
			   (unsigned char *)"し", (unsigned char *)"ち",
			   (unsigned char *)"に", (unsigned char *)"び",
			   (unsigned char *)"み", (unsigned char *)"り",
			   (unsigned char *)"い",};

static
unsigned char *gyouU[] = { (unsigned char *)"く", (unsigned char *)"ぐ",
			   (unsigned char *)"す", (unsigned char *)"つ",
			   (unsigned char *)"ぬ", (unsigned char *)"ぶ",
			   (unsigned char *)"む", (unsigned char *)"る",
			   (unsigned char *)"う",};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 単語登録の品詞選択 〜Yes/No 共通 Quit〜                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuTHinshiYNQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  
  return(dicTourokuHinshi(d));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 単語登録の品詞選択 〜Yes/No 第２段階 共通コールバック〜                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuTHinshi2YesCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;
  int uuTDicExitCatch(), uuTDicQuitCatch();

  popCallback(d); /* yesNo をポップ */

  tourokuYes(d);   /* 品詞が決まれば tc->hcode にセットする */

  tc = (tourokuContext)d->modec;

  if(tc->qbuf[0]) {
    /* もう質問はないよね */
  } else if(tc->hcode[0]) {
    /* 品詞が決まったので、登録するユーザ辞書の指定を行う */
    return(dicTourokuDictionary(d, uuTDicExitCatch, uuTDicQuitCatch));
  }

  return(retval);
}

static
uuTHinshi2NoCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;
  int uuTDicExitCatch(), uuTDicQuitCatch();

  popCallback(d); /* yesNo をポップ */

  tourokuNo(d);   /* 品詞が決まれば tc->hcode にセットする */

  tc = (tourokuContext)d->modec;

  if(tc->qbuf[0]) {
    /* もう質問はないよね */
  } else if(tc->hcode[0]) {
    /* 品詞が決まったので、登録するユーザ辞書の指定を行う */
    return(dicTourokuDictionary(d, uuTDicExitCatch, uuTDicQuitCatch));
  }

  return(retval);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 単語登録の品詞選択 〜Yes/No 第１段階 コールバック〜                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuTHinshi1YesCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;
  coreContext ync;
  int uuTDicExitCatch(), uuTDicQuitCatch();
  
  popCallback(d); /* yesNo をポップ */

  tourokuYes(d);   /* 品詞が決まれば tc->hcode にセットする */

  tc = (tourokuContext)d->modec;

  if(tc->qbuf[0]) {
    /* 質問する */
    makeGLineMessage(d, tc->qbuf, strlen(tc->qbuf));
    if((retval = getYesNoContext(d,
		 0, uuTHinshi2YesCatch,
		 uuTHinshiYNQuitCatch, uuTHinshi2NoCatch)) == NG) {
      return(GLineNGReturnTK(d));
    }
    ync = (coreContext)d->modec;
    ync->majorMode = IROHA_MODE_ExtendMode;
    ync->minorMode = IROHA_MODE_TourokuHinshiMode;
  } else if(tc->hcode[0]) {
    /* 品詞が決まったので、登録するユーザ辞書の指定を行う */
    return(dicTourokuDictionary(d, uuTDicExitCatch, uuTDicQuitCatch));
  }

  return(retval);
}

static
uuTHinshi1NoCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;
  coreContext ync;
  int uuTDicExitCatch(), uuTDicQuitCatch();

  popCallback(d); /* yesNo をポップ */

  tourokuNo(d);   /* 品詞が決まれば tc->hcode にセットする */

  tc = (tourokuContext)d->modec;

  if(tc->qbuf[0]) {
    /* 質問する */
    makeGLineMessage(d, tc->qbuf, strlen(tc->qbuf));
    if((retval = getYesNoContext(d,
		 0, uuTHinshi2YesCatch,
		 uuTHinshiYNQuitCatch, uuTHinshi2NoCatch)) == NG) {
      return(GLineNGReturnTK(d));
    }
    ync = (coreContext)d->modec;
    ync->majorMode = IROHA_MODE_ExtendMode;
    ync->minorMode = IROHA_MODE_TourokuHinshiMode;
  } else if(tc->hcode[0]) {
    /* 品詞が決まったので、登録するユーザ辞書の指定を行う */
    return(dicTourokuDictionary(d, uuTDicExitCatch, uuTDicQuitCatch));
  }

  return(retval);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 単語登録の品詞分けする？                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuTHinshiQYesCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;
  coreContext ync;

  popCallback(d); /* yesNo をポップ */

  tc = (tourokuContext)d->modec;

  makeGLineMessage(d, tc->qbuf, strlen(tc->qbuf)); /* 質問 */
  if((retval = getYesNoContext(d,
	 0, uuTHinshi1YesCatch,
	 uuTHinshiYNQuitCatch, uuTHinshi1NoCatch)) == NG) {
    return(GLineNGReturnTK(d));
  }
  ync = (coreContext)d->modec;
  ync->majorMode = IROHA_MODE_ExtendMode;
  ync->minorMode = IROHA_MODE_TourokuHinshiMode;

  return(retval);
}

static
uuTHinshiQNoCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  int uuTDicExitCatch(), uuTDicQuitCatch();

  popCallback(d); /* yesNo をポップ */

  return(dicTourokuDictionary(d, uuTDicExitCatch, uuTDicQuitCatch));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 単語登録の品詞選択                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

dicTourokuHinshiDelivery(d)
uiContext	d;
{
  tourokuContext tc = (tourokuContext)d->modec;
  coreContext ync;
  int uuTDicExitCatch(), uuTDicQuitCatch();
  int retval = 0;

  makeHinshi(d); /* 品詞、エラーメッセージ、質問をセットしてくる */

#ifdef DEBUG
  if(iroha_debug) {
    printf("tc->genbuf=%s, tc->qbuf=%s, tc->hcode=%s\n", tc->genbuf, tc->qbuf,
	   tc->hcode);
  }
#endif
  if(tc->genbuf[0]) {
    /* 入力されたデータに誤りがあったので、
       メッセージを表示して読み入力に戻る */
    clearYomi(d);
    return(dicTourokuTango(d, uuTTangoQuitCatch));
  } else if(tc->qbuf[0] && gramaticalQuestion) {
    /* 細かい品詞分けのための質問をする */
    strcpy(d->genbuf,
	   "さらに細かい品詞分けのための質問をしても良いですか?(y/n)");
    if((retval = getYesNoContext(d,
		 0, uuTHinshiQYesCatch,
		 uuTHinshiYNQuitCatch, uuTHinshiQNoCatch)) == NG) {
      return(GLineNGReturnTK(d));
    }
    makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
    ync = (coreContext)d->modec;
    ync->majorMode = IROHA_MODE_ExtendMode;
    ync->minorMode = IROHA_MODE_TourokuHinshiMode;
    return(retval);
  } else if(tc->hcode[0]) {
    /* 品詞が決まったので、登録するユーザ辞書の指定を行う */
    return(dicTourokuDictionary(d, uuTDicExitCatch, uuTDicQuitCatch));
  }
}

/*
 * 選択された品詞から次の動作を行う
 * 
 * tc->hcode	品詞
 * tc->qbuf	質問
 * tc->genbuf	エラー
 */
makeHinshi(d)
uiContext	d;
{
  tourokuContext tc = (tourokuContext)d->modec;
  int tlen, ylen, yomi_katsuyou, yomilen, sz, retval = 0;
  unsigned char tmpbuf[100];

  tc->hcode[0] = 0;
  tc->qbuf[0] = 0;
  tc->genbuf[0] = 0;

  tlen = tc->tango_len;
  ylen = tc->yomi_len;

  switch(tc->curHinshi) {
  case 0:  /* 人名 */
    strcpy(tc->hcode, "#JN");
    break;

  case 1:  /* 地名 */
    strcpy(tc->hcode, "#CN");
    break;
    
  case 2:  /* 団体・会社名 */
    strcpy(tc->hcode, "#KK");
    break;

  case 3:  /* 名詞 １ */

  case 4:  /* サ変名詞 １ */
    if(tc->curHinshi == 3)
      strcpy(tc->hcode, "#T35"); /* 詳細の品詞を必要としない場合 */
    else
      strcpy(tc->hcode, "#T30"); /* 詳細の品詞を必要としない場合 */
    tc->katsuyou = 0;
    sprintf((char *)tc->qbuf,
	    "「%sな」は正しいですか。(y/n)", (char *)tc->tango_buffer);
    break;

  case 5:  /* 単漢字 */
    strcpy(tc->hcode, "#KJ");
    break;

  case 6:  /* 動詞 １ */

    /* 入力が終止形か？ */
    for(tc->katsuyou=0; tc->katsuyou<GOBISUU &&
	(strncmp(tc->tango_buffer+tlen-2, gyouU[tc->katsuyou], 2) != 0);
							tc->katsuyou++)
      ;
    for(yomi_katsuyou=0; yomi_katsuyou<GOBISUU &&
	(strncmp(tc->yomi_buffer+ylen-2,
		 gyouU[yomi_katsuyou], 2) != 0); yomi_katsuyou++)
      ;
    if((tc->katsuyou == GOBISUU) || (yomi_katsuyou == GOBISUU)){
      strcpy(tc->genbuf, "読みと候補を 終止形で入力してください。");
      return(0);
    }
    if(tc->katsuyou != yomi_katsuyou){
      strcpy(tc->genbuf,
	     "読みと候補の 活用が違います。入力しなおしてください。");
      return(0);
    }

    /* 例外処理 */
    if(!(strcmp(tc->tango_buffer, "くる"))) {
      /* カ行変格活用 */
      strcpy(tc->hcode, "#KX");
      return(0);
    }
    if(!(strcmp(tc->tango_buffer+tlen-6, "んずる"))) {
      /* ンザ行変格活用 */
      strcpy(tc->hcode, "#NZX");
      return(0);
    }
    if(!(strcmp(tc->tango_buffer+tlen-4, "ずる"))) {
      /* ザ行変格活用 */
      strcpy(tc->hcode, "#ZX");
      return(0);
    }
    if(!(strcmp(tc->tango_buffer, "する"))) {
      /* サ行変格活用 */
      strcpy(tc->hcode, "#SX");
      return(0);
    }

    makeDoushi(d); /* 詳細の品詞を必要としない場合 */
    /* 未然形をつくる */
    strncpy(tmpbuf, tc->tango_buffer, tlen-2);  
    strcpy((tmpbuf + tlen - 2), gyouA[tc->katsuyou]);
    strcat(tmpbuf, "ない");
    sprintf((char *)tc->qbuf,
	    "「%s」は正しいですか。(y/n)", (char *)tmpbuf);
    break;

  case 7:  /* 形容詞 １ */
    tc->katsuyou = 1;
    if((strncmp(tc->tango_buffer+tlen-2, "い", 2) != 0) ||
       (strncmp(tc->yomi_buffer+ylen-2, "い", 2) != 0)) {
      strcpy(tc->genbuf, "読みと候補を 終止形で入力してください。例) 早い");
      return(0);
    }
    strcpy(tc->hcode, "#KY"); /* 詳細の品詞を必要としない場合 */
    strncpy(tmpbuf, tc->tango_buffer, tlen-2);  
    tmpbuf[tlen-2] = 0;
    sprintf((char *)tc->qbuf,
	    "「%s」は正しいですか。(y/n)", (char *)tmpbuf);
    break;

  case 8:  /* 形容動詞 １ */
    tc->katsuyou = 1;
    if((strncmp(tc->tango_buffer+tlen-2, "だ", 2)) ||
       (strncmp(tc->yomi_buffer+ylen-2, "だ", 2))) {
      strcpy(tc->genbuf,
	     "読みと候補を 終止形で入力してください。例) 静かだ");
      return(0);
    }
    strcpy(tc->hcode, "#T05"); /* 詳細の品詞を必要としない場合 */
    strncpy(tmpbuf, tc->tango_buffer, tlen-2);  
    strcpy((tmpbuf + tlen - 2), "する");
    sprintf((char *)tc->qbuf,
	    "「%s」は正しいですか。(y/n)", (char *)tmpbuf);
    break;

  case 9:  /* 副詞 １*/
    strcpy(tc->hcode, "#F14"); /* 詳細の品詞を必要としない場合 */
    tc->katsuyou = 0;
    sprintf((char *)tc->qbuf,
	    "「%sする」は正しいですか。(y/n)", (char *)tc->tango_buffer);
    break;

  case 10: /* 数詞 */
    strcpy(tc->hcode, "#NN");
    break;

  case 11: /* 接続詞・感動詞 */
    strcpy(tc->hcode, "#CJ");
    break;

  case 12: /* 連体詞 */
    strcpy(tc->hcode, "#RT");
    break;

  case 13: /* その他の固有名詞 */
    strcpy(tc->hcode, "#KK");
    break;

  case 14:  /* 副詞 ２Ｙ */

  case 15:  /* 副詞 ２Ｎ */
    sprintf((char *)tc->qbuf,
	    "「%sと」は正しいですか。(y/n)", (char *)tc->tango_buffer);
    break;

  case 16:  /* 動詞 ２Ｙ */
    strncpy(tmpbuf, tc->tango_buffer, tlen-2);
    tmpbuf[tlen-2] = '\0';
    strcat(tmpbuf, gyouI[tc->katsuyou]);
    strcat(tmpbuf, "がいい");
    sprintf((char *)tc->qbuf,
	    "「%s」は正しいですか。(y/n)", (char *)tmpbuf);
    break;

  case 17:  /* 動詞 ２Ｎ */
    /* 上下一段活用を作る */
    strncpy(tmpbuf, tc->tango_buffer, tlen-2);
    tmpbuf[tlen-2] = '\0';
    strcat(tmpbuf, "がいい");
    sprintf((char *)tc->qbuf,
	    "「%s」は正しいですか。(y/n)", (char *)tmpbuf);
    break;

  case 18:  /* 形容動詞 ２Ｙ */

  case 19:  /* 形容動詞 ２Ｎ */
    strncpy(tmpbuf, tc->tango_buffer, tlen-2);  
    strcpy((tmpbuf + tlen - 2), "がある");
    sprintf((char *)tc->qbuf,
	    "「%s」は正しいですか。(y/n)", (char *)tmpbuf);
    break;
  }

  return(0);
}

static
tourokuYes(d)
uiContext	d;
{
  tourokuContext tc = (tourokuContext)d->modec;

  tc->hcode[0] = 0;
  tc->qbuf[0] = 0;
  tc->genbuf[0] = 0;

  switch(tc->curHinshi) {
  case 3:  /* 名詞 */
    strcpy(tc->hcode, "#T15");   /* 色々、強力 */
    break;

  case 4:  /* サ変名詞 */
    strcpy(tc->hcode, "#T10");          /* 安心、浮気 */
    break;

  case 6:  /* 動詞 */
    tc->curHinshi = 16;
    makeHinshi(d);
    break;

  case 7:  /* 形容詞 */
    strcpy(tc->hcode, "#KYT");          /* きいろい */
    break;

  case 8:  /* 形容動詞 */
    tc->curHinshi = 18;
    makeHinshi(d);
    break;

  case 9:  /* 副詞 １*/
    tc->curHinshi = 14;
    makeHinshi(d);
    break;

  case 14:  /* 副詞 ２Ｙ */
    strcpy(tc->hcode, "#F04");          /* ふっくら */
    break;

  case 15:  /* 副詞 ２Ｎ */
    strcpy(tc->hcode, "#F06");          /* 突然 */
    break;

  case 16:  /* 動詞 ２Ｙ */
    makeDoushi(d);
    strcat( tc->hcode, "r" );
    break;

  case 17:  /* 動詞 ２Ｎ */
    strcpy(tc->hcode, "#KS");           /* 降りる */
    break;

  case 18:  /* 形容動詞 ２Ｙ */
    strcpy(tc->hcode, "#T10");          /* 関心だ */
    break;

  case 19:  /* 形容動詞 ２Ｎ */
    strcpy(tc->hcode, "#T15");          /* 意外だ、可能だ */
    break;
  }

  return(0);
}

static
tourokuNo(d)
uiContext	d;
{
  tourokuContext tc = (tourokuContext)d->modec;

  tc->hcode[0] = 0;
  tc->qbuf[0] = 0;
  tc->genbuf[0] = 0;

  switch( tc->curHinshi ) {
  case 3:  /* 名詞 */
    strcpy(tc->hcode, "#T35");   /* 山、本 */
    break;

  case 4:  /* サ変名詞 */
    strcpy(tc->hcode, "#T30");          /* 努力、検査 */
    break;

  case 6:  /* 動詞 */
    tc->curHinshi = 17;
    makeHinshi(d);
    break;

  case 7:  /* 形容詞 */
    strcpy(tc->hcode, "#KY");           /* 美しい、早い */
    break;

  case 8:  /* 形容動詞 */
    tc->curHinshi = 19;
    makeHinshi(d);
    break;

  case 9:  /* 副詞 １*/
    tc->curHinshi = 15;
    makeHinshi(d);
    break;

  case 14:  /* 副詞 ２Ｙ */
    strcpy(tc->hcode, "#F12");          /* そっと */
    break;

  case 15:  /* 副詞 ２Ｎ */
    strcpy(tc->hcode, "#F14");          /* 飽くまで */
    break;

  case 16:  /* 動詞 ２Ｙ */
    makeDoushi(d);
    break;

  case 17:  /* 動詞 ２Ｎ */
    strcpy(tc->hcode, "#KSr");          /* 生きる */
    break;

  case 18:  /* 形容動詞 ２Ｙ */
    strcpy(tc->hcode, "#T13");          /* 多慌てだ */
    break;

  case 19:  /* 形容動詞 ２Ｎ */
    strcpy(tc->hcode, "#T18");          /* 便利だ、静かだ */
    break;
  }

  return(0);
}

static
makeDoushi(d)
uiContext	d;
{
  tourokuContext tc = (tourokuContext)d->modec;

    switch(tc->katsuyou){
    case  0:
      strcpy( tc->hcode, "#K5" );     /* 置く */
      break;
    case  1:
      strcpy( tc->hcode, "#G5" );     /* 仰ぐ */
      break;
    case  2:
      strcpy( tc->hcode, "#S5" );     /* 返す */
      break;
    case  3:
      strcpy( tc->hcode, "#T5" );     /* 絶つ */
      break;
    case  4:
      strcpy( tc->hcode, "#N5" );     /* 死ぬ */
      break;
    case  5:
      strcpy( tc->hcode, "#B5" );     /* 転ぶ */
      break;
    case  6:
      strcpy( tc->hcode, "#M5" );     /* 住む */
      break;
    case  7:
      strcpy( tc->hcode, "#R5" );     /* 威張る */
      break;
    case  8:
      strcpy( tc->hcode, "#W5" );     /* 言う */
      break;
    }
}    

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 辞書の一覧                                                                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuTDicExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  forichiranContext fc;
  int cur;

  retval = d->nbytes = 0;

  popCallback(d); /* 一覧を pop */

  fc = (forichiranContext)d->modec;
  cur = fc->curIkouho;

  popForIchiranMode(d);
  popCallback(d);

  return(tangoTouroku(d, cur));
}

static
uuTDicQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* 一覧を pop */

  popForIchiranMode(d);
  popCallback(d);

  return(dicTourokuHinshi(d));
}

dicTourokuDictionary(d, exitfunc, quitfunc)
uiContext d;
int (*exitfunc)();
int (*quitfunc)();
{
  tourokuContext tc = (tourokuContext)d->modec;
  forichiranContext fc;
  ichiranContext ic;
  unsigned char **work;
  unsigned char inhibit = 0;
  int retval, upnelem = 0;

  retval = d->nbytes = 0;
  d->status = 0;

  for(work = tc->udic; *work; work++)
    upnelem++;

  if((retval = getForIchiranContext(d)) == NG) {
    if(tc->udic)
      free(tc->udic);
    return(GLineNGReturnTK(d));
  }
  fc = (forichiranContext)d->modec;

  /* selectOne を呼ぶための準備 */
  fc->allkouho = tc->udic;
  fc->curIkouho = 0;
  if( !HexkeySelect )
    inhibit |= ((unsigned char)NUMBERING | (unsigned char)CHARINSERT); 
  else
    inhibit |= (unsigned char)CHARINSERT;

  if((retval = selectOne(d, fc->allkouho, &fc->curIkouho, upnelem,
		 BANGOMAX, inhibit, 0, WITHOUT_LIST_CALLBACK,
		 0, exitfunc, quitfunc, uiUtilIchiranTooSmall)) == NG) {
    if(fc->allkouho)
      free(fc->allkouho);
    popForIchiranMode(d);
    popCallback(d);
    return(GLineNGReturnTK(d));
  }

  ic = (ichiranContext)d->modec;
  ic->majorMode = IROHA_MODE_ExtendMode;
  ic->minorMode = IROHA_MODE_TourokuDicMode;
  currentModeInfo(d);

  /* 候補一覧行が狭くて候補一覧が出せない */
  if(ic->tooSmall) {
    d->status = AUX_CALLBACK;
    return(retval);
  }

  makeGlineStatus(d);
  /* d->status = ICHIRAN_EVERYTIME; */

  return(retval);
}

/*
 * 単語登録を行う
 */
static
tangoTouroku(d, cur)
uiContext	d;
int             cur;
{
  tourokuContext tc = (tourokuContext)d->modec;
  char ktmpbuf[256];
  char ttmpbuf[256];
  unsigned char line[ROMEBUFSIZE];
  
  if(tc->katsuyou || (strncmp(tc->hcode, "#K5", 3) == 0)) {
#ifdef DEBUG
  if (iroha_debug) {
      fprintf(stderr, "動詞");
  }
#endif
    strncpy(ttmpbuf, tc->tango_buffer, tc->tango_len - 2);
    ttmpbuf[tc->tango_len - 2] = 0;
    strncpy(ktmpbuf, tc->yomi_buffer, tc->yomi_len - 2);
    ktmpbuf[tc->yomi_len - 2] = 0;
  } else {
#ifdef DEBUG
  if (iroha_debug) {
      fprintf(stderr, "動詞以外");
  }
#endif
    strcpy(ttmpbuf, tc->tango_buffer);
    strcpy(ktmpbuf, tc->yomi_buffer);
  }

  /* 辞書書き込み用の一行を作る */
  sprintf((char *)line, "%s %s %s", ktmpbuf, (char *)tc->hcode, ttmpbuf);

#ifdef DEBUG
  if (iroha_debug) {
      fprintf(stderr, "登録するもの(%s) 辞書(%s)\n", line, tc->udic[cur]);
  }
#endif

  if(defaultContext == -1) {
    if((KanjiInit() != 0) || (defaultContext == -1)) {
      necKanjiError = "irohaserverにコネクトできません";
      if(tc->udic)
	free(tc->udic);
      popTourokuMode(d);
      popCallback(d);
      return(GLineNGReturn(d));
    }
  }
  /* 辞書に登録する */
  if(RkDefineDic(defaultContext, tc->udic[cur], line) != 0) {
    if(errno == EPIPE)
      necKanjiPipeError();
    strcpy(d->genbuf, "単語登録できませんでした");
  } else {
    /* 登録の完了を表示する */
    sprintf((char *)d->genbuf, "『%s』（%s）を登録しました",
	    (char *)tc->tango_buffer, (char *)tc->yomi_buffer);
  }
  makeGLineMessage(d, d->genbuf, strlen(d->genbuf));

  popTourokuMode(d);
  popCallback(d);
  currentModeInfo(d);

  return(0);
}
