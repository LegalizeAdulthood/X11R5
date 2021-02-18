#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)window.c 50.13 90/12/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifdef _XV_DEBUG
#include <xview_private/xv_debug.h>
#endif

#ifdef SVR4
#include <sys/types.h>
#endif SVR4

#include <stdio.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <signal.h>
#ifdef	OW_I18N
#include <xview_private/xv_i18n_impl.h>
#include <widec.h>
#endif

#include <xview/notify.h>
#include <xview_private/windowimpl.h>
#include <xview/server.h>
#include <xview/screen.h>
#include <xview/window.h>
#include <xview/cms.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <xview_private/draw_impl.h>
#include <xview_private/win_keymap.h>
#include <xview_private/win_info.h>

#define WIN_NO_VISUAL_CLASS -1

extern Screen_visual *screen_add_visual();

/*
 * Private
 */

static int      parent_dying = 0;	/* Don't destroy this window if */
/* its parent window is dying.  */
/* Server will do this for us.  */
Pkg_private int window_init();
Pkg_private int window_destroy_win_struct();
Pkg_private XID window_new();

#ifdef	OW_I18N
#define eexit(msg, error_msg) \
  if (error_msg->text) { \
      wchar_t dummy[128]; \
      if( error_msg->encoding_is_wchar ) \
      	(void) wsprintf(dummy, "%ws\n%ws", msg, error_msg->text); \
      else \
	(void) wsprintf(dummy, "%ws\n%s", msg, error_msg->text); \
      xv_error(NULL, \
          ERROR_SEVERITY, ERROR_NON_RECOVERABLE, \
          ERROR_STRING, dummy, ERROR_PKG, WINDOW, 0); \
  } else { \
      xv_error(NULL, \
          ERROR_STRING, msg, ERROR_PKG, WINDOW, 0); \
      return XV_ERROR; \
  }
#else
#define eexit(msg, error_msg) \
  if (error_msg) { \
      char dummy[128]; \
      (void) sprintf(dummy, "%s\n%s", msg, error_msg); \
      xv_error(NULL, \
          ERROR_SEVERITY, ERROR_NON_RECOVERABLE, \
          ERROR_STRING, dummy, ERROR_PKG, WINDOW, 0); \
  } else { \
      xv_error(NULL, \
          ERROR_STRING, msg, ERROR_PKG, WINDOW, 0); \
      return XV_ERROR; \
  }
#endif


/*
 * Initialize win as a window.  Return XV_ERROR if error and no WIN_ERROR_MSG
 * is specified. Caller is required to pass a screen as the parent_public
 * when creating a root window.
 */
Pkg_private int
window_init(parent_public, win_public, avlist)
    Xv_Window       parent_public, win_public;
    Attr_avlist     avlist;
{
    Window_info    		*parent;
    Xv_window_struct 		*win_object = (Xv_window_struct *) win_public;
    register Window_info 	*win;
    register Attr_avlist	attrs;
#ifdef	OW_I18N
    struct  win_error_msg {
	int		encoding_is_wchar;
	wchar_t		*text;
    }	*error_msg,dumy;
#else
    char           		*error_msg = NULL;
#endif
    char           		*default_fontname;
    unsigned char		is_root = FALSE, is_sel_window = FALSE;
    unsigned char             	input_only = FALSE, transparent = FALSE;
    unsigned char 		is_client_pane = FALSE, use_default_cms = TRUE; 
    int		 		border = FALSE, no_decor = FALSE, convert_cu = FALSE;
    int				depth = 0, dynamic_color = FALSE, screen_num = 0;
    Display        		*display;
    Xv_opaque       		screen, server;
    int			        visual_class = WIN_NO_VISUAL_CLASS;
    Visual         		*visual;
    XVisualInfo			vinfo;
    Cms				default_cms;
    Colormap			cmap_id;
    Xv_Cursor       		default_cursor;
    register Xv_Drawable_info 	*info;
    register Xv_Drawable_info 	*parent_info = NULL;
    Xv_singlecolor              colors[2];
#ifdef	OW_I18N
    int		 		win_use_im = TRUE;
#endif

#ifdef	OW_I18N
    dumy.text = NULL;
    error_msg = &dumy;
#endif
    /* get the name and error message */
    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch ((int) attrs[0]) {
	  case WIN_IS_ROOT:	/* create a root window */
	    is_root = TRUE;
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_SELECTION_WINDOW:
	    is_sel_window = TRUE;
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_INPUT_ONLY:	/* create an input only window */
	    input_only = TRUE;
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_TRANSPARENT:/* create a "see through" window */
	    transparent = TRUE;
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_IS_CLIENT_PANE:
	    is_client_pane = TRUE;
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_DEPTH:
	    depth = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_BORDER:
	    border = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_TOP_LEVEL_NO_DECOR:
	    no_decor = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_DYNAMIC_VISUAL:
	    dynamic_color = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case XV_VISUAL_CLASS:
	    visual_class = (int)attrs[1];
	    /* sanity check */
	    if ((visual_class < StaticGray) || (visual_class > TrueColor))
	      visual_class = WIN_NO_VISUAL_CLASS;
	    ATTR_CONSUME(attrs[0]);
	    break;

#ifdef	OW_I18N
	  case WIN_ERROR_MSG:
	    error_msg->text = (wchar_t *) attrs[1];
	    error_msg->encoding_is_wchar = FALSE;
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_ERROR_MSG_WCS:
	    error_msg->text = (wchar_t *) attrs[1];
	    error_msg->encoding_is_wchar = TRUE;
	    ATTR_CONSUME(attrs[0]);
	    break;
#else
	  case WIN_ERROR_MSG:
	    error_msg = (char *) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;
#endif
#ifdef	OW_I18N
	  case WIN_USE_IM:
	    win_use_im = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

#endif  OW_I18N

	    /*
	     * for compatiblity we check these to see if a character unit
	     * macro (ATTR_ROW/COL) has been used.  If so, then we must
	     * replace all of the character units in the avlist before any of
	     * the set procs are called.
	     */
	  case WIN_WIDTH:
	  case WIN_HEIGHT:
	  case WIN_FIT_WIDTH:
	  case WIN_FIT_HEIGHT:
	  case WIN_X:
	  case WIN_Y:
	    convert_cu |= attr_is_cu((int) attrs[1]);
	    break;

	  case WIN_MOUSE_XY:
	    convert_cu |= attr_is_cu((int) attrs[1]) |
		attr_is_cu((int) attrs[2]);
	    break;

	  default:
	    break;
	}
    }
    win = xv_alloc(Window_info);

    if (is_root) {
	parent = 0;
	win->top_level = TRUE;
    } else {
	if (!parent_public) {
	    /* xv_create ensures that xv_default_screen is valid. */
	    parent_public = xv_get(xv_default_screen, XV_ROOT);
	    win->top_level = TRUE;
	} else if (xv_get(parent_public, WIN_IS_ROOT))
	    win->top_level = TRUE;
	else if (!xv_get(parent_public, XV_IS_SUBTYPE_OF, WINDOW)) {
	    xv_error(NULL,
        	     ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
		     /* @S */
       	     ERROR_STRING, 
#ifdef OW_I18N
			XV_I18N_MSG("xv_messages", "Subwindow owner is not a window\n"),
#else
			"Subwindow owner is not a window\n",
#endif
		     ERROR_PKG, WINDOW,
		     0);
	} else
	    win->top_level = FALSE;

	parent = WIN_PRIVATE(parent_public);
	if (!parent->layout_proc) {
	    /* @S */
#ifdef OW_I18N
	    eexit(XV_I18N_MSG("xv_messages", "Parent window does not support children"), error_msg);
#else
	    eexit("Parent window does not support children", error_msg);
#endif
	}
	DRAWABLE_INFO_MACRO(parent_public, parent_info);
    }

    if (no_decor && !win->top_level) {
	xv_error(win_public,
		 ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
		 ERROR_STRING,
		     /* @S */
#ifdef OW_I18N
		XV_I18N_MSG("xv_messages", "no decoration flag set for non-top level window"),
#else
		"no decoration flag set for non-top level window",
#endif
		 0);
    }

    screen = (is_root) ? parent_public : xv_screen(parent_info);
    screen_num = (int) xv_get(screen, SCREEN_NUMBER);
    server = xv_get(screen, SCREEN_SERVER);
    display = (Display *) xv_get(server, XV_DISPLAY);
    DRAWABLE_INFO_MACRO(win_public, info);

#ifdef OW_I18N
    /* if input lang is C, cannot have an IM */
    if (strcmp(xv_get(server, XV_LC_INPUT_LANG),"C")==0)
        win_use_im = FALSE;
    win->win_use_im = win_use_im;
#endif OW_I18N

    win_object->private_data = (Xv_opaque) win;
    win->public_self = win_public;
    win->top_level_no_decor = no_decor;
    win->created = FALSE;
    win->has_border = border;
    win->input_only = input_only;
    win->transparent = transparent;
    win->is_client_pane = is_client_pane;
    win->in_fullscreen_mode = FALSE;
    win->being_rescaled = FALSE;
    win->scale = (int) WIN_SCALE_MEDIUM;
    win->map = TRUE;
    win->win_front = FALSE;
    win->layout_proc = window_layout;
    win->xmask = ExposureMask;
    win->collapse_exposures = TRUE;
    win->background_pixmap = (Pixmap) NULL;
    win->deaf = FALSE;
    win->normal_cursor = (Xv_object)NULL;
    win->owner = parent;
    /* caching the parent for performance reasons */
    win->parent = parent_public;
    if (!is_root) {
	win->cache_rect.r_left = win->cache_rect.r_top = DEFAULT_X_Y;
	win->cache_rect.r_width = win->cache_rect.r_height = DEFAULT_WIDTH_HEIGHT;
    }

    if (!is_root && !is_sel_window) {
        default_cursor = (Xv_Cursor) xv_get(screen, XV_KEY_DATA, WIN_CURSOR);
        if (!default_cursor) {
	    default_cursor = xv_create(screen, CURSOR,
				   CURSOR_SRC_CHAR, OLC_BASIC_PTR,
				   CURSOR_MASK_CHAR, OLC_BASIC_MASK_PTR,
				   /* Can never free */
				   XV_INCREMENT_REF_COUNT,	
				   0);
	    (void) xv_set(screen, XV_KEY_DATA, CURSOR_BASIC_PTR, default_cursor, 0);
	    (void) xv_set(screen, XV_KEY_DATA, WIN_CURSOR, default_cursor, 0);
        } 
        (void) xv_set(default_cursor, XV_INCREMENT_REF_COUNT, 0);
        win->cursor = (Xv_opaque) default_cursor;
    } else {
	win->cursor = (Xv_opaque)NULL;
    }

    if (depth == 0) {
	depth = (is_root) ? DefaultDepth(display, screen_num)
		    : xv_depth(parent_info);
    }


    default_cms = (Cms)xv_get(screen, SCREEN_DEFAULT_CMS);
    
    /* Set up the visual and cms used to create the window */

    if (dynamic_color) 
      visual_class = PseudoColor;
    if ((visual_class != WIN_NO_VISUAL_CLASS) &&
	(XMatchVisualInfo(display, screen_num, depth, visual_class, &vinfo))) {
	visual = vinfo.visual;
	/* All static visuals are even, dynamic visuals are odd */
	xv_dynamic_color(info) = (visual_class % 2);
	if (visual == DefaultVisual(display, screen_num)) {
	    xv_cms(info) = default_cms;
	} else {
	    char cms_name[10];
	    
	    sprintf(cms_name, "xv_cms_%d", visual_class);
	    colors[0].red = colors[0].green = colors[0].blue = 255;
	    colors[1].red = colors[0].green = colors[0].blue = 0;
	    xv_cms(info) = xv_find(xv_screen(parent_info), CMS,
				   CMS_NAME, cms_name,
				   XV_VISUAL_CLASS, visual_class,
				   CMS_TYPE, 
				   dynamic_color ? XV_DYNAMIC_CMS : XV_STATIC_CMS,
				   CMS_COLORS, colors,
				   CMS_FRAME_CMS, TRUE,
				   0);
	}
    } else if (!(win->top_level) && (parent->inherit_colors)) {
	win->inherit_colors = TRUE;
	/* BUG: this information should be in the drawable info */
	visual_class = (int)xv_get(parent_public, XV_VISUAL_CLASS, 0);
	XMatchVisualInfo(display, screen_num, depth, visual_class, &vinfo);
	visual = vinfo.visual;
	xv_dynamic_color(info) = xv_dynamic_color(parent_info);
	xv_cms(info) = xv_cms(parent_info);
    } else {
	visual = DefaultVisual(display, screen_num);
	visual_class = visual->class;
	/* All static visuals are even, dynamic visuals are odd */
	xv_dynamic_color(info) = (visual_class % 2);
	xv_cms(info) = default_cms;
    }
    cmap_id = (Colormap)xv_get(xv_cms(info), CMS_CMAP_ID, 0);
    /*
     * set up background and foreground information.  Can't use
     * window_set_cms, because it would try to set the property,
     * and we don't know if this is a frame window at this level of the
     * creation of the window.
     */  
    if (!(win->top_level) && (parent->inherit_colors)) {
        xv_cms_bg(info) = xv_cms_bg(parent_info);
        xv_cms_fg(info) = xv_cms_fg(parent_info);
        xv_fg(info) = xv_fg(parent_info);
        xv_bg(info) = xv_bg(parent_info);
    } else {
        xv_cms_bg(info) = 0;
        xv_cms_fg(info) = (int)xv_get(xv_cms(info), CMS_SIZE, 0) - 1;
        xv_fg(info) = (int)xv_get(xv_cms(info), CMS_FOREGROUND_PIXEL, 0);
        xv_bg(info) = (int)xv_get(xv_cms(info), CMS_BACKGROUND_PIXEL, 0);
    }

    info->xid = window_new(display, screen, visual, cmap_id, depth,
			   parent_info, win);
    
    win->visual_class = visual->class;
    
    /* Default plane mask is all planes. */
    info->plane_mask = ~0;
    
    /* this is a window, not a bitmap */
    info->is_bitmap = 0;

    info->visual = screen_add_visual(screen, info->xid, depth);
    if (!info->visual) {
	/* @S */
#ifdef OW_I18N
	eexit(XV_I18N_MSG("xv_messages", "Window creation failed to get new visual"), error_msg);
#else
	eexit("Window creation failed to get new visual", error_msg);
#endif
    }
    if (is_root) {
	/* Patch up the visual's root_window. */
	info->visual->root_window = win_public;
    }
    info->private_gc = FALSE;

    /* BUG:  What the hell is this? */
    if (is_client_pane) {
	default_fontname = (char *) getenv("XV_CLIENT_PANE_FONT");
    } else {
	default_fontname = NULL;
    }
    win->font = (Pixfont *) xv_get(server,
				   SERVER_FONT_WITH_NAME, default_fontname);
    (void) xv_set(win->font, XV_INCREMENT_REF_COUNT, 0);

    notify_set_destroy_func(win_public, xv_destroy_status);

    /* register with the notifier */
    if (notify_set_event_func(win_public, window_default_event_func,
			      NOTIFY_SAFE) ==
	NOTIFY_FUNC_NULL) {
	/* @S */
#ifdef OW_I18N
	eexit(XV_I18N_MSG("xv_messages", "notify_set_event_func failed in window creation"), error_msg);
#else
	eexit("notify_set_event_func failed in window creation", error_msg);
#endif
	/*
	 * BUG: do we need this here? (void)notify_remove(win_public);
	 */
    }
    if (notify_set_event_func(win_public, window_default_event_func,
			      NOTIFY_IMMEDIATE) ==
	NOTIFY_FUNC_NULL) {
	/* @S */
#ifdef OW_I18N
	eexit(XV_I18N_MSG("xv_messages", "notify_set_event_func failed in window creation"), error_msg);
#else
	eexit("notify_set_event_func failed in window creation", error_msg);
#endif
    }

    /* for compatibility */
    if (convert_cu)
	window_scan_and_convert_to_pixels(win_public, avlist);

    return XV_OK;
}


Pkg_private XID
window_new(display, screen, visual, cmap_id, depth, parent_info, win)
    Display        	*display;
    Xv_opaque       	screen;
    Visual         	*visual;
    int                 cmap_id;
    int             	depth;
    Xv_Drawable_info 	*parent_info;
    Window_info 	*win;
{
    Window                  new_window;
    XSetWindowAttributes    attrs;
    Rect            	    rect;
    unsigned long   	    value_mask = 0;     
    Xv_object       	    win_public = win->public_self;
    Xv_Drawable_info        *info;

    DRAWABLE_INFO_MACRO(win_public, info);

    if (!parent_info) {
        new_window = RootWindow(display, (int) xv_get(screen, SCREEN_NUMBER));
        win_x_getrect(display, new_window, &rect);
        window_update_cache_rect(win_public, &rect);
    } else {
        attrs.event_mask = win->xmask;
        value_mask = CWEventMask;
        if (!win->input_only) {
            /* BitGravity/background/Cmap are invalid for InputOnly windows */
            attrs.bit_gravity = NorthWestGravity;
            value_mask |= CWBitGravity;
            attrs.colormap = (cmap_id) ? cmap_id : CopyFromParent;
            value_mask |= CWColormap;
            if (!win->transparent) {
                attrs.background_pixel = xv_bg(info);
                value_mask |= CWBackPixel;
                attrs.border_pixel = xv_fg(info);
                value_mask |= CWBorderPixel;
            }
        }
        if (win->top_level_no_decor) {
            attrs.override_redirect = True;
            value_mask |= CWOverrideRedirect;
        }
        if (win->transparent) {
            value_mask |= CWBackPixmap;
            attrs.background_pixmap = None;
        }
	if (win->cursor) {
	    value_mask |= CWCursor;
	    attrs.cursor = (Cursor)xv_get(win->cursor, XV_XID);
	}
        new_window = XCreateWindow(display,
		        xv_xid(parent_info),
		        win->cache_rect.r_left,
		        win->cache_rect.r_top,
		        win->cache_rect.r_width,
		        win->cache_rect.r_height,
		        win->has_border ? WIN_DEFAULT_BORDER_WIDTH : 0,
		        win->input_only ? 0 : depth,
		        win->input_only ? InputOnly : CopyFromParent,
		        visual ? visual : CopyFromParent,
		        value_mask,
		        &attrs);

    }
    XSaveContext(display, new_window, CONTEXT, win_public);
    return (new_window);
}

Xv_private void
window_set_bit_gravity(win_public, value)
    Xv_Window       win_public;
    int             value;
{
    register Xv_Drawable_info *info;
    XSetWindowAttributes win_attr;

    win_attr.bit_gravity = value;
    DRAWABLE_INFO_MACRO(win_public, info);
    XChangeWindowAttributes(xv_display(info), xv_xid(info), CWBitGravity, &win_attr);

}

void
xv_main_loop(win_public)
    Xv_Window       win_public;
{
    Window_info    *win = WIN_PRIVATE(win_public);
    register Xv_Drawable_info *info;
    Display        *display;
    extern int      sview_gprof_start;

    DRAWABLE_INFO_MACRO(win_public, info);
    display = xv_display(info);
    /*
     * sync with the server to make sure we have all outstanding
     * ConfigureNotify events in the queue. Then process the events, then
     * finally map the frame.
     */
    xv_set(xv_server(info), SERVER_SYNC_AND_PROCESS_EVENTS, 0);


    /* install win in tree */
    (void) win_insert(win_public);

    win->map = TRUE;
    if (xv_get(xv_server(info), SERVER_JOURNALLING)) {	/* server is in
							 * journalling mode */
	xv_set(xv_server(info), SERVER_JOURNAL_SYNC_EVENT, 1, 0);
    }
#ifdef GPROF
    {
	int             pend;
	if (sview_gprof_start) {
	    XSync(display, 0);
	    do {
		xv_scheduler_internal();	/* process pending enqueued
						 * events */
		XSync(display, 0);

	    } while (ioctl(display->fd, FIONREAD, (char *) &pend) > sizeof(XEvent));

	    moncontrol(0);
	}
    }
#endif
    XFlush(display);		/* flush anthing left in the buffer */
    (void) notify_start();	/* and then loop in the notifier */
    if (xv_default_server && xv_get(xv_default_server, SERVER_JOURNALLING))
	xv_set(xv_default_server, SERVER_JOURNAL_SYNC_EVENT, 1, 0);
}

int				/* bool */
window_done(win)
    Xv_Window       win;
{
    register Xv_Window owner = win;
    register Xv_Window grand_owner;

    /*
     * find the frame for this window. Chase up the owner list until we hit
     * the root window.
     */
    while ((grand_owner = window_get(owner, WIN_OWNER))) {
	/*
	 * remember the current window as the child of the next owner.
	 */
	win = owner;
	owner = grand_owner;
    }
    /*
     * now grand_owner is NULL, owner is a root window, and win is the frame
     * to destroy.
     */
    xv_destroy(win);
    return TRUE;
}

/* A flag to tell us if the parent window is going to be destoryed. */
Pkg_private int
window_get_parent_dying()
{
    return (parent_dying);
}

Pkg_private int
window_set_parent_dying()
{
    parent_dying = TRUE;
}

Pkg_private int
window_unset_parent_dying()
{
    parent_dying = FALSE;
}

Pkg_private int
window_destroy_win_struct(win_public, status)
    register Xv_Window win_public;
    Destroy_status  status;
{
    register Window_info *win = WIN_PRIVATE(win_public);
    register Xv_Drawable_info *info;

    switch (status) {
      case DESTROY_PROCESS_DEATH:
      case DESTROY_CLEANUP:
	/* Decrement the ref count on all ref counted objects */
	if (win->cursor)
	    (void) xv_set(win->cursor, XV_DECREMENT_REF_COUNT, 0);
	if (win->menu)
	    (void) xv_set(win->menu, XV_DECREMENT_REF_COUNT, 0);
	if (win->font)
	    (void) xv_set(win->font, XV_DECREMENT_REF_COUNT, 0);

	DRAWABLE_INFO_MACRO(win_public, info);
	/*
	 * Remove conditions from notifier.  Could remove just conditions set
	 * in window_init(), but we do everyone a favor here because it is
	 * overwhelmingly the common case.
	 */
	if (status == DESTROY_CLEANUP)
	    (void) notify_remove(win_public);
	if (win->owner && win->owner->layout_proc)
	    (win->owner->layout_proc) (WIN_PUBLIC(win->owner), win_public,
				       WIN_DESTROY);
	/* If this is DESTROY_CLEANUP and the parent window is not going to */
	/* be destroyed, destroy the window.				    */
	if ((status == DESTROY_CLEANUP) && (!window_get_parent_dying())) {
	    win_free(win_public);
	    XFlush(xv_display(info));
	    free(win);
	} else
	    XDeleteContext(xv_display(info), xv_xid(info), 1);
	break;
      default:
	break;
    }
    return XV_OK;
}
