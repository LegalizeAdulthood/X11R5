/*
 * CpickP.h - Color picker widget for X11 Toolkit
 * 
 * Author:	Mike Yang (mikey@sgi.com)
 *		Silicon Graphics, Inc.
 * Date:	Mon Aug 9 1988, fix for R3 Wed Mar 8 1989
 *
 * Copyright (c) 1988, 1991 Mike Yang
 *
 */

#ifndef _CpickP_h
#define _CpickP_h

#include <X11/Xlib.h>
#include "Cpick.h"
#include <X11/CompositeP.h>

#define R 0
#define G 1
#define B 2
#define H 3
#define S 4
#define V 5
#define C 6
#define M 7
#define Y 8
#define NUM Y+1

#define NARROW 0
#define WIDE 1
#define RANGE 2

#define MAXNAME 80

typedef struct {
  XtCallbackList selectProc;
  XtCallbackList okProc;
  XtCallbackList changeProc;
  XtCallbackList restoreProc;
  XColor *allocated;
  String selectlabel, cancellabel, restorelabel, oklabel;
  Dimension nearpixels;
  Boolean usecolors;
  Colormap cmap;
  Widget slides[NUM], labels[NUM], tlevel, toplabel,
         box, hex, nlevel, select0, cancel, restore,
         ok, palette, match, mlabel;
  char mnames[MAXPIXELS][MAXNAME];
  int values[NUM], inc, mdist[MAXPIXELS];
  Boolean keep, matched;
  int wide;
  XColor oldvalue, nearcells[MAXPIXELS];
} CpickPart;

typedef struct _CpickRec {
  CorePart core;
  CompositePart composite;
  CpickPart cpick;
} CpickRec;

typedef struct _CpickClass {
  int make_compiler_happy;
} CpickClassPart;

typedef struct _CpickClassRec {
  CoreClassPart core_class;
  CompositeClassPart composite_class;
  CpickClassPart cpick_class;
} CpickClassRec;

#endif /* _CpickP_h */
