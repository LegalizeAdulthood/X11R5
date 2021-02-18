#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)ow_paint.c 50.6 90/10/16";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	ow_paint.c Product:	SunView 2.0
 * 
 * Description:
 * 
 * paint routines for openwin
 * 
 */


/*
 * Include files:
 */
#include <xview_private/ow_impl.h>
#include <xview/font.h>
#include <xview/rectlist.h>
#include <xview_private/draw_impl.h>

Xv_private GC openwin_gc_list[OPENWIN_NBR_GCS];

static unsigned short openwin_gray50_bitmap[16] = {   /* 50% gray pattern */
    0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555,
    0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555
};

/*
 * XView Private
 */
Xv_private void openwin_check_gc_color();
Xv_private void openwin_create_gcs();

/*
 * Package Private
 */
Pkg_private void openwin_clear_damage();
#ifdef SELECTABLE_VIEWS
Pkg_private void openwin_paint_borders();
Pkg_private void openwin_hilite_view();
Pkg_private void openwin_lolite_view();
#endif SELECTABLE_VIEWS

/*
 * BUG ALERT:  move to screen package
 */
int   OW_GC_KEY;

/******************************************************************/

/*
 * openwin_clear_damage - clear the damaged areas of the openwin
 */
Pkg_private void
openwin_clear_damage(window, rl)
    Xv_Window       window;
    Rectlist       *rl;
{
    register Xv_Drawable_info *info;
    Xv_Screen       screen;
    GC              *openwin_gc_list;

    if (rl) {
	DRAWABLE_INFO_MACRO(window, info);
	openwin_check_gc_color(info, OPENWIN_CLR_GC);
	screen = xv_screen(info);
	openwin_gc_list = (GC *) xv_get( screen, XV_KEY_DATA, OW_GC_KEY );
	XFillRectangle(xv_display(info), xv_xid(info),
		       openwin_gc_list[OPENWIN_CLR_GC], 
		       rl->rl_bound.r_left, rl->rl_bound.r_top,
		       rl->rl_bound.r_width, rl->rl_bound.r_height);
    }
}

/* 
 * NOTE: The selectable view code was never finished.  It should not
 *       be used to implement selectable views, as it was attempting to
 *       use pixwin operations for drawing.  Also it was never maintained
 *       when changes were made to the openwin code.  The reason that
 *       it is still here is that there might be some useful information
 *       to be gleaned from the attempt that might help the person who
 *       will implement selectable views.
 */
#ifdef SELECTABLE_VIEWS
openwin_paint_borders(owin_public)
    Openwin         owin_public;
{
    Openwin_view_info *view;
    Xv_openwin_info *owin = OPENWIN_PRIVATE(owin_public);

    for (view = owin->views; view != NULL; view = view->next_view) {
	openwin_paint_border(owin_public, view, TRUE);
    }
}

openwin_paint_border(owin_public, view, on)
    Openwin         owin_public;
    Openwin_view_info *view;
    int             on;
{
    Rect            r;
    int             x, y, w, h;
    Xv_openwin_info *owin = OPENWIN_PRIVATE(owin_public);
    int             border_width;
    int             stroke;

    if (!STATUS(owin, mapped))
	return;
    /*
     * BUG ALERT:  Do not use pw layer!
     */
    stroke = (on) ? 1 : 0;
    border_width = openwin_border_width(owin_public, view->view);
    win_getrect(view->view, &r);
    x = r.r_left - border_width;
    y = r.r_top - border_width;
    w = r.r_width + 2 * border_width - 1;
    h = r.r_height + 2 * border_width - 1;
    xv_vector(owin_public, x + 0, y + 0, x + w, y + 0, PIX_SRC, stroke);
    xv_vector(owin_public, x + 0, y + h, x + w, y + h, PIX_SRC, stroke);
    xv_vector(owin_public, x + 0, y + 0, x + 0, y + h, PIX_SRC, stroke);
    xv_vector(owin_public, x + w, y + 0, x + w, y + h, PIX_SRC, stroke);

    if (view == owin->seln_view && on)
	openwin_hilite_view(owin_public, view);
    else
	openwin_lolite_view(owin_public, view);
}

void
openwin_hilite_view(owin_public, view)
    Openwin         owin_public;
    Openwin_view_info *view;
{
    Rect            r;
    int             x, y, w, h;
    int             border_width = openwin_border_width(owin_public, view->view);

    win_getrect(view->view, &r);
    x = r.r_left - border_width + 1;
    y = r.r_top - border_width + 1;
    w = r.r_width + 2 * border_width - 3;
    h = r.r_height + 2 * border_width - 3;
    xv_vector(owin_public, x + 0, y + 0, x + w, y + 0, PIX_SRC, 1);
    xv_vector(owin_public, x + 0, y + h, x + w, y + h, PIX_SRC, 1);
    xv_vector(owin_public, x + 0, y + 0, x + 0, y + h, PIX_SRC, 1);
    xv_vector(owin_public, x + w, y + 0, x + w, y + h, PIX_SRC, 1);
}

void
openwin_lolite_view(owin_public, view)
    Openwin         owin_public;
    Openwin_view_info *view;
{
    Rect            r;
    int             x, y, w, h;
    int             border_width = openwin_border_width(owin_public, view->view);

    win_getrect(view->view, &r);
    x = r.r_left - border_width + 1;
    y = r.r_top - border_width + 1;
    w = r.r_width + 2 * border_width - 3;
    h = r.r_height + 2 * border_width - 3;
    xv_vector(owin_public, x + 0, y + 0, x + w, y + 0, PIX_SRC, 0);
    xv_vector(owin_public, x + 0, y + h, x + w, y + h, PIX_SRC, 0);
    xv_vector(owin_public, x + 0, y + 0, x + 0, y + h, PIX_SRC, 0);
    xv_vector(owin_public, x + w, y + 0, x + w, y + h, PIX_SRC, 0);
}
#endif SELECTABLE_VIEWS

/*
 * BUG ALERT: The following code that handles the creation of several
 *            GCs should be moved to the screen package, as it really
 *            doesn't belong in the openwin code.
 */
Xv_private void
openwin_create_gcs(pw)
    Xv_opaque	    pw;   /* paint window */
{
    Pixfont	   *bold_font;
    Display	    *display;
    XGCValues	    gc_value;
    unsigned long   gc_value_mask;
    int		    i;
    Xv_Drawable_info *info;
    Pixfont	   *std_font;
    Drawable	    xid;
    GC             *openwin_gc_list;
    Xv_Screen       screen;
    
    DRAWABLE_INFO_MACRO(pw, info);
    screen = xv_screen(info);
    if (!OW_GC_KEY)
        OW_GC_KEY = xv_unique_key();
    openwin_gc_list = (GC *) xv_get( screen, XV_KEY_DATA, OW_GC_KEY);
    if (openwin_gc_list)
	return;   /* GC's already created */

    openwin_gc_list = (GC *) calloc( OPENWIN_NBR_GCS, sizeof(GC));
    xv_set( screen, XV_KEY_DATA, OW_GC_KEY, openwin_gc_list, 0);

    display = xv_display(info);
    xid = xv_xid(info);
    std_font = (Pixfont *) xv_get(pw, XV_FONT);
    for (i=0; i<OPENWIN_NBR_GCS; i++) {
	gc_value.foreground = xv_fg(info);
	gc_value.background = xv_bg(info);
	gc_value.function = GXcopy;
	gc_value.plane_mask = xv_plane_mask(info);
	gc_value.graphics_exposures = FALSE; /* Don't generate WIN_NO_EXPOSE or
					      * WIN_GRAPHICS_EXPOSE events. */
	gc_value_mask = GCForeground | GCBackground | GCFunction |
	    GCPlaneMask | GCGraphicsExposures;
	switch (i) {
	  case OPENWIN_SET_GC:
	  case OPENWIN_NONSTD_GC:
	    break;
	  case OPENWIN_CLR_GC:
	    gc_value.foreground = xv_bg(info);
	    break;
	  case OPENWIN_TEXT_GC:
#ifdef	OW_I18N
	    /* do nothing since using font sets always */
#else
	     gc_value.font = (Font) xv_get(std_font, XV_XID);
	     gc_value_mask |= GCFont;
#endif	OW_I18N
	    break;
	  case OPENWIN_BOLD_GC:
#ifdef OW_I18N
	    /* do nothing since using font sets always */
#else
	    bold_font = (Pixfont *) xv_find(pw, FONT,
		FONT_FAMILY, xv_get(std_font, FONT_FAMILY),
		FONT_STYLE, FONT_STYLE_BOLD,
		FONT_SIZE, xv_get(std_font, FONT_SIZE),
		0);
	    if (bold_font == NULL) {
		xv_error(NULL,
		    ERROR_STRING,
		        "Unable to find bold font; using standard font",
		    ERROR_PKG, OPENWIN,
		    0);
		bold_font = std_font;
	    }
	    gc_value.font = (Font) xv_get(bold_font, XV_XID);
	    gc_value_mask |= GCFont;
#endif	OW_I18N
	    break;
	  case OPENWIN_GLYPH_GC:
	    gc_value.font = (Font) xv_get(xv_get(pw, WIN_GLYPH_FONT), XV_XID);
	    gc_value_mask |= GCFont;
	    break;
	  case OPENWIN_INACTIVE_GC:
	    gc_value.foreground = xv_bg(info);
	    gc_value.background = xv_fg(info);
	    gc_value.stipple = XCreateBitmapFromData(display, xid,
		openwin_gray50_bitmap, 16, 16);
	    gc_value.fill_style = FillStippled;
	    gc_value_mask |= GCStipple | GCFillStyle;
	    break;
	  case OPENWIN_DIM_GC:
	    gc_value.line_style = LineDoubleDash;
	    gc_value.dashes = 1;
	    gc_value_mask |= GCLineStyle | GCDashList;
	    break;
	  case OPENWIN_INVERT_GC:
	    gc_value.function = GXinvert;
	    gc_value.plane_mask =
		gc_value.foreground ^ gc_value.background;
	    break;
	}
	openwin_gc_list[i] = XCreateGC(display, xid, gc_value_mask, &gc_value);
    }
}

Xv_private void
openwin_check_gc_color(info, gc_index)
    Xv_Drawable_info *info;
    int		gc_index;
{
    register unsigned long new_fg;
    register unsigned long new_bg;
    register unsigned long new_plane_mask = xv_plane_mask(info);
    XGCValues	    gc_values;
    Xv_Screen       screen;
    GC              *openwin_gc_list;
    
    screen = xv_screen(info);
    openwin_gc_list = (GC *) xv_get( screen, XV_KEY_DATA, OW_GC_KEY );
    switch (gc_index) {
      case OPENWIN_SET_GC:
      case OPENWIN_NONSTD_GC:
      case OPENWIN_TEXT_GC:
      case OPENWIN_BOLD_GC:
      case OPENWIN_GLYPH_GC:
      case OPENWIN_DIM_GC:
	new_fg = xv_fg(info);
	new_bg = xv_bg(info);
	break;
      case OPENWIN_INVERT_GC:
	new_fg = xv_fg(info);
	new_bg = xv_bg(info);
	new_plane_mask = new_fg ^ new_bg;
	break;
      case OPENWIN_CLR_GC:
	new_fg = new_bg = xv_bg(info);
	break;
      case OPENWIN_INACTIVE_GC:
	new_fg = xv_bg(info);
	new_bg = xv_fg(info);
	break;
    }
    if (new_fg != openwin_gc_list[gc_index]->values.foreground ||
	new_bg != openwin_gc_list[gc_index]->values.background ||
	new_plane_mask != openwin_gc_list[gc_index]->values.plane_mask) {
	gc_values.foreground = new_fg;
	gc_values.background = new_bg;
	gc_values.plane_mask = new_plane_mask;
	XChangeGC(xv_display(info), openwin_gc_list[gc_index],
		  GCForeground | GCBackground | GCPlaneMask, &gc_values);
    }
}


