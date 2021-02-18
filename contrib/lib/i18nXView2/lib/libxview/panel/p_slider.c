#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_slider.c 50.8 90/12/12 Copyr 1984 Sun Micro";
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

#define SLIDER_PRIVATE(item)	\
	XV_PRIVATE(Slider_info, Xv_panel_slider, item)
#define SLIDER_PUBLIC(item)	XV_PUBLIC(item)

#define	SLIDER_FROM_ITEM(ip)	SLIDER_PRIVATE(ITEM_PUBLIC(ip))

#define	TEXT_VALUE_GAP	6	/* padding on either side of text value */
#define TICK_THICKNESS	2
#define MIN_TICK_GAP	4	/* minimum gap between ticks */

Pkg_private int slider_init();
Pkg_private Xv_opaque slider_set_avlist();
Pkg_private Xv_opaque slider_get_attr();
Pkg_private int slider_destroy();

Pkg_private int panel_round();

static Panel_setting     get_value();

static void	check_endbox_entered();
static void     paint_slider();
static void     update_rects();

static void     slider_begin_preview(), slider_update_preview(),
		slider_cancel_preview(), slider_accept_preview(),
		slider_paint(), slider_layout();

static Panel_ops ops = {
    panel_default_handle_event,		/* handle_event() */
    slider_begin_preview,		/* begin_preview() */
    slider_update_preview,		/* update_preview() */
    slider_cancel_preview,		/* cancel_preview() */
    slider_accept_preview,		/* accept_preview() */
    (void (*) ()) panel_nullproc,	/* accept_menu() */
    (void (*) ()) panel_nullproc,	/* accept_key() */
    slider_paint,			/* paint() */
    (void (*) ()) panel_nullproc,	/* remove() */
    (void (*) ()) panel_nullproc,	/* restore() */
    slider_layout,			/* layout() */
    (void (*) ()) panel_nullproc,	/* accept_kbd_focus() */
    (void (*) ()) panel_nullproc	/* yield_kbd_focus() */
};

typedef struct {	/* data for a slider */
    Panel_item      public_self;/* back pointer to object */
    int             actual;	/* # of pixels the slider box is to the right of
				 * or below the start of the slider bar before
				 * or after a drag-slider-box operation. */
    int             apparent;	/* # of pixels the slider box is to the right of
				 * or below the start of the slider bar during
				 * a drag-slider-box operation. */
    u_int           flags;
    Rect	    max_endbox_rect;	/* maximum end box rect */
    Rect            max_range_rect;	/* maximum range rect */
    int		    max_range_size;	/* # of characters in max_value */
    int             max_value;	/* in client units */
    Rect	    min_endbox_rect;	/* left or top end box rect */
    Rect            min_range_rect;	/* minimum range rect */
    int		    min_range_size;	/* # of characters in min_value */
    int             min_value;	/* in client units */
    int		    nticks;	/* nbr of tick marks on slider */
    int             print_value;/* value from PANEL_VALUE (in client units) */
    int             restore_print_value:1;
    Rect            sliderbox;  /* rect containing the position of last
				 * slider box */
    Rect            sliderrect;	/* rect containing slider */
    Rect	    tickrect;	/* rect containing tick marks */
    int             use_print_value:1;
    Rect            valuerect;	/* rect containing (text) value of slider */
    int             valuesize;	/* max # of characters in current value */
    int             value_offset;
    /*
     * When a user drags the slider box, the value should actually be just to
     * the left of or below the slider box. Value_offset is the distance between
     * event_{x,y}(event) and where the real value should be
     */
    Panel_item      value_textitem;	/* for displaying value as a text
					 * item */
    int             width;
} Slider_info;


/* flags */
#define	SHOWRANGE	0x01	/* show high and low scale numbers */
#define	SHOWVALUE	0x02	/* show current value */
#define SHOWENDBOXES	0x04	/* show end boxes */
#define	CONTINUOUS	0x08	/* notify on any change */
#define READONLY	0x10	/* text value is read-only */
#define	VERTICAL	0x20	/* slider orientation (0=>horizontal) */
#define MIN_ENDBOX_SELECTED	0x40	/* min end box is selected */
#define MAX_ENDBOX_SELECTED	0x80	/* max end box is selected */

#define vertical(dp) ((dp)->flags & VERTICAL)


/*ARGSUSED*/
Pkg_private int
slider_init(panel_public, item_public, avlist)
    Panel           panel_public;
    Panel_item      item_public;
    Attr_avlist     avlist;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);
    register Item_info *ip = ITEM_PRIVATE(item_public);
    Xv_panel_slider *item_object = (Xv_panel_slider *) item_public;
    register Slider_info *dp;

    dp = xv_alloc(Slider_info);

    /* link to object */
    item_object->private_data = (Xv_opaque) dp;
    dp->public_self = item_public;

    if (ops_set(ip))
	*ip->ops = ops;		/* copy the button ops vector */
    else
	ip->ops = &ops;		/* use the static slider ops vector */
    if (panel->event_proc)
	ip->ops->handle_event = (void (*) ()) panel->event_proc;
    ip->item_type = PANEL_SLIDER_ITEM;
    panel_set_bold_label_font(ip);

    /* Initialize non-zero dp fields */
    dp->flags = SHOWRANGE | SHOWVALUE;
    dp->width = 100;
    dp->min_range_size = 1;	/* # of characters in min_value */
    dp->max_value = 100;
    dp->max_range_size = 3;	/* # of characters in max_value */
    dp->valuesize = 3;		/* max # of characters in current value */

    return XV_OK;
}


Pkg_private     Xv_opaque
slider_set_avlist(item_public, avlist)
    Panel_item      item_public;
    register Attr_avlist avlist;
{
    register Item_info *ip = ITEM_PRIVATE(item_public);
    register Slider_info *dp = SLIDER_PRIVATE(item_public);
    register Attr_attribute attr;
    char            buf[16];
    char            buf2[16];
    int             char_width;
    int		    end_create = FALSE;
    int             max_value = dp->max_value;
    int             min_value = dp->min_value;
    int		    range_set = FALSE;
    Xv_opaque       result;
    int		    show_item = -1;
    int		    size_changed = FALSE;
    int		    ticks_set = FALSE;
    int             width = dp->width;
    int             value;
    int		    value_set = FALSE;

#ifdef OW_I18N
    register Attr_avlist 	attrs = avlist;
    Xv_Server	    		server;
    XID		    		db;
    int		    		db_count = 0;

    server =XV_SERVER_FROM_WINDOW(PANEL_PUBLIC(ip->panel));
    db = xv_get(server, SERVER_RESOURCE_DB);
#endif OW_I18N

    /* If a client has called panel_item_parent this item may not */
    /* have a parent -- do nothing in this case */
    if (ip->panel == NULL) {
	return ((Xv_opaque) XV_ERROR);
    }

    /* Parse Panel Item Generic attributes before Slider attributes.
     * Prevent panel_redisplay_item from being called in item_set_avlist.
     */

    ip->panel->status |= NO_REDISPLAY_ITEM;
    result = xv_super_set_avlist(item_public, &xv_panel_slider_pkg, avlist);
    ip->panel->status &= ~NO_REDISPLAY_ITEM;
    if (result != XV_OK)
	return result;

#ifdef OW_I18N
    while (*attrs || db_count) {
	switch (attrs[0]) {

	  case XV_USE_DB:
	    db_count++;
	    break;

	  case PANEL_ITEM_COLOR:
	    if (dp->value_textitem)
		xv_set(dp->value_textitem,
		       PANEL_ITEM_COLOR, (char *) attrs[1],
		       0);
	    break;

	  case PANEL_READ_ONLY:
	    if (attrs[1])
		dp->flags |= READONLY;
	    else
		dp->flags &= ~READONLY;
	    break;

	  case PANEL_VALUE_FONT:
	    /* Sunview1 compatibility attribute: not used */
	    break;

	  case PANEL_NOTIFY_LEVEL:
	    if ((Panel_setting) attrs[1] == PANEL_ALL)
		dp->flags |= CONTINUOUS;
	    else
		dp->flags &= ~CONTINUOUS;
	    break;

	  case PANEL_VALUE:
	    value = (int) attrs[1];
	    value_set = TRUE;
	    dp->use_print_value = FALSE;
	    break;

	  case PANEL_MIN_VALUE:
	    if (db_count) {
		min_value = (int) db_get_data(db,
					ip->instance_qlist,
					"panel_min_value", XV_INT,
					attrs[1]);
	    }
	    else
		min_value = (int) attrs[1];
	    range_set = TRUE;
	    break;

	  case PANEL_MAX_VALUE:
	    if (db_count) {
		max_value = (int) db_get_data(db,
					ip->instance_qlist,
					"panel_max_value", XV_INT,
					attrs[1]);
	    }
	    else
		max_value = (int) attrs[1];
	    range_set = TRUE;
	    break;

	  case PANEL_DIRECTION:
	    if (attrs[1] == PANEL_VERTICAL)
		dp->flags |= VERTICAL;
	    else
		dp->flags &= ~VERTICAL;
	    size_changed = TRUE;
	    break;
		
	  case PANEL_SLIDER_END_BOXES:
	    if (attrs[1]) {
		dp->flags |= SHOWENDBOXES;
	    } else
		dp->flags &= ~SHOWENDBOXES;
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

	  case PANEL_SLIDER_WIDTH:
	    if (db_count) {
		width = (int) db_get_data(db,
				ip->instance_qlist,
				"panel_slider_width", XV_INT,
				attrs[1]);
	    }
	    else
		width = (int) attrs[1];
	    range_set = TRUE;
	    break;

	  case PANEL_VALUE_DISPLAY_LENGTH:
	    if (db_count) {
		char_width = (int) db_get_data(db,
				ip->instance_qlist,
				"panel_value_display_length", XV_INT,
				attrs[1]);
	    }
	    else
		char_width = (int) attrs[1];
	    width = panel_col_to_x(ip->panel->std_font, char_width);
	    range_set = TRUE;
	    break;

	  case XV_SHOW:
	    show_item = (short) attrs[1];
	    break;

	  case PANEL_SHOW_VALUE:
	    if (attrs[1]) {
		dp->flags |= SHOWVALUE;
	    } else
		dp->flags &= ~SHOWVALUE;
	    size_changed = TRUE;
	    break;

	  case PANEL_SHOW_RANGE:
	    if ((int) attrs[1])
		dp->flags |= SHOWRANGE;
	    else
		dp->flags &= ~SHOWRANGE;
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
	  case PANEL_ITEM_COLOR:
	    if (dp->value_textitem)
		xv_set(dp->value_textitem,
		       PANEL_ITEM_COLOR, (char *) *avlist++,
		       0);
	    else
		avlist++;
	    break;

	  case PANEL_READ_ONLY:
	    if (*avlist++)
		dp->flags |= READONLY;
	    else
		dp->flags &= ~READONLY;
	    break;

	  case PANEL_VALUE_FONT:
	    /* Sunview1 compatibility attribute: not used */
	    avlist++;
	    break;

	  case PANEL_NOTIFY_LEVEL:
	    if ((Panel_setting) * avlist++ == PANEL_ALL)
		dp->flags |= CONTINUOUS;
	    else
		dp->flags &= ~CONTINUOUS;
	    break;

	  case PANEL_VALUE:
	    value = (int) *avlist++;
	    value_set = TRUE;
	    dp->use_print_value = FALSE;
	    break;

	  case PANEL_MIN_VALUE:
	    min_value = (int) *avlist++;
	    range_set = TRUE;
	    break;

	  case PANEL_MAX_VALUE:
	    max_value = (int) *avlist++;
	    range_set = TRUE;
	    break;

	  case PANEL_DIRECTION:
	    if (*avlist++ == PANEL_VERTICAL)
		dp->flags |= VERTICAL;
	    else
		dp->flags &= ~VERTICAL;
	    size_changed = TRUE;
	    break;
		
	  case PANEL_SLIDER_END_BOXES:
	    if (*avlist++) {
		dp->flags |= SHOWENDBOXES;
	    } else
		dp->flags &= ~SHOWENDBOXES;
	    size_changed = TRUE;
	    break;

	  case PANEL_TICKS:
	    dp->nticks = (int) *avlist++;
	    size_changed = TRUE;
	    ticks_set = TRUE;
	    break;

	  case PANEL_SLIDER_WIDTH:
	    width = (int) *avlist++;
	    range_set = TRUE;
	    break;

	  case PANEL_VALUE_DISPLAY_LENGTH:
	    char_width = (int) *avlist++;
	    width = panel_col_to_x(ip->panel->std_font, char_width);
	    range_set = TRUE;
	    break;

	  case XV_SHOW:
	    show_item = (short) *avlist++;
	    break;

	  case PANEL_SHOW_VALUE:
	    if (*avlist++) {
		dp->flags |= SHOWVALUE;
	    } else
		dp->flags &= ~SHOWVALUE;
	    size_changed = TRUE;
	    break;

	  case PANEL_SHOW_RANGE:
	    if ((int) *avlist++)
		dp->flags |= SHOWRANGE;
	    else
		dp->flags &= ~SHOWRANGE;
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
	if (dp->nticks > width / (TICK_THICKNESS+MIN_TICK_GAP))
	    dp->nticks = width / (TICK_THICKNESS+MIN_TICK_GAP);
    }
    if (range_set) {
	/* get the current value */
	if (!value_set) {
	    value = itoe(dp, dp->actual);
	    value_set = TRUE;
	}
	dp->min_value = min_value;
	/* don't let the max value be <= the min value */
	dp->max_value =
	    (max_value <= dp->min_value) ? dp->min_value + 1 : max_value;
	sprintf(buf, "%d", dp->min_value);
	sprintf(buf2, "%d", dp->max_value);
	dp->min_range_size = strlen(buf);
	dp->max_range_size = strlen(buf2);
	dp->valuesize = MAX(dp->min_range_size, dp->max_range_size);
	dp->width = width;
	size_changed = TRUE;
	dp->use_print_value = FALSE;   /* print_value is no longer valid */
    }
    /* set apparent & actual value */
    if (value_set) {
	if (value < dp->min_value)
	    value = dp->min_value;
	else if (value > dp->max_value)
	    value = dp->max_value;
	dp->apparent = dp->actual = etoi(dp, value);
	dp->print_value = value;
	dp->use_print_value = TRUE;
    }
    if (created(ip)) {
	if (dp->flags & SHOWVALUE) {
	    if (show_item == -1)
		show_item = hidden(ip) ? FALSE : TRUE;
	    if (!dp->use_print_value) {
		dp->print_value = itoe(dp, dp->actual);
		dp->use_print_value = TRUE;
	    }
	    (void) sprintf(buf, "%d", dp->print_value);
	    if (dp->value_textitem) {
		xv_set(dp->value_textitem,
		       PANEL_ITEM_COLOR, ip->color_index,
		       PANEL_READ_ONLY, dp->flags & READONLY ?
			   TRUE : FALSE,
		       XV_SHOW, show_item,
		       PANEL_VALUE, buf,
		       PANEL_VALUE_DISPLAY_LENGTH, dp->valuesize,
		       0);
	    } else {
		dp->value_textitem =
		    xv_create(PANEL_PUBLIC(ip->panel), PANEL_TEXT,
			      PANEL_ITEM_OWNER, item_public,
			      PANEL_CLIENT_DATA, ip,
			      PANEL_ITEM_COLOR, ip->color_index,
			      PANEL_ITEM_X, rect_right(&ip->label_rect) +
				  TEXT_VALUE_GAP,
			      PANEL_ITEM_Y, ip->label_rect.r_top,
			      PANEL_NOTIFY_PROC, get_value,
			      PANEL_READ_ONLY, dp->flags & READONLY ?
				  TRUE : FALSE,
			      XV_SHOW, show_item,
			      PANEL_VALUE, buf,
			      PANEL_VALUE_DISPLAY_LENGTH, dp->valuesize,
			      0);
		size_changed = TRUE;
	    }
	} else if (dp->value_textitem) {
	    /* Hide value */
	    xv_set(dp->value_textitem,
		   XV_SHOW, FALSE,
		   0);
	}
	if (size_changed)
	    /* Note: This call to update_rects only takes place on an
	     *       xv_set call (i.e., after the slider has been created).
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
slider_get_attr(item_public, status, which_attr, valist)	/*ARGSUSED*/
    Panel_item      item_public;
    int            *status;
    Attr_attribute  which_attr;
    va_list         valist;
{
    register Slider_info *dp = SLIDER_PRIVATE(item_public);

    switch (which_attr) {
      case PANEL_READ_ONLY:
	return (Xv_opaque)
	    (dp->flags & READONLY ? TRUE : FALSE);

      case PANEL_NOTIFY_LEVEL:
	return (Xv_opaque)
	    (dp->flags & CONTINUOUS ? PANEL_ALL : PANEL_DONE);

      case PANEL_VALUE:
	return (Xv_opaque) itoe(dp, dp->actual);

      case PANEL_MIN_VALUE:
	return (Xv_opaque) dp->min_value;

      case PANEL_MAX_VALUE:
	return (Xv_opaque) dp->max_value;

      case PANEL_DIRECTION:
	return (Xv_opaque) (dp->flags & VERTICAL ? PANEL_VERTICAL :
	    PANEL_HORIZONTAL);

      case PANEL_SLIDER_END_BOXES:
	return (Xv_opaque) (dp->flags & SHOWENDBOXES ? TRUE : FALSE);

      case PANEL_TICKS:
	return (Xv_opaque) dp->nticks;

      case PANEL_SLIDER_WIDTH:
	return (Xv_opaque) dp->width;

      case PANEL_SHOW_VALUE:
	return (Xv_opaque) (dp->flags & SHOWVALUE ? TRUE : FALSE);

      case PANEL_SHOW_RANGE:
	return (Xv_opaque) (dp->flags & SHOWRANGE ? TRUE : FALSE);

      default:
	*status = XV_ERROR;
	return (Xv_opaque) 0;
    }
}


Pkg_private int
slider_destroy(item_public, status)
    Panel_item      item_public;
    Destroy_status  status;
{
    Slider_info    *dp = SLIDER_PRIVATE(item_public);

    if ((status == DESTROY_CHECKING) || (status == DESTROY_SAVE_YOURSELF))
	return XV_OK;

    if (dp->value_textitem)
	xv_destroy(dp->value_textitem);

    free((char *) dp);

    return XV_OK;
}


static void
update_rects(ip)
    register Item_info *ip;
{
    register Slider_info *dp = SLIDER_FROM_ITEM(ip);
    int		chrht, chrwth;

    /* Create the text value rect */
    if (dp->flags & SHOWVALUE) {
	dp->valuerect = *(Rect *) xv_get(dp->value_textitem, PANEL_ITEM_RECT);
	dp->valuerect.r_width += TEXT_VALUE_GAP;
    } else {
	rect_construct(&dp->valuerect,
		       rect_right(&ip->label_rect) + TEXT_VALUE_GAP,
	    	       ip->value_rect.r_top, 0, 0);
    }

    chrht = xv_get(ip->panel->std_font, FONT_DEFAULT_CHAR_HEIGHT);
    chrwth = xv_get(ip->panel->std_font, FONT_DEFAULT_CHAR_WIDTH);
    if (vertical(dp)) {
	/* Create the maximum range rect */
	dp->max_range_rect.r_top = dp->valuerect.r_top;
	dp->max_range_rect.r_left = dp->valuerect.r_left +
	   dp->valuerect.r_width;
	if (dp->flags & SHOWRANGE) {
	    dp->max_range_rect.r_height = chrht + TEXT_VALUE_GAP;
	    dp->max_range_rect.r_width = chrwth * (dp->max_range_size + 1);
	} else {
	    dp->max_range_rect.r_height = 0;
	    dp->max_range_rect.r_width = 0;
	}

	/* Create the maximum end box rect */
	dp->max_endbox_rect.r_top = dp->max_range_rect.r_top +
	    dp->max_range_rect.r_height;
	dp->max_endbox_rect.r_left = dp->max_range_rect.r_left + 2;
	if (dp->flags & SHOWENDBOXES) {
	    dp->max_endbox_rect.r_width =
		HorizSliderControl_Height(ip->panel->ginfo) - 4;
	    dp->max_endbox_rect.r_height =
		HorizSliderControl_Width(ip->panel->ginfo) - 5;
	} else {
	    dp->max_endbox_rect.r_width = 0;
	    dp->max_endbox_rect.r_height = 0;
	}

	/* Create the slider rect */
	dp->sliderrect.r_top = dp->max_endbox_rect.r_top +
	    dp->max_endbox_rect.r_height +
	    (dp->flags & SHOWENDBOXES ? TEXT_VALUE_GAP : 0);
	dp->sliderrect.r_left = dp->max_range_rect.r_left;
	dp->sliderrect.r_width = HorizSliderControl_Height(ip->panel->ginfo);
	dp->sliderrect.r_height = dp->width +
	    HorizSliderControl_Width(ip->panel->ginfo);

	/* Define width and height of slider drag box */
	dp->sliderbox.r_width = HorizSliderControl_Height(ip->panel->ginfo);
	dp->sliderbox.r_height = HorizSliderControl_Width(ip->panel->ginfo);

	/* Create the tick rect */
	dp->tickrect.r_top = dp->sliderrect.r_top;
	dp->tickrect.r_left = dp->sliderrect.r_left + dp->sliderrect.r_width;
	if (dp->nticks)
	    dp->tickrect.r_width = 
		HorizSliderControl_Height(ip->panel->ginfo)/2 - 1;
	else
	    dp->tickrect.r_width = 0;
	dp->tickrect.r_height = dp->sliderrect.r_height;

	/* Create the minimum end box rect */
	dp->min_endbox_rect.r_top = dp->sliderrect.r_top +
	    dp->sliderrect.r_height + TEXT_VALUE_GAP;
	dp->min_endbox_rect.r_left = dp->max_range_rect.r_left + 2;
	if (dp->flags & SHOWENDBOXES) {
	    dp->min_endbox_rect.r_width =
		HorizSliderControl_Height(ip->panel->ginfo) - 4;
	    dp->min_endbox_rect.r_height =
		HorizSliderControl_Width(ip->panel->ginfo) - 5;
	} else {
	    dp->min_endbox_rect.r_width = 0;
	    dp->min_endbox_rect.r_height = 0;
	}

	/* Create the minimum range rect */
	dp->min_range_rect.r_left = dp->max_range_rect.r_left;
	dp->min_range_rect.r_top = rect_bottom(&dp->min_endbox_rect) + 1;
	if (dp->flags & SHOWRANGE)
	    dp->min_range_rect.r_width = chrwth * (dp->min_range_size + 1);
	else
	    dp->min_range_rect.r_width = 0;
	dp->min_range_rect.r_height = dp->max_range_rect.r_height;

	/* Update the item value rect */
	ip->value_rect.r_width =
	    MAX(MAX(dp->min_range_rect.r_left + dp->min_range_rect.r_width,
		    dp->tickrect.r_left + dp->tickrect.r_width),
		dp->max_range_rect.r_left + dp->max_range_rect.r_width) -
	    dp->valuerect.r_left;
	ip->value_rect.r_height = dp->min_range_rect.r_top +
	    dp->min_range_rect.r_height - dp->max_range_rect.r_top;

    } else {
	dp->min_range_rect.r_top = dp->valuerect.r_top;
	dp->min_range_rect.r_left = dp->valuerect.r_left +
	   dp->valuerect.r_width;
	if (dp->flags & SHOWRANGE)
	    dp->min_range_rect.r_width =
		chrwth * (dp->min_range_size + 1) + TEXT_VALUE_GAP;
	else
	    dp->min_range_rect.r_width = 0;
	dp->min_range_rect.r_height =
	    HorizSliderControl_Height(ip->panel->ginfo);

	/* Create the minimum end box rect */
	dp->min_endbox_rect.r_top = dp->min_range_rect.r_top + 2;
	dp->min_endbox_rect.r_left = dp->min_range_rect.r_left +
	    dp->min_range_rect.r_width;
	if (dp->flags & SHOWENDBOXES) {
	    dp->min_endbox_rect.r_height =
		HorizSliderControl_Height(ip->panel->ginfo) - 4;
	    dp->min_endbox_rect.r_width =
		HorizSliderControl_Width(ip->panel->ginfo) - 5;
	} else {
	    dp->min_endbox_rect.r_height = 0;
	    dp->min_endbox_rect.r_width = 0;
	}

	/* Create the slider rect */
	dp->sliderrect.r_top = dp->min_range_rect.r_top;
	dp->sliderrect.r_left = dp->min_endbox_rect.r_left +
	    dp->min_endbox_rect.r_width +
	    (dp->flags & SHOWENDBOXES ? TEXT_VALUE_GAP : 0);
	dp->sliderrect.r_width = dp->width + 
	    HorizSliderControl_Width(ip->panel->ginfo);
	dp->sliderrect.r_height = HorizSliderControl_Height(ip->panel->ginfo);

	/* Define width and height of slider drag box */
	dp->sliderbox.r_width = HorizSliderControl_Width(ip->panel->ginfo);
	dp->sliderbox.r_height = HorizSliderControl_Height(ip->panel->ginfo);

	/* Create the tick rect */
	dp->tickrect.r_top = dp->sliderrect.r_top + dp->sliderrect.r_height;
	dp->tickrect.r_left = dp->sliderrect.r_left;
	dp->tickrect.r_width = dp->sliderrect.r_width;
	if (dp->nticks)
	    dp->tickrect.r_height = 
		HorizSliderControl_Height(ip->panel->ginfo)/2 - 1;
	else
	    dp->tickrect.r_height = 0;

	/* Create the maximum end box rect */
	dp->max_endbox_rect.r_top = dp->min_range_rect.r_top + 2;
	dp->max_endbox_rect.r_left = dp->sliderrect.r_left +
	    dp->sliderrect.r_width + TEXT_VALUE_GAP;
	if (dp->flags & SHOWENDBOXES) {
	    dp->max_endbox_rect.r_height =
		HorizSliderControl_Height(ip->panel->ginfo) - 4;
	    dp->max_endbox_rect.r_width =
		HorizSliderControl_Width(ip->panel->ginfo) - 5;
	} else {
	    dp->max_endbox_rect.r_height = 0;
	    dp->max_endbox_rect.r_width = 0;
	}

	/* Create the maximum range rect */
	dp->max_range_rect.r_left = rect_right(&dp->max_endbox_rect) + 1;
	dp->max_range_rect.r_top = dp->min_range_rect.r_top;
	if (dp->flags & SHOWRANGE)
	    dp->max_range_rect.r_width = chrwth * (dp->max_range_size + 1);
	else
	    dp->max_range_rect.r_width = 0;
	dp->max_range_rect.r_height = dp->min_range_rect.r_height;

	/* Update the item value rect */
	ip->value_rect.r_width = rect_right(&dp->max_range_rect) + 1 -
	    dp->valuerect.r_left;
	ip->value_rect.r_height = MAX(dp->valuerect.r_height,
	    dp->sliderrect.r_height + dp->tickrect.r_height);
    }

    ip->value_rect.r_left = dp->valuerect.r_left;

    /* Update the item rect */
    ip->rect = panel_enclosing_rect(&ip->label_rect, &ip->value_rect);
}


static void
slider_layout(ip, deltas)
    Item_info      *ip;
    Rect           *deltas;
{
    Slider_info    *dp = SLIDER_FROM_ITEM(ip);

    if (!created(ip))
	return;
    dp->valuerect.r_left += deltas->r_left;
    dp->valuerect.r_top += deltas->r_top;
    dp->min_endbox_rect.r_left += deltas->r_left;
    dp->min_endbox_rect.r_top += deltas->r_top;
    dp->min_range_rect.r_left += deltas->r_left;
    dp->min_range_rect.r_top += deltas->r_top;
    dp->sliderrect.r_left += deltas->r_left;
    dp->sliderrect.r_top += deltas->r_top;
    dp->tickrect.r_left += deltas->r_left;
    dp->tickrect.r_top += deltas->r_top;
    dp->max_range_rect.r_left += deltas->r_left;
    dp->max_range_rect.r_top += deltas->r_top;
    dp->max_endbox_rect.r_left += deltas->r_left;
    dp->max_endbox_rect.r_top += deltas->r_top;
    if (dp->value_textitem)
	xv_set(dp->value_textitem,
	       PANEL_ITEM_X, dp->valuerect.r_left,
	       PANEL_ITEM_Y, dp->valuerect.r_top,
	       0);
}


static void
slider_paint(ip)
    Item_info      *ip;
{
    Rect           *r;
    Slider_info    *dp = SLIDER_FROM_ITEM(ip);
    Item_info      *tp;
    char            buf[10];
    Xv_Window       pw;
#ifdef OW_I18N
    wchar_t	   buf_wc[10];
#endif /* OW_I18N */

    r = &(ip->label_rect);

    /* paint the label */
    (void) panel_paint_image(ip->panel, &ip->label, r, inactive(ip),
			     ip->color_index);

    /* Paint the text item, if not hidden */
    if (dp->value_textitem) {
	tp = ITEM_PRIVATE(dp->value_textitem);
	if (!hidden(tp))
	    (*tp->ops->paint) (tp);
    }

#ifdef OW_I18N
    /* paint the range */
    if (dp->flags & SHOWRANGE) {
	(void) wsprintf(buf_wc, "%d ", dp->min_value);
	r = &dp->min_range_rect;
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    panel_paint_text(pw, ip->panel->std_font_xid, ip->color_index,
		r->r_left, r->r_top + panel_fonthome(ip->panel->std_font), buf_wc);
	PANEL_END_EACH_PAINT_WINDOW

	(void) wsprintf(buf_wc, " %d", dp->max_value);
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    r = &dp->max_range_rect;
	    panel_paint_text(pw, ip->panel->std_font_xid, ip->color_index,
		r->r_left, r->r_top + panel_fonthome(ip->panel->std_font), buf_wc);
	PANEL_END_EACH_PAINT_WINDOW
    }
#else
    /* paint the range */
    if (dp->flags & SHOWRANGE) {
	(void) sprintf(buf, "%d ", dp->min_value);
	r = &dp->min_range_rect;
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    panel_paint_text(pw, ip->panel->std_font_xid, ip->color_index,
		r->r_left, r->r_top + panel_fonthome(ip->panel->std_font), buf);
	PANEL_END_EACH_PAINT_WINDOW

	(void) sprintf(buf, " %d", dp->max_value);
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    r = &dp->max_range_rect;
	    panel_paint_text(pw, ip->panel->std_font_xid, ip->color_index,
		r->r_left, r->r_top + panel_fonthome(ip->panel->std_font), buf);
	PANEL_END_EACH_PAINT_WINDOW
    }
#endif /*  OW_I18N  */

    /* paint the slider */
    paint_slider(ip, 0);
}


static void
paint_slider(ip, olgx_state)
    Item_info      *ip;
    int		    olgx_state;
{
    register Slider_info    *dp = SLIDER_FROM_ITEM(ip);
    int		    height;
    Xv_Drawable_info *info;
    int		    limit;
    int		    new_value;
    Rect	    old_sliderbox;
    int		    old_value;
    Xv_Window       pw;
    Rect           *r = &dp->sliderrect;
    int		    save_black;
    int		    tick;
    int		    tick_gap;
    int		    width;
    int		    x;
    Drawable	    xid;
    int		    y;

    /* Update the sliderbox location. */
    old_sliderbox = dp->sliderbox;
    new_value = dp->apparent;
    if (new_value < 0)
	new_value = 0;
    else if (new_value > dp->width)
	new_value = dp->width;
    if (vertical(dp)) {
	dp->sliderbox.r_left = dp->min_range_rect.r_left;
	dp->sliderbox.r_top = rect_bottom(r) - new_value -
	   (dp->sliderbox.r_height - 1);
	old_value = rect_bottom(r) - rect_bottom(&old_sliderbox);
	olgx_state |= OLGX_VERTICAL;
    } else {
	dp->sliderbox.r_left = r->r_left + new_value;
	dp->sliderbox.r_top = dp->min_range_rect.r_top;
	old_value = old_sliderbox.r_left - dp->sliderrect.r_left;
	olgx_state |= OLGX_HORIZONTAL;
    }

    if (ip->color_index >= 0)
	save_black = olgx_get_single_color(ip->panel->ginfo, OLGX_BLACK);

    PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	DRAWABLE_INFO_MACRO(pw, info);
	xid = xv_xid(info);
	if (ip->color_index >= 0) {
	    olgx_set_single_color(ip->panel->ginfo, OLGX_BLACK,
				  xv_get(xv_cms(info), CMS_PIXEL,
				  ip->color_index), OLGX_SPECIAL);
	}
	olgx_draw_slider(ip->panel->ginfo, xid,
			 dp->sliderrect.r_left, dp->sliderrect.r_top,
			 dp->width +
			     HorizSliderControl_Width(ip->panel->ginfo),
			 old_value, new_value, olgx_state);
	if ((olgx_state & OLGX_UPDATE) == 0) {
	    if (dp->flags & SHOWENDBOXES) {
		olgx_draw_box(ip->panel->ginfo, xid,
			      dp->min_endbox_rect.r_left,
			      dp->min_endbox_rect.r_top,
			      dp->min_endbox_rect.r_width,
			      dp->min_endbox_rect.r_height,
			      ip->panel->three_d ? 0 : OLGX_ERASE, TRUE);
		olgx_draw_box(ip->panel->ginfo, xid,
			      dp->max_endbox_rect.r_left,
			      dp->max_endbox_rect.r_top,
			      dp->max_endbox_rect.r_width,
			      dp->max_endbox_rect.r_height,
			      ip->panel->three_d ? 0 : OLGX_ERASE, TRUE);
	    }
	    if (dp->nticks) {
		x = dp->tickrect.r_left;
		y = dp->tickrect.r_top;
		if (vertical(dp)) {
		    tick_gap = dp->tickrect.r_height / (dp->nticks - 1);
		    width = dp->tickrect.r_width;
		    height = TICK_THICKNESS;
		    limit = dp->tickrect.r_top+dp->tickrect.r_height -
			TICK_THICKNESS;
		} else {
		    tick_gap = dp->tickrect.r_width / (dp->nticks - 1);
		    width = TICK_THICKNESS;
		    height = dp->tickrect.r_height;
		    limit = dp->tickrect.r_left+dp->tickrect.r_width -
			TICK_THICKNESS;
		}
		for (tick = 1; tick <= dp->nticks; tick++) {
		    olgx_draw_box(ip->panel->ginfo, xid,
				  x, y, width, height, OLGX_NORMAL, FALSE);
		    if (vertical(dp)) {
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
	}
    PANEL_END_EACH_PAINT_WINDOW

    if (ip->color_index >= 0)
	olgx_set_single_color(ip->panel->ginfo, OLGX_BLACK, save_black,
			      OLGX_SPECIAL);
}


static void
check_endbox_entered(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Slider_info    *dp = SLIDER_FROM_ITEM(ip);
    Xv_Drawable_info *info;
    Xv_Window       pw;

    if (rect_includespoint(&dp->min_endbox_rect, event_x(event),
	event_y(event))) {
	dp->flags |= MIN_ENDBOX_SELECTED;
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    DRAWABLE_INFO_MACRO(pw, info);
	    olgx_draw_box(ip->panel->ginfo, xv_xid(info),
			  dp->min_endbox_rect.r_left,
			  dp->min_endbox_rect.r_top,
			  dp->min_endbox_rect.r_width,
			  dp->min_endbox_rect.r_height,
			  OLGX_INVOKED, TRUE);
	PANEL_END_EACH_PAINT_WINDOW
    } else if (rect_includespoint(&dp->max_endbox_rect, event_x(event),
	event_y(event))) {
	dp->flags |= MAX_ENDBOX_SELECTED;
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    DRAWABLE_INFO_MACRO(pw, info);
	    olgx_draw_box(ip->panel->ginfo, xv_xid(info),
			  dp->max_endbox_rect.r_left,
			  dp->max_endbox_rect.r_top,
			  dp->max_endbox_rect.r_width,
			  dp->max_endbox_rect.r_height,
			  OLGX_INVOKED, TRUE);
	PANEL_END_EACH_PAINT_WINDOW
    }
}


static void
slider_begin_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Slider_info    *dp = SLIDER_FROM_ITEM(ip);

    /*
     * We only begin preview if the user action clicks on the slider, not
     * when he/she drags in the slider w/ a button down
     */
    if (event_action(event) != ACTION_SELECT) {
	return;
    }
    if (rect_includespoint(&dp->sliderbox, event_x(event), event_y(event))) {
	(ip->panel)->SliderActive = TRUE;
	if (vertical(dp))
	    dp->value_offset = rect_bottom(&dp->sliderbox) - event_y(event) + 1;
	else
	    dp->value_offset = event_x(event) - dp->sliderbox.r_left + 1;
	/* save status of print value */
	dp->restore_print_value = dp->use_print_value;

	/* update the preview */
	slider_update_preview(ip, event);
    } else
	check_endbox_entered(ip, event);
}

static void
slider_update_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Slider_info    *dp = SLIDER_FROM_ITEM(ip);
    Xv_Drawable_info *info;
    register int    new;
    Xv_Window       pw;
    Rect            r;
    char            buf[20];

    if ((dp->flags & MIN_ENDBOX_SELECTED) && 
	!(rect_includespoint(&dp->min_endbox_rect, event_x(event),
	event_y(event)))) {
	dp->flags &= ~MIN_ENDBOX_SELECTED;
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    DRAWABLE_INFO_MACRO(pw, info);
	    olgx_draw_box(ip->panel->ginfo, xv_xid(info),
			  dp->min_endbox_rect.r_left,
			  dp->min_endbox_rect.r_top,
			  dp->min_endbox_rect.r_width,
			  dp->min_endbox_rect.r_height,
			  OLGX_ERASE, TRUE);
	PANEL_END_EACH_PAINT_WINDOW
	return;
    } else if ((dp->flags & MAX_ENDBOX_SELECTED) &&
	!(rect_includespoint(&dp->max_endbox_rect, event_x(event),
	event_y(event)))) {
	dp->flags &= ~MAX_ENDBOX_SELECTED;
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    DRAWABLE_INFO_MACRO(pw, info);
	    olgx_draw_box(ip->panel->ginfo, xv_xid(info),
			  dp->max_endbox_rect.r_left,
			  dp->max_endbox_rect.r_top,
			  dp->max_endbox_rect.r_width,
			  dp->max_endbox_rect.r_height,
			  OLGX_ERASE, TRUE);
	PANEL_END_EACH_PAINT_WINDOW
	return;
    } else
	check_endbox_entered(ip, event);

    if (!(ip->panel)->SliderActive)
	return;

    r = dp->sliderrect;
    rect_marginadjust(&r, -1);
    if (vertical(dp))
	new = rect_bottom(&r) - event_y(event) - dp->value_offset;
    else
	new = event_x(event) - r.r_left - dp->value_offset;
    if (new == dp->apparent)
	return;			/* state and display both correct */

    dp->apparent = new;
    dp->use_print_value = FALSE;
    paint_slider(ip, OLGX_UPDATE);
    if (dp->flags & SHOWVALUE) {
	(void) sprintf(buf, "%d", itoe(dp, dp->apparent));
	xv_set(dp->value_textitem, PANEL_VALUE, buf, 0);
    }
    /* ONLY NOTIFY IF EXTERNAL VALUE HAS CHANGED? */
    if (dp->flags & CONTINUOUS)
	(*ip->notify) (ITEM_PUBLIC(ip), itoe(dp, dp->apparent), event);
}


static void
slider_cancel_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Slider_info    *dp = SLIDER_FROM_ITEM(ip);
    char            buf[20];

    (ip->panel)->SliderActive = FALSE;
    dp->flags &= ~(MIN_ENDBOX_SELECTED | MAX_ENDBOX_SELECTED);

    if (dp->apparent != dp->actual) {
	dp->apparent = dp->actual;
	dp->use_print_value = dp->restore_print_value;
	if (dp->flags & CONTINUOUS)
	    (*ip->notify) (ITEM_PUBLIC(ip), itoe(dp, dp->actual), event);
	paint_slider(ip, OLGX_UPDATE);
	if (dp->flags & SHOWVALUE) {
	    sprintf(buf, "%d", itoe(dp, dp->apparent));
	    xv_set(dp->value_textitem, PANEL_VALUE, buf, 0);
	}
    }
}


static void
slider_accept_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    char            buf[20];
    int		    delta=0;  /* 0=> no change, +1= increment, -1= decrement */
    Slider_info    *dp = SLIDER_FROM_ITEM(ip);
    int		    mouse_left;
    int		    mouse_top;

    if (dp->flags & MIN_ENDBOX_SELECTED) {
	dp->flags &= ~MIN_ENDBOX_SELECTED;
	if (rect_includespoint(&dp->min_endbox_rect, event_x(event),
	    event_y(event))) {
	    dp->use_print_value = FALSE;
	    dp->actual = dp->apparent = 0;   /* in pixels */
	    panel_clear_rect(ip->panel, dp->sliderrect);
	    paint_slider(ip, 0);
	    if (dp->flags & SHOWVALUE) {
		sprintf(buf, "%d", dp->min_value);
		xv_set(dp->value_textitem, PANEL_VALUE, buf, 0);
	    }
	    (*ip->notify) (ITEM_PUBLIC(ip), dp->min_value, event);
	}
	return;
    } else if (dp->flags & MAX_ENDBOX_SELECTED) {
	dp->flags &= ~MAX_ENDBOX_SELECTED;
	if (rect_includespoint(&dp->max_endbox_rect, event_x(event),
	    event_y(event))) {
	    dp->use_print_value = FALSE;
	    dp->actual = dp->apparent = dp->width;   /* in pixels */
	    panel_clear_rect(ip->panel, dp->sliderrect);
	    paint_slider(ip, 0);
	    if (dp->flags & SHOWVALUE) {
		sprintf(buf, "%d", dp->max_value);
		xv_set(dp->value_textitem, PANEL_VALUE, buf, 0);
	    }
	    (*ip->notify) (ITEM_PUBLIC(ip), dp->max_value, event);
	}
	return;
    }

    (ip->panel)->SliderActive = FALSE;

    if (dp->apparent != dp->actual) {
	dp->actual = dp->apparent;
	/* print_value is no longer valid */
	dp->use_print_value = FALSE;
	(*ip->notify) (ITEM_PUBLIC(ip), itoe(dp, dp->actual), event);
    } else if (vertical(dp)) {
	if (event_y(event) >= dp->sliderrect.r_top &&
	    event_y(event) < dp->sliderbox.r_top) {
	    /* Increment slider by 1.
	     * Position pointer at the top end of the slider bar.
	     */
	    delta = 1;
	    mouse_top = dp->sliderrect.r_top;
	} else if (event_y(event) > rect_bottom(&dp->sliderbox) &&
	    event_y(event) <= rect_bottom(&dp->sliderrect)) {
	    /* Decrement slider by 1.
	     * Position pointer at the bottom end of the slider bar.
	     */
	    delta = -1;
	    mouse_top = rect_bottom(&dp->sliderrect);
	}
	mouse_left = dp->sliderbox.r_left + dp->sliderbox.r_width/2;
    } else {
	if (event_x(event) >= dp->sliderrect.r_left &&
	    event_x(event) < dp->sliderbox.r_left) {
	    /* Decrement slider by 1.
	     * Position pointer at the left end of the slider bar.
	     */
	    delta = -1;
	    mouse_left = dp->sliderrect.r_left;
	} else if (event_x(event) > rect_right(&dp->sliderbox) &&
	    event_x(event) <= rect_right(&dp->sliderrect)) {
	    /* Increment slider by 1.
	     * Position pointer at the right end of the slider bar.
	     */
	    delta = 1;
	    mouse_left = rect_right(&dp->sliderrect);
	}
	mouse_top = dp->sliderbox.r_top + dp->sliderbox.r_height/2;
    }
    if (delta) {
	/* Note: etoi and itoe enforce min/max values */
	dp->print_value =
	    (dp->use_print_value ? dp->print_value : itoe(dp, dp->actual)) +
	    delta;
	if (dp->print_value < dp->min_value)
	    dp->print_value = dp->min_value;
	else if (dp->print_value > dp->max_value)
	    dp->print_value = dp->max_value;
	dp->use_print_value = TRUE;
	dp->apparent = dp->actual = etoi(dp, dp->print_value);  /* in pixels */
	paint_slider(ip, OLGX_UPDATE);
	if (dp->flags & SHOWVALUE) {
	    sprintf(buf, "%d", dp->print_value);
	    xv_set(dp->value_textitem, PANEL_VALUE, buf, 0);
	}
	xv_set(PANEL_PUBLIC(ip->panel),
	       WIN_MOUSE_XY, mouse_left, mouse_top,
	       0);
	(*ip->notify) (ITEM_PUBLIC(ip), dp->print_value, event);
    }
}


/*
 * Convert external value (client units) to internal value (pixels).
 */
static int
etoi(dp, value)
    Slider_info    *dp;
    int             value;
{
    if (value <= dp->min_value)
	return (0);

    if (value >= dp->max_value)
	return (dp->width);

    return (panel_round(((value - dp->min_value) * dp->width),
		        (dp->max_value - dp->min_value + 1)));
}

/*
 * Convert internal value (pixels) to external value (client units).
 */
static int
itoe(dp, value)
    Slider_info    *dp;
    int             value;
{
    /* use the print value if valid */
    if (dp->use_print_value)
	return dp->print_value;

    if (value <= 0)
	return (dp->min_value);

    if (value >= dp->width - 1)
	return (dp->max_value);

    return (dp->min_value +
	    (value * (dp->max_value - dp->min_value + 1)) / dp->width);
}

Pkg_private int
panel_round(x, y)
    register int    x, y;
{
    register int    z, rem;
    register short  is_neg = FALSE;

    if (x < 0) {
	x = -x;
	if (y < 0)
	    y = -y;
	else
	    is_neg = TRUE;
    } else if (y < 0) {
	y = -y;
	is_neg = TRUE;
    }
    z = x / y;
    rem = x % y;
    /* round up if needed */
    if (2 * rem >= y)
	if (is_neg)
	    z--;
	else
	    z++;

    return (is_neg ? -z : z);
}

static Panel_setting
get_value(item_public, event)
    Panel_item     item_public;
    Event          *event;
{
    char            buf[20];
    int		    value_changed = FALSE;
    Item_info      *ip = (Item_info *) xv_get(item_public, PANEL_CLIENT_DATA);
    Slider_info    *dp = SLIDER_FROM_ITEM(ip);
    int             value;

    value = atoi((char *) xv_get(item_public, PANEL_VALUE));
    if (value < dp->min_value) {
	value = dp->min_value;
	value_changed = TRUE;
    } else if (value > dp->max_value) {
	value = dp->max_value;
	value_changed = TRUE;
    }
    if (value_changed) {
	sprintf(buf, "%d", value);
	xv_set(item_public, PANEL_VALUE, buf, 0);
    }
    dp->apparent = dp->actual = etoi(dp, value);
    dp->use_print_value = TRUE;
    dp->print_value = value;
    paint_slider(ip, OLGX_UPDATE);

    (*ip->notify) (ITEM_PUBLIC(ip), value, event);

    return panel_text_notify(item_public, event);
}

