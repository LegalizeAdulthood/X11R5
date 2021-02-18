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

static ICAttrMethodsRec methods = {
    Create,
    Destroy,
    Draw,
    Caret,
    SetValue,
    GetValue
};

typedef struct {
    ICAttrMethods methods;
    Bool active;
} PreeditCalPrivateRec, *PreeditCalPriv;

AllocPreCal(ic)
InputContext ic;
{
    PreeditCalPriv data;

    AllocPreNot(ic);
    ic->preedit->data = data = XAlloc(PreeditCalPrivateRec);
    data->methods = ic->preedit->methods;
    ic->preedit->methods = &methods;
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
    FreeXIMText(ic->preedit->text);
    ic->preedit->text = (XIMText *)NULL;
    Xfree(ic->preedit->data);
    Xfree(ic->preedit);
}

static Bool
Start(ic)
InputContext ic;
{
    PreeditCalPriv priv = ic->preedit->data;
    _ximp_preeditstart(ic);
    priv->active = True;
}

static Bool
Draw(ic, data)
InputContext ic;
XIMText *data;
{
    PreeditCalPriv priv = ic->preedit->data;
    Atom changes = (Atom)0, text = (Atom)0, feedback = (Atom)0;
    XTextProperty prop;
    unsigned long change_data[3];

    if (!priv->active && data->length) Start(ic);

    changes = GetPropAtom(ic);
    change_data[0] = data->length;
    change_data[1] = 0;
    change_data[2] = (ic->preedit->text) ? ic->preedit->text->length : 0;
    XChangeProperty(ic->im->display, ic->im->owner, changes,
		ic->im->_preeditdraw_data, 32, PropModeReplace,
		change_data, 3);
    text = GetPropAtom(ic);
    if (data->encoding_is_wchar) {
	XwcTextListToTextProperty(ic->im->display,
		&data->string.wide_char, 1, XCompoundTextStyle, &prop);
    } else {
	XmbTextListToTextProperty(ic->im->display,
		&data->string.multi_byte, 1, XCompoundTextStyle, &prop);
    }
    XSetTextProperty(ic->im->display, ic->im->owner, &prop, text);
    if (data->feedback) {
	feedback = GetPropAtom(ic);
	XChangeProperty(ic->im->display, ic->im->owner, feedback,
		ic->im->_feedbacks, 32, PropModeReplace,
		data->feedback, data->length);
    }
    _ximp_preeditdraw(ic, changes, text, feedback);
    if (data->length == 0) {
	if (priv->active) {
	    Done(ic);
	}
    }
    (*priv->methods->draw)(ic, data);
}

static Bool
Done(ic)
InputContext ic;
{
    PreeditCalPriv priv = ic->preedit->data;

    priv->active = False;
    _ximp_preeditdone(ic);
}

static Bool
Caret(ic, data)
InputContext ic;
XIMPreeditCaretCallbackStruct *data;
{
    _ximp_preeditcaret(ic, data->position, data->direction, data->style);
}

static Bool
SetValue(ic, mask, prop)
InputContext ic;
unsigned long mask;
PreeditProps prop;
{
}

static Bool
GetValue(ic, mask, prop)
InputContext ic;
unsigned long mask;
PreeditProps prop;
{
}

