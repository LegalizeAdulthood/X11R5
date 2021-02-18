#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)fm_bdry.c 20.15 90/05/10";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Frame interactive boundary mgt.
 */

#include <xview_private/fm_impl.h>

extern void     frame_compute_constraint();

static void     compute_shadows(),
                constrain_from_frame();

/*
 * Compute the constraining rect imposed by the frame. FRAME_BORDER_WIDTH
 * needs to be non zero
 */
static void
constrain_from_frame(frame, rconstrain)
    Frame_class_info *frame;
    register Rect  *rconstrain;
{
    *rconstrain = frame->rectcache;
    rconstrain->r_left += FRAME_BORDER_WIDTH;
    rconstrain->r_width -= 2 * FRAME_BORDER_WIDTH;
    rconstrain->r_top += FRAME_BORDER_WIDTH;	/* only 2 before?? */
    rconstrain->r_height -= 2 * FRAME_BORDER_WIDTH;	/* was a + here?? */
}

/*
 * Constrain target_sw according to its shadows.  rconstrain is computed as
 * the tallest, lowest, and widest that target_sw can be.  Note that if frame
 * is iconic, rconstrain will be offset by the usual TOOL_ICONIC_OFFSET.
 */
void
frame_compute_constraint(frame, target_sw, rconstrain)
    Frame_class_info *frame;
    Xv_Window       target_sw;
    register Rect  *rconstrain;
{
    Xv_Window       sw;
    Rect            rect;
    register int    constraint;
    /* register short	swsp = FRAME_SUBWINDOW_SPACING / 2; */
    Rect            left_rect, right_rect;
    Rect            top_rect, bottom_rect;
    register Rect  *left_shadow = &left_rect, *right_shadow = &right_rect;
    register Rect  *top_shadow = &top_rect, *bottom_shadow = &bottom_rect;

    constrain_from_frame(frame, rconstrain);
    /* compute constraint for the outer borders */
    /* rect_marginadjust(rconstrain, swsp); */


    compute_shadows(frame, target_sw, left_shadow, right_shadow,
		    top_shadow, bottom_shadow);

    /* For each subwindow in the shadow */
    FRAME_EACH_SHOWN_SUBWINDOW(frame, sw)
	if (sw == target_sw)
	continue;

    win_get_outer_rect(sw, &rect);	/* outer rect */
    rect_borderadjust(&rect, WIN_DEFAULT_BORDER_WIDTH);

    if (rect_intersectsrect(&rect, left_shadow)) {
	constraint = rect_right(&rect) + 1;
	if (constraint > rconstrain->r_left) {
	    rconstrain->r_width -= constraint - rconstrain->r_left;
	    rconstrain->r_left = constraint;
	}
    } else if (rect_intersectsrect(&rect, right_shadow)) {
	constraint = rect.r_left - 1;
	if (constraint < rect_right(rconstrain))
	    rconstrain->r_width -= rect_right(rconstrain) - constraint;
    }
    if (rect_intersectsrect(&rect, top_shadow)) {
	constraint = rect_bottom(&rect) + 1;
	if (constraint > rconstrain->r_top) {
	    rconstrain->r_height -= constraint - rconstrain->r_top;
	    rconstrain->r_top = constraint;
	}
    } else if (rect_intersectsrect(&rect, bottom_shadow)) {
	constraint = rect.r_top - 1;
	if (constraint < rect_bottom(rconstrain))
	    rconstrain->r_height -= rect_bottom(rconstrain) - constraint;
    }
    FRAME_END_EACH
}

/*
 * Compute the shadow rects for target_sw.
 */
static void
compute_shadows(frame, target_sw, left_shadow, right_shadow,
		top_shadow, bottom_shadow)
    register Frame_class_info *frame;
    Xv_Window       target_sw;
    register Rect  *left_shadow, *right_shadow;
    register Rect  *top_shadow, *bottom_shadow;
{
    Rect            target_rect;

    /* Compute the min & max shadow rects */
    win_get_outer_rect(target_sw, &target_rect);

    rect_construct(left_shadow, 0, target_rect.r_top,
		   target_rect.r_left, target_rect.r_height);
    rect_construct(right_shadow, rect_right(&target_rect), target_rect.r_top,
		   frame->rectcache.r_width - rect_right(&target_rect),
		   target_rect.r_height);

    rect_construct(top_shadow, target_rect.r_left, 0,
		   target_rect.r_width, target_rect.r_top);
    rect_construct(bottom_shadow, target_rect.r_left, rect_bottom(&target_rect),
	target_rect.r_width, frame->rectcache.r_height - target_rect.r_top);
}
