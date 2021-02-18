/*
 * $Id: hinsi_list.c,v 1.2 1991/09/16 21:31:37 ohm Exp $
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
#include "de_header.h"
#include "wnnerror.h"
#include "jslib.h"
#include "hinsi_file.h"
#include "jdata.h"
#include "wnn_malloc.h"

void
make_hinsi_list(jtl)
struct JT *jtl;
{
    int k, state, cnt = 0, wc;
    w_char *j, *d, *start;

    if (jtl->maxhinsi_list == 0){
	jtl->node = (struct wnn_hinsi_node *)malloc(0);
	jtl->maxnode = 0;
	return;
    }
    jtl->maxnode = Strtime(jtl->hinsi_list, jtl->maxhinsi_list, NODE_CHAR);
    jtl->node = (struct wnn_hinsi_node *)
	malloc(jtl->maxnode * sizeof(struct wnn_hinsi_node) + 
	jtl->maxhinsi_list * sizeof(w_char));
    d = (w_char *)(jtl->node + jtl->maxnode);
    if(jtl->hinsi_list == NULL) return;
    j = jtl->hinsi_list;
    k = -1;
    for(;;){
	wc = get_a_word(j, &start, &state);
	if(state == 2 && wc == 0){ break;}
	Strncpy(d, start, wc);
	d[wc] = 0;
	if(state == 2){ cnt++; break;}
	if(state == 1){
	    if(k >= 0) jtl->node[k].kosuu = cnt;
	    cnt = 0;
	    k++;
	    jtl->node[k].name = d;
	    jtl->node[k].son = d + wc + 1;
	}else{
	    cnt++;
	}
	d += wc + 1;
	j = start + wc + 1;
    }
    if(k >= 0) jtl->node[k].kosuu = cnt;
}
    

int
Strtime(w, wc, c)
w_char *w;
int wc;
w_char c;
{
    int ret = 0;
    for(;wc;wc--, w++){
	if(*w == c) ret++;
    }
    return(ret);
}

int
get_a_word(d, startp, statep)
w_char *d, **startp;
int *statep;
{
    w_char *d0 = d;
    int c, wc;
    for(;;){
	if((c = *d++) == 0){
	    *statep = 2;
	    *startp = d0;
	    return(0);
	}
	if(c == IGNORE_CHAR1 || c == IGNORE_CHAR2 || c == IGNORE_CHAR1 ||
	   c == CONTINUE_CHAR || c == '\n' || c == HINSI_SEPARATE_CHAR){
	    continue;
	}
	if(c == COMMENT_CHAR){
	    for(;(*d++ != '\n'););
	    continue;
	}
	break;
    }
    *startp = d - 1;
    wc = 1;
    for(;;wc++){
	if((c = *d++) == 0){
	    *statep = 2;
	    return(wc);
	}
	if(c == IGNORE_CHAR1 || c == IGNORE_CHAR2 || c == IGNORE_CHAR1 ||
	   c == CONTINUE_CHAR || c == '\n' || c == HINSI_SEPARATE_CHAR){
	    *statep = 0;
	    return(wc);
	}else if(c == NODE_CHAR){
	    *statep = 1;
	    return(wc);
	}
    }
}

extern w_char *wnn_hinsi_name();


int
has_dict(n, m, w)
struct wnn_hinsi_node *n;
int m;
w_char *w;
{
    int j;
    for(;m;m--){
	if(Strcmp(n[m].name, w) == 0)
	for(j = n[m].kosuu; j ; j--){
	    if(Strcmp(n[m].son[j], w) == 0)
		return(1);
	}
    }
    return(0);
}

int
hinsi_table_set(jtl, table)
struct JT *jtl;
w_char *table;
{
    /*
    int len;
    */

    free(jtl->node);
    free(jtl->hinsi_list);

    jtl->maxhinsi_list = Strlen(table);
    
    jtl->hinsi_list = (w_char *)malloc(jtl->maxhinsi_list * sizeof(w_char));
    Strcpy(jtl->hinsi_list, table);
    make_hinsi_list(jtl);
    jtl->dirty = 1;
    return(0);
}
