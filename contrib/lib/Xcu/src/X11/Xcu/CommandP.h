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
 * CommandP.h - Private definitions for XcuCommand widget
 */

#ifndef _Xcu_CommandP_h
#define _Xcu_CommandP_h

#include <X11/Xcu/Command.h>
#include <X11/Xcu/LabelP.h>

/***********************************************************************
 *
 * XcuCommand Widget Private Data
 *
 ***********************************************************************/

/************************************
 *
 *  Class structure
 *
 ***********************************/


   /* New fields for the XcuCommand widget class record */
typedef struct _XcuCommandClass 
  {
    int makes_compiler_happy;  /* not used */
  } XcuCommandClassPart;

   /* Full class record declaration */
typedef struct _XcuCommandClassRec {
    CoreClassPart	core_class;
    XcuSimpleClassPart	simple_class;
    XcuLabelClassPart	label_class;
    XcuCommandClassPart    command_class;
} XcuCommandClassRec;

extern XcuCommandClassRec xcuCommandClassRec;

/***************************************
 *
 *  Instance (widget) structure 
 *
 **************************************/

    /* New fields for the XcuCommand widget record */
typedef struct {
    /* resources */
    Dimension   highlight_thickness;
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
} XcuCommandPart;


/*    XtEventsPtr eventTable;*/


   /* Full widget declaration */
typedef struct _XcuCommandRec {
    CorePart         core;
    XcuSimplePart     simple;
    XcuLabelPart	     label;
    XcuCommandPart    command;
} XcuCommandRec;

#endif /* _Xcu_CommandP_h */
