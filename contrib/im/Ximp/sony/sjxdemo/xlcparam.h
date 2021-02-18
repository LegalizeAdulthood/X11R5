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
 * $Header: xlcparam.h,v 1.1 91/04/29 17:57:18 masaki Exp $ SONY;
 */

#define MAXCOLUMN	80
#define MAXLINE		24

#define BUFFLENGTH	128

#define MAIN_WIDTH	400
#define MAIN_HEIGHT	140
#define DOWN_OFFSET	20

#define FONT_W		8
#define FONT_H		16
#define INIT_X		0
#define INIT_Y		FONT_H -2;
#define LINE_S		2
#define LINE_H		(FONT_H+LINE_S)
#define ICON_O		4

#define MOVE_THRESHOLD	4	/* pixel	*/
#define TIME_THRESHOLD	400	/* millisecond	*/

typedef struct _wininfo {
	int	x;
	int	y;
	int	width;
	int	height;
} wininfo;

typedef struct _drawset {
	unsigned long	fore;
	unsigned long	back;
	Font		font;
	Font		jfont;
} drawset;

