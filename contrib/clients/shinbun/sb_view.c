#ifdef sccs
static char     sccsid[] = "@(#)sb_view.c	1.21 91/09/12";
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
#include <malloc.h>

#include <xview/xview.h>
#include <xview/text.h>
#include <xview/panel.h>
#include <xview/seln.h>
#include <xview/notice.h>
#include <xview/cursor.h>
#include <xview/rect.h>

#include "sb_tool.h"
#include "sb_ui.h"
#include "sb_def.h"
#include "sb_item.h"

int             sb_latest_view_line = -1;
static VIEW_DATA *used_view_show();
static VIEW_DATA *get_new_view_win_MID();

/*
 * Find the view window, whose pin is out.
 */
VIEW_DATA      *
sb_find_view_win()
{
	VIEW_DATA      *view, *last_view;

	last_view = NULL;
	view = view_first_ptr;

	while (view) {
		last_view = view;
		view = view->next;
	}

	if (last_view && (int) xv_get(last_view->frame, FRAME_CMD_PUSHPIN_IN) == FALSE)
		return last_view;
	else
		return NULL;
}

/*
 * Get the best article window.
 */
int
sb_make_article_win(group_name, news_no, latest_line, type)
	char           *group_name;
	int             news_no;
	int             latest_line;
	int             type;
{

	static VIEW_DATA *get_view_ptr_proc();
	VIEW_DATA      *current_win;
	void            sb_adopt_up_position();

	if ((current_win = get_view_ptr_proc(group_name, news_no, type)) == NULL)
		return FALSE;

	if ((int) change_line_to_asread(group_name, latest_line, 1) == -1)
		(void) one_notice(sb_frame,
#ifdef OW_I18N
			       gettext("Can not mark the article as read."),
#else
				  "Can not mark the article as read.",
#endif
				  NULL);

	(void) sb_draw_current_mark(latest_line);
	(void) sb_clear_draw_current_mark(sb_latest_view_line);

	(void) sb_clear_new_mark(latest_line);

	(void) sb_line_force_paint(latest_line);
	/* save the latest line */
	sb_latest_view_line = latest_line;

	(void) xv_set(current_win->frame, WIN_FRONT, XV_SHOW, TRUE, NULL);

	return TRUE;
}

/*
 * When tool is destroyed, all view windows should be freed and destroyed.
 */
void
sb_destroy_all_view_win()
{
	VIEW_DATA      *win, *tmp;

	win = view_first_ptr;

	while (win) {
		(void) textsw_reset(win->textsw);
		(void) free(win->news_group);
		tmp = win->next;
		(void) free(win);
		win = tmp;
	}
}

/*
 * Get the ptr which is used wheather new or old.
 */
static VIEW_DATA *
get_view_ptr_proc(group_name, news_no, type)
	char           *group_name;
	int             news_no;
	Sb_view_seln_type type;	/* Whether SELECT or ADJUST button is pressed */
{
	VIEW_DATA      *win, *new_win, *last_win;
	static VIEW_DATA *get_new_view_win();
	static VIEW_DATA *wsed_view_show();
	int             x, y;

	win = view_first_ptr;

	while (win) {
		/*
		 * If pin of frame is in, look next.
		 */
		if ((int) xv_get(win->frame, FRAME_CMD_PUSHPIN_IN)) {
			win->pinned = TRUE;
			win = win->next;
			continue;
		}
		/*
		 * If the article window of VIEW_DATA *win is under conditon
		 * that pin is out and still diplayed, use the window.
		 */
		if ((int) xv_get(win->frame, XV_SHOW) &&
		    (((int) xv_get(win->frame, FRAME_CMD_PUSHPIN_IN)) == FALSE) &&
		    (type == VSELECT)) {
			win = used_view_show(win, group_name, news_no);
			return win;
		}
		win = win->next;
	}

	last_win = NULL;
	win = view_first_ptr;

	while (win) {
		last_win = win;
		win = win->next;
	}
	if ((new_win = get_new_view_win(group_name, news_no)) == NULL)
		return NULL;

	if (last_win)
		last_win->next = new_win;
	else
		view_first_ptr = new_win;

	(void) sb_adopt_up_position(sb_frame, new_win->frame, &x, &y);
	(void) xv_set(new_win->frame, XV_X, x, XV_Y, y, NULL);

	if (type == VADJUST)
		xv_set(new_win->frame, FRAME_CMD_PUSHPIN_IN, TRUE, NULL);

	return new_win;
}

/*
 * Use old article window.
 */
static VIEW_DATA *
used_view_show(win_data, new_group_name, new_news_no)
	VIEW_DATA      *win_data;
	char           *new_group_name;
	int             new_news_no;
{
	char            buf[128];
	char            file_name[256];
	int             gsize = strlen(new_group_name) + 1;


	win_data->pinned = FALSE;

	if (!get_view_file(new_group_name, new_news_no, file_name)) {
		one_notice(sb_frame,
#ifdef OW_I18N
			   gettext("Can not get the file from server"),
#else
			   "Can not get the file from server",
#endif
			   NULL);
		(void) unlink(file_name);
		return NULL;
	}
	win_data->news_group = (char *) realloc(win_data->news_group, gsize);
	(void) strcpy(win_data->news_group, new_group_name);
	win_data->newsno = new_news_no;

	(void) sprintf(buf,
#ifdef OW_I18N
		       gettext("Group: %s Msg no: %d"),
#else
		       "Group: %s Msg no: %d",
#endif
		       win_data->news_group, win_data->newsno);

	(void) xv_set(win_data->frame, FRAME_LABEL, buf, NULL);

	(void) xv_set(win_data->textsw,
		      TEXTSW_FILE_CONTENTS, file_name,
		      TEXTSW_FIRST, 0,
		      TEXTSW_INSERTION_POINT, 0,
		      NULL);

	(void) unlink(file_name);

	return win_data;
}

/*
 * Make new article window.
 */
static VIEW_DATA *
get_new_view_win(new_group_name, new_news_no)
	char           *new_group_name;
	int             new_news_no;
{
	VIEW_DATA      *win;
	char            buf[128];
	char            file_name[256];
	static void     view_dismiss_proc();
	static Notify_value view_destroy_proc();
	static Notify_value view_pinout_proc();

	win = (VIEW_DATA *) malloc(sizeof(VIEW_DATA));
	win->next = NULL;

	if (win == NULL) {
		free(win);
		return NULL;
	}
	/*
	 * This is propably bad, because this cause confusion if we can not
	 * get the view file from server. Currently I do noting. This makes
	 * the empty view file.
	 */
	if (!get_view_file(new_group_name, new_news_no, file_name)) {
#ifdef OW_I18N
		(void) one_notice(sb_frame, gettext("Can not get the file from server"), NULL);
#else
		(void) one_notice(sb_frame, "Can not get the file from server", NULL);
#endif
		(void) unlink(file_name);
		return NULL;
	}
	win->pinned = FALSE;

	win->frame = xv_create(sb_frame, FRAME_CMD,
			       XV_WIDTH, (int) xv_get(sb_frame, XV_WIDTH),
			       WIN_ROWS,
			       (int) sb_get_props_value(Sb_view_height),
			       FRAME_SHOW_RESIZE_CORNER, TRUE,
			       FRAME_CMD_PUSHPIN_IN, FALSE,
			       FRAME_SHOW_FOOTER, FALSE,
			       FRAME_SHOW_LABEL, TRUE,
			       FRAME_DONE_PROC, view_dismiss_proc,
			       XV_SHOW, FALSE,
			       NULL);

	win->textsw = xv_create(win->frame, TEXTSW,
				XV_X, 0,
				XV_Y, 0,
				XV_WIDTH, WIN_EXTEND_TO_EDGE,
			     XV_HEIGHT, (int) xv_get(win->frame, XV_HEIGHT),
				TEXTSW_FIRST, 0,
				TEXTSW_INSERTION_POINT, 0,
				NULL);

	(void) xv_set(win->textsw, XV_HEIGHT, WIN_EXTEND_TO_EDGE, NULL);
	(void) xv_set(win->frame, WIN_CLIENT_DATA, win, NULL);

	(void) notify_interpose_destroy_func(win->frame, view_destroy_proc);
	(void) notify_interpose_event_func(win->frame,
					   view_pinout_proc, NOTIFY_SAFE);

	win->news_group = (char *) malloc(strlen(new_group_name) + 1);
	(void) strcpy(win->news_group, new_group_name);
	win->newsno = new_news_no;

	(void) sprintf(buf,
#ifdef OW_I18N
		       gettext("Group: %s Msg no: %d"),
#else
		       "Group: %s Msg no: %d",
#endif
		       win->news_group, win->newsno);

	(void) xv_set(win->frame, FRAME_LABEL, buf, NULL);

	(void) xv_set(win->textsw,
		      TEXTSW_FILE_CONTENTS, file_name,
		      TEXTSW_FIRST, 0,
		      TEXTSW_INSERTION_POINT, 0,
		      NULL);
	(void) unlink(file_name);

	return win;
}

sb_view_win_MID(message_ID)
	char           *message_ID;
{
	VIEW_DATA      *last_win, *new_win, *win;
	int             x, y;

	last_win = NULL;
	win = view_first_ptr;

	while (win) {
		last_win = win;
		win = win->next;
	}

	if ((new_win = get_new_view_win_MID(message_ID)) == NULL) {
		char            buf[256];

		sprintf(buf,
#ifdef OW_I18N
			gettext("Couldn't list %s msg ID artilce."),
#else
			"Couldn't list %s msg ID artilce.",
#endif
			message_ID);

		one_notice(sb_frame,
			   buf,
#ifdef OW_I18N
			   gettext("It seems to be gone when expired."),
#else
			   "It seems to be gone when expired.",
#endif
			   NULL);
		return;
	}
	if (last_win)
		last_win->next = new_win;
	else
		view_first_ptr = new_win;

	(void) sb_adopt_up_position(sb_frame, new_win->frame, &x, &y);
	(void) xv_set(new_win->frame, XV_X, x, XV_Y, y, NULL);

	xv_set(new_win->frame, FRAME_CMD_PUSHPIN_IN, TRUE, NULL);

	(void) xv_set(new_win->frame, WIN_FRONT, XV_SHOW, TRUE, NULL);
}

/*
 * Make new article window.
 */
static VIEW_DATA *
get_new_view_win_MID(message_ID)
	char           *message_ID;
{
	VIEW_DATA      *win;
	char            buf[128];
	char            file_name[256];
	static void     view_dismiss_proc();
	static Notify_value view_destroy_proc();
	static Notify_value view_pinout_proc();

	win = (VIEW_DATA *) malloc(sizeof(VIEW_DATA));
	win->next = NULL;

	if (win == NULL) {
		free(win);
		return NULL;
	}
	if (get_view_file_MID(message_ID, file_name) < 0) {
		(void) unlink(file_name);
		return NULL;
	}
	win->pinned = FALSE;

	win->frame = xv_create(sb_frame, FRAME_CMD,
			       XV_WIDTH, (int) xv_get(sb_frame, XV_WIDTH),
			       WIN_ROWS,
			       (int) sb_get_props_value(Sb_view_height),
			       FRAME_SHOW_RESIZE_CORNER, TRUE,
			       FRAME_CMD_PUSHPIN_IN, FALSE,
			       FRAME_SHOW_FOOTER, FALSE,
			       FRAME_SHOW_LABEL, TRUE,
			       FRAME_DONE_PROC, view_dismiss_proc,
			       XV_SHOW, FALSE,
			       NULL);

	win->textsw = xv_create(win->frame, TEXTSW,
				XV_X, 0,
				XV_Y, 0,
				XV_WIDTH, WIN_EXTEND_TO_EDGE,
			     XV_HEIGHT, (int) xv_get(win->frame, XV_HEIGHT),
				TEXTSW_FIRST, 0,
				TEXTSW_INSERTION_POINT, 0,
				NULL);

	(void) xv_set(win->textsw, XV_HEIGHT, WIN_EXTEND_TO_EDGE, NULL);
	(void) xv_set(win->frame, WIN_CLIENT_DATA, win, NULL);

	(void) notify_interpose_destroy_func(win->frame, view_destroy_proc);
	(void) notify_interpose_event_func(win->frame,
					   view_pinout_proc, NOTIFY_SAFE);

	win->news_group = (char *) malloc(strlen(message_ID) + 1);
	(void) strcpy(win->news_group, message_ID);

	(void) sprintf(buf,
#ifdef OW_I18N
		       gettext("MsgID: %s"),
#else
		       "Group: %s",
#endif
		       win->news_group);

	(void) xv_set(win->frame, FRAME_LABEL, buf, NULL);

	(void) xv_set(win->textsw,
		      TEXTSW_FILE_CONTENTS, file_name,
		      TEXTSW_FIRST, 0,
		      TEXTSW_INSERTION_POINT, 0,
		      NULL);
	(void) unlink(file_name);


	return win;
}

/*
 * If pin is in, set IN to the structure. If pin is out, destroy the frame.
 */
static          Notify_value
view_pinout_proc(client, event, arg, when)
	Notify_client   client;
	Event          *event;
	Notify_arg      arg;
	Notify_event_type when;
{
	VIEW_DATA      *win;
	Notify_value    return_value;
	int             result;

	return_value = notify_next_event_func(client, event, arg, NOTIFY_SAFE);

	win = (VIEW_DATA *) xv_get((Frame) client, WIN_CLIENT_DATA);

	if (event_action(event) == ACTION_PINOUT)
		if (win->pinned == FALSE)
			return return_value;
		else {
			if (!(int) xv_get(win->textsw, TEXTSW_MODIFIED))
				(void) xv_destroy(win->frame);
			else
				return return_value;
		}

	if (event_action(event) == ACTION_PININ)
		win->pinned = TRUE;

	return return_value;
}

/*
 * When the article view window is destroyed, using memory should be freed.
 */
static          Notify_value
view_destroy_proc(client, status)
	Notify_client   client;
	Destroy_status  status;
{
	VIEW_DATA      *win, *last_win;

	win = view_first_ptr;
	last_win = NULL;

	if (status == DESTROY_CHECKING) {
		while (win) {
			if ((Frame) client == win->frame) {
				(void) textsw_reset(win->textsw);
				if (last_win)
					last_win->next = win->next;
				else
					view_first_ptr = win->next;

				(void) free(win->news_group);
				(void) free(win);
				win = NULL;
				break;
			}
			last_win = win;
			win = win->next;
		}
	}
	(void) xv_set(client, FRAME_NO_CONFIRM, TRUE, NULL);
	return notify_next_destroy_func(client, status);
}

/*
 * When dismiss, the view is destroyed. After this, view_destroy_proc take
 * care.
 */
static void
view_dismiss_proc(frame)
	Frame           frame;
{
	VIEW_DATA      *win =
	(VIEW_DATA *) xv_get(frame, WIN_CLIENT_DATA);
	int             result;

	if ((int) xv_get(win->textsw, TEXTSW_MODIFIED))
		result = two_notice(win->frame,
#ifdef OW_I18N
				    gettext("Confirm"), gettext("Cancel"),
				    gettext("This window has been edited."),
			     gettext("Do you wish to discard these edits?"),
#else
				    "Confirm", "Cancel",
				    "This window has been edited.",
				    "Do you wish to discard these edits?",
#endif
				    NULL);

	if (!result) {
		xv_set(win->frame,
		       XV_SHOW, TRUE,
		       FRAME_CMD_PUSHPIN_IN, TRUE,
		       NULL);
		return;
	}
	(void) xv_destroy(win->frame);
}

/*
 * Should decide the pop up position.
 */
void
sb_adopt_up_position(pframe, frame, x, y)
	Frame           pframe;
	Frame           frame;
	int            *x;
	int            *y;
{
	int             s_width = DisplayWidth(dpy, screen_no);
	int             s_height = DisplayHeight(dpy, screen_no);
	Rect            prect, rect;
	void            sb_check_dup_pop();
	int             pop_x, pop_y;

	(void) frame_get_rect(pframe, &prect);
	(void) frame_get_rect(frame, &rect);

	if ((s_height - prect.r_top - prect.r_height) > rect.r_height) {
		pop_x = prect.r_left;
		pop_y = prect.r_top + prect.r_height;
		sb_check_dup_pop(frame, s_width, s_height, &pop_x, &pop_y);
	} else if ((s_width - prect.r_left - prect.r_width) > prect.r_left) {
		pop_x = s_width - rect.r_width - 50;
		pop_y = s_height - rect.r_height - 50;
		sb_check_dup_pop(frame, s_width, s_height, &pop_x, &pop_y);
	} else {
		pop_x = 50;
		pop_y = s_height - rect.r_height - 50;
		sb_check_dup_pop(frame, s_width, s_height, &pop_x, &pop_y);
	}
	*x = pop_x;
	*y = pop_y;
}

/*
 * If duplicated, slide
 */
void
sb_check_dup_pop(frame, s_width, s_height, x, y)
	Frame           frame;
	int             s_width, s_height;
	int            *x, *y;
{
	VIEW_DATA      *win, *pin_win;
	Rect            rect;

	win = view_first_ptr;
	pin_win = NULL;

	while (win) {
		if ((int) xv_get(win->frame, FRAME_CMD_PUSHPIN_IN) == TRUE)
			pin_win = win;

		win = win->next;
	}
	if (pin_win) {
		(void) frame_get_rect(pin_win->frame, &rect);

		if ((s_height - (rect.r_top + rect.r_height)) > 10)
			*y = rect.r_top + 10;
		else
			*y = rect.r_top - 10;
		if ((s_width - (rect.r_left + rect.r_width)) > 10)
			*x = rect.r_left + 10;
		else
			*x = rect.r_left - 10;
	}
}
