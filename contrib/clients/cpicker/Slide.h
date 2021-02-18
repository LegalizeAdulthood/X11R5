/*
 * Slide.h - Slider widget for X11 Toolkit
 * 
 * Author:	Mike Yang (mikey@sgi.com)
 *		Silicon Graphics, Inc.
 * Date:	Mon Aug 9 1988, fix for R3 Wed Mar 8 1989
 *
 * A slight modification of code taken from the X11R2 Scrollbar widget
 *
 * Copyright (c) 1988, 1991 Mike Yang
 *
 */

#ifndef _Slide_h
#define _Slide_h

#include "Bargraph.h"

typedef struct _SlideRec	  *SlideWidget;
typedef struct _SlideClassRec *SlideWidgetClass;

extern WidgetClass slideWidgetClass;

#endif /* _Slide_h */
