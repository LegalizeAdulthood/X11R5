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
#include "u_search.h"
#include "w_canvas.h"
#include "w_mousefun.h"

static int      init_delete_point();

delete_point_selected()
{
    set_mousefun("delete point", "", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    init_searchproc_left(init_delete_point);
    canvas_leftbut_proc = point_search_left;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(&pick9_cursor);
}

static
init_delete_point(obj, type, x, y, p, q)
    char           *obj;
    int             type, x, y;
    F_point        *p, *q;
{
    int             n;

    switch (type) {
    case O_POLYLINE:
	cur_l = (F_line *) obj;
	/* the search routine will ensure we don't have a box */
	n = num_points(cur_l->points);
	if (cur_l->type == T_POLYGON) {
	    if (n <= 4) {	/* count first pt twice for closed object */
		put_msg("A polygon cannot have less than 3 points");
		return;
	    }
	} else if (n <= 1) {
	    /* alternative would be to remove the dot altogether */
	    put_msg("A dot must have at least 1 point");
	    return;
	}
	linepoint_deleting(cur_l, p, q);
	break;
    case O_SPLINE:
	cur_s = (F_spline *) obj;
	n = num_points(cur_s->points);
	if (closed_spline(cur_s)) {
	    if (n <= 4) {	/* count first pt twice for closed object */
		put_msg("A closed spline cannot have less than 3 points");
		return;
	    }
	} else if (normal_spline(cur_s)) {
	    if (n <= 2) {
		put_msg("A spline cannot have less than 2 points");
		return;
	    }
	} else if (n <= 3) {	/* it must be an open interpolated spline */
	    put_msg("An interpolated spline cannot have less than 3 points");
	    return;
	}
	splinepoint_deleting(cur_s, p, q);
	break;
    default:
	return;
    }
}

/**************************  spline  *******************************/

splinepoint_deleting(spline, prev_point, selected_point)
    F_spline       *spline;
    F_point        *prev_point, *selected_point;
{
    F_point        *p, *next_point;

    next_point = selected_point->next;
    set_temp_cursor(&wait_cursor);
    if (closed_spline(spline)) {
	mask_toggle_splinemarker(spline);
	draw_spline(spline, ERASE);	/* erase the spline */
	if (prev_point == NULL) {
	    /* The deleted point is the first point */
	    spline->points = next_point;
	    for (prev_point = next_point, p = prev_point->next;
		 p->next != NULL;
		 prev_point = p, p = p->next);
	    /*
	     * prev_point now points at next to last point (the last point is
	     * a copy of the first).
	     */
	    p->x = spline->points->x;
	    p->y = spline->points->y;
	    next_point = p;
	    /*
	     * next_point becomes the last point.  If this operation (point
	     * deletion) is reversed (undo), the selected_point will not be
	     * inserted into it original place, but will be between
	     * prev_point and next_point.
	     */
	} else
	    prev_point->next = next_point;
    } else {			/* open spline */
	mask_toggle_splinemarker(spline);
	draw_spline(spline, ERASE);	/* erase the spline */
	if (prev_point == NULL)
	    spline->points = next_point;
	else
	    prev_point->next = next_point;
    }
    if (int_spline(spline)) {
	F_control      *c;

	c = spline->controls;
	spline->controls = c->next;
	c->next = NULL;
	free((char *) c);
	remake_control_points(spline);
    }
    draw_spline(spline, PAINT);
    mask_toggle_splinemarker(spline);
    clean_up();
    set_action_object(F_DELETE_POINT, O_SPLINE);
    set_latestspline(spline);
    set_last_prevpoint(prev_point);
    set_last_selectedpoint(selected_point);
    set_last_nextpoint(next_point);
    set_modifiedflag();
    reset_cursor();
}

/***************************  line  ********************************/

/*
 * In deleting a point selected_point, linepoint_deleting uses prev_point and
 * next_point of the point. The relationship between the three points is:
 * prev_point->selected_point->next_point except when selected_point is the
 * first point in the list, in which case prev_point will be NULL.
 */
linepoint_deleting(line, prev_point, selected_point)
    F_line         *line;
    F_point        *prev_point, *selected_point;
{
    F_point        *p, *next_point;

    next_point = selected_point->next;
    mask_toggle_linemarker(line);
    draw_line(line, ERASE);	/* erase the line */
    if (line->type == T_POLYGON) {
	if (prev_point == NULL) {
	    /* The deleted point is the first point */
	    line->points = next_point;
	    for (prev_point = next_point, p = prev_point->next;
		 p->next != NULL;
		 prev_point = p, p = p->next);
	    /*
	     * prev_point now points at next to last point (the last point is
	     * a copy of the first).
	     */
	    p->x = next_point->x;
	    p->y = next_point->y;
	    next_point = p;
	    /*
	     * next_point becomes the last point.  If this operation (point
	     * deletion) is reversed (undo), the selected_point will not be
	     * inserted into it original place, but will be between
	     * prev_point and next_point.
	     */
	} else
	    prev_point->next = next_point;
    } else {			/* polyline */
	if (prev_point == NULL)
	    line->points = next_point;
	else
	    prev_point->next = next_point;
    }
    draw_line(line, PAINT);
    mask_toggle_linemarker(line);
    clean_up();
    set_modifiedflag();
    set_action_object(F_DELETE_POINT, O_POLYLINE);
    set_latestline(line);
    set_last_prevpoint(prev_point);
    set_last_selectedpoint(selected_point);
    set_last_nextpoint(next_point);
}
