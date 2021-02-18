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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/ulmount.c,v 3.3 91/07/22 10:34:00 kon Exp $";

#include	<errno.h>
#include 	"iroha.h"

extern int uiUtilIchiranTooSmall();

/* cfunc mountContext
 *
 * mountContext
 *
 */
static mountContext
newMountContext()
{
  mountContext mcxt;

  if((mcxt = (mountContext)calloc(1, sizeof(mountContextRec))) == NULL) {
    necKanjiError = "malloc (newMountContext) �Ǥ��ޤ���Ǥ���";
    return(0);
  }
  mcxt->id = MOUNT_CONTEXT;

  return mcxt;
}

static void
freeMountContext(mc)
mountContext mc;
{
  if (mc) {
    if (mc->mountList) {
      if (*(mc->mountList)) {
	free(*(mc->mountList));
      }
      free(mc->mountList);
    }
    if (mc->mountOldStatus) {
      free(mc->mountOldStatus);
    }
    if (mc->mountNewStatus) {
      free(mc->mountNewStatus);
    }
    free(mc);
  }
}

/*
 * ��������Ԥ���
 */
static
getMountContext(d)
uiContext d;
{
  mountContext mc;
  int retval = 0;

  if(pushCallback(d, d->modec, 0, 0, 0, 0) == 0) {
    necKanjiError = "malloc (pushCallback) �Ǥ��ޤ���Ǥ���";
    return(NG);
  }
  
  if((mc = newMountContext()) == NULL) {
    popCallback(d);
    return(NG);
  }
  mc->majorMode = d->majorMode;
  mc->next = d->modec;
  d->modec = (mode_context)mc;

  mc->prevMode = d->current_mode;

  return(retval);
}

static void
popMountMode(d)
uiContext d;
{
  mountContext mc = (mountContext)d->modec;

  d->modec = mc->next;
  d->current_mode = mc->prevMode;
  freeMountContext(mc);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ����Υޥ���ȡ�����ޥ����                                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuMountExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  mountContext mc;
  int i, nmount = 0;

  popCallback(d); /* OnOff ��ݥå� */

  if(defaultContext == -1) {
    if((KanjiInit() != 0) || (defaultContext == -1)) {
      necKanjiError = "irohaserver�˥��ͥ��ȤǤ��ޤ���";
      popMountMode(d);
      popCallback(d);
      return(GLineNGReturn(d));
    }
  }

  mc = (mountContext)d->modec;
  for(i=0; mc->mountList[i]; i++) {
    if(mc->mountOldStatus[i] != mc->mountNewStatus[i]) {
      if(mc->mountNewStatus[i]) {
	/* �ޥ���Ȥ��� */
	nmount++;
	if((retval = RkMountDic(defaultContext, mc->mountList[i],
			    strlen(mc->mountList[i]))) == NG)
	  strcpy(d->genbuf, "����Υޥ���Ȥ˼��Ԥ��ޤ���");
	else if(d->contextCache != -1 &&
	  (retval = RkMountDic(d->contextCache, mc->mountList[i],
			    strlen(mc->mountList[i]))) == NG)
	  strcpy(d->genbuf, "����Υޥ���Ȥ˼��Ԥ��ޤ���");
      } else {
	/* ����ޥ���Ȥ��� */
	nmount++;
	if((retval = RkUnmountDic(defaultContext, mc->mountList[i])) == NG)
	  strcpy(d->genbuf, "����Υ���ޥ���Ȥ˼��Ԥ��ޤ���");
	else if(d->contextCache != -1 &&
	  (retval = RkUnmountDic(d->contextCache, mc->mountList[i])) == NG)
	  strcpy(d->genbuf, "����Υ���ޥ���Ȥ˼��Ԥ��ޤ���");
      }
    }
  }

  if(nmount)
    makeAllContextToBeClosed(1);

  if(retval != NG)
    strcpy(d->genbuf, "����Υޥ���ȡ�����ޥ���Ȥ�Ԥ��ޤ���");

  makeGLineMessage(d, d->genbuf, strlen(d->genbuf));

  popMountMode(d);
  popCallback(d);
  currentModeInfo(d);

  return(retval);
}

static
uuMountQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* OnOff ��ݥå� */

  popMountMode(d);
  popCallback(d);
  currentModeInfo(d);

  return(uiUtil2Mode(d, 4));
}

/*
 * dicLbuf                dicLp       soldp   snewp
 * ���������������������� ����������  ������  ������
 * ��iroha\@fuzokugo\@k�� ��*iroha��  ��1 ��  ��1 ��
 * ��atakana\@satoko\@s�� ��*fuzo ��  ��1 ��  ��1 ��
 * ��oft\@\@...        �� ��*kata ��  ��0 ��  ��0 ��
 * ��                  �� ��  :   ��  ��: ��  ��: ��
 * ���������������������� ����������  ������  ������
 * dicMbuf                dicMp
 * ���������������������� ����������
 * ��iroha\@fuzokugo\@s�� ��*iroha��
 * ��atoko\@\@...      �� ��*fuzo ��
 * ��                  �� ��*sato ��
 * ��                  �� ��  :   ��
 * ���������������������� ����������
 */
static
getDicList(d)
uiContext d;
{
  mountContext mc = (mountContext)d->modec;
  unsigned char *dicLbuf, dicMbuf[ROMEBUFSIZE];
  unsigned char **dicLp, **bptr, *dicMp[ROMEBUFSIZE/2];
  unsigned char *sop, *snp, *wptr, *soldp, *snewp, **Lp, **Mp;
  int dicLc, dicMc, i;

  if((dicLbuf = (unsigned char *)malloc(ROMEBUFSIZE)) == NULL) {
    necKanjiError = "malloc (getDicList) �Ǥ��ޤ���Ǥ���";
    return(NG);
  }
  if(defaultContext == -1) {
    if((KanjiInit() != 0) || (defaultContext == -1)) {
      necKanjiError = "irohaserver�˥��ͥ��ȤǤ��ޤ���";
      free(dicLbuf);
      return(NG);
    }
  }
  if((dicLc = RkGetDicList(defaultContext, dicLbuf, ROMEBUFSIZE)) <
     (int)NULL) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "�ޥ���Ȳ�ǽ�ʼ���μ��Ф��˼��Ԥ��ޤ���";
    free(dicLbuf);
    return(NG);
  }
  if (dicLc == 0) {
    necKanjiError = "�ޥ���Ȳ�ǽ�ʼ���¸�ߤ��ޤ���";
    free(dicLbuf);
    return NG;
  }
  if((dicLp = (unsigned char **)calloc
      (dicLc + 1, sizeof(char *))) == NULL) {
    necKanjiError = "malloc (getDicList) �Ǥ��ޤ���Ǥ���";
    free(dicLbuf);
    return(NG);
  }
  if((soldp = (unsigned char *)malloc(dicLc + 1)) == NULL) {
    necKanjiError = "malloc (getDicList) �Ǥ��ޤ���Ǥ���";
    free(dicLbuf);
    free(dicLp);
    return(NG);
  }
  if((snewp = (unsigned char *)malloc(dicLc + 1)) == NULL) {
    necKanjiError = "malloc (getDicList) �Ǥ��ޤ���Ǥ���";
    free(dicLbuf);
    free(dicLp);
    free(soldp);
    return(NG);
  }
  for(i = 0, wptr = dicLbuf; i < dicLc; i++) { /* buf ���� */
    dicLp[i] = wptr;
    while(*wptr++); /* NULL �ޤǥ����åפ���NULL �μ��ޤǥݥ��󥿤�ʤ�� */
  }
  dicLp[i] = NULL;

  if(defaultContext == -1) {
    if((KanjiInit() != 0) || (defaultContext == -1)) {
      necKanjiError = "irohaserver�˥��ͥ��ȤǤ��ޤ���";
      free(dicLbuf);
      free(dicLp);
      free(soldp);
      return(NG);
    }
  }
  if((dicMc = RkGetMountList(defaultContext, dicMbuf, ROMEBUFSIZE)) <
     (int)NULL) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "�ޥ���Ȥ��Ƥ��뼭��μ��Ф��˼��Ԥ��ޤ���";
    free(dicLbuf);
    free(dicLp);
    free(soldp);
    free(snewp);
    return(NG);
  }

  for(i = 0, wptr = dicMbuf ; i < dicMc ; i++) { /* buf ���� */
    dicMp[i] = wptr;
    while(*wptr++); /* NULL �ޤǥ����åפ���NULL �μ��ޤǥݥ��󥿤�ʤ�� */
  }
  dicMp[i] = NULL;

  for(i=0, sop=soldp, snp=snewp; i<dicLc; i++, sop++, snp++) {
    *sop = 0;
    *snp = 0;
  }
  for(Lp=dicLp, sop=soldp, snp=snewp; *Lp; Lp++, sop++, snp++) {
    for(Mp=dicMp; *Mp; Mp++) {
      if(!strcmp(*Lp, *Mp)) {
	*sop = *snp = 1;
	break;
      }
    }
  }
  mc->mountList = dicLp;
  mc->mountOldStatus = soldp;
  mc->mountNewStatus = snewp;

  return(dicLc);
}

dicMount(d)
uiContext d;
{
  ichiranContext oc;
  mountContext mc;
  int retval = 0, currentkouho = 0, nelem;

  d->status = 0;

  if((retval = getMountContext(d)) == NG)
    return(GLineNGReturn(d));

  /* ����ꥹ�Ȥȥޥ���ȡ�����ޥ���Ȥξ��֤� montContext �ˤȤäƤ��� */
  if((nelem = getDicList(d)) == NG) {
    popMountMode(d);
    popCallback(d);
    return(GLineNGReturn(d));
  }

  mc = (mountContext)d->modec;
#ifdef DEBUG
  if(iroha_debug) {
    int i;

    printf("<��mount>\n");
    for(i= 0; mc->mountList[i]; i++)
      printf("[%s][%x][%x]\n", mc->mountList[i],
	     mc->mountOldStatus[i], mc->mountNewStatus[i]);
    printf("\n");
  }
#endif

  /* selectOnOff ��Ƥ֤���ν��� */
  mc->curIkouho = currentkouho = 0;
  if((retval = selectOnOff(d, mc->mountList, &mc->curIkouho, nelem,
		 BANGOMAX, currentkouho, mc->mountOldStatus,
		 0, uuMountExitCatch,
		 uuMountQuitCatch, uiUtilIchiranTooSmall)) == NG) {
    popMountMode(d);
    popCallback(d);
    return(GLineNGReturn(d));
  }

  oc = (ichiranContext)d->modec;
  oc->majorMode = IROHA_MODE_ExtendMode;
  oc->minorMode = IROHA_MODE_MountDicMode;
  currentModeInfo(d);

  /* ��������Ԥ������Ƹ���������Ф��ʤ� */
  if(oc->tooSmall) {
    char *p = "��������Ѥ����������ΤǼ���ޥ���ȡ�����ޥ���ȤǤ��ޤ���";

    ichiranFin(d);
    popCallback(d); /* OnOff ��ݥå� */
    popMountMode(d);
    popCallback(d);
    currentModeInfo(d);
    makeGLineMessage(d, p, strlen(p));
    return(0);
  }

  makeGlineStatus(d);
  /* d->status = ICHIRAN_EVERYTIME; */

  return(retval);
}
