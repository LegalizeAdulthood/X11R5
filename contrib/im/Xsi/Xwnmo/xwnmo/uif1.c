/*
 * $Id: uif1.c,v 1.2 1991/09/16 21:37:26 ohm Exp $
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
#include "ext.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"

int
jutil_c(in)
int in;
{
    static WnnClientRec *c_c_sv = NULL;
    static int jutil_c_step = 0;

    if (c_c_sv != NULL && c_c != c_c_sv) {
	ring_bell();
	return (-1);
    }
    if (jutil_c_step == 0) {
	if (c_c_sv) {
	    return (-1);
	}
	c_c_sv = c_c;
	push_func(c_c, jutil_c);
	if(!isconect_jserver()){
	    c_c_sv = NULL;
	    pop_func(c_c);
	    return (0);
	}
	jutil_c_step = 1;
    }
    if (jutil(in) == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    }
    c_c_sv = NULL;
    pop_func(c_c);
    jutil_c_step = 0;
    return(0);
}

int
touroku_c()
{
    if(!isconect_jserver()){
	return (0);
    }
    touroku();
    return(0);
}

int
reconnect_jserver_body(in)
int in;
{
    int k;
    int c;
    static int c_p = 0;
    static WnnClientRec *c_c_sv = 0;
    static int j_con_step = 0;
    static char hostname[32];
    static char *lang;

    if (c_c->use_server == 0) return(0);
    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return (-1);
    }
    if (c_c_sv == 0) {
	for (k = 0; k < 32; hostname[k++] = '\0');
	c_c_sv = c_c;
	c_p = 0;
	lang = cur_lang;
    }
    if (j_con_step == 0) {
	if (init_ichiran((unsigned char**)NULL, 0, -1, -1,
		(unsigned char*)msg_get(cd, 29, default_message[29], lang),
		(unsigned char*)msg_get(cd, 30, default_message[30], lang),
		(unsigned char*)msg_get(cd, 19, default_message[19], lang), 
		32, NYUURYOKU) == -1) {
	ring_bell();
	c_c_sv = 0;
	return(-1);
	}
	sprintf(hostname, "%s", servername);
	c_p = strlen(hostname);
	draw_nyuu_w((unsigned char *)hostname, 1);
	j_con_step++;
	return(BUFFER_IN_CONT);
    }
    if (j_con_step == 1) {
	c = in;
	if (!xim->ichi->map) return(BUFFER_IN_CONT);
	if (xim->sel_ret == -2) {
	    end_ichiran();
	    xim->sel_ret = -1;
	    c_c_sv = 0;
	    j_con_step = 0;
	    romkan_clear();
	    return(-1);
	}
	if (c < 256){
	    if ((c == ESC) || (t_quit == main_table[5][c])) {
		end_ichiran();
		c_c_sv = 0;
		romkan_clear();
		return(-1);
	    } else if (henkan_off == main_table[5][c]) {
		ring_bell();
	    } else if (c == rubout_code && c_p) {
		hostname[--c_p] = '\0';
	    } else if (c == NEWLINE || c == CR) {
		if(c_p == 0) {
		    goto RET;
		} else {
		    end_ichiran();
		    goto NEXT;
		}
	    } else if(c > 20 && c < 128) {
		hostname[c_p++] = (char)c;
		goto RET;
	    } else {
		ring_bell();
	    }
	} else {
	    ring_bell();
	}
RET:
	clear_nyuu_w();
	draw_nyuu_w((unsigned char *)hostname, 1);
	return(BUFFER_IN_CONT);
    }
NEXT:
    if (servername) Free(servername);
    servername = alloc_and_copy(hostname);

    if(connect_server( cur_lang) < 0) {
	print_msg_getc(" %s",msg_get(cd, 31, default_message[31], lang),
		       NULL, NULL);
    } else if(c_c->use_server && !jl_isconnect(bun_data_)) {
	print_msg_getc(" %s",msg_get(cd, 32, default_message[32], lang),
		       NULL, NULL);
    } else {
	print_msg_getc(" %s",msg_get(cd, 33, default_message[33], lang),
		       NULL, NULL);
    }
    c_c_sv = 0;
    j_con_step = 0;
    romkan_clear();
    return(0);
}

int
lang_c(in)
int in;
{
    static WnnClientRec *c_c_sv = 0;
    XIMLangRec *xl;
    XIMNestLangRec *p;
    int ret;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return (-1);
    }
    if (c_c_sv == 0) {
	c_c_sv = c_c;
	push_func(c_c, lang_c);
    }
    if ((ret = lang_set(in, &xl, &p)) == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    }
    c_c_sv = 0;
    pop_func(c_c);
    if (ret >= 0) {
	change_lang(xl, p);
    }
    return(0);
}

int
lang_ct(in)
int in;
{
    static WnnClientRec *c_c_sv = 0;
    XIMLangRec *xl;
    XIMNestLangRec *p;
    int ret;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return (-1);
    }
    if (c_c_sv == 0) {
	c_c_sv = c_c;
	push_func(c_c, lang_ct);
    }
    if ((ret = lang_set_ct(in, &xl, &p)) == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    }
    c_c_sv = 0;
    pop_func(c_c);
    if (ret >= 0) {
	change_lang(xl, p);
    }
    return(0);
}

#define	UNGETBUFSIZE	32
static unsigned int unget_buf[UNGETBUFSIZE];
static int count = 0;

int
push_unget_buf(c)
int c;
{
    if ((count + 1) >= UNGETBUFSIZE) return(-1);
    unget_buf[count++] = c;
    unget_buf[count] = EOLTTR;
    return(0);
}

unsigned int *
get_unget_buf()
{
    if (count <= 0) unget_buf[0] = EOLTTR;
    count = 0;
    return(unget_buf);
}

int
if_unget_buf()
{
    if (count > 0) return(1);
    return(0);
}
