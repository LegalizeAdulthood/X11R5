#ifdef sccs
static char     sccsid[] = "@(#)sb_canvas_pane.c	1.9 91/09/12";
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

#include <X11/X.h>
#include <X11/Xlib.h>
#include <xview/xview.h>
#include <xview/canvas.h>
#include <xview/scrollbar.h>
#include <xview/font.h>
#include <xview/cms.h>
#include <xview/panel.h>
#include <xview/seln.h>
#include <xview/textsw.h>

#include "sb_def.h"

#define DEFAULT_SPACE_X		30	/* Initial x */
#define DEFAULT_NEW_MARK_POSITION	2	/* Initial X of 'N' */
#define DEFAULT_CUR_MARK_POSITION	15	/* Initial X of '=>' */
#define DEFAULT_SPACE		3	/* Space between the msg of the
					 * header pane.  */

long            dclick_usec;	/* Double click */
int             sb_font_height;
int             sb_font_width;
int             sb_line_height;
int             sb_line_width;
int             sb_num_of_article;	/* actual numbgr of articles */
char           *sb_seln_file = "/tmp/SB.seln.XXXXXX";

Scrollbar       sb_scrollbar;
Canvas          sb_canvas;
Drawable        sb_draw;
Display        *sb_dpy;
GC              sb_gc;
GC              sb_cleargc;

Seln_client     sb_seln_client;
Seln_rank       sb_select_flag;

extern Frame    sb_frame;
extern Panel    sb_panel;

extern int      sb_func_key_proc();
extern Seln_result sb_reply_proc();
extern Notify_value sb_canvas_key_proc();
extern Notify_value sb_canvas_proc();
extern void     sb_canvas_repaint_proc();

/*
 * Set Canvas & graphics.
 */
int
make_main_canvas()
{
	Xv_Window       window;
	Xv_Font         font;
	Cms             cms;
	XGCValues       gv;
	int             fore;
	int             back;

	/* create selection svc on the main header */
	sb_seln_client = seln_create(sb_func_key_proc, sb_reply_proc, (char *) 0);
	if (sb_seln_client == NULL) {
		fprintf(stderr, "shinbun tool: seln_create failed!\n");
		exit(1);
	}
	sb_canvas = (Canvas) xv_create(sb_frame, CANVAS,
#ifdef OW_I18N
				       WIN_USE_IM, FALSE,
#endif
				       WIN_X, 0,
				       WIN_BELOW, sb_panel,
				       XV_WIDTH, WIN_EXTEND_TO_EDGE,
				       XV_HEIGHT, WIN_EXTEND_TO_EDGE,
				       CANVAS_AUTO_EXPAND, TRUE,
				       CANVAS_AUTO_SHRINK, FALSE,
				       CANVAS_NO_CLIPPING, TRUE,
				       CANVAS_FIXED_IMAGE, TRUE,
				       CANVAS_RETAINED, FALSE,
				       CANVAS_X_PAINT_WINDOW, TRUE,
				       CANVAS_AUTO_CLEAR, TRUE,
				       WIN_NO_CLIPPING, FALSE,
				       WIN_CLIENT_DATA, make_canvas_menu(),
				       NULL);

	font = (Xv_Font) xv_get(sb_frame, XV_FONT);
	sb_font_height = (int) xv_get(font, FONT_DEFAULT_CHAR_HEIGHT);
	sb_font_width = (int) xv_get(font, FONT_DEFAULT_CHAR_WIDTH);
	sb_line_height = DEFAULT_SPACE + sb_font_height;

	sb_scrollbar = xv_create(sb_canvas, SCROLLBAR,
				 SCROLLBAR_DIRECTION, SCROLLBAR_VERTICAL,
				 SCROLLBAR_PIXELS_PER_UNIT,
				 DEFAULT_SPACE + sb_font_height,
				 SCROLLBAR_VIEW_LENGTH,
				 (int) sb_get_props_value(Sb_pane_height),
				 NULL);

	window = (Xv_Window) xv_get(sb_canvas, CANVAS_NTH_PAINT_WINDOW, 0);
	sb_dpy = (Display *) xv_get(window, XV_DISPLAY);
	sb_draw = (Drawable) xv_get(window, XV_XID);

	cms = (Cms) xv_get(window, WIN_CMS);
	fore = (int) xv_get(cms, CMS_FOREGROUND_PIXEL);
	back = (int) xv_get(cms, CMS_BACKGROUND_PIXEL);

	gv.function = GXcopy;
	gv.font = (Font) xv_get(font, XV_XID);
	gv.foreground = fore;
	gv.background = back;
	gv.graphics_exposures = FALSE;

	sb_gc = XCreateGC(sb_dpy, sb_draw,
			  GCFunction | GCForeground | GCBackground | GCFont |
			  GCGraphicsExposures,
			  &gv);

	gv.foreground = back;
	gv.background = fore;

	sb_cleargc = XCreateGC(sb_dpy, sb_draw,
			 GCFunction | GCForeground | GCBackground | GCFont |
			       GCGraphicsExposures,
			       &gv);

	xv_set(canvas_pixwin(sb_panel),
	       WIN_EVENT_PROC, sb_canvas_key_proc,
	       WIN_CONSUME_EVENTS,
	       KBD_DONE, KBD_USE, LOC_DRAG, LOC_MOVE, LOC_WINENTER,
	       LOC_WINEXIT, WIN_ASCII_EVENTS, WIN_MOUSE_BUTTONS,
	       WIN_LEFT_KEYS, WIN_RIGHT_KEYS,
	       NULL,
	       NULL);

	xv_set(window,
	       WIN_EVENT_PROC, sb_canvas_proc,
	       WIN_CONSUME_EVENTS,
	       KBD_DONE, KBD_USE, LOC_DRAG, LOC_MOVE, LOC_WINENTER,
	       LOC_WINEXIT, WIN_ASCII_EVENTS, WIN_MOUSE_BUTTONS,
	       WIN_LEFT_KEYS, WIN_RIGHT_KEYS,
	       NULL,
	       NULL);

#ifdef SB_INIT_PROC
	(void) sb_canvas_force_repaint_proc((char *) get_current_newsgroup());
#endif

	xv_set(sb_canvas,
	       CANVAS_REPAINT_PROC, sb_canvas_repaint_proc,
	       NULL);

	dclick_usec = 100000 * (int) defaults_get_integer(
			       "MultiClickTimeout", "MultiClickTimeout", 4);

	(void) sb_canvas_layout_func();
#ifdef SB_INIT_PROC
	(void) sb_canvas_shrink_proc();
#endif

	sb_select_flag = SELN_UNKNOWN;

	return TRUE;
}

void
sb_draw_text(x, y, str)
	int             x;
	int             y;
	char           *str;
{
	XDrawString(sb_dpy, sb_draw, sb_gc, x, y, str, strlen(str));
}

void
sb_draw_rectangle(line)
	int             line;
{
	int             x, y, height;

	x = DEFAULT_SPACE_X - 2;
	y = line * sb_line_height - sb_font_height + 1;
	height = sb_line_height - 2;

	XDrawRectangle(sb_dpy, sb_draw, sb_gc, x, y, sb_line_width, height);
}

void
sb_clear_rectangle(line)
	int             line;
{
	int             x, y, height;

	x = DEFAULT_SPACE_X - 2;
	y = line * sb_line_height - sb_font_height + 1;
	height = sb_line_height - 2;

	XDrawRectangle(sb_dpy, sb_draw, sb_cleargc,
		       x, y, sb_line_width, height);
}

void
sb_draw_new_mark(line)
	int             line;
{
	int             x, y;

	x = DEFAULT_NEW_MARK_POSITION;
	y = line * sb_line_height;

	XDrawString(sb_dpy, sb_draw, sb_gc, x, y, "N", 1);
}

void
sb_clear_new_mark(line)
	int             line;
{
	int             x, y;

	x = DEFAULT_NEW_MARK_POSITION;
	y = line * sb_line_height;

	XDrawString(sb_dpy, sb_draw, sb_cleargc, x, y, "N", 1);
}

void
sb_draw_current_mark(line)
	int             line;
{
	int             x, y;

	x = DEFAULT_CUR_MARK_POSITION;
	y = line * sb_line_height;

	XDrawString(sb_dpy, sb_draw, sb_gc, x, y, "->", 2);
}

void
sb_clear_draw_current_mark(line)
	int             line;
{
	int             x, y;

	x = DEFAULT_CUR_MARK_POSITION;
	y = line * sb_line_height;

	XDrawString(sb_dpy, sb_draw, sb_cleargc, x, y, "->", 2);
}

void
sb_draw_cancel_mark(line)
	int             line;
{
	int             x, y;

	x = DEFAULT_NEW_MARK_POSITION;
	y = line * sb_line_height;

	XDrawString(sb_dpy, sb_draw, sb_gc, x, y, "C", 1);
}

void
sb_clear_pane()
{
	XClearWindow(sb_dpy, sb_draw);
}

void
sb_rectangle_position(line, min, max)
	int             line;
	int            *min;
	int            *max;
{
	*min = line * sb_line_height - sb_font_height + 1;
	*max = *min + sb_line_height - 2;
}
