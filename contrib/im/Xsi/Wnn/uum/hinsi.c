/*
 * $Id: hinsi.c,v 1.2 1991/09/16 21:33:35 ohm Exp $
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
#include "sheader.h"
#include "jllib.h"

/*#define ROOT "… ªÏ/" */
#define ROOT "/"
w_char root[sizeof(ROOT) * 2];
/*
char *maeni = "¡∞§ÀÃ·§Î";
*/

#define MAXHINSI 256

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
    cnt = jl_hinsi_list(bun_data_, -1, node, &whbuf);
    if(cnt == -1){
	errorkeyin();
	return(NULL);
    }
    if(cnt == 0) return(node);
    for(k = 0, c = buf ; k < cnt ; k++){
	hbuf[k] = c;
	sStrcpy(c, whbuf[k]);
	c += strlen(c) + 1;
    }
    hbuf[cnt] = MSG_GET(15);
 TOP:
    k = select_one_element(hbuf, cnt + 1, 0, "", 0, 0, main_table[4]);
    if(k == -1)return(NULL);
    if(strcmp(hbuf[k], MSG_GET(15)) == 0) return((w_char *)MSG_GET(15));
    Sstrcpy(tmp, hbuf[k]);
    ret = bunpou_search(tmp);
    if(ret == NULL) return(NULL);
    if(strcmp((char *)ret, MSG_GET(15)) == 0) goto TOP;
    return(ret);
}


int
hinsi_in()
{
    w_char *a;

    Sstrcpy(root, ROOT);
    not_redraw = 1;
    if((a = bunpou_search(root)) == NULL ||
       strcmp((char *)a, MSG_GET(15)) == 0) {
	not_redraw = 0;
	return(-1);
    }
    not_redraw = 0;
    return(jl_hinsi_number(bun_data_, a));
}
