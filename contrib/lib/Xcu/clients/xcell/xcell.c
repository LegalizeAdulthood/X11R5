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


#define CDEBUG
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include "Cell.h"

/* This allows your window to be resized */
static Arg toplevel_args[] = { {XtNallowShellResize, (XtArgVal) True} } ;

#ifdef CDEBUG
XtErrorHandler handler () ;
#endif
static Widget	toplevel, cell_widget ;
static void usage () ;

main (argc, argv)
    int argc ;
    char **argv ;
{
int i ;
static XrmOptionDescRec options[] =
 {
  {"-colors", "*Cell.colors", XrmoptionSepArg, "black,white,blue,green,red,yellow" }
 ,{"-directory","*Cell.directory",	XrmoptionSepArg, NULL }
 ,{"-ignore",	"*Cell.ignoreColors",	XrmoptionSepArg, NULL }
 ,{"-rules",	"*Cell.rulesFile",	XrmoptionSepArg, NULL }
 ,{"-alternate","*Cell.alternating",	XrmoptionNoArg, "TRUE" }
 ,{"-perturb",	"*Cell.perturb",	XrmoptionNoArg, "TRUE" }
 ,{"-pick",	"*Cell.pickFile",	XrmoptionSepArg, NULL }
 ,{"-index",	"*Cell.ruleIndex",	XrmoptionSepArg, NULL }
 ,{"-ncolors",	"*Cell.nColors",	XrmoptionSepArg, NULL }
 ,{"-seed",	"*Cell.seed",		XrmoptionSepArg, NULL }
 ,{"-lcr3",	"*Cell.cellType",	XrmoptionNoArg, "lcr3" }
 ,{"-lcr4",	"*Cell.cellType",	XrmoptionNoArg, "lcr4" }
 ,{"-lcr5",	"*Cell.cellType",	XrmoptionNoArg, "lcr5" }
 ,{"-lcr6",	"*Cell.cellType",	XrmoptionNoArg, "lcr6" }
 ,{"-lccr4",	"*Cell.cellType",	XrmoptionNoArg, "lccr4" }
 ,{"-lccrrr4",	"*Cell.cellType",	XrmoptionNoArg, "lccrrr4" }
 ,{"-klcrt3",	"*Cell.cellType",	XrmoptionNoArg, "klcrt3" }
 ,{"-klcrt4",	"*Cell.cellType",	XrmoptionNoArg, "klcrt4" }
 ,{"-klcrt5",	"*Cell.cellType",	XrmoptionNoArg, "klcrt5" }
 ,{"-cols",	"*Cell.perRow",		XrmoptionSepArg, NULL }
 ,{"-rows",	"*Cell.perCol",		XrmoptionSepArg, NULL }
 ,{"-ordered_rules","*Cell.orderedRules",XrmoptionNoArg,  "TRUE" }
 ,{"-vmerge",	"*Cell.verticalMerge",	XrmoptionNoArg,  "TRUE" }	
 ,{"-hmerge",	"*Cell.horizontalMerge",XrmoptionNoArg,  "TRUE" }	
 ,{"-iwidth",	"*Cell.internalWidth",	XrmoptionSepArg, NULL }	
 ,{"-iheight",	"*Cell.internalHeight",	XrmoptionSepArg, NULL }	
 ,{"-bg",	"*Background",		XrmoptionSepArg, NULL }	
 ,{"-shadow",	"*Cell.shadow",		XrmoptionNoArg, "TRUE" }	
 ,{"-fwidth",	"*FacetWidth",		XrmoptionSepArg, NULL }	
 ,{"-ordered_color_permutations",	"*Cell.sequence",	XrmoptionNoArg,  "ordered_permute" }
 ,{"-random_color_permutations","*Cell.sequence",	XrmoptionNoArg, "random_permute" }
 ,{"-random_colors","*Cell.sequence",	XrmoptionNoArg,  "RANDOM_COLOR" }
 ,{"-fixed_colors","*Cell.frequency",	XrmoptionNoArg,  "FIXED_FREQUENCY" }
 ,{"-page_colors","*Cell.frequency",	XrmoptionNoArg,  "PAGE_FREQUENCY" }
 ,{"-rule_colors","*Cell.frequency",	XrmoptionNoArg,  "RULE_FREQUENCY" }
 ,{"-line_colors","*Cell.frequency",	XrmoptionNoArg,  "LINE_FREQUENCY" }
 ,{"-no_beep",	"*Cell.beep",		XrmoptionNoArg,  "FALSE" }
 ,{"-mutate",	"*Cell.mutate",		XrmoptionNoArg,  "TRUE" }
 } ;

toplevel = XtInitialize("main", "XCell", options, XtNumber(options),
			&argc, argv);
#ifdef CDEBUG
XtSetErrorHandler (handler) ;
XSetIOErrorHandler (handler) ;
#endif

for (i=1;  i < argc;  i++)
    {
    /*
     * Might as well do a half-baked usage check at least
     */
    int found = 0 ;
    if (argv[i][0] != '-')
	continue ;
    if (!found)
	{
	int j ;
	for (j=0;  j < XtNumber(options);  j++)
	    {
	    if (strcmp (options[j].option, argv[i]) == 0)
		break ;
	    }
	if (
	   j == XtNumber(options) &&
	   strcmp(argv[i], "-geometry") &&
	   strcmp(argv[i], "-synchronize")
	   )
	if ( j == XtNumber(options) )
	    {
	    usage () ;
	    exit (1) ;
	    }
	}
    }

/* Perhaps this shouldn't be done? */
XtSetValues (toplevel, toplevel_args, XtNumber(toplevel_args)) ;

/* Create the cell widget */
cell_widget = XtCreateManagedWidget("cell", xcuCellWidgetClass, toplevel,
				    NULL, 0);

/* Realize the widget tree */
XtRealizeWidget (toplevel) ;

for (;;)
    {
    XtInputMask mask ;
    mask = XtPending () ;
    if (mask)
	{
	XEvent event ;
	XtNextEvent (&event) ;
	XtDispatchEvent (&event) ;
	}
    XcuCellStep (cell_widget) ;
    }
}

static void
usage ()
{
fprintf (stderr, "-colors color1,color2,...\n") ;
fprintf (stderr, "-ncolors %%d\n") ;
fprintf (stderr, "-ignore index,...\n") ;
fprintf (stderr, "-rules filename\n") ;
fprintf (stderr, "-index %%d\n") ;
fprintf (stderr, "-ordered_rules\n") ;
fprintf (stderr, "-alternate\n") ;
fprintf (stderr, "-perturb\n") ;
fprintf (stderr, "-lcr3 | -lcr4 | -lcr5 | -lcr6 | -lccr4 | -lccrrr4 \n") ;
fprintf (stderr, "      | -klcrt3 | -klcrt4 | -klcrt5\n") ;
fprintf (stderr, "-ordered_color_permutations | -random_color_permutations | -random_colors\n") ;
fprintf (stderr, "-fixed_colors | -page_colors | -rule_colors | -line_colors\n") ;
fprintf (stderr, "-pick filename\n") ;
fprintf (stderr, "-directory path\n") ;
fprintf (stderr, "-seed %%d\n") ;
fprintf (stderr, "-cols %%d\n") ;
fprintf (stderr, "-rows %%d\n") ;
fprintf (stderr, "-vmerge\n") ;
fprintf (stderr, "-hmerge\n") ;
fprintf (stderr, "-iwidth %%d\n") ;
fprintf (stderr, "-iheight %%d\n") ;
fprintf (stderr, "-bg color\n") ;
fprintf (stderr, "-shadow\n") ;
fprintf (stderr, "-fwidth %%d\n") ;
fprintf (stderr, "-no_beep\n") ;
fprintf (stderr, "-mutate\n") ;
return ;
}
#ifdef CDEBUG
XtErrorHandler
handler (message)
    char *message ;
{
fprintf (stderr, "\nHANDLER\n%s\n", message) ;
abort () ;
}
#endif

