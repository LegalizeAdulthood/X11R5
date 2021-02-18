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
#include "u_search.h"
#include "u_list.h"
#include "u_undo.h"
#include "w_canvas.h"
#include "w_mousefun.h"

static int      init_break();

break_selected()
{
    set_mousefun("break compound", "", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    init_searchproc_left(init_break);
    canvas_leftbut_proc = object_search_left;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(&pick15_cursor);
}

static
init_break(p, type, x, y, px, py)
    char           *p;
    int             type;
    int             x, y;
    int             px, py;
{
    if (type != O_COMPOUND)
	return;

    cur_c = (F_compound *) p;
    mask_toggle_compoundmarker(cur_c);
    clean_up();
    list_delete_compound(&objects.compounds, cur_c);
    tail(&objects, &object_tails);
    append_objects(&objects, cur_c, &object_tails);
    toggle_markers_in_compound(cur_c);
    set_action(F_BREAK);
    set_latestcompound(cur_c);
    set_modifiedflag();
}
