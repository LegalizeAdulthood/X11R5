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
 *
 *		DrawStrings (s, len)
 *			char	*s;
 *			int	len;
 */
/*
 * $Header: xcommon.c,v 1.1 91/09/04 23:08:34 root Locked $ SONY;
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "sj2.h"
#include "sjxparam.h"

extern int	vflag;

extern Draws	*cur_draws;

/*	X11 data		*/

int			screen;
int			depth;
int			bwidth = -1;
unsigned long		fore, back;
int			g_fore, g_back;
int			bord;
char			*forecolor, *backcolor;
char			*display = NULL;
Display			*dpy = NULL;
Window			root;
Window			root0;
Window			win;
Window			topwin;
Window			subwin;
Window			txtwin;
Window			def_txtwin;
Visual			*visual;
XSetWindowAttributes	attributes;
XEvent			event;
XSizeHints		shint;
GC			gcTFore;
GC			gcTBack;
GC			gcTJFore;
GC			gcTJBack;
GC			gcCFore;
GC			gcCBack;
GC			gcCJFore;
GC			gcCJBack;
GC			gcGFore;
GC			gcGBack;
GC			gcGJFore;
GC			gcGJBack;
GC			gcTEFore;
GC			gcTEBack;
GC			gcCEFore;
GC			gcCEBack;
GC			gcGEFore;
GC			gcGEBack;
GC			gcMove;
GCs			*t_gcs;
GCs			*g_gcs;
GCs			*c_gcs;

/*
 *	Extra Character Area
 */

int	extra_hi = 0xff;
int	extra_lo = 0xff;

set_ecode (ecode)
register char	*ecode;
{
	char	num[3];

	if (strlen (ecode) != 4)
		return (1);
	strncpy (num, ecode, 2);
	num[2] = 0;
	extra_hi = Htoi (num);
	strncpy (num, &ecode[2], 2);
	num[2] = 0;
	extra_lo = Htoi (num);
	return (0);
}

Htoi (s)
register u_char	*s;	/* 2 hex digit char	*/
{
	return (h_toi (*s) << 4 | h_toi (s[1]));
}

h_toi (c)
register u_char	c;
{
	if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	else if (c >= '0' && c <= '9')
		return (c - '0');
	else
		return (0);
}

isExtra (s)
register u_char	*s;	/* shift-jis code */
{
	if (*s > extra_hi || (*s == extra_hi && s[1] >= extra_lo))
		return (1);
	else
		return (0);
}

/*
 *	Drawing Routines
 */

DrawStrings (s, len)
register u_char	*s;
register int	len;
{
	register u_char	*p;
	u_char		buff[BUFFLENGTH * 2];
	register int	cflag;
	register int	storelen;

#define	ACHAR	1
#define KCHAR	2
#define ECHAR	3

	p = buff;
	cflag = 0;
	storelen = 0;
	while (len -- > 0) {
		if (isExtra (s)) {
			if (cflag != ECHAR) {
				DrawText (buff, storelen);
				p = buff;
				storelen = 0;
			}
			*(p ++) = *(s ++);
			*(p ++) = *(s ++);
			len --;
			storelen ++;
			storelen ++;
			cflag = ECHAR;
		} else
		if (issjis1 (*s)) {
			if (cflag != KCHAR) {
				DrawText (buff, storelen);
				p = buff;
				storelen = 0;
			}
			*(p ++) = *(s ++);
			*(p ++) = *(s ++);
			len --;
			storelen ++;
			storelen ++;
			cflag = KCHAR;
		}
		else {
			if (cflag != ACHAR) {
				DrawText (buff, storelen);
				p = buff;
				storelen = 0;
			}
			*(p ++) = *(s ++);
			storelen ++;
			cflag = ACHAR;
		}
	}
	if (storelen)
		DrawText (buff, storelen);
}

static
DrawText (s, len)
register u_char	*s;
int		len;
{
	register DrawingSet	*d;

	d = cur_draws->c_draw;
	if (d->mapsw && d->unmap)
		MapInplaceWindow (d);
	if (d->y + d->font_height > d->y2) {
		return (-1);
	}

	if (issjis1 (*s))
		DrawMText (s, len);
	else
		DrawAText (s, len);
}

/*
 *	DrawMText
 *	Draw the multi byte character text.
 */
static
DrawMText (s, len)
register u_char	*s;
int		len;
{
	register int		x, y;
	register int		avlen, wlen;
	GC			drawgc;
	GC			foregc;
	register DrawingSet	*d;

	d = cur_draws->c_draw;
	if (isExtra (s)) {
		if (d->reverse)
			drawgc = d->gcs->e_back_gc;
		else
			drawgc = d->gcs->e_fore_gc;
	}
	else {
		if (d->reverse)
			drawgc = d->gcs->j_back_gc;
		else
			drawgc = d->gcs->j_fore_gc;
	}
	foregc = d->gcs->fore_gc;

	while (len > 0) {
		avlen = (d->x2 - d->x1 - d->text_x)
			/ ((int)d->font_width * 2) * 2;
		if (avlen <= 0) {
			d->text_x = 0;
			d->text_y += d->line_height;
			if (d->height <= (d->text_y - d->y))
			    d->height += d->line_height;
			if (d->y + d->text_y > d->y2) {
			    return (-1);
			}
			continue;
		}
		if (len > avlen)
			wlen = avlen;
		else
			wlen = len;

		DrawImageString16 (dpy, d, drawgc, foregc, s, wlen);

		d->text_x += d->font_width * wlen;
		if (d->ex_window && (d->text_y - (d->y - d->y1)) > d->line_height) {
			if (d->ex_w < d->text_x)
				d->ex_w = d->text_x;
		}
		else if (d->width < d->text_x - (d->x - d->x1))
			d->width = d->text_x - (d->x - d->x1);
		if (d->mapsw)
			ConfigWindow (d);
		len -= wlen;
		s += wlen;
	}
	return (0);
}

/*
 *	DrawAText
 *	Draw the ascii character text
 */
static
DrawAText (s, len)
register u_char	*s;
int		len;
{
	register int		x, y;
	register int		avlen, wlen;
	GC			drawgc;
	GC			foregc;
	register DrawingSet	*d;

	d = cur_draws->c_draw;
	if (d->reverse)
		drawgc = d->gcs->back_gc;
	else
		drawgc = d->gcs->fore_gc;
	foregc = d->gcs->fore_gc;

	while (len > 0) {
		avlen = (d->x2 - d->x1 - d->text_x) / (int)d->font_width;
		if (avlen <= 0) {
			d->text_x = 0;
			d->text_y += d->line_height;
			if (d->height <= (d->text_y - d->y))
				d->height += d->line_height;
			if (d->y + d->text_y > d->y2) {
				return (-1);
			}
			continue;
		}
		if (len > avlen)
			wlen = avlen;
		else
			wlen = len;

		DrawImageString (dpy, d, drawgc, foregc, s, wlen);

		d->text_x += d->font_width * wlen;
		if (d->ex_window && (d->text_y - (d->y - d->y1)) > d->line_height) {
			if (d->ex_w < d->text_x)
				d->ex_w = d->text_x;
		}
		else if (d->width < d->text_x - (d->x - d->x1))
			d->width = d->text_x - (d->x - d->x1);
		if (d->mapsw)
			ConfigWindow (d);
			/*
			 * If do ConfigWindow before doing DrawImageString,
			 * Expose event will not occured.
			 * It may be good.
			 */
		len -= wlen;
		s += wlen;
	}
	return (0);
}

static
DrawImageString16 (dpy, d, dgc, fgc, s, len)
Display			*dpy;
register DrawingSet	*d;
GC			dgc;
GC			fgc;
char			*s;
int			len;
{
	u_char			tmp[BUFFLENGTH * 2];
	register int		tx, ty;
	register Window		win;

	sjis_to_jis (s, tmp, len);

	if (d->ex_window && (d->text_y - (d->y - d->y1)) > d->line_height) {
		win = d->ex_window;
		ty = d->text_y - (d->y - d->y1) - d->line_height;
		tx = d->text_x;
	}
	else {
		win = d->window;
		tx = d->text_x - (d->x - d->x1);
		ty = d->text_y - (d->y - d->y1);
	}

	XDrawImageString16 (dpy, win, dgc, tx, ty, tmp, len/2);

	if (d->under)
		DrawLine (dpy, win, fgc, d, tx, ty, len);
}

static
DrawImageString (dpy, d, dgc, fgc, s, len)
Display			*dpy;
register DrawingSet	*d;
GC			dgc;
GC			fgc;
char			*s;
int			len;
{
	register int	tx, ty;
	register Window	win;

	if (d->ex_window && (d->text_y - (d->y - d->y1)) > d->line_height) {
		win = d->ex_window;
		ty = d->text_y - (d->y - d->y1) - d->line_height;
		tx = d->text_x;
	}
	else {
		win = d->window;
		tx = d->text_x - (d->x - d->x1);
		ty = d->text_y - (d->y - d->y1);
	}

	XDrawImageString (dpy, win, dgc, tx, ty, s, len);

	if (d->under)
		DrawLine (dpy, win, fgc, d, tx, ty, len);
}

static
DrawLine (dpy, win, gc, d, tx, ty, len)
Display			*dpy;
Window			win;
GC			gc;
register DrawingSet	*d;
int			tx;
int			ty;
int			len;
{
	register int	x, y;

	x = tx + d->font_width * len - 1;
	y = ty + 1;
	XDrawLine (dpy, win, gc, tx, y, x, y);
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
