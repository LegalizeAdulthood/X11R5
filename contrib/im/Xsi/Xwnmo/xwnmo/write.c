/*
 * $Id: write.c,v 1.4 1991/09/16 21:37:32 ohm Exp $
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
#include "proto.h"
#include "ext.h"

extern wchar wc_buf[512];
extern char ct_buf[512];

static int
send_returnend()
{
    ximReturnReply reply;

    reply.type = 0;
    reply.length = 0;
    reply.keysym = 0;
    if (need_byteswap() == True) {
	byteswap_ReturnReply(&reply);
    }
    if (_WriteToClient(&reply, sz_ximReturnReply) == -1) return(-1);
    return(0);
}

static int
send_ct(ct, ct_len)
register char *ct;
register int ct_len;
{
    ximReturnReply reply;

    reply.type = XIM_STRING;
    reply.length = ct_len;
    reply.keysym = 0;
    if (need_byteswap() == True) {
	byteswap_ReturnReply(&reply);
    }
    if (_WriteToClient(&reply, sz_ximReturnReply) == -1) return(-1);
    if (reply.length > 0) {
	if (_WriteToClient(ct, reply.length) == -1) return(-1);
    }
    return(0);
}

static int
send_keysym(keysym, str)
KeySym keysym;
char str;
{
    ximReturnReply reply;
    char buf[2];

    reply.type = XIM_KEYSYM;
    if (keysym < 0x20) {
	keysym |= 0xff00;
	reply.length = 0;
    } else {
	reply.length = 1;
	buf[0] = str & 0xff;
	buf[1] = '\0';
    }
    reply.keysym = keysym;
    if (need_byteswap() == True) {
	byteswap_ReturnReply(&reply);
    }
    if (_WriteToClient(&reply, sz_ximReturnReply) == -1) return(-1);
    if (reply.length > 0) {
	if (_WriteToClient(buf, reply.length) == -1) return(-1);
    }
    return(0);
}

int
return_eventreply(state, mode, num)
short state;
short mode;
short num;
{
    ximEventReply reply;

    reply.state = state;
    reply.detail = mode;
    reply.number = num;
    if (need_byteswap() == True) {
	byteswap_EventReply(&reply);
    }
    if (_WriteToClient(&reply, sz_ximEventReply) == -1) return(-1);
    return(0);
}

int
send_nofilter()
{
    if (return_eventreply(0, XIM_NOFILTER, 0) == -1) return(-1);
    return(0);
}

static int
send_ch_locale(locale)
char *locale;
{
    return_eventreply(0, XIM_CH_LOCALE, strlen(locale));
    if (_WriteToClient(locale, strlen(locale)) == -1) return(-1);
    return(0);
}

int
send_end()
{
    if (cur_p->ch_lc_flg) {
	send_ch_locale(cur_p->cur_xl->cur_lc->lc_name);
	cur_p->ch_lc_flg = 0;
    }
    if (return_eventreply(0, 0, 0) == -1) return(-1);
    if (_Send_Flush() == -1) return(-1);
    return(0);
}

int
return_cl_it()
{
#ifdef	XJPLIB
    if (cur_x->xjp) {
	XJp_return_cl_it();
	return(0);
    }
#endif	/* XJPLIB */
    if (cur_input != 0) {
	if (send_nofilter() == -1) return(-1);
    }
    return(0);
}

void
xw_write(w_buf, size)
register w_char	*w_buf;
register int	size;
{
    register XIMLangRec *xl;
    w_char w_c;
    w_char *start;
    register int cnt;
    int ct_len, conv_len, send_len;
    KeySym keysym;

    if (!size) {
	return;
    }
#ifdef	XJPLIB
    if (cur_x->xjp) {
	XJp_xw_write(w_buf, size);
	return;
    }
#endif	/* XJPLIB */

    if (henkan_off_flag) {
	return_cl_it();
	return;
    }
    return_eventreply(0, XIM_RETURN, 0);

    xl = cur_p->cur_xl;

    if (IsPreeditPosition(cur_x) &&
	cur_p->cur_xl->del_x >= cur_p->cur_xl->max_pos) {
	JWMline_clear1();
    }

    ct_len = 512;
    conv_len = 0;
    start = w_buf;
    for (cnt = 0; cnt < size; cnt++, w_buf++) {
	if (!((w_c = *w_buf) & 0xff80)) {
	    if (conv_len) {
		if ((send_len = w_char_to_ct(xl->xlc, start, ct_buf, conv_len,
					     ct_len)) > 0) {
		    send_ct(ct_buf, send_len);
		}
		conv_len = 0;
	    }
	    keysym = (int)w_c;
	    send_keysym(keysym, w_c);
	} else {
	    if (conv_len == 0) {
		start = w_buf;
	    }
	    conv_len++;
	}
    }
    if (conv_len) {
	if ((send_len = w_char_to_ct(xl->xlc, start, ct_buf, conv_len,
				     ct_len)) > 0) {
	    send_ct(ct_buf, send_len);
	}
	conv_len = 0;
    }
    send_returnend();
    return;
}
