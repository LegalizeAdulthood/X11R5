/*
 * Copyright 1991 University of Wisconsin-Madison
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of Wisconsin-Madison not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  The University of
 * Wisconsin-Madison makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF WISCONSIN-MADISON DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF WISCONSIN-MADISON BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:   Marvin Solomon
 *           Computer Sciences Department
 *           University of Wisconsin, Madison WI
 *           solomon@cs.wisc.edu or seismo!uwvax!solomon
 *
 * Current Maintainer:
 *
 *           Tim Theisen           Systems Programmer
 * Internet: tim@cs.wisc.edu       Department of Computer Sciences
 *     UUCP: uwvax!tim             University of Wisconsin-Madison
 *    Phone: (608)262-0438         1210 West Dayton Street
 *      FAX: (608)262-9777         Madison, WI   53706
 */
/* The header of the font file, with all occurence of pointsize replaced
 * by '%'.  Lots of the information (e.g. font bounding box) is really
 * a lie:  It is perhaps approximately right for 10 point, but certainly
 * wrong for other sizes.  However, the current application (xproof) ignores
 * this stuff anyhow, so it doesn't matter.
 *
 * $Header: /src/X11/uw/xproof/RCS/bdfheader.h,v 1.3 1991/10/04 22:05:44 tim Exp $
 */
static struct _font_sizes {
	int point_size, resolution, pixel_size, average_width;
} font_sizes[] = {
	 8,  75,  8,  51,
	10,  75, 10,  61,
	12,  75, 12,  74,
	14,  75, 14,  85,
	18,  75, 18, 107,
	24,  75, 24, 142,
	 8, 100, 11,  61,
	10, 100, 14,  85,
	12, 100, 17,  95,
	14, 100, 20, 107,
	18, 100, 25, 142,
	24, 100, 34, 191,
	 0,   0,  0,   0
};

char *header[] = {
	"STARTFONT 2.1",
	"COMMENT Fake characters for ditroff (device-independent troff)",
	"COMMENT ",
	"FONT -Adobe-DIThacks-Medium-R-Normal--#-%0-@-@-P-$-ADOBE-FONTSPECIFIC",
	"SIZE % @ @",
	"FONTBOUNDINGBOX 11 14 -1 -4",
	"STARTPROPERTIES 23",
	"COMMENT Begin LogicalFontDescription",
	"FONTNAME_REGISTRY \"\"",
	"FAMILY_NAME \"DIThacks\"",
	"FOUNDRY \"Adobe\"",
	"WEIGHT_NAME \"Medium\"",
	"SETWIDTH_NAME \"Normal\"",
	"SLANT \"R\"",
	"ADD_STYLE_NAME \"\"",
	"PIXEL_SIZE #",
	"POINT_SIZE %0",
	"RESOLUTION_X @",
	"RESOLUTION_Y @",
	"SPACING \"P\"",
	"AVERAGE_WIDTH $",
	"CHARSET_REGISTRY \"\"",
	"CHARSET_ENCODING \"\"",
	"COMMENT END LogicalFontDescription",
	"CHARSET_COLLECTIONS \"\"",
	"FONT_DESCENT 2",
	"FONT_ASCENT 8",
	"CAP_HEIGHT 7",
	"X_HEIGHT 5",
	"FULL_NAME \"DIThacks\"",
	"DEVICE_FONT_NAME \"DIThacks\"",
	"COPYRIGHT \"Share and Enjoy\"",
	"COMMENT ***** end of inserted font properties",
	"ENDPROPERTIES",
	"CHARS 22",
	"STARTCHAR space",
	"ENCODING 32",
	"SWIDTH 250 0",
	"DWIDTH 3 0",
	"BBX 1 1 0 0",
	"BITMAP",
	"00",
	"ENDCHAR",
	0
};
