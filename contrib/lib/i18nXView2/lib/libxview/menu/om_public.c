#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)om_public.c 70.1 91/08/02";
#endif
#endif

/*
 * Copyright (c) 1985 by Sun Microsystems, Inc.
 */

#include <xview/win_input.h>	/* includes types & time */
#ifndef FILE
#undef NULL
#include <stdio.h>
#endif				/* FILE */
#include <fcntl.h>

#include <pixrect/pixrect.h>
#include <pixrect/pr_util.h>
#include <pixrect/memvar.h>

#include <X11/Xlib.h>
#include <xview/xview.h>

#include <xview/defaults.h>
#include <xview/font.h>
#include <xview/panel.h>
#include <xview/win_struct.h>
#include <xview/rectlist.h>

#include <xview/screen.h>
#include <xview/server.h>
#include <xview/fullscreen.h>
#include <xview/notify.h>

#include <xview_private/draw_impl.h>
#include <xview_private/om_impl.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N



/* ------------------------------------------------------------------------- */

/*
 * Public
 */
Xv_public Xv_opaque menu_return_value();
Xv_public void  menu_show();
Xv_public Xv_opaque menu_return_item();
Xv_public void  menu_default_pin_proc();

Sv1_public Xv_opaque menu_get();
Sv1_public Xv_opaque menu_set();
Sv1_public void menu_destroy_with_proc();
Sv1_public Menu_item menu_find();

/*
 * Toolkit private
 */
Xv_private void frame_get_rect();
Xv_private void frame_set_rect();
Xv_private Xv_opaque menu_return_default();
Xv_private void menu_select_default();

/*
 * Package private
 */
Pkg_private int menu_create_internal();
Pkg_private int menu_create_item_internal();
Pkg_private int menu_item_destroy_internal();
Pkg_private Xv_opaque menu_pullright_return_result();
Pkg_private Xv_opaque menu_sets();
Pkg_private Xv_opaque menu_gets();
Pkg_private Xv_opaque menu_pkg_find();
Pkg_private Xv_opaque menu_item_sets();
Pkg_private Xv_opaque menu_item_gets();
Pkg_private Xv_opaque menu_return_no_value();
Pkg_private void menu_create_pin_window();
Pkg_private void menu_done();
Pkg_private void menu_render();
Pkg_private void menu_set_pin_window();
Pkg_private void menu_window_event_proc();
Pkg_private void menu_shadow_event_proc();
Pkg_private Notify_value menu_client_window_event_proc();


/* destroy routines for menus and menu_items */
Pkg_private void menu_destroys(), menu_item_destroys();

/* Pkg_private (server XV_KEY_DATA keys) */
int    menu_active_menu_key;

/*
 * Private
 */
static Xv_opaque menu_return_result();
static int    menu_group_info_key;

/* Cache the standard menu data obtained from the defaults database */
static Xv_menu_info *m_cache;

/*
 * Private defs
 */
#define	MENU_DEFAULT_MARGIN		1
#define	MENU_DEFAULT_LEFT_MARGIN	16
#define	MENU_DEFAULT_RIGHT_MARGIN	6
#define	Null_status	(int *)0
#define MENU_KEY 1
#define MENU_ITEM_KEY 2

/* ------------------------------------------------------------------------- */


/*
 * Display the menu, get the menu item, and call notify proc. Default proc
 * returns a pointer to the item selected or NULL.
 */
/* VARARGS3 */
Xv_public void
menu_show(menu_public, win, iep, va_alist)
    Menu            menu_public;
    Xv_Window       win;
    struct inputevent *iep;
va_dcl		/*** WARNING: menu_show does not support ATTR_LIST. ***/
{
#ifdef ATTR_MAKE
    Attr_attribute  avlist[ATTR_STANDARD_SIZE];
    va_list         valist;
#endif				/* ATTR_MAKE */
    Attr_avlist     attrs;
    Xv_Drawable_info *info;
    Display        *display;
    Rect           *enable_rectp = 0;	/* rectangle in which menu stays up.
					 * Also describes position. 0=> popup
					 * menu */
    Xv_menu_group_info *group_info;
    Xv_menu_info   *menu;
    Rect           *position_rectp = 0;	/* describes menu position only */
    Xv_Server	    server;

    if (!menu_public) {
	xv_error(0,
#ifdef	OW_I18N
		 ERROR_STRING, XV_I18N_MSG("xv_messages","menu_show: no menu specified"),
#else
		 ERROR_STRING, "menu_show: no menu specified",
#endif
		 ERROR_PKG, MENU,
		 0);
	return;
    }
    menu = MENU_PRIVATE(menu_public);
    server = xv_get(menu_public, XV_OWNER);
    if (!server)
	server = xv_default_server;
    if (server != XV_SERVER_FROM_WINDOW(win)) {
	xv_error(0,
#ifdef	OW_I18N
		 ERROR_STRING, XV_I18N_MSG("xv_messages","menu_show: menu not owned by this server"),
#else
		 ERROR_STRING, "menu_show: menu not owned by this server",
#endif
		 ERROR_PKG, MENU,
		 0);
	goto menu_show_error;
    }

#ifdef ATTR_MAKE
    va_start(valist);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);
    attrs = avlist;
#else
    attrs = (Attr_avlist) & va_alist;
#endif

    for (; *attrs; attrs = attr_next(attrs)) {
	switch (attrs[0]) {

	  case MENU_ENABLE_RECT:
	    enable_rectp = (Rect *) attrs[1];
	    break;

	  case MENU_BUTTON:
	    event_set_id(iep, (int) attrs[1]);
	    break;

	  case MENU_POS:
	    iep->ie_locx = (int) attrs[1], iep->ie_locy = (int) attrs[2];
	    break;

	  case MENU_POSITION_RECT:
	    position_rectp = (Rect *) attrs[1];
	    break;

	  case MENU_PULLDOWN:
	    menu->pulldown = (int) attrs[1];
	    break;

	  default:
	    (void) xv_check_bad_attr(MENU, attrs);
	    break;
	}
    }

    DRAWABLE_INFO_MACRO(win, info);
    display = xv_display(info);

    /*
     * Grab all input and disable anybody but us from writing to screen while
     * we are violating window overlapping.
     */
    if (XGrabPointer(display, xv_get(win, XV_XID),
		     False,	/* owner events: report events relative to
				 * grab window */
		     ButtonPressMask | ButtonReleaseMask |
			 ButtonMotionMask,	/* event mask */
		     GrabModeAsync,	/* pointer mode */
		     GrabModeAsync,	/* keyboard mode */
		     None,	/* confine to: don't confine pointer */
		     0,		/* cursor: use default window cursor */
		     CurrentTime)	/* time */
	!=GrabSuccess) {
	xv_error(0,
#ifdef	OW_I18N
		 ERROR_STRING, XV_I18N_MSG("xv_messages","menu_show: unable to grab pointer"),
#else
		 ERROR_STRING, "menu_show: unable to grab pointer",
#endif
		 ERROR_PKG, MENU,
		 0);
	goto menu_show_error;
    }
    if (XGrabKeyboard(display, xv_get(win, XV_XID),
		     False,	/* owner events: report events relative to
				 * grab window */
		     GrabModeAsync,	/* pointer mode */
		     GrabModeAsync,	/* keyboard mode */
		     CurrentTime)	/* time */
	!=GrabSuccess) {
	xv_error(0,
#ifdef	OW_I18N
		 ERROR_STRING, XV_I18N_MSG("xv_messages","menu_show: unable to grab keyboard"),
#else
		 ERROR_STRING, "menu_show: unable to grab keyboard",
#endif
		 ERROR_PKG, MENU,
		 0);
	XUngrabPointer(display, CurrentTime);
	goto menu_show_error;
    }

    menu->state = MENU_STATE_INIT;
    if (!menu_active_menu_key)
	menu_active_menu_key = xv_unique_key();
    xv_set(server, XV_KEY_DATA, menu_active_menu_key, menu, 0);
    if (!menu_group_info_key)
	menu_group_info_key = xv_unique_key();
    group_info = (Xv_menu_group_info *) xv_get(server,
	XV_KEY_DATA, menu_group_info_key);
    if (!group_info) {
	/* Allocate and initialize menu group information */
	group_info = (Xv_menu_group_info *) malloc(sizeof(Xv_menu_group_info));
	if (group_info == NULL) {
	    xv_error(0,
#ifdef	OW_I18N
		     ERROR_STRING, XV_I18N_MSG("xv_messages","menu_show: unable to allocate group_info"),
#else
		     ERROR_STRING, "menu_show: unable to allocate group_info",
#endif
		     ERROR_PKG, MENU,
		     0);
	    XUngrabPointer(display, CurrentTime);
	    XUngrabKeyboard(display, CurrentTime);
	    goto menu_show_error;
	}
	xv_set(server, XV_KEY_DATA, menu_group_info_key, group_info, 0);
	group_info->server = server;
    }
    group_info->client_window = win;
    group_info->color_index = menu->color_index;
    group_info->depth = 0;
    group_info->first_event = *iep;
    group_info->last_event = *iep;
    group_info->menu_down_event.action = 0;
	/* no MENU-down event received yet */
    group_info->selected_menu = menu;
    if (xv_depth(info) > 1)
	group_info->three_d = defaults_get_boolean("OpenWindows.3DLook.Color",
	    "OpenWindows.3DLook.Color", TRUE);
    else
#ifdef MONO3D
	group_info->three_d =
	    defaults_get_boolean("OpenWindows.3DLook.Monochrome",
	    "OpenWindows.3DLook.Monochrome", FALSE);
#else
	group_info->three_d = FALSE;
#endif

    if (enable_rectp)
	menu->enable_rect = *enable_rectp;
    else
	/* tell process_event and compute_rects that there's no enable rect */
	menu->enable_rect.r_width = 0;
    if (position_rectp)
	menu->position_rect = *position_rectp;
    menu->popup = (enable_rectp || position_rectp) ?
	FALSE			/* menu is a pulldown or pullright */
	: TRUE;			/* menu is a popup */

    menu->ptr_jumped = FALSE;
    menu->stay_up = FALSE;		/* assume non-stay-up mode */

    /*
     * Interpose an event function on the client window to catch the mouse
     * button events.
     */
    notify_interpose_event_func(win, menu_client_window_event_proc,
				NOTIFY_SAFE);

    menu_render(menu, group_info, (Xv_menu_item_info *) 0);
    return;

menu_show_error:
    menu->notify_status = XV_ERROR;
    if (menu->done_proc)
	(menu->done_proc) (menu_public, MENU_NO_VALUE);
}


Pkg_private void
menu_done(m)
    register Xv_menu_info *m;
{
    Display        *display;
    Xv_Drawable_info *info;
    Xv_opaque       result;
    Xv_Server       server;

    DRAWABLE_INFO_MACRO(m->group_info->client_window, info);
    display = xv_display(info);
    server = xv_server(info);

    XUngrabPointer(display, CurrentTime);
    XUngrabKeyboard(display, CurrentTime);

    if (m->status == MENU_STATUS_PIN)
	(m->group_info->pinned_menu->pin_proc) (
	    MENU_PUBLIC(m->group_info->pinned_menu),
	    m->group_info->pinned_menu->pin_window_pos.x,
	    m->group_info->pinned_menu->pin_window_pos.y);

    m->notify_status = XV_OK;	/* assume notify procedure succeeds */
    if (m->status == MENU_STATUS_DONE) {
	/* Sync the server */
	xv_set(server, SERVER_SYNC, FALSE, 0);
	/*
	 * Call notify_proc.  Should handle special case of selection = 0
	 */
	m->group_info->notify_proc = m->notify_proc;
	if (!m->group_info->notify_proc)
	    m->group_info->notify_proc = MENU_DEFAULT_NOTIFY_PROC;
	result = m->status == MENU_STATUS_DONE ?
	    menu_return_result(m->group_info->selected_menu, m->group_info, 0) :
	    menu_return_no_value(MENU_PUBLIC(m->group_info->selected_menu));
    } else
	result = MENU_NO_VALUE;

    /*
     * Call menu-done procedure, if any.
     */
    if (m->done_proc)
	(m->done_proc) (MENU_PUBLIC(m), result);

    notify_remove_event_func(m->group_info->client_window,
			     menu_client_window_event_proc, NOTIFY_SAFE);
    m->group_info = NULL;
    xv_set(server, XV_KEY_DATA, menu_active_menu_key, NULL, 0);
}


/*VARARGS*/
Pkg_private int
menu_create_internal(parent, object, avlist)	/*ARGSUSED*/
    Xv_opaque       parent;
    Xv_opaque       object;
    Xv_opaque      *avlist;
{
    register Xv_menu_info *m;
    register Xv_pkg *menu_type;
    /*
     * BUG ALERT:  We need to pass a root window to the menu package in order
     * to use multiple screens.
     */

    ((Xv_menu *) (object))->private_data = (Xv_opaque) xv_alloc(Xv_menu_info);
    if (!(m = MENU_PRIVATE(object))) {
	xv_error(object,
#ifdef	OW_I18N
		 ERROR_STRING, XV_I18N_MSG("xv_messages","menu_create: unable to allocate menu structure"),
#else
		 ERROR_STRING, "menu_create: unable to allocate menu structure",
#endif
		 ERROR_PKG, MENU,
		 0);
	return (int) XV_ERROR;
    }
    if (!m_cache) {
	if (!(m_cache = xv_alloc(Xv_menu_info))) {
	    xv_error(object,
		     ERROR_STRING,
#ifdef	OW_I18N
		         XV_I18N_MSG("xv_messages","menu_create: unable to allocate menu structure"),
#else
		         "menu_create: unable to allocate menu structure",
#endif
		     ERROR_PKG, MENU,
		     0);
	    return (int) XV_ERROR;
	}
	m_cache->color_index = -1;
	m_cache->column_major = TRUE;
	m_cache->default_image.bold_font = NULL;
	m_cache->default_image.font = NULL;
	m_cache->default_image.left_margin = 1;
	m_cache->default_image.margin = 1;
	m_cache->default_image.right_margin = 1;
	m_cache->default_position = 1;	/* default item is always the first
					 * if not specified */
	m_cache->extra_destroy_proc = 0;
	m_cache->notify_proc = MENU_DEFAULT_NOTIFY_PROC;
	m_cache->pin_proc = menu_default_pin_proc;
	m_cache->pullright_delta =
	    defaults_get_integer("openWindows.dragRightDistance",
				 "OpenWindows.DragRightDistance", 100);
	m_cache->select_is_menu = defaults_get_boolean(
	    "openWindows.selectDisplaysMenu",
	    "OpenWindows.SelectDisplaysMenu",
	    FALSE);
    }
    bcopy((Xv_opaque) m_cache, (Xv_opaque) m, sizeof(Xv_menu_info));

    /* Above bcopy() zaps all of m, so must set individual fields after it.  */
    m->public_self = object;
    m->type = (int) MENU_MENU;	/* for verifying menu handle in menu_destroys */

    /*
     * Malloc the menu storage and create the item list
     */
    m->nitems = 0, m->max_nitems = 2 * MENU_FILLER;
    m->item_list = (Xv_menu_item_info **)
	calloc(2 * MENU_FILLER, sizeof(Xv_menu_item_info *));
    if (!m->item_list) {
	xv_error(object,
		 ERROR_LAYER, ERROR_SYSTEM,
#ifdef	OW_I18N
		 ERROR_STRING, XV_I18N_MSG("xv_messages","menu_create: unable to allocate an item list"),
#else
		 ERROR_STRING, "menu_create: unable to allocate an item list",
#endif
		 ERROR_PKG, MENU,
		 0);
	return (int) XV_ERROR;
    }
    /*
     * set the class field depending on what type of menu the client created
     */
    menu_type = (Xv_pkg *) xv_get(object, XV_TYPE);
    if (menu_type == MENU_COMMAND_MENU) {
	m->class = MENU_COMMAND;
    } else if (menu_type == MENU_CHOICE_MENU) {
	m->class = MENU_CHOICE;
	m->default_image.left_margin = 3;
	m->default_image.right_margin = 3;
    } else if (menu_type == MENU_TOGGLE_MENU) {
	m->class = MENU_TOGGLE;
    } else {
	xv_error(object,
#ifdef	OW_I18N
		 ERROR_STRING, XV_I18N_MSG("xv_messages","Unknown menu type"),
#else
		 ERROR_STRING, "Unknown menu type",
#endif
		 ERROR_PKG, MENU,
		 0);
    }

    (void) xv_set(object, XV_RESET_REF_COUNT, 0);	/* Mark as ref counted. */

    return (int) XV_OK;
}


/* VARARGS */
Pkg_private int
menu_create_item_internal(parent, object, avlist)	/*ARGSUSED*/
    Xv_opaque       parent;
    Xv_opaque       object;
    Xv_opaque      *avlist;
{
    Xv_menu_item_info *mi;

    ((Xv_menu_item *) (object))->private_data =
	(Xv_opaque) xv_alloc(Xv_menu_item_info);
    if (!(mi = MENU_ITEM_PRIVATE(object))) {
	xv_error(object,
#ifdef	OW_I18N
		 ERROR_STRING, XV_I18N_MSG("xv_messages","Menu_create_item: unable to allocate menu_item"),
#else
		 ERROR_STRING, "Menu_create_item: unable to allocate menu_item",
#endif
		 ERROR_PKG, MENU,
		 0);
	return (int) XV_ERROR;
    }
    mi->color_index = -1;
    mi->public_self = object;
    mi->image.free_image = TRUE;

    return (int) XV_OK;
}


/*
 * destroy the menu data.
 */
Pkg_private int
menu_destroy_internal(menu_public, status)
    Menu            menu_public;
    Destroy_status  status;
{
    Xv_menu_info   *menu = MENU_PRIVATE(menu_public);

    if ((status != DESTROY_CHECKING) && (status != DESTROY_SAVE_YOURSELF))
	menu_destroys(menu, menu->extra_destroy_proc);

    return XV_OK;
}


/*
 * destroy the menu item data.
 */
Pkg_private int
menu_item_destroy_internal(menu_item_public, status)
    Menu_item       menu_item_public;
    Destroy_status  status;
{
    Xv_menu_item_info *menu_item = MENU_ITEM_PRIVATE(menu_item_public);

    if ((status != DESTROY_CHECKING) && (status != DESTROY_SAVE_YOURSELF))
	menu_item_destroys(menu_item, menu_item->extra_destroy_proc);

    return XV_OK;
}


static          Xv_opaque
menu_return_result(menu, group, parent)
    Xv_menu_info   *menu;
    Xv_menu_group_info *group;
    Xv_menu_item_info *parent;
{
    register Xv_menu_info *m;
    register Xv_menu_item_info *mi;
    Menu(*m_gen_proc) ();
    Menu_item(*mi_gen_proc) ();
    Xv_opaque       v, (*notify_proc) ();
    int             i;
    int		    mask;
    int		    toggle_value;

    if (m_gen_proc = menu->gen_proc) {
	m = MENU_PRIVATE((m_gen_proc) (MENU_PUBLIC(menu), MENU_NOTIFY));
	if (m == NULL)
	    return 0;
	m->group_info = group;
	m->parent = parent;
    } else
	m = menu;

    if (m->status != MENU_STATUS_DONE ||
	!range(m->selected_position, 1, m->nitems))
	m->selected_position = m->default_position;
    mi = m->item_list[m->selected_position - 1];

    switch (m->class) {
      case MENU_CHOICE:	/* exclusive choice */
	for (i = 0; i < m->nitems; i++)
	    m->item_list[i]->selected = FALSE;
	mi->selected = TRUE;
	if (mi->panel_item_handle) {
	    if (m->item_list[0]->title)
		i = m->selected_position - 2;
	    else
		i = m->selected_position - 1;
	    xv_set(mi->panel_item_handle,
		   PANEL_VALUE, i,
		   0);
	}
	break;

      case MENU_TOGGLE:	/* nonexclusive choice */
	mi->selected = mi->selected ? FALSE : TRUE;
	if (mi->panel_item_handle) {
	    i = 0;
	    if (m->item_list[0]->title)
		i++;
	    mask = 1;
	    toggle_value = 0;
	    for (; i < m->nitems; i++) {
		if (m->item_list[i]->selected)
		    toggle_value |= mask;
		mask <<= 1;
	    }
	    xv_set(mi->panel_item_handle, PANEL_VALUE, toggle_value, 0);
	}
	break;

      case MENU_COMMAND:
      default:
	mi->selected = m->status == MENU_STATUS_DONE;
	break;
    }

    mi->parent = m;

    if (mi->inactive) {
	v = menu_return_no_value(MENU_PUBLIC(m));
	goto cleanup;
    }
    if (mi_gen_proc = mi->gen_proc) {
	mi = MENU_ITEM_PRIVATE((mi_gen_proc) (MENU_ITEM_PUBLIC(mi), MENU_NOTIFY));
	if (mi == NULL)
	    return menu_return_no_value(MENU_PUBLIC(menu));
	mi->parent = m;
    }
    notify_proc = mi->notify_proc ? mi->notify_proc
	: m->notify_proc ? m->notify_proc : m->group_info->notify_proc;

    v = (Xv_opaque) (notify_proc) (MENU_PUBLIC(m), MENU_ITEM_PUBLIC(mi));

    if (mi_gen_proc)
	(mi_gen_proc) (MENU_ITEM_PUBLIC(mi), MENU_NOTIFY_DONE);

cleanup:
    if (m_gen_proc) {
	(m_gen_proc) (MENU_PUBLIC(m), MENU_NOTIFY_DONE);
    }

    return v;
}


Pkg_private     Xv_opaque
menu_pullright_return_result(menu_item_public)
    Menu_item       menu_item_public;
{
    register Xv_menu_info *m;
    register Xv_menu_item_info *mi;
    register Xv_menu_info *mn;
    Menu	  (*gen_proc) ();
    Menu	    pullright_menu;
    Xv_opaque       v;

    if (!menu_item_public)
	return NULL;
    mi = MENU_ITEM_PRIVATE(menu_item_public);
    if (!mi->pullright)
	return NULL;


    m = mi->parent;

    if (gen_proc = mi->gen_pullright) {
	pullright_menu = (gen_proc) (menu_item_public, MENU_NOTIFY);
	mn = pullright_menu ? MENU_PRIVATE(pullright_menu) : NULL;
	if (mn == NULL)
	    return menu_return_no_value(MENU_PUBLIC(m));
    } else {
	mn = MENU_PRIVATE(mi->value);
    }

    if (mn->nitems) {
	v = menu_return_result(mn, m->group_info, mi);
	m->valid_result = mn->valid_result;
    } else {
	v = (Xv_opaque) MENU_NO_VALUE;
	m->valid_result = FALSE;
    }

    if (gen_proc)
	(gen_proc) (menu_item_public, MENU_NOTIFY_DONE);

    return v;
}


Xv_public       Xv_opaque
menu_return_value(menu_public, menu_item_public)
    Menu            menu_public;
    Menu_item       menu_item_public;
{
    register Xv_menu_info *m;
    register Xv_menu_item_info *mi;

    if (!menu_public || !menu_item_public) {	/* No menu or item */
	if (menu_public) {
	    (MENU_PRIVATE(menu_public))->valid_result = FALSE;
	}
	return (Xv_opaque) MENU_NO_VALUE;
    }
    m = MENU_PRIVATE(menu_public);
    mi = MENU_ITEM_PRIVATE(menu_item_public);
    if (mi->pullright)
	return menu_pullright_return_result(menu_item_public);

    m->valid_result = TRUE;
    return (Xv_opaque) mi->value;	/* Return value */
}


Xv_public       Xv_opaque
menu_return_item(menu_public, menu_item_public)
    Menu            menu_public;
    Menu_item       menu_item_public;
{
    register Xv_menu_info *m;
    register Xv_menu_item_info *mi;

    if (!menu_public || !menu_item_public) {	/* No menu or item */
	if (menu_public) {
	    (MENU_PRIVATE(menu_public))->valid_result = FALSE;
	}
	return (Xv_opaque) MENU_NO_ITEM;
    }
    m = MENU_PRIVATE(menu_public);
    mi = MENU_ITEM_PRIVATE(menu_item_public);
    if (mi->pullright)
	return menu_pullright_return_result(menu_item_public);

    m->valid_result = TRUE;
    return MENU_ITEM_PUBLIC(mi);/* Return pointer */
}


Pkg_private     Xv_opaque
menu_return_no_value(menu_public)
    Menu            menu_public;
{
    register Xv_menu_info *m;

    if (menu_public) {
	m = MENU_PRIVATE(menu_public);
	m->valid_result = FALSE;
	if (m->gen_proc) {
	    (m->gen_proc) (menu_public, MENU_NOTIFY);
	    (m->gen_proc) (menu_public, MENU_NOTIFY_DONE);
	}
    }
    return (Xv_opaque) MENU_NO_VALUE;
}


/* VARARGS1 */
Sv1_public      Menu_item
menu_find(menu_public, va_alist)
    Menu            menu_public;
va_dcl				/*** WARNING: menu_find does not support ATTR_LIST. ***/
{
#ifdef ATTR_MAKE
    Attr_attribute  avlist[ATTR_STANDARD_SIZE];
    va_list         valist;

    va_start(valist);
    (void) attr_make((char **) avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);
    return menu_pkg_find(menu_public, MENUITEM, avlist);
#else
    return menu_pkg_find(menu_public, MENUITEM, (Attr_avlist) &va_alist);
#endif				/* ATTR_MAKE */
}


/*
 * Find the menu_item specified by the avlist. menu_pkg_find is called from
 * xv_find via the menu package ops vector.
 */
Pkg_private     Xv_opaque
menu_pkg_find(menu_public, pkg, avlist)		/*ARGSUSED*/
    Menu            menu_public;
    Xv_pkg         *pkg;	/* Must be MENUITEM */
    Attr_attribute  avlist[ATTR_STANDARD_SIZE];
{
    register Attr_avlist attrs;
    register Xv_menu_item_info *mi, **mip;
    register int    nitems, valid;
    int             submenus = FALSE, descend_first = FALSE;
    Xv_menu_info   *m;
    Menu(*gen_proc) ();
    Menu_item       mi_public;
    Menu_item(*gen_item_proc) ();
    Xv_menu_info   *m_base;
#ifdef	OW_I18N
    extern	char *calloc();
#endif

    if (!menu_public)
	return NULL;

    m_base = MENU_PRIVATE(menu_public);

    nitems = m_base->nitems;
    for (attrs = avlist; *attrs; attrs = attr_next(attrs))
	if (attrs[0] == MENU_DESCEND_FIRST)
	    descend_first = (int) attrs[1];

    if (gen_proc = m_base->gen_proc) {
	m = MENU_PRIVATE((gen_proc) (menu_public, MENU_DISPLAY));
	if (m == NULL) {
	    xv_error(menu_public,
		     ERROR_STRING,
#ifdef	OW_I18N
		         XV_I18N_MSG("xv_messages","menu_find: menu's gen_proc failed to generate a menu"),
#else
		         "menu_find: menu's gen_proc failed to generate a menu",
#endif
		     ERROR_PKG, MENU,
		     0);
	    return NULL;
	}
    } else {
	m = m_base;
    }

    nitems = m->nitems;
    for (mip = m->item_list; valid = TRUE, mi = *mip, nitems--; mip++) {

	if (gen_item_proc = mi->gen_proc) {
	    mi = MENU_ITEM_PRIVATE((gen_item_proc) (MENU_ITEM_PUBLIC(mi),
						    MENU_DISPLAY));
	    if (mi == NULL) {
		xv_error(menu_public,
			 ERROR_STRING,
#ifdef	OW_I18N
			     XV_I18N_MSG("xv_messages","menu_find: menu item's gen_proc failed to generate a menu item"),
#else
			     "menu_find: menu item's gen_proc failed to generate a menu item",
#endif
			 ERROR_PKG, MENU,
			 0);
		goto exit;
	    }
	}
	for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	    switch (attrs[0]) {

	      case MENU_ACTION:/* & case MENU_NOTIFY_PROC: */
		valid = mi->notify_proc == (Xv_opaque(*) ()) attrs[1];
		break;

	      case MENU_CLIENT_DATA:
		valid = mi->client_data == (Xv_opaque) attrs[1];
		break;

	      case MENU_FEEDBACK:
		valid = !mi->no_feedback == (unsigned) attrs[1];
		break;

	      case XV_FONT:
		valid = (mi->image.font ? mi->image.font : 0) == attrs[1];
		break;

	      case MENU_GEN_PROC:
		valid = mi->gen_proc == (Menu_item(*) ()) attrs[1];
		break;

	      case MENU_GEN_PULLRIGHT:
		valid = mi->pullright &&
		    mi->gen_pullright == (Menu(*) ()) attrs[1];
		break;

	      case MENU_IMAGE:
		valid = mi->image.svr_im == (Server_image) attrs[1];
		break;

	      case MENU_INACTIVE:
		valid = mi->inactive == (unsigned) attrs[1];
		break;

	      case MENU_INVERT:
		valid = mi->image.invert == (unsigned) attrs[1];
		break;

	      case MENU_PARENT:
		valid = mi->parent == MENU_PRIVATE(attrs[1]);
		break;

	      case MENU_PULLRIGHT:
		valid = mi->pullright && mi->value == (Xv_opaque) attrs[1];
		break;

#ifdef	OW_I18N
	      case MENU_STRING:{
		int  wcs_len;
		if( !mi->image.string ) {
		    valid = 0;
		    break;
		}
		if( mi->image.string_mbs ) {
		    valid = (strcmp( mi->image.string_mbs , (char *)attrs[1]) == 0 );
		}
		else {
		    wcs_len = wslen( mi->image.string );
		    mi->image.string_mbs = (char *)calloc( (wcs_len+1),sizeof(wchar_t) );
		    wcstombs(mi->image.string_mbs , mi->image.string , 
				(wcs_len+1) * sizeof(wchar_t) );
		    valid = (strcmp( mi->image.string_mbs , (char *)attrs[1]) == 0 );
		}
		break;
	      }
	      case MENU_STRING_WCS:
		valid = mi->image.string && wscmp(mi->image.string,
		    (wchar_t *) attrs[1]) == 0;
		break;
#else
	      case MENU_STRING:
		valid = mi->image.string && strcmp(mi->image.string,
		    (char *) attrs[1]) == 0;
		break;
#endif

	      case MENU_VALUE:
		valid = mi->value == (Xv_opaque) attrs[1];
		break;

	    }
	    if (!valid)
		break;
	}

	if (gen_item_proc)
	    (gen_item_proc) (MENU_ITEM_PUBLIC(mi), MENU_DISPLAY_DONE);

	if (valid)
	    goto exit;

	if (mi->pullright)
	    if (descend_first) {
		mi_public = menu_pkg_find(mi->value, MENUITEM, avlist);
		if (mi_public) {
		    mi = MENU_ITEM_PRIVATE(mi_public);
		    goto exit;
		}
	    } else {
		submenus = TRUE;
	    }
    }

    if (submenus) {
	nitems = m->nitems;
	for (mip = m->item_list; mi = *mip, nitems--; mip++)
	    if (mi->pullright) {
		mi_public = menu_pkg_find(mi->value, MENUITEM, avlist);
		if (mi_public) {
		    mi = MENU_ITEM_PRIVATE(mi_public);
		    goto exit;
		}
	    }
    }
    mi = NULL;

exit:
    if (gen_proc)
	(gen_proc) (menu_public, MENU_DISPLAY_DONE);

    return mi ? MENU_ITEM_PUBLIC(mi) : NULL;
}


Xv_private void
menu_select_default(menu_public)
    Menu            menu_public;
{
    Xv_menu_info   *menu = MENU_PRIVATE(menu_public);
    Xv_menu_item_info *mi;

    if (menu->default_position > menu->nitems)
	/* Menu has no items or only a pushpin: ignore request */
	return;
    menu->selected_position = menu->default_position;
    mi = menu->item_list[menu->selected_position - 1];
    if (mi && mi->pullright && mi->value)
	menu_select_default(mi->value);
}


Xv_private      Xv_opaque
menu_return_default(menu_public, depth)
    Menu            menu_public;
    int             depth;
{
    Xv_menu_info   *menu = MENU_PRIVATE(menu_public);
    Xv_menu_group_info *group;
    Xv_opaque       result;

    group = xv_alloc(Xv_menu_group_info);
    group->depth = depth;
    group->notify_proc = menu->notify_proc;
    if (!group->notify_proc)
	group->notify_proc = MENU_DEFAULT_NOTIFY_PROC;
    menu->notify_status = XV_OK;
    result = menu_return_result(menu, group, (Xv_menu_item_info *) 0);
    xv_free(group);
    return (result);
}


Xv_public void
menu_default_pin_proc(menu_public, x, y)
    Menu            menu_public;
    int             x, y;	/* fullscreen coordinate of top left corner
				 * of pinned window */
{
    Panel_item      default_panel_item;
    Rect           *frame_rect;
    int             i;
    Xv_menu_info   *menu = MENU_PRIVATE(menu_public);
    Xv_menu_item_info *mi;
    Panel           panel;

    if (!menu->pin_window)
	menu_create_pin_window(menu_public, menu->pin_parent_frame,
			       menu->pin_window_header);
    	
    /* Call any Pullright Generate procedures */
    for (i = 0; i < menu->nitems; i++) {
	mi = menu->item_list[i];
	if (mi->gen_pullright && !mi->value) {
	    mi->value = (mi->gen_pullright) (MENU_ITEM_PUBLIC(mi),
					     MENU_DISPLAY);
	    if (mi->panel_item_handle)
		xv_set(mi->panel_item_handle,
		       PANEL_ITEM_MENU, mi->value,
		       0);
	}
    }

    /* Set the pin window panel default item */
    default_panel_item =
	menu->item_list[menu->default_position - 1]->panel_item_handle;
    if (default_panel_item) {
	panel = xv_get(default_panel_item, XV_OWNER);
	xv_set(panel, PANEL_DEFAULT_ITEM, default_panel_item, 0);
    }

    /* Set the pin window's position if the window has not already
     * been shown.
     */
    frame_rect = (Rect *) xv_get(menu->pin_window, XV_RECT);
    if (xv_get(menu->pin_window, XV_KEY_DATA, XV_SHOW) != TRUE) {
	frame_rect->r_left = x;
	frame_rect->r_top = y;
#ifdef	OW_I18N
	menu->pin_window_rect.r_left = x;
	menu->pin_window_rect.r_top  = y;
#endif
    } else {
	frame_rect->r_left = menu->pin_window_rect.r_left;
	frame_rect->r_top = menu->pin_window_rect.r_top;
    }
    menu->pin_window_rect.r_width = frame_rect->r_width;
    menu->pin_window_rect.r_height = frame_rect->r_height;
    xv_set(menu->pin_window, XV_RECT, frame_rect, 0);

    /* Show the pin window.  Set a flag saying that the window has now
     * been shown.
     */
    menu->item_list[0]->inactive = TRUE;  /* first menu item is pushpin-title */
    xv_set(menu->pin_window,
	   FRAME_CMD_PUSHPIN_IN, TRUE,
	   XV_SHOW, TRUE,
	   XV_KEY_DATA, XV_SHOW, TRUE,
	   0);
}


static void
pin_button_notify_proc(item, event)
    Panel_item      item;
    Event          *event;
{
    register Xv_menu_info *m;
    Menu_item	    (*gen_proc)();
    Menu            menu;
    Menu_item       menu_item;
    void            (*notify_proc) ();

    menu = (Menu) xv_get(item, XV_KEY_DATA, MENU_KEY);
    menu_item = (Menu_item) xv_get(item, XV_KEY_DATA, MENU_ITEM_KEY);
    notify_proc = (void (*) ()) xv_get(item, XV_KEY_DATA, MENU_NOTIFY_PROC);
    gen_proc = (Menu_item (*) ()) xv_get(item, XV_KEY_DATA, MENU_GEN_PROC);

    /* Fake a MENU_FIRST_EVENT */
    m = MENU_PRIVATE(menu);
    m->group_info = xv_alloc(Xv_menu_group_info);
    m->group_info->first_event = *event;

    /* Invoke the menu item action proc */
    m->notify_status = XV_OK;
    if (gen_proc)
	(void) gen_proc(menu_item, MENU_NOTIFY);
    notify_proc(menu, menu_item);
    if (gen_proc)
	(void) gen_proc(menu_item, MENU_NOTIFY_DONE);
    xv_set(item, PANEL_NOTIFY_STATUS, m->notify_status, 0);

    xv_free(m->group_info);
}


/*ARGSUSED*/
static void
pin_choice_notify_proc(item, value, event)
    Panel_item	    item;
    int		    value;
    Event	   *event;
{
    int		    i;
    Xv_menu_info   *m;
    int		    mask;
    Menu            menu;
    int		    menu_item_index = 0;
    Xv_menu_item_info *mi = NULL;

    menu = (Menu) xv_get(item, XV_KEY_DATA, MENU_KEY);
    m = MENU_PRIVATE(menu);
    if (m->item_list[0]->title)
	menu_item_index++;
    if (m->class == MENU_CHOICE) {
	/* Update Choice Menu */
	menu_item_index += value;
	for (i=0; i<m->nitems; i++)
	    m->item_list[i]->selected = i == menu_item_index;
    } else {
	/* Update Toggle Menu */
	i = 0;
	if (m->item_list[0]->title)
	    i++;
	mask = 1;
	for (; i<m->nitems; i++) {
	    m->item_list[i]->selected = ((value & mask) != 0);
	    mask <<= 1;
	}
    }
    mi = m->item_list[menu_item_index];
    if (mi) {
	xv_set(item,
	       XV_KEY_DATA, MENU_ITEM_KEY, MENU_ITEM_PUBLIC(mi),
	       0);
	pin_button_notify_proc(item, event);
    }
}


Pkg_private void
menu_create_pin_panel_items(panel, menu)
    Panel           panel;
    Xv_menu_info   *menu;
{
    Panel_item	    choice_item = NULL;	/* choice or toggle item */
    int		    choice_nbr = 0;
    int             i;
    int		    mask;
    Xv_menu_item_info *mi;
    int		    toggle_value;

    xv_set(panel,
#ifdef OW_I18N
	   XV_FONT, menu->default_image.font,
#endif /*OW_I18N*/
	   PANEL_LAYOUT, PANEL_VERTICAL,
	   PANEL_ITEM_Y_GAP, menu->default_image.margin,
	   0);

    if (menu->class == MENU_CHOICE)
	choice_item = xv_create(panel, PANEL_CHOICE,
				PANEL_LAYOUT, PANEL_VERTICAL,
				PANEL_CHOICE_NCOLS, menu->ncols,
				XV_HELP_DATA,
				    xv_get(MENU_PUBLIC(menu), XV_HELP_DATA),
				0);
    else if (menu->class == MENU_TOGGLE)
	choice_item = xv_create(panel, PANEL_TOGGLE,
				PANEL_LAYOUT, PANEL_VERTICAL,
				PANEL_CHOICE_NCOLS, menu->ncols,
				XV_HELP_DATA,
				    xv_get(MENU_PUBLIC(menu), XV_HELP_DATA),
				0);

    for (i = 0; i < menu->nitems; i++) {
	mi = menu->item_list[i];
	if (mi->title)
	    continue;
	if (choice_item)
	    mi->panel_item_handle = choice_item;
	else
	    mi->panel_item_handle = xv_create(panel, PANEL_BUTTON,
		PANEL_INACTIVE, mi->inactive,
		PANEL_MENU_ITEM, TRUE,
		XV_HELP_DATA, xv_get(MENU_ITEM_PUBLIC(mi), XV_HELP_DATA),
		0);
	if (mi->image.string) {
	    if (choice_item)
		xv_set(choice_item,
#ifdef	OW_I18N
		       PANEL_CHOICE_STRING_WCS, choice_nbr++, mi->image.string,
#else
		       PANEL_CHOICE_STRING, choice_nbr++, mi->image.string,
#endif
		       0);
	    else
		xv_set(mi->panel_item_handle,
#ifdef	OW_I18N
		       PANEL_LABEL_STRING_WCS, mi->image.string,
#else
		       PANEL_LABEL_STRING, mi->image.string,
#endif
		       0);
	} else if (mi->image.svr_im) {
	    if (choice_item)
		xv_set(choice_item,
		       PANEL_CHOICE_IMAGE, choice_nbr++, mi->image.svr_im,
		       0);
	    else
		xv_set(mi->panel_item_handle,
		       PANEL_LABEL_IMAGE, mi->image.svr_im,
		       0);
	} else
	    xv_error(NULL,
		     ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
		     ERROR_STRING,
#ifdef	OW_I18N
	 XV_I18N_MSG("xv_messages","menu_create_pin_window: menu item does not have a string or image"),
#else
	 "menu_create_pin_window: menu item does not have a string or image",
#endif
		     ERROR_PKG, MENU,
		     0);
	if (mi->pullright) {
	    if (mi->gen_pullright)
		mi->value = (mi->gen_pullright) (MENU_ITEM_PUBLIC(mi),
						 MENU_DISPLAY);
	    xv_set(mi->panel_item_handle,
		   PANEL_ITEM_MENU, mi->value,
		   0);
	} else {
	    if (choice_item)
		xv_set(choice_item,
		   PANEL_NOTIFY_PROC, pin_choice_notify_proc,
		   XV_KEY_DATA, MENU_KEY, MENU_PUBLIC(menu),
		   XV_KEY_DATA, MENU_NOTIFY_PROC,
			mi->notify_proc ? mi->notify_proc : menu->notify_proc,
		   XV_KEY_DATA, MENU_GEN_PROC, mi->gen_proc,
		   0);
	    else
		xv_set(mi->panel_item_handle,
		   PANEL_NOTIFY_PROC, pin_button_notify_proc,
		   XV_KEY_DATA, MENU_KEY, MENU_PUBLIC(menu),
		   XV_KEY_DATA, MENU_ITEM_KEY, MENU_ITEM_PUBLIC(mi),
		   XV_KEY_DATA, MENU_NOTIFY_PROC,
			mi->notify_proc ? mi->notify_proc : menu->notify_proc,
		   XV_KEY_DATA, MENU_GEN_PROC, mi->gen_proc,
		   0);
	}
    }
    if (menu->class == MENU_CHOICE) {
	for (i=0; i < menu->nitems; i++)
	    if (menu->item_list[i]->selected)
		break;
	if (menu->item_list[0]->title)
	    i--;
	xv_set(choice_item, PANEL_VALUE, i, 0);
    } else if (menu->class == MENU_TOGGLE) {
	i = 0;
	if (menu->item_list[0]->title)
	    i++;
	mask = 1;
	toggle_value = 0;
	for (; i < menu->nitems; i++) {
	    if (menu->item_list[i]->selected)
		toggle_value |= mask;
	    mask <<= 1;
	}
	xv_set(choice_item, PANEL_VALUE, toggle_value, 0);
    }

    xv_set(panel, WIN_FIT_HEIGHT, 1, WIN_FIT_WIDTH, 1, 0);
}


Pkg_private void
menu_create_pin_window(menu_public, parent_frame, frame_label)
    Menu            menu_public;
    Frame           parent_frame;
    CHAR           *frame_label;
{
    Frame           cmd_frame;
    Panel           panel;
    Xv_menu_info   *menu = MENU_PRIVATE(menu_public);

    /*
     * Create the Command Frame, and fill in its panel.
     * The Command Frame is owned by the parent frame,
     * but it's X Window parent is the root window.
     * Thus, it's coordinates are expressed in fullscreen coordinates.
     */
    cmd_frame = xv_create(parent_frame, FRAME_CMD,
#ifdef	OW_I18N
			  WIN_USE_IM,		FALSE,
			  FRAME_LABEL_WCS,	frame_label,
#else
			  FRAME_LABEL, frame_label,
#endif
			  XV_SHOW, FALSE,
			  WIN_PARENT, xv_get(parent_frame, XV_ROOT),
			  0);

    panel = xv_get(cmd_frame, FRAME_CMD_PANEL);
    menu_create_pin_panel_items(panel, menu);

    xv_set(cmd_frame, WIN_FIT_HEIGHT, 0, WIN_FIT_WIDTH, 0, 0);

    menu_set_pin_window(menu, cmd_frame);
}


Xv_private void
menu_save_pin_window_rect(win)
    Xv_Window	    win;
{
    Xv_menu_info   *m = (Xv_menu_info *) xv_get(win, XV_KEY_DATA, MENU_MENU);

    if (m)
	frame_get_rect(win, &m->pin_window_rect);
}


Pkg_private Notify_value
menu_pin_window_event_proc(win, event, arg, type)
    Xv_Window win;
    Event *event;
    Notify_arg arg;
    Notify_event_type type;
{
    int		    i;
    Xv_menu_info   *m;
    Xv_menu_item_info *mi;

    if (event_action(event) == ACTION_DISMISS) {
	menu_save_pin_window_rect(win);
    } else if (event_action(event) == ACTION_CLOSE) {
	m = (Xv_menu_info *) xv_get(win, XV_KEY_DATA, MENU_MENU);
	if (m) {
	    m->item_list[0]->inactive = FALSE;
		/* first menu item is pushpin-title */
	    for (i = 0; i < m->nitems; i++) {
		mi = m->item_list[i];
		if (mi->gen_pullright) {
		    (mi->gen_pullright) (MENU_ITEM_PUBLIC(mi),
					 MENU_DISPLAY_DONE);
		    mi->value = 0;	/* MENU_DISPLAY_DONE complete */
		}
	    }
	}
    }
    return notify_next_event_func(win, event, arg, type);
}
