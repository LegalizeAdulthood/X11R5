/*
 * $Id: dictoasc.c,v 1.2 1991/09/16 21:31:18 ohm Exp $
 */
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/*	Version 4.0
 */
#include <stdio.h>
#include "commonhd.h"
#include "jslib.h"
#include "jh.h"
#include "jdata.h"
#include "de_header.h"

output_entry(jtl, hjtl, kanji1, kanji2, serial, yomi, ofpter)
struct JT *jtl;
struct HJT *hjtl;
int kanji1, kanji2;
int serial;
w_char *yomi;
FILE *ofpter;
{
    w_char comm[LENGTHKANJI];
    w_char kanji[LENGTHKANJI];
    int ima, hindo;
    UCHAR *kptr;

    ima = (hjtl ? hjtl->hindo[serial] : jtl->hindo[serial]) & 0x80;
    hindo = motoni2((hjtl ? hjtl->hindo[serial]: 
		     jtl->hindo[serial]) & 0x7f);

    kptr = jtl->kanji + kanji1;
    get_kanji_str(kptr, kanji, NULL, comm);
    
    Print_entry(yomi, kanji, comm, hindo, ima,
		jtl->hinsi[serial], serial, ofpter);
}

dictoasc(dic_no, ofpter)
int dic_no;
FILE *ofpter;
{
    if(((struct JT *)(files[dic_table[dic_no].body].area))->syurui == WNN_UD_DICT){
	return(udtoasc((struct JT *)(files[dic_table[dic_no].body].area),
		       (dic_table[dic_no].hindo == -1)? NULL:
		       (struct HJT *)(files[dic_table[dic_no].hindo].area),
		       ofpter));
    }else{
	return(sdtoasc((struct JT *)(files[dic_table[dic_no].body].area),
		       (dic_table[dic_no].hindo == -1)? NULL:
		       (struct HJT *)(files[dic_table[dic_no].hindo].area),
		       ofpter));
    }
}


udtoasc(jtl, hjtl, ofpter)
struct JT *jtl;
struct HJT *hjtl;
FILE *ofpter;
{
  struct uind2 *hop;
  int i, k;
  int len;
  int serial;
  UCHAR *hostart = jtl->hontai;
  struct uind1 *tary = jtl->table;
  static  w_char yomi[LENGTHYOMI]; 	/* 2 byte yomi */

  for(i = 0 ; i < jtl->maxtable; i++){
    for(hop = (struct uind2 *)(tary[i].pter + (char *)hostart);
	hop != (struct uind2 *)hostart;
	hop = (struct uind2 *)(hop->next  + (char *)hostart)){
/* Though each hop has the same yomi, but calculate yomi each time. */
      yomi[0] = tary[i].yomi1 >> 16;
      yomi[1] = tary[i].yomi1 & 0xffff;
      if(yomi[1]){
	yomi[2] = tary[i].yomi2 >> 16;
	if(yomi[2]){
	  yomi[3] = tary[i].yomi2 & 0xffff;
	}
      }
      len = hop->yomi[0];
      Strncpy(yomi + 4, (hop->yomi) + 1, len - 4);
      yomi[len] = 0;
      serial = hop->serial;
      for(k = 0 ; k < hop->kosuu; k++, serial++){
	  output_entry(jtl, hjtl, hop->kanjipter, k, serial, yomi, ofpter);
      }
    }
  }
  return(0);
}

sdtoasc(jtl, hjtl, ofpter)
struct JT *jtl;
struct HJT *hjtl;
FILE *ofpter;
{
    return(sdtoasc1(jtl, hjtl, ofpter, jtl->hontai, 0));
}

sdtoasc1(jtl, hjtl, ofpter, hopter, level)
UCHAR *hopter;
struct JT *jtl;
struct HJT *hjtl;
FILE *ofpter;
int level;
{
    int tsize;
    w_char *charst;
    w_char *sumst;
    int *ptrst;
    int j,k;
    int serial;
    int kanji1;
    static  w_char yomi[LENGTHYOMI]; 	/* 2 byte yomi */
    
    yomi[level + 1] = 0;
    switch(*(w_char *)hopter){
    case ST_NORMAL:
	tsize = *(w_char *)(hopter + 2);
	serial = *(int *)(hopter + 4);
	kanji1 = *(int *)(hopter + 8);
	charst = (w_char *)(hopter + 12);
	sumst = ((w_char *)charst + tsize + 2); /* + 2 keeps two zero words */
	ptrst = (int *)((w_char *)sumst + tsize);
	for(k = 0 ; k < tsize ; k++){
	    if(sumst[k] == sumst[k - 1]){
		yomi[level] = charst[k];
		for(j = sumst[k - 1] + 1; j <= sumst[k];j++){
		    output_entry(jtl, hjtl, kanji1, j, serial, yomi, ofpter);
		}
	    }
	}
	for(k = 0 ; k < tsize ; k++){
	    if(ptrst[k] != ENDPTR){
		yomi[level] = charst[k];
		sdtoasc1(jtl, hjtl, ofpter, jtl->hontai + ptrst[k],level + 1);
	    }
	}
	break;
    case ST_NOENT:
	tsize = *(w_char *)(hopter + 2);
	charst = (w_char *)(hopter + 4);
	ptrst = (int *)AL_INT((w_char *)charst + tsize);
	for(k = 0 ; k < tsize ; k++){
	    if(ptrst[k] != ENDPTR){
		yomi[level] = charst[k];
		sdtoasc1(jtl, hjtl, ofpter, jtl->hontai + ptrst[k],level + 1);
	    }
	}
	break;
    case ST_NOPTER:
	tsize = *(w_char *)(hopter + 2);
	serial = *(int *)(hopter + 4);
	kanji1 = *(int *)(hopter + 8);
	charst = (w_char *)(hopter + 12);
	sumst = ((w_char *)charst + tsize + 2); /* + 2 keeps two zero words */
	for(k = 0 ; k < tsize ; k++){
	    if(sumst[k] == sumst[k - 1]){
		yomi[level] = charst[k];
		for(j = sumst[k - 1] + 1; j <= sumst[k];j++){
		    output_entry(jtl, hjtl, kanji1, j, serial, yomi, ofpter);
		}
	    }
	}
	break;
    case ST_SMALL:
	yomi[level] = *(w_char *)(hopter + 2);
	sdtoasc1(jtl, hjtl, ofpter, hopter + 4,level + 1);
	break;
    }
    return(0);
}
