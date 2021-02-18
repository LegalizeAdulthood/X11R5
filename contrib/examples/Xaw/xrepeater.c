/*
 * This an example of how to use the Repeater widget.
 */

/*
 * $XConsortium: xrepeater.c,v 1.5 91/01/22 19:24:56 gildea Exp $
 *
 * Copyright 1989 Massachusetts Institute of Technology
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
 */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>	/* Get standard string definations. */

#include <X11/Xaw/Repeater.h>
#include <X11/Xaw/Cardinals.h>	

char *ProgramName;

String fallback_resources[] = { 
    "*Repeater.Label:    Press Me",
    NULL,
};

static XrmOptionDescRec options[] = {
{"-label",	"*Repeater.label",	XrmoptionSepArg,	NULL},
{"-bitmap",	"*Repeater.bitmap",	XrmoptionSepArg,	NULL},
{"-flash",	"*Repeater.flash",	XrmoptionNoArg,	(caddr_t) "on" },
{"-decay",	"*Repeater.decay",	XrmoptionSepArg,	NULL},
{"-initial",	"*Repeater.initialDelay",	XrmoptionSepArg,	NULL},
{"-repeat",	"*Repeater.repeatDelay",	XrmoptionSepArg,	NULL},
{"-minimum",	"*Repeater.minimumDelay",	XrmoptionSepArg,	NULL},
};

static char *helpmsg[] = {
    "-label string              button label",
    "-bitmap name               button bitmap",
    "-flash                     flash on repeats",
    "-decay number              repeat decay in milliseconds",
    "-initial number            initial delay in milliseconds",
    "-repeat number             repeat delay in milliseconds",
    "-minimum number            minimum repeat delay in milliseconds",
NULL };

static void usage (app_con)
    XtAppContext app_con;
{
    char **msg;

    XtDestroyApplicationContext (app_con);

    fprintf (stderr, "Usage:  %s [-options]\n", ProgramName);
    fprintf (stderr, "\nwhere options include:\n");
    for (msg = helpmsg; *msg; msg++) fprintf (stderr, "    %s\n", *msg);
    fprintf (stderr, "\n");
    exit(1);
}

/*	Function Name: Select
 *	Description: This function prints a message to stdout.
 *	Arguments: w - ** UNUSED **
 *                 call_data - ** UNUSED **
 *                 client_data - ** UNUSED **
 *	Returns: none
 */

static int count = 0;

static void Select (w, client_data, call_data)
    Widget w;
    XtPointer call_data, client_data;
{
    printf ("Press #%d\n", ++count);
}

static void Start (w, client_data, call_data)
    Widget w;
    XtPointer call_data, client_data;
{
    printf ("Starting...\n");
    count = 0;
}

static void Stop (w, client_data, call_data)
    Widget w;
    XtPointer call_data, client_data;
{
    printf ("...Stopping\n");
}

main (argc, argv)
    int argc;
    char **argv;
{
    XtAppContext app_con;
    Widget toplevel, repeater;

    ProgramName = argv[0];

    toplevel = XtAppInitialize(&app_con, "XRepeater",
			       options, XtNumber(options),
			       &argc, argv, fallback_resources,
			       NULL, ZERO);

    if (argc != 1) usage (app_con);

    repeater = XtCreateManagedWidget("repeater", repeaterWidgetClass, toplevel,
				    NULL, ZERO);

    /*
     * Add a callback routine to the Repeater widget that will print
     * the message "button selected" to stdout.
     */

    XtAddCallback (repeater, XtNcallback, Select, NULL);
    XtAddCallback (repeater, XtNstartCallback, Start, NULL);
    XtAddCallback (repeater, XtNstopCallback, Stop, NULL);

    XtRealizeWidget(toplevel);
    XtAppMainLoop(app_con);
}
