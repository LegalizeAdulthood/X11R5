
/* KneditP.h */

/***********************************************************
Copyright 1989 by the Massachusetts Institute of Technology, 
Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  


******************************************************************/

/* 
 * KneditP.h - Private definitions for Knedit widget
 * 
 */

#ifndef _KneditP_h
#define _KneditP_h

/***********************************************************************
 *
 * Knedit Widget Private Data
 *
 ***********************************************************************/

#include "Knedit.h"
#include "LayoutP.h"

/*** These should move somewhere else so they don't
     bother a subclasser. ***/
#define MAXBUF 256
#define MAXCHAR   2048  /* make so it's alloc'ed or a resource */

/* New fields for the Knedit widget class record */
typedef struct {int makes_compiler_happy;} KneditClassPart;

/* Full class record declaration */
typedef struct _KneditClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    LayoutClassPart	layout_class;
    KneditClassPart     knedit_class;
} KneditClassRec;

extern KneditClassRec kneditClassRec;

/* New fields for the Knedit widget record */


typedef struct {
  /* resources */
    Position left_margin;
    Position right_margin;
    Position top_margin;
    Dimension line_height; /* height of each line including leading */
    Dimension separator_height; /* distance between paragraphs */
    Dimension v_space; /* extra spacing for buttons */
    XFontStruct *font;
    XFontStruct *bold_font;
    XFontStruct *italic_font;
    String filename;
    Pixel foreground;
    XtCallbackList internal_button_callback;
    XtCallbackList edit_callback;
    Cursor cursor;
    Dimension total_height;
    Boolean force_size;
    Boolean edit_mode;    
    String knedit_node_name;
    XtCallbackList dynamic_callback;
    Widget func_obj;

  /* private */
    GC normal_gc;
    GC italic_gc;
    GC bold_gc;
    int when_flag;
    Boolean error_flag;
    String textbuffer;
    XtPointer first_passback;

} KneditPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _KneditRec {
    CorePart	    core;
    CompositePart   composite;
    LayoutPart 	    layout;
    KneditPart      knedit;
} KneditRec;

#endif /* _KneditP_h */

