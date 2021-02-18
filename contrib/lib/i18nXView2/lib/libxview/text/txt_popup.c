#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)txt_popup.c 50.8 90/11/09";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Text subwindow menu creation and support.
 */

#include <xview_private/primal.h>
#include <xview_private/txt_impl.h>
#include <xview_private/ev_impl.h>
#include <sys/time.h>
#include <signal.h>
#include <xview/notice.h>
#include <xview/frame.h>
#include <xview/panel.h>
#include <xview/textsw.h>
#include <xview/openmenu.h>
#include <xview/wmgr.h>
#include <xview/pixwin.h>
#include <xview/win_struct.h>
#include <xview/win_screen.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

#define   	MAX_STR_LENGTH		1024
/* This is for select line number */
#define		MAX_SEL_LINE_PANEL_ITEMS  2
/* This is for load, store and include file */
#define		MAX_FILE_PANEL_ITEMS	  3
/* This is for find and replace */
#define		MAX_SEARCH_PANEL_ITEMS	  10
/* This is for find marked text */
#define		MAX_MATCH_PANEL_ITEMS	  6

/* for select line number */
typedef enum {
    SEL_LINE_ITEM = 0,
    SEL_LINE_NUMBER_ITEM = 1,
}               Sel_line_panel_item_enum;

/* for load, store and include file */
typedef enum {
    FILE_CMD_ITEM = 0,
    DIR_STRING_ITEM = 1,
    FILE_STRING_ITEM = 2,
}               File_panel_item_enum;

/* for find and replace */
typedef enum {
    FIND_ITEM = 0,
    FIND_STRING_ITEM = 1,
    FIND_DONE_ITEM = 2,
    REPLACE_ITEM = 3,
    REPLACE_STRING_ITEM = 4,
    WRAP_ITEM = 5,
    FIND_THEN_REPLACE_ITEM = 6,
    REPLACE_THEN_FIND_ITEM = 7,
    REPLACE_ALL_ITEM = 8,
    SEARCH_BLINK_PARENT_ITEM = 9
} Search_panel_item_enum;

/* This is for find marked text */
typedef enum {
    CHOICE_ITEM = 0,
    FIND_PAIR_ITEM = 1,
    FIND_PAIR_CHOICE_ITEM = 2,
    INSERT_ITEM = 3,
    REMOVE_ITEM = 4
} Match_panel_item_enum;

extern int      STORE_FILE_POPUP_KEY;
extern int      LOAD_FILE_POPUP_KEY;
extern int      FILE_STUFF_POPUP_KEY;
extern int      SEARCH_POPUP_KEY;
extern int      MATCH_POPUP_KEY;
extern int      SEL_LINE_POPUP_KEY;

Panel_item      store_panel_items[MAX_FILE_PANEL_ITEMS];
Panel_item      load_panel_items[MAX_FILE_PANEL_ITEMS];
Panel_item      include_panel_items[MAX_FILE_PANEL_ITEMS];
Panel_item      search_panel_items[MAX_SEARCH_PANEL_ITEMS];
Panel_item      match_panel_items[MAX_MATCH_PANEL_ITEMS];
Panel_item      sel_line_panel_items[MAX_SEL_LINE_PANEL_ITEMS];

#ifdef OW_I18N
extern char	*dgettext();
#endif

extern void
textsw_create_popup_frame(view, popup_type)
    Textsw_view_handle view;
    int             popup_type;

{
    Frame           frame_parent = xv_get(VIEW_REP_TO_ABS(view), WIN_FRAME);
    Frame           popup_frame, base_frame;
    Panel           panel;
    char           *label;
#ifdef OW_I18N
    int		    win_use_im = ((popup_type != TEXTSW_MENU_SEL_MARK_TEXT) && 
    				  (popup_type != TEXTSW_MENU_NORMALIZE_LINE));
#endif    

    base_frame = (xv_get(frame_parent, XV_IS_SUBTYPE_OF, FRAME_BASE) ?
		  frame_parent : xv_get(frame_parent, WIN_OWNER));

    popup_frame = (Frame) xv_create(base_frame, FRAME_CMD,
#ifdef OW_I18N
				    WIN_USE_IM,	      win_use_im,
#endif    
				    FRAME_SHOW_LABEL, TRUE,
				    WIN_CLIENT_DATA, view,
				    WIN_FRONT,
				    0);

    switch (popup_type) {
      case TEXTSW_MENU_STORE:
	xv_set(frame_parent, XV_KEY_DATA, STORE_FILE_POPUP_KEY,
	       popup_frame, 0);
	panel = (Panel) textsw_create_store_panel(popup_frame, view);
#ifdef OW_I18N
	label = XV_I18N_MSG("xv_messages", "Text:Store");
#else
	label = "Text:Store";
#endif
	break;
      case TEXTSW_MENU_LOAD:
	xv_set(frame_parent, XV_KEY_DATA, LOAD_FILE_POPUP_KEY,
	       popup_frame, 0);
	panel = (Panel) textsw_create_load_panel(popup_frame, view);
#ifdef OW_I18N
	label = XV_I18N_MSG("xv_messages", "Text:Load");
#else
	label = "Text:Load";
#endif
	break;

      case TEXTSW_MENU_FILE_STUFF:
	xv_set(frame_parent, XV_KEY_DATA, FILE_STUFF_POPUP_KEY,
	       popup_frame, 0);
	panel = (Panel) textsw_create_include_panel(popup_frame, view);
#ifdef OW_I18N
	label = XV_I18N_MSG("xv_messages", "Text:Include");
#else
	label = "Text:Include";
#endif
	break;

      case TEXTSW_MENU_FIND_AND_REPLACE:
	xv_set(frame_parent, XV_KEY_DATA, SEARCH_POPUP_KEY,
	       popup_frame, 0);
	panel = (Panel) textsw_create_search_panel(popup_frame, view);
#ifdef OW_I18N
	label = XV_I18N_MSG("xv_messages", "Text:Find and Replace");
#else
	label = "Text:Find and Replace";
#endif
	break;

      case TEXTSW_MENU_SEL_MARK_TEXT:
	xv_set(frame_parent, XV_KEY_DATA, MATCH_POPUP_KEY,
	       popup_frame, 0);
	panel = (Panel) textsw_create_match_panel(popup_frame, view);
#ifdef OW_I18N
	label = XV_I18N_MSG("xv_messages", "Text:Find Marked Text");
#else
	label = "Text:Find Marked Text";
#endif
	break;

      case TEXTSW_MENU_NORMALIZE_LINE:
	xv_set(frame_parent, XV_KEY_DATA, SEL_LINE_POPUP_KEY,
	       popup_frame, 0);
	panel = (Panel) textsw_create_sel_line_panel(popup_frame, view);
#ifdef OW_I18N
	label = XV_I18N_MSG("xv_messages", "Text:Line Number");
#else
	label = "Text:Line Number";
#endif
	break;
    }



    (void) window_fit(panel);
    (void) window_fit(popup_frame);
    (void) xv_set(popup_frame, FRAME_LABEL, 
		  label,
		  XV_SHOW, TRUE, 0);


}

Pkg_private
textsw_get_and_set_selection(popup_frame, view, popup_type)
    Frame           popup_frame;
    Textsw_view_handle view;
    int             popup_type;
{
    Es_index        dummy;
    CHAR            show_str[MAX_STR_LENGTH];

    show_str[0] = NULL;
    (void) textsw_get_selection(view, &dummy, &dummy, show_str, MAX_STR_LENGTH);

    switch (popup_type) {
      case TEXTSW_MENU_STORE:
#ifdef OW_I18N
	(void) panel_set_value_wcs(store_panel_items[(int) FILE_STRING_ITEM],
			       show_str);
#else
	(void) panel_set_value(store_panel_items[(int) FILE_STRING_ITEM],
			       show_str);
#endif      
	break;
      case TEXTSW_MENU_LOAD:
#ifdef OW_I18N
	(void) panel_set_value_wcs(load_panel_items[(int) FILE_STRING_ITEM],
			       show_str);
#else
	(void) panel_set_value(load_panel_items[(int) FILE_STRING_ITEM],
			       show_str);
#endif 
	break;
      case TEXTSW_MENU_FILE_STUFF:
#ifdef OW_I18N
	(void) panel_set_value_wcs(include_panel_items[(int) FILE_STRING_ITEM],
			       show_str);
#else
	(void) panel_set_value(include_panel_items[(int) FILE_STRING_ITEM],
			       show_str);
#endif 
	break;
      case TEXTSW_MENU_FIND_AND_REPLACE:
#ifdef OW_I18N
	(void) panel_set_value_wcs(search_panel_items[(int) FIND_STRING_ITEM],
			       show_str);
#else
	(void) panel_set_value(search_panel_items[(int) FIND_STRING_ITEM],
			       show_str);
#endif 
	break;
      case TEXTSW_MENU_NORMALIZE_LINE:
#ifdef OW_I18N
	(void) panel_set_value_wcs(sel_line_panel_items[(int) SEL_LINE_NUMBER_ITEM],
			       show_str);
#else
	(void) panel_set_value(sel_line_panel_items[(int) SEL_LINE_NUMBER_ITEM],
			       show_str);
#endif 
	break;

    }

    (void) xv_set(popup_frame, XV_SHOW, TRUE,
		  WIN_CLIENT_DATA, view,
		  WIN_FRONT, 0);

}

Pkg_private
textsw_set_dir_str(popup_type)
    int             popup_type;
{
    char            curr_dir[MAX_STR_LENGTH];

    (void) getwd(curr_dir);
    switch (popup_type) {
      case TEXTSW_MENU_STORE:
	(void) panel_set_value(store_panel_items[(int) DIR_STRING_ITEM],
			       curr_dir);
	break;
      case TEXTSW_MENU_LOAD:
	(void) panel_set_value(load_panel_items[(int) DIR_STRING_ITEM],
			       curr_dir);
	break;
      case TEXTSW_MENU_FILE_STUFF:
	(void) panel_set_value(include_panel_items[(int) DIR_STRING_ITEM],
			       curr_dir);
	break;
    }
}

Pkg_private     Textsw_view_handle
text_view_frm_p_itm(panel_item)
    Panel_item      panel_item;
{
    Panel           panel = panel_get(panel_item, XV_OWNER, 0);
    Xv_Window       search_frame = xv_get(panel, WIN_FRAME);
    Textsw_view_handle view = (Textsw_view_handle) window_get(search_frame, WIN_CLIENT_DATA, 0);

    return (view);
}
Pkg_private     Xv_Window
frame_from_panel_item(panel_item)
    Panel_item      panel_item;
{
    Panel           panel = panel_get(panel_item, XV_OWNER, 0);
    Xv_Window       popup_frame = xv_get(panel, WIN_FRAME);

    return (popup_frame);
}
Pkg_private int
textsw_get_selection(view, first, last_plus_one, selected_str, max_str_len)
    Textsw_view_handle view;
    int            *first, *last_plus_one;
    CHAR           *selected_str;
    int             max_str_len;
{
    /*
     * Return true iff primary selection is in the textsw of current view. If
     * there is a selection in any of the textsw and selected_str is not
     * null, then it will copy it to selected_str.
     */
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);
    Textsw_selection_object selection;
    CHAR            selection_buf[MAX_STR_LENGTH];
    unsigned        options = EV_SEL_PRIMARY;

    textsw_init_selection_object(
	    folio, &selection, selection_buf, SIZEOF(selection_buf), FALSE);

    selection.type = textsw_func_selection_internal(
			       folio, &selection, EV_SEL_BASE_TYPE(options),
						    TFS_FILL_ALWAYS);

    textsw_clear_secondary_selection(folio, selection.type);

    if ((selection.type & TFS_IS_SELF) &&
	(selection.type & EV_SEL_PRIMARY)) {
	/* If this window owns the primary selection, do nothing. */
    } else {
	selection.first = selection.last_plus_one = ES_CANNOT_SET;
    }


    if ((selection.type & EV_SEL_PRIMARY) &&
	(selection.buf_len > 0) && (selected_str != NULL)) {

	if (selection.buf_len >= max_str_len)
	    selection.buf_len = max_str_len - 1;

	STRNCPY(selected_str, selection.buf, selection.buf_len);
	selected_str[selection.buf_len] = NULL;
    }
    *first = selection.first;
    *last_plus_one = selection.last_plus_one;

    return ((*first != ES_CANNOT_SET) && (*last_plus_one != ES_CANNOT_SET));
}

Pkg_private void
textsw_set_pop_up_location(frame_parent, pop_up_frame)
    Frame           frame_parent, pop_up_frame;
{
#define MY_OFFSET		4

    Rect            base_rect, pop_up_rect, screen_rect;
    short           new_x, new_y;
    int             pop_up_fd = (int) window_get(pop_up_frame, WIN_FD);
    int             max_cover_area;

    screen_rect = *((Rect *) window_get(frame_parent, WIN_SCREEN_RECT));
    base_rect = *((Rect *) window_get(frame_parent, WIN_RECT));

    win_getrect(pop_up_fd, &pop_up_rect);

    new_x = pop_up_rect.r_left;
    new_y = pop_up_rect.r_top;
    max_cover_area = base_rect.r_width / 3;

    if ((base_rect.r_top - (pop_up_rect.r_height + MY_OFFSET)) >= 0)
	new_y = base_rect.r_top - (pop_up_rect.r_height + MY_OFFSET);
    else if ((base_rect.r_left - pop_up_rect.r_width + MY_OFFSET) >= 0)
	new_x = base_rect.r_left - (pop_up_rect.r_width + MY_OFFSET);
    else if ((base_rect.r_left + base_rect.r_width + pop_up_rect.r_width + MY_OFFSET) <=
	     screen_rect.r_width)
	new_x = base_rect.r_left + base_rect.r_width;
    else if ((pop_up_rect.r_width + MY_OFFSET - base_rect.r_left) <= max_cover_area)
	new_x = 0;
    else if ((base_rect.r_left + base_rect.r_width - max_cover_area) <= (screen_rect.r_width - (pop_up_rect.r_width + MY_OFFSET)))
	new_x = screen_rect.r_width - (pop_up_rect.r_width + MY_OFFSET);
    if (new_y < 0)
	new_y = 0;

    pop_up_rect.r_left = new_x;
    pop_up_rect.r_top = new_y;


    win_setrect(pop_up_fd, &pop_up_rect);
#undef MY_OFFSET
}
