/***********************************************************
 *							   *	
 *            COPYRIGHT (C) 1990 NEC CORPORATION	   * 
 *      	     ALL RIGHTS RESERVED		   *
 *						           *	
 ***********************************************************/

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk/ui/RCS/oldxlook.c,v 2.5 91/02/14 11:12:45 satoko Exp $";

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include "sglobal.h"
#include <iroha/kanji.h>

#define KANJIBUFSIZE 	1024
#define NULL            0

/*
 * XLookupKanjiString は X ウィンドウのキー入力イベントを event_struct
 * により受け取り、そのキーにしたがって必要ならカナ漢字変換を行い、そ
 * のキー入力の結果として得られる文字列を buffer_return で返す。
 * buffer_return はアプリケーション側に用意するバッファであり、アプリ
 * ケーションはそのバッファの長さを bytes_buffer で渡す。 
 *
 * keysym_return はキーイベントで入力されたキーの論理名を返すための変
 * 数へのポインタであり、その変数はやはりアプリケーション側で領域が確
 * 保されていなければならない。
 *
 * status_return は今のところ用いられない。
 *
 * kanji_status_return は確定していない入力文字列を表示するためのデー
 * タであり、未確定の読みや候補漢字などが返される。kanji_status_return
 * のメンバには、 echoStr, length, revPos, revLen がありそれぞれ、未確
 * 定文字列へのポインタ、その長さ、未確定文字列のうち、強調する部分へ
 * のオフセット、強調する部分の長さを返す。未確定文字列を格納する領域
 * は XLookupKanjiString で自動的に用意される。
 */


static unsigned char *mode_string = (unsigned char *)"\
modeString(alpha)		\"(半角英数)\"\n\
modeString(yomiganai) 		\"(全角かな)\"\n\
modeString(hex)			\"(16進)\"\n\
modeString(bushu)		\"(部首)\"\n\
modeString(henkanNyuryoku)	\"(全角かな)\"\n\
modeString(zenHiraKakutei)	\"(確定かな)\"\n\
modeString(zenKataKakutei)	\"(全角カナ)\"\n\
modeString(hanKataKakutei)	\"(半角カナ)\"\n\
modeString(zenAlphaKakutei)	\"(全角英数)\"\n\
modeString(hanAlphaKakutei)	\"(半角英数)\"\n\
modeString(kigo)		\"(記号)\"\n\
modeString(mojishu)		NULL\n\
modeString(tankouho)		NULL\n\
modeString(ichiran)		NULL\n\
modeString(yesno)		NULL\n\
modeString(extend)		NULL\n\
modeString(russian)		NULL\n\
modeString(greek)		NULL\n\
modeString(line)		NULL\n\
modeString(changingServer)	NULL\n\
modeString(henkanMethod)	NULL\n\
modeString(deleteDic)		NULL\n\
modeString(touroku)		\"(単語登録)\"\n\
modeString(tourokuHinshi)	NULL\n\
modeString(tourokuDic)		NULL\n\
modeString(quotedInsert)	NULL\n\
modeString(bubunMuhenkan)	NULL\n\
modeString(zenKataHenkan)	\"(全角カナ)\"\n\
modeString(hanKataHenkan)	\"(半角カナ)\"\n\
modeString(zenAlphaHenkan)	\"(全角英数)\"\n\
modeString(hanAlphaHenkan)	\"(半角英数)\"\n\
modeString(mountDic)		NULL\n\
";

extern int FirstTime;

static currentMode = -1; /* モード番号 + '@' */

int
  XoldLookupKanjiString(event_struct, buffer_return, bytes_buffer,
		     keysym_return, status_return, kanji_status_return)
XKeyEvent      *event_struct;
unsigned char  *buffer_return;
int            bytes_buffer;
KeySym         *keysym_return;
XComposeStatus *status_return;
XKanjiStatus   *kanji_status_return;
{
  static unsigned char echostrbuf[KANJIBUFSIZE + 32];
  int retval;
  int functionalChar;
  int ch;

  /* 初めて XLookupKanjiString が呼ばれた時は辞書の初期化などの処理が
     行われる。 */

  if (FirstTime) {
    int nwarn;
    char **warn = (char **)mode_string;

    retval = jrKanjiControl(0, KC_INITIALIZE, NULL);
    /* NULL はダミー引数 */
    if (retval == -1)
      return -1;
    jrKanjiControl(0, KC_SETWIDTH, 1);
    /* 候補一覧を出ないようにする */
    if (nwarn = jrKanjiControl(0, KC_PARSE, &warn)) {
      printf("エラーの数 = %d\n", nwarn);
      necKanjiError = *warn;
      /* モード文字列の変更に失敗したらおしまいだ */
      return -1;
    }
    jrKanjiControl(0, KC_SETMODEINFOSTYLE, 0);
    /* モード行は文字表現で返す */
  }

  /* 
    XLookupString を用いてまず、キーイベントに対応する文字列を調べる。
    ここで得られる文字列にしたがって、カナ漢字変換に各種の機能キーが
    動作する様になる。
    */

  {
    KeySym keysym;

    retval = XmuLookupKana(event_struct, buffer_return, bytes_buffer,
			   &keysym, status_return);
    if (keysym_return)
      *keysym_return = keysym;

    functionalChar = checkIfFunctionalChar(event_struct, keysym, 
					   buffer_return, bytes_buffer);
  }

  bzero(kanji_status_return, sizeof(jrKanjiStatus));
  
  ch = *buffer_return;

  if ( !(retval || functionalChar) ) {
    /* キャラクタコードがとれなかった場合（シフトキーなど）... */
    kanji_status_return->length = -1;
    return 0;
  }

  /* キャラクタコードがとれた場合 */
  retval = jrKanjiString(0, ch, buffer_return, bytes_buffer, 
			 kanji_status_return);

  if (retval < 0) { /* エラーは返さないようにしなくっちゃ */
    kanji_status_return->echoStr = (unsigned char *)necKanjiError;
    kanji_status_return->length = 
      kanji_status_return->revLen = strlen(necKanjiError);
    kanji_status_return->revPos = 0;
    return 0;
  }

  if (kanji_status_return->info & KanjiGLineInfo) {
    int len;

    len = kanji_status_return->gline.length;
    strncpy(echostrbuf, kanji_status_return->gline.line, len);
    strncpy(echostrbuf + len, kanji_status_return->echoStr,
	    kanji_status_return->length);
    kanji_status_return->echoStr = echostrbuf;
    kanji_status_return->length += len;
    kanji_status_return->revPos += len;
  }
  if (kanji_status_return->info & KanjiModeInfo
      && currentMode != kanji_status_return->mode[0]) {
    int len;
    unsigned char *mS;

    mS = kanji_status_return->mode;
    len = strlen(mS);

    if (kanji_status_return->echoStr == echostrbuf) {
      strncpy(echostrbuf + len, echostrbuf, kanji_status_return->length);
      strncpy(echostrbuf, mS, len);
    }
    else {
      strncpy(echostrbuf, mS, len);
      strncpy(echostrbuf + len, kanji_status_return->echoStr, 
	      kanji_status_return->length);
    }
    kanji_status_return->echoStr = echostrbuf;
    kanji_status_return->length += len;
    kanji_status_return->revPos += len;
  }
  return retval;
}
