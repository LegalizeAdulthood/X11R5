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
#include <X11/Xcu/Simple.h>
#include <X11/Xcu/Label.h>
#include <X11/Xcu/Tbl.h>
#include <X11/Xaw/Command.h>

static void Activate_text() ;
static void Activate_justify() ;
static void Activate_gravity() ;
static void Activate_fill() ;
static void convert_nl () ;

static XtCallbackRec callback_text[] =
    { { Activate_text, NULL }	,{ NULL, NULL } } ;

static XtCallbackRec callback_justify[] =
    { { Activate_justify, NULL },{ NULL, NULL } } ;

static XtCallbackRec callback_gravity[] =
    { { Activate_gravity, NULL },{ NULL, NULL } } ;

static XtCallbackRec callback_fill[] =
    { { Activate_fill, NULL }	,{ NULL, NULL } } ;

static XrmOptionDescRec options[] =
{
 {"-file",	"*tbl1a.formatFile",	XrmoptionSepArg,	NULL}
,{"-string",	"*tbl1a.formatString",	XrmoptionSepArg,	NULL}
};

Widget toplevel;
Widget lbl[100] ;
Widget tbl0, tbl1a, tbl1b ;
static char lbl_text[100] ;
Cardinal n_widgets ;

static Arg cmd_arg[2] ;

static Arg lbl_args[] =
    {
     {XtNlabel, (XtArgVal) lbl_text}
    ,{XtNfacetWidth, (XtArgVal) 0}
    ,{XtNjustify, (XtArgVal) XcuJUSTIFY_CENTER}
    } ;

static Arg tbl0_args[] =
    {
     {XtNformatString, (XtArgVal) "c c."}
    ,{XtNinterWidth, (XtArgVal) 10}
    ,{XtNinterHeight, (XtArgVal) 10}
    ,{XtNinternalWidth, (XtArgVal) 10}
    ,{XtNinternalHeight, (XtArgVal) 10}
    } ;

static Arg tbl1b_args[] =
    {
     {XtNformatString, (XtArgVal) "c."}
    ,{XtNinterHeight, (XtArgVal) -1}
    ,{XtNinternalWidth, (XtArgVal) 10}
    ,{XtNinternalHeight, (XtArgVal) 10}
    } ;

void
main (argc, argv)
    unsigned int argc;
    char **argv;
{
Cardinal i ;

static Arg toplevel_args[] =
    {
    {XtNallowShellResize, (XtArgVal) True}
    } ;

if (argc < 4)
	{
	fprintf (stderr, "Usage: %s #_of_widgets\n", argv[0]) ;
	fprintf (stderr, "          [-file widget_description_file]\n") ;
	fprintf (stderr, "          [-string widget_description]\n") ;
	fprintf (stderr, "          ... (see source)\n") ;
	exit (1) ;
	}


n_widgets = atoi (argv[1]) ;

toplevel = XtInitialize (NULL, "Demo", options, XtNumber(options), &argc, argv);

/*
 * tbl0 merely places the two layouts side by side
 * tbl1a is the variable test layout
 * tbl1b is the fixed cmd modifier layout
 */

tbl0 = XtCreateManagedWidget ("tbl0", xcuTblWidgetClass, toplevel,
				tbl0_args, XtNumber(tbl0_args)) ;
tbl1a = XtCreateManagedWidget ("tbl1a", xcuTblWidgetClass, tbl0, NULL, NULL) ;
tbl1b = XtCreateManagedWidget ("tbl1b", xcuTblWidgetClass, tbl0,
				tbl1b_args, XtNumber(tbl1b_args)) ;

/*
 * load up the test layout with label widgets
 */

for (i=0; i < n_widgets;  i++)
    {
    char widget_name[100] ;

    sprintf (lbl_text, "ABC\n%d", i) ;
    sprintf (widget_name, "Wdgt%d", i) ;
    lbl[i] = XtCreateManagedWidget (widget_name, xcuLabelWidgetClass, tbl1a,
				    lbl_args, TWO) ;
    }

/*
 * load up the cmd layout with command widgets
 */

XtSetArg (cmd_arg[1], XtNfacetWidth, (XtArgVal) 0) ;
XtSetArg (cmd_arg[0], XtNcallback, (XtArgVal) callback_text) ;
lbl[i++] = XtCreateManagedWidget ("_text_", commandWidgetClass, tbl1b,
				cmd_arg, TWO) ;

XtSetArg (cmd_arg[0], XtNcallback, (XtArgVal) callback_justify) ;
lbl[i++] = XtCreateManagedWidget ("_justify_", commandWidgetClass, tbl1b,
				cmd_arg, TWO) ;

XtSetArg (cmd_arg[0], XtNcallback, (XtArgVal) callback_gravity) ;
lbl[i++] = XtCreateManagedWidget ("_gravity_", commandWidgetClass, tbl1b,
				cmd_arg, TWO) ;

XtSetArg (cmd_arg[0], XtNcallback, (XtArgVal) callback_fill) ;
lbl[i++] = XtCreateManagedWidget ("_fill_", commandWidgetClass, tbl1b,
				cmd_arg, TWO) ;

/*
 * go...
 */

sleep (5) ;
XtRealizeWidget (toplevel) ;
XtSetValues (toplevel, toplevel_args, XtNumber(toplevel_args)) ;
XtMainLoop () ;
}

/*
 * The following four routines all work the same way.
 * They open a file of the same name as the label.
 * The first integer read is the number of widgets to modify
 * This is followed by pairs of widget numbers and new values
 */

/**
 *** Modifies the label of a widget
 **/

#include <X11/IntrinsicP.h>
#include <X11/Xcu/TblP.h>

static void
Activate_text (w, closure, call_data)
    Widget w;
    XPointer closure;
    XPointer call_data;
{
FILE *fd ;
Cardinal i, n ;
int m ;

fd = fopen ("_text_", "r") ;
if (fd <= 0)
	fprintf (stderr, "NO _text_ file!\n") ;
fscanf (fd, "%d", &n) ;
for (i=0;  i < n;  i++)
	{
	fscanf (fd, "%d", &m) ;
	fread (lbl_text, 1, 1, fd) ;
	fscanf (fd, "%[^\n]s", lbl_text) ;
	if (m < 0 || m >= n_widgets)
		{
		fprintf (stderr, "Error: widget %d does not exist\n", m) ;
		continue ;
		}
	convert_nl (lbl_text) ;
	XtSetValues (lbl[m], lbl_args, ONE) ;
	}
fclose (fd) ;
return ;
}

/**
 *** Modifies the justification of the label of a widget
 **/

static void
Activate_justify (w, closure, call_data)
    Widget w;
    XPointer closure;
    XPointer call_data;
{
FILE *fd ;
Cardinal i, n ;
int m ;
XrmValue from, to ;

from.size = sizeof (String) ;
from.addr = (XPointer) lbl_text ;

fd = fopen ("_justify_", "r") ;
if (fd <= 0)
	fprintf (stderr, "NO _justify_ file!\n") ;
fscanf (fd, "%d", &n) ;

for (i=0;  i < n;  i++)
	{
	fscanf (fd, "%d", &m) ;
	fscanf (fd, "%s", lbl_text) ;
	if (m < 0 || m >= n_widgets)
		{
		fprintf (stderr, "Error: widget %d does not exist\n", m) ;
		continue ;
		}
	XtConvert (toplevel, XtRString, &from, XtRJustify, &to) ;
	XtSetArg(lbl_args[2], XtNjustify, (XtArgVal) (*((XcuJustify *)to.addr)));
	XtSetValues (lbl[m], &lbl_args[2], ONE) ;
	}
fclose (fd) ;
return ;
}

/**
 *** Modifies the gravity of a widget
 **/

static void
Activate_gravity (w, closure, call_data)
    Widget w;
    XPointer closure;
    XPointer call_data;
{
FILE *fd ;
Cardinal i, n ;
int m ;
XrmValue from, to ;

from.size = sizeof (String) ;
from.addr = (XPointer) lbl_text ;

fd = fopen ("_gravity_", "r") ;
if (fd <= 0)
	fprintf (stderr, "NO _gravity_ file!\n") ;
fscanf (fd, "%d", &n) ;

for (i=0;  i < n;  i++)
	{
	XcuLabelWidget lw ;

	fscanf (fd, "%d", &m) ;
	fscanf (fd, "%s", lbl_text) ;
	if (m < 0 || m >= n_widgets)
		{
		fprintf (stderr, "Error: widget %d does not exist\n", m) ;
		continue ;
		}
	XtConvert (toplevel, XtRString, &from, XtRGravity, &to) ;
	XtSetArg(lbl_args[2], XtNgravity, (XtArgVal) (*((XcuGravity *)to.addr)));
	lw = (XcuLabelWidget) lbl[m] ;
	XtSetValues (lbl[m], &lbl_args[2], ONE) ;
	}
fclose (fd) ;
return ;
}

/**
 *** Modifies the fill mode of a widget
 **/

static void
Activate_fill (w, closure, call_data)
    Widget w;
    XPointer closure;
    XPointer call_data;
{
FILE *fd ;
Cardinal i, n ;
int m ;
XrmValue from, to ;

from.size = sizeof (String) ;
from.addr = (XPointer) lbl_text ;

fd = fopen ("_fill_", "r") ;
if (fd <= 0)
	fprintf (stderr, "NO _fill_ file!\n") ;
fscanf (fd, "%d", &n) ;

for (i=0;  i < n;  i++)
	{
	XcuLabelWidget lw ;

	fscanf (fd, "%d", &m) ;
	fscanf (fd, "%s", lbl_text) ;
	if (m < 0 || m >= n_widgets)
		{
		fprintf (stderr, "Error: widget %d does not exist\n", m) ;
		continue ;
		}
	XtConvert (toplevel, XtRString, &from, XtRBoolean, &to) ;
	XtSetArg(lbl_args[2], XtNfillColumn, (XtArgVal)(*((Boolean *)to.addr)));
	lw = (XcuLabelWidget) lbl[m] ;
	XtSetValues (lbl[m], &lbl_args[2], ONE) ;
	}
fclose (fd) ;
return ;
}

/***** **** *** ** * convert_nl * ** *** **** *****/

/*
 * convert instances of "\n" into '\n' and collapse the remainder of the text
 */

static void
convert_nl (text)
    char *text ;
{
Cardinal i, j ;
Cardinal len = strlen(text) ;

for (i=0;  i < len;  i++)
    {
    if (text[i] == '\\' && text[i+1] == 'n')
	{
	text[i] = '\n' ;
	j = i+1 ;
	while (text[j] != 0)
	    {
	    text[j] = text[j+1] ;
	    j++ ;
	    }
	}
    }
return ;
}


