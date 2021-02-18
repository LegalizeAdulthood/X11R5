/*
 * $Id: rd_jishoop.c,v 1.3 1991/09/16 21:31:59 ohm Exp $
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

static void rd_make_space(), rd_remove_space();

#ifdef	CONVERT_with_SiSheng
extern w_char *biki_sisheng();
unsigned int sisheng_int;
static char pan_tmp[256];
w_char pan_yomi[LENGTHYOMI];
#endif

int
rd_word_add1(jtl,pyomi,hinsi,pkanji, comment) 
/* pyomi, pkanji is not reverse (normal) */
struct JT *jtl;
int hinsi;
w_char *pkanji, *comment;
register w_char *pyomi;
{
    register struct rind1 *tary;
    register int key,key0,keye;
    int serial = jtl->maxserial;
    w_char *tmp = (w_char *)0;
    register struct rind2 *ri2;
    int ind;
    int which;
    w_char *csult;

    w_char yomi[LENGTHYOMI];
    w_char kanji[LENGTHYOMI];

#ifdef	CONVERT_with_SiSheng
    extern w_char *biki_sisheng();
    unsigned int sisheng_int;
    char pan_tmp[256];
    w_char pan_yomi[LENGTHYOMI];

    sisheng_int = 0;
    biki_sisheng(pyomi,pan_tmp,yomi);
    sscanf(pan_tmp,"%d",&sisheng_int);
    Strcpy(kanji, pkanji);
#else
    Sreverse(yomi, pyomi);
    Sreverse(kanji, pkanji);
#endif

    if(jtl->bufsize_kanji <= jtl->maxkanji + (Strlen(kanji) + Strlen(comment) + Strlen(yomi) + 3) * sizeof(w_char) + 1 &&
       ud_realloc_kanji(jtl) == -1){
	return(-1);
    }
    if(jtl->bufsize_ri1[D_YOMI] <= jtl->maxri1[D_YOMI] + 2 &&
       rd_realloc_ri1(jtl, D_YOMI) == -1){
	return(-1);
    }
    if(jtl->bufsize_ri1[D_KANJI] <= jtl->maxri1[D_KANJI] + 2 &&
       rd_realloc_ri1(jtl, D_KANJI) == -1){
	return(-1);
    }
    if(jtl->bufsize_serial <= jtl->maxserial + 4 &&
       ud_realloc_serial(jtl) == -1){
	return(-1);
    }

    ri2 = jtl->ri2;

    tary = jtl->ri1[D_YOMI];
    key = rd_binary_same(tary,yomi,jtl->maxri1[D_YOMI],jtl->ri2, jtl->kanji, 
		    D_YOMI);
    if(key != -1){	/*  Entry with same yomi */
	ind = tary[key].pter;
	while(1){
	    w_char kanji1[LENGTHKANJI];
	    w_char comment1[LENGTHKANJI];
	    if(jtl->hinsi[ind] == hinsi){
		Get_kanji(ri2[ind].kanjipter + jtl->kanji, 
			  yomi, Strlen(yomi),kanji1, NULL, comment1);
		if(Strcmp(pkanji, kanji1) == 0){
		    /* Same hinsi, same kanji, same yomi */
		    if(Strcmp(comment, comment1) != 0){
			/* set_comment */
			ri2[ind].kanjipter = jtl->maxkanji;
			kanjiadd(jtl, pkanji, pyomi, comment);
		    }
		    return(ind);
		}
	    }
	    if(ri2[ind].next[D_YOMI] == RD_ENDPTR)break;
	    ind = ri2[ind].next[D_YOMI];
	}
    }
    for(which = 0; which < 2; which++){
	csult = (which == 0)? yomi: kanji;
	tary = jtl->ri1[which];
	key = rd_binary1(tary,csult,jtl->maxri1[which],jtl->ri2, jtl->kanji, 
			 which);
	if(key >= 0){
	    tmp = KANJI_str(ri2[tary[key].pter].kanjipter + jtl->kanji, which);
#ifdef	CONVERT_with_SiSheng
	    tmp = biki_sisheng(tmp,pan_tmp,pan_yomi);
#endif
	}
	if(key < 0 || Strcmp(csult,tmp)){ /* No entry with same yomi */
	    key += 1;
	    for(key0 = key - 1; key0 >= 0; key0 = tary[key0].pter1){
		tmp = KANJI_str(ri2[tary[key0].pter].kanjipter + jtl->kanji, which);
#ifdef	CONVERT_with_SiSheng
		tmp = biki_sisheng(tmp,pan_tmp,pan_yomi);
#endif
		if(Substr(tmp,csult)){
		    break;
		}
	    }
#ifdef CONVERT_by_STROKE
            if ( jtl->bind != NULL )
		b_index_add(jtl, pyomi,serial);
#endif
	    rd_make_space(tary, key, jtl, jtl->maxri1[which], which);
	    for(keye = key + 1; keye < jtl->maxri1[which];keye++){
		tmp = KANJI_str(ri2[tary[keye].pter].kanjipter + jtl->kanji, which);
#ifdef	CONVERT_with_SiSheng
		tmp = biki_sisheng(tmp,pan_tmp,pan_yomi);
#endif
		if(!Substr(csult, tmp)) break;
		if(tary[keye].pter1 == key0)
		    tary[keye].pter1 = key;
	    }
	    tary[key].pter1 = key0;
	    ri2[serial].next[which] = RD_ENDPTR;
	}else{
	    ri2[serial].next[which] = tary[key].pter;
	}
	tary[key].pter = serial;
    }
    jtl->maxserial++;
    jtl->maxri2++;
    jtl->gosuu++;
    ri2[serial].kanjipter = jtl->maxkanji;
    jtl->hinsi[serial] = hinsi;
    jtl->hindo[serial] = 0;	/* Hindo is set later on upper module */
#ifdef	CONVERT_with_SiSheng
    jtl->sisheng[serial] = sisheng_int;
#endif
    kanjiadd(jtl, pkanji, pyomi, comment);
    return(serial);
}    

int
rd_word_delete1(jtl,hjtl,serial)
struct JT *jtl;
struct HJT *hjtl;
int serial;
{
    /*
    int k;
    */
    register struct rind1 *tary;
    register int ind1;
    int *pt;
    /*
    UCHAR *c;
    int tmp;
    */
    w_char *yomi;
    struct rind2 *ri2;
    int which;

    ri2 = jtl->ri2;
    for(which = 0; which < 2; which++){
	tary = jtl->ri1[which];
	yomi = KANJI_str(ri2[serial].kanjipter + jtl->kanji, which);
#ifdef	CONVERT_with_SiSheng
	yomi = biki_sisheng(yomi,pan_tmp,pan_yomi);
#endif
	ind1 = rd_binary(tary,yomi,jtl->maxri1[which], 
		      jtl->ri2, jtl->kanji, which);
	if(ind1 == -1){
	    wnn_errorno = WNN_WORD_NO_EXIST;
	    return(-1);
	}
	for(pt = &(tary[ind1].pter);;pt = &(jtl->ri2[*pt].next[which])){
	    if(*pt == serial){
		*pt = jtl->ri2[*pt].next[which];
		break;
	    }
	    if(*pt == RD_ENDPTR){
		wnn_errorno = WNN_WORD_NO_EXIST;  /* Must not come here */
		return(-1);
	    }
	}
	if(tary[ind1].pter == RD_ENDPTR) {
#ifdef CONVERT_by_STROKE
	    if ( jtl->bind != NULL)
		b_index_delete(jtl, serial);
#endif
	    rd_remove_space(tary, ind1, jtl, tary[ind1].pter1, which);
	}
    }

    if(serial == jtl->maxserial - 1){
	jtl->maxserial--;
	jtl->maxri2--;
	if(hjtl)
	    hjtl->maxserial--;
    }
    if((int)(ri2[serial].kanjipter + *(ri2[serial].kanjipter + jtl->kanji))
       >= (int)jtl->maxkanji){
	jtl->maxkanji = ri2[serial].kanjipter;
    }
    jtl->hinsi[serial] = SAKUJO_HINSI;
    jtl->gosuu--;
    return(0);
}


int
inspect_rd(dic_no , serial, yomi, jd)
int dic_no, serial;
w_char *yomi;
struct jdata *jd;
{
    struct JT *jtl;
    struct HJT *hjtl;
    struct rind2 *p;
    /*
    int k;
    int len;
    */

    jtl = (struct JT *)(files[dic_table[dic_no].body].area);
    if(dic_table[dic_no].hindo >= 0)
	hjtl = (struct HJT *)(files[dic_table[dic_no].hindo].area);
    else hjtl = NULL;

    if(serial >= jtl->maxserial){
	wnn_errorno = WNN_WORD_NO_EXIST;
	return(-1);
    }
    if(jtl->hinsi[serial] == SAKUJO_HINSI) return(-1);

    p = jtl->ri2 + serial;
    get_kanji_str(jtl->kanji + p->kanjipter, NULL, yomi, NULL);

    jd->kanji1 = p->kanjipter;
    jd->kanji2 = 0;
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
#ifdef	CONVERT_with_SiSheng
    jd->sisheng = jtl->sisheng + serial;
#endif
    return(0);
}
    
static void
rd_make_space(tary, key, jtl, end, which)
register struct JT *jtl;
register struct rind1 *tary;
register int key;
int end;
int which;
{
    register struct rind1 *p, *p1, *pend;

    pend = tary + key;
    for(p = tary + end - 1, p1 = tary + end; p >= pend; p--, p1--){
	*p1 = *p; /* struct no dainyuu! */
	if(p1->pter1 >= key) p1->pter1++;
    }
    jtl->maxri1[which]++;
}

static void
rd_remove_space(tary, key, jtl, newkey, which)
register struct JT *jtl;
register struct rind1 *tary;
register int key,newkey;
int which;
{
    register struct rind1 *p, *p1, *pend;

    pend = tary + jtl->maxri1[which];
    for(p = tary + key + 1, p1 = tary + key; p < pend; p++, p1++){
	*p1 = *p;
	if(p1->pter1 > key)p1->pter1--;
	else if(p1->pter1 == key) p1->pter1 = newkey;
    }
    jtl->maxri1[which]--;
}
