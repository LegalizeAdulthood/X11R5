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

/* XcuDeck widget private definitions */

#ifndef _Xcu_DeckP_h
#define _Xcu_DeckP_h

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#ifdef CompositeSimple
#include <X11/Xcu/SimpleP.h>	/* Superclass Private Include File	*/
#else
#include <X11/CompositeP.h>	/* Superclass Private Include File	*/
#endif
#include <X11/Xcu/ProcedureR.h>
#include <X11/Xcu/Deck.h>		/* Class Public Include File		*/

/*
 * The new fields added to superclass widget structure
 *
 * "Int" is often used instead of "Dimension" and "Cardinal" because there is
 * frequent use of these variables in expressions involving signed arithmentic
 */

typedef struct _XcuDeckPart
    {
    /* resources */
    int	internal_width ;    /* horizontal space surrounding children*/
    int	internal_height ;   /* vertical space surrounding children  */
    XcuResizeParticipants resize_participants ; /* what expands on resizes   */
    XtCallbackList callbacks;
    XtProcedureList *procedure_list ;

    /* private state */
    Widget	*order ;	/* keeps track of stacking order */
    int		standalone_width ;  /* minimum width of deck		    */
    int		standalone_height ; /* minimum height of deck		    */
    Position	*x ;
    Position	*y ;
    Dimension	*width ;
    Dimension	*height ;
    Dimension	*border_width ;
    Dimension	new_width ;	    /* set in ETS, xfer'd to core.width in  */
    Dimension	new_height ;        /* DoLayout                             */
    Dimension	almost_width ;  /* Values returned with an Almost reply */
    Dimension	almost_height ;
    Position	almost_x ;
    Position	almost_y ;
    Widget	almost_widget ;
    Boolean	almost_status ;
    XtGeometryMask	almost_mode ;
    int		c_slack ;
    int		r_slack ;
#ifdef CompositeSimple
    GC		background_GC ;
#endif
    } XcuDeckPart ;

/*
 * The complete widget instance structure for this widget
 */

typedef struct _XcuDeckRec
    {
    CorePart		core ;
    CompositePart	composite ;
#ifdef CompositeSimple
    XcuSimplePart	simple ;
#endif
    XcuDeckPart		deck ;
    } XcuDeckRec ;

typedef struct
    {
    int not_used ;
    } XcuDeckClassPart ;

/*
 * The complete widget class structure for the widget
 */

typedef struct _XcuDeckClassRec
    {
    CoreClassPart	core_class ;
    CompositeClassPart	composite_class ;
#ifdef CompositeSimple
    XcuSimpleClassPart	simple_class ;
#endif
    XcuDeckClassPart	deck_class ;
    } XcuDeckClassRec ;

/*
 * The name of a "constant" of the generic widget class structure
 */

extern XcuDeckClassRec xcuDeckClassRec ;

#endif /* _Xcu_DeckP_h */
