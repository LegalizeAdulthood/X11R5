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

/********************* IMPORTS *******************/

#include "fig.h"
#include "resources.h"
#include "paintop.h"
#include "w_canvas.h"
#include "w_drawprim.h"
#include "w_util.h"
#include "w_setup.h"

/********************* EXPORTS *******************/

int             put_msg();
int             put_fmsg();
int             init_msgreceiving();

/************************  LOCAL ******************/

#define			BUF_SIZE		128
static char     prompt[BUF_SIZE];

DeclareStaticArgs(8);

int
init_msg(tool)
    TOOL            tool;
{
    /* width */
    FirstArg(XtNwidth, MSGPANEL_WD);
    NextArg(XtNheight, MSGPANEL_HT);
    NextArg(XtNlabel, " ");
    NextArg(XtNfromVert, cmd_panel);
    NextArg(XtNvertDistance, -INTERNAL_BW);
    NextArg(XtNtop, XtChainTop);
    NextArg(XtNbottom, XtChainTop);
    NextArg(XtNborderWidth, INTERNAL_BW);
    msg_panel = XtCreateManagedWidget("message", labelWidgetClass, tool,
				      Args, ArgCount);
    return (1);
}

/*
 * We have to do this after realizing the widget otherwise the
 * width is computed wrong and you get a tiny text box.
 */
setup_msg()
{
    FirstArg(XtNfont, roman_font);
    SetValues(msg_panel);
    msg_win = XtWindow(msg_panel);
    XDefineCursor(tool_d, msg_win, (Cursor) arrow_cursor.bitmap);
}

/* VARARGS1 */
put_msg(format, arg1, arg2, arg3, arg4, arg5)
    char           *format;
    int             arg1, arg2, arg3, arg4, arg5;
{
    sprintf(prompt, format, arg1, arg2, arg3, arg4, arg5);
    FirstArg(XtNlabel, prompt);
    SetValues(msg_panel);
}

/*
 * floating point version - MIPS (DEC3100) doesn't like ints where floats are
 * used
 */

/* VARARGS1 */
put_fmsg(format, arg1, arg2, arg3, arg4, arg5)
    char           *format;
    double          arg1, arg2, arg3, arg4, arg5;
{
    sprintf(prompt, format, arg1, arg2, arg3, arg4, arg5);
    FirstArg(XtNlabel, prompt);
    SetValues(msg_panel);
}

clear_message()
{
    FirstArg(XtNlabel, "");
    SetValues(msg_panel);
}
