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

static PropManager p_head = (PropManager)NULL;

PropManager
AllocPropManager(window)
Window window;
{
    PropManager p;

    for (p = p_head; p; p = p->next) {
	if (p->window == window) {
	    p->link_count++;
	    return(p);
	}
    }
    p = (PropManager)XAllocFreeArea(sizeof(PropManagerRec));
    if (p) {
	p->window = window;
	p->next = p_head;
	p_head = p;
	p->list = (PropList)NULL;
	p->list_size = 0;
	p->link_count = 1;
    }
    return(p);
}

void
FreePropManager(pm)
PropManager pm;
{
    PropManager p, *q;
    if (--(pm->link_count)) return;
    for (q = &p_head; p = *q; q = &p->next) {
	if (p == pm) {
	    *q = p->next;
	    Xfree(pm->list);
	    Xfree(pm);
	    break;
	}
    }
}

Atom
GetPropAtom(ic)
InputContext ic;
{
    static char buf[1024];
    InputMethod im = ic->im;
    PropManager pm = im->prop;
    int i;

    for (i = 0; i < pm->list_size; i++) {
	if (!pm->list[i].busy) {
	    pm->list[i].busy = True;
	    pm->list[i].ic = ic;
	    return(pm->list[i].atom);
	}
    }
    i = pm->list_size++;
    pm->list = pm->list ?
	(PropList)Xrealloc(pm->list, sizeof(PropRec) * pm->list_size) :
	(PropList)Xmalloc(sizeof(PropRec) * pm->list_size);
    sprintf(buf, "_XIMP_DATA_%d", i);
    pm->list[i].atom = XInternAtom(im->display, buf, False);
    pm->list[i].busy = True;
    pm->list[i].ic = ic;
    return(pm->list[i].atom);
}

void
ProcPropertyNotify(dpy, ev)
Display *dpy;
XPropertyEvent *ev;
{
    PropManager pm;
    int i;
    if (ev->state != PropertyDelete) return;

    for (pm = p_head; pm; pm = pm->next) {
	if (pm->window == ev->window) {
	    break;
	}
    }
    if (!pm) return;
    for (i = 0; i < pm->list_size; i++) {
	if (pm->list[i].atom == ev->atom) {
	    pm->list[i].busy = False;
	    pm->list[i].ic = (InputContext)NULL;
	    return;
	}
    }
}

void
FreePropAtom(ic, atom)
InputContext ic;
Atom atom;
{
    PropManager pm = ic->im->prop;
    int i;

    for (i = 0; i < pm->list_size; i++) {
	if (pm->list[i].atom == atom) {
	    pm->list[i].busy = False;
	    pm->list[i].ic = (InputContext)NULL;
	    return;
	}
    }
}

void
FreeAllPropAtom(ic, delete)
InputContext ic;
Bool delete;
{
    PropManager pm = ic->im->prop;
    int i;

    for (i = 0; i < pm->list_size; i++) {
	if (pm->list[i].ic == ic) {
	    if (delete) {
		XDeleteProperty(ic->im->display, pm->window, pm->list[i].atom);
	    } else {
		pm->list[i].busy = False;
		pm->list[i].ic = (InputContext)NULL;
	    }
	}
    }
}
