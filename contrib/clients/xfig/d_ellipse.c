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
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "u_create.h"
#include "u_elastic.h"
#include "u_list.h"
#include "w_canvas.h"
#include "w_mousefun.h"

/*************************  local procedures  ********************/

static int      init_ellipsebyradius_drawing();
static int      init_ellipsebydiameter_drawing();
static int      init_circlebyradius_drawing();
static int      init_circlebydiameter_drawing();
static int      create_ellipsebydia();
static int      create_ellipsebyrad();
static int      create_circlebyrad();
static int      create_circlebydia();
static int      cancel_ellipsebydia();
static int      cancel_ellipsebyrad();
static int      cancel_circlebyrad();
static int      cancel_circlebydia();

ellipsebyradius_drawing_selected()
{
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_leftbut_proc = init_ellipsebyradius_drawing;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(&arrow_cursor);
    set_mousefun("center point", "", "");
    reset_action_on();
}

static
init_ellipsebyradius_drawing(x, y)
    int             x, y;
{
    cur_x = fix_x = x;
    cur_y = fix_y = y;
    center_marker(fix_x, fix_y);
    set_mousefun("corner point", "", "cancel");
    draw_mousefun_canvas();
    canvas_locmove_proc = resizing_ebr;
    canvas_rightbut_proc = cancel_ellipsebyrad;
    canvas_leftbut_proc = create_ellipsebyrad;
    set_temp_cursor(&null_cursor);
    elastic_ebr();
    set_action_on();
}

static
cancel_ellipsebyrad()
{
    elastic_ebr();
    center_marker(fix_x, fix_y);
    ellipsebyradius_drawing_selected();
    draw_mousefun_canvas();
}

static
create_ellipsebyrad(x, y)
    int             x, y;
{
    F_ellipse      *ellipse;

    elastic_ebr();
    center_marker(fix_x, fix_y);
    if ((ellipse = create_ellipse()) == NULL)
	return;

    ellipse->type = T_ELLIPSE_BY_RAD;
    ellipse->style = cur_linestyle;
    ellipse->thickness = cur_linewidth;
    ellipse->style_val = cur_styleval * (cur_linewidth + 1) / 2;
    ellipse->angle = cur_angle;
    ellipse->color = cur_color;
    ellipse->depth = 0;
    ellipse->pen = 0;
    ellipse->area_fill = cur_areafill;
    ellipse->direction = 1;
    ellipse->center.x = fix_x;
    ellipse->center.y = fix_y;
    ellipse->radiuses.x = abs(x - fix_x) + 1;
    ellipse->radiuses.y = abs(y - fix_y) + 1;
    ellipse->start.x = fix_x;
    ellipse->start.y = fix_y;
    ellipse->end.x = x;
    ellipse->end.y = y;
    ellipse->next = NULL;
    draw_ellipse(ellipse, PAINT);
    add_ellipse(ellipse);
    ellipsebyradius_drawing_selected();
    draw_mousefun_canvas();
}

ellipsebydiameter_drawing_selected()
{
    set_mousefun("first corner", "", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_leftbut_proc = init_ellipsebydiameter_drawing;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(&arrow_cursor);
    reset_action_on();
}

static
init_ellipsebydiameter_drawing(x, y)
    int             x, y;
{
    cur_x = fix_x = x;
    cur_y = fix_y = y;
    center_marker(fix_x, fix_y);
    set_mousefun("final corner", "", "cancel");
    draw_mousefun_canvas();
    canvas_locmove_proc = resizing_ebd;
    canvas_rightbut_proc = cancel_ellipsebydia;
    canvas_leftbut_proc = create_ellipsebydia;
    set_temp_cursor(&null_cursor);
    elastic_ebd();
    set_action_on();
}

static
cancel_ellipsebydia()
{
    elastic_ebd();
    center_marker(fix_x, fix_y);
    ellipsebydiameter_drawing_selected();
    draw_mousefun_canvas();
}

static
create_ellipsebydia(x, y)
    int             x, y;
{
    F_ellipse      *ellipse;

    elastic_ebd();
    center_marker(fix_x, fix_y);
    if ((ellipse = create_ellipse()) == NULL)
	return;

    ellipse->type = T_ELLIPSE_BY_DIA;
    ellipse->style = cur_linestyle;
    ellipse->thickness = cur_linewidth;
    ellipse->style_val = cur_styleval * (cur_linewidth + 1) / 2;
    ellipse->angle = cur_angle;
    ellipse->color = cur_color;
    ellipse->depth = 0;
    ellipse->pen = NULL;
    ellipse->area_fill = cur_areafill;
    ellipse->direction = 1;
    ellipse->center.x = (fix_x + x) / 2;
    ellipse->center.y = (fix_y + y) / 2;
    ellipse->radiuses.x = abs(ellipse->center.x - fix_x);
    ellipse->radiuses.y = abs(ellipse->center.y - fix_y);
    ellipse->start.x = fix_x;
    ellipse->start.y = fix_y;
    ellipse->end.x = x;
    ellipse->end.y = y;
    ellipse->next = NULL;
    draw_ellipse(ellipse, PAINT);
    add_ellipse(ellipse);
    ellipsebydiameter_drawing_selected();
    draw_mousefun_canvas();
}

/***************************  circle  section  ************************/

circlebyradius_drawing_selected()
{
    set_mousefun("center point", "", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_leftbut_proc = init_circlebyradius_drawing;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(&arrow_cursor);
    reset_action_on();
}

static
init_circlebyradius_drawing(x, y)
    int             x, y;
{
    cur_x = fix_x = x;
    cur_y = fix_y = y;
    center_marker(fix_x, fix_y);
    set_mousefun("set radius", "", "cancel");
    draw_mousefun_canvas();
    canvas_locmove_proc = resizing_cbr;
    canvas_rightbut_proc = cancel_circlebyrad;
    canvas_leftbut_proc = create_circlebyrad;
    set_temp_cursor(&null_cursor);
    elastic_cbr();
    set_action_on();
}

static
cancel_circlebyrad()
{
    elastic_cbr();
    center_marker(fix_x, fix_y);
    circlebyradius_drawing_selected();
    draw_mousefun_canvas();
}

static
create_circlebyrad(x, y)
    int             x, y;
{
    F_ellipse      *c;
    int             rx, ry;

    elastic_cbr();
    center_marker(fix_x, fix_y);
    if ((c = create_ellipse()) == NULL)
	return;

    c->type = T_CIRCLE_BY_RAD;
    c->style = cur_linestyle;
    c->thickness = cur_linewidth;
    c->style_val = cur_styleval * (cur_linewidth + 1) / 2;
    c->angle = 0.0;
    c->color = cur_color;
    c->depth = 0;
    c->pen = NULL;
    c->area_fill = cur_areafill;
    c->direction = 1;
    c->center.x = fix_x;
    c->center.y = fix_y;
    rx = fix_x - x;
    ry = fix_y - y;
    c->radiuses.x = c->radiuses.y = round(sqrt((double) (rx * rx + ry * ry)));
    c->start.x = fix_x;
    c->start.y = fix_y;
    c->end.x = x;
    c->end.y = y;
    c->next = NULL;
    draw_ellipse(c, PAINT);
    add_ellipse(c);
    circlebyradius_drawing_selected();
    draw_mousefun_canvas();
}

circlebydiameter_drawing_selected()
{
    set_mousefun("diameter point", "", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_leftbut_proc = init_circlebydiameter_drawing;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(&arrow_cursor);
    reset_action_on();
}

static
init_circlebydiameter_drawing(x, y)
    int             x, y;
{
    cur_x = fix_x = x;
    cur_y = fix_y = y;
    center_marker(fix_x, fix_y);
    set_mousefun("final point", "", "cancel");
    draw_mousefun_canvas();
    canvas_locmove_proc = resizing_cbd;
    canvas_leftbut_proc = create_circlebydia;
    canvas_rightbut_proc = cancel_circlebydia;
    set_temp_cursor(&null_cursor);
    elastic_cbd();
    set_action_on();
}

static
cancel_circlebydia()
{
    elastic_cbd();
    center_marker(fix_x, fix_y);
    circlebydiameter_drawing_selected();
    draw_mousefun_canvas();
}

static
create_circlebydia(x, y)
    int             x, y;
{
    F_ellipse      *c;
    int             rx, ry;

    elastic_cbd();
    center_marker(fix_x, fix_y);
    if ((c = create_ellipse()) == NULL)
	return;

    c->type = T_CIRCLE_BY_DIA;
    c->style = cur_linestyle;
    c->thickness = cur_linewidth;
    c->style_val = cur_styleval * (cur_linewidth + 1) / 2;
    c->angle = 0.0;
    c->color = cur_color;
    c->depth = 0;
    c->pen = NULL;
    c->area_fill = cur_areafill;
    c->direction = 1;
    c->center.x = (fix_x + x) / 2 + .5;
    c->center.y = (fix_y + y) / 2 + .5;
    rx = x - c->center.x;
    ry = y - c->center.y;
    c->radiuses.x = c->radiuses.y = round(sqrt((double) (rx * rx + ry * ry)));
    c->start.x = fix_x;
    c->start.y = fix_y;
    c->end.x = x;
    c->end.y = y;
    c->next = NULL;
    draw_ellipse(c, PAINT);
    add_ellipse(c);
    circlebydiameter_drawing_selected();
    draw_mousefun_canvas();
}
