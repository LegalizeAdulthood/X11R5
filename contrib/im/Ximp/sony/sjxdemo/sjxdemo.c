/*
 * Copyright 1991 Sony Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Sony not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Sony makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SONY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SONY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Masaki Takeuchi, Sony Corporation
 */
/*
 *	sjxdemo.c
 *
 *	Copyright (c) 1989 1990 1991 Sony Corporation
 */
/*
 * $Header: sjxdemo.c,v 1.1 91/04/29 17:57:15 masaki Exp $ SONY;
 */

#include <stdio.h>
#include "Xlib.h"
#include "Xlocale.h"
#include "Xos.h"
#include "Xutil.h"
#include "keysym.h"

#include "xcommon.h"

#define SELECT (StructureNotifyMask|ExposureMask|KeyPressMask|ButtonPressMask|PropertyChangeMask|FocusChangeMask)

#define FONT8	"-*-fixed-*-r-normal--16-120-100-100-*-*-jisx0201.1976-0"
#define FONT16	"-*-fixed-*-r-normal--16-120-100-100-*-*-jisx0208.1983-0"

Font		font8, font16;
int		width, height;

main (argc, argv)
int	argc;
char	**argv;
{
	X_init (argc, argv);
	IM_init ();
	Xdispatch ();
	exit (0);
}

X_init (argc, argv)
int	argc;
char	**argv;
{
	dpy = XOpenDisplay (display);

	bwidth = 1;

	fore  = BlackPixel (dpy, DefaultScreen (dpy));
	back  = WhitePixel (dpy, DefaultScreen (dpy));

	mask = 0;
	attributes.background_pixel = back;
	mask |= CWBackPixel;
	attributes.bit_gravity = NorthWestGravity;
	mask |= CWBitGravity;

	set_resize_hint (dpy, &shint, geom);
	set_screen (shint.x, shint.y, shint.width, shint.height);

	width = shint.width;
	height = shint.height;

	win = XCreateWindow (
			dpy, DefaultRootWindow (dpy),
			shint.x, shint.y, shint.width, shint.height,
			bwidth, DefaultDepth (dpy, DefaultScreen (dpy)),
			InputOutput, CopyFromParent, mask, &attributes
		);

	XSetStandardProperties (
			dpy, win, "SJXDEMO", NULL, NULL,
			argv, argc, &shint
	);

	font8  = XLoadFont (dpy, FONT8);
	if (font8 == None) {
		font8  = XLoadFont (dpy, "8x16kana");
	}

	font16 = XLoadFont (dpy, FONT16);

	mask  = GCForeground | GCBackground | GCFont;
	gcv.foreground = fore;
	gcv.background = back;
	gcv.font = font8;
	gcFore   = XCreateGC (dpy, win, mask, &gcv);
	gcv.font = font16;
	gcFore16  = XCreateGC (dpy, win, mask, &gcv);
	gcv.foreground = back;
	gcv.background = fore;
	gcv.font = font8;
	gcBack   = XCreateGC (dpy, win, mask, &gcv);
	gcv.font = font16;
	gcBack16  = XCreateGC (dpy, win, mask, &gcv);

	SetFontInfo (gcFore16);

	XSelectInput (dpy, win, StructureNotifyMask );
	XMapWindow (dpy, win);
	XFlush (dpy);

	AllocScreen ();

	while (event.type != MapNotify) {
		XNextEvent(dpy, &event);
	}

	XSelectInput (dpy, win, SELECT);
}

XIM	im;
XIC	ic;

IM_init ()
{
	XVaNestedList  preedit_attr, status_attr;
	XIMStyle xim_mode =XIMPreeditNothing | XIMStatusNothing;
	char	*p;

	p = setlocale (LC_CTYPE, "");
	im = XOpenIM (dpy, NULL, NULL, NULL);
	if (im == NULL) {
		printf ("error: XOpenIM\n");
		exit (1);
	}
	preedit_attr = XVaCreateNestedList( NULL,
				XNForeground, 1,
				XNBackground, 0,
				XNFontSet, NULL,
				NULL);
	status_attr = XVaCreateNestedList( NULL,
				XNForeground, 1,
				XNBackground, 0,
				XNFontSet, NULL,
				NULL);
	ic = XCreateIC (im,
			XNInputStyle, xim_mode,
			XNClientWindow, win,
			XNFocusWindow, win,
			XNPreeditAttributes, preedit_attr,
			XNStatusAttributes, status_attr,
			NULL
		);
	if (ic == NULL) {
		printf ("error: XCreateIC\n");
		exit (1);
	}
}

Xdispatch ()
{
	while (1) {
		XNextEvent (dpy, &event);
		if (XFilterEvent (&event, NULL) == True)
			continue;
		switch (event.type) {
			case FocusIn:
				XSetICFocus (ic);
				break;
			case FocusOut:
				XUnsetICFocus (ic);
				break;
			case KeyPress:
				proc_keypress (&event);
				break;
			case Expose:
				proc_expose (&event);
				break;
			case ButtonPress:
				proc_buttonpress (&event);
				break;
			case ConfigureNotify:
				proc_configure (&event);
				break;
			case ClientMessage:
				proc_message (&event);
				break;
			default:
				proc_default (&event);
				break;
		}
		XFlush (dpy);
	}
}

proc_default (ev)
XAnyEvent	*ev;
{
}

proc_expose (ev)
XExposeEvent	*ev;
{
	RedrawScreen (dpy, win);
}

proc_buttonpress (ev)
XButtonEvent	*ev;
{
}

Window	owner_window = 0;

proc_keypress (ev)
XKeyEvent	*ev;
{
	char		buffer[2048];
	register int	len;
	KeySym		ks;
	XComposeStatus	xcs;
	static int	count = 0;
	Font		font1, font2;

	len = XmbLookupString (ic, ev, buffer, 2048, &ks, &xcs);
	if (len) {
		buffer[len] = 0;
		write_window (buffer, len);
	}
}

proc_configure (ev)
XConfigureEvent	*ev;
{
	proc_resize (ev);
}

proc_resize (ev)
XConfigureEvent	*ev;
{
	set_screen (ev->x, ev->y, ev->width, ev->height);
}

proc_message (ev)
XClientMessageEvent	*ev;
{
}
