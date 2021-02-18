#ifdef sccs
static char     sccsid[] = "@(#)sb_biff_props.c	1.22 91/09/12";
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

#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/font.h>
#include <xview/cursor.h>

#include "sb_tool.h"
#include "sb_shinbun.h"
#include "shinbun.h"
#include "config.h"

extern Frame    sb_frame;
extern int      num_of_nbiffgrp;

static Frame    sb_props_frame;
static Panel_item p_list_1;
static Panel_item p_list_2;
static Panel_item p_find_txt;

static int      props_changed;

static void     get_newsrc_file();
static void     get_biff_file();

extern char     NEWSBIFFRC[];
extern char    *newsbiffrc[];

/*
 * Make Biff Props Window.
 */
void
make_biff_props_win()
{
	Panel           panel;
	Panel_item      item1;
	Panel_item      item2;
	Menu            menu;
	Font            font;
	int             row;
	int             length;
	static void     apply_proc();
	static void     reset_proc();
	static Menu     make_list_menu();
	static void     props_frame_destroy_proc();
	static void     pinout_proc();

	if (sb_props_frame) {
		(void) xv_set(sb_props_frame, WIN_FRONT, XV_SHOW, TRUE, NULL);
		return;
	}
	if ((sb_props_frame = xv_create(sb_frame, FRAME_CMD,
#ifdef OW_I18N
					WIN_USE_IM, FALSE,
#endif
#ifdef OW_I18N
			      FRAME_LABEL, gettext("Properties: .newsbiff"),
#else
					FRAME_LABEL, "Properties: .newsbiff",
#endif
					FRAME_CMD_PUSHPIN_IN, TRUE,
					FRAME_SHOW_FOOTER, TRUE,
					FRAME_SHOW_RESIZE_CORNER, FALSE,
					NULL)) == NULL) {
#ifdef OW_I18N
		frame_err(sb_frame, gettext("Unable to create sb_props_frame"));
#else
		frame_err(sb_frame, "Unable to create sb_props_frame");
#endif
		return;
	}
	xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	panel = xv_get(sb_props_frame, FRAME_CMD_PANEL);
	font = (int) xv_get(panel, WIN_FONT);

	row = 0;

	length = 30 * (int) xv_get(font, FONT_DEFAULT_CHAR_WIDTH);
	p_list_1 = xv_create(panel, PANEL_LIST,
			     XV_X, xv_col(panel, 1),
			     XV_Y, xv_row(panel, row),
			     PANEL_LIST_WIDTH, length,
			     PANEL_LIST_DISPLAY_ROWS, 25,
			     PANEL_LABEL_STRING, NEWSBIFFRC,
			     PANEL_LAYOUT, PANEL_VERTICAL,
			     PANEL_READ_ONLY, TRUE,
			     PANEL_CHOOSE_ONE, FALSE,
			     NULL);

	p_list_2 = xv_create(panel, PANEL_LIST,
			     XV_X, xv_col(panel, 35),
			     XV_Y, xv_row(panel, row),
			     PANEL_LIST_WIDTH, length,
			     PANEL_LIST_DISPLAY_ROWS, 23,
			     PANEL_LABEL_STRING, NEWSRC_FLIST_NAM,
			     PANEL_LAYOUT, PANEL_VERTICAL,
			     PANEL_READ_ONLY, TRUE,
			     PANEL_CHOOSE_ONE, FALSE,
			     NULL);

	xv_set(panel, PANEL_LAYOUT, PANEL_HORIZONTAL, NULL);
	row += 16;
	(void) xv_create(panel, PANEL_BUTTON,
			 XV_X, xv_col(panel, 35),
			 XV_Y, xv_row(panel, row),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Templete"),
#else
			 PANEL_LABEL_STRING, "Templete",
#endif
			 PANEL_ITEM_MENU, menu = make_list_menu(),
			 NULL);

	p_find_txt = xv_create(panel, PANEL_TEXT,
			       XV_Y, xv_row(panel, row++),
			       PANEL_VALUE_DISPLAY_LENGTH, 10,
			       PANEL_VALUE_STORED_LENGTH, 50,
			       NULL);

	window_fit(panel);

	row += 1;
	item1 = xv_create(panel, PANEL_BUTTON,
			  XV_Y, xv_row(panel, row),
#ifdef OW_I18N
			  PANEL_LABEL_STRING, gettext("Apply"),
#else
			  PANEL_LABEL_STRING, "Apply",
#endif
			  PANEL_NOTIFY_PROC, apply_proc,
			  NULL);

	item2 = xv_create(panel, PANEL_BUTTON,
			  XV_Y, xv_row(panel, row),
#ifdef OW_I18N
			  PANEL_LABEL_STRING, gettext("Reset"),
#else
			  PANEL_LABEL_STRING, "Reset",
#endif
			  PANEL_NOTIFY_PROC, reset_proc,
			  NULL);

	sb_center_items(panel, row, item1, item2, NULL);
	window_fit(panel);
	window_fit(sb_props_frame);

	props_changed = FALSE;

	notify_interpose_destroy_func(sb_props_frame,
				      props_frame_destroy_proc);

	notify_interpose_event_func(sb_props_frame,
				    pinout_proc, NOTIFY_SAFE);

	get_newsrc_file();
	get_biff_file();

	xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);

	xv_set(sb_props_frame, XV_SHOW, TRUE, 0);

}

static          Notify_value
props_frame_destroy_proc(client, status)
	Notify_client   client;
	Destroy_status  status;
{
	Notify_client   tmp = client;

	xv_set(tmp, FRAME_NO_CONFIRM, TRUE, NULL);

	sb_props_frame = NULL;

	return notify_next_destroy_func(tmp, status);
}

static          Notify_value
pinout_proc(client, event, arg, when)
	Notify_client   client;
	Event          *event;
	Notify_arg      arg;
	Notify_event_type when;
{
	Notify_value    return_value =
	notify_next_event_func(client, event, arg, NOTIFY_SAFE);
	static void     apply_proc();

	if (event_action(event) == ACTION_PINOUT) {
		if (props_changed == TRUE) {
#ifdef OW_I18N
			if (two_notice((Frame) client, gettext("Apply"), gettext("Quit"),
				     gettext("Properties has been changed"),
				       gettext("Applied or Not ?"),
				       NULL))
#else
			if (two_notice((Frame) client, "Apply", "Quit",
				       "Properties has been changed",
				       "Applied or Not ?",
				       NULL))
#endif
				apply_proc(NULL, NULL);
		}
		xv_destroy((Frame) client);
	}
	return return_value;
}

/*
 * Make menu for biff window.
 */
static          Menu
make_list_menu()
{
	Menu            menu;
	Menu_item       tmp_item;
	static void     p_add_after_proc();
	static void     p_add_before_proc();
	static void     p_delete_proc();
	static void     p_insert_first_proc();
	static void     p_insert_last_proc();
	static void     p_find_proc();

	menu = xv_create(XV_NULL, MENU,
#ifdef OW_I18N
		   MENU_GEN_PIN_WINDOW, sb_props_frame, gettext("Templete"),
#else
			 MENU_GEN_PIN_WINDOW, sb_props_frame, "Templete",
#endif
			 NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Add After"),
#else
			     MENU_STRING, "Add After",
#endif
			     MENU_NOTIFY_PROC, p_add_after_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Add Before"),
#else
			     MENU_STRING, "Add Bgfore",
#endif
			     MENU_NOTIFY_PROC, p_add_before_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Insert first"),
#else
			     MENU_STRING, "Insert first",
#endif
			     MENU_NOTIFY_PROC, p_insert_first_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Insert last"),
#else
			     MENU_STRING, "Insert last",
#endif
			     MENU_NOTIFY_PROC, p_insert_last_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Find"),
#else
			     MENU_STRING, "Find",
#endif
			     MENU_NOTIFY_PROC, p_find_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Delete"),
#else
			     MENU_STRING, "Delete",
#endif
			     MENU_NOTIFY_PROC, p_delete_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	return menu;
}

/*
 * Add after for biff panel list.
 */
static void
p_add_after_proc(menu, item)
	Menu            menu;
	Menu_item       item;
{
	int             tmp_cnt = (int) xv_get(p_list_1, PANEL_LIST_NROWS);
	int             cnt = (int) xv_get(p_list_2, PANEL_LIST_NROWS);
	int             i;
	int             non_select_cnt;

	xv_set(sb_props_frame, FRAME_BUSY, TRUE, NULL);

	for (i = 0, non_select_cnt = 0; i < tmp_cnt; i++)
		if (!xv_get(p_list_1, PANEL_LIST_SELECTED, i))
			non_select_cnt++;

	if ((tmp_cnt != 0) && (non_select_cnt == tmp_cnt)) {
		one_notice(sb_props_frame,
#ifdef OW_I18N
		       gettext("Not selected the list in your newsrc file"),
#else
			   "Not selected the list in your newsrc file",
#endif
			   NULL);

		xv_set(sb_props_frame, FRAME_BUSY, FALSE, NULL);
		return;
	}
	if (tmp_cnt >= MAX_OBSERVED_GROUPS) {
		one_notice(sb_props_frame,
#ifdef OW_I18N
			   gettext("Can not observe newsgroups any more !"),
#else
			   "Can not observe newsgroups any more !",
#endif
			   NULL);

		xv_set(sb_props_frame, FRAME_BUSY, FALSE, NULL);
		return;
	}
	for (i = 0; i < cnt; i++) {
		if (xv_get(p_list_2, PANEL_LIST_SELECTED, i)) {
			if (!add_after_proc(p_list_1,
			    (char *) xv_get(p_list_2, PANEL_LIST_STRING, i),
					    sb_props_frame)) {
				xv_set(p_list_2,
				       PANEL_LIST_SELECT, i, FALSE,
				       NULL);
			}
		}
	}
	list_delete(p_list_2);

	props_changed = TRUE;

	xv_set(sb_props_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * Add before for biff panel list.
 */
static void
p_add_before_proc(menu, item)
	Menu            menu;
	Menu_item       item;
{
	int             tmp_cnt = (int) xv_get(p_list_1, PANEL_LIST_NROWS);
	int             cnt = (int) xv_get(p_list_2, PANEL_LIST_NROWS);
	int             i;
	int             non_select_cnt;

	xv_set(sb_props_frame, FRAME_BUSY, TRUE, NULL);

	for (i = 0, non_select_cnt = 0; i < tmp_cnt; i++)
		if (!xv_get(p_list_1, PANEL_LIST_SELECTED, i))
			non_select_cnt++;

	if ((tmp_cnt != 0) && (non_select_cnt == tmp_cnt)) {
		one_notice(sb_props_frame,
#ifdef OW_I18N
		       gettext("Not selected the list in your newsrc file"),
#else
			   "Not selected the list in your newsrc file",
#endif
			   NULL);
		xv_set(sb_props_frame, FRAME_BUSY, FALSE, NULL);
		return;
	}
	if (tmp_cnt >= MAX_OBSERVED_GROUPS) {
		one_notice(sb_props_frame,
#ifdef OW_I18N
			   gettext("Can not observe newsgroups any more !"),
#else
			   "Can not observe newsgroups any more !",
#endif
			   NULL);

		xv_set(sb_props_frame, FRAME_BUSY, FALSE, NULL);
		return;
	}
	for (i = 0; i < cnt; i++) {
		if (xv_get(p_list_2, PANEL_LIST_SELECTED, i)) {
			if (!add_before_proc(p_list_1,
			    (char *) xv_get(p_list_2, PANEL_LIST_STRING, i),
					     sb_props_frame)) {
				xv_set(p_list_2,
				       PANEL_LIST_SELECT, i, FALSE,
				       NULL);
			}
		}
	}
	list_delete(p_list_2);

	props_changed = TRUE;

	xv_set(sb_props_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * Delete for biff panel list.
 */
static void
p_delete_proc(menu, item)
	Menu            menu;
	Menu_item       item;
{
	xv_set(sb_props_frame, FRAME_BUSY, TRUE, NULL);

	(void) list_delete(p_list_1);

	props_changed = TRUE;

	xv_set(sb_props_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * Insert the selected news group to the newsrc panel list first.
 */
static void
p_insert_first_proc(menu, item)
	Menu            menu;
	Menu_item       item;
{
	int             cnt = (int) xv_get(p_list_2, PANEL_LIST_NROWS);
	int             i;
	char            tmp[256];

	xv_set(sb_props_frame, FRAME_BUSY, TRUE, NULL);

	if ((int) xv_get(p_list_1, PANEL_LIST_NROWS) >= MAX_OBSERVED_GROUPS) {
		one_notice(sb_props_frame,
#ifdef OW_I18N
			   gettext("Can not observe newsgroups any more !"),
#else
			   "Can not observe newsgroups any more !",
#endif
			   NULL);

		xv_set(sb_props_frame, FRAME_BUSY, FALSE, NULL);
		return;
	}
	for (i = cnt - 1; i >= 0; i--)
		if (xv_get(p_list_2, PANEL_LIST_SELECTED, i)) {
			strcpy(tmp, (char *) xv_get(p_list_2,
						    PANEL_LIST_STRING, i));

			if ((int) list_find_dup(p_list_1, tmp) == FALSE) {
#ifdef OW_I18N
				sprintf(tmp, gettext("%s is already existed."), tmp);
#else
				sprintf(tmp, "%s is already existed.", tmp);
#endif
				(void) one_notice(sb_props_frame, tmp, NULL);

				continue;
			}
			xv_set(p_list_2,
			       PANEL_LIST_DELETE, i,
			       NULL);

			list_insert(p_list_1, tmp, 0);
			i = (int) xv_get(p_list_2, PANEL_LIST_NROWS);
		}
	props_changed = TRUE;

	xv_set(sb_props_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * Insert the selected news group to the newsrc panel list last.
 */
static void
p_insert_last_proc(menu, item)
	Menu            menu;
	Menu_item       item;
{
	int             cnt = (int) xv_get(p_list_2, PANEL_LIST_NROWS);
	int             i;
	char            tmp[256];

	xv_set(sb_props_frame, FRAME_BUSY, TRUE, NULL);

	if ((int) xv_get(p_list_1, PANEL_LIST_NROWS) >= MAX_OBSERVED_GROUPS) {
		one_notice(sb_props_frame,
#ifdef OW_I18N
			   gettext("Can not observe newsgroups any more !"),
#else
			   "Can not observe newsgroups any more !",
#endif
			   NULL);

		xv_set(sb_props_frame, FRAME_BUSY, FALSE, NULL);
		return;
	}
	for (i = cnt - 1; i >= 0; i--)
		if (xv_get(p_list_2, PANEL_LIST_SELECTED, i)) {
			strcpy(tmp, (char *) xv_get(p_list_2,
						    PANEL_LIST_STRING, i));

			if ((int) list_find_dup(p_list_1, tmp) == FALSE) {
#ifdef OW_I18N
				sprintf(tmp, gettext("%s is already existed."), tmp);
#else
				sprintf(tmp, "%s is already existed.", tmp);
#endif
				(void) one_notice(sb_props_frame, tmp, NULL);

				continue;
			}
			xv_set(p_list_2,
			       PANEL_LIST_DELETE, i,
			       NULL);
			cnt = (int) xv_get(p_list_1, PANEL_LIST_NROWS);
			list_insert(p_list_1, tmp, cnt);
			i = (int) xv_get(p_list_2, PANEL_LIST_NROWS);
		}
	props_changed = TRUE;

	xv_set(sb_props_frame, FRAME_BUSY, FALSE, NULL);
}

static void
p_find_proc(menu, item)
	Menu            menu;
	Menu_item       item;
{
	char           *find = (char *) xv_get(p_find_txt, PANEL_VALUE);
	char            buf[256];
	int             cnt = (int) xv_get(p_list_2, PANEL_LIST_NROWS);
	int             found;
	register int    i;

	if (*find == '\0' || *find == '\n')
		return;

	xv_set(sb_props_frame, FRAME_BUSY, TRUE, NULL);

	for (i = 0, found = 0; i < cnt; i++)
		if (strstr((char *) xv_get(p_list_2,
					   PANEL_LIST_STRING, i), find)) {
			xv_set(p_list_2,
			       PANEL_LIST_SELECT, i, TRUE,
			       NULL);
			found++;
		} else
			xv_set(p_list_2,
			       PANEL_LIST_SELECT, i, FALSE,
			       NULL);

	if (found)
		sprintf(buf, "%d newsgroup(s) found", found);
	else
		strcpy(buf, "Not match");

	frame_msg(sb_props_frame, buf);
	xv_set(sb_props_frame, FRAME_BUSY, FALSE, NULL);
}

static void
get_biff_file()
{
	char            buf[256];
	char           *tmp;
	int             i, j;

	xv_set(sb_props_frame, FRAME_BUSY, TRUE, NULL);
	xv_set(p_list_1, XV_SHOW, FALSE, NULL);

	i = j = 0;
	while (i < num_of_nbiffgrp) {
		strncpy(buf, newsbiffrc[i], 50);
		tmp = (char *) rm_colon(buf);
		(void) list_insert(p_list_1, tmp, j++);
		i++;
	}

	xv_set(p_list_1, XV_SHOW, TRUE, NULL);
	xv_set(sb_props_frame, FRAME_BUSY, FALSE, NULL);
}

static void
get_newsrc_file()
{
	char            buf[256];
	int             i, j;

	xv_set(sb_props_frame, FRAME_BUSY, TRUE, NULL);
	xv_set(p_list_2, XV_SHOW, FALSE, NULL);

	i = j = 0;
	while (i < num_of_ngrp) {
		strncpy(buf, return_newsrc_data(i), 255);
		if ((strstr(buf, ":")) != NULL) {
			(void) list_insert(p_list_2,
					   (char *) rm_colon(buf), j++);
		}
		i++;
	}

	xv_set(p_list_2, XV_SHOW, TRUE, NULL);
	xv_set(sb_props_frame, FRAME_BUSY, FALSE, NULL);

	xv_set(p_list_2, PANEL_LABEL_STRING, NEWSRC_FLIST_NAM, NULL);
}

static void
reset_proc(item, event)
	Panel_item      item;
	Event          *event;
{

	list_all_delete(p_list_1);
	get_biff_file();

	props_changed = FALSE;

	list_all_delete(p_list_2);
	get_newsrc_file();
}

static void
apply_proc(item, event)
	Panel_item      item;
	Event          *event;
{
	int             cnt = xv_get(p_list_1, PANEL_LIST_NROWS);
	char            buf[256];
	int             i;
	FILE           *fp;

	if ((fp = fopen(NEWSBIFFRC, "w")) == NULL) {
		return;
	}
	xv_set(sb_props_frame, FRAME_BUSY, TRUE, NULL);

	for (i = 0; i < cnt; i++) {
		rm_return((char *) xv_get(p_list_1, PANEL_LIST_STRING, i), buf);
		if (buf == NULL) {
			continue;
		} else if (i >= MAX_OBSERVED_GROUPS) {
			sprintf(buf,
#ifdef OW_I18N
				gettext("is %d. Newsgroups until now have been saved."),
#else
			     "is %d. Newsgroups until now have been saved.",
#endif
				i);

			one_notice(sb_props_frame,
#ifdef OW_I18N
				   gettext("Number of maximum newsgroups observed by news biff"),
				   buf,
#else
			"Number of maximum newsgroups observed by news biff",
				   buf,
#endif
				   NULL);

			break;
		} else {
			fprintf(fp, "%s:\n", buf);
		}
	}
	fclose(fp);

	props_changed = FALSE;

	clear_newsbiffrc();
	read_newsbiffrc();

#ifdef OW_I18N
	frame_msg(sb_props_frame, gettext("Changes has been applied."));
#else
	frame_msg(sb_props_frame, "Changes has been applied.");
#endif

	xv_set(sb_props_frame, FRAME_BUSY, FALSE, NULL);

}
