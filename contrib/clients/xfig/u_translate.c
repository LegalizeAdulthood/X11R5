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
#include "object.h"

translate_ellipse(ellipse, dx, dy)
    F_ellipse      *ellipse;
    int             dx, dy;
{
    ellipse->center.x += dx;
    ellipse->center.y += dy;
    ellipse->start.x += dx;
    ellipse->start.y += dy;
    ellipse->end.x += dx;
    ellipse->end.y += dy;
}

translate_arc(arc, dx, dy)
    F_arc          *arc;
    int             dx, dy;
{
    arc->center.x += (float) dx;
    arc->center.y += (float) dy;
    arc->point[0].x += dx;
    arc->point[0].y += dy;
    arc->point[1].x += dx;
    arc->point[1].y += dy;
    arc->point[2].x += dx;
    arc->point[2].y += dy;
}

translate_line(line, dx, dy)
    F_line         *line;
    int             dx, dy;
{
    F_point        *point;

    for (point = line->points; point != NULL; point = point->next) {
	point->x += dx;
	point->y += dy;
    }
}

translate_text(text, dx, dy)
    F_text         *text;
    int             dx, dy;
{
    text->base_x += dx;
    text->base_y += dy;
}

translate_spline(spline, dx, dy)
    F_spline       *spline;
    int             dx, dy;
{
    F_point        *point;
    F_control      *cp;

    for (point = spline->points; point != NULL; point = point->next) {
	point->x += dx;
	point->y += dy;
    }
    for (cp = spline->controls; cp != NULL; cp = cp->next) {
	cp->lx += dx;
	cp->ly += dy;
	cp->rx += dx;
	cp->ry += dy;
    }
}

translate_compound(compound, dx, dy)
    F_compound     *compound;
    int             dx, dy;
{
    compound->nwcorner.x += dx;
    compound->nwcorner.y += dy;
    compound->secorner.x += dx;
    compound->secorner.y += dy;

    translate_lines(compound->lines, dx, dy);
    translate_splines(compound->splines, dx, dy);
    translate_ellipses(compound->ellipses, dx, dy);
    translate_arcs(compound->arcs, dx, dy);
    translate_texts(compound->texts, dx, dy);
    translate_compounds(compound->compounds, dx, dy);
}

translate_arcs(arcs, dx, dy)
    F_arc          *arcs;
    int             dx, dy;
{
    F_arc          *a;

    for (a = arcs; a != NULL; a = a->next)
	translate_arc(a, dx, dy);
}

translate_compounds(compounds, dx, dy)
    F_compound     *compounds;
    int             dx, dy;
{
    F_compound     *c;

    for (c = compounds; c != NULL; c = c->next)
	translate_compound(c, dx, dy);
}

translate_ellipses(ellipses, dx, dy)
    F_ellipse      *ellipses;
    int             dx, dy;
{
    F_ellipse      *e;

    for (e = ellipses; e != NULL; e = e->next)
	translate_ellipse(e, dx, dy);
}

translate_lines(lines, dx, dy)
    F_line         *lines;
    int             dx, dy;
{
    F_line         *l;

    for (l = lines; l != NULL; l = l->next)
	translate_line(l, dx, dy);
}

translate_splines(splines, dx, dy)
    F_spline       *splines;
    int             dx, dy;
{
    F_spline       *s;

    for (s = splines; s != NULL; s = s->next)
	translate_spline(s, dx, dy);
}

translate_texts(texts, dx, dy)
    F_text         *texts;
    int             dx, dy;
{
    F_text         *t;

    for (t = texts; t != NULL; t = t->next)
	translate_text(t, dx, dy);
}
