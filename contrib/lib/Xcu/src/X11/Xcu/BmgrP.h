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
 * BmgrP.h - Private definitions for XcuBmgr widget
 */

#ifndef _Xcu_BmgrP_h
#define _Xcu_BmgrP_h

#include <X11/Xcu/ButtonP.h>
#include <X11/Xcu/ProcedureR.h>
#include <X11/Xcu/BmgrR.h>
#include <X11/Xcu/Bmgr.h>

/* New fields for the XcuBmgr widget class record */

typedef struct {int foo ;} XcuBmgrClassPart ;

/* Full class record declaration */
typedef struct _XcuBmgrClassRec
    {
    CoreClassPart	core_class ;
    XcuBmgrClassPart	bmgr_class ;
    } XcuBmgrClassRec ;

extern XcuBmgrClassRec xcuBmgrClassRec ;

/* New fields for the XcuBmgr widget record */

typedef struct
    {
    Boolean	down_action ;
    XcuBmgrType	bmgr_type ;
    /***
    Boolean	selectable ;
    Cardinal	min_set ;
    Cardinal	max_set ;
    ***/
    XtCallbackList set_callbacks;
    XtCallbackList unset_callbacks;
    XtCallbackList illegal_callbacks;
    XtProcedureList *procedure_list ;

    /* private state */
    Cardinal	 n_buttons ;
    XcuButtonWidget *buttons ;
    caddr_t	 *button_values ;
    } XcuBmgrPart ;

/*
 * Full instance record declaration
 */

typedef struct _XcuBmgrRec
    {
    CorePart	core ;
    XcuBmgrPart	bmgr ;
    } XcuBmgrRec ;

#endif /* _Xcu_BmgrP_h */
