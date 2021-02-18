#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_btn.c 50.7 90/12/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/panel_impl.h>
#include <xview/font.h>
#include <xview/openmenu.h>
#include <xview/pixwin.h>
#include <xview/server.h>
#include <xview/svrimage.h>
#include <xview/cms.h>
#include <X11/Xlib.h>
#include <xview_private/draw_impl.h>
#include <xview_private/pw_impl.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N


#define BUTTON_PRIVATE(item) \
	XV_PRIVATE(Button_info, Xv_panel_button, item)
#define BUTTON_FROM_ITEM(ip)	BUTTON_PRIVATE(ITEM_PUBLIC(ip))


static void     btn_begin_preview(), btn_cancel_preview(), btn_accept_preview(),
                btn_accept_menu(), btn_paint();


static Panel_ops ops = {
    panel_default_handle_event,		/* handle_event() */
    btn_begin_preview,			/* begin_preview() */
    (void (*) ()) panel_nullproc,	/* update_preview() */
    btn_cancel_preview,			/* cancel_preview() */
    btn_accept_preview,			/* accept_preview() */
    btn_accept_menu,			/* accept_menu() */
    (void (*) ()) panel_nullproc,	/* accept_key() */
    btn_paint,				/* paint() */
    (void (*) ()) panel_nullproc,	/* remove() */
    (void (*) ()) panel_nullproc,	/* restore() */
    (void (*) ()) panel_nullproc,	/* layout() */
    (void (*) ()) panel_nullproc,	/* accept_kbd_focus() */
    (void (*) ()) panel_nullproc	/* yield_kbd_focus() */
};

typedef struct button_info {
    Panel_item      public_self;/* back pointer to object */
    int		    clear_button_rect;
    Server_image    pin_in_image;
}               Button_info;


/*
 * Function declarations
 */
Xv_private void menu_save_pin_window_rect();

Pkg_private int panel_button_init();
Pkg_private Item_info *panel_finditem();

static void     button_menu_busy_proc();
static void     button_menu_done_proc();
static Menu     generate_menu();
static void     panel_paint_button_image();
static void     take_down_cmd_frame();
#ifdef OW_I18N
extern wchar_t  null_string_wc[];
#endif /* OW_I18N */


/* ------------------------------------------------------------------------- */

/* ARGSUSED */
Pkg_private int
panel_button_init(panel_public, item_public, avlist)
    Panel           panel_public;
    Panel_item      item_public;
    Attr_avlist     avlist;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);
    register Item_info *ip = ITEM_PRIVATE(item_public);
    Xv_panel_button *item_object = (Xv_panel_button *) item_public;
    Button_info    *dp;

    dp = xv_alloc(Button_info);

    item_object->private_data = (Xv_opaque) dp;
    dp->public_self = item_public;

    if (ops_set(ip))
	*ip->ops = ops;		/* copy the button ops vector */
    else
	ip->ops = &ops;		/* use the static button ops vector */
    if (panel->event_proc)
	ip->ops->handle_event = (void (*) ()) panel->event_proc;
    ip->item_type = PANEL_BUTTON_ITEM;

    /*
     * BUG: since set is not called yet, append will not have the right size
     * for the item.
     */
    panel_append(ip);

    return XV_OK;
}


/*ARGSUSED*/
Pkg_private int
panel_button_destroy(item_public, status)
    Panel_item      item_public;
    Destroy_status  status;
{
    Button_info	   *dp = BUTTON_PRIVATE(item_public);

    if ((status == DESTROY_CHECKING) || (status == DESTROY_SAVE_YOURSELF))
	return XV_OK;
    free(dp);
    return XV_OK;
}


static void
btn_paint(ip)
    Item_info      *ip;
{
    panel_paint_button_image(ip, &ip->label, &ip->label_rect,
			     inactive(ip), FALSE, ip->color_index, ip->menu,
			     ip->label_width, 0, FALSE);
}


/* ARGSUSED */
static void
btn_begin_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Menu            default_menu;
    Menu_item       default_menu_item;
    Button_info	   *dp = BUTTON_FROM_ITEM(ip);
    int		    height;
    Panel_image     image;
    Xv_Drawable_info *info;
    Xv_opaque	    label;
    Menu_class      menu_class;
    Menu_item	  (*mi_gen_proc)();
    int		    olgx_state = OLGX_NORMAL;
    int             pin_is_default;	/* boolean */
    Pixlabel	    pixlabel;
    int		    pushpin_width;
    Xv_Window       pw;
    Menu            submenu;

    dp->clear_button_rect = FALSE;
    if (ip->menu) {

	/* Get the Server Image or string of the default item */
	submenu = ip->menu;
	if ((default_menu = generate_menu(submenu)) == NULL)
	    return;
	default_menu_item = (Menu_item) xv_get(default_menu, MENU_DEFAULT_ITEM);
	if (!default_menu_item)
	    /* Invalid menu attached: ignore begin_preview request */
	    return;
	mi_gen_proc = (Menu_item (*) ()) xv_get(default_menu_item,
	    MENU_GEN_PROC);
	if (mi_gen_proc)
	    default_menu_item = mi_gen_proc(default_menu_item, MENU_DISPLAY);
	if (!default_menu_item)
	    /* Invalid menu attached: ignore begin_preview request */
	    return;
	pin_is_default = xv_get(default_menu, MENU_PIN) &&
	    xv_get(default_menu_item, MENU_TITLE);
	height = 0;
#ifdef OW_I18N
	if (pin_is_default) {
	    image.im_type = PIT_STRING;
	    image_string_wc(&image) = null_string_wc;
	} else if (!(image_string_wc(&image) = (wchar_t *) xv_get(default_menu_item,
		MENU_STRING_WCS))) {
	    olgx_state |= OLGX_LABEL_IS_PIXMAP;
	    image.im_type = PIT_SVRIM;
	    image_svrim(&image) = xv_get(default_menu_item, MENU_IMAGE);
	    pixlabel.pixmap = (XID) xv_get(image_svrim(&image), XV_XID);
	    pixlabel.width = ((Pixrect *)image_svrim(&image))->pr_width;
	    pixlabel.height = ((Pixrect *)image_svrim(&image))->pr_height;
	    label = (Xv_opaque) &pixlabel;
	    height = Button_Height(ip->panel->ginfo);
	    dp->clear_button_rect = TRUE;
	} else {
	    image.im_type = PIT_STRING;
	    image_font(&image) = image_font(&ip->label);
	    image_bold(&image) = image_bold(&ip->label);
	    label = (Xv_opaque) image_string_wc(&image);
	}
#else
	if (pin_is_default) {
	    image.im_type = PIT_STRING;
	    image_string(&image) = NULL;
	} else if (!(image_string(&image) = (char *) xv_get(default_menu_item,
		MENU_STRING))) {
	    olgx_state |= OLGX_LABEL_IS_PIXMAP;
	    image.im_type = PIT_SVRIM;
	    image_svrim(&image) = xv_get(default_menu_item, MENU_IMAGE);
	    pixlabel.pixmap = (XID) xv_get(image_svrim(&image), XV_XID);
	    pixlabel.width = ((Pixrect *)image_svrim(&image))->pr_width;
	    pixlabel.height = ((Pixrect *)image_svrim(&image))->pr_height;
	    label = (Xv_opaque) &pixlabel;
	    height = Button_Height(ip->panel->ginfo);
	    dp->clear_button_rect = TRUE;
	} else {
	    image.im_type = PIT_STRING;
	    image_font(&image) = image_font(&ip->label);
	    image_bold(&image) = image_bold(&ip->label);
	    label = (Xv_opaque) image_string(&image);
	}
#endif /* OW_I18N */
	/*
	 * Replace the button stack with the default item Server Image or
	 * string.  If it's a string, truncate the string to the width of the
	 * original button stack string.
	 */
	menu_class = (Menu_class) xv_get(default_menu, MENU_CLASS);
	switch (menu_class) {
	  case MENU_COMMAND:
	    panel_paint_button_image(ip, &image, &ip->label_rect, FALSE, TRUE,
		ip->color_index, (Xv_opaque) NULL, ip->label_width, height,
		FALSE);
	    if (pin_is_default) {
		if (xv_get(default_menu_item, MENU_INACTIVE)) {
		    olgx_state = OLGX_PUSHPIN_OUT | OLGX_DEFAULT |
			OLGX_INACTIVE;
		    pushpin_width = 26; /* ??? point sizes != 12 */
		} else {
		    olgx_state = OLGX_PUSHPIN_IN;
		    pushpin_width = 13; /* ??? point sizes != 12 */
		}
		PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
		    DRAWABLE_INFO_MACRO(pw, info);
		    olgx_draw_pushpin(ip->panel->ginfo, xv_xid(info),
			ip->label_rect.r_left + (ip->label_rect.r_width -
			    pushpin_width) /2,
			ip->label_rect.r_top + 1, olgx_state);
		PANEL_END_EACH_PAINT_WINDOW
	    }
	    break;

	  case MENU_CHOICE:
	  case MENU_TOGGLE:
	    if (menu_class == MENU_CHOICE ||
		!xv_get(default_menu_item, MENU_SELECTED))
		/* Choice item: always selected
		 * Toggle item: selected if previous state is "not selected"
		 */
		olgx_state |= OLGX_INVOKED;
	    panel_clear_rect(ip->panel, ip->label_rect);
	    PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
		DRAWABLE_INFO_MACRO(pw, info);
		olgx_draw_choice_item(ip->panel->ginfo, xv_xid(info),
		    ip->label_rect.r_left, ip->label_rect.r_top,
		    ip->label_rect.r_width, ip->label_rect.r_height,
		    label, olgx_state);
	    PANEL_END_EACH_PAINT_WINDOW
	    dp->clear_button_rect = TRUE;
	}
    } else
	/* Invert the button interior */
	panel_paint_button_image(ip, &ip->label, &ip->label_rect,
	    inactive(ip), TRUE, ip->color_index, ip->menu, ip->label_width, 0,
	    FALSE);
}

/* ARGSUSED */
static void
btn_cancel_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Button_info	   *dp = BUTTON_FROM_ITEM(ip);
    Menu	    default_menu;
    Menu_item	    default_menu_item;
    Menu_item	  (*mi_gen_proc)();

    if (dp->clear_button_rect) {
	dp->clear_button_rect = FALSE;
	panel_clear_rect(ip->panel, ip->label_rect);
    }

    /*
     * If menu button or event is SELECT-down, then repaint button.
     */
    if (ip->menu || action_select_is_down(event))
	panel_paint_button_image(ip, &ip->label, &ip->label_rect,
	    inactive(ip), FALSE, ip->color_index, ip->menu, ip->label_width, 0,
	    FALSE);
    if (ip->menu) {
	if ((default_menu = generate_menu(ip->menu)) == NULL)
	    return;
	default_menu_item = (Menu_item) xv_get(default_menu, MENU_DEFAULT_ITEM);
	if (!default_menu_item)
	    return;
	mi_gen_proc = (Menu_item (*) ()) xv_get(default_menu_item,
	    MENU_GEN_PROC);
	if (mi_gen_proc)
	    mi_gen_proc(default_menu_item, MENU_DISPLAY_DONE);
    }
}


static void
btn_accept_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Button_info	   *dp = BUTTON_FROM_ITEM(ip);

    if (dp->clear_button_rect) {
	dp->clear_button_rect = FALSE;
	panel_clear_rect(ip->panel, ip->label_rect);
    }

    /*
     * If button, then undo the inversion. If button stack, then repaint
     * original button stack with busy feedback.  Sync server to force
     * busy pattern to be painted before entering notify procedure.
     */
    panel_paint_button_image(ip, &ip->label, &ip->label_rect,
	inactive(ip), !ip->menu, ip->color_index, ip->menu, ip->label_width, 0,
	TRUE);
    xv_set(XV_SERVER_FROM_WINDOW(PANEL_PUBLIC(ip->panel)),
	   SERVER_SYNC, FALSE,
	   0);

    panel_btn_accepted(ip, event);

    if (!hidden(ip))
	panel_paint_button_image(ip, &ip->label, &ip->label_rect,
				 inactive(ip), FALSE, ip->color_index, ip->menu,
				 ip->label_width, 0, FALSE);
}


static void
btn_accept_menu(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Xv_Window       paint_window = event_window(event);
    Rect	    rect;

    if (ip->menu == NULL || paint_window == NULL)
	return;

    /*
     * Notify the client.  This callback allows the client to dynamically
     * generate the menu.
     */
    (*ip->notify) (ITEM_PUBLIC(ip), event);

    /*
     * Save public panel handle and current menu color and done proc.
     * Switch to button's color and menu done proc.
     */
    xv_set(ip->menu,
	   XV_KEY_DATA, PANEL_BUTTON, ITEM_PUBLIC(ip),
	   XV_KEY_DATA, MENU_DONE_PROC, xv_get(ip->menu, MENU_DONE_PROC),
	   XV_KEY_DATA, MENU_COLOR, xv_get(ip->menu, MENU_COLOR),
	   MENU_BUSY_PROC, button_menu_busy_proc,
	   MENU_DONE_PROC, button_menu_done_proc,
	   MENU_COLOR, ip->color_index,
	   0);

    /* Invert the button interior */
    panel_paint_button_image(ip, &ip->label, &ip->label_rect,
	inactive(ip), TRUE, ip->color_index, ip->menu, ip->label_width, 0,
	FALSE);

    /* Show menu */
    rect = ip->label_rect;
    if (ip->label_width)
	rect.r_width = ip->label_width;
    menu_show(ip->menu, paint_window, event,
#ifdef MENU_PREVIEWING
	      MENU_ENABLE_RECT, &rect,
#else
	      MENU_POSITION_RECT, &rect,
#endif				/* MENU_PREVIEWING */
	      MENU_PULLDOWN, ip->panel->layout == PANEL_HORIZONTAL,
	      0);
}


static void
button_menu_busy_proc(menu)
    Menu	    menu;
{
    Panel_item	    item = xv_get(menu, XV_KEY_DATA, PANEL_BUTTON);
    Item_info	   *ip = ITEM_PRIVATE(item);

    panel_paint_button_image(ip, &ip->label, &ip->label_rect,
	inactive(ip), !ip->menu, ip->color_index, ip->menu, ip->label_width, 0,
	TRUE);
}


static void
button_menu_done_proc(menu, result)
    Menu            menu;
    Xv_opaque       result;
{
    void            (*orig_done_proc) ();	/* original menu-done
						 * procedure */
    Panel_item	    item = xv_get(menu, XV_KEY_DATA, PANEL_BUTTON);
    Item_info	   *ip = ITEM_PRIVATE(item);
    Panel_info     *panel = ip->panel;
    Panel           panel_public = PANEL_PUBLIC(panel);

    /* Restore menu button to normal state */
    panel_paint_button_image(ip, &ip->label, &ip->label_rect,
			     inactive(ip), FALSE, ip->color_index, ip->menu,
			     ip->label_width, 0, FALSE);

    if (xv_get(menu, MENU_NOTIFY_STATUS) == XV_OK)
	take_down_cmd_frame(panel_public);

    panel->current = NULL;

    /* Restore menu color and original menu done proc. */
    orig_done_proc = (void (*) ()) xv_get(menu, XV_KEY_DATA, MENU_DONE_PROC);
    xv_set(menu,
	MENU_DONE_PROC, orig_done_proc,
	MENU_COLOR, xv_get(menu, XV_KEY_DATA, MENU_COLOR),
	0);

    /* Invoke original menu done proc (if any) */
    if (orig_done_proc)
	(orig_done_proc) (menu, result);
}


static void
panel_paint_button_image(ip, image, rect, inactive_button, invert, color_index,
			 menu, width, height, busy)
    Item_info      *ip;
    Panel_image    *image;
    register Rect  *rect;
    int             inactive_button;
    int             invert;
    int             color_index;
    Xv_opaque       menu;	/* NULL => no menu attached to button */
    int             width;	/* 0 => no truncation necessary; other =
				 * width of string or Server Image */
    int		    height;	/* 0 => use height of image */
    int		    busy;
{
    int             default_button;
    Xv_Drawable_info *info;
    Xv_opaque	    label;
    Panel_info     *panel = ip->panel;
    Pixlabel	    pixlabel;
    Xv_Window       pw;
    int		    save_black;
    int		    state;

    default_button = ITEM_PUBLIC(ip) == panel->default_item;
    state = busy ? OLGX_BUSY :
	    invert ? OLGX_INVOKED :
	    panel->three_d ? OLGX_NORMAL :
	    OLGX_NORMAL | OLGX_ERASE;
    if (image_type(image) == PIT_STRING) {
	height = 0;   /* not used by olgx_draw_button */
#ifdef	OW_I18N
	label = (Xv_opaque) image_string_wc(image);
#else
	label = (Xv_opaque) image_string(image);
#endif	OW_I18N
    } else {
	if (height == 0)
	    height = ((Pixrect *)image_svrim(image))->pr_height +
		OLGX_VAR_HEIGHT_BTN_MARGIN;
	pixlabel.pixmap = (XID) xv_get(image_svrim(image), XV_XID);
	pixlabel.width = ((Pixrect *)image_svrim(image))->pr_width;
	pixlabel.height = ((Pixrect *)image_svrim(image))->pr_height;
	label = (Xv_opaque) &pixlabel;
	state |= OLGX_LABEL_IS_PIXMAP;
    }
    if (is_menu_item(ip)) {
	state |= OLGX_MENU_ITEM;
	if (!busy && !invert)
	    state |= OLGX_ERASE;
    }
    if (default_button)
	state |= OLGX_DEFAULT;
    if (inactive_button)
	state |= OLGX_INACTIVE;
    if (menu)  {
	if (panel->layout == PANEL_VERTICAL)
	    state |= OLGX_HORIZ_MENU_MARK;
	else
	    state |= OLGX_VERT_MENU_MARK;
    }
    PANEL_EACH_PAINT_WINDOW(panel, pw)
	DRAWABLE_INFO_MACRO(pw, info);
	if (color_index >= 0) {
	    save_black = olgx_get_single_color(panel->ginfo, OLGX_BLACK);
	    olgx_set_single_color(panel->ginfo, OLGX_BLACK,
				  xv_get(xv_cms(info), CMS_PIXEL, color_index),
				  OLGX_SPECIAL);
	}
	olgx_draw_button(panel->ginfo, xv_xid(info), rect->r_left, rect->r_top,
			 width ? width : rect->r_width, height, label, state);
	if (color_index >= 0)
	    olgx_set_single_color(panel->ginfo, OLGX_BLACK, save_black,
				  OLGX_SPECIAL);
    PANEL_END_EACH_PAINT_WINDOW
}


static void
take_down_cmd_frame(panel_public)
    Panel           panel_public;
{
    Frame           frame;

    /*
     * If this is a command frame, and the pin is out, then take down the
     * window.  Note: The frame code checks to see if the pin is out.
     */
    frame = xv_get(panel_public, WIN_FRAME);
    if (xv_get(frame, XV_IS_SUBTYPE_OF, FRAME_CMD)) {
	menu_save_pin_window_rect(frame);
	xv_set(frame, XV_SHOW, FALSE, 0);
    }
}


static          Menu
generate_menu(menu)
    Menu            menu;
{
    Menu            gen_menu, (*gen_proc) ();	/* generated menu and
						 * procedure */

    if (gen_proc = (Menu(*) ()) xv_get(menu, MENU_GEN_PROC)) {
	gen_menu = gen_proc(menu, MENU_DISPLAY);
	if (gen_menu == NULL) {
	    xv_error(menu,
		     ERROR_STRING,
#ifdef	OW_I18N
		 XV_I18N_MSG("xv_messages", "begin_preview: menu's gen_proc failed to generate a menu"),
#else
		 "begin_preview: menu's gen_proc failed to generate a menu",
#endif	OW_I18N
		     ERROR_PKG, PANEL,
		     0);
	    return (NULL);
	}
	return (gen_menu);
    } else
	return (menu);
}


Pkg_private void
panel_btn_accepted(ip, event)
    Item_info	   *ip;
    Event	   *event;
{
    Menu            default_menu, submenu, topmenu;
    Menu_item       default_menu_item;
    int             menu_depth = 0;
    void            (*pin_proc) ();	/* pin procedure for default menu */
    int             pin_is_default;	/* boolean */
    int             notify_status;	/* XV_OK or XV_ERROR */

    /* notify the client */
    ip->notify_status = XV_OK;
    (*ip->notify) (ITEM_PUBLIC(ip), event);

    if ((ip->menu) && (submenu = topmenu = generate_menu(ip->menu))) {
	/* Select the default entry */
	menu_select_default(submenu);
	do {
	    menu_depth++;
	    default_menu = submenu;
	    default_menu_item = (Menu_item) xv_get(default_menu,
						   MENU_DEFAULT_ITEM);
	} while (default_menu_item &&
		 (submenu = (Menu) xv_get(default_menu_item, MENU_PULLRIGHT)));
	if (default_menu_item) {
	    pin_is_default = xv_get(default_menu, MENU_PIN) &&
		xv_get(default_menu_item, MENU_TITLE);
	    if (pin_is_default) {
		if (xv_get(default_menu_item, MENU_INACTIVE))
		    notify_status = XV_ERROR;
		else {
		    pin_proc = (void (*) ()) xv_get(default_menu,
						    MENU_PIN_PROC);
		    pin_proc(default_menu, event_x(event), event_y(event));
		    notify_status = XV_OK;
		}
	    } else {
		/*
		 * Invoke the appropriate notify procedure(s) for the menu
		 * selection.
		 */
		menu_return_default(topmenu, menu_depth);
		notify_status = (int) xv_get(topmenu, MENU_NOTIFY_STATUS);
	    }
	} else
	    notify_status = XV_ERROR;
    } else
	notify_status = ip->notify_status;

    if (notify_status == XV_OK)
	take_down_cmd_frame(PANEL_PUBLIC(ip->panel));
}
