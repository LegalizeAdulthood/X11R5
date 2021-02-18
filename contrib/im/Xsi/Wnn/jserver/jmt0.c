/*
 * $Id: jmt0.c,v 1.2 1991/09/16 21:31:48 ohm Exp $
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
#include "commonhd.h"
#include "jslib.h"
#include "ddefine.h"
#include "jdata.h"
#include "kaiseki.h"

#include <stdio.h>


void
init_jmt(x)
register int	x ;
{
	register int	n;
	register struct jdata **keep_ptr ;
	register struct jdata **jmt_ptr_org = jmt_ptr; /* H.T. */
	
	if(x == 0) x = initjmt;	/* H.T. 22/12/89 */

	for (n = x; n < initjmt &&
	    (jmtp[n] == (struct jdata **)0 ||
	     jmtp[n] == (struct jdata **) -1); n++)
		;

	if (n < initjmt) {	/* Tukatte nai nodakara, kesubeki deha... */
		keep_ptr = jmt_ptr ;

		if (n == x)
			jmt_ptr = jmtp[x] ;
		else
			jmt_ptr = jmtp[n] + (maxj[n] - n + 1) ;

		for(n = 0;(jmt_ptr + n) < keep_ptr && *(jmt_ptr + n) <= (struct jdata *)0; n++)
			;
		if ((jmt_ptr + n) < keep_ptr)
			j_e_p = *(jmt_ptr + n) ;
	} else {
	  	jmt_ptr = jmt_ ;  
    		j_e_p = jmtw_ ;
	}
/* this part is added 8/18 by H.T */
	for(keep_ptr = jmt_ptr ; keep_ptr < jmt_ptr_org ; keep_ptr++){
		*keep_ptr = NULL;
	}
}


int
jmt_set (yomi)
register int	yomi;	/* 読み文字列の先頭へのポインタ(逆順) */
{
	register int	n ;
	if ((n = jishobiki(&(bun[yomi]), jmt_ptr)) > 0) {
		jmtp[yomi] = jmt_ptr;
		jmt_ptr += n ;
		maxj[yomi] = yomi + (n - 1) ;
	} else {
		if (n == 0) {
			jmtp[yomi] = 0 ;
			maxj[yomi] = 0 ;
		} else{
		    jmtp[yomi] = 0;
		    maxj[yomi] = 0;
		}
	}
	return(1) ;
}
