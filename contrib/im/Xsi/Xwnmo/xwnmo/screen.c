/*
 * $Id: screen.c,v 1.4 1991/09/16 21:37:16 ohm Exp $
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
#ifdef	XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#else	/* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#endif	/* XJUTIL */
#include "rk_spclval.h"
#include "rk_fundecl.h"


static int col = 0; /** cursor position from startichi*/
static int oldmaxcol = 0; /** redraw sita tokini doko made mae ni kaita data ga nokotteiruka wo simesu */

#define CHAR_BYTE 0

#define w_putchar_move(x) {col += w_putchar(x);}
#define putchar_move(x) {col += 1; putchar_norm(x);}
#define putchar_move1(x) {col += 1; put_char1(x);}

#define set_screen_reverse(X,Y)	\
    {set_hanten_ul(c_b->hanten & X,c_b->hanten & Y);}

#define reset_screen_reverse(X,Y) \
    {flushw_buf(); if(c_b->hanten & X)h_r_off();if(c_b->hanten & Y)u_s_off();}

#define set_screen_bold(X) \
    {flushw_buf(); set_bold(c_b->hanten & X);}

#define reset_screen_bold(X) \
    {flushw_buf(); reset_bold(c_b->hanten & X);}

static char rk_modes[80];

void
throw(x)
int x;
{
    throw_col((col = x));
}

int
char_len(x)
w_char x;
{
    return((*char_len_func)(x));
}

/*return the position of charcter cp, when column 0 is start_point */
int
cur_ichi(cp , start_point)
register int cp;
int start_point;
{
    register int k;
    register int end_of_line = maxlength;
    register int len = 0;

    if(cp < start_point)return(-1);

    for(k = start_point ;k < cp ;k++ ){
	len += char_len(c_b->buffer[k]);
	if(len  > end_of_line - CHAR_BYTE){
	    len = end_of_line;
	    end_of_line += maxlength;
	}
    }	
    return(len);
}

void
t_print_line(st , end, clr_l) 
int st , end, clr_l;
{
    register int k;
    register int col1;
    register int end_of_line = maxlength;

    int mst = MIN_VAL(c_b->t_m_start , c_b->t_c_p);
    int mend = MAX_VAL(c_b->t_m_start , c_b->t_c_p);
  
    int bst = c_b->t_b_st;
    int bend = c_b->t_b_end;
    int bold = 0;

    if(end > c_b->maxlen) end = c_b->maxlen;
    col1 = cur_ichi(st , 0);
    if(col1 == -1){
	col1 = 0;
	k = st = 0;
    }else{
	k = st;
    }

    for(;end_of_line <= col1 ; end_of_line += maxlength);

    throw(col1);
    if(mst >= 0) {
	if(st < mst) {
	    set_screen_reverse(0x01,0x02);
	    for(k = st ; k <mst ; k++) {
		if(bold == 0 && k >= bst && k < bend) {
		    set_screen_bold(0x40);
		    bold = 1;
		} else if (bold && (k < bst || k >= bend)) {
		    reset_screen_bold(0x40);
		    set_screen_reverse(0x01,0x02);
		    bold = 0;
		}
		if(k >= end) {
		    reset_screen_reverse(0x01,0x02);
		    reset_screen_bold(0x40);
		    bold = 0;
		    goto normal_out;
		}
		w_putchar_move(c_b->buffer[k]);
		if(col > end_of_line - CHAR_BYTE) {
		    flushw_buf();
		    if(col  < end_of_line) {
			reset_screen_reverse(0x01,0x02);
			reset_screen_bold(0x40);
			bold = 0;
			putchar_move(' ');
			set_screen_reverse(0x01,0x02);
			if(k >= bst && k < bend) {
			    set_screen_bold(0x40);
			    bold = 1;
			}
		    }
		    throw(end_of_line);
		    end_of_line += maxlength;
		}
	    }
	    reset_screen_reverse(0x01,0x02);
	    reset_screen_bold(0x40);
	    bold = 0;
	}
	if(k < mend){
	    set_screen_reverse(0x04,0x08);
	    for(; k < mend ; k++) {
		if(bold == 0 && k >= bst && k < bend) {
		    set_screen_bold(0x40);
		    bold = 1;
		} else if (bold && (k < bst || k >= bend)) {
		    reset_screen_bold(0x40);
		    set_screen_reverse(0x04,0x08);
		    bold = 0;
		}
		if(k >= end) {
		    reset_screen_reverse(0x04,0x08);
		    reset_screen_bold(0x40);
		    bold = 0;
		    goto normal_out;
		}
		w_putchar_move(c_b->buffer[k]);
		if(col > end_of_line - CHAR_BYTE){
		    flushw_buf();
		    if(col < end_of_line){
			reset_screen_reverse(0x04,0x08);
		  	reset_screen_bold(0x40);
		  	bold = 0;
			putchar_move(' ');
			set_screen_reverse(0x04,0x08);
			if(k >= bst && k < bend) {
			    set_screen_bold(0x40);
			    bold = 1;
			}
		    }
		    throw(end_of_line);
		    end_of_line += maxlength;
		}
	    }
	    reset_screen_reverse(0x04,0x08);
	    reset_screen_bold(0x40);
	    bold = 0;
	}
    }
    if(k < c_b->maxlen && k < end){
	set_screen_reverse(0x10,0x20);
	for(; k <c_b->maxlen ; k++) {
	    if(bold == 0 && k >= bst && k < bend) {
		set_screen_bold(0x40);
		bold = 1;
	    } else if (bold && (k < bst || k >= bend)) {
		reset_screen_bold(0x40);
		set_screen_reverse(0x10,0x20);
		bold = 0;
	    }
	    if(k >= end) {
		reset_screen_reverse(0x10,0x20);
		reset_screen_bold(0x40);
		bold = 0;
		goto normal_out;
	    }
	    w_putchar_move(c_b->buffer[k]);
	    if(col > end_of_line - CHAR_BYTE){
		flushw_buf();
		if(col < end_of_line ){
		    reset_screen_reverse(0x10,0x20);
		    reset_screen_bold(0x40);
		    bold = 0;
		    putchar_move(' ');
		    set_screen_reverse(0x10,0x20);
		    if(k >= bst && k < bend) {
			set_screen_bold(0x40);
			bold = 1;
		    }
		}
		throw(end_of_line);
		end_of_line += maxlength;
	    }
	}
	reset_screen_reverse(0x10,0x20);
	reset_screen_bold(0x40);
	bold = 0;
    }

normal_out:
    if(clr_l == 1){
	clr_end_screen();
	oldmaxcol = col;
    }else if(clr_l == 2){
	clr_end_screen();
	oldmaxcol = col;
    }else{
	oldmaxcol = MAX_VAL(oldmaxcol , col);
    }
    return;
}

void
t_redraw_one_line()
{
    t_print_line(0, c_b->maxlen,1);
}

void
init_screen()
{
    if(c_b->maxlen)t_print_l();
}

void
t_redraw_move(x , start , end,clr_l)
int x, start, end, clr_l;
{
    (*t_redraw_move_func)(x , start , end,clr_l);
}

void
t_move(x)
int x;
{
    int old_cp = c_b->t_c_p;

    if(((c_b->hanten >> 2) & 0x3) != ((c_b->hanten >> 4) & 0x3)) {
	t_redraw_move(x , MIN_VAL(old_cp, x), MAX_VAL(old_cp, x),0);
	return;
    }
    if(x >= c_b->maxlen) x = c_b->maxlen;
    c_b->t_c_p = x;
    throw(cur_ichi(c_b->t_c_p , 0));
    return;
}

void
t_print_l()
{
    (*t_print_l_func)();
}

char *
get_rk_modes()
{
    char        *p;
#ifndef SYSVR2
    extern char *index();
#else
    extern char *strchr();
#endif

    strcpy(rk_modes, (NULL == (p = romkan_dispmode()) ? "[   ]" : p));
#ifndef SYSVR2
    if ((p = (char*)index(rk_modes, ':')) != NULL) {
#else
    if ((p = (char*)strchr(rk_modes, ':')) != NULL) {
#endif
	set_cur_env(*(++p));
	*p = '\0';
    }
    return(rk_modes);
}

void
disp_mode()
{
    jw_disp_mode(get_rk_modes());
}

/* cursor status is saved before call it */
void
display_henkan_off_mode()
{
  char	*p;

  strcpy(rk_modes, (NULL == (p = romkan_offmode()) ? "[---]" : p));
  jw_disp_mode(rk_modes);
}
