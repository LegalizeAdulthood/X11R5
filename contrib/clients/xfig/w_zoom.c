/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1991 by Henning Spruth (hns@regent.e-technik.tu-muenchen.de)
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
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "resources.h"
#include "u_create.h"
#include "u_elastic.h"
#include "w_canvas.h"
#include "w_setup.h"
#include "w_zoom.h"

extern          elastic_box();

/* extern int              gc_thickness[NUMOPS]; */

static          do_zoom();
static          zoom_up();
static          init_zoombox_drawing();

int            zoomscale = 1;
int            zoomxoff = 0;
int            zoomyoff = 0;

#ifdef notdef
zoom_canvas_selected()
{
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_leftbut_proc = init_zoombox_drawing;
    canvas_middlebut_proc = zoom_up;
    canvas_rightbut_proc = null_proc;
    set_cursor(&arrow_cursor);
    reset_action_on();
}

static
init_zoombox_drawing(x, y)
    int             x, y;
{
    cur_x = fix_x = x;
    cur_y = fix_y = y;
    canvas_locmove_proc = elastic_box;
    canvas_leftbut_proc = canvas_rightbut_proc = null_proc;
    canvas_middlebut_proc = do_zoom;
    elastic_box(fix_x, fix_y, cur_x, cur_y);
    set_temp_cursor(&null_cursor);
    set_action_on();
}

static
do_zoom(x, y)
    int             x, y;
{
    int             dimx, dimy;
    int             t;		/* loop counter */
    float           scalex, scaley;

    elastic_box(fix_x, fix_y, cur_x, cur_y);
    zoomxoff = fix_x < x ? fix_x : x;
    zoomyoff = fix_y < y ? fix_y : y;
    dimx = abs(x - fix_x);
    dimy = abs(y - fix_y);
    if (zoomxoff < 0)
	zoomxoff = 0;
    if (zoomyoff < 0)
	zoomyoff = 0;
    if (dimx && dimy) {
	scalex = CANVAS_WD / (float) dimx;
	scaley = CANVAS_HT / (float) dimy;
	zoomscale = (int) scalex > scaley ? scaley : scalex;
	if (zoomscale > 9)
	    zoomscale = 9;
	/* show_zoomscale(); */
	/* for (t=0;t<NUMOPS;t++) gc_thickness[t]= -1; */
	reset_rulers();
	redisplay_rulers();
	setup_grid(cur_gridmode);
    }
    zoom_canvas_selected();
}

static
zoom_up()
{
    int             t;		/* loop counter */

    if (zoomscale > 1) {
	zoomscale--;
	/* show_zoomscale(); */
	/* for (t=0;t<NUMOPS;t++) gc_thickness[t]= -1; */
	reset_rulers();
	redisplay_rulers();
	setup_grid(cur_gridmode);
    }
    zoom_canvas_selected();
}

zoom_scale_1()
{
    int             t;		/* loop counter */

    zoomscale = 1;
    zoomxoff = 0;
    zoomyoff = 0;
    /* show_zoomscale(); */
    reset_rulers();
    /* for (t=0;t<NUMOPS;t++) gc_thickness[t]= -1; */
    redisplay_rulers();
    setup_grid(cur_gridmode);
}
#endif
