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
 * XLookupKanjiString �� X ������ɥ��Υ������ϥ��٥�Ȥ� event_struct
 * �ˤ�������ꡢ���Υ����ˤ������ä�ɬ�פʤ饫�ʴ����Ѵ���Ԥ�����
 * �Υ������Ϥη�̤Ȥ���������ʸ����� buffer_return ���֤���
 * buffer_return �ϥ��ץꥱ�������¦���Ѱդ���Хåե��Ǥ��ꡢ���ץ�
 * ���������Ϥ��ΥХåե���Ĺ���� bytes_buffer ���Ϥ��� 
 *
 * keysym_return �ϥ������٥�Ȥ����Ϥ��줿����������̾���֤��������
 * ���ؤΥݥ��󥿤Ǥ��ꡢ�����ѿ��Ϥ�Ϥꥢ�ץꥱ�������¦���ΰ褬��
 * �ݤ���Ƥ��ʤ���Фʤ�ʤ���
 *
 * status_return �Ϻ��ΤȤ����Ѥ����ʤ���
 *
 * kanji_status_return �ϳ��ꤷ�Ƥ��ʤ�����ʸ�����ɽ�����뤿��Υǡ�
 * ���Ǥ��ꡢ̤������ɤߤ��������ʤɤ��֤���롣kanji_status_return
 * �Υ��Фˤϡ� echoStr, length, revPos, revLen �����ꤽ�줾�졢̤��
 * ��ʸ����ؤΥݥ��󥿡�����Ĺ����̤����ʸ����Τ�������Ĵ������ʬ��
 * �Υ��ե��åȡ���Ĵ������ʬ��Ĺ�����֤���̤����ʸ������Ǽ�����ΰ�
 * �� XLookupKanjiString �Ǽ�ưŪ���Ѱդ���롣
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
    XLookupString ���Ѥ��Ƥޤ����������٥�Ȥ��б�����ʸ�����Ĵ�٤롣
    ������������ʸ����ˤ������äơ����ʴ����Ѵ��˳Ƽ�ε�ǽ������
    ư����ͤˤʤ롣    */

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


/* XKanjiControl -- ���ʴ����Ѵ��������Ԥ� */

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

/* checkIfFunctionalChar -- ���եȥ����Τ褦��̵��̣�ʥ������ɤ�����Ƚ��

  ��:		��	��̣������
  		��	̵��̣(��ǥ��ե����������ʤ�)
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
