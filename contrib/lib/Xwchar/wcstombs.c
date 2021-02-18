/*
 * $XConsortium: wcstombs.c,v 1.11 91/07/01 11:03:39 xguest Exp $
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

extern int _XConvertWCToMB();

#if NeedFunctionPrototypes
size_t
wcstombs(char *s, const wchar_t pwcs, size_t n)
#else
unsigned long
wcstombs(s, pwcs, n)
    char *s;
    wchar_t *pwcs;
    int n;
#endif
{
    int mb_len = n, scaned_len, ret;
    extern int wcslen();

#ifdef	DEBUG
    dump_wc(pwcs);
#endif

    mb_len = n;
    ret = _XConvertWCToMB((XLocale)0, pwcs, wcslen(pwcs), s, &mb_len, &scaned_len);

#ifdef	DEBUG
    dump_mb(s);
#endif

    if (ret < 0)
	return ((unsigned long) -1);
    return (mb_len);
}
