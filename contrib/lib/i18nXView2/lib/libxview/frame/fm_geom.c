#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)fm_geom.c 20.20 90/02/26";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/fm_impl.h>
#include <xview/window.h>
#include <xview/font.h>

extern Pixfont *xv_pf_sys;

static int      frame_sw_size_compute();


Pkg_private int
frame_height_from_lines(n, show_label)
    int             n, show_label;
{
    return (n * xv_get(xv_pf_sys, FONT_DEFAULT_CHAR_HEIGHT)) + FRAME_BORDER_WIDTH;
}

Pkg_private int
frame_width_from_columns(x)
    int             x;
{
    return (x * xv_get(xv_pf_sys, FONT_DEFAULT_CHAR_WIDTH)) + 
		(2 * FRAME_BORDER_WIDTH);
}

/*
 * Position subwindow in frame. This is only called by frame_layout for
 * XV_CREATE, so the width and height are always WIN_EXTEND_TO_EDGE.
 */
Pkg_private void
frame_position_sw(frame, swprevious, width, height, rect)
    Frame_class_info *frame;
    Xv_Window       swprevious;
    int             width, height;
    Rect           *rect;
{
    Rect            framerect;

    /*
     * Iconic adjustments
     */
    (void) frame_getnormalrect(frame, &framerect);

    /*
     * Set position: set up new subwindow (nsw) relative to previous
     * subwindow (psw) in the list. If psw.width is default then assume that
     * it extends to right edge and start nsw below it and flush left.
     * Otherwise, put to the right of it. Use outer rect for calculations but
     * return inner rect
     */
    if (swprevious) {
	win_get_outer_rect(swprevious, rect);	/* outer rect */
	if (EXTEND_WIDTH(swprevious)) {
	    rect->r_top += rect->r_height;
	    rect->r_left = FRAME_BORDER_WIDTH;
	} else {
	    rect->r_left += rect->r_width;
	}
    } else {
	rect->r_top = FRAME_BORDER_WIDTH;	/* frame_stripe_height(frame) */
	rect->r_left = FRAME_BORDER_WIDTH;
    }
    /*
     * Compute width & height
     */
    rect->r_width = frame_sw_size_compute(width,
					  framerect.r_width - rect->r_left);	/*- FRAME_BORDER_WIDTH);*/
    rect->r_height = frame_sw_size_compute(height,
					   framerect.r_height - rect->r_top);	/* -
										 * FRAME_BORDER_WIDTH); */
    if (swprevious)
	window_outer_to_innerrect(swprevious, rect);	/* decrease the rect if
							 * the window
							 * (swprevious) has a
							 * border -- get inner
							 * border */
}

/*
 * Set size: set up new subwindow (nsw) size in available space (avs). If avs
 * is <=0 then set nsw to be requested amount. If avs is > 0 then set nsw =
 * min(requested amount, avs). If nsw = default then use avs unless avs = 0
 * inwhich case choose some small constant.  We don't want 0 size.
 */
static int
frame_sw_size_compute(request, avs)
    int             request, avs;
{
    int             nsw;

    if (request == WIN_EXTEND_TO_EDGE) {
	nsw = (avs <= 0) ? 16 : avs;
    } else
	nsw = request;
    if (avs > 0)
	nsw = MIN(nsw, avs);
    return (nsw);
}
