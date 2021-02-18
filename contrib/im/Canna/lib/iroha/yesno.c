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

static	char	rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/yesno.c,v 1.11 91/05/21 16:04:04 satoko Exp $";

#include	<errno.h>
#include	"iroha.h"

/* cfunc yesNoContext
 *
 * yesNoContext
 *
 */
static coreContext
newYesNoContext()
{
  coreContext ccxt;

  if((ccxt = (coreContext)malloc(sizeof(coreContextRec))) == NULL) {
    necKanjiError = "malloc (newcoreContext) �Ǥ��ޤ���Ǥ���";
    return(0);
  }
  ccxt->id = CORE_CONTEXT;

  return ccxt;
}

static void
freeYesNoContext(qc)
coreContext qc;
{
  free(qc);
}

/*
 * ��������Ԥ���
 */
getYesNoContext(d,
	  everyTimeCallback, exitCallback, quitCallback, auxCallback)
uiContext d;
int (*everyTimeCallback)(), (*exitCallback)();
int (*quitCallback)(), (*auxCallback)();
{
  extern KanjiModeRec tourokureibun_mode;
  coreContext qc;
  int retval = 0;

  if(pushCallback(d, d->modec,
	everyTimeCallback, exitCallback, quitCallback, auxCallback) == 0) {
    necKanjiError = "malloc (pushCallback) �Ǥ��ޤ���Ǥ���";
    return(NG);
  }
  
  if((qc = newYesNoContext()) == NULL) {
    popCallback(d);
    return(NG);
  }
  qc->majorMode = d->majorMode;
  qc->minorMode = IROHA_MODE_HenkanMode;
  qc->next = d->modec;
  d->modec = (mode_context)qc;

  qc->prevMode = d->current_mode;
  d->current_mode = &tourokureibun_mode;

  return(retval);
}

static void
popYesNoMode(d)
uiContext d;
{
  coreContext qc = (coreContext)d->modec;

  d->modec = qc->next;
  d->current_mode = qc->prevMode;
  freeYesNoContext(qc);
}

/*
 * EveryTimeCallback ... y/n �ʳ���ʸ�������Ϥ��줿
 * ExitCallback ...      y �����Ϥ��줿
 * quitCallback ...      quit �����Ϥ��줿
 * auxCallback ...       n �����Ϥ��줿
 */
static
YesNo(d)
uiContext	d;
{
  if((d->ch == 'y') || (d->ch == 'Y')) {
    popYesNoMode(d);
    d->status = EXIT_CALLBACK;
  } else if((d->ch == 'n') || (d->ch == 'N')) {
    popYesNoMode(d);
    d->status = AUX_CALLBACK;
  } else {
    /* d->status = EVERYTIME_CALLBACK; */
    return(NothingChangedWithBeep(d));
  }

  return(0);
}

static
YesNoQuit(d)
uiContext	d;
{
  int retval = 0;

  popYesNoMode(d);
  d->status = QUIT_CALLBACK;
  
  return(retval);
}

#include	"t_reimap.c"
