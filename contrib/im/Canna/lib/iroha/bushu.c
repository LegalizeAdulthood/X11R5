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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/bushu.c,v 3.4 91/08/29 19:00:13 kon Exp $";

#include	<errno.h>
#include "iroha.h"

extern HexkeySelect;

extern int makeGlineStatus(), uuslQuitCatch();
extern int uiUtilIchiranTooSmall(), uuslIchiranQuitCatch();
static int bushuHenkan(), makeBushuIchiranQuit();
static int uuVBushuIchiranQuitCatch();
static int vBushuExitCatch(), bushuQuitCatch();


#define	BUSHU_SZ	150

static
char *bushu_char[] = { "��", "Ш", "��", "��", "��", "��",

		       "���ʤ�äȤ���", "��", "��", "ұ", "��ҹ��", "е",

		       "��", "�͡��Ρʤˤ�٤��", "��", "��", "Ȭ", "ѹ",

		       "��", "��", "׮", "��ʤ�������)", "��", "��",

		       "��", "��", "��ʤ���������)", "�ȡʤ���Τؤ��",

		       "��", "���ʤ����ȡ�", "��", "���ʤ��󤺤���", "׵",

		       "��", "����ñ�ʤġ�", "��ʤ���ˤ礦��", "��", "��",

		       "��", "��ʤƤؤ��", "��", "��", "��", "ͼ",

		       "��", "˻�ʤ�ä���٤��", "��", "��", "��",

		       "�����ʤ����ؤ��", "��", "��", "��", "��", "��",

		       "��", "��", "��", "��", "ھ", "��",

		       "��", "��", "���ʤ�ä���", "��", "��", "��",

		       "��", "��", "��", "��", "��", "���ʤ��᤹�ؤ��", "��",

		       "��", "Ω", "��", "��", "�", "��",

		       "�ˡʤ�ޤ������", "��", "��", "��", "��", "Ϸ",

		       "��", "��", "��ʤ����ؤ��", "��", "��", "��",

		       "�ݡʤ����������", "��", "�סʤȤ餫�����", "��",

		       "��", "��", "��", "��", "��", "��",

		       "��", "��", "­��ɥ", "�", "��",

		       "��", "��", "��", "��", "��", "��", "��", "ë",

		       "��", "��", "��", "Ʀ", "��", "�", "��", "��",

		       "��", "��", "�", "��", "��", "��", "��", "��",

		       "��", "��", "��", "��", "��", "��", "��", "��",

		       "�", "��", "��", "��", "Ļ", "��", "��", "ɡ",

		       "��", "����", "����¾", 0
		     };

static
char *bushu_key[] =  { "����", "��", "�����Ф�", "���夦", "�դ�", "������",

		       "��äȤ�", "��", "����", "��", "���ޤ�", "�ʤ�", "��",

		       "�Ҥ�", "��", "�Ĥ���", "�Ϥ�", "��", "��",

		       "��", "����", "��������", "���Τ�", "�����", "���礦",

		       "��", "����", "�����", "��", "������", "����餤",

		       "��", "����", "���㤯", "��", "����", "����",

		       "����", "��", "��", "�Ϥ�", "��", "���",

		       "�椦", "���", "��ä���", "����", "������", "����",

		       "����", "����", "��", "�����ޤ�", "��", "������",

		       "����", "�Ĥ�", "�Ĥ�", "�ˤ�", "�Τ֤�", "��",

		       "�ۤ�", "�ۤ�", "��ĤƤ�", "��ޤ�", "����", "����",

		       "����", "����", "�����", "����", "���᤹", "��",

		       "����", "��", "����", "�Τ�", "��", "�Ϥ�", "��",

		       "��ޤ�", "���", "����", "����", "����", "����",

		       "����", "����", "�����", "����", "����", "����",

		       "����", "��", "�Ȥ�", "�ˤ�", "�ˤ�", "�Ϥ�", "�ҤĤ�",

		       "�դ�", "�դ�", "�ߤ�", "�ष", "����", "����",

		       "���Τ�", "����", "����", "���餤", "�����", "����",

		       "����", "����", "����", "����", "�Ĥ�", "�Τ���",

		       "�Ф�", "�ޤ�", "��", "�स��", "����", "���餺",

		       "����", "���", "�դ�Ȥ�", "�ڡ���", "����", "����",

		       "����", "����", "����", "���礯", "�ʤᤷ", "���",

		       "����", "����", "����", "������", "�Ȥ�", "�ۤ�",

		       "����", "����", "�Ȥ�", "����", "����", "�Ϥ�",

		       "��", "������", "���Τ�", 0
		     };

/*
 * �������Υ������Ѥ�ʸ�������
 *
 * ������	RomeStruct
 * �����	���ｪλ�� 0
 */
makeBushuEchoStr(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;

  d->kanji_status_return->echoStr = ic->allkouho[*(ic->curIkouho)];
  d->kanji_status_return->length = 2;
  d->kanji_status_return->revPos = 0;
  d->kanji_status_return->revLen = 2;

  return(0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * forichiranContext�Ѵؿ�                                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * forichiranContext �ν����
 */
static
clearForIchiranContext(p)
forichiranContext p;
{
  p->id = FORICHIRAN_CONTEXT;
  p->curIkouho = 0;
  p->allkouho = 0;

  return(0);
}
  
static forichiranContext
newForIchiranContext()
{
  forichiranContext fcxt;

  if((fcxt = 
      (forichiranContext)malloc(sizeof(forichiranContextRec))) == NULL) {
    necKanjiError = "malloc (newForIchiranContext) �Ǥ��ޤ���Ǥ���";
    return(0);
  }
  clearForIchiranContext(fcxt);

  return fcxt;
}

static void
freeForIchiranContext(fc)
forichiranContext fc;
{
  free(fc);
}

getForIchiranContext(d)
uiContext d;
{
  forichiranContext fc;
  int retval = 0;

  if(pushCallback(d, d->modec, 0, 0, 0, 0) == 0) {
    necKanjiError = "malloc (pushCallback) �Ǥ��ޤ���Ǥ���";
    return(NG);
  }
  
  if((fc = newForIchiranContext()) == NULL) {
    popCallback(d);
    return(NG);
  }
  fc->next = d->modec;
  d->modec = (mode_context)fc;

  fc->prevMode = d->current_mode;
  fc->majorMode = d->majorMode;

  return(retval);
}

void
popForIchiranMode(d)
uiContext d;
{
  forichiranContext fc = (forichiranContext)d->modec;

  d->modec = fc->next;
  d->current_mode = fc->prevMode;
  freeForIchiranContext(fc);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * �桼�ƥ���ƥ�������⡼������                                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uuVBushuExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  forichiranContext fc;
  int cur;

  popCallback(d); /* ������ݥå� */

  fc = (forichiranContext)d->modec;
  cur = fc->curIkouho;

  popForIchiranMode(d);
  popCallback(d);

  return(bushuHenkan(d, 1, 0, cur, uuVBushuIchiranQuitCatch));
}

static
uuVBushuIchiranQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* ������ݥå� */

  popForIchiranMode(d);
  popCallback(d);

  return(vBushuMode(d, 0, uuVBushuExitCatch, uuslIchiranQuitCatch));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ����⡼������                                                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
vBushuIchiranQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* ������ݥå� */

  popForIchiranMode(d);
  popCallback(d);

  return(vBushuMode(d, 1, vBushuExitCatch, bushuQuitCatch));
}

static
vBushuExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  forichiranContext fc;
  int cur;

  popCallback(d); /* ������ݥå� */

  fc = (forichiranContext)d->modec;
  cur = fc->curIkouho;

  popForIchiranMode(d);
  popCallback(d);

  return(bushuHenkan(d, 1, 1, cur, vBushuIchiranQuitCatch));
}

BushuMode(d)
uiContext d;
{
  int bushuQuitCatch();
  yomiContext yc = (yomiContext)d->modec;

  if (yc->generalFlags & IROHA_YOMI_CHGMODE_INHIBITTED) {
    return NothingChangedWithBeep(d);
  }    

  return(vBushuMode(d, 1, vBushuExitCatch, bushuQuitCatch));
}

vBushuMode(d, flag, exitfunc, quitfunc)
uiContext d;
int flag;
int (*exitfunc)();
int (*quitfunc)();
{
  forichiranContext fc;
  ichiranContext ic;
  unsigned char inhibit = 0;
  int retval = 0;

  d->status = 0;

  if((retval = getForIchiranContext(d)) == NG) {
    return(GLineNGReturn(d));
  }
  fc = (forichiranContext)d->modec;

  /* selectOne ��Ƥ֤���ν��� */
  fc->allkouho = (unsigned char **)bushu_char;
  fc->curIkouho = 0;
  if( !HexkeySelect )
    inhibit |= ((unsigned char)NUMBERING | (unsigned char)CHARINSERT);
  else
    inhibit |= (unsigned char)CHARINSERT;

  if((retval = selectOne(d, fc->allkouho, &fc->curIkouho, BUSHU_SZ,
		 BANGOMAX, inhibit, 0, WITH_LIST_CALLBACK,
		 0, exitfunc,
		 quitfunc, uiUtilIchiranTooSmall)) == NG) {
    return(GLineNGReturnFI(d));
  }

  ic = (ichiranContext)d->modec;
  if(flag)
    ic->majorMode = ic->minorMode = IROHA_MODE_BushuMode;
  else {
    ic->majorMode = IROHA_MODE_ExtendMode;
    ic->minorMode = IROHA_MODE_BushuMode;
  }
  currentModeInfo(d);

  *(ic->curIkouho) = d->curbushu;

  /* ��������Ԥ������Ƹ���������Ф��ʤ� */
  if(ic->tooSmall) {
    d->status = AUX_CALLBACK;
    return(retval);
  }

  if ( !(ic->flags & ICHIRAN_ALLOW_CALLBACK) ) {
    makeGlineStatus(d);
  }
  /* d->status = ICHIRAN_EVERYTIME; */

  return(retval);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ����⡼�����Ϥΰ���ɽ��                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
bushuEveryTimeCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  makeBushuEchoStr(d);

  return(retval);
}

static
bushuExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* ������ݥå� */

  if (((forichiranContext)env)->allkouho != (unsigned char **)bushu_char) {
    /* bushu_char �� static ����������� free ���ƤϤ����ʤ���
       ���������ΤäƤʤ󤫱����ʤ� */
    freeGetIchiranList(((forichiranContext)env)->allkouho);
  }
  popForIchiranMode(d);
  popCallback(d);
  retval = YomiExit(d, retval);
  currentModeInfo(d);

  return retval;
}

static
bushuQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* ������ݥå� */

  if (((forichiranContext)env)->allkouho != (unsigned char **)bushu_char) {
    /* bushu_char �� static ����������� free ���ƤϤ����ʤ���
       ���������ΤäƤʤ󤫱����ʤ� */
    freeGetIchiranList(((forichiranContext)env)->allkouho);
  }
  popForIchiranMode(d);
  popCallback(d);
  currentModeInfo(d);
  GlineClear(d);

  return(retval);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ����Ȥ��Ƥ��Ѵ��ΰ���ɽ��                                                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
convBushuQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* ������ݥå� */

  if (((forichiranContext)env)->allkouho != (unsigned char **)bushu_char) {
    /* bushu_char �� static ����������� free ���ƤϤ����ʤ���
       ���������ΤäƤʤ󤫱����ʤ� */
    freeGetIchiranList(((forichiranContext)env)->allkouho);
  }
  popForIchiranMode(d);
  popCallback(d);

  makeYomiReturnStruct(d);
  currentModeInfo(d);

  return(retval);
}

/*
 * �ɤߤ�����Ȥ����Ѵ�����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
ConvertAsBushu(d)
uiContext	d;
{
  yomiContext yc = (yomiContext)d->modec;

  d->status = 0; /* clear status */
  
  if (yc->henkanInhibition & IROHA_YOMI_INHIBIT_ASBUSHU) {
    return NothingChangedWithBeep(d);
  }

  if (yc->henkanCallback) {
    strncpy(d->buffer_return, yc->kana_buffer, d->nbytes);
    return yc->henkanCallback(d, IROHA_FN_ConvertAsBushu,
			      d->genbuf, d->nbytes);
  }

  d->nbytes = yc->kEndp;
  strncpy(d->buffer_return, yc->kana_buffer, d->nbytes);

  /* 0 �ϡ�ConvertAsBushu ����ƤФ줿���Ȥ򼨤� */
  return(bushuHenkan(d, 0, 1, 0, convBushuQuitCatch));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ������                                                                    *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * �ɤߤ����󼭽񤫤������Ѵ�����
 */
static
bushuBgnBun(d, st, yomi, length)
uiContext d;
RkStat *st;
unsigned char *yomi;
int length;
{
  int nbunsetsu;

  /* Ϣʸ���Ѵ��򳫻Ϥ��� *//* ����ˤ������Τ߼��Ф� */
  if(d->bushuContext == -1) {
    if((defaultBushuContext == -1) && (KanjiInit() == -1)) {
	necKanjiError = "irohaserver�˥��ͥ��ȤǤ��ޤ���";
	return(NG);
    } else {
      d->bushuContext = RkDuplicateContext(defaultBushuContext);
    }
  }
  if((nbunsetsu = 
      RkBgnBun(d->bushuContext, yomi, length, 0))
	== -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "���ʴ����Ѵ��˼��Ԥ��ޤ���";
    return(NG);
  }
  
  if(RkGetStat(d->bushuContext, st) == -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "���ƥ���������Ф��ޤ���Ǥ���";
    return(NG);
  }

  return(nbunsetsu);
}

/*
 * �ɤߤ�Ⱦ�������ղä��Ƹ�������Ԥ�ɽ������
 *
 * ������	uiContext
 *		flag	ConvertAsBushu����ƤФ줿 0
 *			BushuYomiHenkan����ƤФ줿 1
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 *
 *
 * �����������Ϥޤ� getForIchiranContext ���ƤФ�Ƥ��ʤ���ΤȤ���
 */

static
bushuHenkan(d, flag, ext, cur, quitfunc)
uiContext	d;
int             flag, cur;
int             (*quitfunc)();
{
  forichiranContext fc;
  ichiranContext ic;
  unsigned char *yomi, *ptr, **buf, inhibit = 0, **allBushuCands;
  RkStat	st;
  int nelem, currentkouho, nbunsetsu, length, retval = 0;
  
  unsigned char **getIchiranList();

  if(flag) {
    yomi = (unsigned char *)bushu_key[cur];
    length = strlen(yomi);
    d->curbushu = (short)cur;
  } else {
    d->nbytes = RomajiFlushYomi(d, d->buffer_return, d->bytes_buffer);
    yomi = d->buffer_return;
    length = d->nbytes;
  }

  if((nbunsetsu = bushuBgnBun(d, &st, yomi, length)) == NG)
    return(GLineNGReturn(d));

  if((nbunsetsu != 1) || (st.klen > 2) || (st.maxcand == 0)) {
    /* ����Ȥ��Ƥθ��䤬�ʤ� */

#ifdef DEBUG
  if (iroha_debug) {
  
    char names[256];
    char *sap;
    int satoko, i;
    
    fprintf(stderr, "nbunsetsu(%d), klen(%d), maxcand(%d)\n", 
	    nbunsetsu, st.klen, st.maxcand);

    satoko = RkGetMountList(d->bushuContext, names, sizeof(names));
    for(i=0, sap = names; i<satoko; i++) {
      fprintf(stderr, "%s,  ", sap);
      while(*sap++);
    }

    fprintf(stderr, "context = %d\n", d->bushuContext);
  }
#endif
    
    d->kanji_status_return->length = -1;

    makeBushuIchiranQuit(d, flag);
    currentModeInfo(d);

    if(flag) {
      strcpy(d->genbuf, "��������θ���Ϥ���ޤ���");
      makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
    } else {
      return(NothingChangedWithBeep(d));
    }

    return(0);
  }

  /* ��������Ԥ�ɽ������ */
  /* 0 �ϡ������ȸ��� + 0 �򥫥��ȸ���ˤ��뤳�Ȥ򼨤� */

  if((allBushuCands
      = getIchiranList(d, d->bushuContext, &nelem, &currentkouho)) == 0) {
    return(GLineNGReturn(d));
  }

  /* �����Ѵ��ϳؽ����ʤ��� */
  if(RkEndBun(d->bushuContext, 0) == -1) {	/* 0:�ؽ����ʤ� */
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "���ʴ����Ѵ��ν�λ�˼��Ԥ��ޤ���";
    freeGetIchiranList(allBushuCands);
    return(GLineNGReturn(d));
  }

  if(getForIchiranContext(d) == NG) {
    freeGetIchiranList(allBushuCands);
    return(GLineNGReturn(d));
  }

  fc = (forichiranContext)d->modec;
  fc->allkouho = allBushuCands;

  if( !HexkeySelect )
    inhibit |= (unsigned char)NUMBERING;
  fc->curIkouho = currentkouho;	/* ���ߤΥ����ȸ����ֹ����¸���� */
  currentkouho = 0;	/* �����ȸ��䤫�鲿���ܤ򥫥��ȸ���Ȥ��뤫 */

  if((retval = selectOne(d, fc->allkouho, &fc->curIkouho, nelem, BANGOMAX,
			 inhibit, currentkouho, WITH_LIST_CALLBACK,
			 bushuEveryTimeCatch, bushuExitCatch,
			 quitfunc, uiUtilIchiranTooSmall)) == NG) {
    freeGetIchiranList(allBushuCands);
    return(GLineNGReturnFI(d));
  }

  ic = (ichiranContext)d->modec;

  if(!flag) { /* convertAsBushu */
    ic->majorMode = ic->minorMode = IROHA_MODE_BushuMode;
  } else {
    if(ext) {
      ic->majorMode = IROHA_MODE_BushuMode;
      ic->minorMode = IROHA_MODE_TankouhoMode;
    } else {
      ic->majorMode = IROHA_MODE_ExtendMode;
      ic->minorMode = IROHA_MODE_TankouhoMode;
    }
  }
  currentModeInfo(d);

  /* ��������Ԥ������Ƹ���������Ф��ʤ� */
  if(ic->tooSmall) {
    d->status = AUX_CALLBACK;
    return(retval);
  }

  if ( !(ic->flags & ICHIRAN_ALLOW_CALLBACK) ) {
    makeGlineStatus(d);
  }
  /* d->status = EVERYTIME_CALLBACK; */

  return(retval);
}

/*
 * ����Ԥ�õ������⡼�ɤ���ȴ�����ɤߤ��ʤ��⡼�ɤ˰ܹԤ���
 *
 * ������	uiContext
 *		flag	ConvertAsBushu����ƤФ줿 0
 *			BushuYomiHenkan����ƤФ줿 1
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
makeBushuIchiranQuit(d, flag)
uiContext	d;
int              flag;
{

  /* �����Ѵ��ϳؽ����ʤ��� */
  if(RkEndBun(d->bushuContext, 0) == -1) {	/* 0:�ؽ����ʤ� */
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "���ʴ����Ѵ��ν�λ�˼��Ԥ��ޤ���";
    return(NG);
  }

  if(flag) {
    /* kanji_status_return �򥯥ꥢ���� */
    d->kanji_status_return->length  = 0;
    d->kanji_status_return->revLen  = 0;
    
    d->status = QUIT_CALLBACK;
  } else {
    makeYomiReturnStruct(d);
  }
  GlineClear(d);
  
  return(0);
}
