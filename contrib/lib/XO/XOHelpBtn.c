/*---------------------------- XOHelpBtn.c ----------------------------*/

/*
 *  generic help button for Xt widget-based programs
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
static char _SSS_XOHelpBtn_copyright[] =
    "XOHelpBtn 1.0 Copyright 1990, 1991, Systems & Software Solutions, Inc, Marietta, GA";


#include "Xos.h"
#include "StringDefs.h"

#include "Intrinsic.h"
#include "Shell.h"
#include "Box.h"
#include "Command.h"
#include "Text.h"
#include "AsciiText.h"

#if XT_REVISION == 2 || X_REVISION == 3
#define ASCII_WIDGET_CLASS asciiStringWidgetClass
#else
#define ASCII_WIDGET_CLASS asciiTextWidgetClass
#endif

#include "libXO.h"

/*
 *  help button widget
 */
Arg _helpArg[] = {
    {XtNborderWidth,	(XtArgVal) 3},
};

char *defHelpText[] = "Sorry. It's helpless.";
Arg _HTArg[] = {
    {XtNstring,		(XtArgVal) defHelpText},
};
static void HelpCB ();		/* help callback */

/*
 *  Add help button
 */
Widget
XOCreateHelpBtn (parent, root, cb, res, nres, helpText)

Widget	parent;		/* parent of help button widget */
Widget	root;		/* root of help button widget */
void	(*cb) ();	/* user-supplied callback (if any) */
Arg	*res;		/* additional resources (if any) */
int	nres;		/* # of additional resources (if any) */
char	*helpText;	/* user-supplied help text (they'd better!) */
{
    Widget help;

    if (parent) {
	if (help = XtCreateManagedWidget ("Help", commandWidgetClass,
		parent, _helpArg, XtNumber (_helpArg))) {
	    if (nres > 0 && res) {
		XtSetValues (help, res, nres);
	    }
	    if (cb) {
		XtAddCallback(help, XtNcallback, cb, root);
	    } else {
		XtAddCallback(help, XtNcallback, HelpCB, root);
	    }
	    if (helpText) {
		_HTArg[0].value = (XtArgVal) helpText; /* handy later on */
	    }
	}
    }
    return (Widget) help;
}


static void helpDoneCB();

Arg _HPArg[] = {			/* Help Popup shell defs */
    {XtNlabel, (XtArgVal) NULL},
    {XtNvalue, (XtArgVal) NULL},
    {XtNx, (XtArgVal) 0},
    {XtNy, (XtArgVal) 0},
};

Arg _HBArg[] = {			/* Help box widget defs */
    {XtNlabel, (XtArgVal) NULL},
    {XtNvalue, (XtArgVal) NULL},
};

Arg _HDBArg[] = {			/* Help Done button widget defs */
    {XtNlabel, (XtArgVal) "Done"},
};

static Widget helpPopup = NULL,
    helpBox,
    helpText,
    helpButton;

/*
 *  User pressed Help button - help callback
 *
 *  If the popup doesn't exist, attempt to create it all. If any of
 *  the create fails, we give up on this operation. Otherwise, we
 *  pop it up. If it does exist, we pop it down, then back up. The
 *  extra popdown is in case it's hidden.
 *
 *  Note that the popup shell may not be manageable by non-iC3M window
 *  managers - ie, movable, iconifiable, etc).
 */
static void
HelpCB (w, call_data, event)

Widget w;
caddr_t call_data;
XEvent *event;
{
    if (!helpPopup) {
	_HPArg[2].value = (event ? event->xbutton.x_root - 15 : 100);
	_HPArg[3].value = (event ? event->xbutton.y_root - 15 : 100);
	if (!(helpPopup = XtCreatePopupShell ("helpPopup",
	    transientShellWidgetClass, (Widget) call_data,
		_HPArg, XtNumber (_HPArg)))) {
	    return;
	} else {
	    if (!(helpBox = XtCreateManagedWidget ("helpBox",
		boxWidgetClass, helpPopup, _HBArg, XtNumber (_HBArg))))
		    return;
	    if (!(helpText = XtCreateManagedWidget ("helpText",
		ASCII_WIDGET_CLASS, helpBox, _HTArg, XtNumber (_HTArg))))
		    return;
	    if (!(helpButton = XtCreateManagedWidget ("helpDone",
		commandWidgetClass, helpBox, _HDBArg, XtNumber (_HDBArg))))
		    return;
	    XtAddCallback(helpButton, XtNcallback, helpDoneCB, NULL);
	    XtPopup (helpPopup, XtGrabNone);
	}
    } else {
	XtPopdown (helpPopup);
	XtPopup (helpPopup, XtGrabNone);
    }
}



/*
 *  User pressed Help popup's Done button (callback)
 *
 *  Pop down the help popup box.
*/
static void
helpDoneCB (w, call_data, event)

Widget w;
caddr_t call_data;
XEvent *event;
{
    XtPopdown (helpPopup);
}
