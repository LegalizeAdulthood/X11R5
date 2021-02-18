/*
 * $Id: xw_touroku.c,v 1.3 1991/09/16 21:35:37 ohm Exp $
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

#include <stdio.h>
#include <string.h>
#include "commonhd.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"

extern wchar	wc_buf[];

extern XSizeHints hints;

void
xw_jutil_write_msg(msg)
unsigned char	*msg;
{
    int	wc_len = 512;

    wc_len = char_to_wchar(xjutil->xlc, msg, wc_buf, strlen((char*)msg),
			   wc_len);
    JW1Mputwc(wc_buf, wc_len);
    if (cur_text->currentcol > cur_text->max_pos) {
	cur_text->max_pos = cur_text->currentcol;
    }
    XFlush(dpy);
}

void
init_jutil(title, exec_t, cnt, t1, mes1)
unsigned char *title, *exec_t;
int cnt;
unsigned char *t1[];
unsigned char *mes1[];
{
    JutilRec *jutil = xjutil->jutil;
    int	width, height, x_cood, y_cood;
    int	save_x, save_y;
    Window root, child;
    int	win_x, win_y;
    unsigned int keys_buttons;
    int i;
    int len_t, max_mes_t;

    XSizeHints hints;

    xjutil->mode |= JUTIL_MODE;


    jutil->title->string = title;
    jutil->button[EXEC_W]->string = exec_t;
    jutil->max_mes = cnt;
    for (i = 0; i < jutil->max_mes; i++)
	jutil->mes_button[i]->string = NULL;

    moveresize_box(jutil->title, FontWidth, FontHeight/2,
		   (FontWidth * (len_t = strlen((char*)title)) + IN_BORDER * 2),
		   (FontHeight + IN_BORDER * 2));
    len_t += 1;
    moveresize_box(jutil->button[CANCEL_W],
		   (FontWidth * (len_t += SPACING)), FontHeight/2,
		   (FontWidth * strlen((char*)jutil->button[CANCEL_W]->string)
		    + IN_BORDER * 2), FontHeight + IN_BORDER * 2);
    len_t += strlen((char*)jutil->button[CANCEL_W]->string);
    moveresize_box(jutil->button[EXEC_W],
		   (FontWidth * (len_t += SPACING)), FontHeight/2,
		   (FontWidth * strlen((char*)jutil->button[EXEC_W]->string)
		    + IN_BORDER * 2), FontHeight + IN_BORDER * 2);
    XSetWindowBackground(dpy, jutil->w, xjutil->bg);
    changecolor_box(jutil->title, xjutil->fg, xjutil->bg);
    changecolor_box(jutil->button[CANCEL_W], xjutil->fg, xjutil->bg);
    changecolor_box(jutil->button[EXEC_W], xjutil->fg, xjutil->bg);

    jutil->mode = MESSAGE1;
    max_mes_t = 0;
    for (i = 0; i < jutil->max_mes; i++) {
	jutil->mes_button[i]->string = t1[i];
	cur_text = jutil->mes_text[i];
	JWMline_clear(0);
	moveresize_box(jutil->mes_button[i],
		       FontWidth, (FontHeight * (3 + i * 2)),
		       (FontWidth *
		        (len_t = strlen((char*)jutil->mes_button[i]->string))
			+ IN_BORDER * 2),
		       (FontHeight + IN_BORDER * 2));
	changecolor_box(jutil->mes_button[i], xjutil->fg, xjutil->bg);
	map_box(jutil->mes_button[i]);

        if (len_t > max_mes_t) max_mes_t = len_t;
    }
    for (i = 0; i < jutil->max_mes; i++) {
	XMoveResizeWindow(dpy, jutil->mes_text[i]->wp,
			  (FontWidth * (max_mes_t + SPACING)), 
			  (FontHeight * (3 + i * 2)),
			  (FontWidth * TEXT_WIDTH), FontHeight);
	XSetWindowBackground(dpy, jutil->mes_text[i]->wp, xjutil->bg);
	XClearWindow(dpy, jutil->mes_text[i]->wp);
	resize_text(jutil->mes_text[i]);
	XMapWindow(dpy, jutil->mes_text[i]->wp);
	if (mes1[i] && *mes1[i]) {
	    cur_text = jutil->mes_text[i];
	    JWMline_clear(0);
	    xw_jutil_write_msg(mes1[i]);
	}
    }

    width = FontWidth * (TEXT_WIDTH + max_mes_t + SPACING * 2);
    height = FontHeight * (3 + cnt * 2);

    moveresize_box(jutil->rk_mode,
		   (width - FontWidth * (MHL0 + SPACING)), FontHeight/2,
		   (FontWidth * MHL0 + IN_BORDER * 2),
		   (FontHeight + IN_BORDER * 2));
    changecolor_box(jutil->rk_mode, xjutil->fg, xjutil->bg);

    XQueryPointer(dpy, root_window, &root, &child,
			&save_x, &save_y, &win_x, &win_y, &keys_buttons);
    jutil->save_p.x = (short)save_x;
    jutil->save_p.y = (short)save_y;
    decide_position(width, height, &x_cood, &y_cood);
    XWarpPointer(dpy, None, root_window, 0, 0, 0, 0,
		 x_cood + FontWidth * 2, y_cood + FontHeight);

    XMoveResizeWindow(dpy, jutil->w, x_cood - 2, y_cood - 2, width, height);
    hints.flags = (USSize | USPosition | PMinSize | PMaxSize | PResizeInc
#ifndef	X11R3
		   | PBaseSize | PWinGravity
#endif	 /* X11R3 */
		  );
    hints.x = x_cood;
    hints.y = y_cood;
    hints.width = hints.min_width = hints.max_width = width;
    hints.height = hints.min_height = hints.max_height = height;
    hints.width_inc = hints.height_inc = 0;
#ifndef	X11R3
    hints.base_width = width;
    hints.base_height = height;
    hints.win_gravity = CenterGravity;
    XSetWMNormalHints(dpy, jutil->w, &hints);
#else	/* X11R3 */
    XSetNormalHints(dpy, jutil->w, &hints);
#endif	/* X11R3 */

    XRaiseWindow(dpy, jutil->w);
    XMapWindow(dpy, jutil->w);

    disp_mode();
    XFlush(dpy);

}

static void
clear_jutil_buf(buf)
int *buf;
{
	register int *x;

	x = buf;
	for (; *x != 0; x++) {
		*x = 0;
	}
}

void
end_jutil()
{
    int i;
    JutilRec *jutil = xjutil->jutil;

    for (i = 0; i < jutil->max_mes; i++)  {
	clear_jutil_buf(xjutil->jutil->mes_text[i]->buf);
	unmap_box(xjutil->jutil->mes_button[i]);
	XUnmapWindow(dpy, xjutil->jutil->mes_text[i]->wp);
    }
    XUnmapWindow(dpy, xjutil->jutil->w);
    if (jutil->save_p.x > -1) {
    	XWarpPointer(dpy, None, root_window, 0, 0, 0, 0,
		     jutil->save_p.x, jutil->save_p.y);
	jutil->save_p.x = jutil->save_p.y = -1;
    }
    XFlush(dpy);
    jutil->button[EXEC_W]->in = jutil->button[CANCEL_W]->in = 0;
    xjutil->mode &= ~JUTIL_MODE;
    xjutil->sel_button = 0;
    xjutil->sel_ret = -1;
}

void
change_cur_jutil(cur)
int cur;
{
    cur_text = xjutil->jutil->mes_text[cur];
}

void
hanten_jutil_mes_title(mes, rev)
int	mes;
int	rev;
{
    xjutil->jutil->mes_button[mes]->in = rev;
    redraw_box(xjutil->jutil->mes_button[mes]);
}

void
change_mes_title(mes, msg, rev)
int mes;
unsigned char *msg;
int rev;
{
    xjutil->jutil->mes_button[mes]->string = msg;
    hanten_jutil_mes_title(mes, rev);

}

