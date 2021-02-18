
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

typedef struct _Redraw RedrawRec, *RedrawPtr;

struct _Redraw {
    RedrawPtr next;
    Display *display;
    Window window;
    void *data;
    Bool (*method)();
};

static RedrawPtr head = NULL;

RegisterRedrawWindow(d, w, data, method)
Display *d;
Window w;
void *data;
Bool (*method)();
{
    RedrawPtr p = XAlloc(RedrawRec);

    p->next = head;
    p->display = d;
    p->window = w;
    p->data = data;
    p->method = method;
    head = p;
}

UnregisterRedrawWindow(d, w, data, method)
Display *d;
Window w;
void *data;
Bool (*method)();
{
    RedrawPtr p, *q;

    for (q = &head; p = *q; q = &p->next) {
	if ((p->display == d) &&
	    (p->window == w) &&
	    (p->data == data) &&
	    (p->method == method)) {
	    *q = p->next;
	    Xfree(p);
	    return;
	}
    }
}

ProcExpose(d, ev)
Display *d;
XExposeEvent *ev;
{
    RedrawPtr redraw;

    if (ev->count) return;
    for (redraw = head; redraw; redraw = redraw->next) {
	if ((redraw->display == ev->display) &&
	    (redraw->window == ev->window)) {
	    (*redraw->method)(redraw->display, redraw->window, redraw->data);
	    break;
	}
    }
}
