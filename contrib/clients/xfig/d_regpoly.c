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

extern float    compute_angle();

/*************************** local declarations *********************/

static          init_regpoly_drawing(), create_regpoly(), cancel_regpoly();

regpoly_drawing_selected()
{
    set_mousefun("center point", "", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_leftbut_proc = init_regpoly_drawing;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(&arrow_cursor);
    reset_action_on();
}

static
init_regpoly_drawing(x, y)
    int             x, y;
{
    cur_x = fix_x = x;
    cur_y = fix_y = y;
    work_numsides = cur_numsides;
    set_mousefun("final point", "", "cancel");
    draw_mousefun_canvas();
    canvas_locmove_proc = resizing_poly;
    canvas_leftbut_proc = create_regpoly;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = cancel_regpoly;
    elastic_poly(fix_x, fix_y, cur_x, cur_y, work_numsides);
    set_temp_cursor(&null_cursor);
    set_action_on();
}

static
cancel_regpoly()
{
    elastic_poly(fix_x, fix_y, cur_x, cur_y, work_numsides);
    regpoly_drawing_selected();
    draw_mousefun_canvas();
}

static
create_regpoly(x, y)
    int             x, y;
{
    register float  angle;
    register int    nx, ny, dx, dy, i;
    float           init_angle, mag;
    F_line         *poly;
    F_point        *point;

    elastic_poly(fix_x, fix_y, cur_x, cur_y, work_numsides);
    if (fix_x == x && fix_y == y)
	return;			/* 0 size */

    if ((point = create_point()) == NULL)
	return;

    point->x = x;
    point->y = y;
    point->next = NULL;

    if ((poly = create_line()) == NULL) {
	free((char *) point);
	return;
    }
    poly->type = T_POLYGON;
    poly->style = cur_linestyle;
    poly->thickness = cur_linewidth;
    poly->color = cur_color;
    poly->depth = 0;
    poly->pen = 0;
    poly->area_fill = cur_areafill;
    /* scale dash length by line thickness */
    poly->style_val = cur_styleval * (cur_linewidth + 1) / 2;
    poly->radius = 0;
    poly->points = point;

    dx = x - fix_x;
    dy = y - fix_y;
    mag = sqrt((double) (dx * dx + dy * dy));
    init_angle = compute_angle((float) dx, (float) dy);

    /* now append cur_numsides points */
    for (i = 1; i < cur_numsides; i++) {
	angle = init_angle - 2.0 * M_PI * (float) i / (float) cur_numsides;
	if (angle < 0)
	    angle += 2.0 * M_PI;
	nx = fix_x + (int) (mag * cos((double) angle));
	ny = fix_y + (int) (mag * sin((double) angle));
	append_point(nx, ny, &point);
    }
    append_point(x, y, &point);

    draw_line(poly, PAINT);
    add_line(poly);
    regpoly_drawing_selected();
    draw_mousefun_canvas();
}
