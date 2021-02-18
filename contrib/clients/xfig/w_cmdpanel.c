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
#include "resources.h"
#include "mode.h"
#include "w_canvas.h"	/* for null_proc() */
#include "w_drawprim.h"
#include "w_mousefun.h"
#include "w_util.h"
#include "w_setup.h"

extern          erase_objecthighlight();
extern          emptyfigure();
extern		do_print(), do_export(), do_save();
extern void     undo(), redisplay_canvas();
extern void     popup_print_panel(), popup_file_panel(), popup_export_panel();

void            init_cmd_panel();
void            setup_cmd_panel();

/* internal features and definitions */

/* cmd panel definitions */
#define CMD_LABEL_LEN	16
typedef struct cmd_switch_struct {
    char            label[CMD_LABEL_LEN];
    void            (*cmd_func) ();
    int             (*quick_func) ();
    char            mousefun_l[CMD_LABEL_LEN];
    char            mousefun_r[CMD_LABEL_LEN];
    TOOL            widget;
}               cmd_sw_info;

#define	cmd_action(z)	(z->cmd_func)(z->widget)
#define	quick_action(z)	(z->quick_func)(z->widget)

/* prototypes */
static void     sel_cmd_but();
static void     enter_cmd_but();
void            quit();
static void     delete_all_cmd();
static void     paste();

/* command panel of switches below the lower ruler */
static cmd_sw_info cmd_switches[] = {
    {"Quit", quit, null_proc, "command", ""},
    {"Delete ALL", delete_all_cmd, null_proc, "command", ""},
    {"Undo", undo, null_proc, "command", ""},
    {"Redraw", redisplay_canvas, null_proc, "command", ""},
    {"Paste", paste, null_proc, "command", ""},
    {"File...", popup_file_panel, do_save, "popup", "save shortcut"},
    {"Export...", popup_export_panel, do_export, "popup", "export shortcut"},
    {"Print...", popup_print_panel, do_print, "popup", "print shortcut"},
};

#define         NUM_CMD_SW  (sizeof(cmd_switches) / sizeof(cmd_sw_info))

static XtActionsRec cmd_actions[] =
{
    {"LeaveCmdSw", (XtActionProc) clear_mousefun},
};

static String   cmd_translations =
"<Btn1Down>:set()\n\
    <Btn1Up>:unset()\n\
    <LeaveWindow>:LeaveCmdSw()reset()\n";

DeclareStaticArgs(11);

int
num_cmd_sw()
{
    return (NUM_CMD_SW);
}

/* command panel */

void
init_cmd_panel(tool)
    TOOL            tool;
{
    register int    i;
    register cmd_sw_info *sw;
    Widget          beside = NULL;

    FirstArg(XtNborderWidth, 0);
    NextArg(XtNdefaultDistance, 0);
    NextArg(XtNhorizDistance, 0);
    NextArg(XtNvertDistance, 0);
    NextArg(XtNleft, XtChainLeft);
    NextArg(XtNright, XtChainLeft);
    NextArg(XtNtop, XtChainTop);
    NextArg(XtNbottom, XtChainTop);
    cmd_panel = XtCreateWidget("commands", formWidgetClass, tool,
			       Args, ArgCount);
    XtAppAddActions(tool_app, cmd_actions, XtNumber(cmd_actions));

    FirstArg(XtNborderWidth, INTERNAL_BW);
    NextArg(XtNfont, button_font);
    NextArg(XtNheight, CMDPANEL_HT - 2 * INTERNAL_BW);
    NextArg(XtNwidth, CMDPANEL_WD / NUM_CMD_SW - INTERNAL_BW);
    NextArg(XtNvertDistance, 0);
    NextArg(XtNhorizDistance, 0);
    for (i = 0; i < NUM_CMD_SW; ++i) {
	sw = &cmd_switches[i];
	NextArg(XtNlabel, sw->label);
	NextArg(XtNfromHoriz, beside);
	sw->widget = XtCreateManagedWidget("button", commandWidgetClass,
					   cmd_panel, Args, ArgCount);
	/* setup callback and default actions */
	XtAddEventHandler(sw->widget, ButtonReleaseMask, (Boolean) 0,
			  sel_cmd_but, (caddr_t) sw);
	XtAddEventHandler(sw->widget, EnterWindowMask, (Boolean) 0,
			  enter_cmd_but, (caddr_t) sw);
	XtOverrideTranslations(sw->widget,
			       XtParseTranslationTable(cmd_translations));
	ArgCount -= 3;
	NextArg(XtNhorizDistance, -INTERNAL_BW);
	beside = sw->widget;
    }
    return;
}

void
setup_cmd_panel()
{
    register int    i;
    register cmd_sw_info *sw;

    XDefineCursor(tool_d, XtWindow(cmd_panel), (Cursor) arrow_cursor.bitmap);

    for (i = 0; i < NUM_CMD_SW; ++i) {
	sw = &cmd_switches[i];
	FirstArg(XtNfont, button_font);	/* label font */
	SetValues(sw->widget);
    }
}

static void
enter_cmd_but(widget, sw, event)
    Widget          widget;
    cmd_sw_info    *sw;
    XButtonEvent   *event;
{
    clear_mousefun();
    draw_mousefun(sw->mousefun_l, "", sw->mousefun_r);
}

static void
sel_cmd_but(widget, sw, event)
    Widget          widget;
    cmd_sw_info    *sw;
    XButtonEvent   *event;
{
    if (event->button == Button2)
	return;

    if (action_on) {
	if (cur_mode == F_TEXT)
	    finish_text_input();/* finish up any text input */
	else {
	    put_msg("FINISH drawing current object first");
	    return;
	}
    } else if (highlighting)
	erase_objecthighlight();

    if (event->button == Button1)
	cmd_action(sw);
    else
	quick_action(sw);
}

static char     quit_msg[] = "The current figure is modified.\nDo you want to quit anyway?";

void
quit(w)
    Widget          w;
{
    if (!emptyfigure() && figure_modified && !aborting) {
	XtSetSensitive(w, False);
	if (!win_confirm(canvas_win, quit_msg)) {
	    XtSetSensitive(w, True);
	    return;
	}
    }
    /* delete the cut buffer only if it is a temporary file */
    if (strncmp(cut_buf_name, "/tmp", 4) == 0)
	unlink(cut_buf_name);

    XtDestroyWidget(tool);
    exit(0);
}

static void
paste()
{
    merge_file(cut_buf_name);
}

static void
delete_all_cmd()
{
    if (emptyfigure()) {
	put_msg("Figure already empty");
	return;
    }
    delete_all();
    put_msg("Immediate Undo will restore the figure");
    redisplay_canvas();
}
