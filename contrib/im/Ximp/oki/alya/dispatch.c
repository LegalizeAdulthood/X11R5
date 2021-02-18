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
static char sccsid[] = "@(#)dispatch.c	1.2";
#endif
/*
 * dispatch.c -
 *
 * @(#)dispatch.c	1.2 91/09/28 17:12:14
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include "Client.h"
#include "Conversion.h"
#include "Context.h"
#include "Ximp.h"

#define MAXIC 20
static Widget ic[MAXIC];

struct PropTable {
    ICID ic;
    Atom prop;
};

static struct PropTable table[MAX_PROP];

static int XimpCreateContext();
static Widget XimpICIDtoContext();
static void XimpDestroyContext();
static void GetAppName();

void
Handler(w, client_data, event)
    Widget w;
    caddr_t client_data;
    XEvent *event;
{
    Widget parent, imw;
    Display *dpy;
    int request;
    ICID icid;
    Window clientWindow;
    int ac;
    Arg args[10];
    char buf[BUFSIZ];

    dpy = XtDisplay(w);
    if (event->type == ClientMessage &&
	event->xclient.message_type == XIMP_PROTOCOL) {
	request = (int)event->xclient.data.l[0];
#ifdef DEBUG
	printf("req = %d, id = %x\n", request, event->xclient.data.l[1]);
	printf("arg1 = %x ", event->xclient.data.l[2]);
	printf("arg2 = %x ", event->xclient.data.l[3]);
	printf("arg3 = %x\n", event->xclient.data.l[4]);
#endif
	if (request == XIMP_CREATE) {
	    clientWindow = (Window)event->xclient.data.l[1];
	    imw = XtWindowToWidget(dpy, clientWindow);
	    if (imw == (Widget)NULL) {
		GetAppName(dpy, clientWindow, buf);
		parent = (Widget)client_data;
		ac = 0;
		XtSetArg(args[ac], XtNwindow, clientWindow); ac++;
		imw = XtCreateWidget(buf, clientWindowWidgetClass,
			             parent, args, ac);
		XtRealizeWidget(imw);
	    } else {
		if (!XtIsSubclass(imw, clientWindowWidgetClass)) { 
		}
	    }
	    icid = XimpCreateContext(imw, clientWindow, XtWindow(w),
				     (XIMStyle)event->xclient.data.l[2],
				     (int)event->xclient.data.l[3]);
	} else {
	    icid = (ICID)event->xclient.data.l[1];
	    imw = XimpICIDtoContext(icid);
	    if (imw) {
	        XimpMessage(imw,
		            request,
		            event->xclient.data.l[2],
                            event->xclient.data.l[3],
		            event);
	    }
	}
    }
}

static void
GetAppName(dpy, w, buf)
    Display *dpy;
    Window  w;
    String  buf;
{
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long leftover;
    unsigned char *data = NULL;

    if (XGetWindowProperty(dpy, w, XA_WM_CLASS, 0L, (long)BUFSIZ, False,
                           XA_STRING, &actual_type, &actual_format, 
			   &nitems, &leftover, &data) != Success) {
	strcpy(buf, "client");
    } else {
	if (data) {
	    strcpy(buf, (char *)data);
	    Xfree( (char *)data);
	} else {
	    strcpy(buf, "client");
	}
    }
}

static int
XimpCreateContext(parent, win, server_window, style, mask)
    Widget parent;
    Window win;
    Window server_window;
    XIMStyle style;
    int mask;
{
    int i;
    int ac;
    Arg args[10];
    Widget w;
    char buf[20];

    for (i=1; i< MAXIC; i++) {
	if (ic[i] == (Widget)NULL) {
	    break;
	}
    }
    if (i >= MAXIC) {
	return((ICID)0);
    }
    sprintf(buf, "ic%d", i);
    ac = 0;
    XtSetArg(args[ac], XtNicid, i); ac++;
    XtSetArg(args[ac], XtNstatus, w); ac++;
    XtSetArg(args[ac], XtNwindow, win); ac++;
    XtSetArg(args[ac], XtNserver, server_window); ac++;
    XtSetArg(args[ac], XtNinputStyle, style); ac++;
    XtSetArg(args[ac], XtNmask, mask); ac++;
    ic[i] = XtCreateWidget(buf, inputContextObjectClass,
			   parent, args, ac);
    XtAddCallback(ic[i], XtNdestroyCallback, XimpDestroyContext, i);
    return((ICID)i);
}

static Widget
XimpICIDtoContext(icid)
    ICID icid;
{
    return(ic[icid]);
}

static void
XimpDestroyContext(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     caddr_t call_data;
{
    ICID icid;
    int i;
    Display *dpy;
    int ac;
    Arg args[2];
    Window win;

    icid = (ICID)client_data;
    
#ifdef DEBUG
    printf("Conversion:destroy callback called\n");
#endif
    ac = 0;
    XtSetArg(args[ac], XtNserver, &win); ac++;
    XtGetValues(w, args, ac);
    dpy = XtDisplayOfObject((Object)w);
    ic[icid] = (Widget)NULL;
    for (i=0; i < MAX_PROP; i++) {
	if (table[i].ic == icid) {
	    XDeleteProperty(dpy, win, table[i].prop);
	}
    }
    return;
}

void
PropHandler(w, client_data, event)
    Widget w;
    caddr_t client_data;
    XEvent *event;
{
    int i;

    if (event->type == PropertyNotify) {
	if (event->xproperty.state == PropertyDelete) {
	    for (i=0; i < MAX_PROP; i++) {
		if (event->xproperty.atom == table[i].prop) {
		    table[i].ic = (ICID)0;
		    break;
		}
	    }
	}
    }
}

Atom
AllocProp(icid)
    ICID icid;
{
    int i;
    Atom atom;

    atom = (Atom)0;
    for (i = 0; i < MAX_PROP; i++) {
	if (table[i].ic == (ICID)0) {
	    table[i].ic = icid;
	    atom = table[i].prop;
	    break;
	}
    }
    if (atom == (Atom)0) {
	fprintf(stderr, "Atom allocate error\n");
    }
    return(atom);
}

void
init_prop(dpy)
    Display *dpy;
{
    int i;
    char buf[20];

    for (i=0; i< MAX_PROP; i++) {
	sprintf(buf, "_XIMP_CTEXT_%d", i);
        table[i].prop = XInternAtom(dpy, buf, FALSE);
	table[i].ic = (ICID)0;
    }
}
