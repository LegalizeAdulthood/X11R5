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
#include "w_drawprim.h"		/* for char_height */
#include "w_setup.h"
#include "w_util.h"

extern String   text_translations;
extern Widget   make_popup_menu();

/* LOCAL */

static char     default_file[60];
static char     named_file[60];

static char    *lang_items[] = {
    "box     ", "epic    ", "eepic   ", "eepicemu", "latex   ",
    "null    ", "pic     ", "pictex  ", "ps      ", "pstex   ",
"pstex_t ", "textyl  ", "tpic    ", "Xbitmap "};

#define LANG_PS		8	/* position of ps starting from 0 */
#define LANG_XBITMAP	13	/* position of Xbitmap starting from 0 */

static char    *orient_items[] = {
    "portrait ",
"landscape"};

static void     orient_select();
static Widget   orient_panel, orient_menu, orient_lab;

static void     lang_select();
static Widget   lang_panel, lang_menu, lang_lab;
static int      lang = LANG_PS;

static Widget   cancel_but, export_but;
static Widget   dfile_lab, dfile_text, nfile_lab, nfile_text;
static Widget   export_panel, export_popup, mag_lab, mag_text, export_w;
static Position xposn, yposn;

static void
export_panel_dismiss()
{
    XtPopdown(export_popup);
    XtSetSensitive(export_w, True);
}

static void
export_panel_cancel(w, ev)
    Widget          w;
    XButtonEvent   *ev;
{
    export_panel_dismiss();
}

static char     export_msg[] = "EXPORT";

do_export(w)
    Widget          w;
{
    DeclareArgs(1);
    float           mag;
    char           *fval;

    if (emptyfigure_msg(export_msg))
	return;

    if (export_popup) {
	FirstArg(XtNstring, &fval);
	GetValues(nfile_text);
	if (emptyname(fval))
	    fval = default_file;

	/* check for XBitmap first */
	if (lang == LANG_XBITMAP) {
	    XtSetSensitive(export_but, False);
	    if (write_bitmap(fval) == 0)
		export_panel_dismiss();
	    XtSetSensitive(export_but, True);
	} else {
	    mag = (float) atof(panel_get_value(mag_text)) / 100.0;
	    if (mag <= 0.0)
		mag = 1.0;
	    XtSetSensitive(export_but, False);
	    if (print_to_file(fval, lang_items[lang], mag) == 0)
		export_panel_dismiss();
	    XtSetSensitive(export_but, True);
	}
    } else {
	/* might need to change this if we let user choose default lang */
	print_to_file("unnamed.ps", "ps", 1.0);
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
lang_select(w, new_lang, garbage)
    Widget          w;
    XtPointer       new_lang, garbage;
{
    DeclareArgs(1);

    FirstArg(XtNlabel, XtName(w));
    SetValues(lang_panel);
    lang = (int) new_lang;
    if (lang == LANG_XBITMAP) {
	XtSetSensitive(mag_lab, False);
	XtSetSensitive(mag_text, False);
	XtSetSensitive(orient_lab, False);
	XtSetSensitive(orient_panel, False);
    } else {
	XtSetSensitive(mag_lab, True);
	XtSetSensitive(mag_text, True);
	XtSetSensitive(orient_lab, True);
	XtSetSensitive(orient_panel, True);
    }
    update_def_filename();
    FirstArg(XtNlabel, default_file);
    SetValues(dfile_text);
}

popup_export_panel(w)
    Widget          w;
{
    Widget          image;
    XtTranslations  popdown_actions;
    PIX_FONT        temp_font;

    DeclareArgs(10);

    if (!export_popup) {
	export_w = w;
	XtTranslateCoords(w, (Position) 0, (Position) 0, &xposn, &yposn);

	FirstArg(XtNx, xposn);
	NextArg(XtNy, yposn + 50);
	export_popup = XtCreatePopupShell("xfig: export menu",
					  transientShellWidgetClass,
					  tool, Args, ArgCount);

	export_panel = XtCreateManagedWidget("export_panel", formWidgetClass,
					     export_popup, NULL, ZERO);
	popdown_actions = XtParseTranslationTable(
						  "<Btn1Up>:\n");
	XtOverrideTranslations(export_panel, popdown_actions);

	FirstArg(XtNlabel, "     Magnification%:");
	NextArg(XtNjustify, XtJustifyLeft);
	NextArg(XtNborderWidth, 0);
	mag_lab = XtCreateManagedWidget("mag label", labelWidgetClass,
					export_panel, Args, ArgCount);

	FirstArg(XtNwidth, 40);
	NextArg(XtNfromHoriz, mag_lab);
	NextArg(XtNeditType, "edit");
	NextArg(XtNstring, "100");
	NextArg(XtNinsertPosition, 3);
	NextArg(XtNborderWidth, INTERNAL_BW);
	mag_text = XtCreateManagedWidget("magnification", asciiTextWidgetClass,
					 export_panel, Args, ArgCount);
	XtOverrideTranslations(mag_text,
			       XtParseTranslationTable(text_translations));

	FirstArg(XtNlabel, "        Orientation:");
	NextArg(XtNjustify, XtJustifyLeft);
	NextArg(XtNborderWidth, 0);
	NextArg(XtNfromVert, mag_text);
	orient_lab = XtCreateManagedWidget("orient label", labelWidgetClass,
					   export_panel, Args, ArgCount);

	FirstArg(XtNfromHoriz, orient_lab);
	NextArg(XtNfromVert, mag_text);
	NextArg(XtNborderWidth, INTERNAL_BW);
	orient_panel = XtCreateManagedWidget(orient_items[print_landscape],
					     menuButtonWidgetClass,
					     export_panel, Args, ArgCount);
	orient_menu = make_popup_menu(orient_items, XtNumber(orient_items),
				      orient_panel, orient_select);

	FirstArg(XtNlabel, "           Language:");
	NextArg(XtNjustify, XtJustifyLeft);
	NextArg(XtNborderWidth, 0);
	NextArg(XtNfromVert, orient_panel);
	lang_lab = XtCreateManagedWidget("lang label", labelWidgetClass,
					 export_panel, Args, ArgCount);

	FirstArg(XtNfromHoriz, lang_lab);
	NextArg(XtNfromVert, orient_panel);
	NextArg(XtNborderWidth, INTERNAL_BW);
	lang_panel = XtCreateManagedWidget(
				    lang_items[lang], menuButtonWidgetClass,
					   export_panel, Args, ArgCount);
	lang_menu = make_popup_menu(lang_items, XtNumber(lang_items),
				    lang_panel, lang_select);

	FirstArg(XtNlabel, "Default Output File:");
	NextArg(XtNfromVert, lang_panel);
	NextArg(XtNjustify, XtJustifyLeft);
	NextArg(XtNborderWidth, 0);
	dfile_lab = XtCreateManagedWidget("file label", labelWidgetClass,
					  export_panel, Args, ArgCount);

	FirstArg(XtNlabel, default_file);
	NextArg(XtNfromVert, lang_panel);
	NextArg(XtNfromHoriz, dfile_lab);
	NextArg(XtNjustify, XtJustifyLeft);
	NextArg(XtNborderWidth, 0);
	NextArg(XtNwidth, 250);
	dfile_text = XtCreateManagedWidget("file label", labelWidgetClass,
					   export_panel, Args, ArgCount);

	FirstArg(XtNlabel, "        Output File:");
	NextArg(XtNfromVert, dfile_text);
	NextArg(XtNjustify, XtJustifyLeft);
	NextArg(XtNborderWidth, 0);
	nfile_lab = XtCreateManagedWidget("file label", labelWidgetClass,
					  export_panel, Args, ArgCount);

	FirstArg(XtNfont, &temp_font);
	GetValues(nfile_lab);

	FirstArg(XtNwidth, 250);
	NextArg(XtNheight, char_height(temp_font) * 2 + 4);
	NextArg(XtNfromHoriz, nfile_lab);
	NextArg(XtNfromVert, dfile_text);
	NextArg(XtNeditType, "edit");
	NextArg(XtNborderWidth, INTERNAL_BW);
	NextArg(XtNstring, named_file);
	NextArg(XtNinsertPosition, strlen(named_file));
	NextArg(XtNscrollHorizontal, XawtextScrollWhenNeeded);
	nfile_text = XtCreateManagedWidget("file", asciiTextWidgetClass,
					   export_panel, Args, ArgCount);
	XtOverrideTranslations(nfile_text,
			       XtParseTranslationTable(text_translations));

	FirstArg(XtNlabel, "Cancel");
	NextArg(XtNfromVert, nfile_text);
	NextArg(XtNvertDistance, 15);
	NextArg(XtNhorizDistance, 55);
	NextArg(XtNheight, 25);
	NextArg(XtNborderWidth, INTERNAL_BW);
	cancel_but = XtCreateManagedWidget("cancel", commandWidgetClass,
					   export_panel, Args, ArgCount);
	XtAddEventHandler(cancel_but, ButtonReleaseMask, (Boolean) 0,
			  export_panel_cancel, (XtPointer) NULL);

	FirstArg(XtNlabel, "Export");
	NextArg(XtNfromVert, nfile_text);
	NextArg(XtNfromHoriz, cancel_but);
	NextArg(XtNvertDistance, 15);
	NextArg(XtNhorizDistance, 25);
	NextArg(XtNheight, 25);
	NextArg(XtNborderWidth, INTERNAL_BW);
	export_but = XtCreateManagedWidget("export", commandWidgetClass,
					   export_panel, Args, ArgCount);
	XtAddEventHandler(export_but, ButtonReleaseMask, (Boolean) 0,
			  do_export, (XtPointer) NULL);

	if (lang == LANG_XBITMAP) {
	    XtSetSensitive(mag_lab, False);
	    XtSetSensitive(mag_text, False);
	    XtSetSensitive(orient_lab, False);
	    XtSetSensitive(orient_panel, False);
	}
    }
    XtSetSensitive(export_w, False);
    update_def_filename();
    FirstArg(XtNlabel, default_file);
    NextArg(XtNwidth, 250);
    SetValues(dfile_text);
    XtPopup(export_popup, XtGrabNonexclusive);
}

update_def_filename()
{
    int             i;

    (void) strcpy(default_file, cur_filename);
    if (default_file[0] != '\0') {
	i = strlen(default_file);
	if (i >= 4 && strcmp(&default_file[i - 4], ".fig") == 0)
	    default_file[i - 4] = '\0';
	(void) strcat(default_file, ".");
	(void) strcat(default_file, lang_items[lang]);
    }
    /* remove trailing blanks */
    for (i = strlen(default_file) - 1; i >= 0; i--)
	if (default_file[i] == ' ')
	    default_file[i] = '\0';
	else
	    i = 0;
}
