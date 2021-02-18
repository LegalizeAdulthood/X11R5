/*
 * $Id: touroku.c,v 1.2 1991/09/16 21:37:24 ohm Exp $
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
#include "rk_spclval.h"
#include "ext.h"

static void
make_touroku_buffer(bnst, cbup)
int bnst;
ClientBuf *cbup;
{
    int k , l;
    w_char *bp;
    w_char *buffer_end;

    buffer_end = c_b->buffer + c_b->buflen - 1;
    bp = c_b->buffer;
    for(k = bnst ; k < jl_bun_suu(bun_data_) ; k++){
	if (k < maxbunsetsu) touroku_bnst[k] = bp - c_b->buffer;
	l = jl_get_kanji(bun_data_, k, k+1, bp);
	bp += l;
	if (bp >= buffer_end) {
	    k--;
	    bp -= l;
	    goto GOT_IT;
	}
    }
    if (cbup->maxlen == 0) {
	get_end_of_history(bp);
    } else {
	Strncpy(bp, cbup->buffer + bunsetsu[jl_bun_suu(bun_data_)],
		cbup->maxlen - bunsetsu[jl_bun_suu(bun_data_)]);
	*(bp + cbup->maxlen - bunsetsu[jl_bun_suu(bun_data_)]) = 0;
    }

GOT_IT:
    if(k < maxbunsetsu){
	touroku_bnst[k++] = bp - c_b->buffer;
	touroku_bnst_cnt = k;
    }else{
	touroku_bnst[maxbunsetsu - 1] = bp - c_b->buffer;
	touroku_bnst_cnt = maxbunsetsu;
    }
}

int
touroku()
{
    w_char buffer[1024];
    ClientBuf *c_btmp , c_b1;

#ifdef	USING_XJUTIL
    c_btmp = c_b;
    c_b = &c_b1;

    c_b->buffer = buffer;
    c_b->buflen = 1024;

    make_touroku_buffer(0, c_btmp);   
    xw_touroku(touroku_bnst[cur_bnst_]);
    c_b = c_btmp;
#endif	/* USING_XJUTIL */
    return(0);
}

