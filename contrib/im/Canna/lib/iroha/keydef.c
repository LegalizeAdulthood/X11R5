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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/keydef.c,v 3.2 91/09/02 14:05:24 kon Exp $";

#include "iroha.h"
#include <iroha/mfdef.h>
#include <iroha/keydef.h>

extern KanjiModeRec alpha_mode, empty_mode, yomi_mode;
extern KanjiModeRec jishu_mode;
extern KanjiModeRec tankouho_mode, ichiran_mode, onoff_mode;
extern KanjiModeRec khal_mode, khkt_mode, kzal_mode, kzhr_mode, kzkt_mode;
extern KanjiModeRec kigo_mode;
extern KanjiModeRec tourokureibun_mode;
extern KanjiModeRec bunsetsu_mode;

extern multiSequenceFunc();

static int undefineKeyfunc(), regist_key_hash(), copyMultiSequence();
static int freeMultiSequence();
static void clearAllFuncSequence(), clearAllKeySequence();

#define NONE 0
#define ACTHASHTABLESIZE 64
#define KEYHASHTABLESIZE 16

#define SINGLE 0
#define MULTI  1
#define OTHER  2

struct  seq_struct{
  unsigned char    *to_tbl;
  unsigned char    as_key;
  unsigned char    *kinou_seq;
  struct  seq_struct *next;
};

static struct seq_struct seq_s, *seq_hash[ACTHASHTABLESIZE];

struct map{
  KanjiMode tbl;
  unsigned char key;
  KanjiMode  mode;
  struct map *next;
};

static struct map *otherMap[KEYHASHTABLESIZE];

static KanjiMode ModeTbl[IROHA_MODE_MAX_REAL_MODE] = {
  &alpha_mode,        /* AlphaMode          ����ե��٥åȥ⡼��         */
  &empty_mode,	      /* EmptyMode          �ɤ����Ϥ��ʤ�����           */
  &kigo_mode,	      /* KigoMode           ���������ɽ�����Ƥ������   */
  &kzhr_mode,         /* KakuteiZHRMode     �����������ϥ⡼��           */
  &kzkt_mode,         /* KakuteiZKTMode     �����������ϥ⡼��           */
  &khkt_mode,         /* KakuteiHKTMode     Ⱦ���������ϥ⡼��           */
  &kzal_mode,         /* KakuteiZALMode     �����������ϥ⡼��           */
  &khal_mode,         /* KakuteiHALMode     Ⱦ���������ϥ⡼��           */
  &yomi_mode,         /* YomiMode           �ɤ����Ϥ��Ƥ������         */
  &jishu_mode,	      /* JishuMode          ʸ�����Ѵ����Ƥ������       */
  &tankouho_mode,     /* TankouhoMode       ñ��θ����ɽ�����Ƥ������ */
  &ichiran_mode,      /* IchiranMode        ���������ɽ�����Ƥ������   */
  &tourokureibun_mode, /* TourokuReibunMode ñ����Ͽ����ʸɽ������       */
  &onoff_mode,        /* OnOffMode          On/Off�ΰ�����ɽ������       */
  &bunsetsu_mode,     /* AdjustBunsetsuMode ʸ�῭�̥⡼��               */
};
unsigned char *actFromHash();
static void regist_act_hash();

static unsigned char *
duplicatekmap(kmap)
unsigned char *kmap;
{
  unsigned char *res;
  int i;

  res = (unsigned char *)calloc(256, sizeof(unsigned char));
  if (res) {
    for (i = 0 ; i < 256 ; i++) {
      res[i] = kmap[i];
    }
  }
  return res;
}

static unsigned char *defaultkeytables[IROHA_MODE_MAX_REAL_MODE];
static unsigned char defaultsharing[IROHA_MODE_MAX_REAL_MODE];
static unsigned char *defaultmap, *alphamap, *emptymap, *kakuteimap;

/* cfuncdef

  initKeyTables() -- �����ơ��֥����������ؿ���

  �ǥե���ȤΥ����ơ��֥��Ͽ���Ƥ������»��ѤΥơ��֥�� �ǥե���
  �ȥơ��֥뤫�饳�ԡ����������Ԥ���

*/

initKeyTables()
{
  int i;
  unsigned char *tbl;
  extern unsigned char default_kmap[], alpha_kmap[], empty_kmap[], kakutei_kmap[];

  defaultmap = duplicatekmap(default_kmap);
  alphamap   = duplicatekmap(alpha_kmap);
  emptymap   = duplicatekmap(empty_kmap);
  kakuteimap = duplicatekmap(kakutei_kmap);

  for (i = 0 ; i < IROHA_MODE_MAX_REAL_MODE ; i++) {
    if (ModeTbl[i]) {
      defaultsharing[i] = ModeTbl[i]->flags;
      tbl = defaultkeytables[i] = ModeTbl[i]->keytbl;
      if (tbl == default_kmap) {
	ModeTbl[i]->keytbl = defaultmap;
      }
      else if (tbl == alpha_kmap) {
	ModeTbl[i]->keytbl = alphamap;
      }
      else if (tbl == empty_kmap) {
	ModeTbl[i]->keytbl = emptymap;
      }
      else if (tbl == kakutei_kmap) {
	ModeTbl[i]->keytbl = kakuteimap;
      }
    }
  }
}

void
restoreDefaultKeymaps()
{
  int i;

  for (i = 0 ; i < IROHA_MODE_MAX_REAL_MODE ; i++) {
    if (ModeTbl[i]) {
      if ( !(ModeTbl[i]->flags & IROHA_KANJIMODE_TABLE_SHARED) ) {
	free(ModeTbl[i]->keytbl);
      }
      ModeTbl[i]->keytbl = defaultkeytables[i];
      ModeTbl[i]->flags = defaultsharing[i];
    }
  }
  free(defaultmap);
  free(alphamap);
  free(emptymap);
  free(kakuteimap);
  clearAllFuncSequence();
  clearAllKeySequence();
}


/* 
 * ����⡼�ɤΥ������Ф��ƴؿ��������Ƥ����
 *
 */

/* 

  �����ʤλ��ϣ�ʸ���ܤ����줿���Υ⡼�ɤˤ����ꤹ�롣

 */

changeKeyfunc(modenum, key, fnum, actbuff, keybuff)
int modenum;
int key;
unsigned char fnum;
unsigned char *actbuff, *keybuff;
{
  int i;
  unsigned char *p, *q;
  KanjiMode mode = ModeTbl[modenum];

  if (mode && mode->ftbl) {
    if (mode->ftbl && mode->ftbl[fnum]) { 
      /* ���ε�ǽ�����Υ⡼�ɤˤ�����ͭ���ʵ�ǽ�Ǥ���� */
      if (mode->keytbl) { /* �����ơ��֥뤬¸�ߤ���� */
	/* ��������Ф�¸�ߤ���ΤǤϡ� */
	if (mode->flags & IROHA_KANJIMODE_TABLE_SHARED) {
	  /* �����ޥåפ�¾�Υ⡼�ɤȶ�ͭ����Ƥ���ʤ� */
	  p = (unsigned char *)calloc(256, sizeof(unsigned char));
	  if(p)
	    bcopy(mode->keytbl, p, 256 * sizeof(unsigned char));
	  for (i = 0; i < 256; i++) {
	    if (mode->keytbl[i] == IROHA_FN_FuncSequence) {
	      q = actFromHash(mode->keytbl,i);
	      regist_act_hash(p, i, q);
	    }
	    if (mode->keytbl[i] == IROHA_FN_UseOtherKeymap) {
	      debug_message("changeKeyfunc:�ơ��֥��ư:���ΤȤ��Υ�����%d\n",i,0,0);
	      copyMultiSequence(i, (KanjiMode)mode->keytbl, (KanjiMode)p);
	    }
	  }
	  mode->keytbl = p;
	  mode->flags &= ~IROHA_KANJIMODE_TABLE_SHARED;
	}
	if (key >= 0 && key < 255) {
	  if (mode->keytbl[key] == IROHA_FN_UseOtherKeymap &&
	      fnum != IROHA_FN_UseOtherKeymap)
	    freeMultiSequence(key,(KanjiMode)mode->keytbl);
	  mode->keytbl[key] = fnum;
	  if (fnum == IROHA_FN_FuncSequence) {
	    regist_act_hash(mode->keytbl,key,actbuff);
	  }
	  if (fnum == IROHA_FN_UseOtherKeymap) {
	    regist_key_hash(mode->keytbl,keybuff,actbuff);
	  }
	}
	else if (key == IROHA_KEY_Undefine) {
	  undefineKeyfunc(mode->keytbl, fnum);
	}
      }
    }
  }
}

/*
 * ���ƤΥ⡼�ɤΡ����륭�����Ф��ƴؿ��������Ƥ����
 *
 */

changeKeyfuncOfAll(key, fnum, actbuff, keybuff)
int key, fnum;
unsigned char *actbuff, *keybuff;
{
  KanjiMode mode;
  int i;

  if (key >= 0 && key < 255) {
    if (defaultmap[key] == IROHA_FN_UseOtherKeymap &&
	      fnum != IROHA_FN_UseOtherKeymap)
      freeMultiSequence(key,(KanjiMode)defaultmap);
    if (alphamap[key] == IROHA_FN_UseOtherKeymap &&
	      fnum != IROHA_FN_UseOtherKeymap)
      freeMultiSequence(key,(KanjiMode)alphamap);
    if (emptymap[key] == IROHA_FN_UseOtherKeymap &&
	      fnum != IROHA_FN_UseOtherKeymap)
      freeMultiSequence(key,(KanjiMode)emptymap);
    if (kakuteimap[key] == IROHA_FN_UseOtherKeymap  &&
	      fnum != IROHA_FN_UseOtherKeymap)
      freeMultiSequence(key,(KanjiMode)kakuteimap);
    defaultmap[key] = fnum;
    alphamap[key]   = fnum;
    emptymap[key]   = fnum;
    kakuteimap[key] = fnum;
    if (fnum == IROHA_FN_FuncSequence) {
      regist_act_hash(defaultmap,key,actbuff);
      regist_act_hash(alphamap,key,actbuff);
      regist_act_hash(emptymap,key,actbuff);
      regist_act_hash(kakuteimap,key,actbuff);
    }
    if (fnum == IROHA_FN_UseOtherKeymap) {
      regist_key_hash(defaultmap,keybuff,actbuff);
      regist_key_hash(alphamap,keybuff,actbuff);
      regist_key_hash(emptymap,keybuff,actbuff);
      regist_key_hash(kakuteimap,keybuff,actbuff);
    }
    for (i = 0 ; i < IROHA_MODE_MAX_REAL_MODE ; i++) {
      mode = ModeTbl[i];
      if (mode && /* ���Υ⡼�ɤ�¸�ߤ���ʤ� */
	  mode->ftbl && mode->ftbl[fnum]) { /* �ؿ������Υ⡼�ɤ�ͭ���ʤ� */
	if ( !(mode->flags & IROHA_KANJIMODE_TABLE_SHARED) ) {
	  /* �ơ��֥뤬��ͭ����Ƥ��ʤ��ʤ� */
	  if (mode->keytbl) { /* �����ơ��֥뤬¸�ߤ���� */
	  if (mode->keytbl[key] == IROHA_FN_UseOtherKeymap &&
	      fnum != IROHA_FN_UseOtherKeymap)
	    freeMultiSequence(key,(KanjiMode)mode->keytbl);
	    mode->keytbl[key] = fnum;
	    if (fnum == IROHA_FN_FuncSequence) {
	      regist_act_hash(mode->keytbl,key,actbuff);
	    }
	    if (fnum == IROHA_FN_UseOtherKeymap) {
	      regist_key_hash(mode->keytbl,keybuff,actbuff);
	    }
	  }
	}
      }
    }
  }
  else if (key == IROHA_KEY_Undefine) {
    undefineKeyfunc(defaultmap, fnum);
    undefineKeyfunc(alphamap, fnum);
    undefineKeyfunc(emptymap, fnum);
    undefineKeyfunc(kakuteimap, fnum);
    for (i = 0 ; i < IROHA_MODE_MAX_REAL_MODE ; i++) {
      mode = ModeTbl[i];
      if (mode && /* ���Υ⡼�ɤ�¸�ߤ���ʤ� */
	  mode->ftbl && mode->ftbl[fnum]) { /* �ؿ������Υ⡼�ɤ�ͭ���ʤ� */
	if ( !(mode->flags & IROHA_KANJIMODE_TABLE_SHARED) ) {
	  /* �ơ��֥뤬��ͭ����Ƥ��ʤ��ʤ� */
	  if (mode->keytbl) { /* �����ơ��֥뤬¸�ߤ���� */
	    undefineKeyfunc(mode->keytbl, fnum);
	  }
	}
      }
    }
  }
}

static
undefineKeyfunc(keytbl, fnum)
unsigned char *keytbl;
unsigned char fnum;
{
  int i;

  for (i = 0 ; i < ' ' ; i++) {
    if (keytbl[i] == fnum) {
      keytbl[i] = IROHA_FN_Undefined;
    }
  }
  for (i = ' ' ; i < 0x7f ; i++) {
    if (keytbl[i] == fnum) {
      keytbl[i] = IROHA_FN_FunctionalInsert;
    }
  }
  for (i = 0x7f ; i < 0xa0 ; i++) {
    if (keytbl[i] == fnum) {
      keytbl[i] = IROHA_FN_Undefined;
    }
  }
  for (i = 0xa0 ; i < 0xe0 ; i++) {
    if (keytbl[i] == fnum) {
      keytbl[i] = IROHA_FN_FunctionalInsert;
    }
  }
  for (i = 0xe0 ; i < 0x100 ; i++) {
    if (keytbl[i] == fnum) {
      keytbl[i] = IROHA_FN_Undefined;
    }
  }
}

static unsigned int
createHashKey(data1, data2, which_seq)
unsigned char *data1;
unsigned char data2;
int which_seq;
{
  unsigned int hashKey;

  hashKey = ((unsigned int)data1 + (unsigned int)data2) % which_seq;
  return hashKey;
}

/* ��ǽ�������󥹤���Ф� */
unsigned char *
actFromHash(tbl_ptr, key)
unsigned char *tbl_ptr;
unsigned char key;
{
  unsigned int hashKey;
  struct seq_struct *p;

  hashKey = createHashKey(tbl_ptr, key, ACTHASHTABLESIZE);
  for (p = seq_hash[hashKey] ; p ; p = p->next) {
    if (p->to_tbl == tbl_ptr && p->as_key == key) {
      return p->kinou_seq;
    }
  }
  debug_message("actFromHash:���������󥹤�ߤĤ����ޤ���Ǥ�����\n",0,0,0);
  return NULL; /* �������륭���������󥹤�¸�ߤ��ʤ� */
}

/* �ϥå���ơ��֥����Ͽ */
static void
regist_act_hash(tbl_ptr, key, buff)
unsigned char *tbl_ptr;
unsigned char key;
unsigned char *buff;
{
  unsigned int hashKey;
  struct seq_struct *p, **pp;

  hashKey = createHashKey(tbl_ptr, key, ACTHASHTABLESIZE);
  for (pp = &seq_hash[hashKey] ; p = *pp ; pp = &(p->next)) {
    if (p->to_tbl == tbl_ptr && p->as_key == key) {
      if (p->kinou_seq)
	free(p->kinou_seq);
      p->kinou_seq = (unsigned char *)malloc(strlen(buff)+1);
      if (p->kinou_seq)
	strcpy(p->kinou_seq,buff);
      return;
    }
  }
  p = *pp = (struct seq_struct *)malloc(sizeof(struct seq_struct));
  if(p) {
    p->to_tbl = tbl_ptr;
    p->as_key = key;
    p->kinou_seq = (unsigned char *)malloc(strlen(buff)+1);
    if(p->kinou_seq)
      strcpy(p->kinou_seq,buff);
    p->next = NULL;
  }
}  

/* �ϥå���ơ��֥뤫���� */
static
remove_hash(tbl_ptr, key, which_seq)
unsigned char *tbl_ptr;
unsigned char key;
int which_seq;
{
  unsigned int hashKey;
  struct seq_struct *p, **pp;

  hashKey = createHashKey(tbl_ptr, key, which_seq);
  for (pp = &seq_hash[hashKey] ; p = *pp ; pp = &(p->next)) {
    if (p->to_tbl == tbl_ptr && p->as_key == key) {
      *pp = p->next;
      free(p);
    }
  }
}

static void
freeChain(p)
struct seq_struct *p;
{
  struct seq_struct *nextp;

  while (p) {
    free(p->kinou_seq);
    nextp = p->next;
    free(p);
    p = nextp;
  }
}

static void
clearAllFuncSequence()
{
  int i;
  struct seq_struct *p;

  for (i = 0 ; i < ACTHASHTABLESIZE ; i++) {
    freeChain(seq_hash[i]);
    seq_hash[i] = 0;
  }
}

static
freeKeySeqMode(m)
KanjiMode m;
{
  if (m) {
    if (m->keytbl) {
      free(m->keytbl);
    }
    free(m);
  }
}

static
freeMap(m)
struct map *m;
{
  struct map *n;

  while (m) {
    freeKeySeqMode(m->mode);
    n = m->next;
    free(m);
    m = n;
  }
}

static void
clearAllKeySequence()
{
  int i;
  struct map *p;

  for (i = 0 ; i < KEYHASHTABLESIZE ; i++) {
    freeMap(otherMap[i]);
    otherMap[i] = 0;
  }
}

static
specialen(block)
unsigned char *block;
{
  int i;
  for (i = 0;block[i] != 255; i++);
  debug_message("specialen:Ĺ����%d�ɤ���\n",i,0,0);
  return i;
}

static
to_write_act(depth,keysize,actsize,singleAct)
int depth;
int keysize;
int actsize;
unsigned char singleAct;
{
  if (depth == (keysize -2)) {
    if (actsize > 1){
      debug_message("to_write_act:IROHA_FN_FuncSequence�Ǥ���\n",0,0,0);
      return IROHA_FN_FuncSequence;
}
    if (actsize == 1) {
      debug_message("to_write_act:singleAct%d�Ǥ���\n",singleAct,0,0);
      return (int)singleAct;
}
  }
  else if (depth < (keysize -2)){
    debug_message("to_write_act:IROHA_FN_UseOtherKeymap�Ǥ���\n",0,0,0);
    return IROHA_FN_UseOtherKeymap;
}
}

static struct map *
regist_map(tbl, keybuff, actbuff, depth)
KanjiMode tbl;
unsigned char *keybuff;
unsigned char *actbuff;
int      depth;
{
  unsigned int hashKey;
  int sequencelen,keybuffsize,actbuffsize;
  struct map *p,**pp;
  unsigned char *q,*r, prevfunc;

  actbuffsize = strlen(actbuff);
  keybuffsize = specialen(keybuff);
  hashKey = 
    createHashKey((unsigned char *)tbl, keybuff[depth], KEYHASHTABLESIZE);
  debug_message("regist_map:hashKey = %d �Ǥ���\n",hashKey,0,0);
  for (pp = &otherMap[hashKey]; p = *pp; pp = &(p->next)) {
    if (p->key == keybuff[depth] && p->tbl == tbl) { 
      for (q = p->mode->keytbl; *q != 255; q += 2) {
	if (*q == keybuff[depth+1]) {  /* ����Ʊ��������¸�ߤ����� */
	  ++q;
	  prevfunc = *q; /* ���Υ����κ��ޤǤε�ǽ���äƤ��� */
	  *q = to_write_act(depth,keybuffsize,actbuffsize,actbuff[0]);
	  if(prevfunc == IROHA_FN_UseOtherKeymap &&
	     *q != IROHA_FN_UseOtherKeymap) {
            freeMultiSequence(keybuff[depth + 1], p->mode);
          }
	  if (*q == IROHA_FN_FuncSequence) {
	    regist_act_hash(p->mode, keybuff[depth+1], actbuff);
	  }
	  debug_message("regist_map:����Ʊ��������¸��:q=%d\n",*q,0,0);
	  return p;
	}
      }
      /* �����ޤǤΡ�����������Ϥ��ä������Υ���:keybuff[depth +1]�Ͻ��� */
      sequencelen = specialen(p->mode->keytbl);
      if (p->mode->keytbl) {
	p->mode->keytbl =
	  (unsigned char *)realloc(p->mode->keytbl,sequencelen +3);
	if (p->mode->keytbl) {
	  p->mode->keytbl[sequencelen] = keybuff[depth +1];
	  p->mode->keytbl[++sequencelen] =
	  to_write_act(depth,keybuffsize,actbuffsize,actbuff[0]);
	  p->mode->keytbl[++sequencelen] = -1;
	}
      }
      if (*q == IROHA_FN_FuncSequence) {
	regist_act_hash(p->mode, keybuff[depth+1], actbuff);
      }
      debug_message("regist_map:�����ޤǤΡ�����������Ϥ��ä������Υ���%u�Ͻ���\n",
		    p->mode->keytbl[sequencelen-3],0,0);
      debug_message("regist_map:sequencelen��%d�Ǥ���\n",sequencelen,0,0);
      return p;
    }
  }
  /* ������������Ƥʤ��ΤϤ��������˺��� */
  p = *pp = (struct map *)malloc(sizeof(struct map));
  if (p) {
    p->tbl = tbl;
    p->key = keybuff[depth];
    p->mode = (KanjiMode)malloc(sizeof(KanjiModeRec));
    if (p->mode) {
      p->mode->func = multiSequenceFunc;
      p->mode->flags = 0;
      p->mode->keytbl = (unsigned char *)malloc(3);
      if (p->mode->keytbl) {
	p->mode->keytbl[0] = keybuff[depth +1];
	p->mode->keytbl[1] = to_write_act(depth,keybuffsize,actbuffsize,actbuff[0]);
	debug_message("regist_map:p->mode->keytbl[1]��%d�Ǥ���\n",p->mode->keytbl[1],0,0);
	p->mode->keytbl[2] = -1;
      }
    }
    p->next = NULL;
    if (p->mode->keytbl[1] == IROHA_FN_FuncSequence) {
      regist_act_hash(p->mode, keybuff[depth+1], actbuff);
    }
    return p;
  }
}

struct map *
mapFromHash(tbl, key, ppp)
KanjiMode tbl;
unsigned char key;
struct map ***ppp;
{
  unsigned int hashKey;
  struct map *p, **pp;

  hashKey = createHashKey((unsigned char *)tbl, key, KEYHASHTABLESIZE);
  debug_message("mapFromHash:hashKey��%d\n",hashKey,0,0);
  for(pp = otherMap + hashKey ; p = *pp ; pp = &(p->next)) {
    if (p->tbl == tbl && p->key == key) {
      debug_message("mapFromHash:map���ߤĤ���ޤ�����\n",0,0,0);
      if (ppp) {
	*ppp = pp;
      }
      return p;
    }
  }
  debug_message("mapFromHash:map���ߤĤ���ޤ���\n",0,0,0);
  return NULL;
}

static
regist_key_hash(tbl_ptr,keybuff, actbuff)
unsigned char *tbl_ptr;
unsigned char *keybuff;
unsigned char *actbuff;
{
  struct map *map_ptr;
  int keybuffsize, i;

  keybuffsize = specialen(keybuff);
  map_ptr = regist_map((KanjiMode)tbl_ptr, keybuff, actbuff, 0);  
  for (i = 1; i <= (keybuffsize -2); i++) {
    map_ptr = regist_map(map_ptr->mode, keybuff, actbuff, i);
  }
  debug_message("regist_key_hash:keybuffsize��%d��actbuffsize��%d��i��%d�Ǥ���\n",
		keybuffsize,strlen(actbuff),i);
}

static
copyMultiSequence(key, old_tbl, new_tbl)
unsigned char key;
KanjiMode old_tbl, new_tbl;
{
  unsigned char hashKey;
  unsigned char *old_sequence, *new_sequence;
  int i, sequencelen;
  struct map *p, **pp;
  struct map *old_map, *next_old_map;

  old_map = mapFromHash(old_tbl, key, 0);
  old_sequence = old_map->mode->keytbl;
  sequencelen = specialen(old_sequence);
  debug_message("copyMultiSequence:sequencelen��%d\n",sequencelen,0,0);

  hashKey = createHashKey((unsigned char *)new_tbl, key, KEYHASHTABLESIZE);
  for (pp = &otherMap[hashKey]; p = *pp; pp = &(p->next)) {
    if (p->key == key && p->tbl == new_tbl) { 
      return;
    }
  }
  p = *pp = (struct map *)malloc(sizeof(struct map));
  if (p) {
    p->tbl = new_tbl;
    p->key = key;
    p->mode = (KanjiMode)malloc(sizeof(KanjiModeRec));
    if (p->mode) {
      p->mode->func = multiSequenceFunc;
      p->mode->flags = 0;
      p->mode->keytbl = (unsigned char *)malloc(sequencelen+1);
    }
    p->next = NULL;
  }
  for (i = 0, new_sequence = p->mode->keytbl; i <= sequencelen; i++) {
    debug_message("copyMultiSequence:for��i��%d\n",i,0,0);
    debug_message("copyMultiSequence:��������sequencelen��%d\n",sequencelen,0,0);
    new_sequence[i] = old_sequence[i];
    if (i%2 == 1) {
      if (old_sequence[i] == IROHA_FN_UseOtherKeymap) {
	debug_message("copyMultiSequence:���줫�飱�Ĥդ����ʤ�ޤ���\n",0,0,0);
	copyMultiSequence(old_sequence[i-1], old_map->mode, p->mode);
	debug_message("copyMultiSequence:���ޤ�����\n",0,0,0);
      }
      if (old_sequence[i] == IROHA_FN_FuncSequence) {
	regist_act_hash(p->mode, old_sequence[i-1], actFromHash(old_map->mode,old_sequence[i-1]));
      }
    }
  }
}

static
freeMultiSequence(key, tbl)
unsigned char key;
KanjiMode tbl;
{
  unsigned char *sequence;
  int i, sequencelen;
  struct map *map, **ptr;

  map = mapFromHash(tbl, key, &ptr);
  if (!map)
    return;
  *ptr = map->next;
  sequence = map->mode->keytbl;
  sequencelen = specialen(sequence);

  for (i = 0; i <= sequencelen; i++) {
    if (i%2 == 1) {
      if (sequence[i] == IROHA_FN_UseOtherKeymap)
	freeMultiSequence(sequence[i-1], map->mode);
      if (sequence[i] == IROHA_FN_FuncSequence)
	remove_hash(map->mode, sequence[i-1], ACTHASHTABLESIZE);
    }
  }
  debug_message("��key[%d]��map�ʲ���ե꡼���Ƥ��뤾\n",key,0,0);
  if (map->mode && sequence)
    free(sequence);
  if (map->mode)
    free(map->mode);
  free(map);
}

askQuitKey(key)
unsigned char key;
{
  if (defaultmap[key] == IROHA_FN_Quit) {
    return 1; /* ������ä�key��quit���ä��� */
  }
  return 0; /* ������ä�key��quit�Ǥʤ��ä��� */
}
