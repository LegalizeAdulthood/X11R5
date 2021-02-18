/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of NEC
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  NEC Corporation makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */

/*

  This program is made by modifying LabelP.h.

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
 * HenkanP.h - Private definitions for Henkan widget
 * 
 */

#ifndef _XnwHenkanP_h
#define _XnwHenkanP_h

/***********************************************************************
 *
 * Henkan Widget Private Data
 *
 ***********************************************************************/

#include "Henkan.h"
#include <X11/kanji.h>
#include "SimpleP.h"

/* New fields for the Henkan widget class record */

typedef struct {int foo;} HenkanClassPart;

/* Full class record declaration */
typedef struct _HenkanClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
    HenkanClassPart	henkan_class;
} HenkanClassRec;

extern HenkanClassRec henkanClassRec;

/* New fields for the Henkan widget record */
typedef struct {
    /* resources */
    Pixel	foreground;
    Pixel	background;
    XFontStruct	*font;
    XFontStruct *kanji_font;
    XFontStruct *kana_font;
    Boolean	resize;

    /* private state */
    GC		normal_GC;
    GC          kanji_normal_GC;
    GC          kana_normal_GC;
    GC          invert_GC;
    GC          kanji_invert_GC;
    GC          kana_invert_GC;
    int		max_ascent, max_descent;
    int		char_width;
    int		char_space;
    XKanjiStatus kanji_status;
} HenkanPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _HenkanRec {
    CorePart	core;
    SimplePart	simple;
    HenkanPart	henkan;
} HenkanRec;

#endif /* _XnwHenkanP_h */
