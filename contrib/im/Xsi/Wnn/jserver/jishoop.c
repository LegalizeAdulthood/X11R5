/*
 * $Id: jishoop.c,v 1.3 1991/09/16 21:31:45 ohm Exp $
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

void kanjiadd();
static void make_space();

#ifndef min
#define min(a, b) ((a > b)? b:a)
#define max(a, b) ((a < b)? b:a)
#endif

int
word_add(envi, dic_no, yomi, kanji, hinsi, hindo, comment)
int envi;
int dic_no;
w_char *yomi;
w_char *kanji,*comment;
int hinsi, hindo;
{
    int fid, k;
    struct JT *jtl;
    struct HJT *hjtl;
    int serial;
    
    if(Strlen(yomi) >= LENGTHYOMI){
	wnn_errorno = WNN_YOMI_LONG;
	return(-1);
    }
    if(Strlen(kanji) + Strlen(comment) + Strlen(yomi) >= LENGTHKANJI){
	wnn_errorno = WNN_KANJI_LONG;
	return(-1);
    }
    if(Strlen(yomi) == 0){
	wnn_errorno = WNN_NO_YOMI;
	return(-1);
    }
    if(!dic_in_envp(dic_no,envi)){
	wnn_errorno = WNN_DICT_NOT_IN_ENV;
	return(-1);
    }
    if(dic_table[dic_no].rw == WNN_DIC_RDONLY){
	wnn_errorno = WNN_RDONLY;
	return(-1);
    }

    jtl = (struct JT *)(files[dic_table[dic_no].body].area);

    if(jtl->syurui == WNN_STATIC_DICT){
	wnn_errorno = WNN_NOT_A_UD;
	return(-1);
    }

    if(jtl->syurui == WNN_UD_DICT){
	if((serial = word_add1(jtl,yomi, hinsi, kanji, comment)) == -1)return(-1);
    }else {  /* jtl->syurui == WNN_REV_DICT || CWNN_REV_DICT */
	if((serial = rd_word_add1(jtl,yomi,hinsi,kanji, comment)) == -1)return(-1);
    }
    if(dic_table[dic_no].hindo != -1){
	hjtl = (struct HJT *)(files[dic_table[dic_no].hindo].area);
	if(serial >= hjtl->maxserial){
	    if(serial >= hjtl->bufsize_serial){
		hindo_file_realloc(hjtl);
	    }
	    hjtl->maxserial = jtl->maxserial;
	}
	hjtl->hindo[serial] = asshuku(hindo) | 0x80;
	hjtl->hdirty = 1;
    }else{
	jtl->hindo[serial] = asshuku(hindo) | 0x80;
	jtl->hdirty = 1;
    }
/* Since other hindo file may be used with this dictionary,
   update the size of such hindo file */
    fid = dic_table[dic_no].body;
    for(k = 0; k < MAX_DIC; k++){
	if(dic_table[k].body == fid &&
	   dic_table[k].hindo != -1){
	    hjtl = (struct HJT *)(files[dic_table[k].hindo].area);
	    if(serial >= hjtl->maxserial){
		if(serial >= hjtl->bufsize_serial){
		    hindo_file_realloc(hjtl);
		}
		hjtl->maxserial = jtl->maxserial;
		hjtl->hindo[serial] = 0;
		hjtl->hdirty = 1;
	    }
	}
    }

    jtl->dirty = 1;
    return(0);
}    


int
word_comment_set(envi, dic_no, serial, comment)
int envi;
int dic_no;
int serial;
w_char *comment;
{
    w_char yomi[LENGTHYOMI];
    struct JT *jtl;
    
    if(!dic_in_envp(dic_no,envi)){
	wnn_errorno = WNN_DICT_NOT_IN_ENV;
	return(-1);
    }

    if(dic_table[dic_no].rw == WNN_DIC_RDONLY){
	wnn_errorno = WNN_RDONLY;
	return(-1);
    }

    jtl = (struct JT *)(files[dic_table[dic_no].body].area);

    if(jtl->syurui == WNN_STATIC_DICT){
	wnn_errorno = WNN_NOT_A_UD;
	return(-1);
    }

    if(serial >= jtl->maxserial || serial < 0){
	wnn_errorno = WNN_WORD_NO_EXIST;
	return(-1);
    }
    get_yomi_from_serial(dic_no, serial, yomi);
    jtl->dirty = 1;
    return(word_comment_set1((struct JT *)(files[dic_table[dic_no].body].area),yomi,serial, comment));
}


int
word_delete(envi,dic_no,serial)
int envi;
int dic_no;
int serial;
{
    w_char yomi[LENGTHYOMI];
    struct JT *jtl;
    struct HJT *hjtl;
    int x, fid, k;
    
    if(!dic_in_envp(dic_no,envi)){
	wnn_errorno = WNN_DICT_NOT_IN_ENV;
	return(-1);
    }

    if(dic_table[dic_no].rw == WNN_DIC_RDONLY){
	wnn_errorno = WNN_RDONLY;
	return(-1);
    }

    jtl = (struct JT *)(files[dic_table[dic_no].body].area);
    if(dic_table[dic_no].hindo != -1){
	hjtl = (struct HJT *)(files[dic_table[dic_no].hindo].area);
    }else hjtl = NULL;

    if(jtl->syurui == WNN_STATIC_DICT){
	wnn_errorno = WNN_NOT_A_UD;
	return(-1);
    }

    if((serial >= jtl->maxserial) || (serial < 0)){
	wnn_errorno = WNN_WORD_NO_EXIST;
	return(-1);
    }
    if(get_yomi_from_serial(dic_no, serial, yomi) == -1){
	wnn_errorno = WNN_WORD_NO_EXIST;
	return(-1);
    }
    jtl->dirty = 1;
    if(jtl->syurui == WNN_UD_DICT){
	x = word_delete1(jtl, hjtl, yomi,serial);
#if	defined(CONVERT_by_STROKE) || defined(CONVERT_with_SiSheng)
    }else if((jtl->syurui & 0xff) == WNN_REV_DICT){
#else
    }else if(jtl->syurui == WNN_REV_DICT){
#endif	/* CONVERT_by_STROKE || CONVERT_with_SiSheng */
	x = rd_word_delete1(jtl,hjtl,serial);
    }else{
	wnn_errorno = WNN_NOT_A_UD;
	x = -1;
    } 
    if(x == -1) return(-1);
/* Since other hindo file may be used with this dictionary,
   update the size of such hindo file */
    fid = dic_table[dic_no].body;
    for(k = 0; k < MAX_DIC; k++){
	if(dic_table[k].body == fid &&
	   dic_table[k].hindo != -1){
	    hjtl = (struct HJT *)(files[dic_table[k].hindo].area);
	    if(jtl->maxserial != hjtl->maxserial){
		hjtl->maxserial = jtl->maxserial;
		hjtl->hdirty = 1;
	    }
	}
    }
    return(0);
}


int
word_add1(jtl,pyomi,hinsi,kanji, comment)
struct JT *jtl;
int hinsi;
w_char *kanji, *comment;
w_char *pyomi;
{
    register struct uind1 *tary;
    register struct uind2 *hopter, *hopter1;
    register int key,key0,keye;
    register struct uind2 *hop;
    int len = Strlen(pyomi);
    int serial = jtl->maxserial;
    w_char yomi[LENGTHYOMI];

    Sreverse(yomi, pyomi);

    if(jtl->bufsize_hontai <= jtl->maxhontai + sizeof(struct uind2) + Strlen(yomi) &&
       ud_realloc_hontai(jtl) == -1){
	return(-1);
    }
    if(jtl->bufsize_kanji <= jtl->maxkanji + (Strlen(kanji) + Strlen(comment) + Strlen(yomi) + 3) * sizeof(w_char) + 1 &&
       ud_realloc_kanji(jtl) == -1){
	return(-1);
    }
    if(jtl->bufsize_serial <= jtl->maxserial + 4 &&
       ud_realloc_serial(jtl) == -1){
	return(-1);
    }
    if(jtl->bufsize_table <= jtl->maxtable + sizeof(struct uind1) &&
       ud_realloc_table(jtl) == -1){
	return(-1);
    }

    tary = jtl->table;
    hopter = (struct uind2 *)(jtl->hontai + jtl->maxhontai);

    key = binary1(tary,yomi,jtl->maxtable,jtl);
    if(key < 0 || Strcmpud(&tary[key],yomi,jtl)){ /* No entry with same yomi */
	key += 1;
	for(key0 = key - 1; key0 >= 0; key0 = tary[key0].pter1){
	    hop = (struct uind2 *)(jtl->hontai + tary[key0].pter);
	    if(Substrud(&tary[key0],yomi,hop->yomi[0],jtl)){
		break;
	    }
	}
	make_space(tary, key, jtl);
	for(keye = key + 1; keye < jtl->maxtable && Substrstud(yomi, &tary[keye],jtl); keye++){
	    if(tary[keye].pter1 == key0)
	      tary[keye].pter1 = key;
	}
	tary[key].pter1 = key0;
	tary[key].yomi2 = 0;
	tary[key].yomi1 = yomi[0] << 16;
	if(yomi[1]){
	    tary[key].yomi1 |= yomi[1];
	    if(yomi[2]){
		tary[key].yomi2 = yomi[2] << 16;
		if(yomi[3]){
		    tary[key].yomi2 |= yomi[3];
		}
	    }
	}
	tary[key].pter = 0;
    }else{
	hopter1 = (struct uind2 *)(tary[key].pter + (char *)(jtl->hontai));
	while(1){
	    w_char kanji1[LENGTHKANJI];
	    w_char comment1[LENGTHKANJI];
	    if(jtl->hinsi[hopter1->serial] == hinsi){
		Get_kanji(hopter1->kanjipter + jtl->kanji, 
			  yomi, Strlen(yomi),
			  kanji1, NULL, comment);
		if(Strcmp(kanji, kanji1) == 0){
		/* Same hinsi, same kanji, same yomi */
		    if(Strcmp(comment, comment1) != 0){
		    /* set_comment */
			hopter1->kanjipter = jtl->maxkanji;
			kanjiadd(jtl, kanji, yomi, comment);
		    }
		    return(hopter1->serial);
		}
	    }
	    if(hopter1->next == 0)break;
	    hopter1 = (struct uind2 *)(hopter1->next + (char *)(jtl->hontai));
	}
    }
    hopter->next = tary[key].pter;
    hopter->serial = jtl->maxserial++;
    jtl->gosuu++;
    hopter->kanjipter = jtl->maxkanji;
    hopter->kosuu = 1;
    hopter->yomi[0] = len;
    Strncpy(hopter->yomi + 1, yomi + 4,len - 4);
    tary[key].pter = (char *)hopter - (char *)(jtl->hontai);
    jtl->maxhontai = (char *)(AL_INT(&(hopter->yomi[0]) + 1 + max((len - 4),0)))
	- (char *)(jtl->hontai);
    jtl->hinsi[serial] = hinsi;
    jtl->hindo[serial] = 0;	/* Hindo is set later on upper module */
    kanjiadd(jtl, kanji, yomi, comment);
    
    return(serial);
}    

void
kanjiadd(jtl, kanji, yomi, comment)
struct JT *jtl;
w_char *kanji,*comment, *yomi;
{
    register UCHAR *dest = jtl->kanji + jtl->maxkanji;
    int len;

#if	defined(CONVERT_by_STROKE) || defined(CONVERT_with_SiSheng)
    if((jtl->syurui & 0xff) != WNN_REV_DICT){
#else
    if(jtl->syurui != WNN_REV_DICT){
#endif	/* CONVERT_by_STROKE || CONVERT_with_SiSheng */
	yomi = NULL;
    }
    len = kanjiaddr(dest, kanji, yomi, comment);
    jtl->maxkanji += len;
}

static void
make_space(tary, key, jtl)
register struct JT *jtl;
register struct uind1 *tary;
register int key;
{
    register struct uind1 *p, *p1, *pend;
    int end = jtl->maxtable;

    pend = tary + key;
    for(p = tary + end - 1, p1 = tary + end; p >= pend; p--, p1--){
	*p1 = *p; /* struct no dainyuu! */
	if(p1->pter1 >= key) p1->pter1++;
    }
    jtl->maxtable++;
}

void
remove_space(tary, key, jtl, newkey)
register struct JT *jtl;
register struct uind1 *tary;
register int key,newkey;
{
    register struct uind1 *p, *p1, *pend;
    register int end = jtl->maxtable;

    pend = tary + end;
    for(p = tary + key + 1, p1 = tary + key; p < pend; p++, p1++){
	*p1 = *p;
	if(p1->pter1 > key)p1->pter1--;
	else if(p1->pter1 == key) p1->pter1 = newkey;
    }
    jtl->maxtable--;
}

int
dic_in_envp(dic_no, envi)
int dic_no;
int envi;
{
    register int k;

    for(k = 0 ; k < env[envi]->jishomax; k++){
	if(env[envi]->jisho[k] == dic_no){
	    return(1);
	}
    }
    return(0);
}

int
word_delete1(jtl,hjtl,yomi,serial)
struct JT *jtl;
struct HJT *hjtl;
w_char *yomi;
int serial;
{
    int k;
    register struct uind2 *hopter;
    register struct uind2 *p; 
    register struct uind1 *tary;
    register int ind1;
    int *next_pter;
    UCHAR *c;
    int tmp;

    tary = jtl->table;

    ind1 = binary(tary,yomi,jtl->maxtable, jtl);
    if(ind1 == -1){
	wnn_errorno = WNN_WORD_NO_EXIST;
	return(-1);
    }

    for(
	next_pter = &(tary[ind1].pter),
	p=((struct uind2 *)((tary[ind1].pter) + jtl->hontai));;
	next_pter = &p->next,
	p = ((struct uind2 *)((p->next) + jtl->hontai))){

	if((int)p->serial <= (int)serial && (int)p->serial + (int)p->kosuu > (int)serial)
	    goto found_it;
	if(p->next == ENDPTR) break;
    }
    wnn_errorno = WNN_WORD_NO_EXIST;
    return(-1);

 found_it:
    if(p->kosuu == 1){
	*next_pter = p->next;
	if(tary[ind1].pter == NULL)
	    remove_space(tary, ind1, jtl, tary[ind1].pter1);
	if(p->serial == jtl->maxserial - 1){
	    jtl->maxserial--;
	    if(hjtl)
		hjtl->maxserial--;
	}
	if((int)p->kanjipter + (int)*(p->kanjipter + jtl->kanji) >= (int)jtl->maxkanji){
	    jtl->maxkanji = p->kanjipter;
	}
	if(jtl->maxhontai + (char *)(jtl->hontai) ==
	   (char *)(AL_INT(&(p->yomi[0]) + 1 + max(((int)p->yomi[0] - 4),0)))){
	    jtl->maxhontai = (char *)p - (char *)(jtl->hontai);
	}
    }else if(p->serial == serial){
	p->kosuu -= 1;
	p->serial += 1;
	p->kanjipter += *(p->kanjipter + jtl->kanji);
    }else if(p->serial + p->kosuu - 1 == serial){
	p->kosuu -= 1;
    }else{   /* devide left space into 2 */

	if(jtl->bufsize_hontai <= jtl->maxhontai + sizeof(struct uind2) + Strlen(yomi)){
	    tmp = (char *)p - (char *)jtl->hontai;
	    if(ud_realloc_hontai(jtl) == -1){
		return(-1);
	    }
	    error1("Realloc in Word_delete for serial %d\n",serial);
	    p = (struct uind2 *)(tmp + jtl->hontai);

	}
	hopter = (struct uind2 *)(jtl->hontai + jtl->maxhontai);
	hopter->next = p->next;
	hopter->serial = serial + 1;

	for(k = 0,c = p->kanjipter + jtl->kanji ; k < serial - p->serial + 1; k++){
	    c += *c;
	}
	hopter->kanjipter = c - jtl->kanji;

	hopter->kosuu = p->kosuu - (serial - p->serial) - 1;
	hopter->yomi[0] = p->yomi[0];
	Strncpy(hopter->yomi + 1, p->yomi + 1,max(((int)p->yomi[0] - 4),0));
	p->next = (char *)hopter - (char *)(jtl->hontai);
	jtl->maxhontai = (char *)(AL_INT(&(hopter->yomi[0]) + 1 +
					 max(((int)p->yomi[0] - 4),0)))
	                  - (char *)(jtl->hontai);
	
	p->kosuu = serial - p->serial;
    }
    jtl->hinsi[serial] = SAKUJO_HINSI;
    jtl->gosuu--;
    return(0);
}


int
word_comment_set1(jtl,yomi,serial, comment)
struct JT *jtl;
w_char *yomi;
int serial;
w_char *comment;
{
    int k;
    register struct uind2 *p; 
    register struct uind1 *tary;
    register int ind1;
    UCHAR *kptr;
    w_char yomi1[LENGTHYOMI], kanji[LENGTHKANJI], ocomment[LENGTHKANJI];

    tary = jtl->table;

    if(jtl->syurui == WNN_UD_DICT){
	ind1 = binary(tary,yomi,jtl->maxtable, jtl);
	if(ind1 == -1){
	    wnn_errorno = WNN_WORD_NO_EXIST;
	    return(-1);
	}

	for(p=((struct uind2 *)((tary[ind1].pter) + jtl->hontai));;
	    p = ((struct uind2 *)((p->next) + jtl->hontai))){

	    if((int)p->serial <= (int)serial && (int)p->serial + (int)p->kosuu > (int)serial){
		kptr = p->kanjipter + jtl->kanji;
		for(k = serial - p->serial; k > 0 ; k--){
		    kptr += *kptr;
		}
		goto found_it;
	    }
	    if(p->next == ENDPTR) break;
	}
	wnn_errorno = WNN_WORD_NO_EXIST;
	return(-1);
    }else{
	kptr = jtl->kanji + (jtl->ri2)[serial].kanjipter;
    }
 found_it:
    get_kanji_str(kptr, kanji, yomi1, ocomment);
    if(Strlen(ocomment) > Strlen(comment)){
	w_char *com, *kan, *yom;
	Get_kanji_str_r(kptr, &kan, &yom, &com);
	Strcpy(com, comment);
    }else{
	if(jtl->bufsize_kanji <= jtl->maxkanji + 
	   (Strlen(kanji) + Strlen(comment) + Strlen(yomi1) + 4) * 2){
	    if(ud_realloc_kanji(jtl) == -1){
		return(-1);
	    }
	}
	*(kptr + 1) = FORWARDED;
	*(w_char *)(kptr + 2) = (jtl->maxkanji + jtl->kanji - kptr) >> 16;
	*(w_char *)(kptr + 4) = (jtl->maxkanji + jtl->kanji - kptr) & 0xffff;

	kanjiadd(jtl, kanji, yomi1, comment);
#if	defined(CONVERT_by_STROKE) || defined(CONVERT_with_SiSheng)
	if((jtl->syurui & 0xff) == WNN_REV_DICT){
#else
	if(jtl->syurui == WNN_REV_DICT){
#endif	/* CONVERT_by_STROKE || CONVERT_with_SiSheng */
	    /* We need to spend one serial_no to ensure that the entry
	       before this and after this are not connected */
	    if(jtl->bufsize_serial <= jtl->maxserial + 4 &&
	       ud_realloc_serial(jtl) == -1){
		return(-1);
	    }
	    jtl->hinsi[jtl->maxserial] = SAKUJO_HINSI;
	    jtl->ri2[jtl->maxserial].kanjipter = 0xffffffff;
	    jtl->ri2[jtl->maxserial].next[D_YOMI] = 
		jtl->ri2[jtl->maxserial].next[D_KANJI] = RD_ENDPTR;
	    jtl->maxserial++;
	    jtl->maxri2++;
	}
    }
    return(0);

#ifdef nodef
/*    UCHAR *c, *c0; */
/*	int kan, len; */
    if(p->kosuu == 1){
	Get_kanji(p->kanjipter + jtl->kanji,
		  yomi, Strlen(yomi),
		  kanji, NULL, NULL);
	if(jtl->bufsize_kanji <= jtl->maxkanji + 
	   (Strlen(kanji) + Strlen(comment)) * sizeof(w_char) + 4 &&
	   ud_realloc_kanji(jtl) == -1){
	    return(-1);
	}
	if(p->kanjipter + *(p->kanjipter + jtl->kanji) >= jtl->maxkanji){
	    jtl->maxkanji = p->kanjipter;
	}
	p->kanjipter = jtl->maxkanji;
	kanjiadd(jtl, kanji, yomi, comment);
    }else{
	/*nagasa wo hakatte realloc */
	for(k = 0,c = p->kanjipter + jtl->kanji ; k < p->kosuu; k++)c += *c;
	len = c - (p->kanjipter + jtl->kanji);
	while(jtl->bufsize_kanji <= jtl->maxkanji + 
	      (Strlen(kanji) + Strlen(comment)) * sizeof(w_char) + 4 + len){
	    if(ud_realloc_kanji(jtl) == -1){
		return(-1);
	    }
	}
	if(p->kanjipter + len >= jtl->maxkanji){
	    jtl->maxkanji = p->kanjipter;
	}
	kan = jtl->maxkanji;

	/*mae made no nagasa wo hakatte bcopy */
	for(k = 0,c = p->kanjipter + jtl->kanji ; k < serial - p->serial; k++){
	    c += *c;
	}
	len = c - (p->kanjipter + jtl->kanji);	
	bcopy(p->kanjipter + jtl->kanji, jtl->maxkanji + jtl->kanji, len);

	jtl->maxkanji += len;
	Get_kanji(c, 
		  yomi, Strlen(yomi),
		  kanji, NULL, NULL);
	kanjiadd(jtl, kanji, yomi, comment);
	c += *c;

	c0 = c;
	for(k = serial + 1,c = p->kanjipter + jtl->kanji;
	    k < p->kosuu + p->serial; k++){
	    c += *c;
	}
	len = c - c0;
	bcopy(c0, jtl->maxkanji + jtl->kanji, len);
	jtl->maxkanji += len;
	p->kanjipter = kan;
    }
    return(0);
#endif
}


int
hindo_file_size_justify(wfp, whfp)
struct wnn_file *wfp, *whfp;
{
    struct HJT *hjtp;
    struct JT *jtp;
    int k;

    jtp = (struct JT *)(wfp->area);
    hjtp = (struct HJT *)(whfp->area);

    if(hjtp->maxserial != jtp->maxserial){
	if(hjtp->maxserial < jtp->maxserial){
	    while(hjtp->bufsize_serial <= jtp->maxserial){
		if(hindo_file_realloc(hjtp) == -1)return(-1);
	    }
	    error1("Dic file size is bigger than that of Hindo file!");
	    for(k = hjtp->maxserial; k < jtp->maxserial; k++){
		hjtp->hindo[k] = 0;
	    }
	}else{
	    error1("Hindo file size is bigger than that of Dic file!");
	}
	hjtp->maxserial = jtp->maxserial;
	hjtp->hdirty = 1;
    }
    return(0);
}
