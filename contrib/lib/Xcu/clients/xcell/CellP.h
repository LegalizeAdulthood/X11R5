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

/* 
 * CellP.h - Private definitions for XcuCell widget
 */

#ifndef _Xcu_CellP_h
#define _Xcu_CellP_h

#include <stdio.h>
#include "Cell.h"
#include <X11/Xcu/SimpleP.h>
#include <X11/Xcu/ProcedureR.h>

/* New fields for the XcuCell widget class record */

typedef struct
    {
    XtProc	redisplay_text ;
    } XcuCellClassPart;


/* Full class record declaration */
typedef struct _XcuCellClassRec
    {
    CoreClassPart	core_class ;
    XcuSimpleClassPart	simple_class ;
    XcuCellClassPart	cell_class ;
    } XcuCellClassRec ;

extern XcuCellClassRec xcuCellClassRec ;

/* New fields for the XcuCell widget record */

typedef enum {
	     KLCRT3, KLCRT4, KLCRT5, LCCRRR4, LCCR4, LCR3, LCR4, LCR5, LCR6
	     } XcuCellType ;

typedef struct
    {
    /* resources */
    XColor	*colors ;
    String	color0 ;
    String	color1 ;
    String	color2 ;
    String	color3 ;
    String	color4 ;
    String	color5 ;
    String	color6 ;
    String	color7 ;
    String	color_string ;
    String	rules_file ;
    String	pick_file ;
    String	type_ ;
    String	ignore_colors ;
    String	directory ;
    int		seed_ ;
    int		per_row_ ;
    int		per_col_ ;
    int		rule_index_ ;
    int		n_colors ;
    Boolean	vertical_merge_ ;
    Boolean	horizontal_merge_ ;
    Boolean	order_ ;
    Boolean	masks_active ;
    Boolean	alternating ;
    Boolean	mutate ;
    Boolean	beep ;
    Dimension	internal_width ;
    Dimension	internal_height ;
    XcuCellSequenceType sequence ;
    XcuCellFrequencyType frequency ;
    XtProcedureList *procedure_list ;

    /* private state */
    XcuCellType	mix_ ;
    Pixel	*cells ;
    Pixel	**rcells ;
    int		max_colors ;
    int		current_h ;
    int		n_permutations ;
    int		permutation_index ;
    int		**permute_array ;
    int		max_rw_cells ;
    int		spread_ ;
    int		reduct_ ;
    int		window_ ;
    int		order_number_ ;
    GC		background_GC ;
    GC		cell_GC ;
    int		height ;
    int		width ;
    XImage	*image ;
    unsigned char *row0_, *row1_ ;
    int		***save_rule_ ;
    int		***asave_rule_ ;
    int		*irule_ ;
    int		*arule_ ;
    int		*mutation ;
    int		mod_many_per_page ;
    int		mod_hmany_per_page ;
    int		old_many_per_page ;
    int		old_hmany_per_page ;
    int		old_rcell_hmany ;
    FILE	*fpick_ ;
    int		**mask_ ;
    int		**rule_ ;
    int		nrule_ ;
    int		masks_ ;
    int		row_ ;
    int		row_count_ ;
    int		rule_count ;
    int		many_count_ ;
    int		hmany_count_ ;
    int		mask_iter_ ;
    int		*ignore_array ;
    int		ignore_count ;
    int		rule_number_ ;
    int		old_n_colors ;
    Boolean	is_exposed ;
    Boolean	all_random_ ;
    Boolean	first_initial_row ;
    unsigned
    char	**wrap_left_, **wrap_right_ ;
    Boolean  height_changed ;
    Boolean  hmerge_changed ;
    Boolean  hmany_changed ;
    Boolean  ignore_colors_changed ;
    Boolean  many_changed ;
    Boolean  mod_changed ;
    Boolean  pick_changed ;
    Boolean  rrule_changed ;
    Boolean  rules_changed ;
    Boolean  seed_changed ;
    Boolean  spread_changed ;
    Boolean  type_changed ;
    Boolean  width_changed ;
    Boolean  windo_changed ;
    Boolean  colors_changed ;
    Boolean  reduct_changed ;
    Boolean  n_colors_changed ;
    Boolean  alt_changed ;
    } XcuCellPart ;


/*
 * Full instance record declaration
 */

typedef struct _XcuCellRec
    {
    CorePart	 core ;
    XcuSimplePart simple ;
    XcuCellPart	 cell ;
    } XcuCellRec ;

/*
 * Inheritance definitions
 */

/**
#define XtInheritRedisplayText ((XtProc) _XtInherit)
**/

#endif _Xcu_CellP_h
