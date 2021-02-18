#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_select.c 50.7 90/12/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <strings.h>
#include <xview_private/panel_impl.h>
#include <xview_private/draw_impl.h>
#include <xview/scrollbar.h>
#include <xview/sel_attrs.h>
#include <xview/sel_svc.h>
#include <xview/font.h>
#ifdef OW_I18N
#include <widec.h>
#include <stdlib.h>
#include <xview_private/win_info.h>

extern	wchar_t	null_string_wc[];

#endif /* OW_I18N */

Pkg_private Item_info *panel_finditem();
static void     panel_drag_copy_move();
static int      event_in_view_window();

#define	CTRL_D_KEY	'\004'
#define	CTRL_G_KEY	'\007'

#define UP_CURSOR_KEY           (KEY_RIGHT(8))
#define DOWN_CURSOR_KEY         (KEY_RIGHT(14))
#define RIGHT_CURSOR_KEY        (KEY_RIGHT(12))
#define LEFT_CURSOR_KEY         (KEY_RIGHT(10))

/* VARARGS2 */
Pkg_private     Notify_value
panel_use_event(panel, event, arg, when, paint_window)
    register Panel_info *panel;
    register Event *event;
    Notify_arg      arg;
    Notify_event_type when;
    Xv_Window       paint_window;
{

    switch (event_action(event)) {
      case ACTION_FRONT:
      case ACTION_OPEN:
      case ACTION_PROPS:
	/* BUG: pass these on to the frame? */
	break;

      case ACTION_PASTE:
      case ACTION_CUT:
      case ACTION_COPY:
      case ACTION_FIND_FORWARD:
	panel_seln_inform(panel, event);
	break;

      default:
	break;
    }

    /* process the event */
    panel_default_event(PANEL_PUBLIC(panel), event, arg);

    /* notify next client of event */
    return (notify_next_event_func(paint_window, event, arg, when));
}


Pkg_private int
panel_event_is_xview_semantic(event)
    register Event *event;
{
    switch (event_action(event)) {
      case ACTION_AGAIN:
      case ACTION_PROPS:
      case ACTION_UNDO:
      case ACTION_FRONT:
      case ACTION_BACK:
      case ACTION_COPY:
      case ACTION_OPEN:
      case ACTION_CLOSE:
      case ACTION_PASTE:
      case ACTION_FIND_BACKWARD:
      case ACTION_FIND_FORWARD:
      case ACTION_CUT:
	return 1;
      default:
	return 0;
    }
}

/*
 * panel_event_is_panel_semantic(event) -- replaces event_is_key_left() in
 * panel_default_event() below
 */
Pkg_private int
panel_event_is_panel_semantic(event)
    register Event *event;
{
    switch (event_action(event)) {
      case ACTION_ERASE_CHAR_BACKWARD:
      case ACTION_ERASE_CHAR_FORWARD:
      case ACTION_ERASE_WORD_BACKWARD:
      case ACTION_ERASE_WORD_FORWARD:
      case ACTION_ERASE_LINE_BACKWARD:
      case ACTION_ERASE_LINE_END:
      case ACTION_GO_CHAR_BACKWARD:
      case ACTION_GO_CHAR_FORWARD:
      case ACTION_GO_WORD_BACKWARD:
      case ACTION_GO_WORD_END:
      case ACTION_GO_WORD_FORWARD:
      case ACTION_GO_LINE_BACKWARD:
      case ACTION_GO_LINE_END:
      case ACTION_GO_LINE_FORWARD:
      case ACTION_GO_COLUMN_BACKWARD:
      case ACTION_GO_COLUMN_FORWARD:
	return 1;
      default:
	return 0;
    }
}


static int
panel_default_event(p_public, event, arg)
    Panel           p_public;
    register Event *event;
    Notify_arg      arg;
{
    Xv_Window       paint_window = event_window(event);
    register Panel_info *panel;
    Item_info      *new;
    int             slider_active;
    Panel           panel_public;

    XV_OBJECT_TO_STANDARD(p_public, "panel_default_event", panel_public);
    panel = PANEL_PRIVATE(panel_public);

    if (is_panel(panel)) {	/* panel is a panel_handle */
	slider_active = panel->SliderActive;
    } else {			/* panel is a panel_item_handle */
	Item_info      *object = ITEM_PRIVATE(panel_public);
	slider_active = object->panel->SliderActive;
    }
    if (!slider_active) {
	/* find out who's under the locator */
	if (event_action(event) == SCROLLBAR_REQUEST)
	    new = (Item_info *) xv_get(arg, XV_KEY_DATA, PANEL_LIST);
	else
	    new = panel_finditem(panel, event);

	/* use the panel if not over some item */
	if (!new)
	    new = (Item_info *) panel;
    } else
	new = panel->current;

    /* cancel the old item if needed */
    if (new != panel->current) {
	if (panel->current)
	    panel_cancel(ITEM_PUBLIC(panel->current), event);
	panel->current = new;
	/*
	 * Map the event to an object-enter event: LOC_DRAG ->
	 * PANEL_EVENT_DRAG_IN.
	 */
	if (event_id(event) == LOC_DRAG)
	    event_id(event) = PANEL_EVENT_DRAG_IN;
    }
    /* If help event, call help request function. */
    if (event_action(event) == ACTION_HELP) {
	if (event_is_down(event)) {
	    char           *panel_help = (char *) xv_get(panel_public, XV_HELP_DATA);
	    char           *item_help = 0;

	    if (new != (Item_info *) panel)
		item_help = (char *) xv_get(ITEM_PUBLIC(new), XV_HELP_DATA);
	    if (item_help)
		panel_help = item_help;
	    if (panel_help) {
		xv_help_show(paint_window, panel_help, event);
		return (int) NOTIFY_DONE;
	    } else
		return (int) NOTIFY_IGNORED;
	}
    } else if (event_is_iso(event) ||
	panel_event_is_panel_semantic(event) ||
	panel_event_is_xview_semantic(event)) {
	if (panel->kbd_focus_item)
	    /* Give the key event to the keyboard focus item */
	    panel_handle_event(ITEM_PUBLIC(panel->kbd_focus_item), event);
	else if (wants_key(panel))
	    /* Give the key event to the panel background proc */
	    panel_handle_event(PANEL_PUBLIC(panel), event);
    } else {
	/* Give the non-key event to the item under the pointer */
	(void) panel_handle_event(ITEM_PUBLIC(new), event);
    }
    return (int) NOTIFY_DONE;
}

/*
 * BUG: all these ops routines should pass the client_object to the low-level
 * ops routine.
 */

Sv1_public
panel_handle_event(client_object, event)
    Panel_item      client_object;	/* could be a Panel */
    Event          *event;
{
    Item_info      *object = ITEM_PRIVATE(client_object);

    if (!object)
	return;

    (*object->ops->handle_event) (client_object, event);
}


Sv1_public
panel_begin_preview(client_object, event)
    Panel_item      client_object;	/* could be a Panel */
    Event          *event;
{
    Item_info      *object = ITEM_PRIVATE(client_object);

    if (!object)
	return;

    (*object->ops->begin_preview) (object, event);
}


Sv1_public
panel_update_preview(client_object, event)
    Panel_item      client_object;	/* could be a Panel */
    Event          *event;
{
    Item_info      *object = ITEM_PRIVATE(client_object);

    if (!object)
	return;

    (*object->ops->update_preview) (object, event);
}


Sv1_public
panel_accept_preview(client_object, event)
    Panel_item      client_object;	/* could be a Panel */
    Event          *event;
{
    Item_info      *object = ITEM_PRIVATE(client_object);
    Panel_info     *panel;

    if (!object)
	return;

    (*object->ops->accept_preview) (object, event);

    if (is_item(object))
	panel = object->panel;
    else
	panel = PANEL_PRIVATE(client_object);
    panel->current = NULL;
}


Sv1_public
panel_cancel_preview(client_object, event)
    Panel_item      client_object;	/* could be a Panel */
    Event          *event;
{
    Item_info      *object = ITEM_PRIVATE(client_object);
    Panel_info     *panel;

    if (!object)
	return;

    (*object->ops->cancel_preview) (object, event);

    if (is_item(object))
	panel = object->panel;
    else
	panel = PANEL_PRIVATE(client_object);
    panel->current = NULL;
}


Sv1_public
panel_accept_menu(client_object, event)
    Panel_item      client_object;	/* could be a Panel */
    Event          *event;
{
    Item_info      *object = ITEM_PRIVATE(client_object);
    Panel_info     *panel;

    if (!object)
	return;

    if (event_is_down(event))
	(*object->ops->accept_menu) (object, event);
    else {
	if (is_item(object))
	    panel = object->panel;
	else
	    panel = PANEL_PRIVATE(client_object);
	panel->current = NULL;
    }
}


Sv1_public
panel_accept_key(client_object, event)
    Panel_item      client_object;	/* could be a Panel */
    Event          *event;
{
    Item_info      *object = ITEM_PRIVATE(client_object);

    if (!object)
	return;

    (*object->ops->accept_key) (object, event);
}


Sv1_public
panel_cancel(client_object, event)
    Panel_item      client_object;	/* could be a Panel */
    Event          *event;
{
    Event           cancel_event;

    if (!client_object)
	return;

    cancel_event = *event;
    event_id(&cancel_event) = PANEL_EVENT_CANCEL;
    cancel_event.action = PANEL_EVENT_CANCEL;
    (void) panel_handle_event(client_object, &cancel_event);
}


Xv_public void
panel_default_handle_event(client_object, event)
    Panel_item      client_object;	/* could be a Panel */
    register Event *event;
{
    int             accept;
    Item_info      *object = ITEM_PRIVATE(client_object);
    Xv_Drawable_info *info;
    Item_info      *ip = NULL;
    Panel_info     *panel = PANEL_PRIVATE(client_object);
    Xv_object       screen;
    Xv_object       server;

    if (is_item(object)) {
	if (inactive(object))
	    return;
	ip = object;
	panel = ip->panel;
    }
    switch (event_action(event)) {
      case ACTION_ADJUST:	/* middle button up or down */
	if (panel->SliderActive)
	    break;
	if (!ip || ip->item_type != PANEL_TEXT_ITEM) {
	    /* BUG ALERT: Display "?" cursor on down, restore normal cursor
	     * and set panel->current to NULL on up.
	     */
	    panel->current = NULL;
	    break;
	} /* else execute ACTION_SELECT code */

      case ACTION_SELECT:	/* left button up or down */
	if (select_displays_menu(panel) && !panel->SliderActive &&
	    ip && ip->menu) {
	    /* SELECT over menu button => show menu */
	    (void) panel_accept_menu(client_object, event);
	    break;
	}
	if (event_is_up(event)) {
	    if (ip) {
		ip->flags &= ~PREVIEWING;
		if (!panel_has_input_focus(panel))
		    panel->status &= ~QUICK_MOVE;
		/*
		 * If the SELECT button went up inside any of the panel's
		 * view windows, then accept the preview; else, cancel the
		 * preview.
		 */
		accept = event_in_view_window(panel, event);
	    } else
		accept = FALSE;
	    if (accept)
		panel_accept_preview(client_object, event);
	    else
		panel_cancel_preview(client_object, event);
	} else {
	    /* SELECT down: begin preview */
	    if (ip) {
		if (ip->item_type == PANEL_TEXT_ITEM &&
		    !panel_has_input_focus(panel)) {
		    /*
		     * Remote Quick Move/Copy: Get Selection Function Key
		     * State
		     */
		    ip->flags |= PREVIEWING;
		    panel->status &= ~QUICK_MOVE;	/* assume quick copy */
		    screen = xv_get(PANEL_PUBLIC(panel), XV_SCREEN);
		    server = xv_get(screen, SCREEN_SERVER);
		    if (server_get_seln_function_pending(server)) {
			Seln_holder     holder;
			char           *data;

			holder = seln_inquire(SELN_CARET);
			if (holder.state != SELN_NONE) {
			    Seln_request   *result;

			    result = seln_ask(&holder,
					      SELN_REQ_FUNC_KEY_STATE, 0,
					      0);
			    data = result->data;
			    data += sizeof(Seln_attribute);
			    if (*(int *) data == (int) SELN_FN_CUT)
				panel->status |= QUICK_MOVE;
			}
		    }
		}
		/* Process SELECT-down event */
		panel_begin_preview(client_object, event);
            }
	    if (!panel_has_input_focus(panel) &&
		panel->kbd_focus_item) {
		/* SELECT down over panel background or item that doesn't
		 * accept the input focus: set the input focus to the panel.
		 * Set caret to last kbd_focus_item, or the first kbd_focus_item
		 * if there isn't a previous one.
                 */
                DRAWABLE_INFO_MACRO(event_window(event), info);
                win_set_kbd_focus(event_window(event), xv_xid(info));
            }
	}
	break;

      case ACTION_MENU:	/* right button up or down */
	if (!panel->SliderActive)
	    (void) panel_accept_menu(client_object, event);
	break;

      case ACTION_CUT:
	if (panel_has_input_focus(panel)) {
	    /* Would be local Quick Move: set flag */
	    if (event_is_up(event))
		panel->status &= ~QUICK_MOVE;
	    else
		panel->status |= QUICK_MOVE;
	}
	(void) panel_accept_key(client_object, event);
	break;

      case ACTION_DRAG_MOVE:
	if (ip)
	    panel_drag_copy_move(ip, event, TRUE);
	break;
      case ACTION_DRAG_COPY:
	if (ip)
	    panel_drag_copy_move(ip, event, FALSE);
	break;

      case PANEL_EVENT_DRAG_IN:/* drag into item rect */
	if (event_left_is_down(event))
	    (void) panel_begin_preview(client_object, event);
	break;

      case LOC_DRAG:		/* left, middle, or right drag */
	if (event_left_is_down(event)
	    || event_middle_is_down(event)
	    )
	    (void) panel_update_preview(client_object, event);
	if (event_right_is_down(event))
	    panel_accept_menu(client_object, event);
	break;

      case PANEL_EVENT_CANCEL:
	if (panel->SliderActive) {
	    event_set_id(event, LOC_DRAG);	/* pass more events */
	    (void) panel_update_preview(client_object, event);
	    break;
	} else
	    (void) panel_cancel_preview(client_object, event);
	break;

      default:	/* some other event */
#ifdef OW_I18N
	if (event_is_string(event) || event_is_iso(event) ||
	    (event_is_key_right(event) && event_is_down(event)) ||
	    panel_event_is_panel_semantic(event) ||
	    panel_event_is_xview_semantic(event))
#else
	if (event_is_iso(event) ||
	    (event_is_key_right(event) && event_is_down(event)) ||
	    panel_event_is_panel_semantic(event) ||
	    panel_event_is_xview_semantic(event))
#endif /* OW_I18N */
	    (void) panel_accept_key(client_object, event);
	break;
    }
}

Pkg_private Item_info *
panel_finditem(panel, event)
    Panel_info     *panel;
    Event          *event;
{
    register Item_info *ip = panel->current;
    register int    x = event_x(event);
    register int    y = event_y(event);

    if (!panel->items || !event_in_view_window(panel, event) ||
	event_id(event) == WIN_REPAINT || event_id(event) == WIN_RESIZE)
	return NULL;

    if (ip && is_item(ip) && !hidden(ip) &&
	(rect_includespoint(&ip->rect, x, y) ||
	 (previewing(ip) && ip->item_type == PANEL_TEXT_ITEM)))
	return ip;

    for (ip = hidden(panel->items) ?
	 panel_successor(panel->items) : panel->items;
	 ip && !rect_includespoint(&ip->rect, x, y);
	 ip = panel_successor(ip));

    return ip;
}


/*
 * If the event occurred inside any of the panel's view windows, then return
 * TRUE, else return FALSE.
 */
static int
event_in_view_window(panel, event)
    register Panel_info     *panel;
    register Event          *event;
{
    register Xv_Window       pw;

    PANEL_EACH_PAINT_WINDOW(panel, pw)
	if (rect_includespoint(panel_viewable_rect(panel, pw),
			       event_x(event), event_y(event)))
	    return TRUE;
    PANEL_END_EACH_PAINT_WINDOW
    return FALSE;
}

#ifdef OW_I18N
static void
panel_drag_copy_move(ip, event, is_move)
    Item_info      *ip;
    Event          *event;
    int             is_move;
{
    Seln_holder     holder;
    Seln_request    *result;
    char            *cp, *string, *data;
    int             is_read_only;
    Event           insert_event;

    /* New variables for OW_I18N */

    wchar_t         *string_wc;
    int		    num_chars;
    Attr_avlist	    avlist;

    if (ip->item_type != PANEL_TEXT_ITEM)
	return;

    /*
     * Get text to insert
     */
    /* BUG: This is a performance problem we should look at later */
    holder = seln_inquire(SELN_PRIMARY);
    result = seln_ask(&holder, SELN_REQ_IS_READONLY, 0, 0);
    data = result->data;
    /* Test if is SELN_IS_READONLY */
    data += sizeof(Seln_attribute);
    is_read_only = *(int *) data;
    if (is_read_only)
	return;
    /*
     * Check for Sundae client first, if so then request for
     * wide char content, otherwise just ask for regular
     * ascii content.
     */
    result = seln_ask(&holder, SELN_REQ_CHARSIZE, NULL,
			NULL);
    if (result->status == SELN_FAILED)
	return;
    avlist = (Attr_avlist) result->data;

    if ((Seln_attribute) * avlist++ != SELN_REQ_CHARSIZE) {
    
	result = seln_ask(&holder, SELN_REQ_CONTENTS_ASCII, 0, 0);
	avlist = (Attr_avlist) result->data;

	/* Test if is SELN_REQ_CONTENTS_ASCII */
	if ((Seln_attribute) * avlist++ != SELN_REQ_CONTENTS_ASCII)
	    return;

#ifdef SVR4
	string = malloc(strlen((u_char*) avlist) + 1);
#else
	string = malloc(strlen((u_char*) avlist));
#endif SVR4
	strcpy(string, (u_char*)avlist);
    }
    else {
	result = seln_ask(&holder, SELN_REQ_CHARSIZE, NULL,
				   SELN_REQ_CONTENTS_WCS, NULL,
				   NULL);
	avlist = (Attr_avlist) result->data;

	/* Test if is SELN_REQ_CHARSIZE */
	if ((Seln_attribute) * avlist++ != SELN_REQ_CHARSIZE)
	    return;
	num_chars = (int) * avlist++;

	/* Test if is SELN_REQ_CONTENTS_WCS */
	if ((Seln_attribute) * avlist++ != SELN_REQ_CONTENTS_WCS)
	    return;
	string_wc = (wchar_t *) avlist;
	string_wc[num_chars] = *null_string_wc;
	string = wcstombsdup (string_wc);
    }

    /*
     * Delete from selection holder if operation is drag move
     */
    if (is_move)
	seln_ask(&holder, SELN_REQ_DELETE, 0, 0);

    /*
     * Insert the selection
     */
    /* Set the insert point (i.e., caret) */
    insert_event = *event;
    insert_event.action = ACTION_SELECT;
    event_set_down(&insert_event);
    (*ip->ops->begin_preview) (ip, &insert_event);
    event_set_up(&insert_event);
    (*ip->ops->accept_preview) (ip, &insert_event);
    cp = string;

    {
	/* Chop up content of cp if it's greater than 256
	 * characters, put valid multibyte characters into the
	 * variable buffer, then stuff buffer into ie_string.
	 * Do this until the entire string has been sent.
	 * Need to stuff one character into ie_code so that
	 * panel event handling routines work properly.
	 * Control characters have to be handled separately
	 * in ie_code one at a time
	 */

	char			tmp_buffer[BUFFERSIZE];
	char			*mb_ptr;
	wchar_t			wc;
	register int		i, j, nbytes;


	mb_ptr = cp;
	while (*cp) {
	   
	    /* Insert the text */
	    event_init(&insert_event);
	    insert_event.action = ACTION_NULL_EVENT; /* use event code */
	    event_set_down(&insert_event);
     
	    /* Check for control characters, send them separately */
	    if (iscntrl(*mb_ptr)) {
		event_set_id(&insert_event, *mb_ptr);
		mb_ptr++;
		cp++;
		(*ip->ops->accept_key) (ip, &insert_event);
	    }
	    else {
		for (i=BUFFERSIZE, nbytes=0; i > 0 && *mb_ptr
			&& !iscntrl(*mb_ptr);)
		{
		    if (( j = mbtowc(&wc, mb_ptr, MB_CUR_MAX)) < 0)
			break;
		    mb_ptr += j;
		    i -= j;
		    nbytes += j;
	 
		}
		bcopy(cp, tmp_buffer, nbytes);
		tmp_buffer[nbytes] = NULL;
		cp = mb_ptr;
		event_set_id(&insert_event, tmp_buffer[0]);
		event_set_string(&insert_event, tmp_buffer);
		(*ip->ops->accept_key) (ip, &insert_event);
	    }
	}
    }
    free((char *)string);
}
#else
static void
panel_drag_copy_move(ip, event, is_move)
    Item_info      *ip;
    Event          *event;
    int             is_move;
{
    Seln_holder     holder;
    Seln_request   *result;
    char           *cp, *data, *string;
    int             is_read_only;
    Event           insert_event;

    if (ip->item_type != PANEL_TEXT_ITEM)
	return;

    /*
     * Get text to insert
     */
    /* BUG: This is a performance problem we should look at later */
    holder = seln_inquire(SELN_PRIMARY);
    result = seln_ask(&holder, SELN_REQ_IS_READONLY, 0, 0);
    data = result->data;
    /* Test if is SELN_IS_READONLY */
    data += sizeof(Seln_attribute);
    is_read_only = *(int *) data;
    if (is_read_only)
	return;
    result = seln_ask(&holder, SELN_REQ_CONTENTS_ASCII, 0, 0);
    data = result->data;
    /* Test if is SELN_REQ_CONTENTS_ASCII */
    data += sizeof(Seln_attribute);
#ifndef SVR4
    string = malloc(strlen(data)+1);
#else SVR4
    string = malloc(strlen(data));
#endif SVR4
    strcpy(string, data);

    /*
     * Delete from selection holder if operation is drag move
     */
    if (is_move)
	seln_ask(&holder, SELN_REQ_DELETE, 0, 0);

    /*
     * Insert the selection
     */
    /* Set the insert point (i.e., caret) */
    insert_event = *event;
    insert_event.action = ACTION_SELECT;
    event_set_down(&insert_event);
    (*ip->ops->begin_preview) (ip, &insert_event);
    event_set_up(&insert_event);
    (*ip->ops->accept_preview) (ip, &insert_event);
    /* Insert the text */
    insert_event.action = ACTION_NULL_EVENT;	/* use event code */
    event_set_down(&insert_event);
    cp = string;
    while (*cp) {
	event_id(&insert_event) = (short) *cp++;
	(*ip->ops->accept_key) (ip, &insert_event);
    }

    free(string);
}
#endif /* OW_I18N */
