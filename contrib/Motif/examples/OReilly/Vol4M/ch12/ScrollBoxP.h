/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */

/* 
 * scrollBoxP.h - Private definitions for scrollBox widget
 * 
 */

#ifndef _XORAscrollBoxP_h
#define _XORAscrollBoxP_h

/***********************************************************************
 *
 * scrollBox Widget Private Data
 *
 ***********************************************************************/

#include "ScrollBox.h"

#include <X11/CompositeP.h>

/* New fields for the scrollBox widget class record */
typedef struct _ScrollBoxClass {
	int empty;
} ScrollBoxClassPart;

/* Full class record declaration */
typedef struct _ScrollBoxClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    ScrollBoxClassPart	scrollBox_class;
} ScrollBoxClassRec;

extern ScrollBoxClassRec scrollBoxClassRec;

/* New fields for the scrollBox widget record */
typedef struct {
    Dimension   h_space, v_space;
    Dimension	preferred_width, preferred_height;
    Dimension	last_query_width, last_query_height;
    XtGeometryMask last_query_mode;
} ScrollBoxPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _ScrollBoxRec {
    CorePart	    core;
    CompositePart   composite;
    ScrollBoxPart   scrollBox;
} ScrollBoxRec;

#endif /* _XORAscrollBoxP_h */
