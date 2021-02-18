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

#include <X11/Xlib.h>

#include <xview/xview.h>
#include <xview/textsw.h>
#include <xview/panel.h>
#include <xview/icon.h>
#include <xview/server.h>
#include <xview/seln.h>
#include <xview/font.h>
#include <xview/canvas.h>
#include <xview/scrollbar.h>
#include <xview/notify.h>
#include <xview/cursor.h>

#include "config.h"
#include "sb_tool.h"
#include "sb_item.h"
#include "sb_canvas.h"

extern void     make_main_frame();
extern int      make_main_panel();
extern int      make_main_textsw();
extern void     sb_init_proc();
extern Notify_value sb_frame_event_func();
extern Notify_value sb_frame_destroy_func();
extern void     sb_signal_set();

void
main(argc, argv)
	int             argc;
	char          **argv;
{


	sb_cmd_parse(argc, argv);

	/* connect X server */
	xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv,
#ifdef OW_I18N
		XV_USE_LOCALE, TRUE,
#endif
		NULL);

#ifdef MLE
	{

		char            bind_home[512];
		char           *shinbunhome;

		textdomain("shinbun.messages");

		if ((shinbunhome = (char *) getenv("SHINBUNHOME")) != NULL)
			strcpy(bind_home, shinbunhome);

		else if ((shinbunhome = (char *) getenv("OPENWINHOME")) != NULL)
			sprintf(bind_home, "%s/lib/locale", shinbunhome);

		else
			strcpy(bind_home, "/usr/lib/X11/locale");

		if (bindtextdomain("shinbun.messages", bind_home) == NULL) {
			/*
			 * fprintf(stderr, "bindtextdomain failed, path:
			 * %s\n", bind_home);
			 */
		}
	}
#endif

	/* Make icon */
	(void) sb_icon_create();

	/* Make Main Frame */
	(void) make_main_frame();

	/*
	 * if((char *)getenv("SHINBUN_INTERNAL_MODE") != NULL){
	 * internal_news_mode(); }
	 */

	/* Set signals */
	(void) sb_signal_set(sb_frame);

	/* Make main panel.  */
	sb_panel = (Panel) xv_create(sb_frame, PANEL, NULL);
	window_check(sb_panel, "sb_panel");

	/* Make main panel item on main panel.  */
	if (make_main_panel(sb_panel) != 1) {
		fprintf(stderr, "Can not create sb_panel frame\n");
		exit(1);
	}
	window_fit(sb_panel);
	(void) xv_set(sb_panel, XV_WIDTH, WIN_EXTEND_TO_EDGE, NULL);

	/* Initial newsgroup display.  */
	(void) sb_init_proc();

	/* Make main canvas in main panel.  */
	if (!make_main_canvas()) {
		fprintf(stderr, "Can not create sb_canvas frame\n");
		exit(1);
	}
	(void) window_fit(sb_canvas);
	(void) window_fit(sb_frame);

	/* Set notifier to  main frame.  */
	(void) notify_interpose_event_func(sb_frame, sb_frame_event_func,
					   NOTIFY_SAFE);
	(void) notify_interpose_destroy_func(sb_frame, sb_frame_destroy_func);

	shinbun_init();
	connect_nntp_server();

	if (sb_create_newsrc_file() == TRUE)
		read_newsrc();

	read_newsbiffrc();

	(void) xv_main_loop(sb_frame);

	exit(0);
}
