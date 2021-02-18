#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)frame.c 50.2 90/10/16";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <xview_private/fm_impl.h>
#include <xview_private/frame_cmd.h>
#include <xview_private/frame_base.h>
#include <xview/server.h>
#include <xview_private/draw_impl.h>

Xv_private void
frame_cmdline_help(name)
    char           *name;
{
    xv_usage(name);
}

static int
check_for_configure_event(display, xevent, args)
    Xv_object		display;
    XEvent		*xevent;
    char		*args;
{
    return ((xevent->type & 0177) == ConfigureNotify);
}


extern void
frame_get_rect(frame, rect)
    Frame           frame;
    Rect           *rect;
{
    
    XWindowAttributes	xwin_attr;
    Xv_Drawable_info	*info;      
    XID   		win_xid;
    register Xv_object	display;
    int			top, bottom, left, right;
    int			has_footer = FALSE;
#ifdef OW_I18N
    int			has_IMstatus = FALSE;
#endif

    
    if (xv_get(frame, WIN_TYPE) != (Xv_opaque)FRAME_TYPE) {
	bzero(rect, sizeof(Rect));
        return;
    }
        
    DRAWABLE_INFO_MACRO(frame, info);
    display = (Xv_object)xv_display(info);
    win_xid = xv_xid(info);

    if ((int) xv_get(frame, XV_IS_SUBTYPE_OF, FRAME_BASE)) {
        Frame_base_info	*frame_private = FRAME_BASE_PRIVATE(frame);
        has_footer = status_get(frame_private, show_footer);        
#ifdef OW_I18N
	has_IMstatus = status_get(frame_private, show_IMstatus);
#endif
    } else if ((int) xv_get(frame, XV_IS_SUBTYPE_OF, FRAME_CMD)) {
        Frame_cmd_info	*frame_private = FRAME_CMD_PRIVATE(frame);
        has_footer = status_get(frame_private, show_footer);
#ifdef OW_I18N
	has_IMstatus = status_get(frame_private, show_IMstatus);
#endif
    }
        
    /* Currently the ICCCM does not provide any means for an application to 
     * determine the size of the window manager decor window.  This makes
     * determining the current position and size of the overall application
     * very difficult.  For now we assume we are running under an OPEN LOOK
     * window manager and hope we get resolution from the ICCCM soon. :-(
     */
    top = 26;
    bottom = has_footer ? 23 : 5;
#ifdef OW_I18N
    bottom += (has_IMstatus ? 18 : 0);
#endif
    left = right = 5;
        
    XGetWindowAttributes(display,  win_xid, &xwin_attr);
    
    if (xwin_attr.map_state == IsViewable) {
        int		x, y;
        XID		dummy;
        XTranslateCoordinates(display, win_xid, 
        		      (XID) xv_get(xv_root(info), XV_XID), 
        		      0, 0, &x, &y, &dummy);
        xwin_attr.x = x - left;
        xwin_attr.y = y - top;
    }
    			  
    rect->r_left = xwin_attr.x;
    rect->r_top = xwin_attr.y;
    rect->r_width = xwin_attr.width + left + right;
    rect->r_height = xwin_attr.height + top + bottom;  
}

extern void
frame_set_rect(frame, rect)
    Frame           frame;
    Rect           *rect;
{
    
    Xv_Drawable_info	*info;
    XID   		win_xid;
    register Xv_object	display; 
    int			top, bottom, left, right;
    int			has_footer = FALSE;
#ifdef OW_I18N
    int			has_IMstatus = FALSE;
#endif
    XSizeHints		normal_hints;

    if (xv_get(frame, WIN_TYPE) != (Xv_opaque)FRAME_TYPE)
        return;
        
    DRAWABLE_INFO_MACRO(frame, info);
    display = (Xv_object)xv_display(info); 
    win_xid = xv_xid(info);
    
    if ((int) xv_get(frame, XV_IS_SUBTYPE_OF, FRAME_BASE)) {
            Frame_base_info	*frame_private = FRAME_BASE_PRIVATE(frame);
            has_footer = status_get(frame_private, show_footer);        
#ifdef OW_I18N
            has_IMstatus = status_get(frame_private, show_IMstatus);
#endif
    } else if ((int) xv_get(frame, XV_IS_SUBTYPE_OF, FRAME_CMD)) {
            Frame_cmd_info	*frame_private = FRAME_CMD_PRIVATE(frame);
            has_footer = status_get(frame_private, show_footer);
#ifdef OW_I18N
            has_IMstatus = status_get(frame_private, show_IMstatus);
#endif
    }
    
    /* Currently the ICCCM does not provide any means for an application to 
     * determine the size of the window manager decor window.  This makes
     * determining the current position and size of the overall application
     * very difficult.  For now we assume we are running under an OPEN LOOK
     * window manager and hope we get resolution from the ICCCM soon. :-(
     */
    top = 26;
    bottom = has_footer ? 23 : 5;
#ifdef OW_I18N
    bottom += (has_IMstatus ? 18 : 0);
#endif
    left = right = 5;

    /* Get the old hints first. */
    XGetNormalHints(display, win_xid, &normal_hints);

    normal_hints.flags |= PSize | PPosition;

    XSetWMNormalHints(display, xv_xid(info), &normal_hints);

    /*
     *  This is setting the frame x, y relative to the root,
     *  and the width and height adjust for the outer rect.
     *  This will result in setting the window decor with the
     *  correct rect. 
     */
    XMoveResizeWindow(xv_display(info),  xv_xid(info), 
      rect->r_left,
      rect->r_top,
      rect->r_width - (left + right), 
      rect->r_height - (top + bottom));

    XFlush(display);

}
