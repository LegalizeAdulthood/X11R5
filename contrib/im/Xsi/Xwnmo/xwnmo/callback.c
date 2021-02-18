/*
 * $Id: callback.c,v 1.3 1991/09/16 21:36:29 ohm Exp $
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
#ifdef	CALLBACKS
#include <stdio.h>
#include <string.h>
#include "commonhd.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "proto.h"
#include "ext.h"

extern wchar wc_buf[];
extern char ct_buf[];

static void
send_statusdraw(str, len)
wchar *str;
int len;
{
    ximStatusDrawReply reply;
    int send_len, ct_len;

    ct_len = 512;
    if ((send_len = wchar_to_ct(cur_p->cur_xl->xlc, str, ct_buf, len, ct_len))
	 <= 0) return;
    reply.type = XIM_ST_TEXT;
    reply.encoding_is_wchar = False;
    reply.length = send_len;
    reply.feedback = 0;
    if (need_byteswap() == True) {
	byteswap_StatusDrawReply(&reply);
    }
    if (_WriteToClient(&reply, sz_ximStatusDrawReply) == -1) return;
    if (reply.length > 0) {
	if (_WriteToClient((char*)ct_buf, send_len) == -1) return;
    }
    if (_Send_Flush() == -1) return;
    return;
}

static void
send_callbackpro(mode)
int mode;
{
    ximNormalReply reply;

    reply.state = 0;
    if (need_byteswap() == True) {
	reply.detail = byteswap_s(mode);
    } else {
	reply.detail = mode;
    }
    if (_WriteToClient(&reply, sz_ximNormalReply) == -1) return;
    if (_Send_Flush() == -1) return;
    return;

}

void
CBStatusDraw(w_buf, len)
wchar *w_buf;
int len;
{
    if (len <= 0) return;
    if (cur_x->cb_st_start == 0) return;

    return_eventreply(0, XIM_CALLBACK);
    send_callbackpro(XIM_CB_ST_DRAW);
    send_statusdraw(w_buf, len);
    send_callbackpro(0);
}

void
CBStatusStart()
{
    if (cur_x->cb_st_start) return;

    return_eventreply(0, XIM_CALLBACK);
    send_callbackpro(XIM_CB_ST_START);
    send_callbackpro(0);
    if (_Send_Flush() == -1) return;
    cur_x->cb_st_start = 1;
}

void
CBStatusDone()
{
    if (cur_x->cb_st_start == 0) return;

    return_eventreply(0, XIM_CALLBACK);
    send_callbackpro(XIM_CB_ST_DONE);
    send_callbackpro(0);
    if (_Send_Flush() == -1) return;
    cur_x->cb_st_start = 0;
}

static void
send_preeditdraw(caret, first, chg_len, str, len, flg)
int caret, first, chg_len;
wchar *str;
int len;
int flg;
{
    ximPreDrawReply reply;
    unsigned short fb = 0;
    int send_len, ct_len;

    ct_len = 512;
    if ((send_len = wchar_to_ct(cur_p->cur_xl->xlc, str, ct_buf, len, ct_len))
	 <= 0) return;
    if (flg & REV_FLAG) fb |= XIMReverse;
    if (flg & UNDER_FLAG) fb |= XIMUnderline;
    if (flg & BOLD_FLAG) fb |= XIMHighlight;
    reply.caret = caret;
    reply.chg_first = first;
    reply.chg_length = chg_len;
    reply.length = send_len;
    reply.encoding_is_wchar = False;
    reply.feedback = fb;
    if (need_byteswap() == True) {
	byteswap_PreDrawReply(&reply);
    }
    if (_WriteToClient(&reply, sz_ximPreDrawReply) == -1) return;
    if (reply.length > 0) {
	if (_WriteToClient((char*)str, send_len) == -1)
	    return;
    }
    if (_Send_Flush() == -1) return;
    return;
}

void
CBPreeditClear(x)
int x;
{
    XIMLangRec *xl;
    int caret = 0, max_len = 0;
    int left;

    xl = cur_x->cur_xl;

    if (cur_x->cb_pe_start == 0) return;
    caret = XwcGetChars(xl->buf, x, &left);
    max_len = XwcGetChars(xl->buf, xl->max_pos, &left);
    if (max_len <= caret) return;
    return_eventreply(0, XIM_CALLBACK);
    send_callbackpro(XIM_CB_PRE_DRAW);
    send_preeditdraw(caret, caret, max_len - caret, (wchar*)NULL, 0, 0);
    send_callbackpro(0);
    if (_Send_Flush() == -1) return;
    return;
}

void
CBPreeditDraw(wc, wc_len)
register wchar *wc;
int wc_len;
{
    register int cnt = wc_len;
    register XIMLangRec *xl;
    register wchar *JW_buf, *p = wc;
    register unsigned char *JW_att, flg = 0;
    int caret, first, chg_len = 0, col = 0;
    int mb_len, left, save_len = 0;
    wchar	tmp[80];

    if (cnt <= 0) return;
    xl = cur_x->cur_xl;

    visual_window();
    if (xl->currentcol >= cur_x->maxcolumns)
	return;
    JW_buf = xl->buf + xl->currentcol;
    JW_att = xl->att + xl->currentcol;

    first = XwcGetChars(xl->buf, xl->currentcol, &left);

    if (xl->r_flag) flg |= REV_FLAG;
    if (xl->b_flag) flg |= BOLD_FLAG;
    if (xl->u_line_flag) flg |= UNDER_FLAG;

    if ((save_len = xl->max_pos - xl->currentcol) > 0) {
	bcopy(JW_buf, tmp, (save_len * sizeof(wchar)));
	tmp[save_len] = 0;
    }
    for (; cnt > 0; cnt--) {
	mb_len = put_pending_wchar_and_flg(*p, JW_buf, JW_att, flg);
	col += mb_len;
	JW_buf += mb_len;
	JW_att += mb_len;
    }
    xl->currentcol += col;
    if (save_len > 0) {
	chg_len = XwcGetChars(tmp, col, &left);
	if (left > 0) {
	    CBPreeditClear(xl->currentcol + col);
	}
    }
    caret = XwcGetChars(xl->buf, xl->currentcol, &left);

    return_eventreply(0, XIM_CALLBACK);
    send_callbackpro(XIM_CB_PRE_DRAW);
    send_preeditdraw(caret, first, chg_len, wc, wc_len, flg);
    send_callbackpro(0);
    if (_Send_Flush() == -1) return;
}

void
CBPreeditStart()
{
    if (cur_x->cb_pe_start) return;
    cur_x->cur_xl->visible = 1;
    return_eventreply(0, XIM_CALLBACK);
    send_callbackpro(XIM_CB_PRE_START);
    send_callbackpro(0);
    if (_Send_Flush() == -1) return;
    cur_x->cb_pe_start = 1;
}

void
CBPreeditDone()
{
    if (cur_x->cb_pe_start == 0) return;
    cur_x->cur_xl->visible = 0;
    return_eventreply(0, XIM_CALLBACK);
    send_callbackpro(XIM_CB_PRE_DONE);
    send_callbackpro(0);
    if (_Send_Flush() == -1) return;
    cur_x->cb_pe_start = 0;
}

void
CBPreeditRedraw(xl)
register XIMLangRec *xl;
{
    register wchar *JW_buf, *wc;
    register unsigned char *JW_att, old_JW_att;
    register int currentcol = 0;
    int caret = 0, first = 0;
    int wc_len = 0, mb_len = 0;
    int left;

    if (xl->currentcol >= cur_x->maxcolumns || xl->max_pos <= 0)
	return;
    visual_window();
    CBPreeditClear(0);

    return_eventreply(0, XIM_CALLBACK);
    send_callbackpro(XIM_CB_PRE_DRAW);

    xl = cur_p->cur_xl;

    JW_buf = xl->buf;
    JW_att = xl->att;
    old_JW_att = *JW_att;
    wc = wc_buf;
    skip_pending_wchar(wc, JW_buf);
    wc_len = 0;
    caret = 0;
    first = 0;
    currentcol = 0;
    for (currentcol = 0; currentcol < xl->max_pos;) {
	JW_buf = xl->buf + currentcol;
	JW_att = xl->att + currentcol;
	if ((mb_len = get_columns_wchar(JW_buf)) > 0) {
	    JW_buf += (mb_len - 1);
	    JW_att += (mb_len - 1);
	}
	if ((*JW_att != old_JW_att) && (wc_len > 0)) {
	    caret = XwcGetChars(xl->buf, currentcol, &left);
	    send_preeditdraw(caret, first, 0, wc, wc_len, old_JW_att);
	    old_JW_att = *JW_att;
	    wc += wc_len;
	    wc_len = 0;
	    first = caret;
	}
	wc_len++;
	currentcol += mb_len;
    }

    caret = XwcGetChars(xl->buf, currentcol, &left);
    send_preeditdraw(caret, first, 0, wc, wc_len, old_JW_att);
    send_callbackpro(0);
    if (_Send_Flush() == -1) return;
    return;
}

static void
send_preeditcaret(flg, pos)
int flg;
int pos;
{
    ximPreCaretReply reply;

    if (flg) {
	reply.style = XIMIsPrimary;
    } else {
	reply.style = XIMIsInvisible;
    }
    reply.position = pos;
    reply.direction = XIM_CB_DONT_CHANGE;
    if (need_byteswap() == True) {
	byteswap_PreCaretReply(&reply);
    }
    if (_WriteToClient(&reply, sz_ximPreCaretReply) == -1) return;
    if (_Send_Flush() == -1) return;
    return;
}

void
CBCursorMove(x)
int x;
{
    XIMLangRec *xl;
    int pos = 0;
    int left;

    xl = cur_x->cur_xl;

    if (cur_x->cb_pe_start == 0) return;
    return_eventreply(0, XIM_CALLBACK);
    send_callbackpro(XIM_CB_PRE_CARET);
    pos = XwcGetChars(xl->buf, x, &left);
    send_preeditcaret(xl->mark_flag, pos);
    send_callbackpro(0);
    if (_Send_Flush() == -1) return;
    return;
}

void
CBCursorMark(flg)
int flg;
{
    XIMLangRec *xl;
    int pos = 0;
    int left;

    xl = cur_x->cur_xl;

    if (cur_x->cb_pe_start == 0) return;
    if (xl->cursor_flag == 1 && xl->mark_flag != flg){
	xl->mark_flag = flg;
	return_eventreply(0, XIM_CALLBACK);
	send_callbackpro(XIM_CB_PRE_CARET);
	pos = XwcGetChars(xl->buf, xl->currentcol, &left);
	send_preeditcaret(flg, pos);
	send_callbackpro(0);
	if (_Send_Flush() == -1) return;
	return;
    }
}

void
SendCBRedraw()
{
    XEvent ev;

    ev.type = KeyPress;
    ev.xkey.window = cur_x->focus_window;
    ev.xkey.root = root_window;
    ev.xkey.state = 0;
    ev.xkey.keycode = cur_x->max_keycode + 2;
    XSendEvent(dpy, ev.xkey.window, False, NoEventMask, &ev);
    XFlush(dpy);
}
#endif	/* CALLBACKS */
