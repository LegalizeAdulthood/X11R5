#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)txt_scroll.c 20.32 90/05/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Liason routines between textsw and scrollbar
 */

#include <xview_private/primal.h>
#include <xview_private/txt_impl.h>

#define TEXTSW_FOR_SB(_sb) ((Textsw)xv_get(_sb, SCROLLBAR_NOTIFY_CLIENT))

pkg_private void
textsw_scroll(sb)
    Scrollbar       sb;
{
    Es_index        first, last_plus_one;
    register Textsw_view_handle view;
    Es_index        new_start;


    view = VIEW_ABS_TO_REP(TEXTSW_FOR_SB(sb));

    ev_view_range(view->e_view, &first, &last_plus_one);
    new_start = (Es_index) xv_get(sb, SCROLLBAR_VIEW_START);

    if (new_start != first)
	textsw_normalize_view(VIEW_REP_TO_ABS(view),
			      new_start);

}


pkg_private int
textsw_compute_scroll(sb, pos, length, motion, offset, obj_length)
    Scrollbar       sb;
    int             pos;
    int             length;
    Scroll_motion   motion;
    long unsigned  *offset;
    long unsigned  *obj_length;
{
    register Textsw_view_handle view = VIEW_ABS_TO_REP(TEXTSW_FOR_SB(sb));
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);
    Es_index        new_start = TEXTSW_CANNOT_SET;
    int             lines = 0;
    Es_index        first, last_plus_one;

    *obj_length = es_get_length(folio->views->esh);

    switch (motion) {
      case SCROLLBAR_ABSOLUTE:
	if (length == 0) 
	  new_start = (Es_index) pos;
	else
	  new_start = (*obj_length * (long) pos) / length;
	break;
      case SCROLLBAR_POINT_TO_MIN:
      case SCROLLBAR_MIN_TO_POINT:{
	    lines = ev_line_for_y(view->e_view,
				  view->rect.r_top + pos);
	    if (lines == 0)
		lines++;	/* Always make some progress */
	    if (motion == SCROLLBAR_MIN_TO_POINT)
		lines = -lines;
	}
	break;
      case SCROLLBAR_PAGE_FORWARD:
	lines = view->e_view->line_table.last_plus_one - 2;
	break;
      case SCROLLBAR_PAGE_BACKWARD:
	lines = -view->e_view->line_table.last_plus_one + 2;
	break;
      case SCROLLBAR_LINE_FORWARD:
	lines = 1;
	break;
      case SCROLLBAR_LINE_BACKWARD:
	lines = -1;
	break;
      case SCROLLBAR_TO_START:
	new_start = 0;
	break;
      case SCROLLBAR_TO_END:
	new_start = *obj_length;
	break;
      default:
	break;
    }

    if (new_start != TEXTSW_CANNOT_SET) {
	/*
	 * Motion must be SCROLLBAR_ABSOLUTE, SCROLLBAR_TO_START,
	 * SCROLLBAR_TO_END
	 */
	int             upper_context = (int) ev_get(view->e_view,
						     EV_CHAIN_UPPER_CONTEXT);
	textsw_normalize_internal(view, new_start, new_start,
			  upper_context, 0, TXTSW_NI_DONT_UPDATE_SCROLLBAR);
    } else {
	(void) textsw_take_down_caret(folio);	/* Prevent caret turds */
	(void) ev_scroll_lines(view->e_view, lines, FALSE);
    }

    /* if this is TO_END, change so there is half a screen visible if possible */
    if( motion == SCROLLBAR_TO_END ) {
        lines =  -view->e_view->line_table.last_plus_one / 2 + 1;
	(void) textsw_take_down_caret(folio);	/* Prevent caret turds */
	(void) ev_scroll_lines(view->e_view, lines, FALSE);
    }

    ev_view_range(view->e_view, &first, &last_plus_one);

    xv_set(sb, SCROLLBAR_VIEW_LENGTH, last_plus_one - first,
	   0);
    *offset = first;
    return (XV_OK);
}

/*
 * following routines are called from the interval timer to determine if the
 * caller wanted to update the scrollbars. This is done to minimize the
 * repaint and recalculation of the scrollbar info. In the asynchronous mode,
 * the action is requested as many times as the code wants to do it. When the
 * interval timer triggers and decides to repaint the caret it also calls
 * this routine to determine if it should also update the scrollbars at the
 * same time. There are also times that this is still done using the older
 * synchronous method, but this additional logic greatly reduces the overhead
 * of the scrollbar repaint.
 */
#define	BADVIEW			((Textsw_view_handle)-1)

static short    txt_update_bars = FALSE;
static Textsw_view_handle txt_only_view = BADVIEW;

pkg_private void
textsw_real_update_scrollbars(folio)
    register Textsw_folio folio;
{
    register Textsw_view_handle view;
    register Scrollbar sb;
    Es_index        first, last_plus_one;
    int             vlen, olen;

    if (txt_update_bars == FALSE)
	return;

    FORALL_TEXT_VIEWS(folio, view) {
	if (txt_only_view != BADVIEW &&
	    txt_only_view != 0 &&
	    (view != txt_only_view))
	    continue;

	if ((sb = SCROLLBAR_FOR_VIEW(view)) == 0) {
	    continue;
	}
	ev_view_range(view->e_view, &first, &last_plus_one);
	vlen = last_plus_one - first;
	olen = es_get_length(folio->views->esh);

	/*
	 * printf("real scrollbar update:: first %5d  viewl %5d  objl %5d\n",
	 * first, vlen, olen );
	 */
	xv_set(sb,
	       SCROLLBAR_VIEW_START, first,
	       SCROLLBAR_VIEW_LENGTH, vlen,
	       SCROLLBAR_OBJECT_LENGTH, olen,
	       0);

    }

    txt_update_bars = FALSE;
    txt_only_view = BADVIEW;
}


/*
 * this old routine now just sets flags that indicate that update is needed
 */
pkg_private void
textsw_update_scrollbars(folio, only_view)
    register Textsw_folio folio;
    register Textsw_view_handle only_view;
{
    /* determine the correct global setting for view update */
    if (txt_update_bars) {
	if (only_view && txt_only_view == BADVIEW)
	    txt_only_view = only_view;
	else if (only_view != NULL)
	    txt_only_view = only_view;
	else
	    txt_only_view = 0;
    } else {
	txt_only_view = only_view;
	txt_update_bars = TRUE;
    }

    /* if there is no current timer update do actual update now */
    if (folio != NULL) {	/* (can be passed NULL pointer) */
	if (!(folio->caret_state & TXTSW_CARET_TIMER_ON))
	    textsw_real_update_scrollbars(folio);
    }
}

pkg_private     Scrollbar
textsw_get_scrollbar(view)
    Textsw_view_handle view;
{
    if (!(view->state & TXTSW_SCROLLBAR_DISABLED))
	view->scrollbar = xv_get(FOLIO_REP_TO_ABS(FOLIO_FOR_VIEW(view)),
			 OPENWIN_VERTICAL_SCROLLBAR, VIEW_REP_TO_ABS(view));

    return (view->scrollbar);
}
