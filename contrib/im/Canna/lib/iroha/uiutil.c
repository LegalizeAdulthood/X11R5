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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/uiutil.c,v 3.1 91/08/28 22:05:39 kon Exp $";

#include	<errno.h>
#include "iroha.h"

extern HexkeySelect;

extern int makeGlineStatus();
extern int kigoZenpan(), kigoRussia(), kigoGreek(), kigoKeisen();
extern int uuVBushuExitCatch();
extern int serverInit(), serverFin(), serverChange();
extern int dicTouroku(), dicSakujo(), dicMount();

static int curU1;
static int curU2kigo;
static int curU2server;
static int curU2dic;

#define	UUFL_SZ	5
static
char *uuFirstLine[] =   { "��������",
			  "�����ʥ���������",
			  "��������",
		          "���������",
		          "�������",
		       /* "�Ѵ������ѹ�", */
		           0
		        };

#define UUKL_SZ 4
static
char *uuKigoLine[] =    { "��������",
		          "����ʸ��",
		          "���ꥷ��ʸ��",
		          "����",
		           0
		        };

#define UUSL_SZ 3
static
char *uuServerLine[] =  { "�ƽ����",
		          "�����Ф��ڤ�Υ��",
		          "�����Ф��ڤ��ؤ�",
		           0
		        };

#define UUDL_SZ 3
static
char *uuDicLine[] =     { "ñ����Ͽ",
		          "ñ����",
		          "����ޥ���ȡ�����ޥ����",
		           0
		        };

static
makeUiUtilEchoStr(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;

  d->kanji_status_return->echoStr = ic->allkouho[*(ic->curIkouho)];
  d->kanji_status_return->length = strlen(ic->allkouho[*(ic->curIkouho)]);
  d->kanji_status_return->revPos = 0;
  d->kanji_status_return->revLen = 0;

  return(0);
}

uiUtilIchiranTooSmall(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  makeUiUtilEchoStr(d);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * UI�桼�ƥ���ƥ��ΰ���ɽ��(FirstLine)                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuflExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  forichiranContext fc;
  int cur;

  retval = d->nbytes = 0;

  popCallback(d); /* ������ pop */

  fc = (forichiranContext)d->modec;
  curU1 = cur = fc->curIkouho;

  popForIchiranMode(d);
  popCallback(d);

  return(uiUtil2Mode(d, cur));
}

static
uuflQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* ������ pop */

  popForIchiranMode(d);
  popCallback(d);
  currentModeInfo(d);

  return(retval);
}

/* cfuncdef

  UiUtilMode -- UI�桼�ƥ���ƥ��⡼�ɤˤʤ�Ȥ��˸ƤФ�롣

 */
UiUtilMode(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;
  forichiranContext fc;
  ichiranContext ic;
  unsigned char inhibit = 0;
  int retval = 0;

  if (yc->generalFlags & IROHA_YOMI_CHGMODE_INHIBITTED) {
    return NothingChangedWithBeep(d);
  }    

  d->status = 0;

  if((retval = getForIchiranContext(d)) == NG)
     return(GLineNGReturn(d));
  fc = (forichiranContext)d->modec;

  /* selectOne ��Ƥ֤���ν��� */
  fc->allkouho = (unsigned char **)uuFirstLine;
  fc->curIkouho = 0;
  if( !HexkeySelect )
    inhibit |= ((unsigned char)NUMBERING | (unsigned char)CHARINSERT);
  else
    inhibit |= (unsigned char)CHARINSERT;

  if((retval = selectOne(d, fc->allkouho, &fc->curIkouho, UUFL_SZ, BANGOMAX,
		   inhibit, 0, WITHOUT_LIST_CALLBACK,
		   0, uuflExitCatch,
		   uuflQuitCatch, uiUtilIchiranTooSmall)) == NG) {
    return(GLineNGReturnFI(d));
  }

  ic = (ichiranContext)d->modec;
  ic->majorMode = ic->minorMode = IROHA_MODE_ExtendMode;
  currentModeInfo(d);

  *(ic->curIkouho) = curU1;

  /* ��������Ԥ������Ƹ���������Ф��ʤ� */
  if(ic->tooSmall) {
    d->status = AUX_CALLBACK;
    return(retval);
  }

  makeGlineStatus(d);
  /* d->status = ICHIRAN_EVERYTIME; */

  return(retval);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ����桼�ƥ���ƥ��ΰ���ɽ��                                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
kigoDelivery(d, cur)
uiContext d;
int cur;
{
  int retval = 0;

  switch(cur) {
  case 0 : 
    retval = kigoZenpan(d);
    break;
  case 1 :
    retval = kigoRussia(d);
    break;
  case 2 :
    retval = kigoGreek(d);
    break;
  case 3 :
    retval = kigoKeisen(d);
    break;
  default :
    /* ���򤵤줿���������������ޤ��� */
    return NothingChangedWithBeep(d);
  }

  return(retval);
}

static
uuklExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  forichiranContext fc;
  int cur;

  retval = d->nbytes = 0;

  popCallback(d); /* ������ pop */

  fc = (forichiranContext)d->modec;
  curU2kigo = cur = fc->curIkouho;

  popForIchiranMode(d);
  popCallback(d);

  return(kigoDelivery(d, cur));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * �����Х桼�ƥ���ƥ��ΰ���ɽ��                                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
serverDelivery(d, cur)
uiContext d;
int cur;
{
  int retval = 0;

  switch(cur) {
  case 0 : 
    retval = serverInit(d);
    break;
  case 1 :
    retval = serverFin(d);
    break;
  case 2 :
    retval = serverChange(d);
    break;
  default :
    /* ���򤵤줿���������������ޤ��� */
    return NothingChangedWithBeep(d);
  }

  return(retval);
}

static
uuslExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  forichiranContext fc;
  int cur, sz;

  retval = d->nbytes = 0;

  popCallback(d); /* ������ pop */

  fc = (forichiranContext)d->modec;
  curU2server = cur = fc->curIkouho;

  popForIchiranMode(d);
  popCallback(d);

  return(serverDelivery(d, cur));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ����桼�ƥ���ƥ��ΰ���ɽ��                                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
dicDelivery(d, cur)
uiContext d;
int cur;
{
  int retval = 0;

  switch(cur) {
  case 0 : 
    retval = dicTouroku(d);
    break;
  case 1 :
    retval = dicSakujo(d);
    break;
  case 2 :
    retval = dicMount(d);
    break;
  default :
    /* ���򤵤줿���������������ޤ��� */
    return NothingChangedWithBeep(d);
  }

  return(retval);
}

static
uudlExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  forichiranContext fc;
  int cur;

  retval = d->nbytes = 0;

  popCallback(d); /* ������ pop */

  fc = (forichiranContext)d->modec;
  curU2dic = cur = fc->curIkouho;

  popForIchiranMode(d);
  popCallback(d);

  return(dicDelivery(d, cur));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * UI�桼�ƥ���ƥ��ΰ���ɽ��(SecondLine)                                    *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

uuslIchiranQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* ������ pop */

  popForIchiranMode(d);
  popCallback(d);

  return(UiUtilMode(d));
}

static
uuslQuitCatch(d, retval, env)
uiContext d;
{
  popCallback(d);

  return(UiUtilMode(d));
}

/* cfuncdef

  uiUtil2Mode -- 
  UI�桼�ƥ���ƥ��⡼�ɤ���ƥ桼�ƥ���ƥ������򤵤줿�Ȥ��˸ƤФ��

 */
uiUtil2Mode(d, cur)
uiContext d;
int cur;
{
  forichiranContext fc;
  ichiranContext ic;
  unsigned char inhibit = 0;
  int curU, retval = 0;

  d->status = 0;

  if((retval = getForIchiranContext(d)) == NG)
     return(GLineNGReturn(d));
  fc = (forichiranContext)d->modec;

  /* selectOne ��Ƥ֤���ν��� */
  fc->curIkouho = 0;
  if( !HexkeySelect )
    inhibit |= ((unsigned char)NUMBERING | (unsigned char)CHARINSERT);
  else
    inhibit |= (unsigned char)CHARINSERT;

  switch(cur) {
  case 0 : 
    fc->allkouho = (unsigned char **)uuKigoLine;
    curU = curU2kigo;
    retval = selectOne(d, fc->allkouho, &fc->curIkouho, UUKL_SZ, BANGOMAX,
		       inhibit, 0, WITHOUT_LIST_CALLBACK,
		       0, uuklExitCatch,
		       uuslIchiranQuitCatch, uiUtilIchiranTooSmall);
    break;
  case 1 :
    popForIchiranMode(d);
    popCallback(d);
    return(hexMode(d, 0, uuslQuitCatch)); /* 0 �ϳ�ĥ�⡼�� */
    break;
  case 2 :
    popForIchiranMode(d);
    popCallback(d);
    return(vBushuMode(d, 0, uuVBushuExitCatch, uuslIchiranQuitCatch));
    break;
  case 3 :
    fc->allkouho = (unsigned char **)uuServerLine;
    curU = curU2server;
    retval = selectOne(d, fc->allkouho, &fc->curIkouho, UUSL_SZ, BANGOMAX,
		       inhibit, 0, WITHOUT_LIST_CALLBACK,
		       0, uuslExitCatch,
		       uuslIchiranQuitCatch, uiUtilIchiranTooSmall);
    break;
  case 4 :
    fc->allkouho = (unsigned char **)uuDicLine;
    curU = curU2dic;
    retval = selectOne(d, fc->allkouho, &fc->curIkouho, UUDL_SZ, BANGOMAX,
		       inhibit, 0, WITHOUT_LIST_CALLBACK,
		       0, uudlExitCatch,
		       uuslIchiranQuitCatch, uiUtilIchiranTooSmall);
    break;
  default :
    /* ���򤵤줿���������������ޤ��� */
    popForIchiranMode(d);
    popCallback(d);
    return NothingChangedWithBeep(d);
  }

  if(retval == NG) {
    return(GLineNGReturnFI(d));
  }
  ic = (ichiranContext)d->modec;
  ic->majorMode = ic->minorMode = IROHA_MODE_ExtendMode;
  currentModeInfo(d);

  *(ic->curIkouho) = curU;

  /* ��������Ԥ������Ƹ���������Ф��ʤ� */
  if(ic->tooSmall) {
    d->status = AUX_CALLBACK;
    return(retval);
  }

  makeGlineStatus(d);
  /* d->status = ICHIRAN_EVERYTIME; */

  return(retval);
}
