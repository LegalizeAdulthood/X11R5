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

static	char	rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/kigo.c,v 3.2 91/06/28 18:14:13 kon Exp $";

#include	"iroha.h"

extern SelectDirect;

static unsigned char *kuhaku = (unsigned char *)"��";   /* ��������Ԥζ��� */
static unsigned char *kakko = (unsigned char *)"�Ρ�";  /* �����̤�ʸ���� */

#define BYTE1		84	/* JIS������ɽ�����Х��Ȥο� */
#define BYTE2		94	/* JIS������ɽ������Х��Ȥο� */
#define KIGOSU		(((BYTE1 - 1) * BYTE2) + 4)
    				/* �������� */
#define KAKKOSIZE	2	/* JIS������ɽ���ѳ�̤ΥХ��ȿ� */
#define KCODESIZE	4	/* JIS������ɽ�����Τ�ΤΥХ��ȿ� */
#define KCODEALLSIZE	((KAKKOSIZE * 2) + KCODESIZE)
     				/* JIS������ɽ�����ΤΥХ��ȿ� 8 */

static int kigo_curIkouho, KigoKakutei();

/* cfunc ichiranContext
 *
 * ichiranContext
 *
 */
static ichiranContext
newKigoContext()
{
  ichiranContext kcxt;

  if((kcxt = (ichiranContext)malloc(sizeof(ichiranContextRec))) == NULL) {
    necKanjiError = "malloc (newKigoContext) �Ǥ��ޤ���Ǥ���";
    return(0);
  }
  kcxt->id = ICHIRAN_CONTEXT;

  return kcxt;
}

static void
freeKigoContext(kc)
ichiranContext kc;
{
  free(kc);
}

/*
 * ��������Ԥ���
 */
static
getKigoContext(d,
	  everyTimeCallback, exitCallback, quitCallback, auxCallback)
uiContext d;
int (*everyTimeCallback)(), (*exitCallback)();
int (*quitCallback)(), (*auxCallback)();
{
  extern KanjiModeRec kigo_mode;
  ichiranContext kc;
  int retval = 0;

  if(pushCallback(d, d->modec,
	everyTimeCallback, exitCallback, quitCallback, auxCallback) == 0) {
    necKanjiError = "malloc (pushCallback) �Ǥ��ޤ���Ǥ���";
    return(NG);
  }
  
  if((kc = newKigoContext()) == NULL) {
    popCallback(d);
    return(NG);
  }
  kc->next = d->modec;
  d->modec = (mode_context)kc;

  kc->prevMode = d->current_mode;
  d->current_mode = &kigo_mode;

  return(retval);
}

static void
popKigoMode(d)
uiContext d;
{
  ichiranContext kc = (ichiranContext)d->modec;

  d->modec = kc->next;
  d->current_mode = kc->prevMode;
  freeIchiranContext(kc);
}

/*
 * ��������Ԥ˴ؤ��빽¤�Τ����Ƥ򹹿�����
 *
 * �������ȸ���ˤ�ä� kouhoinfo �� glineinfo �����������Ԥ���
 * �������ȸ���Υ����ɤ򥭥�饯�����Ѵ�����
 *
 * ������	RomeStruct
 * �����	�ʤ�
 */
static
makeKigoGlineStatus(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;
  unsigned char *kptr, *gptr;
  int  i, b1, b2;

  kptr = kc->kouhoifp[*(kc->curIkouho)].khdata;
  gptr = kc->glineifp->gldata + KAKKOSIZE;
  
  /* �����ȵ����JIS�����ɤ�����Ԥ���Υ��å��������� */
  for(i=0; i<KIGOSIZE; i++, kptr++) {
    b1 = (int)((unsigned int)(*kptr & 0x7f) >> 4); /* ���Х����� */
    b2 = *kptr & 0x0f;			/* ���Х����� */
    b1 += ((b1 > 0x09) ? ('a' - 10) : '0');	/* ʸ���ˤ��� */
    b2 += ((b2 > 0x09) ? ('a' - 10) : '0');	/* ʸ���ˤ��� */
    *gptr++ = b1;
    *gptr++ = b2;
  }

  d->kanji_status_return->info |= KanjiGLineInfo;
  d->kanji_status_return->gline.line = kc->glineifp->gldata;
  d->kanji_status_return->gline.length = kc->glineifp->gllen;
  d->kanji_status_return->gline.revPos =
    kc->kouhoifp[*(kc->curIkouho)].khpoint;
  d->kanji_status_return->gline.revLen = KIGOSIZE;

}

/* ��������Ѥ�glineinfo��kouhoinfo����
 *
 * ��glineinfo��
 *    int glkosu   : int glhead     : int gllen  : unsigned char *gldata
 *    ���Ԥθ���� : ��Ƭ���椬     : ���Ԥ�Ĺ�� : ��������Ԥ�ʸ����
 *                 : �����ܤε��椫 :
 * -------------------------------------------------------------------------
 * 0 | 6           : 0              : 24         : ������������������������
 *
 *  ��kouhoinfo��
 *    int khretsu  : int khpoint  : unsigned char *khdata
 *    ̤����       : �Ԥ���Ƭ���� : �����ʸ��
 *                 : ���Х����ܤ� :
 * -------------------------------------------------------------------------
 * 0 | 0           : 0            : ��
 * 1 | 0           : 4            : ��
 * 2 | 0           : 8            : ��
 *          :               :
 *
 * ������	headkouho	�����ȵ������Ԥ���Ƭ����ΰ���
 *					(2121���鲿���ܤ�(2121�ϣ�����))
 *		uiContext
 * �����	���ｪλ�� 0
 */
static
makeKigoInfo(d, headkouho)
uiContext	d;
int		headkouho;
{
  ichiranContext kc = (ichiranContext)d->modec;
  unsigned char *gptr;
  int  i, b1, b2, lnko, cn;
  unsigned char byte1hex = 0xa1;
  unsigned char byte2hex = 0xa1;

  b2 = headkouho % BYTE2;	/* JIS������ɽ��(�ؼ�)�ΰ��� (��-1) */
  b1 = headkouho / BYTE2;	/* JIS������ɽ��(�ټ�)�ΰ��� (��-1) */

#ifdef DEBUG
  if (iroha_debug) {
    printf("kigoinfo = bangomax %d, b1 %d, b2 %d\n", kc->nIkouho, b1, b2);
    printf("kigoinfo = headkouho %d, curIkouho %d\n",
	   headkouho, *(kc->curIkouho));
  }
#endif

  /* ��������Ѥ�glineinfo��kouhoinfo���� */
  gptr = kc->glinebufp;

  kc->glineifp->glhead = headkouho;
  kc->glineifp->gldata = gptr;

  /* JIS�����ɤ�ɽ���ΰ���������˺�� */
  strncpy(gptr, kakko, KAKKOSIZE);
  gptr += KAKKOSIZE;
  for(i=0; i<KCODESIZE; i++)
    *gptr++ = ' ';		/* ���ߡ�������� */
  strncpy(gptr, (kakko + KAKKOSIZE), KAKKOSIZE);
  gptr += KAKKOSIZE;

  for(cn=KCODEALLSIZE, lnko=0;
      b1<BYTE1 && lnko<kc->nIkouho && (headkouho+lnko)<KIGOSU ; b1++) {
    for(; b2<BYTE2 && lnko<kc->nIkouho && (headkouho+lnko)<KIGOSU; b2++, lnko++) {
      
      /* ���ڤ�ˤʤ����򥳥ԡ����� */
      if(lnko != 0) {
	strncpy(gptr, kuhaku, KG_KUHAKUSIZE);
	gptr += KG_KUHAKUSIZE;
	cn += KG_KUHAKUSIZE;
      }

      kc->kouhoifp[lnko].khpoint = cn;
      kc->kouhoifp[lnko].khdata = gptr;
      
      /* ����򥳥ԡ����� */
      *gptr++ = byte1hex + (unsigned char)b1;
      *gptr++ = byte2hex + (unsigned char)b2;

      cn += KIGOSIZE;
    }
    b2 = 0;
  }
  *gptr = (unsigned char)NULL;
  kc->glineifp->glkosu = lnko;
  kc->glineifp->gllen = strlen(kc->glineifp->gldata);

  return(0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * �������                                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static
kigoIchiranExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  retval = YomiExit(d, retval);
  currentModeInfo(d);

  return(retval);
}

static
kigoIchiranQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  currentModeInfo(d);

  return(0);
}

static
uuKigoIchiranQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  currentModeInfo(d);

  return(uiUtil2Mode(d, 0));
}

KigoIchiran(d)
uiContext	d;
{
  yomiContext yc = (yomiContext)d->modec;

  if (yc->generalFlags & IROHA_YOMI_CHGMODE_INHIBITTED) {
    return NothingChangedWithBeep(d);
  }    
  
  if(makeKigoIchiran(d, 1) == NG)
    return(GLineNGReturn(d));
  else
    return(0);
}

/*
 * ��������Ԥ�ɽ������
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
makeKigoIchiran(d, flag)
uiContext	d;
int             flag;
{
  ichiranContext kc;
  int            headkouho, retval = 0;
  unsigned char  inhibit = 0;

  if(d->ncolumns < (KCODEALLSIZE + KIGOSIZE)) {
    NothingChangedWithBeep(d);
    necKanjiError = "��������Ѥ����������Τǵ�������Ǥ��ޤ���";
    return(NG);
  }

  if(flag) { /* ������� */
    if((retval = getKigoContext(d,
		0, kigoIchiranExitCatch, kigoIchiranQuitCatch, 0)) == NG) {
      return(NG);
    }
    kc = (ichiranContext)d->modec;
    kc->majorMode = kc->minorMode = IROHA_MODE_KigoMode;
  } else { /* ��ĥ�ε������ */
    if((retval = getKigoContext(d,
		0, kigoIchiranExitCatch, uuKigoIchiranQuitCatch, 0)) == NG) {
      return(NG);
    }
    kc = (ichiranContext)d->modec;
    kc->majorMode = IROHA_MODE_ExtendMode;
    kc->minorMode = IROHA_MODE_KigoMode;
  }
  currentModeInfo(d);

  /* ���絭��ɽ�����Υ��å� */
  /* ���������� "��JIS ��" ʬ�򺹤������Ʒ׻����� */
  if((kc->nIkouho =
      (((d->ncolumns - KCODEALLSIZE - KIGOSIZE) / KIGOCOL) + 1))
                                                  > KIGOBANGOMAX) {
    kc->nIkouho = KIGOBANGOMAX;
  }

  kc->curIkouho = &kigo_curIkouho;
  inhibit |= (unsigned char)NUMBERING;

  if(allocIchiranBuf(d, inhibit, 0) == NG) { /* ��������⡼�� */
    popKigoMode(d);
    popCallback(d);
    return(NG);
  }

  /* �����ȸ���Τ��뵭������Ԥ���Ƭ����ȡ�
     ��������Υ����ȸ���ΰ��֤���� */
  if(d->curkigo) {		/* a1a1���鲿���ܤε��椫 */
    headkouho = (d->curkigo / kc->nIkouho) * kc->nIkouho;
    *(kc->curIkouho) = d->curkigo % kc->nIkouho;
  } else {
    d->curkigo = 0;
    headkouho = 0;
    *(kc->curIkouho) = 0;
  }

  /* glineinfo��kouhoinfo���� */
  makeKigoInfo(d, headkouho);

  /* kanji_status_return���� */
  makeKigoGlineStatus(d);

  return(0);
}

/*
 * �����������μ��ε���˰�ư����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0
 */
static
KigoForwardKouho(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;
  int  headkouho;

  /* ���ε���ˤ��� */
  ++*(kc->curIkouho);
  
  /* ����ɽ���κǸ�ε�����ä��顢���ΰ����Ԥ���Ƭ����򥫥��ȵ���Ȥ��� */
  if((*(kc->curIkouho) >= kc->nIkouho) ||
     (kc->glineifp->glhead + *(kc->curIkouho) >= KIGOSU)) {
    headkouho  = kc->glineifp->glhead + kc->nIkouho;
    if(headkouho >= KIGOSU)
      headkouho = 0;
    *(kc->curIkouho) = 0;
    makeKigoInfo(d, headkouho);
  }

  /* kanji_status_retusrn ���� */
  makeKigoGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return(0);
}

/*
 * ���������������ε���˰�ư����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0
 */
static
KigoBackwardKouho(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;
  int  headkouho;

  /* ���ε���ˤ��� */
  --*(kc->curIkouho);

  /* ����ɽ������Ƭ�ε�����ä��顢���ΰ����Ԥκǽ�����򥫥��ȵ���Ȥ��� */
  if(*(kc->curIkouho) < 0) {
    headkouho  = kc->glineifp->glhead - kc->nIkouho;
    if(headkouho < 0)
      headkouho = ((KIGOSU - 1) / kc->nIkouho) * kc->nIkouho;
    makeKigoInfo(d, headkouho);
    *(kc->curIkouho) = kc->glineifp->glkosu - 1;
  }

  /* kanji_status_retusrn ���� */
  makeKigoGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return(0);
}

/*
 * �����������˰�ư����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0
 */
static
KigoPreviousKouhoretsu(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;
  int headkouho;

  /** ��������ˤ��� **/
  headkouho  = kc->glineifp->glhead - kc->nIkouho;
  if(headkouho < 0)
    headkouho = ((KIGOSU -1) / kc->nIkouho) * kc->nIkouho;
  makeKigoInfo(d, headkouho);

  /* *(kc->curIkouho) �������ȵ�������ε��������礭���ʤäƤ��ޤä���
     �Ǳ�����򥫥��ȸ���Ȥ��� */
  if(*(kc->curIkouho) >= kc->glineifp->glkosu)
    *(kc->curIkouho) = kc->glineifp->glkosu - 1;

  /* kanji_status_retusrn ���� */
  makeKigoGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return(0);
}

/*
 * �����������˰�ư����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0
 */
static
KigoNextKouhoretsu(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;
  int headkouho;

  /** ��������ˤ��� **/
  headkouho = kc->glineifp->glhead + kc->nIkouho;
  if(headkouho >= KIGOSU)
    headkouho = 0;
  makeKigoInfo(d, headkouho);

  /* *(kc->curIkouho) �������ȵ�������ε��������礭���ʤäƤ��ޤä���
     �Ǳ�����򥫥��ȸ���Ȥ��� */
  if(*(kc->curIkouho) >= kc->glineifp->glkosu)
    *(kc->curIkouho) = kc->glineifp->glkosu - 1;

  /* kanji_status_retusrn ���� */
  makeKigoGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return(0);
}

/*
 * ��������������Ƭ�ε���˰�ư����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0
 */
static
KigoBeginningOfKouho(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;

  /* ���������Ƭ����򥫥��ȸ���ˤ��� */
  *(kc->curIkouho) = 0;

  /* kanji_status_retusrn ���� */
  makeKigoGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return(0);
}

/*
 * �����������κǱ��ε���˰�ư����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0
 */
static
KigoEndOfKouho(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;

  /** ������κǱ�����򥫥��ȸ���ˤ��� **/
  *(kc->curIkouho) = kc->glineifp->glkosu - 1;

  /* kanji_status_retusrn ���� */
  makeKigoGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return(0);
}

/*
 * ���������������Ϥ��줿�ֹ�ε���˰�ư����  ��̤���ѡ�
 *
 * ������	uiContext
 * �����	���ｪλ�� 0
 */
static
KigoBangoKouho(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;
  int num, index;

  /* ���ϥǡ����� ������ ����� ���� */
  if(((0x30 <= d->ch) && (d->ch <= 0x39))
     || ((0x61 <= d->ch) && (d->ch <= 0x66))) {
    if((0x30 <= d->ch) && (d->ch <= 0x39))
      num = (int)(d->ch & 0x0f);
    else if((0x61 <= d->ch) && (d->ch <= 0x66))
      num = (int)(d->ch - 0x57);
  } 
  else {
    /* ���Ϥ��줿�ֹ������������ޤ��� */
    return NothingChangedWithBeep(d);
  }
  /* ���ϥǡ����� ����Ԥ����¸�ߤ�������� */
  if(num >= kc->glineifp->glkosu) {
    /* ���Ϥ��줿�ֹ������������ޤ��� */
    return NothingChangedWithBeep(d);
  }

  /* ���������Ƭ��������� */
  *(kc->curIkouho) = num;

  /* SelectDirect �Υ������ޥ����ν��� */
  if(SelectDirect) /* ON */ {

    return(KigoKakutei(d));
  } else           /* OFF */ {
    /* kanji_status_retusrn ���� */
    makeKigoGlineStatus(d);

    return(0);
  }
}

/*
 * ����������椫�����򤵤줿�������ꤹ��
 *
 * �����˵��������������������ꤷ�����椬�����ȸ���Ȥʤ�褦�ˡ�
 *   ���ꤷ������򥻡��֤��Ƥ���
 *
 * ������	uiContext
 * �����	���ｪλ�� 0
 */
static
KigoKakutei(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;

  /* �����ȵ���򥻡��֤��� */
  d->curkigo = kc->glineifp->glhead + *(kc->curIkouho);

  /* ���������ȥ�󥰤����ʸ����Ȥ��� */
  if (d->bytes_buffer >= KIGOSIZE) {
    d->nbytes = KIGOSIZE;
    strncpy(d->buffer_return, kc->kouhoifp[*(kc->curIkouho)].khdata, 
	    d->nbytes);
    d->buffer_return[KIGOSIZE] = (unsigned char)NULL;
  }
  else {
    d->nbytes = 0;
  }

  popKigoMode(d);
  GlineClear(d);

  d->status = EXIT_CALLBACK;

  return(d->nbytes);
}

/*
 * ��������Ԥ�õ��
 *
 * ������	uiContext
 * �����	���ｪλ�� 0
 */
static
KigoQuit(d)
uiContext	d;
{
  ichiranContext kc = (ichiranContext)d->modec;

  popKigoMode(d);

  /* gline �򥯥ꥢ���� */
  GlineClear(d);

  d->status = QUIT_CALLBACK;

  return(0);
}

#include	"kigomap.c"
