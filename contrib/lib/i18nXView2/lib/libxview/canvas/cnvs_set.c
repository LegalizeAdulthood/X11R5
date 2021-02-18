#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)cnvs_set.c 70.2 91/07/03";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/cnvs_impl.h>
#include <xview/scrollbar.h>
#include <xview_private/draw_impl.h>
#include <X11/Xlib.h>
#ifdef	OW_I18N
#include <xview_private/xv_i18n_impl.h>
#include <xview_private/db_impl.h>
#include <xview/window.h>
#include <xview/frame.h>
#include <xview/server.h>
#include <X11/Xlib.h>
#include <X11/XSunExt.h>
#endif	OW_I18N

extern void     window_set_bit_gravity();

static void     canvas_set_bit_gravity();
static void     canvas_append_paint_attrs();


Xv_opaque
canvas_set_avlist(canvas_public, avlist)
    Canvas          canvas_public;
    Attr_avlist     avlist;
{
    Canvas_info    *canvas = CANVAS_PRIVATE(canvas_public);
    Attr_avlist	    attrs;
    int             width = 0;
    int             height = 0;
    int             vsb_set = 0, hsb_set = 0;
    Scrollbar       vsb = NULL, hsb = NULL;
    short           new_paint_size = FALSE;
    short           recheck_paint_size = FALSE;
    int             ok = TRUE;
    Xv_Window       paint_window;
    Rect            pw_rect;

#ifdef OW_I18N
    int		    db_count = 0;
    XID             db;
    Xv_object	    server;
    Xv_opaque	    qlist;
    int             db_int; 
    char            *db_str;    
   
#endif	OW_I18N

#ifdef OW_I18N
    server = XV_SERVER_FROM_WINDOW(canvas_public);
    db = xv_get(server, SERVER_RESOURCE_DB);
    qlist = xv_get(canvas_public, XV_INSTANCE_QLIST);
#endif  OW_I18N

#ifndef OW_I18N

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch (attrs[0]) {

#else OW_I18N

    attrs = avlist;
    while (*attrs || db_count) {

        switch (attrs[0]) {

          case XV_USE_DB :
           db_count++;
           break;

#endif OW_I18N

	  case CANVAS_WIDTH:
#ifdef OW_I18N
            if (db_count) {
		db_int = (int)db_get_data(db, qlist, "canvas_width", XV_INT,
				    (int)attrs[1]);
            } else {
                db_int = (int) attrs[1];
            }
            if (canvas->width != db_int) {
                width = db_int;
		new_paint_size = TRUE;
            }
#else
	    if (canvas->width != (int) attrs[1]) {
		width = (int) attrs[1];
		new_paint_size = TRUE;
	    }
#endif
	    break;

	  case CANVAS_HEIGHT:
#ifdef OW_I18N 
            if (db_count) { 
		db_int = (int)db_get_data(db, qlist, "canvas_height", XV_INT,
				    (int)attrs[1]);
            } else { 
                db_int = (int) attrs[1]; 
            }   
            if (canvas->height != db_int) { 
                height = db_int; 
                new_paint_size = TRUE;
            }
#else
	    if (canvas->height != (int) attrs[1]) {
		height = (int) attrs[1];
		new_paint_size = TRUE;
	    }
#endif
	    break;

	  case CANVAS_MIN_PAINT_WIDTH:
#ifdef OW_I18N  
            if (db_count) {  
		db_int = (int)db_get_data(db, qlist, "canvas_min_paint_width", 
				    XV_INT, (int)attrs[1]);
            } else { 
                db_int = (int) attrs[1];  
            }    
            if (canvas->min_paint_width != db_int) {  
                canvas->min_paint_width = db_int;  
                new_paint_size = TRUE; 
            }
#else
	    if (canvas->min_paint_width != (int) attrs[1]) {
		canvas->min_paint_width = (int) attrs[1];
		new_paint_size = TRUE;
	    }
#endif
	    break;

	  case CANVAS_MIN_PAINT_HEIGHT:
#ifdef OW_I18N
            if (db_count) {   
		db_int = (int)db_get_data(db, qlist, "canvas_min_paint_height", 
				    XV_INT, (int)attrs[1]);
            } else {  
                db_int = (int) attrs[1];   
            }     
            if (canvas->min_paint_height != db_int) {   
                canvas->min_paint_height = db_int;  
                new_paint_size = TRUE;  
            }
#else
	    if (canvas->min_paint_height != (int) attrs[1]) {
		canvas->min_paint_height = (int) attrs[1];
		new_paint_size = TRUE;
	    }
	    break;
#endif

	  case CANVAS_MARGIN:
	    /*
	     * margin = (int) attrs[1]; if (margin < 0) ok = FALSE; else if
	     * (margin != canvas->margin) { canvas->margin = margin;
	     * new_view_size = TRUE; }
	     */
	    break;

	  case CANVAS_X_PAINT_WINDOW:
	    if ((int) attrs[1] == status(canvas, x_canvas)) {
		break;
	    }
	    if (attrs[1]) {
		status_set(canvas, x_canvas);
	    } else {
		status_reset(canvas, x_canvas);
	    }
	    CANVAS_EACH_PAINT_WINDOW(canvas_public, paint_window)
		xv_set(paint_window, WIN_X_PAINT_WINDOW, attrs[1], 0);
	    CANVAS_END_EACH
		break;

	  case CANVAS_NO_CLIPPING:
	      if ((int) attrs[1] == status(canvas, no_clipping)) {
		  break;
	      }
	      if (attrs[1]) {
		status_set(canvas, no_clipping);
	      } else {
		status_reset(canvas, no_clipping);
	      }
	      CANVAS_EACH_PAINT_WINDOW(canvas_public, paint_window)
		  xv_set(paint_window, WIN_NO_CLIPPING, attrs[1], 0);
	      CANVAS_END_EACH
	      break;

	  case CANVAS_REPAINT_PROC:
	    canvas->repaint_proc = (Function) attrs[1];
	    break;

	  case CANVAS_RESIZE_PROC:
	    canvas->resize_proc = (Function) attrs[1];
	    break;

	  case CANVAS_AUTO_EXPAND:
	    if ((int) attrs[1] == status(canvas, auto_expand))
		break;
	    if (attrs[1])
		status_set(canvas, auto_expand);
	    else
		status_reset(canvas, auto_expand);
	    recheck_paint_size = TRUE;
	    break;

	  case CANVAS_AUTO_SHRINK:
	    if ((int) attrs[1] == status(canvas, auto_shrink))
		break;
	    if (attrs[1])
		status_set(canvas, auto_shrink);
	    else
		status_reset(canvas, auto_shrink);
	    recheck_paint_size = TRUE;
	    break;

	  case CANVAS_RETAINED:
	    if ((int) attrs[1] == status(canvas, retained)) {
		break;
	    }
	    if (attrs[1]) {
		status_set(canvas, retained);
	    } else {
		status_reset(canvas, retained);
	    }
	    CANVAS_EACH_PAINT_WINDOW(canvas_public, paint_window)
		xv_set(paint_window, WIN_RETAINED, attrs[1], 0);
	    CANVAS_END_EACH
		break;

	  case CANVAS_CMS_REPAINT:
	    if (attrs[1]) {
		status_set(canvas, cms_repaint);
	    } else {
		status_reset(canvas, cms_repaint);
	    }
	    break;

	  case CANVAS_FIXED_IMAGE:
	    /* don't do anything if no change */
	    if (status(canvas, fixed_image) != (int) attrs[1]) {
		if ((int) attrs[1]) {
		    status_set(canvas, fixed_image);
		} else {
		    status_reset(canvas, fixed_image);
		}
		canvas_set_bit_gravity(canvas);
	    }
	    break;

	  case WIN_VERTICAL_SCROLLBAR:
	  case OPENWIN_VERTICAL_SCROLLBAR:
	    vsb = (Scrollbar) attrs[1];
	    vsb_set = TRUE;
	    break;

	  case WIN_HORIZONTAL_SCROLLBAR:
	  case OPENWIN_HORIZONTAL_SCROLLBAR:
	    hsb = (Scrollbar) attrs[1];
	    hsb_set = TRUE;
	    break;

	  case WIN_SET_FOCUS:
	    ATTR_CONSUME(attrs[0]);
	    ok = FALSE;
	    CANVAS_EACH_PAINT_WINDOW(canvas_public, paint_window)
		Xv_Drawable_info *pw_info;
	    DRAWABLE_INFO_MACRO(paint_window, pw_info);
	    if (!xv_no_focus(pw_info) &&
		win_getinputcodebit((Inputmask *) xv_get(paint_window,
						WIN_INPUT_MASK), KBD_USE)) {
		win_set_kbd_focus(paint_window, xv_xid(pw_info));
		ok = TRUE;
	    }
	    CANVAS_END_EACH
	    break;

	  case CANVAS_PAINTWINDOW_ATTRS:
	    if (status(canvas, created)) {
		CANVAS_EACH_PAINT_WINDOW(canvas_public, paint_window)
		    xv_set_avlist(paint_window, &(attrs[1]));
		CANVAS_END_EACH
	    } else {
		canvas_append_paint_attrs(canvas, &(attrs[1]));
	    }
	    break;

	  case WIN_CMS_CHANGE:
	     if (status(canvas, created)) {
		 Xv_Drawable_info	*info;
		 Xv_Window      	view_public;
		 Canvas_view_info 	*view;
		 Scrollbar		sb;
		 Cms			cms;
		 int			cms_fg, cms_bg;

		 DRAWABLE_INFO_MACRO(canvas_public, info);
		 cms = xv_cms(info);
		 cms_fg = xv_cms_fg(info);
		 cms_bg = xv_cms_bg(info);
		 OPENWIN_EACH_VIEW(canvas_public, view_public)
		     view = CANVAS_VIEW_PRIVATE(view_public);
		     window_set_cms(view_public, cms, cms_bg, cms_fg);
		     window_set_cms(view->paint_window, cms, cms_bg, cms_fg);
		 OPENWIN_END_EACH
	     }
	     break;

	  case XV_END_CREATE:
	    /* adjust paint window here rather then view end */
	    /* create because canvas_resize_paint_window */
	    /* assumes view window is known to canvas */
	    paint_window = (Xv_Window) xv_get(canvas_public, CANVAS_NTH_PAINT_WINDOW, 0);
	    pw_rect = *(Rect *) xv_get(paint_window, WIN_RECT);
	    canvas_resize_paint_window(canvas, pw_rect.r_width, pw_rect.r_height);

	    if (status(canvas, no_clipping)) {
		CANVAS_EACH_PAINT_WINDOW(canvas_public, paint_window)
		    xv_set(paint_window, WIN_NO_CLIPPING, TRUE, 0);
		CANVAS_END_EACH
	    }
	    break;

	  default:
#ifdef OW_I18N
	    if (attrs[0])
#endif OW_I18N
		xv_check_bad_attr(&xv_canvas_pkg, attrs[0]);
	    break;
	}
#ifdef OW_I18N
       	if (attrs[0] == XV_USE_DB)
	    attrs++;
       	else if (db_count && !attrs[0]) {
      	    db_count--;
     	    attrs++;
        } else {
            attrs = attr_next(attrs);
        }
#endif OW_I18N
    }

    if (!status(canvas, created)) {
	/* copy width and height if set */
	if (width != 0) {
	    canvas->width = width;
	}
	if (height != 0) {
	    canvas->height = height;
	}
    } else {

	if (new_paint_size) {
	    canvas_resize_paint_window(canvas, width, height);
	} else if (recheck_paint_size) {
	    canvas_resize_paint_window(canvas, canvas->width, canvas->height);
	}
    }

    if (vsb_set) {
	canvas_set_scrollbar_object_length(canvas, SCROLLBAR_VERTICAL, vsb);
    }
    if (hsb_set) {
	canvas_set_scrollbar_object_length(canvas, SCROLLBAR_HORIZONTAL, hsb);
    }
    return (Xv_opaque) (ok ? XV_OK : XV_ERROR);
}

static void
canvas_set_bit_gravity(canvas)
    Canvas_info    *canvas;
{
    Xv_Window       paint_window;
    int             bit_value;

    if (status(canvas, fixed_image)) {
	bit_value = NorthWestGravity;
    } else {
	bit_value = ForgetGravity;
    }

    CANVAS_EACH_PAINT_WINDOW(CANVAS_PUBLIC(canvas), paint_window)
	window_set_bit_gravity(paint_window, bit_value);
    CANVAS_END_EACH
}

static void
canvas_append_paint_attrs(canvas, argv)
    Canvas_info    *canvas;
    Attr_avlist     argv;
{
    if (canvas->paint_avlist == NULL) {
	canvas->paint_avlist = (Attr_avlist) xv_alloc_n(Canvas_attribute, ATTR_STANDARD_SIZE);
	canvas->paint_end_avlist = canvas->paint_avlist;
    }
    canvas->paint_end_avlist = (Attr_avlist) attr_copy_avlist(canvas->paint_end_avlist, argv);
}
