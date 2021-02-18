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
static char	*RCSid = "$Header: /home/orchestra/davy/progs/xpostit/RCS/title.c,v 1.2 91/09/06 18:29:29 davy Exp $";
#endif

/*
 * title.c - handle getting a new title from the user.
 *
 * David A. Curry
 * Purdue University
 * Engineering Computer Network
 * West Lafayette, IN 47907
 * davy@ecn.purdue.edu
 *
 * $Log:	title.c,v $
 * Revision 1.2  91/09/06  18:29:29  davy
 * Added copyright/permission notice for submission to MIT R5 contrib.
 * 
 * Revision 1.1  91/09/06  17:14:43  davy
 * Initial revision
 * 
 */
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Shell.h>
#include <stdio.h>

#include "xpostit.h"

Widget		dialog;
static Widget	titlewidget;

static void	TitleOkay();
static void	TitleCancel();

/*
 * GetNoteTitle - return the note's new title.
 */
char *
GetNoteTitle(pn, okayCallback, cancelCallback)
XtCallbackProc okayCallback, cancelCallback;
PostItNote *pn;
{
	Arg args[8];
	Window root, child;
	unsigned int buttons;
	register int nargs, nwidgets;
	static Boolean inited = False;
	int root_x, root_y, child_x, child_y;

	/*
	 * Find out where the mouse is, so we can put the confirmation
	 * box right there.
	 */
	XQueryPointer(display, XtWindow(toplevel), &root, &child,
		      &root_x, &root_y, &child_x, &child_y, &buttons);

	/*
	 * If we need to construct the confirmation box do that,
	 * otherwise just reset the position and callbacks and
	 * put it up again.
	 */
	if (!inited) {
		nargs = 0;
		SetArg(XtNx, root_x);
		SetArg(XtNy, root_y);
		SetArg(XtNallowShellResize, True);

		/*
		 * The dialog box will be a pop-up widget.
		 */
		titlewidget = XtCreatePopupShell("Title",
						   overrideShellWidgetClass,
						   toplevel, args, nargs);

		nargs = 0;
		SetArg(XtNlabel, "New Title:");
		SetArg(XtNvalue, pn->pn_title);

		dialog = XtCreateWidget("Dialog", dialogWidgetClass,
					titlewidget, args, nargs);

		nargs = 0;
		SetArg(XtNresizable, True);
		XtSetValues(XtNameToWidget(dialog, "value"), args, nargs);

		XawDialogAddButton(dialog, "Okay", okayCallback,
				   pn->pn_index);
		XawDialogAddButton(dialog, "Cancel", cancelCallback,
				   pn->pn_index);

		/*
		 * Let the shell widget know we're here.
		 */
		XtManageChild(dialog);

		XtRealizeWidget(titlewidget);
		inited = True;
	}
	else {
		/*
		 * Reset the title box position and value.
		 */
		nargs = 0;
		SetArg(XtNx, root_x);
		SetArg(XtNy, root_y);
		SetArg(XtNvalue, pn->pn_title);
		XtSetValues(titlewidget, args, nargs);
	}

	/*
	 * Pop up the dialog box.
	 */
	XtPopup(titlewidget, XtGrabExclusive);
}

/*
 * ClearTitle - get rid of the title box.
 */
void
ClearTitle()
{
	XtPopdown(titlewidget);
}

