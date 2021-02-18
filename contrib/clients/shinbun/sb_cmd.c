#ifdef sccs
static char     sccsid[] = "@(#)sb_cmd.c	1.34 91/09/13";
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
#include <stdlib.h>
#include <string.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/text.h>
#include <xview/seln.h>
#include <xview/notice.h>
#include <xview/cursor.h>
#include <xview/scrollbar.h>
#include <xview/canvas.h>
#include <xview/seln.h>

#include "sb_tool.h"
#include "sb_ui.h"
#include "sb_def.h"
#include "sb_item.h"
#include "sb_shinbun.h"
#include "shinbun.h"
#include "config.h"

#define SCROLL_WRAP_LINE 	10
#define MAXNEWSSTR 32

int             sb_canceled_flag = FALSE;

extern int      sb_latest_view_line;
extern Menu     sb_select_group_menu;
extern Menu     sb_all_group_menu;
extern int      sb_num_of_article;

/*
 * Query the newsgroup from the NNTP server. This is used for sb_open_proc &
 * sb_group_txt_proc.
 */
int
sb_query_group(group_name, article_flag)
	char           *group_name;
	int             article_flag;
{

	if (article_flag) {
		if ((int) get_subject_file(group_name) == -1)
			return FALSE;
	} else {
		if ((int) get_subject_file_unread(group_name) == -1)
			return FALSE;
	}
	return TRUE;
}

/*
 * Find the next or prev newsgroup.
 */
char           *
sb_find_next_ng_func(newsgroup, flag)
	char           *newsgroup;
	Sb_next_type    flag;
{
	int             num;
	char           *nextnewsgroup;
	char            msg[MAXSTRLEN];

	if ((num = search_ngrp(newsgroup)) == -1) {
		/*
		 * If can not get the current ng number.
		 */
		(void) sb_view_first_unselect_proc(newsgroup);
		return NULL;
	}
	/*
	 * Display the next group. If nothing next unreadble newsgroup, query
	 * the server.
	 */
	if (flag == NEXT)
		(void) frame_msg(sb_frame,
#ifdef OW_I18N
				 (char *) gettext("Query the next news group from the server..."));
#else
			    "Query the next news group from the server...");
#endif
	if (flag == PREV)
		(void) frame_msg(sb_frame,
#ifdef OW_I18N
				 (char *) gettext("Query the previous news group from the server..."));
#else
			"Query the previous news group from the server...");
#endif

	if (flag == NEXT) {
		num++;
		if (num == num_of_ngrp) {
			(void) one_notice(sb_frame,
#ifdef OW_I18N
				gettext("Can not find the next newsgroup."),
#else
					  "Can not find the next newsgroup.",
#endif
					  NULL);
			(void) frame_msg(sb_frame, "");
			xv_set(sb_group_txt, PANEL_VALUE, "", NULL);
			return NULL;
		}
	} else if (flag == PREV) {
		num--;
		if (num < 0) {
			(void) one_notice(sb_frame,
#ifdef OW_I18N
			    gettext("Can not find the previous newsgroup."),
#else
				     "Can not find the previous newsgroup.",
#endif
					  NULL);
			(void) frame_msg(sb_frame, "");
			xv_set(sb_group_txt, PANEL_VALUE, "", NULL);
			return NULL;
		}
	}
	if ((int) sb_get_first_diplay_group(num,
	     (int) sb_get_props_value(Sb_article_display), flag) == FALSE) {
		/*
		 * Can not the next/prev newsgroup. Query the server for the
		 * current group.
		 */
		/* This is BAD work arround. */
		xv_set(sb_group_txt, PANEL_VALUE, newsgroup, NULL);
		(void) sb_rescan_proc(NULL, NULL);

		if ((nextnewsgroup = (char *) get_current_newsgroup()) == NULL) {
			(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				       "Can not get the current newsgroup.",
#endif
					  NULL);
			return NULL;
		}
		(void) xv_set(sb_group_txt, PANEL_VALUE, nextnewsgroup, NULL);

		(void) sb_view_first_unselect_proc(nextnewsgroup);

#ifdef OW_I18N
		sprintf(msg, gettext("Rescan the %s group instead"), nextnewsgroup);
#else
		sprintf(msg, "Rescan the %s group instead", nextnewsgroup);
#endif

		(void) one_notice(sb_frame,
#ifdef OW_I18N
		       gettext("Can not find the next/previous newsgroup."),
#else
				"Can not find the next/previous newsgroup.",
#endif
				  msg,
				  NULL);

	} else {
		if ((nextnewsgroup = (char *) get_current_newsgroup()) == NULL) {
			(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				       "Can not get the current newsgroup.",
#endif
					  NULL);
			return NULL;
		}
		(void) sb_canvas_force_repaint_proc(nextnewsgroup);
	}

	if (flag == NEXT)
		(void) frame_msg(sb_frame,
#ifdef OW_I18N
				 gettext("Query the next news group from the server... Done"));
#else
		       "Query the next news group from the server... Done");
#endif
	if (flag == PREV)
		(void) frame_msg(sb_frame,
#ifdef OW_I18N
				 gettext("Query the previous news group from the server... Done"));
#else
		   "Query the previous news group from the server... Done");
#endif

	return nextnewsgroup;
}


/*
 * Used for double click view.
 */
void
sb_click_view(newsgroup, line)
	char           *newsgroup;
	int             line;
{
	int             num;

	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	if ((int) change_line_to_isselect(newsgroup, line, AT_SELECT) == -1)
		(void) sb_selected_check_error(line);
	else {
		num = (int) change_line_to_anum(newsgroup, line);
		if (num == -1) {
			(void) one_notice(sb_frame,
#ifdef OW_I18N
				 gettext("Can not get the article number."),
#else
					  "Can not get the article number.",
#endif
					  NULL);
			return;
		}
		if (!sb_make_article_win(newsgroup, num, line, VSELECT)) {
			(void) one_notice(sb_frame,
#ifdef OW_I18N
				gettext("Can not create the article view."),
#else
					  "Can not create the article view.",
#endif
					  NULL);
		}
	}

	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * View func used in View menu.
 */
void				/* VARARGS */
sb_view_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	register int    i, j, k;
	int             num, newsnum;
	char           *newsgroup;

	if ((newsgroup = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	num = 0;

	for (i = 1; i <= sb_num_of_article; i++) {
		j = change_line_to_isselect(newsgroup, i, AT_SELECT);
		k = change_line_to_isselect(newsgroup, i, AT_SELECT_MANY);
		if (j == -1 || k == -1) {
			(void) sb_selected_check_error(i);
			continue;
		} else if (j > 0) {
			newsnum = (int) change_line_to_anum(newsgroup, i);
			if (newsnum == -1) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				 gettext("Can not get the article number."),
#else
					  "Can not get the article number.",
#endif
						  NULL);
				return;
			}
			if (!sb_make_article_win(newsgroup, newsnum, i,
						 VSELECT)) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				gettext("Can not create the article view."),
#else
					 "Can not create the article view.",
#endif
						  NULL);
				return;
			}
			num++;
		} else if (k > 0) {
			newsnum = (int) change_line_to_anum(newsgroup, i);
			if (newsnum == -1) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				 gettext("Can not get the article number."),
#else
					  "Can not get the article number.",
#endif
						  NULL);
				return;
			}
			if (!sb_make_article_win(newsgroup, newsnum, i,
						 VADJUST)) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				gettext("Can not create the article view."),
#else
					 "Can not create the article view.",
#endif
						  NULL);
				return;
			}
			num++;
		}
	}
	/*
	 * If not selected, show the first unread article.
	 */
	if (num == 0)
		(void) sb_view_first_unselect_proc(newsgroup);

	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * Next func used in Next menu.
 */
void
sb_next_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	char           *newsgroup;

	if ((newsgroup = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	if (sb_next_select_article_func(newsgroup, NEXT) == FALSE)
		(void) sb_view_first_unselect_proc(newsgroup);

	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * Prev func used in Prev menu.
 */
void
sb_prev_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	char           *newsgroup;

	if ((newsgroup = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	if (sb_next_select_article_func(newsgroup, PREV) == FALSE)
		(void) sb_view_last_unselect_proc(newsgroup);

	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * MsgID func used in Prev menu.
 */
void
sb_MsgID_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	VIEW_DATA      *view;
	char           *seln;

	view = view_first_ptr;

	if (view == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
				  gettext("Select Message ID."),
#else
				  "Select Message ID.",
#endif
				  NULL);
		return;
	}
	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	while (view) {
		if ((seln = get_text_seln(view->textsw)) != NULL) {
			sb_view_msgid(seln);
		}
		view = view->next;
	}

	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * Query the ng from the server used in Open menu.
 */
void
sb_open_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	char           *newsgroup = (char *) xv_get(sb_group_txt, PANEL_VALUE);

	if (*newsgroup == '\0' || *newsgroup == '\n') {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	if (change_grp_to_isnewarticle(newsgroup) > 0) {
		sb_rescan_proc(menu, menu_item);
		return;
	}
	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	(void) xv_set(sb_frame, FRAME_LEFT_FOOTER,
#ifdef OW_I18N
		      gettext("Quering the server..."),
#else
		      "Quering the server...",
#endif
		      NULL);

	if ((int) sb_query_group(newsgroup,
			    (int) sb_get_props_value(Sb_article_display))) {

		(void) sb_canvas_force_repaint_proc(newsgroup);

		if (sb_num_of_article > 0) {
			(void) sb_add_menu_item(newsgroup);
			(void) xv_set(sb_frame, FRAME_LEFT_FOOTER,
#ifdef OW_I18N
				      gettext("Quering the server... Done"),
#else
				      "Quering the server... Done",
#endif
				      NULL);
		} else {
			(void) sb_clear_pane();
			(void) xv_set(sb_frame, FRAME_LEFT_FOOTER,
#ifdef OW_I18N
				      gettext("Article(s) are not arrived."),
#else
				      "Article(s) are not arrived.",
#endif
				      NULL);
		}
	} else {
		sb_num_of_article = 0;
		(void) sb_clear_pane();

		(void) xv_set(sb_frame, FRAME_LEFT_FOOTER,
#ifdef OW_I18N
			      gettext("Quering the server... Failed"),
#else
			      "Quering the server... Failed",
#endif
			      NULL);
	}

	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * To push the return key above the panel text field beside Open menu work as
 * well as Open cmd.
 */
void
sb_group_txt_proc(item, event)
	Panel_item      item;
	Event          *event;
{
	(void) sb_open_proc(NULL, NULL);
}

/*
 * Print the selected article.
 */
void
sb_print_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	char           *newsgroup;
	char            file[MAXSTRLEN];
	char            cmd[MAXSTRLEN];
	char            msg[MAXSTRLEN];
	register int    i, j, k;
	int             num;

	if ((newsgroup = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	for (i = 1; i <= sb_num_of_article; i++) {
		j = change_line_to_isselect(newsgroup, i, AT_SELECT);
		k = change_line_to_isselect(newsgroup, i, AT_SELECT_MANY);
		if (j == -1 || k == -1) {
			(void) sb_selected_check_error(i);
			continue;
		} else if (j > 0 || k > 0) {
			num = (int) change_line_to_anum(newsgroup, i);
			if (num == -1) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				 gettext("Can not get the article number."),
#else
					  "Can not get the article number.",
#endif
						  NULL);
				return;
			}
			if (!get_view_file(newsgroup, num, file)) {
				(void) unlink(file);
				continue;
			}
			sprintf(msg,
#ifdef OW_I18N
				gettext("Print %s's %d..."),
#else
				"Print %s's %d...",
#endif
				newsgroup, num);
			(void) xv_set(sb_frame, FRAME_LEFT_FOOTER, msg, NULL);

			sprintf(cmd, "cat %s | %s 2>&1", file,
			 (char *) sb_get_props_char_value(Sb_print_script));
			(void) system(cmd);

			sprintf(msg,
#ifdef OW_I18N
				gettext("Print %s's %d... Done"),
#else
				"Print %s's %d... Done",
#endif
				newsgroup, num);
			(void) xv_set(sb_frame, FRAME_LEFT_FOOTER, msg, NULL);

			(void) sb_clear_draw_current_mark(sb_latest_view_line);
			sb_latest_view_line = i;

			if ((int) change_line_to_asread(newsgroup, i, 1) == -1)
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				gettext("Can not mark the article as read."),
#else
					"Can not mark the article as read.",
#endif
						  NULL);

			(void) sb_draw_current_mark(i);
			(void) sb_clear_new_mark(i);
			(void) unlink(file);
		}
	}

	if (i > (sb_num_of_article + 1))
		(void) one_notice(sb_frame,
#ifdef OW_I18N
				  gettext("Not selected any message"),
#else
				  "Not selected any message",
#endif
				  NULL);

	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * Set the info of the selected article into the text field of the Cancel
 * frame.
 */
void
sb_set_cancel_txt()
{
	register int    i, j;
	int             num;
	char            newsno[MAXNEWSSTR];
	char           *newsgroup;

	if ((newsgroup = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	for (i = 1; i <= sb_num_of_article; i++) {
		j = change_line_to_isselect(newsgroup, i, AT_SELECT);
		if (j == -1) {
			(void) sb_selected_check_error(i);
			continue;
		} else if (j > 0) {
			break;
		}
	}

	if (i > sb_num_of_article) {
		xv_set(sb_cancel_group_txt, PANEL_VALUE, "", NULL);
		xv_set(sb_cancel_newsno_txt, PANEL_VALUE, "", NULL);
		return;
	}
	num = (int) change_line_to_anum(newsgroup, i);

	if (num == -1) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
				  gettext("Can not get the article number."),
#else
				  "Can not get the article number.",
#endif
				  NULL);
		return;
	}
	sprintf(newsno, "%d", num);

	xv_set(sb_cancel_group_txt, PANEL_VALUE, newsgroup, NULL);
	xv_set(sb_cancel_newsno_txt, PANEL_VALUE, newsno, NULL);
}

/*
 * Cancel the article used in Cancel menu.
 */
void
sb_cancel_proc(item, event)
	Panel_item      item;
	Event          *event;
{
	extern int      cancel_article();	/* Defined in shinbun*.c */
	char           *newsgroup =
	(char *) xv_get(sb_cancel_group_txt, PANEL_VALUE);
	char           *newsno =
	(char *) xv_get(sb_cancel_newsno_txt, PANEL_VALUE);
	char            msg[MAXSTRLEN];

	if (*newsgroup == '\0' || *newsgroup == '\n') {
		one_notice(sb_cancel_frame,
#ifdef OW_I18N
			   gettext("Not fill into the Newsgroup field."),
#else
			   "Not fill into the Newsgroup field.",
#endif
			   NULL);
		return;
	}
	if (*newsno == '\0' || *newsno == '\n') {
		one_notice(sb_cancel_frame,
#ifdef OW_I18N
			 gettext("Not fill into the Article number field."),
#else
			   "Not fill into the Article number field.",
#endif
			   NULL);
		return;
	}
	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

#ifdef OW_I18N
	sprintf(msg, gettext("Cancel... %s"), newsno);
#else
	sprintf(msg, "Cancel... %s", newsno);
#endif

	(void) frame_msg(sb_cancel_frame, msg);

	if ((int) cancel_article(newsgroup, newsno) == -1)
		one_notice(sb_cancel_frame,
#ifdef OW_I18N
			   gettext("Cancel failed. Sorry."),
#else
			   "Cancel failed. Sorry.",
#endif
			   NULL);
	else {
		sb_canceled_flag = TRUE;
#ifdef OW_I18N
		sprintf(msg, gettext("Cancel... %s Done."), newsno);
#else
		sprintf(msg, "Cancel... %s Done.", newsno);
#endif
		(void) frame_msg(sb_cancel_frame, msg);
		(void) sb_canvas_repaint_proc(NULL, NULL, NULL, NULL, NULL);
	}

	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * Set the info of the selected article into the text field of the Find
 * frame.
 */
void
sb_set_find_txt()
{
	int             i, j;
	char           *newsgroup;
	char           *text;
	char           *buf, *tmp;
	char            subject[MAXSTRLEN];
	char            sender[MAXSTRLEN];

	if ((newsgroup = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	for (i = 1; i <= sb_num_of_article; i++) {
		j = change_line_to_isselect(newsgroup, i, AT_SELECT);
		if (j == -1) {
			(void) sb_selected_check_error(i);
			continue;
		} else if (j > 0) {
			buf = (char *) change_line_to_subject(newsgroup, i);
			tmp = (char *) change_line_to_sender(newsgroup, i);
			break;
		}
	}
	if (i > sb_num_of_article) {
		xv_set(sb_find_sender_txt, PANEL_VALUE, "", NULL);
		xv_set(sb_find_subject_txt, PANEL_VALUE, "", NULL);
		return;
	}
	(void) sb_rm_space_subject(buf, subject);
	(void) sb_rm_space_subject(tmp, sender);

	/*
	 * Get include or not.
	 */
	text = &subject[0];

	if (strstr(text, "Re: "))
		text += 4;

	xv_set(sb_find_sender_txt, PANEL_VALUE, sender, NULL);
	xv_set(sb_find_subject_txt, PANEL_VALUE, text, NULL);
}

/*
 * FInd proc in Find menu.
 */
void
sb_find_proc(item, event)
	Panel_item      item;
	Event          *event;
{
	char           *sender =
	(char *) xv_get(sb_find_sender_txt, PANEL_VALUE);
	char           *subject =
	(char *) xv_get(sb_find_subject_txt, PANEL_VALUE);
	char            l_sender[MAXSTRLEN], l_subject[MAXSTRLEN];
	char           *l_subject_tmp, *l_sender_tmp;
	short int       sub_flag = 0, sen_flag = 0;
	short int       sub_find_flag = 0, sen_find_flag = 0;
	char           *buf;
	char           *newsgroup;
	int             i, num;
	Sb_find_type    type_flag =
	(Sb_find_type) xv_get(item, XV_KEY_DATA, SUBJECT_NEXT_TYPE);
	int             cnt;

	if ((newsgroup = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	if (sender == NULL && subject == NULL)
		return;

	(void) sb_clear_all_rectangle(newsgroup);
	(void) frame_msg(sb_find_frame, "");

	if (sender != NULL) {
		sen_flag = 1;
		strcpy(l_sender, (char *) is_to_low(sender));
	}
	if (subject != NULL) {
		sub_flag = 1;
		strcpy(l_subject, (char *) is_to_low(subject));
	}
	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	if (type_flag == FNEXT) {
		if (sb_latest_view_line < 0)
			i = 1;
		else
			i = sb_latest_view_line + 1;

		for (cnt = i; i <= sb_num_of_article; i++) {
			if (sen_flag) {
				buf =
					(char *) change_line_to_sender(newsgroup, i);
				if (buf == NULL) {
					(void) sb_selected_check_error(i);
					(void) xv_set(sb_frame, FRAME_BUSY,
						      FALSE, NULL);
					return;
				}
				l_sender_tmp = (char *) is_to_low(buf);
				if (strstr(l_sender_tmp, l_sender))
					sen_find_flag = 1;
			}
			if (sub_flag) {
				buf =
					(char *) change_line_to_subject(newsgroup, i);
				if (buf == NULL) {
					(void) sb_selected_check_error(i);
					(void) xv_set(sb_frame, FRAME_BUSY,
						      FALSE, NULL);
					return;
				}
				l_subject_tmp = (char *) is_to_low(buf);
				if (strstr(l_subject_tmp, l_subject))
					sub_find_flag = 1;
			}
			if ((sen_flag && sub_flag) &&
			    (!sen_find_flag || !sub_find_flag)) {
				cnt++;
				continue;
			}
			if ((sen_flag && sen_find_flag) ||
			    (sub_flag && sub_find_flag)) {
				num = (int) change_line_to_anum(newsgroup, i);
				if (num == -1) {
					(void) one_notice(sb_frame,
#ifdef OW_I18N
							  gettext("Can not get the article number."),
#else
					  "Can not get the article number.",
#endif
							  NULL);
					return;
				}
				if (!sb_make_article_win(newsgroup,
							 num, i, VSELECT)) {
					(void) one_notice(sb_frame,
#ifdef OW_I18N
							  gettext("Can not create the article view."),
#else
					 "Can not create the article view.",
#endif
							  NULL);
					(void) xv_set(sb_frame, FRAME_BUSY,
						      FALSE, NULL);
					return;
				}
				(void) sb_select_draw_select(newsgroup, i);
				break;
			}
		}
		if (cnt >= sb_num_of_article)
			frame_msg(sb_find_frame,
#ifdef OW_I18N
				  gettext("Not match"));
#else
				  "Not match");
#endif
	}
	if (type_flag == FPREV) {
		if (sb_latest_view_line < 0)
			i = sb_num_of_article;
		else
			i = sb_latest_view_line - 1;

		for (cnt = i; i > 0; i--) {
			if (sen_flag) {
				buf =
					(char *) change_line_to_sender(newsgroup, i);
				if (buf == NULL) {
					(void) sb_selected_check_error(i);
					(void) xv_set(sb_frame, FRAME_BUSY,
						      FALSE, NULL);
					return;
				}
				l_sender_tmp = (char *) is_to_low(buf);
				if (strstr(l_sender_tmp, l_sender))
					sen_find_flag = 1;
			}
			if (sub_flag) {
				buf =
					(char *) change_line_to_subject(newsgroup, i);
				if (buf == NULL) {
					(void) sb_selected_check_error(i);
					(void) xv_set(sb_frame, FRAME_BUSY,
						      FALSE, NULL);
					return;
				}
				l_subject_tmp = (char *) is_to_low(buf);
				if (strstr(l_subject_tmp, l_subject))
					sub_find_flag = 1;
			}
			if ((sen_flag && sub_flag) &&
			    (!sen_find_flag || !sub_find_flag)) {
				cnt++;
				continue;
			}
			if ((sen_flag && sen_find_flag) ||
			    (sub_flag && sub_find_flag)) {
				num = (int) change_line_to_anum(newsgroup, i);
				if (num == -1) {
					(void) one_notice(sb_frame,
#ifdef OW_I18N
							  gettext("Can not get the article number."),
#else
					  "Can not get the article number.",
#endif
							  NULL);
					return;
				}
				if (!sb_make_article_win(newsgroup,
							 num, i, VSELECT)) {
					(void) one_notice(sb_frame,
#ifdef OW_I18N
							  gettext("Can not create the article view."),
#else
					 "Can not create the article view.",
#endif
							  NULL);
					(void) xv_set(sb_frame, FRAME_BUSY,
						      FALSE, NULL);
					return;
				}
				(void) sb_select_draw_select(newsgroup, i);
				break;
			}
		}
		if (cnt <= 1)
			frame_msg(sb_find_frame,
#ifdef OW_I18N
				  gettext("Not match"));
#else
				  "Not match");
#endif
	}
	if (type_flag == FALL) {
		int             dummy;
		char            msg[MAXSTRLEN];

		dummy = 0;

		for (i = 1, cnt = 1; i <= sb_num_of_article; i++) {
			if (sen_flag) {
				buf =
					(char *) change_line_to_sender(newsgroup, i);
				if (buf == NULL) {
					(void) sb_selected_check_error(i);
					(void) xv_set(sb_frame, FRAME_BUSY,
						      FALSE, NULL);
					return;
				}
				l_sender_tmp = (char *) is_to_low(buf);
				if (strstr(l_sender_tmp, l_sender))
					sen_find_flag = 1;
			}
			if (sub_flag) {
				buf =
					(char *) change_line_to_subject(newsgroup, i);
				if (buf == NULL) {
					(void) sb_selected_check_error(i);
					(void) xv_set(sb_frame, FRAME_BUSY,
						      FALSE, NULL);
					return;
				}
				l_subject_tmp = (char *) is_to_low(buf);
				if (strstr(l_subject_tmp, l_subject))
					sub_find_flag = 1;
			}
			if ((sen_flag && sub_flag) &&
			    (!sen_find_flag || !sub_find_flag)) {
				cnt++;
				continue;
			}
			if ((sen_flag && sen_find_flag) ||
			    (sub_flag && sub_find_flag)) {
				(void) sb_select_draw_adjust(newsgroup, i);
				dummy++;
			}
			sub_find_flag = 0;
			sen_find_flag = 0;
		}
		if (cnt >= sb_num_of_article)
#ifdef OW_I18N
			frame_msg(sb_find_frame, gettext("Not match"));
#else
			frame_msg(sb_find_frame, "Not match");
#endif
		else {
#ifdef OW_I18N
			sprintf(msg, gettext("%d article(s) found"), dummy);
#else
			sprintf(msg, "%s article(s) found", dummy);
#endif
			frame_msg(sb_find_frame, msg);
		}
	}
	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

void
sb_next_subject_find_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	char           *newsgroup;
	char            subject[MAXSTRLEN];
	char           *buf;
	char           *find_str;
	register int    i, j;
	int             line, num;

	if ((newsgroup = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	for (i = 1; i <= sb_num_of_article; i++) {
		j = change_line_to_isselect(newsgroup, i, AT_SELECT);
		if (j == -1) {
			(void) sb_selected_check_error(i);
			continue;
		} else if (j > 0) {
			buf = (char *) change_line_to_subject(newsgroup, i);
			line = i;
			break;
		}
	}
	if (i > sb_num_of_article) {
#ifdef OW_I18N
		(void) one_notice(sb_frame, gettext("Not selected any message"), NULL);
#else
		(void) one_notice(sb_frame, "Not selected any message", NULL);
#endif
		return;
	}
	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	(void) sb_rm_space_subject(buf, subject);

	find_str = &subject[0];

	if (strstr(find_str, "Re: "))
		find_str += 4;

	for (i = line + 1; i <= sb_num_of_article; i++) {
		buf = (char *) change_line_to_subject(newsgroup, i);
		if (buf == NULL) {
			(void) sb_selected_check_error(i);
			(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
			return;
		}
		if (strstr(buf, find_str)) {
			num = (int) change_line_to_anum(newsgroup, i);
			if (num == -1) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				 gettext("Can not get the article number."),
#else
					  "Can not get the article number.",
#endif
						  NULL);
				return;
			}
			if (!sb_make_article_win(newsgroup,
						 num, i, VSELECT)) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				gettext("Can not create the article view."),
#else
					 "Can not create the article view.",
#endif
						  NULL);
				(void) xv_set(sb_frame, FRAME_BUSY,
					      FALSE, NULL);
				return;
			}
			(void) sb_select_draw_select(newsgroup, i);
			break;
		}
	}
	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

void
sb_prev_subject_find_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	char           *newsgroup;
	char            subject[MAXSTRLEN];
	char           *buf;
	char           *find_str;
	register int    i, j;
	int             line, num;

	if ((newsgroup = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	for (i = 1; i <= sb_num_of_article; i++) {
		j = change_line_to_isselect(newsgroup, i, AT_SELECT);
		if (j == -1) {
			(void) sb_selected_check_error(i);
			continue;
		} else if (j > 0) {
			buf = (char *) change_line_to_subject(newsgroup, i);
			line = i;
			break;
		}
	}
	if (i > sb_num_of_article) {
#ifdef OW_I18N
		(void) one_notice(sb_frame, gettext("Not selected any message"), NULL);
#else
		(void) one_notice(sb_frame, "Not selected any message", NULL);
#endif
		return;
	}
	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	(void) sb_rm_space_subject(buf, subject);

	find_str = &subject[0];

	if (strstr(find_str, "Re: "))
		find_str += 4;

	for (i = line - 1; i > 0; i--) {
		buf = (char *) change_line_to_subject(newsgroup, i);
		if (buf == NULL) {
			(void) sb_selected_check_error(i);
			(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
			return;
		}
		if (strstr(buf, find_str)) {
			num = (int) change_line_to_anum(newsgroup, i);
			if (num == -1) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				 gettext("Can not get the article number."),
#else
					  "Can not get the article number.",
#endif
						  NULL);
				return;
			}
			if (!sb_make_article_win(newsgroup,
						 num, i, VSELECT)) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				gettext("Can not create the article view."),
#else
					 "Can not create the article view.",
#endif
						  NULL);
				(void) xv_set(sb_frame, FRAME_BUSY,
					      FALSE, NULL);
				return;
			}
			(void) sb_select_draw_select(newsgroup, i);
			break;
		}
	}
	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

void
sb_all_subject_find_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	char           *newsgroup;
	char            subject[MAXSTRLEN];
	char           *buf;
	char           *find_str;
	register int    i, j;
	int             dummy;
	char            msg[MAXSTRLEN];
	int             cnt;

	if ((newsgroup = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	for (i = 1; i <= sb_num_of_article; i++) {
		j = change_line_to_isselect(newsgroup, i, AT_SELECT);
		if (j == -1) {
			(void) sb_selected_check_error(i);
			continue;
		} else if (j > 0) {
			buf = (char *) change_line_to_subject(newsgroup, i);
			break;
		}
	}
	if (i > sb_num_of_article) {
#ifdef OW_I18N
		(void) one_notice(sb_frame, gettext("Not selected any message"), NULL);
#else
		(void) one_notice(sb_frame, "Not selected any message", NULL);
#endif
		return;
	}
	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	(void) sb_rm_space_subject(buf, subject);

	find_str = &subject[0];

	if (strstr(find_str, "Re: "))
		find_str += 4;

	dummy = 0;

	for (i = 1, cnt = 1; i <= sb_num_of_article; i++) {
		buf = (char *) change_line_to_subject(newsgroup, i);
		if (buf == NULL) {
			(void) sb_selected_check_error(i);
			(void) xv_set(sb_frame, FRAME_BUSY,
				      FALSE, NULL);
			return;
		}
		if (strstr(buf, find_str)) {
			(void) sb_select_draw_adjust(newsgroup, i);
			dummy++;
		} else
			cnt++;
	}

	if (cnt >= sb_num_of_article)
#ifdef OW_I18N
		frame_msg(sb_frame, gettext("Not match"));
#else
		frame_msg(sb_frame, "Not match");
#endif
	else {
#ifdef OW_I18N
		sprintf(msg, gettext("%d article(s) found"), dummy);
#else
		sprintf(msg, "%s article(s) found", dummy);
#endif
		frame_msg(sb_frame, msg);
	}

	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

sb_rescan_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	int             i;
	char            fname1[MAXSTRLEN];
	char            command[MAXSTRLEN];
	char           *newsgroup = (char *) xv_get(sb_group_txt, PANEL_VALUE);

	if (*newsgroup == '\0' || *newsgroup == '\n') {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	set_nbiff_as_rescan();

	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);
	(void) frame_msg(sb_frame,
#ifdef OW_I18N
		     gettext("Quering the server for any new articles..."));
#else
			 "Quering the server for any new articles...");
#endif

	/* save .newsrc */
	(void) save_newsrc();

	/* once quit shinbun */
	shinbun_quit();

	/* init data */
	shinbun_init();

	/* Init newsrc data using .newsrc */
	read_newsrc();

	/* Reconnect NNTP with server */
	connect_nntp_server();
	sb_newsrc_status = 0;

	if (internal_mode_status == 1) {
		strcpy(fname1, mktemp("/tmp/NRinternal1.XXXXXX"));

		for (i = 0; i < NUM_INTERNAL_NGRP; i++) {
			sprintf(command,
				"/bin/grep \"^%s\" %s >> %s",
			      INTERNAL_GRPNAM[i], ACTIVE_FLIST_NAM, fname1);
			system(command);
		}
		sprintf(command, "/bin/cp %s %s", fname1, ACTIVE_FLIST_NAM);
		system(command);

		unlink(fname1);
	}
	/* Display subject data */
	if ((int) sb_query_group(newsgroup,
		   (int) sb_get_props_value(Sb_article_display)) == FALSE) {
#ifdef OW_I18N
		(void) one_notice(sb_frame,
				  gettext("Quering the server failed."),
				  NULL);
#else
		(void) one_notice(sb_frame,
				  "Quering the server failed.",
				  NULL);
#endif
		(void) sb_canvas_force_repaint_proc(newsgroup);
	} else {
		(void) sb_canvas_force_repaint_proc(newsgroup);

		if (sb_num_of_article > 0) {
			(void) frame_msg(sb_frame,
#ifdef OW_I18N
					 gettext("Quering the server for any new articles... Done"));
#else
			 "Quering the server for any new articles... Done");
#endif
		} else {
			(void) frame_msg(sb_frame,
#ifdef OW_I18N
				    gettext("Article(s) are not arrived."));
#else
					 "Article(s) are not arrived.");
#endif
			(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
			return;
		}

	}

	/* Display the first new article at the adopt position */
	for (i = 1; i <= sb_num_of_article; i++)
		if ((int) change_line_to_isread(newsgroup, i) == 0) {
			sb_line_force_paint(i);
			break;
		}
	/*
	 * Destroy menu.
	 */
	(void) sb_destroy_group_menu(sb_all_group_menu);

	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}


void
sb_article_asread_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	register int    i, j, k;
	char           *newsgroup;
	int             num = 0;

	if ((newsgroup = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	for (i = 1; i <= sb_num_of_article; i++) {
		j = change_line_to_isselect(newsgroup, i, AT_SELECT);
		k = change_line_to_isselect(newsgroup, i, AT_SELECT_MANY);
		if (j == -1 || k == -1) {
			(void) sb_selected_check_error(i);
			continue;
		} else if (j > 0) {
			if ((int) change_line_to_asread(newsgroup, i, 1) == -1)
				(void) one_notice(sb_frame,
#ifdef OW_I18N
						  gettext("Can not mark the article as read"), NULL);
#else
				  "Can not mark the article as read", NULL);
#endif
			(void) sb_clear_new_mark(i);
			num++;
		} else if (k > 0) {
			if ((int) change_line_to_asread(newsgroup, i, 1) == -1)
				(void) one_notice(sb_frame,
#ifdef OW_I18N
						  gettext("Can not mark the article as read"), NULL);
#else
				  "Can not mark the article as read", NULL);
#endif
			(void) sb_clear_new_mark(i);
			num++;
		}
	}
	if (num == 0)
		(void) one_notice(sb_frame,
#ifdef OW_I18N
				  gettext("Not select article(s)"), NULL);
#else
				  "Not select article(s)", NULL);
#endif

	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

void
sb_article_asunread_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	int             i, j, k;
	char           *newsgroup;
	int             num = 0;

	if ((newsgroup = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);
	for (i = 1; i <= sb_num_of_article; i++) {
		j = change_line_to_isselect(newsgroup, i, AT_SELECT);
		k = change_line_to_isselect(newsgroup, i, AT_SELECT_MANY);
		if (j == -1 || k == -1) {
			(void) sb_selected_check_error(i);
			continue;
		} else if (j > 0) {
			if ((int) change_line_to_asread(newsgroup, i, 0) == -1)
				(void) one_notice(sb_frame,
#ifdef OW_I18N
						  gettext("Can not mark the article as read"), NULL);
#else
				  "Can not mark the article as read", NULL);
#endif
			(void) sb_draw_new_mark(i);
			num++;
		} else if (k > 0) {
			if ((int) change_line_to_asread(newsgroup, i, 0) == -1)
				(void) one_notice(sb_frame,
#ifdef OW_I18N
						  gettext("Can not mark the article as read"), NULL);
#else
				  "Can not mark the article as read", NULL);
#endif
			(void) sb_draw_new_mark(i);
			num++;
		}
	}
	if (num == 0)
		(void) one_notice(sb_frame,
#ifdef OW_I18N
				  gettext("Not select article(s)"), NULL);
#else
				  "Not select article(s)", NULL);
#endif
	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}


void
sb_group_as_unread_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	char           *group_name;
	char            msg[MAXSTRLEN];

	if ((group_name = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);
#ifdef OW_I18N
	(void) frame_msg(sb_frame, gettext("Mark as unread..."));
#else
	(void) frame_msg(sb_frame, "Mark as unread...");
#endif

	if ((int) change_all_to_asread(group_name, 0) == -1) {
#ifdef OW_I18N
		sprintf(msg, gettext("Can not mark to the %s group as unread"),
			group_name);
#else
		sprintf(msg, "Can not mark to the %s group as unread",
			group_name);
#endif
		(void) one_notice(sb_frame, msg, NULL);
	}
	(void) sb_canvas_force_repaint_proc(group_name);

#ifdef OW_I18N
	(void) frame_msg(sb_frame, gettext("Mark as unread... Done"));
#else
	(void) frame_msg(sb_frame, "Mark as unread... Done");
#endif
	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}


void
sb_group_as_read_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	char           *group_name;
	char            msg[MAXSTRLEN];

	if ((group_name = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);
#ifdef OW_I18N
	(void) frame_msg(sb_frame, gettext("Mark as read... "));
#else
	(void) frame_msg(sb_frame, "Mark as read... ");
#endif

	if ((int) change_all_to_asread(group_name, 1) == -1) {
#ifdef OW_I18N
		sprintf(msg, gettext("Can not mark to the %s group as read"),
			group_name);
#else
		sprintf(msg, "Can not mark to the %s group as read",
			group_name);
#endif
		(void) one_notice(sb_frame, msg, NULL);
	}
	(void) sb_canvas_force_repaint_proc(group_name);

#ifdef OW_I18N
	(void) frame_msg(sb_frame, gettext("Mark as read... Done"));
#else
	(void) frame_msg(sb_frame, "Mark as read... Done");
#endif
	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * Query the ng from the server used in read or unread menu.
 */
void
sb_article_view(article_flag)
	int             article_flag;
{
	char           *newsgroup;

	if ((newsgroup = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	if (change_grp_to_isnewarticle(newsgroup) > 0) {
		sb_rescan_proc(NULL, NULL);
		return;
	}
	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	(void) xv_set(sb_frame, FRAME_LEFT_FOOTER,
#ifdef OW_I18N
		      gettext("Quering the server..."), NULL);
#else
		      "Quering the server...", NULL);
#endif

	if ((int) sb_query_group(newsgroup, article_flag)) {
		(void) sb_canvas_force_repaint_proc(newsgroup);

		if (sb_num_of_article > 0) {
			(void) sb_add_menu_item(newsgroup);

			(void) xv_set(sb_frame, FRAME_LEFT_FOOTER,
#ifdef OW_I18N
				      gettext("Quering the server... Done"),
#else
				      "Quering the server... Done",
#endif
				      NULL);
		} else {
			(void) sb_clear_pane();
			(void) xv_set(sb_frame, FRAME_LEFT_FOOTER,
#ifdef OW_I18N
				      gettext("Article(s) are not arrived."),
#else
				      "Article(s) are not arrived.",
#endif
				      NULL);
		}
	} else {
		sb_num_of_article = 0;
		(void) sb_clear_pane();

		xv_set(sb_group_txt, PANEL_VALUE, "", NULL);
		(void) xv_set(sb_frame, FRAME_LEFT_FOOTER,
#ifdef OW_I18N
			      gettext("Quering the server... Failed"),
#else
			      "Quering the server... Failed",
#endif
			      NULL);
	}

	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * Query only unread article.
 */
void
sb_unreads_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	(void) sb_article_view(0);
}

/*
 * Query only read article.
 */
void
sb_reads_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	(void) sb_article_view(1);
}

/*
 * Save the changes into the .newsrc file.
 */
void
sb_save_change_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	char            msg[MAXSTRLEN];

	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

#ifdef OW_I18N
	sprintf(msg, gettext("Saving changes into the %s file..."), NEWSRC_FLIST_NAM);
#else
	sprintf(msg, "Saving changes into the %s file...", NEWSRC_FLIST_NAM);
#endif
	(void) frame_msg(sb_frame, msg);

	if ((int) save_newsrc() == -1) {
#ifdef OW_I18N
		sprintf(msg, gettext("Can not write %s file."), NEWSRC_FLIST_NAM);
#else
		sprintf(msg, "Can not write %s file.", NEWSRC_FLIST_NAM);
#endif
		(void) one_notice(sb_frame, msg, NULL);
	}
	read_newsrc();

#ifdef OW_I18N
	sprintf(msg, gettext("Saving changes into the %s file... Done"),
#else
	sprintf(msg, "Saving changes into the %s file... Done",
#endif
		NEWSRC_FLIST_NAM);

	(void) frame_msg(sb_frame, msg);

	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * View the next or prev line to the current.
 */
int
sb_next_select_article_func(newsgroup, flag)
	char           *newsgroup;
	Sb_next_type    flag;
{
	register int    i, j, num = 0;
	int             line, newsnum;

	if (flag == NEXT) {
		for (i = 1; i <= sb_num_of_article; i++) {
			j = change_line_to_isselect(newsgroup, i, AT_SELECT);
			if (j == -1) {
				(void) sb_selected_check_error(i);
				continue;
			} else if (j > 0) {
				line = i;
				num++;
			}
		}
	} else if (flag == PREV) {
		for (i = sb_num_of_article; i >= 1; i--) {
			j = change_line_to_isselect(newsgroup, i, AT_SELECT);
			if (j == -1) {
				(void) sb_selected_check_error(i);
				continue;
			} else if (j > 0) {
				line = i;
				num++;
			}
		}
	}
	if (num == 0)
		return FALSE;

	if (flag == NEXT) {
		line++;
		if (line > sb_num_of_article)
			return FALSE;
	} else if (flag == PREV) {
		line--;
		if (line < 1)
			return FALSE;
	}
	newsnum = (int) change_line_to_anum(newsgroup, line);
	if (newsnum == -1) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
				  gettext("Can not get the article number."),
#else
				  "Can not get the article number.",
#endif
				  NULL);
		return FALSE;
	}
	if (!sb_make_article_win(newsgroup, newsnum, line, VSELECT)) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
				gettext("Can not create the article view."),
#else
				  "Can not create the article view.",
#endif
				  NULL);
		return FALSE;
	}
	(void) sb_select_draw_select(newsgroup, line);

	return TRUE;
}

void
sb_super_next_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	Sb_next_type    type_flag =
	(Sb_next_type) xv_get(menu_item, XV_KEY_DATA, SUPER_NEXT);

	sb_super_next_proc_x(type_flag);
}

sb_super_next_key(type_flag)
	Sb_next_type    type_flag;
{
	sb_super_next_proc_x(type_flag);
}

/*
 * Find the Next or Prev page, article and newsgroup for the advanced user.
 */
sb_super_next_proc_x(type_flag)
	Sb_next_type    type_flag;
{

	char            newsgroup[MAXSTRLEN];
	char           *buf;
	VIEW_DATA      *view_win;

	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	if ((buf = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
	} else
		strcpy(newsgroup, buf);

	if ((view_win = (VIEW_DATA *) sb_find_view_win()) != NULL) {
		if (sb_scroll_more_proc(view_win, type_flag) == FALSE) {
			if (sb_next_select_article_func(newsgroup, type_flag) == FALSE) {
				if ((buf = (char *) sb_find_next_ng_func(newsgroup, type_flag)) == NULL) {
					(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
					return;
				}
				if (type_flag == NEXT)
					(void) sb_view_first_unselect_proc(buf);
				else if (type_flag == PREV)
					(void) sb_view_last_unselect_proc(buf);
			}
		}
	} else if (sb_next_select_article_func(newsgroup, type_flag) == FALSE) {
		if (type_flag == NEXT)
			(void) sb_view_first_unselect_proc(newsgroup);
		else if (type_flag == PREV)
			(void) sb_view_last_unselect_proc(newsgroup);

	}
	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * Check whether scrolling more or not.
 */
int
sb_scroll_more_proc(view_win, type_flag)
	VIEW_DATA      *view_win;
	Sb_next_type    type_flag;
{
	int             length;
	int             top, bottom;
	static int      prev_bottom;
/* ??? If newsgroup is changed, I miss to care.
	char *current_ng = (char *) get_current_newsgroup();

	if(strcmp(current_ng ,view_win->news_group)){
		sb_super_next_proc_x(type_flag);
		return TRUE;
	}
*/

	(void) textsw_file_lines_visible(view_win->textsw, &top, &bottom);

	/*
	 * length = textsw_screen_line_count(view_win->textsw) / 3;
	 */

	if (prev_bottom != bottom) {
		prev_bottom = bottom;
		length = (bottom - top) / 3;
		/* ??? textsw_scroll_lines() cat not move scrollbar */
		/* I don't know how to sycronize textsw and scrollbar */
		(void) textsw_scroll_lines(view_win->textsw, length);
		return TRUE;
	} else
		return FALSE;
}

void
sb_popup_latest_proc(item, event)
	Panel_item      item;
	Event          *event;
{
	insert_new_group_to_plist();
	(void) make_newsbiff_frame();
}

void
sb_next_ng_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	int             flag = (int) xv_get(menu_item, XV_KEY_DATA, NEXT_NG);

	sb_next_ng_proc_x(flag);
}

sb_next_ng_key(flag)
	int             flag;
{
	sb_next_ng_proc_x(flag);
}

/*
 * Next or Prev Newsgroup proc defined sb_pane.c
 */
sb_next_ng_proc_x(flag)
	int             flag;
{
	char           *newsgroup;

	if ((newsgroup = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the current newsgroup."),
#else
				  "Can not get the current newsgroup.",
#endif
				  NULL);
		return;
	}
	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	(void) sb_find_next_ng_func(newsgroup, flag);

	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

/*
 * Catch up - All articles are marked as read without querying the ng.
 */
void
sb_catch_up_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	char           *newsgroup = (char *) xv_get(sb_group_txt, PANEL_VALUE);
	char            buf[MAXSTRLEN];

	catchup_ngrp(newsgroup);

#ifdef OW_I18N
	sprintf(buf, gettext("Catch up %s... Done"), newsgroup);
#else
	sprintf(buf, "Catch up %s... Done", newsgroup);
#endif

	frame_msg(sb_frame, buf);
}

/*
 * void sb_status_proc(item, event) Panel_item      item; Event *event; {
 * make_status_win(); }
 */
