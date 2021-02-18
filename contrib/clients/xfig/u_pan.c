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
#include "resources.h"
#include "mode.h"
#include "w_zoom.h"

pan_left()
{
    zoomxoff += posn_rnd[P_GRID2];
    reset_topruler();
    redisplay_topruler();
    setup_grid(cur_gridmode);
}

pan_right()
{
    if (zoomxoff == 0)
	return;
    zoomxoff -= posn_rnd[P_GRID2];
    if (zoomxoff < 0)
	zoomxoff = 0;
    reset_topruler();
    redisplay_topruler();
    setup_grid(cur_gridmode);
}

pan_up()
{
    zoomyoff += posn_rnd[P_GRID2];
    reset_sideruler();
    redisplay_sideruler();
    setup_grid(cur_gridmode);
}

pan_down()
{
    if (zoomyoff == 0)
	return;
    zoomyoff -= posn_rnd[P_GRID2];
    if (zoomyoff < 0)
	zoomyoff = 0;
    reset_sideruler();
    redisplay_sideruler();
    setup_grid(cur_gridmode);
}

pan_origin()
{
    if (zoomxoff == 0 && zoomyoff == 0)
	return;
    if (zoomyoff != 0) {
	zoomyoff = 0;
	setup_sideruler();
	redisplay_sideruler();
    }
    if (zoomxoff != 0) {
	zoomxoff = 0;
	reset_topruler();
	redisplay_topruler();
    }
    setup_grid(cur_gridmode);
}
