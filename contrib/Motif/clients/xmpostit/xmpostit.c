/* based on xpostit.c
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94086
 * davy@riacs.edu
 *
 *
 * Copyright 1989, 1990 Genentech, Incorporated
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Genentech not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Genentech makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * GENENTECH DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL GENENTECH BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 * Author:  David Mischel - Genentech, Inc.
 */

#include <signal.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/cursorfont.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/DrawingA.h>
#include <Xm/ScrolledW.h>
#include <Xm/SelectioB.h>
#include <Xm/MwmUtil.h>

void	menuSelection();
void	setBackground();

#include "xmpostit.h"
#include "plaid.bm"

#define MENUCREATE_1P5x2	0
#define MENUCREATE_3x3		1
#define MENUCREATE_3x5		2
#define MENURAISEALL		3
#define MENULOWERALL		4
#define MENUSAVEALL		5
#define MENUPRINTER		6
#define MENUEXIT		7

void	PrinterNameCB();
void	PrinterNameCancelCB();

AppRes	app_res;		/* xpostit application resources	*/
XtAppContext appcontext;	/* application context			*/
Widget	toplevel;		/* top level application shell widget	*/
Screen	*screen;		/* pointer to the screen of the display	*/
Display	*display;		/* pointer to the display we're on	*/
Widget	menuwidget;
Widget		rcwidget;
char printername[MAXNAME +1];	/* device for "hardcopy" button */

/*
 * Command line options and the resources they set.
 */
static XrmOptionDescRec options[] = {
    { "-bs",	".bufSize",		XrmoptionSepArg,	NULL },
    { "-dir",	".noteDir",		XrmoptionSepArg,	NULL },
    { "-sv",	".saveNotes",		XrmoptionNoArg,		"true" }
};

/*
 * Fallback resources.
 */
static String fallback_resources[] = {
    "*Command*font: -adobe-helvetica-bold-r-normal--11-*-*-*-*-*-*-*",
    "*Menu*font:    -adobe-helvetica-medium-r-normal--12-*-*-*-*-*-*-*",
    "*Text*font:    -b&h-lucidatypewriter-medium-r-normal-*-12-*-*-*-*-*-*-*",
    NULL
};
/*
 * Resources we maintain besides those maintained by the toolkit.
 */
static XtResource resources[] = {
#define offset(field)   XtOffset(AppResPtr,field)
    { "bufSize", "BufSize", XtRInt, sizeof(int),
      offset(buf_size), XtRImmediate, (caddr_t) DefaultBufSize },
    { "noteDir", "NoteDir", XtRString, sizeof(String),
      offset(note_dir), XtRString, DefaultNoteDir },
    { "saveNotes", "SaveNotes", XtRBoolean, sizeof(Boolean),
      offset(save_notes), XtRImmediate, (caddr_t) False },
    { "showPlaid", "ShowPlaid", XtRBoolean, sizeof(Boolean),
      offset(show_plaid), XtRImmediate, (caddr_t) True },
    { "scrollOnOverflow", "Scroll", XtRBoolean, sizeof(Boolean),
      offset(scroll_ovf), XtRImmediate, (caddr_t) False }
#undef offset
};

void
main(argc, argv)
char **argv;
int argc;
{
	/*
	 * Initialize the toolkit and create an application shell.
	 */
	toplevel = XtAppInitialize(&appcontext, PostItNoteClass, options,
			XtNumber(options), &argc, argv, fallback_resources,
			NULL, 0);

	display = XtDisplay(toplevel);
	screen = DefaultScreenOfDisplay(display);

   	 /*
   	  * Now get any resources we're interested in.
  	   */
  	XtGetApplicationResources(toplevel, &app_res, resources,
        XtNumber(resources), argv, argc);

	SetNoteDir();
	CreatePlaidWidget();

	XtRealizeWidget(toplevel);
	if (app_res.show_plaid) setBackground();
	XFlush(display);

	XtAppMainLoop(appcontext);
}

static void	HandlePlaidButton();
static Widget scrollW, clipA;

void
CreatePlaidWidget()
{
	Arg args[11];
	register int nargs;
	Cursor menuC;

	strcpy(printername, (char *) getenv("PRINTER"));
	nargs = 0;
    SetArg (XmNscrollBarDisplayPolicy, XmAS_NEEDED);
    SetArg (XmNscrollingPolicy, XmAUTOMATIC);
    scrollW = XmCreateScrolledWindow (toplevel, "scrollW", args, nargs);
	XtManageChild(scrollW);

	nargs = 0;
	SetArg(XmNwidth, DefaultPlaidWidth);
	SetArg(XmNheight, DefaultPlaidHeight);
	SetArg(XmNpacking, XmPACK_COLUMN);
	SetArg(XmNadjustMargin, False);
	SetArg(XmNadjustLast, False);
	SetArg(XmNmarginHeight, 12);
	SetArg(XmNmarginWidth, 12);
	SetArg(XmNentryBorder, 6);
	rcwidget = XmCreateRowColumn(scrollW, "rcwidget", args, nargs);
	XtManageChild(rcwidget);
	menuC = XCreateFontCursor(display, XC_pencil);
	XmSetMenuCursor(display, menuC);

	CreateMenuWidget();

	nargs = 0;
	SetArg(XmNclipWindow, &clipA);
	XtGetValues(scrollW, args, nargs);
	XtAddEventHandler(clipA, ButtonPressMask, False,
		HandlePlaidButton, menuwidget);

	/*
	 * Load the notes the user has saved, and create widgets
	 * for them.
	 */
	LoadSavedNotes();
}

  void
setBackground()
{
	Arg args[11];
	register int nargs;
	int depth;
	Pixel fg, bg;
	Pixmap plaidmap;

	nargs = 0;
	SetArg(XmNforeground, &fg);
	SetArg(XmNbackground, &bg);
	SetArg(XmNdepth, &depth);
	XtGetValues(rcwidget, args, nargs);
	plaidmap = XCreatePixmapFromBitmapData(display, XtWindow(rcwidget),
		plaid_bits, plaid_width, plaid_height, fg, bg, depth);

	/* set the background for the clip area too */
	nargs = 0;
	SetArg(XmNbackgroundPixmap, plaidmap);
	XtSetValues(clipA, args, nargs);
	XtSetValues(rcwidget, args, nargs);
}

/*
 * HandlePlaidButton - even handler for plaid widget
 */
/* ARGSUSED */
  static void
HandlePlaidButton(w, popup, event)
	Widget w, popup;
	XButtonEvent *event;
{
	Arg args[4];
	register int nargs;
	register Position x, y;


	/*
	 * We're only interested in button presses.
	 */
	if (event->type != ButtonPress)
		return;

	/*
	 * Dispatch a function for this button.
	 */
	switch (event->button) {
	case Button3:
		/*
		 * Get position of mouse so we can put
		 * the menu there.
		 */
		x = event->x_root;
		y = event->y_root;

		/*
		 * Move the popup to that location.
		 */
		nargs = 0;
		SetArg(XtNx, x);
		SetArg(XtNy, y);
		XtSetValues(menuwidget, args, nargs);

		/*
		 * Bring up the menu.
		 */
		XmMenuPosition(popup, event);
		XtManageChild(popup);
		break;
	default:
		break;
	}
}

/*
 * CreateMenuWidget - create the widget used for the menu.
 */
void
CreateMenuWidget()
{
	Widget popupBtn[8];
	int popn;
	Arg args[4];
	register int nargs;

	nargs = 0;
	SetArg(XmNwhichButton, (XtArgVal) 3); 
	SetArg(XmNmenuCursor, "pencil");

	menuwidget = XmCreatePopupMenu(rcwidget, "plaidmenu", args, nargs);
	XtAddEventHandler(rcwidget, ButtonPressMask, False,
		HandlePlaidButton, menuwidget);

	popn = 0;
	popupBtn[popn] = XmCreatePushButtonGadget(menuwidget,
		"1.5x3 Note", NULL, 0);
	XtAddCallback(popupBtn[popn], XmNactivateCallback,
		menuSelection, MENUCREATE_1P5x2);
	popn++;

	popupBtn[popn] = XmCreatePushButtonGadget(menuwidget,
		"3x3 Note", NULL, 0);
	XtAddCallback(popupBtn[popn], XmNactivateCallback,
		menuSelection, MENUCREATE_3x3);
	popn++;

	popupBtn[popn] = XmCreatePushButtonGadget(menuwidget,
		"3x5 Note", NULL, 0);
	XtAddCallback(popupBtn[popn], XmNactivateCallback,
		menuSelection, MENUCREATE_3x5);
	popn++;

	popupBtn[popn] = XmCreatePushButtonGadget(menuwidget,
		"Raise Notes", NULL, 0);
	XtAddCallback(popupBtn[popn], XmNactivateCallback,
		menuSelection, MENURAISEALL);
	popn++;

	popupBtn[popn] = XmCreatePushButtonGadget(menuwidget,
		"Lower Notes", NULL, 0);
	XtAddCallback(popupBtn[popn], XmNactivateCallback,
		menuSelection, MENULOWERALL);
	popn++;

	popupBtn[popn] = XmCreatePushButtonGadget(menuwidget,
		"Save Notes", NULL, 0);
	XtAddCallback(popupBtn[popn], XmNactivateCallback,
		menuSelection, MENUSAVEALL);
	popn++;

	popupBtn[popn] = XmCreatePushButtonGadget(menuwidget, "Choose Printer", NULL, 0);
	XtAddCallback(popupBtn[popn], XmNactivateCallback,
		menuSelection, MENUPRINTER);
	popn++;

	popupBtn[popn] = XmCreatePushButtonGadget(menuwidget, "Exit", NULL, 0);
	XtAddCallback(popupBtn[popn], XmNactivateCallback,
		menuSelection, MENUEXIT);
	popn++;

	XtManageChildren(popupBtn, popn);
}

/*
 * menuSelection - callback from list widget to handle a selection.
 */
/* ARGSUSED */
  void
menuSelection(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	/*
	 * Dispatch the command.
	 */
	switch ((int) client_data) {
	case MENUCREATE_1P5x2:
		CreateNewNote(PostItNote_1p5x2);
		break;
	case MENUCREATE_3x3:
		CreateNewNote(PostItNote_3x3);
		break;
	case MENUCREATE_3x5:
		CreateNewNote(PostItNote_3x5);
		break;
	case MENURAISEALL:
		RaiseAllNotes();
		break;
	case MENULOWERALL:
		LowerAllNotes();
		break;
	case MENUSAVEALL:
		SaveAllNotes();
		break;
	case MENUPRINTER:
		ChoosePrinter();
		break;
	case MENUEXIT:
		ByeBye();

	}
}

  void
ChoosePrinter()
{
	Arg args[5];
	int nargs;
	Widget twidget;

	nargs = 0;
	SetArg(XmNdialogType, XmDIALOG_PROMPT);
	SetArg(XmNselectionLabelString,
		XmStringCreateLtoR("Name of Printer for Hardcopy",
		XmSTRING_DEFAULT_CHARSET));
	SetArg(XmNtextString, 
		XmStringCreateLtoR(printername, XmSTRING_DEFAULT_CHARSET));
	SetArg(XmNdialogStyle, XmDIALOG_APPLICATION_MODAL);
	SetArg(XmNtextColumns, MAXNAME);
	twidget = XmCreatePromptDialog(rcwidget, "Printer", args, nargs);
	XtAddCallback(twidget, XmNokCallback, PrinterNameCB, NULL);
	XtAddCallback(twidget, XmNcancelCallback, PrinterNameCancelCB, NULL);

    XtUnmanageChild
        (XmSelectionBoxGetChild (twidget, XmDIALOG_HELP_BUTTON));
	
	/* turn off mwm borders */
	nargs = 0;
	SetArg(XmNmwmDecorations, MWM_DECOR_ALL);
	XtSetValues(XtParent(twidget), args, nargs);
	/* limit input text size */
	nargs = 0;
	SetArg(XmNmaxLength, MAXNAME);
	XtSetValues(XmSelectionBoxGetChild(twidget, XmDIALOG_TEXT), args, nargs);

	XtRealizeWidget(twidget);
	XtManageChild(twidget);
}


/* ARGSUSED */
  void
PrinterNameCB(w, client_data, call_data)
	Widget w;
	caddr_t client_data, call_data;
{
	Widget textW;

	textW = XmSelectionBoxGetChild(w, XmDIALOG_TEXT);
	strcpy(printername, XmTextGetString(textW));
	XtDestroyWidget(XtParent(w));
}


/* ARGSUSED */
  void
PrinterNameCancelCB(w, client_data, call_data)
	Widget w;
	caddr_t client_data, call_data;
{
	XtDestroyWidget(XtParent(w));
}

