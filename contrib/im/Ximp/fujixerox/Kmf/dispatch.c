/*

Copyright 1991 by Fuji Xerox Co., Ltd., Tokyo, Japan.

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of M.I.T. not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission.  Fuji Xerox Co., Ltd. makes no representations
about the suitability of this software for any purpose.  It is provided "as is"
without express or implied warranty.

FUJI XEROX CO.,LTD. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJI XEROX CO.,LTD. BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

*/

#include "XIMServer.h"
#include <stdio.h>

static
ProcKeyPress(dpy, ev)
Display *dpy;
XKeyEvent *ev;
{
    InputContext ic = (InputContext)ICOfKeyPress(ev);
    InputMethod im = ic->im;
    static XKeyEvent key;

    if (ic == NULL) {
	return;
    }
    (*ic->methods->key_press)(ic, ev);
}


ProcDestroyNotify(d, ev)
Display	*d;
XDestroyWindowEvent	*ev;
{
    InputContext ic = (InputContext)ICOfDestroyNotify(ev);
    InputMethod im;

    if (!ic) return;
    im = ic->im;
    if (ic->client == ev->window) {
	(*ic->methods->destroy)(ic, True);
	return;
    }
    if (ic->focus == ev->window) {
	/* ??? */
	ic->focus = (Window)NULL;
	return;
    }
    return;
}

static
ProcButtonPress(dpy, ev)
Display *dpy;
XButtonEvent *ev;
{
}

MainLoop(dpy)
Display *dpy;
{
    XEvent ev;

    while (1) {
	XNextEvent(dpy, &ev);
	switch (ev.type) {
	case KeyPress:
	    ProcKeyPress(dpy, &ev);
	    break;
	case ButtonPress:
	    ProcButtonPress(dpy, &ev);
	    break;
	case Expose:
	    ProcExpose(dpy, &ev);
	    break;
	case DestroyNotify:
	    ProcDestroyNotify(dpy, &ev);
	    break;
	case PropertyNotify:
	    ProcPropertyNotify(dpy, &ev);
	    break;
	case SelectionClear:
	    return;
	case ClientMessage:
	    ProcClientMessage(dpy, &ev);
	    break;
	default:
	    break;
	}
    }
}

