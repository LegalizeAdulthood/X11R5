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
#include "u_list.h"
#include "u_search.h"
#include "w_canvas.h"
#include "w_mousefun.h"

static int      init_convert();

convert_selected()
{
    set_mousefun("spline<->line", "", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    init_searchproc_left(init_convert);
    canvas_leftbut_proc = object_search_left;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(&pick15_cursor);
}

static
init_convert(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y;
    int             px, py;
{
    switch (type) {
    case O_POLYLINE:
	cur_l = (F_line *) p;
	/* the search routine will ensure that we don't have a box */
	line_2_spline(cur_l);
	break;
    case O_SPLINE:
	cur_s = (F_spline *) p;
	/* the search routine will ensure that we have a interp spline */
	spline_2_line(cur_s);
	break;
    default:
	return;
    }
}

line_2_spline(l)
    F_line         *l;
{
    F_spline       *s;

    if (num_points(l->points) < 3) {
	put_msg("Can't CONVERT this line into a spline: insufficient points");
	return;
    }
    if ((s = create_spline()) == NULL)
	return;

    if (l->type == T_POLYGON)
	s->type = T_CLOSED_INTERP;
    else
	s->type = T_OPEN_INTERP;
    s->style = l->style;
    s->thickness = l->thickness;
    s->color = l->color;
    s->depth = l->depth;
    s->style_val = l->style_val;
    s->pen = l->pen;
    s->area_fill = l->area_fill;
    s->for_arrow = l->for_arrow;
    s->back_arrow = l->back_arrow;
    s->points = l->points;
    s->controls = NULL;
    s->next = NULL;

    if (-1 == create_control_list(s)) {
	free_splinestorage(s);
	return;
    }
    remake_control_points(s);

    /* now we have finished creating the spline, we can get rid of the line */
    /* first off the screen */
    mask_toggle_linemarker(l);
    draw_line(l, ERASE);
    list_delete_line(&objects.lines, l);
    /* we reuse the arrows and points, so `detach' them from the line */
    l->for_arrow = l->back_arrow = NULL;
    l->points = NULL;
    /* now get rid of the rest */
    free_linestorage(l);

    /* now put back the new spline */
    draw_spline(s, PAINT);
    mask_toggle_splinemarker(s);
    list_add_spline(&objects.splines, s);
    clean_up();
    set_action_object(F_CONVERT, O_POLYLINE);
    set_latestspline(s);
    return;
}

spline_2_line(s)
    F_spline       *s;
{
    F_line         *l;

    /* Now we turn s into a line */
    if ((l = create_line()) == NULL)
	return;

    if (s->type == T_OPEN_INTERP)
	l->type = T_POLYLINE;
    else if (s->type == T_CLOSED_INTERP)
	l->type = T_POLYGON;
    l->style = s->style;
    l->thickness = s->thickness;
    l->color = s->color;
    l->depth = s->depth;
    l->style_val = s->style_val;
    l->pen = s->pen;
    l->radius = DEF_BOXRADIUS;
    l->area_fill = s->area_fill;
    l->for_arrow = s->for_arrow;
    l->back_arrow = s->back_arrow;
    l->points = s->points;

    /* now we have finished creating the line, we can get rid of the spline */
    /* first off the screen */
    mask_toggle_splinemarker(s);
    draw_spline(s, ERASE);
    list_delete_spline(&objects.splines, s);
    /* we reuse the arrows and points, so `detach' them from the spline */
    s->for_arrow = s->back_arrow = NULL;
    s->points = NULL;
    /* now get rid of the rest */
    free_splinestorage(s);

    /* and put in the new line */
    draw_line(l, PAINT);
    mask_toggle_linemarker(l);
    list_add_line(&objects.lines, l);
    clean_up();
    set_action_object(F_CONVERT, O_SPLINE);
    set_latestline(l);
    return;
}
