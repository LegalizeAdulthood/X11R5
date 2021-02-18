/*
 * Copyright 1991 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          580 Theory Center, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@graphics.cornell.edu
 */

/* 
 * LabelP.h - Private definitions for XcuLabel widget
 */

#ifndef _Xcu_LabelP_h
#define _Xcu_LabelP_h

#include <X11/Xcu/Label.h>
#include <X11/Xcu/SimpleP.h>

/* New fields for the XcuLabel widget class record */

typedef struct
    {
    void (*redisplay_text)() ;
    } XcuLabelClassPart;


/* Full class record declaration */
typedef struct _XcuLabelClassRec
    {
    CoreClassPart	core_class ;
    XcuSimpleClassPart	simple_class ;
    XcuLabelClassPart	label_class ;
    } XcuLabelClassRec ;

extern XcuLabelClassRec xcuLabelClassRec ;

/* New fields for the XcuLabel widget record */

typedef struct
    {
    /* resources */
    Pixel	foreground ;
    XFontStruct	*font ;
    String	label ;
    String	big_label ;
    XcuJustify	justify ;
    XcuGravity	gravity ;
    Dimension	internal_width ;
    Dimension	internal_height ;
    float	line_factor ;
    Pixmap	pixmap ;
    String	bitmap ;
    Boolean	tile ;
    Boolean	preserve_text ;

    /* private state */
    Dimension	pixmap_width ;
    Dimension	pixmap_height ;
    Dimension	internal_width0 ;
    Dimension	internal_height0 ;
    Dimension	facet_width0 ;
    GC		sensitive_text_GC ;
    GC		insensitive_text_GC ;
    GC		background_GC ;
    GC		pixmap_GC ;
    Position	*x ;
    Position	*y ;
    int		total_height ;
    int		total_width ;
    int		depth ;
    int		big_height ;
    int		big_width ;
    int		line_height ;
    int		*line_width ;
    unsigned int *len ;
    unsigned int *locations ;
    unsigned int n_lines ;
    Boolean	newlines_found ;
    Boolean	needs_fill ;
    } XcuLabelPart ;


/*
 * Full instance record declaration
 */

typedef struct _XcuLabelRec
    {
    CorePart	 core ;
    XcuSimplePart simple ;
    XcuLabelPart	 label ;
    } XcuLabelRec ;

/*
 * Inheritance definitions
 */

#define XtInheritRedisplayText ((XtProc) _XtInherit)

#endif /* _Xcu_LabelP_h */
