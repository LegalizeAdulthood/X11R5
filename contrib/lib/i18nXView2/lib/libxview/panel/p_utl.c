#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_utl.c 70.2 91/07/08";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/panel_impl.h>
#include <xview/cms.h>
#include <xview/defaults.h>
#include <xview/sun.h>
#include <xview/openmenu.h>
#include <xview/pixwin.h>
#include <xview/font.h>
#include <xview/xv_xrect.h>
#include <xview_private/draw_impl.h>
#include <xview_private/pw_impl.h>
#include <X11/X.h>
#ifdef	OW_I18N
#include <xview/server.h>
#include <xview/xv_i18n.h>
#include <widec.h>
#include <xview_private/xv_i18n_impl.h>
#endif	OW_I18N

/* caret handling functions */
extern void     (*panel_caret_invert_proc) ();
extern void     (*panel_seln_hilite_proc) ();

/* selection service functions */
extern void     (*panel_seln_inform_proc) ();
extern void     (*panel_seln_destroy_proc) ();

extern struct pr_size xv_pf_textwidth();
#ifdef	OW_I18N
extern struct pr_size	xv_pf_textwidth_wc();
extern wchar_t 		*panel_strsave_wc();
extern wchar_t		null_string_wc[];
#endif	OW_I18N
Xv_private void	openwin_create_gcs();
Xv_private Xv_xrectlist *screen_get_clip_rects();
Xv_private GC	xv_find_proper_gc();
Xv_private void	xv_set_gc_op();

extern int OW_GC_KEY;

/*****************************************************************************/
/* panel_enclosing_rect                                                      */
/*****************************************************************************/

Pkg_private     Rect
panel_enclosing_rect(r1, r2)
    register Rect  *r1, *r2;
{
    /* if r2 is undefined then return r1 */
    if (r2->r_left == -1)
	return (*r1);

    return rect_bounding(r1, r2);
}

/*****************************************************************************/
/* panel_update_extent                                                       */
/* called from panel_attr.c                                                  */
/*****************************************************************************/

Pkg_private void
panel_update_extent(panel, rect)
    Panel_info     *panel;
    Rect            rect;
{
    int             v_end, h_end;

    v_end = panel_height(panel);
    if (rect.r_top + rect.r_height > v_end) {
	v_end = rect.r_top + rect.r_height + panel->extra_height;
	(void) xv_set(PANEL_PUBLIC(panel),
		      CANVAS_MIN_PAINT_HEIGHT, v_end, 0);
    }
    h_end = panel_width(panel);
    if (rect.r_left + rect.r_width > h_end) {
	h_end = rect.r_left + rect.r_width + panel->extra_width;
	(void) xv_set(PANEL_PUBLIC(panel),
		      CANVAS_MIN_PAINT_WIDTH, h_end, 0);
    }
}


/****************************************************************************/
/* font char/pixel conversion routines                                      */
/****************************************************************************/

Pkg_private
panel_col_to_x(font, col)
    Pixfont        *font;
    int             col;
{
    int		chrwth = xv_get(font, FONT_DEFAULT_CHAR_WIDTH);
    return (col * chrwth);
}


Pkg_private
panel_row_to_y(font, line)
    Pixfont        *font;
    int             line;
{
    int		chrht = xv_get(font, FONT_DEFAULT_CHAR_HEIGHT);
    return (line * chrht);
}


Pkg_private
panel_x_to_col(font, x)
    Pixfont        *font;
    int             x;
{
    int		chrwth = xv_get(font, FONT_DEFAULT_CHAR_WIDTH);
    return (x / chrwth);
}


Pkg_private
panel_y_to_row(font, y)
    Pixfont        *font;
    int             y;
{
    int		chrht = xv_get(font, FONT_DEFAULT_CHAR_HEIGHT);
    return (y / chrht);
}


/*****************************************************************************
 * panel_make_image                                                          *
 ****************************************************************************/
Pkg_private struct pr_size
panel_make_image(font, dest, type_code, value, bold_desired,
		 inverted_desired)
    Pixfont        *font;
    Panel_image    *dest;
    int             type_code;
    Xv_opaque       value;
    int             bold_desired;
    int             inverted_desired;
{
    struct pr_size  size;
#ifdef	OW_I18N
    wchar_t        *str_wc;
    wchar_t	   *value_str_wc;
#else
    char           *str;
    char	   *value_str;
#endif	OW_I18N

    size.x = size.y = 0;
    dest->im_type = type_code;
    image_set_inverted(dest, inverted_desired);
    switch (type_code) {
      case PIT_STRING:
#ifdef	OW_I18N
	if (value)
	    value_str_wc = (wchar_t *) value;
	else {
	    value_str_wc = null_string_wc;
	}
	if (!(str_wc = (wchar_t *) panel_strsave_wc((wchar_t *) value_str_wc)))
	    return (size);
	if (image_string_wc(dest))
	    free((char *)image_string_wc(dest));
	image_set_string_wc(dest, str_wc);
	image_set_font(dest, font);
	image_set_bold(dest, bold_desired);
	size = xv_pf_textwidth_wc(wslen(str_wc), font, str_wc);
#else
	if (value)
	    value_str = (char *) value;
	else
	    value_str = "";
	if (!(str = (char *) panel_strsave((u_char *) value_str)))
	    return (size);
	if (image_string(dest))
	    free(image_string(dest));
	image_set_string(dest, str);
	image_set_font(dest, font);
	image_set_bold(dest, bold_desired);
	size = xv_pf_textwidth(strlen(str), font, str);
#endif	OW_I18N
	/* Use max height of font * (ascent + descent) */
	size.y = xv_get(font, FONT_DEFAULT_CHAR_HEIGHT);	
	if (bold_desired)
	    size.x += 1;
	break;

      case PIT_SVRIM:
	if (!value || PR_NOT_SERVER_IMAGE(value))
	    xv_error(value,
#ifdef	OW_I18N
		     ERROR_STRING, XV_I18N_MSG("xv_messages",
					    "Invalid Server Image specified"),
#else
		     ERROR_STRING, "Invalid Server Image specified",
#endif	OW_I18N
		     ERROR_PKG, PANEL,
		     0);
	image_set_svrim(dest, (Server_image) value);
	size = ((Pixrect *) value)->pr_size;
	break;
    }
    return size;
}

Pkg_private void
panel_image_set_font(image, font)
    Panel_image    *image;
    Pixfont        *font;
{
    if (image_font(image)) {
        (void) xv_set(image_font(image), XV_DECREMENT_REF_COUNT, 0);
    }
    image_font(image) = font;
    if (font) {
        (void) xv_set(font, XV_INCREMENT_REF_COUNT, 0);
    }
}
 
/*****************************************************************************/
/* panel_successor -- returns the next unhidden item after ip.               */
/*****************************************************************************/

Pkg_private Item_info *
panel_successor(ip)
    register Item_info *ip;
{
    if (!ip)
	return NULL;

    for (ip = ip->next; ip && hidden(ip); ip = ip->next);

    return ip;
}

/*****************************************************************************/
/* panel_append                                                              */
/*****************************************************************************/

Pkg_private void
panel_append(ip)
    register Item_info *ip;
{
    Panel_info     *panel = ip->panel;
    register Item_info *ip_cursor;

    if (!panel->items) {
	panel->items = ip;
	ip->previous = NULL;
    } else {
	for (ip_cursor = panel->items;
	     ip_cursor->next != NULL;
	     ip_cursor = ip_cursor->next);
	ip_cursor->next = ip;
	ip->previous = ip_cursor;
    }
    ip->next = NULL;

    /* item rect encloses the label and value */
    ip->rect = panel_enclosing_rect(&ip->label_rect, &ip->value_rect);
}


/*****************************************************************************/
/* panel_unlink                                                              */
/*****************************************************************************/

Pkg_private void
panel_unlink(ip, destroy)
    register Item_info *ip;
    int             destroy;	/* boolean: unlink is part of a destroy
				 * operation */
{
    Panel_info     *panel = ip->panel;
    register Item_info *prev_ip = ip->previous;

    /* unlinked item is no longer current */
    if (panel->current == ip)
	panel->current = NULL;

    /* we assume that the caret is off by the time we are called */

    if (!destroy)
	/* remove from any private list */
	(*ip->ops->remove) (ip);

    /* unlink ip */
    if (prev_ip)
	prev_ip->next = ip->next;
    else
	panel->items = ip->next;
    if (ip->next)
	ip->next->previous = prev_ip;

    /* NULL out parent pointer */
    ip->panel = NULL;

    /* update the default position of the next created item */
    (void) panel_find_default_xy(panel);
}



/****************************************************************************/
/* panel_find_default_xy                                                    */
/* computes panel->item_x, panel->item_y, and panel->max_item_y based on    */
/* the geometry of the current items in the panel.                          */
/* First the lowest "row" is found, then the default position is on that    */
/* row to the right of any items which intersect that row.                  */
/* The max_item_y is set to the height of the lowest item rectangle on the  */
/* lowest row.                                                              */
/****************************************************************************/

Pkg_private
panel_find_default_xy(panel)
    Panel_info     *panel;
{
    register Item_info *ip;
    register int    lowest_top = PANEL_ITEM_Y_START;
    register int    rightmost_right = PANEL_ITEM_X_START;
    register int    lowest_bottom = PANEL_ITEM_Y_START;

    if (!panel->items) {
	panel->max_item_y = 0;
	panel->item_x = PANEL_ITEM_X_START;
	panel->item_y = PANEL_ITEM_Y_START;
	return;
    }
    /*
     * Horizontal layout: find the lowest row of any item Vertical layout:
     * find the lowest row in the current column
     */
    for (ip = panel->items; ip; ip = ip->next) {
	if (panel->layout == PANEL_VERTICAL) {
	    if (ip->rect.r_left >= panel->current_col_x)
		lowest_bottom = MAX(lowest_bottom, rect_bottom(&ip->rect));
	} else {
	    lowest_top = MAX(lowest_top, ip->rect.r_top);
	    lowest_bottom = MAX(lowest_bottom, rect_bottom(&ip->rect));
	}
    }

    /*
     * Horizontal layout: find the rightmost position on the lowest row
     * Vertical layout: find the rightmost position of any item
     */
    for (ip = panel->items; ip; ip = ip->next)
	if (panel->layout == PANEL_VERTICAL ||
	    rect_bottom(&ip->rect) >= lowest_top)
	    rightmost_right = MAX(rightmost_right, rect_right(&ip->rect));

    /* update the panel info */
    panel->max_item_y = lowest_bottom - lowest_top; /* offset to next row */
    panel->item_x = rightmost_right + panel->item_x_offset;
    panel->item_y = lowest_top;
    panel->lowest_bottom = lowest_bottom;
    panel->rightmost_right = rightmost_right;

    /* advance to the next row if vertical layout or past right edge */
    if (panel->layout == PANEL_VERTICAL ||
	panel->item_x > panel_viewable_width(panel, panel->paint_window->pw)) {
	panel->item_x = panel->current_col_x;
	panel->item_y = lowest_bottom + panel->item_y_offset;
	panel->max_item_y = 0;
    }
}


/****************************************************************************/
/* panel_item_layout                                                        */
/* lays out the generic item, label & value rects in ip and calls the       */
/* item's layout proc.                                                      */
/****************************************************************************/

Pkg_private void
panel_item_layout(ip, deltas)
    register Item_info *ip;
    register Rect  *deltas;
{
    /* item rect */
    ip->rect.r_left += deltas->r_left;
    ip->rect.r_top += deltas->r_top;

    /* label rect */
    ip->label_rect.r_left += deltas->r_left;
    ip->label_rect.r_top += deltas->r_top;

    /* value rect */
    ip->value_rect.r_left += deltas->r_left;
    ip->value_rect.r_top += deltas->r_top;

    /* item */
    (*ip->ops->layout) (ip, deltas);
}


Pkg_private void
panel_check_item_layout(ip)
    Item_info *ip;
{
    Rect	    deltas;
    Rect	   *view_rect;

    /* Move the item to the start of the next row if its position is not fixed,
     * it doesn't start at the left margin, and it extends past the right edge
     * of the panel's viewable rectangle.
     */
    if (!(item_fixed(ip) || label_fixed(ip) || value_fixed(ip)) &&
	ip->rect.r_left > PANEL_ITEM_X_START)  {
	view_rect = panel_viewable_rect(ip->panel, ip->panel->paint_window->pw);
	if (rect_right(&ip->rect) > rect_right(view_rect)) {
	    deltas.r_left = PANEL_ITEM_X_START - ip->rect.r_left;
	    deltas.r_top = ip->panel->max_item_y + ip->panel->item_y_offset;
	    /* tell the item to move */
	    panel_item_layout(ip, &deltas);
	}
    }
}


/****************************************************************************/
/* panel_paint_image                                                        */
/* paints image in pw in rect.                                              */
/****************************************************************************/
Pkg_private
panel_paint_image(panel, image, rect, inactive_item, color_index)
    Panel_info     *panel;
    Panel_image    *image;
    Rect           *rect;
    int    	    inactive_item;
    int             color_index;
{
    int             top, chrht;
    Xv_Window       pw;
    Xv_Drawable_info *info;
    Xv_Screen      screen;
    GC             *openwin_gc_list;

    chrht = xv_get(image_font(image), FONT_DEFAULT_CHAR_HEIGHT);
    PANEL_EACH_PAINT_WINDOW(panel, pw)
	switch (image->im_type) {
      case PIT_STRING:
	/* baseline offset */
	top = rect->r_top + panel_fonthome(image_font(image)) +
	    (rect->r_height - chrht) / 2;
#ifdef	OW_I18N
	panel_paint_text(pw, 
			 image_bold(image) ? panel->bold_font_xid : 
			 panel->std_font_xid, color_index,
			 rect->r_left, top, image_string_wc(image));
#else
	panel_paint_text(pw, 
			 image_bold(image) ? panel->bold_font_xid : 
			 panel->std_font_xid, color_index,
			 rect->r_left, top, image_string(image));
#endif	OW_I18N
	break;

      case PIT_SVRIM:
	panel_paint_svrim(pw, (Pixrect *) image_svrim(image), rect->r_left,
			  rect->r_top, color_index);
	break;
    }

    if (image_boxed(image)) {
	if (color_index >= 0) {
	    xv_draw_rectangle(pw, rect->r_left, rect->r_top,
			      rect->r_width - 1, rect->r_height - 1,
			      LineSolid, PIX_SRC | PIX_COLOR(color_index));
	} else {
	    DRAWABLE_INFO_MACRO(pw, info);
	    screen = xv_screen(info);
	    openwin_gc_list = (GC *) xv_get( screen, XV_KEY_DATA, OW_GC_KEY );
	    openwin_check_gc_color(info, OPENWIN_SET_GC);
	    XDrawRectangle(xv_display(info), xv_xid(info),
			   openwin_gc_list[OPENWIN_SET_GC],
			   rect->r_left, rect->r_top,
			   rect->r_width - 1, rect->r_height - 1);
	}
    }

    if (image_inverted(image))
	panel_pw_invert(pw, rect, color_index);

    if (inactive_item) {
	DRAWABLE_INFO_MACRO(pw, info);
	screen = xv_screen(info);
	openwin_gc_list = (GC *) xv_get( screen, XV_KEY_DATA, OW_GC_KEY );
	openwin_check_gc_color(info, OPENWIN_INACTIVE_GC);
	XFillRectangle(xv_display(info), xv_xid(info),
		       openwin_gc_list[OPENWIN_INACTIVE_GC],
		       rect->r_left, rect->r_top,
		       rect->r_width, rect->r_height);
    }
    PANEL_END_EACH_PAINT_WINDOW
}


/****************************************************************************/
/* panel_invert                                                             */
/* inverts the rect r using panel's pixwin.                                 */
/****************************************************************************/
Pkg_private void
panel_invert(panel, r, color_index)
    Panel_info     *panel;
    register Rect  *r;
    int		    color_index;
{
    Xv_Window       pw;

    PANEL_EACH_PAINT_WINDOW(panel, pw)
	panel_pw_invert(pw, r, color_index);
    PANEL_END_EACH_PAINT_WINDOW
}

Pkg_private void
panel_pw_invert(pw, rect, color_index)
    Xv_Window pw;
    register Rect *rect;
    int color_index;
{
    XGCValues	gc_value;
    Xv_Drawable_info *info;
    Xv_Screen      screen;
    GC             *openwin_gc_list;

    DRAWABLE_INFO_MACRO(pw, info);
    screen = xv_screen(info);
    openwin_gc_list = (GC *) xv_get( screen, XV_KEY_DATA, OW_GC_KEY );
    if (color_index >= 0)
	gc_value.foreground = xv_get(xv_cms(info), CMS_PIXEL, color_index);
    else
	gc_value.foreground = xv_fg(info);
    gc_value.background = xv_bg(info);
    gc_value.plane_mask = gc_value.foreground ^ gc_value.background;
    XChangeGC(xv_display(info), openwin_gc_list[OPENWIN_INVERT_GC],
	      GCForeground | GCBackground | GCPlaneMask, &gc_value);
    XFillRectangle(xv_display(info), xv_xid(info),
		   openwin_gc_list[OPENWIN_INVERT_GC],
		   rect->r_left, rect->r_top,
		   rect->r_width, rect->r_height);
}


/****************************************************************************/
/* panel_strsave                                                            */
/****************************************************************************/

Pkg_private u_char *
panel_strsave(source)
    u_char         *source;
{
    u_char         *dest;

    dest = (u_char *) malloc(strlen(source) + 1);
    if (!dest)
	return NULL;

    (void) strcpy(dest, source);
    return dest;
}

#ifdef	OW_I18N

/****************************************************************************/
/* panel_strsave_wc							    */
/****************************************************************************/

Pkg_private wchar_t *
panel_strsave_wc(source)
    wchar_t        *source;
{
    wchar_t        *dest;

    dest = (wchar_t *) malloc((wslen(source) + 1) * sizeof(wchar_t));
    if (!dest)
	return ((wchar_t *)NULL);

    (void) wscpy(dest, source);
    return dest;
}

#endif	OW_I18N


/****************************************************************************/
/* miscellaneous utilities                                                  */
/****************************************************************************/

/*
 * Return max baseline offset for specified string.  This should be the same
 * value returned by XTextExtents in overall_return.descent.
 */
#ifdef OW_I18N
Pkg_private int
panel_fonthome(font)
    Pixfont        *font;
{
    register int    max_home = 0;
    XFontSet	    font_set;
    XFontSetExtents	*font_set_extents;
    int		    pc_home_y;

    font_set = (XFontSet)xv_get(font, FONT_SET_ID);
    font_set_extents = XExtentsOfFontSet(font_set);
    pc_home_y = font_set_extents->max_ink_extent.y;

    
    if (pc_home_y < max_home)
        max_home = pc_home_y;
    return -(max_home);
}
#else
Pkg_private int
panel_fonthome(font)
    Pixfont        *font;
{
    register int    max_home = 0;
    XFontStruct		*x_font_info;
    int			pc_home_y;

    x_font_info = (XFontStruct *)xv_get(font, FONT_INFO);
    pc_home_y = -x_font_info->ascent;
    if (pc_home_y < max_home)
        max_home = pc_home_y;
    return -(max_home);
}
#endif /* OW_I18N */


/*VARARGS*/
panel_nullproc()
{
    return 0;
}


Pkg_private void
panel_caret_invert(panel)
    Panel_info     *panel;
{
    (*panel_caret_invert_proc) (panel);
}


Pkg_private void
panel_seln_hilite(selection)
    Panel_selection *selection;
{
    (*panel_seln_hilite_proc) (selection);
}


Pkg_private void
panel_seln_inform(panel, event)
    Panel_info     *panel;
    Event          *event;
{
    (*panel_seln_inform_proc) (panel, event);
}


Pkg_private void
panel_seln_destroy(panel)
    Panel_info     *panel;
{
    (*panel_seln_destroy_proc) (panel);
}


Pkg_private
panel_free_choices(choices, first, last)
    Panel_image    *choices;
    int             first, last;
{
    register int    i;		/* counter */

    if (!choices || last < 0)
	return;

    /* free the choice strings */
    for (i = first; i <= last; i++) {
	panel_free_image(&choices[i]);
    }

    free((char *) choices);
}				/* panel_free_choices */

Pkg_private void
panel_free_image(image)
    Panel_image    *image;
{
    if (is_string(image)) {
#ifdef	OW_I18N
	if (image_string(image))
		free(image_string(image));
	if (image_string_wc(image))
		free(image_string_wc(image));
#else
	free(image_string(image));
#endif	OW_I18N
    }
}

Pkg_private void
panel_set_bold_label_font(ip)
    register Item_info *ip;
{
    image_set_font(&ip->label, ip->panel->bold_font);
    image_set_bold(&ip->label, TRUE);
}


Pkg_private void
#ifdef OW_I18N
panel_paint_text(pw, font_xid, color_index, x, y, str_wc)
#else
panel_paint_text(pw, font_xid, color_index, x, y, str)
#endif OW_I18N
    Xv_opaque	pw;	/* paint window */
    Font	font_xid;
    int		color_index;
    int		x, y;	/* baseline starting position */
#ifdef OW_I18N
    wchar_t    *str_wc;	/* text to paint */
#else
    u_char     *str;	/* text to paint */
#endif OW_I18N

{
    Display	*display;
    XGCValues	gc_value;
    Xv_Drawable_info *info;
    Drawable	xid;
    Xv_Screen      screen;
    GC             *openwin_gc_list;
#ifdef OW_I18N
    u_char     	   *str;	/* text to paint */
#endif

    DRAWABLE_INFO_MACRO(pw, info);
    display = xv_display(info);
    xid = xv_xid(info);

    screen = xv_screen(info);
    openwin_gc_list = (GC *) xv_get( screen, XV_KEY_DATA, OW_GC_KEY );
    if (openwin_gc_list[OPENWIN_NONSTD_GC] == NULL)
	openwin_create_gcs(pw);
    if (color_index >= 0)
	gc_value.foreground = xv_get(xv_cms(info), CMS_PIXEL, color_index);
    else
	gc_value.foreground = xv_fg(info);
    gc_value.background = xv_bg(info);
    gc_value.function = GXcopy;
    gc_value.plane_mask = xv_plane_mask(info);
    gc_value.fill_style = FillSolid;

#ifdef	OW_I18N
    XChangeGC(display, openwin_gc_list[OPENWIN_NONSTD_GC],
	GCForeground | GCBackground | GCFunction | GCPlaneMask |
	GCFillStyle, &gc_value);
    XwcDrawString(display, xid, font_xid, openwin_gc_list[OPENWIN_NONSTD_GC], 
	      x, y, str_wc, wslen(str_wc));
#else
    gc_value.font = font_xid;
    XChangeGC(display, openwin_gc_list[OPENWIN_NONSTD_GC],
	      GCForeground | GCBackground | GCFunction | GCPlaneMask |
	      GCFillStyle | GCFont, &gc_value);
    XDrawString(display, xid, openwin_gc_list[OPENWIN_NONSTD_GC], x, y, str,
		strlen(str));
#endif	OW_I18N
}


Pkg_private void
panel_paint_svrim(pw, pr, x, y, color_index)
    Xv_Window pw;
    Pixrect *pr;	/* a Pixrect or a Server Image */
    int x;
    int y;
    int color_index;
{
    Display	   *display;
    XGCValues	gc_value;
    Xv_Drawable_info *info;
    XID		    xid;
    Xv_Screen      screen;
    GC             *openwin_gc_list;

    DRAWABLE_INFO_MACRO(pw, info);
    display = xv_display(info);
    xid = xv_xid(info);
    screen = xv_screen(info);
    openwin_gc_list = (GC *) xv_get( screen, XV_KEY_DATA, OW_GC_KEY );
    if (openwin_gc_list[OPENWIN_NONSTD_GC] == NULL)
	openwin_create_gcs(pw);
    if (color_index >= 0)
	gc_value.foreground = xv_get(xv_cms(info), CMS_PIXEL, color_index);
    else
	gc_value.foreground = xv_fg(info);
    gc_value.background = xv_bg(info);
    XChangeGC(display, openwin_gc_list[OPENWIN_NONSTD_GC],
	      GCForeground | GCBackground, &gc_value);
    xv_rop_internal(display, xid, openwin_gc_list[OPENWIN_NONSTD_GC],
		    x, y, pr->pr_width, pr->pr_height,
		    (Xv_opaque) pr, 0, 0, info);
}


/* Find the next item that wants keystrokes.  If no other item wants
 * keystrokes, then return NULL.
 */
Pkg_private Item_info *
panel_next_kbd_focus(panel)
    Panel_info	*panel;
{
    Item_info	*ip;

    ip = panel->kbd_focus_item;
    do {
	ip = ip->next;
	if (!ip)
	    ip = panel->items;  /* wrap around to the top */
	if (ip == panel->kbd_focus_item)
	    return NULL;	/* no other item wants keystrokes */
    } while (!wants_key(ip) || hidden(ip) || inactive(ip));
    return(ip);
}


/* Find the previous item that wants keystrokes.  If no other item wants
 * keystrokes, then return NULL.
 */
Pkg_private Item_info *
panel_previous_kbd_focus(panel)
    Panel_info	*panel;
{
    Item_info	*ip;

    ip = panel->kbd_focus_item;
    do {
	ip = ip->previous;
	if (!ip) {
	    /* wrap around to the bottom */
	    for (ip=panel->items; ip->next; ip=ip->next);
	}
	if (ip == panel->kbd_focus_item)
	    return NULL;	/* no other item wants keystrokes */
    } while (!wants_key(ip) || hidden(ip) || inactive(ip));
    return(ip);
}


Pkg_private void
panel_accept_kbd_focus(panel)
    Panel_info     *panel;
{
    register Item_info *ip = panel->kbd_focus_item;

    if (ip)
	(*ip->ops->accept_kbd_focus) (ip);
}


Pkg_private void
panel_yield_kbd_focus(panel)
    Panel_info     *panel;
{
    register Item_info *ip = panel->kbd_focus_item;

    if (ip)
	(*ip->ops->yield_kbd_focus) (ip);
}


Pkg_private void
panel_clear_pw_rect(pw, rect)
    Xv_window	pw;
    Rect	rect;
{
    Display	   *display;
    Xv_Drawable_info *info;
    Drawable	    xid;

    DRAWABLE_INFO_MACRO(pw, info);
    display = xv_display(info);
    xid = xv_xid(info);
    XClearArea(display, xid, rect.r_left, rect.r_top,
	       rect.r_width, rect.r_height, False);
}


Pkg_private void
panel_clear_rect(panel, rect)
    Panel_info	*panel;
    Rect	rect;
{
    Xv_window	    pw;

    PANEL_EACH_PAINT_WINDOW(panel, pw)
	panel_clear_pw_rect(pw, rect);
    PANEL_END_EACH_PAINT_WINDOW
}


Pkg_private Rect *
panel_viewable_rect(panel, pw)
    Panel_info *panel;
    Xv_Window pw;
{
    static Rect viewable_rect;

    if (panel->paint_window->view)
	return ((Rect *) xv_get(panel->public_self, CANVAS_VIEWABLE_RECT, pw));
    else {
	viewable_rect = *(Rect *) xv_get(PANEL_PUBLIC(panel), XV_RECT);
	viewable_rect.r_left = 0;
	viewable_rect.r_top = 0;
	return (&viewable_rect);
    }
}


Pkg_private int
panel_viewable_height(panel, pw)
    Panel_info *panel;
    Xv_Window pw;
{
    if (panel->paint_window->view)
	return (((Rect *) xv_get((panel)->public_self, CANVAS_VIEWABLE_RECT,
		(pw)))->r_height);
    else
	return ((int) xv_get(PANEL_PUBLIC(panel), XV_HEIGHT));
}


Pkg_private int
panel_viewable_width(panel, pw)
    Panel_info *panel;
    Xv_Window pw;
{
    if (panel->paint_window->view)
	return (((Rect *) xv_get((panel)->public_self, CANVAS_VIEWABLE_RECT,
		(pw)))->r_width);
    else
	return ((int) xv_get(PANEL_PUBLIC(panel), XV_WIDTH));
}


Pkg_private int
panel_height(panel)
    Panel_info *panel;
{
    if (panel->paint_window->view)
	return ((int) xv_get(PANEL_PUBLIC(panel), CANVAS_HEIGHT));
    else
	return ((int) xv_get(PANEL_PUBLIC(panel), XV_HEIGHT));
}


Pkg_private int
panel_width(panel)
    Panel_info *panel;
{
    if (panel->paint_window->view)
	return ((int) xv_get(PANEL_PUBLIC(panel), CANVAS_WIDTH));
    else
	return ((int) xv_get(PANEL_PUBLIC(panel), XV_WIDTH));
}
