/*
 * $XConsortium: xtree.c,v 1.7 91/02/17 17:14:08 dave Exp $
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

#include "xtree.h"			/* get Node and externs */

char *ProgramName;			/* for error messages */
XtAppContext app_con;			/* our Application Context */
Widget toplevel;			/* master widget */
Node rootnode;				/* invisible root of tree */
AppResources GlobalResources;		/* global resource data */

#define Offset(field) XtOffsetOf(AppResources, field)

static XtResource resources[] = {
    { "separator", "Separator", XtRString, sizeof(char *),
	Offset(separator), XtRString, (XtPointer) "\t" },
    { "numberBase", "NumberBase", XtRInt, sizeof(int),
	Offset(number_origin), XtRImmediate, (XtPointer) 1 },
    { "inputFormat", "Format", XtRDataFormat, sizeof(Format),
	Offset(input_format), XtRImmediate, (XtPointer) FORMAT_SEPARATOR },
    { "outputFormat", "Format", XtRDataFormat, sizeof(Format),
	Offset(output_format), XtRImmediate, (XtPointer) FORMAT_NAME },
};

#undef offset

static void ActionQuit(), ActionCirculatePanner();
static XtActionsRec actions[] = {
    { "Quit", ActionQuit },
    { "CirculatePanner", ActionCirculatePanner },
};

static Atom wm_delete_window;

static char *fallbacks[] = {
    "*allowShellResize: true",
    "*Porthole.top: ChainTop",
    "*Porthole.left: ChainLeft",
    "*Porthole.bottom: ChainBottom",
    "*Porthole.right:  ChainRight",
    "*Porthole.resizable: on",
    "*Panner.top: ChainTop",
    "*Panner.left: ChainLeft",
    "*Panner.bottom: ChainTop",
    "*Panner.right:  ChainLeft",
    "*Panner.resizable: on",
    "*Porthole*Box.BorderWidth: 0",
    "*Porthole*Box.HSpace: 0",
    "*Porthole*Box.VSpace: 0",
    "*Paned*allowResize: true",
    "*buttonbox.quit.Translations: #override \\n <Btn1Down>,<Btn1Up>: Quit() unset()",
    "*panner.Translations: #augment \\n <Btn3Down>: CirculatePanner()",
    "*tree.Translations: #augment \\n <Btn3Down>: CirculatePanner()",
    (char *) NULL,
};

static XrmOptionDescRec options[] = {
    { "-separator", "Separator", XrmoptionSepArg, (XtPointer) NULL },
    { "-iformat", "inputFormat", XrmoptionSepArg, (XtPointer) NULL },
    { "-oformat", "outputFormat", XrmoptionSepArg, (XtPointer) NULL },
    { "-vertical", "*Tree.Gravity", XrmoptionNoArg, (XtPointer) "north" },
    { "-blw", "*Tree.LineWidth", XrmoptionSepArg, (XtPointer) NULL },
    { "-bbw", "*Tree*Command*BorderWidth", XrmoptionSepArg, (XtPointer) NULL },
    { "-nbase", "numberBase", XrmoptionSepArg, (XtPointer) NULL },
};

static char *help_message[] = {
"where options include:",
"    -separator string                i/o separator character",
"    -vertical                        display tree vertically",
"    -iformat format                  input format (num or sep)",
"    -oformat format                  output format (num, sep, name, res)",
"    -nbase number                    base (0 or 1) for number format",
"",
"formats (abbreviation):",
"    number (num)                     level: label",
"    separator (sep)                  char char label",
"    name                             label (for output only)",
"    resource (res)                   parent.child.subchild (for output only)",
"",
NULL
};

static void usage ()
{
    char **msg;
    fprintf (stderr, "usage:\n\t%s [-options ...]\n\n", ProgramName);
    for (msg = help_message; *msg; msg++) {
	fprintf (stderr, "%s\n", *msg);
    }
    exit (1);
}

main (argc, argv)
    int argc;
    char **argv;
{
    ProgramName = argv[0];

    toplevel = XtAppInitialize (&app_con, "XTree", options, XtNumber(options),
				(Cardinal *)&argc, argv, fallbacks,
				(ArgList) NULL, ZERO);
    if (argc != 1) usage ();

    XtAddConverter (XtRString, XtRDataFormat, cvt_data_format,
		    (XtConvertArgList) NULL, ZERO);
    XtAppAddActions (app_con, actions, XtNumber(actions));
    XtAppAddActions
	(XtWidgetToApplicationContext(toplevel), actions, XtNumber(actions));
    XtOverrideTranslations
	(toplevel, XtParseTranslationTable ("<Message>WM_PROTOCOLS: Quit()"));
    XtGetApplicationResources (toplevel, (caddr_t) &GlobalResources,
			       resources, XtNumber(resources), NULL, ZERO);

    create_ui ();

    read_data (stdin);

    build_tree (&rootnode, (Widget) NULL, FALSE);

    adjust_panner ();

    wm_delete_window = XInternAtom(XtDisplay(toplevel), "WM_DELETE_WINDOW",
				   False);
    (void) XSetWMProtocols (XtDisplay(toplevel), XtWindow(toplevel),
                            &wm_delete_window, 1);

    XtAppMainLoop (app_con);
}

/* ARGSUSED */
static void ActionQuit (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    exit (0);
}

/* ARGSUSED */
static void ActionCirculatePanner (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    circulate_panner_tree();
}
