#ifdef sccs
static char     sccsid[] = "%W% %E%";
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
#ifdef JLE
#include <jcode.h>
#endif

#include <xview/xview.h>
#include <xview/text.h>
#include <xview/panel.h>
#include <xview/cursor.h>

#include "sb_tool.h"
#include "sb_def.h"

extern Frame    sb_frame;

static Frame    sb_sign_props_frame;
static Panel_item sb_sign_txt;
static Textsw   sb_sign_textsw;

void
make_sign_props_win(menu, menu_item)
	Menu            menu;
	Menu_item       menu_item;
{
	Panel           panel;
	static Menu     make_sign_menu();
	static void     sb_sign_load_proc();
	static Notify_value sb_sign_frame_destroy_proc();
	static Notify_value sb_sign_pinout_proc();
	char            buf[256];

	if (sb_sign_props_frame) {
		(void) xv_set(sb_sign_props_frame, WIN_FRONT, XV_SHOW, TRUE, NULL);
		return;
	}
	if ((sb_sign_props_frame = xv_create(sb_frame, FRAME_CMD,
#ifdef OW_I18N
					     WIN_USE_IM, TRUE,
#endif
#ifdef OW_I18N
			      FRAME_LABEL, gettext("Properties: Signature"),
#else
				       FRAME_LABEL, "Properties: Signature",
#endif
					     FRAME_CMD_PUSHPIN_IN, TRUE,
					     FRAME_SHOW_FOOTER, TRUE,
					     FRAME_SHOW_RESIZE_CORNER, TRUE,
					FRAME_DONE_PROC, frame_destroy_proc,
					     NULL)) == NULL) {
#ifdef OW_I18N
		frame_err(sb_frame, gettext("Unable to create sb_sign_props_frame"));
#else
		frame_err(sb_frame, "Unable to create sb_sign_props_frame");
#endif
		return;
	}
	xv_set(sb_frame, FRAME_BUSY, TRUE, NULL);

	panel = xv_get(sb_sign_props_frame, FRAME_CMD_PANEL);

	(void) xv_create(panel, PANEL_BUTTON,
			 XV_X, xv_col(panel, 0),
			 XV_Y, xv_row(panel, 0),
#ifdef OW_I18N
			 PANEL_LABEL_STRING, gettext("FIle"),
#else
			 PANEL_LABEL_STRING, "FIle",
#endif
			 PANEL_ITEM_MENU, make_sign_menu(),
			 NULL);

	sprintf(buf, "%s/.signature", (char *) getenv("HOME"));
	sb_sign_txt = xv_create(panel, PANEL_TEXT,
				XV_Y, xv_row(panel, 0),
				PANEL_VALUE_DISPLAY_LENGTH, 35,
				PANEL_VALUE_STORED_LENGTH, 100,
				PANEL_VALUE, buf,
				PANEL_NOTIFY_PROC, sb_sign_load_proc,
				NULL);

	window_fit(panel);

	sb_sign_textsw = xv_create(sb_sign_props_frame, TEXTSW,
				   WIN_X, 0,
				   WIN_BELOW, panel,
				   WIN_ROWS, 15,
				   WIN_COLUMNS,
				   (int) sb_get_props_value(Sb_pane_width),
				   NULL);

	xv_set(panel,
	       XV_WIDTH, WIN_EXTEND_TO_EDGE,
	       XV_HEIGHT, WIN_EXTEND_TO_EDGE,
	       NULL);

	window_fit(sb_sign_textsw);
	window_fit(sb_sign_props_frame);

	notify_interpose_destroy_func(sb_sign_props_frame,
				      sb_sign_frame_destroy_proc);

	notify_interpose_event_func(sb_sign_props_frame,
				    sb_sign_pinout_proc, NOTIFY_SAFE);

	xv_set(sb_sign_props_frame, XV_SHOW, TRUE, NULL);

	xv_set(sb_frame, FRAME_BUSY, FALSE, NULL);
}

static          Notify_value
sb_sign_frame_destroy_proc(client, status)
	Notify_client   client;
	Destroy_status  status;
{
	Notify_client   tmp = client;

	if (status == DESTROY_CHECKING)
		textsw_reset(sb_sign_textsw, 0, 0);

	xv_set(tmp, FRAME_NO_CONFIRM, TRUE, NULL);

	sb_sign_props_frame = NULL;

	return notify_next_destroy_func(tmp, status);
}

static          Notify_value
sb_sign_pinout_proc(client, event, arg, when)
	Notify_client   client;
	Event          *event;
	Notify_arg      arg;
	Notify_event_type when;
{
	Notify_value    return_value =
	notify_next_event_func(client, event, arg, NOTIFY_SAFE);
	static void     sb_sign_save_proc();

	if (event_action(event) == ACTION_PINOUT) {
		if (xv_get(sb_sign_textsw, TEXTSW_MODIFIED)) {
#ifdef OW_I18N
			if (two_notice((Frame) client, gettext("Apply"), gettext("Quit"),
				       gettext("Text has been changed"),
				       gettext("Applied or Not ?"),
				       NULL))
#else
			if (two_notice((Frame) client, "Apply", "Quit",
				       "Text has been changed",
				       "Applied or Not ?",
				       NULL))
#endif
				sb_sign_save_proc(NULL, NULL);
		}
		xv_destroy((Frame) client);
	}
	return return_value;
}

static          Menu
make_sign_menu()
{
	Menu            menu;
	Menu_item       tmp_item;
	static void     sb_sign_load_proc();
	static void     sb_sign_save_proc();

	menu = xv_create(XV_NULL, MENU, NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Load"),
#else
			     MENU_STRING, "Load",
#endif
			     MENU_NOTIFY_PROC, sb_sign_load_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	tmp_item = xv_create(XV_NULL, MENUITEM,
#ifdef OW_I18N
			     MENU_STRING, gettext("Save"),
#else
			     MENU_STRING, "Save",
#endif
			     MENU_NOTIFY_PROC, sb_sign_save_proc,
			     NULL);

	xv_set(menu,
	       MENU_APPEND_ITEM, tmp_item,
	       NULL);

	return menu;
}

static void
sb_sign_load_proc()
{
	struct stat     fstatus;
	char            filename[256];
#ifdef MLE
	char            buf[256];
	char            tmpfile[256];
#endif

	strcpy(filename, (char *) xv_get(sb_sign_txt, PANEL_VALUE));
	if (stat(filename, &fstatus) == -1) {
#ifdef OW_I18N
		(void) one_notice(sb_sign_props_frame,
				  gettext("File Not Exists."), NULL);
#else
		(void) one_notice(sb_sign_props_frame,
				  "File Not Exists.", NULL);
#endif
		return;
	}
	xv_set(sb_sign_props_frame, FRAME_BUSY, TRUE, NULL);

#ifdef MLE
	sprintf(tmpfile, "/tmp/SB.sign_tmp.%d", (int) getpid());
#ifdef EUCNEWS
	sprintf(buf, "/usr/bin/jistoeuc %s >> %s 2>&1", filename, tmpfile);
#endif
	system(buf);
#endif
	textsw_reset(sb_sign_textsw, 0, 0);
	xv_set(sb_sign_textsw,
#ifdef MLE
	       TEXTSW_FILE_CONTENTS, tmpfile,
#else
	       TEXTSW_FILE_CONTENTS, filename,
#endif
	       TEXTSW_FIRST, 0,
	       TEXTSW_INSERTION_POINT, 0,
	       NULL);

#ifdef MLE
	(void) unlink(tmpfile);
#endif
	xv_set(sb_sign_textsw, WIN_SET_FOCUS, NULL);

	xv_set(sb_sign_props_frame, FRAME_BUSY, FALSE, NULL);
}

static void
sb_sign_save_proc()
{
	struct stat     fstatus;
	char           *filename = (char *) xv_get(sb_sign_txt, PANEL_VALUE);
#ifdef MLE
	char           *tmpfile = "/tmp/SB.tmp.XXXXXX";
	FILE           *fin, *fout;
	char            inbuf[256], outbuf[256];
#endif

#ifdef MLE
	if (stat(filename, &fstatus) == 0) {
#ifdef OW_I18N
		if (!two_notice(sb_sign_props_frame,
				gettext("Yes"), gettext("No"),
				gettext("File Exists, Overwrite it?"),
				NULL))
#else
		if (!two_notice(sb_sign_props_frame,
				"Yes", "No",
				"File Exists, Overwrite it?",
				NULL))
#endif
			return;
	}
	if (mktemp(tmpfile) == -1) {
		fprintf(stderr, "Can not create %s file\n", tmpfile);
		return;
	}
	(void) textsw_store_file(sb_sign_textsw, tmpfile, 0, 0);
	if ((fout = fopen(filename, "w")) == NULL) {
		fprintf(stderr, "Can not open/write %s file", filename);
		return;
	}
	if ((fin = fopen(tmpfile, "r")) == NULL) {
		fprintf(stderr, "Can not open/write %s file", tmpfile);
		return;
	}
	while (fgets(inbuf, sizeof(inbuf), fin) != NULL) {
#ifdef EUCNEWS
		ujtojis(outbuf, inbuf);
#else
		strcpy(outbuf, inbuf);
#endif
		fputs(outbuf, fout);
	}
	fclose(fout);
	fclose(fin);
	unlink(tmpfile);
#else
	(void) textsw_store_file(sb_sign_textsw, filename, 0, 0);
#endif

	frame_msg(sb_sign_props_frame,
#ifdef OW_I18N
		  gettext("Text has been saved."),
#else
		  "Text has been saved.",
#endif
		  NULL);

}
