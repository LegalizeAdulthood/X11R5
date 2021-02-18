/*
 * $Id: hinsi.c,v 1.2 1991/09/16 21:35:14 ohm Exp $
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
#include "xext.h"

#define	ROOT "/"
w_char root[sizeof(ROOT) * 2];

#define MAXHINSI 256

int save_k[5];
int save_k_p;
char *bunpou_title;
int save_num[10];
int save_num_p;
char *save_t[10];
int select_ret;

static w_char *
bunpou_search(node)
w_char *node;
{
    int cnt;
    int k;
    char *hbuf[MAXHINSI];
    w_char tmp[WNN_HINSI_NAME_LEN];
    char buf[1024];		/* iikagen */
    char *c;
    w_char **whbuf;
    w_char *ret;

    if(node == NULL) return(NULL);
    if ((cnt = js_hinsi_list(cur_env, -1, node, &rb)) == -1)
	if_dead_disconnect(cur_env, -1);
    if(cnt == -1){
	errorkeyin();
	return(NULL);
    }
    whbuf = (w_char **)(rb.buf);
    if(cnt == 0) return(node);
    for(k = 0, c = buf ; k < cnt ; k++){
	hbuf[k] = c;
	sStrcpy(c, whbuf[k]);
	c += strlen(c) + 1;
    }
    hbuf[cnt] = msg_get(cd, 0, default_message[0], xjutil->lang);
 TOP:
    k = xw_select_one_element(hbuf, cnt + 1, 0,bunpou_title,
				0, NULL,NULL,NULL,NULL);
    if(k == -1)return(NULL);
    if(k == -3) {
	select_ret = -3;
	return(NULL);
    }
    if(strcmp(hbuf[k], hbuf[cnt]) == 0) {
	JWMline_clear(save_k[--save_k_p]);
	bunpou_title = save_t[save_num_p];
	return((w_char *)hbuf[cnt]);
    }
    Sstrcpy(tmp, hbuf[k]);
    save_k[save_k_p++] = cur_text->currentcol;
    xw_jutil_write_msg(hbuf[k]);
    save_t[save_num_p] = bunpou_title;
    bunpou_title = hbuf[k];
    ret = bunpou_search(tmp);
    if(ret == NULL) return(NULL);
    if(k == -3) {
	select_ret = -3;
	return(NULL);
    }
    if(strcmp((char *)ret, hbuf[cnt]) == 0) goto TOP;
    return(ret);
}

int
hinsi_in()
{
    w_char *a;
    register int x;

    Sstrcpy(root, ROOT);
    bunpou_title = msg_get(cd, 1, default_message[1], xjutil->lang);
    save_k_p = 0;
    save_num_p = 0;
    save_t[0] = bunpou_title;

    if((a = bunpou_search(root)) == NULL ||
       strcmp((char *)a, msg_get(cd, 0, default_message[0], xjutil->lang))
	== 0) {
	return(-1);
    }
    if((x = js_hinsi_number(cur_env->js_id, a)) == -1)
	if_dead_disconnect(cur_env, -1);
    return(x);
}
