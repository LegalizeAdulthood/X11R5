/*
 * $XConsortium: iswprint.c,v 1.7 91/07/01 14:37:53 xguest Exp $
 */

/*
 * Copyright 1990, 1991 by OMRON Corporation, NTT Software Corporation,
 *                      and Nippon Telegraph and Telephone Corporation
 * Copyright 1991 by the Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON, NTT Software, NTT, and M.I.T.
 * not be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission. OMRON, NTT Software,
 * NTT, and M.I.T. make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * OMRON, NTT SOFTWARE, NTT, AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL OMRON, NTT SOFTWARE, NTT, OR M.I.T. BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	Author: Li Yuhong	OMRON Corporation
 *
 */

#include "wcharint.h"

/* 
 * true if wc is printable, i.e.:
 * 			0x20 <= G, P, R, C <= 0x7e 
 *		or	0xa0 <= G, P, R, C <= 0xff
 *				 
*/
#define _wnonprint(w)	((w == 0x7f)?1 : ((unsigned char)(w&0x7f) < 0x20))

#if NeedFunctionPrototypes
 int iswprint(wint_t wc)
#else
 int iswprint(wc)
 wint_t wc;
#endif
{
	register int neg;

	neg = (_wnonprint(wgetgroup(wc)) || _wnonprint(wgetplane(wc)) ||
	       _wnonprint(wgetrow(wc)) || _wnonprint(wgetcell(wc)));
	return(!neg);
}
	
