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
static char	*RCSid = "$Header: /home/orchestra/davy/progs/xpostit/RCS/menu.c,v 1.4 91/09/06 18:29:19 davy Exp $";
#endif

/*
 * menu.c - routines to handle the menu.
 *
 * David A. Curry
 * Purdue University
 * Engineering Computer Network
 * West Lafayette, IN 47907
 * davy@ecn.purdue.edu
 *
 * $Log:	menu.c,v $
 * Revision 1.4  91/09/06  18:29:19  davy
 * Added copyright/permission notice for submission to MIT R5 contrib.
 * 
 * Revision 1.3  91/09/06  17:13:09  davy
 * Added menu items for hide and show.
 * 
 * Revision 1.2  90/06/14  11:19:39  davy
 * Ported to X11 Release 4.  Changed to use the SimpleMenu widget instead
 * of a List widget for the menu.
 * 
 * Revision 1.1  90/06/13  09:48:47  davy
 * Initial revision
 * 
 */
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Shell.h>
#include <stdio.h>

#include "xpostit.h"

static String menustrings[] = {
#define MenuCreate_1p5x2	0
	"Create 1.5x2 Note",
#define MenuCreate_2x3		1
	"Create 2x3 Note",
#define MenuCreate_3x3		2
	"Create 3x3 Note",
#define MenuCreate_3x4		3
	"Create 3x4 Note",
#define MenuCreate_3x5		4
	"Create 3x5 Note",
#define MenuCreate_4x6		5
	"Create 4x6 Note",
#define MenuRaiseAll		6
	"Raise All Notes",
#define MenuLowerAll		7
	"Lower All Notes",
#define MenuHideAll		8
	"Hide All Notes",
#define MenuShowAll		9
	"Show All Notes",
#define MenuSaveAll		10
	"Save All Notes",
#define MenuExit		11
	"Exit",
#define MenuLastEntry		12
	0,
};

Widget			menuwidget;
extern XtAppContext	appcontext;
static void		HandleMenuSelection();

/*
 * CreateMenuWidget - create the widget used for the menu.
 */
void
CreateMenuWidget()
{
	Arg args[8];
	Widget entry;
	register int i, nargs;
	XtCallbackRec callbacks[2];

	/*
	 * Set the callback.
	 */
	bzero(callbacks, sizeof(callbacks));
	SetCallback(HandleMenuSelection, NULL);

	/*
	 * Create the menu widget.
	 */
	nargs = 0;
	SetArg(XtNmenuOnScreen, True);
	SetArg(XtNlabel, PostItNoteClass);

	/*
	 * The menu is done with a popup shell.
	 */
	menuwidget = XtCreatePopupShell("Menu", simpleMenuWidgetClass,
					toplevel, args, nargs);

	for (i=0; menustrings[i] != NULL; i++) {
		entry = XtCreateManagedWidget(menustrings[i],
					      smeBSBObjectClass, menuwidget,
					      NULL, 0);

		XtAddCallback(entry, XtNcallback, HandleMenuSelection, i);
	}
		
	XawSimpleMenuAddGlobalActions(appcontext);

	/*
	 * Let the top level shell know we're here.
	 */
	XtRealizeWidget(menuwidget);
}

/*
 * HandleMenuSelection - callback from menu widget to handle a selection.
 */
static void
HandleMenuSelection(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	/*
	 * Dispatch the command.
	 */
	switch ((int) client_data) {
	case MenuCreate_1p5x2:
		CreateNewNote(PostItNote_1p5x2);
		break;
	case MenuCreate_2x3:
		CreateNewNote(PostItNote_2x3);
		break;
	case MenuCreate_3x3:
		CreateNewNote(PostItNote_3x3);
		break;
	case MenuCreate_3x4:
		CreateNewNote(PostItNote_3x4);
		break;
	case MenuCreate_3x5:
		CreateNewNote(PostItNote_3x5);
		break;
	case MenuCreate_4x6:
		CreateNewNote(PostItNote_4x6);
		break;
	case MenuRaiseAll:
		RaiseAllNotes();
		break;
	case MenuLowerAll:
		LowerAllNotes();
		break;
	case MenuHideAll:
		HideAllNotes();
		break;
	case MenuShowAll:
		ShowAllNotes();
		break;
	case MenuSaveAll:
		SaveAllNotes();
		break;
	case MenuExit:
		ByeBye();
		break;
	}
}
