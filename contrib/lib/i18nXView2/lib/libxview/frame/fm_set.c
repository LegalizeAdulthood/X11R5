#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)fm_set.c 50.11 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <X11/Xlib.h>
#include <xview_private/fm_impl.h>
#include <xview_private/draw_impl.h>
#include <xview/server.h>
#include <xview/cms.h>

#define LEFT_FOOTER 1
#define RIGHT_FOOTER 0

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

#ifdef OW_I18N
/*
 * This is wrong place to put above definition, for the IMStatus
 * defined in fm_impl.h...Ako
 */
#endif

#ifdef PRE_R4_ICCCM
#define XIconifyWindow	xv_iconify_window
#endif

static void     frame_change_state();
#ifndef SVR4
static short    default_frame_icon_image[256] = {
#else SVR4
static unsigned short    default_frame_icon_image[256] = {
#endif SVR4
#include	<images/default.icon>
};

mpr_static(default_frame_icon_mpr, ICON_DEFAULT_WIDTH, ICON_DEFAULT_HEIGHT, 1,
	   default_frame_icon_image);

Pkg_private     Xv_opaque
frame_set_avlist(frame_public, avlist)
    Frame           frame_public;
    Attr_attribute  avlist[];
{
    Attr_avlist     attrs;
    Frame_class_info *frame = FRAME_PRIVATE(frame_public);
    Xv_object       owner;
    int             is_subframe;
    int             result = XV_OK;
    static int	    set_icon_rect;
    static Rect	    icon_rect;	

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch (attrs[0]) {
	  case FRAME_CLOSED:
	  case FRAME_CLOSED_RECT:
	    owner = xv_get(frame_public, XV_OWNER);
	    is_subframe = (int) xv_get(owner, XV_IS_SUBTYPE_OF, FRAME_CLASS);
	    switch (attrs[0]) {
	      case FRAME_CLOSED:
		/* subframes can't be iconic */
		if (!is_subframe)
		    frame_change_state(frame, attrs[1]);
		break;
		/* SunView1 compatibility only */
	      case FRAME_CLOSED_RECT: 
		/* can't set closed rect if subframe */
		if (!is_subframe) {
		    if (frame->icon) {
			Xv_Drawable_info *info;

			DRAWABLE_INFO_MACRO(frame_public, info);

		        (void) win_setrect(frame->icon, (Rect *) attrs[1]);
	    		frame->wmhints.flags |= IconPositionHint;
	    		frame->wmhints.icon_x = ((Rect *)attrs[1])->r_left;
	    		frame->wmhints.icon_y = ((Rect *)attrs[1])->r_top;
		        XSetWMHints(xv_display(info), xv_xid(info),
							     &(frame->wmhints));
		    } else {
		        set_icon_rect = TRUE;
			icon_rect = *(Rect *) attrs[1];
		    }
		}
		break;
	    }
	    break;

	    /* SunView1 compatibility only */
	  case FRAME_CURRENT_RECT:
	    (void) xv_set((frame_is_iconic(frame)) ? frame->icon : frame_public,
			  WIN_RECT, (Rect *) attrs[1], 0);
	    break;

	  case FRAME_DEFAULT_DONE_PROC:
	    frame->default_done_proc = (void (*) ()) attrs[1];
	    if (!frame->default_done_proc)
		frame->default_done_proc = frame_default_done_func;
	    break;

	  case FRAME_DONE_PROC:
	    frame->done_proc = (void (*) ()) attrs[1];
	    break;

	  case FRAME_NO_CONFIRM:
	    status_set(frame, no_confirm, (int) attrs[1]);
	    break;

	  case FRAME_INHERIT_COLORS:
	    xv_set(frame_public, WIN_INHERIT_COLORS, TRUE, 0);
	    break;

	  case FRAME_FOREGROUND_COLOR: {
	    /* 
	     * Create-only attributes must be processed here since it can be 
	     * set through the cmdline.
	     */
	    if (!status_get(frame, created)) {
		frame->fg.red = ((Xv_Singlecolor *)attrs[1])->red;
		frame->fg.green = ((Xv_Singlecolor *)attrs[1])->green;
		frame->fg.blue = ((Xv_Singlecolor *)attrs[1])->blue;
		status_set(frame, frame_color, TRUE);
	    }
	    break;
	  }

	  case FRAME_BACKGROUND_COLOR: {
	    if (!status_get(frame, created)) {
		frame->bg.red = ((Xv_Singlecolor *)attrs[1])->red;
		frame->bg.green = ((Xv_Singlecolor *)attrs[1])->green;
		frame->bg.blue = ((Xv_Singlecolor *)attrs[1])->blue;
		status_set(frame, frame_color, TRUE);
	    }
	    break;
	  }

	  case FRAME_SUBWINDOWS_ADJUSTABLE:
	    status_set(frame, bndrymgr, (int) attrs[1]);
	    break;

	  case FRAME_LEFT_FOOTER:
	    *attrs = (Frame_attribute) ATTR_NOP(*attrs);
#ifdef OW_I18N
	    if (frame->leftfooter_wcs)
		free(frame->leftfooter_wcs);
	    if (frame->leftfooter)
	    {
		free(frame->leftfooter);
		frame->leftfooter = NULL;
	    }
	    if ((char *) attrs[1]) {
		frame->leftfooter_wcs = (wchar_t *) mbstowcsdup(attrs[1]);
	    } else {
		frame->leftfooter_wcs = NULL;
	    }
#else OW_I18N
	    if (frame->leftfooter)
		free(frame->leftfooter);
	    if ((char *) attrs[1]) {
		frame->leftfooter = (char *)
		    calloc(1, strlen((char *) attrs[1]) + 1);
		strcpy(frame->leftfooter, (char *) attrs[1]);
	    } else {
		frame->leftfooter = NULL;
	    }
#endif OW_I18N
	    (void) frame_display_footer(frame, LEFT_FOOTER);
	    break;

#ifdef OW_I18N
	  case FRAME_LEFT_FOOTER_WCS:
	    *attrs = (Frame_attribute) ATTR_NOP(*attrs);
	    if (frame->leftfooter_wcs)
		free(frame->leftfooter_wcs);
	    if (frame->leftfooter)
	    {
		free(frame->leftfooter);
		frame->leftfooter = NULL;
	    }
	    if ((wchar_t *) attrs[1]) {
		frame->leftfooter_wcs = wsdup(attrs[1]);
	    } else {
		frame->leftfooter_wcs = NULL;
	    }
	    (void) frame_display_footer(frame, LEFT_FOOTER);
	    break;
#endif OW_I18N

	  case FRAME_RIGHT_FOOTER:
	    *attrs = (Frame_attribute) ATTR_NOP(*attrs);
#ifdef OW_I18N
	    if (frame->rightfooter_wcs)
		free(frame->rightfooter_wcs);
	    if (frame->rightfooter)
	    {
		free(frame->rightfooter);
		frame->rightfooter = NULL;
	    }
	    if ((char *) attrs[1]) {
		frame->rightfooter_wcs = mbstowcsdup((char *)attrs[1]);
	    } else {
		frame->rightfooter_wcs = NULL;
	    }
#else OW_I18N
	    if (frame->rightfooter)
		free(frame->rightfooter);
	    if ((char *) attrs[1]) {
		frame->rightfooter = (char *)
		    calloc(1, strlen((char *) attrs[1]) + 1);
		strcpy(frame->rightfooter, (char *) attrs[1]);
	    } else {
		frame->rightfooter = NULL;
	    }
#endif OW_I18N
	    (void) frame_display_footer(frame, RIGHT_FOOTER);
	    break;

#ifdef OW_I18N
	  case FRAME_RIGHT_FOOTER_WCS:
	    *attrs = (Frame_attribute) ATTR_NOP(*attrs);
	    if (frame->rightfooter_wcs)
		free(frame->rightfooter_wcs);
	    if (frame->rightfooter)
	    {
		free(frame->rightfooter);
		frame->rightfooter = NULL;
	    }
	    if ((wchar_t *) attrs[1]) {
		frame->rightfooter_wcs = wsdup((wchar_t *) attrs[1]);
	    } else {
		frame->rightfooter_wcs = NULL;
	    }
	    (void) frame_display_footer(frame, RIGHT_FOOTER);
	    break;
#endif OW_I18N

	  case FRAME_ICON:
	    {
		unsigned long   icon_id;
		Xv_Drawable_info *info;
		short           sameicon = FALSE;

		if (frame->icon) {
		    if (frame->icon == (Icon) attrs[1])
			/*
			 * this will prevent notifying WM with the same icon
			 * window. The current WM will destroy the window
			 * even though it is the same one.  This will cause
			 * no icon to appear.  If Stuart's WM works
			 * different, then maybe this code can be taken out
			 * and a bug to be filed against generic WM.
			 */
			sameicon = TRUE;
		    else if (frame->icon == frame->default_icon)
			/* Destroy only the default icon */
			xv_destroy(frame->icon);
		}
		frame->icon = (Icon) attrs[1];
		xv_set(frame->icon, XV_OWNER, frame_public, 0);
		icon_id = (unsigned long) xv_get(frame->icon, XV_XID);
		if (set_icon_rect) {
		    (void) win_setrect(frame->icon, icon_rect);
		}

		if (!sameicon) {
		    frame->wmhints.flags |= IconWindowHint;
		    frame->wmhints.icon_window = icon_id;
		    if (set_icon_rect) {
		        frame->wmhints.flags |= IconPositionHint;
	    	        frame->wmhints.icon_x = icon_rect.r_left;
	    	        frame->wmhints.icon_y = icon_rect.r_top;
		    }
		    DRAWABLE_INFO_MACRO(frame_public, info);
		    XSetWMHints(xv_display(info), xv_xid(info),
							     &(frame->wmhints));
		}
		set_icon_rect = FALSE;
		break;
	    }

	  case FRAME_BUSY:
	    status_set(frame, busy, FALSE);
	    if ((int) attrs[1]) {
		if (xv_deaf(frame_public, TRUE) != XV_OK)  {
#ifdef OW_I18N
		    xv_error(frame_public,
				ERROR_STRING,
				XV_I18N_MSG("xv_messages",
					"Attempt to make frame deaf failed"),
				0);
#else
		    xv_error(frame_public,
				ERROR_STRING,
				"Attempt to make frame deaf failed",
				0);
#endif OW_I18N
		    result = XV_ERROR;
		    break;
		}
		frame_display_busy(frame, WMWindowIsBusy);
		status_set(frame, busy, TRUE);
	    } else {
		if (xv_deaf(frame_public, FALSE) != XV_OK)  {
#ifdef OW_I18N
		    xv_error(frame_public,
				ERROR_STRING,
				XV_I18N_MSG("xv_messages",
					"Attempt to make frame undeaf failed"),
				0);
#else
		    xv_error(frame_public,
				ERROR_STRING,
				"Attempt to make frame undeaf failed",
				0);
#endif OW_I18N
		    result = XV_ERROR;
		    break;
		}
		frame_display_busy(frame, WMWindowNotBusy);
		status_set(frame, busy, FALSE);
	    }

	    break;

	  case FRAME_SCALE_STATE:
	    /*
	     * set the local rescale state bit, then tell the WM the current
	     * state, and then set the scale of our subwindows
	     */
	    /*
	     * WAIT FOR NAYEEM window_set_rescale_state(frame_public,
	     * attrs[1]);
	     */
	    wmgr_set_rescale_state(frame_public, attrs[1]);
	    frame_rescale_subwindows(frame_public, attrs[1]);
	    break;

	  case XV_SHOW:
	    /*
	     * Map the icon. The FRAME pkg should not let the window package
	     * know about icons and wmgr.
	     */
	    if (frame_is_iconic(frame) && (int) attrs[1]) {
		attrs[0] = (Frame_attribute) WIN_MAP;
		wmgr_top(frame_public);
	    }
	    /* Don't expect a change of state if we are already in the state */
	    /* requested.  Also if our initial state is iconic, don't expect */
	    /* the frame to see a change of state and if we are iconic going */
	    /* to withdrawn, don't expect a state change.		     */
	    if ((window_get_map_cache(frame_public) != (int)attrs[1]) &&
		 (((int)attrs[1] && !status_get(frame, initial_state)) ||
		  (!(int)attrs[1] && !frame_is_iconic(frame))))
	    	status_set(frame, map_state_change, TRUE);

	    break;

#ifdef OW_I18N

          case FRAME_LEFT_IMSTATUS:
            *attrs = (Frame_attribute) ATTR_NOP(*attrs);
            if (frame->leftIMstatus)
            {
                free(frame->leftIMstatus);
                frame->leftIMstatus = NULL;
            }
            if (frame->leftIMstatus_wcs) {
                free(frame->leftIMstatus_wcs);
            }
            if ((char *) attrs[1]) {
                frame->leftIMstatus_wcs = mbstowcsdup((char *)attrs[1]);
            } else {
                frame->leftIMstatus_wcs = NULL;
            }
            (void) frame_display_IMstatus(frame, LEFT_IMSTATUS);
            break;

	  case FRAME_LEFT_IMSTATUS_WCS:
	    *attrs = (Frame_attribute) ATTR_NOP(*attrs);
	    if (frame->leftIMstatus_wcs)
		free(frame->leftIMstatus_wcs);
	    if ((wchar_t *) attrs[1]) {
		frame->leftIMstatus_wcs = wsdup((wchar_t *)attrs[1]);
	    } else {
		frame->leftIMstatus_wcs = NULL;
	    }
	    (void) frame_display_IMstatus(frame, LEFT_IMSTATUS);
	    break;


          case FRAME_RIGHT_IMSTATUS:
            *attrs = (Frame_attribute) ATTR_NOP(*attrs);
            if (frame->rightIMstatus)
            {
                free(frame->rightIMstatus);
                frame->rightIMstatus = NULL;
            }
            if (frame->rightIMstatus_wcs) {
                free(frame->rightIMstatus_wcs);
            }
            if ((char *) attrs[1]) {
                frame->rightIMstatus_wcs = mbstowcsdup((char *)attrs[1]);
            } else {
                frame->rightIMstatus_wcs = NULL;
            }
            (void) frame_display_IMstatus(frame, RIGHT_IMSTATUS);
            break;

	  case FRAME_RIGHT_IMSTATUS_WCS:
	    *attrs = (Frame_attribute) ATTR_NOP(*attrs);
	    if (frame->rightIMstatus_wcs)
		free(frame->rightIMstatus_wcs);
	    if ((wchar_t *) attrs[1]) {
		frame->rightIMstatus_wcs = wsdup((wchar_t *)attrs[1]);
	    } else {
		frame->rightIMstatus_wcs = NULL;
	    }
	    (void) frame_display_IMstatus(frame, RIGHT_IMSTATUS);
	    break;
#endif OW_I18N

	  case XV_END_CREATE:{
		unsigned long   icon_id;
		Xv_Drawable_info *info;

		DRAWABLE_INFO_MACRO(frame_public, info);

		owner = xv_get(frame_public, XV_OWNER);
		is_subframe = (int) xv_get(owner, XV_IS_SUBTYPE_OF, FRAME);

		/*
		 * if this is a subframe, we need to reparent it to be a
		 * child of the root window and not of the main frame
		 */
		if (is_subframe) {
		    xv_set(frame_public,
			   WIN_PARENT, xv_get(frame_public, XV_ROOT),
			   0);
		} else if (!frame->icon) {
		    /* Create a default icon for a main frame */
		    if ((xv_depth(info) > 1) && (xv_dynamic_color(info))) {
			/* icon inherits frame color properties */
			frame->icon = frame->default_icon = 
			    xv_create(owner, ICON,
				XV_OWNER, frame_public,
				ICON_IMAGE, &default_frame_icon_mpr,
				WIN_DYNAMIC_VISUAL, TRUE,
				0);
		    } else {
		        frame->icon = frame->default_icon = 
			    xv_create(owner, ICON,
				XV_OWNER, frame_public,
				ICON_IMAGE, &default_frame_icon_mpr,
				0);
		    }

		    icon_id = (unsigned long) xv_get(frame->icon, XV_XID);
		    if (set_icon_rect) {
		        (void) win_setrect(frame->icon, icon_rect);
		    }
		    frame->wmhints.flags |= IconWindowHint;
		    frame->wmhints.icon_window = icon_id;
		    if (set_icon_rect) {
		        frame->wmhints.flags |= IconPositionHint;
	    	        frame->wmhints.icon_x = icon_rect.r_left;
	    	        frame->wmhints.icon_y = icon_rect.r_top;
		    }
		    XSetWMHints(xv_display(info), xv_xid(info),
							     &(frame->wmhints));
		    set_icon_rect = FALSE;
		}
		if (set_icon_rect) {
		    set_icon_rect = FALSE;
		    (void) win_setrect(frame->icon, icon_rect);
	    	    frame->wmhints.flags |= IconPositionHint;
	    	    frame->wmhints.icon_x = icon_rect.r_left;
	    	    frame->wmhints.icon_y = icon_rect.r_top;
		    XSetWMHints(xv_display(info), xv_xid(info),
							     &(frame->wmhints));
		}

		/*
		 * Deal with command line and set size and position if this
		 * is the first time set is called. Command line only applies
		 * to the first frame.
		 */
		if (frame_notify_count == 1) {
		    /*
		     * set the command line options.  This gives the command
		     * line the highest precedence: default, client attrs,
		     * command line.
		     */
		    if (frame_set_cmdline_options(frame_public, avlist) != XV_OK)
			result = XV_ERROR;
		    if (frame_set_icon_cmdline_options(frame_public, avlist)
			!= XV_OK)
			result = XV_ERROR;
		}
		/*
		 * Now position the frame if its position is not fixed yet.
		 */
		frame_set_position(owner, frame);


		/*
		 * On color, set frame colors, if any. Make icon inherit 
		 * frame's fg/bg. 
		 */
		if (xv_depth(info) > 1) {
		    if (status_get(frame, frame_color)) {
			frame_set_color(frame, &frame->fg, &frame->bg);
		    }

		    if (frame->icon) {
			Cms	cms;
			      
			/* 
			 * Make sure we have the same visuals for the icon
			 * and the frame, then inherit colors.
			 */
			if (xv_get(frame_public, XV_VISUAL_CLASS, 0) ==
			    xv_get(frame->icon, XV_VISUAL_CLASS, 0)) {
			    /*
			     * Dont override icon's colormap segment if it has been
			     * set programatically.
			     */
			     cms = (Cms) xv_get(frame->icon, WIN_CMS);
			     if (xv_get(cms, CMS_DEFAULT_CMS) && 
				 (cms != xv_cms(info)))
			       xv_set(frame->icon, WIN_CMS, xv_cms(info), 0);
		     }
			
		  }
	    }
		
		status_set(frame, created, TRUE);
		break;
	}

	  default:
	    xv_check_bad_attr(&xv_frame_class_pkg, attrs[0]);
	    break;

	}
    }

    return (Xv_opaque) result;
}


/*
 * Change the state of frame to iconic or open.
 */
static void
frame_change_state(frame, to_iconic)
    Frame_class_info *frame;
    int               to_iconic;
{
    Frame             frame_public = FRAME_PUBLIC(frame);
    Xv_Drawable_info *info;

    DRAWABLE_INFO_MACRO(frame_public, info);
    status_set(frame, iconic, to_iconic);

    /* If the window is not mapped yet, then we just change the intial state  */
    /* so that when the window is mapped, it will appear as the user expected */
    if (!xv_get(frame_public, XV_SHOW)) {
	status_set(frame, initial_state, to_iconic);
        frame->wmhints.initial_state = (to_iconic ? IconicState : NormalState);
	frame->wmhints.flags |= StateHint;
	XSetWMHints(xv_display(info), xv_xid(info), &(frame->wmhints));

    /* If the window is already mapped, then change it from Normal to Iconic */
    /* or Iconic to Normal.						     */
    } else {
        /* icon is dealt with in wmgr_close/open() */
    	if (to_iconic)
	    XIconifyWindow(xv_display(info), xv_xid(info),
					xv_get(xv_screen(info), SCREEN_NUMBER));
        else {
	    status_set(frame, map_state_change, TRUE);
	    XMapWindow(xv_display(info), xv_xid(info));
	}
    }
}
