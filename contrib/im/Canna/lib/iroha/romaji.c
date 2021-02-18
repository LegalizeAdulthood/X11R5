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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/romaji.c,v 3.9 91/09/05 20:32:11 kon Exp $";
     
#include "iroha.h"
#include <ctype.h>
#ifdef MEASURE_TIME
#include <sys/types.h>
#include <sys/times.h>
#endif

static int KanaYomiInsert(), clearYomiContext();
extern int forceKana;
extern InhibitHankakuKana;

/*
 * int d->rStartp;     ro shu c|h    shi f   ���޻� �������� ����ǥå���
 * int d->rEndp;       ro shu ch     shi f|  ���޻� �Хåե� ����ǥå���
 * int d->rCurs;       ro shu ch|    shi f   ���޻� �����   ����ǥå���
 * int d->rAttr[1024]; 10 100 11     100 1   ���޻������Ѵ���Ƭ�ե饰�Хåե�
 * int d->kEndp;       �� ��  ch  �� ��  f|  ����     �Хåե� ����ǥå���
 * int d->kRStartp;    �� ��  c|h �� ��  f   �������� �������� ����ǥå���
 * int d->kCurs;      �� ��  ch| �� ��  f   �������� �����   ����ǥå���
 * int d->kAttr[1024]; 11 11  00  11 11  0   �����Ѵ������ե饰�Хåե�
 * int d->nrf;                1              ���޻��Ѵ����ޤ���ե饰
 */

/*
 * �ե饰��ݥ��󥿤�ư��
 *
 *           �Ҥ㤯           hyaku
 *  ��       100010           10010
 *  ��       111111
 *  ��       000000
 * rStartp                         1
 * rCurs                           1
 * rEndp                           1
 * kRstartp        1
 * kCurs           1
 * kEndp           1
 *
 * ��
 *           �Ҥ㤯           hyaku
 *  ��       100010           10010
 *  ��       111111
 *  ��       000000
 * rStartp                       1
 * rCurs                         1
 * rEndp                           1
 * kRstartp      1
 * kCurs         1
 * kEndp           1
 *
 * ��
 *           �Ҥ㤯           hyaku
 *  ��       100010           10010
 *  ��       111111
 *  ��       000000
 * rStartp                       1
 * rCurs                         1
 * rEndp                           1
 * kRstartp    1
 * kCurs       1
 * kEndp           1
 *
 * ��
 *           �Ҥ㤯           hyaku
 *  ��       100010           10010
 *  ��       111111
 *  ��       000000
 * rStartp                    1
 * rCurs                      1
 * rEndp                           1
 * kRstartp  1
 * kCurs     1
 * kEndp           1
 *
 * ��
 *           �Ҥ㤯           hyaku
 *  ��       100010           10010
 *  ��       111111
 *  ��       000000
 * rStartp                       1
 * rCurs                         1
 * rEndp                           1
 * kRstartp    1
 * kCurs       1
 * kEndp           1
 *
 * 'k'
 *           ��k�㤯           hyakku
 *  ��       1010010           100110
 *  ��       1101111
 *  ��       0010000
 * rStartp                        1
 * rCurs                           1
 * rEndp                             1
 * kRstartp    1
 * kCurs        1
 * kEndp            1
 *
 * 'i'
 *           �Ҥ��㤯           hyakiku
 *  ��       10100010           1001010
 *  ��       11111111
 *  ��       00110000
 * rStartp                           1
 * rCurs                             1
 * rEndp                               1
 * kRstartp      1
 * kCurs         1
 * kEndp             1
 */

#define  ROMAJILIMIT   250

#define  SENTOU        1
#define  HENKANSUMI    2
#define  WARIKOMIMOJI  4
#define  SHIRIKIRE     8
#define  ATAMAKIRE    16

#define  doubleByteP(x) ((x) & 0x80)

#ifndef CALLBACK
#define kanaReplace(where, insert, insertlen, mask) \
kanaRepl(d, where, insert, insertlen, mask)

static
kanaRepl(d, where, insert, insertlen, mask)
uiContext d;
int where, insertlen, mask;
char *insert;
{
  yomiContext yc = (yomiContext)d->modec;

  generalReplace(yc->kana_buffer, yc->kAttr, &yc->kRStartp,
		 &yc->kCurs, &yc->kEndp,
		 where, insert, insertlen, mask);
}
#else /* CALLBACK */
#define kanaReplace(where, insert, insertlen, mask) \
kanaRepl(d, where, insert, insertlen, mask)

static
kanaRepl(d, where, insert, insertlen, mask)
uiContext d;
int where, insertlen, mask;
char *insert;
{
  char buf[256];
  yomiContext yc = (yomiContext)d->modec;

  strncpy(buf, insert, insertlen);
  buf[insertlen] = '\0';
  printf("���������%s %d �Х��Ȥ� \"%s\" ���Ѥ��ޤ�����\n",
	 where > 0 ? "��" : "��",  where > 0 ? where : -where, buf);

  generalReplace(yc->kana_buffer, yc->kAttr, &yc->kRStartp, 
		 &yc->kCurs, &yc->kEndp,
		 where, insert, insertlen, mask);
}
#endif /* CALLBACK */

#define  romajiReplace(where, insert, insertlen, mask) \
romajiRepl(d, where, insert, insertlen, mask)

static
romajiRepl(d, where, insert, insertlen, mask)
uiContext d;
int where, insertlen, mask;
char *insert;
{
  yomiContext yc = (yomiContext)d->modec;

  generalReplace(yc->romaji_buffer, yc->rAttr,
		 &yc->rStartp, &yc->rCurs, &yc->rEndp,
		 where, insert, insertlen, mask);
}

static int ichitaroMove = 0;
static int ReverseOnlyCursor = 1;

void
setIchitaroMove(x)
int x;
{
  ichitaroMove = x;
}

void
setReverseOnlyCursor(x)
int x;
{
  ReverseOnlyCursor = x;
}

/*
  makeYomiReturnStruct -- �ɤߤ򥢥ץꥱ���������֤����ι�¤�Τ���ؿ�

  makeYomiReturnStruct �� kana_buffer ��Ĵ�٤�Ŭ�����ͤ��Ȥ�Ω�Ƥ롣��
  �λ��˥�С������ΰ�����ꤹ�뤬����С�����ɤΤ��餤���뤫�ϡ�
  ReverseOnlyCursor�Ȥ����ѿ��򸫤Ʒ��ꤹ�롣

  */

void
makeYomiReturnStruct(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  if (ReverseOnlyCursor) {
    if (yc->kCurs == yc->kEndp) {
      d->kanji_status_return->revPos = 0;
      d->kanji_status_return->revLen = 0;
    }
    else {
      d->kanji_status_return->revPos  = yc->kCurs;
      d->kanji_status_return->revLen = 
	doubleByteP(yc->kana_buffer[yc->kCurs]) ? 2 : 1;
    }
  }
  else {
    d->kanji_status_return->revPos = 0;
    d->kanji_status_return->revLen = yc->kCurs;
  }
  d->kanji_status_return->echoStr = yc->kana_buffer;
  d->kanji_status_return->length = yc->kEndp;
}

extern ckverbose;

RomkanaInit(romajifile)
char *romajifile;
{
  extern char *RomkanaTable;

  /* ���޻������Ѵ�����Υ����ץ� */
  if (RomkanaTable) {
    romajidic = RkOpenRoma(RomkanaTable);

#ifdef DEBUG
  if (iroha_debug) {
    fprintf(stderr, "���޻������Ѵ�����̾(%s)\n", RomkanaTable);
  }
#endif /* DEBUG */

    if (ckverbose == CANNA_FULL_VERBOSE) {
      if (romajidic != NULL) { /* ���񤬥����ץ�Ǥ��� */
	printf("���޻������Ѵ������ \"%s\" ���Ѥ��ޤ���\n", RomkanaTable);
      }
    }

    if (romajidic == NULL) { /* �⤷���񤬥����ץ�Ǥ��ʤ���Х��顼 */
      char rdic[1024], *p, *getenv();

      p = getenv("HOME");
      strcpy(rdic, p);
      strcat(rdic, "/");
      strcat(rdic, RomkanaTable);
      romajidic = RkOpenRoma(rdic);

#ifdef DEBUG
  if (iroha_debug) {
    fprintf(stderr, "��μ��񤬥����ץ�Ǥ��ʤ��Τ�(%s)\n", rdic);
  }
#endif /* DEBUG */

      if (ckverbose == CANNA_FULL_VERBOSE) {
	if (romajidic != NULL) {
	  printf("���޻������Ѵ������ \"%s\" ���Ѥ��ޤ���\n", rdic);
	}
      }

      if (romajidic == NULL) { /* ����⥪���ץ�Ǥ��ʤ� */
	strcpy(rdic, LIBDIR);
	strcat(rdic, "/dic/");
	strcat(rdic, RomkanaTable);
	romajidic = RkOpenRoma(rdic);

#ifdef DEBUG
  if (iroha_debug) {
    fprintf(stderr, "��μ��񤬥����ץ�Ǥ��ʤ��Τ�(%s)\n", rdic);
  }
#endif /* DEBUG */

	if (ckverbose) {
	  if (romajidic != NULL) {
	    if (ckverbose == CANNA_FULL_VERBOSE) {
	      printf("���޻������Ѵ������ \"%s\" ���Ѥ��ޤ���\n", rdic);
	    }
	  }
	  else {
	    printf("���޻������Ѵ����� \"%s\" �������ץ�Ǥ��ޤ���\n",
		   RomkanaTable);
	  }
	}

	if (romajidic == NULL) { /* ���������ץ�Ǥ��ʤ� */
	  sprintf(romajifile, 
		  "���޻������Ѵ��ơ��֥�(%s)�������ץ�Ǥ��ޤ���",
		  RomkanaTable);
	  necKanjiError = romajifile;
	  return -1;
	}
      }
    }
  }
  else {
    strcpy(romajifile, LIBDIR);
    strcat(romajifile, "/dic/default.rdic");
    romajidic = RkOpenRoma(romajifile);

    if (ckverbose) {
      if (romajidic != NULL) { /* ���񤬥����ץ�Ǥ��� */
	if (ckverbose == CANNA_FULL_VERBOSE)
	  printf("���޻������Ѵ������ \"%s\" ���Ѥ��ޤ���\n", romajifile);
      }
      else {
	printf("���޻������Ѵ����� \"%s\" �������ץ�Ǥ��ޤ���\n",
	       romajifile);
      }
    }

    if (romajidic == NULL) { /* �⤷���񤬥����ץ�Ǥ��ʤ���Х��顼 */
      sprintf(romajifile, 
	      "�����ƥ�Υ��޻������Ѵ��ơ��֥뤬�����ץ�Ǥ��ޤ���");
      necKanjiError = romajifile;
      return -1;
    }
  }
  return 0;
}

/* ���޻������Ѵ�����Υ����� */

RomkanaFin()
{
  extern char *RomkanaTable;

  /* ���޻������Ѵ�����Υ����� */
  if (romajidic != NULL) {
    RkCloseRoma(romajidic);
  }
  if (RomkanaTable) {
    free(RomkanaTable);
  }
}

/* cfunc yomiContext

  yomiContext ��¤�Τ��ĺ���֤���

 */

yomiContext
newYomiContext(buf, bufsize, allowedc, chmodinhibit,
	       quitTiming, hinhibit, base, hcb)
     char *buf;
     int bufsize;
     char allowedc, chmodinhibit, quitTiming, hinhibit, base;
     int (*hcb)();
{
  yomiContext ycxt;

  ycxt = (yomiContext)malloc(sizeof(yomiContextRec));
  if (ycxt) {
    ycxt->id = YOMI_CONTEXT;
    clearYomiContext(ycxt);
    ycxt->allowedChars = allowedc;
    ycxt->generalFlags = chmodinhibit ? IROHA_YOMI_CHGMODE_INHIBITTED : 0;
    ycxt->generalFlags |= quitTiming ? IROHA_YOMI_END_IF_KAKUTEI : 0;
    ycxt->henkanInhibition = hinhibit;
    ycxt->baseChar = base;
    ycxt->retbufp = ycxt->retbuf = buf;
    ycxt->henkanCallback = hcb;
    if ((ycxt->retbufsize = bufsize) == 0) {
      ycxt->retbufp = 0;
    }
    ycxt->next = 0;
    ycxt->prevMode = 0;
  }
  return ycxt;
}

void
freeYomiContext(yc)
yomiContext yc;
{
  free(yc);
}

/*

  GetKanjiString �ϴ������ʺ�����ʸ���äƤ���ؿ��Ǥ��롣�ºݤˤ� 
  empty �⡼�ɤ����ꤹ������ǥ꥿���󤹤롣�ǽ�Ū�ʷ�̤� buf �ǻ���
  ���줿�Хåե��˳�Ǽ���� exitCallback ���ƤӽФ���뤳�Ȥˤ�äƸƤ�
  �Ф�¦�ϴ������ʺ�����ʸ�������뤳�Ȥ��Ǥ��롣

  �裲������ ycxt ���̾�ϣ�����ꤹ�롣����ե��٥åȥ⡼�ɤ������ܸ�
  �⡼�ɤؤ��ڤ��ؤ��˺ݤ��ƤΤߤ� uiContext �������¸���Ƥ��륳���
  �����Ȥ��Ѥ��롣����ե��٥åȥ⡼�ɤ����ܸ�⡼�ɤȤ��ڤ��ؤ��ϥ�����
  ������Ѥ߹��ޤ줿�⡼�ɤ� push/pop ���ǤϤʤ�������å׾�Υ⡼��
  �ΰ��־�����Ǥ������ؤ��ˤʤ롣

  ���Ĥ� Callback �Τ�����exitCallback �ϤҤ�äȤ�����Ȥ��ʤ��ǡ�
  everyTimeCallback �� quitCallback �����Ѥ��ʤ������Τ�ʤ���

 */

void popCallback();

yomiContext
GetKanjiString(d, buf, bufsize, allowedc, chmodinhibit,
	       quitTiming, hinhibit, base,
	       henkanCallback,
	       everyTimeCallback, exitCallback, quitCallback)
     uiContext d;
     char *buf;
     int bufsize;
     char allowedc, chmodinhibit, quitTiming, hinhibit, base;
     int (*henkanCallback)();
     int (*everyTimeCallback)(), (*exitCallback)(), (*quitCallback)();
{
  extern KanjiModeRec empty_mode;
  struct callback *pushCallback();
  yomiContext yc;

  if ((pushCallback(d, d->modec, everyTimeCallback, exitCallback, quitCallback,
		    (int (*)()) NULL)) == (struct callback *)0) {
    return (yomiContext)0;
  }

  yc = newYomiContext(buf, bufsize, allowedc, chmodinhibit,
		      quitTiming, hinhibit, base, henkanCallback);
  if (yc == (yomiContext)0) {
    popCallback(d);
    return (yomiContext)0;
  }
  yc->majorMode = d->majorMode;
  yc->minorMode = IROHA_MODE_HenkanMode;
  yc->next = d->modec;
  d->modec = (mode_context)yc;
  /* ���Υ⡼�ɤ���¸ */
  yc->prevMode = d->current_mode;
  /* �⡼���ѹ� */
  d->current_mode = &empty_mode;
  return yc;
}

/* cfuncdef

   popYomiMode -- �ɤߥ⡼�ɤ�ݥåץ��åפ��롣

 */

void
popYomiMode(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  d->modec = yc->next;
  d->current_mode = yc->prevMode;
  freeYomiContext(yc);
}

/* cfuncdef

  checkIfYomiExit -- �ɤߥ⡼�ɤ���λ���ɤ�����Ĵ�٤��ͤ��֤��ե��륿

  ���Υե��륿���ɤߥ⡼�ɤγƴؿ����ͤ��֤����Ȥ�����˸Ƥ֡��ɤߥ⡼
  �ɤǤν�������λ����Ȥ���Ǥ���С��ɤߥ⡼�ɤ�λ����uiContext ��
  �ץå��夵��Ƥ���������ǡ�����⡼�ɹ�¤�Τ��ݥåפ���롣

  ������ǡ����� exitCallback ���������Ƥ��ʤ���Ф����ʤ���ˤ�
  ��¤�ΤΥݥåץ��åפϹԤ��ʤ���

  ���ΤȤ����ɤߥ⡼�ɤν�λ�ϼ��Τ褦�ʾ�礬�ͤ����롣

  (1) C-m �������ɤߤκǸ��ʸ���Ȥ����֤��줿����(�Ѵ����Ĥλ�)

  (2) ����ʸ����¸�ߤ����硣(�Ѵ��ػߤλ�)

  quit ���ɤߥ⡼�ɤ�λ�������?¾�δؿ�?��Ƥ֡�

 */

static
checkIfYomiExit(d, retval)
uiContext d;
int retval;
{
  yomiContext yc = (yomiContext)d->modec;

  if (retval <= 0) {
    /* ����ʸ���󤬤ʤ������顼�ξ�� �� exit �ǤϤʤ� */
    return retval;
  }
  if (yc->retbufp && yc->retbufsize - (yc->retbufp - yc->retbuf) > retval) {
    /* ʸ�����Ǽ�Хåե������äơ����ꤷ��ʸ������⤢�ޤäƤ�����
       �褬Ĺ���ΤǤ���г�Ǽ�Хåե��˳��ꤷ��ʸ����򥳥ԡ����� */
    strncpy(yc->retbufp, d->buffer_return, retval);
    yc->retbufp[retval] = '\0';
    yc->retbufp += retval;
  }
  if (yc->generalFlags & IROHA_YOMI_END_IF_KAKUTEI
      || d->buffer_return[retval - 1] == '\n') {
    /* �Ѵ����ػߤ���Ƥ���Ȥ����� exit */
    /* �����Ǥʤ����ϡ�\n �����äƤ����� exit */
    d->status = EXIT_CALLBACK;
    if (d->cb->func[EXIT_CALLBACK] == NO_CALLBACK) {
      /* ������Хå����ʤ����

	 ����ʥ����å�����ڤ˹Ԥ��Τϡ��ɤߥ⡼�ɤ����˴���Ū�ʥ⡼��
	 �Ǥ��ꡢ������ȴ����Ȥ��ˤ虜�虜�ݥåץ��åפ��Ƥ⤹���˥ץå���
	 �����礬¿���ȹͤ����ƽ�����̵�̤�����Ǥ��롣

       */
    }
    else {
      d->status = EXIT_CALLBACK;
      popYomiMode(d);
    }
  }
  return retval;
}

static
checkIfYomiQuit(d, retval)
uiContext d;
int retval;
{
#ifdef QUIT_IN_YOMI /* �����ȥ����Ȥ�����Ū�� ifdef */
  yomiContext yc = (yomiContext)d->modec;

  if (d->cb->func[QUIT_CALLBACK] == NO_CALLBACK) {
    /* ������Хå����ʤ����

       ����ʥ����å�����ڤ˹Ԥ��Τϡ��ɤߥ⡼�ɤ����˴���Ū�ʥ⡼��
       �Ǥ��ꡢ������ȴ����Ȥ��ˤ虜�虜�ݥåץ��åפ��Ƥ⤹���˥ץå���
       �����礬¿���ȹͤ����ƽ�����̵�̤�����Ǥ��롣

     */
  }
  else {
    d->status = QUIT_CALLBACK;
    popYomiMode(d);
  }
#endif /* QUIT_IN_YOMI */
  return retval;
}


/* tabledef

 charKind -- ����饯���μ���Υơ��֥�

 0x20 ���� 0x7f �ޤǤΥ���饯���μ����ɽ���ơ��֥�Ǥ��롣

 3: ����
 2: �����ʿ��Ȥ����Ѥ�����ѻ�
 1: ����ʳ��αѻ�
 0: ����¾

 �Ȥʤ롣

 */

static char charKind[] = {
/*sp !  "  #  $  %  &  '  (  )  *  +  ,  -  .  / */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ? */
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1,
/*@  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O */
  1, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2,
/*P  Q  R  S  T  U  V  W  X  Y  X  [  \  ]  ^  _ */
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
/*`  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o */
  1, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2,
/*p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~  DEL */
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
};

static
getCurrentJishu(yc)
yomiContext yc;
{
  switch (yc->baseChar) {
  case JISHU_ZEN_KATA:
    return RK_SOKON | RK_KFER;
  case JISHU_HAN_KATA:
    return RK_SOKON | RK_HFER;
  default:
    return RK_SOKON | RK_NFER;
  }
}

/*
  YomiInsert -- ���޻���ʸ����������ؿ�

  */

YomiInsert(d)
uiContext d;
{
  char roma_char[1024]; /* �Ѵ��������޻� */
  char kana_char[1024]; /* �Ѵ��������� */
  char xxxx[1024];
  int i, n, m, idou;
  int henkanflag, jishuflag;
  extern BreakIntoRoman;
  yomiContext yc = (yomiContext)d->modec;
  
  if (yc->allowedChars == IROHA_NOTHING_ALLOWED/* �ɤΥ�������դ��ʤ� */
      || yc->rEndp > ROMAJILIMIT) { /* �ޤ��ϡ����°ʾ������褦�Ȥ��� */
    return NothingChangedWithBeep(d);
  }

  if (d->ch > 127) {
    if (yc->allowedChars == IROHA_NOTHING_RESTRICTED) {
      return KanaYomiInsert(d); /* callback �Υ����å��� KanaYomiInsert ��! */
    }
    else {
      return NothingChangedWithBeep(d);
    }
  }

  if (d->ch < 0x20 && yc->allowedChars != IROHA_NOTHING_RESTRICTED
      || charKind[d->ch - 0x20] < yc->allowedChars) {
    /* 0x20 �ϥ���ȥ��륭��饯����ʬ */
    return NothingChangedWithBeep(d);
  }

  if (yc->allowedChars != IROHA_NOTHING_RESTRICTED ||
    /* allowed all �ʳ��Ǥϥ��޻������Ѵ���Ԥ�ʤ� */
      yc->baseChar == JISHU_ZEN_ALPHA || yc->baseChar == JISHU_HAN_ALPHA) {
    romajiReplace(0, &d->ch, 1, 0);

    if (yc->baseChar == JISHU_HAN_KATA || yc->baseChar == JISHU_HAN_ALPHA) {
      kanaReplace(0, &d->ch, 1, HENKANSUMI);
    }
    else {
      unsigned char romanBuf[4]; /* ���Х��Ȥǽ�ʬ���Ȼפ����ɤ� */
      int len;

      len = RkCvtZen(romanBuf, 4, &d->ch, 1);
      kanaReplace(0, romanBuf, len, HENKANSUMI);
    }
    yc->rStartp = yc->rCurs;
    yc->kRStartp = yc->kCurs;
  }
  else { /* ���޻������Ѵ������� */
    if (BreakIntoRoman)
      yc->generalFlags |= IROHA_YOMI_BREAK_ROMAN;

    /* �ޤ�����������ʬ�˥��޻���ʸ������� */

    romajiReplace(0, &d->ch, 1, 0);

    /* ľ����̤�Ѵ�ʸ���󤬤ʤ����ɤ�����ǧ */

    if (yc->kCurs == yc->kRStartp) {
      do { 
	yc->kRStartp--;
	yc->rStartp--;
      } while ( yc->kRStartp >= 0 
	       && !(yc->kAttr[yc->kRStartp] & HENKANSUMI) );
      yc->kRStartp++;
      yc->rStartp++;
    }
    
    jishuflag = getCurrentJishu(yc);

    /* ̤�Ѵ�����ʸ����Τ����Ѵ� */
    if (romajidic != NULL) {
      n = RkMapRoma(romajidic, kana_char, 1024, 
		    yc->romaji_buffer + yc->rStartp, yc->rCurs - yc->rStartp,
		    jishuflag, &m);
    }
    else {
      m = -(n = (yc->rCurs - yc->rStartp) ? 1 : 0);
      strncpy(kana_char, yc->romaji_buffer + yc->rStartp, n);
    }
    
    /* ���޻��Τ��� n ʸ��ʬ���ʤ��Ѵ����줿 */
    
    while (n > 0) {
      if (m < 0) {
	henkanflag = 0;
	m = -m; /* |m| ���Ѵ����줿ʸ�����Ĺ���򼨤� */
      }
      else {
	henkanflag = HENKANSUMI;
      }
      
      for (i = 1 ; i < n ; i++) {
	yc->rAttr[yc->rStartp + i] &= ~SENTOU;      /* ��Ƭ�ʳ��ᣰ */
      }
      yc->rStartp += n;      /* n ʸ��ʬ���� */
      
      /* ���޻������Ѵ��η�̤򥫥ʥХåե�������롣 */
      
      kanaReplace(yc->kRStartp - yc->kCurs/* ��: ��ˤʤ�ޤ� */, 
		  kana_char, m,
		  henkanflag);
      yc->kRStartp = yc->kCurs;
      
      /* ̤�Ѵ�����ʸ����Τ����Ѵ� */
      if (romajidic != NULL) {
	n = RkMapRoma(romajidic, kana_char, 1024, 
		      yc->romaji_buffer + yc->rStartp, yc->rCurs - yc->rStartp,
		      jishuflag, &m);
      }
      else {
	m = -(n = (yc->rCurs - yc->rStartp) ? 1 : 0);
	strncpy(kana_char, yc->romaji_buffer + yc->rStartp, n);
      }
      
      /* ���޻��Τ��� n ʸ��ʬ���ʤ��Ѵ����줿 */
    }
    
    /* ���޻������Ѵ��˻Ȥ��ʤ��ä����޻�������դ�­�� */
    if (yc->rCurs - yc->rStartp) {
      n = 0; /* �����ǧ���Ƥ��� */
      if (forceKana) { /* ;�ä����޻���̵���������ʤˤ��� */
	if (romajidic != NULL) {
	  n = RkMapRoma(romajidic, kana_char, 1024,
			yc->romaji_buffer + yc->rStartp,
			yc->rCurs - yc->rStartp,
			jishuflag | RK_FLUSH, &m);
	}
	else {
	  m = -(n = (yc->rCurs - yc->rStartp) ? 1 : 0);
	  strncpy(kana_char, yc->romaji_buffer + yc->rStartp, n);
	}

	if (n > 0) {
	  if (m < 0) {
	    henkanflag = 0;
	    m = -m; /* |m| ���Ѵ����줿ʸ�����Ĺ���򼨤� */
	  }
	  else {
	    henkanflag = HENKANSUMI;
	  }
      
	  for (i = 1 ; i < n ; i++) {
	    yc->rAttr[yc->rStartp + i] &= ~SENTOU;      /* ��Ƭ�ʳ��ᣰ */
	  }
	  yc->rStartp += n;      /* n ʸ��ʬ���� */

	  kanaReplace(yc->kRStartp - yc->kCurs/* ��: ��ˤʤ�ޤ� */, 
		      kana_char, m, henkanflag);
	  yc->kRStartp = yc->kCurs;
	}
      }
      if (yc->rCurs - yc->rStartp) {
	kanaReplace(yc->kRStartp - yc->kCurs/* ��: ��ˤʤ�ޤ� */, 
		    yc->romaji_buffer + yc->rStartp,
		    yc->rCurs - yc->rStartp,
		    SENTOU);
      }
      if (n > 0) { /* forceKana �Ǥ��ʤ����ä���礳�����̤� */
	yc->rStartp -= n;
	yc->kRStartp -= m;
      }
    }
  }
    
#ifdef DEBUG
  if (iroha_debug) {
    fprintf(stderr, "kCurs(%d), kEndp(%d)\n", yc->kCurs, yc->kEndp);
    /*
      fprintf(stderr, "romab(%s)", yc->romaji_buffer);
      fprintf(stderr, "romac(%s)\n", roma_char);
      fprintf(stderr, "kanab(%s)", yc->kana_buffer);
      fprintf(stderr, "kanac(%s)\n", kana_char);
      */
    fprintf(stderr, "length(%d)\n", d->kanji_status_return->length);
    fprintf(stderr, "revPos(%d)", d->kanji_status_return->revPos);
    fprintf(stderr, "revLen(%d)\n", d->kanji_status_return->revLen);
  }
#endif

  makeYomiReturnStruct(d);
  return 0;
}

#define KANAYOMIINSERT_BUFLEN 10
#define DAKUON		0xde
#define HANDAKUON	0xdf

static
KanaYomiInsert(d)
uiContext d;
{
  static unsigned char kana[] = {' ', ' ', 0x8e, ' '}, *kanap;
  unsigned char buf1[KANAYOMIINSERT_BUFLEN], buf2[KANAYOMIINSERT_BUFLEN];
  int len, replacelen, dakuten = 0;
  yomiContext yc = (yomiContext)d->modec;

  yc->generalFlags &= ~IROHA_YOMI_BREAK_ROMAN;
  kanap = kana + 2; len = 2; replacelen = 0;
  kana[3] = d->ch;
  romajiReplace(0, kanap, len, 0);
  yc->rStartp = yc->rCurs;
  if (d->ch == DAKUON || d->ch == HANDAKUON) { /* ����������ä��ꤷ���� */
    if (yc->rCurs >= 4) { /* ����ʸ����������ˤϼ��ν����򤹤� */
      kana[0] = yc->romaji_buffer[yc->rCurs - 4];
      kana[1] = yc->romaji_buffer[yc->rCurs - 3];
      if ((kana[0] & 0x80) && (kana[1] & 0x80)
	  && (kana[1] != DAKUON) && (kana[1] != HANDAKUON)
	  ) { /* ����ʸ�������ʤǡ�����������ǤϤʤ����˼��򤹤� */
	kanap = kana; len = 4; replacelen = -2;
	dakuten = 1;
      }
    }
  }
#ifdef DEBUG
  if (iroha_debug) {
    char aho[200];

    strncpy(aho, kanap, len);
    aho[len] = 0;
    fprintf(stderr, "�Ѵ���(%s)", aho);
  }
#endif
  switch (yc->baseChar) {
  case JISHU_HIRA:
  case JISHU_ZEN_KATA:
  case JISHU_ZEN_ALPHA:
    len = RkCvtZen(buf1, KANAYOMIINSERT_BUFLEN, kanap, len);
#ifdef DEBUG
    if (iroha_debug) {
      char aho[200];

      strncpy(aho, buf1, len);
      aho[len] = 0;
      fprintf(stderr, "����(%s)", aho);
    }
#endif
    if (yc->baseChar == JISHU_HIRA) {
      len = RkCvtHira(buf2, KANAYOMIINSERT_BUFLEN, buf1, len);
#ifdef DEBUG
      if (iroha_debug) {
	char aho[200];

	strncpy(aho, buf2, len);
	aho[len] = 0;
	fprintf(stderr, "����(%s)\n", aho);
      }
#endif
    }
    else {
      len = RkCvtNone(buf2, KANAYOMIINSERT_BUFLEN, buf1, len);
    }
    break;
  default:
    len = RkCvtNone(buf2,  KANAYOMIINSERT_BUFLEN, kanap, len);
    break;
  }
  if (dakuten && len == 2) { /* ��������Ƭ�Ǥʤ��� */
    yc->rAttr[yc->rCurs - 2] &= ~SENTOU;
  }
  kanaReplace(replacelen, buf2, len, HENKANSUMI);
  if (dakuten && len > 2) { 
    /* "����" �Τ褦��ʸ���󤬰��٤��֤äƤ��ޤä��Ȥ��ˡ�"��"
       ����Ƭ�����դ�������򤹤롣���޻��Хåե������äƤ�������
       �ˤ���Ƭ�����դ��Ƥ���Τǡ������̷��ʤ��������Ԥ���褦�ˤʤ롣
       ���޻��Хåե�����������Ƭ�������뤳�Ȥ��ɤ��ʤ����Ȥ����Τϡ�
       ���ʥХåե��ˤ��������������������Ȥν��������ޤ������ʤ��ʤ뤫��
       �Ǥ��롣 */
    yc->kAttr[yc->kCurs - 2] |= SENTOU;
  }
  yc->kRStartp = yc->kCurs;

  makeYomiReturnStruct(d);
  return 0;
}

#undef KANAYOMIINSERT_BUFLEN

static
moveStrings(str, attr, start, end, distance)
char *str, *attr;
int  start, end, distance;
{     
  int i;

  if (distance == 0)
    ;
  else if (distance > 0) { /* ���ˤ����� */
    for (i = end ; start <= i ; i--) { /* ����餺�餹 */
      str[i + distance]  = str[i];
      attr[i + distance] = attr[i];
    }
  }
  else /* if (distance < 0) */ { /* ���ˤ����� */
    for (i = start ; i <= end ; i++) {     /* �����餺�餹 */
      str[i + distance]  = str[i];
      attr[i + distance] = attr[i];
    }
  }
}

static
howFarToGoBackward(d, chars)
uiContext d;
int *chars;
{
  yomiContext yc = (yomiContext)d->modec;

  *chars = 0;
  if (yc->kCurs == 0)
    return 0;
  else if (ichitaroMove) {
    int i;
    
    for (i = yc->kCurs - 1 ; i > 0 ; i--) {
      if (yc->kana_buffer[i] & 0x80)
	i--;
      ++*chars;
      if (yc->kAttr[i] & SENTOU)
	break;
    }
    return (yc->kCurs - i);
  }
  else {
    *chars = 1;
    return doubleByteP(yc->kana_buffer[yc->kCurs - 1]) ? 2 : 1;
  }
}

static
howFarToGoForward(d, chars)
uiContext d;
int *chars;
{
  yomiContext yc = (yomiContext)d->modec;

  *chars = 0;
  if (yc->kCurs == yc->kEndp)
    return 0;
  else if (ichitaroMove) {
    int i;
    
    i = yc->kCurs;
    i += (yc->kana_buffer[i] & 0x80) ? 2 : 1;
    *chars = 1;
    for (; i < yc->kEndp ; i++) {
      if (yc->kAttr[i] & SENTOU)
	break;
      if (yc->kana_buffer[i] & 0x80)
	i++;
      ++*chars;
    }
    return (i - yc->kCurs);
  }
  else {
    *chars = 1;
    return doubleByteP(yc->kana_buffer[yc->kCurs]) ? 2 : 1;
  }
}

static
YomiBackward(d) /* ��������κ���ư */
uiContext d;
{
  int i, howManyMove, howManyChars;
  yomiContext yc = (yomiContext)d->modec;

  if (yc->kCurs != yc->kRStartp) {
    RomajiFlushYomi(d, NULL, 0);
  }

  howManyMove = howFarToGoBackward(d, &howManyChars);

  if (howManyMove == 0) { /* ���줬���Ȥ������ȤϺ�ü���Ȥ������Ȥ��� */
    extern int CursorWrap;

    if (CursorWrap) {
      yc->kCurs = yc->kRStartp = yc->kEndp;
      yc->rCurs = yc->rStartp = yc->rEndp;
#ifdef CALLBACK
      printf("���ָ�˰�ư����\n");
#endif /* CALLBACK */
    }
    else {
      d->kanji_status_return->length = -1;
      return 0;
    }
  }
  else {  /* ʸ����(yc->kana_buffer)�κ�ü�Ǥʤ���� */
    yc->kCurs -= howManyMove;

#ifdef DEBUG
  if (iroha_debug) {
    fprintf(stderr, "backward(%d), kCurs(%d)\n", howManyMove, yc->kCurs);
  }
#endif /* DEBUG */

    if (yc->kCurs < yc->kRStartp)
      yc->kRStartp = yc->kCurs;   /* ̤������޻���������⤺�餹 */


    /* ���ʤΥݥ��󥿤��Ѵ����줿�Ȥ�������Υǡ����Ǥʤ����
       (�Ĥޤ��Ѵ��λ�����Ƭ�Υǡ������ä����)�ˤϥ��޻���
       ��������⤺�餹 */

    if (yc->kAttr[yc->kCurs] & SENTOU) {
      while ( yc->rCurs > 0 && !(yc->rAttr[--yc->rCurs] & SENTOU) )
	;
      if (yc->rCurs < yc->rStartp)
	yc->rStartp = yc->rCurs;
    }

#ifdef CALLBACK
    printf("%d ʸ�����˰�ư\n", howManyChars);
#endif /* CALLBACK */
  }
  makeYomiReturnStruct(d);

  return 0;
}

static
YomiForward(d) /* ��������α���ư */
uiContext d;
{
  int howManyMove, prevAttr, howManyChars;
  int i, f;
  yomiContext yc = (yomiContext)d->modec;

  if (yc->kCurs != yc->kRStartp) {
    RomajiFlushYomi(d, NULL, 0);
  }

  howManyMove = howFarToGoForward(d, &howManyChars);
  if (howManyMove == 0) { /* ���줬�����Ȥ������Ȥϱ�ü�Ȥ������ȤʤΤ� */
    extern int CursorWrap;

    if (CursorWrap) {
      yc->kRStartp = yc->kCurs = 0;
      yc->rStartp = yc->rCurs = 0;
#ifdef CALLBACK
      printf("��ü�˰�ư\n");
#endif /* CALLBACK */
    }
    else {
      d->kanji_status_return->length = -1;
      return 0;
    }
  }
  else { /* ����ư�������� */
    if (yc->kAttr[yc->kCurs] & SENTOU) { /* ���޻������Ѵ�����Ƭ���ä� */
      while ( !yc->rAttr[++yc->rCurs] )
	; /* ������Ƭ�ޤǤ��餹 */
      yc->rStartp = yc->rCurs;
    }

#ifdef CALLBACK
    printf("%d ʸ�����˰�ư����\n", howManyChars);
#endif /* CALLBACK */

    yc->kCurs += howManyMove;   /* ���̤����ϰ��� ��������򱦤ˤ��餹 */
    yc->kRStartp = yc->kCurs;
  }
  makeYomiReturnStruct(d);
  return 0;
}

static
YomiBeginningOfLine(d) /* ��������κ�ü��ư */
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  if (yc->kCurs != yc->kRStartp) {
    RomajiFlushYomi(d, NULL, 0);
  }
  yc->kRStartp = yc-> kCurs = 0;
  yc->rStartp = yc-> rCurs = 0;
#ifdef CALLBACK
  printf("��ü��ư\n");
#endif /* CALLBACK */
  makeYomiReturnStruct(d);
  return 0;
}

static
YomiEndOfLine(d) /* ��������α�ü��ư */
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  if (yc->kCurs != yc->kRStartp) {
    RomajiFlushYomi(d, NULL, 0);
  }
  yc->kRStartp = yc-> kCurs = yc->kEndp;
  yc->rStartp = yc-> rCurs = yc->rEndp;
#ifdef CALLBACK
  printf("��ü��ư\n");
#endif /* CALLBACK */
  makeYomiReturnStruct(d);
  return 0;
}

/* RomajiFlushYomi(d, buffer, bufsize) �桼�ƥ���ƥ��ؿ�
 *
 * ���δؿ��ϡ�(uiContext)d ���ߤ����Ƥ����ɤߤξ��� 
 * (yc->romaji_buffer �� yc->kana_buffer)���Ѥ��ơ�buffer �ˤ����ɤߤ��
 * ��å��夷����̤��֤��ؿ��Ǥ��롣�ե�å��夷����̤�ʸ�����Ĺ��
 * �Ϥ��δؿ����֤��ͤȤ����֤���롣
 *
 * buffer �Ȥ��� NULL �����ꤵ�줿���ϡ��Хåե����Ф����Ǽ�ϹԤ�ʤ�
 *
 * �ں��ѡ�   
 *
 *    �ɤߤ���ꤹ��
 *
 * �ڰ�����
 *
 *    d  (uiContext)  ���ʴ����Ѵ���¤��
 *    buffer (char *)    �ɤߤ��֤�����ΥХåե� (NULL ��)
 *
 * ������͡�
 *
 *    buffer �˳�Ǽ����ʸ�����Ĺ��(�Х���Ĺ)
 *
 * �������ѡ�
 *
 */

RomajiFlushYomi(d, b, bsize)
uiContext d;
char *b;
int bsize;
{
  int n, m, ret, idou, henkanflag;
  char kana_char[1024];
  yomiContext yc = (yomiContext)d->modec;

  yc->generalFlags &= ~IROHA_YOMI_BREAK_ROMAN;

  while (yc->rStartp < yc->rCurs) {
    if (romajidic != NULL) {
      n = RkMapRoma(romajidic, kana_char, 1024, 
		    yc->romaji_buffer + yc->rStartp, yc->rCurs - yc->rStartp,
		    getCurrentJishu(yc) | RK_FLUSH, &m); 
                   /* ���޻��򥫥ʤ��Ѵ����ڤäƤ��ʤ���ʬ��
                      ���ä��顢������Ѵ����Ƥ�� */
    }
    else {
      m = -(n = (yc->rCurs - yc->rStartp) ? 1 : 0);
      strncpy(kana_char, yc->romaji_buffer + yc->rStartp, n);
    }

    if (n < 0) {
      necKanjiError = "RkMapRoma returns -1";
      return -1;
    }

    yc->rStartp += n;
  
    if (m < 0) {
      henkanflag = 0;
      m = -m;
    }
    else {
      henkanflag = HENKANSUMI;
    }

    kanaReplace(yc->kRStartp - yc->kCurs/* ��: ��ˤʤ�ޤ� */,
		/* ��������β��Ρ���ʸ���� .. */
		kana_char, m,
		/* kana_char mʸ�����֤������� .. */
		henkanflag);
    yc->kRStartp += m;
  }

  ret    = yc->kEndp;    /* ���η�̤����δؿ����֤��ͤˤʤ� */
  if (b) {
    if (bsize > ret) {
      strncpy(b, yc->kana_buffer, ret);
      b[ret] = '\0';
    }
    else {
      strncpy(b, yc->kana_buffer, bsize);
      ret = bsize;
    }
  }
  return ret;
}

#ifdef YOMIKAKUTEIORRETURN
/* ���������å��Ȥ���ޤ���ʤ��褦�ʵ��������ΤǤȤꤢ�����Ϥ�����
   1990.9.21 kon */

static
YomiKakuteiOrReturn(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  if (d->ch != '\n' && d->ch != '\r') {
    return YomiKakutei(d);
  }
  else {
    int i;

    RomajiFlushYomi(d, NULL, 0);
    for (i = 0 ; i < yc->kEndp ; i++) {
      if ( !(yc->kAttr[i] & HENKANSUMI) ) {
        int ret;

        if (yc->rEndp < d->bytes_buffer) {
          strncpy(d->buffer_return, yc->romaji_buffer, yc->rEndp);
          d->buffer_return[yc->rEndp] = d->ch;
          ret = yc->rEndp + 1;
        }
        else {
          strncpy(d->buffer_return, yc->romaji_buffer, d->bytes_buffer);
          ret = d->bytes_buffer;
        }
        RomajiClearYomi(d);
        d->kanji_status_return->length = 0;
	d->current_mode = &empty_mode;
        AlphaMode(d);
        return ret;
      }
    }
    return YomiKakutei(d);
  }
}
#endif /* YOMIKAKUTEIORRETURN */

static
YomiKakutei(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  d->kanji_status_return->length = 0;
  d->nbytes = RomajiFlushYomi(d, d->buffer_return, d->bytes_buffer);
  { /* �����ɤߤ�����֤������򤹤� */
    extern yomiInfoLevel;
    int n;

    n = d->nbytes;
    if (yomiInfoLevel > 0 && n < d->bytes_buffer) {
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
  return YomiExit(d, d->nbytes);
}

YomiExit(d, retval)
uiContext d;
int retval;
{
  extern KanjiModeRec empty_mode;

  RomajiClearYomi(d);

  /* ���ꤷ�Ƥ��ޤä��顢�ɤߤ��ʤ��ʤ�ΤǦե⡼�ɤ����ܤ��롣 */
  d->current_mode = &empty_mode;
  d->kanji_status_return->info |= KanjiEmptyInfo;

  return checkIfYomiExit(d, retval);
}

static
clearYomiContext(yc)
yomiContext yc;
{
  yc->rStartp = 0;
  yc->rCurs = 0;
  yc->rEndp = 0;
  yc->romaji_buffer[0] = (unsigned char)NULL;
  yc->rAttr[0] = SENTOU;
  yc->kRStartp = 0;
  yc->kCurs = 0;
  yc->kEndp = 0;
  yc->kana_buffer[0] = (unsigned char)NULL;
  yc->kAttr[0] = SENTOU;
}

/* RomajiClearYomi(d) �桼�ƥ���ƥ��ؿ�
 *
 * ���δؿ��ϡ�(uiContext)d ���ߤ����Ƥ����ɤߤξ��� 
 * �򥯥ꥢ���롣
 *
 * �ں��ѡ�   
 *
 *    �ɤߤ򥯥ꥢ���롣
 *
 * �ڰ�����
 *
 *    d  (uiContext)  ���ʴ����Ѵ���¤��
 *
 * ������͡�
 *
 *    �ʤ���
 *
 * �������ѡ�
 *
 *    yc->rEndp = 0;
 *    yc->kEndp = 0; ��
 */

RomajiClearYomi(d)
uiContext d;
{
  clearYomiContext((yomiContext)d->modec);
#ifdef CALLBACK
  printf("�ץꥨ�ǥ��åȤ򥯥ꥢ\n");
#endif /* CALLBACK */
}

/* RomajiStoreYomi(d, kana) �桼�ƥ���ƥ��ؿ�
 *
 * ���δؿ��ϡ�(uiContext)d ���ɤߤξ���򥹥ȥ����롣
 *
 * �ں��ѡ�   
 *
 *    �ɤߤ��Ǽ���롣
 *
 * �ڰ�����
 *
 *    d    (uiContext)  ���ʴ����Ѵ���¤��
 *    kana (unsigned char *) ����ʸ����
 *    roma (unsigned char *) ���޻�ʸ����
 * ������͡�
 *
 *    �ʤ���
 *
 * �������ѡ�
 *
 *    yc->rEndp = strlen(kana);
 *    yc->kEndp = strlen(kana); ��
 */

RomajiStoreYomi(d, kana, roma)
uiContext d;
unsigned char *kana, *roma;
{
  int i, ylen, rlen;
  yomiContext yc = (yomiContext)d->modec; /*???����äȵ��䤢�� */

  ylen = strlen(kana);

  if (roma) {
    rlen = strlen(roma);
    strcpy(yc->romaji_buffer, roma);
    yc->rStartp = rlen;
    yc->rCurs = rlen;
    yc->rEndp = rlen;
  }
  else {
    strcpy(yc->romaji_buffer, kana);
    rlen = ylen;
    yc->rStartp = rlen;
    yc->rCurs = rlen;
    yc->rEndp = rlen;
  }
  strcpy(yc->kana_buffer, kana);
  yc->kRStartp = ylen;
  yc->kCurs = ylen;
  yc->kEndp = ylen;
  for (i = 0 ; i < rlen ; i++) {
    yc->rAttr[i] = 0;
  }
  yc->rAttr[0] |= SENTOU;
  yc->rAttr[i] = SENTOU;
  for (i = 0 ; i < ylen ; i++) {
    yc->kAttr[i] = HENKANSUMI;
  }
  yc->kAttr[0] |= SENTOU;
  yc->kAttr[i] = SENTOU;
}

/*
  KanaDeletePrevious -- �����ʤȤ�����Ȥ��롣

*/

KanaDeletePrevious(d)/* ��������κ���ʸ���κ�� */
uiContext d;
{
  int howManyDelete, howManyChars;
  int h;
  int i;
  yomiContext yc = (yomiContext)d->modec;

  /* ��������κ�¦��������Τ�������������κ�¦��

    (1) ���ܸ�ʸ����λ���
    (2) ���Х���ʸ����λ���
    (3) ���޻������Ѵ�������ξ��֤Ǥ��ꡢ����ե��٥åȤˤʤäƤ������
    (4) ��Ƭ�Ǥ���Ȥ�

    �ʤɤ��ͤ����롣(�פ���������Ƥ��ʤ��ΤǤ�äȤ��ꤽ��)
   */

  if (yc->kCurs == 0) { /* ��ü�ΤȤ� */
    d->kanji_status_return->length = -1;
    return 0;
  }
  howManyDelete = howFarToGoBackward(d, &howManyChars);
  if (howManyDelete > 1 && (yc->generalFlags & IROHA_YOMI_BREAK_ROMAN)) {
    yc->generalFlags &= ~IROHA_YOMI_BREAK_ROMAN;
    yc->rStartp--;
    while ( yc->rStartp > 0 && !(yc->rAttr[yc->rStartp] & SENTOU) ) {
      yc->rAttr[yc->rStartp] |= SENTOU;
      yc->rStartp--;
    }
    romajiReplace (-1, NULL, 0, 0);
    yc->kRStartp--;
    while ( yc->kRStartp > 0 && !(yc->kAttr[yc->kRStartp] & SENTOU) )
      yc->kRStartp--;
    kanaReplace(yc->kRStartp - yc->kCurs, 
		yc->romaji_buffer + yc->rStartp,
		yc->rCurs - yc->rStartp,
		SENTOU/* ̤�Ѵ����ᤷ�Ƥ���櫓 */);
    if (forceKana) { /* ;�ä����޻���̵���������ʤˤ��� */
      char kana_char[1024];
      int n, m, henkanflag, jishuflag;

      jishuflag = getCurrentJishu(yc);

      if (romajidic != NULL) {
	n = RkMapRoma(romajidic, kana_char, 1024,
		      yc->romaji_buffer + yc->rStartp,
		      yc->rCurs - yc->rStartp,
		      jishuflag | RK_FLUSH, &m);
      }
      else {
	m = -(n = (yc->rCurs - yc->rStartp) ? 1 : 0);
	strncpy(kana_char, yc->romaji_buffer + yc->rStartp, n);
      }

      if (n > 0) {
	if (m < 0) {
	  henkanflag = 0;
	  m = -m; /* |m| ���Ѵ����줿ʸ�����Ĺ���򼨤� */
	}
	else {
	  henkanflag = HENKANSUMI;
	}
      
	for (i = 1 ; i < n ; i++) {
	  yc->rAttr[yc->rStartp + i] &= ~SENTOU;      /* ��Ƭ�ʳ��ᣰ */
	}
	yc->rStartp += n;      /* n ʸ��ʬ���� */

	kanaReplace(yc->kRStartp - yc->kCurs/* ��: ��ˤʤ�ޤ� */, 
		    kana_char, m, henkanflag);
	yc->kRStartp = yc->kCurs;
	if (yc->rCurs - yc->rStartp) {
	  kanaReplace(yc->kRStartp - yc->kCurs/* ��: ��ˤʤ�ޤ� */, 
		      yc->romaji_buffer + yc->rStartp,
		      yc->rCurs - yc->rStartp,
		      SENTOU);
	}
	yc->rStartp -= n;
	yc->kRStartp -= m;
      }
    }
  }
  else {
    if (yc->kAttr[yc->kCurs - howManyDelete] & SENTOU) { 
      /* ���޻������Ѵ�����Ƭ���ä��� */
      int n;

      /* ��Ƭ���ä�����޻�����Ƭ�ޡ�����Ω�äƤ���Ȥ���ޤ��᤹ */

      for (n = 1 ; yc->rCurs > 0 && !(yc->rAttr[--yc->rCurs] & SENTOU) ; n++)
	;
      moveStrings(yc->romaji_buffer, yc->rAttr, yc->rCurs + n, yc->rEndp, -n);
      if (yc->rCurs < yc->rStartp) {
	yc->rStartp = yc->rCurs;
      }
      yc->rEndp -= n;
    }
    kanaReplace(-howManyDelete, NULL, 0, 0);
  }
}

static
YomiDeletePrevious(d)
uiContext d;
{
  extern KanjiModeRec empty_mode;
  yomiContext yc = (yomiContext)d->modec;

  KanaDeletePrevious(d);
  makeYomiReturnStruct(d);

  if (yc->kEndp == 0) {
    /* ̤����ʸ���������ʤ��ʤä��Τʤ顢�ե⡼�ɤ����ܤ��� */
    d->current_mode = &empty_mode;
    d->kanji_status_return->info |= KanjiEmptyInfo;
  }
  return 0;
}

static
YomiDeleteNext(d)/* ����������ʸ���κ�� */
uiContext d;
{
  extern KanjiModeRec empty_mode;
  int howManyDelete, howManyChars;
  int f;
  int h;
  int i;
  yomiContext yc = (yomiContext)d->modec;

  if (yc->kCurs == yc->kEndp) {
    /* ��ü������ʤˤ⤷�ʤ��ΤǤ��礦�ͤ� */
    d->kanji_status_return->length = -1;
    return 0;
  }

  howManyDelete = howFarToGoForward(d, &howManyChars);

  if (yc->kAttr[yc->kCurs] & SENTOU) {
    int n = 1;
    while ( !(yc->rAttr[++yc->rCurs] & SENTOU) )
      n++;
    moveStrings(yc->romaji_buffer, yc->rAttr, yc->rCurs, yc->rEndp, -n);
    yc->rCurs -= n;
    yc->rEndp -= n;
  }
  kanaReplace(howManyDelete, NULL, 0, 0);

  makeYomiReturnStruct(d);

  if (yc->kEndp == 0) {
    /* ̤����ʸ���������ʤ��ʤä��Τʤ顢�ե⡼�ɤ����ܤ��� */
    d->current_mode = &empty_mode;
    d->kanji_status_return->info |= KanjiEmptyInfo;
  }
  return 0;
}

static
YomiKillToEndOfLine(d)  /* �������뤫�鱦�Τ��٤Ƥ�ʸ���κ�� */
uiContext d;
{
  extern KanjiModeRec empty_mode;
  yomiContext yc = (yomiContext)d->modec;

  romajiReplace (yc->rEndp - yc->rCurs, NULL, 0, 0);
  kanaReplace   (yc->kEndp - yc->kCurs, NULL, 0, 0);

  makeYomiReturnStruct(d);

  if (yc->kEndp == 0) {
    /* ̤����ʸ���������ʤ��ʤä��Τʤ顢�ե⡼�ɤ����ܤ��� */
    d->current_mode = &empty_mode;
    d->kanji_status_return->info |= KanjiEmptyInfo;
  }
  return 0;
}

static
YomiQuit(d)/* �ɤߤμ��ä� */
uiContext d;
{
  extern KanjiModeRec empty_mode;

  /* ̤����ʸ����������� */
  RomajiClearYomi(d);

  makeYomiReturnStruct(d);

  /* ̤����ʸ���������ʤ��ʤä��Τǡ��ե⡼�ɤ����ܤ��� */
  d->current_mode = &empty_mode;
  d->kanji_status_return->info |= KanjiEmptyInfo;

  return checkIfYomiQuit(d, 0);
}

RomajiCursorFlush(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  yc->kRStartp = yc->kCurs = yc->kEndp;
  yc->rStartp  = yc->rCurs = yc->rEndp;
}

/* 
 generalReplace -- ���ʥХåե��ˤ���޻��Хåե��ˤ�Ȥ����ִ��롼����

  �����ִ��롼�����ʸ����Υ������ִ���Ԥ�����Υ饤�֥��롼��
  ��Ǥ��롣������ʸ������ݻ����Ƥ������Ȥߤϼ��Τ褦�ˤʤäƤ���
  ��ΤȤ��롣

    ��ʸ�����ѤΥХåե�
    ��ʸ����°���ѤΥХåե�
    ����������(����ǥå���(�ݥ��󥿤ǤϤʤ�))
    ��ʸ����ν�����ؤ�����ǥå���
    ��ɬ�����������꺸��¸�ߤ��륤��ǥå���(̤�Ѵ�ʸ���ؤΥ���ǥ�
      �����˻Ȥä��ꤹ��)

  �嵭�˼������Хåե���Υ���������������λ��ꤵ�줿Ĺ����ʸ����
  ���̤˻��ꤵ���ʸ������֤�����������򤹤롣

  �ȡ�����ΥХ��ȿ����Ѳ��������ʸ����ν�����ؤ�����ǥå�����
  �ͤ��Ѳ������롣�ޤ������������������ʬ���Ф���ʸ������ִ���Ԥ���
  ��ˤϥ�������ݥ��������ͤ��Ѳ������롣����������Ѳ���������̡�
  ̤�Ѵ�ʸ�����ؤΥ���ǥå������⾮�����ʤä����ˤϡ�̤�Ѵ�ʸ����
  �ؤΥ���ǥå������ͤ򥫡�������ͤ˹�碌�ƾ��������롣

  ���δؿ��κǽ������ˤϿ�������������ʸ�����°���˴ؤ���ҥ�Ȥ�����
  �Ǥ��롣���������������ʸ������Ф��ơ���Ƭʸ���ˤ� SENTOU ��Ϳ����
  �줿�ҥ�ȤΣϣҤ��ä��ͤ�Ϳ����졢����¾��ʸ���ˤϥҥ�Ȥ�Ϳ����
  �줿�ͤ���Ǽ����롣

  �ڰ�����
     buf      �Хåե��ؤΥݥ���
     attr     °���Хåե��ؤΥݥ���
     startp   �Хåե���̤����ʸ����ʤɤؤΥ���ǥå��������Ƥ�����
              ���ؤΥݥ���
     cursor   ����������֤����Ƥ����ѿ��ؤΥݥ���
     endp     ʸ����κǽ����֤�ؤ������Ƥ����ѿ��ؤΥݥ���

     bytes    ���Х����ִ����뤫����ο������ꤵ���ȥ��������������
              ʬ�� |bytes| ʬ��ʸ�����ִ����оݤȤʤꡢ���ο�������
              �����ȥ�������θ�����ʬ�� bytes ʬ��ʸ�����оݤ�
              �ʤ롣
     rplastr  �������֤�ʸ����ؤΥݥ���
     len      �������֤�ʸ�����Ĺ��
     attrmask �������֤�ʸ�����°���Υҥ��

  �ºݤˤϤ��δؿ���ľ�ܤ˻Ȥ鷺�ˡ�bytes, rplastr, len, attrmask ����
  ��Ϳ��������Ǥ���ޥ���kanaReplace, romajiReplace ��Ȥ��Τ��ɤ���
*/

generalReplace(buf, attr, startp, cursor, endp, bytes, rplastr, len, attrmask)
char *buf, *attr, *rplastr;
int *startp, *cursor, *endp,  bytes, len, attrmask; 
{ 
  int idou, begin, end, i; 
  int cursorMove;

  if (bytes > 0) {
    cursorMove = 0;
    begin = *cursor;
    end = *endp;
  }
  else {
    bytes = -bytes;
    cursorMove = 1;
    begin = *cursor - bytes;
    end = *endp;
  }

  idou = len - bytes;

  moveStrings(buf, attr, begin + bytes, end, idou);
  *endp += idou;
  if (cursorMove) {
    *cursor += idou;
    if (*cursor < *startp)
      *startp = *cursor;
  }

  strncpy(buf + begin, rplastr, len);
  for (i = 0 ; i < len ; i++) {
    attr[begin + i] = attrmask;
  }
  if (len)
    attr[begin] |= SENTOU;
}

YomiQuotedInsert(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec, GetKanjiString();

  if (yc->generalFlags & IROHA_YOMI_CHGMODE_INHIBITTED) {
    return NothingChangedWithBeep(d);
  }    

  RomajiFlushYomi(d, NULL, 0);
  yomiQuotedInsertMode(d);
  d->kanji_status_return->length = -1;
  return 0;
}

static
YomiInsertQuoted(d)
uiContext d;
{
  char ch;
  coreContext cc = (coreContext)d->modec;
  yomiContext yc;

  ch = d->buffer_return[0];

  if (IrohaFunctionKey(ch)) {
    d->kanji_status_return->length = -1;
    d->kanji_status_return->info = 0;
    return 0;
  }
  else {
    d->current_mode = cc->prevMode;
    d->modec = cc->next;
    free(cc);

    yc = (yomiContext)d->modec;
    if (ch & 0x80) {
      char *p = (char *)d->buffer_return;
      int i;

      for (i = d->nbytes ; i >= 0 ; i--) {
	p[i + 1] = p[i];
      }
      p[0] = 0x8e; /* SS2 */
      d->nbytes += 1;
    }

    romajiReplace (0, d->buffer_return, d->nbytes, 0);
    kanaReplace   (0, d->buffer_return, d->nbytes, HENKANSUMI);
    yc->rStartp = yc->rCurs;
    yc->kRStartp = yc->kCurs;
    makeYomiReturnStruct(d);
    currentModeInfo(d);
    d->status = EXIT_CALLBACK;
    return 0;
  }
}

/* ConvertAsHex -- �����ʤȤߤʤ��Ƥ��Ѵ� 

  ���޻����Ϥ����ȿžɽ������Ƥ���ʸ����򣱣��ʤ�ɽ������Ƥ��륳���ɤ�
  �ߤʤ����Ѵ����롣

  (MSB�ϣ��Ǥ⣱�Ǥ��ɤ�)

  */

static
ConvertAsHex(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  if (yc->henkanInhibition & IROHA_YOMI_INHIBIT_ASHEX) {
    return NothingChangedWithBeep(d);
  }
  if (convertAsHex(d)) {
    d->kanji_status_return->length = 0;
    return YomiExit(d, 2);
  }
  else {
    return NothingChangedWithBeep(d);
  }
}

/*
  convertAsHex  �����ʤο��������ʸ�����Ѵ�

  ���������Ū�˻��Ѥ��뤿��Υ롼����Ǥ��롣d->romaji_buffer �˴ޤ�
  ���ʸ����򣱣��ʤ�ɽ���줿���������ɤǤ���Ȥߤʤ��ơ����Υ����ɤ�
  ��ä�ɽ����������ʸ�����Ѵ����롣�Ѵ�����ʸ����� buffer_return 
  �˳�Ǽ���롣�꥿�����ͤϥ��顼���ʤ���� buffer_return �˳�Ǽ����ʸ
  �����Ĺ���Ǥ���(�̾�ϣ��Ǥ���)�����顼��ȯ�����Ƥ�����ϡݣ�����Ǽ
  ����롣

  �⡼�ɤ��ѹ����ν����Ϥ��δؿ��ǤϹԤ��ʤ���

  �ޤ��Хåե��Υ��ꥢ�ʤɤ�Ԥ�ʤ��Τ���դ���٤��Ǥ��롣

  <�����>
    �����������ʤ��Ѵ��Ǥ������ϣ������Ǥʤ����ϣ����֤롣
*/

convertAsHex(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  return cvtAsHex(d, d->buffer_return, yc->romaji_buffer, yc->rEndp);
}

cvtAsHex(d, buf, hexbuf, hexlen)
uiContext d;     
unsigned char *buf, *hexbuf;
int hexlen;
{
  int i;
  unsigned char rch;
  
  if (hexlen != 4) { /* ���Ϥ��줿ʸ�����Ĺ������ʸ���Ǥʤ��ΤǤ�����Ѵ�
			���Ƥ����ʤ� */
    d->kanji_status_return->length = -1;
    return 0;
  }
  for (i = 0 ; i < 4 ; i++) {
    rch = hexbuf[i]; /* �ޤ���ʸ�����Ф��������ʤο����ˤ��롣 */
    if ('0' <= rch && rch <= '9') {
      rch -= '0';
    }
    else if ('A' <= rch && rch <= 'F') {
      rch -= 'A' - 10;
    }
    else if ('a' <= rch && rch <= 'f') {
      rch -= 'a' - 10;
    }
    else {
      d->kanji_status_return->length = -1;
      return 0;
    }
    buf[i] = rch; /* ��괺������¸���Ƥ��� */
  }
  buf[0] = 0x80 | (buf[0] * 16 + buf[1]); /* ��¸���Ƥ����ͤ򸵤˴����ˤ��� */
  buf[1] = 0x80 | (buf[2] * 16 + buf[3]);
  if (buf[0] < 0x00a1 || 0x00fe < buf[0]
      || buf[1] < 0x00a1 || 0x00fe < buf[1]) {
    return 0;
  }
  else {
    return 1;
  }
}

/*
 */

static
YomiHenkanOrSpace(d)
uiContext d;
{
  extern KanjiModeRec empty_mode;
  yomiContext yc = (yomiContext)d->modec;

  if (d->ch != ' ') {
    return YomiHenkan(d);
  }
  else {
    int i;

    RomajiFlushYomi(d, NULL, 0);
    for (i = 0 ; i < yc->kEndp ; i++) {
      if ( !(yc->kAttr[i] & HENKANSUMI) ) {
        int ret;

        if (yc->rEndp < d->bytes_buffer) {
          strncpy(d->buffer_return, yc->romaji_buffer, yc->rEndp);
          d->buffer_return[yc->rEndp] = ' ';
          ret = yc->rEndp + 1;
        }
        else {
          strncpy(d->buffer_return, yc->romaji_buffer, d->bytes_buffer);
          ret = d->bytes_buffer;
        }
        RomajiClearYomi(d);
        d->kanji_status_return->length = 0;
	d->current_mode = &empty_mode;
        AlphaMode(d);
        return ret;
      }
    }
    return YomiHenkan(d);
  }
}

static
_yomiHenkanExit(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  d->nbytes = retval;
  retval = YomiExit(d, retval);
  currentModeInfo(d);
  return retval;
}

static
_yomiHenkanQuit(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  makeYomiReturnStruct(d);
  currentModeInfo(d);
  return retval;
}

/*
 * ���ʴ����Ѵ���Ԥ�(�Ѵ����������Ʋ����줿)��TanKouhoMode�˰ܹԤ���
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
YomiHenkan(d)
uiContext	d;
{
  yomiContext yc = (yomiContext)d->modec;
  henkanContext hc, pushHenkanMode();
  int len;
  extern kouho_threshold;
#ifdef MEASURE_TIME
  struct tms timebuf;
  long   currenttime, times();

  currenttime = times(&timebuf);
#endif

  if (yc->henkanInhibition & IROHA_YOMI_INHIBIT_HENKAN) {
    return NothingChangedWithBeep(d);
  }

  len = RomajiFlushYomi(d, d->genbuf, ROMEBUFSIZE);
  RomajiCursorFlush(d);

  if (yc->henkanCallback) {
    return yc->henkanCallback(d, IROHA_FN_Henkan, d->genbuf, len);
  }

  /* TanKouhoMode(ñ����ɽ���⡼��)�˰ܹԤ��� */
  hc = pushHenkanMode(d, d->genbuf, len, yc,
		      0, _yomiHenkanExit, _yomiHenkanQuit, 0);
  if (hc == 0) {
    return -1;
  }
  hc->minorMode = IROHA_MODE_TankouhoMode;
  hc->kouhoCount = 1;

  if (henkan(d, 0) < 0) {
    popHenkanMode(d);
    popCallback(d);
    return -1;
  }

  if (kouho_threshold > 0 && hc->kouhoCount >= kouho_threshold) {
    return tanKouhoIchiran(d, 0);
  }

  currentModeInfo(d);

#ifdef MEASURE_TIME
  hc->proctime = times(&timebuf);
  hc->proctime -= currenttime;
#endif

  return 0;
}

static
yomiChangeMode(d, mode)
uiContext d;
int mode;
{
  yomiContext yc = (yomiContext)d->modec;

  if (yc->generalFlags & IROHA_YOMI_CHGMODE_INHIBITTED) {
    return NothingChangedWithBeep(d);
  }

  RomajiFlushYomi(d, NULL, 0);
  yc->baseChar = mode;
  if (mode == JISHU_HIRA) {
    yc->majorMode = yc->minorMode = IROHA_MODE_HenkanMode;
  }
  else {
    yc->majorMode = yc->minorMode = IROHA_MODE_ZenKataHenkanMode + mode - 1;
  }
  
  makeYomiReturnStruct(d);
  currentModeInfo(d);
  return 0;
}

static
YomiHankakuRomajiMode(d)
uiContext d;
{
  return yomiChangeMode(d, JISHU_HAN_ALPHA);
}

static
YomiZenkakuRomajiMode(d)
uiContext d;
{
  return yomiChangeMode(d, JISHU_ZEN_ALPHA);
}

static
YomiHankakuKatakanaMode(d)
uiContext d;
{
  if (InhibitHankakuKana)
    return NothingChangedWithBeep(d);
  else
    return yomiChangeMode(d, JISHU_HAN_KATA);
}

static
YomiZenkakuKatakanaMode(d)
uiContext d;
{
  return yomiChangeMode(d, JISHU_ZEN_KATA);
}

static
YomiHiraganaMode(d)
uiContext d;
{
  return yomiChangeMode(d, JISHU_HIRA);
}

static
YomiNextMode(d)
uiContext d;
{
  int base;
  yomiContext yc = (yomiContext)d->modec;

  base = yc->baseChar;
  do {
    base++;
    if (base >= MAX_JISHU) {
      base = JISHU_HIRA;
    }
  } while (base == JISHU_HAN_KATA && InhibitHankakuKana);
  return yomiChangeMode(d, base);
}

static
YomiPrevMode(d)
uiContext d;
{
  int base;
  yomiContext yc = (yomiContext)d->modec;

  base = yc->baseChar;
  do {
    base--;
    if (base < 0) {
      base = MAX_JISHU - 1;
    }
  } while (base == JISHU_HAN_KATA && InhibitHankakuKana);
  return yomiChangeMode(d, base);
}
  

#include "yomimap.c"
