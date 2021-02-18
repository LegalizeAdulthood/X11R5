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
 *	sjxparam.h
 *	Copyright (c) 1988 1989 Sony Corporation.
 */
/*
 * $Header: sjxparam.h,v 1.1 91/09/04 23:09:09 root Locked $ SONY;
 */

#define MOVE_THRESHOLD	4	/* pixel	*/
#define TIME_THRESHOLD	400	/* millisecond	*/

typedef struct	_wininfo {
	int		x;
	int		y;
	int		width;
	int		height;
} wininfo;

typedef struct	_drawset {
	unsigned long	fore;
	unsigned long	back;
	Font		font;
	Font		jfont;
} drawset;

typedef struct	_GCs {
	GC		fore_gc;
	GC		back_gc;
	GC		j_fore_gc;
	GC		j_back_gc;
	GC		e_fore_gc;
	GC		e_back_gc;
} GCs;

typedef struct	_DrawingSet {
	Window		window;
	int		x;
	int		y;
	unsigned	width;
	unsigned	height;
	Window		ex_window;
	int		ex_x;
	int		ex_y;
	unsigned	ex_w;
	unsigned	ex_h;
	int		origin_x;
	int		origin_y;
	int		text_x;
	int		text_y;
	unsigned	line;
	unsigned	column;
	unsigned	font_ascent;
	unsigned	font_width;
	unsigned	font_height;
	unsigned	line_height;
	int		x1;
	int		y1;
	int		x2;
	int		y2;
	GCs		*gcs;
	u_char		reverse;
	u_char		under;
	u_char		mapsw;
	u_char		unmap;
	u_char		ex_unmap;
	u_char		auto_replace;
} DrawingSet;

typedef struct	_Draws {
	DrawingSet	*c_draw;
	DrawingSet	*t_draw;
	DrawingSet	*g_draw;
} Draws;

