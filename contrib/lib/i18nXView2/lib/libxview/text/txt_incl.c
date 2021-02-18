#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)txt_incl.c 50.8 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Text include popup frame creation and support.
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

typedef enum {
    FILE_CMD_ITEM = 0,
    DIR_STRING_ITEM = 1,
    FILE_STRING_ITEM = 2,
}               File_panel_item_enum;

extern Panel_item include_panel_items[];
Pkg_private Textsw_view_handle text_view_frm_p_itm();
Pkg_private Xv_Window frame_from_panel_item();


static int
do_include_proc(folio, ie)
    Textsw_folio    folio;
    Event          *ie;
{
#ifdef OW_I18N
    CHAR            *temp_file_str, *temp_dir_str;
    CHAR	     file_str[MAX_STR_LENGTH], dir_str[MAX_STR_LENGTH];
#else
    char            *file_str, *dir_str;
#endif    
    register int    locx, locy;
    char	    curr_dir[MAX_STR_LENGTH];
#ifdef OW_I18N
    CHAR	    curr_dir_ws[MAX_STR_LENGTH];	
#endif    
    Frame           popup_frame;

#ifdef OW_I18N
    temp_dir_str = (CHAR  *) xv_get(include_panel_items[(int) DIR_STRING_ITEM],
			      PANEL_VALUE_WCS);
    temp_file_str = (CHAR  *) xv_get(include_panel_items[(int) FILE_STRING_ITEM],
			       PANEL_VALUE_WCS);
    STRCPY(dir_str, temp_dir_str);
    STRCPY(file_str, temp_file_str);			       
#else
    dir_str = (CHAR  *) xv_get(include_panel_items[(int) DIR_STRING_ITEM],
			      PANEL_VALUE);
    file_str = (CHAR  *) xv_get(include_panel_items[(int) FILE_STRING_ITEM],
			       PANEL_VALUE);
#endif
    if AN_ERROR
	(ie == 0)
	    locx = locy = 0;
    else {
	locx = ie->ie_locx;
	locy = ie->ie_locy;
    }
#ifdef OW_I18N
    if (textsw_expand_filename(folio, dir_str, MAX_STR_LENGTH, locx, locy)) {
	/* error handled inside routine */
	return TRUE;
    }
    if (textsw_expand_filename(folio, file_str, MAX_STR_LENGTH, locx, locy)) {
	/* error handled inside routine */
	return TRUE;
    }
#else    
    if (textsw_expand_filename(folio, dir_str, locx, locy)) {
	/* error handled inside routine */
	return TRUE;
    }
    if (textsw_expand_filename(folio, file_str, locx, locy)) {
	/* error handled inside routine */
	return TRUE;
    }
#endif

    /* if "cd" is not disabled and the "cd" dir is not the current dir */
    (void) getwd(curr_dir);
#ifdef OW_I18N
    mbstowcs(curr_dir_ws, curr_dir, MAX_STR_LENGTH);
    if (STRCMP(curr_dir_ws, dir_str) != 0) {
#else    
    if (strcmp(curr_dir, dir_str) != 0) {
#endif    
	if (!(folio->state & TXTSW_NO_CD)) {
	    if (textsw_change_directory(folio, dir_str, FALSE, locx, locy) != 0) {
		return TRUE;
	    }
	} else {
	    (void) notice_prompt(FRAME_FROM_FOLIO_OR_VIEW(folio),
				 (Event *) 0,
				 NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
				 XV_I18N_MSG("xv_messages", "\
Cannot change directory.\n\
Change Directory Has Been Disabled."),
#else
				 "Cannot change directory.",
				 "Change Directory Has Been Disabled.",
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
	    return TRUE;
	}
    }
    if (STRLEN(file_str) > 0) {
	if (textsw_file_stuff_from_str(VIEW_FROM_FOLIO_OR_VIEW(folio),
				       file_str, locx, locy) == 0) {
	    popup_frame =
		frame_from_panel_item(include_panel_items[(int) FILE_CMD_ITEM]);
	    (void) xv_set(popup_frame, XV_SHOW, FALSE, 0);
	    return FALSE;
	}
	return TRUE;
    }
    (void) notice_prompt(FRAME_FROM_FOLIO_OR_VIEW(folio),
			 (Event *) 0,
			 NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
			 XV_I18N_MSG("xv_messages", "\
No file name was specified.\n\
Specify a file name to Include File."),
#else
			 "No file name was specified.",
			 "Specify a file name to Include File.",
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
    /* if we made it here then there was an error or notice */
    return TRUE;
}

static          Panel_setting
include_cmd_proc(item, event)
    Panel_item      item;
    Event          *event;
{
    Textsw_view_handle view = text_view_frm_p_itm(item);
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);
    int             error;

    if (item == include_panel_items[(int) FILE_CMD_ITEM]) {
	error = do_include_proc(folio, event);
	if (error) {
	    xv_set(item, PANEL_NOTIFY_STATUS, XV_ERROR, 0);
	}
	return PANEL_NONE;
    }
    return PANEL_NEXT;
}

static          Panel_setting
include_cmd_proc_accel(item, event)
    Panel_item      item;
    Event          *event;
{
    Textsw_view_handle view = text_view_frm_p_itm(item);
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);
    int             error;

    if (item == include_panel_items[(int) FILE_STRING_ITEM]) {
	error = do_include_proc(folio, event);
	if (error) {
	    xv_set(item, PANEL_NOTIFY_STATUS, XV_ERROR, 0);
	}
	return PANEL_NONE;
    }
    return PANEL_NEXT;
}
static void
create_include_items(panel, view)
    Panel           panel;
    Textsw_view_handle view;
{
#ifndef OW_I18N
    static char     *include_file = "Include File";
#endif
    CHAR             include_string[MAX_STR_LENGTH];
    char             current_dir_include_string[MAX_STR_LENGTH];
    int             dummy;

    include_string[0] = NULL;
    (void) textsw_get_selection(view, &dummy, &dummy, include_string,
				MAX_STR_LENGTH);
    (void) getwd(current_dir_include_string);
    include_panel_items[(int) DIR_STRING_ITEM] =
	panel_create_item(panel, PANEL_TEXT,
			  PANEL_LABEL_X, ATTR_COL(0),
			  PANEL_LABEL_Y, ATTR_ROW(0),
			  PANEL_VALUE_DISPLAY_LENGTH, MAX_DISPLAY_LENGTH,
			  PANEL_VALUE_STORED_LENGTH, MAX_STR_LENGTH,
			  PANEL_VALUE, current_dir_include_string,
			  PANEL_LABEL_STRING, 
#ifdef OW_I18N
			XV_I18N_MSG("xv_messages", "Directory:"),
#else
			"Directory:",
#endif
			HELP_INFO("textsw:dirstring")
			  0);
    include_panel_items[(int) FILE_STRING_ITEM] =
	panel_create_item(panel, PANEL_TEXT,
			  PANEL_LABEL_X, ATTR_COL(5),
			  PANEL_LABEL_Y, ATTR_ROW(1),
			  PANEL_VALUE_DISPLAY_LENGTH, MAX_DISPLAY_LENGTH,
			  PANEL_VALUE_STORED_LENGTH, MAX_STR_LENGTH,
#ifdef OW_I18N			  
			  PANEL_VALUE_WCS, include_string,
#else
			  PANEL_VALUE, include_string,
#endif			  
			  PANEL_LABEL_STRING, 
#ifdef OW_I18N
			XV_I18N_MSG("xv_messages", "File:"),
#else
			"File:",
#endif
			  PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
			  PANEL_NOTIFY_STRING, "\n\r",
			  PANEL_NOTIFY_PROC, include_cmd_proc_accel,
			  HELP_INFO("textsw:includestring")
			  0);
    xv_set(panel, PANEL_CARET_ITEM,
	   include_panel_items[(int) FILE_STRING_ITEM], 0);

    include_panel_items[(int) FILE_CMD_ITEM] =
	panel_create_item(panel, PANEL_BUTTON,
			  PANEL_LABEL_X, ATTR_COL(26),
			  PANEL_LABEL_Y, ATTR_ROW(2),
			  PANEL_LABEL_STRING, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Include File"),
#else
				include_file,
#endif
			  PANEL_NOTIFY_PROC, include_cmd_proc,
			  HELP_INFO("textsw:includefile")
			  0);
    (void) xv_set(panel,
	       PANEL_DEFAULT_ITEM, include_panel_items[(int) FILE_CMD_ITEM],
		  0);
}
extern          Panel
textsw_create_include_panel(frame, view)
    Frame           frame;
    Textsw_view_handle view;
{
    Panel           panel;

    panel = (Panel) xv_get(frame, FRAME_CMD_PANEL,
			HELP_INFO("textsw:includepanel")
			   0);
    (void) create_include_items(panel, view);

    return (panel);
}
