#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)screen.c 50.8 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/scrn_impl.h>
#include <xview/notify.h>
#include <xview/server.h>
#include <xview/window.h>
#include <xview/cms.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

Pkg_private int
screen_init(parent, screen_public, avlist)
    Xv_opaque       parent;
    Xv_Screen       screen_public;
    Attr_avlist     avlist;
{
    register Screen_info 	*screen;
    Xv_screen_struct 		*screen_object;
    register Attr_avlist 	attrs;
    Xv_object       		font;
    int             		font_id;
    Display        		*display;

    /* Allocate private data and set up forward/backward links. */
    screen = (Screen_info *) xv_alloc(Screen_info);
    screen->public_self = screen_public;
    screen_object = (Xv_screen_struct *) screen_public;
    screen_object->private_data = (Xv_opaque) screen;

    screen->server = parent ? parent : xv_default_server;
    display = (Display *) xv_get(screen->server, XV_DISPLAY);
    screen->number = DefaultScreen(display);

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch ((int) attrs[0]) {
	  case SCREEN_NUMBER:
	    screen->number = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  default:
	    break;
	}
    }

    screen_init_default_cms(screen_public);

    /*
     * By default, monochrome leaf windows (ie. windows into which bits are
     * actually written) are set to retained. This can be turned off by a
     * cmdline arg for debugging.
     */
    if (DisplayPlanes(display, screen->number) > 1) {
	screen->retain_windows = FALSE;
    } else {
	screen->retain_windows =
	    !(defaults_get_boolean("window.mono.disableRetained",
				   "Window.Mono.DisableRetained", FALSE));
    }

#ifndef OW_I18N
    /* set the default font in the GC for this screen */
    font = (Xv_object) xv_get(screen->server, SERVER_FONT_WITH_NAME, NULL, 0);
    if (!font) {
	xv_free(screen);
	return XV_ERROR;
    }
    font_id = (int) xv_get(font, XV_XID);
    xv_set_default_font(xv_get(screen->server, XV_DISPLAY),
			screen->number, font_id);
#endif /*OW_I18N*/

    screen->root_window =
	xv_create(screen_public, WINDOW,
		  WIN_IS_ROOT,
		  WIN_LAYOUT_PROC, screen_layout,
		  0);

    if (!screen->root_window) {
	xv_free(screen);
	return XV_ERROR;
    }
    /* now tell the server it has a new screen */
    xv_set(screen->server,
	   SERVER_NTH_SCREEN, screen->number, screen_public,
	   0);

    return XV_OK;
}


Pkg_private
screen_init_default_cms(screen_public)
    Xv_Screen       screen_public;
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);
    Display        *display = (Display *)xv_get(screen->server, XV_DISPLAY);
    XVisualInfo     visual_info;

    if (XMatchVisualInfo(display, screen->number,
                        XDisplayPlanes(display, screen->number),
                        StaticColor, &visual_info)) {
        screen->static_visual = visual_info.visual;
    }
 
    if (XMatchVisualInfo(display, screen->number,
                        XDisplayPlanes(display, screen->number),
                        PseudoColor, &visual_info)) {
        screen->dynamic_visual = visual_info.visual;
    }
       
    screen->default_cms = xv_create(screen_public, CMS,
                                CMS_DEFAULT_CMS, TRUE,
                                0);
}


Pkg_private int
screen_destroy(screen_public, status)
    Xv_Screen       screen_public;
    Destroy_status  status;
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);

    if (notify_post_destroy(screen->root_window, status, NOTIFY_IMMEDIATE) ==
	NOTIFY_DESTROY_VETOED)
	return XV_ERROR;

    if ((status == DESTROY_CHECKING) || (status == DESTROY_SAVE_YOURSELF))
	return XV_OK;

    /* now tell the server it has lost a screen */
    xv_set(screen->server, SERVER_NTH_SCREEN, screen->number, (Xv_Screen) 0, 0);

    free(screen);

    return XV_OK;
}

Pkg_private Xv_opaque
screen_set_avlist(screen_public, avlist)
    Xv_Screen       screen_public;
    Attr_attribute  avlist[];
{
    Screen_info		    *screen = SCREEN_PRIVATE(screen_public);
    register Attr_avlist    attrs;

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch (attrs[0]) {
	    case SCREEN_COLORMAP_LIST:
		screen->colormap_list = (Xv_opaque)(attrs[1]);
		break;

	    default:
		xv_check_bad_attr(&xv_screen_pkg, attrs[0]);
		break;
	}
    }

    return (XV_OK);
}


Xv_private void
screen_set_clip_rects(screen_public, xrect_array, rect_count)
    Xv_Screen       screen_public;
    XRectangle     *xrect_array;
    int             rect_count;
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);
    int             i;

    for (i = 0; i < rect_count; i++)
	screen->clip_xrects.rect_array[i] = xrect_array[i];
    screen->clip_xrects.count = rect_count;
}


Xv_private Xv_xrectlist *
screen_get_clip_rects(screen_public)
    Xv_Screen       screen_public;
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);

    return (&screen->clip_xrects);
}


Xv_private void
screen_set_cached_window_busy(screen_public, window, busy)
    Xv_Screen       screen_public;
    Xv_window       window;
    int		    busy;
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);
    Xv_cached_window *cached_window;

    for (cached_window = screen->cached_windows; cached_window != NULL;
	 cached_window = cached_window->next) {
	if (cached_window->window == window) {
	    cached_window->busy = busy;
	    break;
	}
    }
    if (cached_window == NULL) {
	xv_error(NULL,
		 ERROR_STRING, 
#ifdef OW_I18N
		     XV_I18N_MSG("xv_messages", "Unable to return window to screen cache"),
#else
		     "Unable to return window to screen cache", 
#endif
		 ERROR_PKG, SCREEN,
		 0);
    }
}

