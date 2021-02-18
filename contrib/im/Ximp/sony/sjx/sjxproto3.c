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
 *	sjxproto3.c
 *	SJX Protocol Version 3 Routines
 *	Copyright (c) 1988 1989 1990 1991 Sony Corporation.
 *	Written by Masaki Takeuchi
 *	Thu Mar 28 02:14:17 JST 1991
 *
 *	This routines support XIM
 */

#include "sjx.h"

#ifdef PROTO3
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include "XIMProto.h"
#include "XIMlib.h"

extern Display	*dpy;
extern Window	subwin;

char	*xip_conversion = "_XIMP_ja_JP";
char	*xip_conversion_sony = "_XIMP_ja_JP@sony";
Atom	xip_owner;
Atom	xip_owner_sony;
Atom	xip_protocol;
Atom	xip_version;
Atom	xip_style;
Atom	xip_keys;
Atom	xip_server_name;
Atom	xip_server_version;
Atom	xip_vendor_name;
Atom	xip_extensions;
Atom	xip_focus;
Atom	xip_preedit;
Atom	xip_status;
Atom	xip_preeditfont;
Atom	xip_statusfont;
/*
Atom	xip_preeditmaxsize;
*/

/*
 * private used by xim_init ()
 */

static void
xim_selection_init ()
{
#ifndef	DEBUG	/* makoto 1991.9.5 */
	static Time	time;

	xip_owner = XInternAtom (dpy, xip_conversion, False);
	XSetSelectionOwner (dpy, xip_owner, subwin, time);
	xip_owner_sony = XInternAtom (dpy, xip_conversion_sony, False);
	XSetSelectionOwner (dpy, xip_owner_sony, subwin, time);
#else
	xip_owner = XInternAtom (dpy, xip_conversion, False);
	XSetSelectionOwner (dpy, xip_owner, subwin, CurrentTime);
	xip_owner_sony = XInternAtom (dpy, xip_conversion_sony, False);
	XSetSelectionOwner (dpy, xip_owner_sony, subwin, CurrentTime);
#endif
}

static void
xim_atom_init ()
{
	xip_protocol       = XInternAtom (dpy, _XIMP_PROTOCOL, False);
	xip_version        = XInternAtom (dpy, _XIMP_VERSION, False);
	xip_style          = XInternAtom (dpy, _XIMP_STYLE, False);
	xip_keys           = XInternAtom (dpy, _XIMP_KEYS, False);
	xip_server_name    = XInternAtom (dpy, _XIMP_SERVERNAME, False);
	xip_server_version = XInternAtom (dpy, _XIMP_SERVERVERSION, False);
	xip_vendor_name    = XInternAtom (dpy, _XIMP_VENDORNAME, False);
	xip_extensions     = XInternAtom (dpy, _XIMP_EXTENSIONS, False);
	xip_focus          = XInternAtom (dpy, _XIMP_FOCUS, False);
	xip_preedit        = XInternAtom (dpy, _XIMP_PREEDIT, False);
	xip_status         = XInternAtom (dpy, _XIMP_STATUS, False);
	xip_preeditfont    = XInternAtom (dpy, _XIMP_PREEDITFONT, False);
	xip_statusfont     = XInternAtom (dpy, _XIMP_STATUSFONT, False);
/*
	xip_preeditmaxsize = XInternAtom (dpy, _XIMP_PREEDITMAXSIZE, False);
*/
}

	/* private used by xim_set_atom */

static void
_set_atom_char (prop, s)
Atom	prop;
char	*s;
{
	XChangeProperty (dpy, subwin, prop, XA_STRING, 8,
		PropModeReplace, s, strlen (s));
}

static void
_set_atom_extensions (prop)
Atom	prop;
{
	long	data_l[3];

	data_l[0] = 0;
	XChangeProperty (dpy, subwin, prop, prop, 32,
		PropModeReplace, data_l, 1);
}

static void
_set_atom_style (prop)
Atom	prop;
{
	long	data_l[3];

	data_l[0] = XIMPreeditNothing | XIMStatusNothing;
	XChangeProperty (dpy, subwin, prop, prop, 32,
		PropModeReplace, data_l, 1);
}

static void
_set_atom_keys (prop)
Atom	prop;
{
	long	data_l[6];

	data_l[0] = 0;
	data_l[1] = 0;
	data_l[2] = XK_Kanji;
#ifdef NOTDEF
	data_l[3] = 0;
	data_l[4] = 0;
	data_l[5] = XK_F10;
#endif
	XChangeProperty (dpy, subwin, prop, prop, 32,
		PropModeReplace, data_l, 6);
}

	/* xim_set_atom */

static void
xim_set_atom ()
{
	_set_atom_extensions (xip_extensions);
	_set_atom_style (xip_style);
	_set_atom_keys (xip_keys);
	_set_atom_char (xip_protocol, "XIMP.3.4");
	_set_atom_char (xip_server_name, "SJX");
	_set_atom_char (xip_server_version, "SJX.2.31X");
	_set_atom_char (xip_version, "XIMP.3.4");
	_set_atom_char (xip_vendor_name, "SONY");
}

/*
 * xim_init ()
 */

xim_init ()
{
	xim_selection_init ();
	xim_atom_init ();
	xim_set_atom ();
}

/*
 * xim_receive
 */

#ifndef DEBUG
#define printf(x)	;
#endif /* !DEBUG */

xim_receive (ev)
XClientMessageEvent	*ev;
{
	switch (ev->data.l[0]) {
		case XIMP_KEYPRESS:
printf ("XIMP_KEYPRESS\n");
			XipKeypress (ev);
			break;
		case XIMP_CREATE:
printf ("XIMP_CREATE\n");
			XipCreate (ev);
			break;
		case XIMP_DESTROY:
printf ("XIMP_DESTROY\n");
			XipDestroy (ev);
			break;
		case XIMP_BEGIN:
printf ("XIMP_BEGIN\n");
			XipBegin (ev);
			break;
		case XIMP_END:
printf ("XIMP_END\n");
			XipEnd (ev);
			break;
		case XIMP_SETFOCUS:
printf ("XIMP_SETFOCUS\n");
			XipSetfocus (ev);
			break;
		case XIMP_UNSETFOCUS:
printf ("XIMP_UNSETFOCUS\n");
			XipUnsetfocus (ev);
			break;
		case XIMP_CHANGE:
printf ("XIMP_CHANGE\n");
			XipChange (ev);
			break;
		case XIMP_MOVE:
printf ("XIMP_MOVE\n");
			XipMove (ev);
			break;
		case XIMP_RESET:
printf ("XIMP_RESET\n");
			XipReset (ev);
			break;
		case XIMP_SETVALUE:
printf ("XIMP_SETVALUE\n");
			XipSetValue (ev);
			break;
		case XIMP_GETVALUE:
printf ("XIMP_GETVALUE\n");
			XipGetValue (ev);
			break;
		case XIMP_GEOMETRY:
printf ("XIMP_GEOMETRY\n");
			XipGeometry (ev);
			break;
		case XIMP_PREEDITSTART:
printf ("XIMP_PREEDITSTART\n");
			XipPreeditstart (ev);
			break;
#ifdef NOTDEF
		case XIMP_PREEDITMAXSIZE:
printf ("XIMP_PREEDITMAXSIZE\n");
			XipPreeditmaxsize (ev);
			break;
#endif
		case XIMP_PREEDITDONE:
printf ("XIMP_PREEDITDONE\n");
			XipPreeditdone (ev);
			break;
		case XIMP_PREEDITDRAW:
printf ("XIMP_PREEDITDRAW\n");
			XipPreeditdraw (ev);
			break;
		case XIMP_PREEDITCARET:
printf ("XIMP_PREEDITCARET\n");
			XipPreeditcaret (ev);
			break;
#ifdef NOTDEF
		case XIMP_CARET_POS:
printf ("XIMP_CARET_POS\n");
			XipCaretPos (ev);
			break;
#endif
		case XIMP_STATUSSTART:
printf ("XIMP_STATUSSTART\n");
			XipStatusstart (ev);
			break;
		case XIMP_STATUSDONE:
printf ("XIMP_STATUSDONE\n");
			XipStatusdone (ev);
			break;
		case XIMP_STATUSDRAW:
printf ("XIMP_STATUSDRAW\n");
			XipStatusdraw (ev);
			break;
		case XIMP_EXTENSION:
printf ("XIMP_EXTENSION\n");
			XipExtension (ev);
			break;
		default:
printf ("Unsupported Protocol\n");
			break;
	}
}
#endif /* PROTO3 */
