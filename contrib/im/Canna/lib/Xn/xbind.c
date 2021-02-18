/***********************************************************
 *							   *	
 *            COPYRIGHT (C) 1988 NEC CORPORATION	   * 
 *      	     ALL RIGHTS RESERVED		   *
 *						           *	
 ***********************************************************/

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk/ui/RCS/xbind.c,v 2.2 91/02/14 11:15:09 satoko Exp $";

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include "sglobal.h"
#include <iroha/kanji.h>
   
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

int
  XLookupKanjiString(event_struct, buffer_return, bytes_buffer,
		     keysym_return, status_return, kanji_status_return)
XKeyEvent      *event_struct;
unsigned char  *buffer_return;
int            bytes_buffer;
KeySym         *keysym_return;
XComposeStatus *status_return;
XKanjiStatus   *kanji_status_return;
{
  int functionalChar, res, nbytes;
  KeySym keysym;

  /* 
    XLookupString を用いてまず、キーイベントに対応する文字列を調べる。
    ここで得られる文字列にしたがって、カナ漢字変換に各種の機能キーが
    動作する様になる。    */

  nbytes = XmuLookupKana(event_struct, buffer_return, bytes_buffer,
			 &keysym, status_return);
  if (keysym_return)
    *keysym_return = keysym;

  functionalChar = checkIfFunctionalChar(event_struct, keysym, 
					 buffer_return, bytes_buffer);

  res = XLookupKanji2((unsigned int)(event_struct->display),
		      (unsigned int)(event_struct->window),
		      buffer_return, bytes_buffer, nbytes, functionalChar,
		      kanji_status_return);
  return res;
}


/* XKanjiControl -- カナ漢字変換の制御を行う */

int
XKanjiControl(display, window, request, arg)
     Display *display;
     Window window;
     int request;
     caddr_t arg;
{
  return XKanjiControl2((unsigned int)display, (unsigned int)window,
			request, (char *)arg);
}

/* checkIfFunctionalChar -- シフトキーのように無意味なキーかどうかの判別

  値:		１	意味がある
  		０	無意味(モディファイアキーなど)
*/

int
checkIfFunctionalChar(event_struct, keysym, buffer_return, bytes_buffer)
XKeyEvent *event_struct;
KeySym keysym;
unsigned char *buffer_return;
int bytes_buffer;
{
  int functionalChar = 0;

  switch ((int)keysym)
    {
    case XK_KP_F1:
    case XK_KP_F2:
    case XK_KP_F3:
    case XK_KP_F4:
      *buffer_return = 	IROHA_KEY_PF1 + (int)keysym - (int)XK_KP_F1;
      functionalChar = 1;
      break;
    case XK_F1:
    case XK_F2:
    case XK_F3:
    case XK_F4:
    case XK_F5:
    case XK_F6:
    case XK_F7:
    case XK_F8:
    case XK_F9:
    case XK_F10:
    case XK_F11:
    case XK_F12:
    case XK_F13:
    case XK_F14:
    case XK_F15:
    case XK_F16:
      *buffer_return = IROHA_KEY_F1 + (int)keysym - (int)XK_F1;
      functionalChar = 1;
      break;
    case XK_Insert:
      *buffer_return = IROHA_KEY_Insert;
      functionalChar = 1;
      break;
    case XK_Prior:
      *buffer_return = IROHA_KEY_Rollup;
      functionalChar = 1;
      break;
    case XK_Next:
      *buffer_return = IROHA_KEY_Rolldown;
      functionalChar = 1;
      break;
    case XK_Muhenkan:
      if (event_struct->state & 4 /* control-shifted */)
	*buffer_return = IROHA_KEY_Cntrl_Nfer;
      else if (event_struct->state & 1 /* shifted */)
	*buffer_return = IROHA_KEY_Shift_Nfer;
      else
	*buffer_return = IROHA_KEY_Nfer;
      functionalChar = 1;
      break;
    case XK_Kanji:
      if (event_struct->state & 4 /* control-shifted */)
	*buffer_return = IROHA_KEY_Cntrl_Xfer;
      else if (event_struct->state & 1 /* shifted */)
	*buffer_return = IROHA_KEY_Shift_Xfer;
      else
	*buffer_return = IROHA_KEY_Xfer;
      functionalChar = 1;
      break;
    case XK_Up:
      if (event_struct->state & 4 /* control-shifted */)
	*buffer_return = IROHA_KEY_Cntrl_Up;
      else if (event_struct->state & 1 /* shifted */)
	*buffer_return = IROHA_KEY_Shift_Up;
      else
	*buffer_return = IROHA_KEY_Up;
      functionalChar = 1;
      break;
    case XK_Down:
      if (event_struct->state & 4 /* control-shifted */)
	*buffer_return = IROHA_KEY_Cntrl_Down;
      else if (event_struct->state & 1 /* shifted */)
	*buffer_return = IROHA_KEY_Shift_Down;
      else
	*buffer_return = IROHA_KEY_Down;
      functionalChar = 1;
      break;
    case XK_Right:
      if (event_struct->state & 4 /* control-shifted */)
	*buffer_return = IROHA_KEY_Cntrl_Right;
      else if (event_struct->state & 1 /* shifted */)
	*buffer_return = IROHA_KEY_Shift_Right;
      else
	*buffer_return = IROHA_KEY_Right;
      functionalChar = 1;
      break;
    case XK_Left:
      if (event_struct->state & 4 /* control-shifted */)
	*buffer_return = IROHA_KEY_Cntrl_Left;
      else if (event_struct->state & 1 /* shifted */)
	*buffer_return = IROHA_KEY_Shift_Left;
      else
	*buffer_return = IROHA_KEY_Left;
      functionalChar = 1;
      break;
    case XK_Help:
      *buffer_return = IROHA_KEY_Help;
      functionalChar = 1;
      break;
    case XK_Home:
      *buffer_return = IROHA_KEY_Home;
      functionalChar = 1;
      break;
    }
  return functionalChar;
}
