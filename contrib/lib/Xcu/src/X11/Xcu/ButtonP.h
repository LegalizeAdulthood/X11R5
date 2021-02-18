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
 * ButtonP.h - Private definitions for XcuButton widget
 */

#ifndef _Xcu_ButtonP_h
#define _Xcu_ButtonP_h

#include <X11/Xcu/Button.h>
#include <X11/Xcu/LabelP.h>
#include <X11/Xcu/SimpleP.h>

/***********************************************************************
 *
 * XcuButton Widget Private Data
 *
 ***********************************************************************/

/************************************
 *
 *  Class structure
 *
 ***********************************/


   /* New fields for the XcuButton widget class record */
typedef struct _XcuButtonClass 
  {
    int makes_compiler_happy;  /* not used */
  } XcuButtonClassPart;

   /* Full class record declaration */
typedef struct _XcuButtonClassRec {
    CoreClassPart	core_class;
    XcuSimpleClassPart	simple_class;
    XcuLabelClassPart	label_class;
    XcuButtonClassPart     button_class;
} XcuButtonClassRec;

extern XcuButtonClassRec xcuButtonClassRec;

/***************************************
 *
 *  Instance (widget) structure 
 *
 **************************************/

    /* New fields for the XcuButton widget record */
typedef struct {
    /* resources */
    XtCallbackList callbacks;

    /* private state */
    Pixmap      gray_pixmap;
    GC          highlight_GC;
    GC          inverse_GC;
    GC          inverse_text_GC;
    GC          normal_GC;
    GC          gray_GC;
    Boolean     set;
    Boolean     display_set;
    Boolean     display_highlighted;
    Boolean     not_expose;
} XcuButtonPart;


/*    XtEventsPtr eventTable;*/


   /* Full widget declaration */
typedef struct _XcuButtonRec {
    CorePart         core;
    XcuSimplePart     simple;
    XcuLabelPart	     label;
    XcuButtonPart     button;
} XcuButtonRec;

#endif /* _Xcu_ButtonP_h */


