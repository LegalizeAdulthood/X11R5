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
#include "object.h"
#include "mode.h"
#include "paintop.h"
#include "w_zoom.h"

#define set_marker(win,x,y,w,h,z1,z2) \
        zXDrawRectangle(tool_d,(win),gccache[INV_PAINT],(x),(y),(w),(h))

#ifdef notdef
/* not used in present implementation */
static u_int    marker_pattern[3] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

mpr_static(pmarker, 5, 5, 1, marker_pattern);
#endif

center_marker(x, y)
    int             x, y;
{
    pw_vector(canvas_win, x, y - 2, x, y + 2, INV_PAINT, 1, RUBBER_LINE, 0.0);
    pw_vector(canvas_win, x - 2, y, x + 2, y, INV_PAINT, 1, RUBBER_LINE, 0.0);
}

toggle_csrhighlight(x, y)
    int             x, y;
{
    set_line_stuff(1, RUBBER_LINE, 0.0, (INV_PAINT));
    set_marker(canvas_win, x - 2, y - 2, 5, 5, 0, 0);
    set_marker(canvas_win, x - 1, y - 1, 3, 3, 0, 0);
}

ellipse_in_mask()
{
    return (cur_objmask & M_ELLIPSE);
}

arc_in_mask()
{
    return (cur_objmask & M_ARC);
}

compound_in_mask()
{
    return (cur_objmask & M_COMPOUND);
}

anytext_in_mask()
{
    return ((cur_objmask & M_TEXT_NORMAL) || (cur_objmask & M_TEXT_HIDDEN));
}

validtext_in_mask(t)
    F_text         *t;
{
    return ((hidden_text(t) && (cur_objmask & M_TEXT_HIDDEN)) ||
	    ((!hidden_text(t)) && (cur_objmask & M_TEXT_NORMAL)));
}

anyline_in_mask()
{
    return ((cur_objmask & M_POLYLINE_LINE) ||
	    (cur_objmask & M_POLYLINE_POLYGON) ||
	    (cur_objmask & M_POLYLINE_BOX));
}

validline_in_mask(l)
    F_line         *l;
{
    return ((((l->type == T_BOX) ||
	      (l->type == T_ARC_BOX)) && (cur_objmask & M_POLYLINE_BOX)) ||
	    ((l->type == T_EPS_BOX) && (cur_objmask & M_POLYLINE_BOX)) ||
	    ((l->type == T_POLYLINE) && (cur_objmask & M_POLYLINE_LINE)) ||
	    ((l->type == T_POLYGON) && (cur_objmask & M_POLYLINE_POLYGON)));
}

anyspline_in_mask()
{
    return ((cur_objmask & M_SPLINE_O_NORMAL) ||
	    (cur_objmask & M_SPLINE_O_INTERP) ||
	    (cur_objmask & M_SPLINE_C_NORMAL) ||
	    (cur_objmask & M_SPLINE_C_INTERP));
}

validspline_in_mask(s)
    F_spline       *s;
{
    return (((s->type == T_OPEN_INTERP) && (cur_objmask & M_SPLINE_O_INTERP)) ||
	((s->type == T_OPEN_NORMAL) && (cur_objmask & M_SPLINE_O_NORMAL)) ||
      ((s->type == T_CLOSED_INTERP) && (cur_objmask & M_SPLINE_C_INTERP)) ||
       ((s->type == T_CLOSED_NORMAL) && (cur_objmask & M_SPLINE_C_NORMAL)));
}

mask_toggle_ellipsemarker(e)
    F_ellipse      *e;
{
    if (ellipse_in_mask())
	toggle_ellipsemarker(e);
}

mask_toggle_arcmarker(a)
    F_arc          *a;
{
    if (arc_in_mask())
	toggle_arcmarker(a);
}

mask_toggle_compoundmarker(c)
    F_compound     *c;
{
    if (compound_in_mask())
	toggle_compoundmarker(c);
}

mask_toggle_textmarker(t)
    F_text         *t;
{
    if (validtext_in_mask(t))
	toggle_textmarker(t);
}

mask_toggle_linemarker(l)
    F_line         *l;
{
    if (validline_in_mask(l))
	toggle_linemarker(l);
}

mask_toggle_splinemarker(s)
    F_spline       *s;
{
    if (validspline_in_mask(s))
	toggle_splinemarker(s);
}

toggle_markers()
{
    toggle_markers_in_compound(&objects);
}

toggle_markers_in_compound(cmpnd)
    F_compound     *cmpnd;
{
    F_ellipse      *e;
    F_arc          *a;
    F_line         *l;
    F_spline       *s;
    F_text         *t;
    F_compound     *c;
    register int    mask;

    mask = cur_objmask;
    if (mask & M_ELLIPSE)
	for (e = cmpnd->ellipses; e != NULL; e = e->next)
	    toggle_ellipsemarker(e);
    if ((mask & M_TEXT_NORMAL) || (mask & M_TEXT_HIDDEN))
	for (t = cmpnd->texts; t != NULL; t = t->next) {
	    if ((hidden_text(t) && (mask & M_TEXT_HIDDEN)) ||
		((!hidden_text(t)) && (mask & M_TEXT_NORMAL)))
		toggle_textmarker(t);
	}
    if (mask & M_ARC)
	for (a = cmpnd->arcs; a != NULL; a = a->next)
	    toggle_arcmarker(a);
    if ((mask & M_POLYLINE_LINE) ||
	(mask & M_POLYLINE_POLYGON) ||
	(mask & M_POLYLINE_BOX))
	for (l = cmpnd->lines; l != NULL; l = l->next) {
	    if ((((l->type == T_BOX) ||
		  (l->type == T_ARC_BOX)) && (mask & M_POLYLINE_BOX)) ||
		((l->type == T_EPS_BOX) && (mask & M_POLYLINE_BOX)) ||
		((l->type == T_POLYLINE) && (mask & M_POLYLINE_LINE)) ||
		((l->type == T_POLYGON) && (mask & M_POLYLINE_POLYGON)))
		toggle_linemarker(l);
	}
    if ((mask & M_SPLINE_O_NORMAL) || (mask & M_SPLINE_O_INTERP) ||
	(mask & M_SPLINE_C_NORMAL) || (mask & M_SPLINE_C_INTERP))
	for (s = cmpnd->splines; s != NULL; s = s->next) {
	    if (((s->type == T_OPEN_INTERP) && (mask & M_SPLINE_O_INTERP)) ||
		((s->type == T_OPEN_NORMAL) && (mask & M_SPLINE_O_NORMAL)) ||
	     ((s->type == T_CLOSED_INTERP) && (mask & M_SPLINE_C_INTERP)) ||
		((s->type == T_CLOSED_NORMAL) && (mask & M_SPLINE_C_NORMAL)))
		toggle_splinemarker(s);
	}
    if (mask & M_COMPOUND)
	for (c = cmpnd->compounds; c != NULL; c = c->next)
	    toggle_compoundmarker(c);
}

update_markers(mask)
    int             mask;
{
    F_ellipse      *e;
    F_arc          *a;
    F_line         *l;
    F_spline       *s;
    F_text         *t;
    F_compound     *c;
    register int    oldmask, newmask;

    oldmask = cur_objmask;
    newmask = mask;
    if ((oldmask & M_ELLIPSE) != (newmask & M_ELLIPSE))
	for (e = objects.ellipses; e != NULL; e = e->next)
	    toggle_ellipsemarker(e);
    if (((oldmask & M_TEXT_NORMAL) != (newmask & M_TEXT_NORMAL)) ||
	((oldmask & M_TEXT_HIDDEN) != (newmask & M_TEXT_HIDDEN)))
	for (t = objects.texts; t != NULL; t = t->next) {
	    if ((hidden_text(t) &&
		 ((oldmask & M_TEXT_HIDDEN) != (newmask & M_TEXT_HIDDEN))) ||
		((!hidden_text(t)) &&
		 ((oldmask & M_TEXT_NORMAL) != (newmask & M_TEXT_NORMAL))))
		toggle_textmarker(t);
	}
    if ((oldmask & M_ARC) != (newmask & M_ARC))
	for (a = objects.arcs; a != NULL; a = a->next)
	    toggle_arcmarker(a);
    if (((oldmask & M_POLYLINE_LINE) != (newmask & M_POLYLINE_LINE)) ||
	((oldmask & M_POLYLINE_POLYGON) != (newmask & M_POLYLINE_POLYGON)) ||
	((oldmask & M_POLYLINE_BOX) != (newmask & M_POLYLINE_BOX)))
	for (l = objects.lines; l != NULL; l = l->next) {
	    if ((((l->type == T_BOX) ||
		  (l->type == T_ARC_BOX || l->type == T_EPS_BOX)) &&
	      ((oldmask & M_POLYLINE_BOX) != (newmask & M_POLYLINE_BOX))) ||
		((l->type == T_POLYLINE) &&
	    ((oldmask & M_POLYLINE_LINE) != (newmask & M_POLYLINE_LINE))) ||
		((l->type == T_POLYGON) &&
		 ((oldmask & M_POLYLINE_POLYGON) != (newmask & M_POLYLINE_POLYGON))))
		toggle_linemarker(l);
	}
    if (((oldmask & M_SPLINE_O_NORMAL) != (newmask & M_SPLINE_O_NORMAL)) ||
	((oldmask & M_SPLINE_O_INTERP) != (newmask & M_SPLINE_O_INTERP)) ||
	((oldmask & M_SPLINE_C_NORMAL) != (newmask & M_SPLINE_C_NORMAL)) ||
	((oldmask & M_SPLINE_C_INTERP) != (newmask & M_SPLINE_C_INTERP)))
	for (s = objects.splines; s != NULL; s = s->next) {
	    if (((s->type == T_OPEN_INTERP) &&
		 ((oldmask & M_SPLINE_O_INTERP) !=
		  (newmask & M_SPLINE_O_INTERP))) ||
		((s->type == T_OPEN_NORMAL) &&
		 ((oldmask & M_SPLINE_O_NORMAL) !=
		  (newmask & M_SPLINE_O_NORMAL))) ||
		((s->type == T_CLOSED_INTERP) &&
		 ((oldmask & M_SPLINE_C_INTERP) !=
		  (newmask & M_SPLINE_C_INTERP))) ||
		((s->type == T_CLOSED_NORMAL) &&
		 ((oldmask & M_SPLINE_C_NORMAL) !=
		  (newmask & M_SPLINE_C_NORMAL))))
		toggle_splinemarker(s);
	}
    if ((oldmask & M_COMPOUND) != (newmask & M_COMPOUND))
	for (c = objects.compounds; c != NULL; c = c->next)
	    toggle_compoundmarker(c);
    cur_objmask = newmask;
}

toggle_ellipsemarker(e)
    F_ellipse      *e;
{
    set_line_stuff(1, RUBBER_LINE, 0.0, (INV_PAINT));
    set_marker(canvas_win, e->start.x - 2, e->start.y - 2, 5, 5, 0, 0);
    set_marker(canvas_win, e->end.x - 2, e->end.y - 2, 5, 5, 0, 0);
}

toggle_ellipsehighlight(e)
    F_ellipse      *e;
{
    set_line_stuff(1, RUBBER_LINE, 0.0, (INV_PAINT));
    set_marker(canvas_win, e->start.x, e->start.y, 1, 1, 0, 0);
    set_marker(canvas_win, e->start.x - 1, e->start.y - 1, 3, 3, 0, 0);
    set_marker(canvas_win, e->end.x, e->end.y, 1, 1, 0, 0);
    set_marker(canvas_win, e->end.x - 1, e->end.y - 1, 3, 3, 0, 0);
}

toggle_arcmarker(a)
    F_arc          *a;
{
    set_line_stuff(1, RUBBER_LINE, 0.0, (INV_PAINT));
    set_marker(canvas_win, a->point[0].x - 2, a->point[0].y - 2, 5, 5, 0, 0);
    set_marker(canvas_win, a->point[1].x - 2, a->point[1].y - 2, 5, 5, 0, 0);
    set_marker(canvas_win, a->point[2].x - 2, a->point[2].y - 2, 5, 5, 0, 0);
}

toggle_archighlight(a)
    F_arc          *a;
{
    set_line_stuff(1, RUBBER_LINE, 0.0, (INV_PAINT));
    set_marker(canvas_win, a->point[0].x, a->point[0].y, 1, 1, 0, 0);
    set_marker(canvas_win, a->point[0].x - 1, a->point[0].y - 1, 3, 3, 0, 0);
    set_marker(canvas_win, a->point[1].x, a->point[1].y, 1, 1, 0, 0);
    set_marker(canvas_win, a->point[1].x - 1, a->point[1].y - 1, 3, 3, 0, 0);
    set_marker(canvas_win, a->point[2].x, a->point[2].y, 1, 1, 0, 0);
    set_marker(canvas_win, a->point[2].x - 1, a->point[2].y - 1, 3, 3, 0, 0);
}

toggle_textmarker(t)
    F_text         *t;
{
    set_line_stuff(1, RUBBER_LINE, 0.0, (INV_PAINT));
    set_marker(canvas_win, t->base_x - 2, t->base_y - t->height - 2, 5, 5, 0, 0);
    set_marker(canvas_win, t->base_x - 2, t->base_y - 2, 5, 5, 0, 0);
}

toggle_texthighlight(t)
    F_text         *t;
{
    set_line_stuff(1, RUBBER_LINE, 0.0, (INV_PAINT));
    set_marker(canvas_win, t->base_x, t->base_y - t->height, 1, 1, 0, 0);
    set_marker(canvas_win, t->base_x - 1, t->base_y - t->height - 1, 3, 3, 0, 0);
    set_marker(canvas_win, t->base_x, t->base_y, 1, 1, 0, 0);
    set_marker(canvas_win, t->base_x - 1, t->base_y - 1, 3, 3, 0, 0);
}

toggle_compoundmarker(c)
    F_compound     *c;
{
    set_line_stuff(1, RUBBER_LINE, 0.0, (INV_PAINT));
    set_marker(canvas_win, c->nwcorner.x - 2, c->nwcorner.y - 2, 5, 5, 0, 0);
    set_marker(canvas_win, c->secorner.x - 2, c->secorner.y - 2, 5, 5, 0, 0);
    set_marker(canvas_win, c->nwcorner.x - 2, c->secorner.y - 2, 5, 5, 0, 0);
    set_marker(canvas_win, c->secorner.x - 2, c->nwcorner.y - 2, 5, 5, 0, 0);
}

toggle_compoundhighlight(c)
    F_compound     *c;
{
    set_line_stuff(1, RUBBER_LINE, 0.0, (INV_PAINT));
    set_marker(canvas_win, c->nwcorner.x, c->nwcorner.y, 1, 1, 0, 0);
    set_marker(canvas_win, c->nwcorner.x - 1, c->nwcorner.y - 1, 3, 3, 0, 0);
    set_marker(canvas_win, c->secorner.x, c->secorner.y, 1, 1, 0, 0);
    set_marker(canvas_win, c->secorner.x - 1, c->secorner.y - 1, 3, 3, 0, 0);
    set_marker(canvas_win, c->nwcorner.x, c->secorner.y, 1, 1, 0, 0);
    set_marker(canvas_win, c->nwcorner.x - 1, c->secorner.y - 1, 3, 3, 0, 0);
    set_marker(canvas_win, c->secorner.x, c->nwcorner.y, 1, 1, 0, 0);
    set_marker(canvas_win, c->secorner.x - 1, c->nwcorner.y - 1, 3, 3, 0, 0);
}

toggle_linemarker(l)
    F_line         *l;
{
    F_point        *p;
    int             fx, fy, x, y;

    set_line_stuff(1, RUBBER_LINE, 0.0, (INV_PAINT));
    p = l->points;
    fx = p->x;
    fy = p->y;
    for (p = p->next; p != NULL; p = p->next) {
	x = p->x;
	y = p->y;
	set_marker(canvas_win, x - 2, y - 2, 5, 5, 0, 0);
    }
    if (x != fx || y != fy || l->points->next == NULL) {
	set_marker(canvas_win, fx - 2, fy - 2, 5, 5, 0, 0);
    }
}

toggle_linehighlight(l)
    F_line         *l;
{
    F_point        *p;
    int             fx, fy, x, y;

    set_line_stuff(1, RUBBER_LINE, 0.0, (INV_PAINT));
    p = l->points;
    fx = p->x;
    fy = p->y;
    for (p = p->next; p != NULL; p = p->next) {
	x = p->x;
	y = p->y;
	set_marker(canvas_win, x, y, 1, 1, 0, 0);
	set_marker(canvas_win, x - 1, y - 1, 3, 3, 0, 0);
    }
    if (x != fx || y != fy) {
	set_marker(canvas_win, fx, fy, 1, 1, 0, 0);
	set_marker(canvas_win, fx - 1, fy - 1, 3, 3, 0, 0);
    }
}

toggle_splinemarker(s)
    F_spline       *s;
{
    F_point        *p;
    int             fx, fy, x, y;

    set_line_stuff(1, RUBBER_LINE, 0.0, (INV_PAINT));
    p = s->points;
    fx = p->x;
    fy = p->y;
    for (p = p->next; p != NULL; p = p->next) {
	x = p->x;
	y = p->y;
	set_marker(canvas_win, x - 2, y - 2, 5, 5, 0, 0);
    }
    if (x != fx || y != fy) {
	set_marker(canvas_win, fx - 2, fy - 2, 5, 5, 0, 0);
    }
}

toggle_splinehighlight(s)
    F_spline       *s;
{
    F_point        *p;
    int             fx, fy, x, y;

    set_line_stuff(1, RUBBER_LINE, 0.0, (INV_PAINT));
    p = s->points;
    fx = p->x;
    fy = p->y;
    for (p = p->next; p != NULL; p = p->next) {
	x = p->x;
	y = p->y;
	set_marker(canvas_win, x, y, 1, 1, 0, 0);
	set_marker(canvas_win, x - 1, y - 1, 3, 3, 0, 0);
    }
    if (x != fx || y != fy) {
	set_marker(canvas_win, fx, fy, 1, 1, 0, 0);
	set_marker(canvas_win, fx - 1, fy - 1, 3, 3, 0, 0);
    }
}
