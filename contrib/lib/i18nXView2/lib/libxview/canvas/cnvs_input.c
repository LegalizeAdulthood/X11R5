#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)cnvs_input.c 70.3 91/08/22";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/cnvs_impl.h>
#include <xview/scrollbar.h>
#include <xview/canvas.h>
#include <xview/xv_xrect.h>
#include <xview_private/draw_impl.h>
#ifdef	OW_I18N
#include <X11/Xlib.h>
#include <X11/XSunExt.h>
#endif	OW_I18N

#define window_event_proc(win, event, arg) \
    (((int (*)())(window_get(win, WIN_EVENT_PROC)))(win, event, arg))


extern int OW_GC_KEY;


Xv_private void openwin_check_gc_color();
Xv_private Xv_xrectlist *screen_get_clip_rects();
Xv_private int server_get_fullscreen();

extern Rectlist *win_get_damage();

static void canvas_clear_damage();
static void canvas_inform_resize();

/*
 * handle events posted to the view window.
 */

/* ARGSUSED */
Notify_value
canvas_view_event(view_public, event, arg, type)
    Canvas_view     view_public;
    Event          *event;
    Notify_arg      arg;
    Notify_event_type type;
{
    Canvas_view_info *view = CANVAS_VIEW_PRIVATE(view_public);
    Canvas_info    *canvas = view->private_canvas;
    Xv_Window       paint_window = view->paint_window;
    Notify_value    result;
    Rect            paint_rect;

    result = notify_next_event_func(view_public, (Notify_event) event, arg, type);

    switch (event_id(event)) {
      case WIN_RESIZE:
	paint_rect = *(Rect *) xv_get(paint_window, WIN_RECT);
	canvas_resize_paint_window(canvas, paint_rect.r_width, paint_rect.r_height);
	break;
      case SCROLLBAR_REQUEST:
	canvas_scroll(paint_window, (Scrollbar) arg);
	break;
      default:
	break;
    }

    return (result);
}

/*
 * Handle events for the paint window.  These events are passed on to the
 * canvas client CANVAS_EVENT_PROC.
 */

/* ARGSUSED */
Notify_value
canvas_paint_event(window_public, event, arg, type)
    Xv_Window       window_public;
    Event          *event;
    Notify_arg      arg;
    Notify_event_type type;
{
    Canvas_info    *canvas;
    Canvas          canvas_public;
    char           *help_data;
    Notify_value    result;
#ifdef	OW_I18N
    XIC		    ic;
    Xv_Window       paint_window;
    int		    flag = 0;
#endif	OW_I18N

    result = notify_next_event_func(window_public, (Notify_event) event, arg, type);

    switch (event_action(event)) {
      case WIN_REPAINT:
      case WIN_GRAPHICS_EXPOSE:
	canvas = (Canvas_info *) xv_get(window_public, XV_KEY_DATA,
					canvas_context_key);
	canvas_inform_repaint(canvas, window_public);
	break;

      case WIN_RESIZE:
	/* scrollbars have already been updated */
	/* tell the client the paint window changed size */
	canvas = (Canvas_info *) xv_get(window_public, XV_KEY_DATA,
					canvas_context_key);
	canvas_inform_resize(canvas);
	break;

      case ACTION_HELP:
	if (event_is_down(event)) {
	    canvas = (Canvas_info *) xv_get(window_public, XV_KEY_DATA,
					    canvas_context_key);
	    canvas_public = CANVAS_PUBLIC(canvas);
	    if ((Attr_pkg) xv_get(canvas_public, WIN_TYPE) == CANVAS_TYPE) {
		help_data = (char *) xv_get(canvas_public, XV_HELP_DATA);
		if (help_data)
		    xv_help_show(window_public, help_data, event);
	    }
	}
	return NOTIFY_DONE;

#ifdef	OW_I18N
      case KBD_USE: {
        Xv_Drawable_info *info;
	XID xid;
        
	canvas = (Canvas_info *) xv_get(window_public, XV_KEY_DATA,
					canvas_context_key);
/*
	canvas_public = CANVAS_PUBLIC(canvas);
*/

	if (xv_get(window_public, WIN_USE_IM) &&
	    xv_get(window_public, WIN_CONSUME_EVENT, WIN_ASCII_EVENTS)) {
	    

	    DRAWABLE_INFO_MACRO(window_public, info);
		
	    if (ic = (XIC)xv_get(window_public, WIN_IC)) {
		XSetICValues(ic, XNFocusWindow, xv_xid(info), NULL);
		XSetICFocus(ic);
	    }
	}
	break;
      }
#endif	OW_I18N
      default:
	break;
    }
    return (result);
}

static void
canvas_inform_resize(canvas)
    register Canvas_info *canvas;
{
    Canvas          canvas_public = CANVAS_PUBLIC(canvas);

    if (!canvas->resize_proc) {
	return;
    }
    (*canvas->resize_proc) (canvas_public, canvas->width, canvas->height);
}

/*
 * tell the client to repaint the paint window.
 */
canvas_inform_repaint(canvas, paint_window)
    Canvas_info    *canvas;
    Xv_Window       paint_window;
{
    Rectlist       *win_damage, damage;

    if (!(win_damage = win_get_damage(paint_window))) {
	win_damage = &rl_null;
    }
    damage = rl_null;
    rl_copy(win_damage, &damage);

    if (xv_get(CANVAS_PUBLIC(canvas), OPENWIN_AUTO_CLEAR)) {
	canvas_clear_damage(paint_window, &damage);
    }
    if (canvas->repaint_proc) {
	if (status(canvas, x_canvas)) {
	    Xv_xrectlist    xrects;

	    /*
	     * If there is no damage on the paint window, pass NULL
	     * xrectangle array and a count of zero to let the application
	     * know that there is no clipping.
	     */
	    if (win_damage == &rl_null) {
		(*canvas->repaint_proc) (CANVAS_PUBLIC(canvas), paint_window,
				       XV_DISPLAY_FROM_WINDOW(paint_window),
				     xv_get(paint_window, XV_XID), NULL);
	    } else {
		xrects.count = win_convert_to_x_rectlist(&damage,
					  xrects.rect_array, XV_MAX_XRECTS);
		(*canvas->repaint_proc) (CANVAS_PUBLIC(canvas), paint_window,
				       XV_DISPLAY_FROM_WINDOW(paint_window),
					 xv_get(paint_window, XV_XID),
					 &xrects);
	    }
	} else {
	    (*canvas->repaint_proc) (CANVAS_PUBLIC(canvas), paint_window, &damage);
	}
    }
    rl_free(&damage);
}

/*
 * translate a canvas paint window-space event to a canvas subwindow-space
 * event.
 */
Event          *
canvas_window_event(canvas_public, event)
    Canvas          canvas_public;
    register Event *event;
{
    Xv_Window       paint_window;
    Event           tmp_event;
    int             x, y;

    paint_window = xv_get(canvas_public, CANVAS_NTH_PAINT_WINDOW, 0);
    if (paint_window == NULL) {
	/* call xv_error */
	return (event);
    }
    tmp_event = *event;
    win_translate_xy(paint_window, canvas_public,
		     event_x(event), event_y(event), &x, &y);
    event_set_x(&tmp_event, x);
    event_set_y(&tmp_event, y);
    return (&tmp_event);
}

/*
 * translate a window-space event to a canvas-space event.
 */
Event          *
canvas_event(canvas_public, event)
    Canvas          canvas_public;
    register Event *event;
{
    Xv_Window       paint_window;
    Event           tmp_event;
    int             x, y;

    paint_window = xv_get(canvas_public, CANVAS_NTH_PAINT_WINDOW, 0);
    if (paint_window == NULL) {
	/* call xv_error */
	return (event);
    }
    tmp_event = *event;
    win_translate_xy(paint_window, canvas_public,
		     event_x(event), event_y(event), &x, &y);
    event_set_x(&tmp_event, x);
    event_set_y(&tmp_event, y);
    return (&tmp_event);
}


/* Clear the damaged area */
static void
canvas_clear_damage(window, rl)
    Xv_Window       window;
    Rectlist       *rl;
{
    register Xv_Drawable_info *info;
    Xv_xrectlist   *clip_xrects;
    Display	   *display;
    XGCValues	    gc_values;
    unsigned long   gc_value_mask;
    Xv_Screen      screen;
    GC             *openwin_gc_list;
    
    if (!rl)
	return;
    DRAWABLE_INFO_MACRO(window, info);
    clip_xrects = screen_get_clip_rects(xv_screen(info));
    gc_value_mask = GCForeground | GCBackground | GCFunction | GCPlaneMask |
	GCSubwindowMode | GCFillStyle;
    gc_values.background = xv_bg(info);
    gc_values.function = GXcopy;
    gc_values.plane_mask = xv_plane_mask(info);
    if (gc_values.stipple = xv_get(window, WIN_BACKGROUND_PIXMAP)) {
	gc_value_mask |= GCStipple;
	gc_values.foreground = xv_fg(info);
	gc_values.fill_style = FillOpaqueStippled;
    } else {
	gc_values.foreground = xv_bg(info);
	gc_values.fill_style = FillSolid;
    }
    if (server_get_fullscreen(xv_server(info)))
	gc_values.subwindow_mode = IncludeInferiors;
    else
	gc_values.subwindow_mode = ClipByChildren;
    display = xv_display(info);
    screen = xv_screen(info);
    openwin_gc_list = (GC *) xv_get( screen, XV_KEY_DATA, OW_GC_KEY );
    XChangeGC(display, openwin_gc_list[OPENWIN_NONSTD_GC], gc_value_mask,
	      &gc_values);
    XSetClipRectangles(display, openwin_gc_list[OPENWIN_NONSTD_GC],
		       0, 0, clip_xrects->rect_array, clip_xrects->count,
		       Unsorted);
    XFillRectangle(display, xv_xid(info),
		   openwin_gc_list[OPENWIN_NONSTD_GC], 
		   rl->rl_bound.r_left, rl->rl_bound.r_top,
		   rl->rl_bound.r_width, rl->rl_bound.r_height);
    XSetClipMask(display, openwin_gc_list[OPENWIN_NONSTD_GC], None);
}
