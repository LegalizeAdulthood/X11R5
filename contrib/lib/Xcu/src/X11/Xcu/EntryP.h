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
 * EntryP.h - Private definitions for XcuEntry widget
 */

#ifndef _Xcu_EntryP_h
#define _Xcu_EntryP_h

#include <X11/Xcu/Entry.h>
#include <X11/Xcu/CommandP.h>
#include <X11/Xcu/ProcedureR.h>

/***********************************************************************
 *
 * XcuEntry Widget Private Data
 *
 ***********************************************************************/

/************************************
 *
 *  Class structure
 *
 ***********************************/


   /* New fields for the XcuEntry widget class record */
typedef struct _XcuEntryClass 
  {
    int makes_compiler_happy;  /* not used */
  } XcuEntryClassPart;

   /* Full class record declaration */
typedef struct _XcuEntryClassRec {
    CoreClassPart	core_class;
    XcuSimpleClassPart	simple_class;
    XcuLabelClassPart	label_class;
    XcuCommandClassPart	command_class;
    XcuEntryClassPart    entry_class;
} XcuEntryClassRec;

extern XcuEntryClassRec xcuEntryClassRec;

/***************************************
 *
 *  Instance (widget) structure 
 *
 **************************************/

    /* New fields for the XcuEntry widget record */
typedef struct {
    /* resources */
    XtCallbackList callbacks;
    Boolean	   color_entry ;
    String	   signal_char ;
    String	   kill_char ;
    String	   erase_char ;
    String	   prefix ;
    XtProcedureList *procedure_list ;

    /* private state */
    String entered_text ;
    String total_text ;
    int n_entered ;
    int buffer_length ;
    int prefix_length ;
} XcuEntryPart;

#define XcuEntryBufferBlock 80

/*    XtEventsPtr eventTable;*/


   /* Full widget declaration */
typedef struct _XcuEntryRec {
    CorePart		core;
    XcuSimplePart	simple;
    XcuLabelPart	label;
    XcuCommandPart	command;
    XcuEntryPart	entry;
} XcuEntryRec;

#endif /* _Xcu_EntryP_h */
