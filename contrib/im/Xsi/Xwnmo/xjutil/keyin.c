/*
 * $Id: keyin.c,v 1.3 1991/09/18 06:29:36 xi18n Exp $
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
#include "config.h"
#include "xext.h"

XPoint	button;
XGCValues     xgcv;
Atom	actual_type;
int	actual_format;
unsigned long	nitems;
unsigned long	leftover;

extern Atom	wm_id;
extern Atom	wm_id1;

static void
change_fore()
{
    xgcv.foreground = xjutil->fg;
    XChangeGC(dpy, xjutil->gc, GCForeground,&xgcv);
    xgcv.background = xjutil->fg;
    XChangeGC(dpy, xjutil->reversegc, GCBackground,&xgcv);
    xgcv.foreground = xjutil->fg;
    xgcv.plane_mask = XOR(xjutil->fg, xjutil->bg);
    XChangeGC(dpy, xjutil->invertgc, (GCForeground | GCPlaneMask), &xgcv);
}

static void
change_back()
{
    xgcv.background = xjutil->bg;
    XChangeGC(dpy, xjutil->gc, GCBackground,&xgcv);
    xgcv.foreground = xjutil->bg;
    XChangeGC(dpy, xjutil->reversegc, GCForeground,&xgcv);
    xgcv.background = xjutil->bg;
    xgcv.plane_mask = XOR(xjutil->fg, xjutil->bg);
    XChangeGC(dpy, xjutil->invertgc, (GCBackground | GCPlaneMask), &xgcv);
}

static int
set_xjutil_bun(xjutil_env, p)
Xjutil_envRec *xjutil_env;
register char *p;
{
    XFontSet fs;
    XjutilFSRec *fsl;
    int i;
    WnnEnv *we;
    extern XjutilFSRec *add_fontset_list(), *get_fontset_list();
    extern XFontSet create_font_set();

    if ((fsl = get_fontset_list(xjutil_env->fs_id)) == NULL) {
	if ((xjutil_env->fn_len <= 0) || (p == NULL) || (*p == '\0')) {
	    print_out("XCreateFontSet failed");
	    return(-1);
	}
	if ((fs = create_font_set(p)) == NULL) {
	    return(-1);
	}
	cur_fs = add_fontset_list(fs, xjutil_env->fs_id);
    } else {
	cur_fs = fsl;
    }
    if (xjutil->fg != xjutil_env->fore_ground) {
    	xjutil->fg = xjutil_env->fore_ground;
	change_fore();
    }
    if (xjutil->bg != xjutil_env->back_ground) {
    	xjutil->bg = xjutil_env->back_ground;
	change_back();
    }
    for (i = 0, we = normal_env; we; i++, we = we->next)  {
	if (we->env == NULL) continue;
	we->env->env_id = xjutil_env->env_id[i];
	if (xjutil_env->env_id[i] == xjutil_env->cur_env_id)
	    cur_normal_env = we;
    }
    for (i = 0, we = reverse_env; we; i++, we = we->next)  {
	if (we->env == NULL) continue;
	we->env->env_id = xjutil_env->env_reverse_id[i];
	if (xjutil_env->env_id[i] == xjutil_env->cur_env_id)
	    cur_reverse_env = we;
    }
    return(0);
}

int
get_env()
{
    Atom atom_env;
    unsigned char *data;
    Xjutil_envRec xjutil_env;

    if ((atom_env = XInternAtom (dpy, XJUTIL_ENV_PRO, True)) == (Atom)NULL) {
	return(-1);
    }
    XGetWindowProperty(dpy, root_window, atom_env, 0L, 1000000L,
    			False,XA_STRING, &actual_type, &actual_format,
			&nitems, &leftover, &data);
    bcopy(data, &xjutil_env, sizeof(Xjutil_envRec));
    set_xjutil_bun(&xjutil_env, (data + sizeof(Xjutil_envRec)));
    Free((char *)data);
    return(0);
}

int
get_touroku_data(buffer)
w_char *buffer;
{
    Atom atom_env;
    unsigned char *data, *data_sv;
    int ret;
    Xjutil_envRec xjutil_env;

    if ((atom_env = XInternAtom (dpy, XJUTIL_ENV_PRO, True)) == (Atom)NULL) {
	return(-1);
    }
    XGetWindowProperty(dpy, root_window, atom_env, 0L, 1000000L, 
    			False,XA_STRING, &actual_type, &actual_format,
			&nitems, &leftover, &data);
    data_sv = data;
    bcopy(data, &xjutil_env, sizeof(Xjutil_envRec));
    set_xjutil_bun(&xjutil_env, (data += sizeof(Xjutil_envRec)));
    data += xjutil_env.fn_len;
    bcopy(data, &ret, sizeof(int));
    data += sizeof(int);
    bcopy(data, buffer, (nitems - (data - data_sv)));
    buffer[(nitems - (data - data_sv))/2] = 0;
    Free(data_sv);
    return(ret);
}

static int
wm_event(ev)
XClientMessageEvent *ev;
{
	if (ev->window == xjutil->ichi->w || ev->window == xjutil->jutil->w) {
		xjutil->sel_ret = -2;
		return(0);
	}
	return(1);
}

static void
check_map(w)
Window w;
{
    if (w == xjutil->ichi->w) {
	xjutil->ichi->map = 1;
	return;
    } else if (w == xjutil->ichi->keytable->w) {
	xjutil->ichi->keytable->map = 1;
	return;
    } else if (w == xjutil->yes_no->w) {
	xjutil->yes_no->map = 1;
	return;
    }
    return;
}

static	XComposeStatus compose_status = {NULL, 0};
static int work = 0;

int
xw_read(buff)
register int	*buff;
{
    XEvent event;
    char strbuf[32];	
    KeySym keycode;
    int	 nbytes;
    int	 ck;
    int	 i;

    for(;;){
	XNextEvent(dpy, &event);
	xjutil->sel_button = 0;
	switch (event.type) {
	    case KeyPress:
		nbytes = XLookupString ((XKeyEvent *)&event, strbuf, 32,
					&keycode, &compose_status);
		if (event.xkey.state & Mod1Mask) { /* 全面キー */
		    if ((ck = cvtmeta(keycode)) == -1) {
			break;
		    } else {
			*buff++ = ck;
			return(1);
		    }
		}
		if ((keycode & 0xff00) == 0xff00) { /* ファンクションキー */
		    if ((keycode >= XK_Multi_key && keycode <= XK_Eisuu_Lock) ||
			(keycode >= XK_Home && keycode <= XK_F35)) {
			if ((ck = cvtfun(keycode)) == -1) {
			    break;
			} else {
			    *buff++ = ck;
			    return(1);
			}
		    } else {
			if (nbytes > 0) {
			    for (i = 0; i < nbytes; i++) {
				*buff++ = (int)strbuf[i];
			    }
			    return(nbytes);
			} else {
			    break;
			}
		    }
		}
		if (event.xkey.state & ControlMask) { /* コントロールキー */
		    if (*strbuf >= 0x20) {
			break;
		    } else if (*strbuf == 0x0) {
			*buff++ = (int)*strbuf;
			return(1);
		    } else if (*strbuf <= 0x1f) {
			*buff++ = (int)*strbuf;
			return(1);
		    }
		}
		if (( keycode >= 0x4a0) && (keycode <= 0x4df)){ /* 仮名キー */
		    *buff++ = (int)SS2;
#ifdef	X11R3
		    *buff = (int)(keycode & 0xff);
		    return(2);
#else	/* X11R3 */
		    for (i = 0; i < nbytes; i++) {
			*buff++ = (int)strbuf[i];
		    }
		    return(nbytes + 1);
#endif	/* X11R3 */
		} else if ((keycode < 0x100) && (keycode > 0)){
		    for (i = 0; i < nbytes; i++) {
			*buff++ = (int)strbuf[i];
		    }
		    return(nbytes);
		}
		break;

	    case ClientMessage: /* 日本語ツールキットからの要求	*/
/* MOTIF */
		if (wm_id && event.xclient.message_type == wm_id
		    && event.xclient.data.l[0] == wm_id1) {
			if (wm_event(&event) == 0) {
				*buff = (int)0xd;
				return(1);
			}
			break;
		}
/* MOTIF */
		if (work) break;
		switch(event.xclient.data.l[0]) {
		    case DIC_ADD:	
			work = 1;
			jishoadd();
			work = 0;
			break;
		    case DIC_KENSAKU:
			work = 1;
			kensaku();
			work = 0;
			break;
		    case DIC_ICHIRAN:
			work = 1;
			select_one_dict9();
			work = 0;
			break;
		    case DIC_PARAM:
			work = 1;
			paramchg();
			work = 0;
			break;
		    case DIC_INFO:
			work = 1;
			dicinfoout();
			work = 0;
			break;
		    case DIC_FZK:
			work = 1;
			fuzoku_set();
			work = 0;
			break;
		    case DIC_TOUROKU:
			work = 1;
			touroku();
			work = 0;
			break;
		    default:
			break;
		}

	    case Expose:
		xw_expose(&event);
		return(-1);
		break;
	    case ButtonRelease:
		button.x = event.xbutton.x_root;
		button.y = event.xbutton.y_root;
		xjutil->sel_button = 1;
		if(xw_buttonpress(&event) == True) {
		    *buff = (int)0xd;
		    return(1);
		}
		break;
	    case DestroyNotify:
		terminate_handler();
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
		read_wm_id();
		break;
	    case MapNotify:
		check_map(event.xmap.window);
		break;
	    default:
		break;
		
	}
    }
}

