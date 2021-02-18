/*
 * BargraphP.h - Bargraph widget for X11 Toolkit
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

#ifndef _BargraphP_h
#define _BargraphP_h

#include "Bargraph.h"
#include <X11/Xaw/SimpleP.h>

typedef struct {
  Pixel	  foreground;	/* thumb foreground color */
  XtOrientation orientation;	/* horizontal or vertical */
  Pixmap	  thumb;	/* thumb color */
  float	  top;		/* What percent is above the win's top */
  float	  shown;	/* What percent is shown in the win */
  Dimension		  length;	/* either height or width */
  Dimension		  thickness;	/* either width or height */
  GC		  gc;		/* a (shared) gc */
  Cardinal		  topLoc;	/* Pixel that corresponds to top */
  Cardinal		  shownLength;	/* Num pixels corresponding to shown */
  Pixel fill_color;
  Pixmap fill_pixmap;
} BargraphPart;

typedef struct _BargraphRec {
  CorePart core;
  SimplePart simple;
  BargraphPart bargraph;
} BargraphRec;

typedef struct _BargraphClass {
  int make_compiler_happy;
} BargraphClassPart;

typedef struct _BargraphClassRec {
  CoreClassPart core_class;
  SimpleClassPart simple_class;
  BargraphClassPart bargraph_class;
} BargraphClassRec;

extern BargraphClassRec bargraphClassRec;

#endif /* _Bargraphp_h */
