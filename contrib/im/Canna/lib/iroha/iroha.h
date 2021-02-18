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

/*
 * @(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/iroha.h,v 3.4 91/09/03 18:54:16 kon Exp $
 * $Revision: 3.4 $
 * $Author: kon $
 */
     
#include <stdio.h>
#include "sglobal.h"
#include <iroha/RK.h>
#include <iroha/jrkanji.h>

#include <memory.h>
#define bzero(buf, size) memset((char *)(buf), 0x00, (size))
#define bcopy(src, dst, size) memcpy((char *)(dst), (char *)(src), (size))

/*
 * LIBDIR  -- システムのカスタマイズファイルやローマ字かな変換テーブルが
 *            置かれるディレクトリ。
 */

#ifndef LIBDIR
#define LIBDIR "/usr/lib/iroha"
#endif

#define XKanjiStatus          jrKanjiStatus
#define XKanjiStatusWithValue jrKanjiStatusWithValue

/*
 * glineinfo -- 候補一覧表示のための内部情報を格納しておくための構造体。
 * それぞれのメンバは以下の意味を持つ。
 *
 * glkosu -- その行にある候補の数
 * glhead -- その行の先頭候補が、kouhoinfoの何番目か(0から数える)
 * gllen  -- その行を表示するための文字列の長さ
 * gldata -- その行を表示するための文字列へのポインタ
 */

typedef struct {
  int glkosu;
  int glhead;
  int gllen;
  unsigned char *gldata;
} glineinfo;

/*
 * kouhoinfo -- 候補一覧のための内部情報を格納しておくための構造体
 * それぞれのメンバは以下の意味を持つ。
 *
 * khretsu -- その候補がある行
 * khpoint -- その候補の行のなかでの位置
 * khdata -- その候補の文字列へのポインタ
 */

typedef struct {
  int khretsu;
  int khpoint;
  unsigned char *khdata;
} kouhoinfo;

#define ROMEBUFSIZE 	1024
#define	BANGOSIZE	4	/* 候補行中の各候補の番号のバイト数 */
#define	REVSIZE		2	/* 候補行中の番号のリバースのバイト数 */
#define	BANGOMAX   	9	/* １候補行中の最大候補数 */
#define KIGOSIZE	2	/* 記号候補のバイト数 */
#define GL_KUHAKUSIZE	2	/* 候補番号の空白文字のバイト数 */
#define KG_KUHAKUSIZE	2	/* 記号の間の空白文字のバイト数 */
#define KIGOCOL		(KIGOSIZE + KG_KUHAKUSIZE)
					/* bangomaxを計算するための数 */
#define	KIGOBANGOMAX   	16	/* １候補行中の最大候補数 */
#define HINSHISUU	14
#define HINSHIBUF	256
#define GOBISUU		9

#define	ON		1
#define	OFF		0

#define ZENHIRA		IROHA_MODE_ZenHiraKakuteiMode	/* 5 確定入力モード */
#define ZENKATA		IROHA_MODE_ZenKataKakuteiMode	/* 6 確定入力モード */
#define HANKATA		IROHA_MODE_HanKataKakuteiMode	/* 7 確定入力モード */

#define	NG		-1

#define NO_CALLBACK	0
#define NCALLBACK	4

#define	JISHU_HIRA	0
#define JISHU_ZEN_KATA	1
#define JISHU_HAN_KATA	2
#define JISHU_ZEN_ALPHA	3
#define JISHU_HAN_ALPHA	4
#define MAX_JISHU	5

#define  SENTOU        1
#define  HENKANSUMI    2
#define  WARIKOMIMOJI  4
#define  SHIRIKIRE     8
#define  ATAMAKIRE    16

typedef char *mode_context;

struct callback {
  int (*func[NCALLBACK])();
  mode_context    env;
  struct callback *next;
};

/* identifier for each context structures */
#define CORE_CONTEXT       (char)0
#define YOMI_CONTEXT       (char)1
#define JISHU_CONTEXT      (char)2
#define HENKAN_CONTEXT     (char)3
#define ICHIRAN_CONTEXT    (char)4
#define FORICHIRAN_CONTEXT (char)5
#define MOUNT_CONTEXT      (char)6
#define TOUROKU_CONTEXT    (char)7
#define ADJUST_CONTEXT     (char)8

typedef struct _coreContextRec {
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode; /* １つ前のモード */
  mode_context    next;
} coreContextRec, *coreContext;

typedef struct  _yomiContextRec {
  /* core 情報と同じ情報 */
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode;	/* １つ前のモード */
  mode_context    next;

  /* ローマ字かな変換関係 */
  unsigned char   romaji_buffer[ROMEBUFSIZE];
  /* ローマ字バッファは rStartp, rEndp の２つのインデックスによって管理され
   * る。rStartp はカナに変換できなかったローマ字の最初の文字へのインデッ
   * クスであり、rEndp は新たにローマ字を入力する時に、格納すべき 
   * romaji_buffer 内のインデックスである。新たに入力されるローマ字は、
   * romaji_buffer + rEndp より先に格納され、そのローマ字をカナに変換す
   * る時は、romaji_buffer + rStartp から rEndp - rStartp バイトの文字が
   * 対象となる。 */
  int		  rEndp, rStartp, rCurs; /* ローマ字バッファのポインタ */
  unsigned char   kana_buffer[ROMEBUFSIZE];
  unsigned char   rAttr[ROMEBUFSIZE], kAttr[ROMEBUFSIZE];
  int		  kEndp; /* かなバッファの最後を押えるポインタ */
  int             kRStartp, kCurs;

  /* その他のオプション */
  char		  generalFlags;		/* see below */
  char		  allowedChars;		/* see jrkanji.h */
  char		  henkanInhibition;	/* see below */
  char		  baseChar;		/* see below */
  int		  (*henkanCallback)();	/* 変換を行う時にこの変数にアドレス
             が設定されているならばそのアドレスの関数を通常の関数の代わりに
             呼ぶ。設定されていない時は通常の関数を呼ぶ。
               部首変換などで変換キーが押された時に特殊な処理が行われるのに
             対応するために付けた。
	       変換コールバックは引数としてどの変換関数が呼ばれたのかを示す
	     番号を伴う。*/

  /* 作業用変数 */
  char		  *retbuf, *retbufp;
  int		  retbufsize;
} yomiContextRec, *yomiContext;

#define IROHA_YOMI_BREAK_ROMAN		1
#define IROHA_YOMI_CHGMODE_INHIBITTED	2
#define IROHA_YOMI_END_IF_KAKUTEI	4
#define IROHA_YOMI_DELETE_DONT_QUIT	8

#define IROHA_YOMI_INHIBIT_NONE		0
#define IROHA_YOMI_INHIBIT_HENKAN	1
#define IROHA_YOMI_INHIBIT_JISHU	2
#define IROHA_YOMI_INHIBIT_ASHEX	4
#define IROHA_YOMI_INHIBIT_ASBUSHU	8
#define IROHA_YOMI_INHIBIT_ALL		15

/* 候補一覧のためのフラグ */
#define NUMBERING 			1
#define CHARINSERT			2

#define IROHA_JISHU_UPPER		1
#define IROHA_JISHU_CAPITALIZE		2

typedef struct _jishuContextRec {
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode;	/* 前のモード */
  mode_context    next;

  unsigned char	 inhibition;
  unsigned char  jishu_buffer[ROMEBUFSIZE];
  int            jishu_len, jishu_kEndp, jishu_rEndp;
  unsigned char  kc, jishu_case;
  yomiContext    ycx;
} jishuContextRec, *jishuContext;

typedef struct _henkanContextRec {
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode;	/* １つ前のモード */
  mode_context    next;

  /* カナ漢字変換関係 */
  int            context;
  int		 kouhoCount;	/* 何回 henkanNext が連続して押されたか */
  unsigned char  yomi_buffer[ROMEBUFSIZE];
  unsigned char  echo_buffer[ROMEBUFSIZE];
  unsigned char  **allkouho; /* RkGetKanjiListで得られる文字列を配列にして
				とっておくところ */
  int            yomilen;    /* 読みの長さ、読み自身は kana_buffer に入れ
			        られる */
  int            curbun;     /* カレント文節 */
  int		 curIkouho;  /* カレント候補 */
  int            nbunsetsu;  /* 文節の数 */
#define MAXNBUNSETSU	256
  int            kugiri[MAXNBUNSETSU]; /* 文節分けを行う時の文節くぎ
					  りの情報。 */
  int		 *kanaKugiri, *romajiKugiri, *jishubun;
  yomiContext    ycx;
/* ifdef MEASURE_TIME */
  long		 proctime;   /* 処理時間(変換で計測する) */
  long		 rktime;     /* 処理時間(RKにかかる時間) */
/* endif MEASURE_TIME */
} henkanContextRec, *henkanContext;

typedef struct _ichiranContextRec {
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode;	/* １つ前のモード */
  mode_context    next;

  int            svIkouho;   /* カレント候補を一時とっておく(一覧表示行) */
  int            *curIkouho; /* カレント候補 */
  int            nIkouho;    /* 候補の数(一覧表示行) */
  int		 tooSmall;   /* カラム数が狭くて候補一覧が出せないよフラグ */
  int            curIchar;   /* 未確定文字列ありの単語登録の単語入力の
    							先頭文字の位置 */
  unsigned char  inhibit;
  unsigned char  flags;	     /* 下を見てね */
  unsigned char  **allkouho; /* RkGetKanjiListで得られる文字列を配列にして
				とっておくところ */
  unsigned char  *glinebufp; /* 候補一覧のある一行を表示するための文字
				列へのポインタ */
  kouhoinfo      *kouhoifp;  /* 候補一覧関係の情報を格納しておく構造体
				へのポインタ */
  glineinfo      *glineifp;  /* 候補一覧関係の情報を格納しておく構造体
				へのポインタ */
} ichiranContextRec, *ichiranContext;

/* フラグの意味 */
#define ICHIRAN_ALLOW_CALLBACK 1 /* コールバックをしても良い */


typedef struct _forichiranContextRec {
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode;	/* １つ前のモード */
  mode_context    next;

  int            curIkouho;  /* カレント候補 */
  unsigned char  **allkouho; /* RkGetKanjiListで得られる文字列を配列にして
				とっておくところ */
} forichiranContextRec, *forichiranContext;

typedef struct _mountContextRec {
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode;	/* １つ前のモード */
  mode_context    next;

  unsigned char   *mountOldStatus; /* マウントされているかいないか */
  unsigned char   *mountNewStatus; /* マウントされているかいないか */
  unsigned char  **mountList;   /* マウント可能な辞書の一覧 */
  int            curIkouho;     /* カレント候補 */
} mountContextRec, *mountContext;

typedef struct _tourokuContextRec {
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode;	/* １つ前のモード */
  mode_context    next;

  unsigned char  genbuf[ROMEBUFSIZE];
  unsigned char  qbuf[ROMEBUFSIZE];
  unsigned char  tango_buffer[ROMEBUFSIZE];
  int            tango_len;  /* 単語登録の単語の文字列の長さ */
  unsigned char  yomi_buffer[ROMEBUFSIZE];
  int            yomi_len;   /* 単語登録の読みの文字列の長さ */
  int            curHinshi;  /* 品詞の選択 */
  char		 hcode[16];   /* 単語登録の品詞 */
  int		 katsuyou;   /* 単語登録の動詞の活用形 */
  unsigned char  **udic;     /* 単語登録できる辞書 (辞書名) */
  int            delContext; /* 単語削除で１つの辞書をマウントする */
} tourokuContextRec, *tourokuContext;

typedef struct _adjustContextRec {
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode;	/* １つ前のモード */
  mode_context    next;

  yomiContext    ycx;
  henkanContext  hcx;

  unsigned char  genbuf[ROMEBUFSIZE];
  int            kanjilen, strlen, bunlen; /* 漢字部分、全体、文節の長さ */
} adjustContextRec, *adjustContext;

struct moreTodo {
  char          todo; /* もっとあるの？を示す */
  char          fnum; /* 関数番号。０なら次の文字で示されることをする */
  unsigned char ch;   /* 文字 */
};

/* モード名を格納するデータの型定義 */

struct ModeNameRecs {
  int           alloc;
  unsigned char *name;
};

/* 

  uiContext はローマ字かな変換、カナ漢字変換に使われる構造体である。
  XLookupKanjiString などによる変換は、ウィンドウに分離された複数の入
  力ポートに対応しているので、入力中のローマ字の情報や、カナ漢字変換
  の様子などをそれぞれのウィンドウ毎に分離して保持しておかなければな
  らない。この構造体はそのために使われる構造体である。
 
  構造体のメンバがどのようなものがあるかは、定義を参照すること
 
 */

typedef struct {

  /* XLookupKanjiStringのパラメタ */
  unsigned char  *buffer_return;
  int            bytes_buffer;
  jrKanjiStatus  *kanji_status_return;

  /* XLookupKanjiStringの戻り値である文字列の長さ */
  int		 nbytes;

  /* キャラクタ */
  unsigned char  ch;

  /* セミグローバルデータ */
  int		 contextCache;	 /* 変換コンテクストキャッシュ */
  int		 bushuContext;	 /* 部首変換コンテクスト */
  KanjiMode      current_mode;
  char		 majorMode, minorMode;	 /* 直前のもの */

  short		 curkigo;	 /* カレント記号(記号全般) */
  char           currussia;	 /* カレント記号(ロシア文字) */
  char           curgreek;	 /* カレント記号(ギリシャ文字) */
  char           curkeisen;	 /* カレント記号(罫線) */
  short          curbushu;       /* カレント部首名 */
  int            ncolumns;	 /* 一行のコラム数、候補一覧の時に用いられる */
  unsigned char  genbuf[ROMEBUFSIZE];	/* 汎用バッファ */

  /* リストコールバック関連 */
  char           *client_data;   /* アプリケーション用データ */
  void           (*list_func)(); /* リストコールバック関数 */

  /* その他 */
  char		 flags;		 /* 下を見てね */
  char		 status;	 /* どのような状態で返ったのかを示す値
				    そのモードを、
				     ・処理中
				     ・処理終了
				     ・処理中断
				     ・その他
				    などが考えられる。(下を見よ) */

  /* コールバックチェーン */
  struct callback *cb;

  /* もっとするときがあるよという構造体 */
  struct moreTodo more;

  /* サブコンテクストへのリンク */
  mode_context   modec;		/* 全部ここにつなぐ予定 */
} uiContextRec, *uiContext;

/* uiContext の flags のビットの意味 */
#define PLEASE_CLEAR_GLINE	1	/* GLine を消してね */
#define PCG_RECOGNIZED		2	/* GLine を次は消しますよ */
#define MULTI_SEQUENCE_EXECUTED	4	/* さっきマルチシーケンスが行われた */

int defaultContext, defaultBushuContext;

#define EVERYTIME_CALLBACK	0
#define EXIT_CALLBACK		1
#define QUIT_CALLBACK		2
#define AUX_CALLBACK		3

/* ローマ字かな変換辞書 */
/*
 * ローマ字かな変換テーブルは１個あれば良いでしょう。複数個必要なので
 * あれば RomeStruct のメンバとして入れておく必要もありましょうが...そ
 * の時はその時で考えましょう。
 */
     
extern struct RkRxDic *romajidic, *RkOpenRoma();

/*
 * 辞書の名前を入れておく変数
 */

#define MAX_DICS 16

extern char *kanjidicname[];
extern int  nkanjidics;

extern char *userdicname[];
extern int  nuserdics;
extern char userdicstatus[];

extern char *bushudicname[];
extern int nbushudics;

extern char *localdicname[];
extern int nlocaldics;

/*
 * エラーのメッセージを入れておく変数
 */

extern char *necKanjiError;

/*
 * デバグ文を表示するかどうかのフラグ
 */

extern iroha_debug;

/*
 * 16進コード入力を一覧行に表示するかどうかを調べる条件。
 */

#define hexGLine(plen) (d->ncolumns >= plen + 4)

/*
 * キーシーケンスを発生するようなキー
 */

#define IrohaFunctionKey(key) \
  ((0x80 <= (int)(unsigned char)(key) &&  \
    (int)(unsigned char)(key) <= 0x8b) || \
   (0x90 <= (int)(unsigned char)(key) &&  \
    (int)(unsigned char)(key) <= 0x9b) || \
   (0xe0 <= (int)(unsigned char)(key) &&  \
    (int)(unsigned char)(key) <= 0xff) )

/* selectOne でコールバックを伴うかどうかを表すマクロ */

#define WITHOUT_LIST_CALLBACK 0
#define WITH_LIST_CALLBACK    1

/*
 * Rk 関数をトレースするための名前の書き換え。
 */

#ifdef DEBUG
#include "traceRK.h"
#endif /* DEBUG */

/*
 * デバグメッセージ出力用のマクロ
 */

#ifdef DEBUG
#define debug_message(fmt, x, y, z)	dbg_msg(fmt, x, y, z)
#else /* !DEBUG */
#define debug_message(fmt, x, y, z)
#endif /* !DEBUG */

/*
 * malloc のデバグ
 */

#ifdef DEBUG_ALLOC
extern int fail_malloc;
#define malloc(n) debug_malloc(n)
#endif /* DEBUG_MALLOC */
