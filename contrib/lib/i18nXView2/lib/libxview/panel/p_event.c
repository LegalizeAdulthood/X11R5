#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_event.c 70.3 91/08/22";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */


#include <xview_private/panel_impl.h>
#include <xview/font.h>
#include <xview_private/draw_impl.h>
#include <X11/Xlib.h>

static Notify_value panel_itimer_expired();

/* ARGSUSED */
Pkg_private     Notify_value
panel_notify_panel_event(window, event, arg, type)
    Xv_Window       window;
    register Event *event;
    Notify_arg      arg;
    Notify_event_type type;
{
    Panel_info     *panel = PANEL_PRIVATE(window);

    switch (event_action(event)) {
      case ACTION_RESCALE:
	panel_refont(panel, (int) arg);
	return notify_next_event_func(window, event, arg, type);
      default:
	return notify_next_event_func(window, event, arg, type);
    }
}


Pkg_private     Notify_value
panel_notify_event(paint_window, event, arg, type)
    Xv_Window       paint_window;
    register Event *event;
    Notify_arg      arg;
    Notify_event_type type;
{
    register Panel_info *panel = (Panel_info *) xv_get(paint_window,
					    XV_KEY_DATA, panel_context_key);
#ifdef	OW_I18N
    XIC		ic;
#endif	OW_I18N

    switch (event_action(event)) {
      case WIN_REPAINT:
	if (!panel->paint_window->view) {
	    Rectlist       *win_damage, damage;

	    if (!(win_damage = win_get_damage(paint_window))) {
		win_damage = &rl_null;
	    }
	    damage = rl_null;
	    rl_copy(win_damage, &damage);
	    panel_redisplay(paint_window, paint_window, &damage);
	    rl_free(&damage);
	    return NOTIFY_DONE;
	}
	break;

      case KBD_USE:
#ifdef	OW_I18N
	if ((xv_get(PANEL_PUBLIC(panel), WIN_USE_IM)) && panel->ic ) {
	        Xv_Drawable_info *info;
        	
        	DRAWABLE_INFO_MACRO(paint_window, info);
	        XSetICValues(panel->ic, XNFocusWindow, xv_xid(info), NULL);
		XSetICFocus(panel->ic);
	}
#endif	OW_I18N
	(void) frame_kbd_use((Xv_Window) xv_get(PANEL_PUBLIC(panel),
					   WIN_FRAME), PANEL_PUBLIC(panel));
	if (blinking(panel) && panel->kbd_focus_item)
	    panel_itimer_set(PANEL_PUBLIC(panel), panel->timer_full);
	if (panel->kbd_focus_item &&
	    panel->kbd_focus_item->item_type == PANEL_TEXT_ITEM) {
	    /* Clear gray diamond caret */
	    panel_text_caret_on(panel, FALSE);
	}
	panel->caret = OLGX_ACTIVE_CARET;
	panel->caret_ascent = panel->active_caret_ascent;
	panel->caret_height = panel->active_caret_height;
	panel->caret_width = panel->active_caret_width;
	panel_accept_kbd_focus(panel);
	panel->status |= PANEL_HAS_INPUT_FOCUS;
	return NOTIFY_DONE;

      case KBD_DONE:
	(void) frame_kbd_done((Xv_Window) xv_get(PANEL_PUBLIC(panel),
					   WIN_FRAME), PANEL_PUBLIC(panel));
	if (panel->kbd_focus_item &&
	    panel->kbd_focus_item->item_type == PANEL_TEXT_ITEM) {
	    panel_text_caret_on(panel, FALSE);
	    panel_itimer_set(PANEL_PUBLIC(panel), NOTIFY_NO_ITIMER);
	}
	panel->caret = OLGX_INACTIVE_CARET;
	panel->caret_ascent = panel->inactive_caret_ascent;
	panel->caret_height = panel->inactive_caret_height;
	panel->caret_width = panel->inactive_caret_width;
	if (panel->kbd_focus_item) {
	    if (panel->kbd_focus_item->item_type == PANEL_TEXT_ITEM)
		panel_text_caret_on(panel, TRUE);
	    else
		panel_yield_kbd_focus(panel);
	}
	panel->status &= ~PANEL_HAS_INPUT_FOCUS;
	return NOTIFY_DONE;

      case ACTION_RESCALE:{
	    panel_refont(panel, (int) arg);
	    break;
	}

      /* Ignore graphics expose events */
      case WIN_GRAPHICS_EXPOSE:
      case WIN_NO_EXPOSE:
      /* Ignore modifier KeyPress & KeyRelease events */
      case SHIFT_LEFT:
      case SHIFT_RIGHT:
      case SHIFT_LEFTCTRL:
      case SHIFT_RIGHTCTRL:
      case SHIFT_META:
	return NOTIFY_IGNORED;

      default:
	break;
    }
    return panel_use_event(panel, event, arg, type, paint_window);
}


/* ARGSUSED */
static          Notify_value
panel_itimer_expired(panel_public, which)
    Panel           panel_public;
    int             which;
{
    register Panel_info *panel = PANEL_PRIVATE(panel_public);

    if (blinking(panel)) {
	panel_caret_invert(panel);
    } else {
	/* turn itimer off */
	panel_itimer_set(panel_public, NOTIFY_NO_ITIMER);
    }
    return NOTIFY_DONE;
}

Pkg_private void
panel_itimer_set(panel_public, timer)
    register Panel panel_public;
    struct itimerval timer;
{

    (void) notify_set_itimer_func((Notify_client) panel_public,
	 panel_itimer_expired, ITIMER_REAL, &timer, (struct itimerval *) 0);
}
