/*
 * $Id: display.c,v 1.5 1991/09/16 21:36:38 ohm Exp $
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

extern wchar_t dol_wchar_t;

wchar	cursor_wchar = 0x20202020;
wchar wc_buf[512];
char ct_buf[512];

GC currentGC;

static void
JWwindow_move(xl)
register XIMLangRec *xl;
{

    if ((check_mb(xl->buf, xl->vst)) == 2) {
	xl->vst++;
    }
    XMoveWindow(dpy, xl->w[0], -(xl->vst * FontWidth(xl)), 0);
    if (IsPreeditPosition(cur_x)) {
	XMoveWindow(dpy, xl->w[1],
		-((cur_x->c0 + xl->vst - xl->linefeed[1]) * FontWidth(xl)), 0);
	XMoveWindow(dpy, xl->w[2],
		-((c01(cur_x) + xl->vst - xl->linefeed[2]) * FontWidth(xl)), 0);
    }
    XFlush(dpy);
}

static void
JWM_note(which)
int which;
{
    register XIMLangRec *xl;
    register XIMClientRec *xc = cur_p;
    int	x, y;

    xl = cur_p->cur_xl;

    if (xl->note[which]) return;

    if (IsPreeditNothing(cur_x) || IsPreeditArea(cur_x)) {
	XClearWindow(dpy, xl->wn[which]);
	XMapWindow(dpy, xl->wn[which]);
	XRaiseWindow(dpy, xl->wn[which]);
	XwcDrawString(dpy, xl->wn[which], xl->pe_fs, xc->pe.gc, 0,
		      FontAscent(xl), &dol_wchar_t, 1);
    } else if (IsPreeditPosition(cur_x)) {
	if (which == 1) {
	    if (xc->c2) {
		x = PreeditX(xc) + (xc->c2 * FontWidth(xl));
		y = PreeditSpotY(xc)
		  + (FontHeight(xl) + xc->pe.line_space) * 2;
		if ((int)(y + FontHeight(xl)) >
		    (int)(PreeditY(xc) + PreeditHeight(xc))) {
		    if ((int)(y - xc->pe.line_space) >
			(int)(PreeditY(xc) + PreeditHeight(xc))) {
			y = PreeditY(xc) + FontHeight(xl) + xc->pe.line_space;
		    } else {
			y = PreeditY(xc);
		    }
		}
	    } else if (xc->c1) {
		x = PreeditX(xc) + (xc->c1 * FontWidth(xl));
		y = PreeditSpotY(xc) + FontHeight(xl) + xc->pe.line_space;
		if ((int)(y + FontHeight(xl)) >
		    (int)(PreeditY(xc) + PreeditHeight(xc))) {
		    y = PreeditY(xc);
		}
	    } else {
		x = PreeditSpotX(xc) + xc->c0 * FontWidth(xl);
		y = PreeditSpotY(xc);
	    }
	} else {
	    x = PreeditSpotX(xc) - FontWidth(xl);
	    y = PreeditSpotY(xc);
	}
	XMoveWindow(dpy, xl->wn[which], x, y);
	XClearWindow(dpy, xl->wn[which]);
	XMapWindow(dpy, xl->wn[which]);
	XRaiseWindow(dpy, xl->wn[which]);
	XwcDrawString(dpy, xl->wn[which], xl->pe_fs, xc->pe.gc, 0,
		      FontAscent(xl), &dol_wchar_t, 1);
    }
    XFlush(dpy);
}

static void
JWM_note_null()
{
    register XIMLangRec *xl;
    register XIMClientRec *xc = cur_p;
    int	x, y;

    xl = cur_p->cur_xl;

    if (xl->visible_line > 2) {
	x = PreeditX(xc) + ((xc->c2 - 1) * FontWidth(xl));
	y = PreeditSpotY(xc) + (FontHeight(xl) + xc->pe.line_space) * 2;
	if ((int)(y + FontHeight(xl)) > (int)(PreeditY(xc) + PreeditHeight(xc)))
	    if ((int)(y - xc->pe.line_space) >
		(int)(PreeditY(xc) + PreeditHeight(xc))) {
		y = PreeditY(xc) + FontHeight(xl) + xc->pe.line_space;
	    } else {
		y = PreeditY(xc);
	    }
    } else if (xl->visible_line > 1) {
	x = PreeditX(xc) + ((xc->c1 - 1) * FontWidth(xl));
	y = PreeditSpotY(xc) + FontHeight(xl) + xc->pe.line_space;
	if ((int)(y + FontHeight(xl)) > (int)(PreeditY(xc) + PreeditHeight(xc)))
	    y = PreeditY(xc);
    } else {
	x = PreeditSpotX(xc) + (xc->c0 - 1) * FontWidth(xl);
	y = PreeditSpotY(xc);
    }
    XMoveWindow(dpy, xl->wn[2], x, y);
    XMapWindow(dpy, xl->wn[2]);
    XRaiseWindow(dpy, xl->wn[2]);
    XFlush(dpy);
}

static void
invisual_note(which)
int which;
{
    register XIMLangRec	*xl;

    xl = cur_p->cur_xl;

    if (xl->note[which]) {
	XUnmapWindow(dpy, xl->wn[which]);
	XFlush(dpy);
    }
}

static void
visible_line2()
{
    register XIMLangRec *xl;
    register XIMClientRec *xc = cur_p;
    int x, y, width;

    xl = cur_p->cur_xl;

    if ((xc->c1 == 0) || !IsPreeditPosition(cur_x)) return;

    x = PreeditX(xc);
    y = PreeditSpotY(xc) + FontHeight(xl) + xc->pe.line_space;
    if ((int)(y + FontHeight(xl)) > (int)(PreeditY(xc) + PreeditHeight(xc)))
	y = PreeditY(xc);
    if (xl->max_cur > (xc->c0 + xc->c1)) {
	width = xc->c1 * FontWidth(xl);
    } else {
	width = (xl->max_cur - xc->c0 + xl->linefeed[1]) * FontWidth(xl);
    }
    XMoveResizeWindow(dpy, xl->wp[1], x, y, width, FontHeight(xl));
    XMoveWindow(dpy, xl->w[1],
		-((xc->c0 + xl->vst - xl->linefeed[1]) * FontWidth(xl)), 0);
    XMapWindow(dpy, xl->wp[1]);
    XRaiseWindow(dpy, xl->wp[1]);
    XFlush(dpy);
}

static void
visible_line3()
{
    register XIMLangRec	*xl;
    register XIMClientRec *xc = cur_p;
    int x, y, width;

    xl = cur_p->cur_xl;

    if ((xc->c2 == 0) || !IsPreeditPosition(cur_x)) return;
    x = PreeditX(xc);
    y = PreeditSpotY(xc) + (FontHeight(xl) + xc->pe.line_space) * 2;
    if ((int)(y + FontHeight(xl)) > (int)(PreeditY(xc) + PreeditHeight(xc))) {
	if ((int)(y - xc->pe.line_space) >
	    (int)(PreeditY(xc) + PreeditHeight(xc))) {
	    y = PreeditY(xc) + FontHeight(xl) + xc->pe.line_space;
	} else {
	    y = PreeditY(xc);
	}
    }
    if (xl->max_cur > c012(xc)) {
	width = xc->c2 * FontWidth(xl);
    } else {
	width = (xl->max_cur - c01(xc) + xl->linefeed[2]) * FontWidth(xl);
    }
    XMoveResizeWindow(dpy, xl->wp[2], x, y, width, FontHeight(xl));
    XMoveWindow(dpy, xl->w[2],
		-((c01(xc) + xl->vst - xl->linefeed[2]) * FontWidth(xl)), 0);
    XMapWindow(dpy, xl->wp[2]);
    XRaiseWindow(dpy, xl->wp[2]);
    XFlush(dpy);
}

static void
check_move1(xc, xl, mb)
register XIMClientRec *xc;
register XIMLangRec *xl;
int mb;
{
    register int currentcol_tmp;
    int max_pos_tmp;

    currentcol_tmp = xl->currentcol
      + xl->linefeed[(xl->visible_line? (xl->visible_line - 1):0)];

    if ((currentcol_tmp - xl->vst + mb) >= xc->max_columns) {
	do {
	    xl->vst = currentcol_tmp - xc->max_columns + mb + 1;
	    if ((check_mb(xl->buf, xl->vst)) == 2) {
		xl->vst++;
	    }
	    currentcol_tmp = xl->currentcol;
	    if (xl->visible_line > 1 && xc->c1) {
		if ((check_mb(xl->buf, (xc->c0 + xl->vst - 1))) == 1) {
		    xl->linefeed[1] = 1;
		} else {
		    xl->linefeed[1] = 0;
		}
		currentcol_tmp = xl->currentcol + xl->linefeed[1];
	    }
	    if (xl->visible_line > 2 && xc->c2) {
		if ((check_mb(xl->buf, 
			      (c01(xc) + xl->vst - xl->linefeed[1] - 1))) == 1){
		    xl->linefeed[2] = xl->linefeed[1] + 1;
		} else {
		    xl->linefeed[2] = xl->linefeed[1];
		}
		currentcol_tmp = xl->currentcol + xl->linefeed[2];
	    }
	}
	while ((currentcol_tmp - xl->vst + mb) >= xc->max_columns);
	JWwindow_move(xl);
    } else if (xl->currentcol < xl->vst) {
	xl->vst = xl->currentcol;
	if (xc->c1 && ((xl->visible_line > 1)
			 || (xl->vst + xc->c0 < xl->max_pos))) {
	    if ((check_mb(xl->buf, (xc->c0 + xl->vst - 1))) == 1) {
		xl->linefeed[1] = 1;
	    } else {
		xl->linefeed[1] = 0;
	    }
	    if (xl->visible_line < 2 && (xl->vst + xc->c0 < xl->max_pos)) {
		visible_line2();
		xl->visible_line = 2;
	    }
	}
	if (xc->c2 && ((xl->visible_line > 2)
		   || ((c01(xc) + xl->vst - xl->linefeed[1]) < xl->max_pos))) {
	    if ((check_mb(xl->buf, 
		(c01(xc) + xl->vst - xl->linefeed[1] - 1))) == 1) {
		xl->linefeed[2] = xl->linefeed[1] + 1;
	    } else {
		xl->linefeed[2] = xl->linefeed[1];
	    }
	    if (xl->visible_line < 3
	    	&& (c01(xc) + xl->vst - xl->linefeed[1] < xl->max_pos)) {
		visible_line3();
		xl->visible_line = 3;
	    }
	}
	JWwindow_move(xl);
    }

    if (xl->vst) {
	if (!xl->note[0]) {
	    JWM_note(0);
	    xl->note[0] = 1;
	}
    } else {
	if (xl->note[0]) {
	    invisual_note(0);
	    xl->note[0] = 0;
	}
    }
    max_pos_tmp = xl->vst + xc->max_columns
      - xl->linefeed[(xl->visible_line? (xl->visible_line - 1):0)];

    if (xl->max_pos > max_pos_tmp) {
	if (!xl->note[1]) {
	    JWM_note(1);
	    xl->note[1] = 1;
	}
	if ((check_mb(xl->buf, (max_pos_tmp - 1))) == 1) {
	    JWM_note_null();
	} else {
	    XUnmapWindow(dpy, xl->wn[2]);
	}
    } else {
	if (xl->note[1]) {
	    invisual_note(1);
	    xl->note[1] = 0;
	}
	XUnmapWindow(dpy, xl->wn[2]);
    }
}

static void
check_move(xc, xl, mb)
register XIMClientRec *xc;
register XIMLangRec *xl;
int mb;
{
    if (IsPreeditPosition(cur_x)) {
		check_move1(xc, xl, mb);
    } else {
	if ((xl->currentcol - xl->vst + mb) >= xc->max_columns) {
	    xl->vst = xl->currentcol - xc->max_columns + mb + 1;
	    JWwindow_move(xl);
	} else if (xl->currentcol < xl->vst) {
	    xl->vst = xl->currentcol;
	    JWwindow_move(xl);
	}
	if (xl->vst) {
	    JWM_note(0);
	    xl->note[0] = 1;
	} else {
	    invisual_note(0);
	    xl->note[0] = 0;
	}
	if (xl->max_pos > (xl->vst + xc->max_columns)) {
	    JWM_note(1);
	    xl->note[1] = 1;
	    if ((check_mb(xl->buf, (xl->vst + xc->max_columns - 1)))
		 == 1) {
		XMapWindow(dpy, xl->wn[2]);
	    } else {
		XUnmapWindow(dpy, xl->wn[2]);
	    }
	} else {
	    invisual_note(1);
	    xl->note[1] = 0;
	    XUnmapWindow(dpy, xl->wn[2]);
	}
    }
}

static void
Resize_jw()
{
    register XIMLangRec *xl;
    Window window;
    int width = 0;
    char ok = '\0';

    xl = cur_p->cur_xl;
    if (!IsPreeditPosition(cur_x) || !xl->m_cur_flag) return;

    if (cur_p->c2) {
	if (xl->max_cur <= (c012(cur_p) - xl->linefeed[2])){
	    width = (xl->max_cur - c01(cur_p) + xl->linefeed[2]);
	    xl->max_l3 = 0;
	    ok = 1;
	} else if (!xl->max_l3) {
	    width = cur_p->c2;
	    xl->max_l3 = 1;
	    ok = 1;
	}
	if (ok) {
	    window = xl->wp[2];
	    XResizeWindow(dpy, window, (width * FontWidth(xl)),
			  FontHeight(xl));
	}
    }
    if (cur_p->c1) {
	ok = '\0';
	if (xl->max_cur <= (c01(cur_p) - xl->linefeed[1])) {
	    width = (xl->max_cur - cur_p->c0 + xl->linefeed[1]);
	    xl->max_l2 = 0;
	    ok = 1;
	} else if (!xl->max_l2) {
	    width = cur_p->c1;
	    xl->max_l2 = 1;
	    ok = 1;
	}
	if (ok) {
	    window = xl->wp[1];
	    XResizeWindow(dpy, window, (width * FontWidth(xl)),
			  FontHeight(xl));
	}
    }
    if (cur_p->c0) {
	ok = '\0';
	if (xl->max_cur <= cur_p->c0) {
	    width = xl->max_cur;
	    xl->max_l1 = 0;
	    ok = 1;
	} else if (!xl->max_l1) {
	    width = cur_p->c0;
	    xl->max_l1 = 1;
	    ok = 1;
	}
	if (ok) {
	    window = xl->wp[0];
	    XResizeWindow(dpy, window, (width * FontWidth(xl)),
			  FontHeight(xl));
	}
    }
    XFlush(dpy);
    xl->m_cur_flag = 0;
}

void
JWmark_cursor(on)
int on;
{
    register XIMLangRec *xl;
    register XIMClientRec *xc = cur_p;
    wchar *JW_buf;
    unsigned char *JW_att;
    Window currentW = 0;
    int mb_len;
    unsigned char flg;
    XCharStruct cs;

    xl = cur_p->cur_xl;
    if (xl->cursor_flag != 1 || xl->mark_flag == on) return;
    xl->mark_flag = on;

    JW_buf = xl->buf + xl->currentcol;
    JW_att = xl->att + xl->currentcol;
    cs.width = FontWidth(xl);
    cs.ascent = FontAscent(xl);
    cs.descent = FontDescent(xl);

    if ((*JW_att & REV_FLAG) != 0) return;

    if (*JW_buf) {
	if ((mb_len = get_columns_wchar(JW_buf)) > 0) {
	    JW_buf += (mb_len - 1);
	    JW_att += (mb_len - 1);
	}
	flg = *JW_att;
    } else {
	JW_buf = &cursor_wchar;
	JW_att = '\0';
	mb_len = 1;
	flg = 0;
    }
    if (on) {
	if (mb_len > 1) {
	    check_move(xc, xl, 1);
	} else {
	    check_move(xc, xl, 0);
	}
	currentGC = xc->pe.reversegc;
	flg |= REV_FLAG;
    } else {
	currentGC = xc->pe.gc;
    }

    if (IsPreeditNothing(cur_x) || IsPreeditArea(cur_x)) {
	currentW = xl->w[0];
    } else if (IsPreeditPosition(cur_x)) {
	if ((xl->currentcol + mb_len - xl->vst) <= xc->c0) {
	    currentW = xl->w[0];
	} else if ((xl->currentcol + xl->linefeed[1] + mb_len - xl->vst)
		   <= c01(xc)) {
	    currentW = xl->w[1];
	} else {
	    currentW = xl->w[2];
	}
	if ((*(xl->buf + xl->currentcol) == 0) && on) {
	    if (xc->c1 && ((xl->currentcol - xl->vst) == xc->c0)) {
		xl->linefeed[1] = 0;
		visible_line2();
		if (xl->visible_line < 2) {
		    xl->visible_line = 2;
		}
	    }
	    if (xc->c2 &&
		((xl->currentcol  + xl->linefeed[1] - xl->vst) == c01(xc))) {
		xl->linefeed[2] = xl->linefeed[1];
		visible_line3();
		if (xl->visible_line < 3) {
		    xl->visible_line = 3;
		}
	    }
	    if ((xl->currentcol - xl->vst) >= xl->max_cur) {
		xl->max_cur += 1;
		xl->m_cur_flag = 1;
		Resize_jw();
	    }
	}
	if (on) {
	    if ((xl->max_pos - xl->vst) > xl->max_cur && !xl->m_cur_flag) {
		xl->max_cur = xl->max_pos - xl->vst;
		xl->m_cur_flag = 1;
		Resize_jw();
	    }
	}
    }
    JWOutput(currentW, xl->pe_fs, currentGC, xl->currentcol, mb_len,
	     flg, 0, 0, &cs, JW_buf, 1);
    XFlush(dpy);
}

void
redraw_window3(x, width)
int	x, width;
{
    register XIMLangRec *xl;
    wchar *JW_buf, *wc;
    unsigned char *JW_att;
    unsigned char old_JW_att;
    int currentcol_tmp = 0;
    int start, end, startcol;
    int wc_len, mb_len;
    int col;
    XCharStruct cs;

    if (IsPreeditNothing(cur_x)) {
	xl = xim->ximclient->cur_xl;
    } else if (IsPreeditArea(cur_x)) {
	xl = cur_p->cur_xl;
    } else {
	return;
    }

    currentcol_tmp = xl->currentcol;

    start = x / FontWidth(xl);
    end = start + width / FontWidth(xl) + ((width % FontWidth(xl))? 1:0);
    if (end > xl->max_pos) end = xl->max_pos;
    if (check_mb(xl->buf, (start)) == 2) start--;
    if (check_mb(xl->buf, (end - 1)) == 1) end++;

    JW_buf = xl->buf + start;
    JW_att = xl->att + start;
    old_JW_att = *JW_att;
    wc = wc_buf;
    skip_pending_wchar(wc, JW_buf);
    wc_len = 0;
    col = 0;
    startcol = start;
    cs.width = FontWidth(xl);
    cs.ascent = FontAscent(xl);
    cs.descent = FontDescent(xl);

    for (xl->currentcol = start; xl->currentcol < end;) {
	JW_buf = xl->buf + xl->currentcol;
	JW_att = xl->att + xl->currentcol;
	if ((mb_len = get_columns_wchar(JW_buf)) > 0) {
	    JW_buf += (mb_len - 1);
	    JW_att += (mb_len - 1);
	}
	if ((*JW_att != old_JW_att) && (wc_len > 0)) {
	    if (old_JW_att & REV_FLAG) {
		currentGC = cur_p->pe.reversegc;
	    } else {
		currentGC = cur_p->pe.gc;
	    }
	    JWOutput(xl->w[0], xl->pe_fs, currentGC, startcol, col,
		     old_JW_att, 0, 0, &cs, wc, wc_len);
	    old_JW_att = *JW_att;
	    wc += wc_len;
	    wc_len = 0;
	    col = 0;
	    startcol = xl->currentcol;
	}
	if (*JW_att & REV_FLAG) {
	    if (mb_len) {
		check_move(cur_p, xl, 1);
	    } else {
		check_move(cur_p, xl, 0);
	    }
	}
	wc_len++;
	col += mb_len;
	xl->currentcol += mb_len;
    }
    if (wc_len > 0) {
	if (old_JW_att & REV_FLAG) {
	    currentGC = cur_p->pe.reversegc;
	} else {
	    currentGC = cur_p->pe.gc;
	}
	JWOutput(xl->w[0], xl->pe_fs, currentGC, startcol, col,
		 old_JW_att, 0, 0, &cs, wc, wc_len);
    }
    xl->currentcol = currentcol_tmp;
    if(xl->cursor_flag == 1 && xl->mark_flag == 1) {
	xl->mark_flag = 0;
	JWmark_cursor(1);
    }
}

void
redraw_lines(x, width, line)
int x, width, line;
{
    register XIMLangRec *xl;
    register XIMClientRec *xc = cur_p;
    register wchar *JW_buf, *wc;
    unsigned char *JW_att;
    unsigned char old_JW_att;
    int currentcol_tmp = 0;
    int start, end, startcol;
    int tmp_col = 0;
    int wc_len, mb_len;
    int col;
    XCharStruct cs;

    xl = cur_p->cur_xl;
    currentcol_tmp = xl->currentcol;
    if (line == 1) {
	tmp_col = xc->c0;
    } else if (line == 2) {
	tmp_col = c01(xc) - xl->linefeed[1];
    }
    start = x / FontWidth(xl);
    end = start + width / FontWidth(xl) + ((width % FontWidth(xl))? 1:0);
    if (end > xl->max_pos) end = xl->max_pos;
    if (check_mb(xl->buf, (start)) == 2) start--;
    if (check_mb(xl->buf, (end - 1)) == 1) end++;

    JW_buf = xl->buf + start;
    JW_att = xl->att + start;
    old_JW_att = *JW_att;
    wc = wc_buf;
    wc_len = 0;
    skip_pending_wchar(wc, JW_buf);
    col = 0;
    startcol = start;
    cs.width = FontWidth(xl);
    cs.ascent = FontAscent(xl);
    cs.descent = FontDescent(xl);

    for ( xl->currentcol = start; xl->currentcol < end;) {
	JW_buf = xl->buf + xl->currentcol;
	JW_att = xl->att + xl->currentcol;
	if ((mb_len = get_columns_wchar(JW_buf)) > 0) {
	    JW_buf += (mb_len - 1);
	    JW_att += (mb_len - 1);
	}
	if ((*JW_att != old_JW_att) && (wc_len > 0)) {
	    if (old_JW_att & REV_FLAG) {
		currentGC = cur_p->pe.reversegc;
	    } else {
		currentGC = cur_p->pe.gc;
	    }
	    JWOutput(xl->w[line-1], xl->pe_fs, currentGC, startcol, col,
		     old_JW_att, 0, 0, &cs, wc, wc_len);
	    old_JW_att = *JW_att;
	    wc += wc_len;
	    wc_len = 0;
	    col = 0;
	    startcol = xl->currentcol;
	}
	wc_len++;
	col += mb_len;
	xl->currentcol += mb_len;
	if (*JW_att & REV_FLAG) {
	    if (mb_len) {
		check_move(cur_p, xl, 1);
	    } else {
		check_move(cur_p, xl, 0);
	    }
	}
	if (line != 3) {
	    if (mb_len > 1) {
		if (xl->currentcol > (tmp_col + xl->vst - 2)) {
		    if ((line == 1 && xc->c1) || (line == 2 && xc->c2)) {
			if (check_mb(xl->buf, (tmp_col + xl->vst - 1)) == 1) {
			    xl->linefeed[line] = xl->linefeed[line - 1] + 1;
			} else {
			    xl->linefeed[line] = xl->linefeed[line - 1];
			}
		    }
		    break;
		}
	    } else {
		if (xl->currentcol > (tmp_col + xl->vst - 1)) {
		    if ((line == 1 && xc->c1) || (line == 2 && xc->c2)) {
			xl->linefeed[line] = xl->linefeed[line - 1];
		    }
		    break;
		}
	    }
	}
    }
    if (wc_len > 0) {
	if (old_JW_att & REV_FLAG) {
	    currentGC = cur_p->pe.reversegc;
	} else {
	    currentGC = cur_p->pe.gc;
	}
	JWOutput(xl->w[line-1], xl->pe_fs, currentGC, startcol, col,
		 old_JW_att, 0, 0, &cs, wc, wc_len);
    }
    xl->currentcol = currentcol_tmp;
    if (line == 1) {
	if (xl->linefeed[line]) {
	    XClearArea(dpy, xl->w[line - 1],
		       (xc->c0 + xl->vst - 1) * FontWidth(xl), 0,0,0, False);
	}
	if(xl->cursor_flag && xl->mark_flag
	   && (xl->currentcol < (xc->c0 + xl->vst))) {
	    xl->mark_flag = 0;
	    JWmark_cursor(1);
	}
    } else if (line == 2) {
	if (xl->linefeed[line]) {
	    XClearArea(dpy, xl->w[line - 1],
		       ((c01(xc) + xl->vst + xl->linefeed[1] - 1)
			* FontWidth(xl)), 0, 0, 0, False);
	}
	if (xl->cursor_flag && xl->mark_flag
	    && (xl->currentcol < (tmp_col + xl->vst))
	    && (xl->currentcol >= (xc->c0 + xl->vst - xl->linefeed[1]))) {
	    xl->mark_flag = 0;
	    JWmark_cursor(1);
	}
    } else {
	if(xl->cursor_flag && xl->mark_flag
	   && (xl->currentcol >= (tmp_col + xl->vst))) {
	   xl->mark_flag = 0;
	   JWmark_cursor(1);
	}
    }
}

void
redraw_note(which)
int which;
{
    XIMLangRec *xl = cur_p->cur_xl;

    XClearWindow(dpy, xl->wn[which]);
    XwcDrawString(dpy, xl->wn[which], xl->pe_fs, cur_p->pe.gc,
		  0, FontAscent(xl), &dol_wchar_t, 1);
    XFlush(dpy);
}

void
redraw_window0()
{
    register XIMLangRec *xl;
    wchar *JW_buf, *wc;
    int wc_len;
    int col = 0;
    int mb_len;
    unsigned char flg;
    XCharStruct cs;

    xl = cur_p->cur_xl;
    JW_buf = xl->buf0;
    flg = xl->att0[0];
    cs.width = StatusFontWidth(xl);
    cs.ascent = StatusFontAscent(xl);
    cs.descent = StatusFontDescent(xl);
    for (;*JW_buf ; JW_buf++) {
	mb_len = get_columns_wchar(JW_buf);
	JW_buf += (mb_len - 1);
	col += mb_len;
    }
    wc = wc_buf;
    wc_len = skip_pending_wchar(wc, xl->buf0);
    if (wc_len <= 0) return;
    JWOutput(xl->ws, xl->st_fs,
	     ((flg & REV_FLAG)? cur_p->st.reversegc: cur_p->st.gc),
	     0, col, flg, 0, 0, &cs, wc, wc_len);
}

void
redraw_xj_all()
{
    register XIMLangRec	*xl;

    xl = cur_p->cur_xl;

    redraw_window0();
    if (IsPreeditNothing(cur_x) || IsPreeditArea(cur_x)) {
	redraw_window3(0, xl->max_pos * FontWidth(xl));
    } else if (IsPreeditPosition(cur_x)) {
	redraw_lines(xl->vst * FontWidth(xl), xl->max_pos * FontWidth(xl), 1);
	redraw_lines((xl->vst + cur_p->c0 - xl->linefeed[1]) * FontWidth(xl),
	    	     xl->max_pos * FontWidth(xl), 2);
	redraw_lines((xl->vst + c01(cur_p) - xl->linefeed[2])
		     * FontWidth(xl), xl->max_pos * FontWidth(xl), 3);
    }
    if (xl->note[0] != 0) JWM_note(0);
    if (xl->note[1] != 0) JWM_note(1);
}

static void
visual_window3()
{
    register XIMLangRec *xl;

    xl = cur_p->cur_xl;

    XMapWindow(dpy, xl->wp[0]);
    XMapWindow(dpy, xl->w[0]);
    if (xl->max_pos) {
	redraw_window3(0, xl->max_pos * FontWidth(xl));
    }
    if (IsPreeditNothing(cur_x)) {
	xl->visible = 3;
    } else {
	xl->visible = 1;
	xl->visible_line = 1;
    }
    if (xl->note[0] != 0) JWM_note(0);
    if (xl->note[1] != 0) JWM_note(1);
}

void
visual_window1()
{
    int	width;
    register int	w_width;
    register XIMLangRec *xl;
    int max_pos_tmp;
    register XIMClientRec *xc = cur_p;

    xl = cur_p->cur_xl;
    if (xl->visible) return;

    if (IsPreeditPosition(cur_x)) {
	w_width = (int)PreeditWidth(xc) / (int)FontWidth(xl);
	xc->max_columns = ((int)PreeditWidth(xc) / (int)FontWidth(xl));
	xc->c0 = (int)(PreeditWidth(xc) + PreeditX(xc) - PreeditSpotX(xc)) /
		    (int)FontWidth(xl);
	if (xc->c0 < 0) xc->c0 = 0;
	if (xc->c0 >= xc->max_columns) {
	    xc->c0 = xc->max_columns;
	    xc->c1 = xc->c2 = 0;
	} else if ((xc->c1 = xc->max_columns - xc->c0) > w_width) {
	    xc->c1 = w_width;
	    xc->c2 = xc->max_columns - xc->c0 - xc->c1;
	} else {
	    xc->c2 = 0;
	}
	xl->max_cur = xl->max_pos - xl->vst;
	if (xl->max_cur > xc->c0) {
	    width = xc->c0 * FontWidth(xl);
	} else {
	    width = xl->max_cur * FontWidth(xl);
	}
	if (width == 0) width = 1;
	XMoveResizeWindow(dpy, xl->wp[0], PreeditSpotX(xc), PreeditSpotY(xc),
			  width, FontHeight(xl));
	XMoveWindow(dpy, xl->w[0], -(xl->vst * FontWidth(xl)), 0);
	XFlush(dpy);
    }
    if (xl->visible_line == 0) xl->visible_line = 1;

    XMapWindow(dpy, xl->wp[0]);
    XRaiseWindow(dpy, xl->wp[0]);
    XFlush(dpy);
    if (xc->c1 && (xl->max_pos >= xc->c0) && (xl->buf[xc->c0] != 0)) {
	if (check_mb(xl->buf, (xc->c0 - 1)) == 1) {
	    xl->linefeed[1] = 1;
	} else {
	    xl->linefeed[1] = 0;
	}
	visible_line2();
	if (xl->visible_line < 2) xl->visible_line = 2;
    }
    if (xc->c2 && (xl->max_pos > (c01(xc) - 1 - xl->linefeed[1]))){
	if (check_mb(xl->buf, (c01(xc) - 1 - xl->linefeed[1])) == 1) {
	    xl->linefeed[2] = xl->linefeed[1] + 1;
	} else {
	    xl->linefeed[2] = xl->linefeed[1];
	}
	visible_line3();
	if (xl->visible_line <3) xl->visible_line = 3;
    }
    if (xl->vst) {
	if (!xl->note[0]) {
	    JWM_note(0);
	    xl->note[0] = 1;
	}
    } else {
	if (!xl->note[1]) {
	    invisual_note(0);
	    xl->note[0] = 0;
	}
    }
    max_pos_tmp = xl->vst + xc->max_columns
      - xl->linefeed[(xl->visible_line? (xl->visible_line - 1):0)];
    if (xl->max_pos > max_pos_tmp) {
	if (!xl->note[1]) {
	    JWM_note(1);
	    xl->note[1] = 1;
	}
	if ((check_mb(xl->buf, (max_pos_tmp - 1))) == 1) {
	    JWM_note_null();
	} else {
	    XUnmapWindow(dpy, xl->wn[2]);
	}
    } else {
	if (xl->note[1]) {
	    invisual_note(1);
	    xl->note[1] = 0;
	}
	XUnmapWindow(dpy, xl->wn[2]);
    }
    xl->visible = 1;
}

#ifdef	CALLBACKS
static void
visual_window_cb()
{
    register XIMLangRec *xl;

    xl = cur_p->cur_xl;

    if (xl->visible == 0) {
        CBPreeditStart();
        if (xl->max_pos) {
            CBPreeditRedraw(xl);
        }
        xl->visible = 1;
    }
}
#endif	/* CALLBACKS */

void
visual_window()
{
    if (cur_p->cur_xl->visible) return;

    if (IsPreeditNothing(cur_x) || IsPreeditArea(cur_x)) {
	visual_window3();
    } else if (IsPreeditPosition(cur_x)) {
	visual_window1();
#ifdef	CALLBACKS
    } else if (IsPreeditCallbacks(cur_x)) {
	visual_window_cb();
#endif	/* CALLBACKS */

    }
    XFlush(dpy);
}

static void
invisual_line2()
{
    XUnmapWindow(dpy, cur_p->cur_xl->wp[1]);
    XFlush(dpy);
}

static void
invisual_line3()
{
    XUnmapWindow(dpy, cur_p->cur_xl->wp[2]);
    XFlush(dpy);
}

static void
invisual_window3()
{
    register XIMLangRec *xl;

    xl = cur_p->cur_xl;

    if (xl->note[0]) {
	XUnmapWindow(dpy, xl->wn[0]);
	xl->note[0] = 0;
    }
    if (xl->note[1]) {
	XUnmapWindow(dpy, xl->wn[1]);
	xl->note[1] = 0;
    }
    if (IsPreeditNothing(cur_x)) {
	xl->visible = 0;
    } else {
	xl->visible_line = 0;
    }
    xl->vst = 0;
    XClearWindow(dpy, xl->w[0]);
    XUnmapWindow(dpy, xl->wp[0]);
    JWwindow_move(xl);
}

void
invisual_window1()
{
    register XIMLangRec *xl;

    xl = cur_p->cur_xl;

    XUnmapWindow(dpy, xl->wp[0]);
    invisual_line2();
    invisual_line3();
    xl->visible = 0;
    xl->visible_line = 0;
    xl->linefeed[0] = xl->linefeed[1] = xl->linefeed[2] = 0;
    if (xl->note[0]) {
	XUnmapWindow(dpy, xl->wn[0]);
	xl->note[0] = 0;
    }
    if (xl->note[1]) {
	XUnmapWindow(dpy, xl->wn[1]);
	xl->note[1] = 0;
    }
    XUnmapWindow(dpy, xl->wn[2]);
}

void
invisual_window()
{
    if (!cur_p->cur_xl->visible) return;
    if (IsPreeditNothing(cur_x) || IsPreeditArea(cur_x)) {
	invisual_window3();
    } else if (IsPreeditPosition(cur_x)) {
	invisual_window1();
#ifdef	CALLBACKS
    } else if (IsPreeditCallbacks(cur_x)) {
	CBPreeditDone();
#endif	/* CALLBACKS */
    }
    cur_p->cur_xl->visible = 0;
    XFlush(dpy);
}

static void
JW0Mputwc(wc, wc_len, status)
register wchar *wc;
int wc_len;
char status;	/* 0: Preedit Window, 1: Status Window */
{
    register int cnt = wc_len;
    register XIMLangRec *xl;
    register XIMClientRec *xc = cur_p;
    register wchar *JW_buf, *p = wc;
    unsigned char *JW_att;
    int mb_len;
    short *cur_col, start_col = 0, end_col = 0;
    register unsigned char flg = 0;
    XCharStruct cs;

    if (cnt <= 0) return;

    xl = xc->cur_xl;

    if (status) {
	JW_buf = xl->buf0 + xl->currentcol0;
	JW_att = xl->att0 + xl->currentcol0;
	cs.width = StatusFontWidth(xl);
	cs.ascent = StatusFontAscent(xl);
	cs.descent = StatusFontDescent(xl);
	start_col = xl->currentcol0;
	cur_col = &xl->currentcol0;
    } else {
	visual_window();
	if (xl->currentcol >= xc->maxcolumns)
	    return;
	JW_buf = xl->buf + xl->currentcol;
	JW_att = xl->att + xl->currentcol;
	cs.width = FontWidth(xl);
	cs.ascent = FontAscent(xl);
	cs.descent = FontDescent(xl);
	start_col = xl->currentcol;
	cur_col = &xl->currentcol;
    }

    if (xl->r_flag) flg |= REV_FLAG;
    if (xl->b_flag) flg |= BOLD_FLAG;
    if (xl->u_line_flag) flg |= UNDER_FLAG;

    for (; cnt > 0; cnt--) {
	mb_len = put_pending_wchar_and_flg(*p++, JW_buf, JW_att, flg);
	JW_buf += mb_len;
	JW_att += mb_len;
	end_col += mb_len;
	if (flg & REV_FLAG) {
	    if (mb_len) {
		check_move(xc, xl, 1);
	    } else {
		check_move(xc, xl, 0);
	    }
	}
	*cur_col += mb_len;
    }
    if (status) {
	JWOutput(xl->ws, xl->st_fs,
		 ((flg & REV_FLAG)? xc->st.reversegc:  xc->st.gc),
		 start_col, end_col, flg, 0, 0, &cs, wc, wc_len);
    } else {
	JWOutput(xl->w[0], xl->pe_fs,
		 ((flg & REV_FLAG)? xc->pe.reversegc:  xc->pe.gc),
		 start_col, end_col, flg, 0, 0, &cs, wc, wc_len);
    }
}

static void
JWMputwc(wc, wc_len)
register wchar	*wc;
int		wc_len;
{
    register int cnt = wc_len;
    register XIMLangRec *xl;
    register XIMClientRec *xc = cur_p;
    register wchar *JW_buf, *p = wc;
    unsigned char *JW_att;
    Window currentW = 0;
    int mb_len;
    short col = 0;
    register unsigned char flg = 0;
    XCharStruct cs;

    if (cnt <= 0) return;

    xl = xc->cur_xl;

    if (xl->currentcol >= xc->maxcolumns) return;
    if (xl->currentcol == 0 && xl->visible == 0) visual_window1();

    if (xl->r_flag) flg |= REV_FLAG;
    if (xl->b_flag) flg |= BOLD_FLAG;
    if (xl->u_line_flag) flg |= UNDER_FLAG;

    JW_buf = xl->buf + xl->currentcol;
    JW_att = xl->att + xl->currentcol;
    cs.width = FontWidth(xl);
    cs.ascent = FontAscent(xl);
    cs.descent = FontDescent(xl);

    for (; cnt > 0; cnt--) {
	mb_len = put_pending_wchar_and_flg(*p++, JW_buf, JW_att, flg);
	JW_buf += mb_len;
	JW_att += mb_len;
	col += mb_len;
	if (mb_len > 1) {
	    if (((xl->currentcol + mb_len - xl->vst) > xc->c0)
		&& ((xl->currentcol - xl->vst) <= xc->c0)
		&& (xc->c1)) {
		xl->linefeed[1] = xc->c0 + xl->vst - xl->currentcol;
		if (xl->linefeed[1] == 1)
		    XClearArea(dpy, xl->w[0], 
			       (xc->c0 + xl->vst - 1) * FontWidth(xl), 0,
			       0, 0, False);
		visible_line2();
		if (xl->visible_line < 2) xl->visible_line = 2;
		if (((xl->currentcol + mb_len + xl->linefeed[1] - xl->vst)
		     > (xc->c0 + xc->c1))
		    && ((xl->currentcol + xl->linefeed[1] - xl->vst) <=
			(xc->c0 + xc->c1))
		    && (xc->c2)) {
		    xl->linefeed[2] = xc->c0 + xc->c1
				    + xl->vst - xl->currentcol;
		    if ((xl->linefeed[2] - xl->linefeed[1]) == 1)
			XClearArea(dpy, xl->w[1], 
				   ((xc->c0 + xc->c1 + xl->vst
				    - xl->linefeed[2]) * FontWidth(xl)),
				   0, 0, 0, False);
		    visible_line3();
		    if (xl->visible_line < 3) xl->visible_line = 3;
		}
	    }
	    if (flg & REV_FLAG) {
		check_move(xc, xl, 1);
	    }
	    if ((xl->currentcol - xl->vst) <= (xc->c0 - 2)) {
	    	currentW = xl->w[0];
	    } else if ((xl->currentcol - xl->vst)
		       <= (xc->c0 + xc->c1 - 2 - xl->linefeed[1])) {
	    	currentW = xl->w[1];
	    } else {
	    	currentW = xl->w[2];
	    }
	} else {
	    if (((xl->currentcol - xl->vst) == xc->c0) && (xc->c1)) {
		xl->linefeed[1] = 0;
		visible_line2();
		if (xl->visible_line < 2) xl->visible_line = 2;
		if (((xl->currentcol + xl->linefeed[1] - xl->vst) ==
		     (xc->c0 + xc->c1))
		    && (xc->c2)) {
		    xl->linefeed[2] = xl->linefeed[1];
		    visible_line3();
		    if (xl->visible_line < 3) xl->visible_line = 3;
		}
	    }
	    if (flg & REV_FLAG) check_move(xc, xl, 0);
	    if ((xl->currentcol - xl->vst) <= (xc->c0 - 1)) {
		currentW = xl->w[0];
	    } else if ((xl->currentcol - xl->vst) <=
			(xc->c0 + xc->c1 - 1 - xl->linefeed[1])) {
		currentW = xl->w[1];
	    } else {
		currentW = xl->w[2];
	    }
	}
        JWOutput(currentW, xl->pe_fs,
	     ((flg & REV_FLAG)? xc->pe.reversegc: xc->pe.gc), xl->currentcol,
	     col, flg, 0, 0, &cs, wc, 1);
	if ((xl->currentcol + col - xl->vst) > xl->max_cur) {
	    xl->max_cur = xl->currentcol + col - xl->vst;
	    xl->m_cur_flag = 1;
	    Resize_jw();
	}
	xl->currentcol += col;
	col = 0;
	wc++;
    }
    return;
}

void
JWMflushw_buf(w_buf, len)
w_char	*w_buf;
int	len;
{
    XIMLangRec *xl = cur_p->cur_xl;
    int wc_len;

    if (len <= 0) return;
    wc_len = 512;
    if ((wc_len = w_char_to_wchar(xl->xlc, w_buf, wc_buf, len, wc_len)) <= 0)
	return;

    if(IsPreeditNothing(cur_x) || IsPreeditArea(cur_x)) {
	(void)JW0Mputwc(wc_buf, wc_len, 0);
    } else if (IsPreeditPosition(cur_x)) {
	(void)JWMputwc(wc_buf, wc_len);
#ifdef	CALLBACKS
    } else if (IsPreeditCallbacks(cur_x)) {
	(void)CBPreeditDraw(wc_buf, wc_len);
#endif	/* CALLBACKS */
    }
    if (xl->currentcol > xl->max_pos) {
	xl->max_pos = xl->currentcol;
    }
    XFlush(dpy);
}

void
jw_disp_mode(c)
unsigned char *c;
{
    XIMLangRec *xl = cur_p->cur_xl;
    int c_len;
    int wc_len;

    c_len = strlen((char *)c);
    wc_len = 512;
    if ((wc_len = char_to_wchar(xl->xlc, c, wc_buf, c_len, wc_len)) <= 0)
	return;

    xl->currentcol0 = 0;
    if (IsStatusNothing(cur_x) || IsStatusArea(cur_x)) {
	JW0Mputwc(wc_buf, wc_len, 1);
#ifdef	CALLBACKS
    } else if (IsStatusCallbacks(cur_x)) {
	CBStatusDraw(wc_buf, wc_len);
#endif	/* CALLBACKS */
    }
    if (xl->currentcol0 > xl->max_pos0) {
	   xl->max_pos0 = xl->currentcol0;
    }
    return;
}

void
JWMline_clear(x)
register int	x;
{
    register XIMLangRec *xl;
    register int cnt;
    register wchar *JW_buf;
    unsigned char *JW_att;

    xl = cur_p->cur_xl;
    JW_buf = xl->buf + x;
    JW_att = xl->att + x;

    if (IsPreeditArea(cur_x) || IsPreeditNothing(cur_x)){
	XClearArea(dpy, xl->w[0], x * FontWidth(xl), 0,
		   (xl->max_pos - x) * FontWidth(xl) + 1, 0, False);
	if(x <= (xl->vst + cur_x->max_columns)) {
	    invisual_note(1);
	    xl->note[1] = 0;
	    XUnmapWindow(dpy, xl->wn[2]);
	}
	if (x <= 0) {
	    invisual_window();
	}
#ifdef	CALLBACKS
    } else if (IsPreeditCallbacks(cur_x)) {
        if (x <= 0) {
	    invisual_window();
        } else {
            CBPreeditClear(x, xl->max_pos);
        }
#endif	/* CALLBACKS */
    } else if (IsPreeditPosition(cur_x)) {
	xl->del_x = x;
    }
    for(cnt = x; cnt < xl->max_pos; cnt++) {
	*JW_buf++ = 0;
	*JW_att++ = 0;
    }
    xl->currentcol = x;
    xl->max_pos = x;
    XFlush(dpy);
}

void
JWMline_clear1()
{
    register XIMLangRec *xl;
    register XIMClientRec *xc = cur_p;
    register int xx;

    xl = xc->cur_xl;
    xx = xl->del_x;
    if (xl->currentcol == xl->max_pos) xx++;
    if (xc->c2 && (((xx - xl->vst) <= (c01(xc) - xl->linefeed[1]))
		   || (xl->visible_line < 3))) {
	    XUnmapWindow(dpy, xl->wp[2]);
	    xl->visible_line = 2;
    }
    if (xc->c1 && (((xx - xl->vst) <= cur_p->c0) || (xl->visible_line < 2))) {
	    XUnmapWindow(dpy, xl->wp[1]);
	    xl->visible_line = 1;
    }
    xl->m_cur_flag = 1;
    if (xl->del_x <= 0) {
	invisual_window();
	xl->visible_line = 0;
	xl->max_l1 = 0;
	xl->max_cur = 0;
	xl->m_cur_flag = 1;
    }
    xl->del_x = -1;
    xl->max_cur = xx - xl->vst;
    Resize_jw();
    XFlush(dpy);
}

void
visual_status()
{
    register XIMLangRec *xl;

    xl = cur_p->cur_xl;

#ifdef	CALLBACKS
    if (IsStatusCallbacks(cur_x)) {
	CBStatusStart();
	if (henkan_off_flag == 0) {
	    disp_mode();
	} else {
	    display_henkan_off_mode();
	}
    } else {
#endif	/* CALLBACKS */
    XMapWindow(dpy, xl->ws);
    XRaiseWindow(dpy, xl->ws);
#ifdef	CALLBACKS
    }
#endif	/* CALLBACKS */
}

void
invisual_status()
{
    register XIMLangRec *xl;

    xl = cur_p->cur_xl;

#ifdef	CALLBACKS
    if (IsStatusCallbacks(cur_x)) {
	CBStatusDone();
    } else {
#endif	/* CALLBACKS */
    XUnmapWindow(dpy, xl->ws);
#ifdef	CALLBACKS
    }
#endif	/* CALLBACKS */
}

void
JWcursor_visible()
{
    cur_p->cur_xl->cursor_flag = 1;
#ifdef	CALLBACKS
    if (IsPreeditCallbacks(cur_x)) {
        CBCursorMark(1);
    } else {
#endif	/* CALLBACKS */
    JWmark_cursor(1);
#ifdef	CALLBACKS
    }
#endif	/* CALLBACKS */
}

void
JWcursor_invisible()
{
#ifdef	CALLBACKS
    if (IsPreeditCallbacks(cur_x)) {
        CBCursorMark(0);
    } else {
#endif	/* CALLBACKS */
    JWmark_cursor(0);
#ifdef	CALLBACKS
    }
#endif	/* CALLBACKS */
    cur_p->cur_xl->cursor_flag = 0;
}

void
JWcursor_move(x)
int x;
{
    register XIMLangRec *xl;

    xl = cur_p->cur_xl;

    if ((xl->visible != 0) && (xl->max_pos > 0)) {
	visual_window();
    }
#ifdef	CALLBACKS
    if (IsPreeditCallbacks(cur_x)) {
        if (xl->currentcol != x) {
            xl->currentcol = x;
            CBCursorMove(xl->currentcol);
        }
    } else {
#endif	/* CALLBACKS */
    JWmark_cursor(0);
    xl->currentcol = x;
    if (x >= 0) JWmark_cursor(1);
#ifdef	CALLBACKS
    }
#endif	/* CALLBACKS */
    if (IsPreeditPosition(cur_x) &&
	xl->max_pos < xl->max_cur && xl->max_pos >= x) {
	xl->del_x = xl->max_pos;
    }
}

void
check_scroll()
{
    register XIMLangRec *xl = cur_p->cur_xl;
    register int mb;
    
    mb = check_mb(xl->buf, xl->currentcol);
    if ((xl->currentcol < xl->vst)
	|| ((xl->currentcol
	     + xl->linefeed[(xl->visible_line? (xl->visible_line-1):0)]
	     - xl->vst - mb)
	    >= cur_p->max_columns)) {
	if (mb == 1) {
	    check_move(cur_p, xl, 1);
	} else {
	    check_move(cur_p, xl, 0);
	}
    }
}
