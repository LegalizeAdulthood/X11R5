/*
 * $Id: jutil.c,v 1.2 1991/09/16 21:37:01 ohm Exp $
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
/* jisho utility routine for otasuke (user interface) process */

#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"

typedef struct _msg_fun {
  char *msg;
  int (*fun)();
} msg_fun;

static msg_fun message1[] = { 
#ifdef	USING_XJUTIL
  {"", jishoadd},
  {"",select_one_dict9},
  {"", touroku},
  {"", kensaku},
  {"",paramchg},
#endif	/* USING_XJUTIL */
  {"",dicsv}
#ifdef	USING_XJUTIL
  ,{"",dicinfoout},
  {"", fuzoku_set}
#endif	/* USING_XJUTIL */
};

int
jutil(in)
int in;
{
    static int last = 3;
    static int c, ret;
    static int k;
    static char *buf[sizeof(message1) / sizeof(msg_fun)];
    static WnnClientRec *c_c_sv = 0;
    static jutil_step = 0;
    static char *lang;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return (0);
    }
    if (jutil_step == 0) {
	c_c_sv = c_c;
	lang = cur_lang;
#ifdef	USING_XJUTIL
	for(k = 0 ; k <( sizeof(message1) / sizeof(msg_fun));k++){
	    buf[k] = msg_get(cd, 43+k, default_message[43+k], lang);
	}
#else	/* USING_XJUTIL */
	buf[0] = msg_get(cd, 48, default_message[48], lang);
#endif	/* USING_XJUTIL */
	jutil_step++;
    }
    if (jutil_step == 1) {
	c = xw_select_one_element(buf, sizeof(message1)/sizeof(msg_fun),
				 -1, last,
				 msg_get(cd, 20, default_message[20], lang),
				 SENTAKU,main_table[4], in);
	if(c == BUFFER_IN_CONT) {
	    return(BUFFER_IN_CONT);
	}
	if(c == -1 || c == -3) {
	    c_c_sv = 0;
	    jutil_step = 0;
	    return(0);
	}
	jutil_step++;
	last = c;
    }
    if (jutil_step == 2) {
	ret = (*message1[c].fun)();
	if (ret == BUFFER_IN_CONT) {
	return(BUFFER_IN_CONT);
	}
    }
    c_c_sv = 0;
    jutil_step = 0;
    return(0);
}

#ifdef	USING_XJUTIL
int
paramchg()
{
    xw_paramchg();
    return(0);
}
#endif	/* USING_XJUTIL */

int
dicsv()
{
    if(jl_dic_save_all(bun_data_) == -1){
    	errorkeyin();
    }
    else {
	print_msg_getc("%s",msg_get(cd, 0, default_message[0], cur_lang));
    }
    return(0);
}


#ifdef	USING_XJUTIL
int
dicinfoout()
{
    xw_dicinfoout();
    return(0);
}

int
select_one_dict9()
{
    xw_select_one_dict9();
    return(0);
}

int
fuzoku_set()
{
    xw_fuzoku_set();
    return(0);
}

int
jishoadd()
{
    xw_jishoadd();
    return(0);
}

int
kensaku()
{
    xw_kensaku();
    return(0);
}

#endif	/* USING_XJUTIL */
