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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/ulserver.c,v 3.2 91/06/28 18:18:26 kon Exp $";

#include	<errno.h>
#include "iroha.h"

static int serverChangeDo();

int iroha_reset = 0;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * �ƽ����                                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

serverInit(d)
uiContext d;
{
  unsigned char **warning;
  int retval = 0;

  d->status = 0;

  /* �ƽ������Ԥ��ե饰��ON�ˤ���
     dofunc ��Ƥ����˼ºݤν������Ԥ���(serverInitDo) */
  iroha_reset = 1;

  return(retval);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * �����Ф��ڤ�Υ��                                                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

serverFin(d)
uiContext d;
{
  unsigned char **warning;
  int retval = 0;

  d->status = 0;

  necKanjiPipeError();
  
  strcpy(d->genbuf, "���ʴ����Ѵ������ФȤ���³���ڤ�ޤ���");
  makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
  currentModeInfo(d);

  return(retval);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * �����Ф��ڤ괹��                                                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuServerChangeEveryTimeCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  yomiContext yc = (yomiContext)d->modec;
  int len, echoLen, revPos;

  if((echoLen = d->kanji_status_return->length) < 0)
    return(retval);

  strcpy(d->genbuf, "�ޥ���̾?[");
  revPos = len = strlen(d->genbuf);
  strncpy(d->genbuf + len, d->kanji_status_return->echoStr, echoLen);
  len += echoLen;
  strcpy(d->genbuf + len, "]");
  len += 1;
  d->kanji_status_return->gline.line = d->genbuf;
  d->kanji_status_return->gline.length = len;
  d->kanji_status_return->gline.revPos = 
    d->kanji_status_return->revPos + revPos;
  d->kanji_status_return->gline.revLen = d->kanji_status_return->revLen;
  d->kanji_status_return->info |= KanjiGLineInfo;
  echostrClear(d);
  checkGLineLen(d);

  return retval;
}

static
uuServerChangeExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* �ɤߤ� pop */

  return(serverChangeDo(d, retval));
}

static
uuServerChangeQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* �ɤߤ� pop */

  return(uiUtil2Mode(d, 3)); /* 3�ϥ�������� */
}

serverChange(d)
uiContext d;
{
  yomiContext GetKanjiString(), yc;
  int retval = 0;

  d->status = 0;

  if ((yc = GetKanjiString(d, (unsigned char *)NULL, 0,
		     IROHA_ONLY_ASCII,
		     IROHA_YOMI_CHGMODE_INHIBITTED,
		     IROHA_YOMI_END_IF_KAKUTEI,
		     IROHA_YOMI_INHIBIT_ALL,
		     JISHU_HAN_KATA,
		     0,
		     uuServerChangeEveryTimeCatch, uuServerChangeExitCatch,
		     uuServerChangeQuitCatch, 0))
      == (yomiContext)0) {
    return NoMoreMemory();
  }
  yc->minorMode = IROHA_MODE_ChangingServerMode;
  currentModeInfo(d);

  return(retval);
}
		 
static
serverChangeDo(d, len)
uiContext d;
int len;
{
  unsigned char newServerName[256];

  d->status = 0;

  if(!len)
    return(serverChange(d));

  strncpy(newServerName, d->buffer_return, len);
  newServerName[len] = 0;
#ifdef DEBUG
  if(iroha_debug)
    printf("iroha_server_name = [%s]\n", newServerName);
#endif

  necKanjiPipeError();
  RkSetServerName(newServerName);

  if(defaultContext == -1) {
    if((KanjiInit() != 0) || (defaultContext == -1)) {
      necKanjiError = "irohaserver�˥��ͥ��ȤǤ��ޤ���";
      return(GLineNGReturn(d));
    } else {
      d->contextCache = RkDuplicateContext(defaultContext);
    }
  }

  sprintf((char *)d->genbuf, 
	  "%s �Τ��ʴ����Ѵ������Ф���³���ޤ���", (char *)newServerName);
  makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
  currentModeInfo(d);

  return(0);
}
