#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)canvas.c 70.2 91/07/03";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/cnvs_impl.h>
#include <xview_private/win_keymap.h>
#include <xview_private/draw_impl.h>
#ifdef	OW_I18N
#include <xview/font.h>
#include <xview/frame.h>
#include <X11/Xlib.h>
#include <X11/XSunExt.h>


Xv_opaque canvas_create_mle();
#endif	OW_I18N

Attr_attribute  canvas_context_key;
Attr_attribute  canvas_view_context_key;

#ifdef OW_I18N
Attr_attribute	frame_ic_refcnt;
#endif OW_I18N

/* ARGSUSED */
int
canvas_init(parent, canvas_public, avlist)
    Xv_Window       parent;
    Canvas          canvas_public;
    Attr_attribute  avlist[];
{
    Xv_canvas          *canvas_object = (Xv_canvas *) canvas_public;
    Canvas_info        *canvas;
    Xv_Drawable_info   *info;

#ifdef OW_I18N
    Attr_avlist		attrs;
    Frame	    	frame_public;
    Xv_pkg	   	*result;
#endif OW_I18N

    DRAWABLE_INFO_MACRO(canvas_public, info);

    if (canvas_context_key == (Attr_attribute) 0) {
	canvas_context_key = xv_unique_key();
#ifdef OW_I18N
	frame_ic_refcnt = xv_unique_key();
#endif OW_I18N
    }
    canvas = xv_alloc(Canvas_info);

    /* link to object */
    canvas_object->private_data = (Xv_opaque) canvas;
    canvas->public_self = canvas_public;

    status_set(canvas, fixed_image);
    status_set(canvas, auto_expand);
    status_set(canvas, auto_shrink);
    status_set(canvas, retained);

    /* Make all the paint windows inherit the WIN_DYNAMIC_VISUAL attribute */
    xv_set(canvas_public, WIN_INHERIT_COLORS, TRUE, 0);

    return XV_OK;
}


int
canvas_destroy(canvas_public, stat)
    Canvas          canvas_public;
    Destroy_status  stat;
{
    Canvas_info    *canvas = CANVAS_PRIVATE(canvas_public);
    int		    ic_refcnt = 0;


    if (stat == DESTROY_CLEANUP) {
#ifdef OW_I18N
	Frame	    	frame_public = 
			    (Frame)xv_get(canvas_public, WIN_FRAME);

	if (canvas->ic) {
	    ic_refcnt = (int) xv_get(frame_public, XV_KEY_DATA,
					frame_ic_refcnt);
	    /* Destroy IC here */
	    if (!--ic_refcnt) {
		Xv_opaque	input_window =
		    (Xv_opaque)xv_get(frame_public, FRAME_INPUT_WINDOW);
		XDestroyIC(canvas->ic);
		canvas->ic = NULL;
		xv_set(frame_public, WIN_IC, NULL, 0);
		xv_destroy(input_window);
		xv_set(frame_public, FRAME_INPUT_WINDOW, NULL, 0);
	    }
	    xv_set(frame_public, XV_KEY_DATA, frame_ic_refcnt, ic_refcnt, 0);
	}
#endif OW_I18N
	free((char *) canvas);
    }
    return XV_OK;
}


#ifdef	OW_I18N
/*
 * canvas_create_mle(frame, font) calls ml_panel_create_canvas_mle()
 * to create a panel sw and a panel text item for the canvas
 * intermediate text. (Xv_font *)font is the font used to display
 * the intermediate text.
 * We need to access panel->pa_tscreen to let repaint happen but
 * anel_impl.h conflicts with canvas_impl.h, thus, we do the real
 * work in ml_panel_create_canvas_mle.
 */
Xv_opaque
canvas_create_mle(frame, font)
	Frame		frame;
	Xv_font		font;
{
	Xv_font canvas_interm_font;
	Frame	input_frame;

	if (font == (Xv_font)NULL) 
	    canvas_interm_font= (Xv_Font)xv_get(frame, XV_FONT);
	else
	    canvas_interm_font = font;

	input_frame = ml_panel_create_canvas_mle(frame, canvas_interm_font);

	return (input_frame);
}

#endif	OW_I18N
