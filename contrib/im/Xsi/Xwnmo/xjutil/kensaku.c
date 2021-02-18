/*
 * $Id: kensaku.c,v 1.2 1991/09/16 21:35:21 ohm Exp $
 */
/*
 * Copyright 1989, 1990, 1991 by OMRON Corporation
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON and MIT not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON and MIT make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * OMRON, MIT AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL OMRON, MIT OR WNN CONSORTIUM BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	Author:	Seiji Kuwari	OMRON Corporation
 *				kuwa@omron.co.jp
 *				kuwa%omron.co.jp@uunet.uu.net
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
#include "jslib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"

extern int (*jutil_table[2][3])();

/* Dirty,  but share it with select_elemet.c */
struct wnn_jdata *word_searched;

static int
sort_func_ws(a,b)
register char *a, *b;
{
    int ah, bh, ai, bi, iah, ibh, iai, ibi;
    ah = ((struct wnn_jdata *)a)->hindo;
    bh = ((struct wnn_jdata *)b)->hindo;
    iah = ((struct wnn_jdata *)a)->int_hindo;
    ibh = ((struct wnn_jdata *)b)->int_hindo;
    ai = ((struct wnn_jdata *)a)->ima;
    bi = ((struct wnn_jdata *)b)->ima;
    iai = ((struct wnn_jdata *)a)->int_ima;
    ibi = ((struct wnn_jdata *)b)->int_ima;

    if(ai == WNN_IMA_OFF && ah == WNN_ENTRY_NO_USE) return(1);
    if(bi == WNN_IMA_OFF && bh == WNN_ENTRY_NO_USE) return(-1);
    if(iai == WNN_IMA_OFF && iah == WNN_ENTRY_NO_USE) return(1);
    if(ibi == WNN_IMA_OFF && ibh == WNN_ENTRY_NO_USE) return(-1);

    if(ai != bi){
	if(ai < bi) return(1);
	return(-1);
    }
    if(iah >= 0){
	ah += iah;
	bh += ibh;
    }
    if(ah > bh)return(-1);
    if(ah < bh)return(1);
    return(0);
}

static struct wnn_jdata *
word_list1(flag, tbl)
int flag, tbl;
{
    int k;
    char *c;
    char tmp[WNN_HINSI_NAME_LEN * 2];
    w_char kana_buf[2048];
    char buf[16384];
    char *info[MAXJIKOUHO];
    char imatukattayo;
    int cnt;
    int rcnt;
    int tcnt;
    int type;

    static char message4[2048];
    int ret;
    char *table_t[3];
    extern int qsort();

    if (get_env() == -1) return(NULL);

    if (init_input(msg_get(cd, 69, default_message[69], xjutil->lang),
		   msg_get(cd, 70, default_message[70], xjutil->lang),
		   msg_get(cd, 8, default_message[8], xjutil->lang)) == -1) {
	    ring_bell();
	    return(NULL);
    }
retry:
    JWMline_clear(0);
    throw_col(0);
    kk_cursor_normal();
    kana_buf[0] = 0;
    if((ret = kana_in(UNDER_LINE_MODE  , kana_buf , 1024)) == -1) {
	end_input();
	return(NULL);
    }
    if (kana_buf[0] == 0) {
	ring_bell();
	goto retry;
    }
    if ((cnt = js_word_search_by_env(cur_env, kana_buf, &wordrb)) < 0) {
	if_dead_disconnect(cur_env, -1);
	end_input();
	errorkeyin();
	return(NULL);
    }
    word_searched = (struct wnn_jdata *)wordrb.buf;
    qsort((char *)word_searched,cnt,sizeof(struct wnn_jdata),sort_func_ws);
    
    update_dic_list();

    if(cnt == 0){
	print_msg_getc(msg_get(cd, 71, default_message[71], xjutil->lang),
		       NULL, NULL, NULL);
	goto retry;
    }
    /* KURI 語数を、事前に調べる */
    if (flag) {
	    tcnt = cnt;
    } else {
	    for(k = 0, tcnt = 0 ; k < cnt ; k++){
		type = dicinfo[find_dic_by_no(word_searched[k].dic_no)].type;
		if(type == WNN_UD_DICT || type == WNN_REV_DICT
#ifdef	CONVERT_with_SiSheng
		    || type == CWNN_REV_DICT
#endif	/* CONVERT_with_SiSheng */
		   ){
		    tcnt++;
		}
	    }
    }
    for(c = buf,k = 0,rcnt = 0 ; k < cnt ; k++){
	type = dicinfo[find_dic_by_no(word_searched[k].dic_no)].type;
	if(flag || type == WNN_UD_DICT || type == WNN_REV_DICT
#ifdef	CONVERT_with_SiSheng
		|| type == CWNN_REV_DICT
#endif	/* CONVERT_with_SiSheng */
		){
	    info[rcnt] = c;
	    sStrcpy(c, word_searched[k].kanji);
	    if(word_searched[k].com && word_searched[k].com[0]){
		strcat(c, "   {");
		sStrcpy(c + strlen(c), word_searched[k].com);
		strcat(c,"}");
	    }
	    strcat(c,"   (");
	    dic_nickname(word_searched[k].dic_no, c + strlen(c));
	    sprintf(c + strlen(c),":%d",word_searched[k].serial );
	    strcat(c,")  ");
	    if(word_searched[k].ima) imatukattayo = '*';
	    else imatukattayo = ' ';
	    if(word_searched[k].hindo == -1){
		sprintf(c + strlen(c),"----  ");
	    }else{
		sprintf(c + strlen(c),"%c%-3d  ",imatukattayo , word_searched[k].hindo );
	    }
	    if(!(word_searched[k].int_ima && word_searched[k].int_hindo == -1)){
				/* Internal Hindo exist */
		if(word_searched[k].int_hindo == -1){
		    sprintf(c + strlen(c),"----");
		}else{
		    sprintf(c + strlen(c),"(%c%-3d)  ", (word_searched[k].int_ima)? '*':' ', word_searched[k].int_hindo );
		}
	    }
	    if(js_hinsi_name(cur_env->js_id, word_searched[k].hinshi, &rb) == -1) {
		if_dead_disconnect(cur_env, NULL);
		return(NULL);
	    }
	    sStrcpy(tmp, (w_char *)(rb.buf));
	    strcat(c,tmp);
	    rcnt ++;
	    sprintf(c + strlen(c),"  %3d/%-3d",rcnt,tcnt);
	    c += strlen(c) + 1;
	}
    }
    end_input();
    sprintf(message4, "%s %s : %s",
	    msg_get(cd, 69, default_message[69], xjutil->lang),
	    msg_get(cd, 70, default_message[70], xjutil->lang), kana_buf);

    table_t[0] = msg_get(cd, 67, default_message[67], xjutil->lang);
    table_t[1] = msg_get(cd, 68, default_message[68], xjutil->lang);
    table_t[2] = msg_get(cd, 39, default_message[39], xjutil->lang);
    k =  xw_select_one_element(info, rcnt, 0,
			     msg_get(cd, 69, default_message[69], xjutil->lang),
			     3, table_t, jutil_table[0],  main_table[8],  NULL);
    if (k == -1 || k == -3) {
	return(NULL);
    }
    if (flag == 0) {
	for(rcnt = 0; k >= 0 && rcnt < cnt; rcnt++) {
	    type = dicinfo[find_dic_by_no(word_searched[rcnt].dic_no)].type;
	    if(type == WNN_UD_DICT || type == WNN_REV_DICT
#ifdef	CONVERT_with_SiSheng
		|| type == CWNN_REV_DICT
#endif	/* CONVERT_with_SiSheng */
		){
		k--;
	    }
	}
	k = rcnt - 1;
    }
    return(word_searched + k);
}

static void
word_list8(flag)
int flag;
{
    word_list1(flag, 8);
}

void
kensaku()
{
    word_list8(1);		/* 1 for all dicts */
}
