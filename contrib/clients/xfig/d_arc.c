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

/********************** DECLARATIONS ********************/

/* IMPORTS */

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

/* LOCAL */

F_pos           point[3];

static int      create_arcobject();
static int      get_arcpoint();
static int      init_arc_drawing();
static int      cancel_arc();

arc_drawing_selected()
{
    set_mousefun("first point", "", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_leftbut_proc = init_arc_drawing;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(&arrow_cursor);
    reset_action_on();
}

static
init_arc_drawing(x, y)
    int             x, y;
{
    set_mousefun("mid point", "", "cancel");
    draw_mousefun_canvas();
    canvas_rightbut_proc = cancel_arc;
    num_point = 0;
    point[num_point].x = fix_x = cur_x = x;
    point[num_point++].y = fix_y = cur_y = y;
    canvas_locmove_proc = freehand_line;
    canvas_leftbut_proc = get_arcpoint;
    canvas_middlebut_proc = null_proc;
    elastic_line();
    set_temp_cursor(&null_cursor);
    set_action_on();
}

static
cancel_arc()
{
    elastic_line();
    if (num_point == 2) {
	/* erase initial part of line */
	cur_x = point[0].x;
	cur_y = point[0].y;
	elastic_line();
    }
    arc_drawing_selected();
    draw_mousefun_canvas();
}

static
get_arcpoint(x, y)
    int             x, y;
{
    if (x == fix_x && y == fix_y)
	return;

    if (num_point == 1) {
	set_mousefun("final point", "", "cancel");
	draw_mousefun_canvas();
    }
    if (num_point == 2) {
	create_arcobject(x, y);
	return;
    }
    elastic_line();
    cur_x = x;
    cur_y = y;
    elastic_line();
    point[num_point].x = fix_x = x;
    point[num_point++].y = fix_y = y;
    elastic_line();
}

static
create_arcobject(lx, ly)
    int             lx, ly;
{
    F_arc          *arc;
    int             x, y, i;
    float           xx, yy;

    elastic_line();
    cur_x = lx;
    cur_y = ly;
    elastic_line();
    point[num_point].x = lx;
    point[num_point++].y = ly;
    x = point[0].x;
    y = point[0].y;
    /* erase previous line segment(s) if necessary */
    for (i = 1; i < num_point; i++) {
	pw_vector(canvas_win, x, y, point[i].x, point[i].y, INV_PAINT,
		  1, RUBBER_LINE, 0.0);
	x = point[i].x;
	y = point[i].y;
    }
    if (!compute_arccenter(point[0], point[1], point[2], &xx, &yy)) {
	put_msg("Invalid ARC geometry");
	arc_drawing_selected();
	draw_mousefun_canvas();
	return;
    }
    if ((arc = create_arc()) == NULL) {
	arc_drawing_selected();
	draw_mousefun_canvas();
	return;
    }
    arc->type = T_3_POINTS_ARC;
    arc->style = cur_linestyle;
    arc->thickness = cur_linewidth;
    /* scale dash length according to linethickness */
    arc->style_val = cur_styleval * (cur_linewidth + 1) / 2;
    arc->pen = 0;
    arc->area_fill = cur_areafill;
    arc->color = cur_color;
    arc->depth = 0;
    arc->direction = compute_direction(point[0], point[1], point[2]);
    if (autoforwardarrow_mode)
	arc->for_arrow = forward_arrow();
    else
	arc->for_arrow = NULL;
    if (autobackwardarrow_mode)
	arc->back_arrow = backward_arrow();
    else
	arc->back_arrow = NULL;
    arc->center.x = xx;
    arc->center.y = yy;
    arc->point[0].x = point[0].x;
    arc->point[0].y = point[0].y;
    arc->point[1].x = point[1].x;
    arc->point[1].y = point[1].y;
    arc->point[2].x = point[2].x;
    arc->point[2].y = point[2].y;
    arc->next = NULL;
    draw_arc(arc, PAINT);
    if (appres.DEBUG) {
	int             xmin, ymin, xmax, ymax;

	arc_bound(arc, &xmin, &ymin, &xmax, &ymax);
	elastic_box(xmin, ymin, xmax, ymax);
    }
    add_arc(arc);
    arc_drawing_selected();
    draw_mousefun_canvas();
}
