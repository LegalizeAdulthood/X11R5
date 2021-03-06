#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)fm_get.c 50.6 90/10/16";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/fm_impl.h>
#include <xview_private/draw_impl.h>
#include <xview/server.h>

static int      frame_fit_direction();

Pkg_private     Xv_opaque
frame_get_attr(frame_public, status, attr, valist)
    Frame           frame_public;
    int            *status;
    Frame_attribute attr;
    va_list         valist;
{
    register Frame_class_info *frame = FRAME_CLASS_PRIVATE(frame_public);
    static Rect     rect_local;

    switch (attr) {
      case WIN_FIT_WIDTH:
	return (Xv_opaque) frame_fit_direction(frame, WIN_DESIRED_WIDTH);

      case WIN_FIT_HEIGHT:
	return (Xv_opaque) frame_fit_direction(frame, WIN_DESIRED_HEIGHT);

	/* Here for SunView1 compatibility only. */
      case FRAME_CLOSED_RECT:{
	    register int    is_subframe = (int) xv_get(
					    xv_get(frame_public, WIN_OWNER),
					     XV_IS_SUBTYPE_OF, FRAME_CLASS);

	    /* subframes don't have a closed rect */
	    if (is_subframe)
		return NULL;
	    (void) win_getrect(frame->icon, &rect_local);
	    return (Xv_opaque) & rect_local;
	}

	/* Here for SunView1 compatibility only. */
      case FRAME_CURRENT_RECT:
	if (frame_is_iconic(frame)) {
	    (void) win_getrect(frame->icon, &rect_local);
	    return (Xv_opaque) & rect_local;
	} else {
	    return xv_get(frame_public, WIN_RECT);
	}

      case FRAME_OLD_RECT:
	return ((Xv_opaque) & frame->oldrect);

      case FRAME_DEFAULT_DONE_PROC:
	return (Xv_opaque) frame->default_done_proc;

      case FRAME_DONE_PROC:
	return (Xv_opaque) (frame->done_proc ?
			    frame->done_proc : frame->default_done_proc);

      case FRAME_FOREGROUND_COLOR: 
	  return ((Xv_opaque) (&frame->fg));

      case FRAME_BACKGROUND_COLOR:
	  return ((Xv_opaque) (&frame->bg));

      case FRAME_ICON:
	return frame->icon;

      case FRAME_LABEL:
#ifdef OW_I18N
	if (frame->label == NULL)
		frame->label = wcstombsdup(frame->label_wcs);
#endif
	return (Xv_opaque) frame->label;

#ifdef OW_I18N
      case XV_LABEL_WCS:
	return (Xv_opaque) frame->label_wcs;
#endif

      case FRAME_LEFT_FOOTER:
#ifdef OW_I18N
	if (frame->leftfooter == NULL)
		frame->leftfooter = wcstombsdup(frame->leftfooter_wcs);
#endif
	return (Xv_opaque) frame->leftfooter;

#ifdef OW_I18N
      case FRAME_LEFT_FOOTER_WCS:
	return (Xv_opaque) frame->leftfooter_wcs;
#endif

      case FRAME_RIGHT_FOOTER:
#ifdef OW_I18N
	if (frame->rightfooter == NULL)
		frame->rightfooter = wcstombsdup(frame->rightfooter_wcs);
#endif
	return (Xv_opaque) frame->rightfooter;

#ifdef OW_I18N
      case FRAME_RIGHT_FOOTER_WCS:
	return (Xv_opaque) frame->rightfooter_wcs;
#endif

      case FRAME_NO_CONFIRM:
	return (Xv_opaque) status_get(frame, no_confirm);

      case FRAME_NTH_SUBWINDOW:{
	    register Xv_Window sw;
	    register int    n = va_arg(valist, int);

	    FRAME_EACH_SUBWINDOW(frame, sw)
		if (--n == 0)
		return sw;
	    FRAME_END_EACH
		return NULL;
	}

      case FRAME_NTH_SUBFRAME:{
	    register Xv_Window sw;
	    register int    n = va_arg(valist, int);

	    FRAME_EACH_SUBFRAME(frame, sw)
		if (--n == 0)
		return sw;
	    FRAME_END_EACH
		return NULL;
	}

      case FRAME_CLOSED:
	/* If the frame is Withdrawn, return the inital_state the frame will
	 * be in when it is mapped.					     */
	if (xv_get(frame_public, XV_SHOW))
	    return (Xv_opaque) frame_is_iconic(frame);
	else
	    return (Xv_opaque) status_get(frame, initial_state);

      case FRAME_INHERIT_COLORS:
	return (Xv_opaque) xv_get(frame_public, WIN_INHERIT_COLORS);

      case FRAME_SUBWINDOWS_ADJUSTABLE:	/* WIN_BOUNDARY_MGR: */
	return (Xv_opaque) status_get(frame, bndrymgr);

      case WIN_TYPE:
	return (Xv_opaque) FRAME_TYPE;

      case FRAME_BUSY:
	return (Xv_opaque) status_get(frame, busy);

#ifdef OW_I18N
      case FRAME_RIGHT_IMSTATUS:
	if (frame->rightIMstatus == NULL)
		frame->rightIMstatus = wcstombsdup(frame->rightIMstatus_wcs);
	return (Xv_opaque) frame->rightIMstatus;

      case FRAME_LEFT_IMSTATUS:
	if (frame->leftIMstatus == NULL)
		frame->leftIMstatus = wcstombsdup(frame->leftIMstatus_wcs);
	return (Xv_opaque) frame->leftIMstatus;

      case FRAME_RIGHT_IMSTATUS_WCS:
	return (Xv_opaque) frame->rightIMstatus_wcs;

      case FRAME_LEFT_IMSTATUS_WCS:
	return (Xv_opaque) frame->leftIMstatus_wcs;
#endif

      default:
	if (xv_check_bad_attr(&xv_frame_class_pkg, attr) == XV_ERROR) {
	    *status = XV_ERROR;
	}
	return (Xv_opaque) 0;
    }
}


static int
frame_fit_direction(frame, direction)
    Frame_class_info *frame;
    Window_attribute direction;
{
    Frame           frame_public = FRAME_PUBLIC(frame);
    register Xv_Window sw;
    Rect            rect, rbound;
    register short *value = (direction == WIN_DESIRED_WIDTH) ?
    &rect.r_width : &rect.r_height;

    rbound = rect_null;
    FRAME_EACH_SHOWN_SUBWINDOW(frame, sw)
	(void)win_get_outer_rect(sw, &rect);
    *value += FRAME_BORDER_WIDTH;
    rbound = rect_bounding(&rbound, &rect);
    FRAME_END_EACH
	if (direction == WIN_DESIRED_WIDTH) {
	if (!rbound.r_width)
	    (void) win_getrect(frame_public, &rbound);
	else
	    rbound.r_width += FRAME_BORDER_WIDTH;
	return rbound.r_width;
    } else {
	if (!rbound.r_height)
	    (void) win_getrect(frame_public, &rbound);
	else
	    rbound.r_height += FRAME_BORDER_WIDTH;	/* frame_stripe_height(fr
							 * ame); */
	return rbound.r_height;
    }
}
