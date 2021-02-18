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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/jishu.c,v 3.3 91/09/05 20:29:53 kon Exp $";

#include "iroha.h"
#include <ctype.h>

static int setInhibitInformation(), JishuNextJishu(), JishuPreviousJishu();
static int jishuAdjustRome(), myjishuAdjustRome(), JishuZenkaku();
static int JishuHankaku(), YomiJishu();

/* jc->kc          ����ʸ���狼
 * d->jishu_rEndp
 * d->jishu_kEndp
 * ����� 
 *               ������sh|
 * C-            ������sh|
 * C-            ������s|h
 * C-            ������|sh
 * C-            ����|��sh
 *
 *               ������sh|
 * C-            aishish|
 * C-            ��������|
 * C-            �������|h
 * C-            ������|sh
 * C-            �����|��sh
 * C-            aish|��sh
 * C-            ����sh|��sh
 * C-            ����sh|sh
 * C-            ����s|��sh
 * C-            ����|��sh
 * C-
 * 
 */

#define	INHIBIT_HANKATA	01
#define	INHIBIT_KANA	02
#define INHIBIT_ALPHA	04

static jishuContext
pushJishuMode(d, yc, len, ev, ex, qu, au)
uiContext d;
yomiContext yc;
int len;
int (*ev)(), (*ex)(), (*qu)(), (*au)();
{
  struct callback *pushCallback();
  void popJishuMode();
  jishuContext jc;
  extern KanjiModeRec jishu_mode;

  jc = (jishuContext)malloc(sizeof(jishuContextRec));
  if (jc) {
    jc->id = JISHU_CONTEXT;
    jc->next = (mode_context)d->modec;
    jc->ycx = yc;
    jc->kc = JISHU_HIRA;/* ���ϤҤ餬�ʥ⡼�ɤǤ� */
    jc->jishu_case = 0; /* ��ʸ���١����Υ⡼�ɤǤ� */
    setInhibitInformation(jc);
    jc->jishu_rEndp = yc->rEndp;/* ���޻��Хåե���Ĺ���򥳥ԡ� */
    jc->jishu_kEndp = yc->kEndp;/* ���ʥХåե���Ĺ���򥳥ԡ� */
    jc->majorMode = d->majorMode;
    d->modec = (mode_context)jc;
    if (pushCallback(d, yc, ev, ex, qu, au) == (struct callback *)0) {
      popJishuMode(d);
      return 0;
    }
    jc->prevMode = d->current_mode;
    d->current_mode = &jishu_mode;
  }
  return jc;
}

void
freeJishuContext(jc)
jishuContext jc;
{
  free(jc);
}

void
popJishuMode(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;

  d->modec = jc->next;
  d->current_mode = jc->prevMode;
  freeJishuContext(jc);
}

static
setInhibitInformation(jc)
jishuContext jc;
{
  int i;
  extern InhibitHankakuKana;
  yomiContext yc = jc->ycx;

  jc->inhibition = InhibitHankakuKana ? INHIBIT_HANKATA : 0;
  for (i = 0 ; i < yc->kEndp ; i++) {
    if ( !(yc->kAttr[i] & HENKANSUMI) ) {
      jc->inhibition |= INHIBIT_KANA;
      break;
    }
  }
  for (i = 0 ; i < yc->rEndp ; i++) {
    if (yc->romaji_buffer[i] & 0x80) {
      jc->inhibition |= INHIBIT_ALPHA;
    }
  }
}

static
exitJishu(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  /* �������鲼�ϴ����ʡ��ɤߡ٥⡼�� */
  retval = YomiExit(d, retval);
  currentModeInfo(d);
  return retval;
}

static
quitJishu(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  extern KanjiModeRec yomi_mode;

  popCallback(d);
  makeYomiReturnStruct(d);
/*  d->current_mode = &yomi_mode; */
  currentModeInfo(d);
  return retval;
}

YomiNextJishu(d) /* �ɤߥ⡼�ɤ���ν���ʸ�����Ѵ� */
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;
  jishuContext jc;
  int ret;

  if (yc->henkanInhibition & IROHA_YOMI_INHIBIT_JISHU) {
    return NothingChangedWithBeep(d);
  }
  jc = pushJishuMode(d, yc, RomajiFlushYomi(d, yc->kana_buffer, ROMEBUFSIZE),
		     NO_CALLBACK, exitJishu, quitJishu, NO_CALLBACK);
  if (jc == (jishuContext)0) {
    return NoMoreMemory();
  }
  ret = JishuNextJishu(d);
  jc->minorMode = IROHA_MODE_JishuMode;
  currentModeInfo(d);
  return ret;
}

YomiPreviousJishu(d) /* �ɤߥ⡼�ɤ���εղ��ʸ�����Ѵ� */
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;
  jishuContext jc;
  int ret;

  if (yc->henkanInhibition & IROHA_YOMI_INHIBIT_JISHU) {
    return NothingChangedWithBeep(d);
  }
  jc = pushJishuMode(d, yc, RomajiFlushYomi(d, yc->kana_buffer, ROMEBUFSIZE),
		     NO_CALLBACK, exitJishu, quitJishu, NO_CALLBACK);
  if (jc == (jishuContext)0) {
    return NoMoreMemory();
  }
  ret = JishuPreviousJishu(d);
  jc->minorMode = IROHA_MODE_JishuMode;
  currentModeInfo(d);
  return ret;
}

int YomiJishu();

YomiZenkaku(d) /* �ɤߥ⡼�ɤ���������Ѵ� */
uiContext d;
{
  int JishuZenkaku();

  return YomiJishu(d, JishuZenkaku);
}

YomiHankaku(d) /* �ɤߥ⡼�ɤ����Ⱦ���Ѵ� */
uiContext d;
{
  int JishuHankaku();
  extern InhibitHankakuKana;

  if (InhibitHankakuKana)
    return NothingChangedWithBeep(d);
  else
    return YomiJishu(d, JishuHankaku);
}

static
jishuHenkan(d) /* �����λ���˻�����Ѵ�������� */
uiContext d;
{
  char xxxx[1024];
  int len, revlen;
  int n, xferflag;
  jishuContext jc = (jishuContext)d->modec;
  yomiContext yc = jc->ycx;

  switch (jc->kc)
    {
    case JISHU_ZEN_KATA: /* ���ѥ������ʤ��Ѵ����� */
      len = RkCvtZen(xxxx, 1024, yc->kana_buffer, jc->jishu_kEndp);
      revlen = RkCvtKana(d->genbuf, ROMEBUFSIZE, xxxx, len);
      break;

    case JISHU_HAN_KATA: /* Ⱦ�ѥ������ʤ��Ѵ����� */
      len = RkCvtKana(xxxx, 1024, yc->kana_buffer, jc->jishu_kEndp);
      revlen = RkCvtHan(d->genbuf, ROMEBUFSIZE, xxxx, len);
      break;

    case JISHU_HIRA: /* �Ҥ餬�ʤ��Ѵ����� */
      len = RkCvtZen(xxxx, 1024, yc->kana_buffer, jc->jishu_kEndp);
      revlen = RkCvtHira(d->genbuf, ROMEBUFSIZE, xxxx, len);
      break;

    case JISHU_ZEN_ALPHA: /* ���ѱѿ����Ѵ����� */
      if (jc->jishu_case == IROHA_JISHU_UPPER) {
	int i;
	unsigned char *p = yc->romaji_buffer;

	for (i = 0 ; i < jc->jishu_rEndp ; i++) {
	  if ('a' <= p[i] && p[i] <= 'z') {
	    xxxx[i] = toupper(p[i]);
	  }
	  else {
	    xxxx[i] = p[i];
	  }
	}
	revlen = RkCvtZen(d->genbuf, ROMEBUFSIZE, xxxx, jc->jishu_rEndp);
      }
      else if (jc->jishu_case == IROHA_JISHU_CAPITALIZE) {
	strncpy(xxxx, yc->romaji_buffer, jc->jishu_rEndp);
	if ('a' <= xxxx[0] && xxxx[0] <= 'z') {
	  xxxx[0] = toupper(xxxx[0]);
	}
	revlen = RkCvtZen(d->genbuf, ROMEBUFSIZE, xxxx, jc->jishu_rEndp);
      }
      else {
	revlen = RkCvtZen(d->genbuf, ROMEBUFSIZE,
			  yc->romaji_buffer, jc->jishu_rEndp);
      }
      break;

    case JISHU_HAN_ALPHA: /* Ⱦ�ѱѿ����Ѵ����� */
      revlen = jc->jishu_rEndp;
      if (jc->jishu_case == IROHA_JISHU_UPPER) {
	int i;
	unsigned char *p = yc->romaji_buffer;

	for (i = 0 ; i < revlen ; i++) {
	  if ('a' <= p[i] && p[i] <= 'z') {
	    d->genbuf[i] = toupper(p[i]);
	  }
	  else {
	    d->genbuf[i] = p[i];
	  }
	}
      }
      else if (jc->jishu_case == IROHA_JISHU_CAPITALIZE) {
	strncpy(d->genbuf, yc->romaji_buffer, revlen);
	d->genbuf[0] = toupper(yc->romaji_buffer[0]);
      }
      else {
	strncpy(d->genbuf, yc->romaji_buffer, revlen);
      }
      break;

    default:/* �ɤ�Ǥ�ʤ��ä����Ѵ�����ʤ��Τǲ��⤷�ʤ� */
      break;
    }

/* ʸ�����Ѵ����ʤ���ʬ���դ��ä��� */
  switch (jc->kc)
    {
    case JISHU_HIRA: /* �Ҥ餬�ʤʤ� */
    case JISHU_ZEN_KATA: /* ���ѥ������ʤʤ� */
    case JISHU_HAN_KATA: /* Ⱦ�ѥ������ʤʤ� */
      strncpy(d->genbuf + revlen, &yc->kana_buffer[jc->jishu_kEndp],
	      yc->kEndp - jc->jishu_kEndp);
                                      /* ���ʥХåե�����ʸ�������Ф� */
      len = revlen + yc->kEndp - jc->jishu_kEndp;
      break;

    case JISHU_ZEN_ALPHA: /* ���ѱѿ��ʤ� */
    case JISHU_HAN_ALPHA: /* Ⱦ�ѱѿ��ʤ� */
      xferflag = RK_XFER;
      switch (yc->baseChar) {
      case JISHU_ZEN_KATA:
	xferflag = RK_KFER;
      case JISHU_HIRA:
	len = RkCvtRoma(romajidic, d->genbuf + revlen, ROMEBUFSIZE - revlen,
			yc->romaji_buffer + jc->jishu_rEndp,
			yc->rEndp - jc->jishu_rEndp,
			RK_FLUSH | RK_SOKON | xferflag);
	break;
      case JISHU_HAN_KATA:
	len = RkCvtRoma(romajidic, xxxx, 1024,
			yc->romaji_buffer + jc->jishu_rEndp,
			yc->rEndp - jc->jishu_rEndp,
			RK_FLUSH | RK_SOKON | RK_KFER);
	len = RkCvtHan(d->genbuf + revlen, ROMEBUFSIZE - revlen,
		       xxxx, len);
	break;
      case JISHU_ZEN_ALPHA:
	len = RkCvtZen(d->genbuf + revlen, ROMEBUFSIZE - revlen,
		       yc->romaji_buffer + jc->jishu_rEndp,
		       yc->rEndp - jc->jishu_rEndp);
	break;
      case JISHU_HAN_ALPHA:
	len = RkCvtNone(d->genbuf + revlen, ROMEBUFSIZE - revlen,
			yc->romaji_buffer + jc->jishu_rEndp,
			yc->rEndp - jc->jishu_rEndp);
	break;
      }
      len += revlen;
      break;
    default:/* �ɤ�Ǥ�ʤ��ä��鲿�⤷�ʤ� */
      break;
    }
  d->genbuf[len] = '\0';
  d->kanji_status_return->echoStr = (unsigned char *)d->genbuf;
  d->kanji_status_return->length  = len;
  d->kanji_status_return->revLen  = revlen;
  d->kanji_status_return->revPos  = 0;

  return revlen;
}

static
inhibittedJishu(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;

  return (((jc->kc == JISHU_ZEN_KATA || jc->kc == JISHU_HAN_KATA) &&
	   (jc->inhibition & INHIBIT_KANA)) ||
	  ((jc->kc == JISHU_ZEN_ALPHA || jc->kc == JISHU_HAN_ALPHA) &&
	   (jc->inhibition & INHIBIT_ALPHA)) ||
	  ((jc->kc == JISHU_HAN_KATA) && 
	   (jc->inhibition & INHIBIT_HANKATA))
	  );
}

static
void
nextJishu(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;

  do {
    jc->kc = (unsigned char)(((int)jc->kc + 1) % MAX_JISHU);
  } while (inhibittedJishu(d));
}

static
void
previousJishu(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;

  do {
    jc->kc = (unsigned char)(((int)jc->kc + MAX_JISHU - 1) % MAX_JISHU);
  } while (inhibittedJishu(d));
}
	    

static
JishuNextJishu(d) /* ����⡼�ɤλ��˽���ʸ�����Ѵ��򤹤� */
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;
  yomiContext yc = jc->ycx;

#ifdef DEBUG
    if (iroha_debug) {
      fprintf(stderr, "ʸ�����Ѵ��λϤ�");
      fprintf(stderr, "jc->kc(%d)\n", jc->kc);
    }
#endif

/* ���Ф���ʸ������Ѵ����� */
    nextJishu(d);
    if (jc->kc == JISHU_HIRA) {
      if (jc->jishu_kEndp == yc->kEndp && jc->jishu_rEndp == yc->rEndp) {
	popJishuMode(d);
	d->status = QUIT_CALLBACK;
	return 0;
      }
    }

#ifdef DEBUG
    if (iroha_debug) {
      fprintf(stderr, "ʸ�����Ѵ��ν����");
      fprintf(stderr, "jc->kc(%d)\n", jc->kc);
    }
#endif

  jishuHenkan(d); /* ������Ѵ� */
  return 0;
}

static
JishuPreviousJishu(d) /* ����⡼�ɤλ��˵ղ��ʸ�����Ѵ��򤹤� */
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;
  yomiContext yc = jc->ycx;

/* ���Ф���ʸ������Ѵ����� */
  previousJishu(d);
  if (jc->kc == JISHU_HIRA) {
    if (jc->jishu_kEndp == yc->kEndp && jc->jishu_rEndp == yc->rEndp) {
      popJishuMode(d);
      d->status = QUIT_CALLBACK;
      return 0;
    }
  }
  jishuHenkan(d);
  return 0;
}

static
JishuShrink(d) /* ʸ�����Ѵ��ΰ��̤�� */
uiContext d;
{  
  int f;
  jishuContext jc = (jishuContext)d->modec;
  yomiContext yc = jc->ycx;

  f = (yc->kana_buffer[jc->jishu_kEndp - 1]&0x80)?2:1;
               /* �������������ʸ���κǺ��ӥåȤ�Ω�äƤ����� */
               /* jc->jishu_kEndp �ϡ����У��ˤʤ�ʤ����顢�Ǥ��롪�� */ 

  /* ��Υݥ��󥿤��᤹ */
  switch (jc->kc)
    {
    case JISHU_ZEN_ALPHA:
    case JISHU_HAN_ALPHA: /* ���ѱѿ�����Ⱦ�ѱѿ����ʤ� */
      myjishuAdjustRome(d);
      jc->jishu_rEndp--; /* ������޻��Хåե�����ǥå������᤹ */
      if (yc->rAttr[jc->jishu_rEndp] & SENTOU) {
	                       /* ���޻������Ѵ���Ƭ�ե饰�Хåե���
				* Ω�äƤ�����
			        */
	for (--jc->jishu_kEndp ; 
	     jc->jishu_kEndp > 0 && !(yc->kAttr[jc->jishu_kEndp] & SENTOU) ;
	     --jc->jishu_kEndp);
	                       /* �����Ѵ������ե饰�Хåե�����Ƭ��
				* Ω�äƤ����ޤ�
				* ���狼�ʥХåե�����ǥå�����
				* �᤹
			        */
      }
      break;
    case JISHU_HIRA:
    case JISHU_ZEN_KATA:
    case JISHU_HAN_KATA: /* �Ҥ餬�ʤ����ѥ������ʤ�Ⱦ�ѥ������ʤʤ� */
      jishuAdjustRome(d);
      jc->jishu_kEndp -= f; /* ���狼�ʥХåե�����ǥå�����
	                    * ��ʸ��ʬ�᤹ */
      if (yc->kAttr[jc->jishu_kEndp] & SENTOU) {
                               /* �����Ѵ������ե饰�Хåե���
				* Ω�äƤ�����
			        */
	for (--jc->jishu_rEndp ; 
	     jc->jishu_rEndp > 0 && !(yc->rAttr[jc->jishu_rEndp] & SENTOU) ;
	     --jc->jishu_rEndp);
	                       /* ���޻������Ѵ���Ƭ�ե饰�Хåե���
				* Ω�äƤ����ޤ�
				* ������޻��Хåե�����ǥå�����
				* �᤹
			        */
      }
      break;
    }

  if(jc->jishu_rEndp <= 0) {/* �������������Хåե�����ǥå�����
			    * ����Ĺ�����᤹
			    */
    jc->jishu_kEndp = yc->kEndp;
    jc->jishu_rEndp = yc->rEndp;
  }
  jishuHenkan(d);
  return 0;
}

static
JishuExtend(d) /* ʸ�����Ѵ��ΰ�򿭤Ф� */
uiContext d;
{
  int f;
  jishuContext jc = (jishuContext)d->modec;
  yomiContext yc = jc->ycx;

  /* ��Υݥ��󥿤����䤹 */
  switch (jc->kc)
    {
    case JISHU_ZEN_ALPHA:
    case JISHU_HAN_ALPHA: /* ���ѱѿ�����Ⱦ�ѱѿ����ʤ� */
      myjishuAdjustRome(d);

      if(jc->jishu_rEndp >= yc->rEndp && jc->jishu_kEndp >= yc->kEndp ) {
                                    /* �������������Хåե�����ǥå�����
				     * ���������᤹
				     */
	jc->jishu_rEndp = 0;
	jc->jishu_kEndp = 0;
      }

      f = (yc->kana_buffer[jc->jishu_kEndp]&0x80)?2:1; 
                           /* ����������ʸ���κǺ��ӥåȤ�Ω�äƤ����� */

      if (yc->rAttr[jc->jishu_rEndp] & SENTOU) {
	                       /* ���޻������Ѵ���Ƭ�ե饰�Хåե���
				* Ω�äƤ�����
			        */

	for (jc->jishu_kEndp++ ; 
	     jc->jishu_kEndp > 0 && !(yc->kAttr[jc->jishu_kEndp] & SENTOU) ;
	     jc->jishu_kEndp++);
	                       /* �����Ѵ������ե饰�Хåե�����Ƭ��
				* Ω�äƤ����ޤ�
				* ���狼�ʥХåե�����ǥå��������䤹
			        */
      }
      jc->jishu_rEndp++; /* ������޻��Хåե�����ǥå��������䤹 */
      break;
    case JISHU_HIRA:
    case JISHU_ZEN_KATA:
    case JISHU_HAN_KATA: /* �Ҥ餬�ʤ����ѥ������ʤ�Ⱦ�ѥ������ʤʤ� */
      jishuAdjustRome(d);

      if(jc->jishu_rEndp >= yc->rEndp && jc->jishu_kEndp >= yc->kEndp ) {
                                    /* �������������Хåե�����ǥå�����
				     * ���������᤹
				     */
	jc->jishu_rEndp = 0;
	jc->jishu_kEndp = 0;
      }

      f = (yc->kana_buffer[jc->jishu_kEndp]&0x80)?2:1; 
                     /* ����������ʸ���κǺ��ӥåȤ�Ω�äƤ����� */

      if (yc->kAttr[jc->jishu_kEndp] & SENTOU) {
                               /* �����Ѵ������ե饰�Хåե���
				* Ω�äƤ�����
			        */
	for (jc->jishu_rEndp++ ; 
	     jc->jishu_rEndp > 0 && !(yc->rAttr[jc->jishu_rEndp] & SENTOU) ;
	     jc->jishu_rEndp++);
	                       /* ���޻������Ѵ���Ƭ�ե饰�Хåե���
				* Ω�äƤ����ޤ�
				* ������޻��Хåե�����ǥå��������䤹
			        */
      }
      jc->jishu_kEndp += f; /* ���狼�ʥХåե�����ǥå�����
	                    * ��ʸ��ʬ���䤹
			    */
      break;
    }
  jishuHenkan(d);
  return 0;
}

static
jishuAdjustRome(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;
  yomiContext yc = jc->ycx;

  while (!(yc->rAttr[jc->jishu_rEndp] & SENTOU)) {
    ++jc->jishu_rEndp;
  }
}

static
myjishuAdjustRome(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;
  yomiContext yc = jc->ycx;

  while (!(yc->kAttr[jc->jishu_kEndp] & SENTOU)
	 && !(jc->jishu_kEndp == yc->kEndp)) {
    ++jc->jishu_kEndp;
  }
}

static
JishuZenkaku(d) /* �����Ѵ� */
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;

/* ���Ф���ʸ������Ѵ����� */
  switch(jc->kc)
    {
    case JISHU_HIRA: /* �Ҥ餬�ʤʤ鲿�⤷�ʤ� */
      break;
      
    case JISHU_HAN_ALPHA: /* Ⱦ�ѱѿ��ʤ����ѱѿ����Ѵ����� */
      jc->kc = JISHU_ZEN_ALPHA;
      break;
      
    case JISHU_ZEN_ALPHA: /* ���ѱѿ��ʤ鲿�⤷�ʤ� */
      break;
      
    case JISHU_HAN_KATA: /* Ⱦ�ѥ������ʤʤ����ѥ������ʤ��Ѵ����� */
      jc->kc = JISHU_ZEN_KATA;
      break;
      
    case JISHU_ZEN_KATA: /* ���ѥ������ʤʤ鲿�⤷�ʤ� */
      break;
      
    default: /* �ɤ�Ǥ�ʤ��ä����Ѵ�����ʤ��Τǲ��⤷�ʤ� */
      break;
    }

  jishuHenkan(d);
  return 0;
}

static
JishuHankaku(d) /* Ⱦ���Ѵ� */
     uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;
  extern InhibitHankakuKana;
  
  /* ���Ф���ʸ������Ѵ����� */
  switch(jc->kc)
    {
    case JISHU_HIRA: /* �Ҥ餬�ʤʤ�Ⱦ�ѥ������ʤ��Ѵ����� */
      jc->kc = JISHU_HAN_KATA;
      break;
      
    case JISHU_ZEN_KATA: /* ���ѥ������ʤʤ�Ⱦ�ѥ������ʤ��Ѵ����� */
      jc->kc = JISHU_HAN_KATA;
      break;
      
    case JISHU_HAN_KATA: /* Ⱦ�ѥ������ʤʤ鲿�⤷�ʤ� */
      break;
      
    case JISHU_ZEN_ALPHA: /* ���ѱѿ��ʤ�Ⱦ�ѱѿ����Ѵ����� */
      jc->kc = JISHU_HAN_ALPHA;
      break;
      
    case JISHU_HAN_ALPHA: /* Ⱦ�ѱѿ��ʤ鲿�⤷�ʤ� */
      break;
      
    default: /* �ɤ�Ǥ�ʤ��ä����Ѵ�����ʤ��Τǲ��⤷�ʤ� */
      break;
    }

  if (jc->kc == JISHU_HAN_KATA && InhibitHankakuKana) {
    return NothingChangedWithBeep(d);
  }

  jishuHenkan(d);
  return 0;
}

static
JishuKakutei(d)
uiContext d;
{
  jishuHenkan(d);
  d->nbytes = strlen(d->genbuf);
  if (d->nbytes > d->bytes_buffer) { /* �Хåե���­��ʤ� */
    d->nbytes = ujisncpy(d->buffer_return, d->genbuf, d->bytes_buffer);
  }
  else {
    strcpy(d->buffer_return, d->genbuf);
  }
  d->kanji_status_return->revPos =
    d->kanji_status_return->revLen =
      d->kanji_status_return->length = 0;

  popJishuMode(d);
  d->status = EXIT_CALLBACK;
  return d->nbytes;
}

static
JishuYomiInsert(d)
uiContext d;
{
  int n;

  n = JishuKakutei(d); /* ����⡼�ɤΥݥåפ� JishuKakutei �ǹԤ��� */
  d->more.todo = 1;
  d->more.ch = d->ch;
  d->more.fnum = 0;    /* ��� ch �Ǽ����������򤻤� */
  return n;
}

static
JishuQuit(d)
uiContext d;
{
  popJishuMode(d);
  d->status = QUIT_CALLBACK;
  return 0;
}

/* ��ʸ���ˤ���ؿ� */

static
JishuToUpper(d)
uiContext d;
{
  int len, revlen, i;
  unsigned char *p;
  jishuContext jc = (jishuContext)d->modec;

  if (jc->kc == JISHU_ZEN_ALPHA || jc->kc == JISHU_HAN_ALPHA) {
    jc->jishu_case = IROHA_JISHU_UPPER;
    jishuHenkan(d);
    return 0;
  }
  else {
    /* ���Ȳ����Ѥ��ޤ��� */
    d->kanji_status_return->length = -1;
    return 0;
  }
}

static
JishuCapitalize(d)
uiContext d;
{
  int len, revlen, i;
  unsigned char *p;
  jishuContext jc = (jishuContext)d->modec;

  if (jc->kc == JISHU_ZEN_ALPHA || jc->kc == JISHU_HAN_ALPHA) {
    jc->jishu_case = IROHA_JISHU_CAPITALIZE;
    jishuHenkan(d);
    return 0;
  }
  else {
    /* ���Ȳ����Ѥ��ޤ��� */
    d->kanji_status_return->length = -1;
    return 0;
  }
}

static
JishuToLower(d)
uiContext d;
{
  int len, revlen, i;
  unsigned char *p;
  jishuContext jc = (jishuContext)d->modec;

  if (jc->kc == JISHU_ZEN_ALPHA || jc->kc == JISHU_HAN_ALPHA) {
    jc->jishu_case = 0;
    jishuHenkan(d);
    return 0;
  }
  else {
    /* ���Ȳ����Ѥ��ޤ��� */
    d->kanji_status_return->length = -1;
    return 0;
  }
}

static
JishuHiragana(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;

  jc->kc = JISHU_HIRA;
  jishuHenkan(d); /* ������Ѵ� */
  return 0;
}

static
JishuKatakana(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;

  jc->kc = JISHU_ZEN_KATA;
  jishuHenkan(d); /* ������Ѵ� */
  return 0;
}

static
JishuRomaji(d)
uiContext d;
{
  jishuContext jc = (jishuContext)d->modec;

  if (jc->inhibition == INHIBIT_ALPHA) {
    return NothingChangedWithBeep(d);
  }
  jc->kc = JISHU_ZEN_ALPHA;
  jishuHenkan(d); /* ������Ѵ� */
  return 0;
}

static
YomiJishu(d, fn) /* �ɤߥ⡼�ɤ���ľ�ܻ���⡼�ɤ� */
uiContext d;
int (*fn)();
{
  yomiContext yc = (yomiContext)d->modec;
  jishuContext jc;

  if (yc->henkanInhibition & IROHA_YOMI_INHIBIT_JISHU) {
    return NothingChangedWithBeep(d);
  }
  jc = pushJishuMode(d, yc, RomajiFlushYomi(d, yc->kana_buffer, ROMEBUFSIZE),
		     NO_CALLBACK, exitJishu, quitJishu, NO_CALLBACK);
  if (jc == (jishuContext)0) {
    return NoMoreMemory();
  }
  jc->minorMode = IROHA_MODE_JishuMode;
  currentModeInfo(d);
  return fn(d);
}

YomiHiraganaJishu(d) /* �ɤߥ⡼�ɤ������⡼�ɤΤҤ餬�ʤ� */
uiContext d;
{
  return YomiJishu(d, JishuHiragana);
}

YomiKatakanaJishu(d) /* �ɤߥ⡼�ɤ������⡼�ɤΤҤ餬�ʤ� */
uiContext d;
{
  return YomiJishu(d, JishuKatakana);
}

YomiRomajiJishu(d) /* �ɤߥ⡼�ɤ������⡼�ɤΤҤ餬�ʤ� */
uiContext d;
{
  int res;

  res = YomiJishu(d, JishuRomaji);
  if (d->kanji_status_return->length < 0) {
    return JishuQuit(d);
  }
  return res;
}

static
_jishuHenkanExit(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  d->nbytes = retval;
  d->status = EXIT_CALLBACK;
  popJishuMode(d);
  currentModeInfo(d);
  return retval;
}

static
_jishuHenkanQuit(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  jishuHenkan(d);
  currentModeInfo(d);
  return 0;
}

/*
 * ���ʴ����Ѵ���Ԥ�(�Ѵ����������Ʋ����줿)��TanKouhoMode�˰ܹԤ���
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
JishuKanjiHenkan(d)
uiContext	d;
{
  jishuContext jc = (jishuContext)d->modec;
  yomiContext yc = jc->ycx;
  henkanContext hc, pushHenkanMode();
  int len;
  extern kouho_threshold;

#ifdef MEASURE_TIME
  struct tms timebuf;
  long   currenttime, times();

  currenttime = times(&timebuf);
#endif

  if (yc->henkanInhibition & IROHA_YOMI_INHIBIT_HENKAN) {
    return NothingChangedWithBeep(d);
  }

  len = jishuHenkan(d);
  d->nbytes = strlen(d->genbuf);
  if (d->nbytes == len) {
    len = 0;
  }
  if (yc->henkanCallback) {
    return yc->henkanCallback(d, IROHA_FN_Henkan, d->genbuf, d->nbytes);
  }

  /* TanKouhoMode(ñ����ɽ���⡼��)�˰ܹԤ��� */
  hc = pushHenkanMode(d, d->genbuf, d->nbytes, yc,
		      0, _jishuHenkanExit, _jishuHenkanQuit, 0);
  if (hc == 0) {
    return -1;
  }
  hc->minorMode = IROHA_MODE_TankouhoMode;
  hc->kouhoCount = 1;

  if (henkan(d, len) < 0) {
    popHenkanMode(d);
    popCallback(d);
    return -1;
  }

  if (kouho_threshold > 0 && hc->kouhoCount >= kouho_threshold) {
    return tanKouhoIchiran(d, 0);
  }

  currentModeInfo(d);

#ifdef MEASURE_TIME
  hc->proctime = times(&timebuf);
  hc->proctime -= currenttime;
#endif

  return 0;
}

#include "jishumap.c"
