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
#include "w_util.h"
#include "w_drawprim.h"
#include "w_mousefun.h"
#include "w_setup.h"

#define	MOUSE_BUT_WID		(int) (MOUSEFUN_WD * 0.045)
#define	MOUSE_BUT_HGT		(int) (MOUSEFUN_HT * 0.5)
#define MOUSE_LEFT_SPACE 	(int) ((MOUSEFUN_WD - 5 * MOUSE_BUT_WID) / 2)
#define MOUSE_LEFT_CTR	 	(int) (MOUSE_LEFT_SPACE/2)
#define MOUSE_MID_CTR	 	(int) (MOUSEFUN_WD / 2)
#define MOUSE_RIGHT_CTR	 	(int) (MOUSEFUN_WD - MOUSE_LEFT_CTR)
#define MOUSEFUN_MAX	20

DeclareStaticArgs(14);
static char     mousefun_l[MOUSEFUN_MAX];
static char     mousefun_m[MOUSEFUN_MAX];
static char     mousefun_r[MOUSEFUN_MAX];

/* labels for the left and right buttons have 15 chars max */
static char     lr_blank[] = "               ";

/* give the middle button label a bit more space - 19 chars max */
static char     mid_blank[] = "                   ";
static Pixmap   mousefun_pm;

void
init_mousefun(tool)
    TOOL            tool;
{
    FirstArg(XtNwidth, MOUSEFUN_WD);
    NextArg(XtNheight, MOUSEFUN_HT);
    NextArg(XtNfromHoriz, msg_panel);
    NextArg(XtNhorizDistance, -INTERNAL_BW);
    NextArg(XtNfromVert, NULL);
    NextArg(XtNvertDistance, 0);
    NextArg(XtNleft, XtChainLeft);
    NextArg(XtNright, XtChainLeft);
    NextArg(XtNtop, XtChainTop);
    NextArg(XtNbottom, XtChainTop);
    NextArg(XtNborderWidth, INTERNAL_BW);
    NextArg(XtNbackgroundPixmap, NULL);
    NextArg(XtNmappedWhenManaged, False);
    NextArg(XtNlabel, "");

    mousefun = XtCreateManagedWidget("mouse_panel", labelWidgetClass,
				     tool, Args, ArgCount);
}

static void
reset_mousefun()
{
    /* get the foreground and background from the mousefun widget */
    /* and create a gc with those values */
    mouse_button_gc = XCreateGC(tool_d, XtWindow(mousefun), (unsigned long) 0, NULL);
    FirstArg(XtNforeground, &mouse_but_fg);
    NextArg(XtNbackground, &mouse_but_bg);
    GetValues(mousefun);
    XSetBackground(tool_d, mouse_button_gc, mouse_but_bg);
    XSetForeground(tool_d, mouse_button_gc, mouse_but_fg);
    XSetFont(tool_d, mouse_button_gc, button_font->fid);

    /* also create gc with fore=background for blanking areas */
    mouse_blank_gc = XCreateGC(tool_d, XtWindow(mousefun), (unsigned long) 0, NULL);
    XSetBackground(tool_d, mouse_blank_gc, mouse_but_bg);
    XSetForeground(tool_d, mouse_blank_gc, mouse_but_bg);

    mousefun_pm = XCreatePixmap(tool_d, XtWindow(mousefun),
		    MOUSEFUN_WD, MOUSEFUN_HT, DefaultDepthOfScreen(tool_s));

    XFillRectangle(tool_d, mousefun_pm, mouse_blank_gc, 0, 0,
		   MOUSEFUN_WD, MOUSEFUN_HT);

    XDrawRectangle(tool_d, mousefun_pm, mouse_button_gc, MOUSE_LEFT_SPACE,
		   (int) (MOUSEFUN_HT * 0.45), MOUSE_BUT_WID, MOUSE_BUT_HGT);
    XDrawRectangle(tool_d, mousefun_pm, mouse_button_gc,
		   MOUSE_LEFT_SPACE + 2 * MOUSE_BUT_WID,
		   (int) (MOUSEFUN_HT * 0.45), MOUSE_BUT_WID, MOUSE_BUT_HGT);
    XDrawRectangle(tool_d, mousefun_pm, mouse_button_gc,
		   MOUSE_LEFT_SPACE + 4 * MOUSE_BUT_WID,
		   (int) (MOUSEFUN_HT * 0.45), MOUSE_BUT_WID, MOUSE_BUT_HGT);
    FirstArg(XtNbackgroundPixmap, mousefun_pm);
    SetValues(mousefun);
    FirstArg(XtNmappedWhenManaged, True);
    SetValues(mousefun);
}

void
setup_mousefun()
{
    reset_mousefun();
    set_mousefun("", "", "");
    XDefineCursor(tool_d, XtWindow(mousefun), (Cursor) arrow_cursor.bitmap);
}

void
resize_mousefun()
{
    XFreePixmap(tool_d, mousefun_pm);
    reset_mousefun();
}

void
set_mousefun(left, middle, right)
    char           *left, *middle, *right;
{
    strcpy(mousefun_l, left);
    strcpy(mousefun_m, middle);
    strcpy(mousefun_r, right);
}

void
draw_mousefun_mode()
{
    clear_mousefun();
    draw_mousefun("change mode", "", "");
}

void
draw_mousefun_ind()
{
    clear_mousefun();
    draw_mousefun("menu", "dec/prev", "inc/next");
}

void
draw_mousefun_unitbox()
{
    clear_mousefun();
    draw_mousefun("pan to origin", "", "");
}

void
draw_mousefun_topruler()
{
    clear_mousefun();
    draw_mousefun("pan left", "drag", "pan right");
}

void
draw_mousefun_sideruler()
{
    clear_mousefun();
    draw_mousefun("pan up", "drag", "pan down");
}

void
draw_mousefun_canvas()
{
    clear_mousefun();
    draw_mousefun(mousefun_l, mousefun_m, mousefun_r);
}

static void
draw_mousefun_msg(s, xctr, ypos)
    char           *s;
    int             xctr, ypos;
{
    int             width;

    width = XTextWidth(button_font, s, strlen(s));
    XDrawImageString(tool_d, mousefun_pm, mouse_button_gc,
		     xctr - (int) (width / 2), ypos, s, strlen(s));
}

void
draw_mousefun(left, middle, right)
    char           *left, *middle, *right;
{
    draw_mousefun_msg(left, MOUSE_LEFT_CTR, 30);
    draw_mousefun_msg(middle, MOUSE_MID_CTR, 11);
    draw_mousefun_msg(right, MOUSE_RIGHT_CTR, 30);
    FirstArg(XtNbackgroundPixmap, 0);
    SetValues(mousefun);
    FirstArg(XtNbackgroundPixmap, mousefun_pm);
    SetValues(mousefun);
}

void
clear_mousefun()
{
    draw_mousefun(lr_blank, mid_blank, lr_blank);
}
