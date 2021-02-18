#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)fm_input.c 20.39 90/03/01";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/fm_impl.h>
#include <xview_private/draw_impl.h>
#include <xview/notice.h>

/* ARGSUSED */
Pkg_private     Notify_value
frame_input(frame_public, event, arg, type)
    Frame           frame_public;
    Event          *event;
    Notify_arg      arg;
    Notify_event_type type;
{
    Frame_class_info *frame = FRAME_CLASS_PRIVATE(frame_public);
    unsigned int    action = event_action(event);
    char           *help_data;

    /* Tell the selection service about GET, PUT, FIND, DELETE */

    switch (action) {
      case ACTION_COPY:
      case ACTION_PASTE:
      case ACTION_CUT:
      case ACTION_FIND_FORWARD:
      case ACTION_FIND_BACKWARD:
	seln_report_event(frame_public, event);
	break;
      default:
	break;
    }
    switch (action) {
      case ACTION_CUT:
	/* only want up of function keys */
	if (event_is_down(event))
	    goto Done;
	break;

      case ACTION_HELP:
	if (event_is_down(event)) {
	    help_data = (char *) xv_get(frame_public, XV_HELP_DATA);
	    if (help_data)
		xv_help_show(frame_public, help_data, event);
	}
	return NOTIFY_DONE;

      case WIN_REPAINT:	/* do not receive these events for the frame */
	break;

      case ACTION_RESCALE:
	frame_rescale_subwindows(frame_public, (int) arg);
	goto Done;
	break;

      case WIN_RESIZE:

	(void) win_getsize(frame_public, &frame->rectcache);

			/* Set width and height size hints for backwards 
			 * compatibility with pre-ICCCM window managers  */
	if (!defaults_get_boolean("xview.icccmcompliant",
				  "XView.ICCCMCompliant", TRUE)) {
		XSizeHints		sizeHints;
		Xv_Drawable_info	*info;

		DRAWABLE_INFO_MACRO(frame_public, info);
		sizeHints.flags = PSize;
		sizeHints.width = frame->rectcache.r_width; 
		sizeHints.height = frame->rectcache.r_height; 
		XSetNormalHints(xv_display(info), xv_xid(info), &sizeHints);
	}

	(void) frame_layout_subwindows(frame_public);
        goto Done;
        break;

      case ACTION_TAKE_FOCUS:{
	    Xv_Window       child = frame->focus_subwindow;

	    /* restore focus to the last subwindow that had the focus */
	    if (child) {
		if (xv_get(child, WIN_KBD_FOCUS) == (Xv_opaque) TRUE)
		    goto Done;

		if (xv_set(child, WIN_SET_FOCUS, 0) == XV_OK)
		    goto Done;
	    }
	    /*
	     * go through the list of subwindows and make sure none of them
	     * have the focus, then find the first subwindow that can accept
	     * keyboard input and assign input focus to it.
	     */
	    FRAME_EACH_CHILD(frame->first_subwindow, child)
		if (xv_get(child, WIN_KBD_FOCUS) == (Xv_opaque) TRUE)
		goto Done;
	    FRAME_END_EACH
	    FRAME_EACH_CHILD(frame->first_subwindow, child)
		if (xv_set(child, WIN_SET_FOCUS, 0) == XV_OK)
		goto Done;
	    FRAME_END_EACH
		break;
	}

      default:
	/* Cannot ignore up events. Forward it */
	if (event_is_up(event))
	    goto Done;
	break;
    }

    switch (action) {

      case ACTION_OPEN:
	if (status_get(frame, map_state_change)) {
	    status_set(frame, map_state_change, FALSE);
	} else {
	    status_set(frame, iconic, FALSE);
	} 
	break;

      case ACTION_CLOSE:
	if (status_get(frame, map_state_change)) {
	    status_set(frame, map_state_change, FALSE);
	} else {
	    status_set(frame, iconic, TRUE);
	} 
	break;

      case ACTION_DISMISS:
	status_set(frame, dismiss, TRUE);
	if (frame->done_proc)
	    (void) (frame->done_proc) (frame_public);
	else
	    (void) (frame->default_done_proc) (frame_public);
	break;

      case ACTION_PROPS:
	frame_handle_props(frame_public);
	break;
    }

Done:
    return notify_next_event_func((Notify_client) frame_public,
				  (Notify_event) event, arg, type);
}
