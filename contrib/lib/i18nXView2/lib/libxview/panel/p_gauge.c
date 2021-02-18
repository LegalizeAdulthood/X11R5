#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_gauge.c 50.6 90/12/12 Copyr 1984 Sun Micro";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <X11/Xlib.h>
#include <xview/sun.h>
#include <xview_private/panel_impl.h>
#include <xview/pixwin.h>
#include <xview/font.h>
#include <xview_private/draw_impl.h>
#include <xview_private/pw_impl.h>
#ifdef OW_I18N
#include <xview/server.h>
#endif OW_I18N

#define GAUGE_PRIVATE(item)	\
	XV_PRIVATE(Gauge_info, Xv_panel_gauge, item)
#define GAUGE_PUBLIC(item)	XV_PUBLIC(item)

#define	GAUGE_FROM_ITEM(ip)	GAUGE_PRIVATE(ITEM_PUBLIC(ip))

#define TICK_THICKNESS	2
#define MIN_TICK_GAP	4	/* minimum gap between ticks */

Pkg_private int gauge_init();
Pkg_private Xv_opaque gauge_set_avlist();
Pkg_private Xv_opaque gauge_get_attr();
Pkg_private int gauge_destroy();

Pkg_private int panel_round();

extern struct pr_size xv_pf_textwidth();

static void	gauge_layout();
static void     gauge_paint();
static void     paint_gauge();
static void     update_rects();

static Panel_ops ops = {
    (void (*) ()) panel_nullproc,	/* handle_event() */
    (void (*) ()) panel_nullproc,	/* begin_preview() */
    (void (*) ()) panel_nullproc,	/* update_preview() */
    (void (*) ()) panel_nullproc,	/* cancel_preview() */
    (void (*) ()) panel_nullproc,	/* accept_preview() */
    (void (*) ()) panel_nullproc,	/* accept_menu() */
    (void (*) ()) panel_nullproc,	/* accept_key() */
    gauge_paint,			/* paint() */
    (void (*) ()) panel_nullproc,	/* remove() */
    (void (*) ()) panel_nullproc,	/* restore() */
    gauge_layout,			/* layout() */
    (void (*) ()) panel_nullproc,	/* accept_kbd_focus() */
    (void (*) ()) panel_nullproc	/* yield_kbd_focus() */
};

typedef struct {	/* data for a gauge */
    Panel_item      public_self;/* back pointer to object */
    int             actual;	/* # of pixels the "fluid" is to the right of
				 * or below the start of the gauge
				 * ("internal"). */
    Rect            gaugerect;	/* rect containing gauge */
    Rect            min_range_rect;	/* minimum range rect */
    int             min_value;
    int             max_value;
    int		    nticks;	/* nbr of tick marks on gauge */
    Rect            max_range_rect;  /* maximum range rect */
    Rect	    tickrect;	/* rect containing tick marks */
    int		    value;	/* value of gauge in client units
				 * ("external") */
    int             width;

    /* flags */
    unsigned int showrange:1;
    unsigned int vertical:1;
} Gauge_info;

/*ARGSUSED*/
Pkg_private int
gauge_init(panel_public, item_public, avlist)
    Panel           panel_public;
    Panel_item      item_public;
    Attr_avlist     avlist;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);
    register Item_info *ip = ITEM_PRIVATE(item_public);
    Xv_panel_gauge *item_object = (Xv_panel_gauge *) item_public;
    register Gauge_info *dp;

    dp = xv_alloc(Gauge_info);

    /* link to object */
    item_object->private_data = (Xv_opaque) dp;
    dp->public_self = item_public;

    if (ops_set(ip))
	*ip->ops = ops;		/* copy the button ops vector */
    else
	ip->ops = &ops;		/* use the static gauge ops vector */
    if (panel->event_proc)
	ip->ops->handle_event = (void (*) ()) panel->event_proc;
    ip->item_type = PANEL_GAUGE_ITEM;
    panel_set_bold_label_font(ip);

    /* Initialize non-zero dp fields */
    dp->showrange = TRUE;
    dp->width = 100;
    dp->max_value = 100;

    return XV_OK;
}


Pkg_private     Xv_opaque
gauge_set_avlist(item_public, avlist)
    Panel_item      item_public;
    register Attr_avlist avlist;
{
    register Item_info *ip = ITEM_PRIVATE(item_public);
    register Gauge_info *dp = GAUGE_PRIVATE(item_public);
    register Attr_attribute attr;
    int		    adjust_values = FALSE;
    int		    end_create = FALSE;
    Xv_opaque	    result;
    int		    size_changed = FALSE;
    int		    ticks_set = FALSE;
#ifdef OW_I18N
    register Attr_avlist	attrs = avlist;
    XID		   	 	db;
    int 			db_count = 0;
    Xv_Server	    		server;

    server =XV_SERVER_FROM_WINDOW(PANEL_PUBLIC(ip->panel));
    db = xv_get(server, SERVER_RESOURCE_DB);
#endif OW_I18N 

    /* if a client has called panel_item_parent this item may not */
    /* have a parent -- do nothing in this case */
    if (ip->panel == NULL) {
	return ((Xv_opaque) XV_ERROR);
    }

    /* Parse Panel Item Generic attributes before Gauge attributes.
     * Prevent panel_redisplay_item from being called in item_set_avlist.
     */
    ip->panel->status |= NO_REDISPLAY_ITEM;
    result = xv_super_set_avlist(item_public, &xv_panel_gauge_pkg, avlist);
    ip->panel->status &= ~NO_REDISPLAY_ITEM;
    if (result != XV_OK)
	return result;

#ifdef OW_I18N
    while (*attrs || db_count) {
	switch (attrs[0]) {
	  case XV_USE_DB:
	    db_count++;
	    break;

	  case PANEL_VALUE:
	    dp->value = (int) attrs[1];
	    adjust_values = TRUE;
	    break;

	  case PANEL_MIN_VALUE:
	    if (db_count) {
		dp->min_value = (int) db_get_data(db,
					ip->instance_qlist,
					"panel_min_value", XV_INT,
					attrs[1]);
	    }
	    else
		dp->min_value = (int) attrs[1];
	    adjust_values = TRUE;
	    break;

	  case PANEL_MAX_VALUE:
	    if (db_count) {
		dp->max_value = (int) db_get_data(db,
					ip->instance_qlist,
					"panel_max_value", XV_INT,
					attrs[1]);
	    }
	    else
		dp->max_value = (int) attrs[1];
	    adjust_values = TRUE;
	    break;

	  case PANEL_DIRECTION:
	    if (attrs[1] == PANEL_VERTICAL)
		dp->vertical = TRUE;
	    else
		dp->vertical = FALSE;
	    size_changed = TRUE;
	    break;
		
	  case PANEL_SHOW_RANGE:
	    if ((int) attrs[1])
		dp->showrange = TRUE;
	    else
		dp->showrange = FALSE;
	    size_changed = TRUE;
	    break;

	  case PANEL_TICKS:
	    if (db_count) {
		dp->nticks = (int) db_get_data(db,
					ip->instance_qlist,
					"panel_ticks", XV_INT,
					attrs[1]);
	    }
	    else
		dp->nticks = (int) attrs[1];
	    size_changed = TRUE;
	    ticks_set = TRUE;
	    break;

	  case PANEL_GAUGE_WIDTH:
	    if (db_count) {
		dp->width = (int) db_get_data(db,
					ip->instance_qlist,
					"panel_gauge_width", XV_INT,
					attrs[1]);
	    }
	    else
		dp->width = (int) attrs[1];
	    size_changed = TRUE;
	    break;

	  case XV_END_CREATE:
	    end_create = TRUE;
	    break;

	  default:
	    break;
	}

	if (attrs[0] == XV_USE_DB)
	    attrs++;
        else if (db_count && !attrs[0]) {
            db_count--;
            attrs++;
        } else
            attrs = attr_next(attrs);
    }
#else OW_I18N
    while (attr = *avlist++) {
	switch (attr) {
	  case PANEL_VALUE:
	    dp->value = (int) *avlist++;
	    adjust_values = TRUE;
	    break;

	  case PANEL_MIN_VALUE:
	    dp->min_value = (int) *avlist++;
	    adjust_values = TRUE;
	    break;

	  case PANEL_MAX_VALUE:
	    dp->max_value = (int) *avlist++;
	    adjust_values = TRUE;
	    break;

	  case PANEL_DIRECTION:
	    if (*avlist++ == PANEL_VERTICAL)
		dp->vertical = TRUE;
	    else
		dp->vertical = FALSE;
	    size_changed = TRUE;
	    break;
		
	  case PANEL_SHOW_RANGE:
	    if ((int) *avlist++)
		dp->showrange = TRUE;
	    else
		dp->showrange = FALSE;
	    size_changed = TRUE;
	    break;

	  case PANEL_TICKS:
	    dp->nticks = (int) *avlist++;
	    size_changed = TRUE;
	    ticks_set = TRUE;
	    break;

	  case PANEL_GAUGE_WIDTH:
	    dp->width = (int) *avlist++;
	    size_changed = TRUE;
	    break;

	  case XV_END_CREATE:
	    end_create = TRUE;
	    break;

	  default:
	    /* skip past what we don't care about */
	    avlist = attr_skip(attr, avlist);
	    break;
	}
    }
#endif OW_I18N

    if (ticks_set) {
	if (dp->nticks > dp->width / (TICK_THICKNESS+MIN_TICK_GAP))
	    dp->nticks = dp->width / (TICK_THICKNESS+MIN_TICK_GAP);
    }
    /* set external (client unit) and internal (pixel) values */
    if (adjust_values) {
	if (dp->value < dp->min_value)
	    dp->value = dp->min_value;
	else if (dp->value > dp->max_value)
	    dp->value = dp->max_value;
	dp->actual = etoi(dp, dp->value);
    }
    if (created(ip) && size_changed) {
	/* Note: This call to update_rects only takes place on an
	 *       xv_set call (i.e., after the gauge has been created).
	 */
	update_rects(ip);
    }
    if (end_create) {
	/* Note: This call to update_rects only takes place on the
	 *       XV_END_CREATE pass of an xv_create call.
	 */
	update_rects(ip);
	panel_check_item_layout(ip);
	panel_append(ip);
    }

    return XV_OK;
}


Pkg_private     Xv_opaque
gauge_get_attr(item_public, status, which_attr, valist)	/*ARGSUSED*/
    Panel_item      item_public;
    int            *status;
    Attr_attribute  which_attr;
    va_list         valist;
{
    register Gauge_info *dp = GAUGE_PRIVATE(item_public);

    switch (which_attr) {
      case PANEL_VALUE:
	return (Xv_opaque) dp->value;

      case PANEL_MIN_VALUE:
	return (Xv_opaque) dp->min_value;

      case PANEL_MAX_VALUE:
	return (Xv_opaque) dp->max_value;

      case PANEL_DIRECTION:
	return (Xv_opaque) (dp->vertical ? PANEL_VERTICAL : PANEL_HORIZONTAL);

      case PANEL_SHOW_RANGE:
	return (Xv_opaque) (dp->showrange ? TRUE : FALSE);

      case PANEL_TICKS:
	return (Xv_opaque) dp->nticks;

      case PANEL_GAUGE_WIDTH:
	return (Xv_opaque) dp->width;

      default:
	*status = XV_ERROR;
	return (Xv_opaque) 0;
    }
}


Pkg_private int
gauge_destroy(item_public, status)
    Panel_item      item_public;
    Destroy_status  status;
{
    Gauge_info    *dp = GAUGE_PRIVATE(item_public);

    if ((status == DESTROY_CHECKING) || (status == DESTROY_SAVE_YOURSELF))
	return XV_OK;

    free((char *) dp);

    return XV_OK;
}


static void
update_rects(ip)
    register Item_info *ip;
{
    register Gauge_info *dp = GAUGE_FROM_ITEM(ip);
    Graphics_info  *ginfo = ip->panel->ginfo;
    char            min_buf[16];
    char            max_buf[16];
    int		    min_range_width;	/* pixels */
    int		    max_range_width;	/* pixels */
    struct pr_size  size;

    sprintf(min_buf, "%d", dp->min_value);
    sprintf(max_buf, "%d", dp->max_value);
    size = xv_pf_textwidth(strlen(min_buf), ip->panel->std_font, min_buf);
    min_range_width = size.x;
    size = xv_pf_textwidth(strlen(max_buf), ip->panel->std_font, max_buf);
    max_range_width = size.x;

    dp->gaugerect.r_left = ip->value_rect.r_left;
    dp->gaugerect.r_top = ip->value_rect.r_top;
    if (dp->vertical) {
	/* Create the gauge rect */
	dp->gaugerect.r_width = Gauge_EndCapHeight(ginfo);
	dp->gaugerect.r_height = dp->width + 2*Gauge_EndCapOffset(ginfo);

	/* Create the tick rect */
	dp->tickrect.r_top = dp->gaugerect.r_top;
	dp->tickrect.r_left = dp->gaugerect.r_left + dp->gaugerect.r_width;
	if (dp->nticks)
	    dp->tickrect.r_width = 
		dp->gaugerect.r_width/2 - 1;
	else
	    dp->tickrect.r_width = 0;
	dp->tickrect.r_height = dp->gaugerect.r_height;

	/* Create the maximum range rect */
	dp->max_range_rect.r_top = dp->tickrect.r_top;
	dp->max_range_rect.r_left = dp->tickrect.r_left + dp->tickrect.r_width;
	if (dp->showrange) {
	    dp->max_range_rect.r_height = 
		xv_get(ip->panel->std_font, FONT_DEFAULT_CHAR_HEIGHT);
	    dp->max_range_rect.r_width = max_range_width;
	} else {
	    dp->max_range_rect.r_height = 0;
	    dp->max_range_rect.r_width = 0;
	}

	/* Create the minimum range rect */
	rect_construct(&dp->min_range_rect,
	    dp->max_range_rect.r_left,
	    dp->gaugerect.r_top + dp->gaugerect.r_height -
	        dp->max_range_rect.r_height,
	    dp->showrange ? min_range_width : 0,
	    dp->max_range_rect.r_height);

	/* Update the item value rect */
	ip->value_rect.r_width =
	    dp->gaugerect.r_width + dp->tickrect.r_width +
	    MAX(min_range_width, max_range_width);
	ip->value_rect.r_height = dp->gaugerect.r_height;

    } else {
	/* Create the gauge rect */
	dp->gaugerect.r_width = dp->width + 2*Gauge_EndCapOffset(ginfo);
	dp->gaugerect.r_height = Gauge_EndCapHeight(ginfo);

	/* Create the tick rect */
	dp->tickrect.r_top = dp->gaugerect.r_top + dp->gaugerect.r_height;;
	dp->tickrect.r_left = dp->gaugerect.r_left;
	dp->tickrect.r_width = dp->gaugerect.r_width;
	if (dp->nticks)
	    dp->tickrect.r_height = 
		dp->gaugerect.r_height/2 - 1;
	else
	    dp->tickrect.r_height = 0;

	/* Create the minimum range rect */
	dp->min_range_rect.r_top = dp->tickrect.r_top + dp->tickrect.r_height;
	dp->min_range_rect.r_left = dp->tickrect.r_left;
	if (dp->showrange) {
	    dp->min_range_rect.r_height 
			= xv_get(ip->panel->std_font, FONT_DEFAULT_CHAR_HEIGHT);
	    dp->min_range_rect.r_width = min_range_width;
	} else {
	    dp->min_range_rect.r_height = 0;
	    dp->min_range_rect.r_width = 0;
	}

	/* Create the maximum range rect */
	rect_construct(&dp->max_range_rect,
	    dp->gaugerect.r_left + dp->gaugerect.r_width - max_range_width,
	    dp->tickrect.r_top + dp->tickrect.r_height,
	    dp->showrange ? max_range_width : 0,
	    dp->min_range_rect.r_height);

	/* Update the item value rect */
	ip->value_rect.r_width = dp->gaugerect.r_width;
	ip->value_rect.r_height = dp->gaugerect.r_height +
	    dp->tickrect.r_height + dp->min_range_rect.r_height;
    }

    /* Update the item rect */
    ip->rect = panel_enclosing_rect(&ip->label_rect, &ip->value_rect);
}


static void
gauge_layout(ip, deltas)
    Item_info      *ip;
    Rect           *deltas;
{
    Gauge_info    *dp = GAUGE_FROM_ITEM(ip);

    if (!created(ip))
	return;
    dp->gaugerect.r_left += deltas->r_left;
    dp->gaugerect.r_top += deltas->r_top;
    dp->tickrect.r_left += deltas->r_left;
    dp->tickrect.r_top += deltas->r_top;
    dp->min_range_rect.r_left += deltas->r_left;
    dp->min_range_rect.r_top += deltas->r_top;
    dp->max_range_rect.r_left += deltas->r_left;
    dp->max_range_rect.r_top += deltas->r_top;
}


static void
gauge_paint(ip)
    Item_info      *ip;
{
    Rect           *r;
    Gauge_info    *dp = GAUGE_FROM_ITEM(ip);
    char            buf[10];
    Xv_Window       pw;
#ifdef OW_I18N
    wchar_t	    buf_wc[10];
#endif /*  OW_I18N  */

    r = &(ip->label_rect);

    /* paint the label */
    (void) panel_paint_image(ip->panel, &ip->label, r, inactive(ip),
			     ip->color_index);

#ifdef OW_I18N
    /* paint the range */
    if (dp->showrange) {
	wsprintf(buf_wc, "%d", dp->min_value);
	r = &dp->min_range_rect;
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    panel_paint_text(pw, ip->panel->std_font_xid, ip->color_index,
		r->r_left, r->r_top + panel_fonthome(ip->panel->std_font), buf_wc);
	PANEL_END_EACH_PAINT_WINDOW

	wsprintf(buf_wc, "%d", dp->max_value);
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    r = &dp->max_range_rect;
	    panel_paint_text(pw, ip->panel->std_font_xid, ip->color_index,
		r->r_left, r->r_top + panel_fonthome(ip->panel->std_font), buf_wc);
	PANEL_END_EACH_PAINT_WINDOW
    }
#else
    /* paint the range */
    if (dp->showrange) {
	sprintf(buf, "%d", dp->min_value);
	r = &dp->min_range_rect;
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    panel_paint_text(pw, ip->panel->std_font_xid, ip->color_index,
		r->r_left, r->r_top + panel_fonthome(ip->panel->std_font), buf);
	PANEL_END_EACH_PAINT_WINDOW

	sprintf(buf, "%d", dp->max_value);
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    r = &dp->max_range_rect;
	    panel_paint_text(pw, ip->panel->std_font_xid, ip->color_index,
		r->r_left, r->r_top + panel_fonthome(ip->panel->std_font), buf);
	PANEL_END_EACH_PAINT_WINDOW
    }
#endif /*  OW_I18N  */

    /* paint the gauge */
    paint_gauge(ip);
}


static void
paint_gauge(ip)
    Item_info      *ip;
{
    register Gauge_info    *dp = GAUGE_FROM_ITEM(ip);
    Graphics_info  *ginfo = ip->panel->ginfo;
    int		    height;
    Xv_Drawable_info *info;
    int		    limit;
    int		    pixel_value;
    Xv_Window       pw;
    int		    save_black;
    int		    tick;
    int		    tick_gap;
    int		    width;
    int		    x;
    Drawable	    xid;
    int		    y;

    if (ip->color_index >= 0)
        save_black = olgx_get_single_color(ginfo, OLGX_BLACK);

    PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	DRAWABLE_INFO_MACRO(pw, info);
	xid = xv_xid(info);
        if (ip->color_index >= 0) {
            olgx_set_single_color(ginfo, OLGX_BLACK,
                                  xv_get(xv_cms(info), CMS_PIXEL,
                                  ip->color_index), OLGX_SPECIAL);
        }
	pixel_value = Gauge_EndCapOffset(ginfo) + dp->actual;
	olgx_draw_gauge(ginfo, xid,
			dp->gaugerect.r_left, dp->gaugerect.r_top,
			dp->width + 2*Gauge_EndCapOffset(ginfo),
			pixel_value, pixel_value,
			dp->vertical ? OLGX_VERTICAL : OLGX_HORIZONTAL);
	if (dp->nticks) {
	    x = dp->tickrect.r_left;
	    y = dp->tickrect.r_top;
	    if (dp->vertical) {
		tick_gap = dp->tickrect.r_height / (dp->nticks - 1);
		width = dp->tickrect.r_width;
		height = TICK_THICKNESS;
		limit = dp->tickrect.r_top+dp->tickrect.r_height-TICK_THICKNESS;
	    } else {
		tick_gap = dp->tickrect.r_width / (dp->nticks - 1);
		width = TICK_THICKNESS;
		height = dp->tickrect.r_height;
		limit = dp->tickrect.r_left+dp->tickrect.r_width-TICK_THICKNESS;
	    }
	    for (tick = 1; tick <= dp->nticks; tick++) {
		olgx_draw_box(ginfo, xid,
			      x, y, width, height, OLGX_NORMAL, FALSE);
		if (dp->vertical) {
		    y += tick_gap;
		    if (y > limit)
			y = limit; 
		} else {
		    x += tick_gap;
		    if (x > limit)
			x = limit;
		}
	    }
	}
    PANEL_END_EACH_PAINT_WINDOW
 
    if (ip->color_index >= 0)
        olgx_set_single_color(ginfo, OLGX_BLACK, save_black, OLGX_SPECIAL);
}


/*
 * Convert external value (client units) to internal value (pixels).
 */
static int
etoi(dp, value)
    Gauge_info    *dp;
    int             value;
{
    if (value <= dp->min_value)
	return (0);

    if (value >= dp->max_value)
	return (dp->width);

    return (panel_round(((value - dp->min_value) * dp->width),
		        (dp->max_value - dp->min_value + 1)));
}


