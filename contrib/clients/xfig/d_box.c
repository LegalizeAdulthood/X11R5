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
#include "u_elastic.h"
#include "u_list.h"
#include "w_canvas.h"
#include "w_mousefun.h"

/*************************** local declarations *********************/

int             init_box_drawing();
static          create_boxobject(), cancel_box();

box_drawing_selected()
{
    set_mousefun("corner point", "", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_leftbut_proc = init_box_drawing;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(&arrow_cursor);
    reset_action_on();
}

init_box_drawing(x, y)
    int             x, y;
{
    cur_x = fix_x = x;
    cur_y = fix_y = y;
    set_mousefun("final point", "", "cancel");
    draw_mousefun_canvas();
    canvas_locmove_proc = resizing_box;
    canvas_leftbut_proc = create_boxobject;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = cancel_box;
    elastic_box(fix_x, fix_y, cur_x, cur_y);
    set_temp_cursor(&null_cursor);
    set_action_on();
}

static
cancel_box()
{
    elastic_box(fix_x, fix_y, cur_x, cur_y);
    box_drawing_selected();
    draw_mousefun_canvas();
}

static
create_boxobject(x, y)
    int             x, y;
{
    F_line         *box;
    F_point        *point;

    elastic_box(fix_x, fix_y, cur_x, cur_y);

    if ((point = create_point()) == NULL)
	return;

    point->x = x;
    point->y = y;
    point->next = NULL;

    if ((box = create_line()) == NULL) {
	free((char *) point);
	return;
    }
    box->type = T_BOX;
    box->style = cur_linestyle;
    box->thickness = cur_linewidth;
    box->color = cur_color;
    box->depth = 0;
    box->pen = 0;
    box->area_fill = cur_areafill;
    /* scale dash length by line thickness */
    box->style_val = cur_styleval * (cur_linewidth + 1) / 2;
    box->radius = 0;
    box->points = point;
    append_point(x, fix_y, &point);
    append_point(fix_x, fix_y, &point);
    append_point(fix_x, y, &point);
    append_point(x, y, &point);
    draw_line(box, PAINT);
    add_line(box);
    box_drawing_selected();
    draw_mousefun_canvas();
}
