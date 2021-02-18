/*
 * Copyright 1991 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          580 Theory Center, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@graphics.cornell.edu
 */

#define CompositeSimple
#include <X11/Core.h>
#ifdef CompositeSimple
#include <X11/Composite.h>
#include <X11/Constraint.h>
#endif
#include <X11/Xcu/SunR.h>

#ifndef _Xcu_Simple_h
#define _Xcu_Simple_h

/* XcuSimple */

#define XtNfirstExpose		"firstExpose"
#define XtNcursor		"cursor"
#define XtNinsensitiveBorder	"insensitiveBorder"
#define XtNcursorForeground	"cursorForeground"
#define XtNcursorBackground	"cursorBackground"

#define XtCInsensitive		"Insensitive"
#define XtCCursorForeground	"CursorCoreground"
#define XtCCursorBackground	"CursorCackground"

/* Facet */

#define XtNdimFacetColor	"dimFacetColor"
#define XtNbrightFacetColor	"brightFacetColor"
#define XtNhighlightColor	"highlightColor"
#define XtNhighlightWidth	"highlightWidth"
#define XtNfacetWidth		"facetWidth"
#define XtNshadow		"shadow"

#define XtCDimFacetColor	"DimFacetColor"
#define XtCBrightFacetColor	"BrightFacetColor"
#define XtCFacetWidth		"FacetWidth"
#define XtCHighlightColor	"HighlightColor"
#define XtCHighlightWidth	"HighlightWidth"
#define XtCShadow		"Shadow"

typedef struct _XcuSimpleClassRec *XcuSimpleWidgetClass;
typedef struct _XcuSimpleRec	 *XcuSimpleWidget;
#ifdef CompositeSimple
typedef struct _XcuSimpleCompClassRec *XcuSimpleCompWidgetClass;
typedef struct _XcuSimpleCompRec      *XcuSimpleCompWidget;
typedef struct _XcuSimpleCompConstClassRec *XcuSimpleCompConstWidgetClass;
typedef struct _XcuSimpleCompConstRec      *XcuSimpleCompConstWidget;
#endif

extern WidgetClass xcuSimpleWidgetClass;
#ifdef CompositeSimple
extern WidgetClass xcuSimpleCompWidgetClass ;
extern WidgetClass xcuSimpleCompConstWidgetClass ;
#endif

#endif /*  _Xcu_Simple_h */
