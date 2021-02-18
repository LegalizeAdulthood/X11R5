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
#include "u_search.h"
#include "w_canvas.h"
#include "w_mousefun.h"
extern void	force_positioning();
extern void	force_nopositioning();

static int      init_point_adding();
static int      fix_linepoint_adding();
static int      mm_fix_linepoint_adding();
static int      fix_splinepoint_adding();
static int	init_splinepointadding();
static int	init_linepointadding();
static int	find_endpoints();

point_adding_selected()
{
    set_mousefun("break/add here", "", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    init_searchproc_left(init_point_adding);
    canvas_leftbut_proc = object_search_left;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(&pick9_cursor);
    force_nopositioning();
    constrained = MOVE_ARB;
}

static int
init_point_adding(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y;
    int             px, py;
{
    switch (type) {
    case O_POLYLINE:
	cur_l = (F_line *) p;
	/* the search routine will ensure that we don't have a box */
	init_linepointadding(px, py);
	break;
    case O_SPLINE:
	cur_s = (F_spline *) p;
	init_splinepointadding(px, py);
	break;
    default:
	return;
    }
    force_positioning();
}

static
wrapup_pointadding()
{
    reset_action_on();
    point_adding_selected();
    draw_mousefun_canvas();
}

static
cancel_pointadding()
{
    elastic_linelink();
    wrapup_pointadding();
}

static
cancel_line_pointadding()
{
    if (left_point != NULL && right_point != NULL)
	pw_vector(canvas_win, left_point->x, left_point->y,
		  right_point->x, right_point->y, INV_PAINT,
		  cur_l->thickness, cur_l->style, cur_l->style_val);
    cancel_pointadding();
}

/**************************  spline  *******************************/

static int
init_splinepointadding(px, py)
    int             px, py;
{
    set_action_on();
    set_mousefun("place new point", "", "cancel");
    draw_mousefun_canvas();
    find_endpoints(cur_s->points, px, py, &left_point, &right_point);
    set_temp_cursor(&null_cursor);
    cur_x = px;
    cur_y = py;
    if (left_point == NULL && closed_spline(cur_s)) {
	/* The added_point is between the 1st and 2nd point. */
	left_point = right_point;
	right_point = right_point->next;
    }
    elastic_linelink();
    canvas_locmove_proc = extending_line;
    canvas_leftbut_proc = fix_splinepoint_adding;
    canvas_rightbut_proc = cancel_pointadding;;
}

static
fix_splinepoint_adding(x, y)
    int             x, y;
{
    F_point        *p;

    if ((p = create_point()) == NULL) {
	wrapup_pointadding();
	return;
    }
    p->x = x;
    p->y = y;
    elastic_linelink();
    splinepoint_adding(cur_s, left_point, p, right_point);
    wrapup_pointadding();
}

/*
 * Added_point is always inserted between left_point and
 * right_point, except in two cases. (1) left_point is NULL, the added_point
 * will be prepended to the list of points. This case will never occur if the
 * spline is closed (periodic). (2) right_point is NULL, the added_point will
 * be appended to the end of the list.
 */

splinepoint_adding(spline, left_point, added_point, right_point)
    F_spline       *spline;
    F_point        *left_point, *added_point, *right_point;
{
    F_control      *c;

    if (int_spline(spline)) {	/* Interpolated spline */
	if ((c = create_cpoint()) == NULL)
	    return;
    }
    set_temp_cursor(&wait_cursor);
    mask_toggle_splinemarker(spline);
    draw_spline(spline, ERASE);	/* erase old spline */
    if (left_point == NULL) {
	added_point->next = spline->points;
	spline->points = added_point;
    } else {
	added_point->next = right_point;
	left_point->next = added_point;
    }

    if (int_spline(spline)) {	/* Interpolated spline */
	c->next = spline->controls;
	spline->controls = c;
	remake_control_points(spline);
    }
    draw_spline(spline, PAINT);	/* draw the modified spline */
    mask_toggle_splinemarker(spline);
    clean_up();
    set_modifiedflag();
    set_last_prevpoint(left_point);
    set_last_selectedpoint(added_point);
    set_action_object(F_ADD_POINT, O_SPLINE);
    set_latestspline(spline);
    reset_cursor();
}

/***************************  line  ********************************/

static int
init_linepointadding(px, py)
    int             px, py;
{
    set_action_on();
    set_mousefun("place new point", "", "cancel");
    draw_mousefun_canvas();
    find_endpoints(cur_l->points, px, py, &left_point, &right_point);
    set_temp_cursor(&null_cursor);
    win_setmouseposition(canvas_win, px, py);

    /* set cur_x etc at new point coords */
    cur_x = fix_x = px;
    cur_y = fix_y = py;
    if (left_point == NULL && cur_l->type == T_POLYGON) {
	left_point = right_point;
	right_point = right_point->next;
    }
    /* erase line segment where new point is */
    if (left_point != NULL && right_point != NULL)
	pw_vector(canvas_win, left_point->x, left_point->y,
		  right_point->x, right_point->y, ERASE,
		  cur_l->thickness, cur_l->style, cur_l->style_val);

    /* draw in rubber-band line */
    elastic_linelink();
    if (latexline_mode || latexarrow_mode) {
	canvas_locmove_proc = latex_line;
	canvas_leftbut_proc = mm_fix_linepoint_adding;
    }
    if ((mountain_mode || manhattan_mode) &&
	(left_point == NULL || right_point == NULL)) {
	canvas_locmove_proc = constrainedangle_line;
	canvas_leftbut_proc = mm_fix_linepoint_adding;
    } else {
	canvas_locmove_proc = reshaping_line;
	canvas_leftbut_proc = fix_linepoint_adding;
    }
    canvas_rightbut_proc = cancel_line_pointadding;;
}

static
fix_linepoint_adding(x, y)
    int             x, y;
{
    F_point        *p;

    if ((p = create_point()) == NULL) {
	wrapup_pointadding();
	return;
    }
    p->x = x;
    p->y = y;
    elastic_linelink();
    linepoint_adding(cur_l, left_point, p, right_point);
    wrapup_pointadding();
}

static
mm_fix_linepoint_adding()
{
    F_point        *p;

    if ((p = create_point()) == NULL) {
	wrapup_pointadding();
	return;
    }
    p->x = cur_x;
    p->y = cur_y;
    elastic_linelink();
    linepoint_adding(cur_l, left_point, p, right_point);
    wrapup_pointadding();
}

linepoint_adding(line, left_point, added_point, right_point)
    F_line         *line;
    F_point        *left_point, *added_point, *right_point;
{
    mask_toggle_linemarker(line);
    draw_line(line, ERASE);
    if (left_point == NULL) {
	added_point->next = line->points;
	line->points = added_point;
    } else {
	added_point->next = left_point->next;
	left_point->next = added_point;
    }
    draw_line(line, PAINT);
    clean_up();
    mask_toggle_linemarker(line);
    set_action_object(F_ADD_POINT, O_POLYLINE);
    set_latestline(line);
    set_last_prevpoint(left_point);
    set_last_selectedpoint(added_point);
    set_modifiedflag();
}

/*******************************************************************/

/*
 * If (x,y) is close to a point, q, fp points to q and sp points to q->next
 * (right).  However if q is the first point, fp contains NULL and sp points
 * to q.
 */

static int
find_endpoints(p, x, y, fp, sp)
    F_point        *p, **fp, **sp;
    int             x, y;
{
    int             d;
    F_point        *a = NULL, *b = p;

    if (x == b->x && y == b->y) {
	*fp = a;
	*sp = b;
	return;
    }
    for (a = p, b = p->next; b != NULL; a = b, b = b->next) {
	if (x == b->x && y == b->y) {
	    *fp = b;
	    *sp = b->next;
	    return;
	}
	if (close_to_vector(a->x, a->y, b->x, b->y, x, y, 1, 1.0, &d, &d)) {
	    *fp = a;
	    *sp = b;
	    return;
	}
    }
    *fp = a;
    *sp = b;
}
