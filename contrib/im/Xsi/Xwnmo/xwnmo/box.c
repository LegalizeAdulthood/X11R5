/*
 * $Id: box.c,v 1.3 1991/09/16 21:36:27 ohm Exp $
 */
/*
 * Copyright 1989, 1990, 1991 by OMRON Corporation
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON and MIT not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON and MIT make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * OMRON, MIT AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL OMRON, MIT OR WNN CONSORTIUM BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	Author:	Seiji Kuwari	OMRON Corporation
 *				kuwa@omron.co.jp
 *				kuwa%omron.co.jp@uunet.uu.net
 */				
/*	Version 4.0
 */
#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#ifdef  XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#else   /* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#endif  /* XJUTIL */

BoxRec *
create_box(parent, x, y, width, height, bw, fg, bg, cursor, rev)
Window parent;
int x, y;
int width, height;
int bw;
unsigned long fg, bg;
Cursor cursor;
char rev;
{
    register BoxRec *box;
    XGCValues	xgcv;

    if ((box = (BoxRec *)Malloc(sizeof(BoxRec))) == NULL) {
	return(NULL);
    }
    box->x = x;
    box->y = y;
    box->width = width;
    box->height = height;
    box->border_width = bw;
    box->fg = fg;
    box->bg = bg;
    box->redraw_cb = NULL;
    box->redraw_cb_data = NULL;
    box->cb = NULL;
    box->cb_data = NULL;
    box->do_ret = False;
    box->sel_ret = -1;
    box->reverse = rev;
    box->in = box->map = '\0';
    box->freeze = '\0';
    box->next = NULL;
    box->window = XCreateSimpleWindow(dpy, parent, (x - bw), (y - bw),
				      width, height, bw, fg, bg);
    xgcv.foreground = fg;
    xgcv.function = GXinvert;
    xgcv.plane_mask = XOR(fg, bg);
    box->invertgc = XCreateGC(dpy, root_window, (GCForeground | GCFunction |
						 GCPlaneMask), &xgcv);

    if (cursor) XDefineCursor(dpy, box->window, cursor);
    if (rev) {
	XSelectInput(dpy, box->window, (ButtonPressMask | ButtonReleaseMask |
					EnterWindowMask |
					LeaveWindowMask | ExposureMask));
    } else {
	XSelectInput(dpy, box->window, (ButtonPressMask | ButtonReleaseMask |
					ExposureMask));
    }
    box->next = box_list;
    box_list = box;
    return(box);
}

int
remove_box(b)
register BoxRec *b;
{
    register BoxRec *p, **prev;
    for (prev = &box_list; p = *prev; prev = &p->next) {
	if (p == b) {
	    *prev = p->next;
	    Free((char *)p);
	    return(0);
	}
    }
    return(-1);
}

void
map_box(p)
register BoxRec *p;
{
    XMapWindow(dpy, p->window);
    p->map = 1;
}

void
unmap_box(p)
register BoxRec *p;
{
    XUnmapWindow(dpy, p->window);
    p->map = '\0';
    p->in = '\0';
}

void
freeze_box(p)
register BoxRec *p;
{
    p->freeze = 1;
}

void
unfreeze_box(p)
register BoxRec *p;
{
    p->freeze = '\0';
}

void
moveresize_box(p, x, y, width, height)
register BoxRec *p;
int x, y;
int width, height;
{
    p->x = x;
    p->y = y;
    p->width = width;
    p->height = height;
    XMoveResizeWindow(dpy, p->window, (x - p->border_width),
		      (y - p->border_width), width, height);
}

void
changecolor_box(p, fg, bg)
register BoxRec *p;
unsigned long fg, bg;
{
    XGCValues	xgcv;

    if (bg != p->bg) {
	p->bg = bg;
	XSetWindowBackground(dpy, p->window, p->bg);
	xgcv.background = bg;
	xgcv.plane_mask = XOR(p->fg, p->bg);
	XChangeGC(dpy, p->invertgc, (GCPlaneMask | GCBackground), &xgcv);
	XClearWindow(dpy, p->window);
    }
    if (fg != p->fg) {
	p->fg = fg;
	xgcv.foreground = fg;
	xgcv.plane_mask = XOR(p->fg, p->bg);
	XChangeGC(dpy, p->invertgc, (GCPlaneMask | GCForeground), &xgcv);
	XSetWindowBorder(dpy, p->window, p->fg);
    }
}

void
redraw_box(p)
register BoxRec *p;
{
    XClearWindow(dpy, p->window);
    JW3Mputc(p->string, p->window, 0, (int)p->in, IN_BORDER);
}

void
reverse_box(p, invertgc)
register BoxRec *p;
GC invertgc;
{
    XFillRectangle(dpy, p->window, invertgc,  0, 0, p->width, p->height);
}
