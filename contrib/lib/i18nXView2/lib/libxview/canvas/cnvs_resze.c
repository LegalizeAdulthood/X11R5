#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)cnvs_resze.c 20.17 90/03/20";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/cnvs_impl.h>

void            canvas_resize_paint_window();

static void     canvas_set_paint_window_size();
static void     canvas_view_maxsize();

Pkg_private void canvas_update_scrollbars();


/*
 * resize the paint window to account for the new view window size.
 */

void
canvas_resize_paint_window(canvas, width, height)
    Canvas_info    *canvas;
    int             width, height;
{
    int             view_width = 0, view_height = 0;

    /* paint window doesn't ever change size */

    /* use the old value if none specified */
    if (width == 0) {
	width = canvas->width;
    }
    if (height == 0) {
	height = canvas->height;
    }
    /* determine maximim view size of all viewers */
    if (status(canvas, auto_expand) || status(canvas, auto_shrink)) {
	canvas_view_maxsize(canvas, &view_width, &view_height);
    }
    /*
     * if auto expand is on, always expand the canvas to at the edges of the
     * viewing pixwin or the minimum width/height.
     */
    if (status(canvas, auto_expand)) {
	width = MAX(width, view_width);
	height = MAX(height, view_height);
    }
    /*
     * if auto shrink is on, always shrink the canvas to the edges of the
     * viewing pixwin or the minimum width/height.
     */
    if (status(canvas, auto_shrink)) {
	width = MIN(width, view_width);
	height = MIN(height, view_height);
    }
    /* width and height must equal some minimum */
    width = MAX(width, canvas->min_paint_width);
    height = MAX(height, canvas->min_paint_height);

    if (canvas->width != width || canvas->height != height) {
	canvas_set_paint_window_size(canvas, width, height);
    }
}

static void
canvas_set_paint_window_size(canvas, width, height)
    Canvas_info    *canvas;
    int             width, height;
{
    Xv_Window       paint_window;
    Rect            paint_rect;
    Xv_Window       view_window;
    Rect	    view_rect;
    unsigned int    visable;

    canvas->width = MAX(width, 1);
    canvas->height = MAX(height, 1);
    CANVAS_EACH_PAINT_WINDOW(CANVAS_PUBLIC(canvas), paint_window)
	paint_rect = *(Rect *) xv_get(paint_window, WIN_RECT, 0);
        paint_rect.r_width = canvas->width;
        paint_rect.r_height = canvas->height;
        view_window = (Xv_Window) xv_get(paint_window, CANVAS_PAINT_VIEW_WINDOW, 0);
        view_rect = *(Rect *) xv_get(view_window, WIN_RECT, 0);
    
        /*
	 * check to see if paint window needs to be moved to accomodate 
	 * new size
	 */
        if (paint_rect.r_width <= view_rect.r_width) 
	  paint_rect.r_left = 0;
	else {
	    visable = paint_rect.r_width + paint_rect.r_left;
	    if (visable < view_rect.r_width)
	      paint_rect.r_left += view_rect.r_width - visable;
	}
    
        if (paint_rect.r_height <= view_rect.r_height)
	  paint_rect.r_top = 0;
	else {
	    visable = paint_rect.r_height + paint_rect.r_top;
	    if (visable < view_rect.r_height)
	      paint_rect.r_top += view_rect.r_height - visable;
	    
	}
    (void) xv_set(paint_window, WIN_RECT, &paint_rect, 0);
    CANVAS_END_EACH


    /* tell the scrollbars about it */
	canvas_update_scrollbars(canvas);
}

static void
canvas_view_maxsize(canvas, view_width, view_height)
    Canvas_info    *canvas;
    int            *view_width, *view_height;
{
    Xv_Window       view;
    Rect            view_rect;

    *view_width = *view_height = 0;

    OPENWIN_EACH_VIEW(CANVAS_PUBLIC(canvas), view)
	view_rect = *(Rect *) xv_get(view, WIN_RECT);
    *view_width = MAX(*view_width, view_rect.r_width);
    *view_height = MAX(*view_height, view_rect.r_height);
    OPENWIN_END_EACH
}
