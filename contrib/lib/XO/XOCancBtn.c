/*----------------------------- XOCancBtn.c -------------------------------*/

/*
 *  general-purpose cancel button for Xt-based clients
 *
 *  Author:  Miles O'Neal
 *
 *  Release: 1.0 meo - FCS release
 *
 *  COPYRIGHT 1991, Systems & Software Solutions, Inc, Marietta, GA
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

static char _SSS_XOCancBtn_copyright[] =
    "XOCancBtn 1.0 Copyright 1991, Systems & Software Solutions, Inc, Marietta, GA";


#include "Xos.h"
#include "StringDefs.h"

#include "Intrinsic.h"
#include "Command.h"

#include "libXO.h"

/*
 *  cancel button widget args
 */
static Arg cancelArg[] = {
    {XtNborderWidth, (XtArgVal) 3},
};
static void CancelCB ();		/* cancel callback */

/*
 *  Add cancel button to a client. If no callback is specified, a default
 *  is supplied to cleanly exit the client.
 */
Widget
XOCreateCancelBtn (parent, root, cb, res, nres)

Widget	parent;		/* parent widget of cancel button widget */
Widget	root;		/* root widget of cancel button widget */
void	(*cb) ();	/* user-supplied callback (if any) */
Arg	*res;		/* additional resources (if any) */
int	nres;		/* # of additional resources (if any) */
{
    Widget cancel;

    if (parent) {
	if (cancel = XtCreateManagedWidget ("Cancel", commandWidgetClass,
		parent, cancelArg, XtNumber (cancelArg))) {
	    if (nres > 0 && res) {
		XtSetValues (cancel, res, nres);
	    }
	    if (cb) {
		XtAddCallback(cancel, XtNcallback, cb, NULL);
	    } else {
		XtAddCallback(cancel, XtNcallback, CancelCB, NULL);
	    }
	}
    }
    return cancel;
}

/*
 *  User pressed Cancel button
 */
static void
CancelCB (w, call_data, event)

Widget w;
caddr_t call_data;
XEvent *event;
{
    XtCloseDisplay (XtDisplay (w)); 
    exit (1);
}



