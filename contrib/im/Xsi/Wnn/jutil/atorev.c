/*
 * $Id: atorev.c,v 1.2 1991/09/16 21:32:18 ohm Exp $
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
/*
UJIS 形式を、逆変換可能形式に変換するプログラム。
*/

#include <stdio.h>
#include <ctype.h>
#include "commonhd.h"
#include "jslib.h"
#include "jh.h"
#include "jdata.h"

extern int kanjicount;
extern struct JT jt;

static void r_alloc(), make_ri2(), make_ri1(), set_rpter1();
extern int sort_func_je();

struct je **ptmp;

void
create_rev_dict()
{
    r_alloc();
    sort_if_not_sorted();
    uniq_je(sort_func_je);
    output_dic_data();
    make_ri2();
    make_ri1(D_YOMI);
    set_rpter1(D_YOMI);
    sort_kanji();
    make_ri1(D_KANJI);
    set_rpter1(D_KANJI);
}    

static void
r_alloc()
{
    if((jt.ri2 = (struct rind2 *)malloc((jt.maxserial) * sizeof(struct rind2))) == NULL ||
       (jt.ri1[D_YOMI] = (struct rind1 *)malloc(jt.maxserial * sizeof(struct rind1))) == NULL ||
       (jt.ri1[D_KANJI] = (struct rind1 *)malloc(jt.maxserial * sizeof(struct rind1))) == NULL ||
       (ptmp = (struct je **)malloc(jt.maxserial * sizeof(struct je *))) == NULL){
	fprintf(stderr, "Malloc Failed\n");
	  exit(1);
      }
}

static void
make_ri2()
{
    register int s;

    for(s = 0 ;s < jt.maxserial; s++){
	jeary[s]->serial = s;
	jt.ri2[s].kanjipter = kanjicount;
	upd_kanjicount(s);
    }
}

static void
make_ri1(which)
int which;
{
    register w_char  *yomi;
    w_char dummy = 0; 	/* 2 byte yomi */
    w_char *pyomi;		/* maeno tangono yomi */
    register int s, t;

    yomi = &dummy;

    for(t = 0,s = 0 ;s < jt.maxserial; s++){
	pyomi = yomi;
	yomi = (which == D_YOMI)? jeary[s]->yomi:jeary[s]->kan;
	
	if(Strcmp(yomi, pyomi)){
	    (jt.ri1[which])[t].pter = jeary[s]->serial;
	    if(s) (jt.ri2[jeary[s - 1]->serial].next)[which] = RD_ENDPTR;
	    ptmp[t] = jeary[s];
	    t++;
	}else{
	    if(s)(jt.ri2[jeary[s - 1]->serial].next)[which] = jeary[s]->serial;
	}
    }
    if(s > 0){
	(jt.ri2[jeary[s - 1]->serial].next)[which] = RD_ENDPTR;
    }
    jt.maxri1[which] = t;
}

static void
set_rpter1(which)
int which;
{
    register int k;
    register int len;
    w_char *oyomi, *nyomi;
    /* May be a little slow, but simple! */
    int lasts[LENGTHYOMI];			/* pter_to */

    for(k = 0 ; k < LENGTHYOMI; k++){
	lasts[k] = -1;
    }

    for(k = 0 ; k < jt.maxri1[which] ; k++){
	nyomi = (which == D_YOMI)? ptmp[k]->yomi: ptmp[k]->kan;
	len = Strlen(nyomi);
	lasts[len] = k;
	for(len--;len ; len--){
	    if(lasts[len] >= 0){
		oyomi = (which == D_YOMI)? 
		    ptmp[lasts[len]]->yomi: ptmp[lasts[len]]->kan;
		if(Substr(oyomi,nyomi)){
		    (jt.ri1[which])[k].pter1 = lasts[len];
		    break;
		}
	    }
	}
	if(len == 0) (jt.ri1[which])[k].pter1 = RD_ENDPTR;
    }
}

void
output_ri(ofpter)
FILE *ofpter;
{
    fwrite(jt.ri1[D_YOMI], sizeof(struct rind1), jt.maxri1[D_YOMI], ofpter);
    fwrite(jt.ri1[D_KANJI], sizeof(struct rind1), jt.maxri1[D_KANJI], ofpter);
    fwrite(jt.ri2, sizeof(struct rind2), jt.maxri2, ofpter);
}
