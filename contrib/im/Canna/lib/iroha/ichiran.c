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

static	char	rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/ichiran.c,v 3.7 91/09/04 13:19:16 kon Exp $";

#include	<errno.h>
#include	"iroha.h"

extern SelectDirect;
extern BunsetsuKugiri;
extern QuitIchiranIfEnd;
extern stayAfterValidate;
extern HexkeySelect;
extern CursorWrap;

extern int kCount;

extern int TanNextKouho(), TanKakutei();

static int clearIchiranContext(), IchiranKakutei();

#define ICHISIZE 9
unsigned char *bango = (unsigned char *)
     "�������������������������������������ᡡ�⡡�㡡�䡡�塡��";
                                                /* ����Ԥθ����ֹ��ʸ���� */
static unsigned char *kuuhaku = (unsigned char *)"��";

/*
 * ������ɽ����Υ�����ʸ��θ���򹹿�����
 *
 * �����Ǥˤ��� d->echo_buffer �Τ����Υ�����ʸ�����ʬ���ѹ�����
 * ������ˤȤ�ʤ� kugiri �⹹�������
 *
 * ������	uiContext
 *              henkanContext
 */
static void
makeIchiranEchoStrCurChange(d, hc)
uiContext	d;
henkanContext   hc;
{
  int  i, j, k, old_klen, new_klen;
  unsigned char *ptr;

  ptr = hc->echo_buffer;

  /* �쥫����ʸ��ΥХ��ȿ� */
  old_klen = hc->kugiri[hc->curbun + 1] - hc->kugiri[hc->curbun];
  if(BunsetsuKugiri && (hc->curbun != (hc->nbunsetsu - 1)))
    old_klen -= 1;

  /* ��������ʸ��ΥХ��ȿ� */
  new_klen = strlen(hc->allkouho[hc->curIkouho]);

  /* ������ʸ���Ĺ��ʬ�����뤿��ˡ�����ʸ��ʹߤ򤺤餹 */
  /* ���餹ʸ����Ĺ */ 
  i = hc->kugiri[hc->nbunsetsu] - hc->kugiri[hc->curbun + 1];

  if(new_klen > old_klen) { /* ���ˤ��餹*/
    j = hc->kugiri[hc->nbunsetsu];
    k = new_klen - old_klen;
    for(; i>=0; i--, j--) /* NULL �⥳�ԡ�����*/
      hc->echo_buffer[j + k] = hc->echo_buffer[j];
  }else if(new_klen < old_klen) { /* ���ˤ��餹*/
    j = hc->kugiri[hc->curbun + 1];
    k = old_klen - new_klen;
    for(; i>=0; i--, j++) /* NULL �⥳�ԡ�����*/
      hc->echo_buffer[j - k] = hc->echo_buffer[j];
  }
  /* �����ȸ����������Ȥ��������� */ 
  strncpy((ptr + hc->kugiri[hc->curbun]), hc->allkouho[hc->curIkouho], 
	  new_klen);
  if(BunsetsuKugiri && (hc->curbun != (hc->nbunsetsu - 1)))
    *(ptr + hc->kugiri[hc->curbun] + new_klen) = (unsigned char)' ';

  /* ��ʸ��ζ褮���ȤäƤ���(��Ƭ����ΥХ��ȿ�) */
  /* ������ʸ��ʹߺ�ʬ�򤿤� */
  for(i=hc->curbun; i<hc->nbunsetsu; i++)
    hc->kugiri[i + 1] += new_klen - old_klen;
}

/*
 * ���ʴ����Ѵ��Ѥι�¤�Τ����Ƥ򹹿�����(���ξ�Τ�)
 *
 * ��d->echo_buffer �򥨥�������������ʸ���ȿž����
 *
 * ������	uiContext
 *              henkanContext
 */
static
makeIchiranKanjiStatusReturn(d, hc)
uiContext	d;
henkanContext   hc;
{
  unsigned char tmp;

  d->kanji_status_return->echoStr = hc->echo_buffer;
  d->kanji_status_return->length = hc->kugiri[hc->nbunsetsu];
  d->kanji_status_return->revPos = hc->kugiri[hc->curbun];
  if(BunsetsuKugiri) {
    tmp = hc->echo_buffer[hc->kugiri[hc->curbun]];
    d->kanji_status_return->revLen =
     ((tmp & 0x80) && (tmp != 0x8e)) ? 2 : 1; 
  } else {
    d->kanji_status_return->revLen =
      hc->kugiri[hc->curbun + 1] - hc->kugiri[hc->curbun];
  }
}

/*
 * ����Ԥ˴ؤ��빽¤�Τ����Ƥ򹹿�����
 *
 * ��glineinfo �� kouhoinfo �������Ԥ�������������ȸ����ֹ��ȿž����
 *
 * ������	uiContext
 * �����	�ʤ�
 */
makeGlineStatus(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
  unsigned char tmp, pos;
  int kcoffset, cur;

  if (kCount) {
    cur = *(ic->curIkouho) + 1;
    kcoffset = (cur < 10) ? 2 : (cur < 100) ? 1 : 0;
  }

  d->kanji_status_return->info |= KanjiGLineInfo;
  d->kanji_status_return->gline.line =
    ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].gldata;
  d->kanji_status_return->gline.length = 
    ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].gllen;
  d->kanji_status_return->gline.revPos = pos = 
    ic->kouhoifp[*(ic->curIkouho)].khpoint;
  tmp = d->kanji_status_return->gline.line[pos];
  d->kanji_status_return->gline.revLen = 
    ((tmp & 0x80) && (tmp != 0x8e)) ? 2 : 1; 

  if (kCount) {
    sprintf(d->kanji_status_return->gline.line + d->ncolumns
	    - ICHISIZE + kcoffset,
	    " %d/%d",
	    cur, ic->nIkouho);
    d->kanji_status_return->gline.length
      = strlen(d->kanji_status_return->gline.line);
  }
}

static
ichiranEveryTimeCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  makeIchiranEchoStrCurChange(d, env);
  makeIchiranKanjiStatusReturn(d, env);

  return(retval);
}

static
ichiranExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  henkanContext hc = (henkanContext)env;

  hc->kouhoCount = 0;
  /* d->curIkouho�򥫥��ȸ���Ȥ��� */
  if((retval = RkXfer(hc->context, hc->curIkouho)) == NG) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "�����ȸ������Ф��ޤ���Ǥ���";
    /* �����ȸ��䤬���Ф��ʤ����餤�Ǥϲ��Ȥ�ʤ��� */
  } else 
    retval = d->nbytes = 0;

  makeIchiranEchoStrCurChange(d, (henkanContext)env);
  makeIchiranKanjiStatusReturn(d, (henkanContext)env);

  freeGetIchiranList(((henkanContext)env)->allkouho);
  
  popCallback(d);

  if(!stayAfterValidate && !d->more.todo) {
    d->more.todo = 1;
    d->more.ch = 0;
    d->more.fnum = IROHA_FN_Forward;
  }
  currentModeInfo(d);

  return(retval);
}

static
ichiranQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  henkanContext hc = (henkanContext)env;

  hc->kouhoCount = 0;

  if((retval = RkXfer(hc->context, hc->curIkouho)) == NG) {
    if(errno == EPIPE)
      necKanjiPipeError();
    necKanjiError = "�����ȸ������Ф��ޤ���Ǥ���";
    /* �����ȸ��䤬���Ф��ʤ����餤�Ǥϲ��Ȥ�ʤ��� */
  } else 
    retval = d->nbytes = 0;

  makeIchiranEchoStrCurChange(d, env); 	/* RkGetKanji �����Τ�ȤäƤ��� */
  makeIchiranKanjiStatusReturn(d, env);

  freeGetIchiranList(((henkanContext)env)->allkouho);

  popCallback(d);
  currentModeInfo(d);
  return(retval);
}

void
freeIchiranContext(ic)
ichiranContext ic;
{
  free(ic);
}

void
freeIchiranBuf(ic)
ichiranContext ic;
{
  if(ic->glinebufp)
    free(ic->glinebufp);
  if(ic->kouhoifp)
    free(ic->kouhoifp);
  if(ic->glineifp)
    free(ic->glineifp);
}

freeGetIchiranList(buf)
unsigned char **buf;
{
  /* �������ɽ�����ѤΥ��ꥢ��ե꡼���� */
  if(*buf)
    free(*buf);
  if(buf)
    free(buf);
}

static void
popIchiranMode(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;

  d->modec = ic->next;
  d->current_mode = ic->prevMode;
  freeIchiranContext(ic);
}

/*
 * ���٤Ƥθ������Ф��ơ�����ˤ���
 */
unsigned char **
getIchiranList(d, context, nelem, currentkouho)
uiContext d;
int context;
int *nelem, *currentkouho;
{
  unsigned char *work, *wptr, **bptr, **buf, **bwork;
  RkStat st;

  /* RkList�����롢���٤Ƥθ���Τ�����ΰ������ */
  if((work = (unsigned char *)malloc(ROMEBUFSIZE)) == (unsigned char *)NULL) {
    necKanjiError = "malloc (getIchiranList) �Ǥ��ޤ���Ǥ���";
    return(0);
  }

  /* ���٤Ƥθ�������롣
     ��: �������� �� �ٴ�@�ʴ�@�ݴ�@@ (@��NULL) */
  if((*nelem = RkGetKanjiList(context, work, ROMEBUFSIZE)) < 0) {
    necKanjiError = "���٤Ƥθ���μ��Ф��˼��Ԥ��ޤ���";
    free(work);
    return(0);
  }

  /* makeKouhoIchiran()���Ϥ��ǡ��� */
  if((buf = (unsigned char **)calloc
      (*nelem + 1, sizeof(unsigned char *))) == (unsigned char **)NULL) {
    necKanjiError = "malloc (getIchiranList) �Ǥ��ޤ���Ǥ���";
    free(work);
    return(0);
  }
  for(wptr = work, bptr = buf; *wptr; bptr++) { /* buf ���� */
    *bptr = wptr;
    while(*wptr++);
  }
  *bptr = (unsigned char *)NULL;

  if(RkGetStat(context, &st) == -1) {
    necKanjiError = "���ƥ���������Ф��ޤ���Ǥ���";
    free(work);
    free(buf);
    return(0);
  }
  *currentkouho = st.candnum; /* �����ȸ���ϲ����ܡ� */

  return(buf);
}

/* cfunc ichiranContext
 *
 * ichiranContext ��������Ѥι�¤�Τ�����������
 *
 */
ichiranContext
newIchiranContext()
{
  ichiranContext icxt;

  if((icxt = (ichiranContext)malloc(sizeof(ichiranContextRec))) == 
     (ichiranContext)NULL) {
    necKanjiError = "malloc (newIchiranContext) �Ǥ��ޤ���Ǥ���";
    return(0);
  }
  clearIchiranContext(icxt);

  return(icxt);
}

/*
 * ��������Ԥ���
 */
selectOne(d, buf, ck, nelem, bangomax, inhibit, currentkouho, allowcallback,
	  everyTimeCallback, exitCallback, quitCallback, auxCallback)
uiContext d;
unsigned char **buf;
int *ck;
int nelem, bangomax;
unsigned char inhibit;
int currentkouho;
int allowcallback;
int (*everyTimeCallback)(), (*exitCallback)();
int (*quitCallback)(), (*auxCallback)();
{
  extern KanjiModeRec ichiran_mode;
  ichiranContext ic;
  int retval = 0;

  if (allowcallback != WITHOUT_LIST_CALLBACK &&
      d->list_func == (void (*)())0) {
    allowcallback = WITHOUT_LIST_CALLBACK;
  }

  if(pushCallback(d, d->modec,
	everyTimeCallback, exitCallback, quitCallback, auxCallback) == 0) {
    necKanjiError = "malloc (pushCallback) �Ǥ��ޤ���Ǥ���";
    return(NG);
  }
  
  if((ic = newIchiranContext()) == (ichiranContext)NULL) {
    popCallback(d);
    return(NG);
  }
  ic->majorMode = d->majorMode;
  ic->next = d->modec;
  d->modec = (mode_context)ic;

  ic->prevMode = d->current_mode;
  d->current_mode = &ichiran_mode;

  ic->allkouho = buf;
  ic->curIkouho = ck;
  ic->inhibit = inhibit;
  ic->nIkouho = nelem;

  if (allowcallback != WITHOUT_LIST_CALLBACK) {
    ic->flags |= ICHIRAN_ALLOW_CALLBACK;
    ic->inhibit |= NUMBERING;
  }

  if (allowcallback == WITHOUT_LIST_CALLBACK) {
    if ((retval = makeKouhoIchiran(d, nelem, bangomax, inhibit,
				  currentkouho))   == NG) {
      popIchiranMode(d);
      popCallback(d);
      return(NG);
    }
  }
  else {
    d->list_func(d->client_data, CANNA_LIST_Start, buf, nelem, ck);
  }

  return(0);
}

/*
 * IchiranContext �ν����
 */
static
clearIchiranContext(p)
ichiranContext p;
{
  p->id = ICHIRAN_CONTEXT;
  p->svIkouho = 0;
  p->curIkouho = 0;
  p->nIkouho = 0;
  p->tooSmall = 0;
  p->curIchar = 0;
  p->allkouho = 0;
  p->glinebufp = 0;
  p->kouhoifp = (kouhoinfo *)0;
  p->glineifp = (glineinfo *)0;
  p->flags = (unsigned char)0;
}
  
/*
 * ��������Υǡ�����¤�Τ��뤿����ΰ����ݤ���
 */
allocIchiranBuf(d, inhibit, flag)
uiContext d;
unsigned char inhibit;
int flag;
{
  ichiranContext ic = (ichiranContext)d->modec;
  unsigned char **work;
  int length = 0, size;

  /* ��������ʬ���ֹ��ʬ���ΰ������*/
  size = ic->nIkouho * (d->ncolumns + 1); /* ������� */
  if((ic->glinebufp = (unsigned char *)malloc(size)) == 
     (unsigned char *)NULL) {
    necKanjiError = "malloc (allocIchiranBuf) �Ǥ��ޤ���Ǥ���";
    return(NG);
  }

  /* kouhoinfo���ΰ������ */
  if((ic->kouhoifp = 
      (kouhoinfo *)malloc((ic->nIkouho + 1) * sizeof(kouhoinfo))) ==
     (kouhoinfo *)NULL) {
    necKanjiError = "malloc (allocIchiranBuf) �Ǥ��ޤ���Ǥ���";
    free(ic->glinebufp);
    return(NG);
  }

  /* glineinfo���ΰ������ */
  if((ic->glineifp = 
      (glineinfo *)malloc((ic->nIkouho + 1) * sizeof(glineinfo))) ==
     (glineinfo *)NULL) {
    necKanjiError = "malloc (allocIchiranBuf) �Ǥ��ޤ���Ǥ���";
    free(ic->glinebufp);
    free(ic->kouhoifp);
    return(NG);
  }
}

/*
 * ��������Ԥ�ɽ���ѤΥǡ�����ơ��֥�˺�������
 *
 * ��glineinfo �� kouhoinfo���������
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
makeKouhoIchiran(d, nelem, bangomax, inhibit, currentkouho)
uiContext d;
int nelem, bangomax;
unsigned char inhibit;
int currentkouho;
{
  ichiranContext ic = (ichiranContext)d->modec;
  unsigned char **kkptr, *kptr, *gptr, *bptr, *svgptr, **work;
  int           i, ko, lnko, cn = 0, svcn, line = 0;
  RkStat	st;

  ic->nIkouho = nelem;	/* ����ο� */

  /* �����ȸ���򥻥åȤ��� */
  ic->svIkouho = *(ic->curIkouho);
  *(ic->curIkouho) += currentkouho;
  if(*(ic->curIkouho) >= ic->nIkouho)
    ic->svIkouho = *(ic->curIkouho) = 0;

#ifdef DEBUG
  if(iroha_debug) {
    printf("<makeKouhoIchiran> nelem = %d, currentkouho = %d\n", nelem, 
	   currentkouho);
    printf("tooSmall = %d\n", ic->tooSmall);
    printf("<makeKouhoIchiran> buf\n");
    for(work = ic->allkouho; *work; work++)
      printf("-> %s ", *work);
  }
#endif

  if(allocIchiranBuf(d, inhibit, 1) == NG)
    return(NG);

  if(d->ncolumns < 2) {
    ic->tooSmall = 1;
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

  kkptr = ic->allkouho;
  kptr = *(ic->allkouho);
  gptr = ic->glinebufp;

  /* line -- �����ܤ�
     ko   -- ���Τ���Ƭ���鲿���ܤθ��䤫
     lnko -- �����Ƭ���鲿���ܤθ��䤫
     cn   -- �����Ƭ���鲿�Х����ܤ� */

  for(line=0, ko=0; ko<ic->nIkouho; line++) {
    ic->glineifp[line].gldata = gptr; /* ����Ԥ�ɽ�����뤿���ʸ���� */
    ic->glineifp[line].glhead = ko;   /* ���ιԤ���Ƭ����ϡ����ΤǤ�ko���� */

    for(lnko=0, cn=0;
	cn<d->ncolumns - (kCount ? ICHISIZE + 1 : 0) &&
	lnko<bangomax && ko<ic->nIkouho ; lnko++, ko++) {
      kptr = kkptr[ko];
      ic->kouhoifp[ko].khretsu = line; /* �����ܤ�¸�ߤ��뤫��Ͽ */
      if(lnko == 0)
	ic->kouhoifp[ko].khpoint = cn;   /* ���ιԤǲ��Х����ܤ���Ͽ */
      else 
	ic->kouhoifp[ko].khpoint = cn + 2;   /* ���ιԤǲ��Х����ܤ���Ͽ */
      ic->kouhoifp[ko].khdata = kptr;  /* ����ʸ����ؤΥݥ��� */
      svgptr = gptr;
      svcn = cn;

      if(!(inhibit & (unsigned char)NUMBERING)) {
	/* �ֹ�򥳥ԡ����� */
	if(lnko == 0) {
	  strncpy(gptr, bango, 2);
	  cn += 2; gptr += 2;
	} else {
	  strncpy(gptr, bango+(2 + BANGOSIZE * (lnko - 1)), BANGOSIZE);
	  cn += BANGOSIZE; gptr += BANGOSIZE;
	}
      } else {
	/* ����򥳥ԡ����� */
	if(lnko) {
	  strncpy(gptr, kuuhaku, KG_KUHAKUSIZE);
	  cn += KG_KUHAKUSIZE; gptr += KG_KUHAKUSIZE;
	}
      }
      /* ����򥳥ԡ����� */
      for(; *kptr && cn<d->ncolumns - (kCount ? ICHISIZE + 1 : 0);
	  gptr++, kptr++, cn++) {
	*gptr = *kptr;
      }

      /* ���������Ϥߤ����Ƥ��ޤ������ˤʤä��Τǣ����᤹ */
      if ((cn >= d->ncolumns - (kCount ? ICHISIZE + 1 : 0)) && *kptr) {
	if (lnko) {
	  gptr = svgptr;
	  cn = svcn;
	  break;
	}
	else {
	  ic->tooSmall = 1;
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
    ic->glineifp[line].glkosu = lnko;
    ic->glineifp[line].gllen = strlen(ic->glineifp[line].gldata);
  }
  /* �Ǹ��NULL������� */
  ic->kouhoifp[ko].khretsu = (int)NULL;
  ic->kouhoifp[ko].khpoint = (int)NULL;
  ic->kouhoifp[ko].khdata  = (unsigned char *)NULL;
  ic->glineifp[line].glkosu  = (int)NULL;
  ic->glineifp[line].glhead  = (int)NULL;
  ic->glineifp[line].gllen   = (int)NULL;
  ic->glineifp[line].gldata  = (unsigned char *)NULL;

#ifdef DEBUG
  if (iroha_debug) {
    for(i=0; ic->glineifp[i].glkosu; i++)
      printf("%d: %s\n", i, ic->glineifp[i].gldata);
  }
#endif

  return(0);
}

/*
 * ��������Ԥ�ɽ������
 *
 * ���������ɽ���Τ���Υǡ�����ơ��֥�˺�������
 * ���������ɽ���Ԥ������Ȥ��ϡ�������ɽ�����ʤ��Ǽ�����򤽤ξ��ɽ������
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */

TanKouhoIchiran(d)
uiContext d;
{
  return tanKouhoIchiran(d, 1);
}

tanKouhoIchiran(d, step)
uiContext d;
int step;
{
  henkanContext hc = (henkanContext)d->modec;
  ichiranContext ic;
  int nelem, currentkouho, retval = 0;
  unsigned char inhibit = 0;
  unsigned char listcallback = (unsigned char)(d->list_func ? 1 : 0);

  unsigned char **getIchiranList();

  /* ��������Ԥ������Ƹ���������Ф��ʤ� */
  if(listcallback == 0 && d->ncolumns - (kCount ? ICHISIZE + 1 : 0) < 4) {
    /* tooSmall */
    return(TanNextKouho(d));
  }

  /* ���٤Ƥθ������Ф� */
  if((hc->allkouho = 
      getIchiranList(d, hc->context, &nelem, &currentkouho)) == 0) {
    necKanjiPipeError();
    strcpy(d->genbuf, necKanjiError);
    d->nbytes = TanKakutei(d);
    makeGLineMessage(d, d->genbuf, strlen(d->genbuf));
    return d->nbytes;
  }

  if ( !HexkeySelect )
    inhibit |= (unsigned char)NUMBERING;

  hc->curIkouho = currentkouho;	/* ���ߤΥ����ȸ����ֹ����¸���� */
  currentkouho = step;	/* �����ȸ��䤫�鲿���ܤ򥫥��ȸ���Ȥ��뤫 */

  /* ��������˰ܹԤ��� */
  if((retval = selectOne(d, hc->allkouho, &hc->curIkouho, nelem, BANGOMAX,
               inhibit, currentkouho, WITH_LIST_CALLBACK,
	       ichiranEveryTimeCatch, ichiranExitCatch,
	       ichiranQuitCatch, 0)) == NG) {
    freeGetIchiranList(hc->allkouho);
    return(GLineNGReturn(d));
  }

  ic = (ichiranContext)d->modec;
  if(ic->tooSmall) {
    freeGetIchiranList(hc->allkouho);
    popIchiranMode(d);
    popCallback(d);
    return(TanNextKouho(d));
  }

  ic->minorMode = IROHA_MODE_IchiranMode;
  currentModeInfo(d);

  if ( !(ic->flags & ICHIRAN_ALLOW_CALLBACK) ) {
    makeGlineStatus(d);
  }
  /* d->status = EVERYTIME_CALLBACK; */

  return(retval);
}

/*
 * ��������Ԥ�ɽ��������λ����
 */
IchiranQuit(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
  int retval = 0;

  if (ic->flags & ICHIRAN_ALLOW_CALLBACK &&
      d->list_func) {
    d->list_func(d->client_data, CANNA_LIST_Quit, 0, 0, 0);
  }

  *(ic->curIkouho) = ic->nIkouho - 1; /* �Ҥ餬�ʸ���ˤ��� */

  ichiranFin(d);

  d->status = QUIT_CALLBACK;

  return(retval);
}

/*
 * ������˰�ư����
 *
 * �������ȸ��䤬�ǽ�������ä�����Ƭ����򥫥��ȸ���Ȥ���
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
IchiranForwardKouho(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
  int mode;

  if (ic->flags & ICHIRAN_ALLOW_CALLBACK &&
      d->list_func) {
    d->list_func(d->client_data, CANNA_LIST_Forward, 0, 0, 0);
    return 0;
  }

  /* ������ˤ��� (ñ�����������֤ǡ��Ǹ�θ�����ä�����������) */
  *(ic->curIkouho) += 1;
  if(*(ic->curIkouho) >= ic->nIkouho) {
    if(QuitIchiranIfEnd && (getMinorMode(d) == IROHA_MODE_IchiranMode)) {
      return(IchiranQuit(d));
    } else if(CursorWrap) {
      *(ic->curIkouho) = 0;
    } else {
      *(ic->curIkouho) -= 1;
      return NothingChangedWithBeep(d);
    }
  }

  if(ic->tooSmall) { /* for bushu */
    d->status = AUX_CALLBACK;
    return 0;
  }

  makeGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return 0;
}

/*
 * ������˰�ư����
 *
 * �������ȸ��䤬��Ƭ������ä���ǽ�����򥫥��ȸ���Ȥ���
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
IchiranBackwardKouho(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
  int mode;

  if (ic->flags & ICHIRAN_ALLOW_CALLBACK &&
      d->list_func) {
    d->list_func(d->client_data, CANNA_LIST_Backward, 0, 0, 0);
    return 0;
  }

  /* ���ߤΥ⡼�ɤ���� */
  if(QuitIchiranIfEnd) {
    mode = getMinorMode(d);
  }

  /* ������ˤ��� (ñ�����������֤ǡ��ǽ�θ�����ä�����������) */
  if(*(ic->curIkouho))
    *(ic->curIkouho) -= 1;
  else {
    if(QuitIchiranIfEnd && (mode == IROHA_MODE_IchiranMode)) {
      return(IchiranQuit(d));
    } else if(CursorWrap) {
      *(ic->curIkouho) = ic->nIkouho - 1;
    } else {
      *(ic->curIkouho) = 0;
      return NothingChangedWithBeep(d);
    }
  }

  if(ic->tooSmall) { /* for bushu */
    d->status = AUX_CALLBACK;
    return 0;
  }

  makeGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return 0;
}

/*
 * ��������˰�ư����
 *
 * �������ȸ������Ƹ�������Ȥ��ξ�θ����ɽ������
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
IchiranPreviousKouhoretsu(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;

  if (ic->flags & ICHIRAN_ALLOW_CALLBACK &&
      d->list_func) {
    d->list_func(d->client_data, CANNA_LIST_Prev, 0, 0, 0);
    return 0;
  }

  if(ic->tooSmall) { /* for bushu */
    return(IchiranBackwardKouho(d));
  }

  /* ��������ˤ��� (*(ic->curIkouho)�����)*/
  getIchiranPreviousKouhoretsu(d);

  makeGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return 0;
}

/*
 * ��������Υ����ȸ�������
 *
 * �������������Ʊ�������ֹ�Τ�Τ򥫥��ȸ���Ȥ���
 * �������ֹ��Ʊ����Τ��ʤ����ϡ����θ�����κǽ�����򥫥��ȸ���Ȥ���
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
getIchiranPreviousKouhoretsu(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
  int index;
  int curretsu, nretsu;

  /* �����ȸ���ԤΤʤ��ǲ����ܤθ��䤫�ʤΤ������� */
  index = *(ic->curIkouho) - 
    ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].glhead;
  /* ������������� */
  curretsu = ic->kouhoifp[*(ic->curIkouho)].khretsu;
  nretsu = ic->kouhoifp[ic->nIkouho - 1].khretsu + 1;
  if(curretsu == 0) {
    if(CursorWrap)
      curretsu = nretsu;
    else
      return NothingChangedWithBeep(d);
  }
  curretsu -= 1;
  /* index �������ȸ�����θ��������礭���ʤäƤ��ޤä���
     �Ǳ�����򥫥��ȸ���Ȥ��� */
  if(ic->glineifp[curretsu].glkosu <= index) 
    index = ic->glineifp[curretsu].glkosu - 1;
  /* ���������Ʊ���ֹ�˰�ư���� */
  *(ic->curIkouho) = index + ic->glineifp[curretsu].glhead;
}

/*
 * ��������˰�ư����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
IchiranNextKouhoretsu(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;

  if (ic->flags & ICHIRAN_ALLOW_CALLBACK &&
      d->list_func) {
    d->list_func(d->client_data, CANNA_LIST_Next, 0, 0, 0);
    return 0;
  }

  if(ic->tooSmall) {
    return(IchiranForwardKouho(d));
  }

  /* ��������ˤ��� (*(ic->curIkouho) �����) */
  getIchiranNextKouhoretsu(d);

  makeGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return 0;
}

/*
 * ��������˰�ư����
 *
 * �������������Ʊ�������ֹ�Τ�Τ򥫥��ȸ���Ȥ���
 * �������ֹ��Ʊ����Τ��ʤ����ϡ����θ�����κǽ�����򥫥��ȸ���Ȥ���
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
getIchiranNextKouhoretsu(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
  int index;
  int curretsu, nretsu;

  /* �����ȸ���ԤΤʤ��ǲ����ܤθ��䤫�ʤΤ������� */
  index = *(ic->curIkouho) - 
    ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].glhead;
  /* ������������� */
  curretsu = ic->kouhoifp[*(ic->curIkouho)].khretsu;
  nretsu = ic->kouhoifp[ic->nIkouho - 1].khretsu + 1;
  curretsu += 1;
  if(curretsu >= nretsu) {
    if(CursorWrap)
      curretsu = 0;
    else {
      return NothingChangedWithBeep(d);
    }
  }
  /* index �������ȸ�����θ��������礭���ʤäƤ��ޤä���
     �Ǳ�����򥫥��ȸ���Ȥ��� */
  if(ic->glineifp[curretsu].glkosu <= index) 
    index = ic->glineifp[curretsu].glkosu - 1;
  /* ���������Ʊ���ֹ�˰�ư���� */
  *(ic->curIkouho) = index + ic->glineifp[curretsu].glhead;
}

/*
 * ����������Ƭ����˰�ư����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
IchiranBeginningOfKouho(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;

  if (ic->flags & ICHIRAN_ALLOW_CALLBACK &&
      d->list_func) {
    d->list_func(d->client_data, CANNA_LIST_BeginningOfLine, 0, 0, 0);
    return 0;
  }

  if(ic->tooSmall) {
    d->status = AUX_CALLBACK;
    return 0;
  }

  /* ���������Ƭ����򥫥��ȸ���ˤ��� */
  *(ic->curIkouho) = 
    ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].glhead;

  makeGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return 0;
}

/*
 * �������κǱ�����˰�ư����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
IchiranEndOfKouho(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;

  if (ic->flags & ICHIRAN_ALLOW_CALLBACK &&
      d->list_func) {
    d->list_func(d->client_data, CANNA_LIST_EndOfLine, 0, 0, 0);
    return 0;
  }

  if(ic->tooSmall) {
    d->status = AUX_CALLBACK;
    return 0;
  }

  /* ������κǱ�����򥫥��ȸ���ˤ��� */
  *(ic->curIkouho) = 
    ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].glhead
    + ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].glkosu - 1;

  makeGlineStatus(d);
  /* d->status = EVERYTIME_CALLBACK; */

  return 0;
}

/*
 * �����������Ϥ��줿�ֹ�θ���˰�ư����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
IchiranBangoKouho(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
  int zflag, retval = 0;

  if(ic->tooSmall) {
    d->status = AUX_CALLBACK;
    return(retval);
  }

  /* d->status = EVERYTIME_CALLBACK; */

  if (HexkeySelect && !(ic->inhibit & NUMBERING)) {
    /* ���Ϥ��줿�ֹ�θ���򥫥��ȸ���Ȥ��� */
    if((zflag = getIchiranBangoKouho(d)) == NG)
      return NothingChangedWithBeep(d);

    /* SelectDirect �Υ������ޥ����ν��� */
    if(SelectDirect) /* ON */ {
      if(zflag) /* �������Ϥ��줿 */
	retval = IchiranQuit(d);
      else
	retval = IchiranKakutei(d);
    } else {          /* OFF */
      makeGlineStatus(d);
      /* d->status = EVERYTIME_CALLBACK; */
    }
    return(retval);
  }
  else {
    if(!(ic->inhibit & CHARINSERT)) {
      retval = IchiranKakutei(d);
      d->more.todo = 1;
      d->more.ch = d->ch;
      d->more.fnum = IROHA_FN_FunctionalInsert;
    } else {
      NothingChangedWithBeep(d);
    }
    return(retval);
  }
}

/*
 * �����������Ϥ��줿�ֹ�θ���˰�ư����
 *
 *
 * ������	uiContext
 * �����	�������Ϥ��줿��              �����֤�
 * 		������������椬���Ϥ��줿��  �����֤�
 * 		���顼���ä���              �������֤�
 */
getIchiranBangoKouho(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
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
    return(NG);
  }
  /* ���ϥǡ����� ����Ԥ����¸�ߤ�������� */
  if(num > ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].glkosu) {
    /* ���Ϥ��줿�ֹ������������ޤ��� */
    return(NG);
  }

  /* ���Ϥ��줿�������� SelectDirect �� ON �ʤ��ɤߤ��ᤷ�ƣ����֤� */
  if(num == 0) {
    if(SelectDirect)
      return(1);
    else {
      /* ���Ϥ��줿�ֹ������������ޤ��� */
      return(NG);
    }  
  } else {
    /* ���������Ƭ��������� */
    index = ic->glineifp[ic->kouhoifp[*(ic->curIkouho)].khretsu].glhead;
    *(ic->curIkouho) = index + (num - 1);
  }

  return(0);
}

/*
 * ������椫�����򤵤줿����򥫥��ȸ���Ȥ���
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
static
IchiranKakutei(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;
  int retval = 0;
  unsigned char *kakuteiStrings;

  if (ic->flags & ICHIRAN_ALLOW_CALLBACK &&
      d->list_func) {
    d->list_func(d->client_data, CANNA_LIST_Select, 0, 0, 0);
  }

  kakuteiStrings = ic->allkouho[*(ic->curIkouho)];
  retval = d->nbytes = strlen(kakuteiStrings);
  strcpy(d->buffer_return, kakuteiStrings);

  ichiranFin(d);

  d->status = EXIT_CALLBACK;

  return(retval);
}

/*
 * �����ɽ���⡼�ɤ���ȴ����
 *
 * ������	uiContext
 * �����	���ｪλ�� 0	�۾ｪλ�� -1
 */
ichiranFin(d)
uiContext d;
{
  ichiranContext ic = (ichiranContext)d->modec;

  /* �������ɽ�����ѤΥ��ꥢ��ե꡼���� */
  freeIchiranBuf(ic);

  popIchiranMode(d);

  /* gline �򥯥ꥢ���� */
  GlineClear(d);
}

#include	"ichiranmap.c"
