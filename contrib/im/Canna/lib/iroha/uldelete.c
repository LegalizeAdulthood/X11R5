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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/uldelete.c,v 3.3 91/08/28 22:01:18 kon Exp $";

#include	<errno.h>
#include 	"iroha.h"

extern HexkeySelect;

extern uiUtilIchiranTooSmall();

static int dicSakujoYomi(), dicSakujoEndBun(), dicSakujoTango(), dicSakujoDo();

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ñ�����μ������                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuSDicExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  forichiranContext fc;
  int cur;

  retval = d->nbytes = 0;

  popCallback(d); /* ������ pop */

  fc = (forichiranContext)d->modec;
  cur = fc->curIkouho;

  popForIchiranMode(d);
  popCallback(d);

  return(dicSakujoYomi(d, cur));
}

static
uuSDicQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;

  popCallback(d); /* ������ pop */

  popForIchiranMode(d);
  popCallback(d);

  tc = (tourokuContext)d->modec;
  if(tc->udic)
    free(tc->udic);
  popTourokuMode(d);
  popCallback(d);

  return(uiUtil2Mode(d, 4)); /* �������� 4 */
}

dicSakujo(d)
uiContext d;
{
  unsigned char **up, **getUserDicName();
  tourokuContext tc;
  int retval = 0;

  d->status = 0;

  /* �桼������ǥޥ���Ȥ���Ƥ����Τ��äƤ��� */
  if((up = getUserDicName(d)) == 0) {
    return(GLineNGReturn(d));
  }

  if((retval == getTourokuContext(d)) == NG) {
    if(up)
      free(up);
    return(GLineNGReturn(d));
  }
  tc = (tourokuContext)d->modec;
  tc->udic = up;

  if(!*up) {
    strcpy(d->genbuf, "�桼�����񤬻��ꤵ��Ƥ��ޤ���");
    makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
    
    if(tc->udic)
      free(tc->udic);
    popTourokuMode(d);
    popCallback(d);
    currentModeInfo(d);
    return(0);
  }

  return(dicTourokuDictionary(d, uuSDicExitCatch, uuSDicQuitCatch));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ñ�������ɤߤ�����                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuSYomiEveryTimeCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc = (tourokuContext)env;
  int len, echoLen, revPos;
  char tmpbuf[ROMEBUFSIZE];

  retval = 0;
  if((echoLen = d->kanji_status_return->length) < 0)
    return(retval);
  strncpy(tmpbuf, d->kanji_status_return->echoStr, echoLen);
  tmpbuf[echoLen] = '\0';

  sprintf((char *)d->genbuf, "�ɤ�?[%s]", tmpbuf);
  revPos = 6;
  len = strlen(d->genbuf);
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
uuSYomiExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;

  popCallback(d); /* �ɤߤ� pop */

  tc = (tourokuContext)d->modec;
  strncpy(tc->yomi_buffer, d->buffer_return, retval);
  tc->yomi_buffer[retval] = 0;
  tc->yomi_len = strlen(tc->yomi_buffer);

  return(dicSakujoTango(d));
}

static
uuSYomiQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;

  popCallback(d); /* �ɤߤ� pop */

  clearYomi(d);

  tc = (tourokuContext)d->modec;
  if(tc->udic)
    free(tc->udic);
  popTourokuMode(d);
  popCallback(d);

  return(dicSakujo(d));
}

static
dicSakujoYomi(d, cur)
uiContext d;
int cur;
{
  tourokuContext tc = (tourokuContext)d->modec;
  yomiContext yc, GetKanjiString();

  d->status = 0;
  tc->curHinshi = cur; /* ����̾���äƤ��� */

  yc = GetKanjiString(d, 0, 0,
	      IROHA_NOTHING_RESTRICTED,
	      IROHA_YOMI_CHGMODE_INHIBITTED,
	      IROHA_YOMI_END_IF_KAKUTEI,
	      (IROHA_YOMI_INHIBIT_HENKAN | IROHA_YOMI_INHIBIT_ASHEX |
	      IROHA_YOMI_INHIBIT_ASBUSHU),
	      JISHU_HIRA,
	      0,
	      uuSYomiEveryTimeCatch, uuSYomiExitCatch,
	      uuSYomiQuitCatch, 0);
  if (yc == (yomiContext)0) {
    return NoMoreMemory();
  }
  yc->majorMode = IROHA_MODE_ExtendMode;
  yc->minorMode = IROHA_MODE_DeleteDicMode;
  currentModeInfo(d);

  return(0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ñ������ñ�������                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuSTangoExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  forichiranContext fc;
  tourokuContext tc;
  RkLex lex[5];
  int cur;

  popCallback(d); /* ������ pop */

  fc = (forichiranContext)d->modec;
  cur = fc->curIkouho;

  popForIchiranMode(d);
  popCallback(d);

  tc = (tourokuContext)d->modec;
  strncpy(tc->tango_buffer, d->buffer_return, d->nbytes);
  tc->tango_buffer[d->nbytes] = 0;
  tc->tango_len = d->nbytes;

  if((retval = RkXfer(tc->delContext, cur)) == NG) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "�����ȸ������Ф��ޤ���Ǥ���";
  }

  if((retval = RkGetLex(tc->delContext, lex, 5)) == 0) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "�����Ǿ������Ф��ޤ���Ǥ���";
  }

  sprintf(tc->hcode, "#%d#%d", lex[0].rownum, lex[0].colnum);

#ifdef DEBUG
  if(iroha_debug)
    printf("row <%d> col <%d> hinshi <%s>\n", lex[0].rownum, lex[0].colnum, tc->hcode);
#endif
  if(dicSakujoEndBun(d) == NG)
    return(GLineNGReturnTK(d));

  retval = d->nbytes = 0;

  return(dicSakujoDo(d));
}

static
uuSTangoQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* ������ pop */

  popForIchiranMode(d);
  popCallback(d);
  clearYomi(d);

  return(dicSakujoYomi(d));
}

/*
 * �ɤߤ���ꤵ�줿���񤫤��Ѵ�����
 */
static
dicSakujoBgnBun(d, st)
uiContext d;
RkStat *st;
{
  tourokuContext tc = (tourokuContext)d->modec;
  int nbunsetsu;

  if((tc->delContext = RkCreateContext())== NG) {
    necKanjiError = "ñ�����ѤΥ���ƥ����Ȥ�����Ǥ��ޤ���";
    return(NG);
  }

  if(RkMountDic(tc->delContext, tc->udic[tc->curHinshi], 0) == NG) {
    necKanjiError = "ñ�����Ѥμ����ޥ���ȤǤ��ޤ���Ǥ���";
    return(NG);
  }

  if((nbunsetsu = RkBgnBun(tc->delContext, tc->yomi_buffer, tc->yomi_len, 0))
	== -1) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "���ʴ����Ѵ��˼��Ԥ��ޤ���";
    return(NG);
  }
  
  if(RkGetStat(tc->delContext, st) == -1) {
    RkEndBun(tc->delContext, 0); /* 0:�ؽ����ʤ� */
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "���ƥ���������Ф��ޤ���Ǥ���";
    return(NG);
  }

  return(nbunsetsu);
}

static
dicSakujoEndBun(d)
uiContext d;
{
  tourokuContext tc = (tourokuContext)d->modec;

  if(RkEndBun(tc->delContext, 0) == -1) {	/* 0:�ؽ����ʤ� */
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "���ʴ����Ѵ��ν�λ�˼��Ԥ��ޤ���";
    return(NG);
  }

  return(0);
}

static
dicSakujoTango(d)
uiContext d;
{
  tourokuContext tc = (tourokuContext)d->modec;
  forichiranContext fc;
  ichiranContext ic;
  unsigned char **allDelCands, **getIchiranList(), inhibit = 0;
  int nbunsetsu, nelem, currentkouho, retval = 0;
  RkStat	st;

  if(tc->yomi_len < 1) {
    strcpy(d->genbuf, "�ɤߤ����Ϥ��Ƥ�������"); /* ?? */
    makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
    return dicSakujoYomi(d);
  }

  if((nbunsetsu = dicSakujoBgnBun(d, &st)) == NG)
    return(GLineNGReturnTK(d));
  if((nbunsetsu != 1) || (st.maxcand == 0)) {
    /* ���䤬�ʤ� */
    if(dicSakujoEndBun(d) == NG)
      return(GLineNGReturnTK(d));

    strcpy(d->genbuf, "�����ɤߤ���Ͽ���줿ñ���¸�ߤ��ޤ���");
    makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
    if(tc->udic)
      free(tc->udic);
    popTourokuMode(d);
    popCallback(d);
    currentModeInfo(d);
    return(0);
  }

  /* ���٤Ƥθ������Ф� */
  if((allDelCands = 
      getIchiranList(d, tc->delContext, &nelem, &currentkouho)) == 0) {
    if(tc->udic)
      free(tc->udic);
    return(GLineNGReturnTK(d));
  }

  if(getForIchiranContext(d) == NG) {
    if(tc->udic)
      free(tc->udic);
    freeGetIchiranList(allDelCands);
    return(GLineNGReturnTK(d));
  }

  fc = (forichiranContext)d->modec;
  fc->allkouho = allDelCands;

  if( !HexkeySelect )
    inhibit |= ((unsigned char)NUMBERING | (unsigned char)CHARINSERT);
  else
    inhibit |= (unsigned char)CHARINSERT;

  fc->curIkouho = currentkouho;	/* ���ߤΥ����ȸ����ֹ����¸���� */
  currentkouho = 0;	/* �����ȸ��䤫�鲿���ܤ򥫥��ȸ���Ȥ��뤫 */

  /* ��������˰ܹԤ��� */
  if((retval = selectOne(d, fc->allkouho, &fc->curIkouho, nelem, BANGOMAX,
               inhibit, currentkouho, WITHOUT_LIST_CALLBACK,
	       0, uuSTangoExitCatch,
	       uuSTangoQuitCatch, uiUtilIchiranTooSmall)) == NG) {
    freeGetIchiranList(fc->allkouho);
    return(GLineNGReturnTK(d));
  }

  ic = (ichiranContext)d->modec;
  if(ic->tooSmall) {
    d->status = AUX_CALLBACK;
    return(retval);
  }

  makeGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return(retval);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ñ����                                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuSDeleteYesCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;

  popCallback(d); /* yesNo ��ݥå� */

  tc = (tourokuContext)d->modec;
  sprintf((char *)d->genbuf, "%s %s %s", 
	  (char *)tc->yomi_buffer, tc->hcode, (char *)tc->tango_buffer);

#ifdef DEBUG
  if(iroha_debug)
    printf("RkDeleteDic ���� <%s>, ���� <%s>\n", d->genbuf, tc->udic[tc->curHinshi]);
#endif

  if(defaultContext == -1) {
    if((KanjiInit() != 0) || (defaultContext == -1)) {
      necKanjiError = "irohaserver�˥��ͥ��ȤǤ��ޤ���";
      if(tc->udic)
	free(tc->udic);
      popTourokuMode(d);
      popCallback(d);
      return(GLineNGReturn(d));
    }
  }
  /* ���񤫤�ñ��������� */
  if((retval = RkDeleteDic(defaultContext, tc->udic[tc->curHinshi],
			   d->genbuf)) == NG) {
    if(errno == EPIPE)
      necKanjiPipeError();
    strcpy(d->genbuf, "ñ�����Ǥ��ޤ���Ǥ���");
  } else {
    /* ����δ�λ��ɽ������ */
    sprintf((char *)d->genbuf, "��%s��(%s)�������ޤ���",
	    (char *)tc->tango_buffer, (char *)tc->yomi_buffer);
  }

  RkCloseContext(tc->delContext);

  makeGLineMessage(d, d->genbuf, strlen(d->genbuf));

  if(tc->udic)
    free(tc->udic);
  popTourokuMode(d);
  popCallback(d);
  currentModeInfo(d);

  return(0);
}

static
uuSDeleteQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc = (tourokuContext)env;

  popCallback(d); /* yesNo ��ݥå� */

  if(tc->udic)
    free(tc->udic);
  popTourokuMode(d);
  popCallback(d);

  return(uiUtil2Mode(d, 4));
}

static
uuSDeleteNoCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc = (tourokuContext)env;

  popCallback(d); /* yesNo ��ݥå� */

  if(tc->udic)
    free(tc->udic);
  popTourokuMode(d);
  popCallback(d);
  currentModeInfo(d);

  GlineClear(d);

  return(retval);
}

static
dicSakujoDo(d)
uiContext d;
{
  tourokuContext tc = (tourokuContext)d->modec;

  sprintf((char *)d->genbuf, "��%s��(%s)���?(y/n)",
	  (char *)tc->tango_buffer, (char *)tc->yomi_buffer);
  if(getYesNoContext(d,
	     0, uuSDeleteYesCatch,
	     uuSDeleteQuitCatch, uuSDeleteNoCatch) == NG) {
    if(tc->udic)
      free(tc->udic);
    return(GLineNGReturnTK(d));
  }
  makeGLineMessage(d, d->genbuf, strlen(d->genbuf));

  return(0);
}
