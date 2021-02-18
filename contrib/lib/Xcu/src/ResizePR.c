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

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xcu/ResizePR.h>

/**
 *** Resource Converter Name
 **/

static void CvtStringToResizeParticipants() ;	/* Resource Converter */

/**
 *** Resource Converter Quarks
 **/

/* ResizeParticipants */
static XrmQuark	XrmQEnone, XrmQEinters, XrmQEinternals, XrmQEchildren ;

/**
 *** Resource Converter AddConverter Routine
 **/

void
XcuAddStringToResizeParticipantsConverter ()
{
static Boolean first_time = True ;
if (first_time)
    {
    XrmQEnone   = XrmStringToQuark("none") ;
    XrmQEinters = XrmStringToQuark("inters") ;
    XrmQEinternals = XrmStringToQuark("internals") ;
    XrmQEchildren  = XrmStringToQuark("children") ;
    XtAddConverter (XtRString, XtRResizeParticipants,
		    CvtStringToResizeParticipants, NULL, 0 ) ;
    first_time = False ;
    }
return ;
}

/**
 *** Resource Converter Routine
 **/

#include <ctype.h>

/*
 * Convert String To ResizeParticipants
 */

static void CvtStringToResizeParticipants(args, num_args, fromVal, toVal)
    XrmValuePtr *args ;		/* unused */
    Cardinal	*num_args ;	/* unused */
    XrmValuePtr fromVal ;
    XrmValuePtr toVal ;
{
static XtArgVal	e ; /* must be static! */
XrmQuark	q ;
Cardinal	i ;
Cardinal	j ;
Cardinal	iptr ;
char	*s = (char *) fromVal->addr ;
String  lower_name ;
String	name_ptr ;
extern void XcuReduceArg () ;

if (s == NULL) return ;

XcuReduceArg (s, "xcu", &lower_name, &name_ptr) ;

e = 0 ;

toVal->size = sizeof(XcuResizeParticipants) ;
toVal->addr = (XPointer) &e ;

iptr = 0 ;
for (i=0;  i < strlen(name_ptr)+1;  i++)
    {
    if (name_ptr[i] == '|' || name_ptr[i] == '+' || name_ptr[i] == '\0')
	{
	name_ptr[i] = '\0' ;
	q = XrmStringToQuark(&name_ptr[iptr]) ;
	if      (q == XrmQEnone)      e |= (XtArgVal)XcuRESIZE_NONE ;
	else if (q == XrmQEinters)    e |= (XtArgVal)XcuRESIZE_INTERS ;
	else if (q == XrmQEinternals) e |= (XtArgVal)XcuRESIZE_INTERNALS ;
	else if (q == XrmQEchildren)  e |= (XtArgVal)XcuRESIZE_CHILDREN ;
	else
	    {
	    char text[80] ;
	    sprintf (text, "(%s) is not a valid ResizeParticipants item\n",
		     name_ptr[i]) ;
	    XtError (text) ;
	    }
	iptr = i+1 ;
	}
    }

XtFree (lower_name) ;
return ;
}

