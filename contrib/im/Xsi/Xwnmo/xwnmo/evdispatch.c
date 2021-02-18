/*
 * $Id: evdispatch.c,v 1.5 1991/09/16 21:36:46 ohm Exp $
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
#include <signal.h>
#include "commonhd.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"

static int
motif_event(ev)
XClientMessageEvent *ev;
{
    extern int kill(), getpid();

    if (ev->window == xim->ximclient->w) {
	kill(getpid(), SIGTERM);
    } else if (ev->window == xim->ichi->w || ev->window == xim->inspect->w) {
	xim->sel_ret = -2;
	cur_cl_change3(xim->j_c);
	return(0);
    }
    return(1);
}


static	char	*syuuryou_title = "X INPUT MANAGER";
static	char	*syuuryou_syuuryou = " EXIT ";
static void
syuuryou_menu(in)
int in;
{
    static char *buf[1];
    int	c;

    cur_x = xim->ximclient;
    cur_p = cur_x;
    cur_lang = cur_p->cur_xl->lang;
    c_c = cur_p->cur_xl->w_c;
    cur_rk = c_c->rk;
    cur_rk_table = cur_rk->rk_table;
    cur_input = 0;
    xim->exit_menu = 1;
    buf[0] = syuuryou_syuuryou;
    c = xw_select_one_element(buf, 1, -1, -1, syuuryou_title, SENTAKU,
			      main_table[4], in);
    if (c == 0) {
	kill(getpid(),SIGTERM);
    } else if (c == BUFFER_IN_CONT) {
	return;
    }
    xim->exit_menu = 0;
    return;
}


static void
search_expose(event)
register XExposeEvent *event;
{
    register XIMClientRec *xc;
    register XIMLangRec *xl;
    register BoxRec *p;
    register Window window = event->window;

    for (xc = ximclient_list; xc != NULL; xc = xc->next) {
	if (IsPreeditArea(xc) || IsPreeditPosition(xc)
	 /* || IsPreeditNothing(xc)*/){
	    cur_p = cur_x = xc;
	    xl = cur_p->cur_xl;
	    cur_lang = xl->lang;
	    if (window == xl->w[0]) {
		if (IsPreeditArea(cur_x)) {
		    redraw_window3(event->x, event->width);
		    return;
		} else if (IsPreeditPosition(cur_x)) {
		    redraw_lines(event->x, event->width, 1);
		    return;
		}
	    } else if (window == xl->w[1]) {
		redraw_lines(event->x, event->width, 2);
		return;
	    } else if (window == xl->w[2]) {
		redraw_lines(event->x, event->width, 3);
		return;
	    } else if (window == xl->wn[0]) {
		redraw_note(0);
		return;
	    } else if (window == xl->wn[1]) {
		redraw_note(1);
		return;
	    }
	}

	if (IsStatusArea(xc) || IsStatusNothing(xc)) {
	    if (window == cur_p->cur_xl->ws) {
		redraw_window0();
		return;
	    }
	}
    }
    for (p = box_list; p != NULL; p = p->next) {
	if (window == p->window) {
	    if (p->redraw_cb) (p->redraw_cb)(p->redraw_cb_data);
	    redraw_box(p);
	    return;
	}
    }
    if (xim->j_c) {
	cur_cl_change3(xim->j_c);
	if (window == xim->ichi->w1) {
	    redraw_ichi_w();
	    return;
	} else if (window == xim->ichi->nyuu_w) {
	    JW3Mputc(xim->ichi->nyuu, xim->ichi->nyuu_w, 0, 1, 0);
	    return;
	}
	if (window == xim->inspect->w1) {
	    JW3Mputc(xim->inspect->msg, xim->inspect->w1, 0, 0, IN_BORDER);
	    return;
	}
    }
    return;
}

static void
xw_expose(event)
register XExposeEvent *event;
{
    register XIMClientRec *cur_x_sv, *cur_p_sv;
    register char *cur_lang_sv;

    cur_x_sv = cur_x;
    cur_p_sv = cur_p;
    cur_lang_sv = cur_lang;
    search_expose(event);
    cur_p = cur_p_sv;
    cur_x = cur_x_sv;
    cur_lang = cur_lang_sv;
}

static Status
xw_buttonpress(event)
XButtonEvent *event;
{
    register XIMClientRec *xc;
    register BoxRec *p;
    register Window window = event->window;

    xim->sel_ret = -1;
    for (xc = ximclient_list; xc != NULL; xc = xc->next) {
	if (window == xc->cur_xl->ws) {
	    if (xim->j_c == NULL) {
		xim->sel_button = 1;
		cur_cl_change3(xc);
		if (event->button == Button1) {
		  jutil_c();
		} else {
		  /*if (ifempty())*/ lang_c();
		}
	    }
	    return(False);
	}
    }

    for (p = box_list; p != NULL; p = p->next) {
	if (window == p->window) {
	    if (p->freeze) return(False);
	    xim->sel_ret = p->sel_ret;
	    if (p->cb) (*p->cb)(p->cb_data);
	    return(p->do_ret);
	}
    }
    if (xim->j_c) {
	if (window == xim->ichi->w1) {
	    xim->sel_button = 1;
	    if (xim->ichi->mode == SENTAKU) {
		xw_select_button(event);
	    }else {
		xw_select_jikouho_button(event);
	    }
	    cur_cl_change3(xim->j_c);
	    return(True);
	}
    }
    return(False);
}

static void
xw_destroy(event)
register XDestroyWindowEvent *event;
{
    register XIMClientRec *xc;

    for (xc = ximclient_list; xc != NULL; xc = xc->next) {
	if (event->window == xc->w) {
	    destroy_client(xc);
	    return;
	}
    }
#ifdef	XJPLIB
    XJp_xw_destroy(event);
#endif	/* XJPLIB */
}

static void
xw_mousemove(event)
XMotionEvent *event;
{
    if (xim->j_c) {
	if (xim->ichi->mode == SENTAKU) {
	    xw_move_hilite(event->y);
	} else {
	    xw_jikouho_move_hilite(event->x, event->y);
	}
    }
}

static void
xw_enterleave(event, el)
XCrossingEvent *event;
char el;
{
    register BoxRec *p;
    register Window window = event->window;

    for (p = box_list; p != NULL; p = p->next) {
	if (window == p->window) {
	    if (!p->freeze && p->reverse && el != p->in) {
		/*
		reverse_box(p, xim->ichi->invertgc);
		*/
		reverse_box(p, p->invertgc);
		p->in = el;
	    }
	    return;
	}
    }
    if (xim->j_c) {
	if (window == xim->ichi->w1) {
	    if (el) {
		xw_mousemove(event);
	    } else {
		if (xim->ichi->mode == SENTAKU) xw_mouseleave();
	    }
	    return;
	}
    }
    return;
}

static void
xw_reparent(event)
XReparentEvent *event;
{
/*
    register XIMClientRec *xc;

    for (xc = ximclient_list; xc != NULL; xc = xc->next) {
	if (xc->xl[0] && (xc->lang_num > 1)) {
	    if (event->window == xc->xl[0]->wp[0]) {
		reparent_preedit(xc, event->parent);
		return;
	    }
	    if (event->window == xc->xl[0]->ws) {
		reparent_status(xc, event->parent);
		return;
	    }
	}
    }
*/
    read_wm_id();
}

static void
xw_configure(ev)
XConfigureEvent *ev;
{
    register XIMClientRec *xc;

    for (xc = ximclient_list; xc != NULL; xc = xc->next) {
	if (xc->w == ev->window) {
	    change_client_area(xc, ev->x, ev->y, ev->width, ev->height);
	    return;
	} else if (xc->focus_window == ev->window) {
	    change_focus_area(xc, ev->x, ev->y, ev->width, ev->height);
	    return;
	}
    }
}

void
X_flush()
{
    if (cur_x && cur_p && IsPreeditPosition(cur_x) &&
	cur_p->cur_xl->del_x >= cur_p->cur_xl->max_pos) {
	JWMline_clear1();
    }
    XFlush(dpy);
}

static Bool
dummy_proc(display, ev, arg)
Display *display;
XEvent *ev;
char *arg;
{
    return(True);
}

void
XEventDispatch()
{
    int buff[32];
    XEvent event;
    int ret = 0;
    register int i;
    extern Atom wm_id;
    extern Atom wm_id1;

/*
    while (XPending(dpy) > 0) {
*/
    do {
/*
	XNextEvent(dpy, &event);
	if (XCheckMaskEvent(dpy, ~NoEventMask, &event) == False) return;
*/
	if (XCheckIfEvent(dpy, &event, dummy_proc, 0) == False) return;
	ret = 0;
	switch (event.type) {
	 case KeyPress:
	    ret = key_input(buff, &event);
	    break;
	 case Expose:
	    xw_expose(&event);
	    break;
	 case ButtonRelease:
	    button.x = event.xbutton.x_root;
	    button.y = event.xbutton.y_root;
	    if (event.xbutton.window == xim->ximclient->w) {
		xim->sel_button = 1;
		syuuryou_menu();
	    } else {
		if(xw_buttonpress(&event) == True) {
		    if (xim->exit_menu &&
			((event.xbutton.window
			  == xim->ichi->button[CANCEL_W]->window)
			 || (event.xbutton.window == xim->ichi->w1))) {
			syuuryou_menu();
		    } else {
			buff[0] = 0xd;
			ret = 1;
		    }
		}
	    }
	    break;
	 case DestroyNotify:
	    xw_destroy(&event);
	    break;
	 case MotionNotify:
	    xw_mousemove(&event);
	    break;
	 case EnterNotify:
	    xw_enterleave(&event, 1);
	    break;
	 case LeaveNotify:
	    xw_enterleave(&event, 0);
	    break;
	 case ReparentNotify:
	    xw_reparent(&event);
	    break;
	 case MapNotify:
	    check_map(event.xmap.window);
	    break;
	 case ConfigureNotify:
	    xw_configure(&event);
	    break;
	 case ClientMessage:
	    if (wm_id && (event.xclient.message_type == wm_id)
		&& (event.xclient.data.l[0] == wm_id1)) {
		motif_event(&event);
		break;
	    }
#ifdef	USING_XJUTIL
	    if (event.xclient.data.l[0] == DIC_START) {
		set_xjutil_id(event.xclient.data.l[1],
			       event.xclient.data.l[2], 0);
	    } else if (event.xclient.data.l[0] == DIC_START_ER) {
		set_xjutil_id(event.xclient.data.l[1],
			       event.xclient.data.l[2], 1);
	    } else if (event.xclient.data.l[0] == DIC_FONTSET_ER) {
		reset_xjutil_fs_id(event.xclient.data.l[1],
				   event.xclient.data.l[2]);
	    }
#endif	/* USING_XJUTIL */
#ifdef	XJPLIB
	    else {
		XJp_event_dispatch(&event);
	    }
#endif
	    break;
	 default:
	    break;
	}
	for (i = 0; i < ret; i++) {
	    in_put(buff[i]);
	}
/*
    }
*/
    } while (XPending(dpy) > 0);
}

int
get_xevent()
{
    int buff[32];
    XEvent event;
    extern Atom wm_id;
    extern Atom wm_id1;

    while(1) {
	XNextEvent(dpy, &event);
	switch (event.type) {
	 case KeyPress:
	    key_input(buff, &event);
	    xim->sel_ret = 1;
	    return(0);
	 case Expose:
	    xw_expose(&event);
	    return(-1);
	    break;
	 case ButtonRelease:
	    button.x = event.xbutton.x_root;
	    button.y = event.xbutton.y_root;
	    if (event.xbutton.window == xim->ximclient->w) {
		xim->sel_button = 1;
		syuuryou_menu();
	    } else {
		if(xw_buttonpress(&event) == True) {
		    if (xim->exit_menu &&
			((event.xbutton.window
			  == xim->ichi->button[CANCEL_W]->window)
			 || (event.xbutton.window == xim->ichi->w1))) {
			syuuryou_menu();
		    } else {
			buff[0] = 0xd;
			return(1);
		    }
		}
	    }
	    break;
	 case DestroyNotify:
	    xw_destroy(&event);
	    break;
	 case MotionNotify:
	    xw_mousemove(&event);
	    break;
	 case EnterNotify:
	    xw_enterleave(&event, 1);
	    break;
	 case LeaveNotify:
	    xw_enterleave(&event, 0);
	    break;
	 case ReparentNotify:
	    xw_reparent(&event);
	    break;
	 case MapNotify:
	    check_map(event.xmap.window);
	    break;
	 case ClientMessage:
	    if (wm_id && (event.xclient.message_type == wm_id)
		&& (event.xclient.data.l[0] == wm_id1)) {
		motif_event(&event);
		break;
	    }
#ifdef	USING_XJUTIL
	    if (event.xclient.data.l[0] == DIC_START) {
		set_xjutil_id(event.xclient.data.l[1],
			      event.xclient.data.l[2], 0);
	    } else if (event.xclient.data.l[0] == DIC_START_ER) {
		set_xjutil_id(event.xclient.data.l[1],
			      event.xclient.data.l[2], 1);
	    }
#endif	/* USING_XJUTIL */
	    break;
	 default:
	    break;
	}
    }
}
