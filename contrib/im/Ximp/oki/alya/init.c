/*
 * Copyright 1991 Oki Technosystems Laboratory, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Oki Technosystems Laboratory not
 * be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * Oki Technosystems Laboratory makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Oki TECHNOSYSTEMS LABORATORY DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL OKI TECHNOSYSTEMS LABORATORY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Yasuhiro Kawai,	Oki Technosystems Laboratory, Inc.
 *				kawai@otsl.oki.co.jp
 */
#ifndef lint
static char sccsid[] = "@(#)init.c	1.2";
#endif
/*
 * init.c
 *
 * @(#)init.c	1.2 91/09/28 17:12:15
 */
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>
#include "Ximp.h"

Atom XIMP_PROTOCOL;

Atom XIMP_VERSION;
Atom XIMP_STYLE;
Atom XIMP_KEYS;
Atom XIMP_SERVERNAME;
Atom XIMP_VENDORNAME;
Atom XIMP_SERVERVERSION;
Atom XIMP_EXTENSIONS;
Atom XIMP_PREEDIT;
Atom XIMP_STATUS;
Atom XIMP_FOCUS;
Atom XIMP_PREEDITFONT;
Atom XIMP_STATUSFONT;
Atom XIMP_PREEDIT_DRAW_DATA;
Atom XIMP_PREEDITDRAWLENGTH;
Atom XIMP_PREEDITDRAWSTRING;
Atom XIMP_PREEDITDRAWFEEDBACK;
Atom XIMP_FEEDBACKS;

Atom XIMP_EXT_UI_STATUSWINDOW;
Atom XIMP_EXT_UI_BACK_FRONT;

extern void prop_init();

IMSInit(w, servername)
    Widget w;
    String servername;
{
    Display *dpy;
    Atom atom;
    unsigned long style[5];
    unsigned long l[3];
    int i;
    char buf[100];

    dpy = XtDisplay(w);
    XIMP_PROTOCOL            = XInternAtom(dpy, _XIMP_PROTOCOL, FALSE);

    XIMP_VERSION             = XInternAtom(dpy, _XIMP_VERSION, FALSE);
    XIMP_STYLE               = XInternAtom(dpy, _XIMP_STYLE, FALSE);
    XIMP_KEYS                = XInternAtom(dpy, _XIMP_KEYS, FALSE);
    XIMP_SERVERNAME          = XInternAtom(dpy, _XIMP_SERVERNAME, FALSE);
    XIMP_VENDORNAME          = XInternAtom(dpy, _XIMP_VENDORNAME, FALSE);
    XIMP_SERVERVERSION       = XInternAtom(dpy, _XIMP_SERVERVERSION, FALSE);
    XIMP_EXTENSIONS          = XInternAtom(dpy, _XIMP_EXTENSIONS, FALSE);
    XIMP_PREEDIT             = XInternAtom(dpy, _XIMP_PREEDIT, FALSE);
    XIMP_STATUS              = XInternAtom(dpy, _XIMP_STATUS, FALSE);
    XIMP_FOCUS               = XInternAtom(dpy, _XIMP_FOCUS, FALSE);
    XIMP_PREEDITFONT         = XInternAtom(dpy, _XIMP_PREEDITFONT, FALSE);
    XIMP_STATUSFONT          = XInternAtom(dpy, _XIMP_STATUSFONT, FALSE);
    XIMP_PREEDIT_DRAW_DATA   = XInternAtom(dpy, _XIMP_PREEDIT_DRAW_DATA, FALSE);
    XIMP_PREEDITDRAWLENGTH   = XInternAtom(dpy, _XIMP_PREEDITDRAWLENGTH, FALSE);
    XIMP_PREEDITDRAWSTRING   = XInternAtom(dpy, _XIMP_PREEDITDRAWSTRING, FALSE);
    XIMP_PREEDITDRAWFEEDBACK = XInternAtom(dpy, _XIMP_PREEDITDRAWFEEDBACK, FALSE);
    XIMP_FEEDBACKS           = XInternAtom(dpy, _XIMP_FEEDBACKS, FALSE);
#ifdef notdef
    XIMP_EXT_UI_STATUSWINDOW = XInternAtom(dpy, _XIMP_EXT_UI_STATUSWINDOW, FALSE);
    XIMP_EXT_UI_BACK_FRONT   = XInternAtom(dpy, _XIMP_EXT_UI_BACK_FRONT, FALSE);
#endif
    
    atom = XInternAtom(dpy, servername, FALSE);
    if (XtOwnSelection(w, atom, CurrentTime, NULL, NULL, NULL) == FALSE) {
	fprintf(stderr, "OwnSelection failed.\n");
    }
    atom = XInternAtom(dpy, "_XIMP_ja_JP@OKI.0", FALSE);
    if (XtOwnSelection(w, atom, CurrentTime, NULL, NULL, NULL) == FALSE) {
	fprintf(stderr, "OwnSelection failed.\n");
    }

    XChangeProperty(dpy, XtWindow(w), XIMP_VERSION, XA_STRING,
	            8, PropModeReplace, XIMP_PROTOCOL_VERSION,
		    strlen(XIMP_PROTOCOL_VERSION));

    style[0] = XIMPreeditPosition  | XIMStatusArea;
/*
    style[1] = XIMPreeditPosition  | XIMStatusCallbacks;
    style[2] = XIMPreeditCallbacks | XIMStatusArea;
    style[3] = XIMPreeditCallbacks | XIMStatusCallbacks;
*/
    XChangeProperty(dpy, XtWindow(w), XIMP_STYLE, XIMP_STYLE,
	            32, PropModeReplace, (char *)style, 1);

    l[0] = ShiftMask;
    l[1] = ShiftMask;
    l[2] = XK_space;
    XChangeProperty(dpy, XtWindow(w), XIMP_KEYS, XIMP_KEYS,
	            32, PropModeReplace, l, 3);

    XChangeProperty(dpy, XtWindow(w), XIMP_SERVERNAME, XA_STRING,
	            8, PropModeReplace, SERVERNAME, strlen(SERVERNAME));

    XChangeProperty(dpy, XtWindow(w), XIMP_VENDORNAME, XA_STRING,
	            8, PropModeReplace, SERVERNAME, strlen(SERVERNAME));

    XChangeProperty(dpy, XtWindow(w), XIMP_SERVERVERSION, XA_STRING,
	            8, PropModeReplace, SERVERVERSION, strlen(SERVERVERSION));

    XChangeProperty(dpy, XtWindow(w), XIMP_EXTENSIONS, XIMP_EXTENSIONS,
	            32, PropModeReplace, "", 0);
    init_prop(dpy);
}
