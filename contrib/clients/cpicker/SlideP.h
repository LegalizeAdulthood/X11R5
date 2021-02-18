/*
 * SlideP.h - Slider widget for X11 Toolkit
 * 
 * Author:	Mike Yang (mikey@sgi.com)
 *		Silicon Graphics, Inc.
 * Date:	Mon Aug 9 1988
 *
 * Copyright (c) 1988, 1991 Mike Yang
 *
 * A slight modification of code taken from the X11R2 Scrollbar widget
 *
 */

#ifndef _SlideP_h
#define _SlideP_h

#include "Slide.h"
#include <X11/Xaw/SimpleP.h>

typedef struct {
    XtCallbackList scrollProc;	/* smooth scroll */
    XtCallbackList thumbProc;	/* jump (to position) scroll */
    XtCallbackList jumpProc;	/* same as thumbProc but pass data by ref */
    Cursor	  upCursor;	/* scroll up cursor */
    Cursor	  downCursor;	/* scroll down cursor */
    Cursor	  leftCursor;	/* scroll left cursor */
    Cursor	  rightCursor;	/* scroll right cursor */
    Cursor	  verCursor;	/* scroll vertical cursor */
    Cursor	  horCursor;	/* scroll horizontal cursor */
    Cursor	  inactiveCursor; /* The normal cursor for scrollbar */
    char	  direction;	/* a scroll has started; which direction */
} SlidePart;

typedef struct _SlideRec {
  CorePart core;
  SimplePart simple;
  BargraphPart bargraph;
  SlidePart slide;
} SlideRec;

typedef struct _SlideClass {
  int make_compiler_happy;
} SlideClassPart;

typedef struct _SlideClassRec {
  CoreClassPart core_class;
  SimpleClassPart simple_class;
  BargraphClassPart bargraph_class;
  SlideClassPart slide_class;
} SlideClassRec;

extern SlideClassRec slideClassRec;

#endif /* _SlideP_h */
