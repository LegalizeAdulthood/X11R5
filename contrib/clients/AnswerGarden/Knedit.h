
/* Knedit.h */
/***********************************************************

Copyright 1990 by the Massachusetts Institute of Technology,
Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of MIT not be used in
advertising or publicity pertaining to distribution of the software
without specific, written prior permission.

******************************************************************/

#ifndef _Knedit_h
#define _Knedit_h

/***********************************************************************
 *
 * Knedit Widget (subclass of CompositeClass)
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

/* Class record constants */

#define XtNlineHeight "lineHeight"
#define XtNfilename "filename"
#define XtNleftMargin "leftMargin"
#define XtNrightMargin "rightMargin"
#define XtNtopMargin "topMargin"
#define XtNboldFont "boldFont"
#define XtNitalicFont "italicFont"
#define XtNinternalButtonCallback "internalButtonCallback"
#define XtNeditCallback "editCallback"
#define XtNtotalHeight "totalHeight"
#define XtNforceSize "forceSize"
#define XtNeditMode "editMode"
#define XtNnodeName "nodeName"
#define XtNseparatorHeight "separatorHeight"
#define XtNdynamicCallback "dynamicCallback"
#define XtNfuncObj "funcObj"

#define XtCLineHeight "LineHeight"
#define XtCFilename "Filename"
#define XtCForceSize "ForceSize"
#define XtCEditMode "EditMode"
#define XtCNodeName "NodeName"
#define XtCWidget "Widget"

typedef struct _AGKneditCallbackStruct {
  AGGrapherButtonCallbackStruct *grapherButton_callback_struct;
  char *node_name;
}  AGKneditCallbackStruct;


extern WidgetClass kneditWidgetClass;

typedef struct _KneditClassRec *KneditWidgetClass;
typedef struct _KneditRec      *KneditWidget;

#endif /* _Knedit_h */

