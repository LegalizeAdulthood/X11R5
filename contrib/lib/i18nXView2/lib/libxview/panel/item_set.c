#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)item_set.c 50.17 90/12/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */


#include <xview_private/panel_impl.h>
#include <xview_private/draw_impl.h>
#include <xview/openmenu.h>
#ifdef	OW_I18N
#include <xview/server.h>
#include <xview/xv_i18n.h>
#endif	OW_I18N

Xv_private      panel_item_parent();

static void item_adjust_label_size();
static Xv_opaque item_set_generic();
static Xv_opaque item_set_ops();

extern struct pr_size xv_pf_textwidth();
#ifdef	OW_I18N
extern struct pr_size xv_pf_textwidth_wc();
#endif	OW_I18N


Pkg_private     Xv_opaque
item_set_avlist(item_public, avlist)
    Panel_item      item_public;
    Attr_avlist     avlist;
{
    register Item_info *ip = ITEM_PRIVATE(item_public);
    Panel_setting   saved_repaint = ip->repaint;

    /* If a client has called panel_item_parent this item may not
     * have a parent; do nothing in this case.
     */
    if (ip->panel == NULL) {
	return ((Xv_opaque) XV_ERROR);
    }

    /* set any generic attributes */
    (void) item_set_generic(ip, avlist);

    /* set the ops vector attributes */
    (void) item_set_ops(ip, avlist);

    if (panel_painted(ip->panel) && !no_redisplay_item(ip->panel))
	panel_redisplay_item(ip, ip->repaint);

    /*
     * restore the item's original repaint behavior.  This allows clients to
     * specify a 'one time' repaint policy for this call to panel_set() only.
     */
    ip->repaint = saved_repaint;

    return XV_OK;
}


static Xv_opaque
item_set_generic(ip, avlist)
    register Item_info *ip;
    register Attr_avlist avlist;
{
    register Attr_avlist attrs;
    register Panel_info *panel = ip->panel;
    register Panel_image *label = &ip->label;
#ifdef ATTR_CONSUME_BUGS_FIXED
    int		    consumed;
#endif
    Xv_Drawable_info *info;
    int             item_x, item_y;
    int             label_x, label_y;
    int             next_col_gap;
    int             next_row_gap;
    int             start_new_col = FALSE;
    int             start_new_row = FALSE;
    int             value_x, value_y;
    short           item_x_changed = FALSE;
    short           item_y_changed = FALSE;
    short           label_size_changed = FALSE;
    short	    label_width_changed = FALSE;
    short           label_x_changed = FALSE;
    short           label_y_changed = FALSE;
    short           value_x_changed = FALSE;
    short           value_y_changed = FALSE;
    short           layout_changed = FALSE;
    short           label_font_changed = FALSE;
    int             label_type = -1;	/* no new label yet */
    int		    label_boxed = image_boxed(label);
    int             label_inverted = image_inverted(label);
    struct pr_size  label_size;
    Xv_opaque       label_data;
    Rect            deltas;
    short           potential_new_rect = FALSE;
#ifdef	OW_I18N
    Xv_opaque	    server;
    XID		    db;
    wchar_t	    *temp_wstr = 0;
    int		    db_int;
    int		    db_count = 0;
#endif	OW_I18N

#ifdef OW_I18N
    server = XV_SERVER_FROM_WINDOW(XV_PUBLIC(panel));
    db = xv_get(server, SERVER_RESOURCE_DB);

    attrs = avlist;
    while (*attrs || db_count) {

	switch (attrs[0]) {

	  case XV_USE_DB:
           db_count++;
           break;

	  case XV_SHOW:
	    if ((int) attrs[1]) {
		if (hidden(ip)) {
		    ip->flags &= ~HIDDEN;
		    (*ip->ops->restore) (ip);
		    panel_update_extent(panel, ip->rect);
		}
	    } else if (!hidden(ip)) {
		panel_clear_item(ip);
		ip->flags |= HIDDEN;
		(*ip->ops->remove) (ip);
	    }
	    break;

	  case PANEL_ITEM_OWNER:
	    ip->owner = (Panel_item) attrs[1];
	    break;

	  /* PANEL_ITEM_X attr must still be supported in addition to XV_X
	   * and XV_Y because they can be used on the panel. When used
	   * on the panel PANEL_ITEM_X != XV_X on the panel.
	   */
	  case XV_X:
	  case PANEL_ITEM_X:
	    if (db_count) {
		item_x = (int)db_get_data(db, ip->instance_qlist,
				(attrs[0] == XV_X) ? "xv_x" : "panel_item_x",
				XV_INT, (int)attrs[1]);
	    } else {
	        item_x = (int) attrs[1];
	    }
	    ip->flags |= ITEM_X_FIXED;
	    item_x_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case XV_Y:
	  case PANEL_ITEM_Y:
	    if (db_count) {
		item_y = (int)db_get_data(db, ip->instance_qlist, 
				(attrs[0] == XV_Y) ? "xv_y" : "panel_item_y",
				XV_INT, (int)attrs[1]);
	    } else {
	        item_y = (int) attrs[1];
	    }
	    ip->flags |= ITEM_Y_FIXED;
	    item_y_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_NEXT_COL:
	    if (db_count) {
		next_col_gap = (int)db_get_data(db, ip->instance_qlist, 
				 "panel_next_col", XV_INT, 
				 (int)attrs[1]);
	    } else
		next_col_gap = (int) attrs[1];
	    if (next_col_gap == 0)
		next_col_gap = 1;
	    start_new_col = TRUE;
	    break;

	  case PANEL_NEXT_ROW:
	    if (db_count) {
		next_row_gap = (int)db_get_data(db, ip->instance_qlist, 
				 "panel_next_row", XV_INT, 
				 (int)attrs[1]);
	    } else
		next_row_gap = (int) attrs[1];
	    if (next_row_gap == 0)
		next_row_gap = 1;
	    start_new_row = TRUE;
	    break;

	  case PANEL_LABEL_X:
	    if (db_count) {
		label_x = (int)db_get_data(db, ip->instance_qlist,
					"panel_label_x", XV_INT, 
					(int) attrs[1]);
	    } else {
		label_x = (int) attrs[1];
	    }
	    ip->flags |= LABEL_X_FIXED;
	    label_x_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_LABEL_Y:
	    if (db_count) {
		label_y = (int)db_get_data(db, ip->instance_qlist, 
					"panel_label_y", XV_INT, 
					(int) attrs[1]);
	    } else {
		label_y = (int) attrs[1];
	    }
	    ip->flags |= LABEL_Y_FIXED;
	    label_y_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_VALUE_X:
	    if (db_count) {
		value_x = (int)db_get_data(db, ip->instance_qlist,
				    	"panel_value_x", XV_INT, 
					(int) attrs[1]);
	    } else {
		value_x = (int) attrs[1];
	    }
	    ip->flags |= VALUE_X_FIXED;
	    value_x_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_VALUE_Y:
	    if (db_count) {
		value_y = (int)db_get_data(db, ip->instance_qlist, 
				    	"panel_value_y", XV_INT, 
					(int) attrs[1]);
	    } else {
		value_y = (int) attrs[1];
	    }
	    ip->flags |= VALUE_Y_FIXED;
	    value_y_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_LABEL_IMAGE:
	    label_type = PIT_SVRIM;
	    label_data = attrs[1];
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_LABEL_STRING:
	    label_type = PIT_STRING;
	    /* Panel will only process wchar_t strings. Make temp space for 
	     * this string and free it at the end of this routine.
 	     * panel_make_image will do the real malloc for this string.
	     * Note: Can optimize this code for performance.
	     */
	    temp_wstr = mbstowcsdup((char *)attrs[1]);
	    label_data = (Xv_opaque) temp_wstr;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_LABEL_STRING_WCS:
	    label_type = PIT_STRING;
	    label_data = attrs[1];
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_LABEL_FONT:
	    /* Sunview1 attribute: ignored for compatibility */
	    break;

	  case PANEL_LABEL_BOLD:
	    if (ip->item_type == PANEL_MESSAGE_ITEM) {
		if (attrs[1] != image_bold(&ip->label)) {
		    if (attrs[1]) {
			image_set_font(&ip->label, ip->panel->bold_font);
			image_set_bold(&ip->label, TRUE);
		    } else {
			image_set_font(&ip->label, ip->panel->std_font);
			image_set_bold(&ip->label, FALSE);
		    }
		    label_font_changed = TRUE;
		    potential_new_rect = TRUE;
		}
	    }
	    break;

	  case PANEL_LABEL_BOXED:
	    label_boxed = (int) attrs[1];
	    image_set_boxed(label, label_boxed);
	    break;

	  case PANEL_LABEL_INVERTED:
	    label_inverted = (int) attrs[1];
	    image_set_inverted(label, label_inverted);
	    break;

	  case PANEL_LABEL_WIDTH:
	    if (db_count) {
	    	if (ip->label_width !=
		    (db_int = (int)db_get_data(db, ip->instance_qlist,
					"panel_label_width", XV_INT,
					(int) attrs[1]))) {
			ip->label_width = db_int;
			label_width_changed = TRUE;
		}
	    }
	    else if (ip->label_width != (int) attrs[1]) {
		ip->label_width = (int) attrs[1];
		label_width_changed = TRUE;
	    }
	    break;

	  case PANEL_LAYOUT:
	    switch ((Panel_setting) attrs[1]) {
	      case PANEL_HORIZONTAL:
	      case PANEL_VERTICAL:
		ip->layout = (Panel_setting) attrs[1];
		layout_changed = TRUE;
		potential_new_rect = TRUE;
		break;

	      default:
		/* invalid layout */
		break;
	    }
	    break;

	  case PANEL_MENU_ITEM:
	    if (attrs[1])
		ip->flags |= IS_MENU_ITEM;
	    else
		ip->flags &= ~IS_MENU_ITEM;
	    break;

	  case PANEL_NOTIFY_PROC:
	    ip->notify = (int (*) ()) attrs[1];
	    ip->notify_wc = panel_nullproc;
	    if (!ip->notify)
		ip->notify = panel_nullproc;
	    break;

	  case PANEL_NOTIFY_PROC_WCS:
	    ip->notify_wc = (int (*) ()) attrs[1];
	    ip->notify = panel_nullproc;
	    if (!ip->notify_wc)
		ip->notify_wc = panel_nullproc;
	    break;

	  case PANEL_NOTIFY_STATUS:
	    ip->notify_status = (int) attrs[1];
	    break;

	  case PANEL_PAINT:
	    ip->repaint = (Panel_setting) attrs[1];
	    break;

	  case PANEL_ACCEPT_KEYSTROKE:
	    if (attrs[1])
		ip->flags |= WANTS_KEY;
	    else
		ip->flags &= ~WANTS_KEY;
	    break;

	  case PANEL_ITEM_MENU:
	    if (ip->menu)
		xv_set(ip->menu, XV_DECREMENT_REF_COUNT, 0);
	    if (ip->menu = (Xv_opaque) attrs[1])
		xv_set(ip->menu,
		       XV_INCREMENT_REF_COUNT,
		       0);
	    if (label_type == -1) {
		label_type = label->im_type;	/* force label (image) to be
						 * remade */
		if (label_type == PIT_STRING)
		    label_data = (Xv_opaque) image_string_wc(label);
		else
		    label_data = (Xv_opaque) image_svrim(label);
	    }
	    break;

	  case PANEL_CLIENT_DATA:
	    ip->client_data = attrs[1];
	    break;

	  case PANEL_INACTIVE:
	    if (attrs[1])
		ip->flags |= INACTIVE;
	    else
		ip->flags &= ~INACTIVE;
	    break;

	  case PANEL_ITEM_COLOR:
	    DRAWABLE_INFO_MACRO(panel->paint_window->pw, info);
	    if (xv_depth(info) > 1)
		ip->color_index = (int) attrs[1];
	    break;

	  case XV_END_CREATE:
#ifdef ATTR_CONSUME_BUGS_FIXED
	    consumed = FALSE;
#endif

	    panel_check_item_layout(ip);
	    ip->flags |= CREATED;

	    /*
	     * for scrolling computations, note new extent of panel, tell
	     * scrollbars
	     */
	    if (!hidden(ip))
		panel_update_extent(ip->panel, ip->rect);
	    break;

	  default:

#ifdef ATTR_CONSUME_BUGS_FIXED
	    consumed = FALSE;
#endif
	    /* Note: xv_check_bad_attr is not called since item_set_generic
	     * may be called via xv_super_set_avlist in one of the
	     * panel item set routines.
	     */
	    break;
	}

	if (attrs[0] == XV_USE_DB)
	    attrs++;
	else if (db_count && !attrs[0]) {
	    db_count--;
	    attrs++;
	} else
	    attrs = attr_next(attrs);

#ifdef ATTR_CONSUME_BUGS_FIXED
	/* BUG ALERT:  Item set routines call xv_super_set_avlist,
	 * which calls this routine.  If this routine consumes attrs,
	 * then when xv_super_set_avlist returns to the item set routines,
	 * the switch (attrs[0]) which then gets executed will not see
	 * the attributes item_set_generic just consumed.
	 */
	if (consumed)
	    ATTR_CONSUME(attrs[0]);
#endif
    }

#else OW_I18N
     for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {

#ifdef ATTR_CONSUME_BUGS_FIXED
	consumed = TRUE;
#endif
	switch (attrs[0]) {
	  case XV_SHOW:
	    if ((int) attrs[1]) {
		if (hidden(ip)) {
		    ip->flags &= ~HIDDEN;
		    (*ip->ops->restore) (ip);
		    panel_update_extent(panel, ip->rect);
		}
	    } else if (!hidden(ip)) {
		panel_clear_item(ip);
		ip->flags |= HIDDEN;
		(*ip->ops->remove) (ip);
	    }
	    break;

	  case PANEL_ITEM_OWNER:
	    ip->owner = (Panel_item) attrs[1];
	    break;

	  /* PANEL_ITEM_X attr must still be supported in addition to XV_X
	   * and XV_Y because they can be used on the panel. When used
	   * on the panel PANEL_ITEM_X != XV_X on the panel.
	   */
	  case XV_X:
	  case PANEL_ITEM_X:
	    item_x = (int) attrs[1];
	    ip->flags |= ITEM_X_FIXED;
	    item_x_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case XV_Y:
	  case PANEL_ITEM_Y:
	    item_y = (int) attrs[1];
	    ip->flags |= ITEM_Y_FIXED;
	    item_y_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_NEXT_COL:
	    next_col_gap = (int) attrs[1];
	    if (next_col_gap == 0)
		next_col_gap = 1;
	    start_new_col = TRUE;
	    break;

	  case PANEL_NEXT_ROW:
	    next_row_gap = (int) attrs[1];
	    if (next_row_gap == 0)
		next_row_gap = 1;
	    start_new_row = TRUE;
	    break;

	  case PANEL_LABEL_X:
	    label_x = (int) attrs[1];
	    ip->flags |= LABEL_X_FIXED;
	    label_x_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_LABEL_Y:
	    label_y = (int) attrs[1];
	    ip->flags |= LABEL_Y_FIXED;
	    label_y_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_VALUE_X:
	    value_x = (int) attrs[1];
	    ip->flags |= VALUE_X_FIXED;
	    value_x_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_VALUE_Y:
	    value_y = (int) attrs[1];
	    ip->flags |= VALUE_Y_FIXED;
	    value_y_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_LABEL_IMAGE:
	    label_type = PIT_SVRIM;
	    label_data = attrs[1];
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_LABEL_STRING:
	    label_type = PIT_STRING;
	    label_data = attrs[1];
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_LABEL_FONT:
	    /* Sunview1 attribute: ignored for compatibility */
	    break;

	  case PANEL_LABEL_BOLD:
	    if (ip->item_type == PANEL_MESSAGE_ITEM) {
		if (attrs[1] != image_bold(&ip->label)) {
		    if (attrs[1]) {
			image_set_font(&ip->label, ip->panel->bold_font);
			image_set_bold(&ip->label, TRUE);
		    } else {
			image_set_font(&ip->label, ip->panel->std_font);
			image_set_bold(&ip->label, FALSE);
		    }
		    label_font_changed = TRUE;
		    potential_new_rect = TRUE;
		}
	    }
	    break;

	  case PANEL_LABEL_BOXED:
	    label_boxed = (int) attrs[1];
	    image_set_boxed(label, label_boxed);
	    break;

	  case PANEL_LABEL_INVERTED:
	    label_inverted = (int) attrs[1];
	    image_set_inverted(label, label_inverted);
	    break;

	  case PANEL_LABEL_WIDTH:
	    if (ip->label_width != (int) attrs[1]) {
		ip->label_width = (int) attrs[1];
		label_width_changed = TRUE;
	    }
	    break;

	  case PANEL_LAYOUT:
	    switch ((Panel_setting) attrs[1]) {
	      case PANEL_HORIZONTAL:
	      case PANEL_VERTICAL:
		ip->layout = (Panel_setting) attrs[1];
		layout_changed = TRUE;
		potential_new_rect = TRUE;
		break;

	      default:
		/* invalid layout */
		break;
	    }
	    break;

	  case PANEL_MENU_ITEM:
	    if (attrs[1])
		ip->flags |= IS_MENU_ITEM;
	    else
		ip->flags &= ~IS_MENU_ITEM;
	    break;

	  case PANEL_NOTIFY_PROC:
	    ip->notify = (int (*) ()) attrs[1];
	    if (!ip->notify)
		ip->notify = panel_nullproc;
	    break;

	  case PANEL_NOTIFY_STATUS:
	    ip->notify_status = (int) attrs[1];
	    break;

	  case PANEL_PAINT:
	    ip->repaint = (Panel_setting) attrs[1];
	    break;

	  case PANEL_ACCEPT_KEYSTROKE:
	    if (attrs[1])
		ip->flags |= WANTS_KEY;
	    else
		ip->flags &= ~WANTS_KEY;
	    break;

	  case PANEL_ITEM_MENU:
	    if (ip->menu)
		xv_set(ip->menu, XV_DECREMENT_REF_COUNT, 0);
	    if (ip->menu = (Xv_opaque) attrs[1])
		xv_set(ip->menu,
		       XV_INCREMENT_REF_COUNT,
		       0);
	    if (label_type == -1) {
		label_type = label->im_type;	/* force label (image) to be
						 * remade */
		if (label_type == PIT_STRING)
		    label_data = (Xv_opaque) image_string(label);
		else
		    label_data = (Xv_opaque) image_svrim(label);
	    }
	    break;

	  case PANEL_CLIENT_DATA:
	    ip->client_data = attrs[1];
	    break;

	  case PANEL_INACTIVE:
	    if (attrs[1])
		ip->flags |= INACTIVE;
	    else
		ip->flags &= ~INACTIVE;
	    break;

	  case PANEL_ITEM_COLOR:
	    DRAWABLE_INFO_MACRO(panel->paint_window->pw, info);
	    if (xv_depth(info) > 1)
		ip->color_index = (int) attrs[1];
	    break;

	  case XV_END_CREATE:
#ifdef ATTR_CONSUME_BUGS_FIXED
	    consumed = FALSE;
#endif

	    panel_check_item_layout(ip);
	    ip->flags |= CREATED;

	    /*
	     * for scrolling computations, note new extent of panel, tell
	     * scrollbars
	     */
	    if (!hidden(ip))
		panel_update_extent(ip->panel, ip->rect);
	    break;

	  default:

#ifdef ATTR_CONSUME_BUGS_FIXED
	    consumed = FALSE;
#endif
	    /* Note: xv_check_bad_attr is not called since item_set_generic
	     * may be called via xv_super_set_avlist in one of the
	     * panel item set routines.
	     */
	    break;
	}

#ifdef ATTR_CONSUME_BUGS_FIXED
	/* BUG ALERT:  Item set routines call xv_super_set_avlist,
	 * which calls this routine.  If this routine consumes attrs,
	 * then when xv_super_set_avlist returns to the item set routines,
	 * the switch (attrs[0]) which then gets executed will not see
	 * the attributes item_set_generic just consumed.
	 */
	if (consumed)
	    ATTR_CONSUME(attrs[0]);
#endif
    }
#endif OW_I18N

    /*
     * first handle any item position changes.
     */
    if (start_new_row) {
	if (!item_x_changed)
	    item_x = PANEL_ITEM_X_START;
	item_y = panel->lowest_bottom +
	    (next_row_gap >= 0 ? next_row_gap : panel->item_y_offset);
    }
    if (start_new_col) {
	if (!item_y_changed)
	    item_y = PANEL_ITEM_Y_START;
	item_x = panel->rightmost_right +
	    (next_col_gap >= 0 ? next_col_gap : panel->item_x_offset);
	panel->current_col_x = item_x;
    }
    if (start_new_row || start_new_col || item_x_changed || item_y_changed) {
	rect_construct(&deltas, 0, 0, 0, 0);
	/* compute item offset */
	if (start_new_row || start_new_col || item_x_changed)
	    deltas.r_left = item_x - ip->rect.r_left;
	if (start_new_row || start_new_col || item_y_changed)
	    deltas.r_top = item_y - ip->rect.r_top;
	if (deltas.r_left || deltas.r_top)
	    /*
	     * ITEM_X or ITEM_Y has changed, so re-layout item in order to
	     * cause the entire item to change position.
	     */
	    panel_item_layout(ip, &deltas);
    }
    /*
     * Now handle label size changes .
     */

    /*
     * update label font & boldness if needed. Note that this may be set
     * again below if a new label string or image has been given.
     */
    if (is_string(label) && label_font_changed) {
#ifdef	OW_I18N
	label_size = xv_pf_textwidth_wc(wslen(image_string_wc(label)),
				     image_font(label), image_string_wc(label));
#else
	label_size = xv_pf_textwidth(strlen(image_string(label)),
				     image_font(label), image_string(label));
#endif	OW_I18N
	if (ip->label_width)
	    label_size.x = ip->label_width;
	item_adjust_label_size(ip->item_type, label_type, &label_size,
	    ip->menu ? TRUE : FALSE, ip->panel->ginfo);
	ip->label_rect.r_width = label_size.x;
	ip->label_rect.r_height = label_size.y;
	label_size_changed = TRUE;
    }
    /* free old label, allocate new */
    if (set(label_type)) {
	label_size = panel_make_image(image_font(label), label,
	    label_type, label_data, image_bold(label), label_inverted);
	if (ip->label_width)
	    label_size.x = ip->label_width;
	item_adjust_label_size(ip->item_type, label_type, &label_size,
	    ip->menu ? TRUE : FALSE, ip->panel->ginfo);
	ip->label_rect.r_width = label_size.x;
	ip->label_rect.r_height = label_size.y;
	label_size_changed = TRUE;
    }
    if (!label_size_changed && !rect_isnull(&ip->label_rect) &&
	label_width_changed) {
	label_size.x = ip->label_width;
	label_size.y = ip->label_rect.r_height;
	item_adjust_label_size(ip->item_type, label_type, &label_size,
	    ip->menu ? TRUE : FALSE, ip->panel->ginfo);
	ip->label_rect.r_width = label_size.x;
	ip->label_rect.r_height = label_size.y;
	label_size_changed = TRUE;
    }

    /*
     * use default positions for label or value if not specified.
     */
    if (layout_changed || label_size_changed || label_x_changed ||
	label_y_changed) {
	/*
	 * layout, label position or size has changed, so re-compute default
	 * value position.
	 */
	if (label_x_changed)
	    ip->label_rect.r_left = label_x;
	if (label_y_changed)
	    ip->label_rect.r_top = label_y;

	/* now move the value if it's not fixed */
	fix_positions(ip);
    }
    if (value_x_changed || value_y_changed) {
	/*
	 * value position has changed, so re-compute default label position.
	 */
	rect_construct(&deltas, 0, 0, 0, 0);
	if (value_x_changed) {
	    deltas.r_left = value_x - ip->value_rect.r_left;
	    ip->value_rect.r_left = value_x;
	}
	if (value_y_changed) {
	    deltas.r_top = value_y - ip->value_rect.r_top;
	    ip->value_rect.r_top = value_y;
	}
	if (deltas.r_left || deltas.r_top) {
	    /*
	     * VALUE_X or VALUE_Y has changed, so tell item to shift all its
	     * components (choices, marks etc.).
	     */
	    (*ip->ops->layout) (ip, &deltas);

	    /* now move the label if it's not fixed */
	    fix_positions(ip);
	}
    }
    /* make sure the item rect encloses the label and value */
    ip->rect = panel_enclosing_rect(&ip->label_rect, &ip->value_rect);

    /* for scrolling computations, note new extent of panel, tell scrollbars */
    if (potential_new_rect && !hidden(ip))
	panel_update_extent(panel, ip->rect);

#ifdef	OW_I18N
    if (temp_wstr)
    	free((char *) temp_wstr);
#endif	OW_I18N
    return XV_OK;
}


/* Adjust size for borders around label */
static void
item_adjust_label_size(item_type, label_type, size, menu_attached, ginfo)
    Panel_item_type item_type;
    int		    label_type;	/* PIT_STRING or PIT_SVRIM */
    struct pr_size *size;
    int		    menu_attached;	/* boolean */
    Graphics_info  *ginfo;
{

    if (item_type != PANEL_BUTTON_ITEM)
	return;

    if (ginfo) {
	size->x += 2*ButtonEndcap_Width(ginfo);
	if (label_type == PIT_STRING)
	    size->y = Button_Height(ginfo);
	else
	    size->y += OLGX_VAR_HEIGHT_BTN_MARGIN;
    } else {
	if (size->y <= 12) {
	    size->x += 15;	/* left text margin = 7, right = width-9 */
	    size->y += 11;	/* top text margin = 4, bottom = height-8 */
	} else if ((size->y > 12) && (size->y < 19)) {
	    size->x += 17;	/* left text margin = 8, right = width-10 */
	    size->y += 12;	/* top text margin = 5, bottom = height-8 */
	} else {
	    size->x += 20;	/* left text margin = 9; right = width-12 */
	    size->y += 15;	/* top text margin = 5; bottom = height-11 */
	}
    }
    if (menu_attached) {
	size->x += 2*MenuMark_Width(ginfo);
	if (size->y < MenuMark_Height(ginfo))
	    size->y = MenuMark_Height(ginfo);
    }
}


/* fix_positions - of label and value rects */

static
fix_positions(ip)
    register Item_info *ip;
{
    if (!value_fixed(ip)) {
	struct rect     deltas;

	/* compute the value position which is to the right of the label */
	/* remember the old value rect position */
	rect_construct(&deltas, 0, 0, 0, 0);
	deltas.r_left = ip->value_rect.r_left;
	deltas.r_top = ip->value_rect.r_top;
	switch (ip->layout) {
	  case PANEL_HORIZONTAL:
	    /* after => to right of */
	    ip->value_rect.r_left = rect_right(&ip->label_rect) + 1 +
		(ip->label_rect.r_width ? LABEL_X_GAP : 0);
	    ip->value_rect.r_top = ip->label_rect.r_top;
	    break;

	  case PANEL_VERTICAL:
	    /* after => below */
	    ip->value_rect.r_left = ip->label_rect.r_left;
	    ip->value_rect.r_top = rect_bottom(&ip->label_rect) + 1 +
		(ip->label_rect.r_height ? LABEL_Y_GAP : 0);
	    break;
	}
	/* delta is new postion minus old position */
	deltas.r_left = ip->value_rect.r_left - deltas.r_left;
	deltas.r_top = ip->value_rect.r_top - deltas.r_top;
	if (deltas.r_left || deltas.r_top)
	    /*
	     * VALUE_X or VALUE_Y has changed, so tell item to shift all its
	     * components (choices, marks etc.).
	     */
	    (*ip->ops->layout) (ip, &deltas);
    }
    panel_fix_label_position(ip);
}


Pkg_private
panel_fix_label_position(ip)
    register Item_info *ip;
{
    if (label_fixed(ip))
	return;

    /* compute the label position as before the value. */
    switch (ip->layout) {
      case PANEL_HORIZONTAL:
	/* before => to left of */
	ip->label_rect.r_left = ip->value_rect.r_left;
	if (ip->label_rect.r_width > 0)
	    ip->label_rect.r_left -= ip->label_rect.r_width + LABEL_X_GAP;
	ip->label_rect.r_top = ip->value_rect.r_top;
	break;

      case PANEL_VERTICAL:
	/* before => above */
	ip->label_rect.r_left = ip->value_rect.r_left;
	ip->label_rect.r_top = ip->value_rect.r_top;
	if (ip->label_rect.r_height > 0)
	    ip->label_rect.r_top -= ip->label_rect.r_height + LABEL_Y_GAP;
	break;
    }
}

static          Xv_opaque
item_set_ops(ip, avlist)
    register Item_info *ip;
    register Attr_avlist avlist;
{
    register Attr_avlist attrs;
    register Panel_ops *new_ops;
    int			consumed;

#ifdef OW_I18N
    int			db_count = 0;

    attrs = avlist;
    while (*attrs || db_count) {
	consumed = TRUE;
	switch (attrs[0]) {
	  case XV_USE_DB:
	    consumed = FALSE;
	    db_count++;
	    break;
	  case PANEL_EVENT_PROC:
	    if (!ops_set(ip)) {
		new_ops = xv_alloc(Panel_ops);
		*new_ops = *ip->ops;
		ip->ops = new_ops;
		ip->flags |= OPS_SET;
	    }
	    ip->ops->handle_event = (void (*) ()) attrs[1];
	    if (!ip->ops->handle_event)
		ip->ops->handle_event = (void (*) ()) panel_nullproc;
	    break;

	  default:
	    consumed = FALSE;
	    break;
	}
	if (consumed)
	    ATTR_CONSUME(attrs[0]);

	if (attrs[0] == XV_USE_DB)
	    attrs++;
	else if (db_count && !attrs[0]) {
	    db_count--;
	    attrs++;
	} else
	    attrs = attr_next(attrs);
    }
#else OW_I18N
    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	consumed = TRUE;
	switch (attrs[0]) {
	  case PANEL_EVENT_PROC:
	    if (!ops_set(ip)) {
		new_ops = xv_alloc(Panel_ops);
		*new_ops = *ip->ops;
		ip->ops = new_ops;
		ip->flags |= OPS_SET;
	    }
	    ip->ops->handle_event = (void (*) ()) attrs[1];
	    if (!ip->ops->handle_event)
		ip->ops->handle_event = (void (*) ()) panel_nullproc;
	    break;

	  default:
	    consumed = FALSE;
	    break;
	}
	if (consumed)
	    ATTR_CONSUME(attrs[0]);
    }
#endif OW_I18N
    return XV_OK;
}


/****************************************************************

                 panel_item_parent

panel_item_parent has been supplied for internal SPD use only.
In particular the information management (database) group uses
it. The function is designed to let a client unparent and reparent
a panel item. One can implement a cache of items with this function
where the items are shared between multiple panels. We provided
this routine rather then making XV_OWNER settable
because too much of the item code expects to have a parent. We
would have had to rewrite too much of the panel code to make this
work from an attribute. As it is we provide this functionality
by letting the client use this routine to unset and reset an
item's parent. While an item has no parent a user can't set
any of its attributes. Code has been put in to prevent this.
The client can however get an items attributes that aren't
depenedent on the panel. If the item's attriibute does depend on
the panel NULL is returned.


The parent parameter is used to both reset and unset an item's
parent. If parent == NULL then the item will not have a parent
after it is removed from its current panel. If the parent != NULL
then the item will be reparented to parent

****************************************************************/

Xv_private
panel_item_parent(item, parent)
    Panel_item      item;
    Panel           parent;
{
    register Item_info *ip = ITEM_PRIVATE(item);
    register Panel_info *new_panel = NULL;
    register Panel_info *current_panel = ip->panel;

    if (parent != NULL) {
	new_panel = PANEL_PRIVATE(parent);
    }
    if (current_panel != NULL) {
	/* clear the  item */
	if (!current_panel->destroying) {
	    if (ip->item_type == PANEL_TEXT_ITEM &&
		ip == current_panel->kbd_focus_item) {
		panel_text_caret_on(current_panel, FALSE);
	    }
	    panel_clear_item(ip);
	}
	/* unlink the item */
	panel_unlink(ip, FALSE);/* unlink is not part of a destroy */
    }
    if (new_panel != NULL) {
	ip->panel = new_panel;

	/* relink the item */
	panel_append(ip);
	(*ip->ops->restore) (ip);

	/* don't repaint -- the client is probably doing a set */
	/* right after this to reposition the item */
    }
}
