/*
 * $XConsortium: mblen.c,v 1.10 91/07/01 11:01:10 xguest Exp $
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
 *	Author: Hiroshi Kuribayashi	OMRON Corporation
 *					kuri@nff.ncl.omron.co.jp
 *   
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include "Xlocaleint.h"

int
#if NeedFunctionPrototypes
mblen(const char *s, size_t n)
#else
mblen(s, n)
    char *s;
    int n;
#endif
{
    int ret;

    XLocale xlc = _XFallBackConvert();

    if (s == NULL) {
	_Xmbcsid(xlc, CODESET0);	/* sets the initial state */
	ret = _Xmbtype(xlc);
	if (ret < 0)			return (-1);
	else if (ret == CDS_STATELESS)	return (0);	/* STATELESS  */
	else				return (1);	/* STATEFUL */
    }

    ret = _Xmbdlen(xlc, s);
    if (ret == 0)
	ret = _Xmblen(xlc);
    if (ret > n)	return (-1);
    return(ret);
}


#ifdef	DEBUG
void
dump_mb(mb)
    unsigned char *mb;
{
    int len, i, j = 0;

    while(*mb) {
	len = mblen(mb, strlen(mb));
	if(len <= 0)	break;
	for (i = 0; i < len; i++) {
	    printf("%02x", *mb);
	    mb++;
	}
	printf(" ", *mb);
	j++;
    }
    printf("\nmb len = %d\n", j);
}

void
dump_wc(wc)
    wchar *wc;
{
    int len;

    len = wcslen(wc);

    while(*wc) {
	printf("%08x ", *wc);
	wc++;
    }
    printf("\n");
    
    printf("wc len = %d\n", len);

}

#endif	/* DEBUG */
