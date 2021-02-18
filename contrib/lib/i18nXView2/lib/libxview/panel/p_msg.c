#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_msg.c 50.1 90/12/12 Copyr 1987 Sun Micro";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/panel_impl.h>

Pkg_private int panel_message_init();


static void     accept(), paint();

static Panel_ops ops = {
    panel_default_handle_event,		/* handle_event() */
    (void (*) ()) panel_nullproc,	/* begin_preview() */
    (void (*) ()) panel_nullproc,	/* update_preview() */
    (void (*) ()) panel_nullproc,	/* cancel_preview() */
    accept,				/* accept_preview() */
    (void (*) ()) panel_nullproc,	/* accept_menu() */
    (void (*) ()) panel_nullproc,	/* accept_key() */
    paint,				/* paint() */
    (void (*) ()) panel_nullproc,	/* remove() */
    (void (*) ()) panel_nullproc,	/* restore() */
    (void (*) ()) panel_nullproc,	/* layout() */
    (void (*) ()) panel_nullproc,	/* accept_kbd_focus() */
    (void (*) ()) panel_nullproc	/* yield_kbd_focus() */
};


/* ARGSUSED */
Pkg_private int
panel_message_init(panel_public, item_public, avlist)
    Panel           panel_public;
    Panel_item      item_public;
    Attr_avlist     avlist;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);
    register Item_info *ip = ITEM_PRIVATE(item_public);

    if (ops_set(ip))
	*ip->ops = ops;		/* copy the button ops vector */
    else
	ip->ops = &ops;		/* use the static message ops vector */
    if (panel->event_proc)
	ip->ops->handle_event = (void (*) ()) panel->event_proc;
    ip->item_type = PANEL_MESSAGE_ITEM;

    /*
     * BUG: since set is not called yet, append will not have the right size
     * for the item.
     */
    panel_append(ip);

    return XV_OK;
}

static void
paint(ip)
    Item_info      *ip;
{
    (void) panel_paint_image(ip->panel, &ip->label, &ip->label_rect,
			     inactive(ip), ip->color_index);
}

static void
accept(ip, event)
    Item_info      *ip;
    Event          *event;
{
    (*ip->notify) (ITEM_PUBLIC(ip), event);
}
