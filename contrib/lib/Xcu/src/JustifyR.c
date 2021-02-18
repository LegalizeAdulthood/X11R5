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

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xcu/JustifyR.h>

/**
 *** Resource Converter Name
 **/

static void CvtStringToJustify() ;	/* Resource Converter */

/**
 *** Resource Converter Quarks
 **/

static XrmQuark	XrmQEleft, XrmQEcenter, XrmQEright ;	/* Justification */

/**
 *** Resource Converter AddConverter Routine
 **/

void
XcuAddStringToJustifyConverter ()
{
static Boolean first_time = True ;
if (first_time)
    {
    XrmQEleft   = XrmStringToQuark("left") ;
    XrmQEcenter = XrmStringToQuark("center") ;
    XrmQEright  = XrmStringToQuark("right") ;
    XtAddConverter( XtRString, XtRJustify, CvtStringToJustify, NULL, 0 ) ;
    first_time = False ;
    }
return ;
}

/**
 *** Resource Converter Routine
 **/

#include <ctype.h>

/*
 * Convert String To Justify
 */

static void CvtStringToJustify(args, num_args, fromVal, toVal)
    XrmValuePtr *args ;		/* unused */
    Cardinal	*num_args ;	/* unused */
    XrmValuePtr fromVal ;
    XrmValuePtr toVal ;
{
static XcuJustify	e ; /* must be static! */
XrmQuark	q ;
char	*s = (char *) fromVal->addr ;
XrmQuark XcuSimplifyArg () ;

if (s == NULL) return ;

q = XcuSimplifyArg (s, "xcu") ;

toVal->size = sizeof(XcuJustify) ;
toVal->addr = (XPointer) &e ;

if (q == XrmQEleft)   { e = XcuJUSTIFY_LEFT;   return; }
if (q == XrmQEcenter) { e = XcuJUSTIFY_CENTER; return; }
if (q == XrmQEright)  { e = XcuJUSTIFY_RIGHT;  return; }

toVal->size = 0 ;
toVal->addr = NULL ;
return ;
}
