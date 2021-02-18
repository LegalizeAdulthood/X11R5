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


static Bool Create(), Destroy(), Draw(), Caret();
static Bool SetValue(), GetValue();

typedef struct {
    ICAttrMethods methods;
    Bool active;
} PrivData;

static ICAttrMethodsRec methods = {
    Create,
    Destroy,
    Draw,
    Caret,
    SetValue,
    GetValue
};

AllocStCal(ic)
InputContext ic;
{
    PrivData *priv;

    AllocStNot(ic);
    priv = ic->status->data = XAlloc(PrivData);
    priv->methods = ic->status->methods;
    ic->status->methods = &methods;
}

static Bool
Create(ic)
InputContext ic;
{
}

static Bool
Destroy(ic)
InputContext ic;
{
    FreeXIMText(ic->status->text);
    Xfree(ic->status);
}

static Bool
Start(ic)
InputContext ic;
{
    PrivData *priv = ic->status->data;

    _ximp_statusstart(ic);
    priv->active = True;
}

static Bool
Draw(ic, data)
InputContext ic;
XIMText *data;
{
    PrivData *priv = ic->status->data;
    unsigned long data0, data1;
    XTextProperty prop;
    unsigned long change_data[3];

    if (data->length == 0) {
	if (priv->active) Done(ic);
	(*priv->methods->draw)(ic, data);
	return;
    }
    if ((!priv->active) && (data->length)) Start(ic);

    data0 = (unsigned long)GetPropAtom(ic);
    if (data->encoding_is_wchar) {
	XwcTextListToTextProperty(ic->im->display,
		&data->string.wide_char, 1, XCompoundTextStyle, &prop);
    } else {
	XmbTextListToTextProperty(ic->im->display,
		&data->string.multi_byte, 1, XCompoundTextStyle, &prop);
    }
    XSetTextProperty(ic->im->display, ic->im->owner, &prop, (Atom)data0);
    if (data->feedback) {
	data1 = (unsigned long)GetPropAtom(ic);
	XChangeProperty(ic->im->display, ic->im->owner, (Atom)data1,
		ic->im->_feedbacks, 32, PropModeReplace,
		data->feedback, prop.nitems);
    }
    _ximp_statusdraw(ic, XIMTextType, data0, data1);
    (*priv->methods->draw)(ic, data);
}

static Bool
Done(ic)
InputContext ic;
{
    PrivData *priv = ic->status->data;

    priv->active = False;
    _ximp_statusdone(ic);
}

static Bool
Caret(ic)
InputContext ic;
{
}

static Bool
SetValue(ic, mask, prop)
InputContext ic;
unsigned long mask;
StatusProps prop;
{
}

static Bool
GetValue(ic, mask, prop)
InputContext ic;
unsigned long mask;
StatusProps prop;
{
}

