#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)frame_init.c 50.5 90/10/16";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <xview_private/draw_impl.h>
#include <xview/cursor.h>
#include <xview_private/fm_impl.h>
#include <xview/server.h>
#include <xview_private/svr_atom.h>
#include <xview/defaults.h>

extern Attr_avlist attr_find();

/*
 * This counter is incremented each time a frame is created and decremented
 * each time a frame is destroyed.  When the value reaches zero, the notifier
 * is stopped, see frame_free().
 */
/* Pkg_private */
int             frame_notify_count = 0;


Pkg_private void
frame_default_done_func(frame)
    Frame           frame;
{
    (void) window_set(frame, XV_SHOW, FALSE, 0);
}

/*
 * Convert geometry flags into a gravity that can be used for
 * setting the window hints.
 */
Pkg_private int
frame_gravity_from_flags(flags)
int flags;
{
    switch (flags & (XNegative | YNegative)) {
      case 0:
	return NorthWestGravity;
      case XNegative:
	return NorthEastGravity;
      case YNegative:
	return SouthWestGravity;
      default:
	return SouthEastGravity;
    }
}

Pkg_private int
frame_init(owner, frame_public, avlist)
    Xv_Window       owner;
    Frame           frame_public;
    Attr_avlist	    avlist;
{
    Xv_frame_class 	*frame_object = (Xv_frame_class *) frame_public;
    Frame_class_info 	*frame;
    int                  is_subframe = owner && xv_get(owner, XV_IS_SUBTYPE_OF,
								   FRAME_CLASS);
    Attr_avlist	    	 attrs;
    Xv_Drawable_info 	*info;
    Atom            	 property_array[3];
    XSizeHints		 normal_hints;

    DRAWABLE_INFO_MACRO(frame_public, info);

    frame = xv_alloc(Frame_class_info);

    /* link to object */
    frame_object->private_data = (Xv_opaque) frame;
    frame->public_self = frame_public;

    /* Tell window manager to leave our input focus alone */
    frame->wmhints.input = FALSE;
    frame->wmhints.flags |= InputHint;
    normal_hints.flags = 0;

    property_array[0] = (Atom) xv_get(xv_server(info), SERVER_WM_TAKE_FOCUS);
    property_array[1] = (Atom) xv_get(xv_server(info), SERVER_WM_DELETE_WINDOW);
    property_array[2] = (Atom) xv_get(xv_server(info), SERVER_WM_SAVE_YOURSELF);

    win_change_property(frame_public, SERVER_WM_PROTOCOLS, XA_ATOM, 32, 
							     property_array, 3);

    /* set default frame flags */
    if (is_subframe) {
	Xv_Drawable_info	*owner_info;
	DRAWABLE_INFO_MACRO(owner, owner_info);
	frame->wmhints.window_group = xv_xid(owner_info);
	normal_hints.flags = PPosition | PSize;
    } else {
	/* Must parse size and position defaults here (instead of in fm_cmdline.c)
	 * to get the sizing hints set correctly
	 */
	frame->geometry_flags = 0;
	frame->user_rect.r_left = 0;
	frame->user_rect.r_top = 0;
	frame->user_rect.r_width = 1;
	frame->user_rect.r_height = 1;
	frame->wmhints.window_group = xv_xid(info);

	if (defaults_exists("icon.x", "Icon.X")) {
	    frame->wmhints.icon_x = defaults_get_integer("icon.x", "Icon.X", 0);
	    frame->wmhints.flags |= IconPositionHint;
	}
	if (defaults_exists("icon.y", "Icon.Y")) {
	    frame->wmhints.icon_y = defaults_get_integer("icon.y", "Icon.Y", 0);
	    frame->wmhints.flags |= IconPositionHint;
	}

	if (defaults_exists("window.x", "Window.X")) {
	    frame->user_rect.r_left = defaults_get_integer("window.x", "Window.X", 0);
	    frame->geometry_flags |= XValue;
	}
	if (defaults_exists("window.y", "Window.Y")) {
	    frame->user_rect.r_top = defaults_get_integer("window.y", "Window.Y", 0);
	    frame->geometry_flags |= YValue;
	}
	if (defaults_exists("window.width", "Window.Width")) {
	    frame->user_rect.r_width = 
	      defaults_get_integer("window.width", "Window.Width", 1);
	    frame->geometry_flags |= WidthValue;
	}
	if (defaults_exists("window.height", "Window.Height")) {
	    frame->user_rect.r_height = 
	      defaults_get_integer("window.height", "Window.Height", 1);
	    frame->geometry_flags |= HeightValue;
	}

	if (defaults_exists("window.geometry", "Window.Geometry")) {
	    int x, y, flags;
	    unsigned int width, height;
	    char *geometry;
	    
	    geometry = defaults_get_string("window.geometry", "Window.Geometry",
					   (char *) NULL);

	    flags = XParseGeometry(geometry, &x, &y, &width, &height);
	    if (flags & WidthValue)
	      frame->user_rect.r_width = width;
	    if (flags & HeightValue)
	      frame->user_rect.r_height = height;
	    if (flags & XValue)
	      frame->user_rect.r_left = x;
	    if (flags & YValue)
	      frame->user_rect.r_top = y;
	    frame->geometry_flags |= flags;
	}

	/* Adjust normal_hints to be in sync with geometry_flags */
	if (frame->geometry_flags & (XValue | YValue))
	  normal_hints.flags |= USPosition;
	if (frame->geometry_flags & (WidthValue | HeightValue))
	  normal_hints.flags |= USSize;

#ifndef PRE_R4_ICCCM
	/* set the bit gravity of the hints */
	if (normal_hints.flags & USPosition) {
	    normal_hints.win_gravity = frame_gravity_from_flags(frame->geometry_flags);
	    normal_hints.flags |= PWinGravity;
	}
#endif PRE_R4_ICCCM
    }
    
    frame->wmhints.flags |= WindowGroupHint;
    frame->default_done_proc = frame_default_done_func;
    
    /*
     * OPEN LOOK change: no cofirmer by default
     */
    status_set(frame, no_confirm, (int) TRUE);

    if (defaults_exists("window.iconic", "Window.Iconic")) {
	int iconic = defaults_get_boolean("window.iconic", "Window.Iconic", FALSE);
	
	status_set(frame, iconic, iconic);
	status_set(frame, initial_state, iconic);
	if (iconic) {
	    frame->wmhints.initial_state = IconicState;
	    frame->wmhints.flags |= StateHint;
	}
    }

    /*
     * Iconic state and name stripe have to be determined before any size
     * parameters are interpreted, so the attribute list is mashed and
     * explicitly interrogated for them here.
     */
    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch ((int) *attrs) {

	  case FRAME_CLOSED:
	    status_set(frame, iconic, (int) attrs[1]);
	    status_set(frame, initial_state, (int) attrs[1]);
	    frame->wmhints.initial_state = ((int) attrs[1] ?
						   IconicState : NormalState);
	    frame->wmhints.flags |= StateHint;
	    ATTR_CONSUME(*attrs);
	    break;
	  case XV_X:
	  case XV_Y:
	  case XV_RECT:
	    normal_hints.flags |= PPosition;
	    break;
	  default:
	    break;
	}
    }

#ifdef	OW_I18N
    /* default settting for canvas input_window */
    frame->input_window = NULL;
#endif	OW_I18N

    (void) xv_set(frame_public,
		  XV_SHOW, FALSE,
		  WIN_INHERIT_COLORS, TRUE,
		  WIN_LAYOUT_PROC, frame_layout,
		  WIN_NOTIFY_SAFE_EVENT_PROC, frame_input,
		  WIN_NOTIFY_IMMEDIATE_EVENT_PROC, frame_input,
		  WIN_TOP_MARGIN, FRAME_BORDER_WIDTH,
		  WIN_BOTTOM_MARGIN, FRAME_BORDER_WIDTH,
		  WIN_LEFT_MARGIN, FRAME_BORDER_WIDTH,
		  WIN_RIGHT_MARGIN, FRAME_BORDER_WIDTH,
		  WIN_ROWS, 34,
		  WIN_COLUMNS, 80,

    /*
     * clear rect info since we are the ones setting (rows, columns). this
     * will let us check later (frame_set_avlist()) if the client has set the
     * position or size.
     */

		  WIN_RECT_INFO, 0,

		  WIN_CONSUME_EVENTS,
		  WIN_MAP_NOTIFY, WIN_UNMAP_NOTIFY,
		  0,
		  WIN_IGNORE_EVENT, WIN_REPAINT,
		  0);

    /* Set cached version of rect */
    (void) win_getsize(frame_public, &frame->rectcache);

    if (!is_subframe) {
	int             frame_count;

	frame_count = (int) xv_get(xv_server(info), XV_KEY_DATA, FRAME_COUNT);
	xv_set(xv_server(info), XV_KEY_DATA, FRAME_COUNT, ++frame_count, 0);
    }

    /* Initialise default foreground and background */
    frame->fg.red = frame->fg.green = frame->fg.blue = 0;
    frame->bg.red = frame->bg.green = frame->bg.blue = 255;

    /* created another frame */
    frame_notify_count++;

    XSetWMHints(xv_display(info), xv_xid(info), &(frame->wmhints));

    /* Use old XSetNormalHints function for non-ICCCM wm's */
    if (!defaults_get_boolean("xview.icccmcompliant",
			      "XView.ICCCMCompliant", TRUE))
    	XSetNormalHints(xv_display(info), xv_xid(info), &normal_hints);
    else
#ifndef PRE_R4_ICCCM
    	XSetWMNormalHints(xv_display(info), xv_xid(info), &normal_hints);
#else   PRE_R4_ICCCM
    	XSetNormalHints(xv_display(info), xv_xid(info), &normal_hints);
#endif  PRE_R4_ICCCM

    return XV_OK;
}

