/*
 * $Id: ichiran.c,v 1.4 1991/09/16 21:36:52 ohm Exp $
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
#include "commonhd.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"

extern GC currentGC;
extern XSizeHints hints;

extern wchar wc_buf[];
extern wchar_t cursor_wchar_t;

static struct jikouho_ichi {
	int	x_s;
	int	y_s;
	int	x_e;
	int	y_e;
	int	line;
} jikouho_ichi[MAXJIKOUHO + 1];

static int jikouho_line[MAXJIKOUHO + 1];

static int
euclen(s)
unsigned char *s;
{
    register int n;
    register unsigned char *c;

    for (c = s, n = 0; *c != 0; c++,  n++) {
	if ((*c & 0xff) == 0x8e) c++;
    }
    return n;
}

static int
Y_InItem(y)
int y;
{
    register XIMLangRec *xl;

    if (xim->j_c == NULL) return(-1);
    xl = xim->j_c->cur_xl;

    if (y < 0 || y > xim->ichi->item_height)
	return(-1);
    return((y/(FontHeight(xl) + IN_BORDER)));
}

static void
xw_fillrec()
{
    XIMLangRec *xl;
    register Ichiran *ichi = xim->ichi;

    if (xim->j_c == NULL) return;
    xl = xim->j_c->cur_xl;

    if (ichi->hilited_item != -1) {
	XFillRectangle(dpy, ichi->w1, xim->ichi->invertgc,
		0, ichi->hilited_item * (FontHeight(xl) + IN_BORDER),
		ichi->item_width, (FontHeight(xl) + IN_BORDER));
	XFlush(dpy);
    }
    return;
}

static void
xw_ji_fillrec()
{
    register Ichiran *ichi = xim->ichi;

    if (ichi->hilited_ji_item != -1) {
	XFillRectangle(dpy, ichi->w1, ichi->invertgc,
	jikouho_ichi[ichi->hilited_ji_item].x_s,
	jikouho_ichi[ichi->hilited_ji_item].y_s,
	(jikouho_ichi[ichi->hilited_ji_item].x_e
	 - jikouho_ichi[ichi->hilited_ji_item].x_s),
	(jikouho_ichi[ichi->hilited_ji_item].y_e
	 - jikouho_ichi[ichi->hilited_ji_item].y_s));
	XFlush(dpy);
    }
    return;
}

static void
decide_position(xl, width, height, ret_x, ret_y)
register XIMLangRec *xl;
register int width, height;
register int *ret_x, *ret_y;
{
    Window root, parent, child, *children;
    XWindowAttributes to_attr;
    int root_x, root_y;
    unsigned int nc;

    if (xim->sel_button) {
	*ret_x = button.x - FontWidth(xl);;
	*ret_y = button.y - FontHeight(xl);
	xim->sel_button = 0;
    }else{
	if (IsPreeditNothing(cur_x)) {
	    XGetWindowAttributes(dpy, xim->ximclient->w, &to_attr);
	    XQueryTree(dpy, xim->ximclient->w, &root, &parent, &children, &nc);
	    XTranslateCoordinates(dpy, parent, root_window, to_attr.x,
				  to_attr.y, &root_x, &root_y, &child);
	    *ret_x = root_x;
	    *ret_y = root_y - height;
	} else {
	    XGetWindowAttributes(dpy, cur_x->w, &to_attr);
	    XQueryTree(dpy, cur_x->w, &root, &parent, &children, &nc);
	    XTranslateCoordinates(dpy, parent, root_window, to_attr.x,
	   			  to_attr.y, &root_x, &root_y, &child);
	    *ret_x = root_x;
	    *ret_y = root_y + to_attr.height;
	}
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

static void
realloc_for_save_buf(size)
int size;
{
    register Ichiran *ichi = xim->ichi;
    register int i;
    ichi->save_buf = (unsigned char **)Realloc((char *)ichi->save_buf,
					     (unsigned)(size * sizeof(char *)));
    for (i = ichi->max_lines; i < size; i++) {
	ichi->save_buf[i]
	= (unsigned char *)Malloc((unsigned)(ichi->max_columns * sizeof(char)));
    }
    ichi->max_lines = size;
}

static int
xw_set_jikouho(info,kosuu)
unsigned char **info;
int kosuu;
{
    register int cnt, cntt;
    int	width;
    register unsigned char *p;
    unsigned char tmp[256];
    register int columns = 0;
    register XIMLangRec *xl;
    register Ichiran *ichi = xim->ichi;

    if (xim->j_c == NULL) return(-1);
    xl = xim->j_c->cur_xl;

    width = (DisplayWidth(dpy, screen)/FontWidth(xl))/2;
    jikouho_ichi[0].x_s = 0;
    jikouho_line[1] = 0;

    p = ichi->buf[0] = ichi->save_buf[0];
    *p = 0;
    for (cnt = 0, cntt = 0; cnt < kosuu; cnt++) {
	if (ichi->mode == JIKOUHO)
	    sprintf((char *)tmp, "  %s  ", info[cnt]);
	else if (ichi->mode == JIS_IN)
	    sprintf((char *)tmp, "%2x. %s ",
		cnt + ((kosuu < 16)? 1 : 33), info[cnt]);
	else if (ichi->mode == KUTEN_IN)
	    sprintf((char *)tmp, "%2d. %s ",cnt + 1, info[cnt]);
	if ((columns += euclen((char *)tmp) ) > width) {
	    jikouho_line[++cntt + 1] = cnt;
	    jikouho_ichi[cnt--].x_s = 0;
	    columns = 0;
	    if (cntt >= xim->ichi->max_lines)
		realloc_for_save_buf(xim->ichi->max_lines + MAX_PAGE_LINES);
	    p = xim->ichi->buf[cntt] = ichi->save_buf[cntt];
	    *p = 0;
	    continue;
	}
	jikouho_ichi[cnt + 1].x_s = columns * FontWidth(xl);
	jikouho_ichi[cnt].x_e = columns * FontWidth(xl) - 1;
	jikouho_ichi[cnt].y_s = cntt * (FontHeight(xl) + IN_BORDER);
	jikouho_ichi[cnt].y_e = (cntt + 1) * (FontHeight(xl)  + IN_BORDER) - 1;
	jikouho_ichi[cnt].line = cntt + 1;
	p = (unsigned char *)strcat((char *)p, (char *)tmp);
    }
    jikouho_line[++cntt + 1] = kosuu;
    return(cntt);
}

static void
check_move_ichi(p)
register int p;
{
    register int tmp;
    if ((tmp = p/MAX_PAGE_LINES + (p % MAX_PAGE_LINES ? 1 : 0))
	!= xim->ichi->cur_page) {
	xim->ichi->cur_page = tmp;
	XMoveWindow(dpy, xim->ichi->w1, 0,
		    -((xim->ichi->cur_page - 1) * xim->ichi->page_height));
    }
}

void
JW3Mputc(s, w, li, flg, in)
unsigned char *s;
register Window w;
register int li, flg, in;
{
    register XIMLangRec *xl = cur_p->cur_xl;
    int wc_len;
    XCharStruct cs;

    if(flg & REV_FLAG) {
	currentGC = cur_p->pe.reversegc;
    } else {
	currentGC = cur_p->pe.gc;
    }
    cs.width = FontWidth(xl);
    cs.ascent = FontAscent(xl);
    cs.descent = FontDescent(xl);

    wc_len = 512;
    wc_len = char_to_wchar(xl->xlc, s, wc_buf, strlen((char *)s), wc_len);
    JWOutput(w, xl->pe_fs, currentGC, 0, 0, flg,
	     in, (li * (FontHeight(xl) + in) + in), &cs, wc_buf, wc_len);
}

int
init_ichiran(info, kosuu, init, def, title, subtitle, exec_title,
	     nyuu_col, mode)
unsigned char **info;
int kosuu, init, def;
unsigned char *title;
unsigned char *subtitle;
unsigned char *exec_title;
int nyuu_col, mode;
{
    register XIMLangRec *xl = cur_p->cur_xl;
    register Ichiran *ichi = xim->ichi;
    int	x_cood, y_cood, width, height;
    int	save_x, save_y;
    int maxlen;
    int def_maxlen;
    int def_height;
    register int cnt;
    register int tmp = 0;
    register int len_t;
    Window root, child;
    int	win_x, win_y;
    unsigned int keys_buttons;
    char *lang;

    if (xim->j_c) return(-1);
    xim->j_c = cur_p;
    ichi->map = 0;
    ichi->exp = 0;

    lang = cur_p->cur_xl->lang;

    ichi->kosuu = kosuu;
    ichi->title->string = title;
    ichi->subtitle->string = subtitle;
    ichi->button[EXEC_W]->string = exec_title;
    ichi->button[CANCEL_W]->string =
	(unsigned char *)msg_get(cd, 16, default_message[16], lang);
    ichi->button[MOUSE_W]->string =
	(unsigned char *)msg_get(cd, 39, default_message[39], lang);

    ichi->mode = mode;
    ichi->invertgc = cur_p->pe.invertgc;
    if (mode == JIKOUHO || mode == JIS_IN || mode == KUTEN_IN) {
	def_maxlen = euclen(title) + euclen(ichi->button[CANCEL_W]->string)
		   + euclen(ichi->button[MOUSE_W]->string) + SPACING * 3 + 1;
    } else if (mode == SENTAKU) {
	def_maxlen = euclen(title) + euclen(ichi->button[CANCEL_W]->string)
		   + SPACING * 2 + 1;
    } else {
	def_maxlen = euclen(title) + euclen(exec_title)
		   + euclen(ichi->button[CANCEL_W]->string) + SPACING * 3 + 1;
    }
    def_height = FontHeight(xl) * 4;

    if (mode != NYUURYOKU) {
	if (mode == SENTAKU) {
	    if (kosuu > ichi->max_lines)
		realloc_for_save_buf(kosuu);
	    for (cnt = 0; cnt < kosuu; cnt++) {
		sprintf((char *)ichi->save_buf[cnt], " %s ", info[cnt]);
		ichi->buf[cnt] = ichi->save_buf[cnt];
	    }
	    ichi->max_line = kosuu;
	    height = kosuu * (FontHeight(xl) + IN_BORDER) 
			+ 2 * FontHeight(xl);
	} else {
	    kosuu = xw_set_jikouho(info,kosuu);
	    if (kosuu > MAX_PAGE_LINES) {
		ichi->page = kosuu / MAX_PAGE_LINES
			   + (kosuu % MAX_PAGE_LINES ? 1 : 0);
		ichi->max_line = MAX_PAGE_LINES;
		height = MAX_PAGE_LINES * (FontHeight(xl) + IN_BORDER) 
			+ 2 * FontHeight(xl);
		ichi->page_height = MAX_PAGE_LINES * (FontHeight(xl) + IN_BORDER);
	    } else {
		ichi->page = 1;
		ichi->max_line = kosuu;
		height = kosuu * (FontHeight(xl) + IN_BORDER) 
		       + 2 * FontHeight(xl);
		ichi->page_height = 0;
	    }
	}
	maxlen = euclen(ichi->buf[0]) + 2;
	for (cnt = 1; cnt < kosuu; cnt++) {
	    if ((tmp = euclen(ichi->buf[cnt])) > maxlen) maxlen = tmp + 2;
	}
	ichi->kosuu_all = kosuu;
	ichi->cur_page = 1;
	if (maxlen < def_maxlen) maxlen = def_maxlen;
    } else if (mode == NYUURYOKU) {
	if (def_maxlen < (euclen(subtitle) + nyuu_col + 4))
	    maxlen = euclen(subtitle) + nyuu_col + 4;
	else
	    maxlen = def_maxlen;
	height = def_height;
    } else {
	maxlen = def_maxlen;
	height = def_height;
    }
    width  = ichi->item_width = maxlen * FontWidth(xl);
    ichi->item_height = height;
    ichi->max_len = maxlen;

    XQueryPointer(dpy, root_window, &root, &child,
		  &save_x, &save_y, &win_x, &win_y, &keys_buttons);
    ichi->save_x = (short)save_x;
    ichi->save_y = (short)save_y;
    decide_position(xl, width, height, &x_cood, &y_cood);
    XWarpPointer(dpy, None, root_window, 0, 0, 0, 0,
		 x_cood + FontWidth(xl),
		 y_cood + FontHeight(xl) * 2 + FontHeight(xl)/2);

    XMoveResizeWindow(dpy, ichi->w,x_cood - 2, y_cood - 2, width, height);

    moveresize_box(ichi->title, FontWidth(xl), (FontHeight(xl)/2 - IN_BORDER),
		   (FontWidth(xl) * (len_t = euclen(title)) + IN_BORDER * 2),
		   (FontHeight(xl) + IN_BORDER * 2));
    len_t += 1;
    moveresize_box(ichi->button[CANCEL_W],
		   (FontWidth(xl) * (len_t += SPACING)),
		   (FontHeight(xl)/2 - IN_BORDER),
		   (euclen(ichi->button[CANCEL_W]->string) * FontWidth(xl)
		    + IN_BORDER * 2), (FontHeight(xl) + IN_BORDER * 2));
    len_t += euclen(ichi->button[CANCEL_W]->string);
    
    XSetWindowBackground(dpy, ichi->w, cur_p->pe.bg);
    XClearWindow(dpy, ichi->w);
    XSetWindowBorder(dpy, ichi->w, cur_p->pe.fg);

    changecolor_box(ichi->title, cur_p->pe.fg, cur_p->pe.bg);
    changecolor_box(ichi->button[CANCEL_W], cur_p->pe.fg, cur_p->pe.bg);
    if (mode != NYUURYOKU) {
	XMoveResizeWindow(dpy, ichi->w0, -1, FontHeight(xl) * 2, width,
			  ichi->max_line * (FontHeight(xl) + IN_BORDER));
	XMoveResizeWindow(dpy, ichi->w1, 0, 0, width,
			  kosuu * (FontHeight(xl) + IN_BORDER));
	XSetWindowBackground(dpy, ichi->w0, cur_p->pe.bg);
	XClearWindow(dpy, ichi->w0);
	XSetWindowBorder(dpy, ichi->w0, cur_p->pe.fg);
	XSetWindowBackground(dpy, ichi->w1, cur_p->pe.bg);
	XClearWindow(dpy, ichi->w1);
	XSetWindowBorder(dpy, ichi->w1, cur_p->pe.fg);
    }
    if (mode == JIKOUHO || mode == JIS_IN || mode == KUTEN_IN) {
	moveresize_box(ichi->button[MOUSE_W],
		       (FontWidth(xl) * (len_t += SPACING)),
		       (FontHeight(xl)/2 - IN_BORDER),
		       (euclen(ichi->button[MOUSE_W]->string) * FontWidth(xl)
			+ IN_BORDER * 2), 
		       (FontHeight(xl) + IN_BORDER * 2));
	len_t += euclen(ichi->button[MOUSE_W]->string);
	changecolor_box(ichi->button[MOUSE_W], cur_p->pe.fg, cur_p->pe.bg);
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
    /*
    hints.win_gravity = CenterGravity;
    */
    hints.win_gravity = SouthWestGravity;
    XSetWMNormalHints(dpy, ichi->w, &hints);
#else	/* X11R3 */
    XSetNormalHints(dpy, ichi->w, &hints);
#endif	/* X11R3 */
    XRaiseWindow(dpy, ichi->w);
    XMapWindow(dpy, ichi->w);
    map_box(ichi->button[CANCEL_W]);
    XFlush(dpy);
    /*
    redraw_box(ichi->title_w, ichi->title, 0);
    redraw_box(ichi->button[CANCEL_W].window, ichi->button[CANCEL_W].title, 0);
    */
    if (mode == NYUURYOKU) {
	moveresize_box(ichi->subtitle, FontWidth(xl),
		       (FontHeight(xl) * 2 + FontHeight(xl)/2 - IN_BORDER),
		       (euclen(subtitle) * FontWidth(xl) + IN_BORDER * 2),
		       (FontHeight(xl) + IN_BORDER * 2));
	changecolor_box(ichi->subtitle, cur_p->pe.fg, cur_p->pe.bg);
	map_box(ichi->subtitle);
	XMoveResizeWindow(dpy, ichi->nyuu_w,
			  FontWidth(xl) * (euclen(subtitle) + 3),
			  FontHeight(xl) * 2 + FontHeight(xl)/2,
			  nyuu_col * FontWidth(xl), FontHeight(xl));
	XSetWindowBackground(dpy, ichi->nyuu_w, cur_p->pe.bg);
	XClearWindow(dpy, ichi->nyuu_w);
	XSetWindowBorder(dpy, ichi->nyuu_w, cur_p->pe.fg);
	XMapWindow(dpy, ichi->nyuu_w);
	/*
	redraw_box(ichi->subtitle_w, ichi->subtitle, 0);
	*/
	moveresize_box(ichi->button[EXEC_W],
		       (FontWidth(xl) * (len_t += SPACING)),
		       (FontHeight(xl)/2 - IN_BORDER),
		       (euclen(exec_title) * FontWidth(xl) + IN_BORDER * 2),
		       (FontHeight(xl) + IN_BORDER * 2));
	changecolor_box(ichi->button[EXEC_W], cur_p->pe.fg, cur_p->pe.bg);
	map_box(ichi->button[EXEC_W]);
	/*
	redraw_box(ichi->button[EXEC_W].window, exec_title, 0);
	*/
	len_t += tmp;
    }
    if (ichi->kosuu) {
	XMapWindow(dpy, ichi->w0);
	XFlush(dpy);
	ichi->init = init;
	if (mode == JIKOUHO || mode == JIS_IN || mode == KUTEN_IN) {
	    if (ichi->select_mode) {
		XSelectInput(dpy, ichi->w1,
			     EnterWindowMask | LeaveWindowMask |
			     PointerMotionMask | ButtonPressMask |
			     ButtonReleaseMask | ExposureMask);
		XDefineCursor(dpy, ichi->w1, XCreateFontCursor(dpy,52));
	    } else {
		ichi->button[MOUSE_W]->string = (unsigned char *)msg_get(cd, 40,
						default_message[40], lang);
		XSelectInput(dpy, ichi->w1, ExposureMask);
	    }
	    map_box(ichi->button[MOUSE_W]);
	    /*
	    redraw_box(ichi->button[MOUSE_W].window,
		       ichi->button[MOUSE_W].title, 0);
	    */
	    ichi->hilited_ji_item = def;
	    check_move_ichi(jikouho_ichi[ichi->hilited_ji_item].line);
	    xw_ji_fillrec();
	    if(ichi->page > 1) {
		ichi->button[NEXT_W]->string = (unsigned char *)msg_get(cd, 41,
						default_message[41], lang);
		ichi->button[BACK_W]->string = (unsigned char *)msg_get(cd, 42,
						default_message[42], lang);
		moveresize_box(ichi->button[NEXT_W],
			       (FontWidth(xl) * (len_t += SPACING)),
			       (FontHeight(xl)/2 - IN_BORDER),
			       (euclen(ichi->button[NEXT_W]->string)
			        * FontWidth(xl) + IN_BORDER * 2),
			       (FontHeight(xl) + IN_BORDER * 2));
		len_t += euclen(ichi->button[NEXT_W]->string);
		moveresize_box(ichi->button[BACK_W],
			       (FontWidth(xl) * (len_t += SPACING)),
			       (FontHeight(xl)/2 - IN_BORDER),
			       (euclen(ichi->button[BACK_W]->string)
				* FontWidth(xl) + IN_BORDER * 2),
			       (FontHeight(xl) + IN_BORDER * 2));
		len_t += euclen(ichi->button[BACK_W]->string);
		changecolor_box(ichi->button[NEXT_W], cur_p->pe.fg,
				cur_p->pe.bg);
		changecolor_box(ichi->button[BACK_W], cur_p->pe.fg,
				cur_p->pe.bg);
		map_box(ichi->button[NEXT_W]);
		map_box(ichi->button[BACK_W]);
		/*
		redraw_box(ichi->button[NEXT_W].window,
			   ichi->button[NEXT_W].title, 0);
		redraw_box(ichi->button[BACK_W].window,
			   ichi->button[BACK_W].title, 0);
		*/
	    }
	} else if (mode == SENTAKU) {
	    ichi->hilited_item = def;
	    xw_fillrec();
	    XSelectInput(dpy, ichi->w1,
			 EnterWindowMask | LeaveWindowMask |
			 PointerMotionMask | ButtonPressMask |
			 ButtonReleaseMask | ExposureMask);
	    XDefineCursor(dpy, ichi->w1, XCreateFontCursor(dpy,132));
	}
    }
    XFlush(dpy);
    return(0);
}

void
end_ichiran()
{
    Ichiran *ichi = xim->ichi;
    register int i;

    if (ichi->kosuu) {
	ichi->init = -1;
	if (ichi->mode == SENTAKU && ichi->hilited_item != -1)
	    xw_fillrec();
	ichi->hilited_item = -1;
	if ((ichi->mode == JIKOUHO || ichi->mode == JIS_IN ||
	    ichi->mode == KUTEN_IN) && ichi->hilited_ji_item != -1)
	    xw_ji_fillrec();
	ichi->hilited_ji_item = -1;
	XUnmapWindow(dpy, ichi->w0);
    }
    XUnmapWindow(dpy, ichi->w);
    XFlush(dpy);
    XUndefineCursor(dpy, ichi->w1);
    XSelectInput(dpy, ichi->w1, ExposureMask);
    unmap_box(ichi->subtitle);
    XUnmapWindow(dpy, ichi->nyuu_w);
    for (i = 0; i < ichi->max_button; i++) {
	unmap_box(ichi->button[i]);
	ichi->button[i]->in = '\0';
    }
    XWarpPointer(dpy, None, root_window, 0, 0, 0, 0,
		 (int)ichi->save_x, (int)ichi->save_y);
    XFlush(dpy);
    xim->sel_button = 0;
    xim->j_c = NULL;
}

void
check_map(w)
Window w;
{
    register XIMClientRec *xc;

    if (w == xim->ichi->w) {
	xim->ichi->map = 1;
	return;
    } else if (w == xim->inspect->w) {
	xim->inspect->map  = 1;
	return;
    } else {
	if (cur_p && cur_p->yes_no && cur_p->yes_no->w == w) {
	    cur_p->yes_no->map = 1;
	    return;
	}
	for (xc = ximclient_list; xc != NULL; xc = xc->next) {
	    if (w == xc->yes_no->w) {
		xc->yes_no->map = 1;
		return;
	    }
	}
    }
}

void
lock_inspect()
{
    xim->inspect->lock = 1;
    freeze_box(xim->inspect->button[DELETE_W]);
    freeze_box(xim->inspect->button[USE_W]);
    freeze_box(xim->inspect->button[NEXT_W]);
    freeze_box(xim->inspect->button[BACK_W]);
}

void
unlock_inspect()
{
    xim->inspect->lock = 0;
    unfreeze_box(xim->inspect->button[DELETE_W]);
    unfreeze_box(xim->inspect->button[USE_W]);
    unfreeze_box(xim->inspect->button[NEXT_W]);
    unfreeze_box(xim->inspect->button[BACK_W]);
}

int
init_inspect(msg)
unsigned char *msg;
{
    int width, height, x_cood, y_cood;
    int save_x, save_y;
    Window root, child;
    int	win_x, win_y;
    unsigned int keys_buttons;
    Inspect *ins = xim->inspect;
    register XIMLangRec *xl = cur_p->cur_xl;
    char *lang;
    int i, len_t = 0;

    if (xim->j_c != NULL) return(-1);
    xim->j_c = cur_p;
    ins->map  = 0;
    ins->exp  = 0;

    lang = xl->lang;
    ins->max_len = euclen(msg);
    width = FontWidth(xl) * ins->max_len;
    height = FontHeight(xl) * 3 + IN_BORDER * 2;

    XQueryPointer(dpy, root_window, &root, &child,
		  &save_x, &save_y, &win_x, &win_y, &keys_buttons);
    ins->save_x = (short)save_x;
    ins->save_y = (short)save_y;
    decide_position(xl, width, height, &x_cood, &y_cood);
    XWarpPointer(dpy, None, root_window, 0, 0, 0, 0,
		 x_cood + width/2, y_cood + FontHeight(xl));
    ins->msg = msg;
    ins->title->string = (unsigned char *)msg_get(cd, 34, default_message[34],
						  lang);
    ins->button[CANCEL_W]->string = (unsigned char *)msg_get(cd, 16,
						default_message[16], lang);
    ins->button[DELETE_W]->string = (unsigned char *)msg_get(cd, 35,
						default_message[35], lang);
    ins->button[USE_W]->string = (unsigned char *)msg_get(cd, 36,
						default_message[36], lang);
    ins->button[NEXT_W]->string = (unsigned char *)msg_get(cd, 37,
						default_message[37], lang);
    ins->button[BACK_W]->string = (unsigned char *)msg_get(cd, 38,
						default_message[38], lang);
    moveresize_box(ins->title, FontWidth(xl), (FontHeight(xl)/2 - IN_BORDER),
		   (FontWidth(xl) * (len_t = euclen(ins->title->string))
		    +IN_BORDER * 2),
		   (FontHeight(xl) + IN_BORDER * 2));
    len_t += 1;
    for (i = 0; i < ins->max_button; i++) {
	moveresize_box(ins->button[i], (FontWidth(xl) * (len_t += SPACING)),
		       (FontHeight(xl)/2 - IN_BORDER),
		       (euclen(ins->button[i]->string) * FontWidth(xl)
		        + IN_BORDER * 2), (FontHeight(xl) + IN_BORDER * 2));
	len_t += euclen(ins->button[i]->string);
    }

    XMoveResizeWindow(dpy, ins->w, x_cood, y_cood, width, height);

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
    XSetWMNormalHints(dpy, ins->w, &hints);
#else	/* X11R3 */
    XSetNormalHints(dpy, ins->w, &hints);
#endif	/* X11R3 */

    height = FontHeight(xl) + IN_BORDER * 2;
    XMoveResizeWindow(dpy, ins->w1, -1, (FontHeight(xl) * 2), width, height);
    XRaiseWindow(dpy, ins->w);
    XSetWindowBackground(dpy, ins->w, cur_p->pe.bg);
    XSetWindowBackground(dpy, ins->w1, cur_p->pe.bg);
    XClearWindow(dpy, ins->w);
    XClearWindow(dpy, ins->w1);
    XSetWindowBorder(dpy, ins->w, cur_p->pe.fg);
    XSetWindowBorder(dpy, ins->w1, cur_p->pe.fg);

    changecolor_box(ins->title, cur_p->pe.fg, cur_p->pe.bg);
    for (i = 0; i < ins->max_button; i++) {
	changecolor_box(ins->button[i], cur_p->pe.fg, cur_p->pe.bg);
	/*
	redraw_box(ins->button[i].window, ins->button[i].title, 0);
	*/
    }
    XMapWindow(dpy, ins->w);
    XFlush(dpy);
    /*
    redraw_box(ins->title_w, ins->title, 0);
    */
    JW3Mputc(ins->msg, ins->w1, 0, 0, IN_BORDER);
    XFlush(dpy);
    return(0);
}

void
end_inspect()
{
    Inspect *ins = xim->inspect;
    int i;

    XUnmapWindow(dpy, ins->w);
    XWarpPointer(dpy, None, root_window, 0, 0, 0, 0,
		 (int)ins->save_x, (int)ins->save_y);
    XFlush(dpy);
    for (i = 0; i < ins->max_button; i++) {
	ins->button[i]->in = '\0';
    }
    xim->sel_ret = -1;
    unlock_inspect();
    xim->j_c = NULL;
}

int
xw_next_inspect()
{
    unsigned char *s;
    register XIMLangRec *xl;
    Inspect *ins = xim->inspect;
    int len;

    if (xim->j_c == NULL) return(-1);
    xl = xim->j_c->cur_xl;
    cur_cl_change3(xim->j_c);

    s = next_inspect();
    if ((len = euclen(s)) > ins->max_len) {
	XResizeWindow(dpy, ins->w, FontWidth(xl) * len,
		      FontHeight(xl) * 3 + IN_BORDER * 2);
	XResizeWindow(dpy, ins->w1, FontWidth(xl) * len,
		      FontHeight(xl) + IN_BORDER * 2);
	ins->max_len = len;
    }
    ins->msg = s;
    XClearWindow(dpy, ins->w1);
    JW3Mputc(ins->msg, ins->w1, 0, 0, IN_BORDER);
    XFlush(dpy);
    return(0);
}

int
xw_back_inspect()
{
    unsigned char *s;
    register XIMLangRec *xl;
    Inspect *ins = xim->inspect;
    int len;

    if (xim->j_c == NULL) return(-1);
    xl = xim->j_c->cur_xl;
    cur_cl_change3(xim->j_c);

    s = previous_inspect();
    if ((len = euclen(s)) > ins->max_len) {
	XResizeWindow(dpy, ins->w, FontWidth(xl) * len,
		      FontHeight(xl) * 3 + IN_BORDER * 2);
	XResizeWindow(dpy, ins->w1, FontWidth(xl) * len,
		      FontHeight(xl) + IN_BORDER * 2);
	ins->max_len = len;
    }
    ins->msg = s;
    XClearWindow(dpy, ins->w1);
    JW3Mputc(ins->msg, ins->w1, 0, 0, IN_BORDER);
    XFlush(dpy);
    return(0);
}

static int
serch_jikouho_table(x, y)
int x, y;
{
    register int tmp, gap, kosuu = xim->ichi->kosuu;

    for (tmp = kosuu/2, gap = kosuu/4; (tmp >= 0 && tmp < kosuu);gap /= 2) {
	if (gap == 0) gap = 1;
	if (y > jikouho_ichi[tmp].y_e) {
	    tmp += gap;
	} else if (y < jikouho_ichi[tmp].y_s) {
	    tmp -= gap;
	} else {
	    for ( ; tmp >= 0 && tmp < kosuu; ) {
		if (x > jikouho_ichi[tmp].x_e) {
		    tmp++;
		} else if (x < jikouho_ichi[tmp].x_s) {
		    tmp--;
		} else if (y <= jikouho_ichi[tmp].y_e
			   && y >=jikouho_ichi[tmp].y_s) {
		    return (tmp);
		} else {
		    return(-1);
		}
	    }
	    return(-1);
	}
    }
    return(-1);
}

void
xw_move_hilite(d)
int d;
{
    static int y;
    register int item;

    y = d;
    item = Y_InItem(y);
    if (item != -1 && item != xim->ichi->hilited_item) {
	if (xim->ichi->hilited_item != -1) {
	    xw_fillrec();
	}
	if ((xim->ichi->hilited_item = item) != -1) {
	    xw_fillrec();
	}
    }
}

void
xw_jikouho_move_hilite(d_x, d_y)
int d_x, d_y;
{
    register int item;

    item = serch_jikouho_table(d_x, d_y);
    if (item != -1 && item != xim->ichi->hilited_ji_item) {
	if (xim->ichi->hilited_ji_item != -1)
	  xw_ji_fillrec();
	if ((xim->ichi->hilited_ji_item = item) != -1) {
	    xw_ji_fillrec();
	}
    }
}

void
alloc_for_save_buf()
{
    register Ichiran *ichi = xim->ichi;
    register int i;
    ichi->max_lines = MAX_ICHI_LINES;
    ichi->max_columns = MAX_ICHI_COLUMNS;

    ichi->save_buf =
	(unsigned char **)Malloc((unsigned)(ichi->max_lines * sizeof(char *)));
    for (i = 0; i < ichi->max_lines; i++) {
	ichi->save_buf[i] =
	(unsigned char *)Malloc((unsigned)(ichi->max_columns * sizeof(char)));
    }
}

static void
JW3mark_cursor(w, col, flg)
Window w;
int col;
int flg;
{
    register XIMLangRec *xl;

    if (xim->j_c == NULL) return;
    xl = xim->j_c->cur_xl;

    if (flg) {
	currentGC = cur_p->pe.reversegc;
    } else {
	currentGC = cur_p->pe.gc;
    }

    XwcDrawImageString(dpy, w, xl->pe_fs, currentGC, col * FontWidth(xl),
		       FontAscent(xl), &cursor_wchar_t, (int)1);
    XFlush(dpy);
}

void
redraw_ichi_w()
{
    register Ichiran *ichi;
    register int line = 0;
    register unsigned char *x;
    register int flg = 0;

    ichi = xim->ichi;

    XClearWindow(dpy, ichi->w1);
    XFlush(dpy);
    for (line = 0; line < ichi->kosuu_all; line++) {
	x = ichi->buf[line];
/*
	if (xim->ichi->mode == SENTAKU) {
	    if (line == ichi->hilited_item) {
		rev = REV_FLAG;
	    } else {
		rev = 0;
	    }
	}
*/
	if (ichi->init == line) {
	    flg = BOLD_FLAG;
	} else {
	    flg = 0;
	}
	JW3Mputc(x, ichi->w1, line, flg, IN_BORDER);
    }
    if (ichi->mode == SENTAKU && ichi->hilited_item != -1) {
	xw_fillrec();
    }
    if ((ichi->mode == JIKOUHO || ichi->mode == JIS_IN ||
	ichi->mode == KUTEN_IN) && ichi->hilited_ji_item != -1) {
	xw_ji_fillrec();
    }
    XFlush(dpy);
}

int
next_ichi()
{
    register Ichiran *ichi = xim->ichi;
    if (ichi->page < 2) return(-1);
    if (++ichi->cur_page > ichi->page)
        ichi->cur_page = 1;
    if (ichi->hilited_ji_item != -1)
	    xw_ji_fillrec();
    XMoveWindow(dpy, xim->ichi->w1, 0,
		-((ichi->cur_page - 1) * ichi->page_height));
    ichi->hilited_ji_item = jikouho_line[(ichi->cur_page - 1) * MAX_PAGE_LINES + 1];
    xw_ji_fillrec();
    XFlush(dpy);
    return(0);
}

int
back_ichi()
{
    register Ichiran *ichi = xim->ichi;
    if (ichi->page < 2) return(-1);
    if (--ichi->cur_page < 1)
        ichi->cur_page = ichi->page;
    if (ichi->hilited_ji_item != -1)
	xw_ji_fillrec();
    XMoveWindow(dpy, xim->ichi->w1, 0,
		-((ichi->cur_page - 1) * ichi->page_height));
    ichi->hilited_ji_item = jikouho_line[(ichi->cur_page - 1) * MAX_PAGE_LINES + 1];
    xw_ji_fillrec();
    XFlush(dpy);
    return(0);
}

int
find_hilite()
{
    return(xim->ichi->hilited_item);
}

int
find_ji_hilite()
{
    return(xim->ichi->hilited_ji_item);
}

void
xw_forward_select()
{
    register Ichiran *ichi = xim->ichi;

    if (ichi->mode == JIKOUHO || ichi->mode == JIS_IN
	|| ichi->mode == KUTEN_IN) {
	if (ichi->hilited_ji_item != -1) {
	    xw_ji_fillrec();
	}
	if(ichi->hilited_ji_item < ichi->kosuu - 1){
	    ichi->hilited_ji_item += 1;
	} else {
	    ichi->hilited_ji_item = 0;
	}
	check_move_ichi(jikouho_ichi[ichi->hilited_ji_item].line);
	xw_ji_fillrec();
    }
}

void
xw_backward_select()
{
    register Ichiran *ichi = xim->ichi;

    if (ichi->mode == JIKOUHO || ichi->mode == JIS_IN
	|| ichi->mode == KUTEN_IN) {
	if (ichi->hilited_ji_item != -1) {
	    xw_ji_fillrec();
	}
	if(ichi->hilited_ji_item > 0) {
	    ichi->hilited_ji_item -= 1;
	} else {
	    ichi->hilited_ji_item = ichi->kosuu - 1;
	}
	check_move_ichi(jikouho_ichi[ichi->hilited_ji_item].line);
	xw_ji_fillrec();
    }
}

void
xw_next_select()
{
    register Ichiran *ichi = xim->ichi;

    register int next, next_l, next_l2;
    register int cur_l = jikouho_ichi[ichi->hilited_ji_item].line;

    if (ichi->mode == JIKOUHO || ichi->mode == JIS_IN
	|| ichi->mode == KUTEN_IN) {
	if (ichi->max_line == 1) return;
	if (ichi->hilited_ji_item != -1)
	    xw_ji_fillrec();
	if (cur_l > ichi->kosuu_all - 1) {
	    next_l = jikouho_line[1];
	    next_l2 = jikouho_line[2];
	} else {
	    next_l = jikouho_line[cur_l + 1];
	    next_l2 = jikouho_line[cur_l + 2];
	}
	next = ichi->hilited_ji_item - jikouho_line[cur_l] + next_l;
	if (next >= next_l2) {
	    ichi->hilited_ji_item = next_l2 - 1;
	} else {
	    ichi->hilited_ji_item = next;
	}
	check_move_ichi(jikouho_ichi[ichi->hilited_ji_item].line);
	xw_ji_fillrec();
    } else if (ichi->mode == SENTAKU) {
	if (ichi->max_line == 1) return;
	if (ichi->hilited_item != -1)
	    xw_fillrec();
	if (++ichi->hilited_item >= ichi->kosuu)
	    ichi->hilited_item = 0;
	xw_fillrec();
    }
}

void
xw_previous_select()
{
    register Ichiran *ichi = xim->ichi;

    int prev, prev_l, prev_l2;
    int cur_l = jikouho_ichi[ichi->hilited_ji_item].line;

    if (ichi->mode == JIKOUHO || ichi->mode == JIS_IN
	|| ichi->mode == KUTEN_IN) {
	if (ichi->max_line == 1) 
	    return;
	if (ichi->hilited_ji_item != -1)
	    xw_ji_fillrec();
	if (cur_l == 1) {
	    prev_l = jikouho_line[ichi->max_line];
	    prev_l2 = jikouho_line[ichi->max_line + 1];
	} else {
	    prev_l = jikouho_line[cur_l - 1];
	    prev_l2 = jikouho_line[cur_l];
	}
	prev = ichi->hilited_ji_item - jikouho_line[cur_l] + prev_l;
	if (prev >= prev_l2) {
	    ichi->hilited_ji_item = prev_l2 - 1;
	} else {
	    ichi->hilited_ji_item = prev;
	}
	check_move_ichi(jikouho_ichi[ichi->hilited_ji_item].line);
	xw_ji_fillrec();
    } else if (ichi->mode == SENTAKU) {
	if (ichi->max_line == 1) 
	    return;
	if (ichi->hilited_item != -1)
	    xw_fillrec();
	if (--ichi->hilited_item < 0)
	    ichi->hilited_item = ichi->kosuu - 1;
	xw_fillrec();
    }
}

void
xw_linestart_select()
{
    register Ichiran *ichi = xim->ichi;

    if (ichi->mode == JIKOUHO || ichi->mode == JIS_IN
	|| ichi->mode == KUTEN_IN) {
	if (ichi->hilited_ji_item != -1)
	   xw_ji_fillrec();
	ichi->hilited_ji_item = jikouho_line[jikouho_ichi[ichi->hilited_ji_item].line];
	check_move_ichi(jikouho_ichi[ichi->hilited_ji_item].line);
	xw_ji_fillrec();
    }
}

void
xw_lineend_select()
{
    register Ichiran *ichi = xim->ichi;

    if (ichi->mode == JIKOUHO || ichi->mode == JIS_IN
	|| ichi->mode == KUTEN_IN) {
	if (ichi->hilited_ji_item != -1)
	    xw_ji_fillrec();
	ichi->hilited_ji_item
		= jikouho_line[jikouho_ichi[ichi->hilited_ji_item].line + 1] - 1;
	check_move_ichi(jikouho_ichi[ichi->hilited_ji_item].line);
	xw_ji_fillrec();
    }
}

int
init_yes_or_no(str, flg)
unsigned char *str;
int flg;
{
    int len, def_width, width, height, x_cood, y_cood;
    Window root, child;
    int	win_x, win_y;
    unsigned int keys_buttons;
    register YesOrNo *yes_no = cur_p->yes_no;
    register XIMLangRec *xl = cur_p->cur_xl;
    char *lang;

    lang = xl->lang;
    yes_no->map = 0;
    yes_no->exp = 0;

    yes_no->mode = flg;
    yes_no->title->string = str;

    if (flg != MessageOnly) {
	if (flg == YesMessage) {
	    yes_no->button[0]->string
		= (unsigned char *)msg_get(cd, 53, default_message[53], lang);
	} else if (flg == YesNoMessage) {
	    yes_no->button[0]->string
		= (unsigned char *)msg_get(cd, 51, default_message[51], lang);
	    yes_no->button[1]->string
		= (unsigned char *)msg_get(cd, 52, default_message[52], lang);
	}
	moveresize_box(yes_no->button[0], (FontWidth(xl) * 2),
		       (FontHeight(xl) * 2 + FontHeight(xl)/2 - IN_BORDER),
		       ((len = euclen(yes_no->button[0]->string))
			* FontWidth(xl) + IN_BORDER * 2),
		       (FontHeight(xl) + IN_BORDER * 2));
	changecolor_box(yes_no->button[0], cur_p->pe.fg, cur_p->pe.bg);
	map_box(yes_no->button[0]);
	def_width = FontWidth(xl) * (len + 8);
	if (flg == YesNoMessage) {
	    moveresize_box(yes_no->button[1], (FontWidth(xl) * (6 + len)),
			   (FontHeight(xl) * 2 + FontHeight(xl)/2 - IN_BORDER),
			   (FontWidth(xl) * euclen(yes_no->button[1]->string)
			    + IN_BORDER * 2),
			   (FontHeight(xl) + IN_BORDER * 2));
	    changecolor_box(yes_no->button[1], cur_p->pe.fg, cur_p->pe.bg);
	    map_box(yes_no->button[1]);
	    len += euclen(yes_no->button[1]->string);
	    def_width = FontWidth(xl) * (len + 8);
	} else {
	    def_width = FontWidth(xl) * (len + 4);
	}
	height = (FontHeight(xl) * 4);
    } else {
	def_width = 0;
	height = (FontHeight(xl) * 2);
    }
    if((width = FontWidth(xl) * ((len = euclen(str)) + 2)) < def_width)
	width = def_width;
    moveresize_box(yes_no->title, FontWidth(xl),
		   (FontHeight(xl)/2 - IN_BORDER),
		   (FontWidth(xl) * len + IN_BORDER * 2),
		   (FontHeight(xl) + IN_BORDER * 2));
    changecolor_box(yes_no->title, cur_p->pe.fg, cur_p->pe.bg);
    map_box(yes_no->title);

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
		     (x_cood + FontWidth(xl) * 2),
		     (y_cood + FontHeight(xl) * 3));
    }
    XMoveResizeWindow(dpy, yes_no->w, x_cood, y_cood, width, height);
    XSetWindowBackground(dpy, yes_no->w, cur_p->pe.bg);
    XClearWindow(dpy, yes_no->w);
    XSetWindowBorder(dpy, yes_no->w, cur_p->pe.fg);

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
    /*
    redraw_box(yes_no->title_w, yes_no->title, 0);
    if (flg == YesNoMessage) {
	redraw_box(yes_no->button[0].window, yes_no->button[0].title, 0);
	redraw_box(yes_no->button[1].window, yes_no->button[1].title, 0);
    } else if (flg == YesMessage) {
	redraw_box(yes_no->button[0].window, yes_no->button[0].title, 0);
    }
    */
    XFlush(dpy);
    return(0);
}

void
end_yes_or_no()
{
    register YesOrNo *yes_no = cur_p->yes_no;

    unmap_box(yes_no->button[0]);
    unmap_box(yes_no->button[1]);
    XUnmapWindow(dpy, yes_no->w);
    if (yes_no->mode != MessageOnly) {
    	XWarpPointer(dpy, None, root_window,
		     0, 0, 0, 0, yes_no->x, yes_no->y);
    }
    yes_no->mode = 0;
    XFlush(dpy);
    yes_no->button[0]->in = yes_no->button[1]->in = 0;
    xim->sel_ret = -1;
}

int
set_j_c()
{
    if (xim->j_c) {
	cur_cl_change3(xim->j_c);
    }
    return(0);
}

int
cur_cl_change3(xc)
XIMClientRec *xc;
{
    if (/*cur_p == 0 || */xc == 0) {
	cur_p = xim->ximclient;
	c_c = cur_p->cur_xl->w_c;
    } else {
	if (IsPreeditNothing(xc)) {
	    cur_p = xim->ximclient;
	    cur_x = xc;
	    c_c = xim->ximclient->cur_xl->w_c;
	} else {
	    cur_p = cur_x = xc;
	    c_c = xc->cur_xl->w_c;
	}
    }
    cur_lang = cur_x->cur_xl->lang;
    cur_rk = c_c->rk;
    cur_rk_table = cur_rk->rk_table;
    cur_input = 0;
    return(0);
}

int
cur_cl_change4(xc)
XIMClientRec *xc;
{
    xc->yes_no->exp = 1;
    cur_cl_change3(xc);
    return(0);
}

void
xw_select_button(event)
XButtonEvent *event;
{
    static int y;
    register int item;

    if (xim->j_c) {
	y = event->y;
	if ((item = Y_InItem(y)) != xim->ichi->hilited_item) {
	    if (xim->ichi->hilited_item != -1)
		xw_fillrec();
	    if ((xim->ichi->hilited_item = item) != -1) {
		xw_fillrec();
	    }
	}
	XFlush(dpy);
	xim->sel_ret = item;
    }
}

void
xw_select_jikouho_button(event)
XButtonEvent *event;
{
    register int item;

    if (xim->j_c) {
	if ((item=serch_jikouho_table(event->x,event->y))
	     != xim->ichi->hilited_ji_item) {
	    if (xim->ichi->hilited_ji_item != -1)
		xw_ji_fillrec();
	    if ((xim->ichi->hilited_ji_item = item) != -1) {
		xw_ji_fillrec();
	    }
	}
	XFlush(dpy);
	xim->sel_ret = item;
    }
}

void
xw_mouseleave()
{
    if (xim->j_c && xim->ichi->kosuu == 1) {
	if (xim->ichi->hilited_item != -1) {
	    xw_fillrec();
	    xim->ichi->hilited_item = -1;
	}
    }
}

int
xw_mouse_select()
{
    register char *lang = cur_p->cur_xl->lang;
    if (xim->ichi->select_mode == 0) {
	XSelectInput(dpy, xim->ichi->w1,
		     (EnterWindowMask | LeaveWindowMask	| PointerMotionMask |
		      ButtonPressMask | ButtonReleaseMask | ExposureMask));
	XDefineCursor(dpy, xim->ichi->w1, XCreateFontCursor(dpy,52));
	xim->ichi->button[MOUSE_W]->string = (unsigned char *)msg_get(cd, 39,
						default_message[39], lang);
	xim->ichi->select_mode = 1;
    } else {
	XSelectInput(dpy, xim->ichi->w1, ExposureMask);
	XUndefineCursor(dpy, xim->ichi->w1);
	xim->ichi->button[MOUSE_W]->string = (unsigned char *)msg_get(cd, 40,
						default_message[40], lang);
	xim->ichi->select_mode = 0;
    }
    JW3Mputc(xim->ichi->button[MOUSE_W]->string,
	     xim->ichi->button[MOUSE_W]->window, 0, REV_FLAG, IN_BORDER);
    XFlush(dpy);
    return(0);
}

void
draw_nyuu_w(buf, rev)
unsigned char *buf;
int rev;
{
    xim->ichi->nyuu = (unsigned char *)buf;
    JW3Mputc(buf, xim->ichi->nyuu_w, 0, (rev ? REV_FLAG : 0), 0);
}

void
clear_nyuu_w()
{
    XClearWindow(dpy, xim->ichi->nyuu_w);
}

void
nyuu_w_cursor(col, on)
int col, on;
{
    JW3mark_cursor(xim->ichi->nyuu_w, col, on);
}

int
yes_or_no(string)
char *string;
{
    XIMClientRec *xl_sv;

    xl_sv = cur_p;
    init_yes_or_no((unsigned char *)string,YesNoMessage);
    for(;;) {
	get_xevent();
	if (!cur_p->yes_no->map) {
	    xim->sel_ret = -1;
	    continue;
	}
	if (xim->sel_ret == 1) {
	    end_yes_or_no();
	    cur_cl_change3(xl_sv);
	    xim->sel_ret = -1;
	    return(1);
	} else if (xim->sel_ret == 0) {
	    end_yes_or_no();
	    cur_cl_change3(xl_sv);
	    xim->sel_ret = -1;
	    return(0);
	} else {
	    xim->sel_ret = -1;
	    continue;
	}
    }
}

int
yes_or_no_or_newline(string)
char *string;
{
    return(yes_or_no(string));
}

