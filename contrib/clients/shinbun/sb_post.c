#ifdef sccs
static char     sccsid[] = "@(#)sb_post.c	1.25 91/09/12";
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

#include <xview/xview.h>
#include <xview/textsw.h>
#include <xview/icon.h>
#include <xview/panel.h>
#include <xview/notice.h>
#include <xview/svrimage.h>
#include <xview/cursor.h>
#include <xview/cms.h>
#include <xview/font.h>
#include <xview/rect.h>
#include <xview/seln.h>

#include "sb_tool.h"
#include "sb_def.h"
#include "sb_ui.h"
#include "sb_item.h"
#include "shinbun.h"
#include "config.h"

#define DEFAULT_SIG_NAME	".signature"
#define ENCODE	"cat %s | compress | uuencode %s.Z > %s"

typedef struct compose_data {
	Frame           frame;
	Textsw          textsw;
	Icon            icon;
	Panel           panel;
	Panel_item      dist_item;
	Panel_item      sign_item;
	Panel_item      from_item;
	Panel_item      path_item;
	Panel_item      newsgroup_item;
	Panel_item      p_subject_item;
	Panel_item      expires_item;
	Panel_item      reference_item;
	Panel_item      sender_item;
	Panel_item      replyto_item;
	Panel_item      followupto_item;
	Panel_item      organaization_item;
	Panel_item      keyword_item;
	Panel_item      to_item;
	Panel_item      m_subject_item;
	Panel_item      cc_item;
	Panel_item      bcc_item;
	/*
	 * This is set for layout of mail.
	 */
	Panel_item      clear_item;
	int             type;
	int             destroy;
	struct compose_data *next;
}               POST_WIN_DATA;

POST_WIN_DATA  *post_first_ptr;

#ifdef FOR_INTERNAL_USE
/* To avoid user to send the external ng. */
char           *adm_can_send_ng[] = {
	INTERNAL_NEWS_GROUP,
	NULL
};

int             sb_can_send_flag = FALSE;
#endif

static void     sb_set_post_txt();
static void     sb_set_mail_txt();
static void     part_proc();
static void     full_proc();
static Panel_setting sb_post_kbd_focus();

char           *sb_post_seln_file = "/tmp/SB.post.XXXXXX";

extern int      sb_num_of_article;
extern int      sb_latest_view_line;

extern POST_WIN_DATA *get_post_win();
extern VIEW_DATA *sb_find_view_win();
extern HD_INFO *nntp_get_header_info();

void
sb_post_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	Sb_post_type
		pm_flag = (Sb_post_type) xv_get(menu_item, XV_KEY_DATA, KIND_WIN);
	Sb_post_window_type
		type_flag = (Sb_post_window_type) xv_get(menu_item, XV_KEY_DATA, COPOSE_TYPE);
	int             newsno = -999;
	char            file_name[256];
	POST_WIN_DATA  *win;
	VIEW_DATA      *view_win;
	HD_INFO        *hd_info;
	int             seln_flag;
	char            newsgroup[256];
	char           *ptr;

	(void) xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	if (ptr = (char *) get_current_newsgroup())
		strcpy(newsgroup, ptr);

	if (type_flag == NEW_POST) {
		if (pm_flag == MAIL) {
			if ((win = get_post_win(MAIL)) == NULL) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				  gettext("Can not create the post window"),
#else
					   "Can not create the post window",
#endif
						  NULL);
				(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
				return;
			}
		}
		if (pm_flag == NEWS) {
			if ((hd_info = nntp_get_header_info(newsgroup, newsno)) == NULL) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
						  gettext("Can not get the header informatons"),
#else
				       "Can not get the header informatons",
#endif
						  NULL);
				(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
				return;
			}
			if ((win = get_post_win(NEWS)) == NULL) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				  gettext("Can not create the post window"),
#else
					   "Can not create the post window",
#endif
						  NULL);
				(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
				return;
			}
			(void) sb_set_post_txt(win, hd_info);
		}
	}
	if (type_flag == REPLY_POST) {
		if ((view_win = (VIEW_DATA *) sb_find_view_win()) == NULL) {
			(void) one_notice(sb_frame,
#ifdef OW_I18N
				    gettext("Not view any aritcle before."),
#else
					  "Not view any aritcle before.",
#endif
					  NULL);
			(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
			return;
		}
		if ((hd_info = nntp_get_header_info(
			 view_win->news_group, view_win->newsno)) == NULL) {
			(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the header informatons"),
#else
				       "Can not get the header informatons",
#endif
					  NULL);
			(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
			return;
		}
		if (pm_flag == MAIL) {
			if ((win = get_post_win(MAIL)) == NULL) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				  gettext("Can not create the post window"),
#else
					   "Can not create the post window",
#endif
						  NULL);
				(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
				return;
			}
			(void) sb_set_mail_txt(win, hd_info);
		}
		if (pm_flag == NEWS) {
			if ((win = get_post_win(NEWS)) == NULL) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				  gettext("Can not create the post window"),
#else
					   "Can not create the post window",
#endif
						  NULL);
				(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
				return;
			}
			(void) sb_set_post_txt(win, hd_info);
		}
	}
	if ((type_flag == INDENT_POST) || (type_flag == BRACKETED_POST)) {
		if ((seln_flag =
		     (int) sb_seln_post_set(newsgroup, &newsno, type_flag)) == FALSE) {
			if ((view_win = (VIEW_DATA *) sb_find_view_win()) == NULL) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				gettext("Not selected the aritcle in pane,"),
				 gettext("or not view any aritcle before."),
#else
					"Not selected the aritcle in pane,",
					  "or not view any aritcle before.",
#endif
						  NULL);
				(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
				return;
			} else {
				strcpy(newsgroup, view_win->news_group);
				newsno = view_win->newsno;
				if ((int) get_view_file(newsgroup, newsno, file_name) == -1) {
					(void) unlink(file_name);
					(void) one_notice(sb_frame,
#ifdef OW_I18N
							  gettext("Can not get the file from server"),
#else
					 "Can not get the file from server",
#endif
							  NULL);
					(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
					return;
				} else {
					if (type_flag == INDENT_POST)
						(void) change_file_to_include(file_name);
					else if (type_flag == BRACKETED_POST)
						(void) change_file_to_bracket(file_name);
				}
			}
		} else
			strcpy(file_name, sb_post_seln_file);

		if ((hd_info = nntp_get_header_info(newsgroup, newsno)) == NULL) {
			(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the header informatons"),
#else
				       "Can not get the header informatons",
#endif
					  NULL);
			(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
			return;
		}
		if (pm_flag == MAIL) {
			if ((win = get_post_win(MAIL)) == NULL) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				  gettext("Can not create the post window"),
#else
					   "Can not create the post window",
#endif
						  NULL);
				(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
				return;
			}
			(void) sb_set_mail_txt(win, hd_info);
		}
		if (pm_flag == NEWS) {
			if ((win = get_post_win(NEWS)) == NULL) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				  gettext("Can not create the post window"),
#else
					   "Can not create the post window",
#endif
						  NULL);
				(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
				return;
			}
			(void) sb_set_post_txt(win, hd_info);
		}
		(void) xv_set(win->textsw,
			      TEXTSW_INSERT_FROM_FILE, file_name,
			      NULL);
		(void) xv_set(win->textsw,
			      TEXTSW_FIRST, 0,
			      TEXTSW_INSERTION_POINT, 0,
			      NULL);
		if (seln_flag == FALSE)
			(void) unlink(file_name);

	}
	(void) xv_set(win->frame,
		      FRAME_CLOSED, FALSE,
		      XV_SHOW, TRUE,
		      NULL);

	(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

static int
sb_seln_post_set(newsgroup, newsno, type_flag)
	char           *newsgroup;
	int            *newsno;
	Sb_post_window_type type_flag;
{
	register int    i, j, k;
	int             num = 0;
	char            file[256];
	int             flag = 0;
	FILE           *fpin, *fpto;
	int             newsnum;
	int             once_of_header = 0;	/* newsno for header */

	for (i = 1; i <= sb_num_of_article; i++) {
		j = change_line_to_isselect(newsgroup, i, AT_SELECT);
		k = change_line_to_isselect(newsgroup, i, AT_SELECT_MANY);
		if (j == -1 || k == -1) {
			(void) sb_selected_check_error(i);
			continue;
		} else if (j > 0 || k > 0) {
			newsnum = (int) change_line_to_anum(newsgroup, i);
			if (newsnum == -1) {
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				 gettext("Can not get the article number."),
#else
					  "Can not get the article number.",
#endif
						  NULL);
				continue;
			}
			if (get_view_file(newsgroup, newsnum, file) == -1) {
				(void) unlink(file);
				continue;
			}
			(void) sb_clear_draw_current_mark(sb_latest_view_line);
			sb_latest_view_line = i;

			if (type_flag == INDENT_POST)
				(void) change_file_to_include(file);
			else if (type_flag == BRACKETED_POST)
				(void) change_file_to_bracket(file);
			else {
				(void) unlink(file);
				continue;
			}

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

			if (once_of_header == 0) {
				*newsno = newsnum;
				once_of_header++;
			}
			if ((fpin = fopen(file, "r")) == NULL)
				goto P_ERROR;

			if (flag == 0) {
				if ((fpto = fopen(sb_post_seln_file, "w")) == NULL)
					goto P_ERROR;
				flag = 1;
			} else {
				if ((fpto = fopen(sb_post_seln_file, "a")) == NULL)
					goto P_ERROR;
			}
			(void) fcopy(fpin, fpto);
			(void) fclose(fpin);
			(void) fclose(fpto);
			(void) unlink(file);
			num++;
		}
	}
	if (num == 0)
		return FALSE;

	return TRUE;

P_ERROR:
	(void) fclose(fpin);
	(void) fclose(fpto);
	(void) unlink(file);

	return FALSE;
}

static void
sb_set_post_txt(win, hd_info)
	POST_WIN_DATA  *win;
	HD_INFO        *hd_info;
{
	extern char    *get_mail_address();
	extern char    *get_mail_path();
	char		tmp_ref[MAXSTRLEN];

	(void) xv_set(win->from_item, PANEL_VALUE, get_mail_address(), NULL);
	(void) xv_set(win->path_item, PANEL_VALUE, get_mail_path(), NULL);
	(void) xv_set(win->organaization_item, PANEL_VALUE, ORGANIZATION, NULL);
	(void) xv_set(win->p_subject_item, PANEL_VALUE, hd_info->subject, NULL);
	(void) xv_set(win->replyto_item, PANEL_VALUE, get_mail_address(), NULL);
	if(hd_info->followup_to[0] == '\0'){
		(void) xv_set(win->newsgroup_item, PANEL_VALUE, 
			hd_info->newsgroup, NULL);
	} else {
		(void) xv_set(win->newsgroup_item, PANEL_VALUE, 
			hd_info->followup_to, NULL);
	}
	if(hd_info->message_id[0] != '\0'){
		strcpy(tmp_ref,(char *)xv_get(win->reference_item,
			PANEL_VALUE));
		strncat(tmp_ref, hd_info->message_id, MAXSTRLEN);
		(void) xv_set(win->reference_item, PANEL_VALUE, 
			tmp_ref, NULL);
	}
}

static void
sb_set_mail_txt(win, hd_info)
	POST_WIN_DATA  *win;
	HD_INFO        *hd_info;
{
	if(hd_info->reply_to[0] == '\0'){
		(void) xv_set(win->to_item, PANEL_VALUE, hd_info->from, NULL);
	} else {
		(void) xv_set(win->to_item, PANEL_VALUE,
			hd_info->reply_to, NULL);
	}
	(void) xv_set(win->m_subject_item, PANEL_VALUE, hd_info->subject, NULL);
	(void) xv_set(win->cc_item, PANEL_VALUE, "", NULL);
	(void) xv_set(win->bcc_item, PANEL_VALUE, "", NULL);
}

static void
sb_set_reference(win, hd_info)
	POST_WIN_DATA  *win;
	HD_INFO        *hd_info;
{
	char            msg[256];

	/*
	 * sprintf(msg, "In article %s %s writes:\n", hd_info->message_id,
	 */
}

POST_WIN_DATA  *
get_post_win(postmail)
	Sb_post_type    postmail;
{
	POST_WIN_DATA  *win, *last_win, *new_win;
	POST_WIN_DATA  *new_make_post_win();

	last_win = NULL;
	win = post_first_ptr;

	while (win) {
		last_win = win;
		win = win->next;
	}

	new_win = new_make_post_win(postmail);
	if (new_win == NULL)
		return NULL;

	if (last_win)
		last_win->next = new_win;
	else
		post_first_ptr = new_win;

	return new_win;
}

POST_WIN_DATA  *
new_make_post_win(postmail)
	Sb_post_type    postmail;
{
	static Notify_value sb_post_win_destroy_func();
	static Notify_value sb_post_win_func();
	POST_WIN_DATA  *new_win;
	int             row = 0;
	static POST_WIN_DATA *sb_make_post_panel();
	static POST_WIN_DATA *sb_make_mail_panel();
	static void     sb_win_panel_layout();
	static Notify_value sb_post_drag_proc();
	void            sb_win_resize_proc();

	new_win = (POST_WIN_DATA *) malloc(sizeof(POST_WIN_DATA));
	new_win->destroy = TRUE;
	new_win->next = NULL;

	if (new_win == NULL) {
		(void) free(new_win);
		return NULL;
	}
	if (postmail == NEWS)
		new_win->type = NEWS;
	else
		new_win->type = MAIL;

	new_win->icon = xv_create(NULL, ICON,
				  ICON_IMAGE, sb_post_image,
				  ICON_MASK_IMAGE, sb_post_mask_image,
				  NULL);

	new_win->frame = (Frame) xv_create(XV_NULL, FRAME,
					   WIN_IS_CLIENT_PANE,
					   XV_SHOW, FALSE,
				 XV_WIDTH, (int) xv_get(sb_frame, XV_WIDTH),
					   FRAME_SHOW_FOOTER, FALSE,
					   FRAME_SHOW_RESIZE_CORNER, TRUE,
					   FRAME_NO_CONFIRM, TRUE,
					   FRAME_ICON, new_win->icon,
				   WIN_CMS, (Cms) xv_get(sb_frame, WIN_CMS),
					   NULL);
	window_check(new_win->frame);

	(void) xv_set(new_win->frame, WIN_CLIENT_DATA, new_win, NULL);

	if (postmail == NEWS) {
#ifdef OW_I18N
		(void) xv_set(new_win->frame, FRAME_LABEL, gettext("Shinbun: Post"), NULL);
#else
		(void) xv_set(new_win->frame, FRAME_LABEL, "Shinbun: Post", NULL);
#endif
#ifdef OW_I18N
		(void) xv_set(new_win->icon, XV_LABEL, gettext("Post"), NULL);
#else
		(void) xv_set(new_win->icon, XV_LABEL, "Post", NULL);
#endif
		new_win = sb_make_post_panel(new_win);
	}
	if (postmail == MAIL) {
#ifdef OW_I18N
		(void) xv_set(new_win->frame, FRAME_LABEL, gettext("Shinbun: Mail"), NULL);
#else
		(void) xv_set(new_win->frame, FRAME_LABEL, "Shinbun: Mail", NULL);
#endif
#ifdef OW_I18N
		(void) xv_set(new_win->icon, XV_LABEL, gettext("Mail"), NULL);
#else
		(void) xv_set(new_win->icon, XV_LABEL, "Mail", NULL);
#endif
		new_win = sb_make_mail_panel(new_win);
	}
	if (postmail == NEWS) {
		(void) xv_set(new_win->dist_item, PANEL_VALUE,
			      DEFAULT_DIST_NAME, NULL);
		(void) xv_set(new_win->sign_item, PANEL_VALUE,
			      DEFAULT_SIG_NAME, NULL);
	}
	new_win->textsw = xv_create(new_win->frame, TEXTSW,
				    WIN_ROWS,
				    (int) sb_get_props_value(Sb_post_height),
				    WIN_COLUMNS,
				    (int) sb_get_props_value(Sb_pane_width),
				    XV_HEIGHT, WIN_EXTEND_TO_EDGE,
				    XV_WIDTH, WIN_EXTEND_TO_EDGE,
				    NULL);

	if ((int) sb_get_props_value(Sb_default_post_header))
		(void) full_proc(new_win);
	else
		(void) part_proc(new_win);

	(void) xv_set(new_win->panel, WIN_CLIENT_DATA, new_win, NULL);
	(void) xv_set(new_win->textsw, WIN_CLIENT_DATA, new_win, NULL);
	(void) sb_win_resize_proc(new_win);

	(void) xv_set(new_win->panel, XV_WIDTH, WIN_EXTEND_TO_EDGE, NULL);
	(void) notify_interpose_event_func(textsw_first(new_win->textsw),
					   sb_post_drag_proc, NOTIFY_SAFE);
	(void) notify_interpose_event_func(new_win->frame,
					   sb_post_win_func, NOTIFY_SAFE);
	(void) notify_interpose_destroy_func(new_win->frame,
					     sb_post_win_destroy_func);

	return new_win;
}

static          Notify_value
sb_post_win_func(frame, event, arg, type)
	Frame           frame;
	Event          *event;
	Notify_arg      arg;
	Notify_event_type type;
{
	Notify_value    value;
	POST_WIN_DATA  *win;
	void            sb_win_resize_proc();

	value = notify_next_event_func(frame, event, arg, NOTIFY_SAFE);

	win = (POST_WIN_DATA *) xv_get(frame, WIN_CLIENT_DATA);
	switch (event_action(event)) {
	case WIN_RESIZE:
		(void) sb_win_resize_proc(win);
		break;
	case ACTION_CLOSE:{
			char            buf[50];
			char           *str;

			(void) strcpy(buf, "To:");
			if (win->type == MAIL) {
				str = (char *) xv_get(win->to_item, PANEL_VALUE);
				if (*str == '\0' || *str == '\n')
					break;
				(void) strncat(buf, str, 7);
			}
			if (win->type == NEWS) {
				str = (char *) xv_get(win->newsgroup_item, PANEL_VALUE);
				if (*str == '\0' || *str == '\n')
					break;
				(void) strncat(buf, str, 7);
			}
			(void) xv_set(win->icon, XV_LABEL, buf, NULL);
			break;
		}
	default:
		break;
	}

	return value;
}

static          Notify_value
sb_post_drag_proc(textsw, event, arg, type)
	Textsw          textsw;
	Event          *event;
	Notify_arg      arg;
	Notify_event_type type;
{

	char            file_name[4096];
	char           *ptr;
	int             result = 1;
	int             edit = FALSE;
	Textsw_status   status;

	POST_WIN_DATA  *win = (POST_WIN_DATA *) xv_get(
				 xv_get(textsw, XV_OWNER), WIN_CLIENT_DATA);

	if (event_action(event) == ACTION_DRAG_LOAD) {
		if (xv_decode_drop(event, file_name, 4096) == -1)
			return (NOTIFY_DONE);

		if (edit = (int) xv_get(win->textsw, TEXTSW_MODIFIED))
#ifdef OW_I18N
			result = two_notice(win->frame,
				      gettext("Confirm"), gettext("Cancel"),
				      gettext("Your file has been edited."),
			     gettext("Do you wish to discard these edits?"),
					    NULL);

#else
			result = two_notice(win->frame,
					    "Confirm", "Cancel",
					    "Your file has been edited.",
				      "Do you wish to discard these edits?",
					    NULL);

#endif
		if (!edit || result) {
			int             notice;
			char            buf[256];

			ptr = file_name;
			while (*ptr && (*ptr != '\t'))
				ptr++;
			*ptr = NULL;

			strcpy(buf, "Would you like");
			strcat(buf, file_name, 100);

			notice = three_notice(sb_frame,
					      "Text", "Encode", "Cancel",
					      buf,
			     "loaded as text or encoded into an enclosure?",
					      NULL);

			if (notice == 1);
			else if (notice == 2)
				sb_encode_file(file_name);
			else if (notice == 3)
				return (NOTIFY_DONE);

			(void) xv_set(win->textsw,
				      TEXTSW_STATUS, &status,
				      TEXTSW_FILE, file_name,
				      TEXTSW_FIRST, 0,
				      TEXTSW_INSERTION_POINT, 0,
				      NULL);

			if (notice == 2)
				(void) unlink(file_name);

			if (status != TEXTSW_STATUS_OKAY)
#ifdef OW_I18N
				one_notice(win->frame,
					   gettext("Can not load file"),
					   NULL);
#else
				one_notice(win->frame,
					   "Can not load file",
					   NULL);
#endif
		}
		return (NOTIFY_DONE);
	}
	return notify_next_event_func(textsw, event, arg, type);

}

static          Notify_value
sb_post_win_destroy_func(client, status)
	Notify_client   client;
	Destroy_status  status;
{
	static int      sb_post_destroy_proc();
	POST_WIN_DATA  *win = (POST_WIN_DATA *) xv_get(
					   (Frame) client, WIN_CLIENT_DATA);
	int             result = 1;

	if (status == DESTROY_CHECKING) {
		if (win->destroy == FALSE)
			return notify_next_destroy_func(client, status);

		if ((int) xv_get(win->textsw, TEXTSW_MODIFIED))
#ifdef OW_I18N
			result = two_notice(win->frame,
				      gettext("Confirm"), gettext("Cancel"),
				      gettext("Your file has been edited."),
			     gettext("Do you wish to destroy this window?"),
					    NULL);
#else
			result = two_notice(win->frame,
					    "Confirm", "Cancel",
					    "Your file has been edited.",
				      "Do you wish to destroy this window?",
					    NULL);
#endif

		if (!result)
			notify_veto_destroy(client);

	} else if (status == DESTROY_CLEANUP) {
		(void) sb_post_destroy_proc((Frame) client);

		return notify_next_destroy_func(client, status);
	}
	return NOTIFY_DONE;
}

static int
sb_post_destroy_proc(frame)
	Frame           frame;
{
	POST_WIN_DATA  *win, *prev;

	prev = NULL;
	win = post_first_ptr;

	while (win) {
		if (frame == win->frame) {
			textsw_reset(win->textsw, 0, 0);
			if (prev)
				prev->next = win->next;
			else
				post_first_ptr = win->next;
			free(win);
			win = NULL;
			break;
		}
		prev = win;
		win = win->next;
	}
	return (TRUE);
}

void
sb_destroy_all_post_win()
{
	POST_WIN_DATA  *win, *tmp;

	win = post_first_ptr;

	while (win) {
		tmp = win->next;
		xv_destroy_safe(win->frame);
		win = tmp;
	}
}

POST_WIN_DATA  *
sb_make_post_panel(win)
	POST_WIN_DATA  *win;
{
	Panel_item      item;
	Menu            menu;
	int             row = 0;
	Font_string_dims dims;
	Font            font;
	int             longest = 0;
	int             theight;
	int             haba;

	static Menu     make_post_include_pulldown();
	static Menu     make_dist_pulldown();
	static Menu     make_send_pulldown();
	static Menu     make_header_pulldown();
	static void     sb_post_textsw_clear_proc();

	win->panel = xv_create(win->frame, PANEL,
			       NULL);
	item = xv_create(win->panel, PANEL_BUTTON,
			 PANEL_LABEL_X, xv_col(win->panel, 0),
			 PANEL_LABEL_Y, xv_row(win->panel, row),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Include"),
#else
			 PANEL_LABEL_STRING, "Include",
#endif
		       PANEL_ITEM_MENU, menu = make_post_include_pulldown(),
			 NULL);
	xv_set(menu, MENU_CLIENT_DATA, item, NULL);
	item = xv_create(win->panel, PANEL_BUTTON,
			 PANEL_LABEL_Y, xv_row(win->panel, row),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Send"),
#else
			 PANEL_LABEL_STRING, "Send",
#endif
			 PANEL_ITEM_MENU, menu = make_send_pulldown(),
			 NULL);
	xv_set(menu, MENU_CLIENT_DATA, item, NULL);
	item = xv_create(win->panel, PANEL_BUTTON,
			 PANEL_LABEL_Y, xv_row(win->panel, row),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Header"),
#else
			 PANEL_LABEL_STRING, "Header",
#endif
			 PANEL_ITEM_MENU, menu = make_header_pulldown(),
			 NULL);
	xv_set(menu, MENU_CLIENT_DATA, item, NULL);
	xv_create(win->panel, PANEL_BUTTON,
		  PANEL_LABEL_Y, xv_row(win->panel, row++),
#ifdef OW_I18N
		  PANEL_LABEL_STRING, gettext("Clear"),
#else
		  PANEL_LABEL_STRING, "Clear",
#endif
		  PANEL_NOTIFY_PROC, sb_post_textsw_clear_proc,
		  NULL);
	item = xv_create(win->panel, PANEL_ABBREV_MENU_BUTTON,
			 PANEL_LABEL_X, xv_col(win->panel, 0),
			 PANEL_LABEL_Y, xv_row(win->panel, row),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Distribution:"),
#else
			 PANEL_LABEL_STRING, "Distribution:",
#endif
			 PANEL_ITEM_MENU, menu = make_dist_pulldown(),
			 NULL);
	xv_set(menu, MENU_CLIENT_DATA, item, NULL);
	win->dist_item = xv_create(win->panel, PANEL_TEXT,
				   PANEL_LABEL_Y, xv_row(win->panel, row),
				   PANEL_VALUE_DISPLAY_LENGTH, 10,
				   PANEL_VALUE_STORED_LENGTH, 100,
				   XV_SHOW, FALSE,
				   NULL);
	xv_create(win->panel, PANEL_MESSAGE,
		  PANEL_LABEL_Y, xv_row(win->panel, row),
#ifdef OW_I18N
		  PANEL_LABEL_STRING, gettext("Signiture:"),
#else
		  PANEL_LABEL_STRING, "Signiture:",
#endif
		  NULL);
	win->sign_item = xv_create(win->panel, PANEL_TEXT,
				   PANEL_LABEL_Y, xv_row(win->panel, row++),
				   PANEL_VALUE_DISPLAY_LENGTH, 20,
				   PANEL_VALUE_STORED_LENGTH, 100,
				   XV_SHOW, FALSE,
				   NULL);
	win->from_item = xv_create(win->panel, PANEL_TEXT,
				   PANEL_LABEL_STRING, "From:",
				   PANEL_VALUE_STORED_LENGTH, 512,
				   NULL);
	win->p_subject_item = xv_create(win->panel, PANEL_TEXT,
					PANEL_LABEL_STRING, "Subject:",
					PANEL_VALUE_STORED_LENGTH, 100,
					NULL);
	win->newsgroup_item = xv_create(win->panel, PANEL_TEXT,
					PANEL_LABEL_STRING, "Newsgroups:",
					PANEL_VALUE_STORED_LENGTH, 100,
					PANEL_NOTIFY_STRING, "\n\r",
					PANEL_NOTIFY_PROC, sb_post_kbd_focus,
					NULL);
	win->path_item = xv_create(win->panel, PANEL_TEXT,
				   PANEL_LABEL_STRING, "Path:",
				   PANEL_VALUE_STORED_LENGTH, 512,
				   NULL);
	win->expires_item = xv_create(win->panel, PANEL_TEXT,
				      PANEL_LABEL_STRING, "Expires:",
				      PANEL_VALUE_STORED_LENGTH, 100,
				      NULL);
	win->reference_item = xv_create(win->panel, PANEL_TEXT,
					PANEL_LABEL_STRING, "References:",
					PANEL_VALUE_STORED_LENGTH, MAXSTRLEN,
					NULL);
	win->sender_item = xv_create(win->panel, PANEL_TEXT,
				     PANEL_LABEL_STRING, "Sender:",
				     PANEL_VALUE_STORED_LENGTH, 100,
				     NULL);
	win->replyto_item = xv_create(win->panel, PANEL_TEXT,
				      PANEL_LABEL_STRING, "Reply-To:",
				      PANEL_VALUE_STORED_LENGTH, 100,
				      NULL);
	win->followupto_item = xv_create(win->panel, PANEL_TEXT,
					 PANEL_LABEL_STRING, "Followup-To:",
					 PANEL_VALUE_STORED_LENGTH, 100,
					 NULL);
	win->organaization_item = xv_create(win->panel, PANEL_TEXT,
					PANEL_LABEL_STRING, "Organization:",
					    PANEL_VALUE_STORED_LENGTH, 100,
					    NULL);
	win->keyword_item = xv_create(win->panel, PANEL_TEXT,
				      PANEL_LABEL_STRING, "Keywords:",
				      PANEL_VALUE_STORED_LENGTH, 100,
				      PANEL_NOTIFY_STRING, "\n\r",
				      PANEL_NOTIFY_PROC, sb_post_kbd_focus,
				      NULL);

	return win;
}

POST_WIN_DATA  *
sb_make_mail_panel(win)
	POST_WIN_DATA  *win;
{
	Panel_item      item;
	Menu            menu;
	int             row = 0;
	Font_string_dims dims;
	Font            font;
	int             longest = 0;
	int             theight;
	int             haba;

	static Menu     make_post_include_pulldown();
	static Menu     make_dist_pulldown();
	static Menu     make_send_pulldown();
	static Menu     make_header_pulldown();
	static void     sb_post_textsw_clear_proc();

	win->panel = xv_create(win->frame, PANEL,
			       NULL);
	item = xv_create(win->panel, PANEL_BUTTON,
			 PANEL_LABEL_X, xv_col(win->panel, 0),
			 PANEL_LABEL_Y, xv_row(win->panel, row),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Include"),
#else
			 PANEL_LABEL_STRING, "Include",
#endif
		       PANEL_ITEM_MENU, menu = make_post_include_pulldown(),
			 NULL);
	xv_set(menu, MENU_CLIENT_DATA, item, NULL);
	item = xv_create(win->panel, PANEL_BUTTON,
			 PANEL_LABEL_Y, xv_row(win->panel, row),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Send"),
#else
			 PANEL_LABEL_STRING, "Send",
#endif
			 PANEL_ITEM_MENU, menu = make_send_pulldown(),
			 NULL);
	xv_set(menu, MENU_CLIENT_DATA, item, NULL);
	item = xv_create(win->panel, PANEL_BUTTON,
			 PANEL_LABEL_Y, xv_row(win->panel, row),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("Header"),
#else
			 PANEL_LABEL_STRING, "Header",
#endif
			 PANEL_ITEM_MENU, menu = make_header_pulldown(),
			 NULL);
	xv_set(menu, MENU_CLIENT_DATA, item, NULL);
	win->clear_item = xv_create(win->panel, PANEL_BUTTON,
				    PANEL_LABEL_Y, xv_row(win->panel, row++),
#ifdef OW_I18N
				    PANEL_LABEL_STRING, gettext("Clear"),
#else
				    PANEL_LABEL_STRING, "Clear",
#endif
			       PANEL_NOTIFY_PROC, sb_post_textsw_clear_proc,
				    NULL);
	win->to_item = xv_create(win->panel, PANEL_TEXT,
				 PANEL_LABEL_STRING, "To:",
				 PANEL_VALUE_STORED_LENGTH, 100,
				 NULL);
	win->m_subject_item = xv_create(win->panel, PANEL_TEXT,
					PANEL_LABEL_STRING, "Subject:",
					PANEL_VALUE_STORED_LENGTH, 100,
					NULL);
	win->cc_item = xv_create(win->panel, PANEL_TEXT,
				 PANEL_LABEL_STRING, "Cc:",
				 PANEL_VALUE_STORED_LENGTH, 100,
				 PANEL_NOTIFY_STRING, "\n\r",
				 PANEL_NOTIFY_PROC, sb_post_kbd_focus,
				 NULL);
	win->bcc_item = xv_create(win->panel, PANEL_TEXT,
				  PANEL_LABEL_STRING, "Bcc:",
				  PANEL_VALUE_STORED_LENGTH, 100,
				  PANEL_NOTIFY_STRING, "\n\r",
				  PANEL_NOTIFY_PROC, sb_post_kbd_focus,
				  NULL);

	return win;
}

static          Menu
make_post_include_pulldown()
{
	Menu            menu;
	Menu_item       tmp_item;
	static void     sb_post_bracket_proc();
	static void     sb_post_seln_inc_proc();
	static void     sb_post_refer_inc_proc();

	menu = (Menu) xv_create(XV_NULL, MENU, NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Bracketed"),
#else
			     MENU_STRING, "Bracketed",
#endif
			     MENU_NOTIFY_PROC, sb_post_bracket_proc,
			 XV_KEY_DATA, INCLUDE_TYPE, (char *) BRACKETED_POST,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Indent"),
#else
			     MENU_STRING, "Indent",
#endif
			     MENU_NOTIFY_PROC, sb_post_bracket_proc,
			     XV_KEY_DATA, INCLUDE_TYPE, (char *) INDENT_POST,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Selected"),
#else
			     MENU_STRING, "Selected",
#endif
			     MENU_NOTIFY_PROC, sb_post_seln_inc_proc,
			  XV_KEY_DATA, INCLUDE_TYPE, (char *) SELECTED_POST,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Reference"),
#else
			     MENU_STRING, "Reference",
#endif
			     MENU_NOTIFY_PROC, sb_post_refer_inc_proc,
		    XV_KEY_DATA, INCLUDE_TYPE, (char *) SELECTED_REFER_POST,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	return menu;
}

static          Menu
make_send_pulldown()
{
	Menu            menu;
	Menu_item       tmp_item;
	static void     sb_send_quit_proc();
	static void     sb_send_close_proc();
	static void     sb_send_clear_proc();
	static void     sb_send_leave_proc();

	menu = (Menu) xv_create(XV_NULL, MENU, NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Quit window"),
#else
			     MENU_STRING, "Quit window",
#endif
			     MENU_NOTIFY_PROC, sb_send_quit_proc,
			 XV_KEY_DATA, INCLUDE_TYPE, (char *) BRACKETED_POST,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Close window"),
#else
			     MENU_STRING, "Close window",
#endif
			     MENU_NOTIFY_PROC, sb_send_close_proc,
			     XV_KEY_DATA, INCLUDE_TYPE, (char *) INDENT_POST,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Clear message"),
#else
			     MENU_STRING, "Clear message",
#endif
			     MENU_NOTIFY_PROC, sb_send_clear_proc,
			     XV_KEY_DATA, INCLUDE_TYPE, (char *) INDENT_POST,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Leave message intact"),
#else
			     MENU_STRING, "Leave message intact",
#endif
			     MENU_NOTIFY_PROC, sb_send_leave_proc,
			     XV_KEY_DATA, INCLUDE_TYPE, (char *) INDENT_POST,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	return menu;
}

static          Menu
make_dist_pulldown()
{
	Menu            menu;
	static void     sb_set_dist_text();

	menu = (Menu) xv_create(NULL, MENU,
				MENU_NOTIFY_PROC, sb_set_dist_text,
				MENU_STRINGS, DEFAULT_DIST_MENU_NAME, NULL,
				NULL);

	return menu;
}

static          Menu
make_header_pulldown()
{
	Menu            menu;
	Menu_item       tmp_item;
	static void     sb_full_head();
	static void     sb_part_head();

	menu = xv_create(XV_NULL, MENU, NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Abbrev"),
#else
			     MENU_STRING, "Abbrev",
#endif
			     MENU_NOTIFY_PROC, sb_part_head,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Full"),
#else
			     MENU_STRING, "Full",
#endif
			     MENU_NOTIFY_PROC, sb_full_head,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	return menu;
}

static void
sb_win_panel_layout(win)
	POST_WIN_DATA  *win;
{
	Panel_item      item, rect_item;
	Font_string_dims dims;
	Font            font;
	int             longest = 0;
	int             highest = 0;
	int             set_y;
	int             row = 2;
	Rect           *rect;

	(void) xv_set(win->panel, XV_SHOW, FALSE, NULL);
	font = xv_get(win->panel, WIN_FONT);
	PANEL_EACH_ITEM(win->panel, item) {
		if (xv_get(item, PANEL_SHOW_ITEM) && (Panel_item_type) xv_get(
			       item, PANEL_ITEM_CLASS) == PANEL_TEXT_ITEM) {
			(void) xv_get(font, FONT_STRING_DIMS,
			  (char *) xv_get(item, PANEL_LABEL_STRING), &dims);
			if (dims.width > longest)
				longest = dims.width;
			if (dims.height > highest)
				highest = dims.height;
		}
	}
	PANEL_END_EACH;
	if (win->type == NEWS) {
		rect = (Rect *) xv_get(win->dist_item, PANEL_ITEM_RECT);
		set_y = rect->r_top + rect->r_height + 5;
	}
	if (win->type == MAIL) {
		rect = (Rect *) xv_get(win->clear_item, PANEL_ITEM_RECT);
		set_y = rect->r_top + rect->r_height + 5;
	}
	PANEL_EACH_ITEM(win->panel, item) {
		if (xv_get(item, PANEL_SHOW_ITEM) && (Panel_item_type) xv_get(
			       item, PANEL_ITEM_CLASS) == PANEL_TEXT_ITEM) {
			(void) xv_set(item,
			     PANEL_VALUE_X, longest + xv_col(win->panel, 2),
				      PANEL_VALUE_Y, set_y,
				      NULL);
			rect_item = item;
			set_y += highest + 5;
		}
	}
	PANEL_END_EACH;
	rect = (Rect *) xv_get(rect_item, PANEL_ITEM_RECT);
	(void) xv_set(win->panel,
		      XV_HEIGHT, rect->r_top + rect->r_height + 5,
		      NULL);
	(void) xv_set(win->textsw,
		      WIN_BELOW, win->panel,
		      XV_HEIGHT, WIN_EXTEND_TO_EDGE,
		      XV_WIDTH, WIN_EXTEND_TO_EDGE,
		      NULL);
	(void) xv_set(win->panel, XV_SHOW, TRUE, NULL);
	(void) window_fit(win->frame);
}

void
sb_win_resize_proc(win)
	POST_WIN_DATA  *win;
{
	int             width;
	int             length;
	Font            font;
	Panel_item      item;

	if (win->type == NEWS) {
		(void) xv_set(win->dist_item, XV_SHOW, FALSE, NULL);
		(void) xv_set(win->sign_item, XV_SHOW, FALSE, NULL);
		width = (int) xv_get(win->panel, XV_WIDTH) -
			(int) xv_get(win->newsgroup_item, PANEL_VALUE_X);
	} else
		width = (int) xv_get(win->panel, XV_WIDTH) -
			(int) xv_get(win->m_subject_item, PANEL_VALUE_X);

	font = (int) xv_get(win->panel, WIN_FONT);
	length = width / (int) xv_get(font, FONT_DEFAULT_CHAR_WIDTH);

	length -= 5;
	PANEL_EACH_ITEM(win->panel, item) {
		if (xv_get(item, PANEL_SHOW_ITEM) && (Panel_item_type) xv_get(
			       item, PANEL_ITEM_CLASS) == PANEL_TEXT_ITEM) {
			xv_set(item, PANEL_VALUE_DISPLAY_LENGTH, length, NULL);
		}
	}
	PANEL_END_EACH;
	if (win->type == NEWS) {
		(void) xv_set(win->dist_item, XV_SHOW, TRUE, NULL);
		(void) xv_set(win->sign_item, XV_SHOW, TRUE, NULL);
	}
}

static void
sb_post_textsw_clear_proc(item, event)
	Panel_item      item;
	Event          *event;
{
	Panel           panel = xv_get(item, PANEL_PARENT_PANEL);
	POST_WIN_DATA  *win = (POST_WIN_DATA *) xv_get(panel,
						       WIN_CLIENT_DATA);
	int             result = 1;

	if ((int) xv_get(win->textsw, TEXTSW_MODIFIED)) {
#ifdef OW_I18N
		result = two_notice(win->frame,
				    gettext("Confirm"), gettext("Cancel"),
				    gettext("Your file has been edited."),
			     gettext("Do you wish to discard these edits?"),
				    NULL);
#else
		result = two_notice(win->frame,
				    "Confirm", "Cancel",
				    "Your file has been edited.",
				    "Do you wish to discard these edits?",
				    NULL);
#endif
	}
	if (result)
		(void) textsw_reset(win->textsw, 0, 0);

}

static void
sb_set_dist_text(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	Panel_item      button = (Panel) xv_get(menu, MENU_CLIENT_DATA);
	POST_WIN_DATA  *win = (POST_WIN_DATA *) xv_get(
				 xv_get(button, XV_OWNER), WIN_CLIENT_DATA);

	(void) xv_set(win->dist_item, PANEL_VALUE,
		      (char *) xv_get(menu_item, MENU_STRING), NULL);

}

static void
sb_full_head(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	Panel_item      button = (Panel) xv_get(menu, MENU_CLIENT_DATA);
	POST_WIN_DATA  *win = (POST_WIN_DATA *) xv_get(
				 xv_get(button, XV_OWNER), WIN_CLIENT_DATA);

	(void) xv_set(win->frame, FRAME_BUSY, TRUE, NULL);
	(void) full_proc(win);
	(void) xv_set(win->frame, FRAME_BUSY, FALSE, NULL);

}

static void
sb_part_head(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	Panel_item      button = (Panel) xv_get(menu, MENU_CLIENT_DATA);
	POST_WIN_DATA  *win = (POST_WIN_DATA *) xv_get(
				 xv_get(button, XV_OWNER), WIN_CLIENT_DATA);

	(void) xv_set(win->frame, FRAME_BUSY, TRUE, NULL);
	(void) part_proc(win);
	(void) xv_set(win->frame, FRAME_BUSY, FALSE, NULL);

}

static void
full_proc(win)
	POST_WIN_DATA  *win;
{
	static void     sb_win_panel_layout();

	if (win->type == NEWS) {
		(void) xv_set(win->dist_item, XV_SHOW, FALSE, NULL);
		(void) xv_set(win->sign_item, XV_SHOW, FALSE, NULL);
		(void) xv_set(win->path_item, XV_SHOW, TRUE, NULL);
		(void) xv_set(win->expires_item, XV_SHOW, TRUE, NULL);
		(void) xv_set(win->reference_item, XV_SHOW, TRUE, NULL);
		(void) xv_set(win->sender_item, XV_SHOW, TRUE, NULL);
		(void) xv_set(win->replyto_item, XV_SHOW, TRUE, NULL);
		(void) xv_set(win->followupto_item, XV_SHOW, TRUE, NULL);
		(void) xv_set(win->organaization_item, XV_SHOW, TRUE, NULL);
		(void) xv_set(win->keyword_item, XV_SHOW, TRUE, NULL);
		(void) sb_win_panel_layout(win);
		(void) xv_set(win->dist_item, XV_SHOW, TRUE, NULL);
		(void) xv_set(win->sign_item, XV_SHOW, TRUE, NULL);
	}
	if (win->type == MAIL) {
		(void) xv_set(win->to_item, XV_SHOW, TRUE, NULL);
		(void) xv_set(win->m_subject_item, XV_SHOW, TRUE, NULL);
		(void) xv_set(win->cc_item, XV_SHOW, TRUE, NULL);
		(void) xv_set(win->bcc_item, XV_SHOW, TRUE, NULL);
		(void) sb_win_panel_layout(win);
	}
}

static void
part_proc(win)
	POST_WIN_DATA  *win;
{

	static void     sb_win_panel_layout();

	if (win->type == NEWS) {
		(void) xv_set(win->dist_item, XV_SHOW, FALSE, NULL);
		(void) xv_set(win->sign_item, XV_SHOW, FALSE, NULL);
		(void) xv_set(win->path_item, XV_SHOW, FALSE, NULL);
		(void) xv_set(win->expires_item, XV_SHOW, FALSE, NULL);
		(void) xv_set(win->reference_item, XV_SHOW, FALSE, NULL);
		(void) xv_set(win->sender_item, XV_SHOW, FALSE, NULL);
		(void) xv_set(win->replyto_item, XV_SHOW, FALSE, NULL);
		(void) xv_set(win->followupto_item, XV_SHOW, FALSE, NULL);
		(void) xv_set(win->organaization_item, XV_SHOW, FALSE, NULL);
		(void) xv_set(win->keyword_item, XV_SHOW, FALSE, NULL);
		(void) sb_win_panel_layout(win);
		(void) xv_set(win->dist_item, XV_SHOW, TRUE, NULL);
		(void) xv_set(win->sign_item, XV_SHOW, TRUE, NULL);
	}
	if (win->type == MAIL) {
		(void) xv_set(win->to_item, XV_SHOW, TRUE, NULL);
		(void) xv_set(win->m_subject_item, XV_SHOW, TRUE, NULL);
		(void) xv_set(win->cc_item, XV_SHOW, TRUE, NULL);
		(void) xv_set(win->bcc_item, XV_SHOW, FALSE, NULL);
		(void) sb_win_panel_layout(win);
	}
}

add_sender_to_line(in_str, out_str, sender)
	char           *in_str;
	char           *out_str;
	char           *sender;
{
	char            buf[MAXSTRLEN];
	char           *tmp = &buf[0];
	int             len;
	register int    i, j;

	if ((int) sb_get_props_value(Sb_include_sender))
		sprintf(buf, "%s%s", sender,
			(char *) sb_get_props_char_value(Sb_indent_prefix));
	else
		strcpy(buf, "%s",
		       (char *) sb_get_props_char_value(Sb_indent_prefix));

	len = (int) strlen(buf);

	/* init */
	for (j = 0; j < len; j++)
		*out_str++ = buf[j];
	*out_str++ = '\n';

	for (j = 0; j < len; j++)
		*out_str++ = buf[j];

	for (i = 0; *in_str; i++) {
		if (*in_str == '\n') {
			*out_str++ = *in_str++;
			for (j = 0; j < len; j++)
				*out_str++ = buf[j];
			continue;
		}
		*out_str++ = *in_str++;
	}

	*out_str++ = '\0';
}

static void
sb_post_seln_inc_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	Panel_item      button = (Panel) xv_get(menu, MENU_CLIENT_DATA);
	POST_WIN_DATA  *win = (POST_WIN_DATA *) xv_get(
				 xv_get(button, XV_OWNER), WIN_CLIENT_DATA);
	VIEW_DATA      *view;
	char           *seln;
	char            sender[256];
	char            changed_seln[BUFSIZ];
	char		tmp_ref[MAXSTRLEN];
        HD_INFO        *hd_info;

	view = view_first_ptr;

	if (view == NULL)
		return;

	while (view) {
		if ((seln = get_text_seln(view->textsw)) != NULL) {
			change_article_to_sender(view->news_group,
						 view->newsno, sender);
#ifdef DEBUG
			printf("sender -> %s\n", sender);
#endif
			add_sender_to_line(seln, changed_seln, sender);
			textsw_insert(win->textsw, changed_seln, strlen(changed_seln));
			return;
		}
		if ((hd_info = nntp_get_header_info(view->news_group, 
			view->newsno)) != NULL) {
        		if(hd_info->message_id[0] != '\0'){
               		   strcpy(tmp_ref,(char *)xv_get(win->reference_item,
                      		 PANEL_VALUE));
	                   strncat(tmp_ref, hd_info->message_id, MAXSTRLEN);
       		           (void) xv_set(win->reference_item, PANEL_VALUE,
               		         tmp_ref, NULL);
			}
        	}
		view = view->next;
	}
}

static void
sb_post_refer_inc_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	Panel_item      button = (Panel) xv_get(menu, MENU_CLIENT_DATA);
	POST_WIN_DATA  *win = (POST_WIN_DATA *) xv_get(
				 xv_get(button, XV_OWNER), WIN_CLIENT_DATA);
	VIEW_DATA      *view;
	char           *file = "/tmp/Sb.selected.XXXXXX";
	char            msg_id[256];
	char            sender[256];
	char            buf[256];
	char		tmp_ref[MAXSTRLEN];
	HD_INFO		*hd_info;

	if ((view = (VIEW_DATA *) sb_find_view_win()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
				  gettext("Not view any aritcle before."),
#else
				  "Not view any aritcle beforg.",
#endif
				  NULL);
		return;
	}
	(void) mktemp(file);
	(void) textsw_store_file(view->textsw, file, 0, 0);
	get_indent_header(file, msg_id, sender);
	sprintf(buf, INDENT_INCLUDE_MSG, msg_id, sender);
	textsw_insert(win->textsw, buf, strlen(buf));
	(void) unlink(file);

	if ((hd_info = nntp_get_header_info(view->news_group,
		view->newsno)) != NULL) {
	        if(hd_info->message_id[0] != '\0'){
			strcpy(tmp_ref,(char *)xv_get(win->reference_item,
				PANEL_VALUE));
			strncat(tmp_ref, hd_info->message_id, MAXSTRLEN);
			(void) xv_set(win->reference_item, PANEL_VALUE,
				tmp_ref, NULL);
		}
        }

}

static void
sb_post_bracket_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	Panel_item      button = (Panel) xv_get(menu, MENU_CLIENT_DATA);
	POST_WIN_DATA  *win = (POST_WIN_DATA *) xv_get(
				 xv_get(button, XV_OWNER), WIN_CLIENT_DATA);
	char           *newsgroup;
	int             newsno;
	char            file_name[256];
	Sb_post_window_type type_flag = (int) xv_get(menu_item, XV_KEY_DATA, INCLUDE_TYPE);
	Textsw_status   status;
	Textsw_index    pos;
	int             seln_flag;
	VIEW_DATA      *view_win;
	char		tmp_ref[MAXSTRLEN];
	HD_INFO		*hd_info;

	if ((newsgroup = (char *) get_current_newsgroup()) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
				  gettext("Can not find the newsgroup."),
#else
				  "Can not find the newsgroup.",
#endif
				  NULL);
		return;
	}
	if ((seln_flag =
	  (int) sb_seln_post_set(newsgroup, &newsno, type_flag)) == FALSE) {
		if ((view_win = (VIEW_DATA *) sb_find_view_win()) == NULL) {
			(void) one_notice(sb_frame,
#ifdef OW_I18N
			       gettext("Not selected the aritcle in pane,"),
				 gettext("or not view any aritcle before."),
#else
					"Not selected the aritcle in pane,",
					  "or not view any aritcle before.",
#endif
					  NULL);
			(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
			return;
		} else {
			strcpy(newsgroup, view_win->news_group);
			newsno = view_win->newsno;
			if ((int) get_view_file(newsgroup, newsno, file_name) == -1) {
				(void) unlink(file_name);
				(void) one_notice(sb_frame,
#ifdef OW_I18N
				gettext("Can not get the file from server"),
#else
					 "Can not get the file from server",
#endif
						  NULL);
				(void) xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
				return;
			} else {
				if (type_flag == INDENT_POST)
					(void) change_file_to_include(file_name);
				else if (type_flag == BRACKETED_POST)
					(void) change_file_to_bracket(file_name);
			}
		}
	} else
		strcpy(file_name, sb_post_seln_file);

	(void) xv_set(win->frame, FRAME_BUSY, TRUE, NULL);
	pos = (Textsw_index) xv_get(win->textsw, TEXTSW_INSERTION_POINT);

	(void) xv_set(win->textsw,
		      TEXTSW_STATUS, &status,
		      TEXTSW_INSERT_FROM_FILE, file_name,
		      NULL);

	if (status != TEXTSW_STATUS_OKAY) {
		(void) one_notice(win->frame,
#ifdef OW_I18N
				  gettext("Can not append file"),
#else
				  "Can not append file",
#endif
				  NULL);
		if (seln_flag == FALSE)
			(void) unlink(file_name);

		(void) xv_set(win->frame, FRAME_BUSY, FALSE, NULL);
		return;
	}
	if (seln_flag == FALSE)
		(void) unlink(file_name);

	(void) textsw_possibly_normalize(win->textsw, pos);
	(void) xv_set(win->textsw,
		      TEXTSW_INSERTION_POINT, pos,
		      NULL);

	if ((hd_info = nntp_get_header_info(newsgroup, newsno)) != NULL) {
		if(hd_info->message_id[0] != '\0'){
			strcpy(tmp_ref,(char *)xv_get(win->reference_item,
				PANEL_VALUE));
			strncat(tmp_ref, hd_info->message_id, MAXSTRLEN);
			(void) xv_set(win->reference_item, PANEL_VALUE,
				tmp_ref, NULL);
		}
        }

	(void) xv_set(win->frame, FRAME_BUSY, FALSE, NULL);

}

static void
sb_send_quit_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	Panel_item      button = (Panel) xv_get(menu, MENU_CLIENT_DATA);
	POST_WIN_DATA  *win = (POST_WIN_DATA *) xv_get(
				 xv_get(button, XV_OWNER), WIN_CLIENT_DATA);
	static int      sb_send_proc();

	if (sb_send_proc(win) < 0)
		return;

	win->destroy = FALSE;

	(void) xv_destroy_safe(win->frame);
}

static void
sb_send_close_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	Panel_item      button = (Panel) xv_get(menu, MENU_CLIENT_DATA);
	POST_WIN_DATA  *win = (POST_WIN_DATA *) xv_get(
				 xv_get(button, XV_OWNER), WIN_CLIENT_DATA);
	static void     sb_post_win_clear();
	static int      sb_send_proc();

	if (sb_send_proc(win) < 0)
		return;

	sb_post_win_clear(win);

	xv_set(win->frame, FRAME_CLOSED, TRUE, NULL);
}

static void
sb_send_clear_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	Panel_item      button = (Panel) xv_get(menu, MENU_CLIENT_DATA);
	POST_WIN_DATA  *win = (POST_WIN_DATA *) xv_get(
				 xv_get(button, XV_OWNER), WIN_CLIENT_DATA);
	static void     sb_post_win_clear();
	static int      sb_send_proc();

	if (sb_send_proc(win) < 0)
		return;

	sb_post_win_clear(win);

	xv_set(win->frame, FRAME_CLOSED, FALSE, NULL);
}

static void
sb_send_leave_proc(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	Panel_item      button = (Panel) xv_get(menu, MENU_CLIENT_DATA);
	POST_WIN_DATA  *win = (POST_WIN_DATA *) xv_get(
				 xv_get(button, XV_OWNER), WIN_CLIENT_DATA);
	static int      sb_send_proc();

	if (sb_send_proc(win) < 0)
		return;

	xv_set(win->frame, FRAME_CLOSED, FALSE, NULL);
}

static int
sb_send_proc(win)
	POST_WIN_DATA  *win;
{
	char           *dist, *sign, *from, *path, *newsgroup, *p_subject;
	char           *expire, *reference, *sender, *replyto, *following;
	char           *orga, *keyword;
	char           *m_subject, *cc, *bcc, *to;
	char           *file = "/tmp/SBs.XXXXXX";
	char           *tfile = "/tmp/SBt.XXXXXX";
	FILE           *fp1, *fp2;
	void            sb_mail_send_proc();
	extern int      nntp_post();
	int             flag;
	char            buf[256];
	struct stat     fstatus;

	if (win->type == NEWS) {
		sign = (char *) xv_get(win->sign_item, PANEL_VALUE);
		if (*sign != '\0' || *sign != '\n') {
			sprintf(buf, "%s/%s", (char *) getenv("HOME"), sign);
			if (stat(buf, &fstatus) < 0) {
#ifdef OW_I18N
				one_notice(win->frame,
				 gettext("Can not find the signature file"),
					   NULL);
#else
				one_notice(win->frame,
					   "Can not find the signature file",
					   NULL);
#endif
				return -1;
			}
		}
	}
	if (win->type == NEWS) {
		dist = (char *) xv_get(win->dist_item, PANEL_VALUE);
		from = (char *) xv_get(win->from_item, PANEL_VALUE);
		path = (char *) xv_get(win->path_item, PANEL_VALUE);
		newsgroup = (char *) xv_get(win->newsgroup_item, PANEL_VALUE);
		p_subject = (char *) xv_get(win->p_subject_item, PANEL_VALUE);
		expire = (char *) xv_get(win->expires_item, PANEL_VALUE);
		reference = (char *) xv_get(win->reference_item, PANEL_VALUE);
		sender = (char *) xv_get(win->sender_item, PANEL_VALUE);
		replyto = (char *) xv_get(win->replyto_item, PANEL_VALUE);
		following = (char *) xv_get(win->followupto_item, PANEL_VALUE);
		orga = (char *) xv_get(win->organaization_item, PANEL_VALUE);
		keyword = (char *) xv_get(win->keyword_item, PANEL_VALUE);
	} else if (win->type == MAIL) {
		to = (char *) xv_get(win->to_item, PANEL_VALUE);
		m_subject = (char *) xv_get(win->m_subject_item, PANEL_VALUE);
		cc = (char *) xv_get(win->cc_item, PANEL_VALUE);
		bcc = (char *) xv_get(win->bcc_item, PANEL_VALUE);
	}
	if (win->type == NEWS) {
		if (*dist == '\0' || *dist == '\n') {
			one_notice(win->frame,
#ifdef OW_I18N
			  gettext("You should fill the Distribution filed"),
#else
				   "You should fill the Distribution filed",
#endif
				   NULL);
			return -1;
		}
		if (*p_subject == '\0' || *p_subject == '\n') {
			one_notice(win->frame,
#ifdef OW_I18N
			       gettext("You should fill the Subject filed"),
#else
				   "You should fill the Subject filed",
#endif
				   NULL);
			return -1;
		}
		if (*newsgroup == '\0' || *newsgroup == '\n') {
			one_notice(win->frame,
#ifdef OW_I18N
			     gettext("You should fill the Newsgroup filed"),
#else
				   "You should fill the Newsgroup filed",
#endif
				   NULL);
			return -1;
		}
#ifdef FOR_INTERNAL_USE
		/* For internal user */
		if (!sb_can_send_flag) {
			int             i = 0, dist_cnt = 0, news_cnt = 0;
			while (adm_can_send_ng[i]) {
				if (strcmp(adm_can_send_ng[i], dist) == 0)
					dist_cnt++;
				if (strstr(newsgroup, adm_can_send_ng[i]))
					news_cnt++;
				i++;
			}
			if (dist_cnt == 0 || news_cnt == 0) {
				one_notice(win->frame,
#ifdef OW_I18N
					   gettext("You can not send to Distribution/Newsgroup."),
#else
					   "You can not send to Distribution/Newsgroup.",
#endif
					   NULL);
				return -1;
			}
		}
#endif	/* FOR_INTERNAL_USE */
	}
	if (win->type == MAIL) {
		if (*to == '\0' || *to == '\n') {
			one_notice(win->frame,
#ifdef OW_I18N
				   gettext("You should fill the To filed"),
#else
				   "You should fill the To filed",
#endif
				   NULL);
			return -1;
		}
	}
	if (mktemp(tfile) == -1) {
		fprintf(stderr, "Can not create %s file\n", tfile);
		return -1;
	}
	(void) textsw_store_file(win->textsw, tfile, 0, 0);
	if (mktemp(file) == -1) {
		fprintf(stderr, "Can not create %s file\n", file);
		(void) unlink(tfile);
		return -1;
	}
	if ((fp1 = fopen(file, "w")) == NULL) {
		fprintf(stderr, "Can not open/write %s file", file);
		(void) unlink(file);
		(void) unlink(tfile);
		return -1;
	}
	if (win->type == NEWS) {
		fprintf(fp1, "From: %s\n", from);
		fprintf(fp1, "Path: %s\n", path);
		fprintf(fp1, "Newsgroups: %s\n", newsgroup);
		fprintf(fp1, "Subject: %s\n", p_subject);
		fprintf(fp1, "Expires: %s\n", expire);
		fprintf(fp1, "References: %s\n", reference);
		fprintf(fp1, "Sender: %s\n", sender);
		fprintf(fp1, "Reply-To: %s\n", replyto);
		fprintf(fp1, "Followup-To: %s\n", following);
		fprintf(fp1, "Distribution: %s\n", dist);
		fprintf(fp1, "Organization: %s\n", orga);
		/*
		 * fprintf(fp1, "Keywords: %s\n\n\n", keyword);
		 */
		fprintf(fp1, "Posting-Front-End: Shinbun -- the OpenLook newsreader\n");
		fprintf(fp1, "Keywords: %s\n\n\n", keyword);
	}
	if (win->type == MAIL) {
		fprintf(fp1, "To: %s\nSubject: %s\nCc: %s\nBcc: %s\n\n",
			to, m_subject, cc, bcc);
	}
	(void) fflush(fp1);
	if ((fp2 = fopen(tfile, "r")) == NULL) {
		fprintf(stderr, "Can not open/write %s file", tfile);
		(void) unlink(file);
		(void) unlink(tfile);
		(void) fclose(fp1);
		return -1;
	}
	fcopy(fp2, fp1);
	(void) fclose(fp1);
	(void) fclose(fp2);
	if (win->type == NEWS) {
		/*
		 * nntp_post, return 0 when success, return -1 when fail.
		 */
		flag = nntp_post(file, sign);
		if (flag == 0)
#ifdef OW_I18N
			(void) xv_set(sb_frame, FRAME_LEFT_FOOTER,
				      gettext("Post successfully"), NULL);
#else
			(void) xv_set(sb_frame, FRAME_LEFT_FOOTER,
				      "Post successfully", NULL);
#endif
		if (flag == -1)
#ifdef OW_I18N
			(void) xv_set(sb_frame, FRAME_LEFT_FOOTER,
				      gettext("Posted fail, sorry"), NULL);
#else
			(void) xv_set(sb_frame, FRAME_LEFT_FOOTER,
				      "Posted fail, sorry", NULL);
#endif
	}
	if (win->type == MAIL)
		sb_mail_send_proc(file);

	(void) unlink(file);
	(void) unlink(tfile);
}

static void
sb_post_win_clear(win)
	POST_WIN_DATA  *win;
{
	if (win->type == NEWS) {
		(void) xv_set(win->p_subject_item, PANEL_VALUE, "", NULL);
		(void) xv_set(win->expires_item, PANEL_VALUE, "", NULL);
		(void) xv_set(win->reference_item, PANEL_VALUE, "", NULL);
		(void) xv_set(win->sender_item, PANEL_VALUE, "", NULL);
		(void) xv_set(win->followupto_item, PANEL_VALUE, "", NULL);
		(void) xv_set(win->keyword_item, PANEL_VALUE, "", NULL);
	} else if (win->type == MAIL) {
		(void) xv_set(win->to_item, PANEL_VALUE, "", NULL);
		(void) xv_set(win->m_subject_item, PANEL_VALUE, "", NULL);
		(void) xv_set(win->cc_item, PANEL_VALUE, "", NULL);
		(void) xv_set(win->bcc_item, PANEL_VALUE, "", NULL);
	}
	textsw_reset(win->textsw, 0, 0);
}

static          Panel_setting
sb_post_kbd_focus(item, event)
	Panel_item      item;
	Event          *event;
{
	Panel           panel = xv_get(item, PANEL_PARENT_PANEL);
	POST_WIN_DATA  *win = (POST_WIN_DATA *) xv_get(panel,
						       WIN_CLIENT_DATA);

	if ((item == win->newsgroup_item) &&
	    ((int) xv_get(win->keyword_item, XV_SHOW) == FALSE)) {
		xv_set(win->textsw, WIN_SET_FOCUS, NULL);
		return (panel_text_notify(item, event));
	} else if ((item == win->cc_item) &&
		   ((int) xv_get(win->bcc_item, XV_SHOW) == FALSE)) {
		xv_set(win->textsw, WIN_SET_FOCUS, NULL);
		return (panel_text_notify(item, event));
	} else if ((item == win->keyword_item) || (item == win->bcc_item)) {
		xv_set(win->textsw, WIN_SET_FOCUS, NULL);
		return (panel_text_notify(item, event));
	} else
		return (panel_text_notify(item, event));

}

/*
 * Send the file to Mail command.
 */
void
sb_mail_send_proc(file)
	char           *file;
{
	register int    i;

	if (vfork() == 0) {
		for (i = getdtablesize(); i > 2; i--)
			(void) close(i);
		(void) close(0);
		(void) open(file, 0);
		execlp("Mail", "Mail", "-t", 0);
		exit(-1);
	}
}


int
sb_frame_drag_proc(window, event)
	Xv_Window       window;
	Event          *event;
{

	char            file_name[4096];
	char           *ptr;
	int             result = 1;
	int             edit = FALSE;
	Textsw_status   status;
	HD_INFO        *hd_info;
	POST_WIN_DATA  *win;
	char            buf[256];

	if (xv_decode_drop(event, file_name, 4096) == -1)
		return FALSE;

	ptr = file_name;
	while (*ptr && (*ptr != '\t'))
		ptr++;
	*ptr = NULL;

	strcpy(buf, "Would you like");
	strcat(buf, file_name, 100);

	result = three_notice(sb_frame,
			      "Text", "Encode", "Cancel",
			      buf,
			      "loaded as text or encoded into an enclosure?",
			      NULL);

	if (result == 1);
	else if (result == 2)
		sb_encode_file(file_name);
	else if (result == 3)
		return TRUE;

	if ((hd_info = nntp_get_header_info(NULL, 0)) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			      gettext("Can not get the header informatons"),
#else
				  "Can not get the header informatons",
#endif
				  NULL);
		return FALSE;
	}
	if ((win = get_post_win(NEWS)) == NULL) {
		(void) one_notice(sb_frame,
#ifdef OW_I18N
				  gettext("Can not create the post window"),
#else
				  "Can not create the post window",
#endif
				  NULL);
		return FALSE;
	}
	(void) sb_set_post_txt(win, hd_info);

	(void) xv_set(win->textsw,
		      TEXTSW_STATUS, &status,
		      TEXTSW_FILE, file_name,
		      TEXTSW_FIRST, 0,
		      TEXTSW_INSERTION_POINT, 0,
		      NULL);

	if (result == 2)
		(void) unlink(file_name);

	if (status != TEXTSW_STATUS_OKAY) {
		one_notice(win->frame,
#ifdef OW_I18N
			   gettext("Can not load file"),
#else
			   "Can not load file",
#endif
			   NULL);
		return FALSE;
	}
	(void) xv_set(win->frame,
		      FRAME_CLOSED, FALSE,
		      XV_SHOW, TRUE,
		      NULL);

	return TRUE;
}

int
sb_encode_file(file_name)
	char           *file_name;
{
	char           *tmp = "/tmp/Sb.encode.XXXXXX";
	char           *compress_file;
	char            buf[256];

	if ((compress_file = strrchr(file_name, '/')) == NULL)
		compress_file = file_name;
	else
		compress_file++;

	mktemp(tmp);

	sprintf(buf, ENCODE, file_name, compress_file, tmp);

	/* no error process */
	(void) system(buf);

	strcpy(file_name, tmp);
}
