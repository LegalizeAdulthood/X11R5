#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)wizzy.c 50.1 90/12/12";
#endif
#endif

#include <xview_private/panel_impl.h>
#include <xview/wizzy.h>


#define WIZZY_PRIVATE(item) \
	XV_PRIVATE(Wizzy_info, Xv_panel_wizzy, item)
#define WIZZY_FROM_ITEM(ip)	WIZZY_PRIVATE(ITEM_PUBLIC(ip))


/* Declare all wizzy item handler procedures used in the Ops Vector Table */
static void     wizzy_begin_preview(), wizzy_update_preview(),
		wizzy_cancel_preview(), wizzy_accept_preview(),
		wizzy_accept_menu(), wizzy_accept_key(),
		wizzy_paint(), wizzy_layout(),
		wizzy_remove(), wizzy_restore(),
		wizzy_accept_kbd_focus(), wizzy_yield_kbd_focus();


/*
 * Panel Operations Vector Table for this item.
 * If any of the operations do not apply, then
 * use "(void (*) ()) panel_nullproc" as the
 * handler.
 */
static Panel_ops ops = {
    panel_default_handle_event,		/* handle_event() */
    wizzy_begin_preview,		/* begin_preview() */
    wizzy_update_preview,		/* update_preview() */
    wizzy_cancel_preview,		/* cancel_preview() */
    wizzy_accept_preview,		/* accept_preview() */
    wizzy_accept_menu,			/* accept_menu() */
    wizzy_accept_key,			/* accept_key() */
    wizzy_paint,			/* paint() */
    wizzy_remove,			/* remove() */
    wizzy_restore,			/* restore() */
    wizzy_layout,			/* layout() */
    wizzy_accept_kbd_focus,		/* accept_kbd_focus() */
    wizzy_yield_kbd_focus,		/* yield_kbd_focus() */
};

typedef struct wizzy_info {
    Panel_item      public_self;/* back pointer to object */
    /*
     * Wizzy private data goes here.
     * *** All references to these entries is for illustration
     * *** purposes only.  They are to be replaced with your
     * *** item's private data requirements.
     */
    Rect	    block;	/* Some rectangle within the item */
    int		    offset;	/* The block's offset */
#ifdef WIZZY_CAN_ACCEPT_KBD_FOCUS
    int		    has_kbd_focus; /* TRUE or FALSE */
#endif WIZZY_CAN_ACCEPT_KBD_FOCUS
} Wizzy_info;


/* Item specific definitions */
#define INITIAL_OFFSET	10
#define BLOCK_WIDTH	16
#define BLOCK_HEIGHT	12


/* ========================================================================= */

/* -------------------- XView Functions  -------------------- */
Pkg_private int
wizzy_init(panel_public, item_public, avlist)
    Panel           panel_public;
    Panel_item      item_public;
    Attr_avlist     avlist;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);
    register Item_info *ip = ITEM_PRIVATE(item_public);
    Xv_panel_wizzy *item_object = (Xv_panel_wizzy *) item_public;
    Wizzy_info    *dp;

    dp = xv_alloc(Wizzy_info);

    item_object->private_data = (Xv_opaque) dp;
    dp->public_self = item_public;

    if (ops_set(ip))
	*ip->ops = ops;		/* copy the wizzy ops vector */
    else
	ip->ops = &ops;		/* use the static wizzy ops vector */
    if (panel->event_proc)
	ip->ops->handle_event = (void (*) ()) panel->event_proc;
    ip->item_type = PANEL_EXTENSION_ITEM;
#ifdef WIZZY_CAN_ACCEPT_KBD_FOCUS
    ip->flags |= WANTS_KEY;
    if (!hidden(ip) && ip->panel->kbd_focus_item == NULL) {
	ip->panel->kbd_focus_item = ip;
	dp->has_kbd_focus = TRUE;
    }
#endif WIZZY_CAN_ACCEPT_KBD_FOCUS

    /*
     * Initialize non-zero private data
     */
    dp->offset = INITIAL_OFFSET;

    /*
     * Process avlist for create-only attributes.
     */
    for ( ; *avlist; avlist = attr_next(avlist)) {
        switch ((int) avlist[0]) {
	  /* case <create_only_attr>: */
	  default:
	    break;
	}
    }

    panel_append(ip);

    return XV_OK;
}


Pkg_private Xv_opaque
wizzy_set_avlist(item_public, avlist)
    Panel_item	    item_public;
    Attr_avlist	    avlist;
{
    Item_info	   *ip = ITEM_PRIVATE(item_public);
    Wizzy_info	   *dp = WIZZY_PRIVATE(item_public);
    Xv_opaque       result;

    /* Call generic item set code to handle layout attributes.
     * Prevent panel_redisplay_item from being called in item_set_avlist.
     */
    if (*avlist != XV_END_CREATE) {
        ip->panel->status |= NO_REDISPLAY_ITEM;
        result = xv_super_set_avlist(item_public, &xv_panel_wizzy_pkg, avlist);
        ip->panel->status &= ~NO_REDISPLAY_ITEM;
        if (result != XV_OK)
            return result;
    }
 
    /* Parse Attribute-Value List.  Complete initialization upon
     * receipt of XV_END_CREATE.
     */
    for ( ; *avlist; avlist = attr_next(avlist)) {
        switch ((int) avlist[0]) {
	  case WIZZY_OFFSET:
	    dp->offset = (int) avlist[1];
	    break;
	  case XV_END_CREATE:
	    rect_construct(&dp->block,
			   ip->value_rect.r_left + dp->offset,
			   ip->value_rect.r_top,
			   BLOCK_HEIGHT, BLOCK_WIDTH);
	    ip->value_rect = panel_enclosing_rect(&ip->value_rect, &dp->block);
	    ip->rect = panel_enclosing_rect(&ip->label_rect, &ip->value_rect);
	    break;
	  default:
	    break;
	}
    }
    return XV_OK;	/* return XV_ERROR if something went wrong... */
}


/*ARGSUSED*/
Pkg_private Xv_opaque
wizzy_get_attr(item_public, status, which_attr, avlist)
    Panel_item      item_public;
    int            *status;	/* set to XV_ERROR if something goes wrong */
    register Attr_attribute which_attr;
    va_list         avlist;
{
    Wizzy_info	   *dp = WIZZY_PRIVATE(item_public);

    switch (which_attr) {
      case WIZZY_OFFSET:
	return (Xv_opaque) dp->offset;
      default:
	*status = XV_ERROR;
	return (Xv_opaque) 0;
    }
}
      

/*ARGSUSED*/
Pkg_private int
wizzy_destroy(item_public, status)
    Panel_item      item_public;
    Destroy_status  status;
{
    Wizzy_info	   *dp = WIZZY_PRIVATE(item_public);
    Item_info	   *ip = ITEM_PRIVATE(item_public);

    if ((status == DESTROY_CHECKING) || (status == DESTROY_SAVE_YOURSELF))
	return XV_OK;
#ifdef WIZZY_CAN_ACCEPT_KBD_FOCUS
    wizzy_remove(ip);
#endif WIZZY_CAN_ACCEPT_KBD_FOCUS
    free(dp);
    return XV_OK;
}



/* --------------------  Panel Item Operations  -------------------- */
/*ARGSUSED*/
static void
wizzy_begin_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    /*
     * SELECT-down has occurred over the item.
     * Highlight the item to show the active feedback,
     * but do not take any action yet.
     */
}


/*ARGSUSED*/
static void
wizzy_update_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    /*
     * The pointer as been dragged within the item after
     * beginning a preview.  Adjust the highlighting to
     * reflect the new position of the pointer, and update
     * appropriate private data.
     */
}


/*ARGSUSED*/
static void
wizzy_cancel_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    /*
     * The pointer as been dragged out of the item after
     * beginning a preview.  Remove the active feedback
     * (i.e., unhighlight) and clean up any private data.
     */
}


/*ARGSUSED*/
static void
wizzy_accept_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    /*
     * The SELECT button has been released over the item.
     * Remove the active feedback (i.e., unhighlight),
     * paint the busy feedback,
     * perform the action associated with the item,
     * and then remove the busy feedback.
     */
}


/*ARGSUSED*/
static void
wizzy_accept_menu(ip, event)
    Item_info      *ip;
    Event          *event;
{
    /*
     * The MENU button has been depressed over the item.
     * Show the menu attached to the item, if any.
     */
}


/*ARGSUSED*/
static void
wizzy_accept_key(ip, event)
    Item_info      *ip;
    Event          *event;
{
    /*
     * A keyboard event has occurred.  Process the key,
     * and update the display.
     */
}


/*ARGSUSED*/
static void
wizzy_paint(ip)
    Item_info      *ip;
{
    /*
     * Do everything necessary to paint the entire item.
     * Don't go outside of ip->rect, the rectangle describing the
     * boundaries of the item.
     */

    /* Paint the label */
    panel_paint_image(ip->panel, &ip->label, &ip->label_rect, inactive(ip),
		      ip->color_index);

    /* Paint the value.
     * In this wizzy example, you'd paint something into dp->block.
     */
}


/*ARGSUSED*/
static void
wizzy_layout(ip, deltas)
    Item_info	   *ip;
    Rect	   *deltas;
{
    /*
     * The item has been moved.  Adjust the item coordinates.
     */
    Wizzy_info	   *dp = WIZZY_FROM_ITEM(ip);

    dp->block.r_left += deltas->r_left;
    dp->block.r_top += deltas->r_top;
}


/*ARGSUSED*/
static void
wizzy_remove(ip)
    Item_info	   *ip;
{
    /*
     * The item has been made hidden via xv_set(item, XV_SHOW, FALSE, avlist).
     */
#ifdef WIZZY_CAN_ACCEPT_KBD_FOCUS
    Wizzy_info	   *dp = WIZZY_FROM_ITEM(ip);

    if (panel_seln(ip->panel, SELN_CARET)->ip == ip)
        panel_seln_cancel(ip->panel, SELN_CARET);

    /*
     * Only reassign the keyboard focus to another item if the panel isn't
     * being destroyed.
     */
    if (!ip->panel->destroying && ip->panel->kbd_focus_item == ip) {
        dp->has_kbd_focus = FALSE;
        if (ip->panel->kbd_focus_item = panel_next_kbd_focus(ip->panel))
            (*ip->panel->kbd_focus_item->ops->accept_kbd_focus) (
                ip->panel->kbd_focus_item);
    }
#endif WIZZY_CAN_ACCEPT_KBD_FOCUS
}


/*ARGSUSED*/
static void
wizzy_restore(ip)
    Item_info	   *ip;
{
    /*
     * The item has been made visible via xv_set(item, XV_SHOW, TRUE, avlist).
     */
#ifdef WIZZY_CAN_ACCEPT_KBD_FOCUS
    Wizzy_info	   *dp = WIZZY_FROM_ITEM(ip);

    if (!ip->panel->seln_client)
        panel_seln_init(ip->panel);
    /* If this is the only item, give the keyboard focus to this item. */
    if (ip->panel->items == ip && !ip->next) {
        ip->panel->kbd_focus_item = ip;
        dp->has_kbd_focus = TRUE;
        /* If the panel has the caret selection, change the item.  */
        if (panel_seln(ip->panel, SELN_CARET)->ip)
            panel_seln(ip->panel, SELN_CARET)->ip = ip;
    }
#endif WIZZY_CAN_ACCEPT_KBD_FOCUS
}


/*ARGSUSED*/
static void
wizzy_accept_kbd_focus(ip)
    Item_info	   *ip;
{
    /*
     * The keyboard focus has been set to this item.
     * Change the keyboard focus feedback to active, and
     * update private data as necessary.
     */
#ifdef WIZZY_CAN_ACCEPT_KBD_FOCUS
    Wizzy_info	   *dp = WIZZY_FROM_ITEM(ip);

    dp->has_kbd_focus = TRUE;
#endif WIZZY_CAN_ACCEPT_KBD_FOCUS
}


/*ARGSUSED*/
static void
wizzy_yield_kbd_focus(ip)
    Item_info	   *ip;
{
    /*
     * The keyboard focus has been removed from this item.
     * Change the keyboard focus feedback to inactive, and
     * update private data as necessary.
     */
#ifdef WIZZY_CAN_ACCEPT_KBD_FOCUS
    Wizzy_info	   *dp = WIZZY_FROM_ITEM(ip);

    dp->has_kbd_focus = FALSE;
#endif WIZZY_CAN_ACCEPT_KBD_FOCUS
}

