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
 *	xinput.c
 *	Copyright (c) 1988 1989 Sony Corporation.
 *	Modified by Masaki Takeuchi.
 *	Thu Sep  8 13:49:16 JST 1988
 *	From jterm written by Michael Knight.
 */
/*
 * $Header: xinput.c,v 1.1 91/09/04 23:08:36 root Locked $ SONY;
 */

#define XK_KATAKANA
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>

#define IsKanaKey(keysym) \
	(((unsigned)(keysym) >= XK_overline)\
	&& ((unsigned)(keysym) <= XK_semivoicedsound))

XLookupKanaString (event, buf, nbuf, keycode, xcs)
XKeyEvent	*event;
char		*buf;
int		nbuf;
KeySym		*keycode;
XComposeStatus	*xcs;
{
	int		nbytes;

	nbytes = XLookupString (event, buf, nbuf, keycode, xcs);
	if (IsKanaKey (*keycode)) {
		*buf = *keycode & 0xff;
		nbytes = 1;
	}
	return (nbytes);
}
