/*
 * Copyright 1991 by David A. Curry
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation.  The
 * author makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied warranty.
 */
#ifndef lint
static char	*RCSid = "$Header: /home/orchestra/davy/progs/xpostit/RCS/xpostit.c,v 1.4 91/09/06 18:29:40 davy Exp $";
#endif

/*
 * xpostit.c - Post-It Notes for the X Window System.
 *
 * This version is for X11R4/R5 and uses the Xt toolkit and Athena widgets.
 *
 * David A. Curry
 * Purdue University
 * Engineering Computer Network
 * West Lafayette, IN 47907
 * davy@ecn.purdue.edu
 *
 * $Log:	xpostit.c,v $
 * Revision 1.4  91/09/06  18:29:40  davy
 * Added copyright/permission notice for submission to MIT R5 contrib.
 * 
 * Revision 1.3  91/09/06  17:15:13  davy
 * New features, bug fixes, etc.
 * 
 * Revision 1.2  90/06/14  11:21:24  davy
 * Ported to X11 Release 4.
 * 
 * Revision 1.1  90/06/13  09:48:51  davy
 * Initial revision
 * 
 */
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <signal.h>
#include <stdio.h>

#include "xpostit.h"

/*
 * Command line options and the resources they set.
 */
static XrmOptionDescRec options[] = {
    { "-bs",	".bufSize",		XrmoptionSepArg,	NULL },
    { "-dir",	".noteDir",		XrmoptionSepArg,	NULL },
    { "-sb",	".scrollBar",		XrmoptionNoArg,		"true" },
    { "-sv",	".saveNotes",		XrmoptionNoArg,		"true" }
};

/*
 * Fallback resources.
 */
static String fallback_resources[] = {
    "*Command*font: -adobe-helvetica-bold-r-normal--11-*-*-*-*-*-*-*",
    "*Menu*font:    -adobe-helvetica-medium-r-normal--12-*-*-*-*-*-*-*",
    "*Text*font:    -b&h-lucidatypewriter-medium-r-normal-*-12-*-*-*-*-*-*-*",

    "*Text*wrap:		word",
    "*Command*shapeStyle:	oval",

    NULL
};
	
/*
 * Resources we maintain besides those maintained by the toolkit.
 */
static XtResource resources[] = {
#define offset(field)	XtOffset(AppResPtr,field)
    { "bufSize", "BufSize", XtRInt, sizeof(int),
      offset(buf_size), XtRImmediate, (caddr_t) DefaultBufSize },
    { "noteDir", "NoteDir", XtRString, sizeof(String),
      offset(note_dir), XtRString, DefaultNoteDir },
    { "saveNotes", "SaveNotes", XtRBoolean, sizeof(Boolean),
      offset(save_notes), XtRImmediate, (caddr_t) False },
    { "scrollBar", "Scroll", XtRBoolean, sizeof(Boolean),
      offset(scroll_bar), XtRImmediate, (caddr_t) False },
#undef offset
};

AppRes	app_res;		/* xpostit application resources	*/
Widget	toplevel;		/* top level application shell widget	*/
Screen	*screen;		/* pointer to the screen of the display	*/
Display	*display;		/* pointer to the display we're on	*/
XtAppContext appcontext;	/* application context			*/

void
main(argc, argv)
char **argv;
int argc;
{
	Arg args[4];
	char *appname;
	char *rindex();
	register int nargs;
	Boolean setsigs = False;

	/*
	 * Ignore signals for now, but record whether they were
	 * already ignored or not so we can catch them later if
	 * need be.
	 */
	if ((signal(SIGQUIT, SIG_IGN) != SIG_IGN) &&
	    (signal(SIGINT, SIG_IGN) != SIG_IGN))
		setsigs = True;

	/*
	 * Get application name.
	 */
	if ((appname = rindex(*argv, '/')) == NULL)
		appname = *argv;
	else
		appname++;

	/*
	 * Initialize the toolkit and create an application shell.
	 */
	toplevel = XtAppInitialize(&appcontext, PostItNoteClass, options,
			XtNumber(options), &argc, argv, fallback_resources,
			NULL, 0);

	display = XtDisplay(toplevel);
	screen = DefaultScreenOfDisplay(display);

	/*
	 * If we need to handle keyboard signals, do it now.
	 */
	if (setsigs) {
		signal(SIGQUIT, ByeBye);
		signal(SIGINT, ByeBye);
	}

	/*
	 * Always handle these.
	 */
	signal(SIGTERM, ByeBye);
	signal(SIGHUP, ByeBye);

	/*
	 * Send X errors to the exit routine.
	 */
	XSetErrorHandler((XErrorHandler) ByeBye);

	/*
	 * Now get any resources we're interested in.
	 */
	XtGetApplicationResources(toplevel, &app_res, resources,
				  XtNumber(resources), argv, argc);

	/*
	 * Construct the path to the directory notes are
	 * stored in.
	 */
	SetNoteDir();

	/*
	 * Create the plaid and menu widgets.
	 */
	CreatePlaidWidget();
	CreateMenuWidget();

	/*
	 * Realize the top level and flush the server, which will
	 * let the user position the plaid window and map it.
	 */
	XtRealizeWidget(toplevel);
	XFlush(display);

	/*
	 * Load the notes the user has saved, and create widgets
	 * for them.
	 */
	LoadSavedNotes();

	/*
	 * Never returns.
	 */
	XtAppMainLoop(appcontext);
}
