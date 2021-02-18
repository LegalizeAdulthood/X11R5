/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
 *
 * "Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty."
 *
 */

#include "fig.h"
#include "resources.h"
#include "w_drawprim.h"
#include "w_util.h"
#include "w_setup.h"

/*
 * The next routine is easy to implement, but I haven't missed it yet.
 * Generally it is a bad idea to warp the mouse without the users consent.
 */

win_setmouseposition(w, x, y)
    Window          w;
    int             x, y;
{
}

/* popup a confirmation window */

static          query_result, query_done;

static void
query_confirmed(w, ev)
    Widget          w;
    XButtonEvent   *ev;
{
    query_done = 1;
    query_result = 1;
}

static void
query_cancelled(w, ev)
    Widget          w;
    XButtonEvent   *ev;
{
    query_done = 1;
    query_result = 0;
}

int
win_confirm(w, message)
    Window          w;
    char           *message;
{
    TOOL            query_popup, query_form, query_message;
    TOOL            query_confirm, query_cancel;
    int             xposn, yposn;
    Window          win;
    XEvent          event;

    DeclareArgs(7);

    XTranslateCoordinates(tool_d, w, XDefaultRootWindow(tool_d),
			  150, 200, &xposn, &yposn, &win);
    FirstArg(XtNallowShellResize, True);
    NextArg(XtNx, xposn);
    NextArg(XtNy, yposn);
    NextArg(XtNborderWidth, POPUP_BW);
    query_popup = XtCreatePopupShell("confirm", transientShellWidgetClass,
				     tool, Args, ArgCount);

    FirstArg(XtNdefaultDistance, 10);
    query_form = XtCreateManagedWidget("query_form", formWidgetClass,
				       query_popup, Args, ArgCount);

    FirstArg(XtNfont, bold_font);
    NextArg(XtNborderWidth, 0);
    query_message = XtCreateManagedWidget(message, labelWidgetClass,
					  query_form, Args, ArgCount);

    FirstArg(XtNlabel, "Confirm");
    NextArg(XtNheight, 25);
    NextArg(XtNvertDistance, 15);
    NextArg(XtNhorizDistance, 55);
    NextArg(XtNfromVert, query_message);
    NextArg(XtNborderWidth, INTERNAL_BW);
    query_confirm = XtCreateManagedWidget("confirm", commandWidgetClass,
					  query_form, Args, ArgCount);
    XtAddEventHandler(query_confirm, ButtonReleaseMask, (Boolean) 0,
		      query_confirmed, (XtPointer) NULL);

    FirstArg(XtNlabel, "Cancel");
    NextArg(XtNheight, 25);
    NextArg(XtNvertDistance, 15);
    NextArg(XtNfromVert, query_message);
    NextArg(XtNfromHoriz, query_confirm);
    NextArg(XtNhorizDistance, 25);
    NextArg(XtNborderWidth, INTERNAL_BW);
    query_cancel = XtCreateManagedWidget("cancel", commandWidgetClass,
					 query_form, Args, ArgCount);
    XtAddEventHandler(query_cancel, ButtonReleaseMask, (Boolean) 0,
		      query_cancelled, (XtPointer) NULL);

    XtPopup(query_popup, XtGrabExclusive);
    XDefineCursor(tool_d, XtWindow(query_popup), (Cursor) arrow_cursor.bitmap);

    query_done = 0;
    while (!query_done) {
	/* pass events */
	XNextEvent(tool_d, &event);
	XtDispatchEvent(&event);
    }

    XtPopdown(query_popup);
    XtDestroyWidget(query_popup);

    return (query_result);
}

static void
CvtStringToFloat(args, num_args, fromVal, toVal)
    XrmValuePtr     args;
    Cardinal       *num_args;
    XrmValuePtr     fromVal;
    XrmValuePtr     toVal;
{
    static float    f;

    if (*num_args != 0)
	XtWarning("String to Float conversion needs no extra arguments");
    if (sscanf((char *) fromVal->addr, "%f", &f) == 1) {
	(*toVal).size = sizeof(float);
	(*toVal).addr = (caddr_t) & f;
    } else
	XtStringConversionWarning((char *) fromVal->addr, "Float");
}

static void
CvtIntToFloat(args, num_args, fromVal, toVal)
    XrmValuePtr     args;
    Cardinal       *num_args;
    XrmValuePtr     fromVal;
    XrmValuePtr     toVal;
{
    static float    f;

    if (*num_args != 0)
	XtWarning("Int to Float conversion needs no extra arguments");
    f = *(int *) fromVal->addr;
    (*toVal).size = sizeof(float);
    (*toVal).addr = (caddr_t) & f;
}

fix_converters()
{
    XtAppAddConverter(tool_app, "String", "Float", CvtStringToFloat, NULL, 0);
    XtAppAddConverter(tool_app, "Int", "Float", CvtIntToFloat, NULL, 0);
}
