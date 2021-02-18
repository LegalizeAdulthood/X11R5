#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)sb_event.c 1.56 90/04/23";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	sb_event.c
 * 
 * Description:
 * 
 * Maps events into actions
 * 
 */


/*
 * Include files:
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <xview_private/sb_impl.h>
#include <xview/notify.h>
#include <xview/openwin.h>
#include <xview/fullscreen.h>
#include <xview/rectlist.h>
#include <xview/screen.h>
#include <xview/server.h>
#include <xview/win_notify.h>
#include <xview_private/draw_impl.h>

/*
 * Definitions
 */
#define SPLIT_VIEW_MENU_ITEM_NBR	5
#define JOIN_VIEWS_MENU_ITEM_NBR	6

/*
 * Declaration of Functions Defined in This File (in order):
 */

Pkg_private Notify_value scrollbar_handle_event();
Pkg_private void scrollbar_line_to_top();
Pkg_private void scrollbar_top_to_line();
Pkg_private void scrollbar_split_view_from_menu();
Pkg_private void scrollbar_join_view_from_menu();
Pkg_private void scrollbar_last_position();
Pkg_private int scrollbar_left_mouse_up();
Pkg_private Menu scrollbar_gen_menu();

static Notify_value scrollbar_timed_out();
static int      scrollbar_handle_elevator_event();
static void     scrollbar_handle_timed_page_event();
static void     scrollbar_handle_timed_line_event();
static void     scrollbar_position_mouse();
static Scroll_motion scrollbar_translate_scrollbar_event_to_motion();
static int      scrollbar_translate_to_elevator_event();
static void     scrollbar_timer_start();
static void     scrollbar_timer_stop();
static void     scrollbar_set_intransit();
static int      scrollbar_preview_split();
static int      scrollbar_invoke_split();
static void     scrollbar_destroy_split();
static int      scrollbar_multiclick();

/*
 * additions for the delay time between repeat action events
 */
#define	SB_SET_FOR_LINE		1
#define	SB_SET_FOR_PAGE		2

static int      sb_delay_time;		/* delay before first action repeat 	 */
static int      sb_page_interval;	/* delay between paging repeats 	 */
static int      sb_line_interval;	/* delay between line   repeats 	 */

/******************************************************************/

Pkg_private     Notify_value
scrollbar_handle_event(sb_public, event, arg, type)
    Xv_Window       sb_public;
    Event          *event;
    Notify_arg      arg;
    Notify_event_type type;
{
    Rect r;
    Xv_scrollbar_info *sb = SCROLLBAR_PRIVATE(sb_public);
    char	      *help_key;
    int               do_split;
    Event             split_event;
    Scroll_motion     motion;
    short             anchors_height;
    Pkg_private void  sb_full_elevator();
    Pkg_private void  sb_abbrev_elevator();

    switch (event_action(event)) {
      case ACTION_RESCALE:
	sb_rescale(sb, (Frame_rescale_state) arg);
	(void) notify_next_event_func(sb_public, event, arg, type);
	break;

      case WIN_RESIZE:
	r = *(Rect *)xv_get(sb_public, WIN_RECT, 0);
	sb_normalize_rect(sb, &r);
	sb->length = r.r_height;
	anchors_height = 2 * (sb_marker_height(sb) + SCROLLBAR_CABLE_GAP);
	sb->cable_height = r.r_height - anchors_height;

	if ((sb->cable_height <= sb->elevator_rect.r_height) &&
	    (sb->elevator_type != OLGX_ABBREV))
	  sb_abbrev_elevator(sb);
	else 
	  sb_full_elevator(sb);
	break;

      case WIN_REPAINT:
	scrollbar_paint(sb_public);
	sb->painted = TRUE;
	break;

      case ACTION_HELP:
	if (event_is_down(event)) {
	    motion = scrollbar_translate_scrollbar_event_to_motion(sb, event);
	    switch (motion) {
	      case SCROLLBAR_TO_END:
		help_key = "xview:scrollbarBottom";
		break;
	      case SCROLLBAR_TO_START:
		help_key = "xview:scrollbarTop";
		break;
	      case SCROLLBAR_PAGE_FORWARD:
		help_key = "xview:scrollbarPageForward";
		break;
	      case SCROLLBAR_PAGE_BACKWARD:
		help_key = "xview:scrollbarPageBackward";
		break;
	      case SCROLLBAR_LINE_FORWARD:
		help_key = "xview:scrollbarLineForward";
		break;
	      case SCROLLBAR_LINE_BACKWARD:
		help_key = "xview:scrollbarLineBackward";
		break;
	      case SCROLLBAR_ABSOLUTE:
		help_key = "xview:scrollbarDrag";
		break;
#ifdef SCROLL_BY_PAGE
	      case SCROLLBAR_PAGE_ALIGNED:
		help_key = "xview:scrollbarPageAligned";
		break;
#endif SCROLL_BY_PAGE
	    }
	    xv_help_show(sb_public, help_key, event);
	}
	break;

      case ACTION_SELECT:
	if (sb->inactive)
	    break;
	if (event_is_down(event)) {
		motion = scrollbar_multiclick(sb, event) ? sb->transit_motion :
		  scrollbar_translate_scrollbar_event_to_motion(sb, event);
		switch (motion) {
		      case SCROLLBAR_TO_END:
		      case SCROLLBAR_TO_START:
			scrollbar_invert_region(sb, motion);
			break;

		      case SCROLLBAR_PAGE_FORWARD:
		      case SCROLLBAR_PAGE_BACKWARD:
			scrollbar_timer_start(SCROLLBAR_PUBLIC(sb), SB_SET_FOR_PAGE);
			break;
			
		      case SCROLLBAR_LINE_FORWARD:
		      case SCROLLBAR_LINE_BACKWARD:
		      case SCROLLBAR_ABSOLUTE:
#ifdef SCROLL_BY_PAGE
		      case SCROLLBAR_PAGE_ALIGNED:
#endif SCROLL_BY_PAGE
			/* translate event to elevator space */
			scrollbar_translate_to_elevator_event(sb, event);
			scrollbar_handle_elevator_event(sb, event, motion);
			break;
			
		      default:
			break;
		}
		scrollbar_set_intransit(sb, motion, event);
	} else {
		switch (sb->transit_motion) {
		      case SCROLLBAR_TO_END:
		      case SCROLLBAR_TO_START:
			scrollbar_invert_region(sb, sb->transit_motion);
			(void) scrollbar_scroll(sb, 0, sb->transit_motion);
			break;

		      case SCROLLBAR_PAGE_FORWARD:
		      case SCROLLBAR_PAGE_BACKWARD:
			if (!sb->transit_occurred) {
				scrollbar_handle_timed_page_event(sb, sb->transit_motion);
			}
			break;
			
		      case SCROLLBAR_LINE_FORWARD:
		      case SCROLLBAR_LINE_BACKWARD:
		      case SCROLLBAR_ABSOLUTE:
#ifdef SCROLL_BY_PAGE
		      case SCROLLBAR_PAGE_ALIGNED:
#endif SCROLL_BY_PAGE
			/* translate event to elevator space */
			scrollbar_translate_to_elevator_event(sb, event);
			scrollbar_handle_elevator_event(sb, event, sb->transit_motion);
			break;
			
		      default:
		break;
		}
		scrollbar_timer_stop(SCROLLBAR_PUBLIC(sb));
	}
	sb->last_select_time = event->ie_time;
	break;
	
      case ACTION_MENU:
	if (sb->inactive)
	  break;
	if (event_is_down(event)) {
		scrollbar_set_intransit(sb, SCROLLBAR_NONE, event);
		if (!sb->menu) {
			scrollbar_create_standard_menu(sb);
		}
		menu_show(sb->menu, SCROLLBAR_PUBLIC(sb), event, 0);
	}
	break;
	
      case LOC_DRAG:
	if (sb->inactive)
	  break;
	if ((sb->transit_motion == SCROLLBAR_TO_END
	     || sb->transit_motion == SCROLLBAR_TO_START)
	    && sb->can_split) {
		/* when user pulls out of split bar start split */
		motion = scrollbar_translate_scrollbar_event_to_motion(sb, event);
		if (motion != sb->transit_motion) {
			r = *(Rect *) xv_get(sb->managee, WIN_RECT);
			do_split = scrollbar_preview_split(sb, event, &r, &split_event);
			/* split event may cause this sb to be destroy */
			/* therefore all painting must be done first */
			scrollbar_invert_region(sb, sb->transit_motion);
			if (do_split == XV_OK) {
				scrollbar_invoke_split(sb, &split_event);
			}
			scrollbar_set_intransit(sb, SCROLLBAR_NONE, event);
		}
	} else if (sb->transit_motion == SCROLLBAR_ABSOLUTE
#ifdef SCROLL_BY_PAGE
		   || sb->transit_motion == SCROLLBAR_PAGE_ALIGNED
#endif SCROLL_BY_PAGE
		   ) {
		/* translate event to elevator space */
		scrollbar_translate_to_elevator_event(sb, event);
		scrollbar_handle_elevator_event(sb, event, sb->transit_motion);
	} 
	break;
	
      default:
	break;
}
    
    return NOTIFY_DONE;
}


static int
scrollbar_handle_elevator_event(sb, event, motion)
    Xv_scrollbar_info *sb;
    Event          *event;
    Scroll_motion   motion;
{
    int             y, x, transit_y;
#ifdef SCROLL_BY_PAGE
    Pkg_private void scrollbar_paint_page_number();
#endif SCROLL_BY_PAGE

    switch (event_action(event)) {
      case ACTION_SELECT:
	if (event_is_down(event)) {
	    y = SB_VERTICAL(sb) ? event_y(event) : event_x(event);
	    scrollbar_invert_region(sb, motion);
	    switch (motion) {
	      case SCROLLBAR_LINE_FORWARD:
	      case SCROLLBAR_LINE_BACKWARD:
		scrollbar_timer_start(SCROLLBAR_PUBLIC(sb), SB_SET_FOR_LINE);
		break;
#ifdef SCROLL_BY_PAGE
	      case SCROLLBAR_PAGE_ALIGNED:
		/* draw the page number box */
		scrollbar_paint_page_number(sb, 1);
		break;
#endif SCROLL_BY_PAGE
	      case SCROLLBAR_ABSOLUTE:
		break;
	      default:
		break;
	    }
	} else {
	    scrollbar_invert_region(sb, sb->transit_motion);
	    switch (sb->transit_motion) {

	      case SCROLLBAR_LINE_FORWARD:
	      case SCROLLBAR_LINE_BACKWARD:
		scrollbar_timer_stop(SCROLLBAR_PUBLIC(sb));
		if (!sb->transit_occurred) {
		    scrollbar_handle_timed_line_event(sb, sb->transit_motion);
		}
		break;

#ifdef SCROLL_BY_PAGE
	      case SCROLLBAR_PAGE_ALIGNED:
		/* remove the page number box */
		scrollbar_paint_page_number(sb, 0);
		break;
#endif SCROLL_BY_PAGE

	      case SCROLLBAR_ABSOLUTE:
		/* 
		 * compute absolute position by top of elevator
		 * in the coordinate space of the cable
		 */
		y = sb->elevator_rect.r_top - sb->cable_start;
		scrollbar_available_cable(sb, &x);
		if (y < 0) {
		    y = 0;
		} else if (y > x) {
		    y = x;
		}
		(void) scrollbar_scroll(sb, y, sb->transit_motion);
		break;

	      default:
		break;
	    }
	    scrollbar_timer_stop(SCROLLBAR_PUBLIC(sb));
	}
	break;
      case LOC_DRAG:
	if (sb->transit_motion == SCROLLBAR_ABSOLUTE
#ifdef SCROLL_BY_PAGE
	    || sb->transit_motion == SCROLLBAR_PAGE_ALIGNED
#endif SCROLL_BY_PAGE
	    ) {
		if (SB_VERTICAL(sb)) {
			y = event_y(event);
			transit_y = event_y(&sb->transit_event);
		} else {
			y = event_x(event);
			transit_y = event_x(&sb->transit_event);
		}
		if (transit_y != y) {
			/* 
			 * compute absolute position by top of elevator
			 * in the coordinate space of the cable
			 */
			y = sb->elevator_rect.r_top + (y - transit_y) - sb->cable_start;
			scrollbar_available_cable(sb, &x);
			if (y < 0) {
				y = 0;
			} else if (y > x) {
				y = x;
			}
			scrollbar_absolute_position_elevator(sb, y);
#ifdef ABSOLUTE_SCROLL
			(void) scrollbar_scroll(sb, y, sb->transit_motion);
#endif ABSOLUTE_SCROLL			
#ifdef SCROLL_BY_PAGE
			if (sb->transit_motion == SCROLLBAR_PAGE_ALIGNED) {
				scrollbar_scroll(sb, y, sb->transit_motion);
			}
#endif SCROLL_BY_PAGE
		}
	}
	break;
      default:
	break;
    }

    return XV_OK;
}

/*ARGSUSED*/
static          Notify_value
scrollbar_timed_out(client, which)
    Notify_client   client;
    int             which;
{
    Xv_scrollbar_info *sb = SCROLLBAR_PRIVATE(client);

    /* if the event has happened, but the user is no longer physically */
    /* pressing the Left Mouse Button, we skip the action. This is a   */
    /* method to use the current programmatic paradigm w/o processing  */
    /* past events/timeouts that have triggered but are no longer valid */
    /* jcb -- 5/3/89 */
    if (scrollbar_left_mouse_up(sb))
	return NOTIFY_DONE;


    /* tell myself to repeat a scroll */
    switch (sb->transit_motion) {

      case SCROLLBAR_LINE_BACKWARD:
	if (sb->view_start) {
	    scrollbar_handle_timed_line_event(sb, sb->transit_motion);
	    sb->transit_occurred = TRUE;
	}
	break;

      case SCROLLBAR_LINE_FORWARD:
	scrollbar_handle_timed_line_event(sb, sb->transit_motion);
	sb->transit_occurred = TRUE;
	break;

      case SCROLLBAR_PAGE_FORWARD:
	scrollbar_handle_timed_page_event(sb, sb->transit_motion);
	sb->transit_occurred = TRUE;
	break;

      case SCROLLBAR_PAGE_BACKWARD:
	if (sb->view_start) {
	    scrollbar_handle_timed_page_event(sb, sb->transit_motion);
	    sb->transit_occurred = TRUE;
	}
	break;

      default:
	break;
    }

    return NOTIFY_DONE;
}


/*ARGSUSED*/
Pkg_private void
scrollbar_line_to_top(menu, item)
    Menu            menu;
    Menu_item       item;
{
    Xv_scrollbar_info *sb;
    int    pos;

    sb = (Xv_scrollbar_info *) xv_get(menu, XV_KEY_DATA, sb_context_key, 0);

    /* look at transit event to see where the mouse button went down */
    pos = SB_VERTICAL(sb) ? event_y(&sb->transit_event)	: event_x(&sb->transit_event);
    scrollbar_scroll(sb, pos, SCROLLBAR_POINT_TO_MIN);
}

/*ARGSUSED*/
Pkg_private void
scrollbar_top_to_line(menu, item)
    Menu            menu;
    Menu_item       item;
{
    Xv_scrollbar_info *sb;
    int    pos;

    sb = (Xv_scrollbar_info *) xv_get(menu, XV_KEY_DATA, sb_context_key);

    /* look at transit event to see where the mouse button went down */
    pos = SB_VERTICAL(sb) ? event_y(&sb->transit_event) : event_x(&sb->transit_event);
    scrollbar_scroll(sb, pos, SCROLLBAR_MIN_TO_POINT);
}


Pkg_private     Menu
scrollbar_gen_menu(menu, op)
    Menu            menu;
    Menu_generate   op;
{
    int             nbr_menu_items;
    int             nbr_views;
    Xv_scrollbar_info *sb;
    Xv_opaque       openwin;

    if (op == MENU_DISPLAY) {
	nbr_menu_items = (int) xv_get(menu, MENU_NITEMS);
	sb = (Xv_scrollbar_info *) xv_get(menu, XV_KEY_DATA, sb_context_key);
	if (sb->can_split) {
	    if (nbr_menu_items < SPLIT_VIEW_MENU_ITEM_NBR)
		xv_set(menu,
		       MENU_APPEND_ITEM,
		     xv_get(menu, XV_KEY_DATA, sb_split_view_menu_item_key, 0),
		       0);
	    openwin = xv_get(SCROLLBAR_PUBLIC(sb), XV_OWNER);
	    nbr_views = (int) xv_get(openwin, OPENWIN_NVIEWS);
	    if (nbr_views > 1 && nbr_menu_items < JOIN_VIEWS_MENU_ITEM_NBR)
		xv_set(menu,
		       MENU_APPEND_ITEM,
		       xv_get(menu, XV_KEY_DATA, sb_join_view_menu_item_key, 0),
		       0);
	    else if (nbr_views == 1 &&
		     nbr_menu_items == JOIN_VIEWS_MENU_ITEM_NBR)
		xv_set(menu, MENU_REMOVE, JOIN_VIEWS_MENU_ITEM_NBR, 0);
	} else {
	    if (nbr_menu_items == JOIN_VIEWS_MENU_ITEM_NBR)
		xv_set(menu, MENU_REMOVE, JOIN_VIEWS_MENU_ITEM_NBR, 0);
	    if (nbr_menu_items >= SPLIT_VIEW_MENU_ITEM_NBR)
		xv_set(menu, MENU_REMOVE, SPLIT_VIEW_MENU_ITEM_NBR, 0);
	}
    }
    return (menu);
}


/*ARGSUSED*/
Pkg_private void
scrollbar_split_view_from_menu(menu, item)
    Menu            menu;
    Menu_item       item;
{
    scrollbar_invoke_split(
	    (Xv_scrollbar_info *) xv_get(menu, XV_KEY_DATA, sb_context_key),
			   (Event *) xv_get(menu, MENU_FIRST_EVENT));
}


/*ARGSUSED*/
Pkg_private void
scrollbar_join_view_from_menu(menu, item)
    Menu            menu;
    Menu_item       item;
{
    scrollbar_destroy_split(
	   (Xv_scrollbar_info *) xv_get(menu, XV_KEY_DATA, sb_context_key));
}


/*ARGSUSED*/
Pkg_private void
scrollbar_last_position(menu, item)
    Menu            menu;
    Menu_item       item;
{
    Xv_scrollbar_info *sb;

    sb = (Xv_scrollbar_info *) xv_get(menu, XV_KEY_DATA, sb_context_key);

    scrollbar_scroll_to_offset(sb, sb->last_view_start);
}

static void
scrollbar_handle_timed_page_event(sb, motion)
    Xv_scrollbar_info *sb;
    Scroll_motion   motion;
{
	int             x, y, new_y;

    if ((scrollbar_scroll(sb, 0, motion) != SCROLLBAR_POSITION_UNCHANGED) &&
	(sb->jump_pointer)) {
	    /* adjust the mouse if necessary */
	    if (SB_VERTICAL(sb)) {
		    x = event_x(&sb->transit_event);
		    y = event_y(&sb->transit_event);
	    } else {
		    x = event_y(&sb->transit_event);
		    y = event_x(&sb->transit_event);
	    }		    
	    
	    new_y = y;
	    if (sb->transit_motion == SCROLLBAR_PAGE_FORWARD && 
		(y <= rect_bottom(&sb->elevator_rect)))
	      new_y = rect_bottom(&sb->elevator_rect) + 1;
	    else if (sb->transit_motion == SCROLLBAR_PAGE_BACKWARD && 
		     (y >= sb->elevator_rect.r_top))
	      new_y = sb->elevator_rect.r_top - 1;
		    
	    scrollbar_position_mouse(sb, x, new_y);
	    
	    if (new_y != y) 
	      if (SB_VERTICAL(sb))
		event_set_y(&sb->transit_event, y);
	      else
		event_set_x(&sb->transit_event, y);
    }
}


static void
scrollbar_handle_timed_line_event(sb, motion)
	Xv_scrollbar_info *sb;
	Scroll_motion      motion;
{
	int             x, y;
	Rect            active_region;

	if ((scrollbar_scroll(sb, 0, motion) != SCROLLBAR_POSITION_UNCHANGED) &&
	    (sb->jump_pointer)) {
		/* reposition mouse if necessary */
		if (motion == SCROLLBAR_LINE_FORWARD)
		  scrollbar_line_forward_rect(sb, &active_region);
		else 
		  scrollbar_line_backward_rect(sb, &active_region);

		x = active_region.r_left + (active_region.r_width / 2);
		y = active_region.r_top + (active_region.r_height / 2);
		scrollbar_position_mouse(sb, x, y);
	}
}

static void
scrollbar_position_mouse(sb, x, y)
	Xv_scrollbar_info *sb;
	int             x, y;
{
	Xv_Window 	win = SCROLLBAR_PUBLIC(sb);
	Rect           *mouse_rect;
	Rect           *sb_rect;
	
	mouse_rect = (Rect *)xv_get(win, WIN_MOUSE_XY, 0);
	sb_rect = (Rect *)xv_get(win, XV_RECT, 0);

	/* move mouse if it is still in the scrollbar window */
	if (mouse_rect->r_left >= 0 &&
	    mouse_rect->r_left < sb_rect->r_width &&
	    mouse_rect->r_top >= 0 &&
	    mouse_rect->r_top < sb_rect->r_height)
	  if (SB_VERTICAL(sb))
	    xv_set(win, WIN_MOUSE_XY, x, y, 0);
	  else
	    xv_set(win, WIN_MOUSE_XY, y, x, 0);
}

static          Scroll_motion
scrollbar_translate_scrollbar_event_to_motion(sb, event)
    Xv_scrollbar_info *sb;
    Event          *event;
{
    int             anchor_height = sb_marker_height(sb);
    int             elevator_start;
    int             elevator_end;
    int		    region_height;
    int             pos;

    pos = SB_VERTICAL(sb) ? event_y(event) : event_x(event);

    elevator_start = sb->elevator_rect.r_top;
    elevator_end = elevator_start + sb->elevator_rect.r_height - 1;
    region_height = sb->elevator_rect.r_height;
    region_height /= (sb->elevator_type == OLGX_ABBREV) ? 2 : 3;

    /* Check for top anchor */
    if (pos <= anchor_height)
      return (SCROLLBAR_TO_START);

    /* Check in between top anchor and elevator */
    else if (pos <= elevator_start)
      if (elevator_start <= anchor_height + SCROLLBAR_CABLE_GAP)
	/* already at the top */
	return (SCROLLBAR_NONE);
      else
	return (SCROLLBAR_PAGE_BACKWARD);

    /* Check for line backward region of elevator */
    else if (pos <= (elevator_start + region_height))
      return (SCROLLBAR_LINE_BACKWARD);

    /* Check for absolute scroll region of elevator */
    else if ((sb->elevator_type != OLGX_ABBREV) &&
	     (pos <= (elevator_start + region_height * 2)))
      return (SCROLLBAR_ABSOLUTE);

    /* Check for line forward region of elevator */
    else if (pos <= elevator_end)
      return (SCROLLBAR_LINE_FORWARD);
    
    /* Check in between elevator and bottom anchor */
    else if (pos <= (sb->length - anchor_height))
      if (elevator_end + SCROLLBAR_CABLE_GAP + 1 >= sb->length - anchor_height)
	/* already at the bottom */
	return (SCROLLBAR_NONE);
      else
	return (SCROLLBAR_PAGE_FORWARD);
    
    /* Must be in the bottom anchor */
    else
      return (SCROLLBAR_TO_END);
}

static void
scrollbar_timer_start(scrollbar, actiontype)
    Scrollbar       scrollbar;
    int             actiontype;
{
    struct itimerval timer;
    int             interval;

    /* Call timeout routine using pre-set times according to actiontype */
    if (actiontype == SB_SET_FOR_LINE)
	interval = sb_line_interval;
    else
	interval = sb_page_interval;

    timer.it_value.tv_usec = sb_delay_time * 1000;
    timer.it_value.tv_sec = 0;

    /* next time call in `interval' msec		 */
    timer.it_interval.tv_usec = interval * 1000;
    timer.it_interval.tv_sec = 0;

    (void) notify_set_itimer_func((Notify_client) scrollbar,
				  scrollbar_timed_out, ITIMER_REAL, &timer,
				  (struct itimerval *) NULL);
}

static void
scrollbar_timer_stop(scrollbar)
    Scrollbar       scrollbar;
{
    (void) notify_set_itimer_func((Notify_client) scrollbar,
				  scrollbar_timed_out,
				  ITIMER_REAL,
				  (struct itimerval *) NULL,
				  (struct itimerval *) NULL);
}

static void
scrollbar_set_intransit(sb, motion, event)
    Xv_scrollbar_info *sb;
    Scroll_motion   motion;
    Event          *event;
{
    sb->transit_motion = motion;
    sb->transit_event = *event;
    sb->transit_occurred = FALSE;
}

static int
scrollbar_translate_to_elevator_event(sb, event)
    Xv_scrollbar_info *sb;
    Event          *event;
{
    int             x, y;

    if (SB_VERTICAL(sb)) {
	    event_set_x(event, event_x(event) - sb_margin(sb));
	    event_set_y(event, event_y(event) - sb->elevator_rect.r_top);
    } else {
	    event_set_x(event, event_x(event) - sb->elevator_rect.r_top);
	    event_set_y(event, event_y(event) - sb_margin(sb));
    }		    
}

static int
scrollbar_preview_split(sb, event, r, completion_event)
    Xv_scrollbar_info *sb;
    Event          *event;
    Rect           *r;
    Event          *completion_event;
{
    Fullscreen      fs;
    Xv_Window       paint_window;
    Rect            sb_r;
    int             x, y;
    int             ep1_x, ep1_y, ep2_x, ep2_y;
    int             pw_ep1_x, pw_ep1_y, pw_ep2_x, pw_ep2_y;
    int             result = XV_OK;
    Scrollbar       scrollbar = SCROLLBAR_PUBLIC(sb);
    Inputmask       im;

    ep1_x = ep1_y = ep2_x = ep2_y = 0;
    sb_r = *(Rect *) xv_get(scrollbar, WIN_RECT);

    if (sb->direction == SCROLLBAR_VERTICAL) {
	if (sb_r.r_left < r->r_left) {
	    /* left handed scrollbar */
	    ep1_x = 0;
	    ep2_x = sb_r.r_width + r->r_width;
	}
	else {
	    ep1_x = r->r_left - sb_r.r_left;
	    ep2_x = sb_r.r_width;
	}
	ep1_y = ep2_y = event_y(event);
    } else {
	ep1_y = r->r_top - sb_r.r_top;
	ep2_y = sb_r.r_height;
	ep1_x = ep2_x = event_x(event);
    }

    fs = xv_create(NULL, FULLSCREEN,
		   FULLSCREEN_INPUT_WINDOW, scrollbar,
		   WIN_CONSUME_EVENTS, WIN_MOUSE_BUTTONS, LOC_DRAG,
		   WIN_UP_EVENTS, 0,
		   0);

    paint_window = (Xv_Window) xv_get(fs, FULLSCREEN_PAINT_WINDOW);
    win_translate_xy(scrollbar, paint_window, ep1_x, ep1_y, &pw_ep1_x, &pw_ep1_y);
    win_translate_xy(scrollbar, paint_window, ep2_x, ep2_y, &pw_ep2_x, &pw_ep2_y);

    xv_set(XV_SERVER_FROM_WINDOW(scrollbar), SERVER_SYNC, 0, 0);

    /* draw first line */
    pw_vector(paint_window, pw_ep1_x, pw_ep1_y, pw_ep2_x, pw_ep2_y, PIX_NOT(PIX_DST), 1);

    win_setinputcodebit(&im, MS_LEFT);
    win_setinputcodebit(&im, MS_MIDDLE);
    win_setinputcodebit(&im, MS_RIGHT);
    win_setinputcodebit(&im, LOC_DRAG);
    im.im_flags = IM_NEGEVENT;

    for (;;) {

	if (xv_input_readevent((Xv_Window) scrollbar, completion_event, TRUE, TRUE, &im) == -1) {
	    xv_destroy(fs);
	    goto Bad_completion;
	}
	x = event_x(completion_event);
	y = event_y(completion_event);
	switch (event_action(completion_event)) {
	  case ACTION_SELECT:
	    if (event_is_up(completion_event)) {
		pw_vector(paint_window, pw_ep1_x, pw_ep1_y, pw_ep2_x, pw_ep2_y,
			  PIX_NOT(PIX_DST), 1);
		goto Good_completion;
	    }
	    break;
	  case LOC_DRAG:
	    if (sb->direction == SCROLLBAR_VERTICAL) {
		if (y >= 0 && y <= sb_r.r_height) {
		    pw_vector(paint_window, pw_ep1_x, pw_ep1_y, pw_ep2_x, pw_ep2_y,
			      PIX_NOT(PIX_DST), 1);
		    pw_ep1_y += y - ep1_y;
		    pw_ep2_y = pw_ep1_y;
		    ep1_y = y;
		    pw_vector(paint_window, pw_ep1_x, pw_ep1_y, pw_ep2_x, pw_ep2_y,
			      PIX_NOT(PIX_DST), 1);
		}
	    } else {
		if (x >= 0 && x <= sb_r.r_width) {
		    pw_vector(paint_window, pw_ep1_x, pw_ep1_y, pw_ep2_x, pw_ep2_y,
			      PIX_NOT(PIX_DST), 1);
		    pw_ep1_x += x - ep1_x;
		    pw_ep2_x = pw_ep1_x;
		    ep1_x = x;
		    pw_vector(paint_window, pw_ep1_x, pw_ep1_y, pw_ep2_x, pw_ep2_y,
			      PIX_NOT(PIX_DST), 1);
		}
	    }
	    break;
	  default:
	    break;
	}
    }
Bad_completion:
    result = XV_ERROR;
Good_completion:
    xv_destroy(fs);
    return (result);
}

static int
scrollbar_invoke_split(sb, event)
    Xv_scrollbar_info *sb;
    Event          *event;
{
    int             split_loc;
    Scroll_motion   motion;
    extern Notify_arg win_copy_event();
    extern void     win_free_event();

    /* make sure the event is in the scrollbar */
    split_loc = (sb->direction == SCROLLBAR_VERTICAL) ? event_y(event)
	: event_x(event);

    motion = scrollbar_translate_scrollbar_event_to_motion(sb, event);
    if (motion != sb->transit_motion) {
	if ((sb->transit_motion == SCROLLBAR_TO_START && motion == SCROLLBAR_TO_END) ||
	    (sb->transit_motion == SCROLLBAR_TO_END && motion == SCROLLBAR_TO_START)) {
	    /* destroy the split */
	    scrollbar_destroy_split(sb);
	} else {
	    /* create the split */
	    (void) win_post_id_and_arg(sb->managee,
	     (sb->direction == SCROLLBAR_VERTICAL) ? ACTION_SPLIT_HORIZONTAL
				       : ACTION_SPLIT_VERTICAL,
				       NOTIFY_SAFE, split_loc,
				       win_copy_event, win_free_event);
	}
    }
}


static void
scrollbar_destroy_split(sb)
    Xv_scrollbar_info *sb;
{
    extern Notify_arg win_copy_event();
    extern void     win_free_event();

    win_post_id_and_arg(sb->managee,
			ACTION_SPLIT_DESTROY, NOTIFY_SAFE, NULL,
			win_copy_event, win_free_event);
}


/*
 * scrollbar_multiclick returns TRUE if this (ACTION_SELECT) event has
 * occurred within sb->multiclick_timeout msec of the last ACTION_SELECT
 * event; otherwise, it returns FALSE.
 */
static int
scrollbar_multiclick(sb, event)
    Xv_scrollbar_info *sb;
    Event          *event;
{
	unsigned int	sec_delta,
			usec_delta;

	sec_delta = event->ie_time.tv_sec - sb->last_select_time.tv_sec;
	usec_delta = event->ie_time.tv_usec - sb->last_select_time.tv_usec;
	if (sec_delta > 0) {		/* normalize the time difference */
		sec_delta--;
		usec_delta += 1000000;
	}

	/* 
	 * Compare the delta time with the multiclick timeout (in msec).
	 * We can't just convert the delta time to msec because we may
	 * very well overflow the machine's integer format with unpredictable
	 * results.
	 */
	if (sec_delta != (sb->multiclick_timeout / 1000)) {
		return (sec_delta < (sb->multiclick_timeout / 1000) );
	}
	else {
		return (usec_delta <= (sb->multiclick_timeout * 1000) );
	}
}


/*
 * setup the values for the repeat status. This routine is called at
 * initialization time (sb_init.c) after .Xdefault values are read.
 */

Pkg_private void
scrollbar_init_delay_values(delay_time, page_interval, line_interval)
    int             delay_time;
    int             page_interval, line_interval;
{
    sb_delay_time = delay_time;
    sb_page_interval = page_interval;
    sb_line_interval = line_interval;
}

/*
 * Poll the state of the mouse to see if the user still has it physically
 * depressed. This is used with the current paradigm to insure speed of user
 * scrolling while also preventing overflow processing after the mouse is
 * released. The other solution is to slow the scroll action so it works
 * correctly on the _slowest_ case, rather than taking advantage of the
 * faster hardware that is available. jcb 5/3/89
 */
Pkg_private int
scrollbar_left_mouse_up(sb)
    Xv_scrollbar_info *sb;

{
    Scrollbar       sb_public = SCROLLBAR_PUBLIC(sb);
    Xv_Drawable_info *info;
    Display        *display;
    Window          window;
    Window          root, child;
    int             root_X, root_Y;
    int             win_X, win_Y;
    unsigned int    key_buttons;

    /* get the root of X information */
    DRAWABLE_INFO_MACRO(sb_public, info);
    display = xv_display(info);
    window = xv_xid(info);

    (void) XQueryPointer(display, window, &root, &child,
			 &root_X, &root_Y, &win_X, &win_Y, &key_buttons);

    return ((key_buttons & Button1Mask) == 0);
}
