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
#include "Cell.h"

/* This allows your window to be resized */
static Arg toplevel_args[] = { {XtNallowShellResize, (XtArgVal) True} } ;

/* This is how we'll pass the menu description to the wlm widget */
static Arg wlm_args[] = { {XtNfile, (XtArgVal) NULL} } ;

static Widget	toplevel, wlm, cell_widget ;
static Boolean running_flag = True ;

main (argc, argv)
    int argc ;
    char **argv ;
{
static XrmOptionDescRec options[] =
    {
     {"-colors", "*Cell.colors", XrmoptionSepArg,
      "black,white,blue,green,red,yellow" }
    } ;
if (argc < 2)
    {
    fprintf (stderr, "\nUsage: %s wlm_layout_file\n\n", argv[0]) ;
    exit (1) ;
    }

/* Initialize toolkit, create the toplevel shell widget */
toplevel = XtInitialize (NULL, "top", options, XtNumber(options), &argc, argv);
XtSetValues (toplevel, toplevel_args, XtNumber(toplevel_args)) ;

/* Create the widget layout manager widget */
XtSetArg (wlm_args[0], XtNfile, argv[1]) ;
wlm = XtCreateManagedWidget("wlm", xcuWlmWidgetClass, toplevel, wlm_args, ONE);

/* Create associations between menu and program variables and routines */

XcuWlmGetBoolean (wlm, XcuWLM_EVENTS, "XcuBmgr", "Pause", "setCallback",
	          &running_flag) ;


/* Realize the widget tree */
XtRealizeWidget (toplevel) ;

/* Sample the wlm widget */
XcuWlmSample (wlm) ;

cell_widget = XcuWlmInquireWidget (wlm, "XcuCell", "cell") ;

/* Repeatedly request events from the wlm */
for (;;)
    {
    XcuWlmEvent (wlm) ;
    if (running_flag)
	{
	XcuCellStep (cell_widget) ;
	}
    }
}

/*
 * You need one include file for each widget class used
 */

#include <X11/Xcu/WlmP.h>
#include <X11/Xcu/Label.h>
#include <X11/Xcu/Bmgr.h>
#include <X11/Xcu/Button.h>
#include <X11/Xcu/Deck.h>
#include <X11/Xcu/Tbl.h>
#include <X11/Xcu/Command.h>
#include <X11/Xcu/Entry.h>

void
make_tag_class_list (ww)
    XcuWlmWidget ww ;
{
/*
 * You need one entry for each widget class used
 */
TAG_CLASS_ENTRY(ww, "XcuLabel",	xcuLabelWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuWlm",	xcuWlmWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuBmgr",	xcuBmgrWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuButton",xcuButtonWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuDeck",	xcuDeckWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuTbl",	xcuTblWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuCommand",xcuCommandWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuCell",	xcuCellWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuEntry",	xcuEntryWidgetClass) ;
return ;
}

