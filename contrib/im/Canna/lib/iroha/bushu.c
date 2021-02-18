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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/bushu.c,v 3.4 91/08/29 19:00:13 kon Exp $";

#include	<errno.h>
#include "iroha.h"

extern HexkeySelect;

extern int makeGlineStatus(), uuslQuitCatch();
extern int uiUtilIchiranTooSmall(), uuslIchiranQuitCatch();
static int bushuHenkan(), makeBushuIchiranQuit();
static int uuVBushuIchiranQuitCatch();
static int vBushuExitCatch(), bushuQuitCatch();


#define	BUSHU_SZ	150

static
char *bushu_char[] = { "一", "丿", "凵", "十", "卩", "刀",

		       "刈（りっとう）", "力", "厂", "勹", "冂匚囗", "亠",

		       "冫", "人／仁（にんべん）", "又", "几", "八", "儿",

		       "冖", "宀", "廴", "郁（おおざと)", "己", "女",

		       "彳", "口", "草（くさかんむり)", "独（けものへん）",

		       "子", "陏（こざと）", "士", "江（さんずい）", "弋",

		       "尸", "小／単（つ）", "辷（しんにょう）", "寸", "大",

		       "土", "手（てへん）", "巾", "广", "山", "夕",

		       "弓", "忙（りっしんべん）", "欠", "歹", "犬",

		       "牛／牡（うしへん）", "片", "木", "气", "毛", "心",

		       "水", "月", "爪", "日", "攵", "火",

		       "方", "戈", "点（れっか）", "殳", "穴", "石",

		       "玉", "皮", "瓦", "皿", "示", "神（しめすへん）", "白",

		       "田", "立", "禾", "目", "癶", "矢",

		       "疔（やまいだれ）", "四", "糸", "臼", "瓜", "老",

		       "缶", "衣", "初（ころもへん）", "米", "舌", "耒",

		       "竹（たけかんむり）", "血", "虎（とらかんむり）", "肉",

		       "西", "羽", "羊", "聿", "舟", "耳",

		       "虫", "赤", "足／疋", "豕", "臣",

		       "貝", "辛", "車", "見", "言", "酉", "走", "谷",

		       "角", "釆", "麦", "豆", "身", "豸", "雨", "非",

		       "金", "門", "隹", "頁", "音", "香", "革", "風",

		       "首", "食", "韋", "面", "馬", "鬼", "髟", "高",

		       "鬥", "骨", "魚", "亀", "鳥", "黒", "鹿", "鼻",

		       "齒", "記号", "その他", 0
		     };

static
char *bushu_key[] =  { "いち", "の", "うけばこ", "じゅう", "ふし", "かたな",

		       "りっとう", "か", "がん", "く", "かまえ", "なべ", "に",

		       "ひと", "ぬ", "つくえ", "はち", "る", "わ",

		       "う", "えん", "おおざと", "おのれ", "おんな", "ぎょう",

		       "ろ", "くさ", "けもの", "こ", "こざと", "さむらい",

		       "し", "しき", "しゃく", "つ", "しん", "すん",

		       "だい", "ど", "て", "はば", "ま", "やま",

		       "ゆう", "ゆみ", "りっしん", "けつ", "いちた", "いぬ",

		       "うし", "かた", "き", "きがまえ", "け", "こころ",

		       "すい", "つき", "つめ", "にち", "のぶん", "ひ",

		       "ほう", "ほこ", "よつてん", "るまた", "あな", "いし",

		       "おう", "かわ", "かわら", "さら", "しめす", "ね",

		       "しろ", "た", "たつ", "のぎ", "め", "はつ", "や",

		       "やまい", "よん", "いと", "うす", "うり", "おい",

		       "かん", "きぬ", "ころも", "こめ", "した", "すき",

		       "たけ", "ち", "とら", "にく", "にし", "はね", "ひつじ",

		       "ふで", "ふね", "みみ", "むし", "あか", "あし",

		       "いのこ", "おみ", "かい", "からい", "くるま", "けん",

		       "ごん", "さけ", "そう", "たに", "つの", "のごめ",

		       "ばく", "まめ", "み", "むじな", "あめ", "あらず",

		       "かね", "もん", "ふるとり", "ぺーじ", "おと", "こう",

		       "かく", "かぜ", "くび", "しょく", "なめし", "めん",

		       "うま", "おに", "かみ", "たかい", "とう", "ほね",

		       "うお", "かめ", "とり", "くろ", "しか", "はな",

		       "は", "きごう", "そのた", 0
		     };

/*
 * 部首候補のエコー用の文字列を作る
 *
 * 引き数	RomeStruct
 * 戻り値	正常終了時 0
 */
makeBushuEchoStr(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;

  d->kanji_status_return->echoStr = ic->allkouho[*(ic->curIkouho)];
  d->kanji_status_return->length = 2;
  d->kanji_status_return->revPos = 0;
  d->kanji_status_return->revLen = 2;

  return(0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * forichiranContext用関数                                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * forichiranContext の初期化
 */
static
clearForIchiranContext(p)
forichiranContext p;
{
  p->id = FORICHIRAN_CONTEXT;
  p->curIkouho = 0;
  p->allkouho = 0;

  return(0);
}
  
static forichiranContext
newForIchiranContext()
{
  forichiranContext fcxt;

  if((fcxt = 
      (forichiranContext)malloc(sizeof(forichiranContextRec))) == NULL) {
    necKanjiError = "malloc (newForIchiranContext) できませんでした";
    return(0);
  }
  clearForIchiranContext(fcxt);

  return fcxt;
}

static void
freeForIchiranContext(fc)
forichiranContext fc;
{
  free(fc);
}

getForIchiranContext(d)
uiContext d;
{
  forichiranContext fc;
  int retval = 0;

  if(pushCallback(d, d->modec, 0, 0, 0, 0) == 0) {
    necKanjiError = "malloc (pushCallback) できませんでした";
    return(NG);
  }
  
  if((fc = newForIchiranContext()) == NULL) {
    popCallback(d);
    return(NG);
  }
  fc->next = d->modec;
  d->modec = (mode_context)fc;

  fc->prevMode = d->current_mode;
  fc->majorMode = d->majorMode;

  return(retval);
}

void
popForIchiranMode(d)
uiContext d;
{
  forichiranContext fc = (forichiranContext)d->modec;

  d->modec = fc->next;
  d->current_mode = fc->prevMode;
  freeForIchiranContext(fc);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ユーティリティの部首モード入力                                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uuVBushuExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  forichiranContext fc;
  int cur;

  popCallback(d); /* 一覧をポップ */

  fc = (forichiranContext)d->modec;
  cur = fc->curIkouho;

  popForIchiranMode(d);
  popCallback(d);

  return(bushuHenkan(d, 1, 0, cur, uuVBushuIchiranQuitCatch));
}

static
uuVBushuIchiranQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* 一覧をポップ */

  popForIchiranMode(d);
  popCallback(d);

  return(vBushuMode(d, 0, uuVBushuExitCatch, uuslIchiranQuitCatch));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 部首モード入力                                                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
vBushuIchiranQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* 一覧をポップ */

  popForIchiranMode(d);
  popCallback(d);

  return(vBushuMode(d, 1, vBushuExitCatch, bushuQuitCatch));
}

static
vBushuExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  forichiranContext fc;
  int cur;

  popCallback(d); /* 一覧をポップ */

  fc = (forichiranContext)d->modec;
  cur = fc->curIkouho;

  popForIchiranMode(d);
  popCallback(d);

  return(bushuHenkan(d, 1, 1, cur, vBushuIchiranQuitCatch));
}

BushuMode(d)
uiContext d;
{
  int bushuQuitCatch();
  yomiContext yc = (yomiContext)d->modec;

  if (yc->generalFlags & IROHA_YOMI_CHGMODE_INHIBITTED) {
    return NothingChangedWithBeep(d);
  }    

  return(vBushuMode(d, 1, vBushuExitCatch, bushuQuitCatch));
}

vBushuMode(d, flag, exitfunc, quitfunc)
uiContext d;
int flag;
int (*exitfunc)();
int (*quitfunc)();
{
  forichiranContext fc;
  ichiranContext ic;
  unsigned char inhibit = 0;
  int retval = 0;

  d->status = 0;

  if((retval = getForIchiranContext(d)) == NG) {
    return(GLineNGReturn(d));
  }
  fc = (forichiranContext)d->modec;

  /* selectOne を呼ぶための準備 */
  fc->allkouho = (unsigned char **)bushu_char;
  fc->curIkouho = 0;
  if( !HexkeySelect )
    inhibit |= ((unsigned char)NUMBERING | (unsigned char)CHARINSERT);
  else
    inhibit |= (unsigned char)CHARINSERT;

  if((retval = selectOne(d, fc->allkouho, &fc->curIkouho, BUSHU_SZ,
		 BANGOMAX, inhibit, 0, WITH_LIST_CALLBACK,
		 0, exitfunc,
		 quitfunc, uiUtilIchiranTooSmall)) == NG) {
    return(GLineNGReturnFI(d));
  }

  ic = (ichiranContext)d->modec;
  if(flag)
    ic->majorMode = ic->minorMode = IROHA_MODE_BushuMode;
  else {
    ic->majorMode = IROHA_MODE_ExtendMode;
    ic->minorMode = IROHA_MODE_BushuMode;
  }
  currentModeInfo(d);

  *(ic->curIkouho) = d->curbushu;

  /* 候補一覧行が狭くて候補一覧が出せない */
  if(ic->tooSmall) {
    d->status = AUX_CALLBACK;
    return(retval);
  }

  if ( !(ic->flags & ICHIRAN_ALLOW_CALLBACK) ) {
    makeGlineStatus(d);
  }
  /* d->status = ICHIRAN_EVERYTIME; */

  return(retval);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 部首モード入力の一覧表示                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
bushuEveryTimeCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  makeBushuEchoStr(d);

  return(retval);
}

static
bushuExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* 一覧をポップ */

  if (((forichiranContext)env)->allkouho != (unsigned char **)bushu_char) {
    /* bushu_char は static の配列だから free してはいけない。
       こう言うのってなんか汚いなあ */
    freeGetIchiranList(((forichiranContext)env)->allkouho);
  }
  popForIchiranMode(d);
  popCallback(d);
  retval = YomiExit(d, retval);
  currentModeInfo(d);

  return retval;
}

static
bushuQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* 一覧をポップ */

  if (((forichiranContext)env)->allkouho != (unsigned char **)bushu_char) {
    /* bushu_char は static の配列だから free してはいけない。
       こう言うのってなんか汚いなあ */
    freeGetIchiranList(((forichiranContext)env)->allkouho);
  }
  popForIchiranMode(d);
  popCallback(d);
  currentModeInfo(d);
  GlineClear(d);

  return(retval);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 部首としての変換の一覧表示                                                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
convBushuQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* 一覧をポップ */

  if (((forichiranContext)env)->allkouho != (unsigned char **)bushu_char) {
    /* bushu_char は static の配列だから free してはいけない。
       こう言うのってなんか汚いなあ */
    freeGetIchiranList(((forichiranContext)env)->allkouho);
  }
  popForIchiranMode(d);
  popCallback(d);

  makeYomiReturnStruct(d);
  currentModeInfo(d);

  return(retval);
}

/*
 * 読みを部首として変換する
 *
 * 引き数	uiContext
 * 戻り値	正常終了時 0	異常終了時 -1
 */
ConvertAsBushu(d)
uiContext	d;
{
  yomiContext yc = (yomiContext)d->modec;

  d->status = 0; /* clear status */
  
  if (yc->henkanInhibition & IROHA_YOMI_INHIBIT_ASBUSHU) {
    return NothingChangedWithBeep(d);
  }

  if (yc->henkanCallback) {
    strncpy(d->buffer_return, yc->kana_buffer, d->nbytes);
    return yc->henkanCallback(d, IROHA_FN_ConvertAsBushu,
			      d->genbuf, d->nbytes);
  }

  d->nbytes = yc->kEndp;
  strncpy(d->buffer_return, yc->kana_buffer, d->nbytes);

  /* 0 は、ConvertAsBushu から呼ばれたことを示す */
  return(bushuHenkan(d, 0, 1, 0, convBushuQuitCatch));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 共通部                                                                    *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * 読みを部首辞書から部首変換する
 */
static
bushuBgnBun(d, st, yomi, length)
uiContext d;
RkStat *st;
unsigned char *yomi;
int length;
{
  int nbunsetsu;

  /* 連文節変換を開始する *//* 辞書にある候補のみ取り出す */
  if(d->bushuContext == -1) {
    if((defaultBushuContext == -1) && (KanjiInit() == -1)) {
	necKanjiError = "irohaserverにコネクトできません";
	return(NG);
    } else {
      d->bushuContext = RkDuplicateContext(defaultBushuContext);
    }
  }
  if((nbunsetsu = 
      RkBgnBun(d->bushuContext, yomi, length, 0))
	== -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "かな漢字変換に失敗しました";
    return(NG);
  }
  
  if(RkGetStat(d->bushuContext, st) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "ステイタスを取り出せませんでした";
    return(NG);
  }

  return(nbunsetsu);
}

/*
 * 読みに半濁点を付加して候補一覧行を表示する
 *
 * 引き数	uiContext
 *		flag	ConvertAsBushuから呼ばれた 0
 *			BushuYomiHenkanから呼ばれた 1
 * 戻り値	正常終了時 0	異常終了時 -1
 *
 *
 * ここに来る時はまだ getForIchiranContext が呼ばれていないものとする
 */

static
bushuHenkan(d, flag, ext, cur, quitfunc)
uiContext	d;
int             flag, cur;
int             (*quitfunc)();
{
  forichiranContext fc;
  ichiranContext ic;
  unsigned char *yomi, *ptr, **buf, inhibit = 0, **allBushuCands;
  RkStat	st;
  int nelem, currentkouho, nbunsetsu, length, retval = 0;
  
  unsigned char **getIchiranList();

  if(flag) {
    yomi = (unsigned char *)bushu_key[cur];
    length = strlen(yomi);
    d->curbushu = (short)cur;
  } else {
    d->nbytes = RomajiFlushYomi(d, d->buffer_return, d->bytes_buffer);
    yomi = d->buffer_return;
    length = d->nbytes;
  }

  if((nbunsetsu = bushuBgnBun(d, &st, yomi, length)) == NG)
    return(GLineNGReturn(d));

  if((nbunsetsu != 1) || (st.klen > 2) || (st.maxcand == 0)) {
    /* 部首としての候補がない */

#ifdef DEBUG
  if (iroha_debug) {
  
    char names[256];
    char *sap;
    int satoko, i;
    
    fprintf(stderr, "nbunsetsu(%d), klen(%d), maxcand(%d)\n", 
	    nbunsetsu, st.klen, st.maxcand);

    satoko = RkGetMountList(d->bushuContext, names, sizeof(names));
    for(i=0, sap = names; i<satoko; i++) {
      fprintf(stderr, "%s,  ", sap);
      while(*sap++);
    }

    fprintf(stderr, "context = %d\n", d->bushuContext);
  }
#endif
    
    d->kanji_status_return->length = -1;

    makeBushuIchiranQuit(d, flag);
    currentModeInfo(d);

    if(flag) {
      strcpy(d->genbuf, "この部首の候補はありません");
      makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
    } else {
      return(NothingChangedWithBeep(d));
    }

    return(0);
  }

  /* 候補一覧行を表示する */
  /* 0 は、カレント候補 + 0 をカレント候補にすることを示す */

  if((allBushuCands
      = getIchiranList(d, d->bushuContext, &nelem, &currentkouho)) == 0) {
    return(GLineNGReturn(d));
  }

  /* 部首変換は学習しない。 */
  if(RkEndBun(d->bushuContext, 0) == -1) {	/* 0:学習しない */
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "かな漢字変換の終了に失敗しました";
    freeGetIchiranList(allBushuCands);
    return(GLineNGReturn(d));
  }

  if(getForIchiranContext(d) == NG) {
    freeGetIchiranList(allBushuCands);
    return(GLineNGReturn(d));
  }

  fc = (forichiranContext)d->modec;
  fc->allkouho = allBushuCands;

  if( !HexkeySelect )
    inhibit |= (unsigned char)NUMBERING;
  fc->curIkouho = currentkouho;	/* 現在のカレント候補番号を保存する */
  currentkouho = 0;	/* カレント候補から何番目をカレント候補とするか */

  if((retval = selectOne(d, fc->allkouho, &fc->curIkouho, nelem, BANGOMAX,
			 inhibit, currentkouho, WITH_LIST_CALLBACK,
			 bushuEveryTimeCatch, bushuExitCatch,
			 quitfunc, uiUtilIchiranTooSmall)) == NG) {
    freeGetIchiranList(allBushuCands);
    return(GLineNGReturnFI(d));
  }

  ic = (ichiranContext)d->modec;

  if(!flag) { /* convertAsBushu */
    ic->majorMode = ic->minorMode = IROHA_MODE_BushuMode;
  } else {
    if(ext) {
      ic->majorMode = IROHA_MODE_BushuMode;
      ic->minorMode = IROHA_MODE_TankouhoMode;
    } else {
      ic->majorMode = IROHA_MODE_ExtendMode;
      ic->minorMode = IROHA_MODE_TankouhoMode;
    }
  }
  currentModeInfo(d);

  /* 候補一覧行が狭くて候補一覧が出せない */
  if(ic->tooSmall) {
    d->status = AUX_CALLBACK;
    return(retval);
  }

  if ( !(ic->flags & ICHIRAN_ALLOW_CALLBACK) ) {
    makeGlineStatus(d);
  }
  /* d->status = EVERYTIME_CALLBACK; */

  return(retval);
}

/*
 * 候補行を消去し、部首モードから抜け、読みがないモードに移行する
 *
 * 引き数	uiContext
 *		flag	ConvertAsBushuから呼ばれた 0
 *			BushuYomiHenkanから呼ばれた 1
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
makeBushuIchiranQuit(d, flag)
uiContext	d;
int              flag;
{

  /* 部首変換は学習しない。 */
  if(RkEndBun(d->bushuContext, 0) == -1) {	/* 0:学習しない */
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "かな漢字変換の終了に失敗しました";
    return(NG);
  }

  if(flag) {
    /* kanji_status_return をクリアする */
    d->kanji_status_return->length  = 0;
    d->kanji_status_return->revLen  = 0;
    
    d->status = QUIT_CALLBACK;
  } else {
    makeYomiReturnStruct(d);
  }
  GlineClear(d);
  
  return(0);
}
