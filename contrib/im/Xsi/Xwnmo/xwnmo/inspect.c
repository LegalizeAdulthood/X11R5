/*
 * $Id: inspect.c,v 1.2 1991/09/16 21:36:56 ohm Exp $
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
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "config.h"
#include "ext.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"

int (*local_tbl[2])() = {
	inspectdel,
	inspectuse
};

static int
xw_inspect(c, in)
char *c;
int in;
{
    static int select_step = 0;
    unsigned int c1;
    int d;
    int ret;
    unsigned int *output;

    if (select_step == 0) {
	if (init_inspect((unsigned char*)c)) {
	    ring_bell();
	    return(-1);
	}
	select_step++;
	return(BUFFER_IN_CONT);
    }
    if (select_step == 1) {
	output = romkan_henkan(in);
	if (*output == EOLTTR) return(BUFFER_IN_CONT);
	c1 = *output;
	if (!xim->inspect->map) return(BUFFER_IN_CONT);
	if (xim->sel_ret == -2) {
	    xim->sel_ret = -1;
            end_inspect();
	    select_step = 0;
	    romkan_clear();
	    return(-1);
	} else if (xim->sel_ret != -1) {
	    d = xim->sel_ret;
	    romkan_clear();
	    xim->sel_ret = -1;
	    if (local_tbl[d]) {
		lock_inspect();
		if((ret = (*local_tbl[d])()) == 1) {
		    end_inspect();
	    	    select_step = 0;
	    	    return(0);
		} else {
	    	    unlock_inspect();
		    return(BUFFER_IN_CONT);
		}
	    } else {
		return(BUFFER_IN_CONT);
	    }
	} else if((c1 < 256) && (main_table[10][c1] != NULL)){
	    if((ret = (*main_table[10][c1])()) == 1){
		end_inspect();
		select_step = 0;
		romkan_clear();
		return(0);
	    }else if(ret == 0){
		romkan_clear();
		return(BUFFER_IN_CONT);
	    } else if(ret == -1){
		end_inspect();
		select_step = 0;
		romkan_clear();
		return(0);
	    }
	} else {
	    ring_bell();
	    romkan_clear();
	    return(BUFFER_IN_CONT);
	}
    }
    romkan_clear();
    return(BUFFER_IN_CONT);
}

int
inspect(bun_no, c, in)
int bun_no;
char *c;
int in;
{
    if (xw_inspect(c, in) == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    }
    return(1);
}

static int next_flag;
#define	I_NEXT 1
#define	I_PREV 2
#define	I_CUR 3

static int
update_dic_list(buf)
struct wnn_buf *buf;
{
    if((dic_list_size = jl_dic_list(bun_data_, &dicinfo)) == -1) {
	errorkeyin();
	return(-1);
    }
    return(0);
}

static int
find_dic_by_no(dic_no)
int dic_no;
{
    int j;

    for(j = 0 ; j < dic_list_size; j++){
	if(dicinfo[j].dic_no == dic_no)
	    return(j);
    }
    return(-1);
}

static int
dic_nickname(dic_no, buf)
int dic_no;
char *buf;
{
    int j;

    if((j = find_dic_by_no(dic_no)) == -1)return(-1);
    if(*dicinfo[j].comment) sStrcpy(buf, dicinfo[j].comment);
    else strcpy(buf, dicinfo[j].fname);
    return(0);
}

static int Bun_no;

static int
make_string_for_ke(bun_no,buf,buf_size)
int bun_no;
char *buf;
int buf_size;
{
    struct wnn_jl_bun *bun;
    w_char buf1[1024];
    w_char save_c;
    char tmp[WNN_HINSI_NAME_LEN * 2];
    char *lang;

    lang = cur_lang;

    Bun_no = bun_no;
    bun = bun_data_->bun[bun_no];
    jl_get_kanji(bun_data_, bun_no, bun_no + 1, buf1);
    if (print_out_func) (*print_out_func)(buf1, buf1, Strlen(buf1));
    sStrcpy(buf,buf1);
    strcat(buf," ");
    jl_get_yomi(bun_data_, bun_no, bun_no + 1, buf1);
    if (print_out_func) (*print_out_func)(buf1, buf1, Strlen(buf1));
    save_c = *(buf1 + bun->jirilen);
    *(buf1 + bun->jirilen) = 0;
    sStrcpy(buf + strlen(buf), buf1);
    *(buf1 + bun->jirilen) = save_c;
    strcat(buf, "-");
    sStrcpy(buf + strlen(buf), buf1 + bun->jirilen);

    set_escape_code(buf);

    strcat(buf,"   (");
    if(bun->dic_no >= 0){
	update_dic_list(bun_data_);
	dic_nickname(bun->dic_no, buf + strlen(buf));
	sStrcpy(tmp, jl_hinsi_name(bun_data_,bun->hinsi));
	sprintf(buf + strlen(buf), ":%d %s",
		bun->entry,tmp);
	if(bun->hindo != 0x7f){
	    sprintf(buf + strlen(buf), " %s:%c%d",msg_get(cd, 4, default_message[4], lang),
		    bun->ima ? '*' : ' ',
		    bun->hindo);
	}else{
	    sprintf(buf + strlen(buf), " ---------");
	}			/* Means This entry is not used.
				   This must not appear in ordinary use!*/

    }else{
	sStrcpy(tmp, jl_hinsi_name(bun_data_,bun->hinsi));
	sprintf(buf + strlen(buf),"GIJI BUNSETU:%s", tmp);
    }
    strcat(buf,")   ");
    sprintf(buf + strlen(buf),"%s:%d %s:%d",
	    msg_get(cd, 5, default_message[5], lang),bun->hyoka,msg_get(cd, 6, default_message[6], lang),
	    bun->kangovect);
	    

    if(bun->dai_top){
	if(bun->daihyoka != -1){
	    sprintf(buf + strlen(buf),"(%s:%d) ",msg_get(cd, 7, default_message[7], lang),
	    bun->daihyoka);
	}else {
	    sprintf(buf + strlen(buf),"(%s:---) ",msg_get(cd, 7, default_message[7], lang));
	}
    }

    return(0);
}

int
inspect_kouho(in)
int in;
{
    static char buf[1024];
    static WnnClientRec *c_c_sv = 0;

#ifdef	CALLBACKS
    if (IsPreeditCallbacks(cur_x) && cur_x->cb_redraw_needed) {
        t_print_l();
        c_c_sv = 0;
        pop_func(c_c);
        cur_x->cb_redraw_needed = 0;
        return(0);
    }
#endif	/* CALLBACKS */

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return(0);
    }
    if (c_c_sv == 0) {
	c_c_sv = c_c;
	push_func(c_c, inspect_kouho);
	next_flag = I_CUR;
    }
    if(make_string_for_ke(cur_bnst_,buf,sizeof(buf)) == -1){
	print_msg_getc("Error in inspect", NULL, NULL, NULL);
	return(0);
    }
    if (inspect(cur_bnst_, buf, in) == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    }
#ifdef	CALLBACKS
    if (IsPreeditCallbacks(cur_x)) {
        cur_x->cb_redraw_needed = 1;
        SendCBRedraw();
        return(BUFFER_IN_CONT);
    }
#endif	/* CALLBACKS */
    t_print_l();
    c_c_sv = 0;
    pop_func(c_c);
    return(0);
}

static int
sakujo_kouho1(bun_no)
int bun_no;
{
    w_char w_buf[512];
    char buf[512];
    int kanji_len = jl_kanji_len(bun_data_, bun_no, bun_no+1)
		  - jl_fuzoku_len(bun_data_, bun_no); 
    char *lang;

    lang = cur_lang;

    jl_get_yomi(bun_data_, bun_no, bun_no + 1, w_buf);
    w_buf[kanji_len] = 0;
    w_sttost(w_buf, buf);
    strcat(buf, msg_get(cd, 8, default_message[8], lang));
    strcat(buf,":");
    jl_get_yomi(bun_data_, bun_no, bun_no + 1, w_buf);
    w_buf[jl_jiri_len(bun_data_, bun_no)] = 0;
    w_sttost(w_buf,buf + strlen(buf));
    strcat(buf," ");
    sprintf(buf + strlen(buf), "%s?",msg_get(cd, 9, default_message[9], lang));
    if(yes_or_no(buf) == 1){
	if(jl_word_delete(bun_data_, bun_data_->bun[bun_no]->dic_no, 
			  bun_data_->bun[bun_no]->entry) == -1){
	    errorkeyin();
	    return(0);
	}
    }
    return(0);
}

int
sakujo_kouho()
{
    sakujo_kouho1(cur_bnst_);
    return(0);
}

int
inspectdel()
{
    struct wnn_jl_bun *bun  = bun_data_->bun[Bun_no];
    w_char buf1[1024];
    char buf[512];
    int type;
    int kanji_len = jl_kanji_len(bun_data_, Bun_no, Bun_no+1) -
	jl_fuzoku_len(bun_data_, Bun_no);
    char *lang;

    lang = cur_lang;

    type = dicinfo[find_dic_by_no(bun->dic_no)].type;
    if(type != WNN_UD_DICT && type != WNN_REV_DICT
#ifdef	CONVERT_with_SiSheng
	       && type != CWNN_REV_DICT
#endif	CONVERT_with_SiSheng
       ){
	print_msg_getc("%s",msg_get(cd, 10, default_message[10], lang),
		       NULL, NULL);
	return(0);
    }
    if(dicinfo[find_dic_by_no(bun->dic_no)].rw == WNN_DIC_RDONLY){
	print_msg_getc("%s",msg_get(cd, 11, default_message[11], lang),
		       NULL, NULL);
	return(0);
    }

    jl_get_kanji(bun_data_, Bun_no, Bun_no + 1, buf1);
    buf1[kanji_len] = 0;

    strcpy(buf, msg_get(cd, 12, default_message[12], lang));
    w_sttost(buf1, buf + strlen(buf));
    sprintf(buf + strlen(buf), "%s",msg_get(cd, 13, default_message[13], lang));
    set_escape_code(buf);	/* ADD KURI */
    if(yes_or_no(buf) == 1){
	if(jl_word_delete(bun_data_, bun->dic_no,
			  bun->entry) == -1){
	    errorkeyin();
	    return(0);
	}
	return(1);
    }
    return(0);
}

int
inspectuse()
{
    struct wnn_jl_bun *bun = bun_data_->bun[Bun_no];
    w_char buf1[1024];
    char buf[512];
    int kanji_len = jl_kanji_len(bun_data_, Bun_no, Bun_no+1) -
	jl_fuzoku_len(bun_data_, Bun_no);
    char *lang;

    lang = cur_lang;

    if(dicinfo[find_dic_by_no(bun->dic_no)].hindo_rw == WNN_DIC_RDONLY){
	print_msg_getc("%s",msg_get(cd, 14, default_message[14], lang),
		       NULL, NULL);
	return(0);
    }
    strcpy(buf, msg_get(cd, 12, default_message[12], lang));
    jl_get_kanji(bun_data_, Bun_no, Bun_no + 1, buf1);
    buf1[kanji_len] = 0;
    w_sttost(buf1, buf + strlen(buf));
    set_escape_code(buf);	/* ADD KURI */
    sprintf(buf + strlen(buf), "%s",msg_get(cd, 15, default_message[15], lang));
    if(yes_or_no(buf) == 1){
	if(jl_word_use(bun_data_, bun->dic_no, bun->entry) == -1){
	    errorkeyin();
	    return(0);
	}
	return(1);
    }
    return(0);
}

unsigned char *
next_inspect()
{
    char buf[1024];
    char *c = buf;

    if (jl_zenkouho_bun(bun_data_) != cur_bnst_ ||
	jl_zenkouho_daip(bun_data_) != WNN_SHO){
	jl_zenkouho(bun_data_, cur_bnst_, WNN_USE_ZENGO, WNN_UNIQ);
    }
    jl_next(bun_data_);
    make_kanji_buffer(cur_bnst_);
    c_b->t_m_start = bunsetsuend[cur_bnst_ ];
    c_b->t_b_st = dai_top(bun_data_, cur_bnst_);	/* add by KUWA */
    c_b->t_b_end = dai_end(bun_data_, cur_bnst_);	/* add by KUWA */
    if(make_string_for_ke(cur_bnst_,buf,sizeof(buf)) == -1){
	print_msg_getc("Error in inspect", NULL, NULL, NULL);
	return(NULL);
    }
    return((unsigned char *)c);
}

unsigned char *
previous_inspect()
{
    char buf[1024];
    char *c = buf;

    if (jl_zenkouho_bun(bun_data_) != cur_bnst_ ||
	jl_zenkouho_daip(bun_data_) != WNN_SHO){
	jl_zenkouho(bun_data_, cur_bnst_, WNN_USE_ZENGO, WNN_UNIQ);
    }
    jl_previous(bun_data_);
    make_kanji_buffer(cur_bnst_);
    c_b->t_m_start = bunsetsuend[cur_bnst_ ];
    c_b->t_b_st = dai_top(bun_data_, cur_bnst_);	/* add by KUWA */
    c_b->t_b_end = dai_end(bun_data_, cur_bnst_);	/* add by KUWA */
    if(make_string_for_ke(cur_bnst_,buf,sizeof(buf)) == -1){
	print_msg_getc("Error in inspect", NULL, NULL, NULL);
	return(NULL);
    }

    return((unsigned char *)c);
}
