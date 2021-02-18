#ifdef sccs
static char     sccsid[] = "@(#)sb_tool.c	1.21 91/09/12";
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


#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/textsw.h>
#include <xview/notice.h>
#include <xview/icon.h>
#include <xview/server.h>
#include <xview/seln.h>
#include <xview/screen.h>
#include <xview/cursor.h>

#define NUM_BEEPS	2
#define CHECK_NUM_STRING	10

static short    Sb_image[] = {
#include "news.icon"
};

static short    Sb_mask_image[] = {
#include "news.mask.icon"
};

static short    Sb_arrive_image[] = {
#include "news.arrive.icon"
};

static short    Sb_arrive_image_mask[] = {
#include "news.arrive.mask.icon"
};

static short    Sb_non_arrive_image[] = {
#include "news.non.arrive.icon"
};

static short    Sb_non_arrive_image_mask[] = {
#include "news.non.arrive.mask.icon"
};

static short    Sb_post_image[] = {
#include "post.edit.icon"
};

static short    Sb_post_mask_image[] = {
#include "post.edit.mask.icon"
};

static short    Sb_article_image[] = {
#include "article.cursor"
};

static short    Sb_articles_image[] = {
#include "articles.cursor"
};

Server_image    sb_image;
Server_image    sb_mask_image;
Server_image    sb_arrive_mask_image;
Server_image    sb_arrive_image;
Server_image    sb_non_arrive_image;
Server_image    sb_non_arrive_mask_image;
Server_image    sb_post_image;
Server_image    sb_post_mask_image;
Xv_Cursor       article_cursor;
Xv_Cursor       articles_cursor;

Xv_server       xserver;
Display        *dpy;
int             screen_no;

void
sb_icon_create()
{
	Server_image    svr_image;
	int             x, y;

	sb_image = (Server_image) xv_create(NULL, SERVER_IMAGE,
					    SERVER_IMAGE_BITS, Sb_image,
					    SERVER_IMAGE_DEPTH, 1,
					    XV_WIDTH, ICON_DEFAULT_WIDTH,
					    XV_HEIGHT, ICON_DEFAULT_HEIGHT,
					    NULL);

	sb_mask_image = (Server_image) xv_create(NULL, SERVER_IMAGE,
					   SERVER_IMAGE_BITS, Sb_mask_image,
						 SERVER_IMAGE_DEPTH, 1,
					       XV_WIDTH, ICON_DEFAULT_WIDTH,
					     XV_HEIGHT, ICON_DEFAULT_HEIGHT,
						 NULL);

	sb_arrive_mask_image = (Server_image) xv_create(NULL, SERVER_IMAGE,
				    SERVER_IMAGE_BITS, Sb_arrive_image_mask,
						      SERVER_IMAGE_DEPTH, 1,
					       XV_WIDTH, ICON_DEFAULT_WIDTH,
					     XV_HEIGHT, ICON_DEFAULT_HEIGHT,
							NULL);

	sb_arrive_image = (Server_image) xv_create(NULL, SERVER_IMAGE,
					 SERVER_IMAGE_BITS, Sb_arrive_image,
						   SERVER_IMAGE_DEPTH, 1,
					       XV_WIDTH, ICON_DEFAULT_WIDTH,
					     XV_HEIGHT, ICON_DEFAULT_HEIGHT,
						   NULL);

	sb_non_arrive_image = (Server_image) xv_create(NULL, SERVER_IMAGE,
				     SERVER_IMAGE_BITS, Sb_non_arrive_image,
						       SERVER_IMAGE_DEPTH, 1,
					       XV_WIDTH, ICON_DEFAULT_WIDTH,
					     XV_HEIGHT, ICON_DEFAULT_HEIGHT,
						       NULL);

	sb_non_arrive_mask_image = (Server_image) xv_create(NULL, SERVER_IMAGE,
				SERVER_IMAGE_BITS, Sb_non_arrive_image_mask,
						      SERVER_IMAGE_DEPTH, 1,
					       XV_WIDTH, ICON_DEFAULT_WIDTH,
					     XV_HEIGHT, ICON_DEFAULT_HEIGHT,
							    NULL);

	sb_post_image = (Server_image) xv_create(NULL, SERVER_IMAGE,
					       XV_WIDTH, ICON_DEFAULT_WIDTH,
					     XV_HEIGHT, ICON_DEFAULT_HEIGHT,
					   SERVER_IMAGE_BITS, Sb_post_image,
						 NULL);

	sb_post_mask_image = (Server_image) xv_create(NULL, SERVER_IMAGE,
					       XV_WIDTH, ICON_DEFAULT_WIDTH,
					     XV_HEIGHT, ICON_DEFAULT_HEIGHT,
				      SERVER_IMAGE_BITS, Sb_post_mask_image,
						      NULL);

	x = ICON_DEFAULT_WIDTH / 2;
	y = ICON_DEFAULT_HEIGHT / 2;

	svr_image = (Server_image) xv_create(NULL, SERVER_IMAGE,
					     XV_WIDTH, ICON_DEFAULT_WIDTH,
					     XV_HEIGHT, ICON_DEFAULT_HEIGHT,
					SERVER_IMAGE_BITS, Sb_article_image,
					     NULL);

	article_cursor = (Xv_Cursor) xv_create(XV_NULL, CURSOR,
					       CURSOR_IMAGE, svr_image,
					       CURSOR_XHOT, x,
					       CURSOR_YHOT, y,
					       CURSOR_OP, PIX_SRC ^ PIX_DST,
					       NULL);

	svr_image = (Server_image) xv_create(NULL, SERVER_IMAGE,
					     XV_WIDTH, ICON_DEFAULT_WIDTH,
					     XV_HEIGHT, ICON_DEFAULT_HEIGHT,
				       SERVER_IMAGE_BITS, Sb_articles_image,
					     NULL);

	articles_cursor = (Xv_Cursor) xv_create(XV_NULL, CURSOR,
						CURSOR_IMAGE, svr_image,
						CURSOR_XHOT, x,
						CURSOR_YHOT, y,
						CURSOR_OP, PIX_SRC ^ PIX_DST,
						NULL);

}

void
window_check(win, title)
	Xv_Window       win;
	char           *title;
{
	if (win == NULL) {
		(void) fprintf(stderr, "bugview: Unable to create window %s\n",
			       title);
		exit(1);
	}
}

void
frame_msg(frame, msg)
	Frame           frame;
	char           *msg;
{
	(void) xv_set(frame, FRAME_LEFT_FOOTER, msg, 0);
}

void
frame_err(frame, msg)
	Frame           frame;
	char           *msg;
{
	int             i;

	for (i = 0; i < NUM_BEEPS; i++)
		xv_set(frame, WIN_ALARM, TRUE, NULL);
	(void) xv_set(frame, FRAME_LEFT_FOOTER, msg, 0);
}

void
one_notice(frame, va_alist)
	Frame           frame;
va_dcl
{
	va_list         args;
	Event           ie;
	char           *p;

	int             status, i;
	char           *buf[100];

	i = 0;
	va_start(args);
	while ((p = va_arg(args, char *)) != (char *) 0)
		buf[i++] = p;
	va_end(args);
	buf[i] = (char *) 0;

	status = notice_prompt(frame,
			       &ie,
			       NOTICE_MESSAGE_STRINGS_ARRAY_PTR, buf,
#ifdef OW_I18N
			       NOTICE_BUTTON_YES, gettext("Confirm"),
#else
			       NOTICE_BUTTON_YES, "Confirm",
#endif
			       NULL);

	switch (status) {
	case NOTICE_YES:
		return;
	case NOTICE_NO:
		return;
	case NOTICE_FAILED:
		return;
	}
}

int
two_notice(frame, yes, no, va_alist)
	Frame           frame;
	char           *yes;
	char           *no;
va_dcl
{
	va_list         args;
	Event           ie;
	char           *p;

	int             status, i;
	char           *buf[100];

	i = 0;
	va_start(args);
	while ((p = va_arg(args, char *)) != (char *) 0)
		buf[i++] = p;
	va_end(args);
	buf[i] = (char *) 0;

	status = notice_prompt(frame,
			       &ie,
			       NOTICE_MESSAGE_STRINGS_ARRAY_PTR, buf,
			       NOTICE_BUTTON_YES, yes,
			       NOTICE_BUTTON_NO, no,
			       NULL);

	switch (status) {
	case NOTICE_YES:
		return 1;
	case NOTICE_NO:
		return 0;
	case NOTICE_FAILED:
		return -1;
	}
}

int
three_notice(frame, str1, str2, str3, va_alist)
	Frame           frame;
	char           *str1, *str2, *str3;
va_dcl
{
	va_list         args;
	Event           ie;
	char           *p;

	int             status, i;
	char           *buf[100];

	i = 0;
	va_start(args);
	while ((p = va_arg(args, char *)) != (char *) 0)
		buf[i++] = p;
	va_end(args);
	buf[i] = (char *) 0;

	status = notice_prompt(frame,
			       &ie,
			       NOTICE_MESSAGE_STRINGS_ARRAY_PTR, buf,
			       NOTICE_BUTTON_YES, str1,
			       NOTICE_BUTTON, str2, 2,
			       NOTICE_BUTTON_NO, str3,
			       NULL);

	switch (status) {
	case NOTICE_YES:
		return 1;
	case 2:
		return 2;
	case NOTICE_NO:
		return 3;
	case NOTICE_FAILED:
		return -1;
	}
}

/*
 * FIle copy to another file.
 */
void
fcopy(fpin, fpto)
	FILE           *fpin, *fpto;
{
	int             c;

	while ((c = getc(fpin)) != EOF)
		putc(c, fpto);
}

int
sb_center_items(panel, row, va_alist)
	Panel           panel;
	int             row;
va_dcl
{
	va_list         args;
	int             x, y;
	int             width;
	int             x_gap;
	Rect           *rect;
	int             rcode;
	Panel_item     *item;
	int             cnt;

	cnt = 0;

	x_gap = (int) xv_get(panel, PANEL_ITEM_X_GAP);

	if (row < 0)
		y = -1;
	else
		y = xv_row(panel, row);

	width = 0;
	va_start(args);
	while ((item = (Panel_item *) va_arg(args, Panel_item *)) != NULL) {
		if (y < 0)
			y = (int) xv_get(item, XV_Y);
		rect = (Rect *) xv_get(item, PANEL_ITEM_RECT);
		width += rect->r_width + x_gap;
		cnt++;
	}
	va_end(args);

	if (cnt == 1) {
		va_start(args);
		width += rect->r_width + x_gap;
		x = ((int) xv_get(panel, XV_WIDTH) - rect->r_width) / 2;
		item = (Panel_item *) va_arg(args, Panel_item *);
		rcode = x;
		xv_set(item, XV_X, x, XV_Y, y, 0);
		va_end(args);
		return (rcode);
	}
	width -= x_gap;

	x = ((int) xv_get(panel, XV_WIDTH) - width) / 2;

	if (x < 0)
		x = 0;

	rcode = x;

	va_start(args);
	while ((item = (Panel_item *) va_arg(args, Panel_item *)) != NULL) {
		xv_set(item, XV_X, x, XV_Y, y, 0);
		rect = (Rect *) xv_get(item, PANEL_ITEM_RECT);
		x += rect->r_width + x_gap;
	}
	va_end(args);
	return (rcode);
}

/*
 * Remove the end of the return code.
 */
void
rm_return(ins, outs)
	char           *ins;
	char           *outs;
{
	if (ins == NULL) {
		outs = NULL;
		return;
	}
	while (*ins != '\n') {
		if (*ins == '\0') {
			*outs = '\0';
			return;
		}
		*outs++ = *ins++;
	}
	*outs = '\0';
}

/*
 * Remove the end of the colon.
 */
char           *
rm_colon(str)
	char           *str;
{
	static char     buf[256];
	char           *ptr = &buf[0];

	while (*str != ':') {
		*ptr++ = *str++;
		if (*str == '\0') {
			*ptr++ = '\0';
			return NULL;
		}
	}
	*ptr++ = '\0';

	return buf;
}

void
get_screen(frame)
	Frame           frame;
{
	Xv_Screen       screen;


	xserver = (Xv_Server) xv_get(xv_get(frame, XV_SCREEN), SCREEN_SERVER);

	dpy = (Display *) xv_get(frame, XV_DISPLAY);
	screen = (Xv_Screen) xv_get(frame, XV_SCREEN);

	screen_no = (int) xv_get(screen, SCREEN_NUMBER);

}

/*
 * Add the string to the next postion of the current list item. If this is
 * found in the panel list, return 0 and dose not add this.
 */
int
list_add_next(item, str)
	Panel_item      item;
	char           *str;
{
	int             cnt = xv_get(item, PANEL_LIST_NROWS);
	int             i;
	int             insert_point;


	if (str == NULL)
		return;

	if (cnt == 0) {
		xv_set(item,
		       PANEL_LIST_INSERT, 0,
		       PANEL_LIST_STRING, 0, str,
		       PANEL_LIST_SELECT, 0, TRUE,
		       NULL);
		return TRUE;
	}
	for (i = 0; i < cnt; i++) {
		if (strcmp(str, (char *) xv_get(item, PANEL_LIST_STRING, i)) == 0) {
			return FALSE;
		}
	}
	for (i = 0; i < cnt; i++) {
		if (xv_get(item, PANEL_LIST_SELECTED, i)) {
			insert_point = i + 1;
			break;
		}
	}
	xv_set(item,
	       PANEL_LIST_INSERT, insert_point,
	       PANEL_LIST_STRING, insert_point, str,
	       NULL);

	return TRUE;
}

/*
 * Add the string to the prev postion of the current list item. If this is
 * found in the panel list, return FALSE and dose not add this.
 */
int
list_add_prev(item, str)
	Panel_item      item;
	char           *str;
{
	int             cnt = xv_get(item, PANEL_LIST_NROWS);
	int             i;
	int             insert_point;

	if (str == NULL)
		return FALSE;

	if (cnt == 0) {
		xv_set(item,
		       PANEL_LIST_INSERT, 0,
		       PANEL_LIST_STRING, 0, str,
		       PANEL_LIST_SELECT, 0, TRUE,
		       NULL);
		return TRUE;
	}
	for (i = 0; i < cnt; i++) {
		if (strcmp(str, (char *) xv_get(item, PANEL_LIST_STRING, i)) == 0) {
			return FALSE;
		}
	}
	for (i = 0; i < cnt; i++) {
		if (xv_get(item, PANEL_LIST_SELECTED, i)) {
			insert_point = i;
			break;
		}
	}
	xv_set(item,
	       PANEL_LIST_INSERT, insert_point,
	       PANEL_LIST_STRING, insert_point, str,
	       NULL);

	return TRUE;
}

/*
 * Delete the current selected string in the panel list.
 */
void
list_delete(item)
	Panel_item      item;
{
	int             cnt = xv_get(item, PANEL_LIST_NROWS);
	int             i;
	int             delete_list[1000];	/* If over 1000, dump core ! */
	int             delete_cnt;

	delete_cnt = 0;

	for (i = 0; i < cnt; i++)
		if (xv_get(item, PANEL_LIST_SELECTED, i))
			delete_list[delete_cnt++] = i;

	if (delete_cnt == 0)
		return;

	for (i = delete_cnt - 1; i >= 0; i--)
		xv_set(item,
		       PANEL_LIST_DELETE, delete_list[i],
		       NULL);

}

/*
 * Insert str at the pos position of the panel list.
 */
void
list_insert(item, str, pos)
	Panel_item      item;
	char           *str;
	int             pos;
{
	xv_set(item,
	       PANEL_LIST_INSERT, pos,
	       PANEL_LIST_STRING, pos, str,
	       NULL);
}

/*
 * Delete all list items
 */
void
list_all_delete(item)
	Panel_item      item;
{
	int             cnt = xv_get(item, PANEL_LIST_NROWS);

	xv_set(item, XV_SHOW, FALSE, NULL);
	for (cnt - 1; cnt >= 0; cnt--)
		xv_set(item,
		       PANEL_LIST_DELETE, cnt,
		       NULL);
	xv_set(item, XV_SHOW, TRUE, NULL);
}

int
list_find_dup(item, str)
	Panel_item      item;
	char           *str;
{
	int             i;
	int             cnt = (int) xv_get(item, PANEL_LIST_NROWS);

	for (i = 0; i < cnt; i++) {
		char            buf[256];
		rm_return((char *) xv_get(item, PANEL_LIST_STRING, i), buf);
		if (strcmp(str, buf) == 0) {
			return FALSE;
		}
	}

	return TRUE;
}

int
add_after_proc(list_item, str, warn_frame)
	Panel_item      list_item;
	char           *str;
	Frame           warn_frame;
{
	if (!list_add_next(list_item, str)) {
		char            buf[256];

#ifdef OW_I18N
		sprintf(buf, gettext("%s is already existed."), str);
#else
		sprintf(buf, "%s is already existed.", str);
#endif
		(void) one_notice(warn_frame, buf, NULL);

		return FALSE;
	}
	return TRUE;
}

int
add_before_proc(list_item, str, warn_frame)
	Panel_item      list_item;
	char           *str;
	Frame           warn_frame;
{
	if (!list_add_prev(list_item, str)) {
		char            buf[256];

#ifdef OW_I18N
		sprintf(buf, gettext("%s is already existed."), str);
#else
		sprintf(buf, "%s is already existed.", str);
#endif
		(void) one_notice(warn_frame, buf, NULL);

		return FALSE;
	}
	return TRUE;
}

void
pop_up_beside(pframe, frame)
	Frame           pframe;
	Frame           frame;
{
	int             s_width = DisplayWidth(dpy, screen_no);
	int             s_height = DisplayHeight(dpy, screen_no);
	Rect            prect, rect;

	(void) frame_get_rect(pframe, &prect);
	(void) frame_get_rect(frame, &rect);

	if ((s_width - (prect.r_left + prect.r_width + rect.r_width)) > 0)
		rect.r_left = prect.r_left + prect.r_width;
	else
		rect.r_left = prect.r_left - rect.r_width;

	rect.r_top = prect.r_top;

	(void) frame_set_rect(frame, &rect);
}

/*
 * Change low & upper char to low.
 */
char           *
is_to_low(str)
	char           *str;
{
	static char     buf[256];
	char           *tmp;

	tmp = &buf[0];

	while (*str) {
		if (isupper(*str)) {
			*tmp++ = *str | 040;
			*str++;
		} else
			*tmp++ = *str++;
	}

	*tmp = '\0';

	return buf;
}

/*
 * Destroy frame and NULL.
 */
void
frame_destroy_proc(frame)
	Frame           frame;
{
	xv_destroy_safe(frame);
	frame = NULL;
}

/*
 * Free xv_key_data.
 */
void
free_key_data(object, key, data)
	Xv_object       object;
	int             key;
	caddr_t         data;
{
	free(data);
}

/*
 * Get the current selection buffer by seln service. Selection buffer is
 * returned.
 */
char           *
get_text_seln(textsw)
	Textsw          textsw;
{
	Seln_holder     holder;
	Seln_request   *response;
	register char  *ptr;
	static char     selection_buf[BUFSIZ];

	holder = selection_inquire(xserver, SELN_PRIMARY);

	if (seln_holder_same_client(&holder, textsw)) {
		response = selection_ask(xserver, &holder,
					 SELN_REQ_CONTENTS_ASCII, NULL,
					 NULL);

		ptr = response->data + sizeof(SELN_REQ_CONTENTS_ASCII);

		(void) strcpy(selection_buf, ptr);

		return selection_buf;

	} else {
		/*
		 * Nothing selected text in the textsw.
		 */
		return NULL;
	}
}
