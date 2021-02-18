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
#include "u_list.h"
#include "u_undo.h"
#include "w_canvas.h"
#include "w_mousefun.h"
#include "w_setup.h"

static int      init_delete();
static int      init_delete_region(), delete_region(), cancel_delete_region();
static int      init_delete_to_scrap();

delete_selected()
{
    set_mousefun("delete object", "delete region", "del to cut buf");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    init_searchproc_left(init_delete);
    init_searchproc_right(init_delete_to_scrap);
    canvas_leftbut_proc = object_search_left;
    canvas_rightbut_proc = object_search_right;
    canvas_middlebut_proc = init_delete_region;
    set_cursor(&buster_cursor);
    reset_action_on();
}

static
init_delete(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y;
    int             px, py;
{
#ifdef FASTSERVER
    int             xmin, ymin, xmax, ymax;

#endif

    switch (type) {
    case O_COMPOUND:
	cur_c = (F_compound *) p;
	delete_compound(cur_c);
	toggle_compoundmarker(cur_c);
#ifdef FASTSERVER
	redisplay_zoomed_region(cur_c->nwcorner.x, cur_c->nwcorner.y,
				cur_c->secorner.x, cur_c->secorner.y);
#else
	draw_compoundelements(cur_c, ERASE);
#endif
	break;
    case O_POLYLINE:
	cur_l = (F_line *) p;
	delete_line(cur_l);
	toggle_linemarker(cur_l);
#ifdef FASTSERVER
	line_bound(cur_l, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
#else
	draw_line(cur_l, ERASE);
#endif
	break;
    case O_TEXT:
	cur_t = (F_text *) p;
	delete_text(cur_t);
	toggle_textmarker(cur_t);
#ifdef FASTSERVER
	text_bound(cur_t, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
#else
	draw_text(cur_t, ERASE);
#endif
	break;
    case O_ELLIPSE:
	cur_e = (F_ellipse *) p;
	delete_ellipse(cur_e);
	toggle_ellipsemarker(cur_e);
#ifdef FASTSERVER
	ellipse_bound(cur_e, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
#else
	draw_ellipse(cur_e, ERASE);
#endif
	break;
    case O_ARC:
	cur_a = (F_arc *) p;
	delete_arc(cur_a);
	toggle_arcmarker(cur_a);
#ifdef FASTSERVER
	arc_bound(cur_a, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
#else
	draw_arc(cur_a, ERASE);
#endif
	break;
    case O_SPLINE:
	cur_s = (F_spline *) p;
	delete_spline(cur_s);
	toggle_splinemarker(cur_s);
#ifdef FASTSERVER
	spline_bound(cur_s, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
#else
	draw_spline(cur_s, ERASE);
#endif
	break;
    default:
	return;
    }
}

static
init_delete_region(x, y)
    int             x, y;
{
    init_box_drawing(x, y);
    set_mousefun("", "final corner", "cancel");
    draw_mousefun_canvas();
    canvas_leftbut_proc = null_proc;
    canvas_middlebut_proc = delete_region;
    canvas_rightbut_proc = cancel_delete_region;
}

static
cancel_delete_region()
{
    elastic_box(fix_x, fix_y, cur_x, cur_y);
    delete_selected();
    draw_mousefun_canvas();
}

static
delete_region(x, y)
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
	delete_selected();
	draw_mousefun_canvas();
	put_msg("Empty region, figure unchanged");
	return;
    }
    clean_up();
    set_latestobjects(*c);
    tail(&objects, &object_tails);
    append_objects(&objects, &saved_objects, &object_tails);
    cut_objects(&objects, &object_tails);
    set_action_object(F_DELETE, O_ALL_OBJECT);
    set_modifiedflag();
    toggle_markers_in_compound(c);
#ifdef FASTSERVER
    redisplay_zoomed_region(c->nwcorner.x, c->nwcorner.y,
			    c->secorner.x, c->secorner.y);
#else
    draw_compoundelements(c, ERASE);
#endif
    delete_selected();
    draw_mousefun_canvas();
}

static
init_delete_to_scrap(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y;
    int             px, py;
{
    extern char     cut_buf_name[];
    extern char     file_header[];

    FILE           *fp;
    struct stat     file_status;

    if (stat(cut_buf_name, &file_status) == 0) {	/* file exists */
	if (file_status.st_mode & S_IFDIR) {
	    put_msg("Error: \"%s\" is a directory", cut_buf_name);
	    return;
	}
	if (file_status.st_mode & S_IWRITE) {	/* writing is permitted */
	    if (file_status.st_uid != geteuid()) {
		put_msg("Error: access denied to cut file");
		return;
	    }
	} else {
	    put_msg("Error: cut file is read only");
	    return;
	}
    } else if (errno != ENOENT) {
	put_msg("Error: cut file didn't pass stat check");
	return;			/* file does exist but stat fails */
    }
    if ((fp = fopen(cut_buf_name, "w")) == NULL) {
	put_msg("Error: couldn't open cut file %s", sys_errlist[errno]);
	return;
    } else {
	(void) fprintf(fp, "%s\n", file_header);
	(void) fprintf(fp, "%d %d\n", PIX_PER_INCH, 2);
    }

    switch (type) {
    case O_COMPOUND:
	cur_c = (F_compound *) p;
	write_compound(fp, cur_c);
	delete_compound(cur_c);
	toggle_compoundmarker(cur_c);
	draw_compoundelements(cur_c, ERASE);
	break;
    case O_POLYLINE:
	cur_l = (F_line *) p;
	write_line(fp, cur_l);
	delete_line(cur_l);
	toggle_linemarker(cur_l);
	draw_line(cur_l, ERASE);
	break;
    case O_TEXT:
	cur_t = (F_text *) p;
	write_text(fp, cur_t);
	delete_text(cur_t);
	toggle_textmarker(cur_t);
	draw_text(cur_t, ERASE);
	break;
    case O_ELLIPSE:
	cur_e = (F_ellipse *) p;
	write_ellipse(fp, cur_e);
	delete_ellipse(cur_e);
	toggle_ellipsemarker(cur_e);
	draw_ellipse(cur_e, ERASE);
	break;
    case O_ARC:
	cur_a = (F_arc *) p;
	write_arc(fp, cur_a);
	delete_arc(cur_a);
	toggle_arcmarker(cur_a);
	draw_arc(cur_a, ERASE);
	break;
    case O_SPLINE:
	cur_s = (F_spline *) p;
	write_spline(fp, cur_s);
	delete_spline(cur_s);
	toggle_splinemarker(cur_s);
	draw_spline(cur_s, ERASE);
	break;
    default:
	fclose(fp);
	return;
    }
    put_msg("Object deleted to scrap");
    fclose(fp);
}

delete_all()
{
    clean_up();
    set_action_object(F_DELETE, O_ALL_OBJECT);

    /*
     * Aggregate assignment between variables is allowed, but not from
     * constant (weird!?)
     */

    set_latestobjects(&objects);

    objects.arcs = NULL;
    objects.compounds = NULL;
    objects.ellipses = NULL;
    objects.lines = NULL;
    objects.splines = NULL;
    objects.texts = NULL;

    object_tails.arcs = NULL;
    object_tails.compounds = NULL;
    object_tails.ellipses = NULL;
    object_tails.lines = NULL;
    object_tails.splines = NULL;
    object_tails.texts = NULL;
}
