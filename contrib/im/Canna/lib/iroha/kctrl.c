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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/kctrl.c,v 3.8 91/09/03 18:54:48 kon Exp $";

#include <sys/types.h>
#include "iroha.h"

#define DEFAULT_COLUMN_WIDTH	70

int KC_initialize(), KC_finalize();

static int KC_changeMode(), KC_setWidth(), KC_setUFunc();
static int KC_setBunsetsuKugiri(), KC_setModeInfoStyle();
static int KC_setHexInputStyle(), KC_inhibitHankakuKana();
static int KC_defineKanji(), KC_kakutei(), KC_kill(), KC_modekeys();
static int KC_queryMode(), KC_queryConnection(), KC_setServerName();
static int KC_parse(), KC_yomiInfo(), KC_storeYomi(), KC_setInitFileName();
static int KC_do(), KC_getContext(), KC_closeUIContext();
static int KC_inhibitChangeMode(), KC_letterRestriction();
static int KC_queryMaxModeStr(), KC_setListCallback(), KC_setVerbose();

static int insertEmptySlots(), KC_setWidth(), callCallback();

/* KanjiControl�θġ�������ؿ��ؤΥݥ��� */

static int (*kctlfunc[])() = {
  KC_initialize,
  KC_finalize,
  KC_changeMode,
  KC_setWidth,
  KC_setUFunc,
  KC_setBunsetsuKugiri,
  KC_setModeInfoStyle,
  KC_setHexInputStyle,
  KC_inhibitHankakuKana,
  KC_defineKanji,
  KC_kakutei,
  KC_kill,
  KC_modekeys,
  KC_queryMode,
  KC_queryConnection,
  KC_setServerName,
  KC_parse,
  KC_yomiInfo,
  KC_storeYomi,
  KC_setInitFileName,
  KC_do,
  KC_getContext,
  KC_closeUIContext,
  KC_inhibitChangeMode,
  KC_letterRestriction,
  KC_queryMaxModeStr,
  KC_setListCallback,
  KC_setVerbose,
};


initRomeStruct(d)
uiContext d;
{
  extern InitialMode;
  extern KanjiModeRec alpha_mode, empty_mode;
  extern KanjiModeRec kzhr_mode, kzkt_mode, kzal_mode;
  extern KanjiModeRec khkt_mode, khal_mode;
  yomiContext yc;

  bzero(d, sizeof(uiContextRec));

  if (insertEmptySlots(d) < 0) {
    return -1;
  }

  yc = (yomiContext)d->modec;
  /* defaultContext �� -1 �λ��ϥ����ФˤĤʤ��äƤ��ʤ� */
  if (defaultContext != -1) {
    d->contextCache = RkDuplicateContext(defaultContext);
    d->bushuContext = RkDuplicateContext(defaultBushuContext);
  }
  else {
    d->contextCache = -1;
    d->bushuContext = -1;
  }

  /* �������ޥ����Υ��˥����⡼�ɤˤ������äƽ���� */
  d->majorMode = IROHA_MODE_HenkanMode;
  yc->majorMode = yc->minorMode = InitialMode;
  switch (InitialMode) {
  case IROHA_MODE_HenkanMode:
    d->minorMode = InitialMode;
    d->current_mode = &empty_mode;
    break;
  case IROHA_MODE_ZenKataHenkanMode:
  case IROHA_MODE_HanKataHenkanMode:
  case IROHA_MODE_ZenAlphaHenkanMode:
  case IROHA_MODE_HanAlphaHenkanMode:
    d->majorMode = d->minorMode = InitialMode;
    d->current_mode = &empty_mode;
    yc->baseChar = InitialMode - IROHA_MODE_ZenKataHenkanMode + 1;
    break;
  case IROHA_MODE_ZenHiraKakuteiMode:
    d->majorMode = d->minorMode = InitialMode;
    d->current_mode = &kzhr_mode;
    break;
  case IROHA_MODE_ZenKataKakuteiMode:
    d->majorMode = d->minorMode = InitialMode;
    d->current_mode = &kzkt_mode;
    break;
  case IROHA_MODE_ZenAlphaKakuteiMode:
    d->majorMode = d->minorMode = InitialMode;
    d->current_mode = &kzal_mode;
    break;
  case IROHA_MODE_HanKataKakuteiMode:
    d->majorMode = d->minorMode = InitialMode;
    d->current_mode = &khkt_mode;
    break;
  case IROHA_MODE_HanAlphaKakuteiMode:
    d->majorMode = d->minorMode = InitialMode;
    d->current_mode = &khal_mode;
    break;
  default:
    d->majorMode = d->minorMode = IROHA_MODE_AlphaMode;
    yc->majorMode = yc->minorMode = IROHA_MODE_HenkanMode;
    alphaMode(d);
    break;
  }
  return 0;
}

static void
freeModec(modec)
mode_context modec;
{
  coreContext cc;
  union {
    coreContext c;
    yomiContext y;
    jishuContext j;
    henkanContext h;
    ichiranContext i;
    forichiranContext f;
    mountContext m;
    tourokuContext t;
    adjustContext a;
  } gc;

  cc = (coreContext)modec;
  while (cc) {
    switch (cc->id) {
    case CORE_CONTEXT:
      gc.c = cc;
      cc = (coreContext)gc.c->next;
      freeCoreContext(gc.c);
      break;
    case YOMI_CONTEXT:
      gc.y = (yomiContext)cc;
      cc = (coreContext)gc.y->next;
      freeYomiContext(gc.y);
      break;
    case JISHU_CONTEXT:
      gc.j = (jishuContext)cc;
      cc = (coreContext)gc.j->next;
      freeJishuContext(gc.j);
      break;
    case HENKAN_CONTEXT:
      gc.h = (henkanContext)cc;
      cc = (coreContext)gc.j->next;
      freeHenkanContext(gc.h);
      break;
    case ICHIRAN_CONTEXT:
      gc.i = (ichiranContext)cc;
      cc = (coreContext)gc.i->next;
      freeIchiranContext(gc.i);
      break;
    case FORICHIRAN_CONTEXT:
      gc.f = (forichiranContext)cc;
      cc = (coreContext)gc.f->next;
      freeIchiranContext(gc.f);
      break;
    case MOUNT_CONTEXT:
      gc.m = (mountContext)cc;
      cc = (coreContext)gc.m->next;
      freeIchiranContext(gc.i);
      break;
    case TOUROKU_CONTEXT:
      gc.t = (tourokuContext)cc;
      cc = (coreContext)gc.t->next;
      freeTourokuContext(gc.t);
      break;
    case ADJUST_CONTEXT:
      gc.a = (adjustContext)cc;
      cc = (coreContext)gc.a->next;
      freeAdjustContext(gc.a);
      break;
    default:
      /* ���������ΤϤ������� */
      break;
    }
  }
}

static void
freeCallbacks(cb)
struct callback *cb;
{
  struct callback *nextcb;

  for (; cb ; cb = nextcb) {
    nextcb = cb->next;
    free(cb);
  }
}

void
freeRomeStruct(d)
uiContext d;
{
  freeModec(d->modec, d->current_mode);
  if (d->cb) {
    freeCallbacks(d->cb);
  }
  if (d->contextCache >= 0) {
    RkCloseContext(d->contextCache);
  }
  if (d->bushuContext >= 0) {
    RkCloseContext(d->bushuContext);
  }
  free(d);
}

static
insertEmptySlots(d)
uiContext d;
{
  extern KanjiModeRec empty_mode;
  extern yomiContext newYomiContext();
  struct callback *pushCallback();
  yomiContext yc;

  if (pushCallback(d, NULL, NO_CALLBACK, NO_CALLBACK, NO_CALLBACK) ==
      (struct callback *)0) {
    return NoMoreMemory();
  }

  yc = newYomiContext(0, 0, /* ��̤ϳ�Ǽ���ʤ� */
		      IROHA_NOTHING_RESTRICTED,
		      !IROHA_YOMI_CHGMODE_INHIBITTED,
		      !IROHA_YOMI_END_IF_KAKUTEI,
		      IROHA_YOMI_INHIBIT_NONE,
		      JISHU_HIRA,
		      0/* �ǥե���Ȥ��Ѵ� */);
  if (yc == (yomiContext)0) {
    popCallback(d);
    return NoMoreMemory();
  }
  d->modec = (mode_context)yc;
  yc->majorMode = yc->minorMode = IROHA_MODE_HenkanMode;
  d->majorMode = d->minorMode = IROHA_MODE_HenkanMode;
  d->current_mode = &empty_mode;
  d->ncolumns = DEFAULT_COLUMN_WIDTH;
  return 0;
}

kanjiControl(request, d, arg)
int request;
uiContext d;
caddr_t arg;
{
  return kctlfunc[request](d, arg);
}


/* 

  display �� window ���Ȥ� ����ƥ�����ID ��ºݤΥ���ƥ����Ȥ��б���
  ���뤿��Υϥå���ơ��֥�

  display �� window �������륭���ΤȤ��������ȡ������˥���ƥ�����
  �����äƤ����Ψ���⤤���⤷���äƤ��ʤ��Ȥ⡢�ݥ��󥿥�������򤿤ɤä�
  �Ԥ��Ȥ��Ĥ��ϥ���ƥ����Ȥ�������˰㤤�ʤ���

 */

#define HASHTABLESIZE 96

static struct bukRec {
  unsigned int data1, data2;
  uiContext context;
  struct bukRec *next;
} *conHash[HASHTABLESIZE];


/* �ϥå��奭������ؿ�(�����ø�) */

static unsigned int
makeKey(data1, data2)
unsigned int data1, data2;
{
  unsigned int key;

  key = data1 % HASHTABLESIZE;
  key += data2 % HASHTABLESIZE;
  key %= HASHTABLESIZE;
  return key;
}

/* 

  keyToContext -- Display �� Window ���Ȥʤɤ��饳��ƥ����Ȥ���Ф�����

  display �� window ���Ȥ�����ƥ����Ȥ���äƤ���Ф��Υ���ƥ�����
  ���֤���

  ���äƤ��ʤ��ΤǤ���С�NULL ���֤���

  */

uiContext 
keyToContext(data1, data2)
unsigned int data1, data2;
{
  unsigned int key;
  struct bukRec *p;

  key = makeKey(data1, data2);
  for (p = conHash[key] ; p ; p = p->next) {
    if (p->data1 == data1 && p->data2 == data2) {
      /* ����㤢����ƥ����Ȥ����Ĥ���ޤ����� */
      return p->context;
    }
  }
  return (uiContext)NULL; /* ���Ĥ���ޤ���Ǥ����� */
}


/* internContext -- �ϥå���ơ��֥����Ͽ���� 

  ���ΤȤ������ˡ�display �� window ���Ȥ�¸�ߤ���ΤǤ���С�
  ������ˤĤʤ��äƤ��륳��ƥ����Ȥ�ե꡼����Τ���ա���

*/

struct bukRec *
internContext(data1, data2, context)
unsigned int data1, data2;
uiContext context;
{
  unsigned int key;
  struct bukRec *p, **pp;

  key = makeKey(data1, data2);
  for (pp = &conHash[key] ; p = *pp ; pp = &(p->next)) {
    if (p->data1 == data1 && p->data2 == data2) {
      freeRomeStruct(p->context);
      p->context = context;
      return p;
    }
  }
  p = *pp = (struct bukRec *)malloc(sizeof(struct bukRec));
  if (p) {
    p->data1 = data1;
    p->data2 = data2;
    p->context = context;
    p->next = (struct bukRec *)NULL;
  }
  return p;
}


/* rmContext -- �ϥå���ơ��֥뤫��������

*/

void
rmContext(data1, data2)
unsigned int data1, data2;
{
  unsigned int key;
  struct bukRec *p, **pp;

  key = makeKey(data1, data2);
  for (pp = &conHash[key] ; p = *pp ; pp = &(p->next)) {
    if (p->data1 == data1 && p->data2 == data2) {
      *pp = p->next;
      free(p);
    }
  }
}

/* cfuncdef

  freeBukRecs() -- �ݥ���Ȥ���Ƥ�����ΥХ��åȤΥե꡼

  �Х��åȤˤ�äƥݥ���Ȥ���Ƥ���ǡ����򤹤٤ƥե꡼���롣
  �ե꡼���оݤˤ� uiContext ��ޤޤ�롣

*/

static void
freeBukRecs(p)
struct bukRec *p;
{
  struct bukRec *nextp;

  if (p) { /* reconfirm that p points some structure */
    freeRomeStruct(p->context);
    nextp = p->next;
    if (nextp) {
      freeBukRecs(nextp);
    }
    free(p);
  }
}

/* cfuncdef

  clearHashTable() -- �ϥå���ơ��֥�����Ƥ򤹤٤ƥե꡼���롣

*/

static void
clearHashTable()
{
  int i;
  struct bukRec *p;

  for (i = 0 ; i < HASHTABLESIZE ; i++) {
    p = conHash[i];
    conHash[i] = 0;
    if (p) {
      freeBukRecs(p);
    }
  }
}

#define NWARNINGMESG 64
static char *WarningMesg[NWARNINGMESG + 1]; /* +1 �ϺǸ�� NULL �ݥ��󥿤�ʬ */
static int nWarningMesg = 0;

void
initWarningMesg()
{
  int i;

  for (i = 0 ; i < nWarningMesg ; i++) {
    free(WarningMesg[i]);
    WarningMesg[i] = (char *)NULL;
  }
  nWarningMesg = 0;
}

void
addWarningMesg(s)
char *s;
{
  int n;
  char *work;

  if (nWarningMesg < NWARNINGMESG) {
    n = strlen(s);
    work = (char *)malloc(n + 1);
    if (work) {
      strcpy(work, s);
      WarningMesg[nWarningMesg++] = work;
    }
  }
}

int FirstTime = 1;

KC_initialize(d, arg)
uiContext d;
char *arg;
{
  extern FirstTime;
  char romajifile[1024];

  if (FirstTime) {

    debug_message("KC_INITIALIZE �������ˤ��뤾\n",0,0,0);

    /* �������˥󥰥�å������ν���� */
    initWarningMesg();

    /* �⡼��̾�ν���� */
    initModeNames();

    /* �����ơ��֥�ν���� */
    initKeyTables();

    /* ������ե�������ɤ߹��� */
    parse();

    /* ���޻������Ѵ��ν���� */
    if (RomkanaInit(romajifile) < 0) {
      /* ���顼 necKanjiError ��ʸ����Ȥ����֤ä���Ƥ��� */
      addWarningMesg(necKanjiError);
    }

    /* ���ʴ����Ѵ��ν���� */
    KanjiInit();
    /* �����Ǥ⥨�顼��̵�뤷�ޤ��������ˤʤ�ʤ��Ƥ⤤������ */

    if (arg) {
      *(char ***)arg = nWarningMesg ? WarningMesg : (char **)NULL;
    }
    FirstTime = 0;
  }
  return 0;
}

static void
restoreBindings()
{
  extern char *RomkanaTable, *RengoGakushu, *KatakanaGakushu;
  extern int InitialMode, CursorWrap, SelectDirect, HexkeySelect;
  extern int nkanjidics, nuserdics, nbushudics, nlocaldics;
  extern int BunsetsuKugiri, Gakushu, QuitIchiranIfEnd, BreakIntoRoman;
  extern int InhibitHankakuKana, gramaticalQuestion, kouho_threshold;
  extern int stayAfterValidate, kakuteiIfEndOfBunsetsu;

  RomkanaTable = (char *)NULL;
  RengoGakushu = (char *)NULL;
  KatakanaGakushu = (char *)NULL;
  InitialMode = 0;
  CursorWrap = 1;
  SelectDirect = 0;
  HexkeySelect = 1;
  BunsetsuKugiri = 0;
  nkanjidics = 0;
  nbushudics = 0;
  nlocaldics = 0;
  nuserdics = 0;
  kanjidicname[nkanjidics] = (char *)NULL;
  bushudicname[nkanjidics] = (char *)NULL;
  localdicname[nkanjidics] = (char *)NULL;
  userdicname[nkanjidics] = (char *)NULL;
  setIchitaroMove(0);
  setReverseOnlyCursor(1);
  Gakushu = 1;
  QuitIchiranIfEnd = 0;
  BreakIntoRoman = 0;
  kouho_threshold = 2;
  gramaticalQuestion = 1;
  stayAfterValidate = 1;
  kakuteiIfEndOfBunsetsu = 0;  

  InhibitHankakuKana = 0;
}

KC_finalize(d, arg)
uiContext d;
char *arg;
{
  extern FirstTime;
  int res;
  
  if (FirstTime) {
    necKanjiError = "������⤵��Ƥ��ʤ��Τˡؽ����٤ȸ����ޤ���";
    return -1;
  }
  else {
    FirstTime = 1;

    /* �������˥󥰥�å������ν���� */
    initWarningMesg();

    /* ���ʴ����Ѵ��ν�λ */
    res = KanjiFin();

    /* ���޻������Ѵ��ν�λ */
    RomkanaFin();

    /* free all uiContexts and hash tables here */

    /* �����ޥåץơ��֥�Υ��ꥢ */
    restoreDefaultKeymaps();

    /* �⡼��ʸ����Υե꡼ */
    resetModeNames();

    /* uiContext �θ����Τ���Υϥå���ơ��֥�򥯥ꥢ
       uiContext ����˥ե꡼���� */
    clearHashTable();

    /* �����ȥ������ޥ�������Ƥ������Ȥ��᤹�� */
    restoreBindings();

    if (arg) {
      *(char ***)arg = nWarningMesg ? WarningMesg : (char **)NULL;
    }
    return res;
  }
}

static
KC_setWidth(d, arg)
uiContext d;
caddr_t arg;
{
  d->ncolumns = (int)arg;
  return 0;
}

static
KC_setBunsetsuKugiri(d, arg)
uiContext d;
caddr_t arg;
{
  SetBunsetsuKugiri(arg);
  return 0;
}

#define CHANGEBUFSIZE 1024

static
KC_changeMode(d, arg)
uiContext d;
jrKanjiStatusWithValue *arg;
{
  coreContext cc;

  d->buffer_return = arg->buffer;
  d->bytes_buffer = arg->bytes_buffer;
  d->kanji_status_return = arg->ks;

  bzero(d->kanji_status_return, sizeof(jrKanjiStatus));

  d->nbytes = escapeToBasicStat(d, IROHA_FN_Quit);
  cc = (coreContext)d->modec;
  if (cc->majorMode == IROHA_MODE_AlphaMode) {
    /* ���⡼�ɤ��ä���ȴ���롣
       �١����å��⡼�ɤϦ��⡼�ɤ��Ѵ��⡼��(���ʴ����Ѵ��⡼�ɡ�
       �������ϥ⡼��)���餤�����ʤ��Ȼפ��� */
    if (arg->val == IROHA_MODE_AlphaMode) {
      return 0;
    }
    else {
      doFunc(d, IROHA_FN_JapaneseMode);
    }
  }
  switch (arg->val) {
  case IROHA_MODE_AlphaMode:
    arg->val = doFunc(d, IROHA_FN_AlphaMode);
    return 0;

  case IROHA_MODE_HenkanMode:
    arg->val = doFunc(d, IROHA_FN_HenkanNyuryokuMode);
    return 0;

  case IROHA_MODE_HexMode:
    arg->val = doFunc(d, IROHA_FN_HexMode);
    return 0;

  case IROHA_MODE_BushuMode:
    arg->val = doFunc(d, IROHA_FN_BushuMode);
    return 0;

  case IROHA_MODE_KigoMode:
    arg->val = doFunc(d, IROHA_FN_KigouMode);
    return 0;

  case IROHA_MODE_ZenHiraKakuteiMode:
    arg->val = doFunc(d, IROHA_FN_ZenHiraKakuteiMode);
    return 0;

  case IROHA_MODE_ZenKataKakuteiMode:
    arg->val = doFunc(d, IROHA_FN_ZenKataKakuteiMode);
    return 0;

  case IROHA_MODE_HanKataKakuteiMode:
    arg->val = doFunc(d, IROHA_FN_HanKataKakuteiMode);
    return 0;

  case IROHA_MODE_ZenAlphaKakuteiMode:
    arg->val = doFunc(d, IROHA_FN_ZenAlphaKakuteiMode);
    return 0;

  case IROHA_MODE_HanAlphaKakuteiMode:
    arg->val = doFunc(d, IROHA_FN_HanAlphaKakuteiMode);
    return 0;
  }
}

static
baseModeP(d)
uiContext d;
{
  extern KanjiModeRec alpha_mode, empty_mode;

  return (d->current_mode == &alpha_mode ||
	  (d->current_mode == &empty_mode &&
	   ((yomiContext)(d->modec))->next == (mode_context)0));
}

/*

  ����Ū�ʾ��֤ˤ�ɤ롣���ʤ�����ɤߤ����äƤ������Ѵ���ξ��֤���ȴ
  ���롣������ȴ���뤫���裲�����ǻ��ꤹ�롣ȴ�����Ȥ��Ƥ�

  ��QUIT (C-g) ��ȴ����
  ������ (Return) ��ȴ����

  �����롣

*/

escapeToBasicStat(d, how)
uiContext d;
int how;
{
  int len = 0, totallen = 0;
  unsigned char *p = d->buffer_return;
  int totalinfo = 0;
  int maxcount = 32;

  do {
    d->kanji_status_return->length = 0;
    totalinfo |= (d->kanji_status_return->info & KanjiModeInfo);
    d->kanji_status_return->info = 0;
    d->nbytes = 0; /* �����ͤ�����ʸ��Ĺ�Ȥ��ƻȤ���礬����Τǥ��ꥢ���� */
    len = doFunc(d, how);
    d->buffer_return += len;
    d->bytes_buffer -= len;
    totallen += len;
    maxcount--;
  } while (maxcount > 0 && !baseModeP(d));
  d->kanji_status_return->info |= KanjiGLineInfo | totalinfo;
  d->kanji_status_return->gline.length = 0;
  d->buffer_return = p;
  return totallen;
}

static
KC_setUFunc(d, arg)
uiContext d;
caddr_t arg;
{
  extern howToBehaveInCaseOfUndefKey;

  howToBehaveInCaseOfUndefKey = (int)arg;
  return 0;
}

static
KC_setModeInfoStyle(d, arg)
uiContext d;
caddr_t arg;
{
  extern howToReturnModeInfo;

  howToReturnModeInfo = (int)arg;
  return 0;
}

static
KC_setHexInputStyle(d, arg)
uiContext d;
caddr_t arg;
{
  extern hexCharacterDefinigStyle;

  hexCharacterDefinigStyle = (int)arg;
  return 0;
}

static
KC_inhibitHankakuKana(d, arg)
uiContext d;
caddr_t arg;
{
  extern InhibitHankakuKana;

  InhibitHankakuKana = (int)arg;
  return 0;
}

static
popTourokuWithGLineClear(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  tourokuContext tc;

  popCallback(d); /* �ɤߤ� pop */

  tc = (tourokuContext)d->modec;
  freeGetIchiranList(tc->udic);
  popTourokuMode(d);
  popCallback(d);
  GlineClear(d);
  currentModeInfo(d);
  return 0;
}


static
KC_defineKanji(d, arg)
uiContext d;
jrKanjiStatusWithValue *arg;
{
  d->buffer_return = arg->buffer;
  d->bytes_buffer = arg->bytes_buffer;
  d->kanji_status_return = arg->ks;

  if(arg->ks->length > 0 && arg->ks->echoStr && arg->ks->echoStr[0]) {
    char xxxx[ROMEBUFSIZE];

    strncpy(xxxx, arg->ks->echoStr, arg->ks->length);
    xxxx[arg->ks->length] = (char)NULL;
    
    bzero(d->kanji_status_return, sizeof(jrKanjiStatus));

    d->nbytes = escapeToBasicStat(d, IROHA_FN_Quit);
    dicTourokuControl(d, xxxx, popTourokuWithGLineClear);
    arg->val = d->nbytes;
  } else {
    d->nbytes = escapeToBasicStat(d, IROHA_FN_Quit);
    arg->val = dicTourokuControl(d, 0, popTourokuWithGLineClear);
  }
  arg->val = callCallback(d, arg->val);

  return 0;
}


/* cfuncdef

  RK ����ƥ����Ȥ�̵���ˤ��롣
  flag �����ʳ��ʤ� RkClose() ��Ԥ���

 */

static void
closeRK(cxp, flag)
int *cxp;
int flag;
{
  if (flag && *cxp >= 0) {
    RkCloseContext(*cxp);
  }
  *cxp = -1;
}

/* cfuncdef

   closeRKContextInUIContext -- uiContext ��� RK ����ƥ����Ȥ� close ���롣

 */

static
closeRKContextInUIContext(d, flag)
uiContext d;
int flag; /* ���ʳ��ʤ饯�����⤹�롣 */
{
  coreContext cc;

  closeRK(&(d->contextCache), flag);
  closeRK(&(d->bushuContext), flag);
  for (cc = (coreContext)d->modec ; cc ; cc = (coreContext)cc->next) {
    switch (cc->id) {
    case HENKAN_CONTEXT:
      closeRK(&(((henkanContext)cc)->context), flag);
      break;
    }
  }
}

/* cfuncdef

  closeRKContextInMemory() -- ���٤Ƥ� RK ����ƥ����ȤΥ�����

  �Х��åȤˤ�äƥݥ���Ȥ���Ƥ���ǡ���������Ƥ� RK ����ƥ����Ȥ�
  ���������롣

*/

static void
closeRKContextInMemory(p, flag)
struct bukRec *p;
int flag;
{
  struct bukRec *nextp;

  while (p) { /* reconfirm that p points some structure */
    closeRKContextInUIContext(p->context, flag);
    p = p->next;
  }
}

/* cfuncdef

  makeContextToBeClosed() -- �ϥå���ơ��֥���Υ���ƥ����Ȥ�̵���ˤ���

*/

void
makeAllContextToBeClosed(flag)
int flag;
{
  int i;
  struct bukRec *p;

  for (i = 0 ; i < HASHTABLESIZE ; i++) {
    p = conHash[i];
    if (p) {
      closeRKContextInMemory(p, flag);
    }
  }
}

static
KC_kakutei(d, arg)
uiContext d;
jrKanjiStatusWithValue *arg;
{
  d->buffer_return = arg->buffer;
  d->bytes_buffer = arg->bytes_buffer;
  d->kanji_status_return = arg->ks;

  bzero(d->kanji_status_return, sizeof(jrKanjiStatus));

  d->nbytes = escapeToBasicStat(d, IROHA_FN_Kakutei);
  if ( !baseModeP(d) ) {
    d->nbytes = escapeToBasicStat(d, IROHA_FN_Quit);
  }
  arg->val = d->nbytes;
  return d->nbytes;
}

static
KC_kill(d, arg)
uiContext d;
jrKanjiStatusWithValue *arg;
{
  d->buffer_return = arg->buffer;
  d->bytes_buffer = arg->bytes_buffer;
  d->kanji_status_return = arg->ks;

  bzero(d->kanji_status_return, sizeof(jrKanjiStatus));

  d->nbytes = escapeToBasicStat(d, IROHA_FN_Quit);
  arg->val = d->nbytes;
  return d->nbytes;
}

static
KC_modekeys(d, arg)
uiContext d;
unsigned char *arg;
{
  int n = 0;
  int i;
  extern KanjiModeRec alpha_mode;
  int func;

  for (i = 0 ; i < 256 ; i++) {
    func = alpha_mode.keytbl[i];
    if (func != IROHA_FN_SelfInsert &&
	func != IROHA_FN_FunctionalInsert &&
	func != IROHA_FN_Undefined &&
	func != IROHA_FN_FuncSequence &&
	func != IROHA_FN_UseOtherKeymap	&&
	alpha_mode.ftbl[func]) {
      arg[n++] = i;
    }
  }
  return n;
}

#define BySTRING 0
#define ByNUMBER 1

static
KC_queryMode(d, arg)
uiContext d;
unsigned char *arg;
{
  return queryMode(d, arg);
}

static
KC_queryConnection(d, arg)
uiContext d;
unsigned char *arg;
{
  if (defaultContext != -1) {
    return 1;
  }
  else {
    return 0;
  }
}

static
KC_setServerName(d, arg)
uiContext d;
unsigned char *arg;
{
  RkSetServerName(arg);
  return 0;
}

static
KC_parse(d, arg)
uiContext d;
char **arg;
{
  /* �������˥󥰥�å������ν���� */
  initWarningMesg();

  parse_string(*arg);

  *(char ***)arg = nWarningMesg ? WarningMesg : (char **)NULL;

  return nWarningMesg;
}

int yomiInfoLevel = 0;

static
KC_yomiInfo(d, arg)
uiContext d;
int arg;
{
  yomiInfoLevel = arg;
  return 0;
}

static
KC_storeYomi(d, arg)
uiContext d;
jrKanjiStatusWithValue *arg;
{
  extern KanjiModeRec yomi_mode;
  coreContext cc;
  unsigned char buf[2048];
  unsigned char *p, *q;
  int len = 0;

  p = arg->ks->echoStr;
  q = arg->ks->mode;
  if (p) {
    strcpy(buf, p);
    p = buf;
    len = strlen(buf);
  }
  if (q) {
    strcpy(buf + len + 1, q);
    q = buf + len + 1;
  }
  KC_kill(d, arg);
  cc = (coreContext)d->modec;
  if (cc->majorMode == IROHA_MODE_AlphaMode) {
    doFunc(d, IROHA_FN_JapaneseMode);
  }
  d->kanji_status_return = arg->ks;
  RomajiStoreYomi(d, p, q);
  if (p && *p) {
    d->current_mode = &yomi_mode;
  }
  makeYomiReturnStruct(d);
  arg->val = 0;
  return 0;
}

char *initFileSpecified = (char *)NULL;

static
KC_setInitFileName(d, arg)
uiContext d;
char *arg;
{
  int len;

  if (initFileSpecified) { /* �����Τ�Τ�ե꡼���� */
    free(initFileSpecified);
  }

  if ( arg && *arg ) {
    len = strlen(arg);
    initFileSpecified = (char *)malloc(len + 1);
    strcpy(initFileSpecified, arg);
  }
  else {
    initFileSpecified = (char *)NULL;
  }
  return 0;
}

static
KC_do(d, arg)
uiContext d;
jrKanjiStatusWithValue *arg;
{
  d->buffer_return = arg->buffer;
  d->bytes_buffer = arg->bytes_buffer;
  d->kanji_status_return = arg->ks;
  d->ch = *(d->buffer_return);
  d->nbytes = 1;

  bzero(d->kanji_status_return, sizeof(jrKanjiStatus));

  arg->val = doFunc(d, arg->val);
  return 0;
}

/*

  �ȥåץ�٥�ˤϤʤ��⡼�ɤ��Ф��Ʋ��餫�κ�Ȥ�
  ���������Ȥ��˸ƤӽФ��ؿ���fnum == 0 �� d->ch �򸫤롣

 */

_do_func_slightly(d, fnum, mode_c, c_mode)
uiContext d;
int fnum;
mode_context mode_c;
KanjiMode c_mode;
{
  uiContextRec e;
  jrKanjiStatus ks;

  e.buffer_return = e.genbuf;
  e.bytes_buffer = ROMEBUFSIZE;
  e.kanji_status_return = &ks;

  e.nbytes = d->nbytes;
  e.ch     = d->ch;

  e.status = 0; /* �⡼�ɤˤĤ���"������"�Υ��ơ�����������ͤȤ��� */
  e.more.todo = 0;
  e.modec = mode_c;
  e.current_mode = c_mode;

  return c_mode->func(&e, c_mode, e.ch, fnum);
}

static
callCallback(d, res)  /* ������Хå���Ƥ� */
uiContext d;
int res;
{
  struct callback *cbp;

  for (cbp = d->cb; cbp ;) {
    int index;
    int (*callbackfunc)();

    index = d->status;
    d->status = 0; /* Callback ���ʤ��Ƥ� EXIT��QUIT��AUX �ϥ��ꥢ���� */
    callbackfunc = cbp->func[index];
    if (callbackfunc) {
      d->kanji_status_return->info &= ~KanjiEmptyInfo;
      if (index) { /* everytime �ʳ� */
	res = callbackfunc(d, res, cbp->env);
	cbp = d->cb; /* ������Хå��ؿ��ˤ�ꥳ����Хå���
			�ƤӽФ����Τ�ٱ礹�뤿������ľ�� */
	/* �����ǥ�����Хå��ؿ���ݥåץ��åפ��褦���ɤ����ͤ��ɤ��� */
	continue;
      }
      else {
	res = callbackfunc(d, res, cbp->env);
      }
    }
    cbp = cbp->next;
  }
  return res;
}

_doFunc(d, fnum)
uiContext d;
int fnum;
{
  int res = 0, tmpres;
  struct callback *cbp;

  d->status = 0; /* �⡼�ɤˤĤ���"������"�Υ��ơ�����������ͤȤ��� */
  d->more.todo = 0;
  tmpres = d->current_mode->func(d, d->current_mode, d->ch, fnum);

  if (d->flags & MULTI_SEQUENCE_EXECUTED) {
    d->flags &= ~MULTI_SEQUENCE_EXECUTED;
    return tmpres;
  }

  /* ������Хå���¹Ԥ��� */
  res = tmpres = callCallback(d, tmpres);

  /* moreToDo ��¹Ԥ��ʤ��Ƥ� */
  while (d->more.todo) {
    unsigned char *prevEcho;
    int prevEchoLen, prevRevPos, prevRevLen;

    d->kanji_status_return->info &= ~(KanjiThroughInfo | KanjiEmptyInfo);
    d->more.todo = 0;
    d->ch = d->more.ch;	/* moreTodo �� more.ch �Ϥ���ʤ��ΤǤϡ� */
    d->nbytes = 1;
    d->buffer_return += tmpres;
    d->bytes_buffer -= tmpres;
    d->buffer_return[0] = d->ch;

    prevEcho = d->kanji_status_return->echoStr;
    prevEchoLen = d->kanji_status_return->length;
    prevRevPos = d->kanji_status_return->revPos;
    prevRevLen = d->kanji_status_return->revLen;
    d->kanji_status_return->length =
      d->kanji_status_return->revPos =
	d->kanji_status_return->revLen = 0;
    /* �����ܰʹߤ˰ʲ��Υǡ����������Ƥ����礬����Τ�����ľ���� */

    tmpres = _doFunc(d, d->more.fnum);

    if (tmpres >= 0) {
      if (d->kanji_status_return->length < 0) {
	d->kanji_status_return->length = prevEchoLen;
	d->kanji_status_return->echoStr = prevEcho;
	d->kanji_status_return->revPos = prevRevPos;
	d->kanji_status_return->revLen = prevRevLen;
      }
      res += tmpres;
    }
  }

  return res;
}

_afterDoFunc(d, retval)
uiContext d;
int retval;
{
  int res = retval;
  jrKanjiStatus   *kanji_status_return = d->kanji_status_return;

  /* GLine ��ä��ȸ����Τʤ�ä��ޤ��礦 */
  if (d->flags & PLEASE_CLEAR_GLINE) {
    if (d->flags & PCG_RECOGNIZED) { /* �����������ʤ� */
      if (res >= 0 &&	kanji_status_return->length >= 0) {
	d->flags &= ~(PLEASE_CLEAR_GLINE | PCG_RECOGNIZED);
	   /* ��������ܤ�̤����ޤ��� */
	if (!(kanji_status_return->info & KanjiGLineInfo)) {
	  GlineClear(d);
	}
      }
    }
    else {
      d->flags |= PCG_RECOGNIZED;
    }
  }
  return res;
}

/* cfuncdef

  doFunc -- _doFunc ���ɤ�ǡ������ ClearGLine �����䡢������Хå���
            �����򤹤롣

 */

doFunc(d, fnum)
uiContext d;
int fnum;
{
  return _afterDoFunc(d, _doFunc(d, fnum));
}

static
KC_getContext(d, arg)
uiContext d;
int arg;
{
  switch (arg)
    {
    case 0:
      return RkDuplicateContext(defaultContext);
    case 1:
      return RkDuplicateContext(defaultBushuContext);
    }
}

static
KC_closeUIContext(d, arg)
uiContext d;
jrKanjiStatusWithValue *arg;
{
  jrKanjiStatus *ks = d->kanji_status_return;
  extern struct ModeNameRecs ModeNames[];
  extern InitialMode;

  d->nbytes = escapeToBasicStat(d, IROHA_FN_Quit);
  arg->val = d->nbytes;
  freeRomeStruct(d);
  ks->mode = ModeNames[InitialMode].name;
  ks->info |= KanjiModeInfo;
  return 0;
}

static yomiContext
getYomiContext(d)
uiContext d;
{
  coreContext cc = (coreContext)d->modec;
  yomiContext yc;

  switch (cc->id) {
  case YOMI_CONTEXT:
    yc = (yomiContext)cc;
    break;
  case JISHU_CONTEXT:
    yc = ((jishuContext)cc)->ycx;
    break;
  case HENKAN_CONTEXT:
    yc = ((henkanContext)cc)->ycx;
    break;
  default:
    if (cc->minorMode == IROHA_MODE_AlphaMode) {
      yc = (yomiContext)(cc->next);
    }
    else {
      yc = (yomiContext)0;
    }
    break;
  }
  return yc;
}

static
KC_inhibitChangeMode(d, arg)
uiContext d;
int arg;
{
  yomiContext yc;

  if (yc = getYomiContext(d)) {
    if (arg) {
      yc->generalFlags |= IROHA_YOMI_CHGMODE_INHIBITTED;
    }
    else {
      yc->generalFlags &= ~IROHA_YOMI_CHGMODE_INHIBITTED;
    }
    return 0;
  }
  else {
    return -1;
  }  
}

static
KC_letterRestriction(d, arg)
uiContext d;
int arg;
{
  yomiContext yc;

  if (yc = getYomiContext(d)) {
    yc->allowedChars = arg;
    return 0;
  }
  else {
    return -1;
  }
}

static
countColumns(str)
unsigned char *str;
{
  int len = 0;
  unsigned char *p;

  if (str) {
    for (p = str ; *p ; p++) {
      if (*p & (unsigned char)0x80) {
	if (*p == (unsigned char)0x8e) {
	  len++; p++;
	}
	else if (*p == (unsigned char)0x8f) {
	  len += 2; p += 2;
	}
	else {
	  len += 2; p++;
	}
      }
      else {
	len++;
      }
    }
  }
  return len;
}

static
KC_queryMaxModeStr(d, arg)
uiContext d;
int arg;
{
  int i, maxcolumns = 0, ncols;
  extern struct ModeNameRecs ModeNames[];

  for (i = 0 ; i < IROHA_MODE_MAX_IMAGINARY_MODE ; i++) {
    ncols = countColumns(ModeNames[i].name);
    if (ncols > maxcolumns) {
      maxcolumns = ncols;
    }
  }
  return maxcolumns;
}

static int
KC_setListCallback(d, arg)
uiContext d;
jrListCallbackStruct *arg;
{
  extern int iListCB;

  if (iListCB) {
    d->client_data = (char *)0;
    d->list_func = (void (*)())0;
    return -1;
  }
  if (arg->callback_func) {
    d->client_data = arg->client_data;
    d->list_func = arg->callback_func;
  }
  else {
    d->client_data = (char *)0;
    d->list_func = (void (*)())0;
  }
  return 0;
}

static int
KC_setVerbose(d, arg)
uiContext d;
int arg;
{
  extern int ckverbose;

  ckverbose = arg;
  return 0;
}
