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
#include <X11/Xcu/FloatR.h>
#include <stdio.h>

/**
 *** Resource Converter Name
 **/

static void CvtStringToFloat() ;

/**
 *** Resource Converter AddConverter Routine
 **/

void
XcuAddStringToFloatConverter ()
{
static Boolean first_time = True ;
if (first_time)
    {
    XtAddConverter( XtRString, XtRFloat, CvtStringToFloat, NULL, 0 ) ;
    first_time = False ;
    }
return ;
}

/**
 *** Resource Converter Routine
 **/

#define	done(address, type) \
	{ (*toVal).size = sizeof(type); (*toVal).addr = (XPointer) address; }

static void CvtStringToFloat(args, num_args, fromVal, toVal)
    XrmValuePtr args;
    Cardinal    *num_args;
    XrmValuePtr	fromVal;
    XrmValuePtr	toVal;
{
    static float f;

    if (*num_args != 0)
	XtWarning("String to Float conversion needs no extra arguments");

    if (sscanf((char *)fromVal->addr, "%f", &f) == 1)
	{
	done (&f, float);
	}
    else
	{
	XtStringConversionWarning((char *) fromVal->addr, "Float");
	}
}


