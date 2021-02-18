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
 *	xip.c
 *	X Input Method Protocol Routines
 *	Copyright (c) 1991 Sony Corporation.
 *	Written by Masaki Takeuchi
 *	Wed Apr 10 02:06:07 JST 1991
 *
 *	This routines support XIMProtocol
 */

#include "sjx.h"

#ifdef PROTO3
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "XIMProto.h"
#include "common.h"
#include "key.h"
#include "xcommon.h"
#include "Xlc.h"
#include "Xlcdef.h"

extern Display	*dpy;

struct xip_ic {
	struct xip_ic	*q_forw;
	struct xip_ic	*q_back;
	Window		window;		/* client_win		in XLC*/
	Window		focus;		/* input_win		in XLC*/
	ICID		icid;		/* in_unmap		in XLC*/
	int		style;		/* client_text		in XLC*/
	int		focused;	/* client_prop		in XLC*/
	int		font_loaded;	/* client_inplace	in XLC*/
	InplaceInfo	*in_info;
	Draws		*in_drws;
	Conversion	*in_conv;
};

typedef struct xip_ic	XipIC;

static XipIC	*xip_ic_q;
static int	xip_ic_num = 0;

void	_xip_send32 ();

/**********************************
 * Private Functions
 **********************************/

static XipIC *
_xip_insert_queue (window, style)
Window	window;
int	style;
{
	register XipIC	*ic;

	ic = (XipIC *) malloc (sizeof (XipIC));
	if (xip_ic_num == 0) {
		xip_ic_q = ic;
		ic->q_forw = ic->q_back = ic;
	}
	else {
		insque (ic, xip_ic_q);
	}
	xip_ic_num ++;
	ic->window = window;
	ic->focus = window;
	ic->icid = xip_ic_num;
	ic->style = style;
	ic->focused = 0;
	ic->font_loaded = 0;
	ic->in_info = 0;
	ic->in_drws = 0;
	ic->in_conv = 0;
	return (ic);
}

static void
_xip_delete_queue (ic)
register XipIC	*ic;
{
	if (xip_ic_num == 0)
		return;
	xip_ic_num --;
	if (xip_ic_num > 0)
		remque (ic);
	free (ic);
}

static Window
xip_icid_window (icid)
ICID	icid;
{
	register XipIC	*ic;

	if (xip_ic_num == 0)
		return (0);
	else if (xip_ic_q->icid == icid)
		return (xip_ic_q->window);
	else
		for (ic = xip_ic_q->q_forw ; ic != xip_ic_q ; ic = ic->q_forw)
			if (ic->icid == icid)
				return (ic->window);
	return (0);
}

static XipIC *
xip_icid_ic (icid)
ICID	icid;
{
	register XipIC	*ic;

	if (xip_ic_num == 0)
		return (0);
	else if (xip_ic_q->icid == icid)
		return (xip_ic_q);
	else
		for (ic = xip_ic_q->q_forw ; ic != xip_ic_q ; ic = ic->q_forw)
			if (ic->icid == icid)
				return (ic);
	return (0);
}

static ICID
xip_window_icid (window)
Window	window;
{
	register XipIC	*ic;

	if (xip_ic_num == 0)
		return (0);
	else if (xip_ic_q->window == window)
		return (xip_ic_q->icid);
	else
		for (ic = xip_ic_q->q_forw ; ic != xip_ic_q ; ic = ic->q_forw)
			if (ic->window == window)
				return (ic->icid);
	return (0);
}

static XipIC *
_xip_window_ic (window)
Window	window;
{
	register XipIC	*ic;

	if (xip_ic_num == 0)
		return (0);
	else if (xip_ic_q->window == window || xip_ic_q->focus == window)
		return (xip_ic_q);
	else
		for (ic = xip_ic_q->q_forw ; ic != xip_ic_q ; ic = ic->q_forw)
			if (ic->window == window || ic->focus == window)
				return (ic);
	return (0);
}

static XipIC *
xip_window_focus_ic (window)					/* Focus */
Window	window;
{
	register XipIC	*ic;

	if (xip_ic_num == 0)
		return (0);
	else if ((xip_ic_q->window == window ||
		xip_ic_q->focus == window) && xip_ic_q->focused)
		return (xip_ic_q);
	else
		for (ic = xip_ic_q->q_forw ; ic != xip_ic_q ; ic = ic->q_forw)
			if ((ic->window == window ||
				ic->focus == window) && ic->focused)
				return (ic);
	return (_xip_window_ic (window));
}

static XipIC *
xip_window_draw_ic (window)
Window	window;
{
	register XipIC		*ic;
	register DrawingSet	*d;

	if (xip_ic_num == 0)
		return (0);
	ic = xip_ic_q;
	if (ic->in_drws) {
		d = ic->in_drws->t_draw;
		if (d->window == window || d->ex_window == window)
			return (ic);
	}
	for (ic = xip_ic_q->q_forw ; ic != xip_ic_q ; ic = ic->q_forw) {
		if (ic->in_drws) {
			d = ic->in_drws->t_draw;
			if (d->window == window || d->ex_window == window)
				return (ic);
		}
	}
	return (0);
}

static
AllocPreedit (ic)
XipIC		*ic;
{
	ic->in_info = (InplaceInfo *) malloc (sizeof(InplaceInfo));
	if (ic->in_info)
		bzero (ic->in_info, sizeof(InplaceInfo));
	else
		return (0);
	ic->in_drws = (Draws *) malloc (sizeof(Draws));
	if (ic->in_drws)
		MakeDraws (ic->in_drws);
	else {
		free (ic->in_info);
		ic->in_info = 0;
		return (0);
	}
	ic->in_conv = AllocConversion ();
	if (ic->in_conv) {
		SetConversion (ic->in_conv);
		all_set_up ();
	}
	else {
		free (ic->in_info);
		free (ic->in_drws);
		ic->in_info = 0;
		ic->in_drws = 0;
		return (0);
	}
	return (1);
}

static
SetPreedit (ic, mask, pe)
XipIC			*ic;
unsigned long		mask;
Ximp_PreeditPropRec	*pe;
{
	ic->in_info->in_attr.border = fore;
	ic->in_info->in_attr.bwidth = 0;
	ic->in_info->in_win = ic->focus;
	ic->in_info->in_flag = FrameInformation | AutoReplace;

	if (mask & XIMP_PRE_AREA_MASK) {
		ic->in_info->in_frame.x = pe->Area.x;
		ic->in_info->in_frame.y = pe->Area.y;
		ic->in_info->in_frame.width = pe->Area.width;
		ic->in_info->in_frame.height = pe->Area.height;
	}
	if (mask & XIMP_PRE_FG_MASK) {
		ic->in_info->in_attr.border = 
		ic->in_info->in_draw.fore = pe->Foreground;
	}
	if (mask & XIMP_PRE_BG_MASK) {
		ic->in_info->in_attr.back =
		ic->in_info->in_draw.back = pe->Background;
	}
	if (mask & XIMP_PRE_COLORMAP_MASK) {
	}
	if (mask & XIMP_PRE_BGPIXMAP_MASK) {
	}
	if (mask & XIMP_PRE_CURSOR_MASK) {
	}
	if (mask & XIMP_PRE_AREANEED_MASK) {
	}
	if (mask & XIMP_PRE_SPOTL_MASK) {
		ic->in_info->in_frame.x_off = pe->SpotLocation.x;
		ic->in_info->in_frame.y_off = pe->SpotLocation.y;
	}

	MakeDrawingSet (ic);

	if (mask & XIMP_PRE_SPOTL_MASK) {
		ic->in_info->in_frame.y_off -=
			ic->in_drws->t_draw->origin_y;
	}
	if (mask & XIMP_PRE_LINESP_MASK) {
		ic->in_info->in_frame.line_height =
			ic->in_drws->t_draw->font_height + pe->LineSpacing;
		ic->in_drws->t_draw->line_height =
			ic->in_info->in_frame.line_height;
	}

	if (mask & (XIMP_PRE_AREA_MASK | XIMP_PRE_SPOTL_MASK
	    | XIMP_PRE_LINESP_MASK)) {
		ResetDrawingSet (ic);
		MoveWindow (ic->in_drws->t_draw);
	}
}

static
get_xlfd (f, save)
char	*f;
char	**save;
{
	int	count;

	*save++ = f;
	count = 1;
	while (*f != 0 && count <= 3) {
		if (*f == ',') {
			*f++ = 0;
			count++;
			*save++ = f;
		}
		else
			f++;
	}
	return (count);
}

static void
set_font_info (ic, xlfd)
XipIC	*ic;
char	**xlfd;
{
	Font	fn;

	if (ic->font_loaded) {
		XUnloadFont (dpy, ic->in_info->in_draw.font8);
		XUnloadFont (dpy, ic->in_info->in_draw.font16);
	}
	fn = XLoadFont (dpy, xlfd[1]);
	ic->in_info->in_draw.font8 = fn;
	fn = XLoadFont (dpy, xlfd[2]);
	ic->in_info->in_draw.font16 = fn;
	ic->font_loaded = 1;
}

static void
default_font_info (ic)
XipIC	*ic;
{
	extern Font	font, jfont;

	if (ic->font_loaded)
		return;
	ic->in_info->in_draw.font8 = font;
	ic->in_info->in_draw.font16 = jfont;
	ic->font_loaded = 0;
}

static
SetPreeditFont (ic, mask, pf)
XipIC			*ic;
unsigned long		mask;
char			*pf;
{
	char	*xlfd[3];
	int	count;

	count = get_xlfd (pf, xlfd);
	if (count == 3)
		set_font_info (ic, xlfd);
	else
		default_font_info (ic);
	ic->in_drws->t_draw->font_width = 0;
}

#define FLAG_FOCUS	XIMP_FOCUS_WIN_MASK
#define FLAG_PREEDIT	(XIMP_PRE_AREA_MASK | \
			XIMP_PRE_FG_MASK | \
			XIMP_PRE_BG_MASK | \
			XIMP_PRE_COLORMAP_MASK | \
			XIMP_PRE_BGPIXMAP_MASK | \
			XIMP_PRE_LINESP_MASK | \
			XIMP_PRE_CURSOR_MASK | \
			XIMP_PRE_AREANEED_MASK | \
			XIMP_PRE_SPOTL_MASK)
#define FLAG_STATUS	(XIMP_STS_AREA_MASK | \
			XIMP_STS_FG_MASK | \
			XIMP_STS_BG_MASK | \
			XIMP_STS_COLORMAP_MASK | \
			XIMP_STS_BGPIXMAP_MASK | \
			XIMP_STS_LINESP_MASK | \
			XIMP_STS_CURSOR_MASK | \
			XIMP_STS_AREANEED_MASK | \
			XIMP_STS_WINDOW_MASK)
#define FLAG_PREFONT	XIMP_PRE_FONT_MASK
#define FLAG_STSFONT	XIMP_STS_FONT_MASK

extern Atom		xip_focus;
extern Atom		xip_preedit;
extern Atom		xip_status;
extern Atom		xip_preeditfont;
extern Atom		xip_statusfont;

static void
_xip_get_icinfo (ic, mask)
XipIC		*ic;
unsigned long	mask;
{
	Window			window;
	int			rval;
	Atom			actual_type;
	int			actual_format;
	unsigned long		nitems;
	unsigned long		bytes_after;
	Window			*data_window;
	Ximp_PreeditPropRec	*data_preedit;
	Ximp_StatusPropRec	*data_status;
	char			*data_preeditfont;
	char			*data_statusfont;

	window = ic->window;
	if (mask & FLAG_FOCUS) {
		rval = XGetWindowProperty (
			dpy, window, xip_focus, 0L, 1000000L, True,
			XA_WINDOW, &actual_type,
			&actual_format, &nitems,
			&bytes_after, &data_window
		);
		if (rval != Success || nitems == 0)
			ic->focus = window;
		else
			ic->focus = *data_window;
		XFree (data_window);
	}
	if (mask & FLAG_PREFONT) {
		rval = XGetWindowProperty (
			dpy, window, xip_preeditfont, 0, 256, True,
			XA_STRING, &actual_type,
			&actual_format, &nitems,
			&bytes_after, &data_preeditfont
		);
		if (SameRoot (dpy, ic->window)) {
		    if (!ic->in_info) {
			if (AllocPreedit (ic))
			    SetPreeditFont (ic, mask, data_preeditfont);
		    }
		    else
			SetPreeditFont (ic, mask, data_preeditfont);
		}
		XFree (data_preeditfont);
	}
	if (mask & FLAG_STSFONT) {
		rval = XGetWindowProperty (
			dpy, window, xip_statusfont, 0, 256, True,
			XA_STRING, &actual_type,
			&actual_format, &nitems,
			&bytes_after, &data_statusfont
		);
		XFree (data_statusfont);
	}
	if (mask & FLAG_PREEDIT) {
		rval = XGetWindowProperty (
			dpy, window, xip_preedit, 0, 256, True,
			xip_preedit, &actual_type,
			&actual_format, &nitems,
			&bytes_after, &data_preedit
		);
		if (SameRoot (dpy, ic->window)) {
		    if (!ic->in_info && SameRoot (dpy, ic->window)) {
			if (AllocPreedit (ic))
			    SetPreedit (ic, mask, data_preedit);
		    }
		    else
			SetPreedit (ic, mask, data_preedit);
		}
		XFree (data_preedit);
	}
	if (mask & FLAG_STATUS) {
		rval = XGetWindowProperty (
			dpy, window, xip_status, 0, 256, True,
			xip_status, &actual_type,
			&actual_format, &nitems,
			&bytes_after, &data_status
		);
		XFree (data_status);
	}
}

static void
_xip_set_icinfo (ic, mask)
XipIC		*ic;
unsigned long	mask;
{
	Window				window;
	int				rval;
	Atom				actual_type;
	int				actual_format;
	unsigned long			nitems;
	unsigned long			bytes_after;
	static Window			data_window;
	static Ximp_PreeditPropRec	data_preedit;
	static Ximp_StatusPropRec	data_status;
	static char			data_preeditfont[8];
	static char			data_statusfont[8];
	Window				focus;

	window = ic->window;
	if (mask & FLAG_FOCUS) {
		focus = ic->focus;
		XChangeProperty (
			dpy, window, xip_focus, XA_WINDOW, 32,
			PropModeAppend, &focus, 1
		);
	}
	if (mask & FLAG_PREEDIT) {
		XChangeProperty (
			dpy, window, xip_preedit, xip_preedit, 32,
			PropModeAppend, &data_preedit, sizeof(data_preedit)/4
		);
	}
	if (mask & FLAG_STATUS) {
		XChangeProperty (
			dpy, window, xip_status, xip_status, 32,
			PropModeAppend, &data_status, sizeof(data_status)/4
		);
	}
	if (mask & FLAG_PREFONT) {
		XChangeProperty (
			dpy, window, xip_preeditfont, xip_preeditfont, 32,
			PropModeAppend, data_preeditfont,
			strlen(data_preeditfont)
		);
	}
	if (mask & FLAG_STSFONT) {
		XChangeProperty (
			dpy, window, xip_statusfont, xip_statusfont, 32,
			PropModeAppend, data_statusfont,
			strlen(data_statusfont)
		);
	}
}

static void
xip_frame_offset (ic, x, y)
register XipIC	*ic;
int		x;
int		y;
{
	if (ic->in_info) {
		ic->in_info->in_flag = OffsetInformation;
		ic->in_info->in_frame.x_off = x;
		ic->in_info->in_frame.y_off =
			y - ic->in_drws->t_draw->origin_y;
		ResetDrawingSet (ic);
		MoveWindow (ic->in_drws->t_draw);
	}
}

static void
xip_create (window, mask, style)
register Window		window;
unsigned long		mask;
int			style;
{
	register XipIC	*ic;

	if (!xip_window_icid (window))
		XSelectInput (dpy, window, StructureNotifyMask);
	ic = _xip_insert_queue (window, style);
	_xip_get_icinfo (ic, mask);
	_xip_send32 (window, (long)XIMP_CREATE_RETURN,
		(long)ic->icid, 0L, 0L, 0L);
}

static void
xip_destroy (ic)
register XipIC	*ic;
{
	Window	window;

	window = ic->window;
					/* Need Free Inplace Data */
	_xip_delete_queue (ic);
	if (!xip_window_icid (window))
		XSelectInput (dpy, window, NoEventMask);
}

/********************************
 * Global Functions
 ********************************/

XipIC *
XipWindow (window)
Window	window;
{
	return (xip_window_focus_ic(window));
}

XipIC *
XipDrawWindow (window)
Window	window;
{
	return (xip_window_draw_ic(window));
}

void
XipKeypress (ev)
XClientMessageEvent	*ev;
{
	ICID			icid;
	unsigned int		keycode;
	unsigned int		state;
	static XKeyEvent	ke;

	icid = ev->data.l[1];
	keycode = ev->data.l[2];
	state = ev->data.l[3];
	ke.type = KeyPress;
	ke.keycode = keycode;
	ke.state = state;
	ke.window = xip_icid_window (icid);
	proc_key (&ke);
}

XipProcDestroy (window)
Window	window;
{
	register XipIC	*ic;

	if (!xip_window_icid (window))
		return (-1);
	while (ic = _xip_window_ic (window))
		_xip_delete_queue (ic);
	return (0);
}

void
XipCreate (ev)
XClientMessageEvent	*ev;
{
	Window		window;
	unsigned long	mask;
	int		style;
	ICID		icid;
	extern int	codevalue;
	extern int	henkan_flag;

	window = ev->data.l[1];
	style = ev->data.l[2];
	mask = ev->data.l[3];
	xip_create (window, mask, style);
}

void
XipDestroy (ev)
XClientMessageEvent	*ev;
{
	XipIC		*ic;
	ICID		icid;

	icid = ev->data.l[1];
	if (ic = xip_icid_ic (icid))
		xip_destroy (ic);
}

void
XipBegin (ev)
XClientMessageEvent	*ev;
{
	XipIC		*ic;
	ICID		icid;

	icid = ev->data.l[1];
	ic = xip_icid_ic (icid);
	XSelectInput (dpy, ic->window, KeyPressMask|StructureNotifyMask);
	XSelectInput (dpy, ic->focus, KeyPressMask);
	if (!henkan_flag)
		codevalue = KEY_CONV;
}

void
XipEnd (ev)
XClientMessageEvent	*ev;
{
	XipIC		*ic;
	ICID		icid;

	icid = ev->data.l[1];
	ic = xip_icid_ic (icid);
	XSelectInput (dpy, ic->window, StructureNotifyMask);
	XSelectInput (dpy, ic->focus, NoEventMask);
}

void
XipSetValue (ev)
XClientMessageEvent	*ev;
{
	XipIC			*ic;
	ICID			icid;
	unsigned long		mask;
	Window			window;

	icid = ev->data.l[1];
	mask = ev->data.l[2];
	ic = xip_icid_ic (icid);
	window = xip_icid_window (icid);

	_xip_get_icinfo (ic, mask);
}

void
XipGetValue (ev)
XClientMessageEvent	*ev;
{
	XipIC		*ic;
	ICID		icid;
	unsigned long	mask;
	Window		window;

	icid = ev->data.l[1];
	mask = ev->data.l[2];
	ic = xip_icid_ic (icid);
	window = xip_icid_window (icid);

	_xip_set_icinfo (ic, mask);
	_xip_send32 (window, (long)XIMP_GETVALUE_RETURN,
		(long)icid, 0L, 0L, 0L);
}

void
XipMove (ev)
XClientMessageEvent	*ev;
{
	XipIC		*ic;
	ICID		icid;
	int		x;
	int		y;

	icid = ev->data.l[1];
	x = ev->data.l[2];
	y = ev->data.l[3];
	ic = xip_icid_ic (icid);

	xip_frame_offset (ic, x, y);
}

/************************************
 * NOT_SUPPORT_YET the following
 ************************************/

void
XipSetfocus (ev)
XClientMessageEvent	*ev;
{
	ICID		icid;
	register XipIC	*ic;
	Window		window;

	icid = ev->data.l[1];
	ic = xip_icid_ic (icid);
	window = xip_icid_window (icid);
	ic->focused = 1;
							/* Focus */
	if (xip_ic_num == 0)
		return;
	if (xip_ic_q->window == window && xip_ic_q->icid != icid)
		xip_ic_q->focused = 0;
	for (ic = xip_ic_q->q_forw ; ic != xip_ic_q ; ic = ic->q_forw)
		if (ic->window == window && ic->icid != icid)
			ic->focused = 0;
}

void
XipUnsetfocus (ev)
XClientMessageEvent	*ev;
{
	ICID		icid;
	register XipIC	*ic;
	Window		window;

	icid = ev->data.l[1];
	ic = xip_icid_ic (icid);
							/* Focus */
	ic->focused = 0;
}

void
XipChange (ev)
XClientMessageEvent	*ev;
{

}

void
XipReset (ev)
XClientMessageEvent	*ev;
{

}

void
XipGeometry (ev)
XClientMessageEvent	*ev;
{

}

void
XipPreeditstart (ev)
XClientMessageEvent	*ev;
{

}

void
XipPreeditmaxsize (ev)
XClientMessageEvent	*ev;
{

}

void
XipPreeditdone (ev)
XClientMessageEvent	*ev;
{

}

void
XipPreeditdraw (ev)
XClientMessageEvent	*ev;
{

}

void
XipPreeditcaret (ev)
XClientMessageEvent	*ev;
{

}

void
XipCaretPos (ev)
XClientMessageEvent	*ev;
{

}

void
XipStatusstart (ev)
XClientMessageEvent	*ev;
{

}

void
XipStatusdone (ev)
XClientMessageEvent	*ev;
{

}

void
XipStatusdraw (ev)
XClientMessageEvent	*ev;
{

}

void
XipExtension (ev)
XClientMessageEvent	*ev;
{

}

/************************************
 * IM Server -> Client Protocol
 ************************************/

void
_xip_send32 (window, d0, d1, d2, d3, d4)
Window	window;
long	d0, d1, d2, d3, d4;
{
	XClientMessageEvent	cm;
	extern Atom		xip_protocol;

	cm.type = ClientMessage;
	cm.window = window;
	cm.send_event = True;
	cm.message_type = xip_protocol;
	cm.format = 32;
	cm.data.l[0] = d0;
	cm.data.l[1] = d1;
	cm.data.l[2] = d2;
	cm.data.l[3] = d3;
	cm.data.l[4] = d4;
	XSendEvent (dpy, window, False, 0, &cm);
}

void
_xip_begin_report (window, icid)
Window	window;
ICID	icid;
{
	_xip_send32 (window, XIMP_PROCESS_BEGIN, icid, 0, 0, 0);
}

void
_xip_end_report (window, icid)
Window	window;
ICID	icid;
{
	_xip_send32 (window, XIMP_PROCESS_END, icid, 0, 0, 0);
}

XipSend (window, buff, len)
Window	window;
char	*buff;
int	len;
{
	XipIC		*ic;
	extern Atom	conv_prop;
	extern Atom	compound_text;
	extern Window	subwin;

	if ((ic = xip_window_focus_ic (window)) == 0)
		return (-1);
	if (len) {
		len = sjis2compound (buff, len);
		XChangeProperty (
			dpy, subwin, conv_prop, compound_text, 8,
			PropModeAppend, buff, len
		);
	}
	_xip_send32 (ic->focus, (long)XIMP_READPROP,
		(long)ic->icid, (long)conv_prop, 0L, 0L);
	return (0);
}

XipSendKeyEvent (window, ev)
Window		window;
XKeyEvent	*ev;
{
	XipIC		*ic;

	if ((ic = xip_window_focus_ic (window)) == 0)
		return (-1);
	_xip_send32 (ic->focus, (long)XIMP_KEYPRESS,
		(long)ic->icid, (long)ev->keycode, (long)ev->state, 0L);
	return (0);
}

void
XipAllMap ()
{
	register XipIC	*ic;

	if (xip_ic_num == 0)
		return;
	_xip_begin_report (xip_ic_q->window, xip_ic_q->icid);
	XSelectInput (dpy, xip_ic_q->window,
			KeyPressMask|StructureNotifyMask);
	XSelectInput (dpy, xip_ic_q->focus, KeyPressMask);
	for (ic = xip_ic_q->q_forw ; ic != xip_ic_q ; ic = ic->q_forw) {
		_xip_begin_report (ic->window, ic->icid);
		XSelectInput (dpy, ic->window,
				KeyPressMask|StructureNotifyMask);
		XSelectInput (dpy, ic->focus, KeyPressMask);
	}
}

void
XipAllUnmap ()
{
	register XipIC	*ic;

	if (xip_ic_num == 0)
		return;
	_xip_end_report (xip_ic_q->window, xip_ic_q->icid);
	XSelectInput (dpy, xip_ic_q->window, StructureNotifyMask);
	XSelectInput (dpy, xip_ic_q->focus, NoEventMask);
	for (ic = xip_ic_q->q_forw ; ic != xip_ic_q ; ic = ic->q_forw) {
		_xip_end_report (ic->window, ic->icid);
		XSelectInput (dpy, ic->window, StructureNotifyMask);
		XSelectInput (dpy, ic->focus, NoEventMask);
	}
}
#endif /* PROTO3 */
