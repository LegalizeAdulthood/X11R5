/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
 *
 * "Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty."
 *
 */

extern pr_size  pf_textwidth();
extern PIX_FONT bold_font;
extern PIX_FONT roman_font;
extern PIX_FONT canvas_font;
extern PIX_FONT canv_zoomed_font;
extern PIX_FONT button_font;

#define		NORMAL_FONT	"fixed"
#define		BOLD_FONT	"8x13bold"
#define		BUTTON_FONT	"6x13"

#define		char_height(font) \
		((font)->max_bounds.ascent + (font)->max_bounds.descent)
#define		char_width(font)	((font)->max_bounds.width)
#define		char_advance(font,char)	\
		    (((font)->per_char)?\
		    ((font)->per_char[(char)-(font)->min_char_or_byte2].width):\
		    ((font)->max_bounds.width))
