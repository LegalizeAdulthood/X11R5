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

#ifndef DEBUG
#define DEBUG
#endif

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/util/sample/RCS/sample.c,v 2.22 91/08/28 21:51:49 kon Exp $";

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/times.h>
#include <iroha/kanji.h>
  
#define ECHO
  
static char sccs_id[] = "@(#)echowin.c	2.5	88/09/29 10:45:43";

#define ctl(x) ((x) & 0x1f) /* コントロールキーを表すマクロ ctl('a') 
			       のように使う */

#define InnerBorder   10
#define OuterBorder   5
#define RomanFont14   "a14"
#define RomanFont16   "neca16"
#define RomanFont24   "neca24"
#define KanjiFont14   "k14"
#define KanjiFont16   "neck16"
#define KanjiFont24   "neck24"
#define WinWidth      80	/* [文字] */
#define STRBUFSIZE    512	/* バイト */

Display     *dpy;
Window      win, parwin;
int         ww = 0;
Font        fn, fk;
GC          gcs[2], rgcs[2], mgcs[2];
XFontStruct *fonts[2];
int         asc, des, cw;	/* 文字のアセント/ディセント/幅 */
int         CursorWidth;
int	    numControl = 0;
int	    jrik = 0;		/* 半角カタカナの禁止 */
int         showkeys = 0;	/* アルファモードで使っているキーを示す */
int	    esckill = 0, esckakutei = 0, escControl = 0, ckconn = 0;
int	    double_init = 0, double_final = 0, final_initial = 0;
int	    given_width, width_given = 0, showMode = 0, kugiri = 0;
int	    printenv = 0;
int         yomiLevel = 0;
int         customtest = 0;
int         storeYomi = 0, storeThen = 0;
int         given_nbytes, nbytes_given = 0;
char        *rfont = NULL, *kfont = NULL;
int         msystime = 0, kstime = 0;
int         parse_data = 0;
char        parse_buf[512];
int         rengo_data = 0;
char        rengo_buf[512];
int         check_context = 0;
int	    no_mode = 0;
int	    show_max_mode = 0;
int	    max_mode_strlen = 0;
int         restrict = 0;
int         escclose = 0;
#ifdef TEISEI
int	    teiseiline = 0;
int	    kanaline = 0;
#endif /* TEISEI */
#ifdef KC_SETSERVERNAME
int	    clist = 0;
#endif

static int glinePosition;

typedef struct {
  int length;
  char str[STRBUFSIZE];
  int revPos, revLen, width, prevWidth;
} strRec;

typedef struct {
  Window win;
  strRec kakutei, mikakutei, mode, gline;
#ifdef TEISEI
  strRec teisei;
  strRec kana;
#endif /* TEISEI */
  int    cursor;
} winRec;

#define DRAW 1
#define NODRAW 0

winRec wb[2];

extern iroha_debug;

int hexBy = 0, modeString = 0, undeffn = kc_normal;

main(argc, argv,envp)
int argc;
char *argv[], *envp[];
{
  int i;
  extern (*jrBeepFunc)();
  int beep();

  jrBeepFunc = beep;

  for (i = 1 ; i < argc ; i++) {
    if ( !strcmp(argv[i], "-debug") ) {
      iroha_debug |= 1;
    }
    else if ( !strcmp(argv[i], "-nc") ) {
      numControl = 1;
    }
    else if ( !strcmp(argv[i], "-nmode") ) {
      modeString = 1;
    }
    else if ( !strcmp(argv[i], "-imhex") ) {
      hexBy = 1;
    }
    else if ( !strcmp(argv[i], "-kakutei") ) {
      undeffn = kc_kakutei;
    }
    else if ( !strcmp(argv[i], "-through") ) {
      undeffn = kc_through;
    }
    else if ( !strcmp(argv[i], "-kill") ) {
      undeffn = kc_kill;
    }
    else if ( !strcmp(argv[i], "-bell") ) {
      jrBeepFunc = 0;
    }
    else if ( !strcmp(argv[i], "-ik") ) {
      jrik = 1;
    }
    else if ( !strcmp(argv[i], "-showkeys") ) {
      showkeys = 1;
    }
    else if ( !strcmp(argv[i], "-esckill") ) {
      esckill = 1;
      escControl = 1;
    }
    else if ( !strcmp(argv[i], "-esckakutei") ) {
      esckakutei = 1;
      escControl = 1;
    }
    else if ( !strcmp(argv[i], "-ckconn") ) {
      ckconn = 1;
    }
    else if ( !strcmp(argv[i], "-dinit") ) {
      double_init = 1;
}
    else if ( !strcmp(argv[i], "-dfinal") ) {
      double_final = 1;
    }
    else if ( !strcmp(argv[i], "-fin_init") ) {
      final_initial = 1;
    }
    else if ( !strcmp(argv[i], "-width") ) {
      width_given = 1;
      i++;
      given_width = atoi(argv[i]);
    }
    else if ( !strcmp(argv[i], "-escmode") ) {
      showMode = 1;
    }
    else if ( !strcmp(argv[i], "-kugiri") ) {
      kugiri = 1;
    }
    else if ( !strcmp(argv[i], "-printenv") ) {
      printenv = 1;
    }
    else if ( !strncmp(argv[i], "-bigmem", 7) ) {
      char *malloc();
      int megabytes = atoi(argv[i] + 7);

      if (argv[i][7] == '\0') {
	megabytes = 2;
      }
      fprintf(stderr, "%d MByte alloc しています...", megabytes);
      if (malloc(megabytes * 1024 * 1024) == (char *)0) {
	fprintf(stderr, "失敗\n");
      }
      else {
	fprintf(stderr, "成功\n");
      }
    }
    else if ( (!strcmp(argv[i], "-is") ||
	       !strcmp(argv[i], "-irohaserver")) && i + 1 < argc ) {
      i++;
      jrKanjiControl(0, KC_SETSERVERNAME, argv[i]);
    }
    else if ( !strcmp(argv[i], "-yomi1") ) {
      yomiLevel = 1;
    }
    else if ( !strcmp(argv[i], "-yomi2") ) {
      yomiLevel = 2;
    }
    else if ( !strcmp(argv[i], "-customtest") ) {
      customtest = 1;
    }
    else if ( !strcmp(argv[i], "-storeyomi") ) {
      storeYomi = 1;
    }
    else if ( !strcmp(argv[i], "-storethen") ) {
      i++;
      storeThen = atoi(argv[i]);
    }
    else if ( !strcmp(argv[i], "-bufsize") ) {
      nbytes_given = 1;
      i++;
      given_nbytes = atoi(argv[i]);
    }
    else if ( !strcmp(argv[i], "-fn14") ) {
      rfont = RomanFont14;
      kfont = KanjiFont14;
    }
    else if ( !strcmp(argv[i], "-fn16") ) {
      rfont = RomanFont16;
      kfont = KanjiFont16;
    }
    else if ( !strcmp(argv[i], "-fn24") ) {
      rfont = RomanFont24;
      kfont = KanjiFont24;
    }
#ifdef KC_SETINITFILENAME
    else if ( !strcmp(argv[i], "-f") ) {
      i++;
      jrKanjiControl(0, KC_SETINITFILENAME, argv[i]);
    }
#endif /* KC_SETINITFILENAME */
    else if ( !strcmp(argv[i], "-time") ) {
      msystime++;
    }
    else if ( !strcmp(argv[i], "-rengodic") ) {
      rengo_data = 1;
      i++;
      sprintf(rengo_buf, "rengodic \"%s\"\n", argv[i]);
    }
    else if ( !strcmp(argv[i], "-parse") ) {
      parse_data = 1;
      i++;
      sprintf(parse_buf, "%s", argv[i]);
    }
    else if ( !strcmp(argv[i], "-context") ) {
      check_context = 1;
    }
    else if ( !strcmp(argv[i], "-nomode") ) {
      no_mode = 1;
    }
    else if ( !strcmp(argv[i], "-maxmode") ) {
      show_max_mode = 1;
    }
    else if ( !strcmp(argv[i], "-restrict") ) {
      i++;
      if ( !strcmp(argv[i], "none") ) {
	restrict = IROHA_NOTHING_RESTRICTED + 1;
      }
      else if ( !strcmp(argv[i], "alphanum") ) {
	restrict = IROHA_ONLY_ALPHANUM + 1;
      }
      else if ( !strcmp(argv[i], "hex") ) {
	restrict = IROHA_ONLY_HEX + 1;
      }
      else if ( !strcmp(argv[i], "num") ) {
	restrict = IROHA_ONLY_NUMERIC + 1;
      }
      else if ( !strcmp(argv[i], "all") ) {
	restrict = IROHA_NOTHING_ALLOWED + 1;
      }
    }
    else if ( !strcmp(argv[i], "-escclose") ) {
      escclose = 1;
      escControl = 1;
    }
#ifdef TEISEI
    else if ( !strcmp(argv[i], "-teisei") ) {
      teiseiline = 1;
    }
    else if ( !strcmp(argv[i], "-kana") ) {
      kanaline = 1;
    }
#endif /* TEISEI */
#ifdef KC_SETLISTCALLBACK
    else if ( !strcmp(argv[i], "-clist") ) {
      clist = 1;
    }
#endif /* KC_SETLISTCALLBACK */
    else {
      fprintf(stderr, "usage: %s [-option ...]\n", argv[0]);
      fprintf(stderr, "options:\n");
/* 次の２つは無効である。
      fprintf(stderr, "-[no]im   一太郎と同じカーソルの動きをする(しない)\n");
      fprintf(stderr, "-[no]roc  幅広く反転する(しない)\n");
*/
      fprintf(stderr, "-bell     未定義キーでベルを鳴らさない\n");
      fprintf(stderr, "-bufsize num buffer_returnのサイズを与える\n");
      fprintf(stderr, "-ckconn   サーバとの接続のチェック\n");
      fprintf(stderr, "-context  コンテクストを調べる\n");
      fprintf(stderr, "-customtest KC_PARSEのテスト\n");
      fprintf(stderr, "-debug    デバグモード\n");
      fprintf(stderr, "-dinit    KC_INITIALIZEを２回続けて行うテスト\n");
      fprintf(stderr, "-dfinal   KC_FINALIZEを２回続けて行うテスト\n");
      fprintf(stderr, "-esckakutei ESCキーで未確定文字列を確定する\n");
      fprintf(stderr, "-esckill  ESCキーで未確定文字列を抹消する\n");
      fprintf(stderr, "-escmode  ESCキーでモードを表示する\n");
      fprintf(stderr, "-f        イニシャライズファイルを指定する\n");
      fprintf(stderr, "-fin_init KC_FINALIZEの後KC_INITIALIZEを行うテスト\n");
      fprintf(stderr, "-fn14     14ドットフォントを使う\n");
      fprintf(stderr, "-fn16     16ドットフォントを使う\n");
      fprintf(stderr, "-fn24     24ドットフォントを使う\n");
      fprintf(stderr, "-ik       半角カタカナに字種変換しない\n");
      fprintf(stderr, "-imhex    １６進入力は４文字目で確定する\n");
      fprintf(stderr, "-kakutei  未定義キーで未確定文字列を確定する\n");
      fprintf(stderr, "-kill     未定義キーで未確定文字列を抹消する\n");
      fprintf(stderr, "-kugiri   文節の区切りに空白を入れる\n");
      fprintf(stderr, "-nc       数字キーでコマンドを実行する\n");
      fprintf(stderr, "-nmode    モード表示文字を数値を表す文字にする\n");
      fprintf(stderr, "-parse str カスタマイズ文字列を与える\n");
      fprintf(stderr, "-printenv 環境変数を表示する\n");
      fprintf(stderr, "-rengodic dic 連語辞書を指定する\n");
      fprintf(stderr, "-s        コネクトするサーバを指定する\n");
      fprintf(stderr, "-showkeys 予約されているキーを表示する\n");
      fprintf(stderr, "-storeyomi読みをストアする\n");
      fprintf(stderr, "-storethen num 読みをストアして次に何をするか指定\n");
      fprintf(stderr, "-through  未定義キーをそのまま通す\n");
      fprintf(stderr, "-width n  幅を与える\n");
      fprintf(stderr, "-yomi1    確定した時読みを表示する\n");
      fprintf(stderr, "-yomi2    確定した時読みと入力キー列を表示する\n");
      exit (1);
    }
  }
  
  initialize (argc,argv,envp);
  mainloop ();
  exit_program ();
}

/* initialize -- 初期化処理
 *
 *
 * 初期化処理ですること
 *
 *  ・サーバのオープン
 *  ・フォントのロード
 *  ・フォント情報を得る (XQueryFont, ascent, descent, width)
 *  ・テキストカーソルの幅を決める
 *  ・ウィンドウの生成
 *  ・グラフィックコンテクストの生成
 *  ・ウィンドウのマップ
 *
 */

initialize(argc,argv,envp)
int argc;
char *argv[],*envp[];
{
  XGCValues   gcv;

  /* サ−バとの接続をする */
  dpy = XOpenDisplay (0);
  
  if (dpy == NULL) {
    fprintf(stderr, "Ｘサーバに接続できませんでした\n");
    exit (1);
  }
  
  /* フォントをロ−ドする */
  if (rfont) {
    fn = XLoadFont (dpy, rfont); /* 英字カナ用フォント */
    fk = XLoadFont (dpy, kfont); /* 漢字フォント */
  }
  else {
    fn = XLoadFont (dpy, RomanFont24); /* 英字カナ用フォント */
    fk = XLoadFont (dpy, KanjiFont24); /* 漢字フォント */
  }
  
  /* フォントの情報(FontStruct)を得る */
  fonts[0] = XQueryFont (dpy, fn);
  fonts[1] = XQueryFont (dpy, fk);
  
  /* フォントの高さを記録しておく */
  asc = fonts[0]->ascent > fonts[1]->ascent 
    ? fonts[0]->ascent : fonts[1]->ascent;
  des = fonts[0]->descent > fonts[1]->descent
    ? fonts[0]->descent : fonts[1]->descent;
  
  /* フォントの横幅をとる。 */
  cw = fonts[0] ->max_bounds.width * 2 > fonts[1]->max_bounds.width
    ? fonts[0]->max_bounds.width * 2 : fonts[1]->max_bounds.width;
  
  /* テキストカーソルの幅を決める。 */
  CursorWidth = XTextWidth(fonts[0], "a", 1);
  
  /* ウィンドウを生成する */
  parwin = XCreateSimpleWindow
    (dpy, DefaultRootWindow(dpy), 0, 0,
     cw * WinWidth / 2 + InnerBorder * 2 + OuterBorder * 2 + 2/*border*/,
#ifndef TEISEI
     ((asc + des + InnerBorder ) * 2/*行*/ + InnerBorder 
#else /* TEISEI */
     ((asc + des + InnerBorder ) * (2 + teiseiline + kanaline)/*行*/
      + InnerBorder 
#endif /* TEISEI */
      + OuterBorder + 2/*border*/) * 2 + OuterBorder,
     1,
     BlackPixel(dpy, DefaultScreen(dpy)),
     WhitePixel(dpy, DefaultScreen(dpy)));

  XStoreName(dpy, parwin, "iroha sample");
  win = parwin;

  /* ＧＣを新しく生成する */
  gcv.foreground = BlackPixel(dpy, DefaultScreen(dpy));
  gcv.background = WhitePixel(dpy, DefaultScreen(dpy));
  
  gcs[0] = XCreateGC (dpy, win, GCForeground|GCBackground, &gcv);
  gcs[1] = XCreateGC (dpy, win, GCForeground|GCBackground, &gcv);
  
  /* それぞれのＧＣにフォントの設定をする */
  XSetFont (dpy, gcs[0], fn);
  XSetFont (dpy, gcs[1], fk);
  
  {
    XColor colordef;

    colordef.red   = 0xdf00;
    colordef.green = 0xef00;
    colordef.blue  = 0xff00;
    colordef.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(dpy, DefaultColormap(dpy, DefaultScreen(dpy)), &colordef);

    gcv.background = colordef.pixel;
    gcv.foreground = BlackPixel(dpy, DefaultScreen(dpy));
    mgcs[0] = XCreateGC (dpy, win, GCForeground|GCBackground, &gcv);
    mgcs[1] = XCreateGC (dpy, win, GCForeground|GCBackground, &gcv);
    
    XSetFont (dpy, mgcs[0], fn);
    XSetFont (dpy, mgcs[1], fk);
  }

  gcv.foreground = gcv.background;
  gcv.background = BlackPixel(dpy, DefaultScreen(dpy));

  rgcs[0] = XCreateGC (dpy, win, GCForeground|GCBackground, &gcv);
  rgcs[1] = XCreateGC (dpy, win, GCForeground|GCBackground, &gcv);
  
  XSetFont (dpy, rgcs[0], fn);
  XSetFont (dpy, rgcs[1], fk);

  win = XCreateSimpleWindow
    (dpy, parwin, OuterBorder, OuterBorder, 
     cw * WinWidth / 2 + InnerBorder * 2,
#ifndef TEISEI
     (asc + des + InnerBorder) * 2 /* 行 */ + InnerBorder,
#else /* TEISEI */
     (asc + des + InnerBorder) * (2 + teiseiline + kanaline) /* 行 */
     + InnerBorder,
#endif /* TEISEI */
     1, 
     BlackPixel(dpy, DefaultScreen(dpy)),
     WhitePixel(dpy, DefaultScreen(dpy)));
  XSelectInput (dpy, win, ExposureMask | ButtonPressMask | KeyPressMask);
  wb[0].win = win;
  
  win = XCreateSimpleWindow
    (dpy, parwin, OuterBorder,
#ifndef TEISEI
     (asc + des + InnerBorder) * 2 /* 行 */ 
#else /* TEISEI */
     (asc + des + InnerBorder) * (2 + teiseiline + kanaline) /* 行 */ 
#endif /* TEISEI */
     + InnerBorder + OuterBorder * 2 + 2,
     cw * WinWidth / 2 + InnerBorder * 2,
#ifndef TEISEI
     (asc + des + InnerBorder) * 2 /* 行 */ + InnerBorder,
#else /* TEISEI */
     (asc + des + InnerBorder) * (2 + teiseiline + kanaline) /* 行 */
     + InnerBorder,
#endif /* TEISEI */
     1, 
     BlackPixel(dpy, DefaultScreen(dpy)),
     WhitePixel(dpy, DefaultScreen(dpy)));
  XSelectInput (dpy, win, ExposureMask | ButtonPressMask | KeyPressMask);
  wb[1].win = win;

  /* ウィンドウをマップする */
  XMapSubwindows (dpy, parwin);
  XMapWindow (dpy, parwin);
  {
    char **warn;

    if (final_initial) {
      XKanjiControl(dpy, wb[0].win, KC_INITIALIZE, 0);
      XKanjiControl(dpy, wb[0].win, KC_FINALIZE, 0);
      XKanjiControl(dpy, wb[0].win, KC_INITIALIZE, 0);
      XKanjiControl(dpy, wb[0].win, KC_FINALIZE, 0);
      XKanjiControl(dpy, wb[0].win, KC_INITIALIZE, 0);
      XKanjiControl(dpy, wb[0].win, KC_FINALIZE, 0);
    }
    XKanjiControl(dpy, wb[0].win, KC_INITIALIZE, &warn);
    if (double_init) {
      int ret;

      ret = XKanjiControl(dpy, wb[0].win, KC_INITIALIZE, &warn);
      fprintf(stderr, "２回目の initialize の値は %d\n", ret);
    }
    if (warn) {
      char **p;
      
      for (p = warn; *p ; p++) {
	fprintf(stderr, "%s\n", *p);
      }
    }
    XKanjiControl(dpy, wb[0].win, KC_QUERYMODE, wb[0].mode.str);
    XKanjiControl(dpy, wb[1].win, KC_QUERYMODE, wb[1].mode.str);
    wb[0].mode.length = strlen(wb[0].mode.str);
    wb[0].mode.revPos = wb[0].mode.revLen = 0;
    wb[1].mode.length = strlen(wb[1].mode.str);
    wb[1].mode.revPos = wb[1].mode.revLen = 0;
  }
  if (ckconn) {
    if (XKanjiControl(dpy, wb[0].win, KC_QUERYCONNECTION, 0)) {
      fprintf(stderr, "サーバとつながっているよ\n");
    }
    else {
      fprintf(stderr, "サーバとつながっていないよ\n");
    }
  }
  max_mode_strlen = XKanjiControl(dpy, wb[0].win, KC_QUERYMAXMODESTR, 0) + 2;
  if (show_max_mode) {
    fprintf(stderr, "モード文字列の最大コラム幅は %d です。\n",
	    max_mode_strlen - 2);
  }
  if (width_given) {
    fprintf(stderr, "候補一覧表示のコラム幅は %d です。\n", given_width);
    XKanjiControl(dpy, wb[0].win, KC_SETWIDTH, (caddr_t)given_width);
    XKanjiControl(dpy, wb[1].win, KC_SETWIDTH, (caddr_t)given_width);
  }
  else {
    XKanjiControl(dpy, wb[0].win, KC_SETWIDTH,
		  (caddr_t)(78 - max_mode_strlen));
    XKanjiControl(dpy, wb[1].win, KC_SETWIDTH,
		  (caddr_t)(78 - max_mode_strlen));
  }
#ifdef KC_SETLISTCALLBACK
  if (clist) {
    jrListCallbackStruct lcs;
    void listcallback();

    lcs.client_data = 0;
    lcs.callback_func = listcallback;
    XKanjiControl(dpy, wb[0].win, KC_SETLISTCALLBACK, (char *)&lcs);
    XKanjiControl(dpy, wb[1].win, KC_SETLISTCALLBACK, (char *)&lcs);
  }
#endif
  if (kugiri) {
    XKanjiControl(dpy, wb[0].win, KC_SETBUNSETSUKUGIRI, 1);
  }
  if (rengo_data) {
    int i, n;
    char *p = rengo_buf, **pp;

    n = XKanjiControl(dpy, wb[0].win, KC_PARSE, &p);
    pp = (char **)p;
    for (i = 0 ; i < n ; i++) {
      fprintf(stderr, "%s\n", *pp++);
    }
  }
  if (parse_data) {
    int i, n;
    char *p = parse_buf, **pp;

    n = XKanjiControl(dpy, wb[0].win, KC_PARSE, &p);
    pp = (char **)p;
    for (i = 0 ; i < n ; i++) {
      fprintf(stderr, "%s\n", *pp++);
    }
  }
  XKanjiControl(dpy, wb[0].win, KC_SETMODEINFOSTYLE, modeString);
  XKanjiControl(dpy, wb[0].win, KC_SETHEXINPUTSTYLE, hexBy);
  XKanjiControl(dpy, wb[0].win, KC_SETUNDEFKEYFUNCTION, undeffn);
  XKanjiControl(dpy, wb[0].win, KC_INHIBITHANKAKUKANA, jrik);
  XKanjiControl(dpy, wb[0].win, KC_YOMIINFO, yomiLevel);
  if (customtest) {
    char *custom = "henkan C-l\nyomiganai {\n  kigouMode ^l\n}\n";

    printf("C-l に変換、読みが無い状態では記号モード、を割り当てます。\n");
    XKanjiControl(dpy, wb[0].win, KC_PARSE, &custom);
  }
  if (showkeys) {
    unsigned char keys[20];
    int n, i;

    n = XKanjiControl(dpy, wb[0].win, KC_MODEKEYS, keys);
    fprintf(stderr, "アルファベットモードで %d 個のキーを使っていて\n", n);
    fprintf(stderr, "それらは、");
    for (i = 0 ; i < n ; i++) {
      fprintf(stderr, "0x%02x、", keys[i]);
    }
    fprintf(stderr, "です。\n");
  }
  if (printenv) {
    char **p;
    for (p = envp ; *p ; p++) {
      fprintf(stderr, "envは、(%s)\n", *p);
    }
  }
  if (check_context) {
    fprintf(stderr, "context: %d, bushuContext: %d.\n",
	    XKanjiControl(dpy, wb[0].win, KC_GETCONTEXT, 0),
	    XKanjiControl(dpy, wb[0].win, KC_GETCONTEXT, 1));
  }
  if (no_mode) {
    fprintf(stderr, "今から下は制限せずに上はモード変更を制限します....\n");
    if (XKanjiControl(dpy, wb[1].win, KC_INHIBITCHANGEMODE, 1) < 0) {
      fprintf(stderr, "...モードの制限をするのに失敗しました。\n");
    }
    else {
      fprintf(stderr, "...モードの変更を制限しました。\n");
    }
  }
  if (restrict) {
    if (XKanjiControl(dpy, wb[1].win,
		      KC_LETTERRESTRICTION, restrict - 1) < 0) {
      fprintf(stderr, "文字種の制限に失敗しました。\n");
    }
  }
}

/* exit_program -- 終了処理
 *
 * このプログラムを終了する時は必ずこの関数を呼ぼう
 */

exit_program()
{
  int res;
  char **warn;

  if (XKanjiControl ( dpy, wb[0].win, KC_FINALIZE, &warn ) < 0) {
    extern char * jrKanjiError;

    fprintf(stderr, "KC_FILENALIZEでエラーが発生したよ\n");
    fprintf(stderr, "error: %s\n", jrKanjiError);
  }
  if (double_final) {
    int res;
    res = XKanjiControl(dpy, wb[0].win, KC_FINALIZE, &warn);
    fprintf(stderr, "kc_finalize = %d\n", res);
    if (res < 0) {
      extern char *jrKanjiError;
      fprintf(stderr, "%s\n", jrKanjiError);
    }
  }
  if (warn) {
    char **p;

    for (p = warn; *p ; p++) {
      fprintf(stderr, "%s\n", *p);
    }
  }
  XCloseDisplay (dpy);
  exit (0);
}

/* グローバル変数の定義 */

int              i, nbytes;	       /* ワークか？ */
/* 候補一覧の再描画するためのカーソル位置などの情報を保存しておく変数 */
int              charwi;	/* 文字の幅 */
XEvent           event;		/* イベント */


/* mainloop -- メインループ
 *
 * メインルーチン
 *
 * イベントを拾ってディスパッチする。
 */

mainloop()
{
  do {
    XNextEvent (dpy, &event);
    erase_textcursor();
    switch (event.type)
      {
      case Expose:
	proc_expose();
	break;
      case KeyPress:
	proc_keypress(&event);
	break;
      case ButtonPress:
	proc_buttonpress(&event);
	break;
      default: /* なんだかよくわからないイベントが来た場合 */
	break;
      }
    if (wb[ww].cursor == DRAW) {
      draw_textcursor();
    }
  } while (1); /* (event.type != ButtonPress); */
}

/* テキストカーソルの描画消去のルーチン。
 *
 * カーソルをどこに描くかという情報を変数 cursorcol から得る。
 */

draw_textcursor()
{
  /* テキストカーソルを描画する。 */
  
  XFillRectangle(dpy, win, gcs[0],
		 InnerBorder + wb[ww].kakutei.width + wb[ww].mikakutei.width,
		 InnerBorder, CursorWidth, asc + des);
}

erase_textcursor()
{
  XClearArea(dpy, win, 
	     InnerBorder + wb[ww].kakutei.width + wb[ww].mikakutei.width,
	     InnerBorder, CursorWidth, asc + des, False);
}

/* proc_expose -- エキスポーズ処理
 *
 * col, rcol, rlen などの情報を使って文字列を描画する。
 */

proc_expose()
{
  int pos;

  redraw_kakutei();
  redraw_mikakutei();
  draw_both_mode();
  draw_gline();
#ifdef TEISEI
  if (teiseiline) {
    draw_teisei();
  }
  if (kanaline) {
    draw_kana();
  }
#endif /* TEISEI */
  XFlush(dpy);
}

/* proc_keypress -- キー入力の時の処理
 *
 */

proc_keypress(event)
XEvent *event;
{
  char             lookupbuf[STRBUFSIZE];
  KeySym           keysym;
  XComposeStatus   compose_status;
  XKanjiStatus     kanji_status;
  extern char *jrKanjiError;
  int notControl = 1;
  int defineKanji = 0;

  nbytes = XLookupString (event, lookupbuf, STRBUFSIZE,
			  &keysym, &compose_status);

  if (escControl && nbytes > 0 && lookupbuf[0] == 0x1b) {
    XKanjiStatusWithValue ksv;
    ksv.buffer = (unsigned char *)lookupbuf;
    ksv.bytes_buffer = STRBUFSIZE;
    ksv.ks = &kanji_status;
    notControl = 0; defineKanji = 0;
    if (esckill) {
      XKanjiControl(dpy, event->xany.window, KC_KILL, &ksv);
      nbytes = ksv.val;
    }
    else if (esckakutei) {
      XKanjiControl(dpy, event->xany.window, KC_KAKUTEI, &ksv);
      nbytes = ksv.val;
    }
    else if (escclose) {
      XKanjiControl(dpy, event->xany.window, KC_CLOSEUICONTEXT, &ksv);
      nbytes = ksv.val;
    }
  }
  else if (showMode && nbytes > 0 && lookupbuf[0] == 0x1b) {
    char foo[100];
    
    XKanjiControl(dpy, event->xany.window, KC_QUERYMODE, foo);
    fprintf(stderr, "%s\n", foo);
    return;
  }
  else if (storeYomi && nbytes > 0 && lookupbuf[0] == 0x1b) {
    XKanjiStatusWithValue ksv;
    
    ksv.buffer = (unsigned char *)lookupbuf;
    ksv.bytes_buffer = STRBUFSIZE;
    ksv.ks = &kanji_status;
    kanji_status.echoStr = 
      (unsigned char *)"あぷりけーしょんからすとあされたよみです。";
    kanji_status.mode = 
      (unsigned char *)"apurike-shonkarasutoasaretayomidesu.";
    notControl = 0; defineKanji = 0;

    XKanjiControl(dpy, event->xany.window, KC_STOREYOMI, &ksv);
#ifdef KC_DO
    if (storeThen) {
      ksv.buffer[0] = '@';
      ksv.val = storeThen;
      XKanjiControl(dpy, event->xany.window, KC_DO, &ksv);
    }
#endif /* KC_DO */
		  
    nbytes = ksv.val;
  }

  if (numControl && nbytes > 0) {
    XKanjiStatusWithValue ksv;
    ksv.buffer = (unsigned char *)lookupbuf;
    ksv.bytes_buffer = STRBUFSIZE;
    ksv.ks = &kanji_status;
    notControl = 0; defineKanji = 0;
    switch (lookupbuf[0])
      {
      case '0':
	ksv.val = IROHA_MODE_AlphaMode;
	break;
      case '1':
	ksv.val = IROHA_MODE_HenkanMode;
	break;
      case '2':
	ksv.val = IROHA_MODE_HexMode;
	break;
      case '3':
	ksv.val = IROHA_MODE_BushuMode;
	break;
      case '4':
	ksv.val = IROHA_MODE_KigoMode;
	break;
      case '5':
	ksv.val = IROHA_MODE_ZenHiraKakuteiMode;
	break;
      case '6':
	ksv.val = IROHA_MODE_ZenKataKakuteiMode;
	break;
      case '7':
	ksv.val = IROHA_MODE_HanKataKakuteiMode;
	break;
      case '8':
	ksv.val = IROHA_MODE_ZenAlphaKakuteiMode;
	break;
      case '9':
	ksv.val = IROHA_MODE_HanAlphaKakuteiMode;
	break;
      case '-':
	ksv.ks->echoStr = (unsigned char *)"登録魑魅魍魎";
	ksv.ks->length = strlen("登録魑魅魍魎");
	notControl = 1;
	defineKanji = 1;
	break;
      case '=':
	ksv.ks->echoStr = (unsigned char *)"";
	ksv.ks->length = 0;
	notControl = 1;
	defineKanji = 1;
	break;
      default:
	notControl = 1;
      }
    if (!notControl) {
      XKanjiControl(dpy, event->xany.window, KC_CHANGEMODE, &ksv);
      nbytes = ksv.val;
    }
    else if (defineKanji) {
      XKanjiControl(dpy, event->xany.window, KC_DEFINEKANJI, &ksv);
      nbytes = ksv.val;
    }
  }
  if (notControl && !defineKanji) {
    int i;
    struct tms tmbuf;

    for (i = 0 ; i < STRBUFSIZE ; i++) 
      lookupbuf[i] = '\0';

    if (msystime) {
      kstime -= times(&tmbuf);
    }
    nbytes = XLookupKanjiString (event, lookupbuf, 
				 nbytes_given ? given_nbytes : STRBUFSIZE,
				 &keysym, &compose_status,
				 &kanji_status);
    if (msystime) {
      kstime += times(&tmbuf);
    }
  }
  
  /* 読みを表示してみる。 */
  if (nbytes) {
    if (yomiLevel && (kanji_status.info & KanjiYomiInfo)) {
      if (nbytes < STRBUFSIZE) {
	printf("読み \"%s\"", lookupbuf + nbytes + 1);
	if (yomiLevel > 1) {
	  if (nbytes + 1 + strlen(lookupbuf + nbytes + 1) < STRBUFSIZE) {
	    printf("、入力 \"%s\"", lookupbuf + nbytes + 1
		   + strlen(lookupbuf + nbytes + 1) + 1);
	  }
	}
	printf(" に対応する部分が確定しました。\n");
      }
    }
  }

  {
    int i;
    
    win = event->xkey.window;
    for (i = 0 ; i < 2 ; i++) {
      if (wb[i].win == win) {
	ww = i;
      }
    }
    if (kanji_status.length >= 0) {
      wb[ww].cursor = (kanji_status.length != 0 && kanji_status.revLen) ? 
	NODRAW : DRAW;
    }
  }

  if (nbytes < 0) {
    fprintf(stderr, "sample: エラー: %s\n", jrKanjiError);
    return;
  }

#ifdef DEBUG
  if (iroha_debug) {
    print_debug(nbytes, event, lookupbuf,
		keysym, &compose_status, &kanji_status);
  }
#endif

  if (nbytes > 0) { /* XLookupKanjiString の結果が０より大きいと言うことは
		       確定文字列が返っていると言うことなので、その確定文
		       字列を描画する。 */
    draw_determined(nbytes, lookupbuf, &kanji_status);
    if (kanji_status.length >= 0) {
      /* 未確定情報を取り敢えずとっておく */
      wb[ww].mikakutei.length = kanji_status.length;
      strncpy(wb[ww].mikakutei.str, kanji_status.echoStr, 
	      wb[ww].mikakutei.length);
      wb[ww].mikakutei.revPos = kanji_status.revPos;
      wb[ww].mikakutei.revLen = kanji_status.revLen;
      wb[ww].mikakutei.prevWidth = wb[ww].mikakutei.width;
      wb[ww].mikakutei.width = 
	XKanjiTextWidth(fonts, wb[ww].mikakutei.str, wb[ww].mikakutei.length,
			fs_euc_mode);
    }

    if (kanji_status.length > 0) {
      redraw_mikakutei();
    } 
  }
  else if (kanji_status.length >= 0) { /* kanji_status.length が０より大き 
					  いと言うことは未確定文字列が返さ 
					  れていると言うことなので、それを
					  描画する。 */
    draw_undetermined(nbytes, lookupbuf, &kanji_status);
  }

  if (kanji_status.info & KanjiModeInfo) {
    /* 上記の判定文が真の時はカナ漢字変換モードに関する情報が返されて
       いると言うことなので、それを描画する。 */
    wb[ww].mode.prevWidth = wb[ww].mode.width;
    strcpy(wb[ww].mode.str, kanji_status.mode);
    wb[ww].mode.length = strlen(wb[ww].mode.str);
    wb[ww].mode.width = XKanjiTextWidth(fonts, wb[ww].mode.str, 
					wb[ww].mode.length,
					fs_euc_mode);
    XClearArea(dpy, win, InnerBorder, 
	       2 * InnerBorder + asc + des,
	       glinePosition,
	       asc + des, False);
    draw_mode();
  }

  if (kanji_status.info & KanjiGLineInfo) {
    /* 上記の判定文が真の時は候補一覧に関する情報が返されていると言う
       ことなので、それを描画する。 */
    wb[ww].gline.prevWidth = wb[ww].gline.width;
    wb[ww].gline.length = kanji_status.gline.length;
    strncpy(wb[ww].gline.str, 
	    kanji_status.gline.line, kanji_status.gline.length);
    wb[ww].gline.revPos = kanji_status.gline.revPos;
    wb[ww].gline.revLen = kanji_status.gline.revLen;
    wb[ww].gline.width = XKanjiTextWidth(fonts, wb[ww].gline.str, 
					 wb[ww].gline.length,
				  fs_euc_mode);
    XClearArea(dpy, win, InnerBorder + glinePosition,
	       2 * InnerBorder + asc + des,
	       wb[ww].gline.prevWidth,
	       asc + des, False);
    draw_gline();
  }

#ifndef KanjiTeiseiInfo
#define KanjiTeiseiWasUndefined
#define KanjiTeiseiInfo 128
#endif /* KanjiTeiseiInfo */

#ifdef TEISEI
  if ((kanji_status.info & KanjiTeiseiInfo) && teiseiline) {
    wb[ww].teisei.prevWidth = wb[ww].teisei.width;
    wb[ww].teisei.length = kanji_status.teisei.length;
    strncpy(wb[ww].teisei.str, 
	    kanji_status.teisei.teiseiStr, kanji_status.teisei.length);
    wb[ww].teisei.revPos = kanji_status.teisei.revPos;
    wb[ww].teisei.revLen = kanji_status.teisei.revLen;
    wb[ww].teisei.width = XKanjiTextWidth(fonts, wb[ww].teisei.str, 
					 wb[ww].teisei.length,
				  fs_euc_mode);
    XClearArea(dpy, win, InnerBorder,
	       InnerBorder + 2 * (InnerBorder + asc + des),
	       wb[ww].teisei.prevWidth,
	       asc + des, False);
    draw_teisei();
  }
#endif /* TEISEI */
}

#ifdef DEBUG
print_debug(n, ev, lub, key, cs, ks)
     int n;
     XEvent *ev;
     char *lub;
     KeySym key;
     XComposeStatus *cs;
     XKanjiStatus *ks;
{
  char buf[1024];

  printf("\n(sample)\n");
  printf("len=%d, ", n);
  printf("keysym=0x%x, ", key);
  printf("&kanjiStat=0x%x, ", ks);
  printf("echo_len=%d, ", ks->length);
  printf("revPos=%d, ", ks->revPos);
  printf("revLen=%d, ", ks->revLen);
  if (ks->length > 0) {
    strncpy(buf, ks->echoStr, ks->length);
    buf[ks->length] = '\0';
    printf("ks.echoStr=("); printtext(buf); printf(")\n");
  }
  printf("info=0x%x, ", ks->info);
  printf("glinelen=%d, ", ks->gline.length);
  if ((ks->info & KanjiGLineInfo) && ks->gline.length > 0) {
    strncpy(buf, ks->gline.line, ks->gline.length);
    buf[ks->gline.length] = '\0';
    printf("\nks.gline.line=("); printtext(buf); printf(")\n");
    printf("gline.revPos = %d gline.revLen = %d\n",
	   ks->gline.revPos, ks->gline.revLen);
  }
  if (ks->info & KanjiModeInfo) {
    printf("mode(%s)\n", ks->mode);
  }
  printf("buffer[0]=0x%x, ", (unsigned char)lub[0]);
  lub[nbytes] = '\0';
  printf("buffer=\""); printtext(lub); printf("\"\n");
}

static printtext(s)
char *s;
{
  unsigned char *p;

  for (p = (unsigned char *)s ; *p ; p++) {
    printchar((int)*p);
  }
}

static printchar(c)
int c;
{
  if (c < ' ') {
    printf("^%c", c + '@');
  }
  else if (c == 0x007f) {
    printf("^?");
  }
  else {
    printf("%c", c);
  }
}

#endif



/* draw_determined -- 確定文字列の描画
 *
 * 確定文字列の描画では以下のことを行う
 *
 * ・コントロール文字だったらその処理ルーチンへ行く。
 * ・新しく確定した文字列を内部バッファにアペンドする。
 * ・新しく確定した部分を描画する。
 * ・新しく確定した文字列の幅を調べて、新しいカーソル位置を計算する。
 * ・buf のインデックスである col を更新する。
 * ・新しく確定した文字列の幅が、以前に未確定文字列として描画されて
 *   いた文字列の幅よりも小さいのであれば、XDrawImageKanjiString だ
 *   けでは消し切れないので、XClearArea を用いてはみだした部分を消去
 *   する。
 */

draw_determined(nbytes, lookupbuf, kanji_status)
     int nbytes;
     char *lookupbuf;
     XKanjiStatus *kanji_status;
{
  int str_width;

  if (nbytes == 1 && lookupbuf[0] < 0x20) {
    wb[ww].kakutei.prevWidth = wb[ww].kakutei.width;
    if (wb[ww].mikakutei.width > 0) {
      XClearArea(dpy, win, InnerBorder + wb[ww].kakutei.width, InnerBorder,
		 wb[ww].mikakutei.width, asc + des, False);
    }
    /* 未確定文字列の部分を消去したので、 mikakutei.width をクリアして
       おく */
    wb[ww].mikakutei.width = 0;

    /* コントロールコードの入力（あまり良いコーディングではない） */
    proc_ctl_keypress(lookupbuf);
  }
  else { /* コントロールコードでない場合 */
    /* length というのは str に格納されている文字列の末尾を抑えるためのイ
       ンデックスである。新しく確定した文字列は length でインデックスされ
       る部分よりも後ろに格納する。 */
    strncpy(wb[ww].kakutei.str + wb[ww].kakutei.length, lookupbuf, nbytes); 
    /* とりあえず、最後に NULL を入れておくが、不要かもしれない */
    wb[ww].kakutei.str[wb[ww].kakutei.length + nbytes] = '\0';

    /* 新しく確定した部分のみを描画する。*/
    XDrawImageKanjiString(dpy, win, gcs, fonts, 
			  InnerBorder + wb[ww].kakutei.width,
			  InnerBorder + asc,
			  wb[ww].kakutei.str + wb[ww].kakutei.length, nbytes,
			  fs_euc_mode);
    
    /* 新しく確定した文字列の幅を調べて、新しいカーソル位置を計算する */
    str_width = XKanjiTextWidth(fonts, 
				wb[ww].kakutei.str + wb[ww].kakutei.length, 
				nbytes,
				fs_euc_mode);
    wb[ww].kakutei.prevWidth = wb[ww].kakutei.width;
    wb[ww].kakutei.width += str_width;

    /* str のインデックスである length を更新する。 */
    wb[ww].kakutei.length += nbytes;
    
    /* 新しく確定した文字列の幅が、以前に未確定文字列として描画されて
       いた文字列の幅よりも小さいのであれば、XDrawImageKanjiString だ
       けでは消し切れないので、XClearArea を用いてはみだした部分を消去
       しなくてはならない。 */
    if (str_width < wb[ww].mikakutei.width) {
      XClearArea(dpy, win, InnerBorder + wb[ww].kakutei.width, InnerBorder,
		 wb[ww].mikakutei.width - str_width, asc + des, False);
    }
    /* 未確定文字列の部分を消去したので、 mikakutei.width をクリアして
       おく */
    wb[ww].mikakutei.width = 0;
  }
}

/* draw_undetermined -- 未確定文字列の描画
 *
 */

draw_undetermined(nbytes, lookupbuf, kanji_status)
     int nbytes;
     char *lookupbuf;
     XKanjiStatus *kanji_status;
{
  int rev_width;
  int pos;
  
  pos = wb[ww].kakutei.width;
  
  if (kanji_status->length < 0) /* kanji_status->length がマイナスの時
				   は前の未確定文字列と変化がないと言
				   うことなので何もしない。*/
    return;
  /* 未確定文字列が存在するのであれば、その処理をする。とりあえず、
    まずはバッファにコピーしておく */
  wb[ww].mikakutei.length = kanji_status->length;
  strncpy(wb[ww].mikakutei.str, kanji_status->echoStr, 
	  wb[ww].mikakutei.length);
  wb[ww].mikakutei.revPos = kanji_status->revPos;
  wb[ww].mikakutei.revLen = kanji_status->revLen;
  wb[ww].mikakutei.prevWidth = wb[ww].mikakutei.width;
  wb[ww].mikakutei.width = XKanjiTextWidth(fonts, 
					   wb[ww].mikakutei.str, 
					   wb[ww].mikakutei.length, 
					   fs_euc_mode);
  redraw_mikakutei();
  
  /* 前に描画してあった文字列の長さが、新しく描画した文字列よりも長かっ
     た場合には、前に描画している部分で、はみ出している部分をクリアす
     る。 */
  if (wb[ww].mikakutei.prevWidth > wb[ww].mikakutei.width) {
    XClearArea(dpy, win, 
	       InnerBorder + wb[ww].kakutei.width + wb[ww].mikakutei.width, 
	       InnerBorder, 
	       wb[ww].mikakutei.prevWidth - wb[ww].mikakutei.width,
	       asc + des, False);
  }
}

/* draw_gline -- 候補一覧表示の部分を描画する。
 *
 */


draw_gline()
{
  static int width, firsttime = 1;

  if (firsttime) {
    char *str;

    if (str = malloc(max_mode_strlen + 1)) {
      for (i = 0 ; i < max_mode_strlen ; i++) {
	str[i] = 'x';
      }
    }
    firsttime = 0;
    glinePosition = XKanjiTextWidth(fonts, str, max_mode_strlen, fs_euc_mode);
    if (str) {
      free(str);
    }
  }
  
  redraw_general(dpy, win, gcs, rgcs, fonts,
		 InnerBorder + glinePosition,
		 2 * InnerBorder + 2 * asc + des,
		 &wb[ww].gline);
}

#ifdef TEISEI

/* draw_teisei -- 訂正文字列を書く
 *
 */

draw_teisei()
{
  redraw_general(dpy, win, gcs, rgcs, fonts,
		 InnerBorder,
		 3 * InnerBorder + 3 * asc + 2 * des,
		 &wb[ww].teisei);
}

draw_kana()
{
  redraw_general(dpy, win, gcs, rgcs, fonts,
		 InnerBorder,
		 3 * InnerBorder + 3 * asc + 2 * des,
		 &wb[ww].kana);
}

#endif /* TEISEI */

/* proc_ctl_keypress -- コントロールキーの処理ルーチン
 *
 */

proc_ctl_keypress(lookupbuf)
     char *lookupbuf;
{
  switch (lookupbuf[0])
    {
    case ctl('M'):
      /* リターンキー */
#ifdef ECHO
      wb[ww].kakutei.str[wb[ww].kakutei.length] = '\0';
      printf("%s\n", wb[ww].kakutei.str);
#endif
      if (msystime) {
	printf("ここまでの XLookupKanjiString の時間 %d tics\n", kstime);
      }
      /* コラムおよびカーソルを戻し、ウィンドウをクリアする。 */
      wb[ww].kakutei.length = 0;
      wb[ww].kakutei.width = 0;
      XClearWindow(dpy, win);
      draw_mode();
      XFlush(dpy);
      break;
    case ctl('H'):
      /* バックスペースキー */
      if (wb[ww].kakutei.length) {
	if (wb[ww].kakutei.str[wb[ww].kakutei.length - 1] & 0x80) {
	  wb[ww].kakutei.length -= 2;
	  charwi = XKanjiTextWidth(fonts, 
				   wb[ww].kakutei.str + wb[ww].kakutei.length,
				   2, fs_euc_mode);
	}
	else {
	  wb[ww].kakutei.length--;
	  charwi = XKanjiTextWidth(fonts, 
				   wb[ww].kakutei.str + wb[ww].kakutei.length,
				   1, fs_euc_mode);
	}
	wb[ww].kakutei.width -= charwi;
	XClearArea(dpy, win, InnerBorder + wb[ww].kakutei.width, 
		   InnerBorder, charwi,
		   asc + des, False);
	XFlush(dpy);
      }
      break;
    case ctl('D'):
      /* EOF が打たれた場合 */
    case 0x7f:
      /* DEL が打たれた場合 */
      exit_program();
      break;
    case ctl('C'):
      /* かな漢字変換を再起動する */
      reset_iroha();
      /* モードを消去する */
      strcpy(wb[0].mode.str, "");
      strcpy(wb[1].mode.str, "");
      /* コラムおよびカーソルを戻し、ウィンドウをクリアする。 */
      wb[0].kakutei.length = 0;
      wb[0].kakutei.width = 0;
      wb[1].kakutei.length = 0;
      wb[1].kakutei.width = 0;
      XClearWindow(dpy, win);
      draw_both_mode();
      XFlush(dpy);
      kstime = 0; /* システム計測時間もリセット */
      break;
    default:
      /* その他 */
/*      if (prev_rev_width) {
	XClearArea(dpy, win, 
		   curpos, InnerBorder,
		   prev_rev_width, asc + des,
		   False);
      }
 これってなんだっけ？*/
      break;
    }
}

reset_iroha()
{
  char **warn;

/*
  printf("いろはを落とします...");
  fflush(stdout);
*/
  jrKanjiControl(0, KC_FINALIZE, &warn);
  if (warn) {
    char **p;

    for (p = warn ; *p ; p++) {
      printf("%s\n", *p);
    }
  }
/*
  printf("いろはを再起動します...");
  fflush(stdout);
*/
  jrKanjiControl(0, KC_INITIALIZE, &warn);
  if (warn) {
    char **p;

    for (p = warn ; *p ; p++) {
      printf("%s\n", *p);
    }
  }
/*
  printf("再起動しました\n");
*/
}

/* draw_mode -- モード表示の部分を描画する。
 *
 */

draw_mode()
{
  int pos;

  /* モード表示の文字を描画する */
  pos = InnerBorder;
  XDrawImageKanjiString(dpy, wb[ww].win, gcs, fonts, pos, 
			2 * InnerBorder + 2 * asc + des,
			wb[ww].mode.str, strlen(wb[ww].mode.str), 
			fs_euc_mode);
}

draw_both_mode()
{
  int wwbk = ww;

  for (ww = 0 ; ww < 2 ; ww++) {
    draw_mode();
  }
  ww = wwbk;
}

redraw_kakutei()
{
  XDrawImageKanjiString(dpy, win, gcs, fonts, InnerBorder, InnerBorder + asc,
			wb[ww].kakutei.str, wb[ww].kakutei.length,
			fs_euc_mode);
}

redraw_mikakutei()
{
  redraw_general(dpy, win, mgcs, rgcs, fonts,
		 InnerBorder + wb[ww].kakutei.width, InnerBorder + asc,
		 &wb[ww].mikakutei);
}


/* redraw_general -- 一般的な再描画ルーチン

  strRec 構造体へのポインタを渡してやると、通常描画、反転描画、通常描画を
  してくれる。

  */

redraw_general(dpy, win, gcs, rgcs, fonts, x, y, s)
Display *dpy;
Window win;
GC *gcs, *rgcs;
XFontStruct **fonts;
int x, y;
strRec *s;
{
  int pos;

  if (s->length == 0) {
    return;
  }
  pos = x;
  XDrawImageKanjiString(dpy, win, gcs, fonts, pos, y,
		    s->str, s->revPos, fs_euc_mode);

  /* 反転表示の文字を描画する */
  pos += XKanjiTextWidth(fonts, s->str, s->revPos,
			 fs_euc_mode);
  XDrawImageKanjiString(dpy, win, rgcs, fonts, pos, y,
			s->str + s->revPos,
			s->revLen, fs_euc_mode);
  
  /* 後ろの通常表示の文字を描画する */
  pos += XKanjiTextWidth(fonts, s->str + s->revPos, 
			 s->revLen, fs_euc_mode);
  XDrawImageKanjiString(dpy, win, gcs, fonts, pos, y,
			s->str + s->revPos + s->revLen,
			s->length - s->revPos - s->revLen,
			fs_euc_mode);
}

proc_buttonpress(event)
XButtonEvent *event;
{
  switch (event->button) {
#ifdef DEBUG
  case 3:
    iroha_debug = iroha_debug ? 0 : 1;
    if (iroha_debug) {
      fprintf(stderr, "《《《デバグメッセージ》》》\n");
    }
    else {
      fprintf(stderr, "《《《 メッセージ終了 》》》\n");
    }
    break;
#endif /* DEBUG */
  default:
#ifdef DEBUG_ALLOC
    {
      extern fail_malloc;
      fail_malloc = !fail_malloc;
      fprintf(stderr, "☆%s malloc()\n",
	      fail_malloc ? "エラーする" : "普通の");
    }
#endif /* DEBUG_ALLOC */
#ifdef SHOW_ROME_STRUCT
    showRomeStruct(event->display, event->window);
#endif
    break;
  }
}

beep()
{
  XBell(dpy, 100);
}

void
listcallback(client_data, func, items, nitems, cur_item)
char *client_data;
int  func;
unsigned char **items;
int nitems, *cur_item;
{
  int i;
  unsigned char **p;

  switch (func) {
  case CANNA_LIST_Start:
    printf("一覧開始\n");
    for (i = 0, p = items ; i < nitems ; i++, p++) {
      if (i == *cur_item) {
	printf("%d: (%s)\n", i, *p);
      }
      else {
	printf("%d: %s\n", i, *p);
      }
    }
    break;
  case CANNA_LIST_Select:
    printf("一覧選択\n");
    break;
  case CANNA_LIST_Quit:
    printf("一覧中止\n");
    break;
  case CANNA_LIST_Forward:
    printf("右\n");
    break;
  case CANNA_LIST_Backward:
    printf("左\n");
    break;
  case CANNA_LIST_Next:
    printf("下\n");
    break;
  case CANNA_LIST_Prev:
    printf("上\n");
    break;
  case CANNA_LIST_BeginningOfLine:
    printf("左端\n");
    break;
  case CANNA_LIST_EndOfLine:
    printf("右端\n");
    break;
  default:
    printf("なにかおかしいぞ\n");
    break;
  }
}
