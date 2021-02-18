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

DrawStatusWindow(d, w, im)
Display *d;
Window w;
InputMethod im;
{
    XIMText *text;
    if (!im->focused_ic) return;
    if (!(text = im->focused_ic->status->text)) return;
    if (text->encoding_is_wchar) {
	XwcDrawImageString(d, w, im->font, im->gc,
		0, -im->f_ext->max_logical_extent.y,
		text->string.wide_char, text->length);
    } else {
	XmbDrawImageString(d, w, im->font, im->gc,
		0, -im->f_ext->max_logical_extent.y,
		text->string.multi_byte, strlen(text->string.multi_byte));
    }
}

AllocStNot(ic)
InputContext ic;
{
    ic->status = (ICAttribute)XAllocFreeArea(sizeof(ICAttributeRec));
    ic->status->text = (XIMText *)NULL;
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
Draw(ic, data)
InputContext ic;
XIMText *data;
{
    FreeXIMText(ic->status->text);
    ic->status->text = data;
    XClearArea(ic->im->display, ic->im->status, 0, 0, 0, 0, True);
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

