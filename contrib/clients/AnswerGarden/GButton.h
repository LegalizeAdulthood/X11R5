/**********************************************************

  The ANSWER GARDEN PROJECT

  GBUTTON.H      Grapher Button, "Towards A Better Button"

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

  GButton.h

  Mark Ackerman
  MIT/Center for Coordination Science
  MIT/Project Athena

  ackerman@athena.mit.edu
  ackerman@xv.mit.edu
**********************************************************************/

#ifndef _XtGrapherButton_h
#define _XtGrapherButton_h

/***********************************************************************
 *
 * Grapher Button Widget
 *
 *
 ***********************************************************************/

#include <X11/Xaw/Command.h>

#define XtNselectionString "selectionString"
#define XtNeditMode "editMode"

#define XtCEditMode "EditMode"

#define AGGrapherButtonEditNotify 0
#define AGGrapherButtonNodeNotify 1
#define AGGrapherButtonDynamicNotify 2

typedef struct {
  XEvent *event;
  int notify_type;
  String selection_string;
  String *params;
  int num_params;
} AGGrapherButtonCallbackStruct;

 
/* Class record constants */

extern WidgetClass grapherButtonWidgetClass;

typedef struct _GrapherButtonClassRec *GrapherButtonWidgetClass;
typedef struct _GrapherButtonRec      *GrapherButtonWidget;

#endif /* _XtGrapherButton_h */
/* DON'T ADD STUFF AFTER THIS #endif */

