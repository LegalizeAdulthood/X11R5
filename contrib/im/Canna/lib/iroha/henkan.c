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
    printf("%14s %-20s ����ꤷ�Ƥ��ޤ���\n", s, buf);
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
static char *mountErrorMessage = "��ޥ���ȤǤ��ޤ���Ǥ���";

int kouho_threshold = 2;

static int changeCurBunsetsuString(), doYomiHenkan(), kanakanError();
static int bubunEveryTimeCatch(), quitBubun(), exitBubun();

#define  SENTOU        1

/*
 * BunsetsuKugiri (ʸ��ζ褮���Ⱦ�Ѷ���ˤ���ե饰) ������
 *
 * ������	�ʤ�
 * �����	�ʤ�
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
    necKanjiError = "irohaserver�˥��ͥ��ȤǤ��ޤ���Ǥ���";
  }
  else {
    necKanjiError = "���ʴ����Ѵ�����ν�����˼��Ԥ��ޤ���";
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
 * ���ʴ����Ѵ��Τ���ν������
 *
 * ��RkInitialize��Ƥ�ǡ�defaultContext ���������
 * ��defaultBushuContext ���������
 * ������Υ������ѥ������ꤹ��
 * �������ƥ༭�������Ѽ��񡢥桼�������ޥ���Ȥ���
 *
 * ������	�ʤ�
 * �����	0:�ޤ���� -1:�Ȥ��Ȥ�����
 */
KanjiInit()
{
  int i;
  char **p;
  char *ptr, *getenv();

  /* Ϣʸ��饤�֥����������� */

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
      necKanjiError = "�����ѤΥ���ƥ����Ȥ�����Ǥ��ޤ���Ǥ���";
      addWarningMesg(necKanjiError);
      defaultContext = -1;
      RkFinalize();
      return -1;
    }
  } else {
    defaultBushuContext = -1;
  }

  debug_message("�ǥե���ȥ���ƥ�����(%d), ���󥳥�ƥ�����(%d)\n",
		defaultContext, defaultBushuContext, 0);

  if (defaultContext != -1) {
    if((ptr = getenv("IROHADICPATH")) == (char *)NULL) {
      if((RkSetDicPath(defaultContext, "iroha")) == -1) {
	necKanjiError = "����Υǥ��쥯�ȥ������Ǥ��ޤ���Ǥ���";
	RkFinalize();
	return(NG);
      }
      if((RkSetDicPath(defaultBushuContext, "iroha")) == -1) {
	necKanjiError = "����Υǥ��쥯�ȥ������Ǥ��ޤ���Ǥ���";
	RkFinalize();
	return(NG);
      }
    }
    else {
      if((RkSetDicPath(defaultContext, ptr)) == -1) {
	necKanjiError = "����Υǥ��쥯�ȥ������Ǥ��ޤ���Ǥ���";
	RkFinalize();
	return(NG);
      }
      if((RkSetDicPath(defaultBushuContext, ptr)) == -1) {
	necKanjiError = "����Υǥ��쥯�ȥ������Ǥ��ޤ���Ǥ���";
	RkFinalize();
	return(NG);
      }
    }

    /* �����ƥ༭��Υޥ���� */
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
      dicMesg("�����ƥ༭��", *p, 1);
    }
    
    /* �����Ѽ���Υޥ���� */
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
      dicMesg("���󼭽�", *p, 1);
    }
    
    /* �桼������Υޥ���� */
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
      dicMesg("�桼������", *p, 0);
    }

    /* Ϣ�켭��Υޥ���� */
    if (RengoGakushu) {
      if(RkMountDic(defaultContext, RengoGakushu, 0) == -1) {
	mountError(RengoGakushu);
      }
      dicMesg("Ϣ�켭��", RengoGakushu, 0);
    }
  }
  return(0);
}

/*
 * ���ʴ����Ѵ��Τ���θ����
 *
 * �������ƥ༭�������Ѽ��񡢥桼������򥢥�ޥ���Ȥ���
 * ��RkFinalize��Ƥ�
 *
 * ������	�ʤ�
 * �����	�ʤ�
 */
KanjiFin()
{
  int i;
  char buf[256];

  for (i = 0 ; i < nkanjidics ; i++) {
    /*
    if(RkUnmountDic(defaultContext, kanjidicname[i]) == -1) {
      sprintf(buf, "%s �򥢥�ޥ���ȤǤ��ޤ���Ǥ�����", kanjidicname[i]);
      addWarningMesg(buf);
    }
    */
    free(kanjidicname[i]);
  }

  for (i = 0 ; i < nbushudics ; i++) {
    /*
    if(RkUnmountDic(defaultBushuContext, bushudicname[i]) == -1) {
      sprintf(buf, "%s �򥢥�ޥ���ȤǤ��ޤ���Ǥ�����", bushudicname[i]);
      addWarningMesg(buf);
    }
    */
    free(bushudicname[i]);
  }

  for (i = 0 ; i < nuserdics ; i++) {
    /*
    if(RkUnmountDic(defaultContext, userdicname[i]) == -1) {
      printf(buf, "%s �򥢥�ޥ���ȤǤ��ޤ���Ǥ�����", userdicname[i]);
      addWarningMesg(buf);
    }
    */
    free(userdicname[i]);
  }

  for (i = 0 ; i < nlocaldics ; i++) {
    /*
    if(RkUnmountDic(defaultContext, localdicname[i]) == -1) {
      printf(buf, "%s �򥢥�ޥ���ȤǤ��ޤ���Ǥ�����", localdicname[i]);
      addWarningMesg(buf);
    }
    */
    free(localdicname[i]);
  }

  if(RengoGakushu)
    free(RengoGakushu);

  if(KatakanaGakushu)
    free(KatakanaGakushu);

  /* Ϣʸ��饤�֥���λ������ */
  RkFinalize();

  return(0);
}

/*
 * ��������ʸ����κ���
 *
 * ����ʸ�����Ƭ����ΰ��֤� kugiri �˥����֤��ʤ���
 *   d->echo_buffer �˥������Ѥ�ʸ�������
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
makeEchoStr(d)
uiContext        d;
{
  int  i, index;
  unsigned char *ptr;
  henkanContext hc = (henkanContext)d->modec;

  ptr = hc->echo_buffer;
  /* ��Ƭ��ʸ�ᤫ�����Ѵ���̤����ơ��������ѤΥХåե�������� */
  /* ��ʸ��ζ褮���ȤäƤ���(��Ƭ����ΥХ��ȿ�) */
  for(i=0, index=0; i < hc->nbunsetsu; i++) {
    if(RkGoTo(hc->context, i) == -1) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "ʸ��ΰ�ư�˼��Ԥ��ޤ���";
      return(NG);
    }
    hc->kugiri[i] = index;
    if(RkGetKanji(hc->context, (ptr + index), (ROMEBUFSIZE - index)) < 0) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "�����ȸ������Ф��ޤ���Ǥ���";
      return(NG);
    }
    index += strlen(ptr + index);
    if(BunsetsuKugiri && ((i + 1) < hc->nbunsetsu)) {
      *(ptr + index) = (unsigned char)' ';
      index++;
    }
  }  
  hc->kugiri[i] = index; /* �����ΰ��� */
  hc->kugiri[i+1] = (int)NULL;

  /* ������ʸ����᤹ */
  if(RkGoTo(hc->context, hc->curbun) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "������ʸ��ؤΰ�ư�˼��Ԥ��ޤ���";
    return(NG);
  }

  return(0);
}

/*
 * ������ʸ��θ���򹹿�����
 *
 * �����Ǥˤ��� d->echo_buffer �Τ����Υ�����ʸ�����ʬ���ѹ�����
 * ������ˤȤ�ʤ� kugiri �⹹�������
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
makeEchoStrCurChange(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;
  unsigned char tmpbuf[ROMEBUFSIZE];

  /* �����ȸ�������� */
  if(RkGetKanji(hc->context, tmpbuf, ROMEBUFSIZE) < 0) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "�����ȸ������Ф��ޤ���Ǥ���";
    return(NG);
  }
  changeCurBunsetsuString(hc, tmpbuf, strlen(tmpbuf));
  return 0;
}

/*
 * ������ʸ�����ꤵ�줿ʸ����˹�������
 *
 * �����Ǥˤ��� d->echo_buffer �Τ����Υ�����ʸ�����ʬ���ѹ�����
 * ������ˤȤ�ʤ� kugiri �⹹�������
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1(����Ϥʤ�)
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

  /* �쥫����ʸ��ΥХ��ȿ� */
  old_klen = hc->kugiri[hc->curbun + 1] - hc->kugiri[hc->curbun];
  if(BunsetsuKugiri && (hc->curbun != (hc->nbunsetsu - 1)))
    old_klen -= 1;

  /* �����ȸ�������� */
  /* ������ʸ���Ĺ��ʬ�����뤿��ˡ�
     ������ʸ��μ���ʸ��ʹߤ򤺤餹 */
  i = hc->kugiri[hc->nbunsetsu] - hc->kugiri[hc->curbun + 1];
  /* ���餹ʸ����Ĺ */ 

  if(len > old_klen) { /* ���ˤ��餹*/
    j = hc->kugiri[hc->nbunsetsu]; /* NULL �⥳�ԡ�����*/
    k = len - old_klen;
    for(; i>=0; i--, j--) /* NULL �⥳�ԡ�����*/
      hc->echo_buffer[j + k] = hc->echo_buffer[j];
  }else if(len < old_klen) { /* ���ˤ��餹*/
    j = hc->kugiri[hc->curbun + 1];
    k = old_klen - len;
    for(; i>=0; i--, j++) /* NULL �⥳�ԡ�����*/
      hc->echo_buffer[j - k] = hc->echo_buffer[j];
  }
  /* �����ȸ����������Ȥ��������� */ 
  ret = hc->kugiri[hc->curbun];
  strncpy(ptr + ret, str, len);
  if(BunsetsuKugiri && (hc->curbun != (hc->nbunsetsu - 1)))
    *(ptr + hc->kugiri[hc->curbun] + len) = (unsigned char)' ';

  /* ��ʸ��ζ褮���ȤäƤ���(��Ƭ����ΥХ��ȿ�) */
  /* ������ʸ��ʹߺ�ʬ�򤿤� */
  for(i=hc->curbun; i<hc->nbunsetsu; i++)
    hc->kugiri[i + 1] += len - old_klen;

  return ret;
}

/*
 * ������ʸ��ʹߤ򹹿�����
 * �����Ǥˤ��� d->echo_buffer �Τ����Υ�����ʸ��ʹߤ��ѹ�����
 * ������ˤȤ�ʤ� kugiri �⹹�������
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
makeEchoStrCurUnderChange(d)
uiContext	d;
{
  int  i, index;
  unsigned char *ptr;
  henkanContext hc = (henkanContext)d->modec;

  ptr = hc->echo_buffer;
  /* ������ʸ�ᤫ�����Ѵ���̤����ơ��������ѤΥХåե�������� */
  /* ��ʸ��ζ褮���ȤäƤ���(��Ƭ����ΥХ��ȿ�) */
  for(i=hc->curbun, index=hc->kugiri[hc->curbun]; i < hc->nbunsetsu; i++) {
    if(RkGoTo(hc->context, i) == -1) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "ʸ��ΰ�ư�˼��Ԥ��ޤ���";
      return(NG);
    }
    hc->kugiri[i] = index;
    if(RkGetKanji(hc->context, (ptr + index), (ROMEBUFSIZE - index)) < 0) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "�����ȸ������Ф��ޤ���Ǥ���";
      return(NG);
    }
    index += strlen(ptr + index);
    if(BunsetsuKugiri && ((i + 1) < hc->nbunsetsu)) {
      *(ptr + index) = (unsigned char)' ';
      index++;
    }
  }  
  hc->kugiri[i] = index; /* �����ΰ��� */
  hc->kugiri[i+1] = (int)NULL;

  /* ������ʸ����᤹ */
  if(RkGoTo(hc->context, hc->curbun) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "������ʸ��ؤΰ�ư�˼��Ԥ��ޤ���";
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
      necKanjiError = "���꤬­�ꤺ�����Ѵ��Ǥ��ޤ���Ǥ���";
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
/* JISHU_HIRA���ʤɤΥޥ�����ͤ��Ѥ�ä��Ȥ��ˤϾ��������Ѥ��ʤ��Ƥ�
   �ʤ�ʤ� */

/*
 * ������ʸ��θ����Ҥ餬��/�������ʤˤ���
 *
 * �����Ǥˤ��� d->echo_buffer �Τ����Υ�����ʸ�����ʬ���ѹ�����
 * ������ˤȤ�ʤ� kugiri �⹹�������
 *
 * ������	uiContext, which �ϻ����ɽ����JISHU_HIRA, JISHU_ZEN_KATA
 *                                             JISHU_HAN_KATA �Τ�����
 *                          �ɤ줫�Ǥʤ���Фʤ�ʤ����ϰϤΥ����å��ʤ���
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
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

  /* �Ҥ餬�ʤ򥫥��ȸ���Ȥ��� */
  if (hc->kanaKugiri
      && ((yepos = hc->kanaKugiri[hc->curbun + 1]) >= 0)) {
    /* ���� RkGetYomi �򤷤Ƥ����餽�η�̤�Ȥ� */
    yspos = hc->kanaKugiri[hc->curbun];
    strncpy(tmpbuf2, hc->ycx->kana_buffer + yspos, yepos - yspos);
    tmpbuf2[yepos - yspos] = '\0';
  }
  else if (RkGetYomi(hc->context, tmpbuf2, ROMEBUFSIZE) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "�ɤߤ���Ф��ޤ���Ǥ���";
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
  �ɤߤ�Ĺ���򥫥���Ȥ������������Ƥ���

  ����Ϥ����ѤΤ�Τȥ��޻��ѤΤ�Τ����ꡢ���줾�졢

  ��ʸ���ܤγ��ϰ��֡���ʸ���ܤγ��ϰ��֡���ʸ���ܤγ��ϰ���.....

  �Τ褦�ˤʤäƤ��롣

  ����γ����ǤϺǽ� -1 �˽��������롣���ʤ����-1 �ˤʤäƤ�����ʬ��
  �ɤߤ�Ĺ�����ޤ�������Ȥ���Ƥ��ʤ���ʬ�Ǥ��롣

  ����: d                uiContext
        start_pos_return d->curbun ���ɤߤ��Ϥޤ����

  �꥿������
        ���ʳ�           �ɤߤ�Ĺ��
        ��               ���顼

  ���顼�ϼ��ξ��ͤ����롣

   ��yc->kana_buffer ���ɤߤ� hc->yomi_buffer ���������ʤ�(��������Ѵ����ʤ�)
   �����ڤ������Ƥ������� alloc �Ǥ��ʤ��ä���
   ��RkGoTo �� RkGetYomi �����顼���֤���

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
    /* �ɤߤȤ���Ϳ����줿��Τȥ��ʥХåե������Ƥ��㤦����
       �ɤߤΥ�����Ȥ��Ǥ��ޤ��� */
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
    /* �ڤ������狼��ʤ��Τ�Ĵ�٤롣 */
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

  changeCurYomiLen -- �Ѵ��⡼�ɤǤ����Ƥ�ǤϹԤ��ʤ�

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
int zen; /* ���Ѥʤ飱��Ⱦ�Ѥʤ飰 */
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
 * ���ʴ����Ѵ��Ѥι�¤�Τ����Ƥ򹹿�����(���ξ�Τ�)
 *
 * ��d->echo_buffer �򥨥�������������ʸ���ȿž����
 *
 * ������	uiContext
 * �����	�ʤ�
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
  necKanjiError = "�Ѵ��˼��Ԥ��ޤ�����";
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
  hc->context = -1; /* popHenkanMode�ʤɤΤ���Ȥꤢ�����ʥ��ˤ��Ƥ����� */
  if (d->contextCache >= 0) {
    hc->context = d->contextCache;
    d->contextCache = -1;
    debug_message("������å���Τ�Ȥ�\n",0,0,0);
  }
  else {
    if (defaultContext == -1) { /* ��³���ڤ�Ƥ����� */
      debug_message("����³�˹Ԥ�\n",0,0,0);
      if (KanjiInit() < 0) {
	popHenkanMode(d);
	popCallback(d);
	necKanjiError = "irohaserver�˥��ͥ��ȤǤ��ޤ���";
	return 0;
      }
    }
    debug_message("���ǥ�ץꥱ���Ȥ���\n",0,0,0);
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
  /* callback �� callback ���Ȥ� pop ���� */
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
  /* ��ߤ�������Ѵ����� */
  if(doYomiHenkan(d, len) == NG) {
    popHenkanMode(d);
    popCallback(d);
    strcpy(d->genbuf, necKanjiError);
    makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
    NothingForGLine(d);
    return 0;
  }

  /* kanji_status_return���� */
  makeKanjiStatusReturn(d, (henkanContext)d->modec);
  return 0;
}


/*
 * ���ʴ����Ѵ���Ԥ�
 * ��d->yomi_buffer�ˤ�ߤ���Ф���RkBgnBun��Ƥ�Ǥ��ʴ����Ѵ��򳫻Ϥ���
 * ��������ʸ�����Ƭʸ��ˤ��ơ�������ʸ�������
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
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

  /* Ϣʸ���Ѵ��򳫻Ϥ��� *//* ����ˤʤ� �������� �Ҥ餬�� ���ղä��� */
  mode = 0;
  mode = (RK_XFER<<RK_XFERBITS) | RK_KFER;
  
  if(hc->context == -1) {
    int tmp;
    if(defaultContext == -1) {
      if(((tmp = KanjiInit()) != 0) || (defaultContext == -1)) {
	necKanjiError = "irohaserver�˥��ͥ��ȤǤ��ޤ���";
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

  /* ������ʸ�����Ƭʸ�� */
  hc->curbun = 0;

  /* ���������ȥ�󥰤��� */
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
  necKanjiError = "���ʴ����Ѵ��˼��Ԥ��ޤ���";
  return(NG);
}

/*
 * ��ʸ��˰�ư����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
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

  /* ������ʸ��򣱤ı��˰ܤ� */
  /* ������ʸ�᤬�Ǳ����ä��顢
     �Ǻ��򥫥���ʸ��ˤ���   */
  if(RkRight(hc->context) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "ʸ��ΰ�ư�˼��Ԥ��ޤ���";
    return(NG);
  }

  /* kanji_status_return���� */
  makeKanjiStatusReturn(d, hc);
  return(0);
}

/*
 * ��ʸ��˰�ư����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
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

  /* ������ʸ��򣱤ĺ��˰ܤ� */
  /* ������ʸ�᤬�Ǻ����ä��顢
     �Ǳ��򥫥���ʸ��ˤ���   */
  if(RkLeft(hc->context) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "ʸ��ΰ�ư�˼��Ԥ��ޤ���";
    return(NG);
  }

  /* kanji_status_return���� */
  makeKanjiStatusReturn(d, hc);

  return(0);
}

/*
 * ������򥫥��ȸ���ˤ���
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
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

  /* ���θ���򥫥��ȸ���Ȥ��� */
  if(RkNext(hc->context) == -1) {
    necKanjiError = "�����ȸ������Ф��ޤ���Ǥ���";
    if(errno == EPIPE)
      necKanjiPipeError();
    return(NG);
  }

#ifdef MEASURE_TIME
  hc->rktime = times(&timebuf);
  hc->rktime -= proctime;
#endif /* MEASURE_TIME */

  /* ���������ȥ�󥰤��� */
  if(makeEchoStrCurChange(d) == NG)
    return(NG);

  /* kanji_status_return���� */
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

  TanHenkan -- ���������å�����ճ��� TanNextKouho �Ȥۤ�Ʊ��

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
 * ������򥫥��ȸ���ˤ���
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
TanPreviousKouho(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;
  /* ���θ���򥫥��ȸ���Ȥ��� */
  if(RkPrev(hc->context) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "�����ȸ������Ф��ޤ���Ǥ���";
    return(NG);
  }

  /* ���������ȥ�󥰤��� */
  if(makeEchoStrCurChange(d) == NG)
    return(NG);

  /* kanji_status_return���� */
  makeKanjiStatusReturn(d, hc);

  return(0);
}

/*
 * �Ҥ餬�ʤ򥫥��ȸ���ˤ���
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
TanHiragana(d)
uiContext	d;
{
  /* ���������ȥ�󥰤��� */
  ((henkanContext)d->modec)->kouhoCount = 0;
  if(makeEchoStrCurKana(d, JISHU_HIRA) == NG)
    return(NG);

  /* kanji_status_return���� */
  makeKanjiStatusReturn(d, (henkanContext)d->modec);
  return(0);
}

/*
 * �������ʤ򥫥��ȸ���ˤ���
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
TanKatakana(d)
uiContext	d;
{
  ((henkanContext)d->modec)->kouhoCount = 0;
  /* ���������ȥ�󥰤��� */
  if(makeEchoStrCurKana(d, JISHU_ZEN_KATA) == NG)
    return(NG);

  /* kanji_status_return���� */
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

  /* ���������ȥ�󥰤��� */
  if(makeEchoStrCurKana(d, JISHU_HAN_KATA) == NG)
    return(NG);

  /* kanji_status_return���� */
  makeKanjiStatusReturn(d, (henkanContext)d->modec);
  return(0);
}

/*
 * ���޻��򥫥��ȸ���ˤ���
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
TanRomaji(d)
uiContext	d;
{
  ((henkanContext)d->modec)->kouhoCount = 0;
  /* ���������ȥ�󥰤��� */
  if(makeEchoStrCurRomaji(d, 1/*����*/) == NG)
    return(NG);

  /* kanji_status_return���� */
  makeKanjiStatusReturn(d, (henkanContext)d->modec);
  return(0);
}

static
TanHankakuRomaji(d)
uiContext	d;
{
  /* ���������ȥ�󥰤��� */
  if(makeEchoStrCurRomaji(d, 0/*Ⱦ��*/) == NG)
    return(NG);

  /* kanji_status_return���� */
  makeKanjiStatusReturn(d, (henkanContext)d->modec);
  return(0);
}

static
TanZenkaku(d)
uiContext d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;
  if (hc->jishubun == 0) { /* �����Ѵ����줿���Ȥ��ʤ� */
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
      return NothingChangedWithBeep(d); /* ���ꤨ�ʤ��ΤǤϡ� */
    }
}

static
TanHankaku(d)
uiContext d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;
  if (hc->jishubun == 0) { /* �����Ѵ����줿���Ȥ��ʤ� */
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
      return NothingChangedWithBeep(d); /* ���ꤨ�ʤ��ΤǤϡ� */
    }
}

/*
 * �Ǻ�ʸ��˰�ư����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
TanBeginningOfBunsetsu(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;
  /* ������ʸ���ü�ˤ��� */
  if(RkGoTo(hc->context, 0) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "ʸ��ΰ�ư�˼��Ԥ��ޤ���";
    return(NG);
  }
  hc->curbun = 0;

  /* kanji_status_return���� */
  makeKanjiStatusReturn(d, hc);

  return(0);
}

/*
 * �Ǳ�ʸ��˰�ư����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
TanEndOfBunsetsu(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;
  /* ������ʸ���ü�ˤ��� */
  if(RkGoTo(hc->context, (hc->nbunsetsu - 1)) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "ʸ��ΰ�ư�˼��Ԥ��ޤ���";
    return(NG);
  }
  hc->curbun = hc->nbunsetsu - 1;

  /* kanji_status_return���� */
  makeKanjiStatusReturn(d, hc);

  return(0);
}

#ifdef SATOKO
TanMuhenkan(d)	/* ��ʬŪ���ɤߤ��᤹ */
uiContext	d;
{
  int  i, index;
  unsigned char *ptr;


  /* �ɤߤ򥫥��ȸ���Ȥ���(̵�Ѵ�) */
  if(RkNfer(hc->context) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "�ɤߤ���Ф��ޤ���Ǥ���";
    return(NG);
  }
  /* ���������ȥ�󥰤��� */
  ptr = d->echo_buffer;
  /* ��Ƭ��ʸ�ᤫ����̵�Ѵ��ˤ��ơ��������ѤΥХåե�������� */
  /* ��ʸ��ζ褮���ȤäƤ���(��Ƭ����ΥХ��ȿ�) */
  for(i=0, index=0; i < d->nbunsetsu; i++) {
    RkGoTo(hc->context, i);
    if(RkNfer(hc->context) == -1) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "�ɤߤ���Ф��ޤ���Ǥ���";
      return(NG);
    }
    d->kugiri[i] = index;
    if(RkGetKanji(hc->context, (ptr + index), (ROMEBUFSIZE - index)) < 0) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "�����ȸ������Ф��ޤ���Ǥ���";
      return(NG);
    }
    index += strlen(ptr + index);
  }  
  d->kugiri[i] = index; /* �����ΰ��� */
  d->kugiri[i+1] = (int)NULL;

  /* ������ʸ�����Ƭ�ˤ��� */
  RkGoTo(hc->context, 0);

  /* kanji_status_return���� */
  makeKanjiStatusReturn(d, (henkanContext)d->modec);

  return(0);
}
#endif

/*
 * ������ʸ������ޤǳ��ꤷ�������Ȱʹߤ�ʸ����ɤߤ��᤹
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
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

  /* ����ʸ���� ���� */
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

  /* ̤����ʸ����ɤߤ��᤹ */
  for(i=0; i<hc->curbun; i++) { /* ���ꤵ�줿ʸ������ɤߤ�Ĺ�������� */ 
    if(RkGoTo(hc->context, i) == -1) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "ʸ��ΰ�ư�˼��Ԥ��ޤ���";
      return(NG);
    }
    if(RkGetStat(hc->context, &st) == -1) {
      necKanjiError = "���ƥ���������Ф��ޤ���Ǥ���";
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

  /* �Ѵ�����ߤ�� */
  /* �ؽ��Υ������ޥ����ϡ� */
  if(Gakushu) { /* ON */
    if(RkEndBun(hc->context, 1) == -1) {	/* 1:�ؽ����� */
      necKanjiError = "���ʴ����Ѵ��ν�λ�˼��Ԥ��ޤ���";
      if(errno == EPIPE)
	necKanjiPipeError();
    }
  } else { /* OFF */
    if(RkEndBun(hc->context, 0) == -1) {	/* 0:�ؽ����ʤ� */
      necKanjiError = "���ʴ����Ѵ��ν�λ�˼��Ԥ��ޤ���";
      if(errno == EPIPE)
	necKanjiPipeError();
    }
  }

  d->status = QUIT_CALLBACK;
  if (hc->next == (mode_context)yc) {
    popHenkanMode(d);
  }
  else { /* ���������ɤߥ���ƥ����ȽФʤ��Ȥ��� QUIT_CALLBACK �����ޤ�
	    Ư���ʤ������������ɤ߽Фʤ��Ȥ��ϻ�����Ȼפ���Τǥݥå�
	    ���Ƥ��ޤ� */
    popHenkanMode(d);
    popCallback(d);
    popJishuMode(d);
  }
    
  return(d->nbytes);
}

/*
 * ���Ƥ�ʸ����ɤߤ��ᤷ��YomiInputMode �����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
TanMuhenkan(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;

  /* �Ѵ�����ߤ�� */
  if(RkEndBun(hc->context, 0) == -1) {	/* 0:�ؽ����ʤ� */
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "���ʴ����Ѵ��ν�λ�˼��Ԥ��ޤ���";
  }

  d->status = QUIT_CALLBACK;
  popHenkanMode(d);
  return(0);
}

/*
 * ���Ƥ�ʸ�����ꤹ��
 *
 * ��d->buffer_return �� d->echo_buffer �򥳥ԡ���������ʸ����Ȥ���
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
TanKakutei(d)
uiContext	d;
{
  int i, n;
  unsigned char *ptr;
  unsigned char tmpbuf[ROMEBUFSIZE];
  char word[1024], *w; /* ��ư�ؽ� */
  henkanContext hc = (henkanContext)d->modec;
  yomiContext yc = hc->ycx;
#ifdef KAKUTEI_MEASURE_TIME
  struct tms timebuf;
  long proctime, rktime, times();

  proctime = times(&timebuf);
#endif /* KAKUTEI_MEASURE_TIME */


  /* ���������ȥ�󥰤����ʸ����Ȥ��� */
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

  if (RengoGakushu && hc->nbunsetsu > 1) { /* Ϣ��ؽ��򤷤褦���ʤ� */
    int    i;
    RkLex  lex[2][RENGOBUFSIZE];
    char   yomi[2][RENGOBUFSIZE];
    char   kanji[2][RENGOBUFSIZE];
    int    nword[2];

    w = word;
    *w = '\0';

    RkGoTo(hc->context, 0);
    nword[0] = RkGetLex(hc->context, lex[0], RENGOBUFSIZE);
    yomi[0][0] = '\0'; /* yomi[current][0]�ο����� �� RkGetYomi������ */

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
	    /* ���δ�����Ĺ��       >=    ���δ�����Ĺ�� */
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

  /* �Ѵ���λ���� */
  /* �ؽ��Υ������ޥ����ϡ� */
  if(RkEndBun(hc->context, Gakushu ? 1 : 0) == -1) { /* 1:�ؽ�����/0:���ʤ� */
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "���ʴ����Ѵ��ν�λ�˼��Ԥ��ޤ���";
  }

#ifdef KAKUTEI_MEASURE_TIME
  d->rktime = times(&timebuf);
  d->rktime -= rktime;
#endif /* KAKUTEI_MEASURE_TIME */

  if (RengoGakushu && hc->nbunsetsu > 1) { /* Ϣ��ؽ��򤷤褦���ʤ� */
    for (w = word ; *w ; w += strlen(w) + 1) {
      RkDefineDic(hc->context, RengoGakushu, w);
    }
  }

  /* �ɤߤ򥯥ꥢ���� */
  { /* �������ˡ������ɤߤ�����֤������򤹤� */
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
 * �����������ꤵ�������޻��򥤥󥵡��Ȥ���
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
TanKakuteiYomiInsert(d)
uiContext d;
{
  d->nbytes = TanKakutei(d);
  d->more.todo = 1;
  d->more.ch = d->ch;
  d->more.fnum = 0;    /* ��� ch �Ǽ����������򤻤� */
  return(d->nbytes);
}


/* cfuncdef

  pos �ǻ��ꤵ�줿ʸ�ᤪ��Ӥ�������ʸ��λ����Ѵ������
  ���ꥢ���롣
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
 * ʸ��򿭤Ф�
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
TanExtendBunsetsu(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;
  /* ʸ��򿭤Ф� */
  if((hc->nbunsetsu = RkEnlarge(hc->context)) == -1) {
    necKanjiError = "ʸ��γ���˼��Ԥ��ޤ���";
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

  /* ���������ȥ�󥰤��� */
  if(makeEchoStrCurUnderChange(d) == NG)
    return(NG);

  /* kanji_status_return���� */
  makeKanjiStatusReturn(d, hc);

  return(0);
}

/*
 * ʸ���̤��
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
TanShrinkBunsetsu(d)
uiContext	d;
{
  henkanContext hc = (henkanContext)d->modec;

  hc->kouhoCount = 0;
  /* ʸ���̤�� */
  if((hc->nbunsetsu = RkShorten(hc->context)) == -1) {
    necKanjiError = "ʸ��ν̾��˼��Ԥ��ޤ���";
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

  /* ���������ȥ�󥰤��� */
  if(makeEchoStrCurUnderChange(d) == NG)
    return(NG);

  /* kanji_status_return���� */
  makeKanjiStatusReturn(d, hc);

  return(0);
}

/*
 * ��ʬ̵�Ѵ�����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
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
    /* �ʤ�餫�λ�����ɤߤ�Ĺ�����ǧ�Ǥ��ʤ��ä��ʤ齪��� */
    if(errno == EPIPE) {
      necKanjiPipeError();
      d->nbytes = TanKakutei(d);
      strcpy(d->genbuf, "���ʴ����Ѵ������ФȤ���³���ڤ�ޤ���");
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
			 0, /* �̾���Ѵ��򤹤� */
			 bubunEveryTimeCatch, exitBubun, quitBubun);
  if (newyc == (yomiContext)0) {
    return NoMoreMemory();
  }
  newyc->minorMode = IROHA_MODE_BubunMuhenkanMode;
  /* �ɤߥХåե��ˤ��餫�����ɤߤ�����Ƥ����� */
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
    necKanjiError = "�Ѵ��˼��Ԥ��ޤ���";
    return(NG);
  }
  bubunEveryTimeCatch(d, 0, hc);
  d->minorMode = IROHA_MODE_HenkanMode;
  /* �⡼�ɤ��ʲ��� currentModeInfo �ǳμ¤˽��Ϥ����褦�ˤ��� */
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
  /* �⡼�ɤ���Ū���Ѥ��� */
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
    necKanjiError = "�Ѵ��˼��Ԥ��ޤ���";
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

  /* �Ѥ�ä���ʬ���ɤߤ��ѹ����� */
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

  /* �����ȸ����ɽ�����Ѥ��� */
  hc->kouhoCount = 0;
  changeCurBunsetsuString(hc, d->buffer_return, retval);
  makeKanjiStatusReturn(d, hc);
  currentModeInfo(d);
  return 0;
}

#ifdef BUNPOU_DISPLAY
/*
 * ʸˡ�����ץ��Ȥ���
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
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
#define LOCAL_RK /* ������� RK �饤�֥���ʸˡɽ���˻Ȥ� */
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
    fprintf(stderr, "�����ȸ�����ɤߤ���Ф��ޤ���Ǥ�����\n");
  }
  fprintf(stderr, "%s\n", buf);
#endif /* DO_GETYOMI */

  if(RkGetKanji(hc->context, buf, 256) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "�����ȸ������Ф��ޤ���Ǥ���";
    return(NG);
  }
  if((n = RkGetLex(hc->context, lex, 32)) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "�����Ǿ������Ф��ޤ���Ǥ���";
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
    strcpy(mesg, "�����Ǥ����ޤ���");
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
  sprintf(buf, "�Ѵ����� %d [ms]������ UI ���� %d [ms]",
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
  debug_message("�ޤä���ˤ����㤤�ޤ�\n",0,0,0);

  defaultContext = -1;
  defaultBushuContext = -1;

  makeAllContextToBeClosed(0);

  RkFinalize();
}

/* cfuncdef

  getKanjiPart -- ������ʸ���ľ���ޤǤΡ�
                  ����ɽ�����Ĺ�����ɤߤ�Ĺ�������롣
                  �Ĥ��Ǥ˥�����ʸ����ɤߤ�Ĺ�����֤���
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
      necKanjiError = "ʸ��ΰ�ư�˼��Ԥ��ޤ���";
      return(NG);
    }
    if ((nlex = RkGetLex(hc->context, lex, 32)) == -1) {
      if(errno == EPIPE)
	necKanjiPipeError();
      necKanjiError = "�����Ǿ������Ф��ޤ���Ǥ���";
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
    necKanjiError = "ʸ��ΰ�ư�˼��Ԥ��ޤ���";
    return(NG);
  }
  if ((nlex = RkGetLex(hc->context, lex, 32)) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "�����Ǿ������Ф��ޤ���Ǥ���";
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

  if (retval > 0) { /* ������̣���� */
    TanMuhenkan(d); /* ñ����⡼�ɤ����ɤߥ⡼�ɤ���� */
    /* ������ɤߥ⡼�ɤ�ȴ����褦������ */
    d->more.todo = 1;
    d->more.ch = d->ch;
    d->more.fnum = IROHA_FN_Quit;
  }
  else {
    hc = (henkanContext)d->modec;
    clearBunsetsuInfo(hc, hc->curbun);

    /* ���������ȥ�󥰤��� */
    if(makeEchoStrCurUnderChange(d) == NG)
      return NG;

    /* kanji_status_return���� */
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

  TanBunsetsuMode -- ñ����⡼�ɤ���ʸ�῭�Ф��̤�⡼�ɤذܹԤ���

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
  /* AdjustBunsetsuMode(ʸ�῭�Ф��̤�⡼��)�˰ܹԤ��� */
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
