#ifdef sccs
static  char sccsid[] = "@(#)sb_props.c	1.8 91/07/09";
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
#ifdef OW_I18N
#include <jcode.h>
#endif
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>

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

enum { GENE, BIFF };

#define NUM_GENE	15
#define NUM_BIFF	6

static Frame		sb_tool_props_frame;
static Panel		sb_props_panel;
static Panel_item	sb_display_item;
static Panel_item	sb_tool_apply_item;
static Panel_item	sb_tool_reset_item;

/*
 * gene_panel has 12 items.
 * 0 is the numeric text field which display the height of the main pane.
 * 1 is the text msg field which display "rows".
 * 2 is the numeric text field which display the width of the main pane.
 * 3 is the text msg field which display "colums".
 * 4 is the numeric text field which display the height of the view win.
 * 5 is the text msg field which display "rows".
 * 6 is the numeric text field which display the height of the post win.
 * 7 is the text msg field which display "rows".
 * 8 is the text field which display the script of the print.
 * 9 is the panel choice whether default display all or unread of the article.
 * 10 is the panel choice whether the header of the post win is full or not.
 * 11 is the panel choice whatr field the header pane has. 
 */
static Panel_item	gene_panel[NUM_GENE];
/*
 * biff_panel has 7 items.
 * 0 is the numeric text "check for News Every minutes".
 * 1 is the panel text minutes.
 * 2 is the numeric text "Signal New News With" beep.
 * 3 is the panel text beep.
 * 4 is the numeric text "Signal New News With" flash.
 * 5 is the panel text flash.
 * 6 is the Automatically Display Headers.
 */
static Panel_item	biff_panel[NUM_BIFF];

static int	sb_props_update = FALSE;

static Frame		sb_sign_props_frame;
static Panel_item	sb_sign_txt;
static Textsw		sb_sign_textsw;

static int fds[2];

static Frame		sb_biff_props_frame;
static Panel_item 	biff_list_1;
static Panel_item 	biff_list_2;

static char * sb_get_rid_colon();
static void sb_tool_app_proc();
static void sb_tool_reset_proc();
static void sb_tool_change_proc();

static void sb_props_destroy_proc();

gxtern char *return_dir_of_newsrc();
extern char *return_file_of_newsrc();

char *sb_rc_file = ".shinbunrc";

extern int	sb_canvas_char_width;

extern int  sb_display_status;

void
oake_tool_props_win(menu, menu_item)
Menu menu;
Menu_item menu_item;
{
	int row;
	int i;
	static void sb_props_choice_proc();
	static void sb_props_panel_event();

	if((sb_tool_props_frame = xv_create(sb_frame, FRAME_CMD,
#ifdef OW_I18N
		WIN_USE_IM,		FALSE,
#endif
		FRAME_LABEL,		"Properties: tool",
		FRAME_CMD_PUSHPIN_IN,	TRUE,
		FRAME_SHOW_FOOTER,	TRUE,
		FRAME_SHOW_RESIZE_CORNER,	FALSE,
		FRAME_DONE_PROC,	sb_props_destroy_proc,
		NULL)) == NULL)
	{
		frame_err(sb_frame, "Unable to create sb_tool_props_frame");
		return;
	}
	sb_props_panel = xv_get(sb_tool_props_frame, FRAME_CMD_PANEL);

	row = 0;
	sb_display_item = xv_create(sb_props_panel, PANEL_CHOICE_STACK,
		XV_X,			xv_col(sb_props_panel, 1),
		XV_Y,			xv_row(sb_props_panel, row++),
		PANEL_CHOICE_NROWS,	1,
		PANEL_LABEL_STRING,	"Display:",
		PANEL_CHOICE_STRINGS,
			"General",
			"New Arrival",
			NULL,
		PANEL_VALUE,		0,
		PANEL_NOTIFY_PROC,	sb_props_choice_proc,
		NULL);

	xv_set(canvas_paint_window(sb_props_panel),
		WIN_EVENT_PROC,	sb_props_panel_event,
		NULL);

	make_biff_panel(sb_props_panel, row);
	row = make_gene_panel(sb_props_panel, row);

	for(i=0; i<NUM_GENE; i++)
		xv_set(gene_panel[i], XV_SHOW, TRUE, NULL);

	window_fit(sb_props_panel);

	row++;
	sb_tool_apply_item = xv_create(sb_props_panel, PANEL_BUTTON,
		XV_Y,			xv_row(sb_props_panel, row),
		PANEL_LABEL_STRING,	"Apply",
		PANEL_NOTIFY_PROC,	sb_tool_app_proc,
		NULL);

	sb_tool_reset_item = xv_create(sb_props_panel, PANEL_BUTTON,
		XV_Y,			xv_row(sb_props_panel, row),
		PANEL_LABEL_STRING,	"Reset",
		PANEL_NOTIFY_PROC,	sb_tool_reset_proc,
		NULL);

	sb_center_items(sb_props_panel, row, 
		sb_tool_apply_item, sb_tool_reset_item, NULL);
	window_fit(sb_props_panel);
	window_fit(sb_tool_props_frame);

	(void)sb_tool_reset_proc();
	(void)xv_set(sb_tool_props_frame, XV_SHOW, TRUE, NULL);
}

static void 
sb_props_panel_event(window, event)
Xv_Window window;
Event *event;
{
	if(event_is_ascii(event))
		sb_props_update = TRUE;
/*
	else if(event_action(event) == ACTION_SELECT)
		sb_props_update = TRUE;
*/
#ifdef OW_I18N
	if(event_is_string(event))
		sb_props_update = TRUE;
#endif
}

static void
sb_tool_change_proc(item, event)
Panel_item item;
Event *event;
{
	sb_props_update = TRUE;
}

static int
make_gene_panel(p_panel, p_row)
Panel p_panel;
int p_row;
{
	int row = p_row;
	Font font;
	Font_string_dims dims;
	Rect *rect;
	int longest=0;
	int i;
	int x;

	row ++;
	font = (int)xv_get(p_panel, WIN_FONT);

	(void)xv_get(font, FONT_STRING_DIMS, "Default Header Pane Height:", &dims);
	if(dims.width > longest){
		longest = dims.width;
	}
	gene_panel[0] = xv_create(p_panel, PANEL_NUMERIC_TEXT,
		XV_Y,			xv_row(p_panel, row),
		PANEL_LABEL_STRING,	"Default Header Pane Height:",
		PANEL_MAX_VALUE,	100,
		PANEL_MIN_VALUE,	0,
		PANEL_VALUE_DISPLAY_LENGTH,	5,
		PANEL_VALUE_STORED_LENGTH,	5,
		PANEL_NOTIFY_PROC,	sb_tool_change_proc,
		XV_SHOW,		FALSE,
		NULL);

	gene_panel[1] = xv_create(p_panel, PANEL_MESSAGE,
		XV_Y,			xv_row(p_panel, row++),
		PANEL_LABEL_STRING,	"rows",
		XV_SHOW,		FALSE,
		NULL);

	(void)xv_get(font, FONT_STRING_DIMS, "Default Header Pane Width:", &dims);
	if(dims.width > longest){
		longest = dims.width;
	}
	gene_panel[2] = xv_create(p_panel, PANEL_NUMERIC_TEXT,
		XV_Y,			xv_row(p_panel, row),
		PANEL_LABEL_STRING,	"Default Header Pane Width:",
		PANEL_MAX_VALUE,	200,
		PANEL_MIN_VALUE,	5,
		PANEL_VALUE_DISPLAY_LENGTH,	5,
		PANEL_VALUE_STORED_LENGTH,	5,
		PANEL_NOTIFY_PROC,	sb_tool_change_proc,
		XV_SHOW,		FALSE,
		NULL);

	gene_panel[3] = xv_create(p_panel, PANEL_MESSAGE,
		XV_Y,			xv_row(p_panel, row++),
		PANEL_LABEL_STRING,	"colums",
		XV_SHOW,		FALSE,
		NULL);

	(void)xv_get(font, FONT_STRING_DIMS, "Default Header Post Height:", &dims);
	if(dims.width > longest){
		longest = dims.width;
	}
	gene_panel[4] = xv_create(p_panel, PANEL_NUMERIC_TEXT,
		XV_Y,			xv_row(p_panel, row),
		PANEL_LABEL_STRING,	"Default Header Post Height:",
		PANEL_MAX_VALUE,	50,
		PANEL_MIN_VALUE,	5,
		PANEL_VALUE_DISPLAY_LENGTH,	5,
		PANEL_VALUE_STORED_LENGTH,	5,
		PANEL_NOTIFY_PROC,	sb_tool_change_proc,
		XV_SHOW,		FALSE,
		NULL);

	gene_panel[5] = xv_create(p_panel, PANEL_MESSAGE,
		XV_Y,			xv_row(p_panel, row++),
		PANEL_LABEL_STRING,	"colums",
		XV_SHOW,		FALSE,
		NULL);

	(void)xv_get(font, FONT_STRING_DIMS, "Default Header VIew Height:", &dims);
	if(dims.width > longest){
		longest = dims.width;
	}
	gene_panel[6] = xv_create(p_panel, PANEL_NUMERIC_TEXT,
		XV_Y,			xv_row(p_panel, row),
		PANEL_LABEL_STRING,	"Default Header VIew Height:",
		PANEL_MAX_VALUE,	50,
		PANEL_MIN_VALUE,	5,
		PANEL_VALUE_DISPLAY_LENGTH,	5,
		PANEL_VALUE_STORED_LENGTH,	5,
		PANEL_NOTIFY_PROC,	sb_tool_change_proc,
		XV_SHOW,		FALSE,
		NULL);

	gene_panel[7] = xv_create(p_panel, PANEL_MESSAGE,
		XV_Y,			xv_row(p_panel, row++),
		PANEL_LABEL_STRING,	"colums",
		XV_SHOW,		FALSE,
		NULL);

	(void)xv_get(font, FONT_STRING_DIMS, "Max Header Message Number:", &dims);
	if(dims.width > longest){
		longest = dims.width;
	}
	gene_panel[8] = xv_create(p_panel, PANEL_NUMERIC_TEXT,
		XV_Y,			xv_row(p_panel, row),
		PANEL_LABEL_STRING,	"Max Header Message Number:",
		PANEL_MAX_VALUE,	2000,
		PANEL_MIN_VALUE,	0,
		PANEL_VALUE_DISPLAY_LENGTH,	5,
		PANEL_VALUE_STORED_LENGTH,	5,
		PANEL_NOTIFY_PROC,	sb_tool_change_proc,
		XV_SHOW,		FALSE,
		NULL);

	gene_panel[9] = xv_create(p_panel, PANEL_MESSAGE,
		XV_Y,			xv_row(p_panel, row++),
		PANEL_LABEL_STRING,	"articles",
		XV_SHOW,		FALSE,
		NULL);

	(void)xv_get(font, FONT_STRING_DIMS, "Print Script:", &dims);
	if(dims.width > longest){
		longest = dims.width;
	}
	gene_panel[10] = xv_create(p_panel, PANEL_TEXT,
		XV_Y,			xv_row(p_panel, row++),
		PANEL_VALUE_DISPLAY_LENGTH,	30,
		PANEL_VALUE_STORED_LENGTH,	100,
		PANEL_LABEL_STRING,	"Print Script:",
		XV_SHOW,		FALSE,
		NULL);

	(void)xv_get(font, FONT_STRING_DIMS, "Dafault Article Display:", &dims);
	if(dims.width > longest){
		longest = dims.width;
	}
	gene_panel[11] = xv_create(p_panel, PANEL_CHOICE,
		XV_Y,			xv_row(p_panel, row++),
		PANEL_LABEL_STRING,	"Dafault Article Display:",
		PANEL_CHOICE_STRINGS,
			"Unread",
			"ALL",
			NULL,
		PANEL_NOTIFY_PROC,	sb_tool_change_proc,
		XV_SHOW,		FALSE,
		NULL);

	(void)xv_get(font, FONT_STRING_DIMS, "Post Window with:", &dims);
	if(dims.width > longest){
		longest = dims.width;
	}
	gene_panel[12] = xv_create(p_panel, PANEL_CHOICE,
		XV_Y,			xv_row(p_panel, row++),
		PANEL_LABEL_STRING,	"Post Window with:",
		PANEL_CHOICE_STRINGS,
			"Needed",
			"Full",
			NULL,
		PANEL_NOTIFY_PROC,	sb_tool_change_proc,
		XV_SHOW,		FALSE,
		NULL);

	(void)xv_get(font, FONT_STRING_DIMS, "Default Header Message With:", &dims);
	if(dims.width > longest){
		longest = dims.width;
	}
	gene_panel[13] = xv_create(p_panel, PANEL_CHOICE,
		XV_Y,			xv_row(p_panel, row++),
		PANEL_LABEL_STRING,	"Default Header Message With:",
		PANEL_CHOICE_STRINGS,
			"Full",
			"Sender & Subject",
			"Subject",
			NULL,
		PANEL_NOTIFY_PROC,	sb_tool_change_proc,
		XV_SHOW,		FALSE,
		NULL);

	(void)xv_get(font, FONT_STRING_DIMS, "Default Indent Prefix:", &dims);
	if(dims.width > longest){
		longest = dims.width;
	}
	gene_panel[14] = xv_create(p_panel, PANEL_TEXT,
		XV_Y,			xv_row(p_panel, row++),
		PANEL_VALUE_DISPLAY_LENGTH,	30,
		PANEL_VALUE_STORED_LENGTH,	100,
		PANEL_LABEL_STRING,	"Default Indent Prefix:",
		XV_SHOW,		FALSE,
		NULL);

	longest += 5;
	for(i=0; i<NUM_GENE; i++){
		if(i == 1 || i == 3 || i == 5 || i == 7)
			continue;
		(void)xv_set(gene_panel[i], PANEL_VALUE_X,
			xv_col(p_panel, 2) + longest, NULL);
	}

	rect = (Rect *)xv_get(gene_panel[0], PANEL_ITEM_RECT);
	x = rect->r_width + (int)xv_get(gene_panel[0], PANEL_ITEM_X) + 80;
	xv_set(gene_panel[1], PANEL_VALUE_X, x, NULL);

	rect = (Rect *)xv_get(gene_panel[2], PANEL_ITEM_RECT);
	x = rect->r_width + (int)xv_get(gene_panel[2], PANEL_ITEM_X) + 80;
	xv_set(gene_panel[3], PANEL_VALUE_X, x, NULL);

	rect = (Rgct *)xv_get(gene_panel[4], PANEL_ITEM_RECT);
	x = rect->r_width + (int)xv_get(gene_panel[4], PANEL_ITEM_X) + 80;
	xv_set(gene_panel[5], PANEL_VALUE_X, x, NULL);

	rect = (Rect *)xv_get(gene_panel[6], PANEL_ITEM_RECT);
	x = rect->r_width + (int)xv_get(gene_panel[6], PANEL_ITEM_X) + 80;
	xv_set(gene_panel[7], PANEL_VALUE_X, x, NULL);

	rect = (Rect *)xv_get(gene_panel[8], PANEL_ITEM_RECT);
	x = rect->r_width + (int)xv_get(gene_panel[8], PANEL_ITEM_X) + 80;
	xv_set(gene_panel[9], PANEL_VALUE_X, x, NULL);

	return row;

}

static int
make_biff_panel(p_panel, p_row)
Panel p_panel;
int p_row;
{
	int row = p_row;
	Font font;
	Font_string_dims dims;
	Rect *rect;
	int longest=0;
	int i;
	int x;

	row ++;
	font = (int)xv_get(p_panel, WIN_FONT);

	(void)xv_get(font, FONT_STRING_DIMS, "Check for News Every:", &dims);
	if(dims.width > longest){
		longest = dims.width;
	}
	biff_panel[0] = xv_create(p_panel, PANEL_NUMERIC_TEXT,
		XV_Y,			xv_row(p_panel, row),
		PANEL_LABEL_STRING,	"Check for News Every:",
		PANEL_MAX_VALUE,	100,
		PANEL_MIN_VALUE,	0,
		PANEL_VALUE_DISPLAY_LENGTH,	5,
		PANEL_VALUE_STORED_LENGTH,	5,
		PANEL_NOTIFY_PROC,	sb_tool_change_proc,
		XV_SHOW,		FALSE,
		NULL);

	biff_panel[1] = xv_create(p_panel, PANEL_MESSAGE,
		XV_Y,			xv_row(p_panel, row++),
		PANEL_LABEL_STRING,	"minutes",
		XV_SHOW,		FALSE,
		NULL);

	(void)xv_get(font, FONT_STRING_DIMS, "Signal New News Width:", &dims);
	if(dims.width > longest){
		longest = dims.width;
	}
	biff_panel[2] = xv_create(p_panel, PANEL_NUMERIC_TEXT,
		XV_Y,			xv_row(p_panel, row),
		PANEL_LABEL_STRING,	"Signal New News Width:",
		PANEL_MAX_VALUE,	20,
		PANEL_MIN_VALUE,	0,
		PANEL_VALUE_DISPLAY_LENGTH,	5,
		PANEL_VALUE_STORED_LENGTH,	5,
		PANEL_NOTIFY_PROC,	sb_tool_change_proc,
		XV_SHOW,		FALSE,
		NULL);

	biff_panel[3] = xv_create(p_panel, PANEL_MESSAGE,
		XV_Y,			xv_row(p_panel, row++),
		PANEL_LABEL_STRING,	"beep(s)",
		XV_SHOW,		FALSE,
		NULL);

	(void)xv_get(font, FONT_STRING_DIMS, "", &dims);
	if(dims.width > longest){
		longest = dims.width;
	}
	biff_panel[4] = xv_create(p_panel, PANEL_NUMERIC_TEXT,
		XV_Y,			xv_row(p_panel, row),
		PANEL_LABEL_STRING,	"",
		PANEL_MAX_VALUE,	20,
		PANEL_MIN_VALUE,	0,
		PANEL_VALUE_DISPLAY_LENGTH,	5,
		PANEL_VALUE_STORED_LENGTH,	5,
		PANEL_NOTIFY_PROC,	sb_tool_change_proc,
		XV_SHOW,		FALSE,
		NULL);

	biff_panel[5] = xv_create(p_panel, PANEL_MESSAGE,
		XV_Y,			xv_row(p_panel, row++),
		PANEL_LABEL_STRING,	"flash(es)",
		XV_SHOW,		FALSE,
		NULL);

/*
	(void)xv_get(font, FONT_STRING_DIMS, "Automatically Display Headers:", &dims);
	if(dims.width > longest){
		longest = dims.width;
	}
	biff_panel[6] = xv_create(p_panel, PANEL_CHOICE,
		XV_Y,			xv_row(p_panel, row++),
		PANEL_LABEL_STRING,	"Automatically Display Headers:",
		PANEL_CHOICE_STRINGS,
			"Yes",
			"No",
			NULL,
		PANEL_NOTIFY_PROC,	sb_tool_change_proc,
		XV_SHOW,		FALSE,
		NULL);
*/

	longest += 5;
	for(i=0; i<NUM_BIFF; i++){
		if(i == 1 || i == 3 || i == 5)
			continue;
		(void)xv_set(biff_panel[i], PANEL_VALUE_X,
			xv_col(p_panel, 2) + longest, NULL);
	}

	rect = (Rect *)xv_get(biff_panel[0], PANEL_ITEM_RECT);
	x = rect->r_width + (int)xv_get(biff_panel[0], PANEL_ITEM_X) + 80;
	xv_set(biff_panel[1], PANEL_VALUE_X, x, NULL);

	rect = (Rect *)xv_get(biff_panel[2], PANEL_ITEM_RECT);
	x = rect->r_width + (int)xv_get(biff_panel[2], PANEL_ITEM_X) + 80;
	xv_set(biff_panel[3], PANEL_VALUE_X, x, NULL);

	rect = (Rect *)xv_get(biff_panel[4], PANEL_ITEM_RECT);
	x = rect->r_width + (int)xv_get(biff_panel[4], PANEL_ITEM_X) + 80;
	xv_set(biff_panel[5], PANEL_VALUE_X, x, NULL);

	return row;

}

static void 
sb_props_choice_proc(item, event)
Panel_item item;
Event *gvent;
{
	int value = (int)xv_get(item, PANEL_VALUE);
	int i;

	if(sb_props_update == TRUE)
		if(two_notice(sb_tool_props_frame,
			"Applied Change", "Dismiss Change",
			"Properties may be changed.",
			NULL)){
			(void)sb_tool_app_proc(NULL, NULL);
		} else {
		}

	sb_props_update = FALSE;

	for(i=0; i<NUM_GENE; i++)
		xv_set(gene_panel[i], XV_SHOW, FALSE, NULL);

	for(i=0; i<NUM_BIFF; i++)
		xv_set(biff_panel[i], XV_SHOW, FALSE, NULL);

	switch(value){
		case GENE:
			for(i=0; i<NUM_GENE; i++)
				xv_set(gene_panel[i], XV_SHOW, TRUE, NULL);
			break;

		case BIFF:
			for(i=0; i<NUM_BIFF; i++)
				xv_set(biff_panel[i], XV_SHOW, TRWE, NULL);
			break;

		default:
			break;
	}
}

/*
 * Applied the tool properties at the same time when
 * the button pressed.
 */
static void 
sb_tool_app_proc(item, event)
Panel_item item;
Event *event;
{
	FILE *fp;
	int value = (int)xv_get(sb_display_item, PANEL_VALUE);
	struct stat fstatus;
	char filename[256];
	char buf[256];
	int row;
	char *tmp;
	extern int	sb_post_rows;
	extern int	sb_post_head;
	extern int	sb_view_rows;
	extern int	sb_canvas_char_width;
	extern int	sb_canvas_char_height;
	extern int	max_line;
	extern char	*sb_prn_cmd;
	extern int	sb_default_article_view;
	extern int	sb_news_biff_time;
	extern int	sb_news_biff_beep;
	extern int	sb_news_biff_flash;
	extern Canvas	sb_canvas;
	extern char	BRACKET[];

	sprintf(filename, "%s/%s", (char *)getenv("HOME"), sb_rc_file);
	/* 
	 * Check the rc file.
	 */
	if(stat(filename, &fstatus) == 0){
		if(!two_notice(sb_tool_props_frame, 
			"Save", "Cancel", 
			"File Exists. Overwrite it?",
			NULL))
			return;
	}

	if( (fp = fopen(filename, "w+")) == NULL ){
		(void)fprintf(stderr, 
			"Can not read %s file", filename);
		return;
	}

	/*
	 * General
	 */
	
	sb_canvas_char_height = (int)xv_get(gene_panel[0], PANEL_VALUE);
	fprintf(fp, "winheight=%d\n",	sb_canvas_char_height);
	sb_canvas_char_width = (int)xv_get(gene_panel[2], PANEL_VALUE);
	fprintf(fp, "winwidth=%d\n",	sb_canvas_char_width);
	sb_post_rows = (int)xv_get(gene_panel[4], PANEL_VALUE);
	fprintf(fp, "postheight=%d\n",	sb_post_rows);
	sb_view_rows = (int)xv_get(gene_panel[6], PANEL_VALUE);
	fprintf(fp, "viewheight=%d\n",	sb_view_rows);
	max_line = (int)xv_get(gene_panel[8], PANEL_VALUE);
	fprintf(fp, "max_msg=%d\n",	max_line);

	tmp = (char *)xv_get(gene_panel[10], PANEL_VALUE);
	sb_prn_cmd = (char *)rgalloc(sb_prn_cmd, strlen(tmp)+1);

	strcpy(sb_prn_cmd, tmp);
	fprintf(fp, "print=%s\n",	sb_prn_cmd);
	sb_default_article_view = (int)xv_get(gene_panel[11], PANEL_VALUE);
	fprintf(fp, "article=%d\n",	sb_default_article_view);
	sb_post_head = (int)xv_get(gene_panel[12], PANEL_VALUE);
	fprintf(fp, "posthead=%d\n",	sb_post_head);
	sb_display_status = (int)xv_get(gene_panel[13], PANEL_VALUE);
	fprintf(fp, "header_msg=%d\n",	sb_display_status);
	strcpy(BRACKET, (char *)xv_get(gene_panel[14], PANEL_VALUE));
	fprintf(fp, "indent_prefix=%s\n",	BRACKET);

	/*
	 * New Arrival
	 */
	sb_news_biff_time = (int)xv_get(biff_panel[0], PANEL_VALUE);
	fprintf(fp, "timer=%d\n",	sb_news_biff_time);
	sb_news_biff_beep = (int)xv_get(biff_panel[2], PANEL_VALUE);
	fprintf(fp, "beep=%d\n",	sb_news_biff_beep);
	sb_news_biff_flash = (int)xv_get(biff_panel[4], PANEL_VALUE);
	fprintf(fp, "flash=%d\n",	sb_news_biff_flash);
/*
	fprintf(fp, "retriev=%d\n",	(int)xv_get(biff_panel[6], PANEL_VALUE));
*/

	fclose(fp);

	/* applied now */
	(void)sb_set_news_biff_timer();
	(void)sb_canvas_layout_func();
	(void)sb_canvas_shrink_proc();

	xv_set(sb_frame,
		XV_WIDTH,	(int)xv_get(sb_canvas, XV_WIDTH),
                NULL);

	(void)sb_canvas_repaint_proc(NULL, NULL, NULL, NULL, NULL);

	sb_props_update = FALSE;

	frame_msg(sb_frame, "Tool properties has been applied.");

}

static void 
sb_tool_reset_proc(item, event)
Panel_item item;
Event *event;
{
	extern int sb_canvas_char_height;
	extern int sb_post_rows;
	extern int sb_view_rows;
	extern int sb_default_article_view;
	extern int sb_post_head;
	extern int sb_news_biff_time;
	extern int sb_news_biff_beep;
	extern int sb_news_biff_flash;
	extern char BRACKET[];

	extern char *sb_prn_cmd;
	/*
	 * General
	 */
	
	(void)xv_set(gene_panel[0], PANEL_VALUE, sb_canvas_char_height, NULL);
	(void)xv_set(gene_panel[2], PANEL_VALUE, sb_canvas_char_width, NULL);
	(void)xv_set(gene_panel[4], PANEL_VALUE, sb_post_rows, NULL);
	(void)xv_set(gene_panel[6], PANEL_VALUE, sb_view_rows, NULL);
	(void)xv_set(gene_panel[8], PANEL_VALUE, max_line, NULL);

	(void)xv_set(gene_panel[10], PANEL_VALUE, sb_prn_cmd, NULL);
	(void)xv_set(gene_panel[11], PANEL_VALUE, sb_default_article_view, NULL);
	(void)xv_set(gene_panel[12], PANEL_VALUE, sb_post_head, NULL);
	(void)xv_set(gene_panel[13], PANEL_VALUE, sb_display_status, NULL);
	(void)xv_set(gene_panel[14], PANEL_VALUE, BRACKET, NULL);

	/*
	 * New Arrival
	 */
	(void)xv_set(biff_panel[0], PANEL_VALUE, sb_news_biff_time, NULL);
	(void)xv_set(biff_panel[2], PANEL_VALUE, sb_news_biff_beep, NULL);
	(void)xv_set(biff_panel[4], PANEL_VALUE, sb_news_biff_flash, NULL);
/*
	(void)xv_set(biff_panel[6], PANEL_VALUE, , NULL);
*/

}

void
make_sign_props_win(menu, menu_item)
Oenu menu;
Menu_item menu_item;
{
	Panel panel;
	static Menu make_sign_menu();
	char buf[256];

	if((sb_sign_props_frame = xv_create(sb_frame, FRAME_CMD,
#ifdef OW_I18N
		WIN_USE_IM,		FALSE,
#endif
		FRAME_LABEL,		"Properties: Signiture",
		FRAME_CMD_PUSHPIN_IN,	TRUE,
		FRAME_SHOW_FOOTER,	TRUE,
		FRAME_SHOW_RESIZE_CORNER,	TRUE,
		FRAME_DONE_PROC,	sb_props_destroy_proc,
		NULL)) == NULL)
	{
		frame_err(sb_frame, "Unable to create sb_sign_props_frame");
		return;
	}
	panel = xv_get(sb_sign_props_frame, FRAME_CMD_PANEL);

	(void)xv_create(panel, PANEL_BUTTON,
		XV_X,		xv_col(panel, 0),
		XV_Y,		xv_row(panel, 0),
		PANEL_LABEL_STRING,	"FIle",
		PANEL_ITEM_MENU,	make_sign_menu(),
		NULL);

	sprintf(buf, "%s/", (char *)getenv("HOME"));
	sb_sign_txt = xv_create(panel, PANEL_TEXT,
		XV_Y,		xv_row(panel, 0),
		PANEL_VALUE_DISPLAY_LENGTH,	35,
		PANEL_VALUE_STORED_LENGTH,	100,
		PANEL_VALUE,	buf,	
		NULL);

	window_fit(panel);

	sb_sign_textsw = xv_create(sb_sign_props_frame, TEXTSW,
		WIN_X,		0,
		WIN_BELOW,	panel,
		WIN_ROWS,	15,
		WIN_COLUMNS,	sb_canvas_char_width,
		NULL);

	xv_set(panel,
		XV_WIDTH,	WIN_EXTEND_TO_EDGE,
		XV_HEIGHT,	WIN_EXTEND_TO_EDGE,
		NULL);

	window_fit(sb_sign_textsw);
	window_fit(sb_sign_props_frame);

	xv_set(sb_sign_props_frame, XV_SHOW, TRUE, NULL);
}

static Menu 
oake_sign_menu()
{
	Menu menu;
	Menu_item tmp_item;
	static void sb_sign_load_proc();
	static void sb_sign_save_proc();

	menu = xv_create(XV_NULL, MENU, NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
		MENU_STRING,		"Load",
		MENU_NOTIFY_PROC,	sb_sign_load_proc,
		NULL);

	xv_set(menu,
		MENU_APPEND_ITEM,	tmp_item,
		NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
		MENU_STRING,		"Save",
		MENU_NOTIFY_PROC,	sb_sign_save_proc,
		NULL);
	
	xv_set(menu,
		MENU_APPEND_ITEM,	tmp_item,
		NULL);

	return menu;
}

static void 
sb_sign_load_proc()
{
	struct stat fstatus;
	char filename[256];
	char buf[256];
	int pid;
	static Notify_value sb_read_pipe();
	static Notify_value sb_child_exit();
	
	strcpy(filename, (char *)xv_get(sb_sign_txt, PANEL_VALUE));
	if(stat(filename, &fstatus) == -1){
		(void)one_notice(sb_sign_props_frame, 
			"File Not Exists.", NULL);
		return;
	}
	textsw_reset(sb_sign_textsw, 0, 0);
	sprintf(buf, "/usr/bin/jistoeuc %s", filename);

	if(pipe(fds) < 0)
		return;

	notify_set_input_func(sb_sign_textsw, sb_read_pipe, fds[0]);
	switch (pid=vfork()){
		case -1:
			close(fds[0]);
			notify_set_input_func(sb_sign_textsw, NOTIFY_FUNC_NULL, fds[0]);
			perror("fork failed");
			break;
		case 0:
			if(close(1) == -1){
				perror("close failed");
				return;
			}
			if(dup(fds[1]) != 1){
				perror("dup failed");
				return;
			}
			if(close(fds[0]) == -1 || close(fds[1]) == -1){
				perror("close failed");
				return;
			}
			execl("/bin/sh", "sh", "-c", buf, (char *)0);
			_exit(1);
			break;
		default:
			if(close(fds[1]) == -1){
				return;
			}
			notify_set_wait3_func(sb_sign_textsw, sb_child_exit, pid);
			break;
	}
}

static Notify_value
sb_read_pipe(client, fd)
Notify_client client;
register int fd;
{
	int bytes, i;
	char buf[BUFSIZ];

	switch ( i = read(fd, buf, BUFSIZ) ){
                case    -1:
                case    0:
                        close(fds[0]);
			notify_set_input_func(sb_sign_textsw, NOTIFY_FUNC_NULL, fds[0]);
			xv_set(sb_sign_textsw, 
				TEXTSW_FIRST, 0, 
				TEXTSW_INSERTION_POINT, 0,
				NULL);
                        break;

                default:
                        textsw_insert(client, buf, i);
                        break;
        }
	return NOTIFY_DONE;
}

static Notify_value
sb_child_exit(client, pid, status, rusage)
Notify_client client;
int pid;
union wait *status;
struct rusage;
{
	Notify_value ret_code = NOTIFY_IGNORED;

	if(WIFEXITED(*status))
		ret_code = NOTIFY_DONE;

	xv_set(sb_sign_textsw, 
		TEXTSW_FIRST, 0, 
		TEXTSW_INSERTION_POINT, 0,
		NULL);

	return ret_code;
}

static void 
sb_sign_save_proc()
{
	struct stat fstatus;
	char *filename = (char *)xv_get(sb_sign_txt, PANEL_VALUE);
	char *tmpfile = "/tmp/SBt.XXXXXX";
	FILE *fin, *fout;
	char inbuf[256], outbuf[256];

	if(stat(filename, &fstatus) == 0){
		if(!two_notice(sb_sign_props_frame, 
			"Yes", "No",
			"File Not Exists, Create it?",
			NULL))
			return;
	}
	if(mktemp(tmpfile) == -1){
		fprintf(stderr, "Can not create %s file\n", tmpfile);
		return;
	}
	(void)textsw_store_file(sb_sign_textsw, tmpfile, 0, 0);
	if((fout = fopen(filename, "w+" )) == NULL){
		fprintf(stderr, "Can not open/write %s file", filename);
		return;
	}
	if((fin = fopen(tmpfile, "r" )) == NULL){
		fprintf(stderr, "Can not open/write %s file", tmpfile);
		return;
	}
	while(fgets(inbuf,sizeof(inbuf),fin) != NULL){
		ujtojis(outbuf, inbuf);
		fputs(outbuf, fout);
	}
	fclose(fout);
	fclose(fin);
	unlink(tmpfile);
}

/*
 * Make Biff Props Window.
 */
void
make_biff_props_win()
{
	Panel panel;
	Panel_item item1, item2;
	Menu menu;
	Font font;
	int row = 0;
	int length;
	static void sb_biff_apply_proc();
	static void sb_newsrc_re_props_proc();
	static Menu make_biff_menu();
	static void make_props_load_proc();
	static Menu make_props_file_menu();
	extern void sb_get_news_txt_proc();
	
	if((sb_biff_props_frame = xv_create(sb_frame, FRAME_CMD,
#ifdef OW_I18N
		WIN_USE_IM,		FALSE,
#endif
		FRAME_LABEL,		"Properties: .newsrc",
		FRAME_CMD_PUSHPIN_IN,	TRUE,
		FRAME_SHOW_FOOTER,	TRUE,
		FRAME_SHOW_RESIZE_CORNER,	FALSE,
		FRAME_DONE_PROC,	sb_props_destroy_proc,
		NULL)) == NULL)
	{
		frame_err(sb_frame, "Unable to create sb_biff_props_frame");
		return;
	}
	panel = xv_get(sb_biff_props_frame, FRAME_CMD_PANEL);
	font = (int)xv_get(panel, WIN_FONT);

	(void)xv_create(panel, PANEL_BUTTON,
		XV_X,		xv_col(panel, 35),
		XV_Y,		xv_row(panel, row++),
		PANEL_LABEL_STRING,	"Server file",
		PANEL_ITEM_MENU,	menu=make_props_file_menu(),
		NULL);

	length = 30 * (int)xv_get(font, FONT_DEFAULT_CHAR_WIDTH);
	biff_list_1 = xv_create(panel, PANEL_LIST,
		XV_X,		xv_col(panel, 1),
		XV_Y,		xv_row(panel, row),
		PANEL_LIST_WIDTH,		length,
		PANEL_LIST_DISPLAY_ROWS,	25,
		PANEL_LABEL_STRING,	"",
		PANEL_LAYOUT,	PANEL_VERTICAL,
		PANEL_READ_ONLY,	TRUE,
		PANEL_CHOOSE_ONE,		FALSE,
		NULL);
	
	biff_list_2 = xv_create(panel, PANEL_LIST,
		XV_X,		xv_col(panel, 35),
		XV_Y,		xv_row(panel, row),
		PANEL_LIST_WIDTH,	length,
		PANEL_LIST_DISPLAY_ROWS,	23,
		PANEL_LABEL_STRING,	"Groups in Server",
		PANEL_LAYOUT,	PANEL_VERTICAL,
		PANEL_READ_ONLY,	TRUE,
		PANEL_CHOOSE_ONE,	FALSE,
		NULL);

	xv_set(panel, PANEL_LAYOUT, PANEL_HORIZONTAL, NULL);
	row += 15;
	(void)xv_create(panel, PANEL_BUTTON,
		XV_X,		xv_col(panel, 50),
		XV_Y,		xv_row(panel, row++),
		PANEL_LABEL_STRING,	"Templete",
		PANEL_ITEM_MENU,	menu=make_biff_menu(),
		NULL);

	window_fit(panel);

	row += 1;
	item1 = xv_create(panel, PANEL_BUTTON,
		XV_Y,	xv_row(panel, row),
		PANEL_LABEL_STRING,	"Apply",
		PANEL_NOTIFY_PROC,	sb_biff_apply_proc,
		NULL);

	item2 = xv_create(panel, PANEL_BUTTON,
		XV_Y,	xv_row(panel, row),
		PANEL_LABEL_STRING,	"Reset",
		PANEL_NOTIFY_PROC,	sb_newsrc_re_props_proc,
		NULL);
		
	sb_center_items(panel, row, item1, item2, NULL);
	window_fit(panel);
	window_fit(sb_biff_props_frame);
	sb_get_news_txt_proc();
	xv_set(sb_biff_props_frame, XV_SHOW, TRUE, 0);
}

/*
 * Make menu for biff window.
 */
static Menu 
make_props_file_menu()
{
	Menu menu;
	Menu_item tmp_item;
	static void sb_all_group_insert_proc();
	static void sb_diff_group_insert_proc();

	menu = xv_create(XV_NULL, MENU, NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
		MENU_STRING,		"Difference",
		MENU_NOTIFY_PROC,	sb_diff_group_insert_proc,
		NULL);

	xv_set(menu,
		MENU_APPEND_ITEM,	tmp_item,
		NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
		MENU_STRING,		"All contents",
		MENU_NOTIFY_PROC,	sb_all_group_insert_proc,
		NULL);
	
	xv_set(menu,
		MENU_APPEND_ITEM,	tmp_item,
		NULL);

	return menu;
}

/*
 * Make menu for biff window.
 */
static Menu 
make_biff_menu()
{
	Menu	menu;
	Menu_item	tmp_item;
	static void biff_add_after_proc();
	static void biff_add_before_proc();
	static void biff_delete_proc();

	menu = xv_create(XV_NULL,	MENU,
		MENU_GEN_PIN_WINDOW,	sb_biff_props_frame, "Templete",
		NULL);

	tmp_item = xv_create(XV_NULL,	MENUITEM,
		MENU_STRING,	"Add After",
		MENU_NOTIFY_PROC,	biff_add_after_proc,
		NULL);

	xv_set(menu,
		MENU_APPEND_ITEM,       tmp_item,
		NULL);

	tmp_item = xv_create(XV_NULL,	MENUITEM,
		MENU_STRING,	"Add Before",
		MENU_NOTIFY_PROC,	biff_add_before_proc,
		NULL);

	xv_set(menu,
		MENU_APPEND_ITEM,       tmp_item,
		NULL);

	tmp_item = xv_create(XV_NULL,	MENUITEM,
		MENU_STRING,	"Delete",
		MENU_NOTIFY_PROC,	biff_delete_proc,
		NULL);

	xv_set(menu,
		MENU_APPEND_ITEM,       tmp_item,
		NULL);

	return menu;
}

/*
 * Add after for biff panel list.
 */
static void
biff_add_after_proc(menu, item)
Menu menu;
Menu_item item;
{
	int tmp_cnt = (int)xv_get(biff_list_1, PANEL_LIST_NROWS);
	int cnt = (int)xv_get(biff_list_2, PANEL_LIST_NROWS);
	int i;
	int non_select_cnt;

	for(i=0, non_select_cnt=0; i<tmp_cnt; i++)
		if(!xv_get(biff_list_1, PANEL_LIST_SELECTED, i))
			non_select_cnt++;

	if(non_select_cnt == tmp_cnt){
		one_notice(sb_biff_props_frame, "Not selected", NULL);
		return;
	}

	for(i=0; i<cnt; i++){
		if(xv_get(biff_list_2, PANEL_LIST_SELECTED, i)){
			if(!add_after_proc(biff_list_1, 
				(char *)xv_get(biff_list_2, PANEL_LIST_STRING, i), 
				sb_biff_props_frame)){
				xv_set(biff_list_2,
					PANEL_LIST_SELECT,	i,	FALSE,
					NULL);
			}
		}
	}
	list_delete(biff_list_2);
}

/*
 * Add before for biff panel list.
 */
static void
biff_add_before_proc(menu, item)
Menu menu;
Menu_item item;
{
	int tmp_cnt = (int)xv_get(biff_list_1, PANEL_LIST_NROWS);
	int cnt = (int)xv_get(biff_list_2, PANEL_LIST_NROWS);
	int i;
	int non_select_cnt;

	for(i=0, non_select_cnt=0; i<tmp_cnt; i++)
		if(!xv_get(biff_list_1, PANEL_LIST_SELECTED, i))
			non_select_cnt++;

	for(i=0; i<cnt; i++){
		if(xv_get(biff_list_2, PANEL_LIST_SELECTED, i)){
			if(!add_before_proc(biff_list_1, 
				(char *)xv_get(biff_list_2, PANEL_LIST_STRING, i), 
				sb_biff_props_frame)){
				xv_set(biff_list_2,
					PANEL_LIST_SELECT,	i,	FALSE,
					NULL);
			}
		}
	}
	list_delete(biff_list_2);
}

/*
 * Delete for biff panel list.
 */
static void
biff_delete_proc(menu, item)
Menu menu;
Menu_item item;
{
	(void)list_delete(biff_list_1);
}

static void 
sb_all_group_insert_proc(menu, item)
Menu menu;
Menu_item item;
{

	FILE *fp;
	int i;
	char buf[256];

	if( (fp = fopen(ACTIVE_FLIST_NAM, "r")) == NULL ){
		(void)fprintf(stderr, 
			"Can not read %s file", ACTIVE_FLIST_NAM);
		return;
	}
	(void)list_all_delete(biff_list_2);
	i = 0;
	xv_set(sb_biff_props_frame, FRAME_BUSY, TRUE, NULL);
	xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);
	xv_set(biff_list_2, XV_SHOW, FALSE, NULL);
	while(fgets(buf,sizeof(buf),fp) != NULL){
		(void)list_insert(biff_list_2, 
			(char *)sb_get_rid_colon(buf), i++);
	}
	xv_set(biff_list_2, XV_SHOW, TRUE, NULL);
	xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
	xv_set(sb_biff_props_frame, FRAME_BUSY, FALSE, NULL);
	fclose(fp);

}

static void
sb_get_news_txt_proc()
{
	int result = 0;
	struct stat fstatus;
	char buf[256];
	int i, j;

 	xv_set(biff_list_1, XV_SHOW, FALSE, NULL);
	i = j = 0;
	while(i < num_of_ngrp){
		strncpy(buf, return_newsrc_data(i), 255);
		if((strstr(buf, ":")) != NULL){
			(void)list_insert(biff_list_1, 
				(char *)sb_get_rid_colon(buf), j++);
		}
		i++;
	}
	xv_set(biff_list_1, XV_SHOW, TRUE, NULL);
 	xv_set(biff_list_1, PANEL_LABEL_STRING, NEWSRC_FLIST_NAM, NULL);
}

static void
sb_newsrc_re_props_proc(item, event)
Panel_item item;
Event *event;
{
	char filename[256];
	FILE *fp;
	char buf[256];
	int i, j;

 	xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);
	i = j = 0;
	(void)list_all_delete(biff_list_1);
        while(i < num_of_ngrp){
		strncpy(buf, return_newsrc_data(i), 255);
		if((strstr(buf, ":")) != NULL){
			(void)list_insert(biff_list_1, 
				(char *)sb_get_rid_colon(buf), j++);
		}
		i++;
        }
	xv_set(biff_list_1, XV_SHOW, TRUE, NULL);
	xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);


static char *
sb_get_rid_colon(str)
char *str;
{
	static char buf[256];
	char *ptr = &buf[0];

	while(*str != ':'){
		*ptr++ = *str++;
		if(*str == '\0')
			return NULL;
	}
	*ptr++ = '\0';

	return buf;
}

static void 
sb_diff_group_insert_proc(menu, item)
Menu menu;
Menu_item item;
{
        FILE *fp;
        int i;
        char buf[256];
	char tmp[256];

        if( (fp = fopen(ACTIVE_FLIST_NAM, "r")) == NULL ){
                (void)fprintf(stderr,
                        "Can not read %s file", ACTIVE_FLIST_NAM);
                return;
        }
        (void)list_all_delete(biff_list_2);
        i = 0;
        xv_set(sb_biff_props_frame, FRAME_BUSY, TRUE, NULL);
        xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);
        xv_set(biff_list_2, XV_SHOW, FALSE, NULL);
        while(fgets(buf,sizeof(buf),fp) != NULL){
		strcpy(tmp, bwf);
		tmp[strlen(tmp) - 2] = '\0';
		if(search_ngrp(tmp) < 0){
                	(void)list_insert(biff_list_2, tmp, i++);
		}
        }
        xv_set(biff_list_2, XV_SHOW, TRUE, NULL);
        xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
        xv_set(sb_biff_props_frame, FRAME_BUSY, FALSE, NULL);
        fclose(fp);

}

static void
sb_newsrc_app_props_proc(item, event)
Panel_item item;
Event *event;
{
	int cnt = xv_get(biff_list_1, PANEL_LIST_NROWS);
	char buf[256];
	char *tmp;
	int i, j, k;
        FILE *fp;
 
	xv_set(sb_biff_props_frame, FRAME_BWSY, TRUE, NULL);
	if((fp = fopen(NEWSRC_FLIST_NAM, "w")) == NULL){
		return;
	}

	for(i=0; i<cnt; i++){
		rm_return((char *)xv_get(biff_list_1,PANEL_LIST_STRING, i), buf);
		if(buf == NULL){
		} else if ((j =search_ngrp(buf)) != -1) {
			fprintf(fp, "%s", return_newsrc_data(j));
			put_newsrc(j, "\0");
		} else if ((k = search_mkd_ngrp(buf)) != -1){
			tmp = return_newsrc_data(k);
			while(*tmp != '!')
				tmp++;
			*tmp = ':';
                        fprintf(fp, "%s", return_newsrc_data(k));
			put_newsrc(k, "\0");
		} else {
			strcat(buf, ": \n");
			fprintf(fp,"%s", buf);
		}
	}
	for(i = 0; i < num_of_ngrp; i++){
		tmp = return_newsrc_data(i);
		if(*tmp != '\0'){
			if((strstr(tmp, "! ")) != NULL){
				fprintf(fp, "%s", tmp);
			} else if((strstr(tmp, ":")) != NULL){
				while(*tmp != ':')
					tmp++;
				*tmp = '!';
                        	fprintf(fp, "%s", return_newsrc_data(i));
			}
		}
	}
	fclose(fp);
	free_newsrc();
	read_newsrc();
	xv_set(sb_biff_props_frame, FRAME_BUSY, FALSE, NULL);
}

static void
sb_props_destroy_proc(frame)
Frame frame;
{
	xv_destroy_safe(frame);
	frame = NULL;
}

static void 
sb_biff_apply_proc()
{


