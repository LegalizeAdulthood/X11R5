/*
* $XConsortium: LabelLcP.h,v 1.27 91/06/22 19:34:58 rws Exp $
*/


/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* 
 * LabelLcP.h - Private definitions for LabelLocale widget
 * 
 */

#ifndef _XawLabelLocaleP_h
#define _XawLabelLocaleP_h

/***********************************************************************
 *
 * LabelLocale Widget Private Data
 *
 ***********************************************************************/

#include <X11/Xaw/LabelLc.h>
#include <X11/Xaw/SimpleP.h>

/* New fields for the LabelLocale widget class record */

typedef struct {int foo;} LabelLocaleClassPart;

/* Full class record declaration */
typedef struct _LabelLocaleClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
    LabelLocaleClassPart	label_class;
} LabelLocaleClassRec;

extern LabelLocaleClassRec labelLocaleClassRec;

/* New fields for the LabelLocale widget record */
typedef struct {
    /* resources */
    Pixel	foreground;
    XFontSet fontset;
    char	*label;
    XtJustify	justify;
    Dimension	internal_width;
    Dimension	internal_height;
    Pixmap	pixmap;
    Boolean	resize;
    Pixmap	left_bitmap;

    /* private state */
    GC		normal_GC;
    GC          gray_GC;
    Pixmap	stipple;
    Position	label_x;
    Position	label_y;
    Dimension	label_width;
    Dimension	label_height;
    Dimension	label_len;
    int		lbm_y;			/* where in label */
    unsigned int lbm_width, lbm_height;	 /* size of pixmap */
} LabelLocalePart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _LabelLocaleRec {
    CorePart	core;
    SimplePart	simple;
    LabelLocalePart	label;
} LabelLocaleRec;

#define LEFT_OFFSET(lw) ((lw)->label.left_bitmap \
			 ? (lw)->label.lbm_width + (lw)->label.internal_width \
			 : 0)

#endif /* _XawLabelLocaleP_h */
