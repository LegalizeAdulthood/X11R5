#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)fm_layout.c 50.4 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/fm_impl.h>
#include <xview_private/draw_impl.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N


static Xv_Window frame_prev_child();
static Xv_Window frame_last_child();
static void     expand_sw();


/* ARGSUSED */
/* VARARGS3 */
Pkg_private int
frame_layout(frame_public, child, op, d1, d2, d3, d4, d5)
    Frame           frame_public;
    register Xv_Window child;
    Window_layout_op op;
    int             d1, d2, d3, d4, d5;
{
    register Frame_class_info *frame = FRAME_CLASS_PRIVATE(frame_public);
    int             is_subframe;
    Rect            rect;


    is_subframe = ((int) xv_get(frame_public, XV_IS_SUBTYPE_OF, FRAME_CLASS) &&
		   (int) xv_get(child, XV_IS_SUBTYPE_OF, FRAME_CLASS));

    switch (op) {
      case WIN_CREATE:{
	    Xv_Window       last_child;
	    Rect            current_rect;
	    unsigned int    rect_info;
	    int             desired_w = WIN_EXTEND_TO_EDGE, desired_h = WIN_EXTEND_TO_EDGE;

	    if (is_subframe)
		last_child = frame_last_child(frame->first_subframe);
	    else {
		/*
		 * set the older sibling of this subwindow to the last
		 * created subwindow.
		 */
		last_child = frame_last_child(frame->first_subwindow);
		(void) frame_position_sw(frame, last_child, WIN_EXTEND_TO_EDGE,
					 WIN_EXTEND_TO_EDGE, &rect);
		(void) win_getrect(child, &current_rect);	/* inner rect */
		rect_info = (unsigned int) xv_get(child, WIN_RECT_INFO);
		/*
		 * compare the rects that client sets for the subwindow w/
		 * the rect that the frame sets for the subwindow. if client
		 * has set a dimension, then leave it alone
		 */
		if (rect_info & WIN_X_SET) {
		    rect.r_left = current_rect.r_left;
		}
		if (rect_info & WIN_Y_SET) {
		    rect.r_top = current_rect.r_top;
		}
		if (rect_info & WIN_WIDTH_SET) {
		    rect.r_width = desired_w = current_rect.r_width;
		}
		if (rect_info & WIN_HEIGHT_SET) {
		    rect.r_height = desired_h = current_rect.r_height;
		}
		(void) win_setrect(child, &rect);
		/* subwindows are extend-to-edge by default */
		xv_set(child,
		       WIN_DESIRED_WIDTH, desired_w,
		       WIN_DESIRED_HEIGHT, desired_h,
		       0);
	    }
	    /* add the child to the subwindow or subframe list */
	    if (last_child)
		xv_set(last_child, XV_KEY_DATA, FRAME_NEXT_CHILD, child, 0);
	    else if (is_subframe)
		frame->first_subframe = child;
	    else
		frame->first_subwindow = child;
	    /* this is the last child in the list */
	    xv_set(child, XV_KEY_DATA, FRAME_NEXT_CHILD, 0, 0);
	    break;
	}

      case WIN_DESTROY:{
	    Xv_Window      *first_child;
	    Xv_Window       prev_child = NULL;
	    Xv_Window       next_child = NULL;

	    /*
	     * we cannot rely on the is_subframe flag for destroy because the
	     * pkg id changes on destruction.  Therefore we must ask the
	     * owner whether we are a subframe or not.  Searching down the
	     * subframe list should be faster than searching down the
	     * subwindow list.
	     */
	    is_subframe = FALSE;
	    FRAME_EACH_CHILD(frame->first_subframe, next_child)
		if (child == next_child) {
		is_subframe = TRUE;
		break;
	    }
	    prev_child = next_child;
	    FRAME_END_EACH

		first_child = is_subframe ?
		&frame->first_subframe : &frame->first_subwindow;
	    prev_child = is_subframe ?
		prev_child : frame_prev_child(*first_child, child);
	    next_child = xv_get(child,
				XV_KEY_DATA, FRAME_NEXT_CHILD);

	    /* remove the child from the subwindow or subframe list */
	    if (prev_child)
		xv_set(prev_child,
		       XV_KEY_DATA, FRAME_NEXT_CHILD, next_child, 0);
	    else
		*first_child = next_child;

	    /* don't reference the child anymore if it had the input focus */
	    if (child == frame->focus_subwindow)
		frame->focus_subwindow = NULL;
	    break;
	}

      case WIN_INSERT:{

	    (void) win_insert(child);
	    if (is_subframe)
		break;

	    if (EXTEND_HEIGHT(child) || EXTEND_WIDTH(child)) {
		(void) win_get_outer_rect(child, &rect);
		expand_sw(frame, child, &rect);
		(void) win_set_outer_rect(child, &rect);
	    }
	    break;
	}

      case WIN_REMOVE:
	(void) win_remove(child);
	if (!is_subframe && window_get(child, WIN_KBD_FOCUS))
	    (void) win_set_kbd_focus(child, WIN_NULLLINK);
	break;

      case WIN_LAYOUT:{
	    int            *layout_supported = (int *) d1;

	    *layout_supported = TRUE;
	    break;
	}


      case WIN_GET_BELOW:
	/*
	 * BUG: If the subwindow we want to be below is EXTEND_TO_EDGE, then
	 * this operation is simply going to put the current child on top of
	 * d1, not below it. ditto for RIGHT_OF.
	 */
	if (window_getrelrect(child, d1, &rect) != XV_OK)	/* outer rect of d1 */
	    return 0;
	{
	    int            *y = (int *) d2;
	    *y = rect.r_top + rect.r_height;	/* + FRAME_BORDER_WIDTH; */

	    if (is_subframe) {
		/* position relative to the base frame */
		(void) win_getrect(frame_public, &rect);	/* inner */
		*y += FRAME_BORDER_WIDTH - rect.r_top;
	    } else {
		/* account for the namestripe and iconic offset */
		*y -= FRAME_BORDER_WIDTH;	/* frame_stripe_height(frame);
						 * */
	    }
	}
	break;


      case WIN_ADJUST_RECT:
	frame_adjust_rect(frame_public, frame, child, is_subframe, d1);
	break;

      case WIN_GET_RIGHT_OF:
	if (window_getrelrect(child, d1, &rect) != XV_OK)
	    return 0;
	{
	    int            *x = (int *) d2;
	    *x = rect.r_left + rect.r_width;

	    if (is_subframe) {
		(void) win_getrect(frame_public, &rect);	/* inner */
		*x += FRAME_BORDER_WIDTH - rect.r_left;
	    } else
		*x -= FRAME_BORDER_WIDTH;
	}
	break;


      case WIN_GET_X:{
	    int            *x = (int *) d1;

	    (void) win_getrect(child, &rect);	/* inner rect */
	    *x = rect.r_left;
	    /* convert to frame space */
	    if (is_subframe) {
		(void) win_getrect(frame_public, &rect);	/* inner rect */
		*x -= rect.r_left;
	    } else
		*x -= FRAME_BORDER_WIDTH;
	    break;
	}

      case WIN_GET_Y:{
	    int            *y = (int *) d1;

	    (void) win_getrect(child, &rect);	/* inner rect */
	    *y = rect.r_top;
	    if (is_subframe) {
		(void) win_getrect(frame_public, &rect);	/* inner rect */
		*y -= rect.r_top;
	    } else
		*y -= FRAME_BORDER_WIDTH;	/* frame_stripe_height(frame) */
	    break;
	}

      case WIN_GET_WIDTH:{
	    int            *w = (int *) d1;

	    (void) win_getrect(child, &rect);	/* inner rect */
	    *w = rect.r_width;
	    break;
	}

      case WIN_GET_HEIGHT:{
	    int            *h = (int *) d1;

	    (void) win_getrect(child, &rect);	/* inner rect */
	    *h = rect.r_height;
	    break;
	}

      case WIN_GET_RECT:{
	    Rect           *r = (Rect *) d1;

	    (void) win_getrect(child, r);	/* inner rect */
	    if (is_subframe) {
		/* translate to owner-frame space */
	        /*(void) win_getrect(frame_public, &rect); */	/* inner rect */
		;
	    } else {
		r->r_left -= FRAME_BORDER_WIDTH;
		r->r_top -= FRAME_BORDER_WIDTH;	/* frame_stripe_height(frame);
						 */
	    }
	    break;
	}

      default:{
	    char            dummy[128];

#ifdef OW_I18N
	    (void) sprintf(dummy,
		XV_I18N_MSG("xv_messages",
		
"frame_layout(internal error): frame layout option (%d) not recognized."), op);
#else
	    (void) sprintf(dummy,
"frame_layout(internal error): frame layout option (%d) not recognized.", op);
#endif OW_I18N
	    xv_error(frame,
		     ERROR_STRING, dummy,
		     ERROR_PKG, FRAME,
		     0);
	    return FALSE;
	}

    }
    return TRUE;
}

frame_adjust_rect(frame_public, frame, child, is_subframe, r)
    Frame           frame_public;
    Frame_class_info *frame;
    Xv_Window       child;
    int             is_subframe;
    Rect           *r;
{
    Rect            rect;
    int             rect_info = (int) xv_get(child, WIN_RECT_INFO);
    int             is_frame = (is_subframe ||
			(int) xv_get(child, XV_IS_SUBTYPE_OF, FRAME_CLASS));

    if (!is_subframe) {
	r->r_left += FRAME_BORDER_WIDTH;
	r->r_top += FRAME_BORDER_WIDTH;	/* frame_stripe_height(frame); */

	if (rect_info & WIN_WIDTH_SET)
	    window_set(child, WIN_DESIRED_WIDTH, r->r_width, 0);
	if (rect_info & WIN_HEIGHT_SET)
	    window_set(child, WIN_DESIRED_HEIGHT, r->r_height, 0);
	if ((r->r_width == WIN_EXTEND_TO_EDGE) ||
	    (r->r_height == WIN_EXTEND_TO_EDGE))
	    expand_sw(frame, child, r);
    }
    (void) win_setrect(child, r);

    if (is_frame) {
	                 /* Set width and height size hints for backwards 
                          * compatibility with pre-ICCCM window managers  */
        if (!defaults_get_boolean("xview.icccmcompliant",
                                  "XView.ICCCMCompliant", TRUE)) {
        	XSizeHints              sizeHints;
        	Xv_Drawable_info        *info;
			       
		DRAWABLE_INFO_MACRO(child, info);
        	sizeHints.flags = PSize;
        	sizeHints.width = r->r_width;
        	sizeHints.height = r->r_height;
        	XSetNormalHints(xv_display(info), xv_xid(info), &sizeHints);
	}

	frame_grant_extend_to_edge(child, FALSE);
	frame_grant_extend_to_edge(child, TRUE);
    }
}

static          Xv_Window
frame_last_child(first)
    Xv_Window       first;
{
    Xv_Window       child;
    Xv_Window       last = first;

    FRAME_EACH_CHILD(first, child)
	last = child;
    FRAME_END_EACH
	return last;
}


static          Xv_Window
frame_prev_child(first, target_child)
    Xv_Window       first, target_child;
{
    Xv_Window       child;
    Xv_Window       prev = 0;

    FRAME_EACH_CHILD(first, child)
	if (child == target_child)
	break;
    prev = child;
    FRAME_END_EACH
	return prev;
}

static void
expand_sw(frame, child, rectp)
    Frame_class_info *frame;
    Xv_Window       child;
    Rect           *rectp;
{
    Rect            rect;

    (void) win_getrect(FRAME_PUBLIC(frame), &rect);	/* inner rect */

    if (EXTEND_WIDTH(child))
	rectp->r_width = rect.r_width - rectp->r_left;
    /* rectp->r_width = rect.r_width - FRAME_BORDER_WIDTH - rectp->r_left; */
    if (EXTEND_HEIGHT(child))
	rectp->r_height = rect.r_height - rectp->r_top;
    /* rectp->r_height = rect.r_height - FRAME_BORDER_WIDTH - rectp->r_top; */
    /* don't allow the subwindow to be less than 1 x 1 */
    if (rectp->r_width < 1)
	rectp->r_width = 1;
    if (rectp->r_height < 1)
	rectp->r_height = 1;
}

/*
 * make subwindows that border the frame be extend-to-edge.
 */
Xv_private void
frame_grant_extend_to_edge(frame_public, to_right)
    Frame           frame_public;
    register int    to_right;
{
    Frame_class_info *frame = FRAME_CLASS_PRIVATE(frame_public);
    Xv_Window       child;
    register int    limit;
    Rect            rect;

    (void) frame_getnormalrect(frame, &rect);

    if (to_right)
	limit = rect.r_width - 1;	/* FRAME_BORDER_WIDTH - 1; */
    else
	limit = rect.r_height - 1;	/* FRAME_BORDER_WIDTH - 1; */

    FRAME_EACH_SHOWN_SUBWINDOW(frame, child)
	(void) win_get_outer_rect(child, &rect);
    if (to_right) {
	if (rect_right(&rect) == limit)
	    window_set(child, WIN_DESIRED_WIDTH, WIN_EXTEND_TO_EDGE, 0);
    } else if (rect_bottom(&rect) == limit)
	window_set(child, WIN_DESIRED_HEIGHT, WIN_EXTEND_TO_EDGE, 0);
    FRAME_END_EACH
}
