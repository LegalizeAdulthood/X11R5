/*
 * $Id: keyin.c,v 1.5 1991/09/16 21:37:04 ohm Exp $
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
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "proto.h"
#include "ext.h"

static int
read_xevent(ev)
XEvent *ev;
{
    ximEventReq req;

    if (EACH_REQUEST(&req, sz_ximEventReq) == -1) return(-1);
    if (_ReadFromClient(ev, sizeof(XEvent)) == -1) return(-1);
    if (need_byteswap() == True) {
	byteswap_xevent(ev);
    }
    return(0);
}

static int
check_cur_input(w)
Window w;
{
    if (cur_input == NULL) {
#ifdef	XJPLIB
	if (XJp_check_cur_input(w) == 0) return(0);
#endif	/* XJPLIB */
	return(1);
    } else {
	if (w != cur_input->w) return(1);
	return(0);
    }
}

static int
cur_cl_set(set_window)
Window set_window;
{
    register XIMClientRec *xc = NULL;
    register XIMInputRec *xi;

    for (xi = input_list; xi != NULL; xi = xi->next) {
	if (xi->w == set_window) {
	    xc = xi->pclient;
	    break;
	}
    }
#ifdef	XJPLIB
    if (xc) {
	XJp_cur_cl_set(0);
    } else {
	xc = XJp_cur_cl_set(set_window);
    }
#endif	/* XJPLIB */
    if (xc) {
	cur_x = xc;
	if (IsPreeditNothing(cur_x)) {
	    cur_p = xim->ximclient;
	    cur_lang = cur_p->cur_xl->lang;
	    c_c = xim->ximclient->cur_xl->w_c;
	} else {
	    cur_p = cur_x;
	    cur_lang = cur_p->cur_xl->lang;
	    c_c = cur_x->cur_xl->w_c;
	}
	cur_rk = c_c->rk;
	cur_rk_table = cur_rk->rk_table;
	cur_input = xi;
	return(0);
    }
    if ((set_window == xim->ichi->w || set_window == xim->inspect->w)
	 && xim->j_c != NULL) {
	cur_p = xim->j_c;
	cur_lang = cur_p->cur_xl->lang;
	c_c = cur_p->cur_xl->w_c;
	cur_rk = c_c->rk;
	cur_rk_table = cur_rk->rk_table;
	return(0);
    }
    return(-1);
}

static	XComposeStatus compose_status = {NULL, 0};

int
key_input(buff, ev)
register int *buff;
register XEvent *ev;
{
    char strbuf[512];	
    KeySym keysym;
    int nbytes;
    int ck;
    register int i;

    if (check_cur_input(ev->xkey.window)) {
	if (cur_cl_set(ev->xkey.window) != 0) {
	    return(0);
	}
    }
    if (dpy != ev->xkey.display) ev->xkey.display = dpy;
#ifdef	CALLBACKS
    if (IsPreeditCallbacks(cur_x) && cur_x->cb_redraw_needed) {
        *buff++ = 0;
        return(1);
    }
#endif	/* CALLBACKS */
    nbytes = XLookupString ((XKeyEvent *)ev, strbuf, 512, &keysym,
			    &compose_status);
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
    XJp_save_sockbuf(nbytes, strbuf, keysym);
#endif	/* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
    if (ev->xkey.state & Mod1Mask) { /* Meta Key */
	if ((ck = cvtmeta(keysym)) == -1) {
	    return(0);
	} else {
#ifdef	XJPLIB
	    XJp_check_save_event(ev, 0);
#endif	/* XJPLIB */
	    *buff++ = ck;
	    return(1);
	}
    }
    if ((keysym & 0xff00) == 0xff00) { /* Function Key */
	if ((keysym >= XK_Multi_key && keysym <= XK_Eisuu_Lock) ||
	    (keysym >= XK_Home && keysym <= XK_F35)) {
	    if ((ck = cvtfun(keysym)) == -1) {
		return(0);
	    } else {
#ifdef	XJPLIB
		XJp_check_save_event(ev, 0);
#endif	/* XJPLIB */
		*buff++ = ck;
		return(1);
	    }
	} else {
	    if (nbytes > 0) {
#ifdef	XJPLIB
		XJp_check_save_event(ev, 1);
#endif	/* XJPLIB */
		for (i = 0; i < nbytes; i++) {
		    *buff++ = (int)strbuf[i];
		}
		return(nbytes);
	    } else {
		return(-1);
	    }
	}
    }
    if (ev->xkey.state & ControlMask) { /* Control Key */
	if (*strbuf >= 0x20) {
#ifdef	XJPLIB
	    XJp_check_send_cl_key(ev);
#endif	/* XJPLIB */
	    return(-1);
	} else if (*strbuf == 0x0) {
#ifdef	XJPLIB
	    XJp_check_save_event(ev, 0);
#endif	/* XJPLIB */
	    *buff++ = (int)*strbuf;
	    return(1);
	} else if (*strbuf <= 0x1f) {
#ifdef	XJPLIB
	    XJp_check_save_event(ev, 0);
#endif	/* XJPLIB */
	    *buff++ = (int)*strbuf;
	    return(1);
	}
    }
    if (( keysym >= 0x4a0) && (keysym <= 0x4df)){ /* Kana Key */
#ifdef	XJPLIB
	XJp_check_save_event(ev, 0);
#endif	/* XJPLIB */
	*buff++ = (int)((SS2 << 8) | (keysym & 0xff));
	return(1);
    } else if ((keysym < 0x100) && (keysym > 0)){
#ifdef	XJPLIB
	XJp_check_save_event(ev, 1);
#endif	/* XJPLIB */
	if (nbytes <= 0) {
	    nbytes = -1;
	}
	for (i = 0; i < nbytes; i++) {
	    *buff++ = (int)strbuf[i];
	}
	return(nbytes);
    }
    return(-1);
}

int
ifempty()
{
    if (cur_p->cur_xl->max_pos == 0) {
	return(1);
    } else {
	return(0);
    }
}

int
RequestDispatch()
{
    int buff[32];
    XEvent event;
    int ret = 0;
    register int i;

    switch(read_requestheader()) {
     case XIM_Event:
	if (read_xevent(&event) == -1) {
	    break;
	}
	ret = key_input(buff, &event);
	if (ret == 0) {
	    send_nofilter();
	} else if (ret > 0) {
	    for (i = 0; i < ret; i++) {
		in_put(buff[i]);
	    }
	}
	send_end();
	break;
     case XIM_GetIM:
	GetIM();
	break;
     case XIM_CreateIC:
	CreateIC();
	break;
     case XIM_ChangeIC:
	ChangeIC();
	break;
     case XIM_GetIC:
	GetIC();
	break;
     case XIM_DestroyIC:
	DestroyIC();
	break;
     case XIM_SetICFocus:
	SetICFocus();
	break;
     case XIM_UnsetICFocus:
	UnsetICFocus();
	break;
     case XIM_ResetIC:
	ResetIC();
	break;
#ifdef	SPOT
     case XIM_ChangeSpot:
	ChangeSpot();
	break;
#endif	/* SPOT */
    }
    return(0);
}

