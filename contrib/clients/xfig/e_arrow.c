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
#include "u_search.h"
#include "w_canvas.h"
#include "w_mousefun.h"

static          add_arrow_head();
static          delete_arrow_head();

arrow_head_selected()
{
    set_mousefun("add arrow", "delete arrow", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    init_searchproc_left(add_arrow_head);
    init_searchproc_middle(delete_arrow_head);
    canvas_leftbut_proc = point_search_left;
    canvas_middlebut_proc = point_search_middle;
    canvas_rightbut_proc = null_proc;
    set_cursor(&pick9_cursor);
}

static
add_arrow_head(obj, type, x, y, p, q)
    char           *obj;
    int             type, x, y;
    F_point        *p, *q;
{
    switch (type) {
    case O_POLYLINE:
	cur_l = (F_line *) obj;
	add_linearrow(cur_l, p, q);
	break;
    case O_SPLINE:
	cur_s = (F_spline *) obj;
	add_splinearrow(cur_s, p, q);
	break;
    case O_ARC:
	cur_a = (F_arc *) obj;
	/* dirty trick - arc point number is stored in p */
	add_arcarrow(cur_a, (int) p);
	break;
    }
}

static
delete_arrow_head(obj, type, x, y, p, q)
    char           *obj;
    int             type, x, y;
    F_point        *p, *q;
{
    switch (type) {
    case O_POLYLINE:
	cur_l = (F_line *) obj;
	delete_linearrow(cur_l, p, q);
	break;
    case O_SPLINE:
	cur_s = (F_spline *) obj;
	delete_splinearrow(cur_s, p, q);
	break;
    case O_ARC:
	cur_a = (F_arc *) obj;
	/* dirty trick - arc point number is stored in p */
	delete_arcarrow(cur_a, (int) p);
	break;
    }
}

add_linearrow(line, prev_point, selected_point)
    F_line         *line;
    F_point        *prev_point, *selected_point;
{
    if (line->points->next == NULL)
	return;			/* A single point line */

    if (prev_point == NULL) {	/* selected_point is the first point */
	if (line->back_arrow)
	    return;
	line->back_arrow = backward_arrow();
	mask_toggle_linemarker(line);
	draw_arrow(selected_point->next->x, selected_point->next->y,
	     selected_point->x, selected_point->y, line->back_arrow, PAINT);
	mask_toggle_linemarker(line);
    } else if (selected_point->next == NULL) {	/* forward arrow */
	if (line->for_arrow)
	    return;
	line->for_arrow = forward_arrow();
	mask_toggle_linemarker(line);
	draw_arrow(prev_point->x, prev_point->y, selected_point->x,
		   selected_point->y, line->for_arrow, PAINT);
	mask_toggle_linemarker(line);
    } else
	return;
    clean_up();
    set_last_prevpoint(prev_point);
    set_last_selectedpoint(selected_point);
    set_latestline(line);
    set_action_object(F_ADD_ARROW_HEAD, O_POLYLINE);
    set_modifiedflag();
}

add_arcarrow(arc, point_num)
    F_arc          *arc;
    int             point_num;
{

    if (point_num == 0) {	/* backward arrow  */
	if (arc->back_arrow)
	    return;
	arc->back_arrow = backward_arrow();
	mask_toggle_arcmarker(arc);
	draw_arcarrows(arc, PAINT);
	mask_toggle_arcmarker(arc);
    } else if (point_num == 2) {/* for_arrow  */
	if (arc->for_arrow)
	    return;
	arc->for_arrow = forward_arrow();
	mask_toggle_arcmarker(arc);
	draw_arcarrows(arc, PAINT);
	mask_toggle_arcmarker(arc);
    } else
	return;
    clean_up();
    set_last_arcpointnum(point_num);
    set_latestarc(arc);
    set_action_object(F_ADD_ARROW_HEAD, O_ARC);
    set_modifiedflag();
}

add_splinearrow(spline, prev_point, selected_point)
    F_spline       *spline;
    F_point        *prev_point, *selected_point;
{
    F_point        *p;
    F_control      *c;

    if (prev_point == NULL) {	/* add backward arrow */
	if (spline->back_arrow)
	    return;
	p = selected_point->next;
	spline->back_arrow = backward_arrow();
	mask_toggle_splinemarker(spline);
	if (normal_spline(spline)) {
	    draw_arrow(p->x, p->y, selected_point->x,
		       selected_point->y, spline->back_arrow, PAINT);
	} else {
	    c = spline->controls;
	    draw_arrow(round(c->rx), round(c->ry), selected_point->x,
		       selected_point->y, spline->back_arrow, PAINT);
	}
	mask_toggle_splinemarker(spline);
    } else if (selected_point->next == NULL) {	/* add forward arrow */
	if (spline->for_arrow)
	    return;
	spline->for_arrow = forward_arrow();
	mask_toggle_splinemarker(spline);
	if (normal_spline(spline)) {
	    draw_arrow(prev_point->x, prev_point->y,
		       selected_point->x, selected_point->y,
		       spline->for_arrow, PAINT);
	} else {
	    for (c = spline->controls; c->next != NULL; c = c->next);
	    draw_arrow(round(c->lx), round(c->ly), selected_point->x,
		       selected_point->y, spline->for_arrow, PAINT);
	}
	mask_toggle_splinemarker(spline);
    }
    clean_up();
    set_last_prevpoint(prev_point);
    set_last_selectedpoint(selected_point);
    set_latestspline(spline);
    set_action_object(F_ADD_ARROW_HEAD, O_SPLINE);
    set_modifiedflag();
}

delete_linearrow(line, prev_point, selected_point)
    F_line         *line;
    F_point        *prev_point, *selected_point;
{
    if (line->points->next == NULL)
	return;			/* A single point line */

    if (prev_point == NULL) {	/* selected_point is the first point */
	if (!line->back_arrow)
	    return;
	mask_toggle_linemarker(line);
	draw_arrow(selected_point->next->x, selected_point->next->y,
	     selected_point->x, selected_point->y, line->back_arrow, ERASE);
	free((char *) line->back_arrow);
	line->back_arrow = NULL;
	draw_line(line, PAINT);
	mask_toggle_linemarker(line);
    } else if (selected_point->next == NULL) {	/* forward arrow */
	if (!line->for_arrow)
	    return;
	mask_toggle_linemarker(line);
	draw_arrow(prev_point->x, prev_point->y, selected_point->x,
		   selected_point->y, line->for_arrow, ERASE);
	free((char *) line->for_arrow);
	line->for_arrow = NULL;
	draw_line(line, PAINT);
	mask_toggle_linemarker(line);
    } else
	return;
    clean_up();
    set_last_prevpoint(prev_point);
    set_last_selectedpoint(selected_point);
    set_latestline(line);
    set_action_object(F_DELETE_ARROW_HEAD, O_POLYLINE);
    set_modifiedflag();
}

delete_arcarrow(arc, point_num)
    F_arc          *arc;
    int             point_num;
{
    if (point_num == 0) {	/* backward arrow  */
	if (!arc->back_arrow)
	    return;
	mask_toggle_arcmarker(arc);
	draw_arcarrows(arc, ERASE);
	free((char *) arc->back_arrow);
	arc->back_arrow = NULL;
	draw_arc(arc, PAINT);
	mask_toggle_arcmarker(arc);
    } else if (point_num == 2) {/* for_arrow  */
	if (!arc->for_arrow)
	    return;
	mask_toggle_arcmarker(arc);
	draw_arcarrows(arc, ERASE);
	free((char *) arc->for_arrow);
	arc->for_arrow = NULL;
	draw_arc(arc, PAINT);
	mask_toggle_arcmarker(arc);
    } else
	return;
    clean_up();
    set_last_arcpointnum(point_num);
    set_latestarc(arc);
    set_action_object(F_DELETE_ARROW_HEAD, O_ARC);
    set_modifiedflag();
}

delete_splinearrow(spline, prev_point, selected_point)
    F_spline       *spline;
    F_point        *prev_point, *selected_point;
{
    F_point        *p;

    if (closed_spline(spline))
	return;
    if (prev_point == NULL) {	/* selected_point is the first point */
	if (!spline->back_arrow)
	    return;
	mask_toggle_splinemarker(spline);
	p = selected_point->next;
	if (normal_spline(spline)) {
	    draw_arrow(p->x, p->y, selected_point->x,
		       selected_point->y, spline->back_arrow, ERASE);
	    free((char *) spline->back_arrow);
	    spline->back_arrow = NULL;
	    draw_spline(spline, PAINT);
	} else {
	    F_control      *a, *b;

	    a = spline->controls;
	    b = a->next;
	    draw_arrow(round(a->rx), round(a->ry), selected_point->x,
		       selected_point->y, spline->back_arrow, ERASE);
	    free((char *) spline->back_arrow);
	    spline->back_arrow = NULL;
	    draw_spline(spline, PAINT);
	}
	mask_toggle_splinemarker(spline);
    } else if (selected_point->next == NULL) {	/* forward arrow */
	if (!spline->for_arrow)
	    return;
	mask_toggle_splinemarker(spline);
	if (normal_spline(spline)) {
	    draw_arrow(prev_point->x, prev_point->y,
		       selected_point->x, selected_point->y,
		       spline->for_arrow, ERASE);
	    free((char *) spline->for_arrow);
	    spline->for_arrow = NULL;
	    draw_spline(spline, PAINT);
	} else {
	    F_control      *a, *b;

	    a = spline->controls;
	    for (b = a->next; b->next != NULL; a = b, b = b->next);
	    draw_arrow(round(b->lx), round(b->ly), selected_point->x,
		       selected_point->y, spline->for_arrow, ERASE);
	    free((char *) spline->for_arrow);
	    spline->for_arrow = NULL;
	    draw_spline(spline, PAINT);
	}
	mask_toggle_splinemarker(spline);
    } else
	return;
    clean_up();
    set_last_prevpoint(prev_point);
    set_last_selectedpoint(selected_point);
    set_latestspline(spline);
    set_action_object(F_DELETE_ARROW_HEAD, O_SPLINE);
    set_modifiedflag();
}
