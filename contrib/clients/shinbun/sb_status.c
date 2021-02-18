#ifdef sccs
static char     sccsid[] = "@(#)sb_status.c	1.13 91/09/12";
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
#include <xview/text.h>
#include <xview/panel.h>
#include <xview/seln.h>
#include <xview/notice.h>
#include <xview/font.h>
#include <xview/cursor.h>
#include <xview/notify.h>
#include <xview/canvas.h>

#include "sb_tool.h"
#include "sb_item.h"
#include "sb_shinbun.h"
#include "shinbun.h"

static Frame    sb_status_frame;
static Panel_item status_list;

static Notify_value sb_status_pinout_proc();
static Notify_value sb_status_frame_destroy_proc();
static void     sb_set_ng_with_num();
static void     sb_set_status_ng_proc();

/*
 * Make Biff Props Window.
 */
void
make_status_win()
{
	Panel           panel;
	Panel_item      item1;
	Font            font;
	int             row;
	int             length;

	if ((sb_status_frame = xv_create(sb_frame, FRAME_CMD,
#ifdef OW_I18N
					 WIN_USE_IM, FALSE,
#endif
#ifdef OW_I18N
				    FRAME_LABEL, gettext("Shinbun: Status"),
#else
					 FRAME_LABEL, "Shinbun: Status",
#endif
					 FRAME_CMD_PUSHPIN_IN, TRUE,
					 FRAME_SHOW_FOOTER, FALSE,
					 FRAME_SHOW_RESIZE_CORNER, FALSE,
					 NULL)) == NULL) {
#ifdef OW_I18N
		frame_err(sb_frame, gettext("Unable to create sb_status_frame"));
#else
		frame_err(sb_frame, "Unable to create sb_status_frame");
#endif
		return;
	}
	panel = xv_get(sb_status_frame, FRAME_CMD_PANEL);
	font = (int) xv_get(panel, WIN_FONT);

	row = 0;

	length = 50 * (int) xv_get(font, FONT_DEFAULT_CHAR_WIDTH);
	status_list = xv_create(panel, PANEL_LIST,
				XV_X, xv_col(panel, 0),
				XV_Y, xv_row(panel, row),
				PANEL_LIST_WIDTH, length,
				PANEL_LIST_DISPLAY_ROWS, 25,
				PANEL_READ_ONLY, TRUE,
				PANEL_CHOOSE_ONE, TRUE,
				PANEL_NOTIFY_PROC, sb_set_status_ng_proc,
				NULL);

	window_fit(panel);
	window_fit(sb_status_frame);

	notify_interpose_destroy_func(sb_status_frame,
				      sb_status_frame_destroy_proc);

	notify_interpose_event_func(sb_status_frame,
				    sb_status_pinout_proc, NOTIFY_SAFE);

	sb_set_ng_with_num();

	xv_set(sb_status_frame, XV_SHOW, TRUE, 0);

}

static          Notify_value
sb_status_frame_destroy_proc(client, status)
	Notify_client   client;
	Destroy_status  status;
{
	if (status == DESTROY_CHECKING);

	xv_set(client, FRAME_NO_CONFIRM, TRUE, NULL);

	return notify_next_destroy_func(client, status);
}

static          Notify_value
sb_status_pinout_proc(client, event, arg, when)
	Notify_client   client;
	Event          *event;
	Notify_arg      arg;
	Notify_event_type when;
{
	Notify_value    return_value =
	notify_next_event_func(client, event, arg, NOTIFY_SAFE);
	static void     sb_biff_app_props_proc();

	if (event_action(event) == ACTION_PINOUT) {
#ifdef OW_I18N
		if (two_notice((Frame) client, gettext("Yes"), gettext("No"),
			       gettext("If this window is destroyed,"),
			       gettext("you need to create once more."),
			       gettext("Because I am TAKO"),
			       gettext("Really quit ?"),
			       NULL))
#else
		if (two_notice((Frame) client, "Yes", "No",
			       "If this window is destroyed,",
			       "you need to create once more.",
			       "Because I am TAKO",
			       "Really quit ?",
			       NULL))
#endif
			xv_destroy((Frame) client);
	}
	return return_value;
}

static void
sb_set_ng_with_num()
{
	char            buf[256];
	char            list_buf[256];
	FILE           *fp;
	int             j;
	extern char    *NEWSBIFFRC;

	if ((fp = fopen(NEWSBIFFRC, "r")) == NULL) {
#ifdef OW_I18N
		sprintf(buf, gettext("%s file not found"), NEWSBIFFRC);
#else
		sprintf(buf, "%s file not found", NEWSBIFFRC);
#endif
		one_notice(sb_frame, buf, NULL);
		return;
	}
	xv_set(sb_status_frame, FRAME_BUSY, TRUE, NULL);
	xv_set(status_list, XV_SHOW, FALSE, NULL);

	j = 0;
	while (fgets(buf, sizeof(buf), fp) != NULL) {
		(void) rm_colon(buf, list_buf);
		(void) list_insert(status_list, list_buf, j++);
	}

	fclose(fp);

	xv_set(status_list, XV_SHOW, TRUE, NULL);

	xv_set(sb_status_frame, FRAME_BUSY, FALSE, NULL);

}

static void
sb_set_status_ng_proc(item, string, client_data, op, event)
	Panel_item      item;
	char           *string;
	caddr_t         client_data;
	Panel_list_op   op;
	Event          *event;
{
	char            buf[256];
	int             i;

	i = 0;

	while (*string != ' ')
		buf[i++] = *string++;

	buf[i] = '\0';

	xv_set(sb_group_txt, PANEL_VALUE, buf, NULL);
}
