#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)scrn_get.c 50.2 90/08/07";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/scrn_impl.h>
#include <xview/notify.h>
#include <xview/win_input.h>
#include <xview/win_screen.h>
#include <xview/base.h>

Xv_private Xv_Window screen_get_cached_window();

/* Caller turns varargs into va_list that has already been va_start'd */
/*ARGSUSED*/
Pkg_private     Xv_opaque
screen_get_attr(screen_public, status, attr, args)
    Xv_Screen       screen_public;
    int            *status;	/* initialized by caller */
    Attr_attribute  attr;
    va_list         args;
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);

    switch (attr) {
      case SCREEN_RETAIN_WINDOWS:
	return ((Xv_opaque) screen->retain_windows);

      case SCREEN_NUMBER:
	return (Xv_opaque) screen->number;

      case SCREEN_SERVER:
	return screen->server;

      case XV_ROOT:
	return screen->root_window;

      case SCREEN_DEFAULT_CMS:
	return (Xv_opaque) (screen->default_cms);

      case SCREEN_DEFAULT_COLORMAP:
	return (Xv_opaque)(xv_get(screen->default_cms, CMS_CMAP_ID));

      case SCREEN_STATIC_VISUAL:
	return (Xv_opaque) (screen->static_visual);

      case SCREEN_DYNAMIC_VISUAL:
	return (Xv_opaque) (screen->dynamic_visual);

      case SCREEN_COLORMAP_LIST:
	return (Xv_opaque) (screen->colormap_list);

      default:
	if (xv_check_bad_attr(&xv_screen_pkg, attr) == XV_ERROR) {
	    *status = XV_ERROR;
	}
	return ((Xv_opaque) 0);
    }
}

/*
 * Windows are cached in the screen and shared between menus.
 */
Xv_private Xv_Window
screen_get_cached_window(screen_public, event_proc, borders, visual_class,
			 new_window)
    Xv_Screen       screen_public;
    Notify_func	    event_proc;
    int             borders;
    int             visual_class;
    int		   *new_window;	/* output parameter */
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);
    Xv_cached_window *cached_window;

    for (cached_window = screen->cached_windows; cached_window != NULL;
	 cached_window = cached_window->next) {
	if (cached_window->busy == FALSE &&
	    cached_window->borders == (short) borders &&
	    cached_window->visual_class == visual_class) {
	    cached_window->busy = TRUE;
	    *new_window = FALSE;
	    return ((Xv_Window) cached_window->window);
	}
    }

    *new_window = TRUE;
    cached_window = (Xv_cached_window *) xv_alloc(Xv_cached_window);
    cached_window->window = (Xv_Window) xv_create(
	xv_get(screen_public, XV_ROOT), WINDOW,
	WIN_BIT_GRAVITY, ForgetGravity,
	WIN_BORDER, borders,
        XV_VISUAL_CLASS, visual_class,						  
	WIN_NOTIFY_SAFE_EVENT_PROC, event_proc,
	WIN_TOP_LEVEL_NO_DECOR, TRUE,
	XV_SHOW, FALSE,
	0);

    if (screen->cached_windows == NULL) {
	screen->cached_windows = cached_window;
    } else {
	cached_window->next = screen->cached_windows;
	screen->cached_windows = cached_window;
    }
    cached_window->busy = TRUE;
    cached_window->borders = (short) borders;
    cached_window->visual_class = visual_class;
    return ((Xv_Window) cached_window->window);
}
