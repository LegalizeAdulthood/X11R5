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
#include "resources.h"
#include "mode.h"
#include "paintop.h"
#include "u_create.h"
#include "u_list.h"
#include "u_search.h"
#include "w_canvas.h"
#include "w_drawprim.h"
#include "w_mousefun.h"

extern          update_current_settings();
static int      init_update_object();
static int      init_update_settings();

update_selected()
{
    set_mousefun("update object", "update settings", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    init_searchproc_left(init_update_object);
    init_searchproc_middle(init_update_settings);
    canvas_leftbut_proc = object_search_left;
    canvas_middlebut_proc = object_search_middle;
    canvas_rightbut_proc = null_proc;
    set_cursor(&pick9_cursor);
}

static
init_update_settings(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y;
    int             px, py;
{
    switch (type) {
    case O_COMPOUND:
	put_msg("There is no support for updating settings from a compound object");
	return;
    case O_POLYLINE:
	cur_l = (F_line *) p;
	cur_linewidth = cur_l->thickness;
	cur_areafill = cur_l->area_fill;
	cur_color = cur_l->color;
	cur_linestyle = cur_l->style;
	cur_styleval = cur_l->style_val;
	if (cur_l->type == T_ARC_BOX)
	    cur_boxradius = cur_l->radius;
	break;
    case O_TEXT:
	cur_t = (F_text *) p;
	cur_color = cur_t->color;
	cur_textjust = cur_t->type;
	cur_textflags = cur_t->flags;
	if (using_ps)
	    cur_ps_font = cur_t->font;
	else
	    cur_latex_font = cur_t->font;
	cur_fontsize = cur_t->size;
	cur_angle = cur_t->angle;
	break;
    case O_ELLIPSE:
	cur_e = (F_ellipse *) p;
	cur_linewidth = cur_e->thickness;
	cur_areafill = cur_e->area_fill;
	cur_color = cur_e->color;
	cur_linestyle = cur_e->style;
	cur_styleval = cur_e->style_val;
	break;
    case O_ARC:
	cur_a = (F_arc *) p;
	cur_linewidth = cur_a->thickness;
	cur_areafill = cur_a->area_fill;
	cur_color = cur_a->color;
	cur_linestyle = cur_a->style;
	cur_styleval = cur_a->style_val;
	break;
    case O_SPLINE:
	cur_s = (F_spline *) p;
	cur_linewidth = cur_s->thickness;
	cur_areafill = cur_s->area_fill;
	cur_color = cur_s->color;
	cur_linestyle = cur_s->style;
	cur_styleval = cur_s->style_val;
	break;
    default:
	return;
    }
    update_current_settings();
    put_msg("Settings UPDATED");
}

static
init_update_object(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y;
    int             px, py;
{
    switch (type) {
    case O_COMPOUND:
	set_temp_cursor(&wait_cursor);
	cur_c = (F_compound *) p;
	toggle_compoundmarker(cur_c);
	new_c = copy_compound(cur_c);
	update_compound(new_c);
	change_compound(cur_c, new_c);
	toggle_compoundmarker(new_c);
	break;
    case O_POLYLINE:
	set_temp_cursor(&wait_cursor);
	cur_l = (F_line *) p;
	toggle_linemarker(cur_l);
	new_l = copy_line(cur_l);
	update_line(new_l);
	change_line(cur_l, new_l);
	toggle_linemarker(new_l);
	break;
    case O_TEXT:
	set_temp_cursor(&wait_cursor);
	cur_t = (F_text *) p;
	toggle_textmarker(cur_t);
	new_t = copy_text(cur_t);
	update_text(new_t);
	change_text(cur_t, new_t);
	toggle_textmarker(new_t);
	break;
    case O_ELLIPSE:
	set_temp_cursor(&wait_cursor);
	cur_e = (F_ellipse *) p;
	toggle_ellipsemarker(cur_e);
	new_e = copy_ellipse(cur_e);
	update_ellipse(new_e);
	change_ellipse(cur_e, new_e);
	toggle_ellipsemarker(new_e);
	break;
    case O_ARC:
	set_temp_cursor(&wait_cursor);
	cur_a = (F_arc *) p;
	toggle_arcmarker(cur_a);
	new_a = copy_arc(cur_a);
	update_arc(new_a);
	change_arc(cur_a, new_a);
	toggle_arcmarker(new_a);
	break;
    case O_SPLINE:
	set_temp_cursor(&wait_cursor);
	cur_s = (F_spline *) p;
	toggle_splinemarker(cur_s);
	new_s = copy_spline(cur_s);
	update_spline(new_s);
	change_spline(cur_s, new_s);
	toggle_splinemarker(new_s);
	break;
    default:
	return;
    }
    reset_cursor();
    put_msg("Object(s) UPDATED");
}

update_ellipse(ellipse)
    F_ellipse      *ellipse;
{
    draw_ellipse(ellipse, ERASE);
    ellipse->thickness = cur_linewidth;
    ellipse->style = cur_linestyle;
    ellipse->area_fill = cur_areafill;
    ellipse->color = cur_color;
    ellipse->style_val = cur_styleval * (cur_linewidth + 1) / 2;
    draw_ellipse(ellipse, PAINT);
}

update_arc(arc)
    F_arc          *arc;
{
    draw_arc(arc, ERASE);
    arc->thickness = cur_linewidth;
    arc->style = cur_linestyle;
    arc->area_fill = cur_areafill;
    arc->color = cur_color;
    arc->style_val = cur_styleval * (cur_linewidth + 1) / 2;
    if (autoforwardarrow_mode)
	arc->for_arrow = forward_arrow();
    else
	arc->for_arrow = NULL;
    if (autobackwardarrow_mode)
	arc->back_arrow = backward_arrow();
    else
	arc->back_arrow = NULL;
    draw_arc(arc, PAINT);
}

update_line(line)
    F_line         *line;
{
    draw_line(line, ERASE);
    line->thickness = cur_linewidth;
    line->style = cur_linestyle;
    line->color = cur_color;
    line->style_val = cur_styleval * (cur_linewidth + 1) / 2;
    line->radius = cur_boxradius;
    line->area_fill = cur_areafill;
    if (line->type != T_POLYGON && line->type != T_BOX &&
	line->type != T_ARC_BOX && line->points->next != NULL) {
	if (autoforwardarrow_mode)
	    line->for_arrow = forward_arrow();
	else
	    line->for_arrow = NULL;
	if (autobackwardarrow_mode)
	    line->back_arrow = backward_arrow();
	else
	    line->back_arrow = NULL;
    }
    draw_line(line, PAINT);
}

update_text(text)
    F_text         *text;
{
    PR_SIZE         size;

    draw_text(text, ERASE);
    text->type = cur_textjust;
    text->font = using_ps ? cur_ps_font : cur_latex_font;
    text->size = cur_fontsize;
    text->angle = cur_angle;
    text->color = cur_color;
    size = pf_textwidth(text->font, psfont_text(text), text->size,
			strlen(text->cstring), text->cstring);
    text->length = size.x;	/* in pixels */
    text->height = size.y;	/* in pixels */
    draw_text(text, PAINT);
}

update_spline(spline)
    F_spline       *spline;
{
    draw_spline(spline, ERASE);
    spline->thickness = cur_linewidth;
    spline->style = cur_linestyle;
    spline->color = cur_color;
    spline->style_val = cur_styleval * (cur_linewidth + 1) / 2;
    spline->area_fill = cur_areafill;
    if (open_spline(spline)) {
	if (autoforwardarrow_mode)
	    spline->for_arrow = forward_arrow();
	else
	    spline->for_arrow = NULL;
	if (autobackwardarrow_mode)
	    spline->back_arrow = backward_arrow();
	else
	    spline->back_arrow = NULL;
    }
    draw_spline(spline, PAINT);
}

update_compound(compound)
    F_compound     *compound;
{
    update_lines(compound->lines);
    update_splines(compound->splines);
    update_ellipses(compound->ellipses);
    update_arcs(compound->arcs);
    update_texts(compound->texts);
    update_compounds(compound->compounds);
    compound_bound(compound, &compound->nwcorner.x, &compound->nwcorner.y,
		   &compound->secorner.x, &compound->secorner.y);
}

update_arcs(arcs)
    F_arc          *arcs;
{
    F_arc          *a;

    for (a = arcs; a != NULL; a = a->next)
	update_arc(a);
}

update_compounds(compounds)
    F_compound     *compounds;
{
    F_compound     *c;

    for (c = compounds; c != NULL; c = c->next)
	update_compound(c);
}

update_ellipses(ellipses)
    F_ellipse      *ellipses;
{
    F_ellipse      *e;

    for (e = ellipses; e != NULL; e = e->next)
	update_ellipse(e);
}

update_lines(lines)
    F_line         *lines;
{
    F_line         *l;

    for (l = lines; l != NULL; l = l->next)
	update_line(l);
}

update_splines(splines)
    F_spline       *splines;
{
    F_spline       *s;

    for (s = splines; s != NULL; s = s->next)
	update_spline(s);
}

update_texts(texts)
    F_text         *texts;
{
    F_text         *t;

    for (t = texts; t != NULL; t = t->next)
	update_text(t);
}
