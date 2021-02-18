/*
 * Bargraph.h - Bargraph widget for X11 Toolkit
 * 
 * Author:	Mike Yang (mikey@sgi.com)
 *		Silicon Graphics, Inc.
 * Date:	Mon Aug 9 1988, fix for R3 Wed Mar 8 1989
 *
 * Copyright (c) 1988, 1991 Mike Yang
 *
 * A slight modification of code taken from the X11R2 Scrollbar widget
 *
 */

#ifndef _Bargraph_h
#define _Bargraph_h

#include <X11/Xmu/Converters.h>
#include <X11/Xfuncproto.h>

#define XtNfillColor            "fillcolor"
#define XtNfillPixmap           "fillpixmap"

#define XtCShown		"Shown"
#define XtCTop			"Top"

/* should this be here, or in Atoms.h equivalent? */
#define XtCFillColor            "FillColor"
#define XtCFillPixmap           "FillPixmap"

typedef struct _BargraphRec	  *BargraphWidget;
typedef struct _BargraphClassRec *BargraphWidgetClass;

extern WidgetClass bargraphWidgetClass;

_XFUNCPROTOBEGIN

extern void XtBargraphSetValue(
#if NeedFunctionPrototypes
  Widget /* bargraph */,
#if NeedWidePrototypes
  /* float */ double /* top */
#else
  float /* top */
#endif
#endif
);

_XFUNCPROTOEND

#endif /* _Bargraph_h */
