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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/mode.c,v 3.3 91/09/05 20:30:56 kon Exp $";
     
#include "iroha.h"
#include <iroha/mfdef.h>

#define BySTRING 0
#define ByNUMBER 1

int howToReturnModeInfo = BySTRING;
static unsigned char numMode[2];

struct ModeNameRecs ModeNames[IROHA_MODE_MAX_IMAGINARY_MODE];

static unsigned char *
_ModeNames[IROHA_MODE_MAX_IMAGINARY_MODE] = {
  (unsigned char *)"",		/* AlphaMode */
  (unsigned char *)"  [��]",	/* EmptyMode */
  (unsigned char *)"  [����]",	/* KigoMode */
  (unsigned char *)"��[��]",	/* ZenHiraKakuteiMode */
  (unsigned char *)"��[��]",	/* ZenKataKakuteiMode */
  (unsigned char *)"��[\216\261]", /* HenKataKakuteiMode */
  (unsigned char *)"��[��]",	/* ZenAlphaKakuteiMode */
  (unsigned char *)"��[a]",	/* HanAlphaKakuteiMode */
  (unsigned char *)NULL,	/* YomiMode (�⡼��ʸ����ɽ���ˤϻȤ�ʤ�) */
  (unsigned char *)"  [����]",	/* JishuMode */
  (unsigned char *)"  [����]",	/* TanKouhoMode */
  (unsigned char *)"  [����]",	/* IchiranMode */
  (unsigned char *)"  [����]",	/* YesNoMode */
  (unsigned char *)NULL,	/* OnOffMode */
  (unsigned char *)"  [ʸ��]",  /* AdjustBunsetsuMode */

  /* Imaginary Mode */

  (unsigned char *)"  [��]",	/* ZenKataHenkanMode */
  (unsigned char *)"  [\216\261]", /* HanKataHenkanMode */
  (unsigned char *)"  [��]",	/* ZenAlphaHenkanMode */
  (unsigned char *)"  [a]",	/* HanAlphaHenkanMode */
  (unsigned char *)"  [16��]",	/* HexMode */
  (unsigned char *)"  [����]",	/* BushuMode */
  (unsigned char *)"  [��ĥ]",	/* ExtendMode */
  (unsigned char *)"  [��]",	/* RussianMode */
  (unsigned char *)"  [��]",	/* GreekMode */
  (unsigned char *)"  [����]",	/* LineMode */
  (unsigned char *)"  [�ѹ�]",	/* ChangingServerMode */
  (unsigned char *)"  [�Ѵ�]",	/* HenkanMethodMode */
  (unsigned char *)"  [���]",	/* DeleteDicMode */
  (unsigned char *)"  [��Ͽ]",	/* TourokuMode */
  (unsigned char *)"  [�ʻ�]",	/* TourokuHinshiMode */
  (unsigned char *)"  [����]",	/* TourokuDicMode */
  (unsigned char *)"  [��]",	/* QuotedInsertMode */
  (unsigned char *)"  [�Խ�]",	/* BubunMuhenkanMode */
  (unsigned char *)"  [����]",  /* MountDicMode */
  };

static char
HenkanModeID[MAX_JISHU] = {
  IROHA_MODE_EmptyMode,
  IROHA_MODE_ZenKataHenkanMode,
  IROHA_MODE_HanKataHenkanMode,
  IROHA_MODE_ZenAlphaHenkanMode,
  IROHA_MODE_HanAlphaHenkanMode,
};

currentModeInfo(d)
uiContext d;
{
  coreContext cc = (coreContext)d->modec;

  if (d->current_mode->flags & IROHA_KANJIMODE_EMPTY_MODE) {
    d->kanji_status_return->info |= KanjiEmptyInfo;
  }

  if (howToReturnModeInfo == BySTRING) {
    unsigned char *modename;
    if (cc->minorMode != d->minorMode
	&& (modename = ModeNames[cc->minorMode].name)
	&& (ModeNames[d->minorMode].name == (unsigned char *)0 ||
	    strcmp(modename, ModeNames[d->minorMode].name))) {
      d->majorMode = cc->majorMode;
      d->minorMode = cc->minorMode;
      d->kanji_status_return->mode = modename;
      d->kanji_status_return->info |= KanjiModeInfo;
    }
  }
  else {
    if (cc->majorMode != d->majorMode) {
      d->majorMode = cc->majorMode;
      d->minorMode = cc->minorMode;
      numMode[0] = (unsigned char)('@' + cc->majorMode);
      numMode[1] = 0;
      d->kanji_status_return->info |= KanjiModeInfo;
      d->kanji_status_return->mode = numMode;
    }
  }
}
     
/* ���Υե�����ˤϥ⡼���ѹ��˴ؤ��������̤����äƤ��롣�⡼�ɤ���
 * ���Ȥϡ����޻������Ѵ���ɽ�̤˸����⡼�ɤ��ѹ������ǤϤʤ�����
 * ���ɤߤ��ʤ����֤��顢�ɤߤ�������֤˰ܤ���ˤ��������Τ�ؤ���
 */

initModeNames()
{
  int i;

  for (i = 0 ; i < IROHA_MODE_MAX_IMAGINARY_MODE ; i++) {
    ModeNames[i].alloc = 0;
    ModeNames[i].name = _ModeNames[i];
  }
}

resetModeNames()
{
  int i;

  for (i = 0 ; i < IROHA_MODE_MAX_IMAGINARY_MODE ; i++) {
    if (ModeNames[i].alloc && ModeNames[i].name) {
      ModeNames[i].alloc = 0;
      free(ModeNames[i].name);
    }
    ModeNames[i].name = _ModeNames[i];
  }
}

static
exitYomiQuotedInsert(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d);
  return retval;
}

static coreContext
newCoreContext()
{
  coreContext cc;

  if (cc = (coreContext)malloc(sizeof(coreContextRec))) {
    cc->id = CORE_CONTEXT;
  }
  return cc;
}

void
freeCoreContext(cc)
coreContext cc;
{
  free(cc);
}

struct callback *pushCallback();

yomiQuotedInsertMode(d)
uiContext d;
{
  extern KanjiModeRec yomi_quoted_insert_mode;
  coreContext cc;

  cc = newCoreContext();
  if (cc == 0) {
    return NothingChangedWithBeep(d);
  }
  cc->prevMode = d->current_mode;
  cc->next = d->modec;
  cc->majorMode = d->majorMode;
  cc->minorMode = IROHA_MODE_QuotedInsertMode;
  if (pushCallback(d, d->modec, 0, exitYomiQuotedInsert, 0, 0) ==
      (struct callback *)0) {
    freeCoreContext(cc);
    return NothingChangedWithBeep(d);
  }
  d->modec = (mode_context)cc;
  d->current_mode = &yomi_quoted_insert_mode;
  currentModeInfo(d);
}

static
simplePopCallback(d, retval, env)
uiContext d;
int retval;
mode_context env;
/* ARGSUSED */
{
  struct callback *oldCB;


  popCallback(d);
  currentModeInfo(d);
  return retval;
}

static
japaneseMode(d)
uiContext d;
{
  coreContext cc = (coreContext)d->modec;

  d->current_mode = cc->prevMode;
  d->modec = cc->next;
  freeCoreContext(cc);
  d->status = EXIT_CALLBACK;
}

alphaMode(d)
uiContext d;
{
  extern KanjiModeRec alpha_mode;
  static unsigned char bad[] = "���꤬­��ޤ���";
  coreContext cc;

  cc = newCoreContext();
  if (cc == (coreContext)0) {
    makeGLineMessage(d, bad, strlen(bad));
    return 0;
  }
  if (pushCallback(d, cc->next,
		   0, simplePopCallback, simplePopCallback, 0) == 0) {
    freeCoreContext(cc);
    makeGLineMessage(d, bad, strlen(bad));
    return 0;
  }
  cc->prevMode = d->current_mode;
  cc->next = d->modec;
  cc->majorMode =
    cc->minorMode = IROHA_MODE_AlphaMode;
  d->current_mode = &alpha_mode;
  d->modec = (mode_context)cc;
  return 0;
}

/* cfuncdef

  JapaneseMode(d) -- �⡼�ɤ����ܸ����ϥ⡼�ɤ��Ѥ��롣

  ���� ���δؿ��Ϧ��⡼�ɤǤ����Ƥ�ǤϤ����ʤ���

 */

JapaneseMode(d)
uiContext d;
{
  yomiContext yc = (yomiContext)(((coreContext)d->modec)->next);

  if (yc->generalFlags & IROHA_YOMI_CHGMODE_INHIBITTED) {
    return NothingChangedWithBeep(d);
  }

  japaneseMode(d);
  d->kanji_status_return->length = 0;
  return 0;
}

AlphaMode(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  if (yc->generalFlags & IROHA_YOMI_CHGMODE_INHIBITTED) {
    return NothingChangedWithBeep(d);
  }
  else {
    alphaMode(d);
    currentModeInfo(d);
    d->kanji_status_return->length = 0;
    return 0;
  }
}

static
KakuteiMode(d, newModeID, newMode)
uiContext d;
int newModeID;
KanjiMode newMode;
{
  yomiContext yc = (yomiContext)d->modec;
  int mode;

  if (yc->generalFlags & IROHA_YOMI_CHGMODE_INHIBITTED) {
    return NothingChangedWithBeep(d);
  }

  if (yc->generalFlags & IROHA_YOMI_END_IF_KAKUTEI) {
    /* ����ǽ����褦�ʥ⡼�ɤ��ä������⡼�ɤˤʤ�ʤ� */
    if (IROHA_MODE_ZenHiraKakuteiMode <= newModeID && 
	newModeID <= IROHA_MODE_HanAlphaKakuteiMode) {
      yc->baseChar = newModeID - IROHA_MODE_ZenHiraKakuteiMode;
    }
    else {
      yc->baseChar = JISHU_HIRA;
    }
    currentModeInfo(d);
    return 0;
  }
      
  /* ���ߤΥ⡼�ɤ���� */
  mode = (int)yc->minorMode;
  
  d->current_mode = newMode;
  yc->majorMode = yc->minorMode = (char)newModeID;
  currentModeInfo(d);

  if(yc->rCurs)
    return kakuteiKakutei(d, mode);

  d->kanji_status_return->length = 0;
  return 0;
}

ZenHiraKakuteiMode(d)
uiContext d;
{
  extern KanjiModeRec kzhr_mode;

  return KakuteiMode(d, IROHA_MODE_ZenHiraKakuteiMode, &kzhr_mode);
}

ZenKataKakuteiMode(d)
uiContext d;
{
  extern KanjiModeRec kzkt_mode;

  return KakuteiMode(d, IROHA_MODE_ZenKataKakuteiMode, &kzkt_mode);
}

HanKataKakuteiMode(d)
uiContext d;
{
  extern KanjiModeRec khkt_mode;
  extern InhibitHankakuKana;

  if (InhibitHankakuKana)
    return NothingChangedWithBeep(d);
  else
    return KakuteiMode(d, IROHA_MODE_HanKataKakuteiMode, &khkt_mode);
}

ZenAlphaKakuteiMode(d)
uiContext d;
{
  extern KanjiModeRec kzal_mode;

  return KakuteiMode(d, IROHA_MODE_ZenAlphaKakuteiMode, &kzal_mode);
}

HanAlphaKakuteiMode(d)
uiContext d;
{
  extern KanjiModeRec khal_mode;

  return KakuteiMode(d, IROHA_MODE_HanAlphaKakuteiMode, &khal_mode);
}

HenkanNyuryokuMode(d)
uiContext d;
{
  extern KanjiModeRec empty_mode;
  yomiContext yc = (yomiContext)d->modec;

  return KakuteiMode(d, HenkanModeID[yc->baseChar], &empty_mode);
}

queryMode(d, arg)
uiContext d;
unsigned char *arg;
{
  coreContext cc = (coreContext)d->modec;

  if (howToReturnModeInfo == BySTRING) {
    if (ModeNames[d->minorMode].name == (unsigned char *)0) {
      strcpy(arg, "");
    }
    else {
      strcpy(arg, ModeNames[d->minorMode].name);
    }
  }
  else {
    arg[0] = (unsigned char)('@' + (int)cc->majorMode);
    arg[1] = '\0';
  }
  return 0;
}

getMinorMode(d)
uiContext d;
{
  coreContext cc = (coreContext)d->modec;

  return (int)cc->minorMode;
}

static
getMajorMode(d)
uiContext d;
{
  coreContext cc = (coreContext)d->modec;

  return (int)cc->majorMode;
}

/* 
 *   ����⡼�ɤ��Ф��ƥ⡼��ɽ��ʸ�������ꤹ�롣
 *
 */

changeModeName(modeid, str)
int modeid;
unsigned char *str;
{
  if (0 <= modeid && modeid < IROHA_MODE_MAX_IMAGINARY_MODE) {
    ModeNames[modeid].alloc = 1;
    ModeNames[modeid].name = str;
    return 0;
  }
  return -1;
}
