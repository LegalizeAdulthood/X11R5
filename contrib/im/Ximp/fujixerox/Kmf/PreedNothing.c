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

DrawPreeditWindow(d, w, im)
Display *d;
Window w;
InputMethod im;
{
    XIMText *text;
    InputContext ic;
    if (!im->focused_ic) return;
    ic = im->focused_ic;
    if (!(text = ic->preedit->text)) return;
    if (text->encoding_is_wchar) {
	XwcDrawImageString(d, w, im->font, im->gc,
		0, -im->f_ext->max_logical_extent.y,
		text->string.wide_char, text->length);
    } else {
	XmbDrawImageString(d, w, im->font, im->gc,
		0, -im->f_ext->max_logical_extent.y,
		text->string.multi_byte, strlen(text->string.multi_byte));
    }
    if (ic->preedit->num_hilight) {
	XFillRectangles(d, w, ic->im->invgc,
		ic->preedit->hilight, ic->preedit->num_hilight);
    }
}

AllocPreNot(ic)
InputContext ic;
{
    ic->preedit = (ICAttribute)XAllocFreeArea(sizeof(ICAttributeRec));
    ic->preedit->text = (XIMText *)NULL;
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
    Xfree(ic->preedit);
}

static Bool
Draw(ic, data)
InputContext ic;
XIMText *data;
{
    FreeXIMText(ic->preedit->text);
    ic->preedit->text = (XIMText *)NULL;
    if(ic->preedit->num_hilight) Xfree(ic->preedit->hilight);
    ic->preedit->text = data;
    ic->preedit->num_hilight = 0;
    if (ic->im->focused_ic == ic)
	XClearArea(ic->im->display, ic->im->preedit, 0, 0, 0, 0, True);
    if (data->feedback) {
	XRectangle *logical_extents = (XRectangle *)Xmalloc(sizeof(XRectangle) * data->length);
	XRectangle *ink_extents = (XRectangle *)Xmalloc(sizeof(XRectangle) * data->length);
	int i, startPos, pos, num_chars;
	XIMFeedback origin;

	if (data->encoding_is_wchar) {
	    XwcTextPerCharExtents(ic->im->font, data->string.wide_char, data->length, ink_extents, logical_extents, data->length, &num_chars, NULL, NULL);
	} else {
	    XmbTextPerCharExtents(ic->im->font, data->string.multi_byte, strlen(data->string.multi_byte), ink_extents, logical_extents, data->length, &num_chars, NULL, NULL);
	}
	origin = (data->feedback[0] & XIMReverse);
	startPos = pos = -ic->im->f_ext->max_logical_extent.x;
	for (i = 0; i < data->length; i++) {
	    if (origin != (data->feedback[i] & XIMReverse)) {
		if (origin) {
		    XRectangle *rect;
		    if (ic->preedit->num_hilight) {
			ic->preedit->hilight = (XRectangle *)Xrealloc(ic->preedit->hilight, sizeof(XRectangle) * (ic->preedit->num_hilight + 1));
		    } else {
			ic->preedit->hilight = XAlloc(XRectangle);
		    }
		    rect = &ic->preedit->hilight[ic->preedit->num_hilight];
		    rect->x = startPos;
		    rect->y = 0;
		    rect->width = pos - startPos;
		    rect->height = ic->im->f_ext->max_logical_extent.height;
		    ic->preedit->num_hilight++;
		}
		startPos = pos;
		origin = (data->feedback[i] & XIMReverse);
	    }
	    pos += logical_extents[i].width;
	}
	if (origin) {
	    XRectangle *rect;
	    if (ic->preedit->num_hilight) {
		ic->preedit->hilight = (XRectangle *)Xrealloc(ic->preedit->hilight, sizeof(XRectangle) * (ic->preedit->num_hilight + 1));
	    } else {
		ic->preedit->hilight = XAlloc(XRectangle);
	    }
	    rect = &ic->preedit->hilight[ic->preedit->num_hilight];
	    rect->x = startPos;
	    rect->y = 0;
	    rect->width = pos - startPos;
	    rect->height = ic->im->f_ext->max_logical_extent.height;
	    ic->preedit->num_hilight++;
	}
	Xfree(logical_extents);
	Xfree(ink_extents);
    }
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

