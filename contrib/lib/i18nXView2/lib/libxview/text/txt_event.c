#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)txt_event.c 50.6 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * New style, notifier-based, event and timer support by text subwindows.
 */


#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#include <xview/xv_i18n.h>
#endif OW_I18N
#include <xview_private/primal.h>
#include <xview_private/draw_impl.h>
#include <xview_private/txt_impl.h>
#include <errno.h>
#include <xview_private/win_keymap.h>


extern int      errno;
extern Notify_error win_post_event();
extern Ev_status ev_set();
extern void     ev_blink_caret();

pkg_private int  textsw_is_typing_pending();
pkg_private void textsw_stablize();
pkg_private void textsw_start_blinker();
pkg_private void textsw_stop_blinker();
pkg_private Notify_value textsw_blink();
pkg_private void textsw_hide_caret();
pkg_private void textsw_show_caret();

pkg_private     Textsw_folio
textsw_folio_for_view(view)
    Textsw_view_handle view;
{
    ASSUME(view->magic == TEXTSW_VIEW_MAGIC);
    ASSUME(view->folio->magic == TEXTSW_MAGIC);
    return (view->folio);
}

/*
 *	there are a couple of crashes that appear to
 *	end up in this routine. therefore a blanket is 
 *	being placed around it to protect minimally 
 *	from bad pointer problems. If anything goes
 *	wrong a NULL pointer is passed back, which
 *	will bubble logic errors back out to the 
 *	place where (we hope) they originate. 6/20/89
 */
pkg_private     Textsw_view_handle
textsw_view_abs_to_rep(abstract)
    Textsw          abstract;
{
    Textsw_view_handle view;

    if( abstract == NULL )
	return NULL;

    view = VIEW_PRIVATE(abstract);

    if( view == NULL )
	return NULL;

    if (view->magic != TEXTSW_VIEW_MAGIC) {
	Textsw_folio    folio = TEXTSW_PRIVATE(abstract);

   	if( folio == NULL )
		return NULL;

	view = folio->first_view;
    }
    return (view);
}

pkg_private     Textsw
textsw_view_rep_to_abs(rep)
    Textsw_view_handle rep;
{
    ASSUME((rep == 0) || (rep->magic == TEXTSW_VIEW_MAGIC));
    return (VIEW_PUBLIC(rep));
}

/* ARGSUSED */
Pkg_private     Notify_value
textsw_event(view_public, event, arg, type)
    Textsw_view     view_public;
    register Event *event;
    Notify_arg      arg;
    Notify_event_type type;	/* Currently unused */
{
    Textsw_view_handle view = VIEW_PRIVATE(view_public);
    register Textsw_folio folio = FOLIO_FOR_VIEW(view);
    register int    process_status;
    Xv_Drawable_info *info;
    Xv_object       frame;
    static short    read_status;

    folio->state |= TXTSW_DOING_EVENT;
    switch (event_id(event)) {
      case ACTION_RESCALE:
	/* don't need to do anything since frame will refont us */
	break;
      case LOC_WINENTER:  /* only enabled in Follow-Mouse mode */
	DRAWABLE_INFO_MACRO(view_public, info);
	win_set_kbd_focus(view_public, xv_xid(info));
	break;
      case KBD_USE:
	textsw_hide_caret(folio); /* To get rid of ghost */
	folio->state |= TXTSW_HAS_FOCUS;
	if (folio->caret_state & TXTSW_CARET_FLASHING)
	    textsw_start_blinker(folio);
	(void) ev_set(view->e_view, EV_CHAIN_CARET_IS_GHOST, FALSE, 0);
	if (frame = xv_get(FOLIO_REP_TO_ABS(folio), WIN_FRAME))
	    frame_kbd_use(frame, FOLIO_REP_TO_ABS(folio));
	goto Default;
      case KBD_DONE:
	textsw_hide_caret(folio); /* To get rid of solid */
	folio->state &= ~TXTSW_HAS_FOCUS;
	(void) ev_set(view->e_view, EV_CHAIN_CARET_IS_GHOST, TRUE, 0);
	if (frame = xv_get(FOLIO_REP_TO_ABS(folio), WIN_FRAME))
	    frame_kbd_done(frame, FOLIO_REP_TO_ABS(folio));
	textsw_stop_blinker(folio);
	goto Default;
      case WIN_MAP_NOTIFY:
	view->state |= TXTSW_VIEW_IS_MAPPED;
	goto Default;
      case WIN_UNMAP_NOTIFY:
	view->state &= ~TXTSW_VIEW_IS_MAPPED;
	goto Default;

      default:{
    Default:
	    process_status = textsw_process_event(view_public, event, arg);
	    if (process_status & TEXTSW_PE_READ_ONLY) {
		if (!read_status) {
		    textsw_read_only_msg(folio, event_x(event), event_y(event));
		    read_status = 1;
		}
	    } else if (process_status == 0) {
		if (read_status)
		    read_status = 0;
		goto Return;
	    }
	    break;
	}
    }
Return:
    /* Stablize window if no more typing expected */
    if (!textsw_is_typing_pending(folio, event))
	textsw_stablize(folio, 0);
    folio->state &= ~TXTSW_DOING_EVENT;
    return notify_next_event_func(view_public, (Notify_event) event, arg, type);
}

/* ARGSUSED */
pkg_private     void
textsw_stablize(folio, blink)
    register Textsw_folio folio;
    int blink;
{
    /* Flush if pending */
    if ((folio->to_insert_next_free != folio->to_insert) &&
	((folio->func_state & TXTSW_FUNC_FILTER) == 0))
	textsw_flush_caches(folio->first_view, TFC_STD);
    /* Display caret */
    if (blink)
	textsw_invert_caret(folio);
    else
	textsw_show_caret(folio);
    /* update the scrollbars if needed */
    textsw_real_update_scrollbars(folio);
}

/*
 * When called from outside this module, it is telling the system that it
 * wants the cursor put back up, because the caller had removed it.
 */
/* ARGSUSED */
pkg_private     Notify_value
textsw_timer_expired(folio, which)
    register Textsw_folio folio;
    int             which;	/* Currently unused */
{
    textsw_show_caret(folio);
    return (NOTIFY_DONE);
}

/*
 * When called from outside this module, it is telling the system that it
 * is no longer readonly.
 */
pkg_private void
textsw_add_timer(folio, timeout)
    register Textsw_folio folio;
    register struct timeval *timeout;
{
    if (!(folio->state & TXTSW_DOING_EVENT))
	/* So that caret will be put back later */
	textsw_start_blinker(folio);
    /* else exiting textsw_event will put caret back */
}

/* Means really pull the caret down and keep it down */
pkg_private void
textsw_remove_timer(folio)
    register Textsw_folio folio;

{
    textsw_stop_blinker(folio);
    textsw_hide_caret(folio);
}

/* Means really pull the caret down, but make sure that it gets up later */
pkg_private void
textsw_take_down_caret(textsw)
    register Textsw_folio textsw;
{
    textsw_hide_caret(textsw);
    if (!(textsw->state & TXTSW_DOING_EVENT))
	/* So that caret can be put back later */
	textsw_start_blinker(textsw);
    /* else exiting textsw_event will put caret back */
}

/* ARGSUSED */
pkg_private     Notify_value
textsw_blink(folio, which)
    register Textsw_folio folio;
    int             which;	/* Currently unused */
{
    /* If views are zero then we are coming in after destruction of the folio */
    if (!(folio->views))
	return (NOTIFY_DONE);
    textsw_stablize(folio, (folio->caret_state & TXTSW_CARET_FLASHING)? 1: 0);
    if (notify_get_itimer_func((Notify_client) folio, ITIMER_REAL) ==
	NOTIFY_FUNC_NULL)
	folio->caret_state &= ~TXTSW_CARET_TIMER_ON;
    else
	folio->caret_state |= TXTSW_CARET_TIMER_ON;

    return (NOTIFY_DONE);
}

pkg_private void
textsw_start_blinker(folio)
    register Textsw_folio folio;
{
    struct itimerval itimer;

    if ((folio->caret_state & TXTSW_CARET_TIMER_ON) ||
        (TXTSW_IS_READ_ONLY(folio)))
	return;
    if ((folio->caret_state & TXTSW_CARET_FLASHING) &&
	(folio->state & TXTSW_HAS_FOCUS)) {
        /* Set interval timer to be repeating */
	itimer.it_value = folio->timer;
        itimer.it_interval = folio->timer;
     }else {
        /* Set interval timer come back ASAP, and not repeat */
	itimer.it_value = NOTIFY_POLLING_ITIMER.it_value;
        itimer.it_interval = NOTIFY_NO_ITIMER.it_interval;
    }
    if (NOTIFY_FUNC_NULL == notify_set_itimer_func((Notify_client) folio,
	textsw_blink, ITIMER_REAL, &itimer, (struct itimerval *) 0)) {
	notify_perror(
#ifdef OW_I18N
		XV_I18N_MSG("xv_messages", "textsw adding timer")
#else
		"textsw adding timer"
#endif
	);
	folio->caret_state &= ~TXTSW_CARET_TIMER_ON;
    } else
	folio->caret_state |= TXTSW_CARET_TIMER_ON;
}

pkg_private void
textsw_stop_blinker(folio)
    register Textsw_folio folio;
{
    if (!(folio->caret_state & TXTSW_CARET_TIMER_ON))
	return;
    /* Stop interval timer */
    if (NOTIFY_FUNC_NULL == notify_set_itimer_func((Notify_client) folio,
        textsw_blink, ITIMER_REAL, &NOTIFY_NO_ITIMER, (struct itimerval *) 0))
	notify_perror(
#ifdef OW_I18N
		XV_I18N_MSG("xv_messages", "textsw removing timer")
#else
		"textsw removing timer"
#endif
	);
    folio->caret_state &= ~TXTSW_CARET_TIMER_ON;
}

pkg_private void
textsw_show_caret(textsw)
    register Textsw_folio textsw;
{

    if ((textsw->caret_state & (TXTSW_CARET_ON | TXTSW_CARET_FROZEN)) ||
	TXTSW_IS_READ_ONLY(textsw) ||
	TXTSW_IS_BUSY(textsw))
	return;
    ev_blink_caret(textsw->views, 1);
    textsw->caret_state |= TXTSW_CARET_ON;
}

pkg_private void
textsw_hide_caret(textsw)
    register Textsw_folio textsw;
{

    if (!(textsw->caret_state & TXTSW_CARET_ON) ||
	(textsw->caret_state & TXTSW_CARET_FROZEN))
	return;
    ev_blink_caret(textsw->views, 0);
    textsw->caret_state &= ~TXTSW_CARET_ON;
}

pkg_private void
textsw_freeze_caret(textsw)
    register Textsw_folio textsw;
{
    textsw->caret_state |= TXTSW_CARET_FROZEN;
}

pkg_private void
textsw_thaw_caret(textsw)
    register Textsw_folio textsw;
{
    textsw->caret_state &= ~TXTSW_CARET_FROZEN;
}

pkg_private void
textsw_invert_caret(textsw)
    register Textsw_folio textsw;
{
    if (textsw->caret_state & TXTSW_CARET_ON)
	textsw_hide_caret(textsw);
    else
	textsw_show_caret(textsw);
}

pkg_private int
textsw_is_typing_pending(folio, event)
    register Textsw_folio folio;
    Event *event;
{
    Xv_opaque public_handle = folio->first_view->scrollbar;
	/* Probably should be something else, but I know this works */
    Xv_Drawable_info *view_info;
    Display *display;
    XEvent xevent_next, *xevent_cur = event->ie_xevent;
    char c;

    /*
     * !public_handle can happen if there is no scrollbar and !xevent_cur
     * can happen when initially going from ttysw to textsw.  Not worth
     * looking ahead if nothing to flush.
     */
    if (!public_handle || !xevent_cur ||
        (folio->to_insert_next_free == folio->to_insert))
	return 0;
    DRAWABLE_INFO_MACRO(public_handle, view_info);
    display = xv_display(view_info);
    if (!QLength(display))
	return 0;
    /*
     * See if next event is a matching KeyRelease to the last event queued
     * on to_insert.
     */
    XPeekEvent(display, &xevent_next);
    if ((xevent_next.type == KeyRelease) &&
      (xevent_cur->xkey.x == xevent_next.xkey.x) &&
      (xevent_cur->xkey.y == xevent_next.xkey.y) &&
      (xevent_cur->xkey.window == xevent_next.xkey.window) &&
      (XLookupString(&xevent_next, &c, 1, (KeySym *)0, 0) == 1) &&
      (c == (folio->to_insert_next_free-(CHAR *)1))) {
	/* Take the event off the queue and discard it */
	XNextEvent(display, &xevent_next);
	/* Get new top of queue */
	if (!QLength(display))
	    return 0;
	XPeekEvent(display, &xevent_next);
    }
    /* See if next event is typing on main key array */
    if ((xevent_next.type == KeyPress) &&
      (xevent_cur->xkey.x == xevent_next.xkey.x) &&
      (xevent_cur->xkey.y == xevent_next.xkey.y) &&
      (xevent_cur->xkey.window == xevent_next.xkey.window) &&
      (XLookupString(&xevent_next, &c, 1, (KeySym *)0, 0) == 1) &&
      (c >= 32/*" "*/ && c <= 126/*"~"*/))
	return 1;
    return 0;
}
