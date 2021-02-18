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

/* XcuTbl widget private definitions */

#ifndef _Xcu_TblP_h
#define _Xcu_TblP_h

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#ifdef CompositeSimple
#include <X11/Xcu/SimpleP.h>	/* Superclass Private Include File	*/
#else
#include <X11/ConstrainP.h>	/* Superclass Private Include File	*/
#endif
#include <X11/Xcu/Tbl.h>		/* Class Public Include File		*/

/*
 * Prepare for declaring new fields added to superclass widget structure
 */

#define MAX_TBL_COLS	100
#define MAX_TBL_ROWS	100

typedef enum { TBL_ITEM, TBL_HSPAN, TBL_VSPAN }
	XtPrimaryType ;

typedef enum
	{
	TBL_FIRST_PASS, TBL_NO_FORMAT, TBL_STRING_FORMAT, TBL_FILE_FORMAT
	} XtFormatMode ;

typedef struct s_item
    {
    XtPrimaryType primary ;/* widget or a span				*/
    XcuJustify justify ;	/* justification of  text			*/
    XcuGravity g ;	/* gravity of child				*/
    Boolean e ;		/* equal columns in this row			*/
    Boolean f ;		/* this column takes up the slack		*/
    Widget pw ;		/* points to widget responsible for this item	*/
    Cardinal pi ;	/* points to upper-left of this spanned item	*/
    Cardinal pj ;	/* points to upper-left of this spanned item	*/
    int	 aligned_width ;
    int	 width ;	/* save until needed by XtConfigureWidget	*/
    int	 height ;	/* save until needed by XtConfigureWidget	*/
    int adj_width ;	/* adjusted for spanning members		*/
    String text ;	/* item text					*/
    } ElementRecord ;

/*
 * The new fields added to superclass widget structure
 *
 * "Int" is often used instead of "Dimension" and "Cardinal" because there is
 * frequent use of these variables in expressions involving signed arithmentic
 */

typedef struct _XcuTblPart
    {
    /* resources */
    Boolean	aligned_columns ;   /* XtNalignedColumns		    */
    Boolean	clip_on_shrink ;    /* XtNclipOnShrink			    */
    Boolean	equal_columns ;	    /* XtNequalColumns			    */
    Boolean	equal_rows ;	    /* XtNequalRows			    */
    String	format_file ;	    /* Tbl description file		    */
    String	format_string ;	    /* Tbl description string		    */
    int	inter_width ;	    /* horizontal distance between children */
    int	inter_height ;	    /* vertical distance between children   */
    int	internal_width ;    /* horizontal space surrounding children*/
    int	internal_height ;   /* vertical space surrounding children  */
    XcuResizeParticipants resize_participants ; /* what expands on resizes   */

    /* private state */
    ElementRecord *item[MAX_TBL_ROWS] ; /* stats for each matrix element    */
    int		*r_item ;	    /* number of "items" in a row	    */
    int		*r_height ;	    /* height of each  row		    */
    int		rows ;		    /* number of rows in matrix		    */
    int		cols[MAX_TBL_ROWS] ;/* number of cols in each row	    */
    int		max_cols ;	    /* need to save for resize calculations */
    Boolean	*equal_cols ;	    /* whether columns in this row are equal*/
    Boolean	*vspan_status ;	    /* whether this row is vert spanned	    */
    Boolean	initting ;	    /* whether InitLayout is active	    */
    Boolean	force_width ;	    /* whether width has been fumped	    */
    int		standalone_width ;  /* minimum width of table		    */
    int		standalone_height ; /* minimum height of table		    */
    int		typical_border ;
    Dimension	pre_almost_width ;
    Dimension	pre_almost_height ;
    Dimension	new_width ;	    /* set in ETS, xfer'd to core.width in  */
    Dimension	new_height ;        /* DoLayout                             */
    Dimension	width0 ;
    Dimension	height0 ;
    XtFormatMode format_mode ;	    /* file or string	    		    */
    /* needed only during parsing */
    int		string_breaker ;
    Cardinal	n_cols ;
    Cardinal	data_row ;
    ElementRecord *temp_elements ;
    ElementRecord *null_elements ;
    /* */
    int		child_border_width ;
    Pixel	child_border_color ;
    Pixel	child_background_color ;
    Pixel	child_foreground_color ;
#ifdef CompositeSimple
    GC background_GC ;
#endif
    } XcuTblPart ;

/*
 * The complete widget instance structure for this widget
 */

typedef struct _XcuTblRec
    {
    CorePart		core ;
    CompositePart	composite ;
    ConstraintPart	constraint ;
#ifdef CompositeSimple
    XcuSimplePart	simple ;
#endif
    XcuTblPart		tbl ;
    } XcuTblRec ;

/*
 * The new fields added to superclass constraint structure
 */

typedef struct _XcuTblConstraintsPart
    {
    Boolean	almost_status ;
    Boolean	differ_geometry ;
    Boolean	fill_column; /* this widget's column will fill on sts	*/
    int		span_width ; /* # of elements spanned			*/
    int		span_height ;
    int		sts_width ;  /* standalone table size			*/
    int		sts_height ;
    Position	ets_x ;      /* enclosed table positions		*/
    Position	ets_y ;
    int		ss_width ;  /* standalone size				*/
    int		ss_height ;
    int		ets_width ;  /* enclosed table size			*/
    int		ets_height ;
    Dimension	almost_width ;  /* Values returned with an Almost reply */
    Dimension	almost_height ;
    Position	almost_x ;
    Position	almost_y ;
    Widget	almost_widget ;
    XtGeometryMask	almost_mode ;
    } XcuTblConstraintsPart ;

/*
 * The complete Constraint structure
 */

typedef struct _XcuTblConstraintsRec
    {
    XcuTblConstraintsPart	tbl ;
    } XcuTblConstraintsRec,
      *XcuTblConstraints ;

/*
 * New fields added to the widget superclass class structure
 */

typedef String (*XtTblStringProc) () ;

typedef struct
    {
    XtTblStringProc interpret_string ;
    } XcuTblClassPart ;

/*
 * The complete widget class structure for the widget
 */

typedef struct _XcuTblClassRec
    {
    CoreClassPart	core_class ;
    CompositeClassPart	composite_class ;
    ConstraintClassPart	constraint_class ;
#ifdef CompositeSimple
    XcuSimpleClassPart  simple_class ;
#endif
    XcuTblClassPart	tbl_class ;
    } XcuTblClassRec ;

/*
 * The name of a "constant" of the generic widget class structure
 */

extern XcuTblClassRec xcuTblClassRec ;

/*
 * For each new procedure in the widget class structure, an inherit operation
 */

#define XtInheritInterpretString ((XtStringProc) _XtInherit)

#endif /* _Xcu_TblP_h */
