/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */

#ifndef _XORAscrollBox_h
#define _XORAscrollBox_h

/***********************************************************************
 *
 * scrollBox Widget (subclass of CompositeClass)
 *
 ***********************************************************************/

/* Parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 hSpace 	     HSpace		Dimension	4
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 vSpace 	     VSpace		Dimension	4
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

*/


/* New fields */
#define XtNhSpace "hSpace"
#define XtNvSpace "vSpace"

/* Class record constants */

extern WidgetClass scrollBoxWidgetClass;

typedef struct _ScrollBoxClassRec *ScrollBoxWidgetClass;
typedef struct _ScrollBoxRec      *ScrollBoxWidget;

#endif /* _XORAscrollBox_h */
