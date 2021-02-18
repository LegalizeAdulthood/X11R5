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

/*
 * Routines for drawing and filling objects under the following headings:
 * 	ARC, ELLIPSE, LINE, SPLINE, TEXT, COMPOUND,
 *	ARROWS, CURVES FOR ARCS AND ELLIPSES, CURVES FOR SPLINES.
 *
 */

#include "fig.h"
#include "resources.h"
#include "object.h"
#include "paintop.h"
#include "u_bound.h"
#include "u_create.h"
#include "u_draw.h"
#include "w_canvas.h"
#include "w_drawprim.h"
#include "w_zoom.h"

extern char    *malloc();

typedef unsigned char byte;

#define			PI		3.14159

/************** POLYGON/CURVE DRAWING FACILITIES ****************/

#define   MaxNumPts	  4000
static int      npoints;
static int      max_points;
static XPoint  *points;
static int      allocstep;

static          Boolean
init_point_array(init_size, step_size)
    int             init_size, step_size;
{
    npoints = 0;
    max_points = init_size;
    allocstep = step_size;
    if (max_points > MaxNumPts)
	max_points = MaxNumPts;
    if ((points = (XPoint *) malloc(max_points * sizeof(XPoint))) == 0) {
	fprintf(stderr, "xfig: insufficient memory to allocate point array\n");
	return False;
    }
    return True;
}

static          Boolean
add_point(x, y)
    int             x, y;
{
    if (npoints >= (max_points - 1)) {
	XPoint         *tmp_p;

	max_points += allocstep;
	if (max_points > MaxNumPts)
	    return False;	/* stop; it is not closing */

	if ((tmp_p = (XPoint *) realloc(points,
					max_points * sizeof(XPoint))) == 0) {
	    fprintf(stderr,
		    "xfig: insufficient memory to reallocate point array\n");
	    return False;
	}
	free(points);
	points = tmp_p;
    }
    points[npoints].x = (short) x;
    points[npoints].y = (short) y;
    npoints++;
    return True;
}

static void
draw_point_array(w, op, line_width, line_style, style_val, area_fill)
    Window          w;
    int             op;
    int             line_width, line_style;
    float           style_val;
    int             area_fill;
{
    pw_lines(w, points, npoints, op,
	     line_width, line_style, style_val, area_fill);
    free(points);
}

/*********************** ARC ***************************/

draw_arc(a, op)
    F_arc          *a;
    int             op;
{
    int             radius, rx, ry;
    int             xmin, ymin, xmax, ymax;

    arc_bound(a, &xmin, &ymin, &xmax, &ymax);
    if (!overlapping(ZOOMX(xmin), ZOOMY(ymin), ZOOMX(xmax), ZOOMY(ymax),
		     clip_xmin, clip_ymin, clip_xmax, clip_ymax))
	return;

    rx = a->point[0].x - a->center.x;
    ry = a->center.y - a->point[0].y;
    radius = round(sqrt((double) (rx * rx + ry * ry)));

    curve(canvas_win, round(a->point[0].x - a->center.x),
	  round(a->center.y - a->point[0].y),
	  round(a->point[2].x - a->center.x),
	  round(a->center.y - a->point[2].y),
	  a->direction, radius, radius,
	  round(a->center.x), round(a->center.y), op,
	  a->thickness, a->style, a->style_val, a->area_fill);

    draw_arcarrows(a, op);
}

/*********************** ELLIPSE ***************************/

draw_ellipse(e, op)
    F_ellipse      *e;
    int             op;
{
    int             a, b, xmin, ymin, xmax, ymax;

    ellipse_bound(e, &xmin, &ymin, &xmax, &ymax);
    if (!overlapping(ZOOMX(xmin), ZOOMY(ymin), ZOOMX(xmax), ZOOMY(ymax),
		     clip_xmin, clip_ymin, clip_xmax, clip_ymax))
	return;

    if (op != ERASE && (e->style == DOTTED_LINE || e->style == DASH_LINE)) {
	a = e->radiuses.x;
	b = e->radiuses.y;
	curve(canvas_win, a, 0, a, 0, e->direction, (b * b), (a * a),
	      e->center.x, e->center.y, op,
	      e->thickness, e->style, e->style_val, e->area_fill);
    } else {
	pw_curve(canvas_win, xmin, ymin, xmax, ymax, op,
		 e->thickness, e->style, e->style_val, e->area_fill);
    }
}

/*********************** LINE ***************************/

draw_line(line, op)
    F_line         *line;
    int             op;
{
    F_point        *point;
    int             num_pts;
    int             xx, yy, x, y;
    int             xmin, ymin, xmax, ymax;
    char           *string;
    F_point        *p0, *p1, *p2;
    pr_size         txt;

    line_bound(line, &xmin, &ymin, &xmax, &ymax);
    if (!overlapping(ZOOMX(xmin), ZOOMY(ymin), ZOOMX(xmax), ZOOMY(ymax),
		     clip_xmin, clip_ymin, clip_xmax, clip_ymax))
	return;

    /* is it an arcbox? */
    if (line->type == T_ARC_BOX) {
	draw_arcbox(line, op);
	return;
    }
    /* is it an eps file? */
    if (line->type == T_EPS_BOX) {
	if (line->eps->bitmap != NULL) {
	    draw_eps_pixmap(line, op);
	    return;
	} else {		/* label empty eps bounding box */
	    if (line->eps->file[0] == '\0')
		string = EMPTY_EPS;
	    else {
		string = rindex(line->eps->file, '/');
		if (string == NULL)
		    string = line->eps->file;
		else
		    string++;
	    }
	    p0 = line->points;
	    p1 = p0->next;
	    p2 = p1->next;
	    xmin = min3(p0->x, p1->x, p2->x);
	    ymin = min3(p0->y, p1->y, p2->y);
	    xmax = max3(p0->x, p1->x, p2->x);
	    ymax = max3(p0->y, p1->y, p2->y);
	    txt = pf_textwidth(0, 0, 12, strlen(string), string);
	    x = (xmin + xmax) / 2 - txt.x / 2;
	    y = (ymin + ymax) / 2;
	    pw_text(canvas_win, x, y, op, 0, 0, 12, string);
	    /* return; */
	}
    }
    /* get first point and coordinates */
    point = line->points;
    x = point->x;
    y = point->y;

    /* is it a single point? */
    if (line->points->next == NULL) {
	/* draw but don't fill */
	pw_point(canvas_win, x, y, line->thickness, op);
	return;
    }
    if (line->back_arrow)	/* backward arrow  */
	draw_arrow(point->next->x, point->next->y, x, y,
		   line->back_arrow, op);

    num_pts = 0;
    /* count number of points in this object */
    for (; point != NULL; point = point->next)
	num_pts++;

    /* accumulate the points in an array */
    if (!init_point_array(num_pts, 0))
	return;

    for (point = line->points; point != NULL; point = point->next) {
	xx = x;
	yy = y;
	x = point->x;
	y = point->y;
	add_point(x, y);
    }

    draw_point_array(canvas_win, op, line->thickness, line->style,
		     line->style_val, line->area_fill);

    if (line->for_arrow)
	draw_arrow(xx, yy, x, y, line->for_arrow, op);
}

draw_arcbox(line, op)
    F_line         *line;
    int             op;
{
    F_point        *point;
    int             xmin, xmax, ymin, ymax;

    point = line->points;
    xmin = xmax = point->x;
    ymin = ymax = point->y;
    while (point->next) {	/* find lower left (upper-left on screen) */
	/* and upper right (lower right on screen) */
	point = point->next;
	if (point->x < xmin)
	    xmin = point->x;
	else if (point->x > xmax)
	    xmax = point->x;
	if (point->y < ymin)
	    ymin = point->y;
	else if (point->y > ymax)
	    ymax = point->y;
    }
    pw_arcbox(canvas_win, xmin, ymin, xmax, ymax, line->radius, op,
	    line->thickness, line->style, line->style_val, line->area_fill);
}

draw_eps_pixmap(box, op)
    F_line         *box;
    int             op;
{
    int             xmin, ymin;
    int             xmax, ymax;
    int		    width, height, rotation;
    F_pos           origin;
    F_pos           opposite;

    origin.x = ZOOMX(box->points->x);
    origin.y = ZOOMY(box->points->y);
    opposite.x = ZOOMX(box->points->next->next->x);
    opposite.y = ZOOMY(box->points->next->next->y);

    xmin = min2(origin.x, opposite.x);
    ymin = min2(origin.y, opposite.y);
    xmax = max2(origin.x, opposite.x);
    ymax = max2(origin.y, opposite.y);
    if (op == ERASE) {
	clear_region(xmin, ymin, xmax, ymax);
	return;
    }

    width = abs(origin.x - opposite.x);
    height = abs(origin.y - opposite.y);
    rotation = 0;
    if (origin.x > opposite.x && origin.y > opposite.y)
	rotation = 180;
    if (origin.x > opposite.x && origin.y <= opposite.y)
	rotation = 270;
    if (origin.x <= opposite.x && origin.y > opposite.y)
	rotation = 90;

    if (box->eps->pix_rotation != rotation ||
        box->eps->pix_width != width ||
	box->eps->pix_height != height ||
	box->eps->pix_flipped != box->eps->flipped)
	create_eps_pixmap(box, rotation, width, height, box->eps->flipped);

    XCopyArea(tool_d, box->eps->pixmap, canvas_win, gccache[op],
	      0, 0, xmax-xmin, ymax-ymin, xmin, ymin);
    XFlush(tool_d);
}

/*
 * The input to this routine is the bitmap which is the "preview"
 * section of an encapsulated postscript file. That input bitmap
 * has an arbitrary number of rows and columns. This routine
 * re-samples the input bitmap creating an output bitmap of dimensions
 * width-by-height. This output bitmap is made into an X-windows pixmap
 * for display purposes.
 */
create_eps_pixmap(box, rotation, width, height, flipped)
    F_line         *box;
    int             rotation, width, height, flipped;
{
    int             i;
    int             j;
    byte           *data;
    byte           *tdata;
    int             nbytes;
    int             bbytes;
    int             ibit;
    int             jbit;
    int             wbit;

    if (box->eps->pixmap != 0)
	XFreePixmap(tool_d, box->eps->pixmap);

    nbytes = (width + 7) / 8;
    bbytes = (box->eps->bit_size.x + 7) / 8;
    data = (byte *) malloc(nbytes * height);
    tdata = (byte *) malloc(nbytes);
    memset((char *) data, NULL, nbytes * height);	/* clear memory */

    /* create a new bitmap at the specified size (requires interpolation) */
    if ((!flipped && (rotation == 0 || rotation == 180)) ||
        (flipped && !(rotation == 0 || rotation == 180))) {
	for (j = 0; j < height; j++)
	    for (i = 0; i < width; i++) {
		ibit = box->eps->bit_size.x * i / width;
		jbit = box->eps->bit_size.y * j / height;
		wbit = *(box->eps->bitmap + jbit * bbytes + ibit / 8);
		if (wbit & (1 << (7 - (ibit & 7))))
		    *(data + j * nbytes + i / 8) += (1 << (i & 7));
	    }
    } else {
	for (j = 0; j < height; j++)
	    for (i = 0; i < width; i++) {
		ibit = box->eps->bit_size.x * j / height;
		jbit = box->eps->bit_size.y * i / width;
		wbit = *(box->eps->bitmap + jbit * bbytes + ibit / 8);
		if (wbit & (1 << (7 - (ibit & 7))))
		    *(data + (height - j) * nbytes + i / 8) += (1 << (i & 7));
	    }
    }

    /* horizontal swap */
    if (rotation == 180 || rotation == 270)
	for (j = 0; j < height; j++) {
	    memset((char *) tdata, NULL, nbytes);
	    for (i = 0; i < width; i++)
		if (*(data + j * nbytes + (width - i - 1) / 8) & (1 << ((width - i - 1) & 7)))
		    *(tdata + i / 8) += (1 << (i & 7));
	    bcopy(tdata, data + j * nbytes, nbytes);
	}

    /* vertical swap */
    if ((!flipped && (rotation == 180 || rotation == 270)) ||
	(flipped && !(rotation == 180 || rotation == 270)))
	for (j = 0; j < (height + 1) / 2; j++) {
	    bcopy(data + j * nbytes, tdata, nbytes);
	    bcopy(data + (height - j - 1) * nbytes, data + j * nbytes, nbytes);
	    bcopy(tdata, data + (height - j - 1) * nbytes, nbytes);
	}

    box->eps->pixmap = XCreatePixmapFromBitmapData(tool_d, canvas_win,
                                (char *) data, width, height,
                                x_fg_color.pixel, x_bg_color.pixel,
				DefaultDepthOfScreen(tool_s));
    free(data);
    free(tdata);

    box->eps->pix_rotation = rotation;
    box->eps->pix_width = width;
    box->eps->pix_height = height;
    box->eps->pix_flipped = flipped;
}

/*********************** SPLINE ***************************/

draw_spline(spline, op)
    F_spline       *spline;
    int             op;
{
    int             xmin, ymin, xmax, ymax;

    spline_bound(spline, &xmin, &ymin, &xmax, &ymax);
    if (!overlapping(ZOOMX(xmin), ZOOMY(ymin), ZOOMX(xmax), ZOOMY(ymax),
		     clip_xmin, clip_ymin, clip_xmax, clip_ymax))
	return;

    if (int_spline(spline))
	draw_intspline(spline, op);
    else if (spline->type == T_CLOSED_NORMAL)
	draw_closed_spline(spline, op);
    else if (spline->type == T_OPEN_NORMAL)
	draw_open_spline(spline, op);
}

draw_intspline(s, op)
    F_spline       *s;
    int             op;
{
    F_point        *p1, *p2;
    F_control      *cp1, *cp2;

    p1 = s->points;
    cp1 = s->controls;
    cp2 = cp1->next;
    if (s->back_arrow)
	draw_arrow(round(cp2->lx), round(cp2->ly), p1->x, p1->y,
		   s->back_arrow, op);

    if (!init_point_array(300, 200))
	return;

    for (p2 = p1->next, cp2 = cp1->next; p2 != NULL;
	 p1 = p2, cp1 = cp2, p2 = p2->next, cp2 = cp2->next) {
	bezier_spline((float) p1->x, (float) p1->y, cp1->rx, cp1->ry,
		      cp2->lx, cp2->ly, (float) p2->x, (float) p2->y, op,
		      s->thickness, s->style, s->style_val);
    }

    add_point(p1->x, p1->y);

    draw_point_array(canvas_win, op, s->thickness, s->style,
		     s->style_val, s->area_fill);

    if (s->for_arrow)
	draw_arrow(round(cp1->lx), round(cp1->ly), p1->x,
		   p1->y, s->for_arrow, op);
}

draw_open_spline(spline, op)
    F_spline       *spline;
    int             op;
{
    F_point        *p;
    float           cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
    float           x1, y1, x2, y2;

    if (!init_point_array(300, 200))
	return;

    p = spline->points;
    x1 = p->x;
    y1 = p->y;
    p = p->next;
    x2 = p->x;
    y2 = p->y;
    cx1 = (x1 + x2) / 2;
    cy1 = (y1 + y2) / 2;
    cx2 = (cx1 + x2) / 2;
    cy2 = (cy1 + y2) / 2;
    if (spline->back_arrow)	/* backward arrow  */
	draw_arrow((int) x2, (int) y2, (int) x1, (int) y1,
		   spline->back_arrow, op);
    add_point((int) x1, (int) y1);

    for (p = p->next; p != NULL; p = p->next) {
	x1 = x2;
	y1 = y2;
	x2 = p->x;
	y2 = p->y;
	cx4 = (x1 + x2) / 2;
	cy4 = (y1 + y2) / 2;
	cx3 = (x1 + cx4) / 2;
	cy3 = (y1 + cy4) / 2;
	quadratic_spline(cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4, op,
		       spline->thickness, spline->style, spline->style_val);
	cx1 = cx4;
	cy1 = cy4;
	cx2 = (cx1 + x2) / 2;
	cy2 = (cy1 + y2) / 2;
    }

    add_point(round(cx1), round(cy1));
    add_point((int) x2, (int) y2);

    draw_point_array(canvas_win, op, spline->thickness, spline->style,
		     spline->style_val, spline->area_fill);

    if (spline->for_arrow)	/* forward arrow  */
	draw_arrow((int) x1, (int) y1, (int) x2, (int) y2,
		   spline->for_arrow, op);
}

draw_closed_spline(spline, op)
    F_spline       *spline;
    int             op;
{
    F_point        *p;
    float           cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
    float           x1, y1, x2, y2;

    if (!init_point_array(300, 200))
	return;

    p = spline->points;
    x1 = p->x;
    y1 = p->y;
    p = p->next;
    x2 = p->x;
    y2 = p->y;
    cx1 = (x1 + x2) / 2;
    cy1 = (y1 + y2) / 2;
    cx2 = (x1 + 3 * x2) / 4;
    cy2 = (y1 + 3 * y2) / 4;

    for (p = p->next; p != NULL; p = p->next) {
	x1 = x2;
	y1 = y2;
	x2 = p->x;
	y2 = p->y;
	cx4 = (x1 + x2) / 2;
	cy4 = (y1 + y2) / 2;
	cx3 = (x1 + cx4) / 2;
	cy3 = (y1 + cy4) / 2;
	quadratic_spline(cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4, op,
		       spline->thickness, spline->style, spline->style_val);
	cx1 = cx4;
	cy1 = cy4;
	cx2 = (cx1 + x2) / 2;
	cy2 = (cy1 + y2) / 2;
    }
    x1 = x2;
    y1 = y2;
    p = spline->points->next;
    x2 = p->x;
    y2 = p->y;
    cx4 = (x1 + x2) / 2;
    cy4 = (y1 + y2) / 2;
    cx3 = (x1 + cx4) / 2;
    cy3 = (y1 + cy4) / 2;
    quadratic_spline(cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4, op,
		     spline->thickness, spline->style, spline->style_val);

    add_point((int) cx4, (int) cy4);

    draw_point_array(canvas_win, op, spline->thickness, spline->style,
		     spline->style_val, spline->area_fill);
}


/*********************** TEXT ***************************/

static char    *hidden_text_string = "<<>>";

draw_text(text, op)
    F_text         *text;
    int             op;
{
    PR_SIZE         size;
    int             x;
    int             xmin, ymin, xmax, ymax;

    text_bound(text, &xmin, &ymin, &xmax, &ymax);
    if (!overlapping(ZOOMX(xmin), ZOOMY(ymin), ZOOMX(xmax), ZOOMY(ymax),
		     clip_xmin, clip_ymin, clip_xmax, clip_ymax))
	return;


    x = text->base_x;
    if (text->type == T_CENTER_JUSTIFIED || text->type == T_RIGHT_JUSTIFIED) {
	size = pf_textwidth(text->font, psfont_text(text), text->size,
			    strlen(text->cstring), text->cstring);
	if (text->type == T_CENTER_JUSTIFIED)
	    x -= size.x / 2;
	else
	    x -= size.x;
    }
    if (hidden_text(text))
	pw_text(canvas_win, x, text->base_y, op, 0, 0, 12, hidden_text_string);
    else
	pw_text(canvas_win, x, text->base_y,
	      op, text->font, psfont_text(text), text->size, text->cstring);
}

/*********************** COMPOUND ***************************/

void
draw_compoundelements(c, op)
    F_compound     *c;
    int             op;
{
    F_line         *l;
    F_spline       *s;
    F_ellipse      *e;
    F_text         *t;
    F_arc          *a;
    F_compound     *c1;

    if (!overlapping(c->nwcorner.x, c->nwcorner.y, c->secorner.x, c->secorner.y,
		     clip_xmin, clip_ymin, clip_xmax, clip_ymax))
	return;

    for (l = c->lines; l != NULL; l = l->next) {
	draw_line(l, op);
    }
    for (s = c->splines; s != NULL; s = s->next) {
	draw_spline(s, op);
    }
    for (a = c->arcs; a != NULL; a = a->next) {
	draw_arc(a, op);
    }
    for (e = c->ellipses; e != NULL; e = e->next) {
	draw_ellipse(e, op);
    }
    for (t = c->texts; t != NULL; t = t->next) {
	draw_text(t, op);
    }
    for (c1 = c->compounds; c1 != NULL; c1 = c1->next) {
	draw_compoundelements(c1, op);
    }
}

/*************************** ARROWS ****************************

 draw arrow heading from (x1, y1) to (x2, y2)

****************************************************************/

draw_arrow(x1, y1, x2, y2, arrow, op)
    int             x1, y1, x2, y2, op;
    F_arrow        *arrow;
{
    float           x, y, xb, yb, dx, dy, l, sina, cosa;
    int             xc, yc, xd, yd;
    float           wid = arrow->wid, ht = arrow->ht;

    dx = x2 - x1;
    dy = y1 - y2;
    l = sqrt((double) (dx * dx + dy * dy));
    if (l == 0)
	return;
    sina = dy / l;
    cosa = dx / l;
    xb = x2 * cosa - y2 * sina;
    yb = x2 * sina + y2 * cosa;
    x = xb - ht;
    y = yb - wid / 2;
    xc = x * cosa + y * sina + .5;
    yc = -x * sina + y * cosa + .5;
    y = yb + wid / 2;
    xd = x * cosa + y * sina + .5;
    yd = -x * sina + y * cosa + .5;
    pw_vector(canvas_win, xc, yc, x2, y2, op,
	      (int) arrow->thickness, arrow->style, 0.0);
    pw_vector(canvas_win, xd, yd, x2, y2, op,
	      (int) arrow->thickness, arrow->style, 0.0);
}

draw_arcarrows(a, op)
    F_arc          *a;
    int             op;
{
    int             x, y;

    if (a->for_arrow) {
	compute_normal(a->center.x, a->center.y, a->point[2].x,
		       a->point[2].y, a->direction, &x, &y);
	draw_arrow(x, y, a->point[2].x, a->point[2].y, a->for_arrow, op);
    }
    if (a->back_arrow) {
	compute_normal(a->center.x, a->center.y, a->point[0].x,
		       a->point[0].y, a->direction ^ 1, &x, &y);
	draw_arrow(x, y, a->point[0].x, a->point[0].y,
		   a->back_arrow, op);
    }
}

/********************* CURVES FOR ARCS AND ELLIPSES ***************

 This routine plot two dimensional curve defined by a second degree
 polynomial of the form : 2    2 f(x, y) = ax + by + g = 0

 (x0,y0) is the starting point as well as ending point of the curve. The curve
 will translate with the offset xoff and yoff.

 This algorithm is derived from the eight point algorithm in : "An Improved
 Algorithm for the generation of Nonparametric Curves" by Bernard W.
 Jordan, William J. Lennon and Barry D. Holm, IEEE Transaction on Computers
 Vol C-22, No. 12 December 1973.

 Will fill the curve if area_fill is != 0

****************************************************************/

curve(window, xstart, ystart, xend, yend, direction,
      a, b, xoff, yoff, op, thick, style, style_val, area_fill)
    Window          window;
    int             xstart, ystart, xend, yend, a, b, xoff, yoff;
    int             direction, op, thick, style, area_fill;
    float           style_val;
{
    register int    deltax, deltay, dfx, dfy, x, y;
    int             dfxx, dfyy;
    int             falpha, fx, fy, fxy, absfx, absfy, absfxy;
    int             margin, test_succeed, inc, dec;

    if (a == 0 || b == 0)
	return;

    if (!init_point_array(a + b + 1, a + b))	/* the +1 is for end point */
	return;

    x = xstart;
    y = ystart;
    dfx = 2 * a * xstart;
    dfy = 2 * b * ystart;
    dfxx = 2 * a;
    dfyy = 2 * b;

    falpha = 0;
    if (direction) {
	inc = 1;
	dec = -1;
    } else {
	inc = -1;
	dec = 1;
    }
    if (xstart == xend && ystart == yend) {
	test_succeed = margin = 1;
    } else {
	test_succeed = 3;
	margin = 3;
    }

    while (test_succeed) {
	if (!add_point(xoff + x, yoff - y))
	    break;

	deltax = (dfy < 0) ? inc : dec;
	deltay = (dfx < 0) ? dec : inc;
	fx = falpha + dfx * deltax + a;
	fy = falpha + dfy * deltay + b;
	fxy = fx + fy - falpha;
	absfx = abs(fx);
	absfy = abs(fy);
	absfxy = abs(fxy);

	if ((absfxy <= absfx) && (absfxy <= absfy))
	    falpha = fxy;
	else if (absfy <= absfx) {
	    deltax = 0;
	    falpha = fy;
	} else {
	    deltay = 0;
	    falpha = fx;
	}
	x += deltax;
	y += deltay;
	dfx += (dfxx * deltax);
	dfy += (dfyy * deltay);
	if (abs(x - xend) < margin && abs(y - yend) < margin)
	    test_succeed--;
    }

    if (margin == 1)		/* end points should touch */
	add_point(xoff + xstart, yoff - ystart);

    draw_point_array(window, op, thick, style, style_val, area_fill);
}

/********************* CURVES FOR SPLINES *****************************

	The following spline drawing routine is from

	"An Algorithm for High-Speed Curve Generation"
	by George Merrill Chaikin,
	Computer Graphics and Image Processing, 3, Academic Press,
	1974, 346-349.

	and

	"On Chaikin's Algorithm" by R. F. Riesenfeld,
	Computer Graphics and Image Processing, 4, Academic Press,
	1975, 304-310.

***********************************************************************/

#define		half(z1, z2)	((z1+z2)/2.0)
#define		THRESHOLD	5

/* iterative version */
/*
 * because we draw the spline with small line segments, the style parameter
 * doesn't work
 */

quadratic_spline(a1, b1, a2, b2, a3, b3, a4, b4, op, thick, style, style_val)
    float           a1, b1, a2, b2, a3, b3, a4, b4;
    int             op, thick, style;
    float           style_val;
{
    register float  xmid, ymid;
    float           x1, y1, x2, y2, x3, y3, x4, y4;

    clear_stack();
    push(a1, b1, a2, b2, a3, b3, a4, b4);

    while (pop(&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4)) {
	xmid = half(x2, x3);
	ymid = half(y2, y3);
	if (fabs(x1 - xmid) < THRESHOLD && fabs(y1 - ymid) < THRESHOLD &&
	    fabs(xmid - x4) < THRESHOLD && fabs(ymid - y4) < THRESHOLD) {
	    add_point(round(x1), round(y1));
	    add_point(round(xmid), round(ymid));
	} else {
	    push(xmid, ymid, half(xmid, x3), half(ymid, y3),
		 half(x3, x4), half(y3, y4), x4, y4);
	    push(x1, y1, half(x1, x2), half(y1, y2),
		 half(x2, xmid), half(y2, ymid), xmid, ymid);
	}
    }
}

/*
 * the style parameter doesn't work for splines because we use small line
 * segments
 */

bezier_spline(a0, b0, a1, b1, a2, b2, a3, b3, op, thick, style, style_val)
    float           a0, b0, a1, b1, a2, b2, a3, b3;
    int             op, thick, style;
    float           style_val;
{
    register float  tx, ty;
    float           x0, y0, x1, y1, x2, y2, x3, y3;
    float           sx1, sy1, sx2, sy2, tx1, ty1, tx2, ty2, xmid, ymid;

    clear_stack();
    push(a0, b0, a1, b1, a2, b2, a3, b3);

    while (pop(&x0, &y0, &x1, &y1, &x2, &y2, &x3, &y3)) {
	if (fabs(x0 - x3) < THRESHOLD && fabs(y0 - y3) < THRESHOLD) {
	    add_point(round(x0), round(y0));
	} else {
	    tx = half(x1, x2);
	    ty = half(y1, y2);
	    sx1 = half(x0, x1);
	    sy1 = half(y0, y1);
	    sx2 = half(sx1, tx);
	    sy2 = half(sy1, ty);
	    tx2 = half(x2, x3);
	    ty2 = half(y2, y3);
	    tx1 = half(tx2, tx);
	    ty1 = half(ty2, ty);
	    xmid = half(sx2, tx1);
	    ymid = half(sy2, ty1);

	    push(xmid, ymid, tx1, ty1, tx2, ty2, x3, y3);
	    push(x0, y0, sx1, sy1, sx2, sy2, xmid, ymid);
	}
    }
}

/* utilities used by spline drawing routines */

#define		STACK_DEPTH		20

typedef struct stack {
    float           x1, y1, x2, y2, x3, y3, x4, y4;
}
                Stack;

static Stack    stack[STACK_DEPTH];
static Stack   *stack_top;
static int      stack_count;

clear_stack()
{
    stack_top = stack;
    stack_count = 0;
}

push(x1, y1, x2, y2, x3, y3, x4, y4)
    float           x1, y1, x2, y2, x3, y3, x4, y4;
{
    stack_top->x1 = x1;
    stack_top->y1 = y1;
    stack_top->x2 = x2;
    stack_top->y2 = y2;
    stack_top->x3 = x3;
    stack_top->y3 = y3;
    stack_top->x4 = x4;
    stack_top->y4 = y4;
    stack_top++;
    stack_count++;
}

int
pop(x1, y1, x2, y2, x3, y3, x4, y4)
    float          *x1, *y1, *x2, *y2, *x3, *y3, *x4, *y4;
{
    if (stack_count == 0)
	return (0);
    stack_top--;
    stack_count--;
    *x1 = stack_top->x1;
    *y1 = stack_top->y1;
    *x2 = stack_top->x2;
    *y2 = stack_top->y2;
    *x3 = stack_top->x3;
    *y3 = stack_top->y3;
    *x4 = stack_top->x4;
    *y4 = stack_top->y4;
    return (1);
}
