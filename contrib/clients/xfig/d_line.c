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

extern int      latex_endpoint();

/*************************** locally global variables *********************/

static int      init_line_drawing();

int             create_lineobject();
int             create_latexobject();
int             get_direction();
int             get_intermediatepoint();
int             get_latexpoint();

/**********************  polyline and polygon section  **********************/

line_drawing_selected()
{
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_leftbut_proc = init_line_drawing;
    canvas_rightbut_proc = null_proc;
    set_cursor(&arrow_cursor);
    reset_action_on();
    if (cur_mode == F_POLYGON) {
	set_mousefun("first point", "", "");
	min_num_points = 3;
	canvas_middlebut_proc = null_proc;
    } else {
	set_mousefun("first point", "single point", "");
	min_num_points = 1;
	num_point = 0;
	fix_x = fix_y = -1;
	canvas_middlebut_proc = create_lineobject;
    }
}

static
init_line_drawing(x, y)
    int             x, y;
{
    init_trace_drawing(x, y);
}

cancel_line_drawing()
{
    elastic_line();
    cur_x = fix_x;
    cur_y = fix_y;
    elastic_moveline(first_point);	/* erase control vector */
    free_points(first_point);
    line_drawing_selected();
    draw_mousefun_canvas();
}

init_trace_drawing(x, y)
    int             x, y;
{
    if ((first_point = create_point()) == NULL)
	return;

    cur_point = first_point;
    set_action_on();
    cur_point->x = fix_x = cur_x = x;
    cur_point->y = fix_y = cur_y = y;
    cur_point->next = NULL;
    if (latexline_mode || latexarrow_mode) {
	canvas_locmove_proc = latex_line;
	canvas_leftbut_proc = get_latexpoint;
	canvas_middlebut_save = create_latexobject;
    } else if (manhattan_mode || mountain_mode) {
	canvas_locmove_proc = constrainedangle_line;
	canvas_leftbut_proc = get_direction;
	canvas_middlebut_save = create_lineobject;
    } else {
	canvas_locmove_proc = freehand_line;
	canvas_leftbut_proc = get_intermediatepoint;
	canvas_middlebut_save = create_lineobject;
    }
    canvas_rightbut_proc = cancel_line_drawing;
    num_point = 1;
    set_mousefun("next point", "", "cancel");
    if (num_point >= min_num_points - 1) {
	set_mousefun("next point", "final point", "cancel");
	canvas_middlebut_proc = canvas_middlebut_save;
    }
    draw_mousefun_canvas();
    set_temp_cursor(&null_cursor);
    cur_cursor = &null_cursor;
    elastic_line();
}

get_direction(x, y)
    int             x, y;
{
    (*canvas_locmove_proc) (x, y);
    canvas_locmove_proc = constrainedangle_line;
    get_intermediatepoint(cur_x, cur_y);
}

get_latexpoint(x, y)
    int             x, y;
{
    elastic_latexline();
    latex_endpoint(fix_x, fix_y, x, y, &cur_x, &cur_y, latexarrow_mode,
		   (cur_pointposn == P_ANY) ? 1 : posn_rnd[cur_pointposn]);
    if (cur_cursor != &null_cursor) {
	set_temp_cursor(&null_cursor);
	cur_cursor = &null_cursor;
    }
    win_setmouseposition(canvas_win, cur_x, cur_y);
    get_intermediatepoint(cur_x, cur_y);
}

get_intermediatepoint(x, y)
    int             x, y;
{
    elastic_line();		/* erase elastic line */
    cur_x = x;
    cur_y = y;
    elastic_line();
    num_point++;
    fix_x = x;
    fix_y = y;
    elastic_line();
    append_point(fix_x, fix_y, &cur_point);
    if (num_point == min_num_points - 1) {
	set_mousefun("next point", "final point", "cancel");
	draw_mousefun_canvas();
	canvas_middlebut_proc = canvas_middlebut_save;
    }
}

create_latexobject(x, y)
    int             x, y;
{
    if (x != fix_x || y != fix_y) {
	elastic_latexline();
	latex_endpoint(fix_x, fix_y, x, y, &cur_x, &cur_y, latexarrow_mode,
		    (cur_pointposn == P_ANY) ? 1 : posn_rnd[cur_pointposn]);
	if (cur_cursor != &null_cursor) {
	    set_temp_cursor(&null_cursor);
	    cur_cursor = &null_cursor;
	}
    }
    create_lineobject(cur_x, cur_y);
}

/* come here upon pressing middle button (last point of lineobject) */

create_lineobject(x, y)
    int             x, y;
{
    F_line         *line;
    int             dot;

    if (num_point == 0) {
	if ((first_point = create_point()) == NULL) {
	    line_drawing_selected();
	    draw_mousefun_canvas();
	    return;
	}
	cur_point = first_point;
	first_point->x = fix_x = cur_x = x;
	first_point->y = fix_y = cur_y = y;
	first_point->next = NULL;
	num_point++;
    } else if (x != fix_x || y != fix_y) {
	if (manhattan_mode || mountain_mode)
	    get_direction(x, y);
	else if (latexline_mode || latexarrow_mode)
	    get_latexpoint(x, y);
	else
	    get_intermediatepoint(x, y);
    }
    dot = (num_point == 1);
    elastic_line();
    if ((line = create_line()) == NULL) {
	line_drawing_selected();
	draw_mousefun_canvas();
	return;
    }
    line->type = T_POLYLINE;
    line->thickness = cur_linewidth;
    line->area_fill = cur_areafill;
    line->style = cur_linestyle;
    line->style_val = cur_styleval * (cur_linewidth + 1) / 2;
    line->color = cur_color;
    line->depth = 0;
    line->pen = 0;
    line->points = first_point;
    if (!dot) {
	if (cur_mode == F_POLYGON) {	/* close off polygon */
	    line->type = T_POLYGON;
	    num_point++;
	    append_point(first_point->x, first_point->y, &cur_point);
	    elastic_line();
	    fix_x = first_point->x;
	    fix_y = first_point->y;
	    elastic_line();	/* fix last elastic line */
	} else {		/* polyline; draw any arrows */
	    if (autoforwardarrow_mode)
		line->for_arrow = forward_arrow();
		/* arrow will be drawn in draw_line below */
	    if (autobackwardarrow_mode) 
		line->back_arrow = backward_arrow();
		/* arrow will be drawn in draw_line below */
	}
	cur_x = fix_x;
	cur_y = fix_y;
	elastic_moveline(first_point);	/* erase temporary outline */
    }
    draw_line(line, PAINT);	/* draw final */
    add_line(line);
    line_drawing_selected();
    draw_mousefun_canvas();
}
