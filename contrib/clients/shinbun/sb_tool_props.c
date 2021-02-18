#ifdef sccs
static char     sccsid[] = "@(#)sb_tool_props.c	1.22 91/09/12";
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


/*
 * Property window of tool properties and argument parsing are handle in this
 * file.
 */

#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/font.h>
#include <xview/cursor.h>

#include "sb_tool.h"
#include "sb_def.h"
#include "config.h"
#include "sb_shinbun.h"

#define MAX_PROPS_ITEM 22

extern Frame    sb_frame;

static Frame    sb_tool_props_frame;
static Panel    sb_tool_props_panel;
static Panel_item sb_tool_display_item;
static Panel_item sb_tool_apply_item;
static Panel_item sb_tool_reset_item;

static int      sb_props_update;

char           *sb_rc_file = ".shinbunrc";

int             sb_get_props_value();
char           *sb_get_props_char_value();

static void     sb_tool_apply_proc();
static void     sb_tool_reset_proc();
static void     sb_tool_change_proc();
static void     sb_show_panel();
static void     sb_set_char_props();
static void     sb_set_global_val();

typedef enum {
	GENERAL,
	BIFF
}               Sb_tool_props_type;

struct _sb_tool_value {
	char           *char_value;
	int             int_value;
}               sb_tool_value[MAX_PROPS_ITEM];

struct _tool_each_item {
	Xv_pkg         *pkg;
	Panel_item      item;
	char           *label_str;
	char           *file_str;
	char           *def_char_cvalue;
	int             def_int_value;
	void            (*notify_proc) ();
	int             max_val;
	int             min_val;
	int             display_val;
	int             stored_val;
	Sb_tool_props_type face;
}               tool_each_item[] = {
	/* 0 */
	PANEL_NUMERIC_TEXT,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"Default Header Pane Height:",
#endif
	"winheight=",
	NULL,
	10,
	sb_tool_change_proc,
	100,
	0,
	5,
	5,
	GENERAL,

	/* 1 */
	PANEL_MESSAGE,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"rows",
#endif
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	0,
	0,
	GENERAL,

	/* 2 */
	PANEL_NUMERIC_TEXT,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"Default Header Pane Width:",
#endif
	"winwidth=",
	NULL,
	80,
	sb_tool_change_proc,
	200,
	5,
	5,
	5,
	GENERAL,

	/* 3 */
	PANEL_MESSAGE,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"colums",
#endif
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	0,
	0,
	GENERAL,

	/* 4 */
	PANEL_NUMERIC_TEXT,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"Default Header Post Height:",
#endif
	"postheight=",
	NULL,
	30,
	sb_tool_change_proc,
	50,
	5,
	5,
	5,
	GENERAL,

	/* 5 */
	PANEL_MESSAGE,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"rows",
#endif
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	0,
	0,
	GENERAL,

	/* 6 */
	PANEL_NUMERIC_TEXT,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"Default Header VIew Height:",
#endif
	"viewheight=",
	NULL,
	30,
	sb_tool_change_proc,
	50,
	5,
	5,
	5,
	GENERAL,

	/* 7 */
	PANEL_MESSAGE,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"rows",
#endif
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	0,
	0,
	GENERAL,

	/* 8 */
	PANEL_NUMERIC_TEXT,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"Max Header Message Number:",
#endif
	"max_msg=",
	NULL,
	200,
	sb_tool_change_proc,
	2000,
	0,
	5,
	5,
	GENERAL,

	/* 9 */
	PANEL_MESSAGE,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"articles",
#endif
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	0,
	0,
	GENERAL,

	/* 10 */
	PANEL_TEXT,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"Print Script:",
#endif
	"print=",
	"lpr",
	0,
	NULL,
	0,
	0,
	30,
	100,
	GENERAL,

	/* 11 */
	PANEL_CHOICE,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"Dafault Article Display:",
#endif
	"article=",
	NULL,
	0,
	sb_tool_change_proc,
	0,
	0,
	0,
	0,
	GENERAL,

	/* 12 */
	PANEL_CHOICE,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"Post Window with:",
#endif
	"posthead=",
	NULL,
	0,
	sb_tool_change_proc,
	0,
	0,
	0,
	0,
	GENERAL,

	/* 13 */
	PANEL_CHOICE,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"Default Header Message With:",
#endif
	"header_msg=",
	NULL,
	0,
	sb_tool_change_proc,
	0,
	0,
	0,
	0,
	GENERAL,

	/* 14 */
	PANEL_TEXT,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"Default Indent Prefix:",
#endif
	"indent_prefix=",
	"> ",
	0,
	sb_tool_change_proc,
	0,
	0,
	30,
	100,
	GENERAL,

	/* 15 */
	PANEL_NUMERIC_TEXT,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"Check for News Every:",
#endif
	"timer=",
	NULL,
	0,
	sb_tool_change_proc,
	100,
	0,
	5,
	5,
	BIFF,

	/* 16 */
	PANEL_MESSAGE,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"minutes",
#endif
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	0,
	0,
	BIFF,

	/* 17 */
	PANEL_NUMERIC_TEXT,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"Signal New News Width:",
#endif
	"beep=",
	NULL,
	0,
	sb_tool_change_proc,
	20,
	0,
	5,
	5,
	BIFF,

	/* 18 */
	PANEL_MESSAGE,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"beep(s)",
#endif
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	0,
	0,
	BIFF,

	/* 19 */
	PANEL_NUMERIC_TEXT,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	NULL,
#endif
	"move=",
	NULL,
	0,
	sb_tool_change_proc,
	20,
	0,
	5,
	5,
	BIFF,

	/* 20 */
	PANEL_MESSAGE,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"move(s)",
#endif
	NULL,
	NULL,
	0,
	NULL,
	0,
	0,
	0,
	0,
	BIFF,

	/* 21 */
	PANEL_CHOICE,
	NULL,
#ifdef OW_I18N
	NULL,
#else
	"Indent at Include with:",
#endif
	"indent_with_sender=",
	NULL,
	0,
	sb_tool_change_proc,
	0,
	0,
	0,
	0,
	GENERAL

};

#if OW_I18N
void
text_convert()
{
	tool_each_item[0].label_str = (char *) gettext("Default Header Pane Height:");
	tool_each_item[1].label_str = (char *) gettext("rows");
	tool_each_item[2].label_str = (char *) gettext("Default Header Pane Width:");
	tool_each_item[3].label_str = (char *) gettext("colums");
	tool_each_item[4].label_str = (char *) gettext("Default Header Post Height:");
	tool_each_item[5].label_str = (char *) gettext("rows");
	tool_each_item[6].label_str = (char *) gettext("Default Header VIew Height:");
	tool_each_item[7].label_str = (char *) gettext("rows");
	tool_each_item[8].label_str = (char *) gettext("Max Header Message Number:");
	tool_each_item[9].label_str = (char *) gettext("articles");
	tool_each_item[10].label_str = (char *) gettext("Print Script:");
	tool_each_item[11].label_str = (char *) gettext("Dafault Article Display:");
	tool_each_item[12].label_str = (char *) gettext("Post Window with:");
	tool_each_item[13].label_str = (char *) gettext("Default Header Message With:");
	tool_each_item[14].label_str = (char *) gettext("Default Indent Prefix:");
	tool_each_item[15].label_str = (char *) gettext("Check for News Every:");
	tool_each_item[16].label_str = (char *) gettext("minutes");
	tool_each_item[17].label_str = (char *) gettext("Signal New News Width:");
	tool_each_item[18].label_str = (char *) gettext("beep(s)");
	tool_each_item[19].label_str = NULL;
	tool_each_item[20].label_str = (char *) gettext("move(s)");
	tool_each_item[21].label_str = (char *) gettext("Indent at Include with:");
}
#endif
void
make_tool_props_win(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	int             row;
	static void     make_general_panel();
	static int      sb_tool_item_layout();
	static void     sb_props_choice_proc();
	static void     sb_tool_props_panel_event();
	static Notify_value sb_tool_frame_destroy_proc();
	static Notify_value sb_tool_pinout_proc();

	if (sb_tool_props_frame) {
		(void) xv_set(sb_tool_props_frame, WIN_FRONT, XV_SHOW, TRUE, NULL);
		return;
	}
	if ((sb_tool_props_frame = xv_create(sb_frame, FRAME_CMD,
#ifdef OW_I18N
					     WIN_USE_IM, FALSE,
#endif
#ifdef OW_I18N
				   FRAME_LABEL, gettext("Properties: tool"),
#else
					     FRAME_LABEL, "Properties: tool",
#endif
					     FRAME_CMD_PUSHPIN_IN, TRUE,
					     FRAME_SHOW_FOOTER, TRUE,
					     FRAME_SHOW_RESIZE_CORNER, FALSE,
					FRAME_DONE_PROC, frame_destroy_proc,
					     NULL)) == NULL) {
#ifdef OW_I18N
		frame_err(sb_frame, gettext("Unable to create sb_tool_props_frame"));
#else
		frame_err(sb_frame, "Unable to create sb_tool_props_frame");
#endif
		return;
	}
	xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	sb_tool_props_panel = xv_get(sb_tool_props_frame, FRAME_CMD_PANEL);

	row = 0;
	sb_tool_display_item = xv_create(sb_tool_props_panel, PANEL_CHOICE_STACK,
				       XV_X, xv_col(sb_tool_props_panel, 1),
				   XV_Y, xv_row(sb_tool_props_panel, row++),
					 PANEL_CHOICE_NROWS, 1,
#ifdef OW_I18N
				    PANEL_LABEL_STRING, gettext("Display:"),
#else
					 PANEL_LABEL_STRING, "Display:",
#endif
					 PANEL_CHOICE_STRINGS,
#ifdef OW_I18N
					 gettext("General"),
					 gettext("New Arrival"),
#else
					 "General",
					 "New Arrival",
#endif
					 NULL,
					 PANEL_VALUE, 0,
				    PANEL_NOTIFY_PROC, sb_props_choice_proc,
					 NULL);

	xv_set(canvas_paint_window(sb_tool_props_panel),
	       WIN_EVENT_PROC, sb_tool_props_panel_event,
	       NULL);
#if OW_I18N
	text_convert();
#endif

	make_general_panel(sb_tool_props_panel);
	row = sb_tool_item_layout(sb_tool_props_panel, row);

	(void) sb_show_panel(GENERAL);

	window_fit(sb_tool_props_panel);

	row++;
	sb_tool_apply_item = xv_create(sb_tool_props_panel, PANEL_BUTTON,
				     XV_Y, xv_row(sb_tool_props_panel, row),
#ifdef OW_I18N
				       PANEL_LABEL_STRING, gettext("Apply"),
#else
				       PANEL_LABEL_STRING, "Apply",
#endif
				       PANEL_NOTIFY_PROC, sb_tool_apply_proc,
				       NULL);

	sb_tool_reset_item = xv_create(sb_tool_props_panel, PANEL_BUTTON,
				     XV_Y, xv_row(sb_tool_props_panel, row),
#ifdef OW_I18N
				       PANEL_LABEL_STRING, gettext("Reset"),
#else
				       PANEL_LABEL_STRING, "Reset",
#endif
				       PANEL_NOTIFY_PROC, sb_tool_reset_proc,
				       NULL);

	sb_center_items(sb_tool_props_panel, row,
			sb_tool_apply_item, sb_tool_reset_item, NULL);
	window_fit(sb_tool_props_panel);
	window_fit(sb_tool_props_frame);

	notify_interpose_destroy_func(sb_tool_props_frame,
				      sb_tool_frame_destroy_proc);

	notify_interpose_event_func(sb_tool_props_frame,
				    sb_tool_pinout_proc, NOTIFY_SAFE);

	sb_props_update = FALSE;

	(void) sb_tool_reset_proc();
	(void) xv_set(sb_tool_props_frame, XV_SHOW, TRUE, NULL);

	xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

static          Notify_value
sb_tool_frame_destroy_proc(client, status)
	Notify_client   client;
	Destroy_status  status;
{
	Notify_client   tmp = client;

	xv_set(tmp, FRAME_NO_CONFIRM, TRUE, NULL);

	sb_tool_props_frame = NULL;

	return notify_next_destroy_func(tmp, status);
}

static          Notify_value
sb_tool_pinout_proc(client, event, arg, when)
	Notify_client   client;
	Event          *event;
	Notify_arg      arg;
	Notify_event_type when;
{
	Notify_value    return_value =
	notify_next_event_func(client, event, arg, NOTIFY_SAFE);

	if (event_action(event) == ACTION_PINOUT) {
		if (sb_props_update == TRUE) {
			if (two_notice((Frame) client,
#ifdef OW_I18N
				       gettext("Apply"), gettext("Quit"),
				     gettext("Properties has been changed"),
				       gettext("Applied or Not ?"),
#else
				       "Apply", "Quit",
				       "Properties has been changed",
				       "Applied or Not ?",
#endif
				       NULL))
				sb_tool_apply_proc(NULL, NULL);
		}
		xv_destroy((Frame) client);
	}
	return return_value;
}

static void
sb_tool_props_panel_event(window, event)
	Xv_Window       window;
	Event          *event;
{
	if (event_is_ascii(event))
		sb_tool_change_proc(NULL, NULL);
#ifdef OW_I18N
	else if (event_is_string(event))
		sb_tool_change_proc(NULL, NULL);
#endif
}

static void
sb_tool_change_proc(item, event)
	Panel_item      item;
	Event          *event;
{
	sb_props_update = TRUE;
}

static void
make_general_panel(p_panel)
	Panel           p_panel;
{
	int             i;

	for (i = 0; i < MAX_PROPS_ITEM; i++) {
		if (tool_each_item[i].pkg == PANEL_NUMERIC_TEXT)
			tool_each_item[i].item =
				xv_create(p_panel, tool_each_item[i].pkg,
			    PANEL_LABEL_STRING, tool_each_item[i].label_str,
				 PANEL_MAX_VALUE, tool_each_item[i].max_val,
				 PANEL_MIN_VALUE, tool_each_item[i].min_val,
					  PANEL_VALUE_DISPLAY_LENGTH, tool_each_item[i].display_val,
					  PANEL_VALUE_STORED_LENGTH, tool_each_item[i].stored_val,
			   PANEL_NOTIFY_PROC, tool_each_item[i].notify_proc,
					  XV_SHOW, FALSE,
					  NULL);

		if (tool_each_item[i].pkg == PANEL_MESSAGE)
			tool_each_item[i].item =
				xv_create(p_panel, tool_each_item[i].pkg,
			    PANEL_LABEL_STRING, tool_each_item[i].label_str,
					  XV_SHOW, FALSE,
					  NULL);

		if (tool_each_item[i].pkg == PANEL_TEXT)
			tool_each_item[i].item =
				xv_create(p_panel, tool_each_item[i].pkg,
			    PANEL_LABEL_STRING, tool_each_item[i].label_str,
					  PANEL_VALUE_DISPLAY_LENGTH, tool_each_item[i].display_val,
					  PANEL_VALUE_STORED_LENGTH, tool_each_item[i].stored_val,
					  XV_SHOW, FALSE,
					  NULL);

		if (tool_each_item[i].pkg == PANEL_CHOICE)
			tool_each_item[i].item =
				xv_create(p_panel, tool_each_item[i].pkg,
			    PANEL_LABEL_STRING, tool_each_item[i].label_str,
			   PANEL_NOTIFY_PROC, tool_each_item[i].notify_proc,
					  XV_SHOW, FALSE,
					  NULL);
	}

	xv_set((Panel_item) tool_each_item[Sb_article_display].item,
	       PANEL_CHOICE_STRINGS,
#ifdef OW_I18N
	       gettext("Unread"),
	       gettext("All"),
#else
	       "Unread",
	       "All",
#endif
	       NULL,
	       NULL);

	xv_set((Panel_item) tool_each_item[Sb_default_post_header].item,
	       PANEL_CHOICE_STRINGS,
#ifdef OW_I18N
	       gettext("Abbrev"),
	       gettext("Full"),
#else
	       "Abbrev",
	       "Full",
#endif
	       NULL,
	       NULL);

	xv_set((Panel_item) tool_each_item[Sb_default_header_type].item,
	       PANEL_CHOICE_STRINGS,
#ifdef OW_I18N
	       gettext("Full"),
	       gettext("Sender & Subject"),
	       gettext("Subject"),
#else
	       "Full",
	       "Sender & Subject",
	       "Subject",
#endif
	       NULL,
	       NULL);

	xv_set((Panel_item) tool_each_item[Sb_include_sender].item,
	       PANEL_CHOICE_STRINGS,
#ifdef OW_I18N
	       gettext("None"),
	       gettext("Sender"),
#else
	       "None",
	       "Sender",
#endif
	       NULL,
	       NULL);

}

static int
sb_tool_item_layout(panel, p_row)
	Panel           panel;
	int             p_row;
{
	Font            font;
	Font_string_dims dims;
	int             longest;
	register int    i;
	int             row;

	font = (int) xv_get(panel, WIN_FONT);

	row = p_row;
	row++;

	for (i = 0, longest = 0; i < MAX_PROPS_ITEM; i++) {
		if (tool_each_item[i].face == BIFF &&
		    (Panel_item_type) xv_get(tool_each_item[i].item, PANEL_ITEM_CLASS)
		    != PANEL_MESSAGE_ITEM) {
			(void) xv_get(font, FONT_STRING_DIMS,
				      tool_each_item[i].label_str, &dims);
			if (dims.width > longest)
				longest = dims.width;
		}
	}
	for (i = 0; i < MAX_PROPS_ITEM; i++)
		if (tool_each_item[i].face == BIFF) {
			if ((Panel_item_type) xv_get(tool_each_item[i].item, PANEL_ITEM_CLASS)
			    != PANEL_MESSAGE_ITEM)
				(void) xv_set(tool_each_item[i].item,
				  PANEL_VALUE_X, longest + xv_col(panel, 2),
					      XV_Y, xv_row(panel, row++),
					      NULL);
			else
				(void) xv_set(tool_each_item[i].item,
				 PANEL_LABEL_X, longest + xv_col(panel, 15),
					      XV_Y, xv_row(panel, row - 1),
					      NULL);
		}
	row = p_row;
	row++;

	for (i = 0, longest = 0; i < MAX_PROPS_ITEM; i++) {
		if (tool_each_item[i].face == GENERAL &&
		    (Panel_item_type) xv_get(tool_each_item[i].item, PANEL_ITEM_CLASS)
		    != PANEL_MESSAGE_ITEM) {
			(void) xv_get(font, FONT_STRING_DIMS,
				      tool_each_item[i].label_str, &dims);
			if (dims.width > longest)
				longest = dims.width;
		}
	}
	for (i = 0; i < MAX_PROPS_ITEM; i++)
		if (tool_each_item[i].face == GENERAL) {
			if ((Panel_item_type) xv_get(tool_each_item[i].item, PANEL_ITEM_CLASS)
			    != PANEL_MESSAGE_ITEM)
				(void) xv_set(tool_each_item[i].item,
				  PANEL_VALUE_X, longest + xv_col(panel, 2),
					      XV_Y, xv_row(panel, row++),
					      NULL);
			else
				(void) xv_set(tool_each_item[i].item,
				 PANEL_LABEL_X, longest + xv_col(panel, 15),
					      XV_Y, xv_row(panel, row - 1),
					      NULL);
		}
	return row;
}

static void
sb_show_panel(flag)
	Sb_tool_props_type flag;
{
	int             i;

	for (i = 0; i < MAX_PROPS_ITEM; i++)
		if (tool_each_item[i].face != flag)
			xv_set(tool_each_item[i].item, XV_SHOW, FALSE, NULL);

	for (i = 0; i < MAX_PROPS_ITEM; i++)
		if (tool_each_item[i].face == flag)
			xv_set(tool_each_item[i].item, XV_SHOW, TRUE, NULL);
}

static void
sb_props_choice_proc(item, event)
	Panel_item      item;
	Event          *event;
{
	Sb_tool_props_type value = (Sb_tool_props_type) xv_get(item, PANEL_VALUE);

	if (sb_props_update == TRUE)
		if (two_notice(sb_tool_props_frame,
#ifdef OW_I18N
		       gettext("Applied Change"), gettext("Dismiss Change"),
			       gettext("Properties may be changed."),
#else
			       "Applied Change", "Dismiss Change",
			       "Properties may be changed.",
#endif
			       NULL)) {
			(void) sb_tool_apply_proc(NULL, NULL);
		} else {
		}

	sb_props_update = FALSE;

	switch ((int) value) {
	case GENERAL:
		sb_show_panel(GENERAL);
		break;

	case BIFF:
		sb_show_panel(BIFF);
		break;

	default:
		break;
	}
}

static void
sb_tool_apply_proc(item, event)
	Panel_item      item;
	Event          *event;
{
	register int    i;
	char            filename[MAXSTRLEN];
	FILE           *fp;
	struct stat     fstatus;
	extern Canvas   sb_canvas;

	for (i = 0; i < MAX_PROPS_ITEM; i++) {
		switch ((Panel_item_type) xv_get(tool_each_item[i].item,
						 PANEL_ITEM_CLASS)) {

		case PANEL_NUMERIC_TEXT_ITEM:
		case PANEL_CHOICE_ITEM:{
				int             value = (int) xv_get(tool_each_item[i].item, PANEL_VALUE);
				sb_tool_value[i].int_value = value;
				break;
			}
		case PANEL_TEXT_ITEM:{
				char           *str = (char *) xv_get(tool_each_item[i].item, PANEL_VALUE);

				sb_tool_value[i].char_value =
					(char *) realloc(sb_tool_value[i].char_value, strlen(str) + 1);

				strcpy(sb_tool_value[i].char_value, str);

				break;
			}
		default:
			break;
		}
	}

	sprintf(filename, "%s/%s", (char *) getenv("HOME"), sb_rc_file);

	if (stat(filename, &fstatus) == 0) {
		if (!two_notice(sb_tool_props_frame,
#ifdef OW_I18N
				gettext("Save"), gettext("Cancel"),
				gettext("File Exists. Overwrite it?"),
#else
				"Save", "Cancel",
				"File Exists. Overwrite it?",
#endif
				NULL))
			return;
	}
	if ((fp = fopen(filename, "w+")) == NULL) {
		(void) fprintf(stderr,
			       "Can not write %s file", filename);
		return;
	}
	for (i = 0; i < MAX_PROPS_ITEM; i++) {
		switch ((Panel_item_type) xv_get(tool_each_item[i].item,
						 PANEL_ITEM_CLASS)) {

		case PANEL_NUMERIC_TEXT_ITEM:
		case PANEL_CHOICE_ITEM:{
				fprintf(fp, "%s%d\n",
					tool_each_item[i].file_str, sb_tool_value[i].int_value);
				break;
			}
		case PANEL_TEXT_ITEM:
			fprintf(fp, "%s%s\n",
				tool_each_item[i].file_str, sb_tool_value[i].char_value);
			break;

		default:
			break;
		}
	}
	fclose(fp);

	(void) sb_set_global_val();
	(void) sb_set_news_biff_timer();

	/*
	 * Can not set the size. (void) sb_canvas_layout_func();
	 * xv_set(sb_frame, XV_WIDTH, (int) xv_get(sb_canvas, XV_WIDTH),
	 * NULL); window_fit_height(sb_frame);
	 */


	/* For the clean */
	(void) sb_canvas_force_repaint_proc((char *) get_current_newsgroup());

	sb_props_update = FALSE;

#ifdef OW_I18N
	frame_msg(sb_frame, gettext("Tool properties has bgen applied."));
#else
	frame_msg(sb_frame, "Tool properties has been applied.");
#endif

}

static void
sb_tool_reset_proc(item, event)
	Panel_item      item;
	Event          *event;
{
	register int    i;

	for (i = 0; i < MAX_PROPS_ITEM; i++) {
		switch ((Panel_item_type) xv_get(tool_each_item[i].item,
						 PANEL_ITEM_CLASS)) {

		case PANEL_NUMERIC_TEXT_ITEM:
		case PANEL_CHOICE_ITEM:{
				xv_set(tool_each_item[i].item, PANEL_VALUE,
				       sb_tool_value[i].int_value, NULL);
				break;
			}
		case PANEL_TEXT_ITEM:
			xv_set(tool_each_item[i].item, PANEL_VALUE,
			       sb_tool_value[i].char_value, NULL);
			break;

		default:
			break;
		}
	}
	sb_props_update = FALSE;
}

static void
sb_default_value_set()
{
	register int    i;

	for (i = 0; i < MAX_PROPS_ITEM; i++) {
		if (tool_each_item[i].pkg == PANEL_NUMERIC_TEXT ||
		    tool_each_item[i].pkg == PANEL_CHOICE)
			sb_tool_value[i].int_value = tool_each_item[i].def_int_value;

		else if (tool_each_item[i].pkg == PANEL_TEXT)
			sb_set_char_props(i, tool_each_item[i].def_char_cvalue);

	}
	sb_set_global_val();
}

int
sb_read_rc_file()
{
	char            buf[MAXSTRLEN];
	char            tmp[MAXSTRLEN];
	char            charvalue[MAXSTRLEN];
	char            file[MAXSTRLEN];
	FILE           *fp;
	register int    i;

	sprintf(file, "%s/%s", (char *) getenv("HOME"), sb_rc_file);

	if ((fp = fopen(file, "r")) == NULL) {
		sb_default_value_set();
		return;
	}
	while (fgets(buf, sizeof(buf), fp) != NULL) {
		for (i = 0; i < MAX_PROPS_ITEM; i++) {
			if (tool_each_item[i].file_str == NULL || *buf == '#')
				continue;
			if (strstr(buf, tool_each_item[i].file_str)) {
				(void) strtok(buf, "=");
				strcpy(tmp, (char *) strtok(NULL, "="));
				rm_return(tmp, charvalue);
				if (tool_each_item[i].pkg == PANEL_TEXT)
					sb_set_char_props(i, charvalue);
				else
					sb_tool_value[i].int_value =
						(int) atoi(charvalue);
				break;
			}
		}
	}
	fclose(fp);

	sb_set_global_val();
}

static void
sb_set_char_props(type, str)
	int             type;
	char           *str;
{
	if (sb_tool_value[type].char_value = malloc(strlen(str) + 1))
		strcpy(sb_tool_value[type].char_value, str);
	else {
		fprintf(stderr, "Can not allocate memory for %s.\n", str);
		exit(0);
	}
}

int
sb_get_props_value(type)
	Sb_props_type   type;
{
	return (int) sb_tool_value[type].int_value;
}

char           *
sb_get_props_char_value(type)
	Sb_props_type   type;
{
	return (char *) sb_tool_value[type].char_value;
}

/*
 * I can not take care of the following global values.
 */
static void
sb_set_global_val()
{
	extern int      max_line;
	extern int      sb_display_status;
	extern char     BRACKET[];

	max_line = (int) sb_get_props_value(Sb_max_msg);
	sb_display_status = (int) sb_get_props_value(Sb_default_header_type);
	strcpy(BRACKET, (char *) sb_get_props_char_value(Sb_indent_prefix));
}

int
sb_create_newsrc_file()
{
	struct stat     fstatus;
	int             status;
	char            msg[MAXSTRLEN];
	char            command[MAXSTRLEN];

	if (*NEWSRC_FLIST_NAM == NULL)
		sprintf(NEWSRC_FLIST_NAM, "%s/.newsrc", getenv("HOME"));

	if (stat(NEWSRC_FLIST_NAM, &fstatus) == -1) {
#ifdef OW_I18N
		sprintf(msg, gettext("%s is not found."), NEWSRC_FLIST_NAM);
#else
		sprintf(msg, "%s is not found.", NEWSRC_FLIST_NAM);
#endif
		status = three_notice(sb_frame,
#ifdef OW_I18N
			     gettext(" 1 "), gettext(" 2 "), gettext(" 3 "),
				      msg,
		     gettext("To start shinbun, .newsrc or the equivalent"),
			   gettext("is needed. You can choice three ways."),
				gettext("1. All newsgrousp are available "),
				gettext("2. Edit the newsgroup you want  "),
				gettext("3. Prepare by yourself then quit"),
#else
				      " 1 ", " 2 ", " 3 ",
				      msg,
			      "To start shinbun, .newsrc or the equivalent",
				    "is needed. You can choice three ways.",
				      "1. All newsgrousp are available ",
				      "2. Edit the newsgroup you want  ",
				      "3. Prepare by yourself then quit",
#endif
				      NULL);

		switch (status) {
		case 1:
			sprintf(command, "/bin/cp %s %s", ACTIVE_FLIST_NAM,
				NEWSRC_FLIST_NAM);
			system(command);
			return TRUE;
			break;
		case 2:
			make_news_props_win();
			return FALSE;
			break;
		case 3:
			exit(0);
			return TRUE;
			break;
		default:
			return FALSE;
			break;
		}
	} else
		return TRUE;
}

int
sb_cmd_parse(argc, argv)
	int             argc;
	char          **argv;
{
	char           *p;
	void            sb_good_by();

	while (--argc > 0) {
		if (**(++argv) == '-') {
			if (strcmp(*argv, "-newsrc") == 0) {
				if ((p = *(++argv)) == NULL)
					sb_good_by(NULL);
				else {
					if (strstr(p, "/"))
						strcpy(NEWSRC_FLIST_NAM, p);
					else
						sprintf(NEWSRC_FLIST_NAM,
							"%s/%s",
							getenv("HOME"), p);
					argc--;
					continue;
				}
			}
#ifdef FOR_INTERNAL_USE
			else if (strcmp(*argv, "-expert") == 0) {
				extern int      sb_can_send_flag;

				if ((p = *(++argv)) == NULL)
					sb_good_by(NULL);
				else {
					if (strcmp(p, "on") == 0 ||
					    strcmp(p, "ON") == 0)
						sb_can_send_flag = TRUE;
					argc--;
					continue;
				}
			}
#endif
		}
	}
}

void
sb_good_by(err)
	char           *err;
{
	if (err)
		fprintf(stderr, "shinbun tool: illegal option : %s\n", err);
	else
		fprintf(stderr, "shinbun tool: Value not specified\n");

	exit(0);
}
