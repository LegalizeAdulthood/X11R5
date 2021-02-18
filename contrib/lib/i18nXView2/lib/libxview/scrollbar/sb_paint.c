#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)sb_paint.c 1.59 90/05/19";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	sb_paint.c
 * 
 * Description:
 * 
 * Maps events into actions
 * 
 */

/*
 * Include files:
 */

#include <xview_private/sb_impl.h>
#include <xview_private/draw_impl.h>
#include <xview/rectlist.h>
#include <xview/window.h>
#include <xview/svrimage.h>
#include <X11/Xlib.h>

/*
 * Declaration of Functions Defined in This File (in order):
 */

Xv_public void  scrollbar_paint();

Pkg_private void scrollbar_clear_damage();
Pkg_private void scrollbar_paint_elevator();
Pkg_private void scrollbar_paint_elevator_move();
Pkg_private void scrollbar_invert_region();
#ifdef SCROLL_BY_PAGE
Pkg_private void scrollbar_paint_page_number();
#endif SCROLL_BY_PAGE
static void     scrollbar_proportional_indicator();
static void     scrollbar_paint_marker();

/******************************************************************/


Xv_public void
scrollbar_paint(sb_public)
    Scrollbar sb_public;
{
    Xv_scrollbar_info *sb = SCROLLBAR_PRIVATE(sb_public);
    Rect            r;

    /* Calculate correct position of elevator.  Paint cable and elevator. */
    scrollbar_position_elevator(sb, TRUE, SCROLLBAR_NONE);

    /* Paint top cable anchor */
    scrollbar_top_anchor_rect(sb, &r);
    scrollbar_paint_marker(sb, &r, sb->top_anchor_inverted);

    /* Paint bottom cable anchor */
    scrollbar_bottom_anchor_rect(sb, &r);
    scrollbar_paint_marker(sb, &r, sb->bottom_anchor_inverted);
}

Pkg_private void
scrollbar_paint_elevator(sb)
    Xv_scrollbar_info *sb;
{
    scrollbar_paint_elevator_move(sb, sb->elevator_rect.r_top, FALSE);
}

Pkg_private void
scrollbar_paint_elevator_move(sb, new_pos, dragging_elevator)
    Xv_scrollbar_info *sb;
    int new_pos;
    int dragging_elevator;	/* TRUE or FALSE */
{
	int x, y, old_pos;
	int prop_pos, prop_length, state;

	state = sb->elevator_type | sb->elevator_state | OLGX_UPDATE | OLGX_ERASE;
	old_pos = sb->elevator_rect.r_top;

	if (sb->direction == SCROLLBAR_VERTICAL) {
		state |= OLGX_VERTICAL;
		x = sb->elevator_rect.r_left;
		y = 0;
	} else {
		state |= OLGX_HORIZONTAL;
		x = 0;
		y = sb->elevator_rect.r_left;
	}
	if (!(state & OLGX_INACTIVE) &&
	    (!(state & (OLGX_SCROLL_BACKWARD | OLGX_SCROLL_FORWARD | 
			OLGX_SCROLL_ABSOLUTE)))) {
		if (sb->view_start == 0) 
		  state |= OLGX_SCROLL_NO_BACKWARD;
		if (sb->view_start >= Max_offset(sb))
		  state |= OLGX_SCROLL_NO_FORWARD;
	}
#ifndef ABSOLUTE_SCROLL
	if (dragging_elevator) {
		prop_pos = new_pos;
		prop_length = 0;
	} else 
#endif ABSOLUTE_SCROLL
	  scrollbar_proportional_indicator(sb, new_pos, &prop_pos, &prop_length);

	olgx_draw_scrollbar(sb->ginfo, sb->window, x, y, sb->length,
			    new_pos, old_pos, prop_pos, prop_length, state);

	sb->elevator_rect.r_top = new_pos;
}

Pkg_private void
scrollbar_invert_region(sb, motion)
    Xv_scrollbar_info *sb;
    Scroll_motion   motion;
{
    Rect            r;
    int		    state;
    
    switch (motion) {
      case SCROLLBAR_TO_START:
	scrollbar_top_anchor_rect(sb, &r);
	state = sb->top_anchor_inverted = !(sb->top_anchor_inverted);
	scrollbar_paint_marker(sb, &r, state);
	break;
      case SCROLLBAR_TO_END:
	scrollbar_bottom_anchor_rect(sb, &r);
	state = sb->bottom_anchor_inverted = !(sb->bottom_anchor_inverted);
	scrollbar_paint_marker(sb, &r, state);
	break;
      case SCROLLBAR_LINE_FORWARD:
	sb->elevator_state = (sb->elevator_state == OLGX_SCROLL_FORWARD) ? 0 :
	  OLGX_SCROLL_FORWARD;
	scrollbar_paint_elevator(sb);
	break;
      case SCROLLBAR_LINE_BACKWARD:
	sb->elevator_state = (sb->elevator_state == OLGX_SCROLL_BACKWARD) ? 0 :
	  OLGX_SCROLL_BACKWARD;
	scrollbar_paint_elevator(sb);
	break;
      case SCROLLBAR_ABSOLUTE:
	sb->elevator_state = (sb->elevator_state == OLGX_SCROLL_ABSOLUTE) ? 0 :
	  OLGX_SCROLL_ABSOLUTE;
	scrollbar_paint_elevator(sb);
	break;
    }
}

static void
scrollbar_paint_marker(sb, r, invoked)
    Xv_scrollbar_info *sb;
    Rect              *r;
    int               invoked;
{
    sb_normalize_rect(sb, r);
    olgx_draw_box(sb->ginfo, sb->window,
		  r->r_left, r->r_top, r->r_width, r->r_height,
		  invoked | OLGX_ERASE, TRUE);
}

#ifdef SCROLL_BY_PAGE
Pkg_private void
scrollbar_paint_page_number(sb, paint)
    Xv_scrollbar_info *sb;
    short           paint;	/* whether to paint or to remove */
{
}
#endif SCROLL_BY_PAGE

/*
 * scrollbar_proportional_indicator - return the starting position, and length 
 *   of the scrollbar's cable proportional indicator.
 */
static void
scrollbar_proportional_indicator(sb, elev_pos, position, length)
Xv_scrollbar_info	*sb;
int			elev_pos;	/* the top of the elevator */
int 			*position;	/* RETURN VALUE */
int			*length;	/* RETURN VALUE */
{
    int cable_size; 
    int elev_bottom;
    
    if (sb->object_length == 0) {
	*position = sb->cable_start;
	*length = sb->cable_height - 2;
    } else {
	*length = sb->view_length * 
	  sb->cable_height / sb->object_length;
	if (*length > sb->cable_height - 2)
	  *length = sb->cable_height - 2;
	
	*position = sb->cable_start + sb->view_start * 
	  sb->cable_height / sb->object_length;
	
	if (*position > elev_pos) {
	    *length += elev_pos - *position;
	    *position = elev_pos;
	}
	elev_bottom = elev_pos + sb->elevator_rect.r_height - 1;
	if ((*position + *length - 1) < elev_bottom) {
	    *length = elev_bottom - *position;
	}
	
	/* 
	 * If the prop indicator would be covered by the elevator,
	 * make the indicator a 3 point mark above (or below) the
	 * elevator
	 */
	if (*length < sb->elevator_rect.r_height) 
	  if ((elev_pos - 4) >= sb->cable_start) {
	      *position = elev_pos - 4;
	      *length = 2 + sb->elevator_rect.r_height + 1;
	  } else if ((elev_pos + sb->elevator_rect.r_height + 1) <= 
		     (sb->cable_start + sb->cable_height - 1)) {
	      *position = elev_pos;
	      *length = 2 + sb->elevator_rect.r_height;
	  } else
	    *length = 0;
    }
}
