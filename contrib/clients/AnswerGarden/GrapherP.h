
/* GrapherP.h */

/***********************************************************
Copyright 1989,the Massachusetts Institute of Technology, Cambridge, Massachusetts.

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
 * GrapherP.h - Private definitions for Grapher widget
 * 
 */

#ifndef _GrapherP_h
#define _GrapherP_h

/***********************************************************************
 *
 * Grapher Widget Private Data
 *
 ***********************************************************************/

#include "Grapher.h"
#include "LayoutP.h"

typedef struct _Node 
{
    struct _Node *child;
    struct _Node *sibling;
    struct _Node *parent;
    Position x;
    Position y;
    Dimension width;
    Dimension height;
    String label;
    String node_name;
    Widget w;
} Node;

#define NEWLINE '\n'


/* New fields for the Grapher widget class record */
typedef struct {int makes_compiler_happy;} GrapherClassPart;

/* Full class record declaration */
typedef struct _GrapherClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    LayoutClassPart	layout_class;
    GrapherClassPart    grapher_class;
} GrapherClassRec;

extern GrapherClassRec grapherClassRec;

/* New fields for the Grapher widget record */
typedef struct {
    Node *root;
    Position next_y;
    Dimension x_increment;
    Dimension y_increment;
#ifdef GRAPHERv1
    String filename;
#else
    String text;
#endif
    XtCallbackList callback;
    XtCallbackList edit_callback;
    Cursor cursor;
    Dimension max_width;
    Dimension min_width;
    Dimension max_height;
    Dimension min_height;
    Boolean edit_mode;
    Pixel foreground;
    XtCallbackList dynamic_callback;


      /* Private */
    int nNodes;
    GC gc;
    Position max_x;
} GrapherPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _GrapherRec {
    CorePart	    core;
    CompositePart   composite;
    LayoutPart 	    layout;
    GrapherPart     grapher;
} GrapherRec;

#endif /* _GrapherP_h */

















