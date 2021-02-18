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
#include "u_draw.h"
#include "u_elastic.h"
#include "u_list.h"
#include "u_undo.h"
#include "w_canvas.h"
#include "w_mousefun.h"

static          create_compoundobject(), cancel_compound(), init_create_compoundobject();
static		get_arc();
static		get_compound();
static		get_ellipse();
static		get_line();
static		get_spline();
static		get_text();

compound_selected()
{
    set_mousefun("first corner", "", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_leftbut_proc = init_create_compoundobject;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(&arrow_cursor);
    reset_action_on();
}

static
init_create_compoundobject(x, y)
    int             x, y;
{
    init_box_drawing(x, y);
    set_mousefun("final corner", "", "cancel");
    draw_mousefun_canvas();
    canvas_leftbut_proc = create_compoundobject;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = cancel_compound;
}

static
cancel_compound()
{
    elastic_box(fix_x, fix_y, cur_x, cur_y);
    compound_selected();
    draw_mousefun_canvas();
}

static
create_compoundobject(x, y)
    int             x, y;
{
    F_compound     *c;

    if ((c = create_compound()) == NULL)
	return;

    elastic_box(fix_x, fix_y, cur_x, cur_y);
    c->nwcorner.x = min2(fix_x, x);
    c->nwcorner.y = min2(fix_y, y);
    c->secorner.x = max2(fix_x, x);
    c->secorner.y = max2(fix_y, y);
    if (compose_compound(c) == 0) {
	free((char *) c);
	compound_selected();
	draw_mousefun_canvas();
	put_msg("Empty compound, ignored");
	return;
    }
    /*
     * Make the bounding box exactly match the dimensions of the compound.
     */
    compound_bound(c, &c->nwcorner.x, &c->nwcorner.y,
		   &c->secorner.x, &c->secorner.y);

    c->next = NULL;
    clean_up();
    set_action(F_GLUE);
    toggle_markers_in_compound(c);
    list_add_compound(&objects.compounds, c);
    mask_toggle_compoundmarker(c);
    set_latestcompound(c);
    set_modifiedflag();
    compound_selected();
    draw_mousefun_canvas();
}

compose_compound(c)
    F_compound     *c;
{
    c->ellipses = NULL;
    c->lines = NULL;
    c->texts = NULL;
    c->splines = NULL;
    c->arcs = NULL;
    c->compounds = NULL;
    get_ellipse(&c->ellipses, c->nwcorner.x, c->nwcorner.y,
		c->secorner.x, c->secorner.y);
    get_line(&c->lines, c->nwcorner.x, c->nwcorner.y,
	     c->secorner.x, c->secorner.y);
    get_spline(&c->splines, c->nwcorner.x, c->nwcorner.y,
	       c->secorner.x, c->secorner.y);
    get_text(&c->texts, c->nwcorner.x, c->nwcorner.y,
	     c->secorner.x, c->secorner.y);
    get_arc(&c->arcs, c->nwcorner.x, c->nwcorner.y,
	    c->secorner.x, c->secorner.y);
    get_compound(&c->compounds, c->nwcorner.x, c->nwcorner.y,
		 c->secorner.x, c->secorner.y);
    if (c->ellipses != NULL)
	return (1);
    if (c->splines != NULL)
	return (1);
    if (c->lines != NULL)
	return (1);
    if (c->texts != NULL)
	return (1);
    if (c->arcs != NULL)
	return (1);
    if (c->compounds != NULL)
	return (1);
    return (0);
}

static
get_ellipse(list, xmin, ymin, xmax, ymax)
    F_ellipse     **list;
    int             xmin, ymin, xmax, ymax;
{
    F_ellipse      *e, *ee, *ellipse;

    for (e = objects.ellipses; e != NULL;) {
	if (xmin > e->center.x - e->radiuses.x) {
	    ee = e;
	    e = e->next;
	    continue;
	}
	if (xmax < e->center.x + e->radiuses.x) {
	    ee = e;
	    e = e->next;
	    continue;
	}
	if (ymin > e->center.y - e->radiuses.y) {
	    ee = e;
	    e = e->next;
	    continue;
	}
	if (ymax < e->center.y + e->radiuses.y) {
	    ee = e;
	    e = e->next;
	    continue;
	}
	if (*list == NULL)
	    *list = e;
	else
	    ellipse->next = e;
	ellipse = e;
	if (e == objects.ellipses)
	    e = objects.ellipses = objects.ellipses->next;
	else {
	    e = ee->next = e->next;
	}
	ellipse->next = NULL;
    }
}

static
get_arc(list, xmin, ymin, xmax, ymax)
    F_arc         **list;
    int             xmin, ymin, xmax, ymax;
{
    F_arc          *a, *arc, *aa;
    int             urx, ury, llx, lly;

    for (a = objects.arcs; a != NULL;) {
	arc_bound(a, &llx, &lly, &urx, &ury);
	if (xmin > llx)
	    goto out;
	if (xmax < urx)
	    goto out;
	if (ymin > lly)
	    goto out;
	if (ymax < ury)
	    goto out;
	if (*list == NULL)
	    *list = a;
	else
	    arc->next = a;
	arc = a;
	if (a == objects.arcs)
	    a = objects.arcs = objects.arcs->next;
	else
	    a = aa->next = a->next;
	arc->next = NULL;
	continue;
out:
	aa = a;
	a = a->next;
    }
}

static
get_line(list, xmin, ymin, xmax, ymax)
    F_line        **list;
    int             xmin, ymin, xmax, ymax;
{
    F_line         *line, *l, *ll;
    F_point        *p;
    int             inbound;

    for (l = objects.lines; l != NULL;) {
	for (inbound = 1, p = l->points; p != NULL && inbound;
	     p = p->next) {
	    inbound = 0;
	    if (xmin > p->x)
		continue;
	    if (xmax < p->x)
		continue;
	    if (ymin > p->y)
		continue;
	    if (ymax < p->y)
		continue;
	    inbound = 1;
	}
	if (!inbound) {
	    ll = l;
	    l = l->next;
	    continue;
	}
	if (*list == NULL)
	    *list = l;
	else
	    line->next = l;
	line = l;
	if (l == objects.lines)
	    l = objects.lines = objects.lines->next;
	else
	    l = ll->next = l->next;
	line->next = NULL;
    }
}

static
get_spline(list, xmin, ymin, xmax, ymax)
    F_spline      **list;
    int             xmin, ymin, xmax, ymax;
{
    F_spline       *spline, *s, *ss;
    int             urx, ury, llx, lly;

    for (s = objects.splines; s != NULL;) {
	spline_bound(s, &llx, &lly, &urx, &ury);
	if (xmin > llx)
	    goto out;
	if (xmax < urx)
	    goto out;
	if (ymin > lly)
	    goto out;
	if (ymax < ury)
	    goto out;
	if (*list == NULL)
	    *list = s;
	else
	    spline->next = s;
	spline = s;
	if (s == objects.splines)
	    s = objects.splines = objects.splines->next;
	else
	    s = ss->next = s->next;
	spline->next = NULL;
	continue;
out:
	ss = s;
	s = s->next;
    }
}

static
get_text(list, xmin, ymin, xmax, ymax)
    F_text        **list;
    int             xmin, ymin, xmax, ymax;
{
    int             halflen;
    F_text         *text, *t, *tt;

    for (t = objects.texts; t != NULL;) {
	halflen = t->length / 2;
	if (((t->type == T_LEFT_JUSTIFIED) && xmin > t->base_x) ||
	  ((t->type == T_CENTER_JUSTIFIED) && xmin > t->base_x - halflen) ||
	 ((t->type == T_RIGHT_JUSTIFIED) && xmin > t->base_x - t->length)) {
	    tt = t;
	    t = t->next;
	    continue;
	}
	if (((t->type == T_LEFT_JUSTIFIED) && xmax < t->base_x + t->length) ||
	  ((t->type == T_CENTER_JUSTIFIED) && xmax < t->base_x + halflen) ||
	    ((t->type == T_RIGHT_JUSTIFIED) && xmax < t->base_x)) {
	    tt = t;
	    t = t->next;
	    continue;
	}
	if (ymin > t->base_y - t->height) {
	    tt = t;
	    t = t->next;
	    continue;
	}
	if (ymax < t->base_y) {
	    tt = t;
	    t = t->next;
	    continue;
	}
	if (*list == NULL)
	    *list = t;
	else
	    text->next = t;
	text = t;
	if (t == objects.texts)
	    t = objects.texts = objects.texts->next;
	else
	    t = tt->next = t->next;
	text->next = NULL;
    }
}

static
get_compound(list, xmin, ymin, xmax, ymax)
    F_compound    **list;
    int             xmin, ymin, xmax, ymax;
{
    F_compound     *compd, *c, *cc;

    for (c = objects.compounds; c != NULL;) {
	if (xmin > c->nwcorner.x) {
	    cc = c;
	    c = c->next;
	    continue;
	}
	if (xmax < c->secorner.x) {
	    cc = c;
	    c = c->next;
	    continue;
	}
	if (ymin > c->nwcorner.y) {
	    cc = c;
	    c = c->next;
	    continue;
	}
	if (ymax < c->secorner.y) {
	    cc = c;
	    c = c->next;
	    continue;
	}
	if (*list == NULL)
	    *list = c;
	else
	    compd->next = c;
	compd = c;
	if (c == objects.compounds)
	    c = objects.compounds = objects.compounds->next;
	else
	    c = cc->next = c->next;
	compd->next = NULL;
    }
}
