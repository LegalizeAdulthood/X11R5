/*
 * $Id: iswblank.c,v 1.7 1991/09/11 08:33:56 proj Exp $
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
 * true if wc is a blank. 
 * where the blank is defined one of the following:
 *    1. Latin-1 blank  0x20202020 or
 *    2. Chinese blank  0x2030A1A1 or
 *    3. Japanese blank 0x2040A1A1 or
 *    4. Korean blank   0x2050A1A1
 * 
 * NOTE: Actually there is only one blank -- Latin-1 blank is defined
 *       in DIS 10646. 
 *       Now I use the stupid method just to compatible with the present
 *       and passed instance of files.
 *       This is a why I only define one constance WCHARSPACE in wchar.h
 * 
*/

#define WideSpace	0x2121
#define WCHNSPACE	(HANZISCRIPT | WideSpace)
#define WJPNSPACE	(KANJISCRIPT | WideSpace)
#define WKRNSPACE	(HANJASCRIPT | WideSpace)

#if NeedFunctionPrototypes
 int iswblank(wint_t wc)
#else
 int iswblank(wc)
 wint_t wc;
#endif
{
	return((wc == WCHARSPACE) || (wc == WCHNSPACE) ||
		(wc == WJPNSPACE) || (wc == WKRNSPACE));
}
