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

static InputMethod im_head = (InputMethod)NULL;

InputMethod
IMOfSelectionOwner(d, owner)
Display *d;
Window owner;
{
    InputMethod im;

    for (im = im_head; im; im = im->next) {
	if ((im->display == d) && (im->owner == owner)) return(im);
    }
    return(NULL);
}

InputContext
ICOfICID(im, icid)
InputMethod im;
unsigned int icid;
{
    InputContext ic;

    for (ic = im->ic; ic; ic = ic->next) {
	if (ic->icid == icid) {
	    return(ic);
	}
    }
    return((InputContext)NULL);
}

InputContext
ICOfFocusWindow(im, focus)
InputMethod im;
Window focus;
{
    InputContext ic;

    for (ic = im->ic; ic; ic = ic->next) {
	if (ic->focus == focus) {
	    return(ic);
	}
    }
    return((InputContext)NULL);
}

InputContext
ICOfKeyPress(ev)
XKeyEvent *ev;
{
    InputMethod im = im_head;
    InputContext ic;
    for (; im; im = im->next) {
	for (ic = im->ic; ic; ic = ic->next) {
	    if (ev->window == ic->focus) return(ic);
	}
    }
    return((InputContext)NULL);
}

InputContext
ICOfDestroyNotify(ev)
XDestroyWindowEvent *ev;
{
    InputMethod im = im_head;
    InputContext ic;
    for (; im; im = im->next) {
	for (ic = im->ic; ic; ic = ic->next) {
	    if (ev->window == ic->client) return(ic);
	    if (ev->window == ic->focus) return(ic);
	}
    }
    return((InputContext)NULL);
}

InputMethod
IMOfClientMessage(ev)
XClientMessageEvent *ev;
{
    InputMethod im = im_head;
    for (; im; im = im->next) {
#ifdef MULTI_INPUT_METHOD
	if (ev->message_type == im->selection) break;
#else MULTI_INPUT_METHOD
	if (ev->message_type == im->_protocol) break;
#endif MULTI_INPUT_METHOD
    }
    return(im);
}

InputMethod
AllocIM()
{
    InputMethod im = (InputMethod)XAllocFreeArea(sizeof(InputMethodRec));

    if (im) {
	im->next = im_head;
	im_head = im;
    }
    return(im);
}

FreeIM(im)
InputMethod im;
{
    InputMethod p, *q;

    for (q = &im_head; p = *q; q = &p->next) {
	if (p == im) {
	    *q = p->next;
	    /* Free Internal Resources */
	    break;
	}
    }
}
