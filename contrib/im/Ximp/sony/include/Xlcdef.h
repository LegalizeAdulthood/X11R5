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
 *	Xlcdef.h
 *	Copyright (c) 1989 1990 Sony Corporation
 *	Fri Feb  3 15:40:25 JST 1989
 *	Mon Jun 18 13:16:47 JST 1990
 *	X Language Conversion Protocol
 */
/*
 * $Header: Xlcdef.h,v 1.1 91/04/29 17:57:38 masaki Exp $ SONY;
 *
 * $Author: masaki $
 * $Date: 91/04/29 17:57:38 $
 * $Locker:  $
 * $Revision: 1.1 $
 * $State: Exp $
 */

#define CONVERSION_INPLACE	"_XLC_ON_THE_SPOT"
#define CONVERSION_STATUS	"_XLC_STATUS"

#define	COMPOUND_TEXT		"COMPOUND_TEXT"

typedef struct _InplaceWindowAttributes {
	unsigned long		back;
	unsigned long		border;
	unsigned int		bwidth;
} InplaceWindowAttributes;

typedef struct _InplaceDrawingSet {
	unsigned long		fore;
	unsigned long		back;
	Font			font8;
	Font			font16;
	Font			efont16;
} InplaceDrawingSet;

typedef struct _InplaceFrame {
	unsigned int		x;
	unsigned int		y;
	unsigned int		width;
	unsigned int		height;
	unsigned int		x_off;
	unsigned int		y_off;
	unsigned int		line_height;
} InplaceFrame;

typedef struct _InplaceInfo {
	unsigned int		in_flag;
	Window			in_win;
	InplaceWindowAttributes	in_attr;
	InplaceDrawingSet	in_draw;
	InplaceFrame		in_frame;
} InplaceInfo;

#define AutoReplace		1
#define AllInformation		2
#define FrameInformation	4
#define OffsetInformation	8

