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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/uldefine.c,v 3.4 91/08/30 22:35:53 kon Exp $";

#include	<errno.h>
#include 	"iroha.h"

extern HexkeySelect;

extern int uiUtilIchiranTooSmall();
extern int dicTourokuHinshiDelivery();

static int dicTourokuDo(), checkUsrDic(), dicTourokuYomi(), dicTourokuYomiDo();

#define HINSHI_SZ 14

static
unsigned char *hinshitbl[] = {(unsigned char *)"��̾",
	                      (unsigned char *)"��̾",
                              (unsigned char *)"���Ρ����̾",
		    	      (unsigned char *)"̾��",
                              (unsigned char *)"����̾��",
		   	      (unsigned char *)"ñ����",
                              (unsigned char *)"ư��",
			      (unsigned char *)"���ƻ�",
                              (unsigned char *)"����ư��",
			      (unsigned char *)"����",
                              (unsigned char *)"����",
			      (unsigned char *)"��³�졦��ư��",
                              (unsigned char *)"Ϣ�λ�",
			      (unsigned char *)"����¾�θ�ͭ̾��",
			                        0
			     };

clearTango(d)
uiContext d;
{
  tourokuContext tc = (tourokuContext)d->modec;

  tc->tango_buffer[0] = 0;
  tc->tango_len = 0;
}

clearYomi(d)
uiContext d;
{
  tourokuContext tc = (tourokuContext)d->modec;

  tc->yomi_buffer[0] = 0;
  tc->yomi_len = 0;
}

static
clearTourokuContext(p)
tourokuContext p;
{
  p->id = TOUROKU_CONTEXT;
  p->genbuf[0] = 0;
  p->qbuf[0] = 0;
  p->tango_buffer[0] = 0;
  p->tango_len = 0;
  p->yomi_buffer[0] = 0;
  p->yomi_len = 0;
  p->curHinshi = 0;
  p->hcode[0] = 0;
  p->katsuyou = 0;
  p->udic = 0;
  p->delContext = 0;

  return(0);
}
  
static tourokuContext
newTourokuContext()
{
  tourokuContext tcxt;

  if((tcxt = 
      (tourokuContext)malloc(sizeof(tourokuContextRec))) == NULL) {
    necKanjiError = "malloc (newTourokuContext) �Ǥ��ޤ���Ǥ���";
    return(0);
  }
  clearTourokuContext(tcxt);

  return tcxt;
}

void
freeTourokuContext(tc)
tourokuContext tc;
{
  free(tc);
}

getTourokuContext(d)
uiContext d;
{
  tourokuContext tc;
  int retval = 0;

  if(pushCallback(d, d->modec, 0, 0, 0, 0) == 0) {
    necKanjiError = "malloc (pushCallback) �Ǥ��ޤ���Ǥ���";
    return(NG);
  }

  if((tc = newTourokuContext()) == NULL) {
    popCallback(d);
    return(NG);
  }
  tc->majorMode = d->majorMode;
  tc->next = d->modec;
  d->modec = (mode_context)tc;

  tc->prevMode = d->current_mode;

  return(retval);
}

void
popTourokuMode(d)
uiContext d;
{
  tourokuContext tc = (tourokuContext)d->modec;

  d->modec = tc->next;
  d->current_mode = tc->prevMode;
  freeTourokuContext(tc);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ñ����Ͽ��ñ�������                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuTTangoEveryTimeCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc = (tourokuContext)env;
  int len, echoLen, revPos;
  char tmpbuf[ROMEBUFSIZE];

  retval = d->nbytes = 0;

  if((echoLen = d->kanji_status_return->length) < 0 || d->more.todo)
    return(retval);

  if(d->kanji_status_return->info & KanjiGLineInfo &&
     d->kanji_status_return->gline.length > 0) {
    echostrClear(d);
    return 0;
  }

  strncpy(tmpbuf, d->kanji_status_return->echoStr, echoLen);
  tmpbuf[echoLen] = '\0';

  sprintf((char *)d->genbuf, "ñ��?[%s]", tmpbuf);
  revPos = strlen("ñ��?[");
  len = revPos + echoLen + 1;
  strcpy(d->genbuf + len, tc->genbuf); /* ��å����� */
  len += strlen(tc->genbuf);
  tc->genbuf[0] = 0;
  d->kanji_status_return->gline.line = d->genbuf;
  d->kanji_status_return->gline.length = len;
  d->kanji_status_return->gline.revPos =
    d->kanji_status_return->revPos + revPos;
  d->kanji_status_return->gline.revLen = d->kanji_status_return->revLen;
  d->kanji_status_return->info |= KanjiGLineInfo;
  d->kanji_status_return->length = 0;

  echostrClear(d);
  checkGLineLen(d);

  return retval;
}

static
uuTTangoExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;

  popCallback(d); /* �ɤߤ� pop */

  tc = (tourokuContext)d->modec;

  strncpy(tc->tango_buffer, d->buffer_return, retval);
  tc->tango_buffer[retval] = '\0';
  tc->tango_len = retval;

  return(dicTourokuYomi(d));
}

uuTTangoQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;

  popCallback(d); /* �ɤߤ� pop */

  tc = (tourokuContext)d->modec;
  if(tc->udic)
    free(tc->udic);
  popTourokuMode(d);
  popCallback(d);

  return(uiUtil2Mode(d, 4)); /* ������� */
}

static
uuT2TangoEveryTimeCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  yomiContext nyc;
  int len, echoLen, revPos, pos, offset;
  char tmpbuf[ROMEBUFSIZE];

  nyc = (yomiContext)env;

  if(d->kanji_status_return->info & KanjiThroughInfo) {
    extern KanjiModeRec yomi_mode;
    _do_func_slightly(d, 0, nyc, &yomi_mode);
  } else if(retval > 0){
    /* �������� */
    generalReplace(nyc->kana_buffer, nyc->kAttr, &nyc->kRStartp,
		   &nyc->kCurs, &nyc->kEndp, 0, d->buffer_return,
		   retval, 2/* HENKANSUMI */);
    generalReplace(nyc->romaji_buffer, nyc->rAttr, &nyc->rStartp,
		   &nyc->rCurs, &nyc->rEndp, 0, d->buffer_return,
		   retval, 0);
  }

  if((echoLen = d->kanji_status_return->length) < 0)
    return(retval);

  strncpy(tmpbuf, d->kanji_status_return->echoStr, echoLen);

  strncpy(d->genbuf, nyc->kana_buffer, pos = offset = nyc->kCurs);

  strncpy(d->genbuf + pos, tmpbuf, echoLen);
  pos += echoLen;
  strncpy(d->genbuf + pos, nyc->kana_buffer + offset, nyc->kEndp - offset);
  pos += nyc->kEndp - offset;
  if (d->kanji_status_return->revLen == 0 && /* ȿžɽ����ʬ�ʤ���... */
      nyc->kEndp - offset) { /* ���ˤ��äĤ�����ʬ������Τʤ� */
    d->kanji_status_return->revLen = (nyc->kana_buffer[offset] & 0x80) ? 2 : 1;
    d->kanji_status_return->revPos = offset + echoLen;
  }
  else {
    d->kanji_status_return->revPos += offset;
  }
  d->kanji_status_return->echoStr = d->genbuf;
  d->kanji_status_return->length = pos;

  return retval;
}

static
uuT2TangoExitCatch(d, retval, nyc)
uiContext d;
int retval;
yomiContext nyc;
{
  yomiContext yc;

  popCallback(d); /* �ɤߤ� pop */

  yc = (yomiContext)d->modec;
  d->nbytes = retval = yc->kEndp;
  strncpy(d->buffer_return, yc->kana_buffer, retval);
  d->buffer_return[retval] = '\0';

  RomajiClearYomi(d);
  popYomiMode(d);
  d->status = EXIT_CALLBACK;
  return retval;
}

static
uuT2TangoQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* �ɤߤ� pop */

  popYomiMode(d);

  d->status = QUIT_CALLBACK;

  return(0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ñ����Ͽ�μ������                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuTMakeDicYesCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  int err = 0;
  tourokuContext tc;
  unsigned char **dp;

  popCallback(d); /* yesNo ��ݥå� */

  tc = (tourokuContext)d->modec;

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
  /* ������� */
  if(RkCreateDic(defaultContext, userdicname[tc->curHinshi], 0x80) != 0) {
    err++;
    strcpy(d->genbuf, "����������˼��Ԥ��ޤ���");
  } else if(RkMountDic(defaultContext, userdicname[tc->curHinshi], 0) == -1) {
    err++;
    strcpy(d->genbuf, "����Υޥ���Ȥ˼��Ԥ��ޤ���");
  } else if(d->contextCache != -1 && 
    RkMountDic(d->contextCache, userdicname[tc->curHinshi], 0) == -1) {
    err++;
    strcpy(d->genbuf, "����Υޥ���Ȥ˼��Ԥ��ޤ���");
  }

  if(err) {
    makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
    if(tc->udic)
      free(tc->udic);
    popTourokuMode(d);
    popCallback(d);
    currentModeInfo(d);
    return(0);
  }

  userdicstatus[tc->curHinshi] = 1;				     

  /* ����θ���κǸ���ɲä��� */
  for(dp=tc->udic; *dp; dp++);
  *dp++ = (unsigned char *)userdicname[tc->curHinshi];
  *dp = 0;

  return(dicTourokuTango(d, uuTTangoQuitCatch));
}

static
uuTMakeDicQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;

  popCallback(d); /* yesNo ��ݥå� */

  tc = (tourokuContext)d->modec;
  if(tc->udic)
    free(tc->udic);
  popTourokuMode(d);
  popCallback(d);

  return(uiUtil2Mode(d, 4)); /* 4 �ϼ��� */
}

static
uuTMakeDicNoCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;

  popCallback(d); /* yesNo ��ݥå� */

  tc = (tourokuContext)d->modec;
  if(tc->udic)
    free(tc->udic);
  popTourokuMode(d);
  popCallback(d);
  currentModeInfo(d);

  GlineClear(d);

  return(retval);
}

unsigned char **
getUserDicName(d)
uiContext d;
{
  char dicMbuf[ROMEBUFSIZE], *dicMp[ROMEBUFSIZE/2];
  char dicLbuf[ROMEBUFSIZE], *dicLp[ROMEBUFSIZE/2];
  char **tourokup, **bptr, **dp, **tp, **up;
  char *sp, *wptr;
  int Mnelem, Lnelem, i;

  /* �桼�����񤬥ޥ���Ȥ���Ƥ��뤫�ɤ����Υե饰�ν���� */
  for(i=0, sp=userdicstatus; i<nuserdics; i++, sp++)
    *sp = 0;

  if(defaultContext == -1) {
    if((KanjiInit() != 0) || (defaultContext == -1)) {
      necKanjiError = "irohaserver�˥��ͥ��ȤǤ��ޤ���";
      return(0);
    }
  }
  if((Mnelem = RkGetMountList(defaultContext, dicMbuf, ROMEBUFSIZE)) <
     (int)NULL) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "�ޥ���Ȥ��Ƥ��뼭��μ��Ф��˼��Ԥ��ޤ���";
    return(0);
  }
  if((Lnelem = RkGetDicList(defaultContext, dicLbuf, ROMEBUFSIZE)) <
     (int)NULL) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "�ޥ���Ȳ�ǽ�ʼ���μ��Ф��˼��Ԥ��ޤ���";
    return(0);
  }
  if((tourokup = (char **)calloc
      (MAX_DICS, sizeof(char *))) == NULL) {
    necKanjiError = "malloc (getUserDicName) �Ǥ��ޤ���Ǥ���";
    return(0);
  }
  for(wptr = dicMbuf, bptr = dicMp; *wptr; bptr++) { /* mount ���Ƥ��뼭�� */
    *bptr = wptr;
    while(*wptr++);
  }
  *bptr = (char *)NULL;
  for(wptr = dicLbuf, bptr = dicLp; *wptr; bptr++) { /* mount ��ǽ�ʼ��� */
    *bptr = wptr;
    while(*wptr++);
  }
  *bptr = (char *)NULL;

  /* .iroha �˽񤫤�Ƥ��ơ��ºݤ˥ޥ���Ȥ���Ƥ����Τ�ԥå����åפ�
     status �� 1 �ˤ��� */
  for(up=userdicname, tp=tourokup, 
      sp=userdicstatus; *up; up++, sp++) {
    for(dp=dicMp; *dp; dp++) {
      if(!strcmp(*up, *dp)) {
	*tp++ = *up;
	*sp = (char)1;
      }
    }
  }
  *tp = 0;

  /* .iroha �˽񤫤�Ƥ��ơ��ޥ���Ȥ���Ƥ��ʤ�����˴ؤ���
     �ޥ���Ȥ˼��Ԥ��Ƥ��뼭��Υ��ƥ������� -1 �ˤ��� */
  for(i=0, up=userdicname; i<MAX_DICS && *up; i++, up++) {
    if(!userdicstatus[i]) {
      for(dp=dicLp; *dp; dp++) {
	if(!strcmp(userdicname[i], *dp)) {
	  userdicstatus[i] = 0;
	  break;
	}
      }
      if(!(*dp))
	userdicstatus[i] = -1;
    }
  }

  return((unsigned char **)tourokup);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ñ����Ͽ��ñ�������                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

dicTouroku(d)
uiContext d;
{
  tourokuContext tc;

  if(dicTourokuDo(d) == NG) {
    return(GLineNGReturn(d));
  }

  tc = (tourokuContext)d->modec;

  if(!*tc->udic) {
    if(checkUsrDic(d) == NG) 
      return(GLineNGReturn(d));
    else
      return(0);
  }
  return(dicTourokuTango(d, uuTTangoQuitCatch));
}

static
dicTourokuDo(d)
uiContext d;
{
  tourokuContext tc;
  unsigned char **up;
  unsigned char **getUserDicName();
  int retval = 0;

  d->status = 0;

  /* �桼������ǥޥ���Ȥ���Ƥ����Τ��äƤ��� */
  if((up = getUserDicName(d)) == 0) {
    return(NG);
  }
#ifdef DEBUG
  if(iroha_debug) {
    char *aho;
    unsigned char **work;

    printf("<touroku> UP");
    for(work=up; *work; work++)
      printf("-> %s ", *work);
    printf("\n");
    for(work=(unsigned char **)userdicname, 
	aho=userdicstatus; *work; work++, aho++)
      printf("UN[%s][%d] ", *work, *aho);
    printf("\n");
  }
#endif

  if((retval == getTourokuContext(d)) == NG) {
    if(up)
      free(up);
    return(NG);
  }

  tc = (tourokuContext)d->modec;
  tc->udic = up;

  return(0);
}

/* 
 * �ޥ���Ȥ���Ƥ��뼭��Υ����å�
 * ��.iroha �� "userdic" �˵��Ҥ���Ƥ��ƥޥ���Ȥ���Ƥ��뼭�񤬤ʤ�
 * ��.iroha �� "userdic" �˵��Ҥ���Ƥ��ƥޥ���Ȥ���Ƥ��ʤ����񤬤���
 *     �� �������(��뼭��ϣ��Ĥ���)
 */
static
checkUsrDic(d)
uiContext d;
{
  tourokuContext tc = (tourokuContext)d->modec;
  coreContext ync;
  int i, nmount = 0;

  for(i=0; i<nuserdics; i++) {
    if(userdicstatus[i] == -1) {
      nmount++;
      sprintf((char *)d->genbuf,
	      "�桼�����񤬤���ޤ��󡣼���(%s)��������ޤ���?(y/n)",
	      userdicname[i]);
      tc->curHinshi = i; /* ��뼭�� */
      if(getYesNoContext(d,
		   0, uuTMakeDicYesCatch,
		   uuTMakeDicQuitCatch, uuTMakeDicNoCatch) == NG) {
	return(GLineNGReturn(d));
      }
      makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
      ync = (coreContext)d->modec;
      ync->majorMode = IROHA_MODE_ExtendMode;
      ync->minorMode = IROHA_MODE_TourokuMode;
      break;
    }
  }

  if(!nmount) {
    strcpy(d->genbuf, "�桼�����񤬻��ꤵ��Ƥ��ޤ���");
    makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
    if(tc->udic)
      free(tc->udic);
    popTourokuMode(d);
    popCallback(d);
    currentModeInfo(d);
  }

  return(0);
}

dicTourokuTango(d, quitfunc)
uiContext d;
int (*quitfunc)();
{
  yomiContext yc, yc2, GetKanjiString();
  tourokuContext tc = (tourokuContext)d->modec;
  int retval = 0;

  yc = GetKanjiString(d, 0, 0,
		      IROHA_NOTHING_RESTRICTED,
		      IROHA_YOMI_CHGMODE_INHIBITTED,
		      IROHA_YOMI_END_IF_KAKUTEI,
		      IROHA_YOMI_INHIBIT_NONE,
		      JISHU_HIRA,
		      0,
		      uuTTangoEveryTimeCatch, uuTTangoExitCatch,
		      quitfunc, 0);
  if (yc == (yomiContext)0) {
    return NoMoreMemory();
  }
  yc2 = GetKanjiString(d, 0, 0,
		      IROHA_NOTHING_RESTRICTED,
		      IROHA_YOMI_CHGMODE_INHIBITTED,
		      !IROHA_YOMI_END_IF_KAKUTEI,
		      IROHA_YOMI_INHIBIT_NONE,
		      JISHU_HIRA,
		      0,
		      uuT2TangoEveryTimeCatch, uuT2TangoExitCatch,
		      uuT2TangoQuitCatch, 0);
  if (yc2 == (yomiContext)0) {
    return NoMoreMemory();
  }
  yc2->generalFlags |= IROHA_YOMI_DELETE_DONT_QUIT;

  yc2->majorMode = IROHA_MODE_ExtendMode;
  yc2->minorMode = IROHA_MODE_TourokuMode;
  currentModeInfo(d);

  return(retval);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ñ����Ͽ���ɤߤ�����                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuTYomiEveryTimeCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc = (tourokuContext)env;
  int len, echoLen, revPos;
  unsigned char tmpbuf[ROMEBUFSIZE];

  retval = d->nbytes = 0;

  if((echoLen = d->kanji_status_return->length) < 0)
    return(retval);

  strncpy(tmpbuf, d->kanji_status_return->echoStr, echoLen);
  tmpbuf[echoLen] = 0;

  sprintf((char *)d->genbuf, "ñ��[%s] �ɤ�?[", (char *)tc->tango_buffer);
  revPos = len = strlen(d->genbuf);
  strncpy(d->genbuf + len, tmpbuf, echoLen);
  len += echoLen;
  strcpy(d->genbuf + len, "]");
  len += 1;
  strcpy(d->genbuf + len, tc->genbuf);
  len += strlen(tc->genbuf);
  tc->genbuf[0] = 0;
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
uuTYomiExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;

  popCallback(d); /* �ɤߤ� pop */

  tc = (tourokuContext)d->modec;

  strncpy(tc->yomi_buffer, d->buffer_return, retval);
  tc->yomi_buffer[retval] = '\0';
  tc->yomi_len = retval;

  return(dicTourokuHinshi(d));
}

static
uuTYomiQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* �ɤߤ� pop */

  clearTango(d);
  clearYomi(d);

  return(dicTourokuTango(d, uuTTangoQuitCatch));
}

static
dicTourokuYomi(d)
uiContext d;
{
  return(dicTourokuYomiDo(d, uuTYomiQuitCatch));
}

static
dicTourokuYomiDo(d, quitfunc)
uiContext d;
int (*quitfunc)();
{
 yomiContext yc, GetKanjiString();
  tourokuContext tc = (tourokuContext)d->modec;
  int retval = 0;

  if(tc->tango_len < 1) {
    strcpy(d->genbuf, "ñ������Ϥ��Ƥ�������"); /* ?? */
    makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
    clearTango(d);

    return(dicTourokuTango(d, uuTTangoQuitCatch));
  }

  yc = GetKanjiString(d, 0, 0,
		      IROHA_NOTHING_RESTRICTED,
		      IROHA_YOMI_CHGMODE_INHIBITTED,
		      IROHA_YOMI_END_IF_KAKUTEI,
		      (IROHA_YOMI_INHIBIT_HENKAN | IROHA_YOMI_INHIBIT_ASHEX |
		      IROHA_YOMI_INHIBIT_ASBUSHU),
		      JISHU_HIRA,
		      0,
		      uuTYomiEveryTimeCatch, uuTYomiExitCatch,
		      quitfunc, 0);
  if (yc == (yomiContext)0) {
    return NoMoreMemory();
  }
  yc->majorMode = IROHA_MODE_ExtendMode;
  yc->minorMode = IROHA_MODE_TourokuMode;
  currentModeInfo(d);

  return(retval);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ñ����Ͽ���ʻ������                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuTHinshiExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  forichiranContext fc;
  tourokuContext tc;
  int cur;

  retval = d->nbytes = 0;

  popCallback(d); /* ������ pop */

  fc = (forichiranContext)d->modec;
  cur = fc->curIkouho;

  popForIchiranMode(d);
  popCallback(d);

  tc = (tourokuContext)d->modec;
  tc->curHinshi = cur;

  return(dicTourokuHinshiDelivery(d));
}

static
uuTHinshiQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* ������ pop */

  popForIchiranMode(d);
  popCallback(d);

  clearYomi(d);

  return(dicTourokuYomi(d));
}

dicTourokuHinshi(d)
uiContext d;
{
  tourokuContext tc = (tourokuContext)d->modec;
  forichiranContext fc;
  ichiranContext ic;
  unsigned char inhibit = 0;
  int currentkouho, retval = 0;

  d->status = 0;

#ifdef DEBUG
  if(iroha_debug) {
    tourokuContext tc = (tourokuContext)d->modec;
    printf("ñ�� = %s, �ɤ� = %s\n", tc->tango_buffer, tc->yomi_buffer);
  }
#endif

  if(tc->yomi_len < 1) {
    strcpy(d->genbuf, "�ɤߤ����Ϥ��Ƥ�������"); /* ?? */
    makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
    return dicTourokuYomi(d);
  }

  if((retval = getForIchiranContext(d)) == NG) {
    return(GLineNGReturnTK(d));
  }

  fc = (forichiranContext)d->modec;
  /* selectOne ��Ƥ֤���ν��� */
  fc->allkouho = (unsigned char **)hinshitbl;
  fc->curIkouho = 0;
  currentkouho = 0;
  if( !HexkeySelect )
    inhibit |= ((unsigned char)NUMBERING | (unsigned char)CHARINSERT); 
  else
    inhibit |= (unsigned char)CHARINSERT; 

  if((retval = selectOne(d, fc->allkouho, &fc->curIkouho, HINSHI_SZ,
		 BANGOMAX, inhibit, currentkouho, WITH_LIST_CALLBACK,
		 0, uuTHinshiExitCatch, 
		 uuTHinshiQuitCatch, uiUtilIchiranTooSmall)) == NG) {
    popForIchiranMode(d);
    popCallback(d);
    return(GLineNGReturnTK(d));
  }

  ic = (ichiranContext)d->modec;
  ic->majorMode = IROHA_MODE_ExtendMode;
  ic->minorMode = IROHA_MODE_TourokuHinshiMode;
  currentModeInfo(d);

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
 * jrKanjiControl ��                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

dicTourokuControl(d, tango, quitfunc)
uiContext d;
unsigned char *tango;
int (*quitfunc)();
{
  tourokuContext tc;

  if(dicTourokuDo(d) == NG) {
    return(GLineNGReturn(d));
  }

  tc = (tourokuContext)d->modec;

  if(!*tc->udic) {
    if(checkUsrDic(d) == NG) 
      return(GLineNGReturn(d));
    else
      return(0);
  }

  if(tango == 0 || tango[0] == 0) {
    return(dicTourokuTango(d, quitfunc));
  }

  strcpy(tc->tango_buffer, tango);
  tc->tango_len = strlen(tc->tango_buffer);

  return(dicTourokuYomiDo(d, quitfunc));
}
