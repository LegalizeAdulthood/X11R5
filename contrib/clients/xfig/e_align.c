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
#include "paintop.h"
#include "mode.h"
#include "u_create.h"
#include "u_draw.h"
#include "u_search.h"
#include "u_undo.h"
#include "w_canvas.h"
#include "w_mousefun.h"

static int      init_align();
static int      llx, lly, urx, ury;
static int      xcmin, ycmin, xcmax, ycmax;
static int      dx, dy;
static int	align_arc();
static int	align_ellipse();
static int	align_line();
static int	align_spline();
static int	align_text();
static int	align_compound();
static int	get_dx_dy();

align_selected()
{
    set_mousefun("align compound", "", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    init_searchproc_left(init_align);
    canvas_leftbut_proc = object_search_left;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(&pick15_cursor);
}

static
init_align(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y;
    int             px, py;
{
    if (type != O_COMPOUND)
	return;
    cur_c = (F_compound *) p;
    toggle_compoundmarker(cur_c);
    draw_compoundelements(cur_c, ERASE);
    old_c = copy_compound(cur_c);
    compound_bound(cur_c, &xcmin, &ycmin, &xcmax, &ycmax);
    align_ellipse();
    align_arc();
    align_line();
    align_spline();
    align_compound();
    align_text();
    /*
     * recompute the compound's bounding box
     */
    compound_bound(cur_c, &cur_c->nwcorner.x, &cur_c->nwcorner.y,
		   &cur_c->secorner.x, &cur_c->secorner.y);
    draw_compoundelements(cur_c, PAINT);
    toggle_compoundmarker(cur_c);
    clean_up();
    old_c->next = cur_c;
    set_latestcompound(old_c);
    set_action_object(F_CHANGE, O_COMPOUND);
    set_modifiedflag();
}

static int
align_ellipse()
{
    F_ellipse      *e;

    for (e = cur_c->ellipses; e != NULL; e = e->next) {
	ellipse_bound(e, &llx, &lly, &urx, &ury);
	get_dx_dy();
	translate_ellipse(e, dx, dy);
    }
}

static int
align_arc()
{
    F_arc          *a;

    for (a = cur_c->arcs; a != NULL; a = a->next) {
	arc_bound(a, &llx, &lly, &urx, &ury);
	get_dx_dy();
	translate_arc(a, dx, dy);
    }
}

static int
align_line()
{
    F_line         *l;

    for (l = cur_c->lines; l != NULL; l = l->next) {
	line_bound(l, &llx, &lly, &urx, &ury);
	get_dx_dy();
	translate_line(l, dx, dy);
    }
}

static int
align_spline()
{
    F_spline       *s;

    for (s = cur_c->splines; s != NULL; s = s->next) {
	spline_bound(s, &llx, &lly, &urx, &ury);
	get_dx_dy();
	translate_spline(s, dx, dy);
    }
}

static int
align_compound()
{
    F_compound     *c;

    for (c = cur_c->compounds; c != NULL; c = c->next) {
	compound_bound(c, &llx, &lly, &urx, &ury);
	get_dx_dy();
	translate_compound(c, dx, dy);
    }
}

static int
align_text()
{
    F_text         *t;

    for (t = cur_c->texts; t != NULL; t = t->next) {
	text_bound(t, &llx, &lly, &urx, &ury);
	get_dx_dy();
	translate_text(t, dx, dy);
    }
}

static int
get_dx_dy()
{
    switch (cur_valign) {
	case NONE:
	dy = 0;
	break;
    case TOP:
	dy = ycmin - lly;
	break;
    case BOTTOM:
	dy = ycmax - ury;
	break;
    case CENTER:
	dy = (ycmin - lly) + (abs(ycmin - lly) + abs(ycmax - ury)) / 2;
	break;
    }
    switch (cur_halign) {
    case NONE:
	dx = 0;
	break;
    case LEFT:
	dx = xcmin - llx;
	break;
    case RIGHT:
	dx = xcmax - urx;
	break;
    case CENTER:
	dx = (xcmin - llx) + (abs(xcmin - llx) + abs(xcmax - urx)) / 2;
	break;
    }
}
