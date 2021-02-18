#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)txt_tool.c 50.5 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Initialization and finalization of text subwindows.
 */

#include <xview_private/primal.h>
#include <xview_private/txt_impl.h>
#include <xview/notice.h>
#include <xview/frame.h>
#include <xview/wmgr.h>
#include <fcntl.h>
#define _NOTIFY_MIN_SYMBOLS
#include <xview/notify.h>
#undef _NOTIFY_MIN_SYMBOLS

#include <xview/win_struct.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

extern Notify_error win_post_event();
Pkg_private Textsw_view_handle textsw_init_internal();

Pkg_private int
textsw_default_notify(abstract, attrs)
    Textsw          abstract;
    Attr_attribute *attrs;
{
    register Textsw_view_handle view = VIEW_ABS_TO_REP(abstract);
    register Frame  frame = xv_get(abstract, WIN_FRAME);

    for (; *attrs; attrs = attr_next(attrs)) {
	switch (*attrs) {
	  case TEXTSW_ACTION_TOOL_CLOSE:
	  case TEXTSW_ACTION_TOOL_MGR:
	  case TEXTSW_ACTION_TOOL_DESTROY:
	  case TEXTSW_ACTION_TOOL_QUIT:{
		switch ((Textsw_action) (*attrs)) {
		  case TEXTSW_ACTION_TOOL_CLOSE:
		    if (!xv_get(frame, FRAME_CLOSED))
			xv_set(frame, FRAME_CLOSED, TRUE, 0);
		    break;
		  case TEXTSW_ACTION_TOOL_MGR:{
			(void) win_post_event(frame,
				      (Event *) attrs[1], NOTIFY_IMMEDIATE);
			break;
		    }
		  case TEXTSW_ACTION_TOOL_QUIT:
		    if (textsw_has_been_modified(abstract)) {
			int             result;
			result = notice_prompt(
					       frame,
					       (Event *) 0,
					       NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
					       XV_I18N_MSG("xv_messages", "\
The text has been edited.\n\
 \n\
You may discard edits now and quit, or cancel\n\
the request to Quit and go back and either save the\n\
contents or store the contents as a new file."),
#else
					       "The text has been edited.",
					       " ",
			    "You may discard edits now and quit, or cancel",
					       "the request to Quit and go back and either save the",
			    "contents or store the contents as a new file.",
#endif
					       0,
				   NOTICE_BUTTON_YES, 
#ifdef OW_I18N
					XV_I18N_MSG("xv_messages", "Cancel, do NOT Quit"),
#else
					"Cancel, do NOT Quit",
#endif
			     NOTICE_BUTTON, 
#ifdef OW_I18N
					XV_I18N_MSG("xv_messages", "Discard edits, then Quit"),
#else
					"Discard edits, then Quit", 
#endif
					123,
					       NOTICE_TRIGGER, ACTION_STOP,
					       0);
			if ((result == ACTION_STOP) || (result == NOTICE_YES) || (result == NOTICE_FAILED)) {
			    break;
			} else {
			    (void) textsw_reset(abstract, 0, 0);
			    (void) textsw_reset(abstract, 0, 0);
			}
		    }
		    xv_destroy_safe(frame);
		    break;
		  case TEXTSW_ACTION_TOOL_DESTROY:
		    xv_set(frame, FRAME_NO_CONFIRM, TRUE, 0);
		    xv_destroy_safe(frame);
		    break;
		}
		break;
	    }
	  default:
	    break;
	}
    }
}
