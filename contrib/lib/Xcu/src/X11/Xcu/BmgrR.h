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

#ifndef _Xcu_Bmgr_e
#define _Xcu_Bmgr_e

typedef enum
    {
     XcuBMGR_TRANSIENT  
    ,XcuBMGR_SINGLE_TOGGLE
    ,XcuBMGR_DOUBLE_TOGGLE
    ,XcuBMGR_ONE_OF_MANY
    ,XcuBMGR_ANY_OF_MANY
    ,XcuBMGR_HELP
    } XcuBmgrType ;

#define XtNbmgrType	"bmgrType"
#define XtCXcuBmgrType	"XcuBmgrType"
#define XtRXcuBmgrType	"XcuBmgrType"

void XcuAddStringToBmgrTypeConverter () ;

#endif /* _Xcu_Bmgr_e */
