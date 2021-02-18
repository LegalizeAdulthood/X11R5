/*
 * $XConsortium: iswascii.c,v 1.6 91/07/01 11:00:47 xguest Exp $
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
 * true if wc is DIS 10646's ASCII, i.e, Latin-1 script, code 0x2020207F(DEL),
 * and control set(C0 PAD PAD PAD) plus wnull-terminator WNULL.
 */
#ifdef DEL
#undef DEL
#endif

#define DEL	0x7f

#if NeedFunctionPrototypes
 int iswascii(wint_t wc)
#else
 int iswascii(wc)
 wint_t wc;
#endif
{
	return(((wc&CONTROLSCRIPT) == CLPADPADPAD && iscntrl(wgetgroup(wc))) ||
		((wc&SCRIPT1) == LATINSCRIPT && (isprint(wgetcell(wc)) ||
		wgetcell(wc) == DEL)) || (wc == WNULL));	
}
	
#undef DEL
