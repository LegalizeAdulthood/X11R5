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

/****************** IMPORTS *************/

#include "fig.h"
#include "resources.h"
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "u_elastic.h"
#include "w_canvas.h"
#include "w_zoom.h"

extern float    compute_angle();

/********************** EXPORTS **************/

int             constrained;
int             fix_x, fix_y, work_numsides;
int             x1off, x2off, y1off, y2off;
CURSOR          cur_latexcursor;
int             from_x, from_y;
double          cosa, sina;
int             movedpoint_num;
int             latex_fix_x, latex_fix_y;
F_point        *left_point, *right_point;

/*************************** BOXES *************************/

elastic_box(x1, y1, x2, y2)
    int             x1, y1, x2, y2;
{
    /* line_style = RUBBER_LINE so that we don't scale it */
    pw_vector(canvas_win, x1, y1, x1, y2, INV_PAINT, 1, RUBBER_LINE, 0.0);
    pw_vector(canvas_win, x1, y2, x2, y2, INV_PAINT, 1, RUBBER_LINE, 0.0);
    pw_vector(canvas_win, x2, y2, x2, y1, INV_PAINT, 1, RUBBER_LINE, 0.0);
    pw_vector(canvas_win, x2, y1, x1, y1, INV_PAINT, 1, RUBBER_LINE, 0.0);
}

elastic_movebox()
{
    register int    x1, y1, x2, y2;

    x1 = cur_x + x1off;
    x2 = cur_x + x2off;
    y1 = cur_y + y1off;
    y2 = cur_y + y2off;
    elastic_box(x1, y1, x2, y2);
}

moving_box(x, y)
    int             x, y;
{
    elastic_movebox();
    adjust_pos(x, y, fix_x, fix_y, &cur_x, &cur_y);
    elastic_movebox();
}

resizing_box(x, y)
    int             x, y;
{
    elastic_box(fix_x, fix_y, cur_x, cur_y);
    cur_x = x;
    cur_y = y;
    elastic_box(fix_x, fix_y, cur_x, cur_y);
}

constrained_resizing_box(x, y)
    int             x, y;
{
    elastic_box(fix_x, fix_y, cur_x, cur_y);
    adjust_box_pos(x, y, from_x, from_y, &cur_x, &cur_y);
    elastic_box(fix_x, fix_y, cur_x, cur_y);
}

scaling_compound(x, y)
    int             x, y;
{
    elastic_scalecompound(cur_c);
    adjust_box_pos(x, y, fix_x, fix_y, &cur_x, &cur_y);
    elastic_scalecompound(cur_c);
}

elastic_scalecompound(c)
    F_compound     *c;
{
    int             newx, newy, oldx, oldy, x1, y1, x2, y2;
    float           newd, oldd, scalefact;

    newx = cur_x - fix_x;
    newy = cur_y - fix_y;
    newd = sqrt((double) (newx * newx + newy * newy));
    oldx = from_x - fix_x;
    oldy = from_y - fix_y;
    oldd = sqrt((double) (oldx * oldx + oldy * oldy));
    scalefact = newd / oldd;
    x1 = fix_x + (c->secorner.x - fix_x) * scalefact;
    y1 = fix_y + (c->secorner.y - fix_y) * scalefact;
    x2 = fix_x + (c->nwcorner.x - fix_x) * scalefact;
    y2 = fix_y + (c->nwcorner.y - fix_y) * scalefact;
    elastic_box(x1, y1, x2, y2);
}

/*************************** LINES *************************/

elastic_line()
{
    pw_vector(canvas_win, fix_x, fix_y, cur_x, cur_y,
	      INV_PAINT, 1, RUBBER_LINE, 0.0);
}

freehand_line(x, y)
    int             x, y;
{
    elastic_line();
    cur_x = x;
    cur_y = y;
    elastic_line();
}

elastic_latexline()
{
    int             x, y;
    CURSOR          c;

    latex_endpoint(fix_x, fix_y, cur_x, cur_y, &x, &y, latexarrow_mode,
		   (cur_pointposn == P_ANY) ? 1 : posn_rnd[cur_pointposn]);
    pw_vector(canvas_win, fix_x, fix_y, x, y, INV_PAINT, 1, RUBBER_LINE, 0.0);
    c = (x == cur_x && y == cur_y) ? &null_cursor : &crosshair_cursor;
    if (c != cur_cursor) {
	set_temp_cursor(c);
	cur_cursor = c;
    }
}

latex_line(x, y)
    int             x, y;
{
    elastic_latexline();
    cur_x = x;
    cur_y = y;
    elastic_latexline();
}

constrainedangle_line(x, y)
    int             x, y;
{
    float           angle, dx, dy;

    dx = x - fix_x;
    dy = fix_y - y;
    if (sqrt((double) (dx * dx + dy * dy)) < 7)
	return;
    if (dx == 0)
	angle = -90;
    else
	angle = 180 * atan((double) (dy / dx)) / 3.1416;

    if (manhattan_mode) {
	if (mountain_mode) {
	    if (angle < -67.5)
		angle90_line(x, y);
	    else if (angle < -22.5)
		angle135_line(x, y);
	    else if (angle < 22.5)
		angle0_line(x, y);
	    else if (angle < 67.5)
		angle45_line(x, y);
	    else
		angle90_line(x, y);
	} else {
	    if (angle < -45)
		angle90_line(x, y);
	    else if (angle < 45)
		angle0_line(x, y);
	    else
		angle90_line(x, y);
	}
    } else {
	if (angle < 0)
	    angle135_line(x, y);
	else
	    angle45_line(x, y);
    }
}

angle0_line(x, y)
    int             x, y;
{
    if (x == cur_x && y == cur_y)
	return;
    pw_vector(canvas_win, fix_x, fix_y, cur_x, cur_y, INV_PAINT, 1, RUBBER_LINE, 0.0);
    cur_x = x;
    cur_y = fix_y;
    pw_vector(canvas_win, fix_x, fix_y, cur_x, cur_y, INV_PAINT, 1, RUBBER_LINE, 0.0);
}

angle90_line(x, y)
    int             x, y;
{
    if (x == cur_x && y == cur_y)
	return;
    pw_vector(canvas_win, fix_x, fix_y, cur_x, cur_y, INV_PAINT, 1, RUBBER_LINE, 0.0);
    cur_y = y;
    cur_x = fix_x;
    pw_vector(canvas_win, fix_x, fix_y, cur_x, cur_y, INV_PAINT, 1, RUBBER_LINE, 0.0);
}

angle45_line(x, y)
    int             x, y;
{
    if (x == cur_x && y == cur_y)
	return;
    pw_vector(canvas_win, fix_x, fix_y, cur_x, cur_y, INV_PAINT, 1, RUBBER_LINE, 0.0);
    if (abs(x - fix_x) < abs(y - fix_y)) {
	cur_x = fix_x - y + fix_y;
	cur_y = y;
    } else {
	cur_y = fix_y + fix_x - x;
	cur_x = x;
    }
    pw_vector(canvas_win, fix_x, fix_y, cur_x, cur_y, INV_PAINT, 1, RUBBER_LINE, 0.0);
}

angle135_line(x, y)
    int             x, y;
{
    if (x == cur_x && y == cur_y)
	return;
    pw_vector(canvas_win, fix_x, fix_y, cur_x, cur_y, INV_PAINT, 1, RUBBER_LINE, 0.0);
    if (abs(x - fix_x) < abs(y - fix_y)) {
	cur_x = fix_x + y - fix_y;
	cur_y = y;
    } else {
	cur_y = fix_y + x - fix_x;
	cur_x = x;
    }
    pw_vector(canvas_win, fix_x, fix_y, cur_x, cur_y, INV_PAINT, 1, RUBBER_LINE, 0.0);
}

reshaping_line(x, y)
    int             x, y;
{
    elastic_linelink();
    adjust_pos(x, y, from_x, from_y, &cur_x, &cur_y);
    elastic_linelink();
}

reshaping_latexline(x, y)
    int             x, y;
{
    CURSOR          c;

    elastic_linelink();
    adjust_pos(x, y, from_x, from_y, &cur_x, &cur_y);
    latex_endpoint(latex_fix_x, latex_fix_y, x, y, &cur_x, &cur_y,
    latexarrow_mode, (cur_pointposn == P_ANY) ? 1 : posn_rnd[cur_pointposn]);
    elastic_linelink();
    c = (x == cur_x && y == cur_y) ? &null_cursor : &crosshair_cursor;
    if (c != cur_latexcursor) {
	set_temp_cursor(c);
	cur_latexcursor = c;
    }
}

elastic_linelink()
{
    if (left_point != NULL) {
	pw_vector(canvas_win, left_point->x, left_point->y,
		  cur_x, cur_y, INV_PAINT, 1, RUBBER_LINE, 0.0);
    }
    if (right_point != NULL) {
	pw_vector(canvas_win, right_point->x, right_point->y,
		  cur_x, cur_y, INV_PAINT, 1, RUBBER_LINE, 0.0);
    }
}

extending_line(x, y)
    int             x, y;
{
    elastic_linelink();
    cur_x = x;
    cur_y = y;
    elastic_linelink();
}

moving_line(x, y)
    int             x, y;
{
    elastic_moveline(new_l->points);
    adjust_pos(x, y, fix_x, fix_y, &cur_x, &cur_y);
    elastic_moveline(new_l->points);
}

elastic_moveline(pts)
    F_point        *pts;
{
    F_point        *p;
    int             dx, dy, x, y, xx, yy;

    p = pts;
    if (p->next == NULL) {	/* dot */
	pw_vector(canvas_win, cur_x, cur_y, cur_x, cur_y,
		  INV_PAINT, 1, RUBBER_LINE, 0.0);
    } else {
	dx = cur_x - fix_x;
	dy = cur_y - fix_y;
	x = p->x + dx;
	y = p->y + dy;
	for (p = p->next; p != NULL; x = xx, y = yy, p = p->next) {
	    xx = p->x + dx;
	    yy = p->y + dy;
	    pw_vector(canvas_win, x, y, xx, yy, INV_PAINT, 1, RUBBER_LINE, 0.0);
	}
    }
}

scaling_line(x, y)
    int             x, y;
{
    elastic_scalepts(cur_l->points);
    adjust_box_pos(x, y, fix_x, fix_y, &cur_x, &cur_y);
    elastic_scalepts(cur_l->points);
}

scaling_spline(x, y)
    int             x, y;
{
    elastic_scalepts(cur_s->points);
    adjust_box_pos(x, y, fix_x, fix_y, &cur_x, &cur_y);
    elastic_scalepts(cur_s->points);
}

elastic_scalepts(pts)
    F_point        *pts;
{
    F_point        *p;
    int             newx, newy, oldx, oldy, ox, oy, xx, yy;
    float           newd, oldd, scalefact;

    p = pts;
    newx = cur_x - fix_x;
    newy = cur_y - fix_y;
    newd = sqrt((double) (newx * newx + newy * newy));

    oldx = from_x - fix_x;
    oldy = from_y - fix_y;
    oldd = sqrt((double) (oldx * oldx + oldy * oldy));

    scalefact = newd / oldd;
    ox = fix_x + (p->x - fix_x) * scalefact;
    oy = fix_y + (p->y - fix_y) * scalefact;
    for (p = p->next; p != NULL; ox = xx, oy = yy, p = p->next) {
	xx = fix_x + (p->x - fix_x) * scalefact;
	yy = fix_y + (p->y - fix_y) * scalefact;
	pw_vector(canvas_win, ox, oy, xx, yy, INV_PAINT, 1, RUBBER_LINE, 0.0);
    }
}

elastic_poly(x1, y1, x2, y2, numsides)
    int             x1, y1, x2, y2, numsides;
{
    register float  angle;
    register int    nx, ny, dx, dy, i;
    float           init_angle, mag;
    int             ox, oy;

    dx = x2 - x1;
    dy = y2 - y1;
    mag = sqrt((double) (dx * dx + dy * dy));
    init_angle = compute_angle((float) dx, (float) dy);
    ox = x2;
    oy = y2;

    /* now append numsides points */
    for (i = 1; i < numsides; i++) {
	angle = init_angle - 2.0 * M_PI * (float) i / (float) numsides;
	if (angle < 0)
	    angle += 2.0 * M_PI;
	nx = x1 + (int) (mag * cos((double) angle));
	ny = y1 + (int) (mag * sin((double) angle));
	pw_vector(canvas_win, nx, ny, ox, oy, INV_PAINT, 1, RUBBER_LINE, 0.0);
	ox = nx;
	oy = ny;
    }
    pw_vector(canvas_win, ox, oy, x2, y2, INV_PAINT, 1, RUBBER_LINE, 0.0);
}

resizing_poly(x, y)
    int             x, y;
{
    elastic_poly(fix_x, fix_y, cur_x, cur_y, work_numsides);
    cur_x = x;
    cur_y = y;
    work_numsides = cur_numsides;
    elastic_poly(fix_x, fix_y, cur_x, cur_y, work_numsides);
}

/*********************** ELLIPSES *************************/

elastic_ebr()
{
    register int    x1, y1, x2, y2;
    int             rx, ry;

    rx = cur_x - fix_x;
    ry = cur_y - fix_y;
    x1 = fix_x + rx;
    x2 = fix_x - rx;
    y1 = fix_y + ry;
    y2 = fix_y - ry;
    pw_curve(canvas_win, x1, y1, x2, y2, INV_PAINT, 1, RUBBER_LINE, 0.0, 0);
}

resizing_ebr(x, y)
    int             x, y;
{
    elastic_ebr();
    cur_x = x;
    cur_y = y;
    elastic_ebr();
}

constrained_resizing_ebr(x, y)
    int             x, y;
{
    elastic_ebr();
    adjust_box_pos(x, y, from_x, from_y, &cur_x, &cur_y);
    elastic_ebr();
}

elastic_ebd()
{
    pw_curve(canvas_win, fix_x, fix_y, cur_x, cur_y,
	     INV_PAINT, 1, RUBBER_LINE, 0.0, 0);
}

resizing_ebd(x, y)
    int             x, y;
{
    elastic_ebd();
    cur_x = x;
    cur_y = y;
    elastic_ebd();
}

constrained_resizing_ebd(x, y)
    int             x, y;
{
    elastic_ebd();
    adjust_box_pos(x, y, from_x, from_y, &cur_x, &cur_y);
    elastic_ebd();
}

elastic_cbr()
{
    register int    radius, x1, y1, x2, y2, rx, ry;

    rx = cur_x - fix_x;
    ry = cur_y - fix_y;
    radius = round(sqrt((double) (rx * rx + ry * ry)));
    x1 = fix_x + radius;
    x2 = fix_x - radius;
    y1 = fix_y + radius;
    y2 = fix_y - radius;
    pw_curve(canvas_win, x1, y1, x2, y2, INV_PAINT, 1, RUBBER_LINE, 0.0, 0);
}

resizing_cbr(x, y)
    int             x, y;
{
    elastic_cbr();
    cur_x = x;
    cur_y = y;
    elastic_cbr();
}

elastic_cbd()
{
    register int    x1, y1, x2, y2;
    int             radius, rx, ry;

    rx = (cur_x - fix_x) / 2;
    ry = (cur_y - fix_y) / 2;
    radius = round(sqrt((double) (rx * rx + ry * ry)));
    x1 = fix_x + rx + radius;
    x2 = fix_x + rx - radius;
    y1 = fix_y + ry + radius;
    y2 = fix_y + ry - radius;
    pw_curve(canvas_win, x1, y1, x2, y2, INV_PAINT, 1, RUBBER_LINE, 0.0, 0);
}

resizing_cbd(x, y)
    int             x, y;
{
    elastic_cbd();
    cur_x = x;
    cur_y = y;
    elastic_cbd();
}

constrained_resizing_cbd(x, y)
    int             x, y;
{
    elastic_cbd();
    adjust_box_pos(x, y, from_x, from_y, &cur_x, &cur_y);
    elastic_cbd();
}

elastic_moveellipse()
{
    register int    x1, y1, x2, y2;

    x1 = cur_x + x1off;
    x2 = cur_x + x2off;
    y1 = cur_y + y1off;
    y2 = cur_y + y2off;
    pw_curve(canvas_win, x1, y1, x2, y2, INV_PAINT, 1, RUBBER_LINE, 0.0, 0);
}

moving_ellipse(x, y)
    int             x, y;
{
    elastic_moveellipse();
    adjust_pos(x, y, fix_x, fix_y, &cur_x, &cur_y);
    elastic_moveellipse();
}

elastic_scaleellipse(e)
    F_ellipse      *e;
{
    register int    x1, y1, x2, y2;
    int             rx, ry;
    int             newx, newy, oldx, oldy;
    float           newd, oldd, scalefact;

    newx = cur_x - fix_x;
    newy = cur_y - fix_y;
    newd = sqrt((double) (newx * newx + newy * newy));

    oldx = from_x - fix_x;
    oldy = from_y - fix_y;
    oldd = sqrt((double) (oldx * oldx + oldy * oldy));

    scalefact = newd / oldd;

    rx = e->radiuses.x * scalefact;
    ry = e->radiuses.y * scalefact;
    x1 = fix_x + rx;
    x2 = fix_x - rx;
    y1 = fix_y + ry;
    y2 = fix_y - ry;
    pw_curve(canvas_win, x1, y1, x2, y2, INV_PAINT, 1, RUBBER_LINE, 0.0, 0);
}

scaling_ellipse(x, y)
    int             x, y;
{
    elastic_scaleellipse(cur_e);
    adjust_box_pos(x, y, fix_x, fix_y, &cur_x, &cur_y);
    elastic_scaleellipse(cur_e);
}

/*************************** ARCS *************************/

reshaping_arc(x, y)
    int             x, y;
{
    elastic_arclink();
    adjust_pos(x, y, cur_a->point[movedpoint_num].x,
	       cur_a->point[movedpoint_num].y, &cur_x, &cur_y);
    elastic_arclink();
}

elastic_arclink()
{
    switch (movedpoint_num) {
    case 0:
	pw_vector(canvas_win, cur_x, cur_y,
		  cur_a->point[1].x, cur_a->point[1].y,
		  INV_PAINT, 1, RUBBER_LINE, 0.0);
	break;
    case 1:
	pw_vector(canvas_win, cur_a->point[0].x, cur_a->point[0].y,
		  cur_x, cur_y, INV_PAINT, 1, RUBBER_LINE, 0.0);
	pw_vector(canvas_win, cur_a->point[2].x, cur_a->point[2].y,
		  cur_x, cur_y, INV_PAINT, 1, RUBBER_LINE, 0.0);
	break;
    default:
	pw_vector(canvas_win, cur_a->point[1].x, cur_a->point[1].y,
		  cur_x, cur_y, INV_PAINT, 1, RUBBER_LINE, 0.0);
    }
}

moving_arc(x, y)
    int             x, y;
{
    elastic_movearc(new_a);
    adjust_pos(x, y, fix_x, fix_y, &cur_x, &cur_y);
    elastic_movearc(new_a);
}

elastic_movearc(a)
    F_arc          *a;
{
    int             dx, dy;

    dx = cur_x - fix_x;
    dy = cur_y - fix_y;
    pw_vector(canvas_win, a->point[0].x + dx, a->point[0].y + dy,
	      a->point[1].x + dx, a->point[1].y + dy,
	      INV_PAINT, 1, RUBBER_LINE, 0.0);
    pw_vector(canvas_win, a->point[1].x + dx, a->point[1].y + dy,
	      a->point[2].x + dx, a->point[2].y + dy,
	      INV_PAINT, 1, RUBBER_LINE, 0.0);
}

scaling_arc(x, y)
    int             x, y;
{
    elastic_scalearc(cur_a);
    adjust_box_pos(x, y, fix_x, fix_y, &cur_x, &cur_y);
    elastic_scalearc(cur_a);
}

elastic_scalearc(a)
    F_arc          *a;
{
    int             newx, newy, oldx, oldy;
    float           newd, oldd, scalefact;
    F_pos           p0, p1, p2;

    newx = cur_x - fix_x;
    newy = cur_y - fix_y;
    newd = sqrt((double) (newx * newx + newy * newy));

    oldx = from_x - fix_x;
    oldy = from_y - fix_y;
    oldd = sqrt((double) (oldx * oldx + oldy * oldy));

    scalefact = newd / oldd;

    p0 = a->point[0];
    p1 = a->point[1];
    p2 = a->point[2];
    p0.x = fix_x + (p0.x - fix_x) * scalefact;
    p0.y = fix_y + (p0.y - fix_y) * scalefact;
    p1.x = fix_x + (p1.x - fix_x) * scalefact;
    p1.y = fix_y + (p1.y - fix_y) * scalefact;
    p2.x = fix_x + (p2.x - fix_x) * scalefact;
    p2.y = fix_y + (p2.y - fix_y) * scalefact;

    pw_vector(canvas_win, p0.x, p0.y, p1.x, p1.y,
	      INV_PAINT, 1, RUBBER_LINE, 0.0);
    pw_vector(canvas_win, p1.x, p1.y, p2.x, p2.y,
	      INV_PAINT, 1, RUBBER_LINE, 0.0);
}

/*************************** TEXT *************************/

moving_text(x, y)
    int             x, y;
{
    elastic_movetext();
    adjust_pos(x, y, fix_x, fix_y, &cur_x, &cur_y);
    elastic_movetext();
}

elastic_movetext()
{
    pw_text(canvas_win, cur_x + x1off, cur_y + y1off, INV_PAINT,
	    new_t->font, psfont_text(new_t), new_t->size, new_t->cstring);
}


/*************************** SPLINES *************************/

moving_spline(x, y)
    int             x, y;
{
    elastic_moveline(new_s->points);
    adjust_pos(x, y, fix_x, fix_y, &cur_x, &cur_y);
    elastic_moveline(new_s->points);
}

/*********** AUXILIARY FUNCTIONS FOR CONSTRAINED MOVES ******************/

adjust_box_pos(curs_x, curs_y, orig_x, orig_y, ret_x, ret_y)
    int             curs_x, curs_y, orig_x, orig_y;
    int            *ret_x, *ret_y;
{
    int             xx, sgn_csr2fix_x, yy, sgn_csr2fix_y;
    double          mag_csr2fix_x, mag_csr2fix_y;

    switch (constrained) {
    case MOVE_ARB:
	*ret_x = curs_x;
	*ret_y = curs_y;
	break;
    case BOX_HSTRETCH:
	*ret_x = curs_x;
	*ret_y = orig_y;
	break;
    case BOX_VSTRETCH:
	*ret_x = orig_x;
	*ret_y = curs_y;
	break;
    default:
	/* calculate where scaled and stretched box corners would be */
	xx = curs_x - fix_x;
	sgn_csr2fix_x = signof(xx);
	mag_csr2fix_x = (double) abs(xx);

	yy = curs_y - fix_y;
	sgn_csr2fix_y = signof(yy);
	mag_csr2fix_y = (double) abs(yy);

	if (mag_csr2fix_x * sina > mag_csr2fix_y * cosa) {	/* above diagonal */
	    *ret_x = curs_x;
	    if (constrained == BOX_SCALE)
		*ret_y = fix_y + sgn_csr2fix_y * (int) (mag_csr2fix_x * sina / cosa);
	    else
		*ret_y = fix_y + sgn_csr2fix_y * abs(fix_y - orig_y);
	} else {
	    *ret_y = curs_y;
	    if (constrained == BOX_SCALE)
		*ret_x = fix_x + sgn_csr2fix_x * (int) (mag_csr2fix_y * cosa / sina);
	    else
		*ret_x = fix_x + sgn_csr2fix_x * abs(fix_x - orig_x);
	}
    }
}

adjust_pos(curs_x, curs_y, orig_x, orig_y, ret_x, ret_y)
    int             curs_x, curs_y, orig_x, orig_y;
    int            *ret_x, *ret_y;
{
    if (constrained) {
	if (abs(orig_x - curs_x) > abs(orig_y - curs_y)) {
	    *ret_x = curs_x;
	    *ret_y = orig_y;
	} else {
	    *ret_x = orig_x;
	    *ret_y = curs_y;
	}
    } else {
	*ret_x = curs_x;
	*ret_y = curs_y;
    }
}
