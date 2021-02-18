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
#include "object.h"
#include "mode.h"
#include "w_drawprim.h"         /* for char_height */
#include "w_util.h"
#include "w_setup.h"

extern String   text_translations;

static char     load_msg[] = "The current figure is modified.\nDo you want to discard it and load the new file?";

DeclareStaticArgs(12);
static Widget   file_panel, file_popup, file_status;
static Widget	cancel, save, merge, load, newfile, newdir;
static Widget   file_w;
static Position xposn, yposn;

static void
file_panel_dismiss()
{
    XtPopdown(file_popup);
    XtSetSensitive(file_w, True);
}

static void
do_merge(w, ev)
    Widget          w;
    XButtonEvent   *ev;
{
    char            filename[100];
    char           *fval, *dval;

    FirstArg(XtNstring, &dval);
    GetValues(newdir);
    FirstArg(XtNstring, &fval);
    GetValues(newfile);

    if (emptyname_msg(fval, "MERGE"))
	return;

    strcpy(filename, dval);
    strcat(filename, "/");
    strcat(filename, fval);
    if (merge_file(filename) == 0)
	file_panel_dismiss();
}

static void
do_load(w, ev)
    Widget          w;
    XButtonEvent   *ev;
{
    char           *fval, *dval;

    FirstArg(XtNstring, &dval);
    GetValues(newdir);
    FirstArg(XtNstring, &fval);
    GetValues(newfile);

    if (emptyname_msg(fval, "LOAD"))
	return;

    if (!emptyfigure() && figure_modified) {
	XtSetSensitive(load, False);
	if (!win_confirm(canvas_win, load_msg)) {
	    XtSetSensitive(load, True);
	    return;
	}
	XtSetSensitive(load, True);
    }
    if (change_directory(dval) == 0) {
	if (load_file(fval) == 0)
	    file_panel_dismiss();
    }
}

do_save(w)
    Widget          w;
{
    char           *fval, *dval;

    if (file_popup) {
	FirstArg(XtNstring, &dval);
	GetValues(newdir);
	FirstArg(XtNstring, &fval);
	GetValues(newfile);

	if (emptyname_msg(fval, "SAVE"))
	    return;

	if (change_directory(dval) == 0) {
	    XtSetSensitive(save, False);
	    if (write_file(fval) == 0) {
		strcpy(cur_filename, fval);
		reset_modifiedflag();
		file_panel_dismiss();
	    }
	    XtSetSensitive(save, True);
	}
    } else {
	if (write_file(DEF_NAME) == 0)
	    reset_modifiedflag();
    }
}

static void
file_panel_cancel(w, ev)
    Widget          w;
    XButtonEvent   *ev;
{
    file_panel_dismiss();
}

popup_file_panel(w)
    Widget          w;
{
    Widget          file, dir;
    XtTranslations  popdown_actions;
    PIX_FONT        temp_font;


    if (!file_popup) {
	file_w = w;
	XtTranslateCoords(w, (Position) 0, (Position) 0, &xposn, &yposn);

	FirstArg(XtNx, xposn);
	NextArg(XtNy, yposn + 50);
	file_popup = XtCreatePopupShell("xfig: file menu",
					transientShellWidgetClass,
					tool, Args, ArgCount);

	file_panel = XtCreateManagedWidget("file_panel", formWidgetClass,
					   file_popup, NULL, ZERO);
	popdown_actions = XtParseTranslationTable("<Btn1Up>:\n");
	XtOverrideTranslations(file_panel, popdown_actions);

	FirstArg(XtNlabel, "Directory:");
	NextArg(XtNwidth, 380);
	NextArg(XtNjustify, XtJustifyLeft);
	NextArg(XtNborderWidth, 0);
	dir = XtCreateManagedWidget("dir label", labelWidgetClass,
				    file_panel, Args, ArgCount);
	FirstArg(XtNfont, &temp_font);
        GetValues(dir);

	FirstArg(XtNwidth, 380);
        NextArg(XtNheight, char_height(temp_font) * 2 + 4);
	NextArg(XtNeditType, "edit");
	NextArg(XtNfromVert, dir);
	NextArg(XtNstring, cur_dir);
	NextArg(XtNinsertPosition, strlen(cur_dir));
	NextArg(XtNborderWidth, INTERNAL_BW);
        NextArg(XtNscrollHorizontal, XawtextScrollWhenNeeded);
	newdir = XtCreateManagedWidget("directory", asciiTextWidgetClass,
				       file_panel, Args, ArgCount);
	XtOverrideTranslations(newdir,
			       XtParseTranslationTable(text_translations));

	FirstArg(XtNlabel, "File:");
	NextArg(XtNwidth, 380);
	NextArg(XtNfromVert, newdir);
	NextArg(XtNjustify, XtJustifyLeft);
	NextArg(XtNborderWidth, 0);
	file = XtCreateManagedWidget("file label", labelWidgetClass,
				     file_panel, Args, ArgCount);

	FirstArg(XtNwidth, 380);
        NextArg(XtNheight, char_height(temp_font) * 2 + 4);
	NextArg(XtNeditType, "edit");
	NextArg(XtNstring, cur_filename);
	NextArg(XtNinsertPosition, strlen(cur_filename));
	NextArg(XtNfromVert, file);
	NextArg(XtNborderWidth, INTERNAL_BW);
        NextArg(XtNscrollHorizontal, XawtextScrollWhenNeeded);
	newfile = XtCreateManagedWidget("file", asciiTextWidgetClass,
					file_panel, Args, ArgCount);
	XtOverrideTranslations(newfile,
			       XtParseTranslationTable(text_translations));

	FirstArg(XtNlabel, "");
	NextArg(XtNwidth, 380);
	NextArg(XtNfromVert, newfile);
	NextArg(XtNjustify, XtJustifyLeft);
	NextArg(XtNborderWidth, 0);
	file_status = XtCreateManagedWidget("file status", labelWidgetClass,
					    file_panel, Args, ArgCount);

	FirstArg(XtNlabel, "Cancel");
	NextArg(XtNvertDistance, 15);
	NextArg(XtNhorizDistance, 45);
	NextArg(XtNheight, 25);
	NextArg(XtNfromVert, file_status);
	NextArg(XtNborderWidth, INTERNAL_BW);
	cancel = XtCreateManagedWidget("cancel", commandWidgetClass,
				       file_panel, Args, ArgCount);
	XtAddEventHandler(cancel, ButtonReleaseMask, (Boolean) 0,
			  file_panel_cancel, (XtPointer) NULL);

	FirstArg(XtNlabel, "Save");
	NextArg(XtNfromVert, file_status);
	NextArg(XtNfromHoriz, cancel);
	NextArg(XtNvertDistance, 15);
	NextArg(XtNhorizDistance, 25);
	NextArg(XtNheight, 25);
	NextArg(XtNborderWidth, INTERNAL_BW);
	save = XtCreateManagedWidget("save", commandWidgetClass,
				     file_panel, Args, ArgCount);
	XtAddEventHandler(save, ButtonReleaseMask, (Boolean) 0,
			  do_save, (XtPointer) NULL);

	FirstArg(XtNlabel, "Load");
	NextArg(XtNfromVert, file_status);
	NextArg(XtNfromHoriz, save);
	NextArg(XtNborderWidth, INTERNAL_BW);
	NextArg(XtNvertDistance, 15);
	NextArg(XtNhorizDistance, 25);
	NextArg(XtNheight, 25);
	load = XtCreateManagedWidget("load", commandWidgetClass,
				     file_panel, Args, ArgCount);
	XtAddEventHandler(load, ButtonReleaseMask, (Boolean) 0,
			  do_load, (XtPointer) NULL);

	FirstArg(XtNlabel, "Merge Read");
	NextArg(XtNfromVert, file_status);
	NextArg(XtNfromHoriz, load);
	NextArg(XtNborderWidth, INTERNAL_BW);
	NextArg(XtNvertDistance, 15);
	NextArg(XtNhorizDistance, 25);
	NextArg(XtNheight, 25);
	merge = XtCreateManagedWidget("merge", commandWidgetClass,
				     file_panel, Args, ArgCount);
	XtAddEventHandler(merge, ButtonReleaseMask, (Boolean) 0,
			  do_merge, (XtPointer) NULL);

    }
    XtSetSensitive(file_w, False);
    FirstArg(XtNlabel, (figure_modified ? "File Status: Modified" :
			"File Status: Not modified"));
    SetValues(file_status);
    XtPopup(file_popup, XtGrabNonexclusive);
}

