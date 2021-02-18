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
#include "mode.h"
#include "object.h"
#include "u_create.h"
#include "u_list.h"

void
list_delete_arc(arc_list, arc)
    F_arc         **arc_list, *arc;
{
    F_arc          *a, *aa;

    if (*arc_list == NULL)
	return;
    if (arc == NULL)
	return;

    for (a = aa = *arc_list; aa != NULL; a = aa, aa = aa->next) {
	if (aa == arc) {
	    if (aa == *arc_list)
		*arc_list = (*arc_list)->next;
	    else
		a->next = aa->next;
	    break;
	}
    }
    arc->next = NULL;
}

void
list_delete_ellipse(ellipse_list, ellipse)
    F_ellipse     **ellipse_list, *ellipse;
{
    F_ellipse      *q, *r;

    if (*ellipse_list == NULL)
	return;
    if (ellipse == NULL)
	return;

    for (q = r = *ellipse_list; r != NULL; q = r, r = r->next) {
	if (r == ellipse) {
	    if (r == *ellipse_list)
		*ellipse_list = (*ellipse_list)->next;
	    else
		q->next = r->next;
	    break;
	}
    }
    ellipse->next = NULL;
}

void
list_delete_line(line_list, line)
    F_line         *line, **line_list;
{
    F_line         *q, *r;

    if (*line_list == NULL)
	return;
    if (line == NULL)
	return;

    for (q = r = *line_list; r != NULL; q = r, r = r->next) {
	if (r == line) {
	    if (r == *line_list)
		*line_list = (*line_list)->next;
	    else
		q->next = r->next;
	    break;
	}
    }
    line->next = NULL;
}

void
list_delete_spline(spline_list, spline)
    F_spline      **spline_list, *spline;
{
    F_spline       *q, *r;

    if (*spline_list == NULL)
	return;
    if (spline == NULL)
	return;

    for (q = r = *spline_list; r != NULL; q = r, r = r->next) {
	if (r == spline) {
	    if (r == *spline_list)
		*spline_list = (*spline_list)->next;
	    else
		q->next = r->next;
	    break;
	}
    }
    spline->next = NULL;
}

void
list_delete_text(text_list, text)
    F_text        **text_list, *text;
{
    F_text         *q, *r;

    if (*text_list == NULL)
	return;
    if (text == NULL)
	return;

    for (q = r = *text_list; r != NULL; q = r, r = r->next)
	if (r == text) {
	    if (r == *text_list)
		*text_list = text->next;
	    else
		q->next = text->next;
	    break;
	}
    text->next = NULL;
}

void
list_delete_compound(list, compound)
    F_compound    **list, *compound;
{
    F_compound     *c, *cc;

    if (*list == NULL)
	return;
    if (compound == NULL)
	return;

    for (cc = c = *list; c != NULL; cc = c, c = c->next) {
	if (c == compound) {
	    if (c == *list)
		*list = (*list)->next;
	    else
		cc->next = c->next;
	    break;
	}
    }
    compound->next = NULL;
}

void
list_add_arc(arc_list, a)
    F_arc         **arc_list, *a;
{
    F_arc	*aa;

    a->next = NULL;
    if ((aa = last_arc(*arc_list)) == NULL)
	*arc_list = a;
    else
	aa->next = a;
}

void
list_add_ellipse(ellipse_list, e)
    F_ellipse     **ellipse_list, *e;
{
    F_ellipse	*ee;

    e->next = NULL;
    if ((ee = last_ellipse(*ellipse_list)) == NULL)
	*ellipse_list = e;
    else
	ee->next = e;
}

void
list_add_line(line_list, l)
    F_line        **line_list, *l;
{
    F_line	*ll;

    l->next = NULL;
    if ((ll = last_line(*line_list)) == NULL)
	*line_list = l;
    else
	ll->next = l;
}

void
list_add_spline(spline_list, s)
    F_spline      **spline_list, *s;
{
    F_spline	*ss;

    s->next = NULL;
    if ((ss = last_spline(*spline_list)) == NULL)
	*spline_list = s;
    else
	ss->next = s;
}

void
list_add_text(text_list, t)
    F_text        **text_list, *t;
{
    F_text	*tt;

    t->next = NULL;
    if ((tt = last_text(*text_list)) == NULL)
	*text_list = t;
    else
	tt->next = t;
}

void
list_add_compound(list, c)
    F_compound    **list, *c;
{
    F_compound	*cc;

    c->next = NULL;
    if ((cc = last_compound(*list)) == NULL)
	*list = c;
    else
	cc->next = c;
}

void
delete_line(old_l)
    F_line         *old_l;
{
    list_delete_line(&objects.lines, old_l);
    clean_up();
    set_latestline(old_l);
    set_action_object(F_DELETE, O_POLYLINE);
    set_modifiedflag();
}

void
delete_arc(old_a)
    F_arc          *old_a;
{
    list_delete_arc(&objects.arcs, old_a);
    clean_up();
    set_latestarc(old_a);
    set_action_object(F_DELETE, O_ARC);
    set_modifiedflag();
}

void
delete_ellipse(old_e)
    F_ellipse      *old_e;
{
    list_delete_ellipse(&objects.ellipses, old_e);
    clean_up();
    set_latestellipse(old_e);
    set_action_object(F_DELETE, O_ELLIPSE);
    set_modifiedflag();
}

void
delete_text(old_t)
    F_text         *old_t;
{
    list_delete_text(&objects.texts, old_t);
    clean_up();
    set_latesttext(old_t);
    set_action_object(F_DELETE, O_TEXT);
    set_modifiedflag();
}

void
delete_spline(old_s)
    F_spline       *old_s;
{
    list_delete_spline(&objects.splines, old_s);
    clean_up();
    set_latestspline(old_s);
    set_action_object(F_DELETE, O_SPLINE);
    set_modifiedflag();
}

void
delete_compound(old_c)
    F_compound     *old_c;
{
    list_delete_compound(&objects.compounds, old_c);
    clean_up();
    set_latestcompound(old_c);
    set_action_object(F_DELETE, O_COMPOUND);
    set_modifiedflag();
}

void
add_line(new_l)
    F_line         *new_l;
{
    list_add_line(&objects.lines, new_l);
    clean_up();
    set_latestline(new_l);
    set_action_object(F_ADD, O_POLYLINE);
    set_modifiedflag();
}

void
add_arc(new_a)
    F_arc          *new_a;
{
    list_add_arc(&objects.arcs, new_a);
    clean_up();
    set_latestarc(new_a);
    set_action_object(F_ADD, O_ARC);
    set_modifiedflag();
}

void
add_ellipse(new_e)
    F_ellipse      *new_e;
{
    list_add_ellipse(&objects.ellipses, new_e);
    clean_up();
    set_latestellipse(new_e);
    set_action_object(F_ADD, O_ELLIPSE);
    set_modifiedflag();
}

void
add_text(new_t)
    F_text         *new_t;
{
    list_add_text(&objects.texts, new_t);
    clean_up();
    set_latesttext(new_t);
    set_action_object(F_ADD, O_TEXT);
    set_modifiedflag();
}

void
add_spline(new_s)
    F_spline       *new_s;
{
    list_add_spline(&objects.splines, new_s);
    clean_up();
    set_latestspline(new_s);
    set_action_object(F_ADD, O_SPLINE);
    set_modifiedflag();
}

void
add_compound(new_c)
    F_compound     *new_c;
{
    list_add_compound(&objects.compounds, new_c);
    clean_up();
    set_latestcompound(new_c);
    set_action_object(F_ADD, O_COMPOUND);
    set_modifiedflag();
}


void
change_line(old_l, new_l)
    F_line         *old_l, *new_l;
{
    list_delete_line(&objects.lines, old_l);
    list_add_line(&objects.lines, new_l);
    clean_up();
    old_l->next = new_l;
    set_latestline(old_l);
    set_action_object(F_CHANGE, O_POLYLINE);
    set_modifiedflag();
}

void
change_arc(old_a, new_a)
    F_arc          *old_a, *new_a;
{
    list_delete_arc(&objects.arcs, old_a);
    list_add_arc(&objects.arcs, new_a);
    clean_up();
    old_a->next = new_a;
    set_latestarc(old_a);
    set_action_object(F_CHANGE, O_ARC);
    set_modifiedflag();
}

void
change_ellipse(old_e, new_e)
    F_ellipse      *old_e, *new_e;
{
    list_delete_ellipse(&objects.ellipses, old_e);
    list_add_ellipse(&objects.ellipses, new_e);
    clean_up();
    old_e->next = new_e;
    set_latestellipse(old_e);
    set_action_object(F_CHANGE, O_ELLIPSE);
    set_modifiedflag();
}

void
change_text(old_t, new_t)
    F_text         *old_t, *new_t;
{
    list_delete_text(&objects.texts, old_t);
    list_add_text(&objects.texts, new_t);
    clean_up();
    old_t->next = new_t;
    set_latesttext(old_t);
    set_action_object(F_CHANGE, O_TEXT);
    set_modifiedflag();
}

void
change_spline(old_s, new_s)
    F_spline       *old_s, *new_s;
{
    list_delete_spline(&objects.splines, old_s);
    list_add_spline(&objects.splines, new_s);
    clean_up();
    old_s->next = new_s;
    set_latestspline(old_s);
    set_action_object(F_CHANGE, O_SPLINE);
    set_modifiedflag();
}

void
change_compound(old_c, new_c)
    F_compound     *old_c, *new_c;
{
    list_delete_compound(&objects.compounds, old_c);
    list_add_compound(&objects.compounds, new_c);
    clean_up();
    old_c->next = new_c;
    set_latestcompound(old_c);
    set_action_object(F_CHANGE, O_COMPOUND);
    set_modifiedflag();
}

tail(ob, tails)
    F_compound     *ob, *tails;
{
    F_arc          *a;
    F_compound     *c;
    F_ellipse      *e;
    F_line         *l;
    F_spline       *s;
    F_text         *t;

    if (NULL != (a = ob->arcs))
	for (; a->next != NULL; a = a->next);
    if (NULL != (c = ob->compounds))
	for (; c->next != NULL; c = c->next);
    if (NULL != (e = ob->ellipses))
	for (; e->next != NULL; e = e->next);
    if (NULL != (l = ob->lines))
	for (; l->next != NULL; l = l->next);
    if (NULL != (s = ob->splines))
	for (; s->next != NULL; s = s->next);
    if (NULL != (t = ob->texts))
	for (; t->next != NULL; t = t->next);

    tails->arcs = a;
    tails->compounds = c;
    tails->ellipses = e;
    tails->lines = l;
    tails->splines = s;
    tails->texts = t;
}

/*
 * Make pointers in tails point to the last element of each list of l1 and
 * Append the lists in l2 after those in l1. The tails pointers must be
 * defined prior to calling append.
 */
append_objects(l1, l2, tails)
    F_compound     *l1, *l2, *tails;
{
    if (tails->arcs)
	tails->arcs->next = l2->arcs;
    else
	l1->arcs = l2->arcs;
    if (tails->compounds)
	tails->compounds->next = l2->compounds;
    else
	l1->compounds = l2->compounds;
    if (tails->ellipses)
	tails->ellipses->next = l2->ellipses;
    else
	l1->ellipses = l2->ellipses;
    if (tails->lines)
	tails->lines->next = l2->lines;
    else
	l1->lines = l2->lines;
    if (tails->splines)
	tails->splines->next = l2->splines;
    else
	l1->splines = l2->splines;
    if (tails->texts)
	tails->texts->next = l2->texts;
    else
	l1->texts = l2->texts;
}

/* Cut is the dual of append. */

cut_objects(objects, tails)
    F_compound     *objects, *tails;
{
    if (tails->arcs)
	tails->arcs->next = NULL;
    else
	objects->arcs = NULL;
    if (tails->compounds)
	tails->compounds->next = NULL;
    else
	objects->compounds = NULL;
    if (tails->ellipses)
	tails->ellipses->next = NULL;
    else
	objects->ellipses = NULL;
    if (tails->lines)
	tails->lines->next = NULL;
    else
	objects->lines = NULL;
    if (tails->splines)
	tails->splines->next = NULL;
    else
	objects->splines = NULL;
    if (tails->texts)
	tails->texts->next = NULL;
    else
	objects->texts = NULL;
}

append_point(x, y, point)
    int             x, y;
    F_point       **point;
{
    F_point        *p;

    if ((p = create_point()) == NULL)
	return;

    p->x = x;
    p->y = y;
    p->next = NULL;
    (*point)->next = p;
    *point = p;
}

num_points(points)
    F_point        *points;
{
    int             n;
    F_point        *p;

    for (p = points, n = 0; p != NULL; p = p->next, n++);
    return (n);
}

F_text *
last_text(list)
    F_text        *list;
{
    F_text	*tt;

    if (list == NULL)
	return NULL;

    for (tt = list; tt->next != NULL; tt = tt->next);
    return tt;
}

F_line *
last_line(list)
    F_line        *list;
{
    F_line	*ll;

    if (list == NULL)
	return NULL;

    for (ll = list; ll->next != NULL; ll = ll->next);
    return ll;
}

F_spline *
last_spline(list)
    F_spline        *list;
{
    F_spline	*ss;

    if (list == NULL)
	return NULL;

    for (ss = list; ss->next != NULL; ss = ss->next);
    return ss;
}

F_arc *
last_arc(list)
    F_arc        *list;
{
    F_arc	*tt;

    if (list == NULL)
	return NULL;

    for (tt = list; tt->next != NULL; tt = tt->next);
    return tt;
}

F_ellipse *
last_ellipse(list)
    F_ellipse        *list;
{
    F_ellipse	*tt;

    if (list == NULL)
	return NULL;

    for (tt = list; tt->next != NULL; tt = tt->next);
    return tt;
}

F_compound *
last_compound(list)
    F_compound        *list;
{
    F_compound	*tt;

    if (list == NULL)
	return NULL;

    for (tt = list; tt->next != NULL; tt = tt->next);
    return tt;
}

F_arc *
prev_arc(list, arc)
    F_arc	    *list, *arc;
{
    F_arc	    *csr;

    if (list == arc)
	return NULL;

    for (csr = list; csr->next != arc; csr = csr->next);
    return csr;
}

F_compound *
prev_compound(list, compound)
    F_compound	    *list, *compound;
{
    F_compound	    *csr;

    if (list == compound)
	return NULL;

    for (csr = list; csr->next != compound; csr = csr->next);
    return csr;
}

F_ellipse *
prev_ellipse(list, ellipse)
    F_ellipse	    *list, *ellipse;
{
    F_ellipse	    *csr;

    if (list == ellipse)
	return NULL;

    for (csr = list; csr->next != ellipse; csr = csr->next);
    return csr;
}

F_line *
prev_line(list, line)
    F_line	    *list, *line;
{
    F_line	    *csr;

    if (list == line)
	return NULL;

    for (csr = list; csr->next != line; csr = csr->next);
    return csr;
}

F_spline *
prev_spline(list, spline)
    F_spline	    *list, *spline;
{
    F_spline	    *csr;

    if (list == spline)
	return NULL;

    for (csr = list; csr->next != spline; csr = csr->next);
    return csr;
}

F_text *
prev_text(list, text)
    F_text	    *list, *text;
{
    F_text	    *csr;

    if (list == text)
	return NULL;

    for (csr = list; csr->next != text; csr = csr->next);
    return csr;
}
