#ifndef lint
     static char *rcsid = "$Header: /usr3/Src/emu/menus/RCS/menus.c,v 1.24 91/09/25 11:45:18 tom Exp $";
#endif

/*
 * This file is part of the PCS emu system.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * Menu code for the emu client.
 *
 * Author: Thomas Bagli
 * Date: June 28th, 1990
 * Description: This file contains  code implementing menus for
 *		the emu client. Either Athena or Motif menus may
 *		be chosen.
 *
 * Revision History:
 *
 * $Log:	menus.c,v $
 * Revision 1.24  91/09/25  11:45:18  tom
 * Fixed bogus "String class" in XtGetSubresources calls.  Surprisingly
 * this was only noticed when compiled with R5.
 * 
 * Revision 1.23  91/09/24  16:23:57  tom
 * menu_item_mark becomes a resource
 * 
 * Revision 1.22  91/03/05  16:43:05  jkh
 * Memory tracing added.
 * 
 * Revision 1.21  90/12/20  17:11:18  jkh
 * Fixed bogus NULL check for string in get_token().
 * 
 * Revision 1.20  90/11/20  17:43:35  tom
 * Alpha.
 * 
 * Revision 1.19  90/10/22  09:37:58  tom
 * Fixing up menus for Motif 1.1
 *
 */

#include "menus.h"

/*
 *    XpEmuInitializeMenus :
 *    public menu initialization function.
 */

Export Widget
XpEmuInitializeMenus (parent, menubar, menubar_height)
    Widget parent;
    Boolean menubar;
    Dimension *menubar_height;
{
#ifdef MOTIF
#else /* ATHENA */
     WidgetList children;
#endif /* MOTIF */
    Arg args[8];
    Cardinal i, nargs, number_rows;
    char menu_button_name[16];
    Widget menuBar, menuButton;

#ifdef MOTIF
    Dimension shadowThickness, borderWidth, height;
#else /* ATHENA */
    Dimension fBorderWidth, mBorderWidth, mHeight;
    int defaultDistance;
#endif /* MOTIF */

    static XtResource menubarResourcesList[] = {
	{ XpNnumberColumns, XpCNumberColumns, XtRInt, sizeof (int),
	    XtOffset (struct _MenubarResources *, number_columns),
	    XtRString, (XtPointer)"0",
	 },
	 { XpNnumberMenus, XpCNumberMenus, XtRInt, sizeof (int),
	    XtOffset (struct _MenubarResources *, number_menus),
	    XtRString, (XtPointer)"1",
	 },
    };
    static MenubarResourcesRec menubarResources;
     
    XtAddActions (actionsList, XtNumber (actionsList));
#ifdef MOTIF
#else /* ATHENA */
    XtRegisterGrabAction (XpEmuPopupMenu, True,
	(ButtonPressMask | ButtonReleaseMask),
	GrabModeAsync, GrabModeAsync);
#endif /* MOTIF */
     
    if (!menubar)
	return NULL;
    /* else continue to menubar initialization. */
     
    nargs = 0;
    XtSetArg (args[nargs], XtNborderWidth, 1);		nargs++;
#ifdef MOTIF
    XtSetArg (args[nargs], XmNpacking, XmPACK_COLUMN);	nargs++;
    menuBar = XmCreateMenuBar (parent, "menuBar", args, nargs);
    XtManageChild (menuBar);

#else /* ATHENA */
    menuBar = XtCreateManagedWidget ("menuBar", formWidgetClass,
	parent, args, nargs);
#endif /* MOTIF */

    debug ("XpEmuInitializeMenus: w=0x%x, XtName=%s",
	 menuBar, XtName (menuBar));

    XtGetSubresources (menuBar, &menubarResources,
	XtName (menuBar), MENUBAR_CLASS_STR, menubarResourcesList,
	XtNumber (menubarResourcesList), (ArgList)NULL, (Cardinal)0);

    if (menubarResources.number_menus < 1)
	menubarResources.number_menus = 1;
    if (menubarResources.number_columns < 1)
	menubarResources.number_columns = menubarResources.number_menus;

    number_rows =
	(menubarResources.number_menus + 1) / menubarResources.number_columns;

#ifdef MOTIF
    /*
     *    Motif MenuBar is a horizontally-oriented RowColumn widget,
     *    so XmNnumColumns indicates how many rows are to be built.
     */
    if (menubarResources.number_columns < menubarResources.number_menus) {
	nargs = 0;
	XtSetArg (args[nargs], XmNnumColumns, number_rows); nargs++;
	XtSetValues (menuBar, args, nargs);
    }
#endif /* MOTIF */

    for (i = 1; i < menubarResources.number_menus + 1; i++) {
	sprintf (menu_button_name, "menuButton%d", i);
#ifdef MOTIF
	menuButton = XtCreateManagedWidget (menu_button_name,
	    BUTTON_WIDGET_CLASS, menuBar, (ArgList)NULL, (Cardinal)0);

#else /* ATHENA */
	nargs = 0;
	XtSetArg (args[nargs], XtNchildren, &children); nargs++;
	XtGetValues (menuBar, args, nargs);

	nargs = 0;
	if (i > menubarResources.number_columns) {
	    XtSetArg (args[nargs], XtNfromVert,
		children[i - menubarResources.number_columns - 1]);
	    nargs++;
	}
	if ((i - 1) % menubarResources.number_columns) {
	    XtSetArg (args[nargs], XtNfromHoriz, children[i - 2]);
	    nargs++;
	}
	menuButton = XtCreateManagedWidget (menu_button_name,
	    BUTTON_WIDGET_CLASS, menuBar, args, nargs);
#endif /* MOTIF */

	debug ("XpEmuInitializeMenus: w=0x%x, XtName=%s",
	    menuButton, XtName (menuButton));
    }

    /* Calculate the total height of the menuBar. */
#ifdef MOTIF
    nargs = 0;
    XtSetArg (args[nargs], XmNshadowThickness, &shadowThickness); nargs++;
    XtSetArg (args[nargs], XmNborderWidth, &borderWidth); nargs++;
    XtSetArg (args[nargs], XmNheight, &height); nargs++;
    XtGetValues (menuBar, args, nargs);

    *menubar_height = number_rows * ((2 * shadowThickness)
	+ (1 * borderWidth) + height);

#else /* ATHENA */
    nargs = 0;
    XtSetArg (args[nargs], XtNborderWidth, &fBorderWidth); nargs++;
    XtSetArg (args[nargs], XtNdefaultDistance, &defaultDistance); nargs++;
    XtGetValues (menuBar, args, nargs);

    nargs = 0;
    XtSetArg (args[nargs], XtNborderWidth, &mBorderWidth); nargs++;
    XtSetArg (args[nargs], XtNheight, &mHeight); nargs++;
    XtGetValues (menuButton, args, nargs);

    *menubar_height = number_rows * ((1 * fBorderWidth)
	+ (2 * defaultDistance) + (2 * mBorderWidth) + mHeight);
#endif /* MOTIF */

    return menuBar;
}


/*
 *    XpEmuPopupMenu :
 *    Action Routine expects the first parameter to be a menu name.
 *    Additional parameters, if any, are interpreted as menu action
 *    ROP numbers, which are defined in the resource file.
 */

Export void
XpEmuPopupMenu (w, event, params, param_count)
    Widget w;
    XEvent *event;              /* unused */
    String *params;             /* menu name and ROPs */
    Cardinal *param_count;      /* > 0 */
{
    Local XtResource menuResourcesList[] = {
	{ XpNnumberItems, XpCNumberItems, XtRInt, sizeof (int),
	    XtOffset (struct _MenuResources *, number_items),
	    XtRString, (XtPointer)NULL,
	},
	{ XpNitemLines, XpCItemLines, XtRString, sizeof (String),
	    XtOffset (struct _MenuResources *, item_lines),
	    XtRString, (XtPointer)NULL,
	},
    };
    Local MenuResourcesRec menuResources;
    char menu_path[64];
    Cardinal pc;
    Widget menu_id, reference = XtParent (w);

    if (*param_count == 0) {
	warn ("XpEmuPopupMenu name not properly specified.\n");
	return;
    }
     
    debug ("XpEmuPopupMenu: w=0x%x, XtName=%s, menu_name=%s",
	 w, XtName (w), params[0]);
/*
    while (!XtIsComposite (reference))
	reference = XtParent (reference);

#ifdef MOTIF
    reference = XtParent (XtParent (XtParent (XtParent (w)));
#else /* ATHENA
    reference = XtParent (XtParent (XtParent (w)));
#endif /* MOTIF */

    sprintf (menu_path, "*%s", params[0]);
/*
    debug ("XpEmuPopupMenu: reference=0x%x, XtName=%s, menu_path=%s",
	 reference, XtName (reference), menu_path);

    if ((menu_id = XtNameToWidget (reference, menu_path)) == NULL) {
*/
    if ((menu_id = XtNameToWidget (w, menu_path)) == NULL) {
	XtGetSubresources (w, &menuResources, params[0], MENU_CLASS_STR,
	    menuResourcesList, XtNumber (menuResourcesList),
	    (ArgList)NULL, (Cardinal)0);
	menu_id = DoCreateMenu (w, params[0], menuResources.number_items,
	    menuResources.item_lines);
    }

    /* Execute any specified ROPs. */
    for (pc = 1; pc < *param_count; pc++)
	DoMenuDispatch (menu_id, params[pc]);

#ifdef MOTIF
    XmMenuPosition (menu_id, (XButtonPressedEvent *)event);
    XtManageChild (menu_id);
#else /* ATHENA */
    XtCallActionProc (w, "XawPositionSimpleMenu", event, params, 1);
    XtCallActionProc (w, "MenuPopup", event, params, 1);
#endif /* MOTIF */
}


/*
 *    XpEmuMenuAction :
 *    Routine for IOP calls.  Sets / resets menu item sensitivity
 *    and menu item toggle marks.
 */

Export void
XpEmuMenuAction (w, name, action)
    Widget w;
    String name;
    int action;
{
    Arg args[1];
    Cardinal nargs;
    Widget menu_item_id;

#ifdef MOTIF
#else /* ATHENA */
    static XtResource menuItemResourcesList[] = {
	{ XpNmenuItemMark, XpCMenuItemMark, XtRBitmap, sizeof (Pixmap),
	    XtOffset (struct _MenuItemResources *, menu_item_mark),
	    XtRString, (XtPointer)NULL,
	},
    };
    static MenuItemResourcesRec menuItemResources;
#endif /* MOTIF */

    debug ("XpEmuMenuAction: w=0x%x, XtName=%s, name=%s, action=%d",
	 w, XtName (w), name, action);

    if ((menu_item_id = XtNameToWidget (w, name)) == NULL) {
	warn ("Unknown menu and/or item name : %s in %s.", name, XtName (w));
	return;
    }

#ifdef MOTIF
#else /* ATHENA */
    XtGetSubresources (menu_item_id, &menuItemResources,
	XtName (menu_item_id), MENUENTRY_CLASS_STR, menuItemResourcesList,
	XtNumber (menuItemResourcesList), (ArgList)NULL, (Cardinal)0);
#endif /* MOTIF */

    nargs = 0;
    switch (action) {
    case MENU_ITEM_ACTIVATE:
	XtSetArg (args[nargs], XtNsensitive, True); nargs++;
	break;

    case MENU_ITEM_DEACTIVATE:
	XtSetArg (args[nargs], XtNsensitive, False); nargs++;
	break;

    case MENU_ITEM_MARK:
#ifdef MOTIF
#else /* ATHENA */
	XtSetArg (args[nargs], XtNleftBitmap,
	    menuItemResources.menu_item_mark);
	nargs++;
#endif /* MOTIF */
	break;

    case MENU_ITEM_UNMARK:
#ifdef MOTIF
#else /* ATHENA */
	XtSetArg (args[nargs], XtNleftBitmap, None); nargs++;
#endif /* MOTIF */
	break;

    default:
	warn ("Unknown menu action %d for menu item %s.", action, name);
	return;
    }
    XtSetValues (menu_item_id, args, nargs);
}


#ifdef TEST
/*
 *    DoMenuDispatch
 *    emu client program function; it's a dummy for test purposes.
 */

Export void
DoMenuDispatch (w, str)
    Widget w;
    String str;
{
    debug ("DoMenuDispatch: w=0x%x, XtName=%s, action=%s",
	  w, XtName (w), str);
}
#endif /* TEST */


/*
 *    DoCreateMenu
 *    DoInteract :
 *    Private Menu Routines
 */

Local Widget
DoCreateMenu (w, menu_name, number_items, item_lines)
    Widget w;
    String menu_name;
    Cardinal number_items;
    String item_lines;
{
    Cardinal i;
    Widget menu, item_widget;
     
    static XtCallbackRec cb[2] = {
	{ (XtCallbackProc)DoInteract, (XtPointer)NULL },
	{ (XtCallbackProc)NULL, (XtPointer)NULL }
    };
    static Arg args[] = { { CALLBACK_REC, (XtArgVal)cb } };
    Import char *get_token();
 
    String lines = XtNewString (item_lines);
    char *line = get_token (lines);

    debug ("DoCreateMenu: w=0x%x, XtName=%s, menu_name=%s\n\
	number_items=%d\n        item_lines=%s",
	w, XtName (w), menu_name, number_items, item_lines);
     
#ifdef MOTIF
    menu = XmCreatePopupMenu (w, menu_name, (ArgList)NULL, (Cardinal)0);

    item_widget = XtCreateManagedWidget (menu_name,
	xmLabelGadgetClass, menu, (ArgList)NULL, (Cardinal)0);
    item_widget = XtCreateManagedWidget ("titleSeparator",
	xmSeparatorGadgetClass, menu, (ArgList)NULL, (Cardinal)0);

#else /* ATHENA */
    menu = XtCreatePopupShell (menu_name, simpleMenuWidgetClass,
	w, (ArgList)NULL, (Cardinal)0);
#endif /* MOTIF */
     
    for (i = 1; i <= number_items; i++) {
	char item_name[10];

	sprintf (item_name, "item%d", i);
	item_widget = XtCreateManagedWidget (item_name, 
	    ITEM_WIDGET_CLASS, menu, args, (Cardinal)1);

	debug ("DoCreateMenu: item_widget=0x%x, item_name=%s",
	    item_widget, item_name);

	if (line != NULL && ((strcmp (item_name, line)) == 0)) {
	    item_widget = XtCreateManagedWidget ("line", 
		LINE_WIDGET_CLASS, menu, (ArgList)NULL,
		(Cardinal)0);
	    line = get_token ((char *)NULL);
	}
    }
    XtFree (lines);
    return menu;
}

Local void
DoInteract (w, closure, data)
    Widget w;
    XtPointer closure, data;    /* not used */
{
    Arg args[2];
    Cardinal nargs;
    String action = NULL;

    static XtResource itemResourcesList[] = {
	{ XpNaction, XpCAction, XtRString, sizeof (String),
	    XtOffset (struct _ItemResources *, action),
	    XtRString, (XtPointer)NULL,
	},
    };
    static ItemResourcesRec itemResources;

    XtGetSubresources (XtParent (w), &itemResources, XtName (w),
	MENUENTRY_CLASS_STR, itemResourcesList, XtNumber (itemResourcesList),
	(ArgList)NULL, (Cardinal)0);

    /*  Use the menu_item label if no "action" resource exists. */
    if ((action = itemResources.action) == NULL) {
	nargs = 0;
	XtSetArg (args[nargs], XtNlabel, &action);	nargs++;
	XtGetValues (w, args, nargs);
    }
    DoMenuDispatch (w, action);
}

/*    get_token:
 *    Private routine that works like strtok.  The first call to this
 *    function should have the string to be dismantled as its parameter.
 *    All subsequent calls should have "NULL" as the parameter.  In both
 *    cases it returns either a null character terminated token or "NULL"
 *    if no token is found.
 */
char *
get_token (string)
char *string;
{
    register char *str, *end;
    static char *savestr;

    if (string == NULL)
	string = savestr;

    if (string == NULL)
	return (NULL);

    /* Find start of "item". */
    str = index (string, 'i');
    if (str == NULL || *str == '\0')
	return (NULL);

    /* Find end of "item[0-9]". */
    end = str + 5;
    /* Check for trailing "[0-9]"'s. */
    for (; isdigit (*end); end++)
	;
    *end = '\0';

    savestr = end + 1;
    if (*savestr == '\0' || savestr == NULL)
	savestr = NULL;

    return (str);
}
