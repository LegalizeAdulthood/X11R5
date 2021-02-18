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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/ulhinshi.c,v 3.3 91/08/28 22:01:21 kon Exp $";

#include <errno.h>
#include "iroha.h"

int gramaticalQuestion = 1;

extern HexkeySelect;

extern int uiUtilIchiranTooSmall();
extern int uuTTangoQuitCatch();

static int tourokuYes(), tourokuNo(), makeDoushi(), uuTDicExitCatch();
static int uuTDicQuitCatch(), tangoTouroku();

static
unsigned char *gyouA[] = { (unsigned char *)"��", (unsigned char *)"��",
			   (unsigned char *)"��", (unsigned char *)"��",
			   (unsigned char *)"��", (unsigned char *)"��",
			   (unsigned char *)"��", (unsigned char *)"��",
			   (unsigned char *)"��",};

static
unsigned char *gyouI[] = { (unsigned char *)"��", (unsigned char *)"��",
			   (unsigned char *)"��", (unsigned char *)"��",
			   (unsigned char *)"��", (unsigned char *)"��",
			   (unsigned char *)"��", (unsigned char *)"��",
			   (unsigned char *)"��",};

static
unsigned char *gyouU[] = { (unsigned char *)"��", (unsigned char *)"��",
			   (unsigned char *)"��", (unsigned char *)"��",
			   (unsigned char *)"��", (unsigned char *)"��",
			   (unsigned char *)"��", (unsigned char *)"��",
			   (unsigned char *)"��",};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ñ����Ͽ���ʻ����� ��Yes/No ���� Quit��                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuTHinshiYNQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  
  return(dicTourokuHinshi(d));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ñ����Ͽ���ʻ����� ��Yes/No �裲�ʳ� ���̥�����Хå���                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuTHinshi2YesCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;
  int uuTDicExitCatch(), uuTDicQuitCatch();

  popCallback(d); /* yesNo ��ݥå� */

  tourokuYes(d);   /* �ʻ줬��ޤ�� tc->hcode �˥��åȤ��� */

  tc = (tourokuContext)d->modec;

  if(tc->qbuf[0]) {
    /* �⤦����Ϥʤ���� */
  } else if(tc->hcode[0]) {
    /* �ʻ줬��ޤä��Τǡ���Ͽ����桼������λ����Ԥ� */
    return(dicTourokuDictionary(d, uuTDicExitCatch, uuTDicQuitCatch));
  }

  return(retval);
}

static
uuTHinshi2NoCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;
  int uuTDicExitCatch(), uuTDicQuitCatch();

  popCallback(d); /* yesNo ��ݥå� */

  tourokuNo(d);   /* �ʻ줬��ޤ�� tc->hcode �˥��åȤ��� */

  tc = (tourokuContext)d->modec;

  if(tc->qbuf[0]) {
    /* �⤦����Ϥʤ���� */
  } else if(tc->hcode[0]) {
    /* �ʻ줬��ޤä��Τǡ���Ͽ����桼������λ����Ԥ� */
    return(dicTourokuDictionary(d, uuTDicExitCatch, uuTDicQuitCatch));
  }

  return(retval);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ñ����Ͽ���ʻ����� ��Yes/No �裱�ʳ� ������Хå���                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuTHinshi1YesCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;
  coreContext ync;
  int uuTDicExitCatch(), uuTDicQuitCatch();
  
  popCallback(d); /* yesNo ��ݥå� */

  tourokuYes(d);   /* �ʻ줬��ޤ�� tc->hcode �˥��åȤ��� */

  tc = (tourokuContext)d->modec;

  if(tc->qbuf[0]) {
    /* ���䤹�� */
    makeGLineMessage(d, tc->qbuf, strlen(tc->qbuf));
    if((retval = getYesNoContext(d,
		 0, uuTHinshi2YesCatch,
		 uuTHinshiYNQuitCatch, uuTHinshi2NoCatch)) == NG) {
      return(GLineNGReturnTK(d));
    }
    ync = (coreContext)d->modec;
    ync->majorMode = IROHA_MODE_ExtendMode;
    ync->minorMode = IROHA_MODE_TourokuHinshiMode;
  } else if(tc->hcode[0]) {
    /* �ʻ줬��ޤä��Τǡ���Ͽ����桼������λ����Ԥ� */
    return(dicTourokuDictionary(d, uuTDicExitCatch, uuTDicQuitCatch));
  }

  return(retval);
}

static
uuTHinshi1NoCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;
  coreContext ync;
  int uuTDicExitCatch(), uuTDicQuitCatch();

  popCallback(d); /* yesNo ��ݥå� */

  tourokuNo(d);   /* �ʻ줬��ޤ�� tc->hcode �˥��åȤ��� */

  tc = (tourokuContext)d->modec;

  if(tc->qbuf[0]) {
    /* ���䤹�� */
    makeGLineMessage(d, tc->qbuf, strlen(tc->qbuf));
    if((retval = getYesNoContext(d,
		 0, uuTHinshi2YesCatch,
		 uuTHinshiYNQuitCatch, uuTHinshi2NoCatch)) == NG) {
      return(GLineNGReturnTK(d));
    }
    ync = (coreContext)d->modec;
    ync->majorMode = IROHA_MODE_ExtendMode;
    ync->minorMode = IROHA_MODE_TourokuHinshiMode;
  } else if(tc->hcode[0]) {
    /* �ʻ줬��ޤä��Τǡ���Ͽ����桼������λ����Ԥ� */
    return(dicTourokuDictionary(d, uuTDicExitCatch, uuTDicQuitCatch));
  }

  return(retval);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ñ����Ͽ���ʻ�ʬ�����롩                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuTHinshiQYesCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;
  coreContext ync;

  popCallback(d); /* yesNo ��ݥå� */

  tc = (tourokuContext)d->modec;

  makeGLineMessage(d, tc->qbuf, strlen(tc->qbuf)); /* ���� */
  if((retval = getYesNoContext(d,
	 0, uuTHinshi1YesCatch,
	 uuTHinshiYNQuitCatch, uuTHinshi1NoCatch)) == NG) {
    return(GLineNGReturnTK(d));
  }
  ync = (coreContext)d->modec;
  ync->majorMode = IROHA_MODE_ExtendMode;
  ync->minorMode = IROHA_MODE_TourokuHinshiMode;

  return(retval);
}

static
uuTHinshiQNoCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  int uuTDicExitCatch(), uuTDicQuitCatch();

  popCallback(d); /* yesNo ��ݥå� */

  return(dicTourokuDictionary(d, uuTDicExitCatch, uuTDicQuitCatch));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ñ����Ͽ���ʻ�����                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

dicTourokuHinshiDelivery(d)
uiContext	d;
{
  tourokuContext tc = (tourokuContext)d->modec;
  coreContext ync;
  int uuTDicExitCatch(), uuTDicQuitCatch();
  int retval = 0;

  makeHinshi(d); /* �ʻ졢���顼��å�����������򥻥åȤ��Ƥ��� */

#ifdef DEBUG
  if(iroha_debug) {
    printf("tc->genbuf=%s, tc->qbuf=%s, tc->hcode=%s\n", tc->genbuf, tc->qbuf,
	   tc->hcode);
  }
#endif
  if(tc->genbuf[0]) {
    /* ���Ϥ��줿�ǡ����˸�꤬���ä��Τǡ�
       ��å�������ɽ�������ɤ����Ϥ���� */
    clearYomi(d);
    return(dicTourokuTango(d, uuTTangoQuitCatch));
  } else if(tc->qbuf[0] && gramaticalQuestion) {
    /* �٤����ʻ�ʬ���Τ���μ���򤹤� */
    strcpy(d->genbuf,
	   "����˺٤����ʻ�ʬ���Τ���μ���򤷤Ƥ��ɤ��Ǥ���?(y/n)");
    if((retval = getYesNoContext(d,
		 0, uuTHinshiQYesCatch,
		 uuTHinshiYNQuitCatch, uuTHinshiQNoCatch)) == NG) {
      return(GLineNGReturnTK(d));
    }
    makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
    ync = (coreContext)d->modec;
    ync->majorMode = IROHA_MODE_ExtendMode;
    ync->minorMode = IROHA_MODE_TourokuHinshiMode;
    return(retval);
  } else if(tc->hcode[0]) {
    /* �ʻ줬��ޤä��Τǡ���Ͽ����桼������λ����Ԥ� */
    return(dicTourokuDictionary(d, uuTDicExitCatch, uuTDicQuitCatch));
  }
}

/*
 * ���򤵤줿�ʻ줫�鼡��ư���Ԥ�
 * 
 * tc->hcode	�ʻ�
 * tc->qbuf	����
 * tc->genbuf	���顼
 */
makeHinshi(d)
uiContext	d;
{
  tourokuContext tc = (tourokuContext)d->modec;
  int tlen, ylen, yomi_katsuyou, yomilen, sz, retval = 0;
  unsigned char tmpbuf[100];

  tc->hcode[0] = 0;
  tc->qbuf[0] = 0;
  tc->genbuf[0] = 0;

  tlen = tc->tango_len;
  ylen = tc->yomi_len;

  switch(tc->curHinshi) {
  case 0:  /* ��̾ */
    strcpy(tc->hcode, "#JN");
    break;

  case 1:  /* ��̾ */
    strcpy(tc->hcode, "#CN");
    break;
    
  case 2:  /* ���Ρ����̾ */
    strcpy(tc->hcode, "#KK");
    break;

  case 3:  /* ̾�� �� */

  case 4:  /* ����̾�� �� */
    if(tc->curHinshi == 3)
      strcpy(tc->hcode, "#T35"); /* �ܺ٤��ʻ��ɬ�פȤ��ʤ���� */
    else
      strcpy(tc->hcode, "#T30"); /* �ܺ٤��ʻ��ɬ�פȤ��ʤ���� */
    tc->katsuyou = 0;
    sprintf((char *)tc->qbuf,
	    "��%s�ʡפ��������Ǥ�����(y/n)", (char *)tc->tango_buffer);
    break;

  case 5:  /* ñ���� */
    strcpy(tc->hcode, "#KJ");
    break;

  case 6:  /* ư�� �� */

    /* ���Ϥ����߷����� */
    for(tc->katsuyou=0; tc->katsuyou<GOBISUU &&
	(strncmp(tc->tango_buffer+tlen-2, gyouU[tc->katsuyou], 2) != 0);
							tc->katsuyou++)
      ;
    for(yomi_katsuyou=0; yomi_katsuyou<GOBISUU &&
	(strncmp(tc->yomi_buffer+ylen-2,
		 gyouU[yomi_katsuyou], 2) != 0); yomi_katsuyou++)
      ;
    if((tc->katsuyou == GOBISUU) || (yomi_katsuyou == GOBISUU)){
      strcpy(tc->genbuf, "�ɤߤȸ���� ���߷������Ϥ��Ƥ���������");
      return(0);
    }
    if(tc->katsuyou != yomi_katsuyou){
      strcpy(tc->genbuf,
	     "�ɤߤȸ���� ���Ѥ��㤤�ޤ������Ϥ��ʤ����Ƥ���������");
      return(0);
    }

    /* �㳰���� */
    if(!(strcmp(tc->tango_buffer, "����"))) {
      /* �����ѳʳ��� */
      strcpy(tc->hcode, "#KX");
      return(0);
    }
    if(!(strcmp(tc->tango_buffer+tlen-6, "�󤺤�"))) {
      /* �󥶹��ѳʳ��� */
      strcpy(tc->hcode, "#NZX");
      return(0);
    }
    if(!(strcmp(tc->tango_buffer+tlen-4, "����"))) {
      /* �����ѳʳ��� */
      strcpy(tc->hcode, "#ZX");
      return(0);
    }
    if(!(strcmp(tc->tango_buffer, "����"))) {
      /* �����ѳʳ��� */
      strcpy(tc->hcode, "#SX");
      return(0);
    }

    makeDoushi(d); /* �ܺ٤��ʻ��ɬ�פȤ��ʤ���� */
    /* ̤������Ĥ��� */
    strncpy(tmpbuf, tc->tango_buffer, tlen-2);  
    strcpy((tmpbuf + tlen - 2), gyouA[tc->katsuyou]);
    strcat(tmpbuf, "�ʤ�");
    sprintf((char *)tc->qbuf,
	    "��%s�פ��������Ǥ�����(y/n)", (char *)tmpbuf);
    break;

  case 7:  /* ���ƻ� �� */
    tc->katsuyou = 1;
    if((strncmp(tc->tango_buffer+tlen-2, "��", 2) != 0) ||
       (strncmp(tc->yomi_buffer+ylen-2, "��", 2) != 0)) {
      strcpy(tc->genbuf, "�ɤߤȸ���� ���߷������Ϥ��Ƥ�����������) �ᤤ");
      return(0);
    }
    strcpy(tc->hcode, "#KY"); /* �ܺ٤��ʻ��ɬ�פȤ��ʤ���� */
    strncpy(tmpbuf, tc->tango_buffer, tlen-2);  
    tmpbuf[tlen-2] = 0;
    sprintf((char *)tc->qbuf,
	    "��%s�פ��������Ǥ�����(y/n)", (char *)tmpbuf);
    break;

  case 8:  /* ����ư�� �� */
    tc->katsuyou = 1;
    if((strncmp(tc->tango_buffer+tlen-2, "��", 2)) ||
       (strncmp(tc->yomi_buffer+ylen-2, "��", 2))) {
      strcpy(tc->genbuf,
	     "�ɤߤȸ���� ���߷������Ϥ��Ƥ�����������) �Ť���");
      return(0);
    }
    strcpy(tc->hcode, "#T05"); /* �ܺ٤��ʻ��ɬ�פȤ��ʤ���� */
    strncpy(tmpbuf, tc->tango_buffer, tlen-2);  
    strcpy((tmpbuf + tlen - 2), "����");
    sprintf((char *)tc->qbuf,
	    "��%s�פ��������Ǥ�����(y/n)", (char *)tmpbuf);
    break;

  case 9:  /* ���� ��*/
    strcpy(tc->hcode, "#F14"); /* �ܺ٤��ʻ��ɬ�פȤ��ʤ���� */
    tc->katsuyou = 0;
    sprintf((char *)tc->qbuf,
	    "��%s����פ��������Ǥ�����(y/n)", (char *)tc->tango_buffer);
    break;

  case 10: /* ���� */
    strcpy(tc->hcode, "#NN");
    break;

  case 11: /* ��³�졦��ư�� */
    strcpy(tc->hcode, "#CJ");
    break;

  case 12: /* Ϣ�λ� */
    strcpy(tc->hcode, "#RT");
    break;

  case 13: /* ����¾�θ�ͭ̾�� */
    strcpy(tc->hcode, "#KK");
    break;

  case 14:  /* ���� ���� */

  case 15:  /* ���� ���� */
    sprintf((char *)tc->qbuf,
	    "��%s�ȡפ��������Ǥ�����(y/n)", (char *)tc->tango_buffer);
    break;

  case 16:  /* ư�� ���� */
    strncpy(tmpbuf, tc->tango_buffer, tlen-2);
    tmpbuf[tlen-2] = '\0';
    strcat(tmpbuf, gyouI[tc->katsuyou]);
    strcat(tmpbuf, "������");
    sprintf((char *)tc->qbuf,
	    "��%s�פ��������Ǥ�����(y/n)", (char *)tmpbuf);
    break;

  case 17:  /* ư�� ���� */
    /* �岼���ʳ��Ѥ��� */
    strncpy(tmpbuf, tc->tango_buffer, tlen-2);
    tmpbuf[tlen-2] = '\0';
    strcat(tmpbuf, "������");
    sprintf((char *)tc->qbuf,
	    "��%s�פ��������Ǥ�����(y/n)", (char *)tmpbuf);
    break;

  case 18:  /* ����ư�� ���� */

  case 19:  /* ����ư�� ���� */
    strncpy(tmpbuf, tc->tango_buffer, tlen-2);  
    strcpy((tmpbuf + tlen - 2), "������");
    sprintf((char *)tc->qbuf,
	    "��%s�פ��������Ǥ�����(y/n)", (char *)tmpbuf);
    break;
  }

  return(0);
}

static
tourokuYes(d)
uiContext	d;
{
  tourokuContext tc = (tourokuContext)d->modec;

  tc->hcode[0] = 0;
  tc->qbuf[0] = 0;
  tc->genbuf[0] = 0;

  switch(tc->curHinshi) {
  case 3:  /* ̾�� */
    strcpy(tc->hcode, "#T15");   /* ���������� */
    break;

  case 4:  /* ����̾�� */
    strcpy(tc->hcode, "#T10");          /* �¿����ⵤ */
    break;

  case 6:  /* ư�� */
    tc->curHinshi = 16;
    makeHinshi(d);
    break;

  case 7:  /* ���ƻ� */
    strcpy(tc->hcode, "#KYT");          /* ������ */
    break;

  case 8:  /* ����ư�� */
    tc->curHinshi = 18;
    makeHinshi(d);
    break;

  case 9:  /* ���� ��*/
    tc->curHinshi = 14;
    makeHinshi(d);
    break;

  case 14:  /* ���� ���� */
    strcpy(tc->hcode, "#F04");          /* �դä��� */
    break;

  case 15:  /* ���� ���� */
    strcpy(tc->hcode, "#F06");          /* ���� */
    break;

  case 16:  /* ư�� ���� */
    makeDoushi(d);
    strcat( tc->hcode, "r" );
    break;

  case 17:  /* ư�� ���� */
    strcpy(tc->hcode, "#KS");           /* �ߤ�� */
    break;

  case 18:  /* ����ư�� ���� */
    strcpy(tc->hcode, "#T10");          /* �ؿ��� */
    break;

  case 19:  /* ����ư�� ���� */
    strcpy(tc->hcode, "#T15");          /* �ճ�������ǽ�� */
    break;
  }

  return(0);
}

static
tourokuNo(d)
uiContext	d;
{
  tourokuContext tc = (tourokuContext)d->modec;

  tc->hcode[0] = 0;
  tc->qbuf[0] = 0;
  tc->genbuf[0] = 0;

  switch( tc->curHinshi ) {
  case 3:  /* ̾�� */
    strcpy(tc->hcode, "#T35");   /* ������ */
    break;

  case 4:  /* ����̾�� */
    strcpy(tc->hcode, "#T30");          /* ���ϡ����� */
    break;

  case 6:  /* ư�� */
    tc->curHinshi = 17;
    makeHinshi(d);
    break;

  case 7:  /* ���ƻ� */
    strcpy(tc->hcode, "#KY");           /* ���������ᤤ */
    break;

  case 8:  /* ����ư�� */
    tc->curHinshi = 19;
    makeHinshi(d);
    break;

  case 9:  /* ���� ��*/
    tc->curHinshi = 15;
    makeHinshi(d);
    break;

  case 14:  /* ���� ���� */
    strcpy(tc->hcode, "#F12");          /* ���ä� */
    break;

  case 15:  /* ���� ���� */
    strcpy(tc->hcode, "#F14");          /* ˰���ޤ� */
    break;

  case 16:  /* ư�� ���� */
    makeDoushi(d);
    break;

  case 17:  /* ư�� ���� */
    strcpy(tc->hcode, "#KSr");          /* ������ */
    break;

  case 18:  /* ����ư�� ���� */
    strcpy(tc->hcode, "#T13");          /* ¿���Ƥ� */
    break;

  case 19:  /* ����ư�� ���� */
    strcpy(tc->hcode, "#T18");          /* ���������Ť��� */
    break;
  }

  return(0);
}

static
makeDoushi(d)
uiContext	d;
{
  tourokuContext tc = (tourokuContext)d->modec;

    switch(tc->katsuyou){
    case  0:
      strcpy( tc->hcode, "#K5" );     /* �֤� */
      break;
    case  1:
      strcpy( tc->hcode, "#G5" );     /* �Ĥ� */
      break;
    case  2:
      strcpy( tc->hcode, "#S5" );     /* �֤� */
      break;
    case  3:
      strcpy( tc->hcode, "#T5" );     /* ��� */
      break;
    case  4:
      strcpy( tc->hcode, "#N5" );     /* ��� */
      break;
    case  5:
      strcpy( tc->hcode, "#B5" );     /* ž�� */
      break;
    case  6:
      strcpy( tc->hcode, "#M5" );     /* ���� */
      break;
    case  7:
      strcpy( tc->hcode, "#R5" );     /* ��ĥ�� */
      break;
    case  8:
      strcpy( tc->hcode, "#W5" );     /* ���� */
      break;
    }
}    

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ����ΰ���                                                                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
uuTDicExitCatch(d, retval, env)
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

  return(tangoTouroku(d, cur));
}

static
uuTDicQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* ������ pop */

  popForIchiranMode(d);
  popCallback(d);

  return(dicTourokuHinshi(d));
}

dicTourokuDictionary(d, exitfunc, quitfunc)
uiContext d;
int (*exitfunc)();
int (*quitfunc)();
{
  tourokuContext tc = (tourokuContext)d->modec;
  forichiranContext fc;
  ichiranContext ic;
  unsigned char **work;
  unsigned char inhibit = 0;
  int retval, upnelem = 0;

  retval = d->nbytes = 0;
  d->status = 0;

  for(work = tc->udic; *work; work++)
    upnelem++;

  if((retval = getForIchiranContext(d)) == NG) {
    if(tc->udic)
      free(tc->udic);
    return(GLineNGReturnTK(d));
  }
  fc = (forichiranContext)d->modec;

  /* selectOne ��Ƥ֤���ν��� */
  fc->allkouho = tc->udic;
  fc->curIkouho = 0;
  if( !HexkeySelect )
    inhibit |= ((unsigned char)NUMBERING | (unsigned char)CHARINSERT); 
  else
    inhibit |= (unsigned char)CHARINSERT;

  if((retval = selectOne(d, fc->allkouho, &fc->curIkouho, upnelem,
		 BANGOMAX, inhibit, 0, WITHOUT_LIST_CALLBACK,
		 0, exitfunc, quitfunc, uiUtilIchiranTooSmall)) == NG) {
    if(fc->allkouho)
      free(fc->allkouho);
    popForIchiranMode(d);
    popCallback(d);
    return(GLineNGReturnTK(d));
  }

  ic = (ichiranContext)d->modec;
  ic->majorMode = IROHA_MODE_ExtendMode;
  ic->minorMode = IROHA_MODE_TourokuDicMode;
  currentModeInfo(d);

  /* ��������Ԥ������Ƹ���������Ф��ʤ� */
  if(ic->tooSmall) {
    d->status = AUX_CALLBACK;
    return(retval);
  }

  makeGlineStatus(d);
  /* d->status = ICHIRAN_EVERYTIME; */

  return(retval);
}

/*
 * ñ����Ͽ��Ԥ�
 */
static
tangoTouroku(d, cur)
uiContext	d;
int             cur;
{
  tourokuContext tc = (tourokuContext)d->modec;
  char ktmpbuf[256];
  char ttmpbuf[256];
  unsigned char line[ROMEBUFSIZE];
  
  if(tc->katsuyou || (strncmp(tc->hcode, "#K5", 3) == 0)) {
#ifdef DEBUG
  if (iroha_debug) {
      fprintf(stderr, "ư��");
  }
#endif
    strncpy(ttmpbuf, tc->tango_buffer, tc->tango_len - 2);
    ttmpbuf[tc->tango_len - 2] = 0;
    strncpy(ktmpbuf, tc->yomi_buffer, tc->yomi_len - 2);
    ktmpbuf[tc->yomi_len - 2] = 0;
  } else {
#ifdef DEBUG
  if (iroha_debug) {
      fprintf(stderr, "ư��ʳ�");
  }
#endif
    strcpy(ttmpbuf, tc->tango_buffer);
    strcpy(ktmpbuf, tc->yomi_buffer);
  }

  /* ����񤭹����Ѥΰ�Ԥ��� */
  sprintf((char *)line, "%s %s %s", ktmpbuf, (char *)tc->hcode, ttmpbuf);

#ifdef DEBUG
  if (iroha_debug) {
      fprintf(stderr, "��Ͽ������(%s) ����(%s)\n", line, tc->udic[cur]);
  }
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
  /* �������Ͽ���� */
  if(RkDefineDic(defaultContext, tc->udic[cur], line) != 0) {
    if(errno == EPIPE)
      necKanjiPipeError();
    strcpy(d->genbuf, "ñ����Ͽ�Ǥ��ޤ���Ǥ���");
  } else {
    /* ��Ͽ�δ�λ��ɽ������ */
    sprintf((char *)d->genbuf, "��%s�١�%s�ˤ���Ͽ���ޤ���",
	    (char *)tc->tango_buffer, (char *)tc->yomi_buffer);
  }
  makeGLineMessage(d, d->genbuf, strlen(d->genbuf));

  popTourokuMode(d);
  popCallback(d);
  currentModeInfo(d);

  return(0);
}
