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

static int      flip_axis;
static int      copy;
static int      init_flip();
static int      init_copynflip();
static int      init_fliparc();
static int      init_flipcompound();
static int      init_flipellipse();
static int      init_flipline();
static int      init_flipspline();
static int      flip_selected();
static int      flip_search();

flip_ud_selected()
{
    flip_axis = UD_FLIP;
    flip_selected();
}

flip_lr_selected()
{
    flip_axis = LR_FLIP;
    flip_selected();
}

static
flip_selected()
{
    set_mousefun("flip", "copy & flip", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    init_searchproc_left(init_flip);
    init_searchproc_middle(init_copynflip);
    canvas_leftbut_proc = object_search_left;
    canvas_middlebut_proc = object_search_middle;
    canvas_rightbut_proc = null_proc;
    set_cursor(&pick15_cursor);
}

static
init_flip(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y;
    int             px, py;
{
    copy = 0;
    flip_search(p, type, x, y, px, py);
}

static
init_copynflip(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y;
    int             px, py;
{
    copy = 1;
    flip_search(p, type, x, y, px, py);
}

static
flip_search(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y;
    int             px, py;
{
    switch (type) {
    case O_POLYLINE:
	cur_l = (F_line *) p;
	init_flipline(cur_l, px, py);
	break;
    case O_ARC:
	cur_a = (F_arc *) p;
	init_fliparc(cur_a, px, py);
	break;
    case O_ELLIPSE:
	cur_e = (F_ellipse *) p;
	init_flipellipse(cur_e, px, py);
	break;
    case O_SPLINE:
	cur_s = (F_spline *) p;
	init_flipspline(cur_s, px, py);
	break;
    case O_COMPOUND:
	cur_c = (F_compound *) p;
	init_flipcompound(cur_c, px, py);
	break;
    default:
	return;
    }
}

static
init_fliparc(a, px, py)
    F_arc          *a;
    int             px, py;
{
    F_arc          *arc;

    set_temp_cursor(&wait_cursor);
    arc = copy_arc(a);
    flip_arc(arc, px, py, flip_axis);
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

static
init_flipcompound(c, px, py)
    F_compound     *c;
    int             px, py;
{
    F_compound     *compound;

    set_temp_cursor(&wait_cursor);
    compound = copy_compound(c);
    flip_compound(compound, px, py, flip_axis);
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

static
init_flipellipse(old_e, px, py)
    F_ellipse      *old_e;
{
    F_ellipse      *new_e;

    new_e = copy_ellipse(old_e);
    flip_ellipse(new_e, px, py, flip_axis);
    if (copy) {
	add_ellipse(new_e);
    } else {
	toggle_ellipsemarker(old_e);
	draw_ellipse(old_e, ERASE);
	change_ellipse(old_e, new_e);
    }
    draw_ellipse(new_e, PAINT);
    toggle_ellipsemarker(new_e);
}

static
init_flipline(old_l, px, py)
    F_line         *old_l;
    int             px, py;
{
    F_line         *new_l;

    new_l = copy_line(old_l);
    flip_line(new_l, px, py, flip_axis);
    if (copy) {
	add_line(new_l);
    } else {
	toggle_linemarker(old_l);
	draw_line(old_l, ERASE);
	change_line(old_l, new_l);
    }
    draw_line(new_l, PAINT);
    toggle_linemarker(new_l);
}

static
init_flipspline(old_s, px, py)
    F_spline       *old_s;
    int             px, py;
{
    F_spline       *new_s;

    new_s = copy_spline(old_s);
    flip_spline(new_s, px, py, flip_axis);
    if (copy) {
	add_spline(new_s);
    } else {
	toggle_splinemarker(old_s);
	draw_spline(old_s, ERASE);
	change_spline(old_s, new_s);
    }
    draw_spline(new_s, PAINT);
    toggle_splinemarker(new_s);
}

flip_line(l, x, y, flip_axis)
    F_line         *l;
    int             x, y, flip_axis;
{
    F_point        *p;

    switch (flip_axis) {
    case UD_FLIP:		/* x axis  */
	for (p = l->points; p != NULL; p = p->next)
	    p->y = y + (y - p->y);
	break;
    case LR_FLIP:		/* y axis  */
	for (p = l->points; p != NULL; p = p->next)
	    p->x = x + (x - p->x);
	break;
    }
    if (l->type == T_EPS_BOX)
	l->eps->flipped = 1 - l->eps->flipped;
}

flip_spline(s, x, y, flip_axis)
    F_spline       *s;
    int             x, y, flip_axis;
{
    F_point        *p;
    F_control      *cp;

    switch (flip_axis) {
    case UD_FLIP:		/* x axis  */
	for (p = s->points; p != NULL; p = p->next)
	    p->y = y + (y - p->y);
	for (cp = s->controls; cp != NULL; cp = cp->next) {
	    cp->ly = y + (y - cp->ly);
	    cp->ry = y + (y - cp->ry);
	}
	break;
    case LR_FLIP:		/* y axis  */
	for (p = s->points; p != NULL; p = p->next)
	    p->x = x + (x - p->x);
	for (cp = s->controls; cp != NULL; cp = cp->next) {
	    cp->lx = x + (x - cp->lx);
	    cp->rx = x + (x - cp->rx);
	}
	break;
    }
}

flip_text(t, x, y, flip_axis)
    F_text         *t;
    int             x, y, flip_axis;
{
    switch (flip_axis) {
    case UD_FLIP:		/* x axis  */
	t->base_y = y + (y - t->base_y);
	break;
    case LR_FLIP:		/* y axis  */
	t->base_x = x + (x - t->base_x);
	break;
    }
}

flip_ellipse(e, x, y, flip_axis)
    F_ellipse      *e;
    int             x, y, flip_axis;
{
    switch (flip_axis) {
    case UD_FLIP:		/* x axis  */
	e->direction ^= 1;
	e->center.y = y + (y - e->center.y);
	e->start.y = y + (y - e->start.y);
	e->end.y = y + (y - e->end.y);
	break;
    case LR_FLIP:		/* y axis  */
	e->direction ^= 1;
	e->center.x = x + (x - e->center.x);
	e->start.x = x + (x - e->start.x);
	e->end.x = x + (x - e->end.x);
	break;
    }
}

flip_arc(a, x, y, flip_axis)
    F_arc          *a;
    int             x, y, flip_axis;
{
    switch (flip_axis) {
    case UD_FLIP:		/* x axis  */
	a->direction ^= 1;
	a->center.y = y + (y - a->center.y);
	a->point[0].y = y + (y - a->point[0].y);
	a->point[1].y = y + (y - a->point[1].y);
	a->point[2].y = y + (y - a->point[2].y);
	break;
    case LR_FLIP:		/* y axis  */
	a->direction ^= 1;
	a->center.x = x + (x - a->center.x);
	a->point[0].x = x + (x - a->point[0].x);
	a->point[1].x = x + (x - a->point[1].x);
	a->point[2].x = x + (x - a->point[2].x);
	break;
    }
}

flip_compound(c, x, y, flip_axis)
    F_compound     *c;
    int             x, y, flip_axis;
{
    F_line         *l;
    F_arc          *a;
    F_ellipse      *e;
    F_spline       *s;
    F_text         *t;
    F_compound     *c1;
    int             p, q;

    switch (flip_axis) {
    case UD_FLIP:		/* x axis  */
	p = y + (y - c->nwcorner.y);
	q = y + (y - c->secorner.y);
	c->nwcorner.y = min2(p, q);
	c->secorner.y = max2(p, q);
	break;
    case LR_FLIP:		/* y axis  */
	p = x + (x - c->nwcorner.x);
	q = x + (x - c->secorner.x);
	c->nwcorner.x = min2(p, q);
	c->secorner.x = max2(p, q);
	break;
    }
    for (l = c->lines; l != NULL; l = l->next)
	flip_line(l, x, y, flip_axis);
    for (a = c->arcs; a != NULL; a = a->next)
	flip_arc(a, x, y, flip_axis);
    for (e = c->ellipses; e != NULL; e = e->next)
	flip_ellipse(e, x, y, flip_axis);
    for (s = c->splines; s != NULL; s = s->next)
	flip_spline(s, x, y, flip_axis);
    for (t = c->texts; t != NULL; t = t->next)
	flip_text(t, x, y, flip_axis);
    for (c1 = c->compounds; c1 != NULL; c1 = c1->next)
	flip_compound(c1, x, y, flip_axis);
}
