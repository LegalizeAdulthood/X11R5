#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)wmgr_decor.c 1.19 90/02/26";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Window mgr decoration handling. handling.
 */

#include <X11/Xlib.h>
#include <xview_private/wmgr_decor.h>
#include <xview/server.h>
#include <xview_private/fm_impl.h>
#include <xview_private/draw_impl.h>

#include <X11/Xatom.h>


wmgr_add_decor(frame_public, decor_list, num_of_decor)
    Frame           frame_public;
    Atom           *decor_list;
    int             num_of_decor;
{
    Xv_Drawable_info *info;
    Xv_opaque       server_public;
    int             status;

    DRAWABLE_INFO_MACRO(frame_public, info);
    server_public = xv_server(info);
    status = XChangeProperty(xv_display(info), xv_xid(info),
			     xv_get(server_public, SERVER_WM_ADD_DECOR),
			     XA_ATOM, 32,
			     PropModeReplace, decor_list, num_of_decor);
    if (status)
	return XV_ERROR;
    else
	return XV_OK;
}

wmgr_delete_decor(frame_public, decor_list, num_of_decor)
    Frame           frame_public;
    Atom           *decor_list;
    int             num_of_decor;
{
    Xv_Drawable_info *info;
    Xv_opaque       server_public;
    int             status;

    DRAWABLE_INFO_MACRO(frame_public, info);
    server_public = xv_server(info);
    status = XChangeProperty(xv_display(info), xv_xid(info),
			     xv_get(server_public, SERVER_WM_DELETE_DECOR),
			     XA_ATOM, 32,
			     PropModeReplace, decor_list, num_of_decor);
    if (status)
	return XV_ERROR;
    else
	return XV_OK;
}



wmgr_set_win_attr(frame_public, win_attr)
    Frame           frame_public;
    WM_Win_Type    *win_attr;
{
    Xv_Drawable_info *info;
    Xv_opaque       server_public;
    int             status;
    Atom            atom = 0;

    DRAWABLE_INFO_MACRO(frame_public, info);
    server_public = xv_server(info);
    atom = (Atom) xv_get(server_public, SERVER_WM_WIN_ATTR);
    status = XChangeProperty(xv_display(info), xv_xid(info),
			     atom, atom, 32,
			     PropModeReplace, win_attr, 3);
    if (status)
	return XV_ERROR;
    else
	return XV_OK;
}

void
wmgr_set_rescale_state(frame_public, state)
    Frame           frame_public;
    int             state;
{

    Xv_Drawable_info *info;
    Xv_opaque       server_public;

    DRAWABLE_INFO_MACRO(frame_public, info);
    xv_server(info);
    XChangeProperty(xv_display(info), xv_xid(info),
		    xv_get(server_public, SERVER_WM_RESCALE_STATE),
		    XA_INTEGER, 32,
		    PropModeReplace, &state, 1);
}
