/**********************************************************

  The ANSWER GARDEN PROJECT


 * GrapherButtonP.h - Private definitions for GrapherButton widget

Copyright 1989, 1990, 1991 by the Massachusetts Institute of Technology,
Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of MIT not be used in
advertising or publicity pertaining to distribution of the software
without specific, written prior permission.

Answer Garden is a trademark of the Massachusetts Institute of
Technology.  All rights reserved.

  Mark Ackerman
  MIT/Center for Coordination Science
  MIT/Project Athena

  ackerman@athena.mit.edu
  ackerman@xv.mit.edu
**********************************************************************/

#ifndef _XtGrapherButtonP_h
#define _XtGrapherButtonP_h

/***********************************************************************
 *
 * Grapher Button Widget Private Data
 *
 ***********************************************************************/

#include "GButton.h"
#include <X11/CoreP.h>
#include <X11/Xaw/CommandP.h>



/* New fields for the GrapherButton widget class record */

typedef struct {int makes_compiler_happy;} GrapherButtonClassPart;

/* Full class record declaration */
typedef struct _GrapherButtonClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
    LabelClassPart      label_class;
    CommandClassPart    command_class;
    GrapherButtonClassPart	grapherButton_class;
} GrapherButtonClassRec;

extern GrapherButtonClassRec grapherButtonClassRec;

/* New fields for the GrapherButton widget record */
typedef struct {
    Boolean edit_mode;
    String  selection_string;

    /* private state */
    Boolean owns_selection;
} GrapherButtonPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _GrapherButtonRec {
    CorePart	core;
    SimplePart	simple;
    LabelPart   label;
    CommandPart  command;
    GrapherButtonPart	grapherButton;
} GrapherButtonRec;



#endif /* _XtGrapherButtonP_h */
