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
 *	sj_misc.c
 *	sj_ctype function.
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi
 *	Sat May  9 01:44:52 JST 1987
 */
/*
 * $Header: sj_misc.c,v 1.1 91/04/29 17:56:47 masaki Exp $ SONY;
 */

#include "sjparam.h"
#include "rtbl.h"

IsaTOKUSYU (c)
register int	c;
{
	if ((c >= ' ' && c <= '@') || (c >= '[' && c <= '`') ||
	    (c >= '{' && c <= '~'))
		return (1);
	else
		return (0);
}

IsaBOIN (c)
register int	c;
{
	if (c == 'a' || c == 'i' || c == 'u' || c == 'e' || c == 'o' ||
	    c == 'A' || c == 'I' || c == 'U' || c == 'E' || c == 'O')
		return (1);
	else
		return (0);
}

IsaN (c)
register int	c;
{
	if (c == 'n' || c == 'N')
		return (1);
	else
		return (0);
}

tolowerstr (from, to)
register u_char	*from, *to;
{
	register u_char	c;

	while ((c = *(from++)) != '\0') {
		if (isupper (c))
			*(to++) = tolower (c);
		else
			*(to++) = c;
	}
	*to = '\0';
}

IsaSHIIN (c)
register u_char	c;
{
	if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')) {
		if (!IsaBOIN ((int)c) && c != 'n' && c != 'N')
			return (1);
	}
	return (0);
}

