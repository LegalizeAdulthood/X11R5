/*
 * $Id: jishosub.c,v 1.3 1991/09/16 21:31:47 ohm Exp $
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
#include "de_header.h"
#include "jdata.h"

int
inspect(dic_no, serial, yomi, jd)
int dic_no;
int serial;
w_char *yomi;
struct jdata *jd;
{
    struct JT *jtl;

    if(dic_no < 0)return(-1);
    jtl = (struct JT *)(files[dic_table[dic_no].body].area);
    if(jtl->syurui == WNN_UD_DICT){
	if(inspect_ud(dic_no , serial, yomi, jd) == -1) return(-1);
#if	defined(CONVERT_by_STROKE) || defined(CONVERT_with_SiSheng)
    }else if((jtl->syurui & 0xff) == WNN_REV_DICT){
#else
    }else if(jtl->syurui == WNN_REV_DICT){
#endif	/* CONVERT_by_STROKE || CONVERT_with_SiSheng */
	if(inspect_rd(dic_no , serial, yomi, jd) == -1) return(-1);
    }else{
	if(inspect_sd(dic_no , serial, yomi, jd) == -1) return(-1);
    }
    return(0);
}



int get_yomi_from_serial(dic_no, serial, yomi)
int dic_no;
int serial;
w_char *yomi;
{
    int k;
    struct jdata jdata;
    struct JT *jtl;

    jtl = (struct JT *)(files[dic_table[dic_no].body].area);
    for(k = 0 ; k < LENGTHYOMI; k++){
	yomi[k] = 0;
    }

    if(jtl->syurui == WNN_UD_DICT){
	if(inspect_ud(dic_no , serial, yomi, &jdata) == -1) return(-1);
#if	defined(CONVERT_by_STROKE) || defined(CONVERT_with_SiSheng)
    }else if((jtl->syurui & 0xff) == WNN_REV_DICT){
#else
    }else if(jtl->syurui == WNN_REV_DICT){
#endif	/* CONVERT_by_STROKE || CONVERT_with_SiSheng */
	if(inspect_rd(dic_no , serial, yomi, &jdata) == -1) return(-1);
    }else{
	if(inspect_sd(dic_no , serial, yomi, &jdata) == -1) return(-1);
    }
    return(0);
}


int
inspect_sd(dic_no , serial, yomi, jd)
int dic_no;
int serial;
w_char *yomi;
struct jdata *jd;
{
    int len;
    struct JT *jtl;
    struct HJT *hjtl;
    UCHAR *hontaistart;
    UCHAR *hopter;
    int key;
    int tsize;
    w_char *charst;
    w_char *sumst;
    int *ptrst;
    int state;
    int sdiff;
    int last;
    int serialst;
    int kanji1;
    

    yomi[0] = 0;
    jtl = (struct JT *)(files[dic_table[dic_no].body].area);
    if(dic_table[dic_no].hindo >= 0)
	hjtl = (struct HJT *)(files[dic_table[dic_no].hindo].area);
    else hjtl = NULL;
    hontaistart = jtl->hontai;
    hopter = hontaistart;
    if(serial >= jtl->maxserial){
	wnn_errorno = WNN_WORD_NO_EXIST;
	return(-1);
    }
    if(jtl->hinsi[serial] == SAKUJO_HINSI) return(-1);

    for(;;){
	switch((state = *(w_char *)hopter)){
	case ST_NORMAL:
	case ST_NOPTER:
	    tsize = *(w_char *)(hopter + 2);
	    serialst = *(int *)(hopter + 4);
	    kanji1 = *(int *)(hopter + 8);
	    charst = (w_char *)(hopter + 12);
	    sumst = ((w_char *)charst + tsize + 2);
	    /* + 2 keeps two zero words */
	    ptrst = (int *)((w_char *)sumst + tsize);
	    sdiff = serial - *(int *)(hopter + 4);
	    
	    /* May be a little late, but enough */
	    if((int)sdiff <  (int)sumst[tsize - 1]){
		for(key = 0 ; key < tsize; key++){
		    if((int)sdiff < (int)sumst[key]){
			break;
		    }
		}
		len = Strlen(yomi);
		yomi[len] = charst[key];
		yomi[len + 1] = 0;

		jd->kanji1 = kanji1;
		jd->kanji2 = serial - serialst;
		jd->serial = serial;
		jd->kosuu = 1;	
		jd->jishono = dic_no;
		jd->jptr = NULL;
		jd->hinsi = jtl->hinsi + serial;
		if(hjtl){
		    jd->hindo = hjtl->hindo + serial;
		    jd->hindo_in = jtl->hindo + serial;
		}else{
		    jd->hindo = jtl->hindo + serial;
		    jd->hindo_in = NULL;
		}
		return(0);
	    }else{
		if(state == ST_NOPTER){
		    wnn_errorno = WNN_WORD_NO_EXIST;
		    return(-1);
		}
		last = 0;
		for(key = 0 ; key < tsize; key++){
		    if(ptrst[key]){
			if(serial < get_fst_serial(hontaistart + ptrst[key], hontaistart))
			    break;
			last = key;
		    }
		}
		len = Strlen(yomi);
		yomi[len] = charst[last];
		yomi[len + 1] = 0;
		hopter = hontaistart + ptrst[last];
	    }
	    break;
	case ST_NOENT:
	    tsize = *(w_char *)(hopter + 2);
	    charst = (w_char *)(hopter + 4);
	    ptrst = (int *)AL_INT((w_char *)charst + tsize);
	    last = 0;
	    for(key = 0 ; key < tsize; key++){
		if(ptrst[key]){
		    if(serial < get_fst_serial(hontaistart + ptrst[key],hontaistart))
			break;
		    last = key;
		}
	    }
	    len = Strlen(yomi);
	    yomi[len] = charst[last];
	    yomi[len + 1] = 0;
	    hopter = hontaistart + ptrst[last];
	    break;
	case ST_SMALL:
	    len = Strlen(yomi);
	    yomi[len] = *(w_char *)(hopter + 2);
	    yomi[len + 1] = 0;
	    hopter = hopter + 4;
	    break;
	default:
	    error1( "inspect_sd:Error 2\n");
	}
    }
}

int
get_fst_serial(hopter, hontaistart)
UCHAR *hontaistart;
char *hopter;
{
    int tsize;
    int k;
    int *ptrst;
    w_char *charst;

    switch(*(w_char *)hopter){
    case ST_NORMAL:
    case ST_NOPTER:
	return(*(int *)(hopter + 4));
    case ST_NOENT:
	tsize = *(w_char *)(hopter + 2);
	charst = (w_char *)(hopter + 4);
	ptrst = (int *)AL_INT((w_char *)charst + tsize);
	for(k = 0; k < tsize; k++){
	    if(ptrst[k]){
		return(get_fst_serial(hontaistart + ptrst[k], hontaistart));
	    }
	}
	break;
    case ST_SMALL:
	return(get_fst_serial(hopter + 4, hontaistart));
    default:
	error1( "get_fst_serial:Error 1\n");
    }
    return(-1);
}
	

int
inspect_ud(dic_no , serial, yomi, jd)
int dic_no;
int serial;
w_char *yomi;
struct jdata *jd;
{
    struct JT *jtl;
    struct HJT *hjtl;
    struct uind1 *tary;
    struct uind2 *p;
    int k;
    int len;

    jtl = (struct JT *)(files[dic_table[dic_no].body].area);
    tary = jtl->table;
    if(dic_table[dic_no].hindo >= 0)
	hjtl = (struct HJT *)(files[dic_table[dic_no].hindo].area);
    else hjtl = NULL;

    if(serial >= jtl->maxserial){
	wnn_errorno = WNN_WORD_NO_EXIST;
	return(-1);
    }
    if(jtl->hinsi[serial] == SAKUJO_HINSI) return(-1);

    for(k = 0 ; k < jtl->maxtable ; k++){
	for(p=((struct uind2 *)((tary[k].pter) + jtl->hontai));
	    ;p = ((struct uind2 *)((p->next) + jtl->hontai))){
	    if((int)p->serial <=(int)serial &&
	       (int)(p->serial + p->kosuu) > (int)serial) goto found_it;
	    if(p->next == ENDPTR) break;
	}
    }
    wnn_errorno = WNN_WORD_NO_EXIST;
    return(-1);

 found_it:
    yomi[0] = tary[k].yomi1 >> 16;
    yomi[1] = tary[k].yomi1 & 0xffff;
    if(yomi[1]){
	yomi[2] = tary[k].yomi2 >> 16;
	if(yomi[2]){
	    yomi[3] = tary[k].yomi2 & 0xffff;
	}
    }
    len = p->yomi[0];
    Strncpy(yomi + 4, (p->yomi) + 1, len - 4);
    yomi[len] = 0;

    jd->kanji1 = p->kanjipter;
    jd->kanji2 = serial - p->serial;
    jd->serial = serial;
    jd->kosuu = 1;	
    jd->jishono = dic_no;
    jd->jptr = NULL;
    jd->hinsi = jtl->hinsi + serial;
    if(hjtl){
	jd->hindo = hjtl->hindo + serial;
	jd->hindo_in = jtl->hindo + serial;
    }else{
	jd->hindo = jtl->hindo + serial;
	jd->hindo_in = NULL;
    }

    return(0);
}


