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

static char m_s_map_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/multi.c,v 3.1 91/06/28 11:49:55 kon Exp $";

#include "iroha.h"
#include <iroha/mfdef.h>

#define NONE IROHA_FN_Undefined

extern unsigned int askQuitKey();
extern int simpleUndefBehavior();
extern int checkGLineLen();
extern int NothingChangedWithBeep();

unsigned char *keyHistory;

struct map {
  KanjiMode tbl;
  unsigned char key;
  KanjiMode mode;
  struct map *next;
};

extern struct map *mapFromHash();

unsigned char *
showChar(c)
int c;
{
  static unsigned char Gkey[9];
  static char *keyCharMap[] = {               
    "space",    "DEL",      "Nfer",     "Xfer",     "Up",
    "Left",     "Right",    "Down",     "Insert",   "Rollup",
    "Rolldown", "Home",     "HELP",     "KeyPad",   "S-nfer",
    "S-xfer",   "S-up",     "S-left",   "S-right",  "S-down",
    "C-nfer",   "C-xfer",   "C-up",     "C-left",   "C-right",
    "C-down",   "F1",       "F2",       "F3",       "F4",
    "F5",       "F6",       "F7",       "F8",       "F9",
    "F10",      "PF1",      "PF2",      "PF3",      "PF4",
    "PF5",      "PF6",      "PF7",      "PF8",      "PF9",
    "PF10",
  };

  if (c < 0x20) {
    strcpy(Gkey, "C-");
    if (c == 0x00 || (c > 0x1a && c < 0x20 ))
      Gkey[2] = c + 0x40;
    else
      Gkey[2] = c + 0x60;
    Gkey[3] = '\0';
  }
  else if (c > ' ' && c <= '~' ) {
    Gkey[0] = c;
    Gkey[1] = '\0';
  }
  else if (c > 0xa0 && c < 0xdf) {
    Gkey[0] = 0x8e;
    Gkey[1] = c;
    Gkey[2] = '\0';
  }
  else if (c == 0x20)
    strcpy(Gkey, keyCharMap[0]);
  else if (c > 0x7e && c < 0x8c)
    strcpy(Gkey, keyCharMap[c -0x7f +1]);
  else if (c > 0x8f && c < 0x9c)
    strcpy(Gkey, keyCharMap[c -0x90 +14]);
  else if (c > 0xdf && c < 0xea)
    strcpy(Gkey, keyCharMap[c -0xe0 +26]);
  else if (c > 0xef && c < 0xfa)
    strcpy(Gkey, keyCharMap[c -0xf0 +36]);
  else
    return 0;
  return Gkey;
}

multiSequenceFunc(d, mode, key, fnum)
uiContext d;
KanjiMode mode;
unsigned char key;
int fnum;
{
  int i, (*func)();
  unsigned char *p;
  struct map *m;

  if (askQuitKey(key)) {
    free(keyHistory);
    GlineClear(d);
    d->current_mode = (KanjiMode)(mode->ftbl);
    if (d->current_mode->flags & IROHA_KANJIMODE_EMPTY_MODE) {
      d->kanji_status_return->info |= KanjiEmptyInfo;
    }
    return NothingForGLine(d);
  }
  p = mode->keytbl;
  for (i= 0, p; *p != 255; p += 2,i+=2) {
    debug_message("multiSequenceFunc:階層[%d]\n",i,0,0);
    if (*p == key) { /* このキーは登録されていた。 */
      keyHistory =
	(unsigned char *)realloc(keyHistory, strlen(keyHistory) + strlen(showChar(key)) +2);
      if (keyHistory) {
	strcat(keyHistory," ");
	strcat(keyHistory,showChar(key));
      }
      makeGLineMessage(d, keyHistory, strlen(keyHistory));
      if (*++p == IROHA_FN_UseOtherKeymap) { /* まだキーシケンスの続きが存在 */
	m = mapFromHash(mode, key, 0);
	m->mode->ftbl = mode->ftbl;
	d->current_mode = m->mode;
	return NothingForGLine(d);
      }
      free(keyHistory);
      GlineClear(d);
      d->current_mode = (KanjiMode)(mode->ftbl); /* 機能を実行 */
      if (*p == IROHA_FN_FuncSequence) {
	return _DoFuncSequence(d, (unsigned char *)mode, key);
      }
      func = d->current_mode->ftbl[*p];
      if (func || (func = d->current_mode->ftbl[0])) {
	return func(d);
      }
      else {
	return simpleUndefBehavior(d); 
      }
    }
  }
  return NothingForGLineWithBeep(d);  /* 登録していないキーを押した */
}
