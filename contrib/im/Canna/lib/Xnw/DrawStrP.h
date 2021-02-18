/*
* $XConsortium: DrawStrP.h,v 1.24 89/06/08 18:05:01 swick Exp $
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
 * DrawStrP.h - Private definitions for DrawStr widget
 * 
 */

#ifndef _XawDrawStrP_h
#define _XawDrawStrP_h

/***********************************************************************
 *
 * DrawStr Widget Private Data
 *
 ***********************************************************************/

#include "DrawStr.h"
#include "SimpleP.h"

/* New fields for the DrawStr widget class record */

typedef struct {int foo;} DrawStrClassPart;

/* Full class record declaration */
typedef struct _DrawStrClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
    DrawStrClassPart	drawStr_class;
} DrawStrClassRec;

extern DrawStrClassRec drawStrClassRec;

/* New fields for the DrawStr widget record */
typedef struct {
    /* resources */
    Pixel	foreground;
    Pixel	background;
    XFontStruct	*font;
    XFontStruct	*kanji_font;
    XFontStruct	*kana_font;
    char	*label;
    Boolean	reverse;

    /* private state */
    GC		normal_GC;
    GC		kanji_normal_GC;
    GC		kana_normal_GC;
    GC          reverse_GC;
    GC          kanji_reverse_GC;
    GC          kana_reverse_GC;
    Position	label_x;
    Position	label_y;
    Dimension	label_width;
    Dimension	label_height;
    Dimension	label_len;
    int		max_ascent, max_descent;
} DrawStrPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _DrawStrRec {
    CorePart	core;
    SimplePart	simple;
    DrawStrPart	drawStr;
} DrawStrRec;

#endif /* _XawDrawStrP_h */
