/*
 * $Id: kuten.c,v 1.2 1991/09/16 21:37:05 ohm Exp $
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
/*
**   programs for KUTEN NYUURYOKU
*/

#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"

#define MAX_TEN_LENGTH  94

static int 
get_ichiran(ku, in)
int ku;
int in;
{
    static char buf[1024];
    static char *buf_ptr[MAX_TEN_LENGTH]; 
    static int i;
    static char *c;
    static char	kuten_title[32];
    static int kuten_ichi_step = 0;
    static char *lang;

    if (kuten_ichi_step == 0) {
	for(i = 0,c = buf ; i < MAX_TEN_LENGTH; i++) {
	    buf_ptr[i] = c;
	    *c++ = (ku + 32) | 0x80;
	    *c++ = ((i + 1) + 32) | 0x80;
	    *c++ = 0;
	}
	lang = cur_lang;
	sprintf(kuten_title, "%s %2d %s", msg_get(cd, 24, default_message[24], lang), ku, msg_get(cd, 26, default_message[26], lang));
	kuten_ichi_step++;
    }
    if((i = xw_select_one_jikouho(buf_ptr, MAX_TEN_LENGTH,-1, 0, kuten_title,
				  KUTEN_IN, main_table[4], in)) == -1){
	kuten_ichi_step = 0;
	return(-1);
    } else if (i == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    }
    kuten_ichi_step = 0;
    return((((ku + 32) | 0x80) << 8) | ((i + 1) + 32) | 0x80);
}

int 
in_kuten(in)		/*  returns code for a moji  */
int in;
{
    static char buffer[5];
    static int code;
    static int c_p = 0;
    static int c , k;
    static int ret1 , ret2;
    static int mode = -1;
    static WnnClientRec *c_c_sv = 0;
    static int kuten_step = 0;
    static char *lang;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return(-1);
    }
    if (c_c_sv == 0) {
	for(k=0;k < 5;buffer[k++] = ' ');
	buffer[5] = '\0';
	c_c_sv = c_c;
	c_p = 0;
	mode = -1;
	lang = cur_lang;
    }

start:
    if (kuten_step == 0) {
	if (init_ichiran((unsigned char **)NULL, 0, -1, -1,
		(unsigned char *)msg_get(cd, 25, default_message[25], lang),
		(unsigned char *)msg_get(cd, 24, default_message[24], lang),
		(unsigned char *)msg_get(cd, 19, default_message[19], lang),
		5, NYUURYOKU) == -1) {
	    ring_bell();
	    c_c_sv = 0;
	    return(-1);
	}
	draw_nyuu_w((unsigned char *)buffer, 1);
	kuten_step++;
	return(BUFFER_IN_CONT);
    }
    if (kuten_step == 1) {
	c = in;
	if (!xim->ichi->map) return(BUFFER_IN_CONT);
	if (xim->sel_ret == -2) {
	    end_ichiran();
	    xim->sel_ret = -1;
	    c_c_sv = 0;
	    kuten_step = 0;
	    return(-1);
	}
	if(c < 256){
	    if ((c == ESC) || (t_quit == main_table[5][c])){
		end_ichiran();
		c_c_sv = 0;
		kuten_step = 0;
		return(-1);
	    } else if (henkan_off == main_table[5][c]) {
		ring_bell();
		return(BUFFER_IN_CONT);
	    }
	}
	if((mode >= 0 ? (c_p - mode) <= 2 : c_p < 4)
			 && c <= '9' && c >= '0'){
	    buffer[c_p++] = (char)c;
	}if((mode == -1) &&(c ==(int)'.') && 0 < c_p && c_p <= 2){
	    mode = c_p;
	    buffer[c_p++] = (char)c;
	}else if(c == rubout_code && c_p){
	    if(buffer[c_p - 1] == '.')
		mode = -1;
	    buffer[--c_p] = ' ';
	}else if(c == rubout_code && c_p == 0){
	    end_ichiran();
	    c_c_sv = 0;
	    kuten_step = 0;
	    return(-1);
	}else if( ((c == NEWLINE) || (c == CR))  && (c_p >= 3)
				&& (buffer[c_p - 1] != '.')){
	    if((!(sscanf(buffer , "%d.%d" , &ret1 , &ret2 ) == 2) && 
		    !((c_p == 4) && 
			sscanf(buffer , "%2d%2d",&ret1 , &ret2) == 2) &&
		    !((c_p == 3) && 
			sscanf(buffer , "%1d%2d",&ret1 , &ret2) == 2) )
		   || ret1 <= 0 || ret1 > 94 || ret2 <= 0 || ret2 > 94){
		    ring_bell();	/* ADD KURI */
		    goto RET;
	    }
	    code = (((ret1 + 32) | 0x80) << 8) |((ret2 + 32) | 0x80);
	    end_ichiran();
	    goto LAST;	
	}else if((c == NEWLINE || c == CR) && c_p <= 2 && mode == -1){
	    if(sscanf(buffer , "%d" , &ret1 ) != 1 ||
		(ret1 <= 0 || ret1 > 94 )){
		goto RET;
	    }
	    end_ichiran();
	    kuten_step++;
	    goto NEXT;
	}
RET:
	draw_nyuu_w((unsigned char *)buffer, 1);
	return(BUFFER_IN_CONT);
    }
NEXT:
    if (kuten_step == 2) {
	if((code = get_ichiran(ret1, in)) == -1){
	    kuten_step = 0;
	    goto start;
	} else if (code == BUFFER_IN_CONT) {
	    return(BUFFER_IN_CONT);
	}
    }
LAST:
    c_c_sv = 0;
    kuten_step = 0;
    return(code);
}

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/
