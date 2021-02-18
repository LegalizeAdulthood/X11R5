/*
 * $Id: uif.c,v 1.3 1991/09/20 11:18:25 proj Exp $
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
#include "rk_spclval.h" /* defines of CHMSIG, NISEBP */
#include "xext.h"


static int henkan_mode;
/*  0: select
    1: yomi_nyuuryoku
    2: bunsetu_nobasi
    3: kara
*/

static void
change_to_insert_mode()
{
  c_b->key_table = main_table[1];
  c_b->rk_clear_tbl =  romkan_clear_tbl[1];
  c_b->key_in_fun = NULL;
  c_b->ctrl_code_fun = NULL;
  c_b->hanten = 0x08 | 0x20;
  henkan_mode = 1;
  kk_cursor_normal();
}

static void
change_to_empty_mode()
{
  c_b->key_table = main_table[3];
  c_b->rk_clear_tbl =  romkan_clear_tbl[3];
  c_b->key_in_fun = NULL;
  c_b->ctrl_code_fun = NULL;
  c_b->hanten =  0x08 | 0x20;
  henkan_mode = 3;
  throw_col(0);
  kk_cursor_normal();
}

int
empty_modep()
{
    return(henkan_mode == 3);
}

int
insert_modep()
{
    return(henkan_mode == 1);
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
isconect_jserver()
{
    if (js_isconnect(cur_env)) {
	connect_server();
    }
    if (js_isconnect(cur_env)) {
	print_msg_getc(" jserver(?)", NULL, NULL, NULL);
	t_print_l();
	return (0);
    }
    return(1);
}

int kill_c()
{
    t_kill();
    if(c_b->maxlen == 0){
	change_to_empty_mode();
    }
    return(0);
}

#ifdef	CONVERT_by_STROKE
int errorkill_c()
{
    errort_kill();
    if(c_b->maxlen == 0){
	change_to_empty_mode();
    }
    return(0);
}
#endif	/* CONVERT_by_STROKE */

int delete_c(c,romkan)
int c,romkan;
{
    t_delete_char(c , romkan);
    if((c_b->maxlen == 0) && is_HON(romkan)){
	change_to_empty_mode();
    }
    return(0);
}

int rubout_c(c , romkan)
int c, romkan;
{
    t_rubout(c, romkan);
    if((c_b->maxlen == 0) && is_HON(romkan)){
	change_to_empty_mode();
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
    }else{
	backward();
    }
    return(0);
}

int
insert_it_as_yomi()
{
    change_to_insert_mode();
    c_b->t_m_start = 0;
    cur_bnst_ = 0;
    t_print_l();
    return(0);
}

int
reconnect_server()
{
  return(0);
}

int
disconnect_server()
{
    WnnEnv *p;

    for (p = normal_env; p; p = p->next) {
    	if (p->env != NULL && js_isconnect(p->env)) {
	    js_close(p->env);
	}
    }
    return(1);
}
