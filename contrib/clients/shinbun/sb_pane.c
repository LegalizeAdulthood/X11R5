#ifdef sccs
static char     sccsid[] = "@(#)sb_pane.c	1.29 91/09/13";
#endif
/*
****************************************************************************

              Copyright 1991, by Sun Microsystems, Inc.

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Sun Microsystems, Inc.
not be used in advertising or publicity pertaining to distribution of
the software without specific, written prior permission.  
Sun Microsystems, Inc. makes no representations about the suitability
of this software for any purpose.  It is provided "as is" without 
express or implied warranty.

SUN MICROSYSTEMS, INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL  SUN MICROSYSTEMS, INC. BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Auther: Hiroshi Watanabe	(nabe@japan.sun.com) Sun Microsystems, Inc.
	Tomonori Shioda		(shioda@japan.sun.com) Sun Microsystems, Inc.

****************************************************************************
*/


#include <stdio.h>
#include <string.h>

#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/text.h>
#include <xview/seln.h>
#include <xview/server.h>
#include <xview/font.h>
#include <xview/cursor.h>

#include "sb_def.h"
#include "sb_tool.h"
#include "shinbun.h"
#include "config.h"

/*
 * Used for main frame
 */
Frame           sb_frame;	/* main frame */
Panel           sb_panel;	/* main panel */
Icon            sb_frame_icon;	/* frame icon */
Panel_item      sb_group_txt;	/* panel text in main frame */
Menu            sb_group_menu;	/* menu in main frame */

/*
 * Used for find frame
 */
Frame           sb_find_frame;	/* find frame */
Panel_item      sb_find_sender_txt;	/* panel text in find frame */
Panel_item      sb_find_subject_txt;	/* panel text in find frame */

/*
 * Used for cancel frame
 */
Frame           sb_cancel_frame;/* cancel frame */
Panel_item      sb_cancel_group_txt;	/* panel text used for group name */
Panel_item      sb_cancel_newsno_txt;	/* panel text used for newsno name */

/*
 * for biff
 */
Frame           sb_newsbiff_frame;	/* news biff frame */
Panel_item      sb_newsbiff_panel;	/* news biff panel list */

/*
 * view proc
 */
extern void     sb_view_proc();
extern void     sb_next_proc();
extern void     sb_prev_proc();
extern void     sb_MsgID_proc();

/*
 * Article proc
 */
extern void     sb_article_asread_proc();
extern void     sb_article_asunread_proc();
static void     make_find_frame();
void            make_cancel_frame();
extern void     sb_print_proc();

/*
 * Newsgroup proc
 */
extern void     sb_reads_proc();
extern void     sb_unreads_proc();
extern void     sb_group_as_read_proc();
extern void     sb_group_as_unread_proc();
extern void     sb_save_change_proc();
extern void     sb_popup_latest_proc();
extern void     sb_next_ng_proc();
extern void     sb_catch_up_proc();
#if NOT_IMPREMENT
extern void     sb_status_proc();
#endif

/*
 * Open proc
 */
extern void     sb_open_proc();
extern void     sb_rescan_proc();

/*
 * post proc
 */
extern void     sb_post_proc();

/*
 * Props proc
 */
extern void     make_news_props_win();
extern void     make_tool_props_win();
extern void     make_sign_props_win();
extern void     make_biff_props_win();

/*
 * Biff proc
 */
extern void     sb_biff_panel_proc();

/*
 * canvas menu
 */
extern void     sb_next_subject_find_proc();
extern void     sb_prev_subject_find_proc();
extern void     sb_all_subject_find_proc();

/*
 * Panel text field proc
 */
extern void     sb_group_txt_proc();

/*
 * Menu proc
 */
extern Menu     sb_group_all_menu_proc();
extern Menu     sb_group_select_menu_proc();

/*
 * Find proc
 */
extern void     sb_find_proc();

/*
 * Super next proc
 */
extern void     sb_super_next_proc();

/*
 * New article is arrived or not.
 */
extern int      sb_arrive_icon;

/*
 * Make main frame & icon.
 */
void
make_main_frame()
{

#ifdef OW_I18N
	Font            icon_font;
#endif

	sb_frame_icon = xv_create(NULL, ICON,
	/*
	 * ICON_IMAGE,		sb_non_arrive_image, ICON_MASK_IMAGE,
	 * sb_non_arrive_mask_image, ICON_TRANSPARENT,	TRUE,
	 */
#ifdef OW_I18N
				  XV_LABEL, gettext("Shinbun tool"),
#else
				  XV_LABEL, "Shinbun tool",
#endif
				  NULL);

	sb_frame = xv_create(NULL, FRAME,
			     WIN_IS_CLIENT_PANE,
#ifdef OW_I18N
			     WIN_USE_IM, FALSE,
#endif
			     FRAME_NO_CONFIRM, TRUE,
			     FRAME_SHOW_FOOTER, TRUE,
			     FRAME_SHOW_RESIZE_CORNER, TRUE,
			     FRAME_NO_CONFIRM, TRUE,
			     FRAME_ICON, sb_frame_icon,
			     FRAME_CLOSED, TRUE,
			     NULL);
	(void) window_check(sb_frame, "sb_frame");

#ifdef OW_I18N
	icon_font = xv_find(sb_frame_icon, FONT,
			    FONT_FAMILY, FONT_FAMILY_SANS_SERIF,
			    FONT_STYLE, FONT_STYLE_NORMAL,
			    FONT_SIZE, 12,
			    NULL);
	(void) xv_set(sb_frame_icon, XV_FONT, icon_font, NULL);
#endif

	sb_arrive_icon = FALSE;

#ifdef OW_I18N
	(void) xv_set(sb_frame, XV_LABEL,
		      gettext(SHINBUN_VERSION_NO), NULL);
#else
	(void) xv_set(sb_frame, XV_LABEL,
		      SHINBUN_VERSION_NO, NULL);
#endif

	/* Make news biff panel at first */
	(void) make_newsbiff_frame();
}

int
make_main_panel(panel)
	Panel           panel;
{

	Menu            menu;
	int             row = 0;
	static Menu     make_file_pulldown();
	static Menu     make_open_pulldown();
	static Menu     make_article_pulldown();
	static Menu     make_view_pulldown();
	static Menu     make_post_pulldown();
	static Menu     make_props_pulldown();
	static Menu     make_group_pulldown();
	static Menu     make_next_pulldown();

	(void) xv_create(panel, PANEL_BUTTON,
			 XV_X, xv_col(panel, 1),
			 XV_Y, xv_row(panel, row),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Newsgroup"),
#else
			 PANEL_LABEL_STRING, "Newsgroup",
#endif

			 PANEL_ITEM_MENU, menu = make_file_pulldown(),
			 NULL);
	(void) xv_create(panel, PANEL_BUTTON,
			 XV_Y, xv_row(panel, row),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Open"),
#else
			 PANEL_LABEL_STRING, "Open",
#endif

			 PANEL_ITEM_MENU, menu = make_open_pulldown(),
			 NULL);
	(void) xv_create(panel, PANEL_ABBREV_MENU_BUTTON,
			 XV_Y, xv_row(panel, row),
		     PANEL_ITEM_MENU, sb_group_menu = make_group_pulldown(),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Group:"),
#else
			 PANEL_LABEL_STRING, "Group:",
#endif
			 PANEL_LABEL_BOLD, TRUE,
			 NULL);
	sb_group_txt = xv_create(panel, PANEL_TEXT,
				 XV_Y, xv_row(panel, row++),
				 PANEL_VALUE_DISPLAY_LENGTH, 30,
				 PANEL_VALUE_STORED_LENGTH, 100,
				 PANEL_NOTIFY_PROC, sb_group_txt_proc,
				 NULL);
	(void) xv_create(panel, PANEL_BUTTON,
			 XV_X, xv_col(panel, 1),
			 XV_Y, xv_row(panel, row),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Article"),
#else
			 PANEL_LABEL_STRING, "Article",
#endif
			 PANEL_ITEM_MENU, menu = make_article_pulldown(),
			 NULL);
	(void) xv_create(panel, PANEL_BUTTON,
			 XV_Y, xv_row(panel, row),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("View"),
#else
			 PANEL_LABEL_STRING, "View",
#endif
			 PANEL_ITEM_MENU, menu = make_view_pulldown(),
			 NULL);
	(void) xv_create(panel, PANEL_BUTTON,
			 XV_Y, xv_row(panel, row),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Post"),
#else
			 PANEL_LABEL_STRING, "Post",
#endif
			 PANEL_ITEM_MENU, menu = make_post_pulldown(),
			 NULL);
	(void) xv_create(panel, PANEL_BUTTON,
			 XV_Y, xv_row(panel, row),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Props"),
#else
			 PANEL_LABEL_STRING, "Props",
#endif
			 PANEL_ITEM_MENU, menu = make_props_pulldown(),
			 NULL);
	(void) xv_create(panel, PANEL_BUTTON,
			 XV_Y, xv_row(panel, row),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Read Next"),
#else
			 PANEL_LABEL_STRING, "Read Next",
#endif
			 PANEL_ITEM_MENU, menu = make_next_pulldown(),
			 NULL);

	return 1;

}

static          Menu
make_file_pulldown()
{
	Menu            menu, sub_menu;
	Menu_item       tmp_item;

	menu = xv_create(XV_NULL, MENU,
#ifdef OW_I18N
			 MENU_GEN_PIN_WINDOW, sb_frame, gettext("Newsgroup"),
#else
			 MENU_GEN_PIN_WINDOW, sb_frame, "Newsgroup",
#endif
			 NULL);

	sub_menu = xv_create(XV_NULL, MENU,
			     NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Next"),
#else
			     MENU_STRING, "Next",
#endif
			     MENU_ACTION_PROC, sb_next_ng_proc,
			     XV_KEY_DATA, NEXT_NG, (char *) NEXT,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Prev"),
#else
			     MENU_STRING, "Prev",
#endif
			     MENU_ACTION_PROC, sb_next_ng_proc,
			     XV_KEY_DATA, NEXT_NG, (char *) PREV,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			  MENU_PULLRIGHT_ITEM, gettext("Next ng"), sub_menu,
#else
			     MENU_PULLRIGHT_ITEM, "Next ng", sub_menu,
#endif
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	/*
	 * sub_menu = xv_create(XV_NULL,	MENU, NULL);
	 * 
	 * tmp_item = xv_create(XV_NULL, MENUITEM, #ifdef OW_I18N MENU_STRING,
	 * ettext("Unread"), #else MENU_STRING,		"Unread", #endif
	 * MENU_ACTION_PROC,	sb_unreads_proc, NULL);
	 * 
	 * xv_set(sub_menu, MENU_APPEND_ITEM,	tmp_item, NULL);
	 * 
	 * tmp_item = xv_create(XV_NULL, MENUITEM, #ifdef OW_I18N MENU_STRING,
	 * ettext("All"), #else MENU_STRING,		"All", #endif
	 * MENU_ACTION_PROC,	sb_reads_proc, NULL);
	 * 
	 * xv_set(sub_menu, MENU_APPEND_ITEM,	tmp_item, NULL);
	 * 
	 * tmp_item = xv_create(XV_NULL, MENUITEM, #ifdef OW_I18N
	 * MENU_PULLRIGHT_ITEM,	gettext("Read"),	sub_menu, #else
	 * MENU_PULLRIGHT_ITEM,	"Read",	sub_menu, #endif NULL);
	 * 
	 * xv_set(menu, MENU_APPEND_ITEM,	tmp_item, NULL);
	 * 
	 */
	sub_menu = xv_create(XV_NULL, MENU,
			     NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("As read"),
#else
			     MENU_STRING, "As read",
#endif
			     MENU_ACTION_PROC, sb_group_as_read_proc,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("As unread"),
#else
			     MENU_STRING, "As unread",
#endif
			     MENU_ACTION_PROC, sb_group_as_unread_proc,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_PULLRIGHT_ITEM, gettext("Mark"), sub_menu,
#else
			     MENU_PULLRIGHT_ITEM, "Oark", sub_menu,
#endif
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Catch up"),
#else
			     MENU_STRING, "Catch up",
#endif
			     MENU_NOTIFY_PROC, sb_catch_up_proc,
			     NULL);

	/*
	 * xv_set(menu, MENU_APPEND_ITEM,	tmp_item, NULL);
	 * 
	 * sub_menu = xv_create(XV_NULL,	MENU, NULL);
	 * 
	 * tmp_item = xv_create(XV_NULL, MENUITEM, #ifdef OW_I18N MENU_STRING,
	 * ettext("Subscribe"), #else MENU_STRING,		"Subscribe",
	 * #endif MENU_ACTION_PROC,	sb_subscribe_proc, NULL);
	 * 
	 * xv_set(sub_menu, MENU_APPEND_ITEM,	tmp_item, NULL);
	 * 
	 * tmp_item = xv_create(XV_NULL, MENUITEM, #ifdef OW_I18N MENU_STRING,
	 * ettext("unsubscribe"), #else MENU_STRING,
	 * #endif MENU_ACTION_PROC,	sb_unsubscribe_proc, NULL);
	 * 
	 * xv_set(sub_menu, MENU_APPEND_ITEM,	tmp_item, NULL);
	 * 
	 * tmp_item = xv_create(XV_NULL, MENUITEM, #ifndef NOT_IMPREMENT
	 * MENU_INACTIVE,		TRUE, #endif #ifdef OW_I18N
	 * MENU_PULLRIGHT_ITEM,	gettext("Subscribe"),	sub_menu, #else
	 * MENU_PULLRIGHT_ITEM,	"Subscribe",	sub_menu, #endif NULL);
	 */

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Latest..."),
#else
			     MENU_STRING, "Latest...",
#endif
			     MENU_NOTIFY_PROC, sb_popup_latest_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	/*
	 * tmp_item = xv_create(XV_NULL, MENUITEM, #ifdef OW_I18N
	 * MENU_STRING,		gettext("Rescan"), #else MENU_STRING,
	 * Rgscan", #endif MENU_NOTIFY_PROC,	sb_rescan_proc, NULL);
	 * 
	 * xv_set(menu, MENU_APPEND_ITEM,	tmp_item, NULL);
	 */

#if NOT_IMPREMENT
	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Status..."),
#else
			     MENU_STRING, "Status...",
#endif
			     MENU_NOTIFY_PROC, sb_status_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);
#endif

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Save changes"),
#else
			     MENU_STRING, "Save changes",
#endif
			     MENU_NOTIFY_PROC, sb_save_change_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	return menu;
}

static          Menu
make_open_pulldown()
{
	Menu            menu;
	Menu_item       tmp_item;

	menu = xv_create(XV_NULL, MENU,
			 NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Open"),
#else
			     MENU_STRING, "Open",
#endif
			     MENU_ACTION_PROC, sb_open_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Rescan"),
#else
			     MENU_STRING, "Rescan",
#endif
			     MENU_ACTION_PROC, sb_rescan_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	return menu;
}

static          Menu
make_article_pulldown()
{
	Menu            menu, sub_menu;
	Menu_item       tmp_item;


	menu = xv_create(XV_NULL, MENU,
#ifdef OW_I18N
			 MENU_GEN_PIN_WINDOW, sb_frame, gettext("Article"),
#else
			 MENU_GEN_PIN_WINDOW, sb_frame, "Article",
#endif
			 NULL);

	sub_menu = xv_create(XV_NULL, MENU,
			     NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("As read"),
#else
			     MENU_STRING, "As read",
#endif
			     MENU_ACTION_PROC, sb_article_asread_proc,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("As unread"),
#else
			     MENU_STRING, "As unread",
#endif
			     MENU_ACTION_PROC, sb_article_asunread_proc,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_PULLRIGHT_ITEM, gettext("Mark"), sub_menu,
#else
			     MENU_PULLRIGHT_ITEM, "Mark", sub_menu,
#endif
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	sub_menu = xv_create(XV_NULL, MENU,
			     NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Unread"),
#else
			     MENU_STRING, "Unread",
#endif
			     MENU_ACTION_PROC, sb_unreads_proc,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Read"),
#else
			     MENU_STRING, "Read",
#endif
			     MENU_ACTION_PROC, sb_reads_proc,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_PULLRIGHT_ITEM, gettext("View"), sub_menu,
#else
			     MENU_PULLRIGHT_ITEM, "View", sub_menu,
#endif
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Find..."),
#else
			     MENU_STRING, "Find...",
#endif
			     MENU_NOTIFY_PROC, make_find_frame,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Cancel..."),
#else
			     MENU_STRING, "Cancel...",
#endif
			     MENU_NOTIFY_PROC, make_cancel_frame,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Print"),
#else
			     MENU_STRING, "Print",
#endif
			     MENU_NOTIFY_PROC, sb_print_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	return menu;
}

static          Menu
make_view_pulldown()
{
	Menu            menu;
	Menu_item       tmp_item;


	menu = xv_create(XV_NULL, MENU,
#ifdef OW_I18N
			 MENU_GEN_PIN_WINDOW, sb_frame, gettext("View"),
#else
			 MENU_GEN_PIN_WINDOW, sb_frame, "View",
#endif
			 NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Next"),
#else
			     MENU_STRING, "Next",
#endif
			     MENU_NOTIFY_PROC, sb_next_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("View"),
#else
			     MENU_STRING, "View",
#endif
			     MENU_NOTIFY_PROC, sb_view_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Prev"),
#else
			     MENU_STRING, "Prev",
#endif
			     MENU_NOTIFY_PROC, sb_prev_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("MsgID"),
#else
			     MENU_STRING, "MsgID",
#endif
			     MENU_NOTIFY_PROC, sb_MsgID_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	return menu;

}

static          Menu
make_props_pulldown()
{
	Menu            menu;
	Menu_item       tmp_item;

	menu = xv_create(XV_NULL, MENU,
			 NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("tool properties..."),
#else
			     MENU_STRING, "tool properties...",
#endif
			     MENU_NOTIFY_PROC, make_tool_props_win,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("newsrc properties..."),
#else
			     MENU_STRING, "newsrc properties...",
#endif
			     MENU_NOTIFY_PROC, make_news_props_win,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("signature edit..."),
#else
			     MENU_STRING, "signature edit...",
#endif
			     MENU_NOTIFY_PROC, make_sign_props_win,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
	/*
	 * MENU_INACTIVE,		TRUE,
	 */
#ifdef OW_I18N
			     MENU_STRING, gettext("biff properties..."),
#else
			     MENU_STRING, "biff properties...",
#endif
			     MENU_NOTIFY_PROC, make_biff_props_win,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	return menu;
}

static          Menu
make_post_pulldown()
{
	Menu            menu, sub_menu;
	Menu_item       tmp_item;

	menu = xv_create(XV_NULL, MENU,
#ifdef OW_I18N
			 MENU_GEN_PIN_WINDOW, sb_frame, gettext("Compose"),
#else
			 MENU_GEN_PIN_WINDOW, sb_frame, "Compose",
#endif
			 NULL);

	sub_menu = xv_create(XV_NULL, MENU,
			     NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Post..."),
#else
			     MENU_STRING, "Post...",
#endif
			     MENU_ACTION_PROC, sb_post_proc,
			     XV_KEY_DATA, KIND_WIN, (char *) NEWS,
			     XV_KEY_DATA, COPOSE_TYPE, (char *) NEW_POST,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Bracketed post..."),
#else
			     MENU_STRING, "Bracketed post...",
#endif
			     MENU_ACTION_PROC, sb_post_proc,
			     XV_KEY_DATA, KIND_WIN, (char *) NEWS,
			  XV_KEY_DATA, COPOSE_TYPE, (char *) BRACKETED_POST,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Followup..."),
#else
			     MENU_STRING, "Followup...",
#endif
			     MENU_ACTION_PROC, sb_post_proc,
			     XV_KEY_DATA, KIND_WIN, (char *) NEWS,
			     XV_KEY_DATA, COPOSE_TYPE, (char *) REPLY_POST,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Indent followup..."),
#else
			     MENU_STRING, "Indent followup...",
#endif
			     MENU_ACTION_PROC, sb_post_proc,
			     XV_KEY_DATA, KIND_WIN, (char *) NEWS,
			     XV_KEY_DATA, COPOSE_TYPE, (char *) INDENT_POST,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_PULLRIGHT_ITEM, gettext("Post"), sub_menu,
#else
			     MENU_PULLRIGHT_ITEM, "Post", sub_menu,
#endif
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	sub_menu = xv_create(XV_NULL, MENU,
			     NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Forward..."),
#else
			     MENU_STRING, "Forward...",
#endif
			     MENU_ACTION_PROC, sb_post_proc,
			     XV_KEY_DATA, KIND_WIN, (char *) MAIL,
			     XV_KEY_DATA, COPOSE_TYPE, (char *) NEW_POST,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Bracketed forward..."),
#else
			     MENU_STRING, "Bracketed forward...",
#endif
			     MENU_ACTION_PROC, sb_post_proc,
			     XV_KEY_DATA, KIND_WIN, (char *) MAIL,
			  XV_KEY_DATA, COPOSE_TYPE, (char *) BRACKETED_POST,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Reply..."),
#else
			     MENU_STRING, "Reply...",
#endif
			     MENU_ACTION_PROC, sb_post_proc,
			     XV_KEY_DATA, KIND_WIN, (char *) MAIL,
			     XV_KEY_DATA, COPOSE_TYPE, (char *) REPLY_POST,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Indent reply..."),
#else
			     MENU_STRING, "Indent reply...",
#endif
			     MENU_ACTION_PROC, sb_post_proc,
			     XV_KEY_DATA, KIND_WIN, (char *) MAIL,
			     XV_KEY_DATA, COPOSE_TYPE, (char *) INDENT_POST,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_PULLRIGHT_ITEM, gettext("Mail"), sub_menu,
#else
			     MENU_PULLRIGHT_ITEM, "Mail", sub_menu,
#endif
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	return menu;
}

static          Menu
make_next_pulldown()
{
	Menu            menu;
	Menu_item       tmp_item;


	menu = xv_create(XV_NULL, MENU,
#ifdef OW_I18N
			 MENU_GEN_PIN_WINDOW, sb_frame, gettext("Next"),
#else
			 MENU_GEN_PIN_WINDOW, sb_frame, "Next",
#endif
			 NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Next"),
#else
			     MENU_STRING, "Next",
#endif
			     MENU_NOTIFY_PROC, sb_super_next_proc,
			     XV_KEY_DATA, SUPER_NEXT, (char *) NEXT,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Prev"),
#else
			     MENU_STRING, "Prev",
#endif
			     MENU_NOTIFY_PROC, sb_super_next_proc,
			     XV_KEY_DATA, SUPER_NEXT, (char *) PREV,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	return menu;

}

static          Menu
make_group_pulldown()
{
	Menu            menu, submenu;
	Menu_item       tmp_item;

	menu = xv_create(XV_NULL, MENU, NULL);

	submenu = xv_create(XV_NULL, MENU,
			    MENU_GEN_PROC, sb_group_select_menu_proc,
			    NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_PULLRIGHT_ITEM, gettext("File"), submenu,
#else
			     MENU_PULLRIGHT_ITEM, "File", submenu,
#endif
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	submenu = xv_create(XV_NULL, MENU,
			    MENU_GEN_PROC, sb_group_all_menu_proc,
			    NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_PULLRIGHT_ITEM, gettext("Server"), submenu,
#else
			     MENU_PULLRIGHT_ITEM, "Server", submenu,
#endif
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	return menu;
}

Menu
make_canvas_menu()
{

	Menu            menu, sub_menu;
	Menu_item       tmp_item;

	menu = xv_create(XV_NULL, MENU,
#ifdef OW_I18N
			 MENU_GEN_PIN_WINDOW, sb_frame, gettext("Message"),
#else
			 MENU_GEN_PIN_WINDOW, sb_frame, "Message",
#endif
			 NULL);

	sub_menu = xv_create(XV_NULL, MENU,
			     NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("View"),
#else
			     MENU_STRING, "View",
#endif
			     MENU_ACTION_PROC, sb_view_proc,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Next"),
#else
			     MENU_STRING, "Next",
#endif
			     MENU_ACTION_PROC, sb_next_proc,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Prev"),
#else
			     MENU_STRING, "Prev",
#endif
			     MENU_ACTION_PROC, sb_prev_proc,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_PULLRIGHT_ITEM, gettext("View"), sub_menu,
#else
			     MENU_PULLRIGHT_ITEM, "View", sub_menu,
#endif
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	sub_menu = xv_create(XV_NULL, MENU,
			     NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("As read"),
#else
			     MENU_STRING, "As read",
#endif
			     MENU_ACTION_PROC, sb_article_asread_proc,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("As unread"),
#else
			     MENU_STRING, "As unread",
#endif
			     MENU_ACTION_PROC, sb_article_asunread_proc,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_PULLRIGHT_ITEM, gettext("Mark"), sub_menu,
#else
			     MENU_PULLRIGHT_ITEM, "Mark", sub_menu,
#endif
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	sub_menu = xv_create(XV_NULL, MENU,
			     NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Next"),
#else
			     MENU_STRING, "Next",
#endif
			     MENU_ACTION_PROC, sb_next_subject_find_proc,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Prev"),
#else
			     MENU_STRING, "Prev",
#endif
			     MENU_ACTION_PROC, sb_prev_subject_find_proc,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("All"),
#else
			     MENU_STRING, "All",
#endif
			     MENU_ACTION_PROC, sb_all_subject_find_proc,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
		     MENU_PULLRIGHT_ITEM, gettext("Subject find"), sub_menu,
#else
			     MENU_PULLRIGHT_ITEM, "Subject find", sub_menu,
#endif
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	sub_menu = xv_create(XV_NULL, MENU,
			     NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Next"),
#else
			     MENU_STRING, "Next",
#endif
			     MENU_ACTION_PROC, sb_next_ng_proc,
			     XV_KEY_DATA, NEXT_NG, (char *) NEXT,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Prev"),
#else
			     MENU_STRING, "Prev",
#endif
			     MENU_ACTION_PROC, sb_next_ng_proc,
			     XV_KEY_DATA, NEXT_NG, (char *) PREV,
			     NULL);

	xv_set(sub_menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			  MENU_PULLRIGHT_ITEM, gettext("Next ng"), sub_menu,
#else
			     MENU_PULLRIGHT_ITEM, "Next ng", sub_menu,
#endif
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Print"),
#else
			     MENU_STRING, "Print",
#endif
			     MENU_ACTION_PROC, sb_print_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Save changes"),
#else
			     MENU_STRING, "Save changes",
#endif
			     MENU_NOTIFY_PROC, sb_save_change_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	return menu;
}

static void
make_find_frame()
{

	Panel           panel;
	Panel_item      item1, item2, item3;
	int             row = 0;
	Font            font;
	Font_string_dims dims;
	int             longest = 0;

	if (sb_find_frame) {
		/* set the selected message */
		(void) sb_set_find_txt();
		xv_set(sb_find_frame, XV_SHOW, TRUE, NULL);
		return;
	}
	if ((sb_find_frame = xv_create(sb_frame, FRAME_CMD,
#ifdef OW_I18N
				       WIN_USE_IM, FALSE,
#endif
#ifdef OW_I18N
				     FRAME_LABEL, gettext("Shinbun : Find"),
#else
				       FRAME_LABEL, "Shinbun : Find",
#endif
				       FRAME_CMD_PUSHPIN_IN, FALSE,
				       FRAME_SHOW_FOOTER, TRUE,
				       FRAME_SHOW_RESIZE_CORNER, FALSE,
				       NULL)) == NULL) {
#ifdef OW_I18N
		frame_err(sb_frame, gettext("Unable to create sb_find_frame"));
#else
		frame_err(sb_frame, "Unable to create sb_find_frame");
#endif
		return;
	}
	panel = xv_get(sb_find_frame, FRAME_CMD_PANEL);
	font = xv_get(panel, WIN_FONT);

#ifdef OW_I18N
	(void) xv_get(font, FONT_STRING_DIMS, gettext("Sender:"), &dims);
#else
	(void) xv_get(font, FONT_STRING_DIMS, "Sender:", &dims);
#endif
	if (dims.width > longest) {
		longest = dims.width;
	}
	sb_find_sender_txt = xv_create(panel, PANEL_TEXT,
				       XV_X, xv_col(panel, 1),
				       XV_Y, xv_row(panel, row++),
#ifdef OW_I18N
				     PANEL_LABEL_STRING, gettext("Sender:"),
#else
				       PANEL_LABEL_STRING, "Sender:",
#endif
				       PANEL_VALUE_DISPLAY_LENGTH, 50,
				       PANEL_VALUE_STORED_LENGTH, 150,
				       NULL);


#ifdef OW_I18N
	(void) xv_get(font, FONT_STRING_DIMS, gettext("Subject:"), &dims);
#else
	(void) xv_get(font, FONT_STRING_DIMS, "Subject:", &dims);
#endif
	if (dims.width > longest) {
		longest = dims.width;
	}
	sb_find_subject_txt = xv_create(panel, PANEL_TEXT,
					XV_Y, xv_row(panel, row++),
#ifdef OW_I18N
				    PANEL_LABEL_STRING, gettext("Subject:"),
#else
					PANEL_LABEL_STRING, "Subject:",
#endif
					PANEL_VALUE_DISPLAY_LENGTH, 50,
					PANEL_VALUE_STORED_LENGTH, 150,
					NULL);

	longest += 5;
	(void) xv_set(sb_find_sender_txt, PANEL_VALUE_X,
		      xv_col(panel, 2) + longest, NULL);
	(void) xv_set(sb_find_subject_txt, PANEL_VALUE_X,
		      xv_col(panel, 2) + longest, NULL);

	window_fit(panel);

	item1 = xv_create(panel, PANEL_BUTTON,
			  XV_Y, xv_row(panel, row),
#ifdef OW_I18N
			  PANEL_LABEL_STRING, gettext("Find Forward"),
#else
			  PANEL_LABEL_STRING, "Find Forward",
#endif
			  PANEL_NOTIFY_PROC, sb_find_proc,
			  XV_KEY_DATA, SUBJECT_NEXT_TYPE, (char *) FNEXT,
			  NULL);

	item2 = xv_create(panel, PANEL_BUTTON,
			  XV_Y, xv_row(panel, row),
#ifdef OW_I18N
			  PANEL_LABEL_STRING, gettext("Find Backward"),
#else
			  PANEL_LABEL_STRING, "Find Backward",
#endif
			  PANEL_NOTIFY_PROC, sb_find_proc,
			  XV_KEY_DATA, SUBJECT_NEXT_TYPE, (char *) FPREV,
			  NULL);

	item3 = xv_create(panel, PANEL_BUTTON,
			  XV_Y, xv_row(panel, row),
#ifdef OW_I18N
			  PANEL_LABEL_STRING, gettext("Select All"),
#else
			  PANEL_LABEL_STRING, "Select All",
#endif
			  PANEL_NOTIFY_PROC, sb_find_proc,
			  XV_KEY_DATA, SUBJECT_NEXT_TYPE, (char *) FALL,
			  NULL);

	sb_center_items(panel, row, item1, item2, item3, NULL);
	window_fit(panel);
	window_fit(sb_find_frame);

	/* set the selected message */
	(void) sb_set_find_txt();
	xv_set(sb_find_frame, XV_SHOW, TRUE, NULL);
}

void
make_cancel_frame()
{

	Panel           panel;
	Panel_item      item;
	Font            font;
	Font_string_dims dims;
	int             longest = 0;
	int             row = 0;
	extern char    *sb_cancel_proc();
	extern void     sb_set_cancel_txt();

	if (sb_cancel_frame) {
		/* set the selected message */
		(void) sb_set_cancel_txt();
		xv_set(sb_cancel_frame, XV_SHOW, TRUE, NULL);
		return;
	}
	if ((sb_cancel_frame = xv_create(sb_frame, FRAME_CMD,
#ifdef OW_I18N
					 WIN_USE_IM, FALSE,
#endif
#ifdef OW_I18N
				    FRAME_LABEL, gettext("Shinbun: Cancel"),
#else
					 FRAME_LABEL, "Shinbun: Cancel",
#endif
					 FRAME_CMD_PUSHPIN_IN, FALSE,
					 FRAME_SHOW_FOOTER, TRUE,
					 FRAME_SHOW_RESIZE_CORNER, FALSE,
					 NULL)) == NULL) {
#ifdef OW_I18N
		frame_err(sb_frame, gettext("Unable to create sb_cancel_frame"));
#else
		frame_err(sb_frame, "Unable to create sb_cancel_frame");
#endif
		return;
	}
	panel = xv_get(sb_cancel_frame, FRAME_CMD_PANEL);
	font = xv_get(panel, WIN_FONT);

#ifdef OW_I18N
	(void) xv_get(font, FONT_STRING_DIMS, gettext("Newsgroup:"), &dims);
#else
	(void) xv_get(font, FONT_STRING_DIMS, "Newsgroup:", &dims);
#endif
	if (dims.width > longest) {
		longest = dims.width;
	}
	sb_cancel_group_txt = xv_create(panel, PANEL_TEXT,
					XV_Y, xv_row(panel, row++),
#ifdef OW_I18N
				  PANEL_LABEL_STRING, gettext("Newsgroup:"),
#else
					PANEL_LABEL_STRING, "Newsgroup:",
#endif
					PANEL_VALUE_DISPLAY_LENGTH, 20,
					PANEL_VALUE_STORED_LENGTH, 100,
					NULL);

#ifdef OW_I18N
	(void) xv_get(font, FONT_STRING_DIMS, gettext("Article number:"), &dims);
#else
	(void) xv_get(font, FONT_STRING_DIMS, "Article number:", &dims);
#endif
	if (dims.width > longest) {
		longest = dims.width;
	}
	sb_cancel_newsno_txt = xv_create(panel, PANEL_TEXT,
					 XV_Y, xv_row(panel, row++),
#ifdef OW_I18N
			     PANEL_LABEL_STRING, gettext("Article number:"),
#else
				      PANEL_LABEL_STRING, "Article number:",
#endif
					 PANEL_NOTIFY_PROC, sb_cancel_proc,
					 PANEL_VALUE_DISPLAY_LENGTH, 20,
					 PANEL_VALUE_STORED_LENGTH, 100,
					 NULL);
	longest += 5;
	(void) xv_set(sb_cancel_group_txt, PANEL_VALUE_X,
		      xv_col(panel, 2) + longest, NULL);
	(void) xv_set(sb_cancel_newsno_txt, PANEL_VALUE_X,
		      xv_col(panel, 2) + longest, NULL);

	window_fit(panel);
	item = xv_create(panel, PANEL_BUTTON,
			 XV_Y, xv_row(panel, row),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Cancel"),
#else
			 PANEL_LABEL_STRING, "Cancel",
#endif
			 PANEL_NOTIFY_PROC, sb_cancel_proc,
			 NULL);

	sb_center_items(panel, row, item, NULL);
	window_fit(panel);
	window_fit(sb_cancel_frame);

	/* set the selected message */
	(void) sb_set_cancel_txt();
	xv_set(sb_cancel_frame, XV_SHOW, TRUE, NULL);
}

/*
 * Create or pop-up the newsgroup of the new article arrival. We should
 * create this frame at first. If frame_flag != 0, this is only created and
 * not shown.
 */
int
make_newsbiff_frame()
{
	Panel           panel;
	Panel_item      item;
	int             x, y;
	static Menu     make_open_pulldown();

	x = 0;
	y = 0;

	if (sb_newsbiff_frame) {
		if ((int) xv_get(sb_newsbiff_frame, XV_SHOW) != TRUE) {
			pop_up_beside(sb_frame, sb_newsbiff_frame);
			xv_set(sb_newsbiff_frame, XV_SHOW, TRUE, NULL);
		}
		return;
	}
	if ((sb_newsbiff_frame = xv_create(sb_frame, FRAME_CMD,
#ifdef OW_I18N
					   WIN_USE_IM, FALSE,
#endif
#ifdef OW_I18N
				 FRAME_LABEL, gettext("Shinbun: News biff"),
#else
					   FRAME_LABEL, "Shinbun: News biff",
#endif
					   FRAME_CMD_PUSHPIN_IN, TRUE,
					   FRAME_SHOW_FOOTER, FALSE,
					   FRAME_SHOW_RESIZE_CORNER, FALSE,
					   NULL)) == NULL) {
#ifdef OW_I18N
		frame_err(sb_frame,
			  gettext("Unable to create sb_newsbiff_frame"));
#else
		frame_err(sb_frame, "Unable to create sb_newsbiff_frame");
#endif
		return;
	}
	panel = xv_get(sb_newsbiff_frame, FRAME_CMD_PANEL);

	xv_set(panel, PANEL_LAYOUT, PANEL_VERTICAL, NULL);
	sb_newsbiff_panel = xv_create(panel, PANEL_LIST,
				      XV_X, xv_col(panel, 0),
				      XV_Y, xv_row(panel, 0),
				      PANEL_LIST_WIDTH, 250,
				      PANEL_LIST_DISPLAY_ROWS, 9,
				      PANEL_READ_ONLY, TRUE,
				      PANEL_CHOOSE_ONE, TRUE,
				      PANEL_NOTIFY_PROC, sb_biff_panel_proc,
				      NULL);

	item = xv_create(panel, PANEL_BUTTON,
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Open"),
#else
			 PANEL_LABEL_STRING, "Open",
#endif

			 PANEL_ITEM_MENU, make_open_pulldown(),
			 NULL);

	window_fit(panel);
	sb_center_items(panel, -1, item, NULL);

	window_fit(panel);
	window_fit(sb_newsbiff_frame);

	xv_set(sb_newsbiff_frame, XV_SHOW, FALSE, NULL);
}
