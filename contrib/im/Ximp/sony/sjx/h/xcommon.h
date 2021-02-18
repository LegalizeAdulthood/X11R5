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
 *	xcommon.h
 *	Copyright (c) 1988 1989 Sony Corporation.
 */
/*
 * $Header: xcommon.h,v 1.1 91/09/04 23:09:10 root Locked $ SONY;
 */

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "sjxparam.h"

extern unsigned long		black;
extern unsigned long		white;
extern int			screen;
extern int			depth;
extern int			bwidth;
extern unsigned long		fore, back;
extern int			g_fore, g_back;
extern int			bord;
extern char			*forecolor, *backcolor;
extern char			*geom;
extern char			*display;
extern Display			*dpy;
extern Window			root;
extern Window			root0;
extern Window			win;
extern Window			topwin;
extern Window			subwin;
extern Window			txtwin;
extern Window			def_txtwin;
extern Visual			*visual;
extern XSetWindowAttributes	attributes;
extern XEvent			event;
extern XSizeHints		shint;
extern GC			gcTFore;
extern GC			gcTBack;
extern GC			gcTJFore;
extern GC			gcTJBack;
extern GC			gcCFore;
extern GC			gcCBack;
extern GC			gcCJFore;
extern GC			gcCJBack;
extern GC			gcGFore;
extern GC			gcGBack;
extern GC			gcGJFore;
extern GC			gcGJBack;
extern GC			gcTEFore;
extern GC			gcTEBack;
extern GC			gcCEFore;
extern GC			gcCEBack;
extern GC			gcGEFore;
extern GC			gcGEBack;
extern GC			gcMove;
extern GCs			*t_gcs;
extern GCs			*g_gcs;
extern GCs			*c_gcs;
