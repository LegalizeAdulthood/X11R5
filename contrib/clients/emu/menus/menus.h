#ifndef lint
/* $Header: /usr3/Src/emu/menus/RCS/menus.h,v 1.10 91/09/25 11:47:05 tom Exp $ */
#endif

/*
 * This file is part of the PCS emu program.
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
 * Menu header file for the emu client.
 *
 * Author: Thomas Bagli
 * Date: June 12th, 1990.
 * Description:
 *
 * Revision History:
 *
 * $Log:	menus.h,v $
 * Revision 1.10  91/09/25  11:47:05  tom
 * Fixed bogus "String class" in XtGetSubresources calls.  Surprisingly
 * this was only noticed when compiled with R5.
 * 
 * Revision 1.9  91/09/24  16:22:53  tom
 * menu_item_mark becomes a resource
 * 
 * Revision 1.8  91/03/05  16:43:27  jkh
 * Memory tracing added.
 * 
 * Revision 1.7  90/11/20  17:43:42  tom
 * Alpha.
 * 
 * Revision 1.6  90/10/09  14:53:09  tom
 * Yo-k.
 * 
 * Revision 1.5  90/07/26  02:24:17  jkh
 * Added new copyright.
 * 
 * Revision 1.4  90/07/18  19:05:43  tom
 * Miscellaneous little tweaks.
 * 
 * Revision 1.3  90/07/17  14:58:31  tom
 * Reduction in number of self-defined StringDefs, data structures.
 * 
 * Revision 1.2  90/07/10  11:59:46  tom
 * Consolidated all menu defines, declarations, etc. from various places
 * (menus.c).
 * 
 * Revision 1.1  90/06/28  15:59:58  tom
 * Initial revision
 * 
 *
 */


#include "common.h"

#ifdef MOTIF
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/CascadeB.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>

#else /* ATHENA */
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#endif /* MOTIF */


#ifdef MOTIF_ONE_ZERO
/* For Motif 1.0 Xt, from MIT Xt/Intrinsic.h */
typedef char* XtPointer;
#endif /* MOTIF_ONE_ZERO */


/* Action Procedures */

Export void XpEmuPopupMenu();

static XtActionsRec actionsList[] = {
    { "XpEmuPopupMenu",	XpEmuPopupMenu },
};


#ifdef TEST
Local void DoMenuDispatch ();
#else /* !TEST */
Import void DoMenuDispatch ();
#endif /* TEST */


/* Private Routines */
Local Widget DoCreateMenu ();
Local char *gettoken ();

/* menu items Callback Function */
Local void DoInteract ();


typedef struct _MenuResources {
    Cardinal number_items;
    String item_lines;
} MenuResourcesRec, *MenuResources;

typedef struct _MenubarResources {
    int number_columns, number_menus;
} MenubarResourcesRec, *MenubarResources;

#ifdef MOTIF
#else /* ATHENA */
/* menu item checkmark pixmap */
typedef struct _MenuItemResources {
    Pixmap menu_item_mark;
} MenuItemResourcesRec, *MenuItemResources;
#endif /* MOTIF */

typedef struct _ItemResources {
    String action;
} ItemResourcesRec, *ItemResources;

#define XpNnumberItems "numberItems"
#define XpCNumberItems "NumberItems"

#define XpNitemLines "itemLines"
#define XpCItemLines "ItemLines"

#define XpNnumberColumns "numberColumns"
#define XpCNumberColumns "NumberColumns"

#define XpNnumberMenus "numberMenus"
#define XpCNumberMenus "NumberMenus"

#ifdef MOTIF
#else /* ATHENA */
#define XpNmenuItemMark "menuItemMark"
#define XpCMenuItemMark "menuItemMark"
#endif /* MOTIF */

#define XpNaction "action"
#define XpCAction "Action"


#ifdef MOTIF
#define BUTTON_WIDGET_CLASS	xmCascadeButtonWidgetClass
#define CALLBACK_REC		XmNactivateCallback
#define ITEM_WIDGET_CLASS	xmPushButtonGadgetClass
#define LINE_WIDGET_CLASS	xmSeparatorGadgetClass
#define MENUBAR_CLASS_STR	"XmRowColumn"
#define MENU_CLASS_STR		"XmRowColumn"
#define MENUENTRY_CLASS_STR	"XmPushButtonGadget"

#else /* ATHENA */
#define BUTTON_WIDGET_CLASS	menuButtonWidgetClass
#define CALLBACK_REC		XtNcallback
#define ITEM_WIDGET_CLASS	smeBSBObjectClass
#define LINE_WIDGET_CLASS	smeLineObjectClass
#define MENUBAR_CLASS_STR	"Form"
#define MENU_CLASS_STR		"SimpleMenu"
#define MENUENTRY_CLASS_STR	"SmeBSB"
#endif /* MOTIF */
