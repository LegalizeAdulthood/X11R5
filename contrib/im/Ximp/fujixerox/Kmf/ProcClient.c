
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
#include <X11/Xatom.h>

ProcClientMessage(dpy, ev)
Display *dpy;
XClientMessageEvent *ev;
{
    InputMethod im = (InputMethod)IMOfClientMessage(ev);
    InputContext ic;
    static XKeyEvent key;

    if (im == NULL) return;
    if (ev->data.l[0] == XIMP_CREATE) {
	ic = AllocIC(im);
	ic->icid = (unsigned long)ic;
	ic->style = (XIMStyle)ev->data.l[2];
	ic->client = (Window)ev->data.l[1];
	XSelectInput(im->display, ic->client, StructureNotifyMask);
	(*ic->methods->create)(ic, ev->data.l[3]);
	return;
    }
    ic = (InputContext)ICOfICID(im, (ICID)ev->data.l[1]);
    if (ic == NULL) {
	return;
    }
    switch (ev->data.l[0]) {
    case XIMP_KEYPRESS:
	key.type = KeyPress;
	key.serial = ev->serial;
	key.send_event = False;
	key.display = ev->display;
	key.window = ev->window;
	key.root = DefaultRootWindow(ev->display);
	key.subwindow = ev->window;
	key.time = 0;
	key.x = 0;
	key.y = 0;
	key.x_root = 0;
	key.y_root = 0;
	key.state = ev->data.l[3];
	key.keycode = ev->data.l[2];
	key.same_screen = True;
	(*ic->methods->key_press)(ic, &key);
	break;
    case XIMP_CREATE:
	/* Not reached */
	break;
    case XIMP_DESTROY:
	(*ic->methods->destroy)(ic, False);
	break;
    case XIMP_BEGIN:
	(*ic->methods->begin)(ic);
	break;
    case XIMP_END:
	(*ic->methods->end)(ic);
	break;
    case XIMP_SETFOCUS:
	(*ic->methods->set_focus)(ic);
	break;
    case XIMP_UNSETFOCUS:
	(*ic->methods->unset_focus)(ic);
	break;
    case XIMP_CHANGE:
	/* Change is now altered to Setvalue */
	break;
    case XIMP_MOVE:
	(*ic->methods->move)(ic, ev->data.l[2], ev->data.l[3]);
	break;
    case XIMP_RESET:
	(*ic->methods->reset)(ic);
	break;
    case XIMP_SETVALUE:
	(*ic->methods->setvalue)(ic, ev->data.l[2]);
	break;
    case XIMP_GETVALUE:
	(*ic->methods->getvalue)(ic, ev->data.l[2]);
	break;
    case XIMP_PREEDITSTART_RETURN:
	break;
    case XIMP_PREEDITCARET_RETURN:
	break;
    case XIMP_EXTENSION:
	if (ev->data.l[2] == ic->im->_ext_ui_back_front) {
	    ic->is_bep = ev->data.l[3];
	}
	break;
    default:
	break;
    }
}
