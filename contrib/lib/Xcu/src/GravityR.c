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
#include <X11/Xcu/GravityR.h>

/**
 *** Resource Converter Name
 **/

static void CvtStringToGravity() ;	/* Resource Converter */

/**
 *** Resource Converter Quarks
 **/

static XrmQuark	XrmQEn, XrmQEne, XrmQEe, XrmQEse,	/* Gravity */
	        XrmQEs, XrmQEsw, XrmQEw, XrmQEnw, XrmQEc ;

/**
 *** Resource Converter AddConverter Routine
 **/

void
XcuAddStringToGravityConverter ()
{
static Boolean first_time = True ;
if (first_time)
    {
    XrmQEn   = XrmStringToQuark("n") ;
    XrmQEe   = XrmStringToQuark("e") ;
    XrmQEs   = XrmStringToQuark("s") ;
    XrmQEw   = XrmStringToQuark("w") ;
    XrmQEne   = XrmStringToQuark("ne") ;
    XrmQEnw   = XrmStringToQuark("nw") ;
    XrmQEse   = XrmStringToQuark("se") ;
    XrmQEsw   = XrmStringToQuark("sw") ;
    XrmQEc   = XrmStringToQuark("c") ;
    XtAddConverter( XtRString, XtRGravity, CvtStringToGravity, NULL, 0 ) ;
    first_time = False ;
    }
return ;
}

/**
 *** Resource Converter
 **/

#include <ctype.h>

/*
 * Convert String To Gravity
 */

static void
CvtStringToGravity(args, num_args, fromVal, toVal)
    XrmValuePtr *args ;		/* unused */
    Cardinal	*num_args ;	/* unused */
    XrmValuePtr fromVal ;
    XrmValuePtr toVal ;
{
static XcuGravity	e ; /* must be static! */
XrmQuark	q ;
char	*s = (char *) fromVal->addr ;
XrmQuark XcuSimplifyArg () ;

if (s == NULL) return ;

q = XcuSimplifyArg (s, "xcu") ;

toVal->size = sizeof(XcuGravity) ;
toVal->addr = (XPointer) &e ;

if (q == XrmQEn)	{ e = XcuNORTH_GRAVITY;   return; }
if (q == XrmQEe)	{ e = XcuEAST_GRAVITY;   return; }
if (q == XrmQEs)	{ e = XcuSOUTH_GRAVITY;   return; }
if (q == XrmQEw)	{ e = XcuWEST_GRAVITY;   return; }
if (q == XrmQEne)	{ e = XcuNORTH_EAST_GRAVITY;   return; }
if (q == XrmQEnw)	{ e = XcuNORTH_WEST_GRAVITY;   return; }
if (q == XrmQEse)	{ e = XcuSOUTH_EAST_GRAVITY;   return; }
if (q == XrmQEsw)	{ e = XcuSOUTH_WEST_GRAVITY;   return; }
if (q == XrmQEc)	{ e = XcuCENTER_GRAVITY;   return; }

toVal->size = 0 ;
toVal->addr = NULL ;
return ;
}

