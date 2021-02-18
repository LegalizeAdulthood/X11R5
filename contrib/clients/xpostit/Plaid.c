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
static char	*RCSid = "$Header: /home/orchestra/davy/progs/xpostit/RCS/Plaid.c,v 1.4 91/09/06 18:29:03 davy Exp $";
#endif

/*
 * Plaid.c - code for the plaid widget.
 *
 * Based on the Template widget from the X11R4 distribution.
 *
 * David A. Curry
 * Purdue University
 * Engineering Computer Network
 * West Lafayette, IN 47907
 * davy@ecn.purdue.edu
 *
 * $Log:	Plaid.c,v $
 * Revision 1.4  91/09/06  18:29:03  davy
 * Added copyright/permission notice for submission to MIT R5 contrib.
 * 
 * Revision 1.3  91/09/06  17:12:41  davy
 * Added hide, show, and quit actions.
 * 
 * Revision 1.2  90/06/14  11:17:42  davy
 * Ported to X11 Release 4.  Added translations to invoke all actions from
 * the plaid widget, instead of passing the event back to a single callback.
 * 
 * Revision 1.1  90/06/13  09:48:39  davy
 * Initial revision
 * 
 */
#include <X11/copyright.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "PlaidP.h"

#define plaid_width	22
#define plaid_height	22

/*
 * The bits representing the plaid background.
 */
static char plaid_bits[] = {
	0x75, 0xfd, 0x3f, 0xaa, 0xfa, 0x3e, 0x75, 0xfd, 0x3f, 0xaa, 0xfa, 0x3e,
	0x75, 0xfd, 0x3f, 0xff, 0x57, 0x15, 0x75, 0xfd, 0x3f, 0xaa, 0xfa, 0x3e,
	0x75, 0xfd, 0x3f, 0xaa, 0xfa, 0x3e, 0x75, 0xfd, 0x3f, 0x20, 0xa8, 0x2b,
	0x20, 0x50, 0x15, 0x20, 0xa8, 0x2b, 0x20, 0x50, 0x15, 0x20, 0xa8, 0x2b,
	0xff, 0xff, 0x3f, 0x20, 0xa8, 0x2b, 0x20, 0x50, 0x15, 0x20, 0xa8, 0x2b,
	0x20, 0x50, 0x15, 0x20, 0xa8, 0x2b
};

/*
 * The resources specific to the plaid widget.
 */
static XtResource resources[] = {
#define offset(field)	XtOffset(PlaidWidget,plaid.field)
	{ XtNlowerCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
		offset(lower_callback), XtRCallback, NULL },
	{ XtNraiseCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
		offset(raise_callback), XtRCallback, NULL },
	{ XtNhideCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
		offset(hide_callback), XtRCallback, NULL },
	{ XtNshowCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
		offset(show_callback), XtRCallback, NULL },
	{ XtNquitCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
		offset(quit_callback), XtRCallback, NULL },
	{ XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
		offset(foreground), XtRString, "XtDefaultForeground" }
#undef offset
};

/*****************************************************************************
 * Plaid Widget Routines
 *****************************************************************************/

/*
 * RaiseAction - called when raise button is pressed.
 */
static void
RaiseAction(w, event, params, num_params)
Cardinal *num_params;	/* unused */
String *params;		/* unused */
XEvent *event;
Widget w;
{
	/*
	 * Just pass it off to the user's callback function.
	 */
	XtCallCallbacks(w, XtNraiseCallback, (caddr_t) event);
}

/*
 * LowerAction - called when lower button is pressed.
 */
static void
LowerAction(w, event, params, num_params)
Cardinal *num_params;	/* unused */
String *params;		/* unused */
XEvent *event;
Widget w;
{
	/*
	 * Just pass it off to the user's callback function.
	 */
	XtCallCallbacks(w, XtNlowerCallback, (caddr_t) event);
}

/*
 * HideAction - called when hide button is pressed.
 */
static void
HideAction(w, event, params, num_params)
Cardinal *num_params;	/* unused */
String *params;		/* unused */
XEvent *event;
Widget w;
{
	/*
	 * Just pass it off to the user's callback function.
	 */
	XtCallCallbacks(w, XtNhideCallback, (caddr_t) event);
}

/*
 * ShowAction - called when show button is pressed.
 */
static void
ShowAction(w, event, params, num_params)
Cardinal *num_params;	/* unused */
String *params;		/* unused */
XEvent *event;
Widget w;
{
	/*
	 * Just pass it off to the user's callback function.
	 */
	XtCallCallbacks(w, XtNshowCallback, (caddr_t) event);
}

/*
 * QuitAction - called when quit key is pressed.
 */
static void
QuitAction(w, event, params, num_params)
Cardinal *num_params;	/* unused */
String *params;		/* unused */
XEvent *event;
Widget w;
{
	/*
	 * Just pass it off to the user's callback function.
	 */
	XtCallCallbacks(w, XtNquitCallback, (caddr_t) event);
}

/*
 * RealizePlaid - realize the window by creating it, and by creating the
 *		  plaid background pixmap.
 */
static void
RealizePlaid(w, value_mask, attributes)
XSetWindowAttributes *attributes;
Mask *value_mask;
Widget w;
{
	Pixmap p;
	Window window;
	Display *display;
	PlaidWidget plaidw;

	plaidw = (PlaidWidget) w;

	/*
	 * Create the window.
	 */
	XtCreateWindow(w, (unsigned int) InputOutput,
		       (Visual *) CopyFromParent, *value_mask, attributes);

	display = XtDisplay(w);
	window = XtWindow(w);

	/*
	 * Create the plaid pixmap.
	 */
	p = XCreatePixmapFromBitmapData(display, window, plaid_bits,
					plaid_width, plaid_height,
					plaidw->plaid.foreground,
					w->core.background_pixel,
					w->core.depth);

	/*
	 * Tile the window.
	 */
	XSetWindowBackgroundPixmap(display, window, p);

	/*
	 * Now save the pixmap in the core widget.
	 */
	w->core.background_pixmap = p;
}

/*
 * DestroyPlaid - free up server resources when widget is destroyed.
 */
static void
DestroyPlaid(w)
Widget w;
{
	/*
	 * All we need to do is get rid of the pixmap we created.
	 */
	XFreePixmap(XtDisplay(w), w->core.background_pixmap);
}

/*****************************************************************************
 * Plaid Widget definitions
 *****************************************************************************/

/*
 * Action table.  We're only interested in button presses.
 */
static XtActionsRec actions[] =
{
	{ "raise",	RaiseAction },
	{ "lower",	LowerAction },
	{ "hide",	HideAction  },
	{ "show",	ShowAction  },
	{ "quit",	QuitAction  }
};

/*
 * Translation table.  We're only interested in button presses.
 */
static char translations[] =
	"~Ctrl <Btn1Down>:	raise()	\n\
	 ~Ctrl <Btn2Down>:	lower()	\n\
	 Ctrl <Btn1Down>:	show() \n\
	 Ctrl <Btn2Down>:	hide() \n\
	 <Btn3Down>:		XawPositionSimpleMenu(Menu) MenuPopup(Menu) \n\
	 <Key> Q:		quit() \n\
	 <Key> q:		quit() \n\
";

/*
 * The plaid widget class record.   Initialization of values.
 */
PlaidClassRec plaidClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"Plaid",
    /* widget_size		*/	sizeof(PlaidRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	NULL,
    /* initialize_hook		*/	NULL,
    /* realize			*/	RealizePlaid,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	DestroyPlaid,
    /* resize			*/	NULL,
    /* expose			*/	NULL,
    /* set_values		*/	NULL,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	translations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* plaid fields */
    /* empty			*/	0
  }
};

/*
 * The class declaration.
 */
WidgetClass plaidWidgetClass = (WidgetClass)&plaidClassRec;
