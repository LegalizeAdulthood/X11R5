/*
 * $Id: jis_in.c,v 1.2 1991/09/16 21:37:00 ohm Exp $
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
 *	programs for JIS NYUURYOKU
 */

#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"


#define FROM_JIS 	0x21
#define TO_JIS		0x7E

static int
hextodec(c)
char c;
{
    if (c >= '0' && c <= '9'){
	return(c - '0');
    } else if (c >= 'A' && c <= 'F'){
	return(c - 'A' + 10);
    } else if (c >= 'a' && c <= 'f'){
	return(c - 'a' + 10);
    } else
	return(-1);
}

static int
get_jis_ichiran(st, num, in)
char *st;
int num;
int in;
{
    static int from;
    static int i;
    static char *c;
    static int maxh;
    static int code;
    static char buf[1024];
    static char *buf_ptr[96]; 
    static char jis_title[32];
    static int jis_ichi_step = 0;

    if (jis_ichi_step == 0) {
	if (num == 2) {
	    from = hextodec(st[0]) << (8 + 4) | hextodec(st[1]) << 8 | 0x21 ;
	    from |= 0x8080;
	    maxh = 94;
	} else {
	    from = hextodec(st[0]) << (8 + 4) | hextodec(st[1]) << 8 |
		   hextodec(st[2]) << 4;
	    from |= 0x8080;
	    maxh = 16;
	    if ((from & 0xf0) == 0xf0) {
		maxh--;
	    } else if ((from & 0xf0) == 0xa0) {
		maxh--;
		from++;
	    }
	}

	for(i = 0,c = buf ; i < maxh; i++){
	    code = from + i;
	    buf_ptr[i] = c;
	    *c++ = (code & 0xff00 ) >> 8;
	    *c++ = (code & 0x00ff );
	    *c++ = 0;
	}
	sprintf(jis_title, "%s 0x%s",msg_get(cd, 17, default_message[17], cur_lang), st);
	jis_ichi_step++;
    }
    if((i = xw_select_one_jikouho(buf_ptr, maxh, -1, 0, jis_title,
				  JIS_IN, main_table[4], in)) == -1){
	jis_ichi_step = 0;
	return(-1);
    } else if (i == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
    }
    jis_ichi_step = 0;
    return(from + i);
}

int 
in_jis(in)		/*  returns code for a moji  */
int in;
{
    static char buffer[5];
    static int code;
    static int c_p = 0;
    static int c , k;
    static WnnClientRec *c_c_sv = 0;
    static int jis_step = 0;
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
	lang = cur_lang;
    }
start:
    if (jis_step == 0) {
	if (init_ichiran((unsigned char **)NULL, 0, -1, -1,
		(unsigned char*)msg_get(cd, 17, default_message[17], lang),
		(unsigned char*)msg_get(cd, 18, default_message[18], lang),
		(unsigned char*)msg_get(cd, 19, default_message[19], lang),
		5, NYUURYOKU) == -1) {
	    ring_bell();
	    c_c_sv = 0;
	    return(-1);
	}
	draw_nyuu_w((unsigned char *)buffer, 1);
	jis_step++;
	return(BUFFER_IN_CONT);
    }
    if (jis_step == 1) {
	c = in;
	if (!xim->ichi->map) return(BUFFER_IN_CONT);
	if (xim->sel_ret == -2) {
	    end_ichiran();
	    xim->sel_ret = -1;
	    c_c_sv = 0;
	    jis_step = 0;
	    return(-1);
	}
	if((c == ESC) || (t_quit == main_table[5][c])){
	    end_ichiran();
	    c_c_sv = 0;
	    jis_step = 0;
	    return(-1);
	} else if (henkan_off == main_table[5][c]){
	    ring_bell();
	    return(BUFFER_IN_CONT);
	}else if(c_p < 4 && 
		 ((c <= '9' && c >= '0') || 
		 (c <= 'F' && c >= 'A') || 
		 (c <= 'f' && c >= 'a'))) {
	    if ((c_p == 0 || c_p == 2) ?
		(c <= 'F' && c >= 'A') || /* For EUC */
		(c <= 'f' && c >= 'a') || /* For EUC */
		(c <= '7' && c >= '2') :
		!((buffer[c_p-1]=='2' || 
		   buffer[c_p-1]=='A' || buffer[c_p-1]=='a')
		  && c == '0' ||
		  (buffer[c_p-1]=='7' ||
		   buffer[c_p-1]=='F' || buffer[c_p-1]=='f')
		  && (c == 'F' || c == 'f'))){
		buffer[c_p++] = (char)c;
		goto RET;
	    } else {
		ring_bell();	/* ADD KURI */
		return(BUFFER_IN_CONT);
	    }
	}else if(c == rubout_code && c_p){
	    buffer[--c_p] = ' ';
	    goto RET;
	}else if(c == rubout_code && c_p == 0){
	    end_ichiran();
	    c_c_sv = 0;
	    jis_step = 0;
	    return(-1);
	}else if( c == NEWLINE || c == CR ){
	    if (c_p <= 1){
		goto RET;
	    } else if (c_p == 4){
		code = 0;
		for(k = 0; k < 4; k++){
			code = code * 16 + hextodec(buffer[k]);
		}
		code |= 0x8080;
		end_ichiran();
		goto LAST;
	    }
	    end_ichiran();
	    jis_step++;
	    goto NEXT;
	} else {
	    ring_bell();	/* ADD KURI */
	}
RET:
	draw_nyuu_w((unsigned char *)buffer, 1);
	return(BUFFER_IN_CONT);
    }
NEXT:
    if (jis_step == 2) {
	if((code = get_jis_ichiran(buffer, c_p, in)) == -1){
	    jis_step = 0;
	    goto start;
	} else if (code == BUFFER_IN_CONT) {
	    return(BUFFER_IN_CONT);
	}
    }
LAST:
    c_c_sv = 0;
    jis_step = 0;
    return(code);
}

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/
