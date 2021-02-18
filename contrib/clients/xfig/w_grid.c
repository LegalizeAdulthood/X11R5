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
#include "paintop.h"
#include "object.h"
#include "w_setup.h"
#include "w_util.h"
#include "w_zoom.h"

#define null_width 32
#define null_height 32
static char     null_bits[null_width * null_height / 8] = {0};

static Pixmap   null_pm, grid_pm = 0;
static unsigned long bg, fg;

init_grid()
{
    DeclareArgs(2);

    FirstArg(XtNbackground, &bg);
    NextArg(XtNforeground, &fg);
    GetValues(canvas_sw);

    null_pm = XCreatePixmapFromBitmapData(tool_d, canvas_win,
					  null_bits, null_width, null_height,
				      fg, bg, DefaultDepthOfScreen(tool_s));

    if (appres.INCHES) {
	posn_rnd[P_MAGNET] = 5;
	posn_hlf[P_MAGNET] = 3;
	posn_rnd[P_GRID1] = 20;
	posn_hlf[P_GRID1] = 10;
	posn_rnd[P_GRID2] = 40;
	posn_hlf[P_GRID2] = 20;
    } else {
	posn_rnd[P_MAGNET] = 6;
	posn_hlf[P_MAGNET] = 3;
	posn_rnd[P_GRID1] = 15;
	posn_hlf[P_GRID1] = 7;
	posn_rnd[P_GRID2] = 30;
	posn_hlf[P_GRID2] = 15;
    }
}

/* grid in X11 is simply the background of the canvas */

setup_grid(grid)
    int             grid;
{
    int             coarse, fine, dim, x, x0c, x0f, y, y0c, y0f;
    static          prev_grid = -1;

    DeclareArgs(1);

    if (grid == GRID_0) {
	FirstArg(XtNbackgroundPixmap, null_pm);
    } else {
	if (grid == GRID_1) {
	    coarse = (int) (posn_rnd[P_GRID1] * zoomscale);
	    if (appres.INCHES)
		fine = (int) (posn_rnd[P_MAGNET] * zoomscale);
	    else
		fine = (int) (posn_hlf[P_MAGNET] * zoomscale);
	} else {
	    coarse = (int) (posn_rnd[P_GRID2] * zoomscale);
	    fine = (int) (posn_rnd[P_MAGNET] * zoomscale);
	}
	dim = (coarse > 64) ? coarse : (64 / coarse + 1) * coarse;
	if (grid_pm)
	    XFreePixmap(tool_d, grid_pm);
	grid_pm = XCreatePixmap(tool_d, canvas_win, dim, dim,
				DefaultDepthOfScreen(tool_s));
	XSetForeground(tool_d, gc, bg);
	XFillRectangle(tool_d, grid_pm, gc, 0, 0, dim, dim);
	XSetForeground(tool_d, gc, fg);
	x0c = -(zoomscale * zoomxoff) % coarse - zoomscale;
	y0c = -(zoomscale * zoomyoff) % coarse - zoomscale;
	x0f = -(zoomscale * zoomxoff) % fine - zoomscale;
	y0f = -(zoomscale * zoomyoff) % fine - zoomscale;
	for (x = x0c; x < dim; x += coarse)
	    for (y = y0f; y < dim; y += fine)
		XDrawPoint(tool_d, grid_pm, gc, x, y);
	for (y = y0c; y < dim; y += coarse)
	    for (x = x0f; x < dim; x += fine)
		XDrawPoint(tool_d, grid_pm, gc, x, y);
	FirstArg(XtNbackgroundPixmap, grid_pm);
    }
    SetValues(canvas_sw);
    if (prev_grid == GRID_0 && grid == GRID_0)
	redisplay_canvas();
    prev_grid = grid;
}

redisplay_grid()
{
}
