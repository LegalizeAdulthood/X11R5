
/* Grapher.h */

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

#ifndef _Grapher_h
#define _Grapher_h

/***********************************************************************
 *
 * Grapher Widget (subclass of CompositeClass)
 *
 ***********************************************************************/

/* Parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

*/

#include "GButton.h"

#define XtNxIncrement "xIncrement"
#define XtNyIncrement "yIncrement"
#define XtNnextY "nextY"
#define XtNnodeCallback "nodeCallback"
#ifdef GRAPHERv1
#define XtNfilename "filename"
#else
#define XtNtext "text"
#endif
#define XtNrootNode "rootNode"
#ifndef _XtShell_h
#define XtNminHeight "minHeight"
#define XtNmaxHeight "maxHeight"
#define XtNminWidth "minWidth"
#define XtNmaxWidth "maxWidth"
#endif
#define XtNeditMode "editMode"
#define XtNeditCallback "editCallback"
#define XtNdynamicCallback "dynamicCallback"

#define XtCIncrement "Increment"
#define XtCNextY "NextY"
#ifdef GRAPHERv1
#define XtCFilename "Filename"
#else
#define XtCText "Text"
#endif
#define XtCRootNode "RootNode"
#define XtCEditMode "EditMode"
#define XtCEditCallback "EditCallback"
#define XtCDynamicCallback "DynamicCallback"

typedef struct {
  AGGrapherButtonCallbackStruct *grapherButton_callback_struct;
  char *node_name;
}  AGGrapherCallbackStruct;



extern WidgetClass grapherWidgetClass;

typedef struct _GrapherClassRec *GrapherWidgetClass;
typedef struct _GrapherRec      *GrapherWidget;

#endif /* _Grapher_h */

