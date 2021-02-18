/*
 * $XConsortium: xpanner.c,v 1.3 91/01/22 19:55:25 gildea Exp $
 *
 * Copyright 1990 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Jim Fulton, MIT X Consortium
 */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Panner.h>

char *ProgramName;

static String fallback_resources[] = {
    "*Panner.CanvasWidth: 1200",
    "*Panner.CanvasHeight: 1000",
    "*Panner.SliderWidth: 400",
    "*Panner.SliderHeight: 500",
    NULL
};

static XrmOptionDescRec options[] = {
    { "-cw", "*Panner.CanvasWidth", XrmoptionSepArg, NULL },
    { "-ch", "*Panner.CanvasHeight", XrmoptionSepArg, NULL },
    { "-sw", "*Panner.SliderWidth", XrmoptionSepArg, NULL },
    { "-sh", "*Panner.SliderHeight", XrmoptionSepArg, NULL },
    { "-sc", "*shadowColor", XrmoptionSepArg, NULL },
    { "-bs", "*backgroundStipple", XrmoptionSepArg, NULL },
};

static void usage ()
{
    fprintf (stderr, "usage:  %s [-options ...]\n", ProgramName);
    fprintf (stderr, "where options include:\n");
    fprintf (stderr, "    -cw number        canvas width\n");
    fprintf (stderr, "    -ch number        canvas height\n");
    fprintf (stderr, "    -sw number        slider width\n");
    fprintf (stderr, "    -sh number        slider height\n");
    fprintf (stderr, "    -bs string        background stipple name\n");
    fprintf (stderr, "\n");
    exit (1);
}

static void report (w, closure, data)
    Widget w;
    XtPointer closure;
    XtPointer data;
{
    XawPannerReport *rep = (XawPannerReport *) data;
    printf ("slider %dx%d+%d+%d in canvas %dx%d\n", 
	    rep->slider_width, rep->slider_height,
	    rep->slider_x, rep->slider_y, 
	    rep->canvas_width, rep->canvas_height);
}

main (argc, argv)
    int argc;
    char **argv;
{
    Widget toplevel;
    XtAppContext app;
    Arg args[1];
    static XtCallbackRec cbrec[2] = {{ report, NULL }, { NULL, NULL }};

    ProgramName = argv[0];

    toplevel = XtAppInitialize (&app, "XPanner", options, XtNumber(options),
				&argc, argv, fallback_resources,
				NULL, ZERO);
    if (argc != 1) usage ();

    XtSetArg (args[0], XtNcallback, cbrec);
    (void) XtCreateManagedWidget ("panner", pannerWidgetClass, toplevel,
				  args, ONE);
    XtRealizeWidget (toplevel);
    XtAppMainLoop (app);
}
