/*
 * $Id: uif.c,v 1.4 1991/09/16 21:37:25 ohm Exp $
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
#include "config.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"

/*
static int henkan_mode;
*/
/*  0: select
    1: yomi_nyuuryoku
    2: bunsetu_nobasi
    3: kara
*/

static void
not_to_han_or_hira()
{
    cur_bnst_sv = -1;
    save_yomi[0] = 0;
}

static void
recover_to_han_or_hira(yomi)
register w_char *yomi;
{
    int i;
    if (cur_bnst_sv == cur_bnst_) {
	Strcpy(yomi, save_yomi);
    } else if (cur_bnst_sv >= 0 && cur_bnst_sv < cur_bnst_) {
	i = jl_yomi_len(bun_data_, cur_bnst_sv, cur_bnst_);
	Strcpy(yomi, save_yomi + i);
    } else {
        jl_get_yomi(bun_data_, cur_bnst_, cur_bnst_sv, yomi);
	yomi[jl_yomi_len(bun_data_, cur_bnst_, cur_bnst_sv)] = 0;
	Strcat(yomi, save_yomi);
    }
}

void
push_func(w_c, func)
WnnClientRec *w_c;
int (*func)();
{
    if (w_c->func_stack_count >= (MAX_FUNC_STACK - 1)) return;
    w_c->func_stack_count++;
    w_c->command_func_stack[w_c->func_stack_count] = func;
}

void
pop_func(w_c)
WnnClientRec *w_c;
{
    if (w_c->func_stack_count < 0) return;
    w_c->command_func_stack[w_c->func_stack_count] = (int (*)())0;
    w_c->func_stack_count--;
}

static int
push_char_return(c,romkan)
int c,romkan;
{
    push_unget_buf(romkan);
    kakutei();
    return(1);
}

static void
change_to_henkango_mode()
{
    c_b->key_table = main_table[0];
    c_b->rk_clear_tbl = romkan_clear_tbl[0];
    c_b->key_in_fun = push_char_return;
    c_b->ctrl_code_fun = (int (*)())0;
    c_b->hanten = 0x04 | 0x40;
    henkan_mode = 0;
    kk_cursor_invisible();
}

static void
change_to_insert_mode()
{
    c_b->key_table = main_table[1];
    c_b->rk_clear_tbl = romkan_clear_tbl[1];
    c_b->key_in_fun = (int (*)())0;
    c_b->ctrl_code_fun = (int (*)())0;
    c_b->hanten = 0x08 | 0x20;
    henkan_mode = 1;
    kk_cursor_normal();
}

static int
ignore_it_and_clear_romkan()
{
    romkan_clear();
    return(0);
}

static void
change_to_nobasi_tijimi_mode()
{
    c_b->key_table = main_table[2];
    c_b->rk_clear_tbl = romkan_clear_tbl[2];
    c_b->key_in_fun = ignore_it_and_clear_romkan;
    c_b->ctrl_code_fun = (int (*)())0;
    c_b->hanten = 0x04;
    henkan_mode = 2;
    kk_cursor_invisible();
}

int
insert_char_and_change_to_insert_mode(c)
int c;
{
    insert_char(c);
    t_redraw_move(c_b->t_c_p + 1 ,c_b->t_c_p, c_b->maxlen,0);
    change_to_insert_mode();
    if (send_ascii_char_quote == 1 && send_ascii_char == 0){
	send_ascii_char = 1;
	send_ascii_char_quote = 0;
    }
    check_scroll();
    return(0);
}

int
insert_modep()
{
    return(henkan_mode == 1);
}

int
empty_modep()
{
    return(henkan_mode == 3);
}

static int
nobasi_tijimi_mode()
{
    return(henkan_mode == 2);
}

int
kakutei()
{
    int moji_suu = 0;
    w_char *w = 0;
    w_char yomi[512];
     /* conversion nashi de kakutei surubaai wo kouryo seyo. */

    remember_buf[0] = 0;
    if (jl_bun_suu(bun_data_) != 0) {
	jl_get_yomi(bun_data_, 0, -1, remember_buf);
    }
    if(insert_modep()){
	moji_suu =  c_b->maxlen - c_b->t_m_start;
	if(moji_suu > 0){
	    w = remember_buf + Strlen(remember_buf);
	    Strncat(w,c_b->buffer + c_b->t_m_start, moji_suu);
	}
    }else if(nobasi_tijimi_mode()){
	moji_suu = jl_get_yomi(bun_data_, cur_bnst_, -1, yomi);
	Strcpy(c_b->buffer + c_b->t_c_p ,yomi);
	c_b->maxlen = Strlen(c_b->buffer);
	jl_kill(bun_data_, cur_bnst_, -1);
    }
    c_b->buffer[0] = 0;
    if (jl_bun_suu(bun_data_) != 0) {
	jl_get_kanji(bun_data_, 0, -1, c_b->buffer);
	if(jl_update_hindo(bun_data_, 0, -1) == -1){
	    errorkeyin();
	    redraw_line();
	}
    } 
    if (insert_modep() && moji_suu > 0 && w) {
	    Strcat(c_b->buffer,w);
    }else if(nobasi_tijimi_mode() && moji_suu > 0){
	Strcat(c_b->buffer,yomi);
    }
    c_b->maxlen = Strlen(c_b->buffer);

    throw_col(-1);
    clr_line_all();
    return(1);
}

int
return_it(c, romkan)
int c, romkan;
{
    return_cl_it();
    return(0);
}

int
return_it_if_ascii(c , nisemono)
int c;
int nisemono;  /* it means that what romkan returned is a nisemono character. */
{
    if((NORMAL_CHAR(c) || c == 0x09) && is_HON(nisemono)){
	c_b->maxlen = 1;
	c_b->buffer[0] = c;
	return(1);
    }else{
	insert_char_and_change_to_insert_mode(c);
	return(0);
    }
}

static void
change_to_empty_mode()
{
    c_b->key_table = main_table[3];
    c_b->rk_clear_tbl = romkan_clear_tbl[3];
    if (send_ascii_char) {
	c_b->key_in_fun = return_it_if_ascii;
    } else {
	c_b->key_in_fun = insert_char_and_change_to_insert_mode;
    }
    c_b->ctrl_code_fun = return_it;
    c_b->hanten = 0x08 | 0x20;
    henkan_mode = 3;
    throw_col(0);
    kk_cursor_normal();
}

int
redraw_nisemono_c()
{
    redraw_nisemono();
    if(c_b->maxlen == 0){
	change_to_empty_mode();
    }
    return(0);
}

int
kk(c)
int c;
{
    int len;

    if (buffer_in(c) == -1)
	return(-1);
    pop_func(c_c);
    if (print_out_func) {
	len = (*print_out_func)(return_buf , c_b->buffer , c_b->maxlen);
    } else {
	Strncpy(return_buf , c_b->buffer , c_b->maxlen);
	len = c_b->maxlen;
    }
    clear_c_b();
    /***
    romkan_clear();
    ***/
    return(len);
}

static void
initialize_vars()
{
    cur_bnst_ = 0;
}

void
clear_c_b()
{
    initialize_vars();
    if (bun_data_) jl_kill(bun_data_, 0, -1);
    c_b->buffer = input_buffer;
    c_b->t_c_p = 0;
    c_b->maxlen = 0;
    c_b->buflen = maxchg;
    c_b->redraw_fun = redraw_nisemono_c;
    c_b->t_m_start = 0;
    c_b->t_b_st = 0;
    c_b->t_b_end = 0;
    change_to_empty_mode();
    init_screen();
}

void
make_kanji_buffer(bnst)
int bnst;
{
    int k , l;
    w_char *bp;
    w_char *buffer_end;

    buffer_end = c_b->buffer + c_b->buflen - 1;
    if(bnst) bp = c_b->buffer + bunsetsuend[bnst - 1];
    else bp = c_b->buffer;
    for(k = bnst ; k < jl_bun_suu(bun_data_) ; k++) {
	if (k < maxbunsetsu) bunsetsu[k] = bp - c_b->buffer;
	l = jl_get_kanji(bun_data_, k, k+1, bp);
	bp += l;
	if (k < maxbunsetsu) bunsetsuend[k] = bp - c_b->buffer;
	if (bp > buffer_end) {
	    c_b->maxlen = bp - c_b->buffer;
	}
    }
    if (k < maxbunsetsu) {
	bunsetsu[k++] = bp - c_b->buffer;
	bunsetsuend[k++] = bp - c_b->buffer;
    } else {
	bunsetsu[maxbunsetsu - 1] = bp - c_b->buffer;
	bunsetsuend[maxbunsetsu - 1] = bp - c_b->buffer;
    }
    c_b->maxlen = bp - c_b->buffer;
}


int
isconect_jserver()
{
    if (c_c->use_server == 0) return(0);
    if (!jl_isconnect(bun_data_)) {
#ifdef	USING_XJUTIL
	kill_lang_xjutil(cur_lang);
#endif	/* USING_XJUTIL */
	connect_server(cur_lang);
    }
    if (c_c->use_server && !jl_isconnect(bun_data_)) {
	print_msg_getc(" %s", msg_get(cd, 27, default_message[27], cur_lang),
		       NULL, NULL);
	return (0);
    }
    return(1);
}

static int
ren_henkan0()
{
    w_char yomi[256];
    int moji_suu;
    register int i;

    if (!isconect_jserver()) {
	return(0);
    }

    c_b->buflen = maxchg;
    moji_suu =  c_b->maxlen - c_b->t_m_start;
    if(moji_suu > 0) {
	int ret;
	Strncpy(yomi, c_b->buffer + c_b->t_m_start, moji_suu);
	yomi[moji_suu] = 0;
	ret = jl_ren_conv(bun_data_, yomi, cur_bnst_, -1, WNN_USE_MAE );
	for (i = cur_bnst_; i < jl_bun_suu(bun_data_); i++)
	    bunsetsu_env[i] = jl_env_get(bun_data_);
	if(ret == -1){
	    errorkeyin();
	    t_print_l();
	    return(0);
	}
	make_kanji_buffer(0);
    change_to_henkango_mode();
    c_b->t_m_start = bunsetsuend[cur_bnst_ ];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    t_redraw_move(bunsetsu[cur_bnst_ ] ,bunsetsu[dai_top(bun_data_, cur_bnst_)],
		  c_b->maxlen,1);
    check_scroll();
    }
    return(0);
}

int
ren_henkan()
{
    jl_env_set(bun_data_, env_normal);
    return (ren_henkan0());
}

int
kankana_ren_henkan()
{
    if (*reverse_envrcname != '\0') {
	not_to_han_or_hira();
	jl_env_set(bun_data_, env_reverse);
	return (ren_henkan0());
    } else {
	ring_bell();
	return (0);
    }
}

static void
print_mes_dicconect_js()
{
    print_msg_getc(" %s", msg_get(cd, 27, default_message[27], cur_lang),
		   NULL, NULL);
}

#ifdef CONVERT_by_STROKE
int errorkaijo()
{
    w_char yomi[512];

    c_b->t_b_end = c_b->t_b_st;
    change_to_insert_mode();
    c_b->t_c_p = bunsetsu[cur_bnst_];
    c_b->t_m_start = c_b->t_c_p;
    if (jl_env(bun_data_) == env_normal)
	jl_get_yomi(bun_data_, cur_bnst_, -1, yomi);
    else if (jl_env(bun_data_) == env_reverse)
	jl_get_yomi(bun_data_, cur_bnst_, -1, yomi);
    else
	recover_to_han_or_hira(yomi);
    Strcpy(c_b->buffer + c_b->t_c_p ,yomi);

    c_b->buflen = maxchg;
    c_b->maxlen = Strlen(c_b->buffer);
    jl_kill(bun_data_, cur_bnst_, -1);
    return(0);
}
#endif /* CONVERT_by_STROKE */

static int
tan_conv(daip)
int daip;
{
    w_char yomi[512];
    int moji_suu;
    register int i;

    jl_env_set(bun_data_, env_normal);
    if (!isconect_jserver()) {
	return(0);
    }

    c_b->buflen = maxchg;
    moji_suu =  c_b->maxlen - c_b->t_m_start;
    if(moji_suu > 0){
	int ret;
	Strncpy(yomi, c_b->buffer + c_b->t_m_start, moji_suu);
	yomi[moji_suu] = 0;
	ret = jl_tan_conv(bun_data_, yomi, cur_bnst_, -1, WNN_USE_MAE, daip);
	for (i = cur_bnst_; i < dai_end(bun_data_, cur_bnst_); i++)
	    bunsetsu_env[i] = jl_env_get(bun_data_);
	if(ret == -1){
#ifdef	CONVERT_by_STROKE
	    errorkeyin_q();
	    errorkaijo();
	    errorkill_c();
#else
	    errorkeyin();
#endif	/* CONVERT_by_STROKE */
	    t_print_l();
	    return(0);
      }
      make_kanji_buffer(0);
    change_to_henkango_mode();
    c_b->t_m_start = bunsetsuend[cur_bnst_ ];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    t_redraw_move(bunsetsu[cur_bnst_ ] 
		  ,bunsetsu[dai_top(bun_data_, cur_bnst_)],c_b->maxlen,1);
    }
    return(0);
}

int
tan_henkan()
{
    jl_env_set(bun_data_, env_normal);
    return (tan_conv(WNN_SHO));
}

int
tan_henkan_dai()
{
    jl_env_set(bun_data_, env_reverse);
    return (tan_conv(WNN_DAI));
}

static int
tan_henkan1(daip, env)
struct wnn_env *env;
int daip;
{
    int ret;
    int moji_suu = c_b->t_c_p - c_b->t_m_start;
    register int i;

    if (c_c->use_server == 0) return(-1);
    if(!jl_isconnect(bun_data_)) {
	print_mes_dicconect_js();
	return(-1);
    }

    if(moji_suu == 0){
	return(-1);
    }
    bunsetsu_env[cur_bnst_] = jl_env_get(bun_data_);
    if(env == NULL) {
	ret = jl_nobi_conv(bun_data_, cur_bnst_, moji_suu, -1,
			   WNN_USE_MAE, daip);
    } else {
	ret = jl_nobi_conv_e2(bun_data_, env, cur_bnst_, moji_suu, -1,
			  WNN_USE_MAE, daip);
    }

    if (ret == -1) {
	errorkeyin();
	return(-1);
    }
    make_kanji_buffer(0);
    for (i = cur_bnst_+1; i < jl_bun_suu(bun_data_); i++)
	bunsetsu_env[i] = jl_env_get(bun_data_);
    change_to_henkango_mode();
    c_b->t_c_p = bunsetsu[cur_bnst_ ];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    return(0);
}

/* nobi_conv(daip) */
static int
nobi_conv(daip, env)
int daip;
struct wnn_env *env;
{
    int tmp;
    if (c_c->use_server == 0) return(0);
    if(!jl_isconnect(bun_data_)) {
	connect_server(cur_lang);
    }
    tmp = tmp = c_b->t_b_st;
    if(tan_henkan1(daip, env) == 0){
	c_b->t_m_start = bunsetsuend[cur_bnst_ ];
	t_redraw_move(bunsetsu[cur_bnst_ ] ,MIN_VAL(tmp,c_b->t_b_st),
		      c_b->maxlen,1);
	check_scroll();
    }
    return(0);
}

int
nobi_henkan()
{
    return (nobi_conv(WNN_SHO, (struct wnn_env *)NULL));
}

int
nobi_henkan_dai()
{
    return (nobi_conv(WNN_DAI, (struct wnn_env *)NULL));
}

void
henkan_if_maru(c)
int c;
{
    if (ISKUTENCODE(c) && c_b->key_table == main_table[1])
	ren_henkan();
}

#ifdef nodef
/** Convert by stroke : BWNN */
#ifdef CONVERT_by_STROKE
/** return Question Mark char */
static w_char
romkan_state()
{
     char *dspmod;
     dspmod = romkan_dispmode();
     return(strlen(dspmod) > WNN_DISP_MODE_LEN ?
		 *(dspmod + WNN_DISP_MODE_LEN) : 0);
}

static int
q_conv()
{
    w_char yomi[512];
    int moji_suu;
    register int i;

    jl_env_set(bun_data_, env_normal);
    if (!isconect_jserver()) {
	return(0);
    }

    c_b->buflen = maxchg;
    moji_suu =  c_b->maxlen - c_b->t_m_start;
    if(moji_suu > 0){
	int ret;
	w_char rk_state = (w_char)romkan_state();
	Strncpy(yomi, c_b->buffer + c_b->t_m_start, moji_suu);
	if(rk_state)
	    for(i = 0; i < moji_suu; i++)
		if(yomi[i] == rk_state) yomi[i] = Q_MARK;
	yomi[moji_suu] = 0;
	ret = jl_q_conv(bun_data_, yomi, cur_bnst_, -1, WNN_USE_MAE, WNN_SHO);
    	for (i = cur_bnst_; i < dai_end(bun_data_, cur_bnst_); i++)
	    bunsetsu_env[i] = jl_env_get(bun_data_);
	if(ret == -1){
	    errorkeyin_q();
	    errorkaijo();
	    errorkill_c();
	    t_print_l();
	    return(0);
      }
      make_kanji_buffer(0);
    change_to_henkango_mode();
    c_b->t_m_start = bunsetsuend[cur_bnst_ ];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    t_redraw_move(bunsetsu[cur_bnst_ ] 
		  ,bunsetsu[dai_top(bun_data_, cur_bnst_)],c_b->maxlen,1);
    }
    return(0);
}

int
question_henkan()
{
    jl_env_set(bun_data_, env_normal);
    return (q_conv());
}
#endif /* CONVERT_by_STROKE */
#endif

/*yank*/
int
yank_c()
{
    if(empty_modep()){
	change_to_insert_mode();
    }
    t_yank();
    return(0);
}

int
remember_me()
{
    if(jl_bun_suu(bun_data_) == 0){
	if(c_b->key_in_fun){
	    (*c_b->key_in_fun)(-1);
	    c_b->key_in_fun = (int (*)())0;
	}
	Strcpy(c_b->buffer ,remember_buf);
	c_b->maxlen = Strlen(remember_buf);
	call_t_redraw_move(0, 0, c_b->maxlen, 1, 1);
	check_scroll();
    }
    return(0);
}

int
kill_c()
{
    t_kill();
    if(c_b->maxlen == 0){
	change_to_empty_mode();
    }
    return(0);
}

#ifdef CONVERT_by_STROKE
int
errorkill_c()
{
    errort_kill();
    if(c_b->maxlen == 0){
	change_to_empty_mode();
    }
    return(0);
}
#endif /* CONVERT_by_STROKE */
int
delete_c(c,romkan)
int c,romkan;
{
    t_delete_char();
    if((c_b->maxlen == 0) && is_HON(romkan)){
	change_to_empty_mode();
    }
    return(0);
}

int
rubout_c(c , romkan)
int c, romkan;
{
    t_rubout(c, romkan);
    if((c_b->maxlen == 0) && is_HON(romkan)){
	change_to_empty_mode();
    }
    return(0);
}  

int
end_bunsetsu()
{
    int tmp = cur_bnst_;
    cur_bnst_ = jl_bun_suu(bun_data_) - 1;
    c_b->t_m_start = bunsetsuend[cur_bnst_ ];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    t_redraw_move(bunsetsu[cur_bnst_], bunsetsu[dai_top(bun_data_, tmp)],
		  bunsetsuend[cur_bnst_],0);
  check_scroll();
  return(0);
}

int
top_bunsetsu()
{
    int tmp = cur_bnst_;
    cur_bnst_ = 0;
    c_b->t_m_start = bunsetsuend[0];
    c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
    c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
    t_redraw_move(bunsetsu[0],0,bunsetsuend[dai_end(bun_data_, tmp) -1],0);
    check_scroll();
    return(0);
}

int
forward_bunsetsu()
{
    if(cur_bnst_ < jl_bun_suu(bun_data_) - 1) {
	cur_bnst_ += 1;
	c_b->t_m_start = bunsetsuend[cur_bnst_ ];
	c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
	c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
	t_redraw_move(bunsetsu[cur_bnst_ ],
		      bunsetsu[dai_top(bun_data_, cur_bnst_ - 1)],
		      bunsetsuend[dai_end(bun_data_, cur_bnst_) -1],0);
	check_scroll();
    }
    return(0);
}

int
backward_bunsetsu()
{
    if(cur_bnst_ > 0) {
	cur_bnst_ -= 1;
	c_b->t_m_start = bunsetsuend[cur_bnst_];
	c_b->t_b_st = bunsetsu[dai_top(bun_data_, cur_bnst_)];
	c_b->t_b_end = bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1];
	t_redraw_move(bunsetsu[cur_bnst_ ],
		      bunsetsu[dai_top(bun_data_, cur_bnst_)],
		      bunsetsuend[dai_end(bun_data_, cur_bnst_ + 1) - 1],0);
	check_scroll();
    }
    return(0);
}

int
kaijo()
{
    w_char yomi[512];

    c_b->t_b_end = c_b->t_b_st;
    t_redraw_move(bunsetsu[cur_bnst_],
		  bunsetsu[dai_top(bun_data_, cur_bnst_)],
		  bunsetsuend[dai_end(bun_data_, cur_bnst_) - 1],0);
    change_to_insert_mode();
    c_b->t_c_p = bunsetsu[cur_bnst_];
    c_b->t_m_start = c_b->t_c_p;
    if (jl_env(bun_data_) == env_normal)
	jl_get_yomi(bun_data_, cur_bnst_, -1, yomi);
    else if (jl_env(bun_data_) == env_reverse)
	jl_get_yomi(bun_data_, cur_bnst_, -1, yomi); /* kankana_ren_henkan() */
    else
	recover_to_han_or_hira(yomi);
    Strcpy(c_b->buffer + c_b->t_c_p ,yomi);
    c_b->buflen = maxchg;
    c_b->maxlen = Strlen(c_b->buffer);
    jl_kill(bun_data_, cur_bnst_, -1);
    call_t_redraw_move(c_b->t_c_p, c_b->t_c_p, c_b->maxlen, 1, 1);
    check_scroll();
    return(0);
}

int
enlarge_bunsetsu()
{
    w_char yomi[512];
    int area_len, len1;

    area_len = jl_yomi_len(bun_data_, cur_bnst_, dai_end(bun_data_, cur_bnst_));
    len1 = jl_get_yomi(bun_data_, cur_bnst_, -1, yomi);

    if(area_len < len1)
	area_len ++;

    change_to_nobasi_tijimi_mode();
    c_b->t_m_start = c_b->t_c_p;
    c_b->t_b_end = c_b->t_c_p;
    Strcpy(c_b->buffer + c_b->t_m_start ,yomi);
    c_b->maxlen = Strlen(c_b->buffer);
    t_redraw_move(c_b->t_m_start + area_len, c_b->t_m_start, c_b->maxlen,1);
    check_scroll();
    return(0);
}

int
smallen_bunsetsu()
{
    w_char yomi[512];
    int area_len;

    area_len = jl_yomi_len(bun_data_, cur_bnst_, dai_end(bun_data_, cur_bnst_));
    jl_get_yomi(bun_data_, cur_bnst_, -1, yomi);

    if (area_len > 1) area_len--;

    change_to_nobasi_tijimi_mode();
    c_b->t_m_start = c_b->t_c_p;
    c_b->t_b_end = c_b->t_c_p;
    Strcpy(c_b->buffer + c_b->t_m_start ,yomi);
    c_b->maxlen = Strlen(c_b->buffer);
    t_redraw_move(c_b->t_m_start + area_len, c_b->t_m_start , c_b->maxlen,1);
    check_scroll();
    return(0);
}


int
send_string(c)
int c;
{
    kakutei();
    c_b->buffer[c_b->maxlen] = c;
    c_b->maxlen += 1;
    c_b->t_c_p = c_b->maxlen;
    return(1);
}

int
tijime()
{
    if(c_b->t_c_p > c_b->t_m_start + 1){
	backward();
    }
    return(0);
}

int
jmptijime()
{
    call_t_redraw_move_1(c_b->t_m_start, c_b->t_m_start, c_b->maxlen, 1, 1, 1);
    backward();
    return(0);
}


int
henkan_forward()
{
    if (c_c->use_server == 0) return(0);
    if(!jl_isconnect(bun_data_)) {
	return(0);
    }

    if(tan_henkan1(WNN_DAI, (struct wnn_env *)NULL) == -1){
	return(0);
    }
    zenkouho_dai_c();
    forward_bunsetsu();
    return(0);
}

int
henkan_backward()
{
    if (c_c->use_server == 0) return(0);
    if(!jl_isconnect(bun_data_)) {
	return(0);
    }

    if (cur_bnst_ <= 0) {
	nobi_henkan_dai();
    } else {
    	if(tan_henkan1(WNN_DAI, (struct wnn_env *)NULL) == -1){
	    return(0);
    	}
    	zenkouho_dai_c();
    	backward_bunsetsu();
    }
    return(0);
}

int
backward_c()
{
    if(!isconect_jserver()) {
	return(0);
    }
    if(c_b->t_c_p == c_b->t_m_start){
	if(c_b->t_c_p != 0) {
	    ren_henkan0();
	    change_to_henkango_mode();
	    backward_bunsetsu();
	}
    }else{
	backward();
    }
    return(0);
}

int
insert_it_as_yomi()
{
    kakutei();
    change_to_insert_mode();
    c_b->t_m_start = 0;
    cur_bnst_ = 0;
    call_t_print_l(c_b->t_c_p, 1);
    if(bun_data_)
	jl_kill(bun_data_, 0, -1);
    return(0);
}

/****history *******/
int
previous_history()
{
    int k;
    if (jl_bun_suu(bun_data_) == 0) {
	k = previous_history1(c_b->buffer);
	if(k > 0){
	    change_to_insert_mode();
	    c_b->maxlen = k;
	    c_b->t_c_p = 0;
	    call_t_print_l(c_b->t_c_p, 1);
	}
    }
    return(0);
}

int
next_history()
{
    int k;
    if (jl_bun_suu(bun_data_) == 0) {
	k = next_history1(c_b->buffer);
	if(k > 0){
	    change_to_insert_mode();
	    c_b->maxlen = k;
	    c_b->t_c_p = 0;
	    call_t_print_l(c_b->t_c_p, 1);
	}
    }
    return(0);
}

/*
static int send_ascii_stack = 0;
*/

int
send_ascii()
{
    send_ascii_stack = send_ascii_char;
    send_ascii_char = 1;
    return(0);
}

int
not_send_ascii()
{
    send_ascii_stack = send_ascii_char;
    send_ascii_char = 0;
    return(0);
}

int
toggle_send_ascii()
{
    send_ascii_stack = send_ascii_char;
    if(send_ascii_char == 0){
	send_ascii_char = 1;
    }else{
	send_ascii_char = 0;
    }
    return(0);
}

int
pop_send_ascii()
{
    send_ascii_char = send_ascii_stack;
    return(0);
}
/*
int send_ascii_char_quote = 0;
*/
static void
quote_send_ascii()
{
    if(send_ascii_char == 1){
	send_ascii_char = 0;
	send_ascii_char_quote = 1;
    }
}

static void
check_empty_mode_keyin_fun()
{
    if(send_ascii_char){
	c_b->key_in_fun = return_it_if_ascii;
    }else{
	c_b->key_in_fun = insert_char_and_change_to_insert_mode;
    }
}

int
send_ascii_e()
{
    send_ascii();
    check_empty_mode_keyin_fun();
    return(0);
}

int
not_send_ascii_e()
{
    not_send_ascii();
    check_empty_mode_keyin_fun();
    return(0);
}

int
toggle_send_ascii_e()
{
    toggle_send_ascii();
    check_empty_mode_keyin_fun();
    return(0);
}

int
pop_send_ascii_e()
{
    pop_send_ascii();
    check_empty_mode_keyin_fun();
    return(0);
}

int
quote_send_ascii_e()
{
    quote_send_ascii();
    check_empty_mode_keyin_fun();
    return(0);
}

int
reconnect_server(in)
int in;
{
    static WnnClientRec *c_c_sv = 0;

    if (c_c->use_server == 0) return(0);
    if(!jl_isconnect_e(env_normal) ||
       env_reverse != 0 && !jl_isconnect_e(env_reverse)) {

	if (c_c_sv != 0 && c_c != c_c_sv) {
	    ring_bell();
	    return(0);
	}
	if (c_c_sv == 0) {
	    c_c_sv = c_c;
	    push_func(c_c, reconnect_server);
	}

	if (reconnect_jserver_body(in) == BUFFER_IN_CONT) {
	    return(BUFFER_IN_CONT);
	}
	c_c_sv = 0;
	pop_func(c_c);
    }
    return(0);
}

int
disconnect_server()
{
    if (c_c->use_server == 0) return(0);
    if (jl_isconnect_e(env_reverse)) {
	jl_dic_save_all_e(env_reverse);
    }
    if (jl_isconnect_e(env_reverse)) {
	jl_disconnect(env_reverse);
	env_reverse =0;
    }
    if (jl_isconnect_e(env_normal)) {
	jl_dic_save_all_e(env_normal);
    }
    if (jl_isconnect_e(env_normal)) {
	jl_disconnect(env_normal);
	env_normal =0;
    }
    return(1);
}

int
henkan_off(in)
int in;
{
    unsigned char c;
    static w_char wc[1];
    static int two_byte_first = 0;

    if (c_c->command_func_stack[c_c->func_stack_count] == (int (*)())0) {
	if (IsStatusArea(cur_p)
#ifdef	CALLBACKS
	    || IsStatusCallbacks(cur_x)
#endif	/* CALLBACKS */
	    ) {
	    display_henkan_off_mode();
	}
	invisual_window();
	henkan_off_flag = 1;
	push_func(c_c, henkan_off);
	return(BUFFER_IN_CONT);
    } else if (c_c->command_func_stack[c_c->func_stack_count] != henkan_off) {
	ring_bell();
	return(0);
    }

    
    for(;;) {
	c = (char)in;
	if ((!quote_flag) && (c == quote_code)) {
	    quote_flag = 1;
	    return(BUFFER_IN_CONT);
	}
	if ((quote_flag == 0) && (c_b->key_table[c] == henkan_off)){
	    break;
	}
        if (c == 0x8e || (c >= 0xa0 && c <= 0xfe)) {
	    if(two_byte_first == 1) {
		wc[0] += c;
		xw_write(wc, 1);
		two_byte_first = 0;
		quote_flag = 0;
	    } else {
		wc[0] = c * 0x100;
		two_byte_first = 1;
	    }
	} else {
	    wc[0] = c;
	    xw_write(wc, 1);
	    two_byte_first = 0;
	}
	quote_flag = 0;
	return(BUFFER_IN_CONT);
    }
    henkan_off_flag = 0;
    pop_func(c_c);
    disp_mode();
    if (!empty_modep()) {
	visual_window();
    }
    return(BUFFER_IN_CONT);
}

int
reset_c_b()
{
    if (c_c->func_stack_count >= 0) {
	ring_bell();
	return(-1);
    }
    clear_c_b();
    romkan_clear();
    return(0);
}

