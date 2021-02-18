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

static int      create_intsplineobject();
static int      init_intspline_drawing();

intspline_drawing_selected()
{
    set_mousefun("first point", "", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_leftbut_proc = init_intspline_drawing;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(&arrow_cursor);
    reset_action_on();
}

static
init_intspline_drawing(x, y)
    int             x, y;
{
    min_num_points = 3;
    init_trace_drawing(x, y);
    canvas_middlebut_save = create_intsplineobject;
}

static
create_intsplineobject(x, y)
    int             x, y;
{
    F_spline       *spline;

    if (x != fix_x || y != fix_y) {
	if (manhattan_mode || mountain_mode)
	    get_direction(x, y);
	else if (latexline_mode || latexarrow_mode)
	    get_latexpoint(x, y);
	else
	    get_intermediatepoint(x, y);
    }
    elastic_line();
    if ((spline = create_spline()) == NULL) {
	if (num_point == 1) {
	    free((char *) cur_point);
	    cur_point = NULL;
	}
	free((char *) first_point);
	first_point = NULL;
	return;
    }
    spline->style = cur_linestyle;
    spline->thickness = cur_linewidth;
    spline->style_val = cur_styleval * (cur_linewidth + 1) / 2;
    spline->color = cur_color;
    spline->depth = 0;
    spline->area_fill = cur_areafill;
    /*
     * The current area fill color is saved in all intspline objects (but
     * support for filling may not be available in all fig2dev languages).
     * Currently, splines are drawn in overlapping sections which make
     * drawing the area fill difficult, so no filling is done on screen.
     */
    spline->pen = 0;
    spline->points = first_point;
    spline->controls = NULL;
    spline->next = NULL;
    cur_x = cur_y = fix_x = fix_y = 0;	/* used in elastic_moveline */
    elastic_moveline(spline->points);	/* erase control vector */
    /* initialize to no arrows - changed below if necessary */
    spline->for_arrow = NULL;
    spline->back_arrow = NULL;
    if (cur_mode == F_CLOSED_INTSPLINE) {
	spline->type = T_CLOSED_INTERP;
	num_point++;
	append_point(first_point->x, first_point->y, &cur_point);
    } else {
	spline->type = T_OPEN_INTERP;
	if (autoforwardarrow_mode)
	    spline->for_arrow = forward_arrow();
	if (autobackwardarrow_mode)
	    spline->back_arrow = backward_arrow();
    }
    make_control_points(spline);
    draw_intspline(spline, PAINT);
    if (appres.DEBUG) {
	int             xmin, ymin, xmax, ymax;

	spline_bound(spline, &xmin, &ymin, &xmax, &ymax);
	elastic_box(xmin, ymin, xmax, ymax);
    }
    add_spline(spline);
    intspline_drawing_selected();
    draw_mousefun_canvas();
}

/* Tension : 0 (min) -> 1 (max)	 */

create_control_list(s)
    F_spline       *s;
{
    F_point        *p;
    F_control      *cp;

    if ((cp = create_cpoint()) == NULL)
	return (-1);

    s->controls = cp;
    for (p = s->points->next; p != NULL; p = p->next) {
	if ((cp->next = create_cpoint()) == NULL)
	    return (-1);
	cp = cp->next;
    }
    cp->next = NULL;
    return (1);
}

make_control_points(s)
    F_spline       *s;
{
    if (-1 == create_control_list(s))
	return;

    remake_control_points(s);
}

remake_control_points(s)
    F_spline       *s;
{
    if (s->type == T_CLOSED_INTERP)
	compute_cp(s->points, s->controls, CLOSED_PATH);
    else
	compute_cp(s->points, s->controls, OPEN_PATH);
}

#define		T		0.45
#define		_2xPI		6.2832
#define		_1dSQR2		0.7071
#define		_SQR2		1.4142

compute_cp(points, controls, path)
    F_point        *points;
    F_control      *controls;
    int             path;
{
    F_control      *cp, *cpn;
    F_point        *p, *p2, *pk;/* Pk is the next-to-last point. */
    float           dx, dy;
    float           x1, y1, x2, y2, x3, y3;
    float           l1, l2, theta1, theta2;

    x1 = points->x;
    y1 = points->y;
    pk = p2 = points->next;
    x2 = p2->x;
    y2 = p2->y;
    p = p2->next;
    x3 = p->x;
    y3 = p->y;

    dx = x1 - x2;
    dy = y2 - y1;
    l1 = sqrt((double) (dx * dx + dy * dy));
    if (l1 == 0.0)
	theta1 = 0.0;
    else
	theta1 = atan2((double) dy, (double) dx);
    dx = x3 - x2;
    dy = y2 - y3;
    l2 = sqrt((double) (dx * dx + dy * dy));
    if (l2 == 0.0)
	theta2 = 0.0;
    else
	theta2 = atan2((double) dy, (double) dx);
    /* -PI <= theta1, theta2 <= PI */
    if (theta1 < 0)
	theta1 += _2xPI;
    if (theta2 < 0)
	theta2 += _2xPI;
    /* 0 <= theta1, theta2 < 2PI */

    cp = controls->next;
    control_points(x2, y2, l1, l2, theta1, theta2, T, cp);
    /* control points for (x2, y2) */
    if (path == OPEN_PATH) {
	controls->lx = 0.0;
	controls->ly = 0.0;
	controls->rx = (x1 + 3 * cp->lx) / 4;
	controls->ry = (y1 + 3 * cp->ly) / 4;
	cp->lx = (3 * cp->lx + x2) / 4;
	cp->ly = (3 * cp->ly + y2) / 4;
    }
    while (1) {
	x2 = x3;
	y2 = y3;
	l1 = l2;
	if (theta2 >= M_PI)
	    theta1 = theta2 - M_PI;
	else
	    theta1 = theta2 + M_PI;
	if ((p = p->next) == NULL)
	    break;
	pk = pk->next;
	x3 = p->x;
	y3 = p->y;
	dx = x3 - x2;
	dy = y2 - y3;
	l2 = sqrt((double) (dx * dx + dy * dy));
	if (l2 == 0.0)
	    theta2 = 0.0;
	else
	    theta2 = atan2((double) dy, (double) dx);
	if (theta2 < 0)
	    theta2 += _2xPI;
	cp = cp->next;
	control_points(x2, y2, l1, l2, theta1, theta2, T, cp);
    };

    if (path == CLOSED_PATH) {
	dx = p2->x - x2;
	dy = y2 - p2->y;
	l2 = sqrt((double) (dx * dx + dy * dy));
	theta2 = atan2((double) dy, (double) dx);
	if (theta2 < 0)
	    theta2 += _2xPI;
	cp = cp->next;
	control_points(x2, y2, l1, l2, theta1, theta2, T, cp);
	controls->lx = cp->lx;
	controls->ly = cp->ly;
	controls->rx = cp->rx;
	controls->ry = cp->ry;
    } else {
	cpn = cp->next;
	cpn->lx = (3 * cp->rx + x2) / 4;
	cpn->ly = (3 * cp->ry + y2) / 4;
	cpn->rx = 0.0;
	cpn->ry = 0.0;
	cp->rx = (pk->x + 3 * cp->rx) / 4;
	cp->ry = (pk->y + 3 * cp->ry) / 4;
    }
}

/*
 * The parameter t is the tension.  It must range in [0, 1]. The bigger the
 * value of t, the higher the tension.
 */

control_points(x, y, l1, l2, theta1, theta2, t, cp)
    float           x, y, l1, l2, theta1, theta2, t;
    F_control      *cp;
{
    float           s, theta, r = 1 - t;

    /* 0 <= theta1, theta2 < 2PI */

    theta = (theta1 + theta2) / 2;

    if (theta1 > theta2) {
	s = sin((double) (theta - theta2));
	theta1 = theta + M_PI_2;
	theta2 = theta - M_PI_2;
    } else {
	s = sin((double) (theta2 - theta));
	theta1 = theta - M_PI_2;
	theta2 = theta + M_PI_2;
    }
    if (s > _1dSQR2)
	s = _SQR2 - s;
    s *= r;
    l1 *= s;
    l2 *= s;
    cp->lx = x + l1 * cos((double) theta1);
    cp->ly = y - l1 * sin((double) theta1);
    cp->rx = x + l2 * cos((double) theta2);
    cp->ry = y - l2 * sin((double) theta2);
}
