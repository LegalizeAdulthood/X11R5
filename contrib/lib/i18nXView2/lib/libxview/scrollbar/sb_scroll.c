#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)sb_scroll.c 1.33 90/04/25";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	sb_scroll.c
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
#include <xview/win_notify.h>

/*
 * Declaration of Functions Defined in This File (in order):
 */
Xv_public void  scrollbar_default_compute_scroll_proc();

Pkg_private int scrollbar_scroll();
Pkg_private int scrollbar_scroll_to_offset();

static int      scrollbar_offset_to_client_units();
static unsigned long scrollbar_absolute_offset();

#ifdef SCROLL_BY_PAGE
static int      rounded();
#endif SCROLL_BY_PAGE


/******************************************************************/

Pkg_private int
scrollbar_scroll(sb, pos, motion)
    Xv_scrollbar_info *sb;
    int             pos;
    Scroll_motion   motion;
{
    long unsigned   voffset = 0, vstart = 0;
    int             result = SCROLLBAR_POSITION_UNCHANGED;
    int             available_cable;
#ifdef SCROLL_BY_PAGE
    int             new_page, total_pages, length_per_page;
    int		    rounded_pos;
#endif SCROLL_BY_PAGE

    if (motion == SCROLLBAR_NONE)
	return (result);

    /* translate position into client space */
    scrollbar_available_cable(sb, &available_cable);

#ifdef SCROLL_BY_PAGE
    if (sb->page_length) {
	total_pages = rounded(sb->object_length, sb->page_length);
	length_per_page = available_cable / total_pages;
	new_page = pos / length_per_page + 1;
	rounded_pos = length_per_page * (new_page - 1) + 1;
    }
    if (motion == SCROLLBAR_PAGE_ALIGNED) {
	if (new_page == sb->prev_page) {
	    return;
	}
    }
    sb->prev_page = new_page;
#endif SCROLL_BY_PAGE

    if (sb->compute_scroll_proc != NULL) {
	/* client wants to do all necessary scrolling */
#ifdef SCROLL_BY_PAGE
	if (motion == SCROLLBAR_PAGE_ALIGNED) {
	    motion = SCROLLBAR_ABSOLUTE;
	    sb->compute_scroll_proc(SCROLLBAR_PUBLIC(sb), rounded_pos, available_cable,
				    motion, &voffset, &sb->object_length);
	} else
#endif SCROLL_BY_PAGE
	{
	    sb->compute_scroll_proc(SCROLLBAR_PUBLIC(sb), pos, available_cable,
				    motion, &voffset, &sb->object_length);
	}
    }
    if (sb->normalize_proc != NULL) {
	sb->normalize_proc(SCROLLBAR_PUBLIC(sb), voffset, motion, &vstart);
    } else {
	vstart = voffset;
    }

    if (vstart != sb->view_start) {
	result = scrollbar_scroll_to_offset(sb, vstart);
    }
    scrollbar_position_elevator(sb, sb->painted, motion);

    return (result);
}

Pkg_private int
scrollbar_scroll_to_offset(sb, view_start)
    Xv_scrollbar_info *sb;
    long unsigned   view_start;
{
    extern Notify_arg win_copy_event();
    extern void     win_free_event();

    /* do bounds checking */
    if (view_start > sb->object_length) {
	view_start = sb->object_length;
    } else if (view_start < 0) {
	view_start = 0;
    }
    if (view_start != sb->view_start) {

	sb->last_view_start = sb->view_start;
	sb->view_start = view_start;

	(void) win_post_id_and_arg(sb->managee,
				   SCROLLBAR_REQUEST, NOTIFY_SAFE,
				   SCROLLBAR_PUBLIC(sb), win_copy_event,
				   win_free_event);
	return (XV_OK);
    } else {
	return (SCROLLBAR_POSITION_UNCHANGED);
    }
}

Xv_public void
scrollbar_default_compute_scroll_proc(scroll_public, pos, length, motion,
				      offset, object_length)
    Scrollbar       scroll_public;
    int             pos;
    int             length;
    Scroll_motion   motion;
    unsigned long  *offset;
    unsigned long  *object_length;
{
    Xv_scrollbar_info *sb = SCROLLBAR_PRIVATE(scroll_public);
    int             minus_movement;
    unsigned long   pixel_offset;

    pixel_offset = sb->view_start * sb->pixels_per_unit;

    switch (motion) {
      case SCROLLBAR_ABSOLUTE:
	/* pos is position in the cable */
	pixel_offset = scrollbar_absolute_offset(sb, pos, length);
	break;

      case SCROLLBAR_POINT_TO_MIN:
	pixel_offset += pos;
	break;

      case SCROLLBAR_PAGE_FORWARD:
	if (sb->page_length != SCROLLBAR_DEFAULT_LENGTH) {
	    /* page scrolling */
	    pixel_offset += (sb->page_length * sb->pixels_per_unit);
	} else {
	    /* display scrolling */
	    pixel_offset += (sb->view_length * sb->pixels_per_unit);
	}
	break;

      case SCROLLBAR_LINE_FORWARD:
	pixel_offset += sb->pixels_per_unit;
	break;

      case SCROLLBAR_MIN_TO_POINT:
	if (pos > pixel_offset) {
	    *offset = 0;
	} else {
	    pixel_offset -= pos;
	}
	break;

      case SCROLLBAR_PAGE_BACKWARD:
	if (sb->page_length != SCROLLBAR_DEFAULT_LENGTH) {
	    /* page scrolling */
	    minus_movement = sb->page_length * sb->pixels_per_unit;
	} else {
	    /* display scrolling */
	    minus_movement = sb->view_length * sb->pixels_per_unit;
	}
	if (minus_movement > pixel_offset) {
	    pixel_offset = 0;
	} else {
	    pixel_offset -= minus_movement;
	}
	break;

      case SCROLLBAR_LINE_BACKWARD:
	if (sb->pixels_per_unit > pixel_offset) {
	    pixel_offset = 0;
	} else {
	    pixel_offset -= sb->pixels_per_unit;
	}
	break;

      case SCROLLBAR_TO_END:
	pixel_offset = Max_offset(sb) * sb->pixels_per_unit;
	break;

      case SCROLLBAR_TO_START:
	pixel_offset = 0;
	break;

      default:
	break;
    }

    if (pixel_offset > Max_offset(sb) * sb->pixels_per_unit) {
	pixel_offset = Max_offset(sb) * sb->pixels_per_unit;
    }
    scrollbar_offset_to_client_units(sb, pixel_offset, motion, offset);
    *object_length = sb->object_length;
}

static int
scrollbar_offset_to_client_units(sb, offset, motion, vs)
    Xv_scrollbar_info *sb;
    long unsigned   offset;
    Scroll_motion   motion;
    long unsigned  *vs;
{
    long unsigned   normalized_offset = sb->view_start;

    switch (motion) {
      case SCROLLBAR_ABSOLUTE:
      case SCROLLBAR_MIN_TO_POINT:
      case SCROLLBAR_TO_END:
      case SCROLLBAR_TO_START:
      case SCROLLBAR_LINE_FORWARD:
      case SCROLLBAR_LINE_BACKWARD:
	normalized_offset = offset / sb->pixels_per_unit;
	break;

      case SCROLLBAR_POINT_TO_MIN:
	normalized_offset =
	    ((offset % sb->pixels_per_unit) == 0) ? offset / sb->pixels_per_unit
	    : offset / sb->pixels_per_unit + 1;
	break;

      case SCROLLBAR_PAGE_FORWARD:
      case SCROLLBAR_PAGE_BACKWARD:
	if (sb->page_length != SCROLLBAR_DEFAULT_LENGTH && sb->page_length != 0) {
	    normalized_offset =
		(offset / (sb->pixels_per_unit * sb->page_length)) * sb->page_length;
	    /* should always divide equally -- won't when last page */
	    /* isn't a full page -- in this case jump to the end */
	    if ((normalized_offset * sb->pixels_per_unit) != offset) {
		normalized_offset = Max_offset(sb);
	    }
	} else {
	    normalized_offset = offset / sb->pixels_per_unit;
	}
	break;

      default:
	break;
    }

    if (normalized_offset >= 0) {
	*vs = normalized_offset;
    } else {
	*vs = 0;
    }
    return (XV_OK);
}


static unsigned long
scrollbar_absolute_offset(sb, pos, length)
    Xv_scrollbar_info *sb;
    int             pos;
    int             length;
{
    if (length <= 0)
	return (0);
    else
	return (Max_offset(sb) * pos / length * sb->pixels_per_unit);
}


#ifdef SCROLL_BY_PAGE
/*
 * return the next integer from a/b
 */
static int
rounded(a, b)
    register int a, b;
{
    if (a % b == 0) {
	return (a / b);
    } else {
	return (a / b + 1);
    }
}
#endif SCROLL_BY_PAGE
