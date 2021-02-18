/*
 * $Id: get_kaiarea.c,v 1.2 1991/09/16 21:31:35 ohm Exp $
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
#include "ddefine.h"
#include "de_header.h"
#include "jdata.h"
#include "kaiseki.h"
#include "wnn_malloc.h"

int
get_kaiseki_area(kana_len)
int kana_len ;
{
	char *c;
	char *area_pter;

	int maxj_len = kana_len * sizeof(int);
	int jmtp_len = kana_len * sizeof(struct jdata **);
	int jmt_len  = SIZE_JISHOTABLE * sizeof(struct jdata *);
	int jmtw_len = SIZE_JISHOHEAP * sizeof(struct jdata);

	int bun_len = (kana_len + 1) * sizeof(w_char);

	if((area_pter = 
		malloc(bun_len + jmtw_len + jmt_len + jmtp_len + maxj_len)) == NULL){
		wnn_errorno = WNN_MALLOC_INITIALIZE;
		error1("malloc in get_kaiseki_area");
		return(-1);
	}

	maxj = (int *)area_pter; 
	area_pter += maxj_len;
	for(c = (char *)maxj;c < area_pter ; ){
		*(c++) = 0;
	}

	jmtp = (struct jdata ***)area_pter; 
	area_pter += jmtp_len;
	for(c = (char *)jmtp ;c < area_pter ; ){
		*(c++) = 0;
	}

	jmt_ = (struct jdata **)area_pter;
	area_pter += jmt_len;
	jmtw_ = (struct jdata *)area_pter;
	area_pter += jmtw_len;
	jmt_end = jmt_ + SIZE_JISHOTABLE;
	jmt_ptr = jmt_end;	/* H.T. To initialize all in jmt_init */
	jmtw_end = jmtw_ + SIZE_JISHOHEAP;

	bun = (w_char *)area_pter;
	area_pter += bun_len;

	maxchg = kana_len;
	initjmt = maxchg - 1;
	bun[maxchg] = NULL;

	return(0);
}


/* サーバーが起きた時に呼ぶ
	解析ワークエリアをクリアする
	変換がエラーリターンした時にも呼んでね。*/

void
init_work_areas()
{
	init_ichbnp();
	init_bzd();
	init_sbn();
	init_jktdbn();
	init_jktsbn();
	init_jktsone();
}
