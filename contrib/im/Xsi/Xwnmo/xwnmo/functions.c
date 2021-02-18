/*
 * $Id: functions.c,v 1.3 1991/09/16 21:36:48 ohm Exp $
 */
/*
 * Copyright 1991 by OMRON Corporation
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

#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#ifdef	XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#else	/* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#endif	/* XJUTIL */

int
t_redraw_move_normal(x , start , end,clr_l)
int x, start, end, clr_l;
{
    c_b->t_c_p = x;
    t_print_line(start, end,clr_l);
    throw(cur_ichi(c_b->t_c_p , 0));
    return(0);
}

int
t_redraw_move_yincod(x , start , end,clr_l)
int x, start, end, clr_l;
{
    c_b->t_c_p = x;
    t_print_line(start, end,clr_l);
    if ( c_b->t_c_p == c_b->maxlen)
      kk_cursor_normal();
    else
      kk_cursor_invisible();
    throw(cur_ichi(c_b->t_c_p , 0));
    return(0);
}

int
t_print_l_normal()
{
    clr_line_all();
    t_redraw_one_line();
    throw(cur_ichi(c_b->t_c_p , 0));
    return(0);
}

int
t_print_l_yincod()
{
    clr_line_all();
    t_redraw_move_yincod(c_b->t_c_p,0,c_b->maxlen,1);
    return(0);
}

int
call_t_redraw_move_normal(x, start, end, clt_l, add)
int x, start, end, clt_l, add;
{
    t_redraw_move(x, start, end, clt_l);
    return(0);
}

int
call_t_redraw_move_yincod(x, start, end, clt_l, add)
int x, start, end, clt_l, add;
{
    int hantentmp;
    int marktmp;

   if (insert_modep()) {
	hantentmp = c_b->hanten;
	marktmp = c_b->t_m_start;
        c_b->hanten = 0x20 | 0x02 | 0x40 | 0x04;
        c_b->t_b_st = x;
        c_b->t_b_end = x + add;
        c_b->t_m_start = x + add;
        t_redraw_move_yincod(x, start, end, clt_l);
        c_b->hanten = hantentmp;
        c_b->t_m_start = marktmp;
    } else {
	t_redraw_move(x, start, end, clt_l);
    }
    return(0);
}

int
call_t_redraw_move(x, start, end, clt_l, add)
int x, start, end, clt_l, add;
{
    (*call_t_redraw_move_func)(x, start, end, clt_l, add);
    return(0);
}

int
call_t_redraw_move_1_normal(x, start, end, clt_l, add1, add2)
int x, start, end, clt_l, add1, add2;
{
    t_move(x + add2);
    return(0);
}

int
call_t_redraw_move_1_yincod(x, start, end, clt_l, add1, add2)
int x, start, end, clt_l, add1, add2;
{
    int hantentmp;
    int marktmp;

    if (insert_modep()) {
	hantentmp = c_b->hanten;
	marktmp = c_b->t_m_start;
        c_b->hanten = 0x20 | 0x02 | 0x40 | 0x04;
        c_b->t_b_st = x;
        c_b->t_b_end = x + add1;
        c_b->t_m_start = x + add1;
        t_redraw_move_yincod(x, start, end, clt_l);
        c_b->hanten = hantentmp;
        c_b->t_m_start = marktmp;
    } else {
	t_move(x + add2);
    }
    return(0);
}

int
call_t_redraw_move_1(x, start, end, clt_l, add1, add2)
int x, start, end, clt_l, add1, add2;
{
    (*call_t_redraw_move_1_func)(x, start, end, clt_l, add1, add2);
    return(0);
}

int
call_t_redraw_move_2_normal(x, start1, start2, end1, end2, clt_l, add)
int x, start1, start2, end1, end2, clt_l, add;
{
    t_move(x);
    return(0);
}

int
call_t_redraw_move_2_yincod(x, start1, start2, end1, end2, clt_l, add)
int x, start1, start2, end1, end2, clt_l, add;
{
    int hantentmp;
    int marktmp;
#ifdef	XJUTIL
    extern int touroku_mode;

    if (touroku_mode != KANJI_IN_END) {
#endif	/* XJUTIL */
	hantentmp = c_b->hanten;
	marktmp = c_b->t_m_start;
        c_b->hanten = 0x20 | 0x02 | 0x40 | 0x04;
        c_b->t_b_st = x;
        c_b->t_b_end = x + add;
        c_b->t_m_start = x + add;
        t_redraw_move_yincod(x, start1, end1, clt_l);
        c_b->hanten = hantentmp;
        c_b->t_m_start = marktmp;
#ifdef	XJUTIL
    } else {
	t_redraw_move_yincod(x, start2, end2, clt_l);
    }
#endif	/* XJUTIL */
    return(0);
}

int
call_t_redraw_move_2(x, start1, start2, end1, end2, clt_l, add)
int x, start1, start2, end1, end2, clt_l, add;
{
    (*call_t_redraw_move_2_func)(x, start1, start2, end1, end2, clt_l, add);
    return(0);
}

int
call_t_print_l_normal(x, add)
int x, add;
{
    t_print_l();
    return(0);
}

int
call_t_print_l_yincod(x, add)
int x, add;
{
    int hantentmp;
    int marktmp;

    hantentmp = c_b->hanten;
    marktmp = c_b->t_m_start;
    c_b->hanten = 0x20 | 0x02 | 0x40 | 0x04;
    c_b->t_b_st = x;
    c_b->t_b_end = x + add;
    c_b->t_m_start = x + add;
    t_print_l();
    c_b->hanten = hantentmp;
    c_b->t_m_start = marktmp;
    return(0);
}

int
call_t_print_l(x, add)
int x, add;
{
    (*call_t_print_l_func)(x, add);
    return(0);
}

int
input_yincod(s1, s2)
unsigned int *s1, *s2;
{
    char env_s = env_state();
    int which = 0;

    if (env_s == 'P') {
	which = CWNN_PINYIN;
    } else if (env_s == 'Z') {
	which = CWNN_ZHUYIN;
    } else {
	return(0);
    }
    return(cwnn_pzy_yincod(s1, s2, which));
}

int
redraw_when_chmsig_yincod()
{
    int hantentmp;
    int marktmp;
    char  env_s;

    if (env_s = env_state()) {
	set_cur_env(env_s);
    }
    if (insert_modep()) {
	hantentmp = c_b->hanten;
	marktmp = c_b->t_m_start;
	c_b->hanten = 0x40 | 0x04 |0x20 | 0x02;
	c_b->t_b_st = c_b->t_c_p;
	c_b->t_b_end = c_b->t_c_p + 1;
	c_b->t_m_start = c_b->t_c_p + 1;
	redraw_line();
	c_b->hanten = hantentmp;
	c_b->t_m_start = marktmp;
    } else {
	redraw_line();
    }
    return(0);
}

int
c_top_normal()
{
    if(c_b->t_m_start < 0){
	t_move(0);
    }else{
	t_move(c_b->t_m_start);
    }
    return(0);
}


int
c_top_yincod()
{
    int hantentmp = c_b->hanten;
    int marktmp = c_b->t_m_start;
#ifdef	XJUTIL
    extern int touroku_mode;

    if (touroku_mode != KANJI_IN_END) {
#endif	/* XJUTIL */
    c_b->hanten = 0x02 | 0x20 | 0x04 | 0x40;
    if (c_b->t_m_start < 0) {
	c_b->t_b_st = 0 ;
	c_b->t_b_end = 1;
	c_b->t_m_start = 1;
	t_redraw_move_yincod(0,0,c_b->maxlen,1);
    } else {
	c_b->t_b_st = c_b->t_m_start ;
	c_b->t_b_end = c_b->t_m_start + 1;
	c_b->t_m_start = c_b->t_m_start + 1;
	t_redraw_move_yincod(c_b->t_b_st,c_b->t_b_st,c_b->maxlen,1);
    }
    c_b->hanten = hantentmp;
    c_b->t_m_start = marktmp;
#ifdef	XJUTIL
    } else {
	t_redraw_move_yincod(c_b->t_m_start, c_b->t_m_start, c_b->maxlen,0);
    }
#endif	/* XJUTIL */
    return(0);
}

int
c_end_normal()
{
    t_move(c_b->maxlen);
    return(0);
}

int
c_end_yincod()
{
    int hantentmp = c_b->hanten;
    int marktmp = c_b->t_m_start;
#ifdef	XJUTIL
    extern int touroku_mode;

    if (touroku_mode != KANJI_IN_END) {
#endif	/* XJUTIL */
    c_b->hanten = 0x02 | 0x20 | 0x04 | 0x40;
    c_b->t_b_st = c_b->maxlen ;
     c_b->t_b_end = c_b->maxlen;
     c_b->t_m_start = c_b->maxlen;
     t_redraw_move_yincod(c_b->maxlen,c_b->t_c_p,c_b->maxlen,1);
     kk_cursor_normal();
     c_b->hanten = hantentmp;
     c_b->t_m_start = marktmp;
#ifdef	XJUTIL
    } else {
	t_redraw_move_yincod(c_b->maxlen, c_b->t_m_start, c_b->maxlen, 0);
    }
#endif	/* XJUTIL */
    return(0);
}

int
c_end_nobi_normal()
{
    t_move(c_b->maxlen);
    return(0);
}

int
c_end_nobi_yincod()
{
    t_redraw_move_yincod(c_b->maxlen, c_b->t_m_start, c_b->maxlen, 0);
    return(0);
}

int
print_out_yincod(s1, s2, len)
w_char *s1, *s2;
int len;
{
    return(cwnn_yincod_pzy_str(s1, s2, len,
		((env_state() == 'P') ? CWNN_PINYIN : CWNN_ZHUYIN)));
}

int char_q_len_normal(x)
w_char x;
{
    return((hankaku(x))? 1 : 2);
}

int char_q_len_yincod(x)
w_char x;
{
    if (((int)(x & 0x00ff) > 0xa0) && ((int)(x & 0x00ff) < 0xff) &&
	((int)(x >> 8) > 0xa0) && ((int)(x >> 8) < 0xff) )
	return(2);
    else
	return(1);
}

int
char_len_normal(x)
w_char x;
{
    return(char_q_len_normal(x));
}

int
char_len_yincod(x)
w_char x;
{
    w_char tmp_wch[10];
    int len, i, ret_col = 0;

    len = (*print_out_func)(tmp_wch, &x, 1);
    for (i = 0; i < len; i++) {
	ret_col += char_q_len_yincod(tmp_wch[i]);
    }
    return(ret_col);
}
