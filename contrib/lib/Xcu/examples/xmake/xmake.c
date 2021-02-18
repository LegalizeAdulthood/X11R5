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
#include <X11/Xaw/Cardinals.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xcu/Wlm.h>

static Arg toplevel_args[] =
    {
     {XtNallowShellResize, (XtArgVal) True}
    } ;

static Arg wlm_args[] =
    {
     {XtNfile, (XtArgVal) NULL}
    } ;

Widget toplevel, wlm ;

main (argc, argv)
    int argc ;
    char **argv ;
{
Cardinal i ;

if (argc < 2)
    {
    fprintf (stderr, "\nUsage: %s wlm_layout_file\n\n",
	     argv[0]) ;
    exit (1) ;
    }

toplevel = XtInitialize (NULL, "top", NULL, NULL, &argc, argv);

XtSetArg (wlm_args[0], XtNfile, argv[1]) ;
XtSetValues (toplevel, toplevel_args, XtNumber(toplevel_args)) ;
wlm = XtCreateManagedWidget ( "wlm", xcuWlmWidgetClass, toplevel, wlm_args, ONE) ;

XtRealizeWidget (toplevel) ;

XcuWlmSample (wlm) ;
for (;;)
    {
    fprintf (stderr, "\nMaking a request...\n") ;
    XcuWlmRequest ( wlm) ;
    }
}

#include "xmake.wh"
