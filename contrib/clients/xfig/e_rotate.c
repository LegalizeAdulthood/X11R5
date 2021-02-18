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
#include "u_draw.h"
#include "u_search.h"
#include "u_create.h"
#include "u_list.h"
#include "w_canvas.h"
#include "w_mousefun.h"

extern float    compute_angle();

static int      copy;
static int      rotn_dirn;
static int      init_rotate();
static int      init_copynrotate();
static int      rotate_selected();
static int      rotate_search();
static int      init_rotateline();

rotate_cw_selected()
{
    rotn_dirn = 1;
    rotate_selected();
}

rotate_ccw_selected()
{
    rotn_dirn = -1;
    rotate_selected();
}

static
rotate_selected()
{
    set_mousefun("rotate object", "copy & rotate", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    init_searchproc_left(init_rotate);
    init_searchproc_middle(init_copynrotate);
    canvas_leftbut_proc = object_search_left;
    canvas_middlebut_proc = object_search_middle;
    canvas_rightbut_proc = null_proc;
    set_cursor(&pick15_cursor);
}

static
init_rotate(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y;
    int             px, py;
{
    copy = 0;
    rotate_search(p, type, x, y, px, py);
}

static
init_copynrotate(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y;
    int             px, py;
{
    copy = 1;
    rotate_search(p, type, x, y, px, py);
}

static
rotate_search(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y;
    int             px, py;
{
    switch (type) {
    case O_POLYLINE:
	cur_l = (F_line *) p;
	init_rotateline(cur_l, px, py);
	break;
    case O_ARC:
	cur_a = (F_arc *) p;
	init_rotatearc(cur_a, px, py);
	break;
    case O_ELLIPSE:
	cur_e = (F_ellipse *) p;
	init_rotateellipse(cur_e, px, py);
	break;
    case O_SPLINE:
	cur_s = (F_spline *) p;
	init_rotatespline(cur_s, px, py);
	break;
    case O_COMPOUND:
	cur_c = (F_compound *) p;
	init_rotatecompound(cur_c, px, py);
	break;
    default:
	return;
    }
}

static
init_rotateline(l, px, py)
    F_line         *l;
    int             px, py;
{
    F_line         *line;

    set_temp_cursor(&wait_cursor);
    line = copy_line(l);
    rotate_line(line, px, py, rotn_dirn);
    if (copy) {
	add_line(line);
    } else {
	toggle_linemarker(l);
	draw_line(l, ERASE);
	change_line(l, line);
    }
    draw_line(line, PAINT);
    toggle_linemarker(line);
    reset_cursor();
}

init_rotateellipse(e, px, py)
    F_ellipse      *e;
    int             px, py;
{
    F_ellipse      *ellipse;

    set_temp_cursor(&wait_cursor);
    ellipse = copy_ellipse(e);
    rotate_ellipse(ellipse, px, py, rotn_dirn);
    if (copy) {
	add_ellipse(ellipse);
    } else {
	toggle_ellipsemarker(e);
	draw_ellipse(e, ERASE);
	change_ellipse(e, ellipse);
    }
    draw_ellipse(ellipse, PAINT);
    toggle_ellipsemarker(ellipse);
    reset_cursor();
}

init_rotatearc(a, px, py)
    F_arc          *a;
    int             px, py;
{
    F_arc          *arc;

    set_temp_cursor(&wait_cursor);
    arc = copy_arc(a);
    rotate_arc(arc, px, py, rotn_dirn);
    if (copy) {
	add_arc(arc);
    } else {
	toggle_arcmarker(a);
	draw_arc(a, ERASE);
	change_arc(a, arc);
    }
    draw_arc(arc, PAINT);
    toggle_arcmarker(arc);
    reset_cursor();
}

init_rotatespline(s, px, py)
    F_spline       *s;
    int             px, py;
{
    F_spline       *spline;

    set_temp_cursor(&wait_cursor);
    spline = copy_spline(s);
    rotate_spline(spline, px, py, rotn_dirn);
    if (copy) {
	add_spline(spline);
    } else {
	toggle_splinemarker(s);
	draw_spline(s, ERASE);
	change_spline(s, spline);
    }
    draw_spline(spline, PAINT);
    toggle_splinemarker(spline);
    reset_cursor();
}

init_rotatecompound(c, px, py)
    F_compound     *c;
    int             px, py;
{
    F_compound     *compound;

    set_temp_cursor(&wait_cursor);
    compound = copy_compound(c);
    rotate_compound(compound, px, py, rotn_dirn);
    if (copy) {
	add_compound(compound);
    } else {
	toggle_compoundmarker(c);
	draw_compoundelements(c, ERASE);
	change_compound(c, compound);
    }
    draw_compoundelements(compound, PAINT);
    toggle_compoundmarker(compound);
    reset_cursor();
}

rotate_line(l, x, y, rotn_dirn)
    F_line         *l;
    int             x, y, rotn_dirn;
{
    F_point        *p;
    int             dx;

    /* for speed we treat 90 degrees as a special case */
    if (cur_rotnangle == 90) {
	for (p = l->points; p != NULL; p = p->next) {
	    dx = p->x - x;
	    p->x = x + rotn_dirn * (y - p->y);
	    p->y = y + rotn_dirn * dx;
	}
    } else {
	for (p = l->points; p != NULL; p = p->next)
	    rotate_point(p, x, y, rotn_dirn);
    }
}

rotate_spline(s, x, y, rotn_dirn)
    F_spline       *s;
    int             x, y, rotn_dirn;
{
    F_point        *p;
    F_control      *cp;
    int             dx;

    /* for speed we treat 90 degrees as a special case */
    if (cur_rotnangle == 90) {
	for (p = s->points; p != NULL; p = p->next) {
	    dx = p->x - x;
	    p->x = x + rotn_dirn * (y - p->y);
	    p->y = y + rotn_dirn * dx;
	}
	for (cp = s->controls; cp != NULL; cp = cp->next) {
	    dx = cp->lx - x;
	    cp->lx = x + rotn_dirn * (y - cp->ly);
	    cp->ly = y + rotn_dirn * dx;
	    dx = cp->rx - x;
	    cp->rx = x + rotn_dirn * (y - cp->ry);
	    cp->ry = y + rotn_dirn * dx;
	}
    } else {
	for (p = s->points; p != NULL; p = p->next)
	    rotate_point(p, x, y, rotn_dirn);
	if (int_spline(s))
	    remake_control_points(s);
    }
}

rotate_text(t, x, y, rotn_dirn)
    F_text         *t;
    int             x, y, rotn_dirn;
{
    int             dx;

    dx = t->base_x - x;
    t->base_x = x + rotn_dirn * (y - t->base_y);
    t->base_y = y + rotn_dirn * dx;
}

rotate_ellipse(e, x, y, rotn_dirn)
    F_ellipse      *e;
    int             x, y, rotn_dirn;
{
    int             dx;

    dx = e->center.x - x;
    e->center.x = x + rotn_dirn * (y - e->center.y);
    e->center.y = y + rotn_dirn * dx;
    dx = e->radiuses.x;
    e->radiuses.x = e->radiuses.y;
    e->radiuses.y = dx;
    dx = e->start.x - x;
    e->start.x = x + rotn_dirn * (y - e->start.y);
    e->start.y = y + rotn_dirn * dx;
    dx = e->end.x - x;
    e->end.x = x + rotn_dirn * (y - e->end.y);
    e->end.y = y + rotn_dirn * dx;
}

rotate_arc(a, x, y, rotn_dirn)
    F_arc          *a;
    int             x, y, rotn_dirn;
{
    int             dx;
    F_pos           p[3];
    float           xx, yy;

    /* for speed we treat 90 degrees as a special case */
    if (cur_rotnangle == 90) {
	dx = a->center.x - x;
	a->center.x = x + rotn_dirn * (y - a->center.y);
	a->center.y = y + rotn_dirn * dx;
	dx = a->point[0].x - x;
	a->point[0].x = x + rotn_dirn * (y - a->point[0].y);
	a->point[0].y = y + rotn_dirn * dx;
	dx = a->point[1].x - x;
	a->point[1].x = x + rotn_dirn * (y - a->point[1].y);
	a->point[1].y = y + rotn_dirn * dx;
	dx = a->point[2].x - x;
	a->point[2].x = x + rotn_dirn * (y - a->point[2].y);
	a->point[2].y = y + rotn_dirn * dx;
    } else {
	p[0] = a->point[0];
	p[1] = a->point[1];
	p[2] = a->point[2];
	rotate_point(&p[0], x, y, rotn_dirn);
	rotate_point(&p[1], x, y, rotn_dirn);
	rotate_point(&p[2], x, y, rotn_dirn);
	if (compute_arccenter(p[0], p[1], p[2], &xx, &yy)) {
	    a->point[0].x = p[0].x;
	    a->point[0].y = p[0].y;
	    a->point[1].x = p[1].x;
	    a->point[1].y = p[1].y;
	    a->point[2].x = p[2].x;
	    a->point[2].y = p[2].y;
	    a->center.x = xx;
	    a->center.y = yy;
	    a->direction = compute_direction(p[0], p[1], p[2]);
	}
    }
}

rotate_compound(c, x, y, rotn_dirn)
    F_compound     *c;
    int             x, y, rotn_dirn;
{
    F_line         *l;
    F_arc          *a;
    F_ellipse      *e;
    F_spline       *s;
    F_text         *t;
    F_compound     *c1;

    for (l = c->lines; l != NULL; l = l->next)
	rotate_line(l, x, y, rotn_dirn);
    for (a = c->arcs; a != NULL; a = a->next)
	rotate_arc(a, x, y, rotn_dirn);
    for (e = c->ellipses; e != NULL; e = e->next)
	rotate_ellipse(e, x, y, rotn_dirn);
    for (s = c->splines; s != NULL; s = s->next)
	rotate_spline(s, x, y, rotn_dirn);
    for (t = c->texts; t != NULL; t = t->next)
	rotate_text(t, x, y, rotn_dirn);
    for (c1 = c->compounds; c1 != NULL; c1 = c1->next)
	rotate_compound(c1, x, y, rotn_dirn);

    /*
     * Make the bounding box exactly match the dimensions of the compound.
     */
    compound_bound(c, &c->nwcorner.x, &c->nwcorner.y,
		   &c->secorner.x, &c->secorner.y);
}

rotate_point(p, x, y, rotn)
    F_point        *p;
    int             x, y, rotn;
{
    /* rotate point p about coordinate (x, y) */
    int             dx, dy;
    float           cosa, sina, mag, theta;

    dx = p->x - x;
    dy = y - p->y;
    if (dx == 0 && dy == 0)
	return;

    theta = compute_angle((float) dx, (float) dy);
    theta -= (float) (rotn_dirn * cur_rotnangle * M_PI / 180);
    if (theta < 0)
	theta += 2 * M_PI;
    else if (theta >= 2 * M_PI)
	theta -= 2 * M_PI;
    mag = sqrt((double) (dx * dx + dy * dy));
    cosa = mag * cos((double) theta);
    sina = mag * sin((double) theta);
    p->x = x + cosa;
    p->y = y - sina;
}
