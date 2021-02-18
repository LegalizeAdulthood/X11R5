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
#include "u_search.h"
#include "u_undo.h"
#include "w_canvas.h"
#include "w_mousefun.h"

static int      init_box_scale();
static int      init_boxscale_ellipse();
static int      init_boxscale_line();
static int      init_boxscale_compound();
static int      assign_newboxpoint();
static int      boxrelocate_ellipsepoint();

static int      init_center_scale();
static int      init_scale_arc();
static int      init_scale_compound();
static int      init_scale_ellipse();
static int      init_scale_line();
static int      init_scale_spline();
static int      rescale_points();
static int      relocate_ellipsepoint();
static int      relocate_arcpoint();

static int      fix_scale_arc();
static int      fix_scale_spline();
static int      fix_scale_line();
static int      fix_scale_ellipse();
static int      fix_boxscale_ellipse();
static int      fix_boxscale_line();
static int      fix_scale_compound();
static int      fix_boxscale_compound();

static int      cancel_scale_arc();
static int      cancel_scale_spline();
static int      cancel_scale_line();
static int      cancel_scale_ellipse();
static int      cancel_boxscale_ellipse();
static int      cancel_boxscale_line();
static int      cancel_scale_compound();
static int      cancel_boxscale_compound();

scale_selected()
{
    set_mousefun("scale box", "scale about center", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    init_searchproc_left(init_box_scale);
    init_searchproc_middle(init_center_scale);
    canvas_leftbut_proc = object_search_left;
    canvas_middlebut_proc = object_search_middle;
    canvas_rightbut_proc = null_proc;
    set_cursor(&pick15_cursor);
    reset_action_on();
}

static
init_box_scale(obj, type, x, y, px, py)
    char           *obj;
    int             type, x, y;
    int             px, py;
{
    switch (type) {
    case O_POLYLINE:
	cur_l = (F_line *) obj;
	if (cur_l->type != T_BOX &&
	    cur_l->type != T_ARC_BOX &&
	    cur_l->type != T_EPS_BOX)
	    return;
	init_boxscale_line(px, py);
	break;
    case O_ELLIPSE:
	cur_e = (F_ellipse *) obj;
	if (!init_boxscale_ellipse(px, py))	/* selected center, ignore */
	    return;
	break;
    case O_COMPOUND:
	cur_c = (F_compound *) obj;
	init_boxscale_compound(px, py);
	break;
    default:
	return;
    }
    set_mousefun("new posn", "", "cancel");
    draw_mousefun_canvas();
    canvas_middlebut_proc = null_proc;
}

static
init_center_scale(obj, type, x, y, px, py)
    char           *obj;
    int             type, x, y, px, py;
{
    double          dx, dy, l;

    cur_x = from_x = px;
    cur_y = from_y = py;
    constrained = BOX_SCALE;
    switch (type) {
    case O_POLYLINE:
	cur_l = (F_line *) obj;
	if (!init_scale_line())	/* selected center */
	    return;
	break;
    case O_SPLINE:
	cur_s = (F_spline *) obj;
	if (!init_scale_spline())	/* selected center */
	    return;
	break;
    case O_ELLIPSE:
	cur_e = (F_ellipse *) obj;
	if (!init_scale_ellipse())	/* selected center */
	    return;
	break;
    case O_ARC:
	cur_a = (F_arc *) obj;
	if (!init_scale_arc())	/* selected center */
	    return;
	break;
    case O_COMPOUND:
	cur_c = (F_compound *) obj;
	init_scale_compound();
	break;
    }

    dx = (double) (from_x - fix_x);
    dy = (double) (from_y - fix_y);
    l = sqrt(dx * dx + dy * dy);
    cosa = fabs(dx / l);
    sina = fabs(dy / l);

    set_mousefun("", "new posn", "cancel");
    draw_mousefun_canvas();
    canvas_leftbut_proc = null_proc;
}

static
wrapup_scale()
{
    reset_action_on();
    scale_selected();
    draw_mousefun_canvas();
}

/*************************  ellipse  *******************************/

static
init_boxscale_ellipse(x, y)
    int             x, y;
{
    double          dx, dy, l;

    if (cur_e->type == T_ELLIPSE_BY_RAD ||
	cur_e->type == T_CIRCLE_BY_RAD)
	return False;

    if (x == cur_e->start.x && y == cur_e->start.y) {
	fix_x = cur_e->end.x;
	fix_y = cur_e->end.y;
	cur_x = from_x = x;
	cur_y = from_y = y;
    } else if (x == cur_e->end.x && y == cur_e->end.y) {
	fix_x = cur_e->start.x;
	fix_y = cur_e->start.y;
	cur_x = from_x = x;
	cur_y = from_y = y;
    } else
	return False;

    set_action_on();
    toggle_ellipsemarker(cur_e);
    constrained = BOX_SCALE;
    dx = (double) (cur_x - fix_x);
    dy = (double) (cur_y - fix_y);
    l = sqrt(dx * dx + dy * dy);
    cosa = fabs(dx / l);
    sina = fabs(dy / l);

    set_temp_cursor(&crosshair_cursor);
    if (cur_e->type == T_CIRCLE_BY_DIA) {
	canvas_locmove_proc = constrained_resizing_cbd;
	elastic_cbd();
    } else {
	canvas_locmove_proc = constrained_resizing_ebd;
	elastic_ebd();
    }
    canvas_leftbut_proc = fix_boxscale_ellipse;
    canvas_rightbut_proc = cancel_boxscale_ellipse;
    return True;
}

static
cancel_boxscale_ellipse()
{
    if (cur_e->type == T_CIRCLE_BY_DIA)
	elastic_cbd();
    else
	elastic_ebd();
    toggle_ellipsemarker(cur_e);
    wrapup_scale();
}

static
fix_boxscale_ellipse(x, y)
    int             x, y;
{
    if (cur_e->type == T_CIRCLE_BY_DIA)
	elastic_cbd();
    else
	elastic_ebd();
    adjust_box_pos(x, y, from_x, from_y, &cur_x, &cur_y);
    new_e = copy_ellipse(cur_e);
    boxrelocate_ellipsepoint(new_e, cur_x, cur_y);
    change_ellipse(cur_e, new_e);
    toggle_ellipsemarker(new_e);
    wrapup_scale();
}

static
boxrelocate_ellipsepoint(ellipse, x, y)
    F_ellipse      *ellipse;
    int             x, y;
{
    int             dx, dy;

    set_temp_cursor(&wait_cursor);
    draw_ellipse(ellipse, ERASE);
    if (ellipse->start.x == fix_x)
	ellipse->end.x = x;
    if (ellipse->start.y == fix_y)
	ellipse->end.y = y;
    if (ellipse->end.x == fix_x)
	ellipse->start.x = x;
    if (ellipse->end.y == fix_y)
	ellipse->start.y = y;
    if (ellipse->type == T_CIRCLE_BY_DIA) {
	dx = ellipse->center.x = (fix_x + x) / 2 + .5;
	dy = ellipse->center.y = (fix_y + y) / 2 + .5;
	dx -= x;
	dy -= y;
	ellipse->radiuses.x = sqrt((double) (dx * dx + dy * dy)) + .5;
	ellipse->radiuses.y = ellipse->radiuses.x;
    } else {
	ellipse->center.x = (fix_x + x) / 2;
	ellipse->center.y = (fix_y + y) / 2;
	ellipse->radiuses.x = abs(ellipse->center.x - fix_x);
	ellipse->radiuses.y = abs(ellipse->center.y - fix_y);
    }
    draw_ellipse(ellipse, PAINT);
    reset_cursor();
}

static
init_scale_ellipse()
{
    fix_x = cur_e->center.x;
    fix_y = cur_e->center.y;
    if (from_x == fix_x && from_y == fix_y) {
	put_msg("Center point selected, ignored");
	return False;
    }
    set_action_on();
    toggle_ellipsemarker(cur_e);
    set_temp_cursor(&crosshair_cursor);
    canvas_locmove_proc = scaling_ellipse;
    elastic_scaleellipse(cur_e);
    canvas_middlebut_proc = fix_scale_ellipse;
    canvas_rightbut_proc = cancel_scale_ellipse;
    return True;		/* all is Ok */
}

static
cancel_scale_ellipse()
{
    elastic_scaleellipse(cur_e);
    toggle_ellipsemarker(cur_e);
    wrapup_scale();
}

static
fix_scale_ellipse(x, y)
    int             x, y;
{
    elastic_scaleellipse(cur_e);
    adjust_box_pos(x, y, from_x, from_y, &cur_x, &cur_y);
    new_e = copy_ellipse(cur_e);
    relocate_ellipsepoint(new_e, cur_x, cur_y);
    change_ellipse(cur_e, new_e);
    toggle_ellipsemarker(new_e);
    wrapup_scale();
}

static
relocate_ellipsepoint(ellipse, x, y)
    F_ellipse      *ellipse;
    int             x, y;
{
    int             newx, newy, oldx, oldy;
    float           newd, oldd, scalefact;

    set_temp_cursor(&wait_cursor);
    draw_ellipse(ellipse, ERASE);

    newx = x - fix_x;
    newy = y - fix_y;
    newd = sqrt((double) (newx * newx + newy * newy));
    oldx = from_x - fix_x;
    oldy = from_y - fix_y;
    oldd = sqrt((double) (oldx * oldx + oldy * oldy));
    scalefact = newd / oldd;

    ellipse->radiuses.x = ellipse->radiuses.x * scalefact;
    ellipse->radiuses.y = ellipse->radiuses.y * scalefact;
    ellipse->end.x = fix_x + (ellipse->end.x - fix_x) * scalefact;
    ellipse->end.y = fix_y + (ellipse->end.y - fix_y) * scalefact;
    ellipse->start.x = fix_x + (ellipse->start.x - fix_x) * scalefact;
    ellipse->start.y = fix_y + (ellipse->start.y - fix_y) * scalefact;
    draw_ellipse(ellipse, PAINT);
    reset_cursor();
}

/***************************  arc  *********************************/

static
init_scale_arc()
{
    fix_x = cur_a->center.x;
    fix_y = cur_a->center.y;
    if (from_x == fix_x && from_y == fix_y) {
	put_msg("Center point selected, ignored");
	return False;
    }
    set_action_on();
    toggle_arcmarker(cur_a);
    elastic_scalearc(cur_a);
    set_temp_cursor(&crosshair_cursor);
    canvas_locmove_proc = scaling_arc;
    canvas_middlebut_proc = fix_scale_arc;
    canvas_rightbut_proc = cancel_scale_arc;
    return True;
}

static
cancel_scale_arc()
{
    elastic_scalearc(cur_a);
    toggle_arcmarker(cur_a);
    wrapup_scale();
}

static
fix_scale_arc(x, y)
    int             x, y;
{
    elastic_scalearc(cur_a);
    adjust_box_pos(x, y, from_x, from_y, &x, &y);
    new_a = copy_arc(cur_a);
    relocate_arcpoint(new_a, x, y);
    change_arc(cur_a, new_a);
    toggle_arcmarker(new_a);
    wrapup_scale();
}

static
relocate_arcpoint(arc, x, y)
    F_arc          *arc;
    int             x, y;
{
    int             newx, newy, oldx, oldy;
    float           newd, oldd, scalefact, xx, yy;
    F_pos           p0, p1, p2;

    newx = x - fix_x;
    newy = y - fix_y;
    newd = sqrt((double) (newx * newx + newy * newy));

    oldx = from_x - fix_x;
    oldy = from_y - fix_y;
    oldd = sqrt((double) (oldx * oldx + oldy * oldy));

    scalefact = newd / oldd;

    p0 = arc->point[0];
    p1 = arc->point[1];
    p2 = arc->point[2];
    p0.x = fix_x + (p0.x - fix_x) * scalefact;
    p0.y = fix_y + (p0.y - fix_y) * scalefact;
    p1.x = fix_x + (p1.x - fix_x) * scalefact;
    p1.y = fix_y + (p1.y - fix_y) * scalefact;
    p2.x = fix_x + (p2.x - fix_x) * scalefact;
    p2.y = fix_y + (p2.y - fix_y) * scalefact;
    if (compute_arccenter(p0, p1, p2, &xx, &yy)) {
	set_temp_cursor(&wait_cursor);
	draw_arc(arc, ERASE);	/* erase old arc */
	arc->point[0].x = p0.x;
	arc->point[0].y = p0.y;
	arc->point[1].x = p1.x;
	arc->point[1].y = p1.y;
	arc->point[2].x = p2.x;
	arc->point[2].y = p2.y;
	arc->center.x = xx;
	arc->center.y = yy;
	arc->direction = compute_direction(p0, p1, p2);
	draw_arc(arc, PAINT);	/* draw new arc */
	reset_cursor();
    }
    set_modifiedflag();
}

/**************************  spline  *******************************/

static
init_scale_spline()
{
    int             sumx, sumy, cnt;
    F_point        *p;

    p = cur_s->points;
    if (closed_spline(cur_s))
	p = p->next;
    for (sumx = 0, sumy = 0, cnt = 0; p != NULL; p = p->next) {
	sumx = sumx + p->x;
	sumy = sumy + p->y;
	cnt++;
    }
    fix_x = sumx / cnt;
    fix_y = sumy / cnt;
    if (from_x == fix_x && from_y == fix_y) {
	put_msg("Center point selected, ignored");
	return False;
    }

    set_action_on();
    set_temp_cursor(&crosshair_cursor);
    toggle_splinemarker(cur_s);
    draw_spline(cur_s, ERASE);
    elastic_scalepts(cur_s->points);
    canvas_locmove_proc = scaling_spline;
    canvas_middlebut_proc = fix_scale_spline;
    canvas_rightbut_proc = cancel_scale_spline;
    return True;
}

static
cancel_scale_spline()
{
    elastic_scalepts(cur_s->points);
    draw_spline(cur_s, PAINT);
    toggle_splinemarker(cur_s);
    wrapup_scale();
}

static
fix_scale_spline(x, y)
    int             x, y;
{
    elastic_scalepts(cur_s->points);
    adjust_box_pos(x, y, from_x, from_y, &x, &y);
    /* make a copy of the original and save as unchanged object */
    old_s = copy_spline(cur_s);
    clean_up();
    set_latestspline(old_s);
    set_action_object(F_CHANGE, O_SPLINE);
    old_s->next = cur_s;
    /* now change the original to become the new object */
    rescale_points(cur_s->points, x, y);
    if (int_spline(cur_s))
	remake_control_points(cur_s);
    draw_spline(cur_s, PAINT);
    toggle_splinemarker(cur_s);
    wrapup_scale();
}

/***************************  compound  ********************************/

static
init_boxscale_compound(x, y)
    int             x, y;
{
    int             xmin, ymin, xmax, ymax;
    double          dx, dy, l;

    set_action_on();
    toggle_compoundmarker(cur_c);
    draw_compoundelements(cur_c, ERASE);
    set_temp_cursor(&crosshair_cursor);
    xmin = min2(cur_c->secorner.x, cur_c->nwcorner.x);
    ymin = min2(cur_c->secorner.y, cur_c->nwcorner.y);
    xmax = max2(cur_c->secorner.x, cur_c->nwcorner.x);
    ymax = max2(cur_c->secorner.y, cur_c->nwcorner.y);

    if (x == xmin) {
	fix_x = xmax;
	from_x = x;
	if (y == ymin) {
	    fix_y = ymax;
	    from_y = y;
	    constrained = BOX_SCALE;
	} else if (y == ymax) {
	    fix_y = ymin;
	    from_y = y;
	    constrained = BOX_SCALE;
	} else {
	    fix_y = ymax;
	    from_y = ymin;
	    constrained = BOX_HSTRETCH;
	}
    } else if (x == xmax) {
	fix_x = xmin;
	from_x = x;
	if (y == ymin) {
	    fix_y = ymax;
	    from_y = y;
	    constrained = BOX_SCALE;
	} else if (y == ymax) {
	    fix_y = ymin;
	    from_y = y;
	    constrained = BOX_SCALE;
	} else {
	    fix_y = ymax;
	    from_y = ymin;
	    constrained = BOX_HSTRETCH;
	}
    } else {
	if (y == ymin) {
	    fix_y = ymax;
	    from_y = y;
	    fix_x = xmax;
	    from_x = xmin;
	    constrained = BOX_VSTRETCH;
	} else {		/* y == ymax */
	    fix_y = ymin;
	    from_y = y;
	    fix_x = xmax;
	    from_x = xmin;
	    constrained = BOX_VSTRETCH;
	}
    }

    cur_x = from_x;
    cur_y = from_y;

    dx = (double) (cur_x - fix_x);
    dy = (double) (cur_y - fix_y);
    l = sqrt(dx * dx + dy * dy);
    cosa = fabs(dx / l);
    sina = fabs(dy / l);

    elastic_box(fix_x, fix_y, cur_x, cur_y);
    canvas_locmove_proc = constrained_resizing_box;
    canvas_leftbut_proc = fix_boxscale_compound;
    canvas_rightbut_proc = cancel_boxscale_compound;
}

static
cancel_boxscale_compound()
{
    elastic_box(fix_x, fix_y, cur_x, cur_y);
    draw_compoundelements(cur_c, PAINT);
    toggle_compoundmarker(cur_c);
    wrapup_scale();
}

static
fix_boxscale_compound(x, y)
    int             x, y;
{
    float           scalex, scaley;

    elastic_box(fix_x, fix_y, cur_x, cur_y);
    adjust_box_pos(x, y, from_x, from_y, &x, &y);
    new_c = copy_compound(cur_c);
    scalex = ((float) (x - fix_x)) / (from_x - fix_x);
    scaley = ((float) (y - fix_y)) / (from_y - fix_y);
    scale_compound(new_c, scalex, scaley, fix_x, fix_y);
    change_compound(cur_c, new_c);
    draw_compoundelements(new_c, PAINT);
    toggle_compoundmarker(new_c);
    wrapup_scale();
}

static
init_scale_compound()
{
    fix_x = (cur_c->nwcorner.x + cur_c->secorner.x) / 2;
    fix_y = (cur_c->nwcorner.y + cur_c->secorner.y) / 2;
    set_action_on();
    toggle_compoundmarker(cur_c);
    set_temp_cursor(&crosshair_cursor);
    draw_compoundelements(cur_c, ERASE);
    elastic_scalecompound(cur_c);
    canvas_locmove_proc = scaling_compound;
    canvas_middlebut_proc = fix_scale_compound;
    canvas_rightbut_proc = cancel_scale_compound;
}

static
cancel_scale_compound()
{
    elastic_scalecompound(cur_c);
    draw_compoundelements(cur_c, PAINT);
    toggle_compoundmarker(cur_c);
    wrapup_scale();
}

static
fix_scale_compound(x, y)
    int             x, y;
{
    elastic_scalecompound(cur_c);
    adjust_box_pos(x, y, from_x, from_y, &cur_x, &cur_y);
    /* make a copy of the original and save as unchanged object */
    old_c = copy_compound(cur_c);
    clean_up();
    set_latestcompound(old_c);
    set_action_object(F_CHANGE, O_COMPOUND);
    old_c->next = cur_c;
    /* now change the original to become the new object */
    prescale_compound(cur_c, cur_x, cur_y);
    draw_compoundelements(cur_c, PAINT);
    toggle_compoundmarker(cur_c);
    wrapup_scale();
}

prescale_compound(c, x, y)
    F_compound     *c;
    int             x, y;
{
    int             newx, newy, oldx, oldy;
    float           newd, oldd, scalefact;

    newx = x - fix_x;
    newy = y - fix_y;
    newd = sqrt((double) (newx * newx + newy * newy));
    oldx = from_x - fix_x;
    oldy = from_y - fix_y;
    oldd = sqrt((double) (oldx * oldx + oldy * oldy));
    scalefact = newd / oldd;
    scale_compound(c, scalefact, scalefact, fix_x, fix_y);
}

scale_compound(c, sx, sy, refx, refy)
    F_compound     *c;
    float           sx, sy;
    int             refx, refy;
{
    F_line         *l;
    F_spline       *s;
    F_ellipse      *e;
    F_text         *t;
    F_arc          *a;
    F_compound     *c1;
    int             x1, y1, x2, y2;

    x1 = round(refx + (c->nwcorner.x - refx) * sx);
    y1 = round(refy + (c->nwcorner.y - refy) * sy);
    x2 = round(refx + (c->secorner.x - refx) * sx);
    y2 = round(refy + (c->secorner.y - refy) * sy);
    c->nwcorner.x = min2(x1, x2);
    c->nwcorner.y = min2(y1, y2);
    c->secorner.x = max2(x1, x2);
    c->secorner.y = max2(y1, y2);

    for (l = c->lines; l != NULL; l = l->next) {
	scale_line(l, sx, sy, refx, refy);
    }
    for (s = c->splines; s != NULL; s = s->next) {
	scale_spline(s, sx, sy, refx, refy);
    }
    for (a = c->arcs; a != NULL; a = a->next) {
	scale_arc(a, sx, sy, refx, refy);
    }
    for (e = c->ellipses; e != NULL; e = e->next) {
	scale_ellipse(e, sx, sy, refx, refy);
    }
    for (t = c->texts; t != NULL; t = t->next) {
	scale_text(t, sx, sy, refx, refy);
    }
    for (c1 = c->compounds; c1 != NULL; c1 = c1->next) {
	scale_compound(c1, sx, sy, refx, refy);
    }
}

scale_line(l, sx, sy, refx, refy)
    F_line         *l;
    float           sx, sy;
    int             refx, refy;
{
    F_point        *p;

    for (p = l->points; p != NULL; p = p->next) {
	p->x = round(refx + (p->x - refx) * sx);
	p->y = round(refy + (p->y - refy) * sy);
    }
}

scale_spline(s, sx, sy, refx, refy)
    F_spline       *s;
    float           sx, sy;
    int             refx, refy;
{
    F_point        *p;
    F_control      *c;

    for (p = s->points; p != NULL; p = p->next) {
	p->x = round(refx + (p->x - refx) * sx);
	p->y = round(refy + (p->y - refy) * sy);
    }
    for (c = s->controls; c != NULL; c = c->next) {
	c->lx = refx + (c->lx - refx) * sx;
	c->ly = refy + (c->ly - refy) * sy;
	c->rx = refx + (c->rx - refx) * sx;
	c->ry = refy + (c->ry - refy) * sy;
    }
}

scale_arc(a, sx, sy, refx, refy)
    F_arc          *a;
    float           sx, sy;
    int             refx, refy;
{
    int             i;

    for (i = 0; i < 3; i++) {
	a->point[i].x = round(refx + (a->point[i].x - refx) * sx);
	a->point[i].y = round(refy + (a->point[i].y - refy) * sy);
    }
    compute_arccenter(a->point[0], a->point[1], a->point[2],
		      &a->center.x, &a->center.y);
    a->direction = compute_direction(a->point[0], a->point[1], a->point[2]);
}

scale_ellipse(e, sx, sy, refx, refy)
    F_ellipse      *e;
    float           sx, sy;
    int             refx, refy;
{
    e->center.x = round(refx + (e->center.x - refx) * sx);
    e->center.y = round(refy + (e->center.y - refy) * sy);
    e->start.x = round(refx + (e->start.x - refx) * sx);
    e->start.y = round(refy + (e->start.y - refy) * sy);
    e->end.x = round(refx + (e->end.x - refx) * sx);
    e->end.y = round(refy + (e->end.y - refy) * sy);
    e->radiuses.x = round(e->radiuses.x * sx);
    e->radiuses.y = round(e->radiuses.y * sy);
}

scale_text(t, sx, sy, refx, refy)
    F_text         *t;
    float           sx, sy;
    int             refx, refy;
{
    t->base_x = round(refx + (t->base_x - refx) * sx);
    t->base_y = round(refy + (t->base_y - refy) * sy);
    if (!rigid_text(t))
	t->size = round(t->size * sx);
}


/***************************  line  ********************************/

static
init_boxscale_line(x, y)
    int             x, y;
{
    int             xmin, ymin, xmax, ymax;
    F_point        *p0, *p1, *p2;
    double          dx, dy, l;

    set_action_on();
    toggle_linemarker(cur_l);
    p0 = cur_l->points;
    p1 = p0->next;
    p2 = p1->next;
    xmin = min3(p0->x, p1->x, p2->x);
    ymin = min3(p0->y, p1->y, p2->y);
    xmax = max3(p0->x, p1->x, p2->x);
    ymax = max3(p0->y, p1->y, p2->y);

    if (x == xmin) {
	fix_x = xmax;
	from_x = x;
	if (y == ymin) {
	    fix_y = ymax;
	    from_y = y;
	    constrained = BOX_SCALE;
	} else if (y == ymax) {
	    fix_y = ymin;
	    from_y = y;
	    constrained = BOX_SCALE;
	} else {
	    fix_y = ymax;
	    from_y = ymin;
	    constrained = BOX_HSTRETCH;
	}
    } else if (x == xmax) {
	fix_x = xmin;
	from_x = x;
	if (y == ymin) {
	    fix_y = ymax;
	    from_y = y;
	    constrained = BOX_SCALE;
	} else if (y == ymax) {
	    fix_y = ymin;
	    from_y = y;
	    constrained = BOX_SCALE;
	} else {
	    fix_y = ymax;
	    from_y = ymin;
	    constrained = BOX_HSTRETCH;
	}
    } else {
	if (y == ymin) {
	    fix_y = ymax;
	    from_y = y;
	    fix_x = xmax;
	    from_x = xmin;
	    constrained = BOX_VSTRETCH;
	} else {		/* y == ymax */
	    fix_y = ymin;
	    from_y = y;
	    fix_x = xmax;
	    from_x = xmin;
	    constrained = BOX_VSTRETCH;
	}
    }

    cur_x = from_x;
    cur_y = from_y;
    set_temp_cursor(&crosshair_cursor);
    draw_line(cur_l, ERASE);

    dx = (double) (cur_x - fix_x);
    dy = (double) (cur_y - fix_y);
    l = sqrt(dx * dx + dy * dy);
    cosa = fabs(dx / l);
    sina = fabs(dy / l);

    elastic_box(fix_x, fix_y, cur_x, cur_y);
    canvas_locmove_proc = constrained_resizing_box;
    canvas_leftbut_proc = fix_boxscale_line;
    canvas_rightbut_proc = cancel_boxscale_line;
}

static
cancel_boxscale_line()
{
    elastic_box(fix_x, fix_y, cur_x, cur_y);
    draw_line(cur_l, PAINT);
    toggle_linemarker(cur_l);
    wrapup_scale();
}

static
fix_boxscale_line(x, y)
    int             x, y;
{
    elastic_box(fix_x, fix_y, cur_x, cur_y);
    adjust_box_pos(x, y, from_x, from_y, &x, &y);
    new_l = copy_line(cur_l);
    draw_line(cur_l, ERASE);
    assign_newboxpoint(new_l, fix_x, fix_y, x, y);
    if (new_l->type == T_EPS_BOX) {
        if (signof(fix_x - from_x) != signof(fix_x - x))
            new_l->eps->flipped = 1 - new_l->eps->flipped;
        if (signof(fix_y - from_y) != signof(fix_y - y))
            new_l->eps->flipped = 1 - new_l->eps->flipped;
    }
    change_line(cur_l, new_l);
    draw_line(new_l, PAINT);
    toggle_linemarker(new_l);
    wrapup_scale();
}

static
assign_newboxpoint(b, x1, y1, x2, y2)
    F_line         *b;
    int             x1, y1, x2, y2;
{
    F_point        *p;

    p = b->points;
    if (p->x != x1)
	p->x = x2;
    if (p->y != y1)
	p->y = y2;
    p = p->next;
    if (p->x != x1)
	p->x = x2;
    if (p->y != y1)
	p->y = y2;
    p = p->next;
    if (p->x != x1)
	p->x = x2;
    if (p->y != y1)
	p->y = y2;
    p = p->next;
    if (p->x != x1)
	p->x = x2;
    if (p->y != y1)
	p->y = y2;
    p = p->next;
    if (p->x != x1)
	p->x = x2;
    if (p->y != y1)
	p->y = y2;
}

static
init_scale_line()
{
    int             sumx, sumy, cnt;
    F_point        *p;

    p = cur_l->points;
    if (cur_l->type != T_POLYLINE)
	p = p->next;
    for (sumx = 0, sumy = 0, cnt = 0; p != NULL; p = p->next) {
	sumx = sumx + p->x;
	sumy = sumy + p->y;
	cnt++;
    }
    fix_x = sumx / cnt;
    fix_y = sumy / cnt;
    if (from_x == fix_x && from_y == fix_y) {
	put_msg("Center point selected, ignored");
	return False;
    }

    set_action_on();
    toggle_linemarker(cur_l);
    set_temp_cursor(&crosshair_cursor);
    draw_line(cur_l, ERASE);
    elastic_scalepts(cur_l->points);
    canvas_locmove_proc = scaling_line;
    canvas_middlebut_proc = fix_scale_line;
    canvas_rightbut_proc = cancel_scale_line;
    return True;
}

static
cancel_scale_line()
{
    elastic_scalepts(cur_l->points);
    draw_line(cur_l, PAINT);
    toggle_linemarker(cur_l);
    wrapup_scale();
}

static
fix_scale_line(x, y)
    int             x, y;
{
    elastic_scalepts(cur_l->points);
    adjust_box_pos(x, y, from_x, from_y, &x, &y);
    /* make a copy of the original and save as unchanged object */
    old_l = copy_line(cur_l);
    clean_up();
    set_latestline(old_l);
    set_action_object(F_CHANGE, O_POLYLINE);
    old_l->next = cur_l;
    /* now change the original to become the new object */
    rescale_points(cur_l->points, x, y);
    draw_line(cur_l, PAINT);
    toggle_linemarker(cur_l);
    wrapup_scale();
}

static
rescale_points(pts, x, y)
    F_point        *pts;
    int             x, y;
{
    F_point        *p;
    int             newx, newy, oldx, oldy;
    float           newd, oldd, scalefact;

    p = pts;
    newx = x - fix_x;
    newy = y - fix_y;
    newd = sqrt((double) (newx * newx + newy * newy));

    oldx = from_x - fix_x;
    oldy = from_y - fix_y;
    oldd = sqrt((double) (oldx * oldx + oldy * oldy));

    scalefact = newd / oldd;
    for (p = pts; p != NULL; p = p->next) {
	p->x = fix_x + (p->x - fix_x) * scalefact;
	p->y = fix_y + (p->y - fix_y) * scalefact;
    }
    set_modifiedflag();
}
