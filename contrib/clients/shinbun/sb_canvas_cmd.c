#ifdef sccs
static char     sccsid[] = "@(#)sb_canvas_cmd.c	1.17 91/09/13";
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
#include <sys/types.h>
#include <sys/stat.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/canvas.h>
#include <xview/scrollbar.h>
#include <xview/font.h>
#include <xview/cms.h>
#include <xview/xv_xrect.h>
#include <xview/seln.h>
#include <xview/textsw.h>
#include <xview/cursor.h>
#include <xview/fullscreen.h>

#include "sb_tool.h"
#include "sb_def.h"
#include "sb_canvas.h"
#include "shinbun.h"

extern Frame    sb_frame;

extern int      sb_canceled_flag;
extern int      sb_latest_view_line;

static Fullscreen fullscreen;
static struct timeval last_click_time;	/* Double click */
static int      sb_drag_droped = FALSE;
static int      sb_after_drag_drop = FALSE;

struct _INIT_POS {
	int             x;
	int             y;
}               INIT_POS;

/*
 * Set the changed size to canvas when ng is changed.
 */
static void
sb_canvas_size(line)
	int             line;
{
	int             height;
	int             slength =
	(int) xv_get(sb_scrollbar, SCROLLBAR_VIEW_LENGTH);

	if (line < slength)
		line = slength;

	height = line * sb_line_height;

	xv_set(sb_canvas, CANVAS_HEIGHT, height, NULL);

	xv_set(sb_canvas,
	       XV_WIDTH, WIN_EXTEND_TO_EDGE,
	       XV_HEIGHT, WIN_EXTEND_TO_EDGE,
	       NULL);
}

/*
 * Rectangle & Current Mark is/are repainted.
 */
static void
sb_all_line_repaint(newsgroup, start, end)
	char           *newsgroup;
	int             start;
	int             end;
{
	int             i, j, k;

	if (newsgroup == NULL)
		return;

	for (i = start; i < end; i++) {
		j = change_line_to_isselect(newsgroup, i, AT_SELECT);
		k = change_line_to_isselect(newsgroup, i, AT_SELECT_MANY);
		if (j == -1 || k == -1) {
			continue;
		} else if ((j > 0) || (k > 0)) {
			(void) sb_draw_rectangle(i);
			if (i == sb_latest_view_line)
				(void) sb_draw_current_mark(i);
		}
	}
}

/*
 * Rectangle & Current Mark is/are cleared.
 */
static void
sb_all_line_erase(newsgroup, start, end)
	char           *newsgroup;
	int             start;
	int             end;
{
	int             i, j, k;

	if (newsgroup == NULL)
		return;

	for (i = start; i < end; i++) {
		j = change_line_to_isselect(newsgroup, i, AT_SELECT);
		k = change_line_to_isselect(newsgroup, i, AT_SELECT_MANY);
		if (j == -1 || k == -1) {
			continue;
		} else if ((j > 0) || (k > 0)) {
			(void) sb_clear_rectangle(i);
			if (i == sb_latest_view_line)
				(void) sb_draw_current_mark(i);
		}
	}
}

/*
 * Force to re-paint the current view window by the application or scrollbar.
 */
void
sb_line_force_paint(line)
	int             line;
{
	int             starty =
	(int) xv_get(sb_scrollbar, SCROLLBAR_VIEW_START) + 1;
	int             lengthy =
	(int) xv_get(sb_scrollbar, SCROLLBAR_VIEW_LENGTH);
	int             endy =
	(int) xv_get(sb_scrollbar, SCROLLBAR_OBJECT_LENGTH);
	int             pos = starty + lengthy - 1;
	int             new_start;

	if (starty <= line && line <= pos)
		return;

	else if (line < starty) {
		new_start = line - 2;
		if (new_start < 1)
			new_start = 0;

	} else if (pos < line) {
		new_start = line - 7;
		if ((new_start + lengthy) > endy)
			new_start = endy - lengthy;
	} else
		return;

	xv_set(sb_scrollbar, SCROLLBAR_VIEW_START, new_start, NULL);
}

/*
 * It' used at WIN_RESIZE of the frame because it's not able to notify the
 * event of resizing smaller.
 */
static void
sb_canvas_resize_proc(canvas, width, height)
	Canvas          canvas;
	int             width;
	int             height;
{
	Rect           *rect = (Rect *) xv_get(sb_scrollbar, XV_RECT);
	int             sb_width = (int) rect->r_width;

	xv_set(sb_canvas, CANVAS_WIDTH, width, NULL);
	sb_line_width = width - DEFAULT_SPACE_X - sb_width - 5;
}

void
sb_canvas_shrink_proc()
{
	Rect           *rect = (Rect *) xv_get(sb_scrollbar, XV_RECT, 0);
	int             width = (int) xv_get(sb_canvas, XV_WIDTH);
	int             sb_width = (int) rect->r_width;
	int             sbr = (int) xv_get(sb_scrollbar, SCROLLBAR_VIEW_LENGTH);
	int             starty =
	(int) xv_get(sb_scrollbar, SCROLLBAR_VIEW_START) + 1;

	xv_set(sb_scrollbar, SCROLLBAR_PAGE_LENGTH, sbr, NULL);
	xv_set(sb_canvas, CANVAS_WIDTH, width, NULL);

	(void) sb_all_line_erase((char *) get_current_newsgroup(),
				 starty, starty + sbr);

	sb_line_width = width - DEFAULT_SPACE_X - sb_width - 5;

}

void
sb_canvas_rect_get(height, width)
	int            *height;
	int            *width;
{
	int             sb_canvas_char_height =
	(int) sb_get_props_value(Sb_pane_height);
	int             sb_canvas_char_width =
	(int) sb_get_props_value(Sb_pane_width);
	Rect           *sc_rect = (Rect *) xv_get(sb_scrollbar, XV_RECT, 0);
	int             sc_width = (int) sc_rect->r_width;

	*height = sb_line_height * sb_canvas_char_height + DEFAULT_SPACE + 1;

	*width = sb_font_width * sb_canvas_char_width + sc_width + 10;
}

void
sb_canvas_layout_func()
{
	int             height, width;

	sb_canvas_rect_get(&height, &width);

	xv_set(sb_canvas,
	       XV_HEIGHT, height,
	       XV_WIDTH, width,
	       NULL);

	xv_set(sb_canvas,
	       XV_WIDTH, WIN_EXTEND_TO_EDGE,
	       XV_HEIGHT, WIN_EXTEND_TO_EDGE,
	       NULL);

}

void
sb_clear_all_rectangle(newsgroup)
	char           *newsgroup;
{
	register int    i;
	int             j, k;

	for (i = 1; i <= sb_num_of_article; i++) {
		j = change_line_to_isselect(newsgroup, i, AT_SELECT);
		k = change_line_to_isselect(newsgroup, i, AT_SELECT_MANY);
		if (j == -1 || k == -1) {
			continue;
		} else if (j > 0) {
			(void) change_line_to_asselect(newsgroup, i,
						       AT_SELECT, 0);
			(void) sb_clear_rectangle(i);
		} else if (k > 0) {
			(void) change_line_to_asselect(newsgroup, i,
						       AT_SELECT_MANY, 0);
			(void) sb_clear_rectangle(i);
		}
	}
}

void
sb_select_draw_select(newsgroup, line)
	char           *newsgroup;
	int             line;
{
	if (sb_canceled_flag == TRUE) {
		if ((int) change_line_to_iscancel(newsgroup, line)) {
#ifdef OW_I18N
			one_notice(sb_frame, gettext("This has been canceled"), NULL);
#else
			one_notice(sb_frame, "This has been canceled", NULL);
#endif
			return;
		}
	}
	if ((int) change_line_to_isselect(newsgroup, line, AT_SELECT) > 0)
		return;

	(void) sb_clear_all_rectangle(newsgroup);

	if (!sb_select_flag)
		sb_select_flag = (int) seln_acquire(sb_seln_client,
						    SELN_PRIMARY);

	(void) change_line_to_asselect(newsgroup, line,
				       AT_SELECT, 1);

	(void) sb_draw_rectangle(line);
}

void
sb_select_draw_adjust(newsgroup, line)
	char           *newsgroup;
	int             line;
{

	if (sb_canceled_flag == TRUE) {
		if ((int) change_line_to_iscancel(newsgroup, line)) {
#ifdef OW_I18N
			one_notice(sb_frame, gettext("This has been canceled"), NULL);
#else
			one_notice(sb_frame, "This has been canceled", NULL);
#endif
			return;
		}
	}
	if ((int) change_line_to_isselect(newsgroup, line, AT_SELECT) > 0)
		return;

	if ((int) change_line_to_isselect(newsgroup, line, AT_SELECT_MANY) > 0) {
		(void) change_line_to_asselect(newsgroup, line,
					       AT_SELECT_MANY, 0);
		(void) sb_clear_rectangle(line);
		return;
	}
	if (!sb_select_flag)
		sb_select_flag = (int) seln_acquire(sb_seln_client,
						    SELN_PRIMARY);

	(void) change_line_to_asselect(newsgroup, line,
				       AT_SELECT_MANY, 1);
	(void) sb_draw_rectangle(line);
}

void
sb_drag_draw_adjust(newsgroup, line, old_line, basic_line)
	char           *newsgroup;
	int             line;
	int             old_line;
	int             basic_line;
{

	if ((int) change_line_to_isselect(newsgroup, line, AT_SELECT) > 0)
		return;

	if ((int) change_line_to_isselect(newsgroup, line, AT_SELECT_MANY) > 0) {
#ifdef DEBUG
		printf("line %d old_line %d basic_line %d\n",
		       line, old_line, basic_line);
#endif
		if (((line - basic_line) * (line - old_line)) < 0) {
			(void) change_line_to_asselect(newsgroup, old_line,
						       AT_SELECT_MANY, 0);
			(void) sb_clear_rectangle(old_line);
			return;
		}
		if (line == basic_line && line != old_line) {
			(void) change_line_to_asselect(newsgroup, old_line,
						       AT_SELECT_MANY, 0);
			(void) sb_clear_rectangle(old_line);
			return;
		}
		if (line == old_line)
			return;
	} else {
		if (sb_canceled_flag == TRUE)
			return;

		(void) change_line_to_asselect(newsgroup, line,
					       AT_SELECT_MANY, 1);
		(void) sb_draw_rectangle(line);
	}
}

/*
 * Repaint only the part of the view window.
 */
void
sb_canvas_repaint_proc(canvas, paint_window, dpy, xwin, area)
	Canvas          canvas;
	Xv_Window       paint_window;
	Display        *dpy;
	Window          xwin;
	Xv_xrectlist   *area;
{
	register int    i;
	int             y, cnt;
	char           *newsgroup = (char *) get_current_newsgroup();
	int             starty =
	(int) xv_get(sb_scrollbar, SCROLLBAR_VIEW_START) + 1;
	int             lengthy =
	(int) xv_get(sb_scrollbar, SCROLLBAR_VIEW_LENGTH);
	char           *msg;

	if (newsgroup == NULL || *newsgroup == '\n')
		return;

	cnt = starty + lengthy;

	for (i = starty; i < cnt; i++) {
		msg = (char *) change_line_to_header(newsgroup, i);
		if (msg == NULL)
			continue;
		else {
			y = i * sb_line_height;
			(void) sb_draw_text(DEFAULT_SPACE_X, y, msg);

			if ((int) change_line_to_isread(newsgroup, i) == 0)
				(void) sb_draw_new_mark(i);

		}
		if (sb_canceled_flag == TRUE) {
			if ((int) change_line_to_iscancel(newsgroup, i)) {
				(void) sb_clear_new_mark(i);
				(void) sb_draw_cancel_mark(i);
			}
		}
	}

	(void) sb_all_line_repaint(newsgroup, starty, cnt);
}

/*
 * Force to repaint ,reseze the canvas for the new newsgroup.
 */
void
sb_canvas_force_repaint_proc(newsgroup)
	char           *newsgroup;
{

	register int    i, real_paint_cnt;
	char           *msg;

	if (newsgroup == NULL)
		return;

	if ((int) sb_get_props_value(Sb_article_display)) {
		sb_num_of_article = return_num_article(newsgroup);
		if (sb_num_of_article == 0) {
			(void) sb_clear_pane();
			sb_latest_view_line = -1;
			sb_canceled_flag = FALSE;
			return;
		}
	} else {
		sb_num_of_article = (int) return_num_unread(newsgroup);
		if (sb_num_of_article == -1)
			sb_num_of_article = return_num_article(newsgroup);
		else if (sb_num_of_article == 0) {
			(void) sb_clear_pane();
			sb_latest_view_line = -1;
			sb_canceled_flag = FALSE;
			return;
		}
	}
	for (i = 1, real_paint_cnt = 0; i <= sb_num_of_article; i++) {
		msg = (char *) change_line_to_header(newsgroup, i);
		if (msg == NULL)
			continue;
		else
			real_paint_cnt++;
	}
	sb_num_of_article = real_paint_cnt;

	(void) sb_canvas_size(sb_num_of_article);

	(void) sb_clear_pane();

	(void) xv_set(sb_scrollbar,
		      SCROLLBAR_OBJECT_LENGTH, sb_num_of_article,
		      NULL);

	(void) xv_set(sb_scrollbar, SCROLLBAR_VIEW_START, 0, NULL);

	(void) sb_canvas_repaint_proc(sb_canvas, xv_get(sb_canvas, XV_DISPLAY),
				      sb_dpy, sb_draw, NULL);

	sb_latest_view_line = -1;
	sb_canceled_flag = FALSE;
}

/*
 * Handle the left function keys for selection.
 */
int
sb_func_key_proc(client_data, args)
	char           *client_data;
	Seln_function_buffer *args;
{
	Seln_holder    *holder;

	switch (seln_figure_response(args, &holder)) {
	case SELN_IGNORE:
		break;

	case SELN_REQUEST:
		break;

	case SELN_SHELVE:
		(void) sb_copy_file_proc(1);
		break;

	case SELN_FIND:
		break;

	case SELN_DELETE:
		break;
	}
}

/*
 * Copy the selected article to /tmp file for the selection.
 */
static int
sb_copy_file_proc(seln_flag)
	int             seln_flag;
{
	register int    i, j, k;
	int             num = 0;
	char            file[256];
	char           *newsgroup = (char *) get_current_newsgroup();
	FILE           *fpin, *fpto;
	int             newsnum;

	for (i = 1; i <= sb_num_of_article; i++) {
		j = change_line_to_isselect(newsgroup, i, AT_SELECT);
		k = change_line_to_isselect(newsgroup, i, AT_SELECT_MANY);
		if (j == -1 || k == -1) {
			/* ignore errors */
			continue;
		} else if (j > 0 || k > 0) {
			newsnum = (int) change_line_to_anum(newsgroup, i);
			/* ignore errors */
			if (newsnum == -1)
				return 0;

			if (!get_view_file(newsgroup, newsnum, file)) {
				/* ignore errors */
				(void) unlink(file);
				continue;
			}
			/*
			 * (void)
			 * sb_clear_draw_current_mark(sb_latest_view_line);
			 * sb_latest_view_line = i;
			 * 
			 * (void) change_line_to_asread(newsgroup, i, 1); (void)
			 * sb_draw_current_mark(i); (void)
			 * sb_clear_new_mark(i);
			 */

			if ((fpin = fopen(file, "r")) == NULL)
				goto F_ERROR;

			if (num == 0) {
				if ((fpto = fopen(sb_seln_file, "w")) == NULL)
					goto F_ERROR;
			} else {
				if ((fpto = fopen(sb_seln_file, "a")) == NULL)
					goto F_ERROR;
				fprintf(fpto, "\n\n");
			}
			(void) fcopy(fpin, fpto);
			(void) fclose(fpin);
			(void) fclose(fpto);

			(void) unlink(file);
			num++;
		}
	}
	if (num == 0) {
		if ((fpto = fopen(sb_seln_file, "w")) == NULL)
			return 0;
		(void) fclose(fpto);
	}
	if (seln_flag)
		seln_hold_file(SELN_SHELF, sb_seln_file);

	return num;
F_ERROR:
	(void) fclose(fpin);
	(void) fclose(fpto);
	(void) unlink(file);

	return 0;
}

Seln_result
sb_reply_proc(item, context, length)
	Seln_attribute  item;
	Seln_replier_data *context;
	int             length;
{
	int             size, needed;
	char           *destp;
	static FILE    *fp;
	static int      selnflag;
	struct stat     fstatus;

	switch (context->rank) {
	case SELN_PRIMARY:
		break;
	case SELN_SECONDARY:
		return SELN_DIDNT_HAVE;
		break;

	case SELN_SHELF:
		return SELN_DIDNT_HAVE;
		break;

	default:
		return SELN_DIDNT_HAVE;
		break;
	}

	switch (item) {
	case SELN_REQ_CONTENTS_ASCII:
		if (fp == NULL) {
			if (selnflag == 0) {
				if ((int) sb_copy_file_proc(0) == 0)
					return SELN_FAILED;
			}
			if ((fp = fopen(sb_seln_file, "r")) == NULL)
				return SELN_FAILED;
		}
		fread(destp, sizeof(char), needed, fp);

		if (context->context == NULL) {
			if (destp == NULL)
				return (SELN_DIDNT_HAVE);
			context->context = destp;
		}
		size = strlen(context->context);
		destp = (char *) context->response_pointer;

		needed = size + 4;
		if (size % 4 != 0)
			needed += 4 - size % 4;

		if (needed <= length) {
			strcpy(destp, context->context);
			destp += size;
			while ((int) destp % 4 != 0) {
				*destp++ = '\0';
			}
			context->response_pointer = (char **) destp;
			*context->response_pointer++ = 0;
			fclose(fp);
			fp = NULL;
			selnflag = 0;
			return (SELN_SUCCESS);
		} else {
			strncpy(destp, context->context, length);
			destp += length;
			context->response_pointer = (char **) destp;
			context->context += length;
			return (SELN_CONTINUED);
		}
		break;
	case SELN_REQ_YIELD:
		if (sb_select_flag)
			sb_select_flag = SELN_UNKNOWN;
		(void) sb_clear_all_rectangle(
					  (char *) get_current_newsgroup());

		*context->response_pointer++ = (char *) SELN_SUCCESS;
		return (SELN_SUCCESS);
		break;
	case SELN_REQ_BYTESIZE:
		if (selnflag == 0) {
			if ((int) sb_copy_file_proc(0) == 0)
				return SELN_DIDNT_HAVE;
		}
		stat(sb_seln_file, &fstatus);
		*context->response_pointer++ = (char *) fstatus.st_size;
		return (SELN_SUCCESS);
		break;
	case SELN_REQ_END_REQUEST:
		return (SELN_SUCCESS);
		break;

	default:
		return (SELN_UNRECOGNIZED);
		break;
	}
	/* NOTREACHED */
}

void
sb_frame_move(w, h)
	int             w, h;
{
	int             l_w, l_h;

	l_w = (int) ((double) w * 1.5);
	l_h = (int) ((double) h * 1.5);


	xv_set(sb_frame,
	       XV_WIDTH, l_w,
	       XV_HEIGHT, l_h,
	       NULL);

	window_fit(sb_frame);

	sleep(2);

	xv_set(sb_frame,
	       XV_WIDTH, w,
	       XV_HEIGHT, h,
	       NULL);

	window_fit(sb_frame);

}

void
sb_canvas_drag_func()
{
	int             send_file_num;
	Xv_Cursor       cursor;
	Event           event;
	int             data[5];
	XID             xid;
	int             new_x, new_y;
	Rect            frame_rect;
	char            atom_name[32];
	Atom            drop_atom;
	Frame           root_frame;

	sb_drag_droped = FALSE;

	send_file_num = sb_copy_file_proc(0);

	if (send_file_num > 1)
		cursor = articles_cursor;
	else
		cursor = article_cursor;

	fullscreen = xv_create(sb_frame, FULLSCREEN,
			       FULLSCREEN_INPUT_WINDOW, sb_frame,
	/* dose not work on i18nxview contributed to mit */
			       WIN_CURSOR, cursor,
			       FULLSCREEN_SYNC, FALSE,
			       WIN_CONSUME_EVENTS,
			       WIN_MOUSE_BUTTONS, LOC_DRAG, LOC_MOVE, NULL,
			       NULL);

	while (xv_input_readevent(sb_frame, &event, TRUE, FALSE, 0)) {
		if (event_id(&event) == MS_LEFT) {
			data[1] = event_x(&event);
			data[2] = event_y(&event);
			break;
		}
	}

	xv_destroy(fullscreen);
	fullscreen = NULL;

	if ((xid = win_pointer_under(sb_frame,
			    event_x(&event), event_y(&event))) == (XID) 0) {
		frame_msg(sb_frame,
#ifdef OW_I18N
			  gettext("Drag & Drop aborted."));
#else
			  "Drag & Drop aborted.");
#endif
		return;
	}
	frame_get_rect(sb_frame, &frame_rect);
	root_frame = xv_get(sb_frame, XV_ROOT);

	win_translate_xy(sb_frame, root_frame, event_x(&event),
			 event_y(&event), &new_x, &new_y);

	if ((new_x > frame_rect.r_left) &&
	    (new_x < (frame_rect.r_left + frame_rect.r_width)) &&
	    (new_y > frame_rect.r_top) &&
	    (new_y < (frame_rect.r_top + frame_rect.r_height))) {
		frame_msg(sb_frame,
#ifdef OW_I18N
			  gettext("Drag & Drop aborted."));
#else
			  "Drag & Drop aborted.");
#endif
		return;
	}
	sprintf(atom_name, "MAILDROP%d", time(0));

	drop_atom = XInternAtom(xv_get(sb_frame, XV_DISPLAY),
				atom_name, FALSE);

	XChangeProperty(xv_get(sb_frame, XV_DISPLAY),
			xv_get(sb_frame, XV_XID),
			drop_atom, XA_STRING, 8, PropModeReplace,
			(Xv_opaque) sb_seln_file, strlen(sb_seln_file) + 1);

	data[0] = XV_POINTER_WINDOW;
	data[3] = (int) xv_get(sb_frame, XV_XID);
	data[4] = drop_atom;

	xv_send_message(sb_frame, xid, "XV_DO_DRAG_LOAD",
			32, data, 20);
}

/*
 * Notifier on the header canvas.
 */
Notify_value
sb_canvas_proc(window, event, arg)
	Xv_Window       window;
	Event          *event;
	Notify_arg      arg;
{
	int             line;
	struct timeval  click_time;
	char           *newsgroup = (char *) get_current_newsgroup();
	static int      article_drag;
	static int      old_drag_basic_line;
	static int      old_drag_line;

	click_time = event->ie_time;

	if (event_action(event) == ACTION_SELECT) {
		line = event_y(event) / sb_line_height + 1;
		if (line > sb_num_of_article)
			return NOTIFY_DONE;

		sb_drag_droped = FALSE;
		sb_after_drag_drop = FALSE;

		if (event_is_down(event)) {
			/* Double click */
			if (click_time.tv_sec == last_click_time.tv_sec) {
				if ((click_time.tv_usec -
				     last_click_time.tv_usec)
				    < dclick_usec) {
					(void) sb_click_view(newsgroup, line);
					return NOTIFY_DONE;
				}
			} else if (click_time.tv_sec ==
				   (last_click_time.tv_sec + 1)) {
				if (((1000000 - last_click_time.tv_usec) +
				     dclick_usec)
				    < dclick_usec) {
					(void) sb_click_view(newsgroup, line);
					return NOTIFY_DONE;
				}
			}
			last_click_time = click_time;

			if ((int) change_line_to_isselect(newsgroup, line, AT_SELECT) > 0 ||
			    (int) change_line_to_isselect(newsgroup, line, AT_SELECT_MANY) > 0) {
				INIT_POS.x = event_x(event);
				INIT_POS.y = event_y(event);
				xv_set(sb_canvas, WIN_CONSUME_PICK_EVENTS,
				       LOC_DRAG, NULL, NULL);
				xv_set(sb_frame, WIN_CONSUME_PICK_EVENTS,
				       LOC_DRAG, NULL, NULL);

				sb_drag_droped = TRUE;
			} else {
				sb_clear_all_rectangle(newsgroup);
				old_drag_line = old_drag_basic_line = line;
			}

			article_drag = FALSE;

			return NOTIFY_DONE;
			/* end of event_is_down of ACTION_SELECT */
		} else if (event_is_up(event)) {
			if (sb_drag_droped == TRUE || article_drag == TRUE) {
				xv_set(sb_canvas, WIN_IGNORE_PICK_EVENTS,
				       LOC_DRAG, NULL, NULL);
				xv_set(sb_frame, WIN_IGNORE_PICK_EVENTS,
				       LOC_DRAG, NULL, NULL);
				sb_drag_droped = FALSE;
				sb_after_drag_drop = FALSE;
				article_drag = FALSE;
			} else
				sb_select_draw_select(newsgroup, line);

			if (fullscreen)
				xv_destroy(fullscreen);

			frame_msg(sb_frame, "");
			return NOTIFY_DONE;
		}
		/* end of ACTION_SELECT */
	} else if ((event_action(event) == ACTION_ADJUST)) {
		line = event_y(event) / sb_line_height + 1;
		if (line > sb_num_of_article)
			return NOTIFY_DONE;
		if (event_is_down(event))
			sb_drag_droped = TRUE;
		if (event_is_up(event)) {
			(void) sb_select_draw_adjust(newsgroup, line);
			sb_drag_droped = FALSE;
		}
		sb_after_drag_drop = FALSE;
		return NOTIFY_DONE;
		/* end of ACTION_ADJUST */
	} else if (event_id(event) == LOC_DRAG) {
		if (sb_drag_droped == TRUE) {
			if ((abs(event_x(event) - INIT_POS.x) > 5) ||
			    (abs(event_y(event) - INIT_POS.y) > 5)) {
				sb_canvas_drag_func();
				sb_drag_droped = FALSE;
				sb_after_drag_drop = TRUE;
				return NOTIFY_DONE;
			}
		}
		line = event_y(event) / sb_line_height + 1;
		if (line <= sb_num_of_article && sb_after_drag_drop == FALSE) {
			(void) sb_drag_draw_adjust(newsgroup,
				  line, old_drag_line, old_drag_basic_line);
			article_drag = TRUE;
			old_drag_line = line;
		}
		return NOTIFY_DONE;
	} else if (event_id(event) == LOC_MOVE) {
		if (event_is_up(event)) {
			if (fullscreen)
				xv_destroy(fullscreen);
			sb_drag_droped = FALSE;
			sb_after_drag_drop = FALSE;
			return NOTIFY_DONE;
		}
	} else if (event_action(event) == LOC_WINEXIT) {
		if (sb_drag_droped == TRUE) {
			xv_set(sb_canvas, WIN_IGNORE_PICK_EVENTS,
			       LOC_DRAG, NULL, NULL);
			xv_set(sb_frame, WIN_IGNORE_PICK_EVENTS,
			       LOC_DRAG, NULL, NULL);
			sb_drag_droped = FALSE;
			return NOTIFY_DONE;
		}
	} else if (event_action(event) == LOC_WINENTER) {
		win_set_kbd_focus(window, xv_get(window, XV_XID));
		return NOTIFY_DONE;
	} else if (event_action(event) == ACTION_MENU) {
		if (event_is_down(event)) {
			/* MENU SHOW */
			(void) menu_show((Menu) xv_get(sb_canvas,
						       WIN_CLIENT_DATA),
					 window, event, NULL);
			return NOTIFY_DONE;
		}
		/* end of ACTION_MENU */
		/*
		 * Dose not support. } else if(event_action(event) ==
		 * WIN_REPAINT){ return NOTIFY_DONE;
		 */
	} else if (event_action(event) == ACTION_GO_COLUMN_FORWARD) {
		if (event_is_up(event))
			(void) sb_next_proc(NULL, NULL);
		return NOTIFY_DONE;
		/* next_proc */
	} else if (event_action(event) == ACTION_GO_COLUMN_BACKWARD) {
		if (event_is_up(event))
			(void) sb_prev_proc(NULL, NULL);
		return NOTIFY_DONE;
		/* prev_proc */
	} else if (event_action(event) == ACTION_GO_PAGE_FORWARD) {
		if (event_is_up(event))
			(void) sb_super_next_key(NEXT);
		return NOTIFY_DONE;
	} else if (event_action(event) == ACTION_GO_PAGE_BACKWARD) {
		if (event_is_up(event))
			(void) sb_super_next_key(PREV);
		return NOTIFY_DONE;
	} else if (event_action(event) == ACTION_GO_DOCUMENT_START) {
		if (event_is_up(event))
			(void) sb_next_ng_key(PREV);
		return NOTIFY_DONE;
	} else if (event_action(event) == ACTION_GO_DOCUMENT_END) {
		if (event_is_up(event))
			(void) sb_next_ng_key(NEXT);
		return NOTIFY_DONE;
	} else if (event_action(event) == ACTION_PASTE ||
		   event_action(event) == ACTION_COPY) {
		if (event_is_up(event))
			seln_report_event(sb_seln_client, event);
		return NOTIFY_DONE;
		/* copy to shelf */
	} else if (event_action(event) == ACTION_CUT) {
		if (event_is_up(event))
			make_cancel_frame();
		return NOTIFY_DONE;
		/* cancel_proc */
	} else if (event_action(event) == ACTION_FIND_FORWARD) {
		if (event_is_up(event))
			(void) sb_next_subject_find_proc(NULL, NULL);
		return NOTIFY_DONE;
		/* find subject proc */
	} else if (event_action(event) == ACTION_PROPS) {
		if (event_is_up(event))
			make_tool_props_win();
		return NOTIFY_DONE;
		/* props proc */
	} else if (event_action(event) == ACTION_DRAG_LOAD) {
		sb_frame_drag_proc(window, event);
		return NOTIFY_DONE;
	}
	return NOTIFY_DONE;

}

/*
 * This is to catch the event of the canvas and the panel without a break.
 * Because window forcus is not always on the canvas, the event can not be
 * get. Currently dose not support drag & drop.
 */
Notify_value
sb_canvas_key_proc(window, event, arg)
	Xv_Window       window;
	Event          *event;
	Notify_arg      arg;
{

	if (event_is_down(event)) {
		return NOTIFY_DONE;
	}
	switch (event_action(event)) {
	case ACTION_GO_COLUMN_FORWARD:
		if (event_is_up(event))
			(void) sb_next_proc(NULL, NULL);
		break;
	case ACTION_GO_COLUMN_BACKWARD:
		if (event_is_up(event))
			(void) sb_prev_proc(NULL, NULL);
		break;
	case ACTION_GO_PAGE_FORWARD:
		if (event_is_up(event))
			(void) sb_super_next_key(NEXT);
		break;
	case ACTION_GO_PAGE_BACKWARD:
		if (event_is_up(event))
			(void) sb_super_next_key(PREV);
		break;
	case ACTION_GO_DOCUMENT_START:
		if (event_is_up(event))
			(void) sb_next_ng_key(PREV);
		break;
	case ACTION_GO_DOCUMENT_END:
		if (event_is_up(event))
			(void) sb_next_ng_key(NEXT);
		break;
	case ACTION_PASTE:
	case ACTION_COPY:
		if (event_is_up(event))
			seln_report_event(sb_seln_client, event);
		break;
	case ACTION_CUT:
		if (event_is_up(event))
			make_cancel_frame();
		break;
	case ACTION_FIND_FORWARD:
		if (event_is_up(event))
			(void) sb_next_subject_find_proc(NULL, NULL);
		break;
	case ACTION_PROPS:
		if (event_is_up(event))
			make_tool_props_win();
		break;
		/*
		 * case ACTION_SELECT: case ACTION_ADJUST: case ACTION_MENU:
		 * if (event_is_up(event)) frame_msg(sb_frame, ""); break;
		 */
	default:
		break;
	}

	return NOTIFY_DONE;

}
