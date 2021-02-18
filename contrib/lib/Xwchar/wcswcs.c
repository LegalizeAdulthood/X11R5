/*
 * $XConsortium: wcswcs.c,v 1.1 91/07/01 11:03:42 xguest Exp $
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

/*
 *	find one ws within another;
 *      if ws1==""|ws2==""|ws1==NULL|ws2==NULL return NULL    
 */

#include <stdio.h>		/* for NULL */
#include "wcharint.h"

extern unsigned long wcslen();
extern wchar_t *wcschr();

wchar_t  *
wcswcs(ws1,ws2)
register wchar_t *ws1,*ws2;
{
	register wchar_t *pos, *wp1 = ws1, *wp2 = ws2;
	register wchar_t wc; 
	int	 len2;
	
	len2 = wcslen(ws2);
	if (ws1 == NULL || ws2 == NULL || *ws2 == WNULL 
		|| *ws1 == WNULL || (len2 > wcslen(ws1)))
		return(NULL); 
	
	wc = *ws2;
	while (*ws1 != WNULL) {
		if ((wp1 = pos = wcschr(ws1, wc)) == NULL)
			return(NULL);
		if (len2 > wcslen(wp1))
			  return(NULL);
		while ( *wp2 != WNULL && *wp1++ == *wp2++);
		if (*wp2 == WNULL) 
			return(pos);
		ws1 = pos + 1; /* forward ws1 for the next occurence of wc */
		wp2 = ws2; /* Restore wp2 to the start of ws2 */
	}
	return(NULL);
}
