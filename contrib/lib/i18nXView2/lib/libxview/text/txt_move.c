#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)txt_move.c 50.7 90/10/25";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Text subwindow move & duplicate
 */
#ifdef OW_I18N
#include <xview/xv_i18n.h>
#endif
#include <xview_private/primal.h>
#include <xview_private/ev_impl.h>
#include <xview_private/txt_impl.h>
#include <xview_private/draw_impl.h>
#include <pixrect/pr_util.h>
#include <pixrect/memvar.h>
#include <pixrect/pixfont.h>
#include <xview/cursor.h>
#include <xview/font.h>
#include <xview/pkg.h>
#include <xview/attrol.h>
#include <xview/screen.h>
#include <xview/seln.h>
#include <xview/fullscreen.h>
#include <xview/svrimage.h>
#include <xview/server.h>
#include <xview/win_struct.h>
#include <xview/pixwin.h>


Pkg_private Es_index ev_resolve_xy();
extern Es_index textsw_do_balance_beam();
extern int      ev_get_selection();
extern Es_handle textsw_esh_for_span();
#ifdef OW_I18N
extern int      server_image_pf_text_wc();
#else
extern int      xv_pf_text();
#endif
pkg_private int textsw_end_quick_move();

#ifndef SVR4
static short    drag_move_arrow_data[] = {
#else SVR4
static unsigned short    drag_move_arrow_data[] = {
#endif  SVR4
#include <images/text_move_cursor.pr>
};

mpr_static(drag_move_arrow_pr, 16, 16, 1, drag_move_arrow_data);

struct  textsw_context {
    int    size;
    char   *sel_buffer;
};  

textsw_save_selection(folio)
    Textsw_folio    folio;
{
    static          repeat_call;

    if (!repeat_call) {
	(void) ev_get_selection(folio->views,
			     &folio->move_first, &folio->move_last_plus_one,
				EV_SEL_PRIMARY);
	repeat_call = TRUE;
    }
}


pkg_private int
textsw_do_move(view, selection_is_local)
    Textsw_view_handle view;
    int             selection_is_local;
{
    register Textsw_folio folio = FOLIO_FOR_VIEW(view);
    Es_index        delta, first, last_plus_one, ro_bdry, pos;
    register Es_handle secondary = ES_NULL;
    register Ev_chain chain = folio->views;
    register int    is_pending_delete;
    CHAR           *string;
    int             lower_context =
    (int) ev_get(view->e_view, EV_CHAIN_LOWER_CONTEXT);

    /*
     * First, pre-process the primary selection.
     */
    ev_set(view->e_view, EV_CHAIN_DELAY_UPDATE, TRUE, 0);
    ro_bdry = textsw_read_only_boundary_is_at(folio);

    is_pending_delete = (EV_SEL_PENDING_DELETE & ev_get_selection(
			    chain, &first, &last_plus_one, EV_SEL_PRIMARY));

    if ((first < last_plus_one) && is_pending_delete) {
	/*
	 * A non-empty pending-delete primary selection exists. It must be
	 * the contents of the trashbin when we are done.
	 */
	secondary = folio->trash;	/* Recycle old trash pieces */
	folio->trash =
	    textsw_esh_for_span(view, first, last_plus_one, ES_NULL);
	pos = last_plus_one;
    } else {
	secondary = ES_NULL;
	pos = EV_GET_INSERT(folio->views);
    }

    if (pos < ro_bdry) {
	textsw_clear_secondary_selection(folio, EV_SEL_SECONDARY);
	return (TEXTSW_PE_READ_ONLY);
    }
    /*
     * Completely process local secondary selection.
     */
    if (selection_is_local) {
	ev_get_selection(chain, &first, &last_plus_one, EV_SEL_SECONDARY);
	if (last_plus_one <= ro_bdry) {
	    textsw_clear_secondary_selection(folio, EV_SEL_SECONDARY);
	    return (TEXTSW_PE_READ_ONLY);
	}
	secondary = textsw_esh_for_span(
				     view, first, last_plus_one, secondary);
	textsw_delete_span(view, (first < ro_bdry) ? ro_bdry : first,
			   last_plus_one, TXTSW_DS_SHELVE);
	if (first != ES_INFINITY)
	    textsw_set_selection(VIEW_REP_TO_ABS(view),
				 ES_INFINITY, ES_INFINITY, EV_SEL_SECONDARY);
    } else {
	Seln_holder     holder;
	Seln_request   *result;
	char           *data;
	int             is_read_only;

	/* BUG: This is a performance problem we should look at later */
	holder = seln_inquire(SELN_SECONDARY);
	result = seln_ask(&holder, SELN_REQ_IS_READONLY, 0, 0);
	data = result->data;
	/* Test if is SELN_IS_READONLY */
	data += sizeof(Seln_attribute);
	is_read_only = *(int *) data;

	if (is_read_only) {
	    return (TEXTSW_PE_READ_ONLY);
	} else {
#ifdef OW_I18N
	    result = seln_ask(&holder, SELN_REQ_CONTENTS_WCS, 0, 0);
#else	
	    result = seln_ask(&holder, SELN_REQ_CONTENTS_ASCII, 0, 0);
#endif	    
	    data = result->data;
	    /* Test if is SELN_REQ_CONTENTS_ASCII */
	    data += sizeof(Seln_attribute);
	    string = MALLOC(STRLEN(data) + 1);
	    STRCPY(string, data);
	    result = seln_ask(&holder, SELN_REQ_COMMIT_PENDING_DELETE, 0, 0);
	}

    }
    /*
     * Third, post-process the primary selection.
     */
    is_pending_delete = (EV_SEL_PENDING_DELETE & ev_get_selection(
			    chain, &first, &last_plus_one, EV_SEL_PRIMARY));
    if (first < last_plus_one) {
	if (is_pending_delete && (ro_bdry < last_plus_one)) {
	    ev_delete_span(chain,
			   (first < ro_bdry) ? ro_bdry : first,
			   last_plus_one, &delta);
	}
	if (first != ES_INFINITY)
	    textsw_set_selection(VIEW_REP_TO_ABS(view),
				 ES_INFINITY, ES_INFINITY, EV_SEL_PRIMARY);
    }
    /*
     * Fourth, insert the text being gotten.
     */
    ev_set(view->e_view, EV_CHAIN_DELAY_UPDATE, FALSE, 0);
    EV_SET_INSERT(chain, textsw_get_saved_insert(folio), first);
    if (lower_context != EV_NO_CONTEXT) {
	ev_check_insert_visibility(chain);
    }
    if (selection_is_local)
	(void) textsw_insert_pieces(view, first, secondary);
    else {
	(void) textsw_do_input(view, string, STRLEN(string), TXTSW_UPDATE_SCROLLBAR);
	free(string);
    }

    TEXTSW_DO_INSERT_MAKES_VISIBLE(view);
    folio->track_state &= ~TXTSW_TRACK_QUICK_MOVE;
}

pkg_private int
textsw_end_quick_move(view)
    Textsw_view_handle view;
{
    extern void     textsw_init_selection_object();
    extern void     textsw_clear_secondary_selection();
    int             result = 0;
    register Textsw_folio folio = FOLIO_FOR_VIEW(view);
    int             selection_is_local;


    selection_is_local = textsw_inform_seln_svc(folio, TXTSW_FUNC_DELETE, FALSE);
    if ((folio->func_state & TXTSW_FUNC_DELETE) == 0)
	return (0);
    /*
     * if ((folio->func_state & TXTSW_FUNC_EXECUTE) == 0) goto Done;
     */

    if (TXTSW_IS_READ_ONLY(folio)) {
	result = TEXTSW_PE_READ_ONLY;
	textsw_clear_secondary_selection(folio, EV_SEL_SECONDARY);
	goto Done;
    }
    textsw_checkpoint_undo(VIEW_REP_TO_ABS(view),
			   (caddr_t) TEXTSW_INFINITY - 1);
    ASSUME(allock());
    result = textsw_do_move(view, selection_is_local);
    ASSUME(allock());
    textsw_checkpoint_undo(VIEW_REP_TO_ABS(view),
			   (caddr_t) TEXTSW_INFINITY - 1);

Done:
    textsw_end_function(view, TXTSW_FUNC_DELETE);
    textsw_update_scrollbars(folio, TEXTSW_VIEW_NULL);
    folio->track_state &= ~TXTSW_TRACK_QUICK_MOVE;
    return (result);


}

pkg_private int
textsw_track_move(view, ie)
    Textsw_view_handle view;
    register Event *ie;
{

    if (win_inputnegevent(ie))
	textsw_finish_move(view, ie);
}

pkg_private int
textsw_finish_move(view, ie)
    Textsw_view_handle view;
    register Event *ie;
{
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);

    xv_do_move(view, ie);
    folio->track_state &= ~TXTSW_TRACK_MOVE;
    textsw_reset_cursor(view);
}

textsw_clear_move(view)
    Textsw_view_handle view;
{
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);

    folio->track_state &= ~TXTSW_TRACK_MOVE;
    textsw_reset_cursor(view);
}

pkg_private int
textsw_track_duplicate(view, ie)
    Textsw_view_handle view;
    register Event *ie;
{

    if (win_inputnegevent(ie))
	textsw_finish_duplicate(view, ie);
}

pkg_private int
textsw_finish_duplicate(view, ie)
    Textsw_view_handle view;
    register Event *ie;
{
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);

    textsw_do_duplicate(view, ie);
    folio->track_state &= ~TXTSW_TRACK_DUPLICATE;
    textsw_reset_cursor(view);
}

textsw_clear_duplicate(view)
    Textsw_view_handle view;
{
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);

    folio->track_state &= ~TXTSW_TRACK_DUPLICATE;
    textsw_reset_cursor(view);
}

textsw_reset_cursor(view)
    Textsw_view_handle view;
{
    Xv_Cursor       main_cursor;
    Xv_object       screen, server;

    screen = xv_get(VIEW_REP_TO_ABS(view), XV_SCREEN);
    server = xv_get(screen, SCREEN_SERVER);
    main_cursor = (Xv_Cursor) xv_get(server,
				     XV_KEY_DATA, CURSOR_BASIC_PTR);
    xv_set(VIEW_REP_TO_ABS(view), WIN_CURSOR, main_cursor, 0);
}

/*
 * 1) if two spaces are left after deleting, they should be collapsed. 2) if
 * the selection is a word, when it is moved or duplicated, if there is a
 * space on one end there should be a space on both ends.
 */
/*
 * Menu proc() is called... sets SELN_FN_MOVE to true saves the location of
 * the selection in the folio. As long as track state is TXTSW_TRACK_MOVE,
 * all input events go to track move(). On button up, xv_do_move() is called...
 * resets track state.
 */
pkg_private int
xv_do_move(view, ie)
    Textsw_view_handle view;
    register Event *ie;
{
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);
    Textsw          textsw = VIEW_REP_TO_ABS(view);
    Es_index        first, last_plus_one;
    Es_index        pos, original_pos;
    CHAR             sel[1024], buf[2];
    int             sel_len;

    (void) ev_get_selection(folio->views, &first, &last_plus_one,
			    EV_SEL_PRIMARY);

    textsw_get_selection_as_string(folio, EV_SEL_PRIMARY, sel, 1024);
    sel_len = STRLEN(sel);

    pos = ev_resolve_xy(view->e_view, ie->ie_locx, ie->ie_locy);
    pos = textsw_do_balance_beam(view, ie->ie_locx, ie->ie_locy, pos, pos + 1);

    /* don't do anything if destination is within selection */
    if (pos >= first && pos <= last_plus_one)
	return;

    original_pos = pos;

    /* if spaces on either side, collapse them */
#ifdef OW_I18N    
    xv_get(textsw, TEXTSW_CONTENTS_WCS, first - 1, buf, 2);
#else
    xv_get(textsw, TEXTSW_CONTENTS, first - 1, buf, 2);
#endif    
    if (buf[0] == ' ') {
#ifdef OW_I18N        
	xv_get(textsw, TEXTSW_CONTENTS_WCS, last_plus_one, buf, 1);
#else
	xv_get(textsw, TEXTSW_CONTENTS, last_plus_one, buf, 1);
#endif	
	if (buf[0] == ' ') {
	    last_plus_one++;
	}
    }
    /* delete the source */
    textsw_delete(textsw, first, last_plus_one);

    /* correct for deletion */
    if (original_pos > first)
	pos -= last_plus_one - first;

    /* if punctuation to the right and space to the left, delete space */
#ifdef OW_I18N    
    xv_get(textsw, TEXTSW_CONTENTS_WCS, first - 1, buf, 2);
#else
    xv_get(textsw, TEXTSW_CONTENTS, first - 1, buf, 2);
#endif
    
    if (buf[1] == '.' || buf[1] == ',' || buf[1] == ';' || buf[1] == ':') {
	if (buf[0] == ' ') {
	    textsw_delete(textsw, first - 1, first);
	    if (original_pos > first)
		pos--;
	}
    }
    xv_set(textsw, TEXTSW_INSERTION_POINT, pos, 0);


    /* add leading/trailing space to selection if needed for new location */
#ifdef OW_I18N    
    xv_get(textsw, TEXTSW_CONTENTS_WCS, pos - 1, buf, 2);
#else
    xv_get(textsw, TEXTSW_CONTENTS, first - 1, buf, 2);
#endif
    
    if (buf[1] == ' ') {
	if (buf[0] != ' ') {
	    /* add leading space */
	    BCOPY(sel, sel + 1, sel_len);
	    sel[0] = ' ';
	    sel_len++;
	    sel[sel_len] = '\0';
	    textsw_insert(textsw, sel, sel_len);
	    /* reset selection to original span */
	    textsw_set_selection(textsw, pos + 1, pos + sel_len, EV_SEL_PRIMARY);
	    return;
	}
    } else {
	if (buf[0] == ' ') {
	    /* add trailing space */
	    sel[sel_len] = ' ';
	    sel_len++;
	    sel[sel_len] = '\0';
	    textsw_insert(textsw, sel, sel_len);
	    /* reset selection to original span */
	    textsw_set_selection(textsw, pos, pos + sel_len - 1, EV_SEL_PRIMARY);
	    /* correct insertion point */
	    xv_set(textsw, TEXTSW_INSERTION_POINT, pos + sel_len - 1, 0);
	    return;
	}
    }
    if (buf[1] == '.' ||
	buf[1] == ',' ||
	buf[1] == ';' ||
	buf[1] == ':') {
	/* before punctuation mark -- add leading space */
	BCOPY(sel, sel + 1, STRLEN(sel));
	sel[0] = ' ';
	sel_len++;
	sel[sel_len] = '\0';
	textsw_insert(textsw, sel, sel_len);
	/* reset selection to original span */
	textsw_set_selection(textsw, pos + 1, pos + sel_len, EV_SEL_PRIMARY);
	return;
    } else {
	/* don't add any spaces */
	textsw_insert(textsw, sel, sel_len);
	/* reset selection to original span */
	textsw_set_selection(textsw, pos, pos + sel_len, EV_SEL_PRIMARY);
	return;
    }
}

pkg_private int
textsw_do_duplicate(view, ie)
    Textsw_view_handle view;
    register Event *ie;
{
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);
    Textsw          textsw = VIEW_REP_TO_ABS(view);
    Es_index        position, pos;
    CHAR             buf[1024];
    int             len;

    pos = ev_resolve_xy(view->e_view, ie->ie_locx, ie->ie_locy);
    position = textsw_do_balance_beam(view, ie->ie_locx, ie->ie_locy, pos, pos + 1);
    xv_set(textsw, TEXTSW_INSERTION_POINT, position, 0);
    
#ifdef OW_I18N    
    xv_get(textsw, TEXTSW_CONTENTS_WCS, position, buf, 1);
#else
    xv_get(textsw, TEXTSW_CONTENTS, position, buf, 1);
#endif

    if (buf[0] == ' ') {
#ifdef OW_I18N    
	xv_get(textsw, TEXTSW_CONTENTS_WCS, position - 1, buf, 1);
#else
	xv_get(textsw, TEXTSW_CONTENTS, position - 1, buf, 1);
#endif    
	if (buf[0] != ' ') {
	    /* space after -- add leading space */
	    buf[0] = ' ';
	    textsw_get_selection_as_string(folio, EV_SEL_PRIMARY, buf + 1, 1024);
	    textsw_insert(textsw, buf, STRLEN(buf));
	    /* reset selection to original span */
	    textsw_set_selection(textsw, position + 1,
				 position + STRLEN(buf), EV_SEL_PRIMARY);
	    return;
	}
    } else {
#ifdef OW_I18N    
	xv_get(textsw, TEXTSW_CONTENTS_WCS, position - 1, buf, 1);
#else
	xv_get(textsw, TEXTSW_CONTENTS, position - 1, buf, 1);
#endif    
	if (buf[0] == ' ') {
	    /* space before -- add trailing space */
	    textsw_get_selection_as_string(folio, EV_SEL_PRIMARY, buf, 1024);
	    len = STRLEN(buf);
	    buf[len] = ' ';
	    buf[len + 1] = '\0';
	    textsw_insert(textsw, buf, STRLEN(buf));
	    textsw_set_selection(textsw, position,
				 position + STRLEN(buf) - 1, EV_SEL_PRIMARY);
	    xv_set(textsw, TEXTSW_INSERTION_POINT,
		   position + STRLEN(buf) - 1, 0);
	    return;
	}
    }

#ifdef OW_I18N    
    xv_get(textsw, TEXTSW_CONTENTS_WCS, position, buf, 1);
#else
    xv_get(textsw, TEXTSW_CONTENTS, position, buf, 1);
#endif
    if (buf[0] == '.' ||
	buf[0] == ',' ||
	buf[0] == ';' ||
	buf[0] == ':') {
	/* before punctuation mark -- add leading space */
	buf[0] = ' ';
	textsw_get_selection_as_string(folio, EV_SEL_PRIMARY, buf + 1, 1024);
	textsw_insert(textsw, buf, STRLEN(buf));
	/* reset selection to original span */
	textsw_set_selection(textsw, position + 1,
			     position + STRLEN(buf), EV_SEL_PRIMARY);
	return;
    } else {
	/* don't add any spaces */
	textsw_get_selection_as_string(folio, EV_SEL_PRIMARY, buf, 1024);
	textsw_insert(textsw, buf, STRLEN(buf));
	/* reset selection to original span */
	textsw_set_selection(textsw, position,
			     position + STRLEN(buf), EV_SEL_PRIMARY);
	return;
    }
}

pkg_private int
textsw_clean_up_move(view, first, last_plus_one)
    Textsw_view_handle view;
    Es_index        first, last_plus_one;
{
    Textsw          textsw = VIEW_REP_TO_ABS(view);
    CHAR             first_buf[1], last_buf[1];
    int             shift_left;

    /* if spaces on either side, collapse them */
#ifdef OW_I18N    
    xv_get(textsw, TEXTSW_CONTENTS_WCS, first - 1, first_buf, 1);
#else
    xv_get(textsw, TEXTSW_CONTENTS, first - 1, first_buf, 1);
#endif
    
    if (first_buf[0] == ' ') {
#ifdef OW_I18N    
	xv_get(textsw, TEXTSW_CONTENTS_WCS, last_plus_one, last_buf, 1);
#else
	xv_get(textsw, TEXTSW_CONTENTS, last_plus_one, last_buf, 1);
#endif
	if (last_buf[0] == ' ')
	    shift_left = TRUE;
    }
#ifdef OW_I18N    
    xv_get(textsw, TEXTSW_CONTENTS_WCS, last_plus_one, last_buf, 1);
#else
    xv_get(textsw, TEXTSW_CONTENTS, last_plus_one, last_buf, 1);
#endif    
    /* if punctuation to the right and space to the left, delete space */
    if (last_buf[0] == '.' || last_buf[0] == ',' || last_buf[0] == ';'
	|| last_buf[0] == ':') {
	if (first_buf[0] == ' ')
	    shift_left = TRUE;
    }
    if (shift_left)
	textsw_delete(textsw, first - 1, first);

    return (shift_left);
}

static          Server_image
build_drag_move_cursor(height, width)
    int            *height, *width;
{
    Server_image    cursor_pr;

    *height += drag_move_arrow_pr.pr_height + 2;
    *width += drag_move_arrow_pr.pr_width + 2;
    cursor_pr = xv_create(0, SERVER_IMAGE,
			  XV_WIDTH, *width,
			  XV_HEIGHT, *height,
			  SERVER_IMAGE_DEPTH, 1, 0);
    pw_rop(cursor_pr, 0, 0, *width, *height, PIX_CLR, NULL, 0, 0);
    /* construct the cursor pr */
    /* the arrow part */
    pw_rop(cursor_pr, 0, 0, 16, 16, PIX_SRC, &drag_move_arrow_pr, 0, 0);
    /* draw the vectors that enclose the text rectangle */
    pw_vector(cursor_pr, 16, 5, (*width) - 3, 5, PIX_SET, 0);	/* Top */
    pw_vector(cursor_pr, (*width) - 3, 5, (*width) - 3, (*height) - 3, PIX_SET, 0);	/* Right */
    pw_vector(cursor_pr, (*width) - 3, (*height) - 3, 5, (*height) - 3, PIX_SET, 0);	/* Bottom */
    pw_vector(cursor_pr, 5, (*height) - 3, 5, 16, PIX_SET, 0);	/* Left */
    return (cursor_pr);
}

static void
fill_in_drag_cursor(font, height, width, str, cursor_pr, is_copy, last_is_copy)
    Pixfont        *font;
    CHAR            *str;
    Server_image    cursor_pr;
    int            *is_copy;
    short          *last_is_copy;
{
    struct pr_prpos pr_and_position;

    pr_and_position.pos.x = 10;
    pr_and_position.pos.y = height - 7;
    pr_and_position.pr = (Pixrect *) cursor_pr;
#ifdef OW_I18N
    server_image_pf_text_wc(pr_and_position, PIX_SRC | PIX_DST, font, str);
#else    
    xv_pf_text(pr_and_position, PIX_SRC | PIX_DST, font, str);
#endif   

    if (*is_copy) {
	pw_vector(cursor_pr, width - 1, 7, width - 1, height - 1, PIX_SET, 0);
	pw_vector(cursor_pr, width - 1, height - 1, 7, height - 1, PIX_SET, 0);
    } else {
	pw_vector(cursor_pr, width - 1, 7, width - 1, height - 1, PIX_CLR, 0);
	pw_vector(cursor_pr, width - 1, height - 1, 7, height - 1, PIX_CLR, 0);
    }

}

Pkg_private void
textsw_do_drag_copy_move(view, ie, is_copy)
    Textsw_view_handle view;
    Event          *ie;
    int             is_copy;
{
#define MAX_CHARS_SHOWN	 10	/* most chars shown in the drag move cursor */
#define SELECTION_BUF_SIZE	MAX_CHARS_SHOWN + 2
    Xv_opaque       public_view = VIEW_REP_TO_ABS(view);
    register Textsw_folio folio = FOLIO_FOR_VIEW(view);
    Textsw          textsw = FOLIO_REP_TO_ABS(folio);
    Xv_object       screen = xv_get(textsw, XV_SCREEN);
    Xv_object       server = xv_get(screen, SCREEN_SERVER);
    Xv_Drawable_info *view_info, *folio_info;
    int             final_x, final_y;
    XID             dest_win_xid;
    static Textsw_view_handle get_button_up_view();
    Textsw_view_handle button_up_view;
    Xv_Cursor       quick_move_cursor;
    CHAR             buf[SELECTION_BUF_SIZE];
    static Server_image cursor_pr;
    Pixfont        *font;
    short           l, i;
    static short    last_is_copy;
    static int      height, width;
    Fullscreen      my_fullscreen;
    Event           my_ie;
    Font_string_dims str_dims;
    static Font_string_dims old_str_dims;
    int             new_cursor = FALSE;
    Inputmask       im;
    Xv_window       dest_win;
    int		    do_remote_case = FALSE;

    /* first find out under which window the mouse is */
    DRAWABLE_INFO_MACRO(public_view, view_info);
    DRAWABLE_INFO_MACRO(textsw, folio_info);

    l = textsw_get_selection_as_string(folio, EV_SEL_PRIMARY, buf,
				       MAX_CHARS_SHOWN + 1);
    /* pad the buffer w/ space */
    buf[MAX_CHARS_SHOWN] = NULL;
    if (l <= MAX_CHARS_SHOWN) {
	/*
	 * weird behavior of textsw_get_selection_as_string(): If selection
	 * length is longer than the last argument specified (the maximum
	 * length the caller is intersted), then l = last argument but if
	 * selection length is shorter, l = length of selection(not counting
	 * the null terminator) + 1.
	 */
	l--;
	/* we can show the entire selection */
	for (i = l; i < MAX_CHARS_SHOWN; i++) {
	    buf[i] = ' ';
	}
    } else {
	/* only show 7 chars and show three .'s */
	buf[MAX_CHARS_SHOWN - 3] = buf[MAX_CHARS_SHOWN - 2] = buf[MAX_CHARS_SHOWN - 1] = '.';
    }

    font = (Pixfont *) ei_get(folio->views->eih, EI_FONT);
#ifdef OW_I18N
    xv_get(font, FONT_STRING_DIMS_WC, buf, &str_dims);   
#else    
    xv_get(font, FONT_STRING_DIMS, buf, &str_dims);
#endif    
    quick_move_cursor = (Xv_Cursor) xv_get(server, XV_KEY_DATA, DRAG_MOVE_CURSOR_KEY);

    /* Prepare the quick move cursor if it has not been created */
    new_cursor = ((!quick_move_cursor) ||
		  ((old_str_dims.height != str_dims.height) || (old_str_dims.width != str_dims.width)));

    if (new_cursor) {
	if (quick_move_cursor)
	    xv_destroy(cursor_pr);

	quick_move_cursor = xv_create(0, CURSOR,
				      CURSOR_XHOT, 0,
				      CURSOR_YHOT, 0,
				      CURSOR_OP, PIX_SRC,
				      0);
	(void) xv_set(server,
		   XV_KEY_DATA, DRAG_MOVE_CURSOR_KEY, quick_move_cursor, 0);
    }
    if (new_cursor) {
	old_str_dims = str_dims;
	height = str_dims.height;
	width = str_dims.width;
	cursor_pr = build_drag_move_cursor(&height, &width);
    }
    /* now print the string */
    (void) fill_in_drag_cursor(font, height, width, buf, cursor_pr, &is_copy, &last_is_copy);

    if ((xv_depth(view_info) < 2) || xv_get(xv_cms(view_info), CMS_DEFAULT_CMS)) {
        xv_set(quick_move_cursor, CURSOR_IMAGE, cursor_pr, 0);
    } else {
	Xv_singlecolor	fg, bg;
	XColor		xcolor;

	/* Note - Need to provide a xv_get() interface to query colors. */
	xcolor.flags = DoRed | DoGreen | DoBlue;
	xcolor.pixel = xv_fg(view_info);
	XQueryColor(xv_display(view_info), 
		(Colormap)xv_get(xv_cms(view_info), CMS_CMAP_ID), &xcolor);
	fg.red = xcolor.red >> 8;
	fg.green = xcolor.green >> 8;
	fg.blue = xcolor.blue >> 8;

	xcolor.pixel = xv_bg(view_info);
	XQueryColor(xv_display(view_info), 
		(Colormap)xv_get(xv_cms(view_info), CMS_CMAP_ID), &xcolor);
	bg.red = xcolor.red >> 8;
	bg.green = xcolor.green >> 8;
	bg.blue = xcolor.blue >> 8;

	xv_set(quick_move_cursor, 
	       CURSOR_IMAGE, cursor_pr,
	       CURSOR_FOREGROUND_COLOR, &fg,
	       CURSOR_BACKGROUND_COLOR, &bg,
	       0);
    }

    my_fullscreen = xv_create(0, FULLSCREEN,
			      FULLSCREEN_INPUT_WINDOW, public_view,
			      FULLSCREEN_SYNC, True,
			      WIN_CURSOR, quick_move_cursor,
			      WIN_CONSUME_EVENTS, 
				  WIN_MOUSE_BUTTONS,
			          0,
			      0);

    input_imnull(&im);
    win_setinputcodebit(&im, MS_LEFT);
    win_setinputcodebit(&im, LOC_DRAG);
    im.im_flags = IM_NEGEVENT;

    XAllowEvents(xv_display(view_info), SyncPointer, CurrentTime);
    do {
	if (xv_input_readevent(public_view, &my_ie, TRUE, TRUE, &im) == -1) {
	    abort();
	}
    } while (!(win_inputnegevent(&my_ie) && event_id(&my_ie) == MS_LEFT));

    fullscreen_destroy(my_fullscreen);

    win_translate_xy_internal(xv_display(view_info), xv_xid(view_info),
			      xv_xid(folio_info), event_x(&my_ie), event_y(&my_ie), &final_x, &final_y);
    dest_win_xid = win_pointer_under(textsw, final_x, final_y);
    dest_win = (Xv_Window) win_data(xv_display(view_info), dest_win_xid);

    if (dest_win == screen)
	/*
	 * button-up is in root window, so don't do anything.
	 */
	return;

    
    if ((dest_win == NULL) ||
       !xv_get(dest_win, XV_IS_SUBTYPE_OF, TEXTSW_VIEW)) 
        do_remote_case = TRUE;
    else
	do_remote_case = (folio != FOLIO_FOR_VIEW(VIEW_ABS_TO_REP(dest_win)));
    
    if (do_remote_case) {
	/*
	 * button-up is in a window that is not the child of textsw(not in a
	 * split view), in which case we will send a client msg to that
	 * process.
	 */
	int             data[5];

	data[0] = XV_POINTER_WINDOW;
	data[1] = event_x(&my_ie);
	data[2] = event_y(&my_ie);
	data[3] = xv_xid(view_info);
	data[4] = 0;

	if (is_copy) {
	    xv_send_message(public_view, dest_win_xid, "XV_DO_DRAG_COPY", 32, data, 16);
	} else {
	    xv_send_message(public_view, dest_win_xid, "XV_DO_DRAG_MOVE", 32, data, 16);
	}
    } else if (button_up_view = get_button_up_view(folio, dest_win)) {
	Es_index        pos;
	Es_index        first, last_plus_one;

	/* button-up happens in one of the views of this textsw */
	if (button_up_view != view) {
	    /* need to translate coordinates if button-up is in a diff view */
	    win_translate_xy_internal(xv_display(view_info), xv_xid(view_info),
				      dest_win_xid, event_x(&my_ie), event_y(&my_ie), &final_x, &final_y);
	} else {
	    final_x = event_x(&my_ie);
	    final_y = event_y(&my_ie);
	}
	pos = ev_resolve_xy(button_up_view->e_view, final_x, final_y);
	(void) ev_get_selection(folio->views, &first, &last_plus_one, EV_SEL_PRIMARY);
	/*
	 * Go ahead w/ the move if the button-up didn't happen in the
	 * selection
	 */
	if (!(pos >= first && pos < last_plus_one)) {
	    Es_index        insert_pt, ro_bound;
	    Es_handle       selection;

	    ro_bound = textsw_read_only_boundary_is_at(folio);
	    pos = (pos >= ro_bound) ? pos : EV_GET_INSERT(folio->views);
 
	    if (pos >= ro_bound) {
		selection = textsw_esh_for_span(view, first, last_plus_one, ES_NULL);

		ev_set(view->e_view, EV_CHAIN_DELAY_UPDATE, TRUE, 0);
		if (last_plus_one > ro_bound && !is_copy) {
		    Ev_mark_object  insert_mark;

		    insert_mark = textsw_add_mark_internal(folio, pos, TEXTSW_MARK_MOVE_AT_INSERT);

		    textsw_delete_span(view, (first < ro_bound) ? ro_bound : first,
				       last_plus_one, TXTSW_DS_SHELVE);

		    if (first != ES_INFINITY)
			textsw_set_selection(VIEW_REP_TO_ABS(view),
				ES_INFINITY, ES_INFINITY, EV_SEL_SECONDARY);

		    pos = textsw_find_mark_internal(folio, insert_mark);
		    textsw_remove_mark_internal(folio, insert_mark);
		}
		EV_SET_INSERT(folio->views, pos, insert_pt);
		if (AN_ERROR(insert_pt == ES_CANNOT_SET)) {
		    return;
		}
		textsw_insert_pieces(button_up_view, insert_pt, selection);

		ev_set(view->e_view, EV_CHAIN_DELAY_UPDATE, FALSE, 0);
		ev_update_chain_display(folio->views);
		textsw_update_scrollbars(folio, TEXTSW_VIEW_NULL);

	    }
	}
    }
}


/*
 * When a textsw gets a ACTION_DRAG_MOVE event, this routines gets called to
 * get the primary selection from another process and do move/copy
 * ACTION_DRAG_MOVE is a result of XSendEvent called by the subwindow that
 * originally sees the button-down that starts the drag move
 */
Pkg_private
textsw_do_remote_drag_copy_move(view, ie, is_copy)
    register Textsw_view_handle view;
    Event          *ie;
    short           is_copy;
{
    register Textsw_folio folio = FOLIO_FOR_VIEW(view);
    Seln_holder     holder;
    Seln_request   *result;
    char            *data;
    CHAR	    *string;
    int             is_read_only;
    Es_index        ro_bdry, pos, temp;

    Seln_result     query_result, buf_reader();
    struct textsw_context  context;
#ifdef OW_I18N
    CHAR		*str_ptr;
#endif    


    /*
     * First, process insertion point .
     */
    ev_set(view->e_view, EV_CHAIN_DELAY_UPDATE, TRUE, 0);
    ro_bdry = textsw_read_only_boundary_is_at(folio);
    pos = ev_resolve_xy(view->e_view,
			event_x(ie), event_y(ie));

    if (pos < ro_bdry) {
	Es_index	insert;

	insert = EV_GET_INSERT(folio->views);
	if (insert >= ro_bdry)
	    pos = insert;
	else
	    return;
    }
    /* Second, process the primary selection */
    /* BUG: This is a performance problem we should look at later */
    holder = seln_inquire(SELN_PRIMARY);
    result = seln_ask(&holder, SELN_REQ_IS_READONLY, 0, 0);
    data = result->data;
    /* Test if is SELN_IS_READONLY */
    data += sizeof(Seln_attribute);
    is_read_only = *(int *) data;

    if (is_read_only) {
	return;
    } else {
        context.size = 0;
#ifdef OW_I18N
	    query_result = seln_query( &holder, buf_reader, (char *)(&context),SELN_REQ_CONTENTS_ASCII,0,0);
	    str_ptr = mbstowcsdup((char *)context.sel_buffer);
#else /* OW_I18N */
        query_result = seln_query( &holder, buf_reader, (char *)(&context),SELN_REQ_CONTENTS_ASCII,0,0);
#endif /* OW_I18N */
        if (!is_copy) {
            seln_ask(&holder, SELN_REQ_DELETE, 0, 0);
        }
    }

    ev_set(view->e_view, EV_CHAIN_DELAY_UPDATE, FALSE, 0);
    EV_SET_INSERT(folio->views, pos, temp);
#ifdef OW_I18N
    (void) textsw_do_input(view, str_ptr, (long int ) STRLEN(str_ptr), TXTSW_UPDATE_SCROLLBAR);
    free(str_ptr);
#else
    (void) textsw_do_input(view, (char *) context.sel_buffer, (long int ) strlen(context.sel_buffer), TXTSW_UPDATE_SCROLLBAR);
#endif
 
    free((char *) context.sel_buffer);
    TEXTSW_DO_INSERT_MAKES_VISIBLE(view);
}

Seln_result
buf_reader( result )
Seln_request   *result;
{    
    char  *data;
    static char *str_ptr;
    struct textsw_context  *context;
     
    context = (struct textsw_context * ) result->requester.context;
     
    data = (char *)result->data;
    data += sizeof(Seln_attribute);
     
    if ( !context->size )
        str_ptr  = (char * ) malloc( strlen( data ) + 1 );
    else
        str_ptr = (char * ) realloc( str_ptr, ( strlen( str_ptr ) + strlen( data ) ) );
     
    strcpy( str_ptr + context->size, data );
     
    context->size += strlen( data );
    if ( result->status != SELN_CONTINUED ) {
        context->sel_buffer = (char * ) malloc( strlen( str_ptr ) + 1 );
        strcpy( context->sel_buffer, str_ptr );
        free( str_ptr );
    }
    return result->status;
}

/*
 * Cycle through all views of the folio to see if dest_win matches any of the
 * views.  If so, return that view, else return NULL
 */
static          Textsw_view_handle
get_button_up_view(folio, dest_win)
    Textsw_folio    folio;
    Textsw_view     dest_win;
{
    Textsw_view_handle v;
    Textsw_view_handle dest_view = VIEW_PRIVATE(dest_win);

    FORALL_TEXT_VIEWS(folio, v) {
	if (dest_view == v)
	    return v;

    }
    return NULL;
}

