/*
 * Copyright 1991 Oki Technosystems Laboratory, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Oki Technosystems Laboratory not
 * be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * Oki Technosystems Laboratory makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Oki TECHNOSYSTEMS LABORATORY DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL OKI TECHNOSYSTEMS LABORATORY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Yasuhiro Kawai,	Oki Technosystems Laboratory, Inc.
 *				kawai@otsl.oki.co.jp
 */
#ifndef lint
static char sccsid[] = "@(#)XimpServer.c	1.2";
#endif
/*
 * ximp_server.c -
 *
 * @(#)1.2 91/09/28 17:12:12
 */
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/keysym.h>
#include <X11/Xmu/Atoms.h>
#include "Ximp.h"

/*****************************************************************
 *
 *   XIMP Protocol support functions.
 *
 *****************************************************************/

void
ximp_keypress(dpy, w, icid, event)
    Display *dpy;
    Window w;
    ICID icid;
    XEvent	*event;
{
    XEvent	Message;
    
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 32;
    Message.xclient.window	 = w;
    Message.xclient.data.l[0] 	 = XIMP_KEYPRESS;
    Message.xclient.data.l[1] 	 = icid;
    Message.xclient.data.l[2] 	 = event->xkey.keycode;
    Message.xclient.data.l[3] 	 = event->xkey.state;
    XSendEvent(dpy, w, False, NoEventMask, &Message);
    XFlush(dpy);
}

/*
 * ximp_create_return -
 *
 *       ICID をクライアントに通知する。
 */

void
ximp_create_return(dpy, w, icid)
    Display *dpy;
    Window w;
    ICID icid;
{
    XEvent	Message;
    
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 32;
    Message.xclient.window	 = w;
    Message.xclient.data.l[0] 	 = XIMP_CREATE_RETURN;
    Message.xclient.data.l[1] 	 = icid;
    XSendEvent(dpy, w, False, NoEventMask, &Message);
    XFlush(dpy);
}

void
ximp_process_begin(dpy, w, icid)
    Display *dpy;
    Window w;
    ICID icid;
{
    XEvent	Message;
    
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 32;
    Message.xclient.window	 = w;
    Message.xclient.data.l[0]    = XIMP_PROCESS_BEGIN;
    Message.xclient.data.l[1]    = icid;
    XSendEvent(dpy, w, False, NoEventMask, &Message);
    XFlush(dpy);
}

void
ximp_process_end(dpy, w, icid)
    Display *dpy;
    Window w;
    ICID icid;
{
    XEvent	Message;
    
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 32;
    Message.xclient.window	 = w;
    Message.xclient.data.l[0]    = XIMP_PROCESS_END;
    Message.xclient.data.l[1]    = icid;
    XSendEvent(dpy, w, False, NoEventMask, &Message);
    XFlush(dpy);
}

void
ximp_read(dpy, w, icid, s, l)
    Display *dpy;
    Window w;
    ICID icid;
    char *s;
    int l;
{
    XEvent	Message;
    unsigned long ul;
    
    if (l > MESSAGE_MAX_SIZE) {
	l = MESSAGE_MAX_SIZE;
    }
    ul = htonl(icid);
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 8;
    Message.xclient.window	 = w;
    bcopy(&ul, &Message.xclient.data.b[0], sizeof(ul));
    Message.xclient.data.b[4]    = l;
    bcopy(s, &Message.xclient.data.b[5], l);
    XSendEvent(dpy, w, False, NoEventMask, &Message);
    XFlush(dpy);
}

void
ximp_readprop(dpy, cw, icid, prop, sw, s, l)
    Display *dpy;
    Window cw;	/* client window */
    ICID icid;
    Atom prop;
    Window sw;	/* server window */
    unsigned char *s;
    int l;
{
    XEvent	Message;
    
    XChangeProperty(dpy, sw, prop, XA_COMPOUND_TEXT(dpy),
		    8, PropModeAppend, s, l);
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 32;
    Message.xclient.window	 = cw;
    Message.xclient.data.l[0]    = XIMP_READPROP;
    Message.xclient.data.l[1]    = icid;
    Message.xclient.data.l[2]    = prop;
    XSendEvent(dpy, cw, False, NoEventMask, &Message);
    XFlush(dpy);
}

void
ximp_getvalue_return(dpy, w, icid)
    Display *dpy;
    Window w;
    ICID icid;
{
    XEvent	Message;
    
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 32;
    Message.xclient.window	 = w;
    Message.xclient.data.l[0]    = XIMP_GETVALUE_RETURN;
    Message.xclient.data.l[1]    = icid;
    XSendEvent(dpy, w, False, NoEventMask, &Message);
    XFlush(dpy);
}

void
ximp_reset_return(dpy, w, icid)
    Display *dpy;
    Window w;
    ICID icid;
{
    XEvent	Message;
    
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 32;
    Message.xclient.window	 = w;
    Message.xclient.data.l[0]    = XIMP_RESET_RETURN;
    Message.xclient.data.l[1]    = icid;
    XSendEvent(dpy, w, False, NoEventMask, &Message);
    XFlush(dpy);
}

/*
 *
 */

void
ximp_geometry(dpy, w, icid)
    Display *dpy;
    Window w;
    ICID icid;
{
    XEvent	Message;
    
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 32;
    Message.xclient.window	 = w;
    Message.xclient.data.l[0]    = XIMP_GEOMETRY;
    Message.xclient.data.l[1]    = icid;
    XSendEvent(dpy, w, False, NoEventMask, &Message);
    XFlush(dpy);
}

void
ximp_preeditstart(dpy, w, icid)
    Display *dpy;
    Window w;
    ICID icid;
{
    XEvent	Message;
    
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 32;
    Message.xclient.window	 = w;
    Message.xclient.data.l[0]    = XIMP_PREEDITSTART;
    Message.xclient.data.l[1]    = icid;
    XSendEvent(dpy, w, False, NoEventMask, &Message);
    XFlush(dpy);
}

void
ximp_preeditdone(dpy, w, icid)
    Display *dpy;
    Window w;
    ICID icid;
{
    XEvent	Message;
    
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 32;
    Message.xclient.window	 = w;
    Message.xclient.data.l[0]    = XIMP_PREEDITDONE;
    Message.xclient.data.l[1]    = icid;
    XSendEvent(dpy, w, False, NoEventMask, &Message);
    XFlush(dpy);
}

void
ximp_preeditdraw(dpy, w, icid, atom1, atom2, atom3)
    Display *dpy;
    Window w;
    ICID icid;
    Atom atom1, atom2, atom3;
{
    XEvent	Message;
    
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 32;
    Message.xclient.window	 = w;
    Message.xclient.data.l[0]    = XIMP_PREEDITDRAW;
    Message.xclient.data.l[1]    = icid;
    Message.xclient.data.l[2]    = atom1;
    Message.xclient.data.l[3]    = atom2;
    Message.xclient.data.l[4]    = atom3;
    XSendEvent(dpy, w, False, NoEventMask, &Message);
    XFlush(dpy);
}

void
ximp_preeditcaret(dpy, w, icid, call_data)
    Display *dpy;
    Window w;
    ICID icid;
    XIMPreeditCaretCallbackStruct *call_data;
{
    XEvent	Message;
    
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 32;
    Message.xclient.window	 = w;
    Message.xclient.data.l[0]    = XIMP_PREEDITCARET;
    Message.xclient.data.l[1]    = icid;
    Message.xclient.data.l[2]    = call_data->position;
    Message.xclient.data.l[3]    = call_data->direction;
    Message.xclient.data.l[4]    = call_data->style;
    XSendEvent(dpy, w, False, NoEventMask, &Message);
    XFlush(dpy);
}

void
ximp_statusstart(dpy, w, icid)
    Display *dpy;
    Window w;
    ICID icid;
{
    XEvent	Message;
    
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 32;
    Message.xclient.window	 = w;
    Message.xclient.data.l[0]    = XIMP_STATUSSTART;
    Message.xclient.data.l[1]    = icid;
    XSendEvent(dpy, w, False, NoEventMask, &Message);
    XFlush(dpy);
}

void
ximp_statusdone(dpy, w, icid)
    Display *dpy;
    Window w;
    ICID icid;
{
    XEvent	Message;
    
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 32;
    Message.xclient.window	 = w;
    Message.xclient.data.l[0]    = XIMP_STATUSDONE;
    Message.xclient.data.l[1]    = icid;
    XSendEvent(dpy, w, False, NoEventMask, &Message);
    XFlush(dpy);
}

void
ximp_statusdraw(dpy, w, icid, type, atom1, atom2)
    Display *dpy;
    Window w;
    ICID icid;
    int type;
    Atom atom1, atom2;
{
    XEvent	Message;
    
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 32;
    Message.xclient.window	 = w;
    Message.xclient.data.l[0]    = XIMP_STATUSDRAW;
    Message.xclient.data.l[1]    = icid;
    Message.xclient.data.l[2]    = type;
    Message.xclient.data.l[3]    = atom1;
    Message.xclient.data.l[4]    = atom2;
    XSendEvent(dpy, w, False, NoEventMask, &Message);
    XFlush(dpy);
}

void
ximp_extension(dpy, w, icid, atom, data1, data2)
    Display *dpy;
    Window w;
    ICID icid;
    Atom atom;
    long data1, data2;
{
    XEvent	Message;
    
    Message.xclient.type 	 = ClientMessage;
    Message.xclient.message_type = XIMP_PROTOCOL;
    Message.xclient.format	 = 32;
    Message.xclient.window	 = w;
    Message.xclient.data.l[0]    = XIMP_EXTENSION;
    Message.xclient.data.l[1]    = icid;
    Message.xclient.data.l[2]    = atom;
    Message.xclient.data.l[3]    = data1;
    Message.xclient.data.l[4]    = data2;
    XSendEvent(dpy, w, False, NoEventMask, &Message);
    XFlush(dpy);
}

