#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)frame_base.c 50.3 90/11/10";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <X11/Xlib.h>
#include <xview_private/fm_impl.h>
#include <xview_private/frame_base.h>
#include <xview_private/draw_impl.h>
#include <xview/cursor.h>
#include <xview/server.h>
#include <xview_private/svr_atom.h>

#include <X11/Xatom.h>

#if defined(WITH_3X_LIBC) || defined(vax)
/* 3.x - 4.0 libc transition code; old (pre-4.0) code must define the symbol */
#define jcsetpgrp(p)  setpgrp((p),(p))
#endif

extern Attr_avlist attr_find();

Pkg_private int
frame_base_init(owner, frame_public, avlist)
    Xv_Window       owner;
    Frame           frame_public;
    Attr_attribute  avlist[];
{
    Xv_frame_base  *frame_object = (Xv_frame_base *) frame_public;
    Xv_Drawable_info *info;
    Xv_opaque       server_public;
    Frame_base_info *frame;
    Attr_avlist      attrs;
#ifndef OW_I18N
    int             dont_show_resize_corner = FALSE;
#endif

    DRAWABLE_INFO_MACRO(frame_public, info);
    server_public = xv_server(info);
    frame = xv_alloc(Frame_base_info);

    /* link to object */
    frame_object->private_data = (Xv_opaque) frame;
    frame->public_self = frame_public;

    /* set initial window decoration flags */

    frame->win_attr.win_type = (Atom) xv_get(server_public, SERVER_WM_WT_BASE);
    frame->win_attr.menu_type = (Atom) xv_get(server_public, SERVER_WM_MENU_FULL);
    frame->win_attr.pin_initial_state = (Atom) xv_get(server_public, SERVER_WM_NONE);

    status_set(frame, show_label, TRUE);
    status_set(frame, show_footer, FALSE);
    status_set(frame, props_active, FALSE);
                          /* Wmgr default to have resize corner for cmd frame */
    status_set(frame, show_resize_corner, TRUE);
#ifdef OW_I18N
    status_set(frame, show_resize_corner_init, FALSE);
    status_set(frame, show_IMstatus_init, FALSE);
    status_set(frame, show_IMstatus, FALSE);
#endif

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch (*attrs) {

	  case FRAME_SCALE_STATE:
	    /*
	     * change scale and find the apprioprate size of the font Don't
	     * call frame_rescale_subwindows because they have not created
	     * yet.
	     */
	    /*
	     * WAIT FOR NAYEEM window_set_rescale_state(frame_public,
	     * attrs[1]);
	     */
	    wmgr_set_rescale_state(frame_public, attrs[1]);
	    break;
#ifndef OW_I18N
	  case FRAME_SHOW_RESIZE_CORNER:
	    dont_show_resize_corner = !attrs[1];
	    break;
#endif
	  default:
	    break;
	}
    }

#ifndef OW_I18N
    if (dont_show_resize_corner)
	    (void)xv_set(frame_public, FRAME_SHOW_RESIZE_CORNER, FALSE, 0);
#endif

    return XV_OK;
}

Xv_private
frame_handle_props(frame_public)
    Frame           frame_public;
{
    Frame_base_info *frame = FRAME_BASE_PRIVATE(frame_public);

    if (frame->props_proc && status_get(frame, props_active)) {
	(frame->props_proc) (frame_public);
    }
}
