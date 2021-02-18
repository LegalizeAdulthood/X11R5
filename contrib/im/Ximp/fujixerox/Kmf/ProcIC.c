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

static Bool createIC(), destroyIC(), beginIC(), endIC();
static Bool getvalueIC(), setvalueIC(), setfocusIC(), unsetfocusIC();
static Bool resetIC(), getKey(), moveIC();
static Bool sendKey(), sendString(), resetReturnIC();
static Bool proc_begin(), proc_end();

static ICMethodsRec ICMethod = {
    createIC,
    destroyIC,
    beginIC,
    endIC,
    getvalueIC,
    setvalueIC,
    setfocusIC,
    unsetfocusIC,
    resetIC,
    getKey,
    moveIC,
    sendKey,
    sendString,
    resetReturnIC,
    proc_begin,
    proc_end
};

InputContext
AllocIC(im)
InputMethod im;
{
    InputContext ic = (InputContext)XAllocFreeArea(sizeof(InputContextRec));

    ic->next = im->ic;
    ic->methods = &ICMethod;
    ic->im = im;
    im->ic = ic;
    return(ic);
}

static Bool
getKey(ic, ev)
InputContext ic;
XKeyEvent *ev;
{
    (*ic->private->methods->keyin)(ic, ev);
}

static Bool
createIC(ic, mask)
InputContext ic;
unsigned long mask;
{
    switch (ic->style & XIMPreeditStyles) {
    case XIMPreeditCallbacks:
	AllocPreCal(ic);
	break;
    case XIMPreeditPosition:
	AllocPrePos(ic);
	break;
    case XIMPreeditArea:
	/* AllocPreArea(ic); */
	break;
    case XIMPreeditNothing:
	AllocPreNot(ic);
	break;
    case XIMPreeditNone:
	break;
    }
    switch (ic->style & XIMStatusStyles) {
    case XIMStatusCallbacks:
	AllocStCal(ic);
	break;
    case XIMStatusArea:
	AllocStArea(ic);
	break;
    case XIMStatusNothing:
	AllocStNot(ic);
	break;
    case XIMStatusNone:
	break;
    }
    AllocInputMethodPrivate(ic);
    setvalueIC(ic, mask);
    _ximp_create_return(ic);
    (*ic->preedit->methods->create)(ic);
    (*ic->status->methods->create)(ic);
    (*ic->private->methods->create)(ic);
}

static Bool
destroyIC(ic, event_generated)
InputContext ic;
Bool event_generated;
{
    InputMethod im = ic->im;
    InputContext p, *q;

    if (!ic) return;

    (*ic->preedit->methods->destroy)(ic, event_generated);
    (*ic->status->methods->destroy)(ic, event_generated);
    (*ic->private->methods->destroy)(ic, event_generated);

    for (q = &im->ic; p = *q; q = &p->next) {
	if (p == ic) {
	    *q = p->next;
	    Xfree(ic);
	    break;
	}
    }
}

static Bool
beginIC(ic)
InputContext ic;
{
    if (ic->is_bep == XIMP_FRONTEND) {
	XSelectInput(ic->im->display, ic->focus, KeyPressMask | StructureNotifyMask);
    }
    ic->intercepting = True;
    (*ic->private->methods->begin)(ic);
    setfocusIC(ic);
}

static Bool
endIC(ic)
InputContext ic;
{
    if (ic->is_bep == XIMP_FRONTEND) {
	XSelectInput(ic->im->display, ic->focus, StructureNotifyMask);
    }
    ic->intercepting = False;
    (*ic->private->methods->end)(ic);
}

static Bool
setfocusIC(ic)
InputContext ic;
{
    ic->im->focused_ic = ic;
    XClearArea(ic->im->display, ic->im->preedit, 0, 0, 0, 0, True);
    XClearArea(ic->im->display, ic->im->status, 0, 0, 0, 0, True);
}

static Bool
unsetfocusIC(ic)
InputContext ic;
{
    if (ic->im->focused_ic == ic) {
	ic->im->focused_ic = (InputContext)NULL;
    }
    XClearWindow(ic->im->display, ic->im->preedit);
    XClearWindow(ic->im->display, ic->im->status);
}

static Bool
moveIC(ic, x, y)
InputContext ic;
int x, y;
{
    static PreeditPropsRec prop;
    prop.spot_location.x = x;
    prop.spot_location.y = y;
    (*ic->preedit->methods->setvalue)(ic, XIMP_PRE_SPOTL_MASK, &prop);
}

static Bool
resetIC(ic)
InputContext ic;
{
    (*ic->private->methods->reset)(ic);
}

static Bool
setvalueIC(ic, mask)
InputContext ic;
unsigned long mask;
{
    InputMethod im = ic->im;
    int i;
    long *l_prop;
    char *b_prop;
    Atom act_type;
    unsigned long act_format, nitems, after;

    if (mask & XIMP_FOCUS_WIN_MASK) {
	if (XGetWindowProperty(im->display, ic->client, im->_focus,
				0, 1, True, XA_WINDOW,
				&act_type, &act_format,
				&nitems, &after, &l_prop) == Success) {
	    if (ic->is_bep == XIMP_FRONTEND) {
		if (ic->focus) {
		    XSelectInput(ic->im->display, ic->focus, NULL);
		}
	    }
	    ic->focus = (Window)*l_prop;
	    if (ic->is_bep == XIMP_FRONTEND) {
		if (ic->intercepting) {
		    XSelectInput(ic->im->display, ic->focus, KeyPressMask | StructureNotifyMask);
		} else {
		    XSelectInput(ic->im->display, ic->focus, StructureNotifyMask);
		}
	    }
	    Xfree(l_prop);
	}
    }
    if (mask & XIMP_PROP_PREEDIT) {
	if (XGetWindowProperty(im->display, ic->client, im->_preedit,
				0, 1000, True, im->_preedit,
				&act_type, &act_format,
				&nitems, &after, &l_prop) == Success) {
	    (*ic->preedit->methods->setvalue)(ic, mask & XIMP_PROP_PREEDIT, l_prop);
	    Xfree(l_prop);
	}
    }
    if (mask & XIMP_PROP_STATUS) {
	if (XGetWindowProperty(im->display, ic->client, im->_status,
				0, 1000, True, im->_status,
				&act_type, &act_format,
				&nitems, &after, &l_prop) == Success) {
	    (*ic->status->methods->setvalue)(ic, mask & XIMP_PROP_STATUS, l_prop);
	    Xfree(l_prop);
	}
    }
    if (mask & XIMP_PRE_FONT_MASK) {
	if (XGetWindowProperty(im->display, ic->client, im->_preedit_font,
				0, 1000, True, XA_STRING,
				&act_type, &act_format,
				&nitems, &after, &b_prop) == Success) {
	    (*ic->preedit->methods->setvalue)(ic, mask & XIMP_PRE_FONT_MASK, b_prop);
	    Xfree(b_prop);
	}
    }
    if (mask & XIMP_STS_FONT_MASK) {
	if (XGetWindowProperty(im->display, ic->client, im->_status_font,
				0, 1000, True, XA_STRING,
				&act_type, &act_format,
				&nitems, &after, &b_prop) == Success) {
	    (*ic->status->methods->setvalue)(ic, mask & XIMP_STS_FONT_MASK, b_prop);
	    Xfree(b_prop);
	}
    }
}

static Bool
getvalueIC(ic, mask)
InputContext ic;
unsigned long mask;
{
    InputMethod im = ic->im;
    int i;
    long *l_prop;
    char **b_prop;
    Atom act_type;
    unsigned long act_format, nitems, after;

    if (mask & XIMP_FOCUS_WIN_MASK) {
	XChangeProperty(im->display, im->owner, im->_focus, XA_WINDOW, 32, PropModeReplace, &ic->focus, 1);
    }
    if (mask & XIMP_PROP_PREEDIT) {
	l_prop = (long *)Xmalloc(sizeof(PreeditPropsRec));
	(*ic->preedit->methods->getvalue)(ic, mask & XIMP_PROP_PREEDIT, l_prop);
	XChangeProperty(im->display, im->owner,
		im->_preedit, im->_preedit,
		32, PropModeReplace, l_prop, sizeof(PreeditPropsRec) / 4);
	Xfree(l_prop);
    }
    if (mask & XIMP_PROP_STATUS) {
	l_prop = (long *)Xmalloc(sizeof(StatusPropsRec));
	(*ic->status->methods->getvalue)(ic, mask & XIMP_PROP_STATUS, l_prop);
	XChangeProperty(im->display, im->owner,
		im->_status, im->_status,
		32, PropModeReplace, l_prop, sizeof(StatusPropsRec) / 4);
	Xfree(l_prop);
    }
    if (mask & XIMP_PRE_FONT_MASK) {
	(*ic->preedit->methods->setvalue)(ic, mask & XIMP_PRE_FONT_MASK, b_prop);
	XChangeProperty(im->display, im->owner,
		im->_preedit_font, im->_preedit_font,
		8, PropModeReplace, *b_prop, strlen(*b_prop));
    }
    if (mask & XIMP_STS_FONT_MASK) {
	(*ic->status->methods->setvalue)(ic, mask & XIMP_STS_FONT_MASK, b_prop);
	XChangeProperty(im->display, im->owner,
		im->_status_font, im->_status_font,
		8, PropModeReplace, *b_prop, strlen(*b_prop));
    }
    _ximp_getvalue_return(ic);
}

static Bool
sendKey(ic, ev)
InputContext ic;
XKeyEvent *ev;
{
    _ximp_send_keypress(ic, ev);
}

static Bool
sendString(ic, text)
InputContext ic;
XIMText *text;
{
    Atom atom = GetPropAtom(ic);
    XTextProperty prop;

    if (text->encoding_is_wchar) {
	XwcTextListToTextProperty(ic->im->display,
		&text->string.wide_char, 1, XCompoundTextStyle, &prop);
    } else {
	XmbTextListToTextProperty(ic->im->display,
		&text->string.multi_byte, 1, XCompoundTextStyle, &prop);
    }
    XSetTextProperty(ic->im->display, ic->im->owner, &prop, atom);
    _ximp_readprop(ic, atom);
}

static Bool
resetReturnIC(ic, text)
InputContext ic;
XIMText *text;
{
    Atom atom = GetPropAtom(ic);
    XTextProperty prop;

    if (text->encoding_is_wchar) {
	XwcTextListToTextProperty(ic->im->display,
		&text->string.wide_char, 1, XCompoundTextStyle, &prop);
    } else {
	XmbTextListToTextProperty(ic->im->display,
		&text->string.multi_byte, 1, XCompoundTextStyle, &prop);
    }
    XSetTextProperty(ic->im->display, ic->im->owner, &prop, atom);
    _ximp_reset_return(ic, atom);
}

static Bool
proc_begin(ic)
InputContext ic;
{
    if (ic->is_bep == XIMP_FRONTEND) {
	XSelectInput(ic->im->display, ic->focus, KeyPressMask | StructureNotifyMask);
    }
    ic->intercepting = True;
    _ximp_conversion_begin(ic);
}

static Bool
proc_end(ic)
InputContext ic;
{
    if (ic->is_bep == XIMP_FRONTEND) {
	XSelectInput(ic->im->display, ic->focus, StructureNotifyMask);
    }
    ic->intercepting = False;
    _ximp_conversion_end(ic);
}
