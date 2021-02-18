/*
 * Cpick.h - Color picker widget for X11 Toolkit
 * 
 * Author:	Mike Yang (mikey@sgi.com)
 *		Silicon Graphics, Inc.
 * Date:	Mon Aug 9 1988
 *
 * Copyright (c) 1988, 1991 Mike Yang
 *
 */

#ifndef _Cpick_h
#define _Cpick_h

#include <X11/Composite.h>

/* yuck... hardware dependency */
#define MAXPIXELS 256

/* #define COLORLESSPIXELS 3 */
#define COLORLESSPIXELS 5
#define NECESSARYPIXELS (COLORLESSPIXELS+6)
#define HNEARRATIO 1
#define VNEARRATIO 1

typedef struct _CpickRec *CpickWidget;
typedef struct _CpickClassRec *CpickWidgetClass;

#define XtNselectProc	"selectproc"
#define XtNokProc	"okproc"
#define XtNchangeProc	"changeproc"
#define XtNrestoreProc	"restoreproc"
#define XtNallocated	"allocated"
#define XtNcmap		"cmap"
#define XtNselectLabel	"selectlabel"
#define XtNcancelLabel	"cancellabel"
#define XtNrestoreLabel	"restorelabel"
#define XtNokLabel	"oklabel"
#define XtNnearPixels	"nearpixels"
#define XtNuseColors	"usecolors"

#define XtCAllocated	"Allocated"
#define XtCCmap		"Cmap"
#define XtCDimension	"Dimension"
#define XtCUsecolors	"Usecolors"

#define XtRXColor	"xcolor"

extern WidgetClass cpickWidgetClass;

#endif /* _Cpick_h */
