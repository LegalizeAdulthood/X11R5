/*
 * $Id: ichiran.c,v 1.3 1991/09/16 21:35:15 ohm Exp $
 */
/*
 * Copyright 1989, 1990, 1991 by OMRON Corporation
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON and MIT not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON and MIT make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * OMRON, MIT AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL OMRON, MIT OR WNN CONSORTIUM BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	Author:	Seiji Kuwari	OMRON Corporation
 *				kuwa@omron.co.jp
 *				kuwa%omron.co.jp@uunet.uu.net
 */				
/*	Version 4.0
 */
#include <stdio.h>
#include <string.h>
#include "commonhd.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"

extern wchar	cursor_wchar;
extern wchar	wc_buf[];

extern XPoint button;
extern GC currentGC;
extern XSizeHints hints;

extern Cursor	cursor1;

static int
euclen(s)
char *s;
{
    register int n;
    register char *c;

    for (c = s, n = 0; *c != 0; c++, n++) {
	if ((*c & 0xff) == 0x8e) c++;
    }
    return n;
}

void
decide_position(width, height, ret_x, ret_y)
register int width, height;
register int *ret_x, *ret_y;
{
    if (xjutil->sel_button) {
	*ret_x = button.x - width/2;
	*ret_y = button.y - FontHeight;
	xjutil->sel_button = 0;
    } else {
	*ret_x = (DisplayWidth(dpy, screen) - width)/2;
	*ret_y = (DisplayHeight(dpy, screen) - height)/2;
    }
    if ((*ret_x + width) > DisplayWidth(dpy, screen))
	*ret_x = DisplayWidth(dpy, screen) - width;
    if (*ret_x < 0)
	*ret_x = 0;
    if ((*ret_y + height) > DisplayHeight(dpy, screen))
	*ret_y = DisplayHeight(dpy, screen) - height;
    if (*ret_y < 0)
	*ret_y = 0;
    return;
}

void
resize_text(text)
register JutilTextRec   *text;
{
    XMoveResizeWindow(dpy, text->w, 0, 0, 
		      FontWidth * (text->maxcolumns + 1), FontHeight);
    XMoveResizeWindow(dpy, text->wn[0], 0, 0, FontWidth, FontHeight);
    XMoveResizeWindow(dpy, text->wn[1], (text->width - FontWidth), 0, 
		      FontWidth, FontHeight);
    XMoveResizeWindow(dpy, text->wn[2], (text->width - FontWidth), 0, 
		      FontWidth, FontHeight);
    XSetWindowBackground(dpy, text->w, xjutil->bg);
    XSetWindowBackground(dpy, text->wn[0], xjutil->bg);
    XSetWindowBackground(dpy, text->wn[1], xjutil->bg);
    XSetWindowBackground(dpy, text->wn[2], xjutil->bg);
    XClearWindow(dpy, text->w);
    XClearWindow(dpy, text->wn[0]);
    XClearWindow(dpy, text->wn[1]);
    XClearWindow(dpy, text->wn[2]);
}

static int
Y_InItem(y)
register int y;
{
    if (y < 0 || y > xjutil->ichi->item_height) return(-1);
    return((y/(FontHeight + IN_BORDER)));
}

void
xw_mousemove(event)
XMotionEvent *event;
{
    static int y;
    int item;
    register Ichiran *ichi = xjutil->ichi;

    if(!ichi->lock && xjutil->mode & ICHIRAN_MODE) {
	y = event->y;
	item = Y_InItem(y);
	if (item != -1 && item != ichi->hilited_item) {
	    if (ichi->hilited_item != -1)
		xw_fillrec();
	    if ((ichi->hilited_item = item) != -1)
		xw_fillrec();
	}
    }
}

static void
xw_mouseleave()
{
    register Ichiran *ichi = xjutil->ichi;

    if(!ichi->lock && xjutil->mode & ICHIRAN_MODE && ichi->kosuu == 1) {
	if (ichi->hilited_item != -1) {
	    xw_fillrec();
	    ichi->hilited_item = -1;
	}
    }
}

void
init_ichiran(info, kosuu,init, title, comment)
unsigned char **info;
int kosuu, init;
unsigned char *title;
unsigned char *comment;
{
    register Ichiran *ichi = xjutil->ichi;
    int	x_cood, y_cood, width, height, bwidth;
    int	save_x, save_y;
    int		maxlen;
    int		def_maxlen;
    int		def_height;
    int		cnt, ret;
    int		len_t;
    Window	root, child;
    int	win_x, win_y;
    unsigned int keys_buttons;

    xjutil->mode |= ICHIRAN_MODE;
    ichi->invertgc = xjutil->invertgc;
    ichi->map = 0;

    if(comment != NULL) {
	ichi->comment->string = comment;
	maxlen =  euclen(comment);
    } else {
	ichi->comment->string = NULL;
    	maxlen = euclen(info[0]);
    }
    ichi->kosuu = kosuu;
    ichi->title->string = title;

    XSelectInput(dpy, ichi->w1, (EnterWindowMask | LeaveWindowMask |
				 PointerMotionMask | ButtonPressMask |
				 ButtonReleaseMask | ExposureMask));
    XDefineCursor(dpy, ichi->w1, cursor1);

    def_height = FontHeight * 4;

    ichi->start_line = 0;
    ichi->kosuu_all = kosuu;
    ichi->cur_page = 1;
    if (kosuu > MAX_PAGE_LINES) {
	ichi->page = kosuu / MAX_PAGE_LINES + (kosuu % MAX_PAGE_LINES ? 1 : 0);
	ichi->max_line = MAX_PAGE_LINES;
	ichi->end_line = ichi->max_line - 1;
	height = (MAX_PAGE_LINES * (FontHeight + IN_BORDER)) + (FontHeight * 2);
	def_maxlen = euclen(title) + euclen(ichi->button[CANCEL_W]->string)
		   + euclen(ichi->button[NEXT_W]->string)
		   + euclen(ichi->button[BACK_W]->string)
		   + SPACING * 4 + 1;
    } else {
	ichi->page = 1;
	ichi->max_line = kosuu;
	ichi->end_line = kosuu - 1;
	height = (kosuu * (FontHeight + IN_BORDER)) + (FontHeight * 2);
	def_maxlen = euclen(title) + euclen(ichi->button[CANCEL_W]->string)
		   + SPACING * 2 + 1;
    }
    if (comment) height += FontHeight + IN_BORDER * 2;
    for (cnt = 0; cnt < kosuu; cnt++) {
	ichi->buf[cnt] = info[cnt];
	if ((ret = euclen(ichi->buf[cnt])) > maxlen)
	    maxlen = ret + 2;
    }
    if (maxlen < def_maxlen) maxlen = def_maxlen;

    bwidth = 2;
    width  = ichi->item_width = maxlen * FontWidth;
    ichi->item_height = height;
    ichi->max_len = maxlen;

    XQueryPointer(dpy, root_window, &root, &child,
		  &save_x, &save_y, &win_x, &win_y, &keys_buttons);
    ichi->save_p.x = (short)save_x;
    ichi->save_p.y = (short)save_y;
    decide_position(width, height, &x_cood, &y_cood);
    XWarpPointer(dpy, None, root_window, 0, 0, 0, 0,
		 x_cood + FontWidth, y_cood + FontHeight);

    XMoveResizeWindow(dpy, ichi->w, x_cood, y_cood, width, height);

    moveresize_box(ichi->title, FontWidth, (FontHeight/2 - IN_BORDER),
		   (FontWidth * (len_t = euclen(title))) + (IN_BORDER * 2),
		   (FontHeight + (IN_BORDER * 2)));
    len_t += 1;
    moveresize_box(ichi->button[CANCEL_W],
		   (FontWidth * (len_t += SPACING)), (FontHeight/2 - IN_BORDER),
		   (FontWidth * euclen(ichi->button[CANCEL_W]->string)
		    + (IN_BORDER * 2)), FontHeight + (IN_BORDER * 2));
    len_t += euclen(ichi->button[CANCEL_W]->string);

    XSetWindowBackground(dpy, ichi->w, xjutil->bg);
    XClearWindow(dpy, ichi->w);
    XSetWindowBorder(dpy, ichi->w, xjutil->fg);
    changecolor_box(ichi->title, xjutil->fg, xjutil->bg);
    changecolor_box(ichi->button[CANCEL_W], xjutil->fg, xjutil->bg);

    if(comment != NULL) {
	y_cood = FontHeight * 3 + (IN_BORDER * 2);
	moveresize_box(ichi->comment, 0, FontHeight * 2,
		       width, (FontHeight + IN_BORDER * 2));
	changecolor_box(ichi->comment, xjutil->fg, xjutil->bg);
	map_box(ichi->comment);
    } else {
	y_cood = FontHeight * 2;
    }
    XMoveResizeWindow(dpy, ichi->w1, -1, y_cood, width,
		      ((FontHeight + IN_BORDER) * ichi->max_line));
    XSetWindowBackground(dpy, ichi->w1, xjutil->bg);
    XClearWindow(dpy, ichi->w1);
    XSetWindowBorder(dpy, ichi->w1, xjutil->fg);

    if(ichi->page > 1) {
	moveresize_box(ichi->button[NEXT_W],
		       (FontWidth * (len_t += SPACING)),
		       (FontHeight / 2 - IN_BORDER), 
		       (FontWidth * euclen(ichi->button[NEXT_W]->string)
			+ IN_BORDER * 2), (FontHeight + IN_BORDER * 2));
	len_t += euclen(ichi->button[NEXT_W]->string);
	moveresize_box(ichi->button[BACK_W],
		       (FontWidth * (len_t += SPACING)),
		       (FontHeight / 2 - IN_BORDER),
		       (FontWidth * euclen(ichi->button[BACK_W]->string)
			+ IN_BORDER * 2), FontHeight + IN_BORDER * 2);
	changecolor_box(ichi->button[NEXT_W], xjutil->fg, xjutil->bg);
	changecolor_box(ichi->button[BACK_W], xjutil->fg, xjutil->bg);
	map_box(ichi->button[NEXT_W]);
	map_box(ichi->button[BACK_W]);
    }

    hints.flags = (USSize | USPosition | PMinSize | PMaxSize | PResizeInc
#ifndef	X11R3
		   | PBaseSize | PWinGravity
#endif	/* X11R3 */
		  );
    hints.x = x_cood;
    hints.y = y_cood;
    hints.width = hints.min_width = hints.max_width = width;
    hints.height = hints.min_height = hints.max_height = height;
    hints.width_inc = hints.height_inc = 0;
#ifndef	X11R3
    hints.base_width = width;
    hints.base_height = height;
    hints.win_gravity = CenterGravity;
    XSetWMNormalHints(dpy, ichi->w, &hints);
#else	/* X11R3 */
    XSetNormalHints(dpy, ichi->w, &hints);
#endif	/* X11R3 */
    XRaiseWindow(dpy, ichi->w);
    XMapWindow(dpy, ichi->w);
    if (ichi->kosuu) {
	XMapWindow(dpy, ichi->w1);
    }
    XFlush(dpy);
    return;
}

void
lock_ichiran()
{
    freeze_box(xjutil->ichi->button[NEXT_W]);
    freeze_box(xjutil->ichi->button[BACK_W]);
    xjutil->ichi->lock = 1;
}

void
end_ichiran()
{
    register Ichiran *ichi = xjutil->ichi;

    if (ichi->kosuu) {
	if (ichi->hilited_item != -1)
	    xw_fillrec();
	ichi->hilited_item = -1;
	XUnmapWindow(dpy, ichi->w1);
    }
    XUnmapWindow(dpy, ichi->w);
    XFlush(dpy);
    XUndefineCursor(dpy, ichi->w1);
    unmap_box(ichi->comment);
    unmap_box(ichi->button[NEXT_W]);
    unmap_box(ichi->button[BACK_W]);
    if (ichi->save_p.x > -1)
	XWarpPointer(dpy, None, root_window, 0, 0, 0, 0,
		     ichi->save_p.x, ichi->save_p.y);
    XFlush(dpy);
    ichi->lock = 0;
    unfreeze_box(ichi->button[NEXT_W]);
    unfreeze_box(ichi->button[BACK_W]);
    xjutil->mode &= ~ICHIRAN_MODE;
    xjutil->sel_ret = -1;
}

int
init_input(msg1, msg2,msg3)
unsigned char *msg1, *msg2, *msg3;
{
    register Ichiran	*ichi = xjutil->ichi;
    int	x_cood, y_cood, width, height, bwidth;
    int	save_x, save_y;
    int		maxlen;
    int		def_maxlen;
    int		len_t;
    Window	root, child;
    int	win_x, win_y;
    unsigned int keys_buttons;

    cur_text = ichi->text;

    xjutil->mode |= ICHIRAN_MODE;
    ichi->kosuu = 0;
    ichi->title->string = msg1;
    ichi->subtitle->string = msg2;
    ichi->button[EXEC_W]->string = msg3;
    ichi->rk_mode->string = (unsigned char *)get_rk_modes();

    height = FontHeight * 6;
    def_maxlen = MHL0 + 64;

    maxlen = euclen(msg1) + euclen(msg3) + 18; 
    if (maxlen < def_maxlen) maxlen = def_maxlen;

    bwidth = 2;
    width  = maxlen * FontWidth;

    XQueryPointer(dpy, root_window, &root, &child,
		  &save_x, &save_y, &win_x, &win_y, &keys_buttons);
    ichi->save_p.x = (short)save_x;
    ichi->save_p.y = (short)save_y;
    decide_position(width, height, &x_cood, &y_cood);
    XWarpPointer(dpy, None, root_window, 0, 0, 0, 0,
		 x_cood + FontWidth * 2, y_cood + FontHeight);

    XMoveResizeWindow(dpy, ichi->w, x_cood, y_cood, width, height);
    XSetWindowBackground(dpy, ichi->w, xjutil->bg);
    XClearWindow(dpy, ichi->w);
    XSetWindowBorder(dpy, ichi->w, xjutil->fg);

    moveresize_box(ichi->title, FontWidth, FontHeight/2,
		   (FontWidth * (len_t = euclen(msg1)) + IN_BORDER * 2),
		   (FontHeight + IN_BORDER * 2));
    len_t += 1;
    moveresize_box(ichi->button[CANCEL_W],
		   (FontWidth * (len_t += SPACING)), (FontHeight / 2),
		   (FontWidth * euclen(ichi->button[CANCEL_W]->string)
		    + IN_BORDER * 2), (FontHeight + IN_BORDER * 2));
    len_t += euclen(ichi->button[CANCEL_W]->string);
    moveresize_box(ichi->button[EXEC_W],
		   (FontWidth * (len_t += SPACING)), (FontHeight / 2),
		   (FontWidth * euclen(ichi->button[EXEC_W]->string)
		    + IN_BORDER * 2), (FontHeight + IN_BORDER * 2));
    len_t += euclen(ichi->button[EXEC_W]->string);
    moveresize_box(ichi->subtitle,
		   (FontWidth * 2), (FontHeight * 2 + FontHeight / 2),
		   (FontWidth * euclen(ichi->subtitle->string) * FontWidth
		    + IN_BORDER * 2), (FontHeight + IN_BORDER * 2));
    moveresize_box(ichi->rk_mode, FontWidth,
		   ((FontHeight * 4) + (FontHeight / 2) - IN_BORDER),
		   (FontWidth * MHL0 + IN_BORDER * 2),
		   (FontHeight + IN_BORDER * 2));

    changecolor_box(ichi->title, xjutil->fg, xjutil->bg);
    changecolor_box(ichi->button[CANCEL_W], xjutil->fg, xjutil->bg);
    changecolor_box(ichi->button[EXEC_W], xjutil->fg, xjutil->bg);
    changecolor_box(ichi->subtitle, xjutil->fg, xjutil->bg);
    changecolor_box(ichi->rk_mode, xjutil->fg, xjutil->bg);

    ichi->text->x = FontWidth * (MHL0 + 2);
    ichi->text->y = FontHeight * 4 + FontHeight/2;
    ichi->text->width = FontWidth * TEXT_WIDTH;
    ichi->text->height = FontHeight;
    XMoveResizeWindow(dpy, ichi->text->wp, ichi->text->x, ichi->text->y,
		      ichi->text->width, ichi->text->height);
    XSetWindowBackground(dpy, ichi->text->wp, xjutil->bg);
    XClearWindow(dpy, ichi->text->wp);
    resize_text(ichi->text);

    XSelectInput(dpy, ichi->w, KeyPressMask);
    hints.flags = (USSize | USPosition | PMinSize | PMaxSize | PResizeInc
#ifndef	X11R3
		   | PBaseSize | PWinGravity
#endif	/* X11R3 */
		  );
    hints.x = x_cood;
    hints.y = y_cood;
    hints.width = hints.min_width = hints.max_width = width;
    hints.height = hints.min_height = hints.max_height = height;
    hints.width_inc = hints.height_inc = 0;
#ifndef	X11R3
    hints.base_width = width;
    hints.base_height = height;
    hints.win_gravity = CenterGravity;
    XSetWMNormalHints(dpy, ichi->w, &hints);
#else	/* X11R3 */
    XSetNormalHints(dpy, ichi->w, &hints);
#endif	/* X11R3 */
    XRaiseWindow(dpy, ichi->w);
    XMapWindow(dpy, ichi->w);
    XMapWindow(dpy, ichi->text->wp);
    map_box(ichi->button[EXEC_W]);
    map_box(ichi->subtitle);
    map_box(ichi->rk_mode);
    XFlush(dpy);
    return(0);
}

void
end_input()
{
    Ichiran *ichi = xjutil->ichi;

    unmap_box(ichi->button[EXEC_W]);
    unmap_box(ichi->subtitle);
    unmap_box(ichi->rk_mode);
    XUnmapWindow(dpy, ichi->text->wp);
    XUnmapWindow(dpy, ichi->w);
    if (ichi->save_p.x > -1)
	XWarpPointer(dpy, None, root_window,0,0,0,0,
		     ichi->save_p.x, ichi->save_p.y);
    XFlush(dpy);
    ichi->button[CANCEL_W]->in = ichi->button[EXEC_W]->in = 0;
    xjutil->mode &= ~ICHIRAN_MODE;
    xjutil->sel_ret = -1;
}

void
init_keytable(kosuu, title)
int kosuu;
unsigned char *title[];
{
    int cnt;
    int len, x, y;
    int width, height, x_cood, y_cood;
    Window	root, child;
    int	win_x, win_y;
    unsigned int keys_buttons;
    int	save_x, save_y;
    Keytable *keytable = xjutil->ichi->keytable;

    keytable->map = 0;

    x = FontWidth * 2;
    y = FontHeight/2 - IN_BORDER;
    height = FontHeight + IN_BORDER * 2;
    keytable->cnt = kosuu;
    for (cnt = 0; cnt < kosuu; cnt++) {
	len = euclen(title[cnt]);
	keytable->button[cnt]->string = title[cnt];
	moveresize_box(keytable->button[cnt], x, y,
		       (FontWidth * len + IN_BORDER * 2), height);
	map_box(keytable->button[cnt]);
	x += FontWidth * (len + 2) + IN_BORDER * 2;
    }
    width = x;
    height = FontHeight * 2;

    XQueryPointer(dpy, root_window, &root, &child,
		  &save_x, &save_y, &win_x, &win_y, &keys_buttons);
    keytable->save_p.x = (short)save_x;
    keytable->save_p.y = (short)save_y;
    decide_position(width, height, &x_cood, &y_cood);
    XWarpPointer(dpy, None, root_window, 0, 0, 0, 0,
		 x_cood + width/2, y_cood + FontHeight);

    XMoveResizeWindow(dpy, keytable->w, x_cood, y_cood, width, height);
    XRaiseWindow(dpy, keytable->w);
    XMapWindow(dpy, keytable->w);
    XFlush(dpy);
}

void
end_keytable()
{
    int cnt;
    Keytable *keytable = xjutil->ichi->keytable;

    for (cnt = 0; cnt < keytable->cnt; cnt++) {
	unmap_box(keytable->button[cnt]);
    }
    XUnmapWindow(dpy, keytable->w);
    if (keytable->save_p.x > -1)
	XWarpPointer(dpy, None, root_window, 0, 0, 0, 0,
		     keytable->save_p.x, keytable->save_p.y);
    XFlush(dpy);
    keytable->cnt = 0;
    xjutil->sel_ret = -1;
}

void
init_yes_or_no(str, flg)
unsigned char *str;
int flg;
{
    int len, def_width, width, height, x_cood, y_cood;
    Window root, child;
    int	win_x, win_y;
    unsigned int keys_buttons;
    YesOrNo *yes_no = xjutil->yes_no;

    yes_no->map = 0;
    yes_no->title->string = str;

    def_width = FontWidth * 18;
    height = FontHeight * 4;

    if (flg != MessageOnly) {
	if (flg == YesMessage) {
	    yes_no->button[0]->string
		= (unsigned char *)msg_get(cd, 7, default_message[7],
					   xjutil->lang);
	} else {
	    yes_no->button[0]->string
		= (unsigned char *)msg_get(cd, 5, default_message[5],
					   xjutil->lang);
	    yes_no->button[1]->string
		= (unsigned char *)msg_get(cd, 6, default_message[6],
					   xjutil->lang);
	}
	moveresize_box(yes_no->button[0], FontWidth,
		       (FontHeight * 2 + (FontHeight / 2) - IN_BORDER),
		       ((len = euclen(yes_no->button[0]->string)) * FontWidth
			+ IN_BORDER * 2), (FontHeight + IN_BORDER * 2));
	changecolor_box(yes_no->button[0], xjutil->fg, xjutil->bg);
	map_box(yes_no->button[0]);
	if (flg == YesNoMessage) {
	    moveresize_box(yes_no->button[1],
			   (FontWidth * (len + SPACING)),
			   (FontHeight * 2 + (FontHeight / 2) - IN_BORDER),
			   (FontWidth * euclen(yes_no->button[1]->string)
			    + IN_BORDER * 2),
			   (FontHeight + IN_BORDER * 2));
	    changecolor_box(yes_no->button[1], xjutil->fg, xjutil->bg);
	    map_box(yes_no->button[1]);
	    len += euclen(yes_no->button[1]->string);
	    def_width = FontWidth * (len + SPACING * 3);
	} else {
	    def_width = FontWidth * (len + SPACING * 2);
	}
    } else {
	def_width = 0;
	height = (FontHeight * 2);
    }
    if((width = FontWidth * ((len = euclen(str)) + SPACING * 2)) < def_width)
	width = def_width;
    moveresize_box(yes_no->title, FontWidth,
		   ((FontHeight / 2) - IN_BORDER),
		   (FontWidth * len + IN_BORDER * 2),
		   (FontHeight + IN_BORDER * 2));
    changecolor_box(yes_no->title, xjutil->fg, xjutil->bg);

    if (flg != MessageOnly) {
	XQueryPointer(dpy, root_window, &root, &child, &yes_no->x,
		      &yes_no->y, &win_x, &win_y, &keys_buttons);
    }
    x_cood = (DisplayWidth(dpy, screen) - width)/2;
    y_cood = (DisplayHeight(dpy, screen) - height)/2;
    if ((x_cood + width ) > DisplayWidth(dpy, screen))
	x_cood = DisplayWidth(dpy, screen) - width;
    if (x_cood < 0)
	x_cood = 0;
    if ((y_cood + height ) > DisplayHeight(dpy, screen))
	y_cood = DisplayHeight(dpy, screen) - height;
    if (y_cood < 0)
	y_cood = 0;
    if (flg != MessageOnly) {
	XWarpPointer(dpy, None, root_window, 0, 0, 0, 0,
		     (x_cood + FontWidth * 2), (y_cood + FontHeight * 3));
    }
    XMoveResizeWindow(dpy, yes_no->w, x_cood, y_cood, width, height);
    XSetWindowBackground(dpy, yes_no->w, xjutil->bg);
    XClearWindow(dpy, yes_no->w);
    XSetWindowBorder(dpy, yes_no->w, xjutil->fg);
    hints.flags = (USSize | USPosition | PMinSize | PMaxSize | PResizeInc
#ifndef	X11R3
		   | PBaseSize | PWinGravity
#endif	/* X11R3 */
		  );
    hints.x = x_cood;
    hints.y = y_cood;
    hints.width = hints.min_width = hints.max_width = width;
    hints.height = hints.min_height = hints.max_height = height;
    hints.width_inc = hints.height_inc = 0;
#ifndef	X11R3
    hints.base_width = width;
    hints.base_height = height;
    hints.win_gravity = CenterGravity;
    XSetWMNormalHints(dpy, yes_no->w, &hints);
#else	/* X11R3 */
    XSetNormalHints(dpy, yes_no->w, &hints);
#endif	/* X11R3 */
    XRaiseWindow(dpy, yes_no->w);
    XMapWindow(dpy, yes_no->w);
    XFlush(dpy);
}

void
end_yes_or_no()
{
    register YesOrNo *yes_no = xjutil->yes_no;

    unmap_box(yes_no->button[0]);
    unmap_box(yes_no->button[1]);
    XUnmapWindow(dpy, yes_no->w);
    if (yes_no->mode != MessageOnly) {
	XWarpPointer(dpy, None, root_window, 0, 0, 0, 0, yes_no->x, yes_no->y);
    }
    XFlush(dpy);
    xjutil->sel_ret = -1;
}

void
JW3Mputc(s, w, li, flg, in)
unsigned char *s;
register Window w;
register int li, flg, in;
{
    int wc_len;
    XCharStruct cs;

    if(flg & REV_FLAG) {
	currentGC = xjutil->reversegc;
    } else {
	currentGC = xjutil->gc;
    }
    wc_len = 512;
    wc_len = char_to_wchar(xjutil->xlc, s, wc_buf, strlen((char*)s), wc_len);
    cs.width = FontWidth;    
    cs.ascent = FontAscent;
    cs.descent = FontDescent;
    JWOutput(w, cur_fs->fs, currentGC, 0, 0, flg,
	     in, (li * (FontHeight + in) + in), &cs, wc_buf, wc_len);
}

static void
redraw_ichi_w()
{
    Ichiran	 *ichi;
    int re_start_line, re_end_line;
    int write_line;
    int line = 0;
    unsigned int flg = 0;

    ichi = xjutil->ichi;

    re_start_line = 0;
    re_end_line = ichi->max_line - 1;

    for (line = re_start_line, write_line = (re_start_line + ichi->start_line);
	 (line <= re_end_line && write_line < ichi->kosuu_all);
	 line++, write_line++) {
	/*
	if (line == ichi->hilited_item)
		rev = 1;
	else
	*/
		flg = 0;
	JW3Mputc(ichi->buf[write_line], ichi->w1, line, flg, IN_BORDER);
    }
    if (ichi->hilited_item != -1) {
	xw_fillrec();
    }
    XFlush(dpy);
}

int
next_ichi()
{
    register Ichiran *ichi = xjutil->ichi;

    if (ichi->page < 2) return(0);
    if (++ichi->cur_page > ichi->page) ichi->cur_page = 1;
    ichi->start_line = (ichi->cur_page - 1) * ichi->max_line;
    ichi->end_line = ichi->start_line + ichi->max_line - 1;
    XClearWindow(dpy, ichi->w1);
    redraw_ichi_w();
    return(0);
}

int
back_ichi()
{
    register Ichiran *ichi = xjutil->ichi;

    if (ichi->page < 2) return(0);
    if (--ichi->cur_page < 1) ichi->cur_page = ichi->page;
    ichi->start_line = (ichi->cur_page - 1) * ichi->max_line;
    ichi->end_line = ichi->start_line + ichi->max_line - 1;
    XClearWindow(dpy, ichi->w1);
    redraw_ichi_w();
    return(0);
}

int
find_hilite()
{
    register Ichiran *ichi = xjutil->ichi;

    return(((ichi->cur_page - 1) * ichi->max_line + ichi->hilited_item));
}

int
xw_next_select()
{
    register Ichiran *ichi = xjutil->ichi;

    if (!ichi->lock) {
	if (ichi->hilited_item != -1) xw_fillrec();
	if(ichi->hilited_item < ichi->kosuu - 1){
	    ichi->hilited_item += 1;
	} else {
	    ichi->hilited_item = 0;
	}
	xw_fillrec();
    }
    return(0);
}

int
xw_previous_select()
{
    register Ichiran *ichi = xjutil->ichi;

    if (!ichi->lock) {
	if (ichi->hilited_item != -1) xw_fillrec();
	if(ichi->hilited_item > 0) {
	    ichi->hilited_item -= 1;
	} else {
	    ichi->hilited_item = ichi->kosuu - 1;
	}
	xw_fillrec();
    }
    return(0);
}

void
xw_expose(event)
register XExposeEvent *event;
{
    register int i;
    Ichiran *ichi = xjutil->ichi;
    JutilRec *jutil = xjutil->jutil;
    Window window = event->window;
    BoxRec *p;

    for (p = box_list; p != NULL; p = p->next) {
	if (window == p->window) {
	    redraw_box(p);
	    return;
	}
    }
    if (xjutil->mode & ICHIRAN_MODE) {
	if (window == ichi->w1) {
	    redraw_ichi_w();
	    return;
	} else if (ichi->text->w == window) {
	    redraw_text(ichi->text, event->x, event->width);
	    return;
	} else if (ichi->text->wn[0] == window) {
	    redraw_note(ichi->text, 0);
	    return;
	} else if (ichi->text->wn[1] == window) {
	    redraw_note(ichi->text, 1);
	    return;
	}
    }
    if (xjutil->mode & JUTIL_MODE) {
	for (i = 0; i < jutil->max_mes; i++) {
	    if (jutil->mes_text[i]->w == window) {
		redraw_text(jutil->mes_text[i], event->x, event->width);
		return;
	    } else if (jutil->mes_text[i]->wn[0] == window) {
		redraw_note(jutil->mes_text[i], 0);
		return;
	    } else if (jutil->mes_text[i]->wn[1] == window) {
		redraw_note(jutil->mes_text[i], 1);
		return;
	    }
	}
    }
    return;
}

static void
xw_select_button(event)
XButtonEvent *event;
{
    static int y;
    int item;

    y = event->y;
    if ((item = Y_InItem(y)) != xjutil->ichi->hilited_item) {
	if (xjutil->ichi->hilited_item != -1)
	    xw_fillrec();
    }
    XFlush(dpy);
    xjutil->sel_ret = item;
}

Status
xw_buttonpress(event)
XButtonEvent *event;
{
    Ichiran *ichi = xjutil->ichi;
    Window window = event->window;
    BoxRec *p;

    xjutil->sel_ret = -1;
    for (p = box_list; p != NULL; p = p->next) {
	if (window == p->window) {
	    if (p->freeze) return(False);
	    xjutil->sel_ret = p->sel_ret;
	    if (p->cb) (*p->cb)(p->cb_data);
	    return(p->do_ret);
	}
    }

    if (xjutil->mode & ICHIRAN_MODE) {
	if (window == ichi->w1 && !ichi->lock) {
	    xjutil->sel_ret = 1;
	    xw_select_button(event);
	    return(True);
	}
    }
    return(False);
}

int
jutil_mode_set(mes)
int mes;
{
    xjutil->jutil->mode = mes;
    return(0);
}

void
xw_fillrec()
{
    register Ichiran *ichi = xjutil->ichi;

    if (ichi->hilited_item != -1) {
	XFillRectangle(dpy, ichi->w1, ichi->invertgc,
		0, ichi->hilited_item * (FontHeight + IN_BORDER),
		ichi->item_width, (FontHeight + IN_BORDER));
	XFlush(dpy);
    }
}

void
xw_enterleave(event, el)
XCrossingEvent *event;
int el;
{
    register Ichiran *ichi = xjutil->ichi;
    register Window window = event->window;
    BoxRec *p;

    for (p = box_list; p != NULL; p = p->next) {
	if (window == p->window) {
	    if (p->freeze) return;
	    if (p->reverse && el != p->in) {
		reverse_box(p, xjutil->invertgc);
		p->in = el;
		return;
	    }
	}
    }

    if(xjutil->mode & ICHIRAN_MODE) {
	if (window == ichi->w1 && !ichi->lock) {
	    if (el) {
		xw_mousemove(event);
	    } else {
		xw_mouseleave();
	    }
	}
    }
    return;
}
