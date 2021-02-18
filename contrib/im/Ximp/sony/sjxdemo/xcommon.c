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
 *	xcommon.c
 *	X11 common routine
 *	Copyright (c) 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Mon Jun 27 21:59:16 JST 1988
 */
/*
 * $Header: xcommon.c,v 1.1 91/04/29 17:57:16 masaki Exp $ SONY;
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <ctype.h>
#include "sjctype.h"

#include "xlcparam.h"

/*	X11 data		*/

int			screen;
int			depth;
int			bwidth = -1;
unsigned long		fore, back;
int			bord;
char			*forecolor, *backcolor;
char			*geom;
char			*display = NULL;
Display			*dpy = NULL;
Window			root;
Window			win;
Window			def_txtwin;
Visual			*visual;
XSetWindowAttributes	attributes;
unsigned long		mask;
XEvent			event;
XSizeHints		shint;
GC			gcFore;
GC			gcBack;
GC			gcFore16;
GC			gcBack16;
XGCValues		gcv;

static wininfo		txt;

static int		under = 0;
static int		reverse = 0;

int			ascent;
int			text_x		= INIT_X;
int			text_y		= INIT_Y;
int			l_height	= LINE_H;
static int		org_x		= INIT_X;
static int		org_y		= INIT_Y;
static int		f_width		= FONT_W;
static int		f_height	= FONT_H;
static int		l_space		= LINE_S;

static int		line = 0;
static int		column = 0;

static int		max_line;
static int		max_column;

static char		**sbuff;

set_screen (x, y, width, height)
int	x, y, width, height;
{
	txt.x = x;
	txt.y = y;
	txt.width = width;
	txt.height = height;
	max_line = height / l_height;
	max_column = width / f_width;
}

AllocScreen ()
{
	register int	i;

	if ((sbuff = (char **) malloc (MAXLINE * sizeof(char *))) == NULL) {
		fprintf (stderr, "can't alloc memory\n");
		exit (1);
	}
	for (i = 0 ; i < MAXLINE ; i ++) {
		if ((sbuff[i] = (char *) malloc (MAXCOLUMN)) == NULL) {
			fprintf (stderr, "can't alloc memory\n");
			exit (1);
		}
	}
}

ClearScreen ()
{
	XClearWindow (dpy, win);
	line = 0;
	column = 0;
	sbuff[line][column] = 0;
	text_x = org_x;
	text_y = org_y;
	XFlush (dpy);
}

LineFeed (store)
{
	if (store) {
		sbuff[line][column] = 0;
		line ++;
		column = 0;
		if (line >= max_line || line >= MAXLINE) {
			ClearScreen ();
			return;
		}
		sbuff[line][column] = 0;
	}
	text_x = org_x;
	text_y += l_height;
}

RedrawScreen (dpy, win)
{
	register int	i, len;

	text_x = org_x;
	text_y = org_y;
	for (i = 0 ; i < line ; i ++) {
		DrawStrings (sbuff[i], strlen (sbuff[i]), 0);
		LineFeed (0);
	}
	if (*sbuff[line]) {
		DrawStrings (sbuff[line], strlen (sbuff[line]), 0);
	}
	XFlush (dpy);
}

write_screen (s, len)
char	*s;
int	len;
{
	strncpy (&(sbuff[line][column]), s, len);
	column += len;
	sbuff[line][column] = 0;
}

SetFontInfo (gc)
GC	gc;
{
	XFontStruct	*fs;

	fs = XQueryFont (dpy, XGContextFromGC (gc));
	f_width = fs->max_bounds.width / 2;
	ascent = fs->ascent;
	f_height = fs->ascent + fs->descent;
	l_height = f_height + l_space;
	text_y = org_y = fs->ascent;
}

set_wm_hints (xsh)
XSizeHints	*xsh;
{
	xsh->min_width  = f_width * 65;
	xsh->min_height = l_height * 6;
	xsh->max_width  = 0;
	xsh->max_height = 0;
	xsh->width_inc  = f_width;
	xsh->height_inc = l_height;
	xsh->flags |= PMinSize;
	xsh->flags |= PResizeInc;
}

set_resize_hint (dpy, xsh, geom)
Display		*dpy;
XSizeHints	*xsh;
char		*geom;
{
	int	result;

	xsh->width = MAIN_WIDTH;
	xsh->height = MAIN_HEIGHT;
	xsh->x = (DisplayWidth (dpy, screen) - xsh->width) / 2;
	xsh->y = DisplayHeight (dpy, screen)
		- xsh->height - DOWN_OFFSET;
	result = NoValue;
	if (geom) {
		result = XParseGeometry (
				geom, &xsh->x, &xsh->y,
				&xsh->width, &xsh->height
			);
		if ((result & WidthValue) && (result & HeightValue))
			xsh->flags |= USSize;
		if ((result & XValue) && (result & YValue))
			xsh->flags |= USPosition;
	}
	if (!(xsh->flags & USSize))
		xsh->flags |= PSize;
	if (!(xsh->flags & USPosition))
		xsh->flags |= PPosition;
}

write_window (buff, len)
unsigned char	*buff;
int		len;
{
	register int	c;
	register int	i;

	if (text_x > txt.width ||
		column >= MAXCOLUMN || column >= max_column) {
		LineFeed (1);			/* 1: store screen	*/
	}
	if (text_y > txt.height ||
		line >= MAXLINE || line >= max_line) {
		ClearScreen ();
	}

	c = buff[len - 1];
	if (c == '\n' || c == '\r')
		len --;

/*
	len = compound_to_sjis (buff, len);
*/

	DrawStrings (buff, len, 1);		/* 1: store screen	*/
	XFlush (dpy);

	if (c == '\n' || c == '\r') {
		LineFeed (1);			/* 1: store screen	*/
	}
}

/*
#define	isKanji(x)	issjis1(x)
*/
#define	isKanji(x)	iseuc(x)

DrawStrings (s, len, store)
register u_char	*s;
register int	len;
int		store;
{
	register u_char	*p;
	u_char		buff[2048];
	register int	kflag, aflag, storelen;

	p = buff;
	kflag = 0;
	aflag = 0;
	storelen = 0;
	while (len -- > 0) {
		if (isKanji(*s)) {
			if (aflag) {
				DrawText (buff, storelen, store);
				p = buff;
				aflag = 0;
				storelen = 0;
			}
			*(p ++) = *(s ++);
			*(p ++) = *(s ++);
			len --;
			storelen ++;
			storelen ++;
			kflag = 1;
		}
		else {
			if (kflag) {
				DrawText (buff, storelen, store);
				p = buff;
				kflag = 0;
				storelen = 0;
			}
			*(p ++) = *(s ++);
			storelen ++;
			aflag = 1;
		}
	}
	if (storelen)
		DrawText (buff, storelen, store);
}

DrawText (s, len, store)
register u_char	*s;
int		len;
int		store;
{
	if (isKanji(*s))
		DrawJText (s, len, store);
	else
		DrawAText (s, len, store);
}

DrawJText (s, len, store)
register u_char	*s;
int		len;
int		store;
{
	u_char		tmp[2048];
	register u_char	*t;
	register int	x, y;
	register int	avlen, wlen;
	GC		drawgc;

/*
	sjis_to_jis (s, tmp, len);
*/
	euc_to_jis (s, tmp, len);

	if (reverse)
		drawgc = gcBack16;
	else
		drawgc = gcFore16;

	t = tmp;
	while (len > 0) {
		avlen = (txt.width - text_x) / (f_width * 2) * 2;
		if (store && avlen >= MAXCOLUMN - column)
			avlen = MAXCOLUMN - column;
		if (store && avlen >= max_column - column)
			avlen = max_column - column;
		if (len > avlen)
			wlen = avlen;
		else
			wlen = len;
		XDrawImageString16 (
			dpy, win, drawgc, text_x, text_y, t, wlen/2
		);
		if (under) {
			x = text_x + f_width * wlen - 1;
			y = text_y + 1;
			XDrawLine (dpy, win, gcFore, text_x, y, x, y);
		}
		if (store)
			write_screen (s, wlen);
		if (len > avlen) {
			LineFeed (store);
		}
		else {
			text_x += f_width * wlen;
		}
		len -= wlen;
		t += wlen;
		s += wlen;
	}
}

DrawAText (s, len, store)
register u_char	*s;
int		len;
int		store;
{
	register int	x, y;
	register int	avlen, wlen;
	GC		drawgc;

	if (reverse)
		drawgc = gcBack;
	else
		drawgc = gcFore;

	while (len > 0) {
		avlen = (txt.width - text_x) / f_width;
		if (store && avlen >= MAXCOLUMN - column)
			avlen = MAXCOLUMN - column;
		if (store && avlen >= max_column - column)
			avlen = max_column - column;
		if (len > avlen)
			wlen = avlen;
		else
			wlen = len;
		XDrawImageString (
			dpy, win, drawgc, text_x, text_y, s, wlen
		);
		if (under) {
			x = text_x + f_width * wlen - 1;
			y = text_y + 1;
			XDrawLine (dpy, win, gcFore, text_x, y, x, y);
		}
		if (store)
			write_screen (s, wlen);
		if (len > avlen) {
			LineFeed (store);
		}
		else {
			text_x += f_width * wlen;
		}
		len -= wlen;
		s += wlen;
	}
}

sjis_to_jis (s, t, len)
register u_char	*s, *t;
register int	len;
{
	register int	i;

	len /= 2;
	while (len --) {
#ifdef	sony_news
		i = *(s ++) << 8;
		i |= *(s ++);
		i = sjis2jis (i);
		*(t ++) = i >> 8;
		*(t ++) = i & 0xff;
#else	/* sony_news	*/
		*t = *(s ++);
		*(t + 1) = *(s ++);
		sj_sjis2jis (t);
		t ++;
		t ++;
#endif	/* sony_news	*/
	}
}

euc_to_jis (s, t, len)
register u_char	*s, *t;
register int	len;
{
	while (len --)
		*(t ++) = *(s ++) & 0x7f;
}

#define	ESC		033
#define ESC_BEGIN1	'$'
#define ESC_BEGIN2	'('
#define	ESC_KANJI	'B'
#define	ESC_KANA	'I'
#define ESC_B_KANA	')'
#define ESC_B_ASCII	'-'
#define ESC_ASCII	'A'
#define ASCII_STAT	0
#define KANA_STAT	1
#define KANJI_STAT	2

compound_to_sjis (store, n)
u_char		*store;
register int	n;
{
	u_char		buff[2048];
	register u_char	*t, *s;
	register int	c;
	register int	count;
	register int	stat;

	s = store;
	t = buff;
	count = 0;
	stat = ASCII_STAT;
	while (n > 0) {
		c = *s++, n--;
		if (c == ESC) {
			c = *s++, n--;
			if (c == ESC_BEGIN1) {
				c = *s++, n--;
				if (c == ESC_BEGIN2) {
					c = *s++, n--;
					/* if (c == ESC_KANJI)	*/
					stat =  KANJI_STAT;
				}
			}
			else if (c == ESC_BEGIN2) {
				c = *s++, n--;
				if (c == ESC_KANA)
					stat = KANA_STAT;
				else	/*  if (c == ESC_KANJI) */
					stat = ASCII_STAT;
			}
			else if (c == ESC_B_KANA) {
				c = *s++, n--;
				if (c == ESC_KANA)
					stat = KANA_STAT;
				else
					stat = ASCII_STAT;
			}
			else if (c == ESC_B_ASCII) {
				c = *s++, n--;
				if (c == ESC_ASCII)
					stat = ASCII_STAT;
				else
					stat = ASCII_STAT;
			}
			else
				break;
			if (n <= 0)
				break;
			continue;
		}
		if (stat == KANJI_STAT) {
			c <<= 8;
			c |= *s++, n--;
			c = jis2sjis (c);
			*t++ = c >> 8, count++;
			*t++ = c & 0xff, count++;
		}
		else if (stat == KANA_STAT) {
			*t++ = c | 0x80, count++;
		}
		else {
			*t++ = c, count++;
		}
	}
	strncpy (store, buff, count);
	store[count] = 0;
	return (count);
}

