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


static unsigned char *mode_string = (unsigned char *)"\
modeString(alpha)		\"(Ⱦ�ѱѿ�)\"\n\
modeString(yomiganai) 		\"(���Ѥ���)\"\n\
modeString(hex)			\"(16��)\"\n\
modeString(bushu)		\"(����)\"\n\
modeString(henkanNyuryoku)	\"(���Ѥ���)\"\n\
modeString(zenHiraKakutei)	\"(���꤫��)\"\n\
modeString(zenKataKakutei)	\"(���ѥ���)\"\n\
modeString(hanKataKakutei)	\"(Ⱦ�ѥ���)\"\n\
modeString(zenAlphaKakutei)	\"(���ѱѿ�)\"\n\
modeString(hanAlphaKakutei)	\"(Ⱦ�ѱѿ�)\"\n\
modeString(kigo)		\"(����)\"\n\
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
modeString(touroku)		\"(ñ����Ͽ)\"\n\
modeString(tourokuHinshi)	NULL\n\
modeString(tourokuDic)		NULL\n\
modeString(quotedInsert)	NULL\n\
modeString(bubunMuhenkan)	NULL\n\
modeString(zenKataHenkan)	\"(���ѥ���)\"\n\
modeString(hanKataHenkan)	\"(Ⱦ�ѥ���)\"\n\
modeString(zenAlphaHenkan)	\"(���ѱѿ�)\"\n\
modeString(hanAlphaHenkan)	\"(Ⱦ�ѱѿ�)\"\n\
modeString(mountDic)		NULL\n\
";

extern int FirstTime;

static currentMode = -1; /* �⡼���ֹ� + '@' */

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

  /* ���� XLookupKanjiString ���ƤФ줿���ϼ���ν�����ʤɤν�����
     �Ԥ��롣 */

  if (FirstTime) {
    int nwarn;
    char **warn = (char **)mode_string;

    retval = jrKanjiControl(0, KC_INITIALIZE, NULL);
    /* NULL �ϥ��ߡ����� */
    if (retval == -1)
      return -1;
    jrKanjiControl(0, KC_SETWIDTH, 1);
    /* ���������Фʤ��褦�ˤ��� */
    if (nwarn = jrKanjiControl(0, KC_PARSE, &warn)) {
      printf("���顼�ο� = %d\n", nwarn);
      necKanjiError = *warn;
      /* �⡼��ʸ������ѹ��˼��Ԥ����餪���ޤ��� */
      return -1;
    }
    jrKanjiControl(0, KC_SETMODEINFOSTYLE, 0);
    /* �⡼�ɹԤ�ʸ��ɽ�����֤� */
  }

  /* 
    XLookupString ���Ѥ��Ƥޤ����������٥�Ȥ��б�����ʸ�����Ĵ�٤롣
    ������������ʸ����ˤ������äơ����ʴ����Ѵ��˳Ƽ�ε�ǽ������
    ư����ͤˤʤ롣
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
    /* ����饯�������ɤ��Ȥ�ʤ��ä����ʥ��եȥ����ʤɡ�... */
    kanji_status_return->length = -1;
    return 0;
  }

  /* ����饯�������ɤ��Ȥ줿��� */
  retval = jrKanjiString(0, ch, buffer_return, bytes_buffer, 
			 kanji_status_return);

  if (retval < 0) { /* ���顼���֤��ʤ��褦�ˤ��ʤ��ä��� */
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
