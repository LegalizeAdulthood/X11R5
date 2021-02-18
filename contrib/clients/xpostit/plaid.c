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
static char	*RCSid = "$Header: /home/orchestra/davy/progs/xpostit/RCS/plaid.c,v 1.5 91/09/06 18:29:27 davy Exp $";
#endif

/*
 * plaid.c - routines for manipulating the plaid widget.
 *
 * David A. Curry
 * Purdue University
 * Engineering Computer Network
 * West Lafayette, IN 47907
 * davy@ecn.purdue.edu
 *
 * $Log:	plaid.c,v $
 * Revision 1.5  91/09/06  18:29:27  davy
 * Added copyright/permission notice for submission to MIT R5 contrib.
 * 
 * Revision 1.4  91/09/06  17:13:50  davy
 * Changed my address, added callbacks.
 * 
 * Revision 1.3  90/06/14  11:20:42  davy
 * Ported to X11 Release 4.  Got rid of button callback from the Plaid widget,
 * since it handles its own actions now.
 * 
 * Revision 1.2  89/01/10  09:30:30  davy
 * Fixed menu call-up so that the menu is always on the screen.
 * 
 * Revision 1.1  89/01/10  09:13:59  davy
 * Initial revision
 * 
 */
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <stdio.h>

#include "xpostit.h"
#include "Plaid.h"

Widget		plaidwidget;

/*
 * CreatePlaidWidget - create the plaid widget.
 */
void
CreatePlaidWidget()
{
	Arg args[4];
	register int nargs;

	/*
	 * Create the plaid widget.
	 */
	plaidwidget = XtCreateWidget("Plaid", plaidWidgetClass, toplevel,
				     NULL, 0);

	XtAddCallback(plaidwidget, XtNlowerCallback, LowerAllNotes, 0);
	XtAddCallback(plaidwidget, XtNraiseCallback, RaiseAllNotes, 0);
	XtAddCallback(plaidwidget, XtNhideCallback, HideAllNotes, 0);
	XtAddCallback(plaidwidget, XtNshowCallback, ShowAllNotes, 0);
	XtAddCallback(plaidwidget, XtNquitCallback, ByeBye, 0);
	
	/*
	 * Get the width and height of the widget.
	 */
	nargs = 0;
	SetArg(XtNwidth, NULL);
	SetArg(XtNheight, NULL);
	XtGetValues(plaidwidget, args, nargs);

	/*
	 * If the user didn't set them, then we
	 * should set them to the defaults.
	 */
	if ((args[0].value == 0) || (args[1].value == 0)) {
		if (args[0].value == 0)
			XtSetArg(args[0], XtNwidth, DefaultPlaidWidth);

		if (args[1].value == 0)
			XtSetArg(args[1], XtNheight, DefaultPlaidHeight);

		XtSetValues(plaidwidget, args, nargs);
	}

	/*
	 * Inform the application shell we're here.
	 */
	XtManageChild(plaidwidget);
}
