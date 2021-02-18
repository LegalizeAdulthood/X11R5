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

static	char	rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/onoff.c,v 3.3 91/07/30 13:28:19 kon Exp $";

#include	<errno.h>
#include	"iroha.h"

#define ICHISIZE 9

extern int kCount;

static unsigned char *black = (unsigned char *)"��";
static unsigned char *white = (unsigned char *)"��";
static unsigned char *space = (unsigned char *)"��";

static int makeOnOffIchiran();

static void
popOnOffMode(d)
uiContext d;
{
  ichiranContext oc = (ichiranContext)d->modec;

  d->modec = oc->next;
  d->current_mode = oc->prevMode;
  freeIchiranContext(oc);
}

/*
 * ��������Ԥ���
 */
selectOnOff(d, buf, ck, nelem, bangomax, currentkouho, status,
	  everyTimeCallback, exitCallback, quitCallback, auxCallback)
uiContext d;
unsigned char **buf;
int *ck;
int nelem, bangomax;
int currentkouho;
unsigned char *status;
int (*everyTimeCallback)(), (*exitCallback)();
int (*quitCallback)(), (*auxCallback)();
{
  extern KanjiModeRec onoff_mode;
  ichiranContext oc;
  int retval = 0;
  ichiranContext newIchiranCotext();

  if(pushCallback(d, d->modec,
	everyTimeCallback, exitCallback, quitCallback, auxCallback) == 0) {
    necKanjiError = "malloc (pushCallback) �Ǥ��ޤ���Ǥ���";
    return(NG);
  }
  
  if((oc = (ichiranContext)newIchiranContext()) == NULL) {
    popCallback(d);
    return(NG);
  }
  oc->next = d->modec;
  d->modec = (mode_context)oc;

  oc->prevMode = d->current_mode;
  d->current_mode = &onoff_mode;

  oc->allkouho = buf;
  oc->curIkouho = ck;

  if((retval = makeOnOffIchiran(d, nelem, bangomax,
			currentkouho, status))   == NG) {
    popOnOffMode(d);
    popCallback(d);
    return(NG);
  }
  return(retval);
}

/*
 * ��������Ԥ�ɽ���ѤΥǡ�����ơ��֥�˺�������
 *
 * ��glineinfo �� kouhoinfo���������
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
makeOnOffIchiran(d, nelem, bangomax, currentkouho, status)
uiContext d;
int nelem, bangomax;
int currentkouho;
unsigned char *status;
{
  ichiranContext oc = (ichiranContext)d->modec;
  unsigned char **kkptr, *kptr, *gptr, *bptr, *svgptr, **work;
  int           i, ko, lnko, cn = 0, svcn, line = 0;
  RkStat	st;

  oc->nIkouho = nelem;	/* ����ο� */

  /* �����ȸ���򥻥åȤ��� */
  oc->svIkouho = *(oc->curIkouho);
  *(oc->curIkouho) += currentkouho;
  if(*(oc->curIkouho) >= oc->nIkouho)
    oc->svIkouho = *(oc->curIkouho) = 0;

#ifdef DEBUG
  if(iroha_debug) {
    printf("<makeKouhoIchiran> nelem = %d, currentkouho = %d\n", nelem, 
	   currentkouho);
    printf("tooSmall = %d\n", oc->tooSmall);
    printf("<makeKouhoIchiran> buf\n");
    for(work = oc->allkouho; *work; work++)
      printf("-> %s ", *work);
  }
#endif

  if(allocIchiranBuf(d, 0, 1) == NG)
    return(NG);

  if(d->ncolumns < 2) {
    oc->tooSmall = 1;
    return(0);
  }

  /* glineinfo��kouhoinfo���� */
  /* 
   ��glineinfo��
      int glkosu   : int glhead     : int gllen  : unsigned char *gldata
      ���Ԥθ���� : ��Ƭ���䤬     : ���Ԥ�Ĺ�� : ��������Ԥ�ʸ����
                   : �����ܤθ��䤫 :
   -------------------------------------------------------------------------
   0 | 6           : 0              : 24         : �����������ʣ�����������
   1 | 4           : 6              : 16         : ���ã�����������

    ��kouhoinfo��
      int khretsu  : int khpoint  : unsigned char *khdata
      �ʤ����ܤ�   : �Ԥ���Ƭ���� : �����ʸ����
      ������䤫   : ���Х����ܤ� :
   -------------------------------------------------------------------------
   0 | 0           : 0            : ��
   1 | 0           : 4            : ��
             :                :             :
   7 | 1           : 0            : ��
   8 | 1           : 4            : ��
  */

  kkptr = oc->allkouho;
  kptr = *(oc->allkouho);
  gptr = oc->glinebufp;

  /* line -- �����ܤ�
     ko   -- ���Τ���Ƭ���鲿���ܤθ��䤫
     lnko -- �����Ƭ���鲿���ܤθ��䤫
     cn   -- �����Ƭ���鲿�Х����ܤ� */

  for(line=0, ko=0; ko<oc->nIkouho; line++) {
    oc->glineifp[line].gldata = gptr; /* ����Ԥ�ɽ�����뤿���ʸ���� */
    oc->glineifp[line].glhead = ko;   /* ���ιԤ���Ƭ����ϡ����ΤǤ�ko���� */

    for(lnko=0, cn=0;
	cn<d->ncolumns - (kCount ? ICHISIZE + 1: 0) &&
	lnko<bangomax && ko<oc->nIkouho ; lnko++, ko++) {
      kptr = kkptr[ko];
      oc->kouhoifp[ko].khretsu = line; /* �����ܤ�¸�ߤ��뤫��Ͽ */
      if(lnko == 0)
	oc->kouhoifp[ko].khpoint = cn;   /* ���ιԤǲ��Х����ܤ���Ͽ */
      else 
	oc->kouhoifp[ko].khpoint = cn + 2;   /* ���ιԤǲ��Х����ܤ���Ͽ */
      oc->kouhoifp[ko].khdata = kptr;  /* ����ʸ����ؤΥݥ��� */
      svgptr = gptr;
      svcn = cn;

      /* �������򥳥ԡ����� */
      if(lnko) {
	strcpy(gptr, space, 2);
	cn += 2; gptr += 2;
      }
      if(status[ko] == (unsigned char)1)
	strncpy(gptr, black, 2);
      else
	strncpy(gptr, white, 2);
      cn += 2; gptr += 2;
      /* ����򥳥ԡ����� */
      for(; *kptr && cn<d->ncolumns - (kCount ? ICHISIZE + 1: 0);
	  gptr++, kptr++, cn++) {
	*gptr = *kptr;
      }

      /* ���������Ϥߤ����Ƥ��ޤ������ˤʤä��Τǣ����᤹ */
      if ((cn >= d->ncolumns - (kCount ? ICHISIZE + 1: 0)) && *kptr) {
	if (lnko) {
	  gptr = svgptr;
	  cn = svcn;
	  break;
	}
	else {
	  oc->tooSmall = 1;
	  return(0);
	}
      }
    }
    if (kCount) {
      while (cn < d->ncolumns - 1) {
	*gptr++ = (unsigned char)' ';
	cn++;
      }
    }
    /* ���Խ���� */
    *gptr++ = (unsigned char)NULL;
    oc->glineifp[line].glkosu = lnko;
    oc->glineifp[line].gllen = strlen(oc->glineifp[line].gldata);
  }
  /* �Ǹ��NULL������� */
  oc->kouhoifp[ko].khretsu = (int)NULL;
  oc->kouhoifp[ko].khpoint = (int)NULL;
  oc->kouhoifp[ko].khdata  = (unsigned char *)NULL;
  oc->glineifp[line].glkosu  = (int)NULL;
  oc->glineifp[line].glhead  = (int)NULL;
  oc->glineifp[line].gllen   = (int)NULL;
  oc->glineifp[line].gldata  = (unsigned char *)NULL;

#ifdef DEBUG
  if (iroha_debug) {
    for(i=0; oc->glineifp[i].glkosu; i++)
      printf("%d: %s\n", i, oc->glineifp[i].gldata);
  }
#endif

  return(0);
}

/*
 * �����ȸ���򸽺ߤ�ȿ�Фˤ���(ON��OFF, OFF��ON)
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
OnOffSelect(d)
uiContext d;
{
  ichiranContext oc = (ichiranContext)d->modec;
  mountContext mc = (mountContext)oc->next;
  int point, retval = 0;
  unsigned char *gline;

  /* mountNewStatus ���ѹ����� (1��0, 0��1) */
  if(mc->mountNewStatus[*(oc->curIkouho)])
    mc->mountNewStatus[*(oc->curIkouho)] = 0;
  else
    mc->mountNewStatus[*(oc->curIkouho)] = 1;

  /* gline�ѤΥǡ�����񤭴����� (������, ������) */
  gline = oc->glineifp[oc->kouhoifp[*(oc->curIkouho)].khretsu].gldata;
  point = oc->kouhoifp[*(oc->curIkouho)].khpoint;

  if(mc->mountNewStatus[*(oc->curIkouho)])
    strncpy(gline + point, black, 2);
  else
    strncpy(gline + point, white, 2);

  makeGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return(retval);
}

/*
 * status �򤽤Τޤ��֤���OnOff�⡼�ɤ�POP���� (EXIT_CALLBACK)
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
OnOffKakutei(d)
uiContext d;
{
  ichiranContext oc = (ichiranContext)d->modec;
  int retval = 0;
  unsigned char *kakuteiStrings;

  /* �������ɽ�����ѤΥ��ꥢ��ե꡼���� */
  freeIchiranBuf(oc);

  popOnOffMode(d);

#ifdef DEBUG
  if(iroha_debug) {
    mountContext mc = (mountContext)d->modec;
    int i;

    printf("<��mount>\n");
    for(i= 0; mc->mountList[i]; i++)
      printf("[%s][%x][%x]\n", mc->mountList[i],
	     mc->mountOldStatus[i], mc->mountNewStatus[i]);
    printf("\n");
  }
#endif

  /* gline �򥯥ꥢ���� */
  GlineClear(d);

  d->status = EXIT_CALLBACK;

  return(retval);
}

#include	"onoffmap.c"
