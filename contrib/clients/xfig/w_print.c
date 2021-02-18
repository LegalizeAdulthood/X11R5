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

/* IMPORTS */

#include "fig.h"
#include "mode.h"
#include "resources.h"
#include "w_icons.h"
#include "w_setup.h"
#include "w_util.h"

extern String   text_translations;
extern Widget   make_popup_menu();

/* LOCAL */

static char    *orient_items[] = {
    "portrait ",
"landscape"};

static char    *just_items[] = {
    "flush left",
"centered  "};

static void     orient_select();
static Widget   orient_panel, orient_menu, orient_lab;

static void     just_select();
static Widget   just_panel, just_menu, just_lab;

static Widget   print_panel, print_popup, cancel, print, printer_text,
                printer_lab, mag_lab, print_w, mag_text;
static int      print_centered = 0;
static Position xposn, yposn;

static void
print_panel_dismiss()
{
    XtPopdown(print_popup);
    XtSetSensitive(print_w, True);
}

static void
print_panel_cancel(w, ev)
    Widget          w;
    XButtonEvent   *ev;
{
    print_panel_dismiss();
}

static char     print_msg[] = "PRINT";

do_print(w)
    Widget          w;
{
    DeclareArgs(1);
    float           mag;
    char           *pval;

    if (emptyfigure_msg(print_msg))
        return;

    if (print_popup) {
	mag = (float) atof(panel_get_value(mag_text)) / 100.0;
	if (mag <= 0.0)
	    mag = 1.0;

	FirstArg(XtNstring, &pval);
	GetValues(printer_text);
	print_to_printer(pval, print_centered, mag);
	print_panel_dismiss();
    } else {
	print_to_printer("", 0, 1.0);
    }
}

static void
orient_select(w, new_orient, garbage)
    Widget          w;
    XtPointer       new_orient, garbage;
{
    DeclareArgs(1);

    FirstArg(XtNlabel, XtName(w));
    SetValues(orient_panel);
    print_landscape = (int) new_orient;
}

static void
just_select(w, new_just, garbage)
    Widget          w;
    XtPointer       new_just, garbage;
{
    DeclareArgs(1);

    FirstArg(XtNlabel, XtName(w));
    SetValues(just_panel);
    print_centered = (int) new_just;
}

popup_print_panel(w)
    Widget          w;
{
    Widget          image;
    XtTranslations  popdown_actions;
    Pixmap          p;

    DeclareArgs(10);

    if (!print_popup) {
	print_w = w;
	XtTranslateCoords(w, (Position) 0, (Position) 0, &xposn, &yposn);

	FirstArg(XtNx, xposn);
	NextArg(XtNy, yposn + 50);
	print_popup = XtCreatePopupShell("xfig: print menu",
					 transientShellWidgetClass,
					 tool, Args, ArgCount);

	print_panel = XtCreateManagedWidget("print_panel", formWidgetClass,
					    print_popup, NULL, ZERO);
	popdown_actions = XtParseTranslationTable(
						  "<Btn1Up>:\n");
	XtOverrideTranslations(print_panel, popdown_actions);

	p = XCreateBitmapFromData(tool_d, XtWindow(canvas_sw),
		      printer_ic.data, printer_ic.width, printer_ic.height);

	FirstArg(XtNlabel, "   ");
	NextArg(XtNwidth, printer_ic.width);
	NextArg(XtNheight, printer_ic.height);
	NextArg(XtNbitmap, p);
	NextArg(XtNborderWidth, 0);
	NextArg(XtNinternalWidth, 0);
	NextArg(XtNinternalHeight, 0);
	NextArg(XtNresize, False);
	NextArg(XtNresizable, False);
	image = XtCreateManagedWidget("printer image", labelWidgetClass,
				      print_panel, Args, ArgCount);

	FirstArg(XtNlabel, "  Magnification%:");
	NextArg(XtNfromVert, image);
	NextArg(XtNjustify, XtJustifyLeft);
	NextArg(XtNborderWidth, 0);
	mag_lab = XtCreateManagedWidget("mag label", labelWidgetClass,
					print_panel, Args, ArgCount);

	FirstArg(XtNwidth, 40);
	NextArg(XtNfromVert, image);
	NextArg(XtNfromHoriz, mag_lab);
	NextArg(XtNeditType, "edit");
	NextArg(XtNstring, "100");
	NextArg(XtNinsertPosition, 3);
	NextArg(XtNborderWidth, INTERNAL_BW);
	mag_text = XtCreateManagedWidget("magnification", asciiTextWidgetClass,
					 print_panel, Args, ArgCount);
	XtOverrideTranslations(mag_text,
			       XtParseTranslationTable(text_translations));

	FirstArg(XtNlabel, "     Orientation:");
	NextArg(XtNjustify, XtJustifyLeft);
	NextArg(XtNborderWidth, 0);
	NextArg(XtNfromVert, mag_text);
	orient_lab = XtCreateManagedWidget("orient label", labelWidgetClass,
					   print_panel, Args, ArgCount);

	FirstArg(XtNfromHoriz, orient_lab);
	NextArg(XtNfromVert, mag_text);
	NextArg(XtNborderWidth, INTERNAL_BW);
	orient_panel = XtCreateManagedWidget(orient_items[print_landscape],
					     menuButtonWidgetClass,
					     print_panel, Args, ArgCount);
	orient_menu = make_popup_menu(orient_items, XtNumber(orient_items),
				      orient_panel, orient_select);

	FirstArg(XtNlabel, "   Justification:");
	NextArg(XtNjustify, XtJustifyLeft);
	NextArg(XtNborderWidth, 0);
	NextArg(XtNfromVert, orient_panel);
	just_lab = XtCreateManagedWidget("just label", labelWidgetClass,
					 print_panel, Args, ArgCount);

	FirstArg(XtNfromHoriz, just_lab);
	NextArg(XtNfromVert, orient_panel);
	NextArg(XtNborderWidth, INTERNAL_BW);
	just_panel = XtCreateManagedWidget(just_items[print_centered],
					   menuButtonWidgetClass,
					   print_panel, Args, ArgCount);
	just_menu = make_popup_menu(just_items, XtNumber(just_items),
				    just_panel, just_select);


	FirstArg(XtNlabel, "         Printer:");
	NextArg(XtNfromVert, just_panel);
	NextArg(XtNjustify, XtJustifyLeft);
	NextArg(XtNborderWidth, 0);
	printer_lab = XtCreateManagedWidget("dir label", labelWidgetClass,
					    print_panel, Args, ArgCount);

	/*
	 * don't SetValue the XtNstring so the user may specify the default
	 * printer in a resource, e.g.:  *printer*string: at6
	 */

	FirstArg(XtNwidth, 225);
	NextArg(XtNfromVert, just_panel);
	NextArg(XtNfromHoriz, printer_lab);
	NextArg(XtNeditType, "edit");
	NextArg(XtNinsertPosition, 0);
	NextArg(XtNborderWidth, INTERNAL_BW);
	printer_text = XtCreateManagedWidget("printer", asciiTextWidgetClass,
					     print_panel, Args, ArgCount);

	XtOverrideTranslations(printer_text,
			       XtParseTranslationTable(text_translations));

	FirstArg(XtNlabel, "Cancel");
	NextArg(XtNfromVert, printer_text);
	NextArg(XtNvertDistance, 15);
	NextArg(XtNhorizDistance, 55);
	NextArg(XtNheight, 25);
	NextArg(XtNborderWidth, INTERNAL_BW);
	cancel = XtCreateManagedWidget("cancel", commandWidgetClass,
				       print_panel, Args, ArgCount);
	XtAddEventHandler(cancel, ButtonReleaseMask, (Boolean) 0,
			  print_panel_cancel, (XtPointer) NULL);

	FirstArg(XtNlabel, "Print");
	NextArg(XtNfromVert, printer_text);
	NextArg(XtNfromHoriz, cancel);
	NextArg(XtNheight, 25);
	NextArg(XtNborderWidth, INTERNAL_BW);
	NextArg(XtNvertDistance, 15);
	NextArg(XtNhorizDistance, 25);
	print = XtCreateManagedWidget("print", commandWidgetClass,
				      print_panel, Args, ArgCount);
	XtAddEventHandler(print, ButtonReleaseMask, (Boolean) 0,
			  do_print, (XtPointer) NULL);

    }
    XtSetSensitive(print_w, False);
    XtPopup(print_popup, XtGrabNonexclusive);
}
