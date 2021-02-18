/*----------------------------- XOQuitBtn.c -------------------------------*/

/*
 *  general-purpose quit button for Xt-based clients
 *
 *  Author:  Miles O'Neal
 *
 *  Release: 1.0 meo - FCS release
 *
 *  COPYRIGHT 1990, 1991, Systems & Software Solutions, Inc, Marietta, GA
 *
 *  Permission is hereby granted to use, copy, modify and/or distribute
 *  this software and supporting documentation, for any purpose and
 *  without fee, as long as this copyright notice and disclaimer are
 *  included in all such copies and derived works, and as long as
 *  neither the author's nor the copyright holder's name are used
 *  in publicity or distribution of said software and/or documentation
 *  without prior, written permission.
 *
 *  This software is presented as is, with no warranties expressed or
 *  implied, including implied warranties of merchantability and
 *  fitness. In no event shall the author be liable for any special,
 *  direct, indirect or consequential damages whatsoever resulting
 *  from loss of use, data or profits, whether in an action of
 *  contract, negligence or other tortious action, arising out of
 *  or in connection with the use or performance of this software.
 *  In other words, you are on your own. If you don't like it, don't
 *  use it!
 */

static char _SSS_XOQuitBtn_copyright[] =
    "XOQuitBtn 1.0 Copyright 1990, 1991, Systems & Software Solutions, Inc, Marietta, GA";


#include "Xos.h"
#include "StringDefs.h"

#include "Intrinsic.h"
#include "Command.h"

#include "libXO.h"

/*
 *  quit button widget args
 */
static Arg quitArg[] = {
    {XtNborderWidth, (XtArgVal) 3},
};
static void QuitCB ();		/* quit callback */

/*
 *  Add quit button to a client. If no callback is specified, a default
 *  is supplied to cleanly exit the client.
 */
Widget
XOCreateQuitBtn (parent, root, cb, res, nres)

Widget	parent;		/* parent widget of quit button widget */
Widget	root;		/* root widget of quit button widget */
void	(*cb) ();	/* user-supplied callback (if any) */
Arg	*res;		/* additional resources (if any) */
int	nres;		/* # of additional resources (if any) */
{
    Widget quit;

    if (parent) {
	if (quit = XtCreateManagedWidget ("Quit", commandWidgetClass,
		parent, quitArg, XtNumber (quitArg))) {
	    if (nres > 0 && res) {
		XtSetValues (quit, res, nres);
	    }
	    if (cb) {
		XtAddCallback(quit, XtNcallback, cb, NULL);
	    } else {
		XtAddCallback(quit, XtNcallback, QuitCB, NULL);
	    }
	}
    }
    return quit;
}

/*
 *  User pressed Quit button
 */
static void
QuitCB (w, call_data, event)

Widget w;
caddr_t call_data;
XEvent *event;
{
    XtCloseDisplay (XtDisplay (w)); 
    exit (0);
}



