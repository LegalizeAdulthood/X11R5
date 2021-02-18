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
    int width;
    XRectangle *hilight;
    int num_hilight;
    /* ICValues */
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
    XRectangle spot;
} PreeditPosPrivateRec, *PreeditPosPriv;

DrawPreeditPos(d, w, ic)
Display *d;
Window w;
InputContext ic;
{
    XIMText *text;
    PreeditPosPriv data = (PreeditPosPriv)ic->preedit->data;
    int i;

    if (!(text = ic->preedit->text)) return;
    if (text->encoding_is_wchar) {
	XwcDrawImageString(d, w, data->fontset, data->gc,
		-data->f_ext->max_logical_extent.x,
		-data->f_ext->max_logical_extent.y,
		text->string.wide_char, text->length);
    } else {
	XmbDrawImageString(d, w, data->fontset, data->gc,
		-data->f_ext->max_logical_extent.x,
		-data->f_ext->max_logical_extent.y,
		text->string.multi_byte,
		strlen(text->string.multi_byte));
    }
    XFillRectangles(d, w, data->invgc, data->hilight, data->num_hilight);
}

AllocPrePos(ic)
InputContext ic;
{
    PreeditPosPriv data;
    char **miss_list, *default_str;
    int miss_list_count;

    AllocPreNot(ic);
    data = ic->preedit->data = XAlloc(PreeditPosPrivateRec);
    data->methods = ic->preedit->methods;
    ic->preedit->methods = &methods;
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
    PreeditPosPriv data = (PreeditPosPriv)ic->preedit->data;
    XGCValues values;

    data->window = XCreateSimpleWindow(ic->im->display, ic->focus,
		data->spot.x, data->spot.y + data->f_ext->max_logical_extent.y,
		data->area.width, data->area.height,
		0, data->background, data->background);
    data->width = data->area.width;
    values.function = GXcopy;
    values.foreground = data->foreground;
    values.background = data->background;
    data->gc = XCreateGC(ic->im->display, data->window, GCFunction|GCForeground|GCBackground, &values);
    values.function = GXxor;
    values.foreground = (data->background ^ data->foreground);
    data->invgc = XCreateGC(ic->im->display, data->window, GCFunction|GCForeground, &values);
    XSelectInput(ic->im->display, data->window, ExposureMask);
    RegisterRedrawWindow(ic->im->display, data->window, ic, DrawPreeditPos);
}

static Bool
Destroy(ic, event_generated)
InputContext ic;
Bool event_generated;
{
    PreeditPosPriv data = (PreeditPosPriv)ic->preedit->data;
    XEvent ev;

    XGrabServer(ic->im->display);
    if ((!event_generated) &&
	(!XCheckIfEvent(ic->im->display, &ev, CheckDestroyNotify, ic->client))) {
	XDestroyWindow(ic->im->display, data->window);
    }
    XUngrabServer(ic->im->display);
    XFlush(ic->im->display);
    FreeXIMText(ic->preedit->text);
    if (data->num_hilight) Xfree(data->hilight);
    XFreeGC(ic->im->display, data->gc);
    XFreeGC(ic->im->display, data->invgc);
    XFreeFontSet(ic->im->display, data->fontset);
    Xfree(data);
    Xfree(ic->preedit);
}

static Bool
Draw(ic, data)
InputContext ic;
XIMText *data;
{
    PreeditPosPriv priv = (PreeditPosPriv)ic->preedit->data;
    int width;

    (*priv->methods->draw)(ic, data);
    if (priv->num_hilight) Xfree(priv->hilight);
    priv->num_hilight = 0;
    if (data->length) {
	XMapWindow(ic->im->display, priv->window);
    } else {
	XUnmapWindow(ic->im->display, priv->window);
    }
    if (data->encoding_is_wchar) {
	width = XwcTextEscapement(priv->fontset,
		data->string.wide_char, data->length);
    } else {
	width = XmbTextEscapement(priv->fontset,
		data->string.multi_byte, strlen(data->string.multi_byte));
    }
    if (data->feedback) {
	XRectangle *logical_extents = (XRectangle *)Xmalloc(sizeof(XRectangle) * data->length);
	XRectangle *ink_extents = (XRectangle *)Xmalloc(sizeof(XRectangle) * data->length);
	int i, startPos, pos, num_chars;
	XIMFeedback origin;

	if (data->encoding_is_wchar) {
	    XwcTextPerCharExtents(priv->fontset, data->string.wide_char, data->length, ink_extents, logical_extents, data->length, &num_chars, NULL, NULL);
	} else {
	    XmbTextPerCharExtents(priv->fontset, data->string.multi_byte, strlen(data->string.multi_byte), ink_extents, logical_extents, data->length, &num_chars, NULL, NULL);
	}
	origin = (data->feedback[0] & XIMReverse);
	startPos = pos = -priv->f_ext->max_logical_extent.x;
	for (i = 0; i < data->length; i++) {
	    if (origin != (data->feedback[i] & XIMReverse)) {
		if (origin) {
		    XRectangle *rect;
		    if (priv->num_hilight) {
			priv->hilight = (XRectangle *)Xrealloc(priv->hilight, sizeof(XRectangle) * (priv->num_hilight + 1));
		    } else {
			priv->hilight = XAlloc(XRectangle);
		    }
		    rect = &priv->hilight[priv->num_hilight];
		    rect->x = startPos;
		    rect->y = 0;
		    rect->width = pos - startPos;
		    rect->height = priv->f_ext->max_logical_extent.height;
		    priv->num_hilight++;
		}
		startPos = pos;
		origin = (data->feedback[i] & XIMReverse);
	    }
	    pos += logical_extents[i].width;
	}
	if (origin) {
	    XRectangle *rect;
	    if (priv->num_hilight) {
		priv->hilight = (XRectangle *)Xrealloc(priv->hilight, sizeof(XRectangle) * (priv->num_hilight + 1));
	    } else {
		priv->hilight = XAlloc(XRectangle);
	    }
	    rect = &priv->hilight[priv->num_hilight];
	    rect->x = startPos;
	    rect->y = 0;
	    rect->width = pos - startPos;
	    rect->height = priv->f_ext->max_logical_extent.height;
	    priv->num_hilight++;
	}
	Xfree(logical_extents);
	Xfree(ink_extents);
    }
    if (width == 0) width = 1;
    XResizeWindow(ic->im->display, priv->window,
		width, priv->f_ext->max_logical_extent.height);
    if (width == priv->width) XClearArea(ic->im->display, priv->window, 0, 0, 0, 0, True);
    priv->width = width;
    XClearArea(ic->im->display, ic->im->preedit, 0, 0, 0, 0, True);
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
    PreeditPosPriv data = (PreeditPosPriv)ic->preedit->data;
    char *base_fontset_name_list = (char *)prop;
    char **miss_list, *default_str;
    int miss_list_count;
    Display *d = ic->im->display;
    Window w = data->window;
    XGCValues values;

    if (mask & XIMP_PRE_SPOTL_MASK) {
	data->spot.x = prop->spot_location.x;
	data->spot.y = prop->spot_location.y;
	if (w) XMoveWindow(d, w, data->spot.x, data->spot.y + data->f_ext->max_logical_extent.y);
    }
    if (mask & XIMP_PRE_AREA_MASK) {
	data->area.x = prop->area.x;
	data->area.y = prop->area.y;
	data->area.width = prop->area.width;
	data->area.height = prop->area.height;
    }
    if (mask & XIMP_PRE_FG_MASK) {
	data->foreground = prop->foreground;
	if (w) {
	    values.function = GXcopy;
	    values.foreground = data->foreground;
	    values.background = data->background;
	    XChangeGC(d, data->gc, GCFunction|GCForeground|GCBackground, &values);
	    values.function = GXxor;
	    values.foreground = (data->background ^ data->foreground);
	    XChangeGC(d, data->invgc, GCFunction|GCForeground, &values);
	}
    }
    if (mask & XIMP_PRE_BG_MASK) {
	data->background = prop->background;
	if (w) {
	    values.function = GXcopy;
	    values.foreground = data->foreground;
	    values.background = data->background;
	    XChangeGC(d, data->gc, GCFunction|GCForeground|GCBackground, &values);
	    values.function = GXxor;
	    values.foreground = (data->background ^ data->foreground);
	    XChangeGC(d, data->invgc, GCFunction|GCForeground, &values);
	}
    }
    if (mask & XIMP_PRE_COLORMAP_MASK) {
	data->colormap = prop->colormap;
	if (w) XSetWindowColormap(d, w, data->colormap);
    }
    if (mask & XIMP_PRE_BGPIXMAP_MASK) {
	data->bg_pixmap = prop->background_pixmap;
    }
    if (mask & XIMP_PRE_LINESP_MASK) {
	data->line_spacing = prop->line_spacing;
    }
    if (mask & XIMP_PRE_CURSOR_MASK) {
	data->cursor = prop->cursor;
	if (w) XDefineCursor(d, w, data->cursor);
    }
    if (mask & XIMP_PRE_AREANEED_MASK) {
	data->area_needed.width = prop->area_needed.width;
	data->area_needed.height = prop->area_needed.height;
    }
    if (mask & XIMP_PRE_FONT_MASK) {
	if (data->fontset) XFreeFontSet(d, data->fontset);
	data->fontset = XCreateFontSet(d,
		    base_fontset_name_list,
		    &miss_list, &miss_list_count, &default_str);
	data->f_ext = XExtentsOfFontSet(data->fontset);
    }
}

static Bool
GetValue(ic, mask, prop)
InputContext ic;
unsigned long mask;
PreeditProps prop;
{
    char **p_name = (char **)prop;
    PreeditPosPriv data = (PreeditPosPriv)ic->preedit->data;

    if (mask & XIMP_PRE_SPOTL_MASK) {
	prop->spot_location.x = data->spot.x;
	prop->spot_location.y = data->spot.y;
    }
    if (mask & XIMP_PRE_AREA_MASK) {
	prop->area.x = data->area.x;
	prop->area.y = data->area.y;
	prop->area.width = data->area.width;
	prop->area.height = data->area.height;
    }
    if (mask & XIMP_PRE_FG_MASK) {
	prop->foreground = data->foreground;
    }
    if (mask & XIMP_PRE_BG_MASK) {
	prop->background = data->background;
    }
    if (mask & XIMP_PRE_COLORMAP_MASK) {
	prop->colormap = data->colormap;
    }
    if (mask & XIMP_PRE_BGPIXMAP_MASK) {
	prop->background_pixmap = data->bg_pixmap;
    }
    if (mask & XIMP_PRE_LINESP_MASK) {
	prop->line_spacing = data->line_spacing;
    }
    if (mask & XIMP_PRE_CURSOR_MASK) {
	prop->cursor = data->cursor;
    }
    if (mask & XIMP_PRE_AREANEED_MASK) {
	prop->area_needed.width = data->area_needed.width;
	prop->area_needed.height = data->area_needed.height;
    }
    if (mask & XIMP_PRE_FONT_MASK) {
	*p_name = XBaseFontNameListOfFontSet(data->fontset);
    }
}

