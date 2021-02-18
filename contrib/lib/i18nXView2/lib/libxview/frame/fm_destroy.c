#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)fm_destroy.c 50.4 90/11/04";
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

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

static int      frame_confirm_destroy();
static void     frame_free();

/*
 * Destroy the frame struct, its subwindows, and its subframes. If any
 * subwindow or subframe vetoes the destroy, return NOTIFY_IGNORED. If
 * confirmation is enabled, prompt the user.
 */
Pkg_private int
frame_destroy(frame_public, status)
    Frame           frame_public;
    Destroy_status  status;
{
    Frame_class_info *frame = FRAME_CLASS_PRIVATE(frame_public);
    Xv_Window       child;
    Xv_Drawable_info *info;
    int             is_subframe = (int) xv_get(xv_get(frame_public, WIN_OWNER),
					     XV_IS_SUBTYPE_OF, FRAME_CLASS);

    DRAWABLE_INFO_MACRO(frame_public, info);

    /* Unmapped the frame that is about to be destroyed. */
    if ((status == DESTROY_CLEANUP) || (status == DESTROY_PROCESS_DEATH))
	win_remove(frame_public);

    /* If FRAME_GROUP_LEADER is set for this frame, then destroy all other  */
    /* top-level frames.                                                    */
    if (((status != DESTROY_CHECKING) && (status != DESTROY_SAVE_YOURSELF)) &&
	!is_subframe) {
	int             frame_count = (int) xv_get(xv_server(info), XV_KEY_DATA, FRAME_COUNT);

	/* if this was the last top-level frame...go to process death mode. */
	if (--frame_count == 0)
	    xv_set_destruction_type(DESTROY_PROCESS_DEATH);
	else
	    xv_set(xv_server(info), XV_KEY_DATA, FRAME_COUNT, frame_count, 0);
    }
    /* destroy the subframes */
    FRAME_EACH_SUBFRAME(frame, child)
	if (xv_destroy_status(child, ((status == DESTROY_CHECKING) ||
			       (status == DESTROY_SAVE_YOURSELF)) ? (int) status :
			      xv_get_destruction_type()) != XV_OK)
	return XV_ERROR;
    FRAME_END_EACH

    /* Since this frame is going away, set a flag to tell window_destroy_win_struct */
    /* not to destroy any subwindows...when the frame window is destroyed    */
    /* the server will destroy all subwindows for us.			     */
	if ((status != DESTROY_CHECKING) && (status != DESTROY_SAVE_YOURSELF))
	window_set_parent_dying();

    /* destroy the subwindows */
    FRAME_EACH_SUBWINDOW(frame, child)
	if (xv_destroy_status(child, ((status == DESTROY_CHECKING) ||
			       (status == DESTROY_SAVE_YOURSELF)) ? (int) status :
			      xv_get_destruction_type()) != XV_OK)
	return XV_ERROR;
    FRAME_END_EACH

    if ((status != DESTROY_CHECKING) && (status != DESTROY_SAVE_YOURSELF)) {
	window_unset_parent_dying();
	/*
	 * Conditionally stop the notifier.  This is a special case so that
	 * single tool clients don't have to interpose in from of tool_death
	 * in order to notify_remove(...other clients...) so that
	 * notify_start will return.
	 */
	if (--frame_notify_count == 0)
	    (void) notify_stop();
    } else if (status != DESTROY_SAVE_YOURSELF) {
	/* subframe does not need confirmation */
	return ((is_subframe || frame_confirm_destroy(frame) == XV_OK)
		? XV_OK : XV_ERROR);
    }
    if (status == DESTROY_CLEANUP) {	/* waste of time if ...PROCESS_DEATH */
	frame_free(frame);
    }
    return XV_OK;
}

/*
 * Return XV_OK if confirmation is allowed, and the user says yes, or if
 * confirmation is not allowed. Reset confirmation to be allowed.
 */
static int
frame_confirm_destroy(frame)
    Frame_class_info *frame;
{
    Xv_object       window = FRAME_CLASS_PUBLIC(frame);
    int             result;

    if (status_get(frame, no_confirm)) {
	status_set(frame, no_confirm, FALSE);
	return XV_OK;
    }
    if (frame_is_iconic(frame))
	window = (Xv_object) frame->icon;

#ifdef OW_I18N
    result = notice_prompt((Frame) window, 0,
		NOTICE_MESSAGE_STRINGS,
		  XV_I18N_MSG("xv_messages", "Are you sure you want to Quit?"), 0,
		NOTICE_BUTTON_YES,
		  XV_I18N_MSG("xv_messages", "Confirm"),
		NOTICE_BUTTON_NO,
		  XV_I18N_MSG("xv_messages", "Cancel"),
		NOTICE_NO_BEEPING, 1,
		0);
#else

    result = notice_prompt((Frame) window, 0,
		NOTICE_MESSAGE_STRINGS, "Are you sure you want to Quit?", 0,
			   NOTICE_BUTTON_YES, "Confirm",
			   NOTICE_BUTTON_NO, "Cancel",
			   NOTICE_NO_BEEPING, 1,
			   0);
#endif OW_I18N
    /* BUG ALERT Should not abort if alerts failed */
    if (result == NOTICE_FAILED)
	xv_error(frame,
	         ERROR_STRING, "Notice failed on attempt to destroy frame.",
		 ERROR_PKG, FRAME,
		 0);
    return ((result == NOTICE_YES) ? XV_OK : XV_ERROR);
}


/*
 * free the frame struct and all its resources.
 */
static void
frame_free(frame)
    Frame_class_info *frame;
{
    if (frame->icon)
	icon_destroy(frame->icon);

    /* Free frame struct */
    if ((char *) frame->label)
    	free((char *) frame->label);
    free((char *) frame);
}
