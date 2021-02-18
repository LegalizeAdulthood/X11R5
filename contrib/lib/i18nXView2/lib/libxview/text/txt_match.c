#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)txt_match.c 50.8 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Text match delimiter popup frame creation and support.
 */


#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#include <xview/xv_i18n.h>
#endif OW_I18N
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

#define		MAX_DISPLAY_LENGTH	50
#define   	MAX_STR_LENGTH		1024

#define HELP_INFO(s) XV_HELP_DATA, s,

/* This is for find marked text */
typedef enum {
    CHOICE_ITEM = 0,
    FIND_PAIR_ITEM = 1,
    FIND_PAIR_CHOICE_ITEM = 2,
    INSERT_ITEM = 3,
    REMOVE_ITEM = 4
} Match_panel_item_enum;

extern Panel_item match_panel_items[];

Pkg_private Textsw_view_handle text_view_frm_p_itm();
Pkg_private Xv_Window frame_from_panel_item();

#ifdef OW_I18N

static CHAR l_curly_brace[] = { '{', 0 };
static CHAR l_paren[] = { '(', 0 };
static CHAR dbl_quote[] = { '"', 0 };
static CHAR sgl_quote[] = { '\'', 0 };
static CHAR accent_grave[] = { '`', 0 };
static CHAR l_square_brace[] = { '[', 0 };
static CHAR bar_gt[] = { '|', '>', 0 };
static CHAR open_comment[] = { '/', '*', 0 };

static CHAR r_curly_brace[] = { '}', 0 };
static CHAR r_paren[] = { ')', 0 };
static CHAR r_square_brace[] = { ']', 0 };
static CHAR lt_bar[] = { '<', '|', 0 };
static CHAR close_comment[] = { '*', '/', 0 };

static CHAR     *delimiter_pairs[2][8] =
{{ l_paren, dbl_quote, sgl_quote, accent_grave, bar_gt, l_square_brace, 
   l_curly_brace, open_comment,},
 { r_paren, dbl_quote, sgl_quote, accent_grave, lt_bar, r_square_brace, 
   r_curly_brace, close_comment,}};
#else
static char     *delimiter_pairs[2][8] = {
    {"(", "\"", "'", "`", "|>", "[", "{", "/*"},
    {")", "\"", "'", "`", "<|", "]", "}", "*/"}
};
#endif

static void
do_insert_or_remove_delimiter(view, value, do_insert)
    Textsw_view_handle view;
    int             do_insert;
{
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);
    Es_index        first, last_plus_one, ro_bdry, num_of_byte, temp;
    CHAR            *buf, *sel_str, *temp_ptr;


    if (TXTSW_IS_READ_ONLY(folio)) {
	(void) notice_prompt(FRAME_FROM_FOLIO_OR_VIEW(folio),
			     (Event *) 0,
			     NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
			     XV_I18N_MSG("xv_messages", "\
Operation is aborted.\n\
This text window is read only."),
#else
			     "Operation is aborted.",
			     "This text window is read only.",
#endif
			     0,
			     NOTICE_BUTTON_YES, 
#ifdef OW_I18N
			     XV_I18N_MSG("xv_messages", "Continue"),
#else
			     "Continue",
#endif
			     NOTICE_TRIGGER, ACTION_STOP,
			     0);
	return;
    }
    ro_bdry = textsw_read_only_boundary_is_at(folio);
    (void) ev_get_selection(folio->views, &first, &last_plus_one, EV_SEL_PRIMARY);
    if ((ro_bdry != 0) && (last_plus_one <= ro_bdry)) {
	(void) notice_prompt(FRAME_FROM_FOLIO_OR_VIEW(folio),
			     (Event *) 0,
			     NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
			     XV_I18N_MSG("xv_messages", "\
Operation is aborted.\n\
Selected text is in read only area."),
#else
			     "Operation is aborted.",
			     "Selected text is in read only area.",
#endif
			     0,
			     NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
			     NOTICE_TRIGGER, ACTION_STOP,
			     0);
	return;

    }
    buf = sel_str = temp_ptr = NULL;
    if (do_insert) {
	temp_ptr = buf = MALLOC((unsigned) ((last_plus_one - first) + 4));
	buf[0] = NULL;
	STRCPY(buf, delimiter_pairs[0][value]);
	temp_ptr = buf + STRLEN(buf);

	if (first < last_plus_one) {
	    sel_str = MALLOC((unsigned) (last_plus_one - first));
	    if (textsw_get_selection_as_string(folio, EV_SEL_PRIMARY, sel_str, ((last_plus_one - first) + 1))) {
		STRCPY(temp_ptr, sel_str);
		temp_ptr = buf + STRLEN(buf);
	    }
	} else {
	    first = last_plus_one = EV_GET_INSERT(folio->views);
	}
	STRCPY(temp_ptr, delimiter_pairs[1][value]);
    } else {
	int             sel_str_len = last_plus_one - first;
	int             del_len1 = STRLEN(delimiter_pairs[0][value]);
	int             del_len2 = STRLEN(delimiter_pairs[1][value]);

	buf = MALLOC(sel_str_len);
	if (first < last_plus_one) {
	    sel_str = MALLOC(sel_str_len);
	    if (textsw_get_selection_as_string(folio, EV_SEL_PRIMARY, sel_str, (sel_str_len + 1))) {
		temp_ptr = sel_str + (sel_str_len - del_len2);

		if ((STRNCMP(delimiter_pairs[0][value], sel_str, del_len1) == 0) &&
		    (STRNCMP(delimiter_pairs[1][value], temp_ptr, del_len2) == 0)) {
		    STRNCPY(buf, sel_str + del_len1,
			    (sel_str_len - (del_len1 + del_len2)));
		    buf[(sel_str_len - (del_len1 + del_len2))] = NULL;
		} else {
		    (void) notice_prompt(FRAME_FROM_FOLIO_OR_VIEW(folio),
					 (Event *) 0,
					 NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "\
Operation is aborted.\n\
Selection does not include the indicated pair."),
#else
					 "Operation is aborted.",
			   "Selection does not include the indicated pair.",
#endif
					 0,
					 NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
					 NOTICE_TRIGGER, ACTION_STOP,
					 0);
		    return;
		}
	    }
	} else {
	    (void) notice_prompt(FRAME_FROM_FOLIO_OR_VIEW(folio),
				 (Event *) 0,
				 NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
			XV_I18N_MSG("xv_messages", "Operation is aborted, because no text is selected"),
#else
			"Operation is aborted, because no text is selected",
#endif
				 0,
				 NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
				 NOTICE_TRIGGER, ACTION_STOP,
				 0);
	    return;
	}
    }

    num_of_byte = textsw_replace(FOLIO_REP_TO_ABS(folio), first, last_plus_one, buf, STRLEN(buf));

    if (num_of_byte != 0)
	EV_SET_INSERT(folio->views, last_plus_one + num_of_byte, temp);
    if (buf != NULL)
	free(buf);
    if (sel_str != NULL)
	free(sel_str);

}

static          Panel_setting
match_cmd_proc(item, event)
    Panel_item      item;
    Event          *event;
{
    Textsw_view_handle view = text_view_frm_p_itm(item);
    int             delimiter_value = (int) panel_get(
			 match_panel_items[(int) CHOICE_ITEM], PANEL_VALUE);

    if (item == match_panel_items[(int) FIND_PAIR_ITEM]) {
	int             value = (int) panel_get(match_panel_items[(int) FIND_PAIR_CHOICE_ITEM],
						PANEL_VALUE);

	switch (value) {
	  case 0:
	    (void) textsw_match_selection_and_normalize(view,
					delimiter_pairs[0][delimiter_value],
						  TEXTSW_DELIMITER_FORWARD);
	    break;
	  case 1:
	    (void) textsw_match_selection_and_normalize(view,
					delimiter_pairs[1][delimiter_value],
						 TEXTSW_DELIMITER_BACKWARD);
	    break;
	  case 2:{
		Textsw_folio    folio = FOLIO_FOR_VIEW(view);
		int             first, last_plus_one;

		first = last_plus_one = EV_GET_INSERT(folio->views);
		(void) textsw_match_field_and_normalize(view,
							&first,
							&last_plus_one,
					delimiter_pairs[1][delimiter_value],
				STRLEN(delimiter_pairs[1][delimiter_value]),
					   TEXTSW_DELIMITER_ENCLOSE, FALSE);
		break;
	    }
	}

    } else if (item == match_panel_items[(int) INSERT_ITEM]) {
	(void) do_insert_or_remove_delimiter(view, delimiter_value, TRUE);
    } else if (item == match_panel_items[(int) REMOVE_ITEM]) {
	(void) do_insert_or_remove_delimiter(view, delimiter_value, FALSE);
    }
    return PANEL_NEXT;
}
static void
create_match_items(panel, view)
    Panel           panel;
    Textsw_view_handle view;
{

    char            *curly_bracket = " { }  ";
    char            *parenthesis = " ( )  ";
    char            *double_quote = " \" \"  ";
    char            *single_quote = " ' '  ";
    char            *back_qoute = " ` `  ";
    char            *square_bracket = " [ ]  ";
    char            *field_marker = " |> <|  ";
    char            *comment = " /* */  ";

#ifdef OW_I18N
    char            *Insert_Pair = (char *)XV_I18N_MSG("xv_messages", "Insert Pair");
    char            *Backward = (char *)XV_I18N_MSG("xv_messages", "Backward");
    char            *Expand = (char *)XV_I18N_MSG("xv_messages", "Expand");
    char            *Forward = (char *)XV_I18N_MSG("xv_messages", "Forward");
    char            *Remove_Pair = (char *)XV_I18N_MSG("xv_messages", "Remove Pair");
    char            *Find_Pair = (char *)XV_I18N_MSG("xv_messages", "Find Pair");
#else
    CHAR            *Insert_Pair = "Insert Pair";
    CHAR            *Backward = "Backward";
    CHAR            *Expand = "Expand";
    CHAR            *Forward = "Forward";
    CHAR            *Remove_Pair = "Remove Pair";
    CHAR            *Find_Pair = "Find Pair";
#endif


    match_panel_items[(int) CHOICE_ITEM] =
	panel_create_item(panel, PANEL_CHOICE,
			  PANEL_LABEL_X, ATTR_COL(0),
			  PANEL_LABEL_Y, ATTR_ROW(0),
			  PANEL_CHOICE_STRINGS, parenthesis, double_quote,
			  single_quote, back_qoute,
			  field_marker, square_bracket,
			  curly_bracket, comment, 0,
			  XV_HELP_DATA, 
				"textsw:fieldchoice",
			  0);


    match_panel_items[(int) FIND_PAIR_ITEM] =
	panel_create_item(panel, PANEL_BUTTON,
			  PANEL_LABEL_X, ATTR_COL(0),
			  PANEL_LABEL_Y, ATTR_ROW(1),
			  PANEL_LABEL_STRING, Find_Pair,
			  PANEL_NOTIFY_PROC, match_cmd_proc,
			  XV_HELP_DATA, 
				"textsw:findpair",
			  0);

    match_panel_items[(int) FIND_PAIR_CHOICE_ITEM] =
	panel_create_item(panel, PANEL_CHOICE,
			  PANEL_LABEL_STRING, ":",
			  PANEL_CHOICE_STRINGS, Forward, Backward, Expand, 0,
			  XV_HELP_DATA,
				"textsw:findpairchoice",
			  0);

    match_panel_items[(int) INSERT_ITEM] =
	panel_create_item(panel, PANEL_BUTTON,
			  PANEL_LABEL_X, ATTR_COL(0),
			  PANEL_LABEL_Y, ATTR_ROW(3),
			  PANEL_LABEL_STRING, Insert_Pair,
			  PANEL_NOTIFY_PROC, match_cmd_proc,
			  XV_HELP_DATA, 
				"textsw:insertpair",
			  0);

    match_panel_items[(int) REMOVE_ITEM] =
	panel_create_item(panel, PANEL_BUTTON,
			  PANEL_LABEL_STRING, Remove_Pair,
			  PANEL_NOTIFY_PROC, match_cmd_proc,
			  XV_HELP_DATA, 
				"textsw:removepair",
			  0);

}
extern          Panel
textsw_create_match_panel(frame, view)
    Frame           frame;
    Textsw_view_handle view;
{
    Panel           panel;

    panel = (Panel) xv_get(frame, FRAME_CMD_PANEL,
			   XV_HELP_DATA, 
				"textsw:fieldpanel",
			   0);
    (void) create_match_items(panel, view);

    return (panel);
}
