/*
 * $Id: display.c,v 1.3 1991/09/16 21:35:13 ohm Exp $
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
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"

extern wchar_t dol_wchar_t;

wchar	cursor_wchar = 0x20202020;
wchar	wc_buf[512];
wchar	ct_buf[512];
GC currentGC;

static void
JWwindow_move(text)
register JutilTextRec	*text;
{

	if ((check_mb(text->buf, text->vst)) == 2)
		text->vst++;
	XMoveWindow(dpy, text->w, -(text->vst * FontWidth), 0);
	XFlush(dpy);
}

static void
JWM_note(which)
int which;
{
    if (!cur_text->note[which]) {
	XMapWindow(dpy, cur_text->wn[which]);
	XRaiseWindow(dpy, cur_text->wn[which]);
	XwcDrawImageString(dpy, cur_text->wn[which], cur_fs->fs, xjutil->gc,
			   0, FontAscent, &dol_wchar_t, 1);
	XFlush(dpy);
    }
}

static void
invisual_note(which)
int which;
{
    if (cur_text->note[which]) {
	XUnmapWindow(dpy, cur_text->wn[which]);
	XFlush(dpy);
    }
}

static void
check_move(text,mb)
register JutilTextRec	*text;
int	mb;
{
    if ((text->currentcol - text->vst + mb) >= text->max_columns) {
	text->vst = text->currentcol - text->max_columns + mb + 1;
	JWwindow_move(text);
    } else if (text->currentcol < text->vst) {
	text->vst = text->currentcol;
	JWwindow_move(text);
    }
    if (text->vst) {
	JWM_note(0);
	text->note[0] = 1;
    } else {
	invisual_note(0);
	text->note[0] = 0;
    }
    if (text->max_pos > (text->vst + text->max_columns)) {
	JWM_note(1);
	text->note[1] = 1;
	if ((check_mb(text->buf, text->vst + text->max_columns - 1)) == 1)
		XMapWindow(dpy, text->wn[2]);
	else
		XUnmapWindow(dpy, text->wn[2]);
    } else {
	invisual_note(1);
	text->note[1] = 0;
	XUnmapWindow(dpy, text->wn[2]);
    }
}

static void
visual_window()
{
    XMapWindow(dpy, cur_text->wp);
    XMapWindow(dpy, cur_text->w);
    XFlush(dpy);
}

void
JWmark_cursor(on)
int on;
{
    register JutilTextRec *text;
    wchar *JW_buf;
    unsigned char *JW_att;
    int mb_len;
    char flg;
    XCharStruct cs;

    text = cur_text;
    JW_buf = text->buf + text->currentcol;
    JW_att = text->att + text->currentcol;
    visual_window();
    cs.width = FontWidth;
    cs.ascent = FontAscent;
    cs.descent = FontDescent;

    if(text->cursor_flag != 1 || text->mark_flag == on) return;
    text->mark_flag = on;

    if ((*JW_att & REV_FLAG) != 0) return;

    if (*JW_buf) {
	if (*JW_buf == PENDING_WCHAR) {
	    JW_buf++;
	    JW_att++;
	}
	mb_len = XwcGetColumn(*JW_buf);
	flg = *JW_att;
    } else {
	JW_buf = &cursor_wchar;
	JW_att = 0;
	mb_len = 1;
	flg = 0;
    }
    if (on) {
	if (mb_len > 1) {
	    check_move(text, 1);
	} else {
	    check_move(text, 0);
	}
	currentGC = xjutil->reversegc;
	flg |= REV_FLAG;
    } else {
	currentGC = xjutil->gc;
    }
    JWOutput(text->w, cur_fs->fs, currentGC, text->currentcol, mb_len,
	     flg, 0, 0, &cs, JW_buf, 1);
    XFlush(dpy);
}

void
JW1Mputwc(wc, wc_len)
register wchar *wc;
int wc_len;
{
    register int cnt = wc_len;
    register JutilTextRec *text;
    register wchar *JW_buf, *p = wc;
    unsigned char *JW_att;
    int mb_len;
    short col = 0;
    register int flg = 0;
    XCharStruct cs;

    text = cur_text;
    visual_window();
    if (text->currentcol >= text->maxcolumns)
	return;
    JW_buf = text->buf + text->currentcol;
    JW_att = text->att + text->currentcol;
    if (text->r_flag) flg |= REV_FLAG;
    if (text->b_flag) flg |= BOLD_FLAG;
    if (text->u_line_flag) flg |= UNDER_FLAG;

    cs.width = FontWidth;
    cs.ascent = FontAscent;
    cs.descent = FontDescent;

    for (; cnt > 0; cnt--, JW_buf++, JW_att++) {
	mb_len = XwcGetColumn(*p);
	if (mb_len > 1) {
	    *(JW_buf++) = PENDING_WCHAR;
	    *(JW_att++) = flg;
	}
	*JW_buf = *p++;
	*JW_att = flg;
	col += mb_len;
	if (flg & REV_FLAG) {
	    if (mb_len) {
		check_move(text, 1);
	    } else {
		check_move(text, 0);
	    }
	}
    }
    JWOutput(text->w, cur_fs->fs,
	     ((flg & REV_FLAG)? xjutil->reversegc: xjutil->gc),
	     text->currentcol, col, flg, 0, 0, &cs, wc, wc_len);
    text->currentcol += col;
}

void
JWMflushw_buf(w_buf, len)
w_char	*w_buf;
int	len;
{
    int wc_len;

    wc_len = 512;
    if ((wc_len = w_char_to_wchar(xjutil->xlc, w_buf, wc_buf, len, wc_len))
	<= 0)
	return;
    JW1Mputwc(wc_buf, wc_len);
    if (cur_text->currentcol > cur_text->max_pos) {
	cur_text->max_pos = cur_text->currentcol;
    }
    XFlush(dpy);
}

void
jw_disp_mode(mode)
unsigned char *mode;
{
    if (xjutil->mode & JUTIL_MODE) {
	xjutil->jutil->rk_mode->string = mode;
	redraw_box(xjutil->jutil->rk_mode);
    } else if (xjutil->mode & ICHIRAN_MODE) {
	xjutil->ichi->rk_mode->string = mode;
	redraw_box(xjutil->ichi->rk_mode);
    }
    return;
}

void
JWMline_clear(x)
register int	x;
{
    register JutilTextRec *text;
    register int cnt;
    register wchar *JW_buf;
    unsigned char *JW_att;

    text = cur_text;
    JW_buf = text->buf + x;
    JW_att = text->att + x;
    XClearArea(dpy, text->w, x * FontWidth, 0,
	       (text->max_pos - x) * FontWidth + 1, 0, False);
    if(x <= (text->vst + text->max_columns)) {
	invisual_note(1);
	text->note[1] = 0;
	XUnmapWindow(dpy, text->wn[2]);
    }
    for(cnt = x; cnt < text->max_pos; cnt++) {
	*JW_buf++ = 0;
	*JW_att++ = 0;
    }
    text->currentcol = x;
    text->max_pos = x;
    XFlush(dpy);
}

void
JWcursor_visible()
{
    cur_text->cursor_flag = 1;
    JWmark_cursor(1);
}

void
JWcursor_invisible()
{
    JWmark_cursor(0);
    cur_text->cursor_flag = 0;
}

void
JWcursor_move(x)
int x;
{
    visual_window();
    JWmark_cursor(0);
    cur_text->currentcol = x;
    if (x >= 0)
	JWmark_cursor(1);
}

void
redraw_text(tx, x, width)
register JutilTextRec *tx;
int x, width;
{
    register wchar *JW_buf, *old_JW_buf, *wc;
    unsigned char *JW_att;
    unsigned char old_JW_att;
    int currentcol_tmp = 0;
    int start, end, startcol;
    int wc_len, mb_len;
    int col;
    XCharStruct cs;

    currentcol_tmp = tx->currentcol;
    start = x / FontWidth;
    end = start + width / FontWidth + ((width % FontWidth) ? 1 : 0);
    if (end > tx->max_pos) end = tx->max_pos;
    if (check_mb(tx->buf, start) == 2) start--;
    if (check_mb(tx->buf, (end - 1)) == 1) end++;

    old_JW_buf = JW_buf = tx->buf + start;
    JW_att = tx->att + start;
    old_JW_att = *JW_att;
    wc = wc_buf;
    skip_pending_wchar(wc, JW_buf);
    wc_len = 0;
    col = 0;
    startcol = start;
    cs.width = FontWidth;
    cs.ascent = FontAscent;
    cs.descent = FontDescent;

    for (tx->currentcol = start; tx->currentcol < end;) {
	JW_buf = tx->buf + tx->currentcol;
	JW_att = tx->att + tx->currentcol;
	if (*JW_buf == PENDING_WCHAR) {
	    JW_buf++;
	    JW_att++;
	}
	mb_len = XwcGetColumn(*JW_buf);
	if ((*JW_att != old_JW_att) && (wc_len > 0)) {
	    if (old_JW_att & REV_FLAG) {
		currentGC = xjutil->reversegc;
	    } else {
		currentGC = xjutil->gc;
	    }
	    JWOutput(tx->w, cur_fs->fs, currentGC, startcol, col,
		     old_JW_att, 0, 0, &cs, wc, wc_len);
	    old_JW_buf = JW_buf;
	    old_JW_att = *JW_att;
	    wc += wc_len;
	    wc_len = 0;
	    col = 0;
	    startcol = tx->currentcol;
	}
	if (*JW_att & REV_FLAG) {    
	    if (mb_len) {
		check_move(tx, 1);
	    } else {
		check_move(tx, 0);
	    }
	}
	wc_len++;
	col += mb_len;
	tx->currentcol += mb_len;
    }
    if (wc_len > 0) {
	if (old_JW_att & REV_FLAG) {
	    currentGC = xjutil->reversegc;
	} else {
	    currentGC = xjutil->gc;
	}
	JWOutput(tx->w, cur_fs->fs, currentGC, startcol, col,
		 old_JW_att, 0, 0, &cs, wc, wc_len);
    }
    tx->currentcol = currentcol_tmp;
    if(tx->cursor_flag == 1 && tx->mark_flag == 1) {
	tx->mark_flag = 0;
	JWmark_cursor(1);
    }
}

void
redraw_note(tx, which)
register JutilTextRec *tx;
int which;
{
    XwcDrawImageString(dpy, tx->wn[which], cur_fs->fs, xjutil->gc,
		       0, FontAscent, &dol_wchar_t, 1);
    XFlush(dpy);
}

void
check_scroll()
{
    int	mb;

    mb = check_mb(cur_text->buf, cur_text->currentcol);
    if ((cur_text->currentcol < cur_text->vst) ||
	((cur_text->currentcol - cur_text->vst - mb)
	 >= cur_text->max_columns)) {
	if (mb == 1)
	    check_move(cur_text, 1);
	else
	    check_move(cur_text, 0);
    }
}
