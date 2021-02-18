#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)item.c 50.8 90/12/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/panel_impl.h>


Pkg_private int item_init();
Pkg_private int item_destroy();
static void     item_free();

#ifdef OW_I18N
extern wchar_t		null_string_wc[];
#endif

/*
 * Note that this must be modified when the ops structure is modified.
 */
static Panel_ops null_ops = {
    (void (*) ()) panel_nullproc,	/* handle_event() */
    (void (*) ()) panel_nullproc,	/* begin_preview() */
    (void (*) ()) panel_nullproc,	/* update_preview() */
    (void (*) ()) panel_nullproc,	/* cancel_preview() */
    (void (*) ()) panel_nullproc,	/* accept_preview() */
    (void (*) ()) panel_nullproc,	/* accept_menu() */
    (void (*) ()) panel_nullproc,	/* accept_key() */
    (void (*) ()) panel_nullproc,	/* paint() */
    (void (*) ()) panel_nullproc,	/* remove() */
    (void (*) ()) panel_nullproc,	/* restore() */
    (void (*) ()) panel_nullproc,	/* layout() */
    (void (*) ()) panel_nullproc,	/* accept_kbd_focus() */
    (void (*) ()) panel_nullproc	/* yield_kbd_focus() */
};


#define         FOREGROUND      -1


/*ARGSUSED*/
Pkg_private int
item_init(parent, item_public, avlist)
    Xv_Window       parent;
    Panel_item      item_public;
    Attr_avlist     avlist;
{
    Panel_info     *panel = PANEL_PRIVATE(parent);
    register Item_info *ip;
    Xv_item        *item_object = (Xv_item *) item_public;
    ip = xv_alloc(Item_info);

    /* link to object */
    item_object->private_data = (Xv_opaque) ip;
    ip->public_self = item_public;

    ip->panel = panel;
    ip->next = NULL;
    ip->flags = IS_ITEM | panel->flags;	/* get panel-wide attrs */
    ip->flags &= ~IS_PANEL;	/* this is not a panel */
    ip->flags &= ~OPS_SET;	/* ops vector is not altered */
    ip->layout = PANEL_HORIZONTAL;
    ip->notify = panel_nullproc;
#ifdef OW_I18N
    /* Cache the instance quark list of the item for fast access */
    ip->instance_qlist = xv_get(item_public, XV_INSTANCE_QLIST);
    ip->notify_wc = panel_nullproc;
#endif OW_I18N
    ip->ops = &null_ops;
    ip->repaint = panel->repaint;	/* default painting behavior */
    ip->color_index = -1;	/* use foreground color */

    /* determine the next default position */
    (void) panel_find_default_xy(panel);

    image_set_type(&ip->label, PIT_STRING);
#ifdef	OW_I18N
    image_set_string_wc(&ip->label, (wchar_t *) panel_strsave_wc(null_string_wc));
#endif	OW_I18N
    image_set_string(&ip->label, (char *) panel_strsave(""));
    image_set_font(&ip->label, ip->panel->std_font);
    image_set_bold(&ip->label, FALSE);
    image_set_inverted(&ip->label, label_inverted_flag(ip));
    image_set_color(&ip->label, -1);	/* use foreground color */

    /* nothing is painted yet */
    rect_construct(&ip->painted_rect, 0, 0, 0, 0)
    /*
     * start the item, label and value rects at the default position
     */
	rect_construct(&ip->rect, ip->panel->item_x, ip->panel->item_y, 0, 0);
    ip->label_rect = ip->rect;
    ip->value_rect = ip->rect;

    /* set up the menu */
    ip->menu = NULL;		/* no menu */
#ifdef SUNVIEW1
    ip->menu_title = ip->label;
    image_set_string(&ip->menu_title, panel_strsave(""));
    ip->menu_choices = NULL;	/* no menu choices */
    ip->menu_last = -1;		/* no last slot */
    ip->menu_choices_bold = FALSE;	/* regular choice strings */
    ip->menu_values = NULL;	/* no values */
    ip->menu_type_pr = NULL;	/* no type indication yet */
    ip->menu_max_width = 0;	/* menu is zero wide */
#endif

    if (panel->event_proc) {
	Panel_ops      *new_ops;

	if (!ops_set(ip)) {
	    new_ops = xv_alloc(Panel_ops);
	    *new_ops = *ip->ops;
	    ip->ops = new_ops;
	    ip->flags |= OPS_SET;
	}
	ip->ops->handle_event = (void (*) ()) panel->event_proc;
    }

    return XV_OK;
}

Pkg_private int
item_destroy(item_public, status)
    Panel_item      item_public;
    Destroy_status  status;
{
    Item_info      *item = ITEM_PRIVATE(item_public);
    register Panel_info *panel = item->panel;

    if ((status == DESTROY_CHECKING) || (status == DESTROY_SAVE_YOURSELF))
	return XV_OK;

    /* Note:  It is the responsibility of the item's destroy routine
     * to move the keyboard focus to the next keyboard focus item
     * if the item has the keyboard focus at the time its destroy
     * routine is called.  The item's destroy routine is called before
     * item_destroy().
     */

    /* If the panel isn't going away, clear the item */
    if (!panel->destroying)
	panel_clear_item(item);

    panel_unlink(item, TRUE);	/* unlink is part of a destroy */

    item_free(item);
    return XV_OK;
}


static void
item_free(ip)
    register Item_info *ip;
{

    /* Free the label storage */
    panel_free_image(&ip->label);

    if (ip->menu) {
	/* Free the menu storage */
	xv_set(ip->menu, XV_DECREMENT_REF_COUNT, 0);
	xv_destroy(ip->menu);
    }

    /* Free the ops vector storage */
    if (ops_set(ip))
	free((char *) ip->ops);

    free((char *) ip);
}
