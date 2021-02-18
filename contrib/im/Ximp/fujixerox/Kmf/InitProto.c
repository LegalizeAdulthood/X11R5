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
#include <X11/Xlocale.h>
#include <X11/Xatom.h>
#include <string.h>

Status
InitializeProtocol(dpy, owner, preedit, status, fore, back, server_name, server_version, proto_version, keys, font)
Display *dpy;
Window owner;
Window preedit;
Window status;
unsigned long fore, back;
char *server_name;
char *server_version;
char *proto_version;
Ximp_KeyList *keys;
XFontSet font;
{
    InputMethod im = IMOfSelectionOwner(dpy, owner);
    Window w;
    static char buf[256];
    char *s;
    XGCValues values;
    static XIMStyle style[] = {
	XIMPreeditCallbacks | XIMStatusCallbacks,
	XIMPreeditCallbacks | XIMStatusArea,
	XIMPreeditCallbacks | XIMStatusNothing,
	XIMPreeditPosition | XIMStatusCallbacks,
	XIMPreeditPosition | XIMStatusArea,
	XIMPreeditPosition | XIMStatusNothing,
	XIMPreeditNothing | XIMStatusNothing,
    };
    static XIMStyles styles = {
	sizeof(style) / sizeof(style[0]),
	style
    };
    extern DrawPreeditWindow();
    extern DrawStatusWindow();
    extern RegisterRedrawWindow();

    if (im) return(0); /* Another Input Method is own same window. */
    im = AllocIM();

    im->display = dpy;
    im->owner = owner;
    im->server_name = server_name;
    im->server_version = server_version;
    im->protocol_version = proto_version;
    im->styles = styles;
    im->preedit = preedit;
    RegisterRedrawWindow(dpy, preedit, im, DrawPreeditWindow);
    im->status = status;
    RegisterRedrawWindow(dpy, status, im, DrawStatusWindow);
    im->font = font;
    im->f_ext = XExtentsOfFontSet(font);
    values.function = GXcopy;
    values.foreground = im->fore = fore;
    values.background = im->back = back;
    im->gc = XCreateGC(im->display, im->preedit, GCFunction|GCForeground|GCBackground, &values);
    values.function = GXxor;
    values.foreground = (im->back ^ im->fore);
    im->invgc = XCreateGC(im->display, im->preedit, GCFunction|GCForeground, &values);
    im->locale_name = setlocale(LC_ALL, NULL);
    s = index(im->locale_name, '.');
    if (s) *s = '\0';
    sprintf(buf, "_XIMP_%s", im->locale_name);
    im->sel_simple = XInternAtom(dpy, buf, False);
    sprintf(buf, "_XIMP_%s@%s.0", im->locale_name, im->server_name);
    if (s) *s = '.';
    im->sel_detail = XInternAtom(dpy, buf, False);
    im->prop = AllocPropManager(owner);

    im->_version = XInternAtom(dpy, "_XIMP_VERSION", False);
    im->_style = XInternAtom(dpy, "_XIMP_STYLE", False);
    im->_keys = XInternAtom(dpy, "_XIMP_KEYS", False);
    im->_servername = XInternAtom(dpy, "_XIMP_SERVERNAME", False);
    im->_serverversion = XInternAtom(dpy, "_XIMP_SERVERVERSION", False);
    im->_protocol = XInternAtom(dpy, "_XIMP_PROTOCOL", False);
    im->_extensions = XInternAtom(dpy, "_XIMP_EXTENSIONS", False);
    im->_ext_ui_back_front = XInternAtom(dpy, "_XIMP_EXT_UI_BACK_FRONT", False);
    im->_focus = XInternAtom(dpy, "_XIMP_FOCUS", False);
    im->_preedit = XInternAtom(dpy, "_XIMP_PREEDIT", False);
    im->_status = XInternAtom(dpy, "_XIMP_STATUS", False);
    im->_preedit_font = XInternAtom(dpy, "_XIMP_PREEDITFONT", False);
    im->_status_font = XInternAtom(dpy, "_XIMP_STATUSFONT", False);
    im->_preeditdraw_data = XInternAtom(dpy, "_XIMP_PREEDITDRAW_DATA", False);
    im->_feedbacks = XInternAtom(dpy, "_XIMP_FEEDBACKS", False);

    XChangeProperty(dpy, owner, im->_version, XA_STRING, 8, PropModeReplace,
				    proto_version, strlen(proto_version));
    XChangeProperty(dpy, owner, im->_style, im->_style, 32, PropModeReplace,
				    style, sizeof(style) / sizeof(style[0]));
    XChangeProperty(dpy, owner, im->_keys, im->_keys, 32, PropModeReplace,
				    keys->keys_list, keys->count_keys * 3);
    XChangeProperty(dpy, owner, im->_servername, XA_STRING, 8, PropModeReplace,
				    server_name, strlen(server_name));
    XChangeProperty(dpy, owner, im->_serverversion, XA_STRING, 8, PropModeReplace,
				    server_version, strlen(server_version));
    XChangeProperty(dpy, owner, im->_extensions, XA_ATOM, 32, PropModeReplace,
				    &im->_ext_ui_back_front, 1);

    XGrabServer(dpy);
    if((w = XGetSelectionOwner(dpy, im->sel_simple)) != None) {
	XUngrabServer(dpy);
	FreeIM(im);
	return(1); /* Another imserver is running. */
    }
    XSetSelectionOwner(dpy, im->sel_detail, owner, CurrentTime);
    XSetSelectionOwner(dpy, im->sel_simple, owner, CurrentTime);
    XUngrabServer(dpy);

    return(0);
}
