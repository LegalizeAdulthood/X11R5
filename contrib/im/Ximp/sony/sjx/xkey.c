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
 *	xkey.c
 *	Copyright (c) 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Fri Aug  5 16:41:04 JST 1988
 */
/*
 * $Header: xkey.c,v 1.3 91/10/02 09:14:30 root Locked $ SONY;
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>

#include "key.h"
#include "sj2.h"

extern int	codevalue;
char		pars_seq[2];
int		pars_n = 0;
XKeyEvent	*pars_ev = 0;

typedef struct _keyassign {
	KeySym	ksym;
	int	kval;
	char	*kcha;
	int	uval;
} KeyAssign;

KeyAssign	kassign[] = {
	{XK_Kanji,	KEY_CONV,	"xk_kanji",	KEY_IGNORE},
	{XK_Cancel,	KEY_MUHENKAN,	"xk_cancel",	KEY_IGNORE},
	{XK_Select,	KEY_HENKAN,	"xk_select",	KEY_IGNORE},
	{XK_Execute,	KEY_KETTEI,	"xk_execute",	KEY_IGNORE},
	{XK_Left,	KEY_LEFT,	"xk_left",	KEY_IGNORE},
	{XK_Up,		KEY_UP,		"xk_up",	KEY_IGNORE},
	{XK_Right,	KEY_RIGHT,	"xk_right",	KEY_IGNORE},
	{XK_Down,	KEY_DOWN,	"xk_down",	KEY_IGNORE},
	{XK_F10,	KEY_ZHIRA,	"xk_f10",	KEY_IGNORE},
	{XK_F11,	KEY_OTHER,	"xk_f11",	KEY_IGNORE},
	{XK_F12,	KEY_OTHER,	"xk_f12",	KEY_IGNORE},
	{XK_F13,	KEY_OTHER,	"xk_f13",	KEY_IGNORE},
	{XK_F14,	KEY_OTHER,	"xk_f14",	KEY_IGNORE},
	{XK_F15,	KEY_OTHER,	"xk_f15",	KEY_IGNORE},
	{XK_F16,	KEY_OTHER,	"xk_f16",	KEY_IGNORE},
	{XK_F17,	KEY_OTHER,	"xk_f17",	KEY_IGNORE},
	{XK_F18,	KEY_OTHER,	"xk_f18",	KEY_IGNORE},
	{XK_F19,	KEY_OTHER,	"xk_f19",	KEY_IGNORE},
	{XK_F20,	KEY_OTHER,	"xk_f20",	KEY_IGNORE},
	{XK_F1,		KEY_TOROKU,	"xk_f1",	KEY_IGNORE},
	{XK_F2,		KEY_SYOUKYO,	"xk_f2",	KEY_IGNORE},
	{XK_F3,		KEY_EDIT,	"xk_f3",	KEY_IGNORE},
	{XK_F4,		KEY_OTHER,	"xk_f4",	KEY_IGNORE},
	{XK_F5,		KEY_CODE,	"xk_f5",	KEY_IGNORE},
	{XK_F6,		KEY_HALPHA,	"xk_f6",	KEY_IGNORE},
	{XK_F7,		KEY_ZALPHA,	"xk_f7",	KEY_IGNORE},
	{XK_F8,		KEY_HKATA,	"xk_f8",	KEY_IGNORE},
	{XK_F9,		KEY_ZKATA,	"xk_f9",	KEY_IGNORE},
	{0,		0,		"",		KEY_IGNORE}
};

sjx_eval_key (ev, buff, len, ks, xcs)
XKeyEvent	*ev;
char		*buff;
int		len;
KeySym		*ks;
XComposeStatus	*xcs;
{
	extern char	pars_seq[];
	extern int	pars_n;

	pars_ev = ev;
	codevalue = eval_ks (ks);
	if (codevalue == KEY_NORMAL)
		codevalue = eval_code (ev, buff, len, xcs);
	else {
		*pars_seq = 0;
		pars_n = 0;
	}
}

eval_code (ev, s, len, xcs)
XKeyEvent	*ev;
char		*s;
int		len;
XComposeStatus	*xcs;
{
	int	val, slen;

	slen = len;
	val = parse_command (s, &slen);
	return (val);
}

eval_ks (ks)
KeySym	*ks;
{
	extern int	vflag;
	register int	i;

	for (i = 0 ; kassign[i].ksym != 0 ; i ++) {
		if (kassign[i].ksym == *ks) {
			if (kassign[i].uval != KEY_IGNORE)
				return (kassign[i].uval);
			else
				return (kassign[i].kval);
		}
	}

	if (IsModifierKey (*ks)) {
		return (KEY_IGNORE);
	}

	return (KEY_NORMAL);
}

clear_ukeys ()
{
	register int	i;

	for (i = 0 ; kassign[i].ksym != 0 ; i ++) {
		kassign[i].uval = KEY_IGNORE;
	}
}

set_func (word)
struct wordent	word[];
{
	register int	i, c;

	if (!mutch (word[0].word_str, "sjxkey"))
		return;
	for (i = 0 ; i < KEY_OTHER && *comstr[i] != '\0' ; i ++) {
		if (mutch (word[1].word_str, comstr[i])) {
			set_Ukey (word[2].word_str, i);
			return;
		}
	}
}

set_Ukey (s, comnum)
register char	*s;
int		comnum;
{
	register int	i;

	for (i = 0 ; kassign[i].ksym != 0 ; i ++) {
		if (mutch (s, kassign[i].kcha)) {
			kassign[i].uval = comstrvalue[comnum];
			return;
		}
	}
}

write_pseq (mod)
int	mod;
{
	register int	c;
	/*
	 * mod
	 *	0:write
	 *	1:write if pars_n != 1, else return char
	 *	2:return char. if pars_n != 1, return null.
	 *	3:return char. if pars_n > 0.
	 */
	if (mod == 3) {
		if (pars_n > 0) {
			c = *pars_seq & 0xff;
			*pars_seq = 0;
			pars_n = 0;
			pars_ev = 0;
			return (c);
		}
	}
	else if (mod == 2) {
		if (pars_n == 1) {
			c = *pars_seq & 0xff;
/*
			*pars_seq = 0;
			pars_n = 0;
			pars_ev = 0;
*/
			return (c);
		}
/*
		*pars_seq = 0;
		pars_n = 0;
		pars_ev = 0;
*/
		return (0);
	}
	else if (mod == 1) {
		if (pars_n == 1 && *pars_seq != 0) {
			c = *pars_seq & 0xff;
			*pars_seq = 0;
			pars_n = 0;
/*
			pars_ev = 0;
*/
			return (c);
		}
	}
	SJ_through (pars_seq, pars_n, pars_ev);
	*pars_seq = 0;
	pars_n = 0;
	pars_ev = 0;
	return (0);
}

control_flush (s, len)
char	*s;
int	len;
{
	SJ_through (s, len, pars_ev);
	*pars_seq = 0;
	pars_n = 0;
	pars_ev = 0;
}

KeySym
find_bc_key ()
{
	register int	i;

	for (i = 0 ; kassign[i].ksym != 0 ; i ++) {
		if (kassign[i].uval == KEY_CONV)
			return (kassign[i].ksym);
	}
	return (XK_Kanji);
}

Xlc_SetKeycode ()
{
	Window		win;
	Atom		k_prop;
	Atom		m_prop;
	unsigned int	keycode;
	unsigned int	modifier;
	extern Window	subwin;
	extern Display	*dpy;
	KeySym		ks;

	win = subwin;
	k_prop = XInternAtom (dpy, "_XLC_BC_KEYCODE", False);
	m_prop = XInternAtom (dpy, "_XLC_BC_MODIFIER", False);
	ks = find_bc_key ();
	keycode = XKeysymToKeycode (dpy, ks);
	if (ks == XK_Kanji)
		modifier = ShiftMask;
	else
		modifier = 0;

	XChangeProperty (
		dpy, win, k_prop, XA_INTEGER, 32,
		PropModeReplace, (char *)&keycode, 1
	);
	XChangeProperty (
		dpy, win, m_prop, XA_INTEGER, 32,
		PropModeReplace, (char *)&modifier, 1
	);
}
