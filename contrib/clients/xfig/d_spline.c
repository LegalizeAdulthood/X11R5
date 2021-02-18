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

static int      create_splineobject();
static int      init_spline_drawing();

spline_drawing_selected()
{
    set_mousefun("first point", "", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_leftbut_proc = init_spline_drawing;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(&arrow_cursor);
    reset_action_on();
}

static
init_spline_drawing(x, y)
    int             x, y;
{
    if (cur_mode == F_CLOSED_SPLINE) {
	min_num_points = 3;
	init_trace_drawing(x, y);
	canvas_middlebut_save = create_splineobject;
    } else {
	min_num_points = 2;
	init_trace_drawing(x, y);
	canvas_middlebut_proc = create_splineobject;
    }
}

static
create_splineobject(x, y)
    int             x, y;
{
    F_spline       *spline;

    if (x != fix_x || y != fix_y)
	get_intermediatepoint(x, y);
    elastic_line();
    if ((spline = create_spline()) == NULL)
	return;

    spline->style = cur_linestyle;
    spline->thickness = cur_linewidth;
    spline->style_val = cur_styleval * (cur_linewidth + 1) / 2;
    spline->color = cur_color;
    spline->depth = 0;
    spline->pen = 0;
    spline->area_fill = cur_areafill;
    /*
     * The current area fill color is saved in all spline objects (but
     * support for filling may not be available in all fig2dev languages).
     * Currently, splines are drawn in overlapping sections which make
     * drawing the area fill difficult, so no filling is done on screen.
     */
    spline->points = first_point;
    spline->controls = NULL;
    spline->next = NULL;
    /* initialise for no arrows - updated below if necessary */
    spline->for_arrow = NULL;
    spline->back_arrow = NULL;
    cur_x = cur_y = fix_x = fix_y = 0;	/* used in elastic_moveline */
    elastic_moveline(spline->points);	/* erase control vector */
    if (cur_mode == F_CLOSED_SPLINE) {
	spline->type = T_CLOSED_NORMAL;
	num_point++;
	append_point(first_point->x, first_point->y, &cur_point);
	draw_closed_spline(spline, PAINT);
    } else {			/* It must be F_SPLINE */
	if (autoforwardarrow_mode)
	    spline->for_arrow = forward_arrow();
	if (autobackwardarrow_mode)
	    spline->back_arrow = backward_arrow();
	spline->type = T_OPEN_NORMAL;
	draw_open_spline(spline, PAINT);
    }
    if (appres.DEBUG) {
	int             xmin, ymin, xmax, ymax;

	spline_bound(spline, &xmin, &ymin, &xmax, &ymax);
	elastic_box(xmin, ymin, xmax, ymax);
    }
    add_spline(spline);
    spline_drawing_selected();
    draw_mousefun_canvas();
}
