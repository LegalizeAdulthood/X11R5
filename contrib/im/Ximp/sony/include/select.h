/*
 * Copyright 1991 Sony Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Sony not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Sony makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SONY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SONY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Masaki Takeuchi, Sony Corporation
 */
/*
 *	select.h
 *	4.[23]BSD
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 */
/*
 * $Header: select.h,v 1.1 91/04/29 17:57:45 masaki Exp $ SONY;
 */

#include <sys/param.h>

#include <sys/types.h>
#if ((BSD >= 43) || defined (BSD4_3) || defined (FD_SET))
#define LONG_FD
typedef fd_set	SELECT_FD;
#else
typedef int	SELECT_FD;
#define FD_SET(n, p)	(*p |= (1 << n))
#define FD_ISSET(n, p)	(*p & (1 << n))
#define FD_ZERO(p)	(*p = 0)
#define FD_CLR(n, p)	(*p &= ~(1 << n))
#endif
