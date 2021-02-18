/*
 * $Id: jikouho.c,v 1.2 1991/09/16 21:36:59 ohm Exp $
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
#include "commonhd.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "rk_spclval.h" /* defines of CHMSIG, NISEBP */
#include "ext.h"


/* add by KUWA */
int
dai_top(buf, bnst)	
struct wnn_buf *buf;
int bnst;
{
    if(bnst >= jl_bun_suu(buf)) return(jl_bun_suu(buf));
    for(;bnst >= 0 && !jl_dai_top(buf, bnst); bnst--);
    return(bnst);
}

int
dai_end(buf, bnst)
struct wnn_buf *buf;
int bnst;
{
    for(bnst++;bnst < jl_bun_suu(buf) && !jl_dai_top(buf, bnst); bnst++);
    return(bnst);
}

void
set_escape_code(x)
char *x;
{
    int m;
    char buf[512];
    char *tmp;

    for(m = 0,tmp = x;*tmp;tmp++) {
	if (ESCAPE_CHAR((*tmp & 0xff))) {
	    buf[m++] = '^';
	    if(*tmp == 0x7f)buf[m++] = '?';
	    else buf[m++] = (*tmp + 'A' - 1);
	} else {
	    buf[m++] = *tmp;
	}
    }
    buf[m] = '\0';
    strcpy(x, buf);
}

/* 次候補を一つずつ表示する関数
num = 1 のとき次候補を、num = -1 のとき前候補を表示する */
static int
jikouho(num)
int num;
{
    int tmp;
    int mae_p;
    /* 注目している文節の次候補がまだ用意されていない時は
       まずそれを用意する。*/
    jl_env_set(bun_data_, bunsetsu_env[cur_bnst_]);
    if (bunsetsu_env[cur_bnst_] == bunsetsu_env[cur_bnst_+1])
	mae_p = WNN_USE_ZENGO;
    else
	mae_p = WNN_USE_MAE;
    if (jl_zenkouho_bun(bun_data_) != cur_bnst_ || jl_zenkouho_daip(bun_data_)){
	if (jl_zenkouho(bun_data_, cur_bnst_, mae_p, WNN_UNIQ) < 0) {
	    errorkeyin();
	    return(-1);
	}
    }

    if (num == 1) 
	jl_next(bun_data_);
    else if (num == -1) 
	jl_previous(bun_data_);
    else 
	return(-1);
    make_kanji_buffer(cur_bnst_);

    tmp = c_b->t_b_st;
    c_b->t_m_start = bunsetsuend[cur_bnst_ ];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    t_redraw_move(bunsetsu[cur_bnst_], MIN_VAL(tmp, c_b->t_b_st),
		  c_b->maxlen,1);
    check_scroll();
    return(0);
}

/* 次候補を一つずつ表示する関数
num = 1 のとき次候補を、num = -1 のとき前候補を表示する */
static int
jikouho_dai(num)
int num;
{
    int tmp;
    int mae_p;
    /* 注目している文節の次候補がまだ用意されていない時は
       まずそれを用意する。*/
    jl_env_set(bun_data_, bunsetsu_env[cur_bnst_]);
    if (bunsetsu_env[cur_bnst_] == bunsetsu_env[dai_end(bun_data_, cur_bnst_)])
	mae_p = WNN_USE_ZENGO;
    else
	mae_p = WNN_USE_MAE;
    if(jl_zenkouho_bun(bun_data_) != cur_bnst_ || !jl_zenkouho_daip(bun_data_)){
	if (jl_zenkouho_dai(bun_data_, cur_bnst_, dai_end(bun_data_, cur_bnst_),
			    mae_p, WNN_UNIQ) < 0) {
	   errorkeyin();
	   return(-1);
	}
    }

    if (num == 1) 
	jl_next_dai(bun_data_);
    else if (num == -1) 
	jl_previous_dai(bun_data_);
    else 
	return(-1);
    make_kanji_buffer(cur_bnst_);
    for (tmp = cur_bnst_+1; tmp < jl_bun_suu(bun_data_); tmp++)
	bunsetsu_env[tmp] = jl_env_get(bun_data_);
    tmp = c_b->t_b_st;
    c_b->t_m_start = bunsetsuend[cur_bnst_ ];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    t_redraw_move(bunsetsu[cur_bnst_], MIN_VAL(tmp, c_b->t_b_st),
		  c_b->maxlen,1);
    check_scroll();
    return(0);
}
    
    

int
jikouho_c()
{
    jikouho(1);
    return(0);
}

int
zenkouho_c()
{
    jikouho(-1);
    return(0);
}

int
jikouho_dai_c()
{
    jikouho_dai(1);
    return(0);
}

int
zenkouho_dai_c()
{
    jikouho_dai(-1);
    return(0);
}

static int
make_jikouho_retu(h,h_m,r,r_m)
char *h;
int h_m;
int r_m;
char *r[];
{
    int k;
    char *c = h;
    w_char tmp[512];
    char tmp1[512];

    if(r_m <= jl_zenkouho_suu(bun_data_)) return(-1);

    for(k = 0 ; k < jl_zenkouho_suu(bun_data_); k++){
	r[k] = c;
	tmp[0] = 0;
	jl_get_zenkouho_kanji(bun_data_, k, tmp);
	if (print_out_func) (*print_out_func)(tmp, tmp, Strlen(tmp));
	sStrcpy(tmp1, tmp);
	if(c + strlen(tmp1) + 1 >= h_m + h){
	    return(-1);
	}
	set_escape_code(tmp1);	/* add by KUWA */
	strcpy(c,tmp1);
	c += strlen(tmp1) + 1;
    }
    return(0);
}
  
static int
select_jikouho1(daip, in)
int daip;
int in;
{
    static char *j_retu[MAXJIKOUHO];
    static char jikouho_heap[MAXJIKOUHO * 10];
    int c;
    int tmp;
    static int jikouho_step = 0;
    static WnnClientRec *c_c_sv = 0;
    static char *lang;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return(-1);
    }
    if (jikouho_step == 0) {
    jl_env_set(bun_data_, bunsetsu_env[cur_bnst_]);
    if (jl_zenkouho_bun(bun_data_) != cur_bnst_  ||
        jl_zenkouho_daip(bun_data_) != daip ){
        if(daip){
  	    if(jl_zenkouho_dai(bun_data_, cur_bnst_,
			       dai_end(bun_data_, cur_bnst_), WNN_USE_ZENGO,
			       WNN_UNIQ) == -1) {
		errorkeyin();
		return(-1);
	    }
        }else{
  	    if(jl_zenkouho(bun_data_, cur_bnst_, WNN_USE_ZENGO,
			   WNN_UNIQ) == -1) {
		errorkeyin();
		return(-1);
	    }
        }
    }
    lang = cur_lang;
    if(make_jikouho_retu(jikouho_heap, MAXJIKOUHO * 10, j_retu,
			 MAXJIKOUHO) == -1){
	print_msg_getc("%s",msg_get(cd, 3, default_message[3], lang),
		       NULL, NULL);
	return(-1);
    }
	c_c_sv = c_c;
	jikouho_step++;
    }

    if((c = xw_select_one_jikouho(j_retu,jl_zenkouho_suu(bun_data_), -1,
				  jl_c_zenkouho(bun_data_),
				  (daip ? msg_get(cd, 2, default_message[2], lang) :
				   msg_get(cd, 1, default_message[1], lang))
				  ,JIKOUHO, main_table[4], in)) == -1){
	jikouho_step = 0;
	c_c_sv = 0;
	return(-1);
    } else if (c == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    }
    if(daip == WNN_SHO){
	jl_set_jikouho(bun_data_, c);
    }else{
	jl_set_jikouho_dai(bun_data_, c);
    }
    make_kanji_buffer(cur_bnst_);
    for (tmp = cur_bnst_+1; tmp < jl_bun_suu(bun_data_); tmp++)
	bunsetsu_env[tmp] = jl_env_get(bun_data_);
    c_b->t_m_start = bunsetsuend[cur_bnst_ ];
    c_b->t_c_p = bunsetsu[cur_bnst_];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    jikouho_step = 0;
    c_c_sv = 0;
    return(0);
}

int
select_jikouho(in)
int in;
{
    int ret;
    static WnnClientRec *c_c_sv = 0;

#ifdef	CALLBACKS
    if (IsPreeditCallbacks(cur_x) && cur_x->cb_redraw_needed) {
        redraw_line();
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
	push_func(c_c, select_jikouho);
    }
    if((ret = select_jikouho1(WNN_SHO, in)) == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    } else if (ret == -1){
	if (cur_p->cur_xl->visible == 3) {
	    t_print_l();
	}
    }else{
#ifdef	CALLBACKS
	if (IsPreeditCallbacks(cur_x)) {
            cur_x->cb_redraw_needed = 1;
            SendCBRedraw();
            return(BUFFER_IN_CONT);
        } else {
#endif	/* CALLBACKS */
	kk_cursor_invisible();
	redraw_line();
	kk_cursor_normal();
#ifdef	CALLBACKS
        }
#endif	/* CALLBACKS */
    }
    c_c_sv = 0;
    pop_func(c_c);
    check_scroll();
    return(0);
}

int
select_jikouho_dai(in)
int in;
{
    int ret;
    static WnnClientRec *c_c_sv = 0;

#ifdef	CALLBACKS
    if (IsPreeditCallbacks(cur_x) && cur_x->cb_redraw_needed) {
        redraw_line();
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
	push_func(c_c, select_jikouho_dai);
    }
    if((ret = select_jikouho1(WNN_DAI, in)) == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    } else if (ret == -1){
	if (cur_p->cur_xl->visible == 3) {
	    t_print_l();
	}
    }else{
#ifdef	CALLBACKS
        if (IsPreeditCallbacks(cur_x)) {
            cur_x->cb_redraw_needed = 1;
            SendCBRedraw();
            return(BUFFER_IN_CONT);
        } else {
#endif	/* CALLBACKS */
	kk_cursor_invisible();
	redraw_line();
	kk_cursor_normal();
#ifdef	CALLBACKS
        }
#endif	/* CALLBACKS */
    }
    c_c_sv = 0;
    pop_func(c_c);
    check_scroll();
    return(0);
}

#ifdef	nodef
#ifdef	CONVERT_by_STROKE
static int
make_jikouho_retu_B(h,h_m,r,r_m)
char *h;
int h_m;
int r_m;
char *r[];
{
    int k;
    char *c = h;
    w_char tmp[512];
    char *tmp1 = (char *)tmp;
    char *tmp2 = (char *)tmp;

    if(r_m <= jl_zenkouho_suu(bun_data_)) return(-1);

    for(k = 0 ; k < jl_zenkouho_suu(bun_data_); k++){
	r[k] = c;
	tmp[0] = 0;
	jl_get_zenkouho_kanji(bun_data_, k, tmp);

	sStrcpy(tmp1, tmp);
	for(;*tmp2;tmp2++);
	jl_get_zenkouho_yomi(bun_data_, k, tmp2);

	sStrcpy(tmp2, tmp2);
	tmp2 = (char *)tmp;
	if(c + strlen(tmp1) + 1 >= h_m + h){
	    return(-1);
	}
	set_escape_code(tmp1);
	strcpy(c,tmp1);
	c += strlen(tmp1) + 1;
    }
    return(0);
}

static int
select_question1(in)
int in;
{
    char *q_retu[MAXJIKOUHO];
    char question_heap[MAXJIKOUHO * 10];
    int c;
    int tmp;
    static int question_step = 0;
    static WnnClientRec *c_c_sv = 0;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return(-1);
    }
    if (question_step == 0) {
	if (jl_zenkouho_bun(bun_data_) != cur_bnst_  ||
	    jl_zenkouho_daip(bun_data_) != WNN_SHO) {
	    jl_env_set(bun_data_, bunsetsu_env[cur_bnst_]);
	    if(jl_zenkouho_q(bun_data_, cur_bnst_, WNN_USE_ZENGO,
			     WNN_UNIQ) == -1) {
	       errorkeyin();
	       t_print_l();
	       return(-1);
	    }
	}
	if(make_jikouho_retu_B(question_heap, MAXJIKOUHO * 10, q_retu,
			       MAXJIKOUHO) == -1){
	    print_msg_getc("%s", "湊奈逗�〆邁耕�謹　肝作温音嬬函竃。(奕担一?)",
			   NULL, NULL);
	    return(-1);
	}
	c_c_sv = c_c;
	question_step++;
    }

    if ((c = xw_select_one_jikouho(q_retu,jl_zenkouho_suu(bun_data_), -1,
				  jl_c_zenkouho(bun_data_),
				  (char *)"肝作温僉夲",
				  JIKOUHO,main_table[4], in)) == -1){
	question_step = 0;
	c_c_sv = 0;
	return(-1);
    } else if (c == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    }
    jl_set_jikouho(bun_data_, c);
    make_kanji_buffer(cur_bnst_);
    for (tmp = cur_bnst_+1; tmp < jl_bun_suu(bun_data_); tmp++)
	bunsetsu_env[tmp] = jl_env_get(bun_data_);
    c_b->t_m_start = bunsetsuend[cur_bnst_ ];
    c_b->t_c_p = bunsetsu[cur_bnst_];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    question_step = 0;
    c_c_sv = 0;
    return(0);
}

int
select_question(in)
int in;
{
    if(select_question1(in) == -1){
	t_print_l();
    }else{
	redraw_line();
    }
    return(0);
}

#endif CONVERT_by_STROKE
#endif

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/
