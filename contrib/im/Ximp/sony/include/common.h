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
 *	common.h
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Sat May  9 01:44:52 JST 1987
 */
/*
 * $Header: common.h,v 1.1 91/04/29 17:57:39 masaki Exp $ SONY;
 */

/* buffers			*/

#include "conversion.h"

extern Conversion	*current_conversion;

extern u_char	PZbuff[];
extern u_char	PHbuff[];
extern u_char	Object[];

extern int	allways_buff;		/* allways buff mode		*/
extern int	no_flush;		/* kettei flush mode		*/
extern int	termflag;		/* terminal identifier		*/
extern int	HHlevel;		/* help level			*/
extern int	Hlevel;			/* help level			*/
extern int	oasys;			/* OYAYUBI Shift		*/
extern int	flush_conversion;	/* Flush After Conversion	*/

Conversion	*AllocConversion ();
Conversion	*GetConversion ();
