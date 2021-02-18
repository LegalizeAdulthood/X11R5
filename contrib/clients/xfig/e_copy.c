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
#include "u_elastic.h"
#include "u_search.h"
#include "u_create.h"
#include "w_canvas.h"
#include "w_mousefun.h"
#include "w_setup.h"

/* local routine declarations */
static          init_copy(), init_arb_copy(), init_constrained_copy();
static          init_copy_to_scrap();

copy_selected()
{
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    init_searchproc_left(init_arb_copy);
    init_searchproc_middle(init_constrained_copy);
    init_searchproc_right(init_copy_to_scrap);
    canvas_leftbut_proc = object_search_left;
    canvas_middlebut_proc = object_search_middle;
    canvas_rightbut_proc = object_search_right;
    return_proc = copy_selected;
    set_cursor(&pick15_cursor);
    set_mousefun("copy object", "horiz/vert copy", "copy to cut buf");
    reset_action_on();
}

static
init_arb_copy(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y, px, py;
{
    constrained = MOVE_ARB;
    init_copy(p, type, x, y, px, py);
    canvas_middlebut_proc = null_proc;
    set_mousefun("place object", "", "cancel");
    draw_mousefun_canvas();
}

static
init_constrained_copy(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y, px, py;
{
    constrained = MOVE_HORIZ_VERT;
    init_copy(p, type, x, y, px, py);
    canvas_middlebut_proc = canvas_leftbut_proc;
    canvas_leftbut_proc = null_proc;
    set_mousefun("", "place object", "cancel");
    draw_mousefun_canvas();
}

static
init_copy(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y, px, py;
{
    switch (type) {
    case O_COMPOUND:
	set_temp_cursor(&null_cursor);
	cur_c = (F_compound *) p;
	new_c = copy_compound(cur_c);
	init_compounddragging(new_c, px, py);
	break;
    case O_POLYLINE:
	set_temp_cursor(&null_cursor);
	cur_l = (F_line *) p;
	new_l = copy_line(cur_l);
	init_linedragging(new_l, px, py);
	break;
    case O_TEXT:
	set_temp_cursor(&null_cursor);
	cur_t = (F_text *) p;
	new_t = copy_text(cur_t);
	init_textdragging(new_t, x, y);
	break;
    case O_ELLIPSE:
	set_temp_cursor(&null_cursor);
	cur_e = (F_ellipse *) p;
	new_e = copy_ellipse(cur_e);
	init_ellipsedragging(new_e, px, py);
	break;
    case O_ARC:
	set_temp_cursor(&null_cursor);
	cur_a = (F_arc *) p;
	new_a = copy_arc(cur_a);
	init_arcdragging(new_a, px, py);
	break;
    case O_SPLINE:
	set_temp_cursor(&null_cursor);
	cur_s = (F_spline *) p;
	new_s = copy_spline(cur_s);
	init_splinedragging(new_s, px, py);
	break;
    default:
	return;
    }
}

static
init_copy_to_scrap(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y;
    int             px, py;
{
    FILE           *fp;
    struct stat     file_status;

    if (stat(cut_buf_name, &file_status) == 0) {	/* file exists */
	if (file_status.st_mode & S_IFDIR) {
	    put_msg("\"%s\" is a directory", cut_buf_name);
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
    } else if (errno != ENOENT)
	return;			/* file does exist but stat fails */

    if ((fp = fopen(cut_buf_name, "w")) == NULL) {
	put_msg("Couldn't open cut file %s", sys_errlist[errno]);
	return;
    } else {
	(void) fprintf(fp, "%s\n", file_header);
	(void) fprintf(fp, "%d %d\n", PIX_PER_INCH, 2);
    }

    switch (type) {
    case O_COMPOUND:
	cur_c = (F_compound *) p;
	write_compound(fp, cur_c);
	break;
    case O_ARC:
	cur_a = (F_arc *) p;
	write_arc(fp, cur_a);
	break;
    case O_ELLIPSE:
	cur_e = (F_ellipse *) p;
	write_ellipse(fp, cur_e);
	break;
    case O_POLYLINE:
	cur_l = (F_line *) p;
	write_line(fp, cur_l);
	break;
    case O_TEXT:
	cur_t = (F_text *) p;
	write_text(fp, cur_t);
	break;
    case O_SPLINE:
	cur_s = (F_spline *) p;
	write_spline(fp, cur_s);
	break;
    default:
	fclose(fp);
	return;
    }
    put_msg("Object copied to scrap");
    fclose(fp);
}
