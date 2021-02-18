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

#ifndef _Xcu_SimpleP_h
#define _Xcu_SimpleP_h

#include <X11/Xcu/Simple.h>
#include <X11/CoreP.h>
#ifdef CompositeSimple
#include <X11/CompositeP.h>
#endif

typedef struct {
    void	(*change_sensitive)();
    void	(*draw_facets)();	/* draw the facet parts   */
    void	(*draw_highlight)();	/* display a highlighted border*/
} XcuSimpleClassPart;

/* Full class record declaration */

typedef struct _XcuSimpleClassRec {
    CoreClassPart	core_class;
    XcuSimpleClassPart	simple_class;
} XcuSimpleClassRec;

extern XcuSimpleClassRec xcuSimpleClassRec;

typedef struct {
    /* resources */
    Cursor	cursor;
    Pixmap	insensitive_border;
    XtCallbackList firstExposeCallbacks;
    XtCallbackList resizeCallbacks;

    Pixel	dim_facet_color;        /* color of the dim facets	*/
    Pixel	bright_facet_color;     /* color of the bright facets	*/
    Pixel       highlight_color;        /* for subclass convenience	*/
    Pixel       unhighlight_color;      /* for subclass convenience	*/
    Pixel	cursor_foreground;	/*				*/
    Pixel	cursor_background;	/*				*/
    Pixel	background_pixel;	/*				*/
    Dimension   facet_width;		/* facet width			*/
    Dimension   highlight_width;	/* highlight width		*/
    Dimension   save_border_width;	/* saved core.border_width	*/
    Boolean     shadow;			/* shadow instead of facet	*/
    Boolean     first_expose;		/* True until first exposure	*/
    XcuSunType	sun_direction;		/* Location of shadow causing sun */

    /* private state */
    XColor	cursor_fg;
    XColor	cursor_bg;
    GC          dim_facet_GC;
    GC		bright_facet_GC;
    GC		highlight_GC;
    GC		unhighlight_GC;
    GC		background_GC;
    GC		bkg_GC;
    Boolean     highlighted;
    Boolean     highlight_change;
    Boolean     exposing;
    Dimension	total_facet_width ;
    Dimension	left_facet_width ;
    Dimension	right_facet_width ;
    Dimension	top_facet_width ;
    Dimension	bot_facet_width ;
} XcuSimplePart;

typedef struct _XcuSimpleRec {
    CorePart	core;
    XcuSimplePart	simple;
} XcuSimpleRec;

#define XtInheritChangeSensitive ((XtProc) _XtInherit)
#define XtInheritDrawFacets     ((XtProc) _XtInherit)
#define XtInheritDrawHighlight ((XtProc) _XtInherit)

#ifdef CompositeSimple

typedef struct _XcuSimpleCompClassRec {
    CoreClassPart	core_class ;
    CompositeClassPart	composite_class ;
    XcuSimpleClassPart	simple_class ;
} XcuSimpleCompClassRec;

extern XcuSimpleCompClassRec xcuSimpleCompClassRec;

typedef struct _XcuSimpleCompRec {
    CorePart		core ;
    CompositePart	composite ;
    XcuSimplePart	simple ;
} XcuSimpleCompRec;

typedef struct _XcuSimpleCompConstClassRec {
    CoreClassPart	core_class ;
    CompositeClassPart	composite_class ;
    ConstraintClassPart	constraint_class ;
    XcuSimpleClassPart	simple_class ;
} XcuSimpleCompConstClassRec;

extern XcuSimpleCompConstClassRec xcuSimpleCompConstClassRec;

typedef struct _XcuSimpleCompConstRec {
    CorePart		core ;
    CompositePart	composite ;
    ConstraintPart	constraint ;
    XcuSimplePart	simple ;
} XcuSimpleCompConstRec;
#endif /* CompositeSimple */
#endif /*  _Xcu_SimpleP_h */
