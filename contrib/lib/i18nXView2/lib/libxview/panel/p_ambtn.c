#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_ambtn.c 50.1 90/12/12";
#endif
#endif

/*
 *	(c) Copyright 1990 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/panel_impl.h>
#include <xview/openmenu.h>
#include <xview_private/draw_impl.h>

/*
 * Abbreviated Menu Button Panel Item
 */


#define AMBTN_PRIVATE(item) \
	XV_PRIVATE(Ambtn_info, Xv_panel_ambtn, item)
#define AMBTN_FROM_ITEM(ip)	AMBTN_PRIVATE(ITEM_PUBLIC(ip))

/* Declare all ambtn item handler procedures used in the Ops Vector Table */
static void     ambtn_begin_preview(), ambtn_cancel_preview(),
		ambtn_accept_preview(), ambtn_accept_menu(),
		ambtn_paint();

/* Local routines */
static void ambtn_menu_busy_proc();
static void ambtn_menu_done_proc();
static void ambtn_paint_value();


/*
 * Panel Operations Vector Table for this item.
 * If any of the operations do not apply, then
 * use "(void (*) ()) panel_nullproc" as the
 * handler.
 */
static Panel_ops ops = {
    panel_default_handle_event,		/* handle_event() */
    ambtn_begin_preview,		/* begin_preview() */
    ambtn_begin_preview,		/* update_preview() */
    ambtn_cancel_preview,		/* cancel_preview() */
    ambtn_accept_preview,		/* accept_preview() */
    ambtn_accept_menu,			/* accept_menu() */
    (void (*) ()) panel_nullproc,	/* accept_key() */
    ambtn_paint,			/* paint() */
    (void (*) ()) panel_nullproc,	/* remove() */
    (void (*) ()) panel_nullproc,	/* restore() */
    (void (*) ()) panel_nullproc,	/* layout() */
    (void (*) ()) panel_nullproc,	/* accept_kbd_focus() */
    (void (*) ()) panel_nullproc	/* yield_kbd_focus() */
};


typedef struct ambtn_info {
    Panel_item      public_self;/* back pointer to object */
    int		    invoked;
} Ambtn_info;


/* Item specific definitions */
#define AMB_OFFSET	4	/* gap between label and Abbrev. Menu Button */


/* ========================================================================= */

/* -------------------- XView Functions  -------------------- */
/*ARGSUSED*/
Pkg_private int
panel_ambtn_init(panel_public, item_public, avlist)
    Panel           panel_public;
    Panel_item      item_public;
    Attr_avlist     avlist;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);
    register Item_info *ip = ITEM_PRIVATE(item_public);
    Xv_panel_ambtn *item_object = (Xv_panel_ambtn *) item_public;
    Ambtn_info	   *dp;

    dp = xv_alloc(Ambtn_info);

    item_object->private_data = (Xv_opaque) dp;
    dp->public_self = item_public;
    
    if (ops_set(ip))
	*ip->ops = ops;		/* copy the ambtn ops vector */
    else
	ip->ops = &ops;		/* use the static ambtn ops vector */
    if (panel->event_proc)
	ip->ops->handle_event = (void (*) ()) panel->event_proc;
    ip->item_type = PANEL_ABBREV_MENU_BUTTON_ITEM;
    panel_set_bold_label_font(ip);

    panel_append(ip);

    return XV_OK;
}


Pkg_private Xv_opaque
panel_ambtn_set_avlist(item_public, avlist)
    Panel_item	    item_public;
    Attr_avlist	    avlist;
{
    Item_info	   *ip = ITEM_PRIVATE(item_public);
    Xv_opaque       result;

    /* Call generic item set code to handle layout attributes.
     * Prevent panel_redisplay_item from being called in item_set_avlist.
     */
    if (*avlist != XV_END_CREATE) {
        ip->panel->status |= NO_REDISPLAY_ITEM;
        result = xv_super_set_avlist(item_public, &xv_panel_ambtn_pkg, avlist);
        ip->panel->status &= ~NO_REDISPLAY_ITEM;
        if (result != XV_OK)
            return result;
    }
 
    /* Parse Attribute-Value List.  Complete initialization upon
     * receipt of XV_END_CREATE.
     */
    for ( ; *avlist; avlist = attr_next(avlist)) {
        switch ((int) avlist[0]) {
	  case XV_END_CREATE:
	    ip->value_rect.r_width = AMB_OFFSET +
		Abbrev_MenuButton_Width(ip->panel->ginfo);
	    ip->value_rect.r_height =
		Abbrev_MenuButton_Height(ip->panel->ginfo);
	    ip->rect = panel_enclosing_rect(&ip->label_rect, &ip->value_rect);
	    break;
	  default:
	    break;
	}
    }
    return XV_OK;	/* return XV_ERROR if something went wrong... */
}


/*ARGSUSED*/
Pkg_private int
panel_ambtn_destroy(item_public, status)
    Panel_item      item_public;
    Destroy_status  status;
{
    Ambtn_info	   *dp = AMBTN_PRIVATE(item_public);

    if ((status == DESTROY_CHECKING) || (status == DESTROY_SAVE_YOURSELF))
	return XV_OK;
    free(dp);
    return XV_OK;
}



/* --------------------  Panel Item Operations  -------------------- */
/*ARGSUSED*/
static void
ambtn_begin_preview(ip, event)	 /* + update_preview() */
    Item_info      *ip;
    Event          *event;
{
    Rect	    amb_rect;
    Ambtn_info	   *dp = AMBTN_FROM_ITEM(ip);
    
    rect_construct(&amb_rect,
		   ip->value_rect.r_left + AMB_OFFSET,
		   ip->value_rect.r_top,
		   ip->value_rect.r_width - AMB_OFFSET,
		   ip->value_rect.r_height);
    if (rect_includespoint(&amb_rect, event_x(event), event_y(event))) {
	if (!dp->invoked) {  /* Note : could be LOC_DRAG within glyph */
	    ambtn_paint_value(ip, OLGX_INVOKED);
	    dp->invoked = TRUE;
	}
    } else
	ambtn_cancel_preview(ip, event);
	
}


/*ARGSUSED*/
static void
ambtn_cancel_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Ambtn_info	   *dp = AMBTN_FROM_ITEM(ip);

    if (dp->invoked) {
	dp->invoked = FALSE;
	ambtn_paint_value(ip,
	    ip->panel->three_d ? OLGX_NORMAL : OLGX_ERASE | OLGX_NORMAL);
    }
}


/*ARGSUSED*/
static void
ambtn_accept_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Ambtn_info	   *dp = AMBTN_FROM_ITEM(ip);

    if (dp->invoked) {
	dp->invoked = FALSE;
	ambtn_paint_value(ip, OLGX_BUSY);
	panel_btn_accepted(ip, event);
	if (!hidden(ip))
	    ambtn_paint_value(ip,
		ip->panel->three_d ? OLGX_NORMAL : OLGX_ERASE | OLGX_NORMAL);
    }
}


/*ARGSUSED*/
static void
ambtn_accept_menu(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Rect	    rect;
    
    rect_construct(&rect,
		   ip->value_rect.r_left + AMB_OFFSET,
		   ip->value_rect.r_top,
		   ip->value_rect.r_width - AMB_OFFSET,
		   ip->value_rect.r_height);
    if (!rect_includespoint(&rect, event_x(event), event_y(event)))
	return;

    ambtn_paint_value(ip, OLGX_INVOKED);

    /*
     * Save public panel handle and current menu done proc. Switch to
     * Abbreviated Menu Button's menu done proc.
     */
    xv_set(ip->menu,
	   XV_KEY_DATA, PANEL_FIRST_ITEM, ip,
	   XV_KEY_DATA, MENU_DONE_PROC, xv_get(ip->menu, MENU_DONE_PROC),
	   MENU_BUSY_PROC, ambtn_menu_busy_proc,
	   MENU_DONE_PROC, ambtn_menu_done_proc,
	   0);

    /* Show the menu */
    rect.r_width = 0;	/* paint menu flush left */
    menu_show(ip->menu, event_window(event), event,
	      MENU_POSITION_RECT, &rect,
	      MENU_PULLDOWN, TRUE,
	      0);
}


static void
ambtn_menu_busy_proc(menu)
    Menu	    menu;
{
    Item_info	   *ip;
    
    ip = (Item_info *) xv_get(menu, XV_KEY_DATA, PANEL_FIRST_ITEM);
    ambtn_paint_value(ip, OLGX_BUSY);
}


static void
ambtn_menu_done_proc(menu, result)
    Menu            menu;
    Xv_opaque       result;
{
    Item_info	   *ip;
    void          (*orig_done_proc) ();	/* original menu-done procedure */
    
    ip = (Item_info *) xv_get(menu, XV_KEY_DATA, PANEL_FIRST_ITEM);
    ambtn_paint_value(ip,
	ip->panel->three_d ? OLGX_NORMAL : OLGX_ERASE | OLGX_NORMAL);

    /* Restore original menu done proc. */
    orig_done_proc = (void (*) ()) xv_get(menu, XV_KEY_DATA, MENU_DONE_PROC);
    xv_set(menu,
	MENU_DONE_PROC, orig_done_proc,
	0);

    /* Invoke original menu done proc (if any) */
    if (orig_done_proc)
	(orig_done_proc) (menu, result);
}


/*ARGSUSED*/
static void
ambtn_paint(ip)
    Item_info      *ip;
{
    /* Paint the label */
    panel_paint_image(ip->panel, &ip->label, &ip->label_rect, inactive(ip),
		      ip->color_index);

    /* Paint the value */
    ambtn_paint_value(ip,
	ip->panel->three_d ? OLGX_NORMAL : OLGX_ERASE | OLGX_NORMAL);
}


/* --------------------  Local Routines  -------------------- */
static void
ambtn_paint_value(ip, state)
    Item_info      *ip;
    int		    state;
{
    Xv_Drawable_info *info;
    Xv_Window	    pw;

    PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	DRAWABLE_INFO_MACRO(pw, info);
	olgx_draw_abbrev_button(ip->panel->ginfo, xv_xid(info),
	    ip->value_rect.r_left + AMB_OFFSET,
	    ip->value_rect.r_top, state);
    PANEL_END_EACH_PAINT_WINDOW
}
