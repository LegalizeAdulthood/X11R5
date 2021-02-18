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
 *	display.c
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Fri May 29 15:45:18 JST 1987
 */
/*
 *	Modified by Masaki Takeuchi.
 *	For X11 display.
 *	Fri Jul  8 17:22:27 JST 1988
 */
/*
 * $Header: xdisplay.c,v 1.1 91/09/04 23:08:35 root Locked $ SONY;
 */

#include <stdio.h>
#include "common.h"
#include "xcommon.h"
#include "sjx.h"

extern int	vflag;

extern int	KB_mode;
extern int	oasys;
extern char	*GKanaMode, *GRomaMode;

#define XMODE_W	10

static char	*InputMode;
static char	InputKana[XMODE_W];
static char	InputRoma[XMODE_W];

static char	*space_buff;
static int	space_lim;

extern Draws	*cur_draws;

xdisplay_init ()
{
	char		*malloc ();
	register char	*c;
	register int	i;

	space_lim = BUFFLENGTH * 2;
	if ((space_buff = malloc (space_lim)) == (char *)NULL) {
		fprintf (stderr, "sjx: can't alloc memories\n");
		exit (1);
	}
	c = space_buff;
	for (i = 0 ; i < space_lim ; i ++)
		*(c++) = ' ';
}

put_space (n)
int	n;
{
	while (n) {
		if (n > space_lim) {
			DrawStrings (space_buff, space_lim);
			n -= space_lim;
		}
		else {
			DrawStrings (space_buff, n);
			n = 0;
		}
	}
}

Backspace ()
{
	register DrawingSet	*d;

	d = cur_draws->c_draw;
	d->text_x -= d->font_width;
	if (d->text_x < 0)
		d->text_x = 0;
}

master_flush ()
{
	flush_to_client ();
}

under_in ()
{
	cur_draws->c_draw->under = 1;
}

under_out ()
{
	cur_draws->c_draw->under = 0;
}

reverse_in ()
{
	cur_draws->c_draw->reverse = 1;
}

reverse_out ()
{
	cur_draws->c_draw->reverse = 0;
}

/* bell				*/

beep ()
{
	XBell (dpy, 100);
	Flush ();
}

/* cursor manupilation		*/

CursorSet (row, col)
u_short	row, col;
{
	register DrawingSet	*d;

	d = cur_draws->c_draw;
	d->text_x = d->origin_x + d->font_width * --col;
	d->text_y = d->origin_y + d->line_height * --row;
}

CursorRead (row, col)
u_short	*row, *col;
{
	register DrawingSet	*d;

	d = cur_draws->c_draw;
	*col = (d->text_x - d->origin_x) / d->font_width + 1;
	*row = (d->text_y - d->origin_y) / d->line_height + 1;
}

/* clear end of line	*/

Clear_EOL ()
{
	register DrawingSet	*d;
	u_short			row, col;

	d = cur_draws->c_draw;
	reverse_out ();
	CursorRead (&row, &col);
	put_space (d->column - col + 2);
	CursorSet (row, col);
}

StartGuide (row, col)
u_short	*row, *col;
{
	GotoSubwin ();
}

EndGuide (row, col)
u_short	row, col;
{
	GotoTxtwin ();
}

Flush ()
{
	XFlush (dpy);
}

/*
 *	only x display routine
 */

DrawScreen ()
{
	if (IsHenkan ())
		Bdisp (0);
	else if (!IsCflag ())
		Sdisp (0);
}

static u_char	cursor[3];

CursorOn (s)
register u_char	*s;
{
	if (s == (u_char *)0 || iskanji (*s)) {
		if (s == (u_char *)0) {
			cursor[0] = 0x81;
			cursor[1] = 0x40;
		}
		else {
			cursor[0] = *s ++;
			cursor[1] = *s;
		}
		cursor[2] = '\0';
		printR (cursor);
		Backspace ();
		Backspace ();
	}
	else {
		cursor[0] = *s;
		cursor[1] = '\0';
		printR (cursor);
		Backspace ();
	}
}

CursorOff ()
{
	if (*cursor == 0) {
		;
	}
	else if (iskanji (*cursor)) {
		SJ_print (cursor);
		Backspace ();
		Backspace ();
	}
	else {
		SJ_print (cursor);
		Backspace ();
	}
}

Cscroll ()
{
	/* scroll off	*/
	return (0);
}

CleanUp ()
{
	register DrawingSet	*d;

	d = cur_draws->c_draw;
	if (d->auto_replace) {
		if (d->text_y > (d->y - d->y1) + d->line_height) {
			d->y = d->y1 + d->text_y - d->origin_y;
			if (d->y + d->font_height > d->y2)
				d->y -= d->line_height;
			d->ex_y = d->y + d->line_height;
		}
		d->x = d->text_x;
	}
	d->ex_w = 1;
	d->width = 1;
	UnmapInplaceWindow (d);
}

Reconfigure ()
{
	register DrawingSet	*d;

	d = cur_draws->c_draw;
	if (d) {
		while ((d->text_y - d->y) + d->line_height < d->height)
			d->height -= d->line_height;
		if (d->text_y == (d->y - d->y1) + d->origin_y) {
			d->ex_w = 1;
			if (d->text_x < (d->x - d->x1) + d->font_width) {
				d->width = 1;
				UnmapInplaceWindow (d);
			}
			else if (d->width > d->text_x - (d->x - d->x1)) {
				d->width = d->text_x - d->x;
			}
		}
		if (d->ex_window &&
			d->text_y > (d->y - d->y1) + d->line_height) {
			if (d->text_y < (d->y - d->y1) + d->line_height * 2) {
				if (d->ex_w > d->text_x)
					d->ex_w = d->text_x;
			}
		}
		ConfigWindow (d);
	}
}

IsTxtWindow ()
{
        register DrawingSet     *d;

        d = cur_draws->c_draw;
	if (d->window == txtwin)
		return (1);
	else
		return (0);
}

