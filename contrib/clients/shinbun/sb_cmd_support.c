#ifdef sccs
static char     sccsid[] = "@(#)sb_cmd_support.c	1.27 91/08/22";
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
#include <sys/types.h>
#include <sys/stat.h>
#include <malloc.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>

#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/text.h>
#include <xview/seln.h>
#include <xview/notice.h>
#include <xview/cursor.h>
#include <xview/canvas.h>
#include <xview/scrollbar.h>
#include <xview/notify.h>
#include <xview/font.h>

#include "sb_tool.h"
#include "sb_def.h"
#include "sb_item.h"
#include "sb_canvas.h"
#include "sb_shinbun.h"
#include "shinbun.h"
#include "config.h"

/* Flag whether new articles are arrived or not.  */
int             sb_arrive_icon;

int             sb_newsrc_status;

char            NEWSBIFFRC[256];
struct itimerval sb_timer;

extern int      sb_display_status;
extern char    *sb_post_seln_file;

void
sb_view_first_unselect_proc(newsgroup)
	char           *newsgroup;
{
	register int    i;
	int             num;

	for (i = 1; i <= sb_num_of_article; i++)
		if ((int) change_line_to_isread(newsgroup, i) == 0) {
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
			if (!sb_make_article_win(newsgroup, num, i, VSELECT)) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				gettext("Can not create the article view."),
#else
					 "Can not create the article view.",
#endif
						  NULL);
				return;
			}
			(void) sb_select_draw_select(newsgroup, i);
			break;
		}
}

void
sb_view_last_unselect_proc(newsgroup)
	char           *newsgroup;
{
	register int    i;
	int             num;

	for (i = sb_num_of_article; i > 0; i--)
		if ((int) change_line_to_isread(newsgroup, i) == 0) {
			num = (int) change_line_to_anum(newsgroup, i);
			if (num == -1) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				 gettext("Can not get the article numbgr."),
#else
					  "Can not get the article number.",
#endif
						  NULL);
				return;
			}
			if (!sb_make_article_win(newsgroup, num, i, VSELECT)) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				gettext("Can not create the article view."),
#else
					 "Can not create the article view.",
#endif
						  NULL);
				return;
			}
			(void) sb_select_draw_select(newsgroup, i);
			break;
		}
}

sb_view_msgid(msgids)
	char           *msgids;
{
	char            tmpmsgid[512];
	int             i = -1;

	while (*msgids != '\0') {
		switch (*msgids) {
		case '<':
			if (i == -1) {
				i = 0;
				tmpmsgid[i] = '<';
				i++;
			}
			break;
		case '>':
			if (i > 0) {
				tmpmsgid[i] = '>';
				tmpmsgid[i + 1] = '\0';
				i = -1;
#ifdef DEBUG
				printf("%s\n", tmpmsgid);
#endif
				sb_view_win_MID(tmpmsgid);
			}
			break;
		default:
			if (i > 0) {
				tmpmsgid[i] = *msgids;
				i++;
			}
			break;
		}
		msgids++;
	}
}

int
sb_get_first_diplay_group(number, article_flag, flag)
	int             number;
	int             article_flag;
	Sb_next_type    flag;
{
	char            newsgroup[MAXSTRLEN];
	register int    i;
	char           *sb_get_num_newsrc();

	if (number == num_of_ngrp)
		number = 0;

	if (flag == NEXT) {
		for (i = number; i < num_of_ngrp; i++) {
			if ((char *) sb_get_num_newsrc(i, newsgroup) == NULL)
				continue;

			if (article_flag) {
				if (get_subject_file(newsgroup))
					break;
			} else {
				if (get_subject_file_unread(newsgroup))
					break;
			}
		}
	} else if (flag == PREV) {
		for (i = number; i >= 0; i--) {
			if ((char *) sb_get_num_newsrc(i, newsgroup) == NULL)
				continue;

			if (article_flag) {
				if (get_subject_file(newsgroup))
					break;
			} else {
				if (get_subject_file_unread(newsgroup))
					break;
			}
		}
	}
	/*
	 * We have serach the ng, but if no ng it, return FALSE.
	 */
	if (flag == NEXT)
		if (i == num_of_ngrp) {
			return FALSE;
		}
	if (flag == PREV)
		if (i < 0) {
			return FALSE;
		}
	(void) xv_set(sb_group_txt, PANEL_VALUE, newsgroup, NULL);
	(void) sb_add_menu_item(newsgroup);

	return TRUE;
}

char           *
sb_get_num_newsrc(num, newsgroup)
	int             num;
	char           *newsgroup;
{
	char            ptr[MAXSTRLEN];
	int             i = 0;

	strncpy(ptr, return_newsrc_data(num), 255);
	while (ptr[i] != ':') {
		*newsgroup++ = ptr[i];
		if (ptr[i] == '\0')
			return NULL;
		if (ptr[i] == '!')
			return NULL;
		if (i > (MAXSTRLEN - 1))
			return NULL;
		i++;
	}
	*newsgroup++ = '\0';

	return newsgroup;
}

void
sb_set_news_biff_timer()
{
	extern Notify_value nbiff();
	int             sb_news_biff_time = (int) sb_get_props_value(Sb_biff_timer);
	extern int      jxx;

	if (sb_news_biff_time > 0) {
		jxx = 0;
		sb_timer.it_value.tv_sec = sb_news_biff_time * 60;
		sb_timer.it_interval.tv_sec = sb_news_biff_time * 60;

		notify_set_itimer_func((Frame) sb_frame,
			(Notify_value) nbiff, ITIMER_REAL, &sb_timer, NULL);

		(void) xv_set(sb_frame_icon,
			      ICON_IMAGE, sb_non_arrive_image,
			      ICON_MASK_IMAGE, sb_non_arrive_mask_image,
			      ICON_TRANSPARENT, TRUE,
#ifdef OW_I18N
			      XV_LABEL, gettext("Shinbun tool"),
#else
			      XV_LABEL, "Shinbun tool",
#endif
			      NULL);
		(void) xv_set(sb_frame, FRAME_ICON, sb_frame_icon, NULL);
	} else {
		jxx = -1;
		sb_timer.it_value.tv_sec = 300;
		sb_timer.it_interval.tv_sec = 300;

		notify_set_itimer_func((Frame) sb_frame,
			(Notify_value) nbiff, ITIMER_REAL, &sb_timer, NULL);

		(void) xv_set(sb_frame_icon,
			      ICON_IMAGE, sb_image,
			      ICON_MASK_IMAGE, sb_mask_image,
			      ICON_TRANSPARENT, TRUE,
#ifdef OW_I18N
			      XV_LABEL, gettext("Shinbun tool"),
#else
			      XV_LABEL, "Shinbun tool",
#endif
			      NULL);
		(void) xv_set(sb_frame, FRAME_ICON, sb_frame_icon, NULL);
	}
}

/*
 * Get the screen size for article view window, and get the fisrt display
 * newsgroup when bringing up.
 */
void
sb_init_proc()
{

	(void) get_screen(sb_frame);

	(void) sb_read_rc_file();

#ifdef SB_INIT_PROC
	sb_get_first_diplay_group(0, (int) sb_get_props_value(Sb_article_display), NEXT);
#endif

	if (mktemp(sb_seln_file) == -1)
		fprintf(stderr, "Can not create %s file\n", sb_seln_file);

	if (mktemp(sb_post_seln_file) == -1)
		fprintf(stderr, "Can not create %s file\n", sb_post_seln_file);

	sprintf(NEWSBIFFRC, "%s/.newsbiff", (char *) getenv("HOME"));

	(void) sb_set_news_biff_timer();

	sb_newsrc_status = FALSE;
}

void
sb_clean_when_die()
{

	(void) unlink(sb_seln_file);
	(void) unlink(sb_post_seln_file);
	(void) seln_done(sb_seln_client);
	(void) nntp_quit();
	(void) sb_destroy_all_view_win();
	(void) sb_destroy_all_post_win();
}

Notify_value
sb_signal_func(client, sig, when)
	Notify_client   client;
	int             sig;
	Notify_signal_mode when;
{
	int             result;

	if (sb_newsrc_status == TRUE)
		result = two_notice(client,
#ifdef OW_I18N
			gettext("Save Changes"), gettext("Discard Changes"),
		       gettext("The current .newsrc file has been changed"),
				gettext("Do you wish to save the changes?"),
#else
				    "Save Changes", "Discard Changes",
				"The current .newsrc file has been changed",
				    "Do you wish to save the changes?",
#endif
				    NULL);

	if (result == 1)
		(void) save_newsrc();

	sb_clean_when_die();
	exit(0);
}

void
sb_signal_set(frame)
	Frame           frame;
{
	Notify_value    sb_signal_func();

	(void) notify_set_signal_func(frame, sb_signal_func,
				      SIGHUP, NOTIFY_ASYNC);
	(void) notify_set_signal_func(frame, sb_signal_func,
				      SIGINT, NOTIFY_ASYNC);
	(void) notify_set_signal_func(frame, sb_signal_func,
				      SIGBUS, NOTIFY_ASYNC);
	(void) notify_set_signal_func(frame, sb_signal_func,
				      SIGSEGV, NOTIFY_ASYNC);
}

void
sb_frame_adopt_layout()
{
	int             width = (int) xv_get(sb_frame, XV_WIDTH);
	int             spos = (int) xv_get(sb_group_txt, PANEL_VALUE_X);
	Font            font = (int) xv_get(sb_panel, WIN_FONT);
	int             txt_length = width - spos;
	int             char_length =
	txt_length / (int) xv_get(font, FONT_DEFAULT_CHAR_WIDTH);

	char_length -= 5;

	xv_set(sb_group_txt, PANEL_VALUE_DISPLAY_LENGTH, char_length, NULL);
}

/*
 * Notify resizing, icon open & close.
 */
Notify_value
sb_frame_event_func(client, event, arg, when)
	Notify_client   client;
	Event          *event;
	Notify_arg      arg;
	Notify_event_type when;
{
	Notify_value    return_value = notify_next_event_func(client,
						   event, arg, NOTIFY_SAFE);
	int             sb_news_biff_time = (int) sb_get_props_value(Sb_biff_timer);
	extern int      make_newsbiff_frame();

	if (event_id(event) == WIN_RESIZE) {
		sb_canvas_shrink_proc();
		sb_frame_adopt_layout();
		sb_canvas_repaint_proc(NULL, NULL, NULL, NULL, NULL);
	} else if (event_action(event) == ACTION_OPEN) {
		if (sb_arrive_icon == TRUE) {
			sb_arrive_icon = FALSE;
			insert_new_group_to_plist();
			make_newsbiff_frame();
		} else {
			/*
			 * It takes too long time at start.
			 * sb_rescan_proc(NULL, NULL);
			 */
		}
	} else if (event_action(event) == ACTION_CLOSE) {
		sb_arrive_icon = FALSE;
		if (sb_news_biff_time > 0)
			(void) xv_set(sb_frame_icon,
				      ICON_IMAGE, sb_non_arrive_image,
				  ICON_MASK_IMAGE, sb_non_arrive_mask_image,
				      ICON_TRANSPARENT, TRUE,
#ifdef OW_I18N
				      XV_LABEL, gettext("Shinbun tool"),
#else
				      XV_LABEL, "Shinbun tool",
#endif

				      NULL);
		else
			(void) xv_set(sb_frame_icon,
				      ICON_IMAGE, sb_image,
				      ICON_MASK_IMAGE, sb_mask_image,
				      ICON_TRANSPARENT, TRUE,
#ifdef OW_I18N
				      XV_LABEL, gettext("Shinbun tool"),
#else
				      XV_LABEL, "Shinbun tool",
#endif
				      NULL);
		(void) xv_set(sb_frame, FRAME_ICON, sb_frame_icon, NULL);
	}
	return (return_value);
}

Notify_value
sb_frame_destroy_func(client, status)
	Notify_client   client;
	Destroy_status  status;
{

	if (status == DESTROY_CHECKING) {
		int             result = 0;

		if (sb_newsrc_status == TRUE)
			result = three_notice(client,
#ifdef OW_I18N
					      gettext("Save Changes"), gettext("Discard Changes"), gettext("Cancel"),
			gettext("The current .newsrc file has been changed"),
				gettext("Do you wish to save the changes?"),
#else
				"Save Changes", "Discard Changes", "Cancel",
				"The current .newsrc file has been changed",
					 "Do you wish to save the changes?",
#endif
					      NULL);
		if (result == 0)
			return notify_next_destroy_func(client, status);
		else if (result == 1) {
			(void) save_newsrc();
		} else if (result == 2) {
			/* through DESTROY_CLEANUP */
		} else if (result == 3)
			notify_veto_destroy(client);

	} else if (status == DESTROY_CLEANUP) {
		sb_clean_when_die();
		return notify_next_destroy_func(client, status);
	} else
		return notify_next_destroy_func(client, status);

	return NOTIFY_DONE;
}

/*
 * Devide the seleced text by seln service to news number & subject.
 */
void
sb_rm_space_subject(isubject, osubject)
	char           *isubject;
	char           *osubject;
{
	while (*isubject == ' ')
		isubject++;

	while (*isubject)
		*osubject++ = *isubject++;

	*osubject++ = '\0';
}

void
sb_biff_panel_proc(item, string, op, event)
	Panel_item      item;
	char           *string;
	Panel_list_op   op;
	Event          *event;
{
	(void) xv_set(sb_group_txt, PANEL_VALUE, string, NULL);
}

void
sb_selected_check_error(line)
	int             line;
{
	char            msg[MAXSTRLEN];

#ifdef OW_I18N
	sprintf(msg, gettext("Can not get the info of the %d line's selection."), line);
#else
	sprintf(msg, "Can not get the info of the %d line's selection.", line);
#endif

	(void) one_notice(sb_frame, msg, NULL);
}
