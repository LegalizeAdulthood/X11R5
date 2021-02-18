/*
* $XConsortium: CommandLcP.h,v 1.30 90/12/01 13:00:10 rws Exp $
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
 * CommandLcP.h - Private definitions for CommandLocale widget
 * 
 */

#ifndef _XawCommandLocaleP_h
#define _XawCommandLocaleP_h

#include <X11/Xaw/CommandLc.h>
#include <X11/Xaw/LabelLcP.h>

/***********************************************************************
 *
 * CommandLocale Widget Private Data
 *
 ***********************************************************************/

#include <X11/Xaw/CommandP.h> /* For XtCommandHighlight */

/************************************
 *
 *  Class structure
 *
 ***********************************/


   /* New fields for the CommandLocale widget class record */
typedef struct _CommandLocaleClass 
  {
    int makes_compiler_happy;  /* not used */
  } CommandLocaleClassPart;

   /* Full class record declaration */
typedef struct _CommandLocaleClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
    LabelLocaleClassPart	label_class;
    CommandLocaleClassPart    command_class;
} CommandLocaleClassRec;

extern CommandLocaleClassRec commandLocaleClassRec;

/***************************************
 *
 *  Instance (widget) structure 
 *
 **************************************/

    /* New fields for the CommandLocale widget record */
typedef struct {
    /* resources */
    Dimension   highlight_thickness;
    XtCallbackList callbacks;

    /* private state */
    Pixmap      	gray_pixmap;
    GC          	normal_GC;
    GC          	inverse_GC;
    Boolean     	set;
    XtCommandHighlight	highlighted;
    /* more resources */
    int			shape_style;    
    Dimension		corner_round;
} CommandLocalePart;


/*    XtEventsPtr eventTable;*/


   /* Full widget declaration */
typedef struct _CommandLocaleRec {
    CorePart         core;
    SimplePart	     simple;
    LabelLocalePart	     label;
    CommandLocalePart      command;
} CommandLocaleRec;

#endif /* _XawCommandLocaleP_h */


