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
    Window window;
    XRectangle area;
    XRectangle area_needed;
    Colormap colormap;
    unsigned long foreground;
    unsigned long background;
    GC gc;
    GC invgc;
    Pixmap bg_pixmap;
    XFontSet fontset;
    XFontSetExtents *f_ext;
    long line_spacing;
    Cursor cursor;
} StatusAreaPrivateRec, *StatusAreaPriv;

DrawStatusArea(d, w, ic)
Display *d;
Window w;
InputContext ic;
{
    XIMText *text;
    StatusAreaPriv data = (StatusAreaPriv)ic->status->data;

    if (!(text = ic->status->text)) return;
    if (text->encoding_is_wchar) {
	XwcDrawImageString(d, w, data->fontset, data->gc,
		0, -data->f_ext->max_logical_extent.y,
		text->string.wide_char, text->length);
    } else {
	XmbDrawImageString(d, w, data->fontset, data->gc,
		0, -data->f_ext->max_logical_extent.y,
		text->string.multi_byte, strlen(text->string.multi_byte));
    }
}

AllocStArea(ic)
InputContext ic;
{
    StatusAreaPriv data;
    char **miss_list, *default_str;
    int miss_list_count;

    AllocStNot(ic);
    data = ic->status->data = XAlloc(StatusAreaPrivateRec);
    data->methods = ic->status->methods;
    ic->status->methods = &methods;
    data->foreground = ic->im->fore;
    data->background = ic->im->back;
    data->bg_pixmap = None;
    data->fontset = XCreateFontSet(ic->im->display,
		XBaseFontNameListOfFontSet(ic->im->font),
		&miss_list, &miss_list_count, &default_str);
    data->f_ext = XExtentsOfFontSet(data->fontset);
    data->line_spacing = data->f_ext->max_logical_extent.height;
    data->cursor = None;
}

static Bool
Create(ic)
InputContext ic;
{
    StatusAreaPriv data = (StatusAreaPriv)ic->status->data;
    XGCValues values;

    data->window = XCreateSimpleWindow(ic->im->display, ic->client,
		data->area.x, data->area.y,
		data->area.width, data->area.height,
		0, data->background, data->background);
    values.function = GXcopy;
    values.foreground = data->foreground;
    values.background = data->background;
    data->gc = XCreateGC(ic->im->display, data->window, GCFunction|GCForeground|GCBackground, &values);
    values.function = GXxor;
    values.foreground = (data->background ^ data->foreground);
    data->invgc = XCreateGC(ic->im->display, data->window, GCFunction|GCForeground, &values);
    XSelectInput(ic->im->display, data->window, ExposureMask);
    RegisterRedrawWindow(ic->im->display, data->window, ic, DrawStatusArea);
    XMapWindow(ic->im->display, data->window);
}

static Bool
Destroy(ic, event_generated)
InputContext ic;
Bool event_generated;
{
    StatusAreaPriv data = (StatusAreaPriv)ic->status->data;
    XEvent ev;

    XGrabServer(ic->im->display);
    if ((!event_generated) &&
	(!XCheckIfEvent(ic->im->display, &ev, CheckDestroyNotify, ic->client))) {
	XDestroyWindow(ic->im->display, data->window);
    }
    XUngrabServer(ic->im->display);
    XFlush(ic->im->display);
    FreeXIMText(ic->status->text);
    XFreeGC(ic->im->display, data->gc);
    XFreeGC(ic->im->display, data->invgc);
    XFreeFontSet(ic->im->display, data->fontset);
    Xfree(data);
    Xfree(ic->status);
}

static Bool
Draw(ic, data)
InputContext ic;
XIMText *data;
{
    StatusAreaPriv priv = (StatusAreaPriv)ic->status->data;

    (*priv->methods->draw)(ic, data);
    XClearArea(ic->im->display, priv->window, 0, 0, 0, 0, True);
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
    StatusAreaPriv data = (StatusAreaPriv)ic->status->data;
    char *base_fontset_name_list = (char *)prop;
    char **miss_list, *default_str;
    int miss_list_count;
    XGCValues values;

    if (mask & XIMP_STS_AREA_MASK) {
	data->area.x = prop->area.x;
	data->area.y = prop->area.y;
	data->area.width = prop->area.width;
	data->area.height = prop->area.height;
    }
    if (mask & XIMP_STS_FG_MASK) {
	data->foreground = prop->foreground;
	if (data->window) {
	    values.function = GXcopy;
	    values.foreground = data->foreground;
	    values.background = data->background;
	    XChangeGC(ic->im->display, data->gc, GCFunction|GCForeground|GCBackground, &values);
	    values.function = GXxor;
	    values.foreground = (data->background ^ data->foreground);
	    XChangeGC(ic->im->display, data->invgc, GCFunction|GCForeground, &values);
	}
    }
    if (mask & XIMP_STS_BG_MASK) {
	data->background = prop->background;
	if (data->window) {
	    values.function = GXcopy;
	    values.foreground = data->foreground;
	    values.background = data->background;
	    XChangeGC(ic->im->display, data->gc, GCFunction|GCForeground|GCBackground, &values);
	    values.function = GXxor;
	    values.foreground = (data->background ^ data->foreground);
	    XChangeGC(ic->im->display, data->invgc, GCFunction|GCForeground, &values);
	}
    }
    if (mask & XIMP_STS_COLORMAP_MASK) {
	data->colormap = prop->colormap;
    }
    if (mask & XIMP_STS_BGPIXMAP_MASK) {
	data->bg_pixmap = prop->background_pixmap;
    }
    if (mask & XIMP_STS_LINESP_MASK) {
	data->line_spacing = prop->line_spacing;
    }
    if (mask & XIMP_STS_CURSOR_MASK) {
	data->cursor = prop->cursor;
    }
    if (mask & XIMP_STS_AREANEED_MASK) {
	data->area_needed.width = prop->area_needed.width;
	data->area_needed.height = prop->area_needed.height;
    }
    if (mask & XIMP_STS_FONT_MASK) {
	if (data->fontset) XFreeFontSet(ic->im->display, data->fontset);
	data->fontset = XCreateFontSet(ic->im->display,
		    base_fontset_name_list,
		    &miss_list, &miss_list_count, &default_str);
	data->f_ext = XExtentsOfFontSet(data->fontset);
    }
}

static Bool
GetValue(ic, mask, prop)
InputContext ic;
unsigned long mask;
StatusProps prop;
{
    char **p_name = (char **)prop;
    StatusAreaPriv data = (StatusAreaPriv)ic->status->data;

    if (mask & XIMP_STS_AREA_MASK) {
	prop->area.x = data->area.x;
	prop->area.y = data->area.y;
	prop->area.width = data->area.width;
	prop->area.height = data->area.height;
    }
    if (mask & XIMP_STS_FG_MASK) {
	prop->foreground = data->foreground;
    }
    if (mask & XIMP_STS_BG_MASK) {
	prop->background = data->background;
    }
    if (mask & XIMP_STS_COLORMAP_MASK) {
	prop->colormap = data->colormap;
    }
    if (mask & XIMP_STS_BGPIXMAP_MASK) {
	prop->background_pixmap = data->bg_pixmap;
    }
    if (mask & XIMP_STS_LINESP_MASK) {
	prop->line_spacing = data->line_spacing;
    }
    if (mask & XIMP_STS_CURSOR_MASK) {
	prop->cursor = data->cursor;
    }
    if (mask & XIMP_STS_AREANEED_MASK) {
	prop->area_needed.width = data->area_needed.width;
	prop->area_needed.height = data->area_needed.height;
    }
    if (mask & XIMP_STS_FONT_MASK) {
	*p_name = XBaseFontNameListOfFontSet(data->fontset);
    }
}

