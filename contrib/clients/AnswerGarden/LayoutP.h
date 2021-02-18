
/* LayoutP.h */

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
 * LayoutP.h - Private definitions for Layout widget
 * 
 */

#ifndef _LayoutP_h
#define _LayoutP_h

/***********************************************************************
 *
 * Layout Widget Private Data
 *
 ***********************************************************************/

#include "Layout.h"
#include <X11/CompositeP.h>

/* New fields for the Layout widget class record */
typedef struct {int empty;} LayoutClassPart;

/* Full class record declaration */
typedef struct _LayoutClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    LayoutClassPart	layout_class;
} LayoutClassRec;

extern LayoutClassRec layoutClassRec;

/* New fields for the Layout widget record */
typedef struct {
    Dimension	last_query_width, last_query_height;
    XtGeometryMask last_query_mode;
} LayoutPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _LayoutRec {
    CorePart	    core;
    CompositePart   composite;
    LayoutPart 	    layout;
} LayoutRec;

#endif /* _LayoutP_h */

