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

static XClientMessageEvent Event;
static XClientMessageEvent *event = &Event;

_XIMP_SendClMsg(ic, type, data0, data1, data2)
InputContext ic;
int type;
int data0, data1, data2;
{
    event->type = ClientMessage;
    event->display = ic->im->display;
    event->window = ic->focus;
    event->message_type = ic->im->_protocol;
    event->format = 32;
    event->data.l[0] = type;
    event->data.l[1] = ic->icid;
    event->data.l[2] = data0;
    event->data.l[3] = data1;
    event->data.l[4] = data2;
    XSendEvent(ic->im->display, ic->focus, False, NoEventMask, event);
    XFlush(ic->im->display);
}

_ximp_send_keypress(ic, ev)
InputContext ic;
XKeyEvent *ev;
{
    _XIMP_SendClMsg(ic, XIMP_KEYPRESS, ev->keycode, ev->state, 0);
}

_ximp_geometry(ic)
InputContext ic;
{
    _XIMP_SendClMsg(ic, XIMP_GEOMETRY, 0, 0, 0);
}

_ximp_preeditstart(ic)
InputContext ic;
{
    _XIMP_SendClMsg(ic, XIMP_PREEDITSTART, 0, 0, 0);
}

_ximp_preeditdone(ic)
InputContext ic;
{
    _XIMP_SendClMsg(ic, XIMP_PREEDITDONE, 0, 0, 0);
}

_ximp_preeditdraw(ic, atom0, atom1, atom2)
InputContext ic;
Atom atom0, atom1, atom2;
{
    _XIMP_SendClMsg(ic, XIMP_PREEDITDRAW, atom0, atom1, atom2);
}

_ximp_preeditcaret(ic, pos, dir, style)
InputContext ic;
int pos;
XIMCaretDirection dir;
XIMCaretStyle style;
{
    _XIMP_SendClMsg(ic, XIMP_PREEDITCARET, pos, dir, style);
}

_ximp_statusstart(ic)
InputContext ic;
{
    _XIMP_SendClMsg(ic, XIMP_STATUSSTART, 0, 0, 0);
}

_ximp_statusdone(ic)
InputContext ic;
{
    _XIMP_SendClMsg(ic, XIMP_STATUSDONE, 0, 0, 0);
}

_ximp_statusdraw(ic, type, data0, data1)
InputContext ic;
XIMStatusDataType type;
int data0, data1;
{
    _XIMP_SendClMsg(ic, XIMP_STATUSDRAW, type, data0, data1);
}

_ximp_send_extension(ic, ext_atom, data0, data1)
InputContext ic;
Atom ext_atom;
int data0, data1;
{
    _XIMP_SendClMsg(ic, XIMP_EXTENSION, ext_atom, data0, data1);
}

_ximp_create_return(ic, icid)
InputContext ic;
unsigned int icid;
{
    _XIMP_SendClMsg(ic, XIMP_CREATE_RETURN, 0, 0, 0);
}

_ximp_reset_return(ic, atom)
InputContext ic;
Atom atom;
{
    _XIMP_SendClMsg(ic, XIMP_RESET_RETURN, atom, 0, 0);
}

_ximp_getvalue_return(ic)
InputContext ic;
Atom atom;
{
    _XIMP_SendClMsg(ic, XIMP_GETVALUE_RETURN, 0, 0, 0);
}

_ximp_conversion_begin(ic)
InputContext ic;
{
    _XIMP_SendClMsg(ic, XIMP_CONVERSION_BEGIN, 0, 0, 0);
}

_ximp_conversion_end(ic)
InputContext ic;
{
    _XIMP_SendClMsg(ic, XIMP_CONVERSION_END, 0, 0, 0);
}

_ximp_readprop(ic, atom)
InputContext ic;
Atom atom;
{
    _XIMP_SendClMsg(ic, XIMP_READPROP, atom, 0, 0);
}
