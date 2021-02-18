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
#include "paintop.h"

static u_int    arrow_cursor_array[8] = {
    0xC000E000, 0xF000F800, 0xFC00F000, 0x90001800,
0x18000C00, 0x0C000600, 0x06000300, 0x03000100};

mpr_static(arrow_cursor_pr, 16, 16, 1, arrow_cursor_array);
CURSORREC       arrow_cursor = {0, 0, MERGE, &arrow_cursor_pr};

static short    bull_cursor_array[16] = {
    0x0F00, 0x30C0, 0x4020, 0x4020, 0x8010, 0x8610, 0x8610, 0x8010,
    0x4020, 0x4020, 0x30C0, 0x0F00, 0x0000, 0x0000, 0x0000, 0x0000
};

mpr_static(bull_cursor_pr, 16, 16, 1, bull_cursor_array);
CURSORREC       bull_cursor = {5, 5, INV_PAINT, &bull_cursor_pr};

static u_int    buster_cursor_array[8] = {
    0x0FE01010, 0x20085004, 0x88028402, 0x82028102,
0x80828042, 0x80224014, 0x20081010, 0x0FE00000,};

mpr_static(buster_cursor_pr, 16, 16, 1, buster_cursor_array);
CURSORREC       buster_cursor = {7, 7, MERGE, &buster_cursor_pr};

static short    char_cursor_data[16] = {
    0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00,
    0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00,
};

mpr_static(char_cursor_pr, 16, 16, 1, char_cursor_data);
CURSORREC       char_cursor = {0, 13, INV_PAINT, &char_cursor_pr};

static short    crosshair_cursor_data[16] = {
    0x1000, 0x1000, 0x1000, 0xFE00, 0x1000, 0x1000, 0x1000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

mpr_static(crosshair_cursor_pr, 16, 16, 1, crosshair_cursor_data);
CURSORREC       crosshair_cursor = {3, 3, INV_PAINT, &crosshair_cursor_pr};

CURSORREC       null_cursor = {0, 0, MERGE, 0};

static short    magnifier_cursor_array[16] = {
    0x0F80, 0x3060, 0x4010, 0x4010, 0x8008, 0x8008, 0x8008, 0x8008,
    0x8008, 0x4010, 0x4010, 0x3078, 0x0F9C, 0x000E, 0x0007, 0x0003,
};

mpr_static(magnifier_cursor_pr, 16, 16, 1, magnifier_cursor_array);
CURSORREC       magnifier_cursor = {6, 6, MERGE, &magnifier_cursor_pr};

static short    pencil_cursor_array[16] = {
    0x0000, 0x0018, 0x0024, 0x0075, 0x009B, 0x0117, 0x022E, 0x045C,
    0x08B8, 0x1170, 0x22E0, 0x25C0, 0x7B80, 0x6700, 0x8600, 0x0800,
};

mpr_static(pencil_cursor_pr, 16, 16, 1, pencil_cursor_array);
CURSORREC       pencil_cursor = {0, 14, MERGE, &pencil_cursor_pr};

static u_int    pick15_cursor_array[8] = {
    0x0FE01010, 0x20084004, 0x80028002, 0x80028002,
0x80028002, 0x80024004, 0x20081010, 0x0FE00000,};

mpr_static(pick15_cursor_pr, 16, 16, 1, pick15_cursor_array);
CURSORREC       pick15_cursor = {7, 7, MERGE, &pick15_cursor_pr};

static short    pick9_cursor_array[16] = {
    0x3E00, 0x4100, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0x4100,
    0x3E00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

mpr_static(pick9_cursor_pr, 16, 16, 1, pick9_cursor_array);
CURSORREC       pick9_cursor = {4, 4, MERGE, &pick9_cursor_pr};

static short    vbar_cursor_array[16] = {
    0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
    0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
};

mpr_static(vbar_cursor_pr, 16, 16, 1, vbar_cursor_array);
CURSORREC       vbar_cursor = {0, 13, INV_PAINT, &vbar_cursor_pr};

static u_int    wait_cursor_array[8] = {
    0xFFE04040, 0x40403F80, 0x3F801F00, 0x0E000400,
0x0A001100, 0x20802080, 0x40404040, 0xFFE00000,};

mpr_static(wait_cursor_pr, 16, 16, 1, wait_cursor_array);
CURSORREC       wait_cursor = {7, 5, MERGE, &wait_cursor_pr};

static char     panel_cursor_array[26] = {
    0xff, 0x01, 0x01, 0x01, 0x1d, 0x01, 0x1d, 0x01, 0x1d, 0x01, 0x01, 0x01,
    0x7d, 0x01, 0x01, 0x01, 0x1d, 0x01, 0x01, 0x01, 0x3d, 0x01, 0x01, 0x01,
0xff, 0x01,};

mpr_static(panel_cursor_pr, 9, 13, 1, panel_cursor_array);
CURSORREC       panel_cursor = {3, 3, MERGE, &panel_cursor_pr};

mpr_static(l_arrow_cursor_pr, 16, 16, 1, wait_cursor_array);
CURSORREC       l_arrow_cursor = {7, 5, MERGE, &l_arrow_cursor_pr};

mpr_static(r_arrow_cursor_pr, 16, 16, 1, wait_cursor_array);
CURSORREC       r_arrow_cursor = {7, 5, MERGE, &r_arrow_cursor_pr};

mpr_static(lr_arrow_cursor_pr, 16, 16, 1, wait_cursor_array);
CURSORREC       lr_arrow_cursor = {7, 5, MERGE, &lr_arrow_cursor_pr};

mpr_static(u_arrow_cursor_pr, 16, 16, 1, wait_cursor_array);
CURSORREC       u_arrow_cursor = {7, 5, MERGE, &u_arrow_cursor_pr};

mpr_static(d_arrow_cursor_pr, 16, 16, 1, wait_cursor_array);
CURSORREC       d_arrow_cursor = {7, 5, MERGE, &d_arrow_cursor_pr};

mpr_static(ud_arrow_cursor_pr, 16, 16, 1, wait_cursor_array);
CURSORREC       ud_arrow_cursor = {7, 5, MERGE, &ud_arrow_cursor_pr};

CURSOR          cur_cursor = &arrow_cursor;	/* current cursor */

init_cursor()
{
    register Display *d = tool_d;

    arrow_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_left_ptr);
    bull_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_circle);
    buster_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_pirate);
    char_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_question_arrow);
    crosshair_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_crosshair);
    null_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_tcross);
    magnifier_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_question_arrow);
    pencil_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_pencil);
    pick15_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_dotbox);
    pick9_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_hand1);
    vbar_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_xterm);
    wait_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_watch);
    panel_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_icon);
    lr_arrow_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_sb_h_double_arrow);
    l_arrow_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_sb_left_arrow);
    r_arrow_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_sb_right_arrow);
    ud_arrow_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_sb_v_double_arrow);
    u_arrow_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_sb_up_arrow);
    d_arrow_cursor.bitmap = (XImage *) XCreateFontCursor(d, XC_sb_down_arrow);
}

recolor_cursors()
{
    register Display *d = tool_d;

    XRecolorCursor(d, (Cursor) arrow_cursor.bitmap, &x_fg_color, &x_bg_color);
    XRecolorCursor(d, (Cursor) bull_cursor.bitmap, &x_fg_color, &x_bg_color);
    XRecolorCursor(d, (Cursor) buster_cursor.bitmap, &x_fg_color, &x_bg_color);
    XRecolorCursor(d, (Cursor) char_cursor.bitmap, &x_fg_color, &x_bg_color);
    XRecolorCursor(d, (Cursor) crosshair_cursor.bitmap, &x_fg_color,
		   &x_bg_color);
    XRecolorCursor(d, (Cursor) null_cursor.bitmap, &x_fg_color, &x_bg_color);
    XRecolorCursor(d, (Cursor) magnifier_cursor.bitmap, &x_fg_color,
		   &x_bg_color);
    XRecolorCursor(d, (Cursor) pencil_cursor.bitmap, &x_fg_color, &x_bg_color);
    XRecolorCursor(d, (Cursor) pick15_cursor.bitmap, &x_fg_color, &x_bg_color);
    XRecolorCursor(d, (Cursor) pick9_cursor.bitmap, &x_fg_color, &x_bg_color);
    XRecolorCursor(d, (Cursor) vbar_cursor.bitmap, &x_fg_color, &x_bg_color);
    XRecolorCursor(d, (Cursor) wait_cursor.bitmap, &x_fg_color, &x_bg_color);
    XRecolorCursor(d, (Cursor) panel_cursor.bitmap, &x_fg_color, &x_bg_color);
    XRecolorCursor(d, (Cursor) l_arrow_cursor.bitmap, &x_fg_color, &x_bg_color);
    XRecolorCursor(d, (Cursor) r_arrow_cursor.bitmap, &x_fg_color, &x_bg_color);
    XRecolorCursor(d, (Cursor) lr_arrow_cursor.bitmap, &x_fg_color, &x_bg_color);
    XRecolorCursor(d, (Cursor) u_arrow_cursor.bitmap, &x_fg_color, &x_bg_color);
    XRecolorCursor(d, (Cursor) d_arrow_cursor.bitmap, &x_fg_color, &x_bg_color);
    XRecolorCursor(d, (Cursor) ud_arrow_cursor.bitmap, &x_fg_color, &x_bg_color);
}

reset_cursor()
{
    XDefineCursor(tool_d, canvas_win, (Cursor) cur_cursor->bitmap);
}

set_temp_cursor(cursor)
    CURSOR          cursor;
{
    XDefineCursor(tool_d, canvas_win, (Cursor) cursor->bitmap);
}

set_cursor(cursor)
    CURSOR          cursor;
{
    cur_cursor = cursor;
    XDefineCursor(tool_d, canvas_win, (Cursor) cursor->bitmap);
}
