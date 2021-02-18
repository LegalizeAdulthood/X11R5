/*
 * $XConsortium: wcscspn.c,v 1.1 91/07/01 11:01:29 xguest Exp $
 */
/*
 * Copyright 1991 by OMRON Corporation
 * Copyright 1991 by the Massachusetts Institute of Technology
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the names of OMRON and M.I.T.  not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission. OMRON and M.I.T. 
 * make no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * OMRON AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL OMRON OR M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 *	Author: Yang Liqun	OMRON Corporation
 */

#include "wcharint.h"

unsigned long wcscspn(ws1,ws2)
register wchar_t *ws1,*ws2;
{
	register unsigned long n = 0;
	register wchar_t *wp;
	register wchar_t wc;

	while (wc = *ws1++) {
		for (wp = ws2; *wp!=WNULL; wp++)
			if (wc == *wp)
				break;
		if (*wp) 
			return(n);
		n++;
	}
	return(n);
}
