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
 *	sjxproto2.c
 *	SJX Protocol Version 2 Routines
 *	Copyright (c) 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi
 *	Mon Jul 31 14:29:53 JST 1989
 *
 *	Written by Satoshi Obata for Using Property
 *	Fri Jan 06 10:30:00 JST 1989
 *
 *	Compatible JUS xxi protocol
 *
 *	Modified by Masaki Takeuchi for Xlc Protocol
 *	Modified by Masaki Takeuchi for on-the-spot Conversion
 *	Thu May 10 17:39:41 JST 1990
 */
/*
 * $Header: sjxproto2.c,v 1.2 91/09/30 21:17:35 root Exp $ SONY;
 */

#include "common.h"
#include "sjx.h"
#include "key.h"
#include "xcommon.h"

extern int	vflag;

extern int	inchar;
extern int	keylen;
extern u_char	keybuff[];
extern KeySym	keysym;
extern Window	inwindow;
extern int	input;
extern Atom	selection;

extern int	henkan_flag;

#ifdef PROTO2
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "Xlc.h"
#include "Xlcdef.h"

/*
 * Client Information Structure p_qelem Queue
 */
struct p_qelem {
	struct p_qelem	*q_forw;
	struct p_qelem	*q_back;
	Window		client_win;
	Window		input_win;
	int		in_unmap;
	Atom		client_text;
	Atom		client_prop;
	Atom		client_inplace;
	InplaceInfo	*in_info;
	Draws		*in_drws;
	Conversion	*in_conv;
};

typedef struct p_qelem	PropQ;

static int		prop_init_flag = 0;
static PropQ		prop_qelem;
static Atom		conv_request, conv_notify, conv_end;
static Atom		conv_end_request;
static Atom		conv_close;
static int		prop_num = 0;
static int		real_prop_num = 0;
static char		*default_prop = "_SONY_CONVERSION_STORE";
static Atom		conv_inplace;

Atom			conv_prop;
Atom			compound_text;

extern PropQ		*XipWindow (), *XipDrawWindow ();

PropQ *
search_client_window (window)
register Window	window;
{
	register PropQ	*p;

	p = &prop_qelem;
	if (p->client_win == window)
		return (p);
	for (p = prop_qelem.q_forw ; p != &prop_qelem ; p = p->q_forw) {
		if (p->client_win == window)
			return (p);
	}
	return ((PropQ *)0);
}

PropQ *
search_input_window (window)
register Window	window;
{
	register PropQ	*p;

	p = &prop_qelem;
	if (p->input_win == window)
		return (p);
	for (p = prop_qelem.q_forw ; p != &prop_qelem ; p = p->q_forw) {
		if (p->input_win == window)
			return (p);
	}
	return ((PropQ *)0);
}

PropQ *
search_drawing_set (d)
register DrawingSet	*d;
{
	register PropQ	*p;

	p = &prop_qelem;
	if (p->in_drws && p->in_drws->t_draw == d)
		return (p);
	for (p = prop_qelem.q_forw ; p != &prop_qelem ; p = p->q_forw) {
		if (p->in_drws && p->in_drws->t_draw == d)
			return (p);
	}
	return ((PropQ *)0);
}

PropQ *
search_draw_window (window)
register Window	window;
{
	register PropQ		*p;
	register DrawingSet	*d;

	p = &prop_qelem;
	if (p->in_drws) {
		d = p->in_drws->t_draw;
		if (d->window == window)
			return (p);
		else if (d->ex_window == window)
			return (p);
	}
	for (p = prop_qelem.q_forw ; p != &prop_qelem ; p = p->q_forw) {
		if (p->in_drws) {
			d = p->in_drws->t_draw;
			if (d->window == window)
				return (p);
			else if (d->ex_window == window)
				return (p);
		}
	}
	return ((PropQ *)0);
}

proc_key (ev)
register XKeyEvent	*ev;
{
        KeySym		ks;
        XComposeStatus	xcs;
	register PropQ	*p;

        *keybuff = '\0';
        keylen = XLookupKanaString (ev, keybuff, BUFFLENGTH, &ks, &xcs);
        sjx_eval_key (ev, keybuff, keylen, &ks, &xcs);
        inchar = *keybuff;

        keysym = ks;
	input = INPUT_WINDOW;

	/*
	 * For input focus
	 * 1990.08.30	masaki
	 */
	if ((p = search_input_window (ev->window)) != 0)
		;
	else if ((p = search_client_window (ev->window)) != 0)
		;
#ifdef PROTO3
	else if ((p = XipWindow (ev->window)) != 0)
		;
#endif /* PROTO3 */
	else {
		return (-1);
	}

	inwindow = p->client_win;
	ChangeDS (p);
	return (0);
}

DrawInplace (window)
Window	window;
{
	PropQ	*p;

	if ((p = search_draw_window (window)) != 0)
		;
#ifdef PROTO3
	else if ((p = XipDrawWindow (window)) != 0)
		;
#endif /* PROTO3 */
	else
		return (-1);
	ChangeDS (p);
	PushTextDrawing ();
	config_off ();
	DrawScreen ();
	config_on ();
	PopDrawing ();
	return (0);
}

proc_destroy (ev)
XDestroyWindowEvent	*ev;
{
        register PropQ	*p;

	if ((p = search_client_window (ev->window)) != 0)
		;
	else if ((p = search_input_window (ev->window)) != 0)
		;
#ifdef PROTO3
	else if ((p = XipWindow (ev->window)) != 0) {
		 return XipProcDestroy (ev->window);
	}
#endif /* PROTO3 */
	else {
		return (-1);
	}
	/* remove deleted client from que */
	if (p->in_info) {
		free (p->in_info);
		p->in_info = 0;
	}
	if (p->in_drws) {
		FreePushDraws (p->in_drws);
		free (p->in_drws->t_draw);
		free (p->in_drws);
		p->in_drws = 0;
	}
	if (p->in_conv) {
		FreeConversion (p->in_conv);
		p->in_conv = 0;
	}
	p->client_win = 0;
	p->input_win = 0;
	p->in_unmap = 1;
	p->client_text = 0;
	p->client_prop = 0;
	if (real_prop_num > 0) {
		remque (p);
		free (p);
	}
	real_prop_num --;
	return (0);
}

ConversionClose (ev)
XClientMessageEvent	*ev;
{
        register PropQ	*p;
	Window		req_win;

	req_win = ev->data.l[1];
	if ((p = search_client_window (req_win)) == 0) {
		return (-1);
	}
	if (p->input_win) {
		XDestroyWindow (dpy, p->input_win);
		p->input_win = 0;
	}
	if (p->in_info) {
		free (p->in_info);
		p->in_info = 0;
	}
	if (p->in_drws) {
		if (p->in_drws->t_draw->window) {
			XDestroyWindow (dpy, p->in_drws->t_draw->window);
			p->in_drws->t_draw->window = 0;
		}
		if (p->in_drws->t_draw->ex_window) {
			XDestroyWindow (dpy, p->in_drws->t_draw->ex_window);
			p->in_drws->t_draw->ex_window = 0;
		}
		FreePushDraws (p->in_drws);
		free (p->in_drws->t_draw);
		free (p->in_drws);
		p->in_drws = 0;
	}
	if (p->in_conv) {
		FreeConversion (p->in_conv);
		p->in_conv = 0;
	}
	p->client_win = 0;
	p->input_win = 0;
	p->client_text = 0;
	p->client_prop = 0;
	if (real_prop_num > 0) {
		remque (p);
		free (p);
	}
	real_prop_num --;
	return (0);
}

proc_clientmsg (ev)
XClientMessageEvent *ev;
{
#ifdef PROTO3
	extern Atom	xip_protocol;
#endif /* PROTO3 */
	int	rval;

	if (ev->message_type == conv_end_request)
		ConversionRegist (ev, 1);
	else if (ev->message_type == conv_request)
		ConversionRegist (ev, 0);
	else if (ev->message_type == conv_close)
		rval = ConversionClose (ev);
#ifdef PROTO3
	else if (ev->message_type == xip_protocol)
		xim_receive (ev);
#endif /* PROTO3 */
}

ConversionRegist (ev, end)
XClientMessageEvent	*ev;
int			end;
{
	if (search_client_window (ev->data.l[1]) == 0)
		proc_clientmsg_newface (ev, end);
	else
		proc_clientmsg_oldface (ev, end);
}

SendConversionNotify (p)
register PropQ	*p;
{
	XClientMessageEvent	event_send;

	event_send.type = ClientMessage;
	event_send.window = p->client_win;
	event_send.send_event = True;
	event_send.message_type = conv_notify;
	event_send.format = 32;
	event_send.data.l[0] = selection;
	event_send.data.l[1] = p->client_text;
	event_send.data.l[2] = p->client_prop;
	event_send.data.l[3] = p->input_win;
	event_send.data.l[4] = p->client_inplace;
	XSendEvent (dpy, p->client_win, False, NULL, &event_send);
}

SendConversionEnd (p)
register PropQ	*p;
{
	XClientMessageEvent	event_send;

	event_send.type = ClientMessage;
	event_send.window = p->client_win;
	event_send.send_event = True;
	event_send.message_type = conv_end;
	event_send.format = 32;
	event_send.data.l[0] = selection;
	event_send.data.l[1] = subwin;
	XSendEvent (dpy, p->client_win, False, NULL, &event_send); 
}

SameRoot (dpy, win)
Display	*dpy;
Window	win;
{
	Window		c_root;
	unsigned int	c_w, c_h, c_bw, c_d;
	int		c_x, c_y;
	int		stat;

	stat = XGetGeometry (dpy, win, &c_root, &c_x, &c_y,
			&c_w, &c_h, &c_bw, &c_d);
	if (c_root == root)
		return (1);
	else
		return (0);
}

proc_clientmsg_newface (ev, end)
XClientMessageEvent	*ev;
int			end;
{
	register Window		req_win, in_only_w;
	Atom			req_prop;
	Atom			req_text;
	Atom			req_inplace;
	extern int		codevalue;
	register PropQ		*p;
	PropQ			*insert_prop_que ();

	codevalue = KEY_IGNORE;
	if (end) {
		if (henkan_flag != 0) {
			codevalue = KEY_CONV;
			XFlush (dpy);
		}
		return;
	}
	req_win = ev->data.l[1];
	if (req_win == NULL)
		return;
	req_text = ev->data.l[2];
	if ((req_prop = ev->data.l[3]) == None) {
		req_prop = conv_prop;
	}
	req_inplace = ev->data.l[4];

	in_only_w = XCreateWindow (
			dpy, req_win, 0, 0, 10000, 10000, 0,
			0, InputOnly, 0, 0, NULL
		);
	if (!SameRoot (dpy, req_win))
		req_inplace = 0;
	p = insert_prop_que (in_only_w, req_win, req_text, req_prop, req_inplace);

	XSelectInput (dpy, in_only_w, KeyPressMask);
	XSelectInput (dpy, req_win, StructureNotifyMask);

	if (req_inplace != conv_inplace) {
		if (end) {
			if (henkan_flag != 0)
				codevalue = KEY_CONV;
		}
		else {
			if (henkan_flag == 0) {
				codevalue = KEY_CONV;
				Xlc_MapWindow (p);
			}
			else {
				XMapWindow (dpy, p->input_win);
				p->in_unmap = 0;
				SendConversionNotify (p);
			}
		}
	}
	XFlush (dpy);
}

proc_clientmsg_oldface (ev, end)
XClientMessageEvent	*ev;
int			end;
{
	XClientMessageEvent	event_send;
	register PropQ		*p;

	if ((p = search_client_window (ev->data.l[1])) == 0)
		return (-1);
	if ((p->client_prop = ev->data.l[3]) == None)
		p->client_prop = conv_prop;
	p->client_text = compound_text;
	p->client_inplace = ev->data.l[4];

	if (!SameRoot (dpy, ev->data.l[1]))
		p->client_inplace = 0;
	codevalue = KEY_IGNORE;
	if (p->client_inplace != conv_inplace) {
		if (end) {
			if (henkan_flag != 0)
				codevalue = KEY_CONV;
		}
		else {
			if (henkan_flag == 0) {
				codevalue = KEY_CONV;
				Xlc_MapWindow (p);
			}
			else {
				XMapWindow (dpy, p->input_win);
				p->in_unmap = 0;
				SendConversionNotify (p);
			}
		}
	}
	if (p->client_inplace != conv_inplace) {
		if (p->in_info) {
			free (p->in_info);
			p->in_info = 0;
		}
		if (p->in_drws) {
			XDestroyWindow (dpy, p->in_drws->t_draw->window);
			p->in_drws->t_draw->window = 0;
			if (p->in_drws->t_draw->ex_window) {
				XDestroyWindow (
					dpy,
					p->in_drws->t_draw->ex_window
				);
				p->in_drws->t_draw->ex_window = 0;
			}
			FreePushDraws (p->in_drws);
			free (p->in_drws->t_draw);
			free (p->in_drws);
			p->in_drws = 0;
		}
		if (p->in_conv) {
			FreeConversion (p->in_conv);
			p->in_conv = 0;
		}
	}
	XFlush (dpy);
}

PropQ *
insert_prop_que (in_only_w, client_w, text, property, inplace)
Window	in_only_w, client_w;
Atom	text;
Atom	property;
Atom	inplace;
{
	register PropQ	*p;

	if (real_prop_num == 0) {
		p = &prop_qelem;
		p->q_forw = p->q_back = p;
	}
	else {
		p = (PropQ *) malloc (sizeof (PropQ));
		insque (p, prop_qelem);
	}
	real_prop_num++;
	p->input_win = in_only_w;
	p->client_win = client_w;
	p->client_text = text;
	p->client_prop = property;
	p->client_inplace = inplace;
	p->in_info = 0;
	p->in_drws = 0;
	p->in_conv = 0;
	return (p);
}

send_to_window (c_window, storebuff, storebytes)
Window	c_window;
char	*storebuff;
int	storebytes;
{
	register PropQ	*p;

	if (!storebytes)
		return (-1);
#ifdef PROTO3
	if (XipWindow (c_window)) {
		XipSend (c_window, storebuff, storebytes);
		return (0);
	}
	else
#endif /* PROTO3 */
	if ((p = search_client_window (c_window)) == 0)
		return (-1);
	storebytes = sjis2compound (storebuff, storebytes);
	XChangeProperty (
		dpy, p->client_win, p->client_prop, p->client_text, 8,
		PropModeAppend, storebuff, storebytes
	);
	return (0);
}

send_key_event (c_window, ev)
Window		c_window;
XKeyEvent	*ev;
{
	register PropQ	*p;

#ifdef PROTO3
	if (XipWindow (c_window)) {
		XipSendKeyEvent (c_window, ev);
		return (0);
	}
#endif /* PROTO3 */
	if ((p = search_client_window (c_window)) == 0)
		return (-1);
	if (ev) {
		ev->window = p->client_win;
		XSendEvent (dpy, p->client_win, False, NULL, ev);
	}
	return (0);
}

All_Unmap ()
{
	register PropQ		*p;
	register DrawingSet	*d;

#ifdef PROTO3
	XipAllUnmap ();
#endif /* PROTO3 */
	if (!prop_init_flag)
		return;
	p = &prop_qelem;
	{
		if (p->in_unmap == 0)
			XUnmapWindow (dpy, p->input_win);
		p->in_unmap = 1;
		if (p->in_drws) {
			d = p->in_drws->t_draw;
			if (d->unmap == 0)
				XUnmapWindow (dpy, d->window);
			d->unmap = 1;
			if (d->ex_window) {
				if (d->ex_unmap == 0)
					XUnmapWindow (dpy, d->ex_window);
				d->ex_unmap = 1;
			}
		}
		SendConversionEnd (p);
		/*
		 * I need client's StructureNotify because of destroy.
		 */
		XSelectInput (dpy, p->client_win, StructureNotifyMask);
		XFlush (dpy);
	}
	for (p = prop_qelem.q_forw ; p != &prop_qelem ; p = p->q_forw) {
		if (p->in_unmap == 0)
			XUnmapWindow (dpy, p->input_win);
		p->in_unmap = 1;
		if (p->in_drws) {
			d = p->in_drws->t_draw;
			if (d->unmap == 0)
				XUnmapWindow (dpy, d->window);
			d->unmap = 1;
			if (d->ex_window) {
				if (d->ex_unmap == 0)
					XUnmapWindow (dpy, d->ex_window);
				d->ex_unmap = 1;
			}
		}
		SendConversionEnd (p);
		/*
		 * I need client's StructureNotify because of destroy.
		 */
		XSelectInput (dpy, p->client_win, StructureNotifyMask);
		XFlush (dpy);
	}
}

All_Map ()
{
	register PropQ		*p;

	p = &prop_qelem;
	Xlc_MapWindow (p);
	for (p = prop_qelem.q_forw ; p != &prop_qelem ; p = p->q_forw)
		Xlc_MapWindow (p);
	XFlush (dpy);
}

Xlc_MapWindow (p)
register PropQ	*p;
{
	register DrawingSet	*d;

	XMapWindow (dpy, p->input_win);
	p->in_unmap = 0;
	if (p->in_drws) {
		d = p->in_drws->t_draw;
		if (d->width >= d->font_width) {
			XMapWindow (dpy, d->window);
			d->unmap = 0;
		}
		if (d->ex_window && (d->text_y - d->y) > d->line_height
			&& d->ex_w >= d->font_width) {
			XMapWindow (dpy, d->ex_window);
			d->ex_unmap = 0;
		}
		XRaiseWindow (dpy, p->input_win);
	}
	SendConversionNotify (p);
}

prop_init ()
{
	/* setup properties		*/
	conv_request = XInternAtom (dpy, CONVERSION_REQUEST, False);
	conv_notify = XInternAtom (dpy, CONVERSION_NOTIFY, False);
	conv_end = XInternAtom (dpy, CONVERSION_END, False);
	conv_end_request = XInternAtom (dpy, CONVERSION_END_REQUEST, False);
	conv_close = XInternAtom (dpy, CONVERSION_CLOSE, False);
	compound_text = XInternAtom (dpy, COMPOUND_TEXT, False);
	conv_inplace = XInternAtom (dpy, CONVERSION_INPLACE, False);
	conv_prop = XInternAtom (dpy, default_prop, False);

        /* setup properties' que	*/
        prop_que_init ();

	Xlc_SetKeycode ();
}

prop_que_init ()
{
	prop_qelem.q_forw = prop_qelem.q_back = &prop_qelem;
	prop_init_flag = 1;
}

proc_prop (ev)
XPropertyEvent	*ev;
{
	int		rval;
	Atom		actual_type;
	int		actual_format;
	unsigned long	nitems;
	unsigned long	bytes_after;
	char		*prop;
	register PropQ	*p;
	Window		req_win;
	InplaceInfo	*q;

	if (ev->state != PropertyNewValue)
		return (-1);
	if (ev->atom != conv_inplace) {
		return (-1);
	}

	rval = XGetWindowProperty (
		dpy, ev->window, conv_inplace, 0, 256, True,
		AnyPropertyType, &actual_type,
		&actual_format, &nitems,
		&bytes_after, &prop
	);
	if (rval != Success)
		return (-1);
	if ((q = (InplaceInfo *) malloc (sizeof(InplaceInfo))) == NULL) {
		XFree (prop);
		return (-1);
	}
	bcopy (prop, q, sizeof(InplaceInfo));
	XFree (prop);

	if ((p = search_client_window (q->in_win)) == 0) {
		free (q);
		return (-1);
	}
	if (p->client_inplace != conv_inplace)
		return (-1);
	if (p->in_info) {
		p->in_info->in_flag = q->in_flag;
		if (q->in_flag & AllInformation)
			bcopy (q, p->in_info, sizeof(InplaceInfo));
		else if (q->in_flag & FrameInformation)
			bcopy (q->in_frame,
			p->in_info->in_frame, sizeof(InplaceFrame));
		else if (q->in_flag & OffsetInformation) {
			p->in_info->in_frame.x_off
			= q->in_frame.x_off;
			p->in_info->in_frame.y_off
			= q->in_frame.y_off;
		}
		free (q);
	}
	else
		p->in_info = q;

	if (!p->in_drws) {
		if ((p->in_drws = (Draws *) malloc (sizeof(Draws))) == NULL) {
			return (-1);
		}
		if (MakeDraws (p->in_drws)) {
			free (p->in_drws);
			p->in_drws = 0;
			return (-1);
		}
	}

	if (!p->in_conv) {
		p->in_conv = AllocConversion ();
		SetConversion (p->in_conv);
		all_set_up ();
	}

	MakeDrawingSet (p);

	ChangeDS (p);

	if (p->in_info->in_flag & AllInformation) {
		XMapWindow (dpy, p->input_win);
		p->in_unmap = 0;

		SendConversionNotify (p);

		if (henkan_flag == 0)
			codevalue = KEY_CONV;
		else
			codevalue = KEY_IGNORE;
	}
	else
		codevalue = KEY_IGNORE;

	return (0);
}

MakeDraws (d)
register Draws	*d;
{
	extern Draws	*def_draws;

	if ((d->t_draw = (DrawingSet *) malloc (sizeof(DrawingSet))) == NULL)
		return (-1);

	d->t_draw->window = 0;
	d->t_draw->ex_window = 0;
	d->t_draw->ex_unmap = 1;
	d->t_draw->gcs = c_gcs;
	d->t_draw->mapsw = 1;
	d->t_draw->unmap = 1;
	d->t_draw->font_width = 0;
	d->t_draw->width = 0;
	d->t_draw->x1 = 0;
	d->t_draw->x2 = 0;

	d->g_draw = def_draws->g_draw;
	d->c_draw = d->t_draw;

	return (0);
}

MakeDrawingSet (p)
PropQ	*p;
{
	register InplaceInfo	*i;
	register DrawingSet	*d;
	register Conversion	*c;
	register XFontStruct	*fs;
	char			**names;

	i = p->in_info;
	d = p->in_drws->t_draw;
	c = p->in_conv;

	/* reset 4 GC					*/

	ResetGCs (p);

	/* calculate font width				*/

	if (!d->font_width) {
		extern Font	jfont;

		if (!i->in_draw.font16)
			i->in_draw.font16 = jfont;
		fs = XQueryFont (dpy, i->in_draw.font16);
		d->font_width = fs->max_bounds.width / 2;
		d->origin_x = 0;			/* dummy */
		d->origin_y = fs->ascent;
		d->font_ascent = fs->ascent;
		d->font_height = fs->ascent + fs->descent;
		/* names is a dummy for XFreeFontInfo	*/
		/* names will be free in XFreeFontInfo	*/
		names = (char **) malloc (sizeof(char*));
		names[0] = (char *) malloc (2);
		XFreeFontInfo (names, fs, 1);
		if (i->in_frame.line_height < d->font_height)
			d->line_height = d->font_height;
		else
			d->line_height = i->in_frame.line_height;

		if (i->in_flag & AutoReplace)
			d->auto_replace = 1;
		else
			d->auto_replace = 0;
	}

	ResetDrawingSet (p);

	/* create drawing window			*/
	if (d->window) {
		MoveWindow (d);
	}
	else {
		d->window = CreateTextWindow (
			i->in_win,
			i->in_attr.back,
			i->in_attr.border,
			i->in_attr.bwidth,
			d->ex_x,
			d->y,
			d->width,
			d->height
		);
		d->ex_window = CreateTextWindow (
			i->in_win,
			i->in_attr.back,
			i->in_attr.border,
			i->in_attr.bwidth,
			d->x,
			d->y + d->line_height,
			d->ex_w,
			d->height
		);
	}
}

ResetDrawingSet (p)
register PropQ	*p;
{
	register InplaceInfo	*i;
	register DrawingSet	*d;
	register Conversion	*c;

	i = p->in_info;
	d = p->in_drws->t_draw;
	c = p->in_conv;

	/* modification frame info if neccessary	*/
	if (!(i->in_flag & OffsetInformation) || d->x1 == d->x2) {
		if (i->in_frame.x < 0)
			i->in_frame.x = 0;
		else if (i->in_frame.x > i->in_frame.width)
			i->in_frame.x = i->in_frame.width;
		d->x1 = i->in_frame.x;
		if (i->in_frame.y < 0)
			i->in_frame.y = 0;
		else if (i->in_frame.y > i->in_frame.height)
			i->in_frame.y = i->in_frame.height;
		d->y1 = i->in_frame.y;

		if ((i->in_frame.width <= 0 || i->in_frame.height <= 0)) {
			XWindowAttributes	attr;

			XGetWindowAttributes (dpy, i->in_win, &attr);
			if (i->in_frame.width <= 0)
				d->x2 = attr.width;
			else
				d->x2 = d->x1 + i->in_frame.width;
			if (i->in_frame.height <= 0)
				d->y2 = attr.height;
			else
				d->y2 = d->y1 + i->in_frame.height;
		}
		else {
			d->x2 = d->x1 + i->in_frame.width;
			d->y2 = d->y1 + i->in_frame.height;
		}
	}

	if (i->in_frame.x_off < d->x1)
		i->in_frame.x_off = d->x1;
	if (i->in_frame.y_off < d->y1)
		i->in_frame.y_off = d->y1;

	d->x = i->in_frame.x_off;
	d->y = i->in_frame.y_off;
	/* This modification is for extra window	*/
	/* 1990.05.04	masaki				*/
	d->ex_x = d->x1;
	d->ex_y = d->y + d->line_height;

	d->text_x = i->in_frame.x_off;
	d->text_y = i->in_frame.y_off + d->origin_y;

	/* set other parameters				*/
	if (!(i->in_flag & OffsetInformation) || d->width < d->font_width) {
		d->width = 1;
		d->ex_w = 1;
		d->height = d->font_height;
		/*
		d->height = d->line_height;
		d->text_x = i->in_frame.x_off;
		d->text_y = i->in_frame.y_off + d->origin_y;
		*/
			/*
			 * ToDo:
			 * must be modify text_y
			 */
		d->reverse = 0;
		d->under = 0;
	}

	/* set line and column				*/
	c->line = d->line = (d->y2 - d->y1) / d->line_height;
	c->column = d->column = (d->x2 - d->x1) / (d->font_width);
}

MoveWindow (d)
register DrawingSet	*d;
{
	if (d->mapsw) {
		XMoveWindow (dpy, d->window, d->x, d->y);
		if (d->ex_window)
			XMoveWindow (dpy, d->ex_window, d->ex_x, d->ex_y);
	}
}

static int	config_sw = 1;

config_off ()
{
	config_sw = 0;
}

config_on ()
{
	config_sw = 1;
}

ConfigWindow (d)
register DrawingSet	*d;
{
	PropQ	*p;

    if (config_sw && d->mapsw && d->window) {
	if (d->ex_window) {
		register int	h;

		if (d->unmap && d->width >= d->font_width) {
			XMapWindow (dpy, d->window);
			d->unmap = 0;
			if ((p = search_drawing_set (d)) != 0)
				XRaiseWindow (dpy, p->input_win);
		}
		h = d->line_height;
		XMoveResizeWindow (
			dpy, d->window, d->x, d->y, d->width, h
		);
		if (d->text_y > (d->y - d->y1) + d->line_height
			&& d->ex_w >= d->font_width) {
			if (d->ex_unmap) {
				XMapWindow (dpy, d->ex_window);
				d->ex_unmap = 0;
				if ((p = search_drawing_set (d)) != 0)
					XRaiseWindow (dpy, p->input_win);
			}
			h = d->height - d->line_height;
			XMoveResizeWindow (
			    dpy, d->ex_window, d->ex_x, d->ex_y, d->ex_w, h
			);
		}
		else {
			if (d->ex_unmap == 0)
				XUnmapWindow (dpy, d->ex_window);
			d->ex_unmap = 1;
		}
	}
	else {
		if (d->unmap && d->width >= d->font_width) {
			XMapWindow (dpy, d->window);
			d->unmap = 0;
			if ((p = search_drawing_set (d)) != 0)
				XRaiseWindow (dpy, p->input_win);
		}
		XMoveResizeWindow (
			dpy, d->window, d->x, d->y, d->width, d->height
		);
	}
    }
}

MapInplaceWindow (d)
register DrawingSet	*d;
{
	PropQ	*p;

	if (d->mapsw && d->width >= d->font_width) {
		XMapWindow (dpy, d->window);
		d->unmap = 0;
		if (d->ex_window && (d->text_y - d->y) > d->line_height
			&& d->ex_w > d->font_width) {
			XMapWindow (dpy, d->ex_window);
			d->ex_unmap = 0;
		}
		if ((p = search_drawing_set (d)) == 0)
			return (-1);
		XRaiseWindow (dpy, p->input_win);
	}
	return (0);
}

UnmapInplaceWindow (d)
register DrawingSet	*d;
{
	if (config_sw && d->window && d->mapsw) {
		if (d->unmap == 0)
			XUnmapWindow (dpy, d->window);
		d->unmap = 1;
		if (d->ex_window) {
			if (d->ex_unmap == 0)
				XUnmapWindow (dpy, d->ex_window);
			d->ex_unmap = 1;
		}
	}
	return (0);
}

ChangeDS (p)
register PropQ	*p;
{
	if (p && p->in_drws) {
		SetConversion (p->in_conv);
		SetDraws (p->in_drws);
		ResetGCs (p);
	}
	else {
		ChangeDefaultDS ();
	}
}

ChangeDefaultDS ()
{
	DefaultConversion ();
	DefaultDrawingSet ();
}

ResetGCs (p)
register PropQ	*p;
{
	ChangeGCs (
		p->in_drws->t_draw->gcs,
		p->in_info->in_draw.fore,
		p->in_info->in_draw.back,
		p->in_info->in_draw.font8,
		p->in_info->in_draw.font16,
		p->in_info->in_draw.efont16
	);
}

proto2_end ()
{
	All_Unmap ();
}

Xlc_conversion (on)
int	on;
{
	Window	win;
	Atom	prop;
	int	status;

	win = subwin;
	prop = XInternAtom (dpy, CONVERSION_STATUS, False);
	status = on;

	XChangeProperty (
		dpy, win, prop, XA_INTEGER, 32,
		PropModeReplace, (char *)&status, 1
	);
}
#endif	/* PROTO2 */

sjis2compound (store, n)
u_char	*store;
int	n;
{
	u_char		buff[BUFFLENGTH * 4];
	register u_char	*s;
	register u_char	*t;
	register int	c;
	register int	stat;
	register int	len;
	register int	count;

#define ASCII_STAT	0
#define KANA_STAT	1
#define KANJI_STAT	2

	s = store;
	t = buff;
	stat = ASCII_STAT;
	count = 0;
	for (; n > 0 ; n--, count++) {
		c = *s++;
		if (issjis1 (c)) {
			if (stat != KANJI_STAT) {
				if (stat == KANA_STAT) {
					len = endkana (t);
					t += len;
					count += len;
				}
				len = gotokanji (t);
				t += len;
				count += len;
				stat = KANJI_STAT;
			}
			c <<= 8;
			c |= *s++;
			c = sjis2jis (c);
			*t++ = (c >> 8) & 0x7f;
			*t++ = c & 0x7f;
			count ++;
			n --;
		}
		else if (iskana (c)) {
			if (stat != KANA_STAT) {
				if (stat != ASCII_STAT) {
					len = gotoascii (t);
					t += len;
					count += len;
					stat = ASCII_STAT;
				}
				len = gotokana (t);
				t += len;
				count += len;
				stat = KANA_STAT;
			}
			*t++ = c;
		}
		else {
			if (stat == KANA_STAT) {
				len = endkana (t);
				t += len;
				count += len;
				stat = ASCII_STAT;
			} else if (stat == KANJI_STAT) {
				len = gotoascii (t);
				t += len;
				count += len;
				stat = ASCII_STAT;
			}
			*t++ = c & 0x7f;
		}
	}
	if (stat == KANA_STAT) {
		len = endkana (t);
		t += len;
		count += len;
		stat = ASCII_STAT;
	} else if (stat == KANJI_STAT) {
		len = gotoascii (t);
		t += len;
		count += len;
		stat = ASCII_STAT;
	}
	strncpy (store, buff, count);
	return (count);
}

#define SEQUENCE_KANJI	"\033$(B"
#define SEQUENCE_KANA	"\033)I"
#define SEQUENCE_KEND	"\033-A"
#define SEQUENCE_ASCII	"\033(B"

gotokanji (s)
u_char	*s;
{
	strcpy (s, SEQUENCE_KANJI);
	return (strlen (SEQUENCE_KANJI));
}

gotokana (s)
u_char	*s;
{
	strcpy (s, SEQUENCE_KANA);
	return (strlen (SEQUENCE_KANA));
}

endkana (s)
u_char	*s;
{
	strcpy (s, SEQUENCE_KEND);
	return (strlen (SEQUENCE_KEND));
}

gotoascii (s)
u_char	*s;
{
	strcpy (s, SEQUENCE_ASCII);
	return (strlen (SEQUENCE_ASCII));
}
