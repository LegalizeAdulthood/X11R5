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
 *	xicon.c
 *	Copyright (c) 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Tue Sep 20 20:48:38 JST 1988
 */
/*
 * $Header: xicon.c,v 1.1 91/09/04 23:08:36 root Locked $ SONY;
 */

#include <X11/Xlib.h>
#include "xcommon.h"
#include "sjx32.bitmap"

#define MAX_PIXMAP_WIDTH	48
#define MAX_PIXMAP_HEIGHT	48

XImage	*image;
Pixmap	icon_fore;
Pixmap	icon_back;

MakeIconPixmap (win, iconic)
Window	win;
int	iconic;
{
	int		width, height;

	MakePixmap (
		&icon_fore, &icon_back, &width, &height,
		sjx32_bits, sjx32_width, sjx32_height
	);
	MakeIcon (win, iconic);
}

MakePixmap (
	fore_pix, back_pix, width, height,
	def_bits, def_width, def_height
)
Pixmap	*fore_pix, *back_pix;
int	*width, *height;
char	*def_bits;
int	def_width, def_height;
{
	int		rval;
	int		w, h, x_hot, y_hot;
	Pixmap		tile;

	*fore_pix = XCreateBitmapFromData (
		dpy, subwin, def_bits, def_width, def_height
	);
	*back_pix = XCreateBitmapFromData (
		dpy, subwin, def_bits, def_width, def_height
	);
	*width = def_width;
	*height = def_height;
}

MakeIcon (win, iconic)
Window	win;
int	iconic;
{
	XWMHints	wmhints;

	wmhints.flags = IconPixmapHint | StateHint;
	wmhints.icon_pixmap = icon_fore;
	if (iconic)
		wmhints.initial_state = IconicState;
	else
		wmhints.initial_state = NormalState;
	XSetWMHints (dpy, win, &wmhints);
}
