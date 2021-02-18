/*
 * $Id: byte_swap.c,v 1.2 1991/09/16 21:36:28 ohm Exp $
 */
/*
 * Copyright 1991 by OMRON Corporation
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

#include "commonhd.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "proto.h"
#include "ext.h"

unsigned long
byteswap_l(l)
unsigned long l;
{
    register unsigned char *c = (unsigned char*)&l;
    return((*(c+3) << 24) | (*(c+2) << 16) | (*(c+1) << 8) | (*c));
}

unsigned short
byteswap_s(s)
unsigned short s;
{
    register unsigned char *c = (unsigned char*)&s;
    return((*(c+1) << 8) | (*c));
}

void
byteswap_xevent(ev)
register XKeyEvent *ev;
{
    ev->serial = byteswap_s(ev->serial);
    ev->time = byteswap_l(ev->time);
    ev->root = byteswap_l(ev->root);
    ev->window = byteswap_l(ev->window);
    ev->subwindow = byteswap_l(ev->subwindow);
    ev->x_root = byteswap_s(ev->x_root);
    ev->y_root = byteswap_s(ev->y_root);
    ev->x = byteswap_s(ev->x);
    ev->y = byteswap_s(ev->y);
    ev->keycode = byteswap_s(ev->keycode);
}

void
byteswap_GetIMReply(p)
register ximGetIMReply *p;
{
    p->state = byteswap_s(p->state);
    p->num_styles = byteswap_s(p->num_styles);
    p->nbytes = byteswap_s(p->nbytes);
}

void
byteswap_CreateICReply(p)
register ximCreateICReply *p;
{
    p->state = byteswap_s(p->state);
    p->detail = byteswap_s(p->detail);
    p->xic = byteswap_l(p->xic);
}

void
byteswap_GetICReply(p)
register ximGetICReply *p;
{
    p->state = byteswap_s(p->state);
    p->detail = byteswap_s(p->detail);
}

void
byteswap_ICValuesReq(p)
register ximICValuesReq *p;
{
    p->mask = byteswap_l(p->mask);
    p->input_style = byteswap_l(p->input_style);
    p->c_window = byteswap_l(p->c_window);
    p->focus_window = byteswap_l(p->focus_window);
    p->filter_events = byteswap_l(p->filter_events);
    p->max_keycode = byteswap_l(p->max_keycode);
    p->nbytes = byteswap_s(p->nbytes);
    p->nbytes2 = byteswap_s(p->nbytes2);
}

void
byteswap_ICAttributesReq(p)
register ximICAttributesReq *p;
{
    p->area_x = byteswap_s(p->area_x);
    p->area_y = byteswap_s(p->area_y);
    p->area_width = byteswap_s(p->area_width);
    p->area_height = byteswap_s(p->area_height);
    p->areaneeded_width = byteswap_s(p->areaneeded_width);
    p->areaneeded_height = byteswap_s(p->areaneeded_height);
    p->spot_x = byteswap_s(p->spot_x);
    p->spot_y = byteswap_s(p->spot_y);
    p->colormap = byteswap_l(p->colormap);
    p->std_colormap = byteswap_l(p->std_colormap);
    p->foreground = byteswap_l(p->foreground);
    p->background = byteswap_l(p->background);
    p->pixmap = byteswap_l(p->pixmap);
    p->line_space = byteswap_s(p->line_space);
    p->cursor = byteswap_l(p->cursor);
    p->nfonts = byteswap_s(p->nfonts);
    p->nbytes = byteswap_s(p->nbytes);
}

void
byteswap_EventReply(p)
register ximEventReply *p;
{
    p->state = byteswap_s(p->state);
    p->detail = byteswap_s(p->detail);
    p->number = byteswap_s(p->number);
}

void
byteswap_ReturnReply(p)
register ximReturnReply *p;
{
    p->type = byteswap_s(p->type);
    p->length = byteswap_s(p->length);
    p->keysym = byteswap_l(p->keysym);
}

#ifdef	CALLBACKS
void
byteswap_StatusDrawReply(p)
register ximStatusDrawReply *p;
{
    p->type = byteswap_s(p->type);
    p->encoding_is_wchar = byteswap_s(p->encoding_is_wchar);
    p->length = byteswap_s(p->length);
    p->feedback = byteswap_s(p->feedback);
    p->bitmap = byteswap_l(p->bitmap);
}

void
byteswap_PreDrawReply(p)
register ximPreDrawReply *p;
{
    p->caret = byteswap_s(p->caret);
    p->chg_first = byteswap_s(p->chg_first);
    p->chg_length = byteswap_s(p->chg_length);
    p->encoding_is_wchar = byteswap_s(p->encoding_is_wchar);
    p->length = byteswap_s(p->length);
    p->feedback = byteswap_l(p->feedback);
}

void
byteswap_PreCaretReply(p)
register ximPreCaretReply *p;
{
    p->position = byteswap_s(p->position);
    p->direction = byteswap_s(p->direction);
    p->style = byteswap_s(p->style);
}
#endif	/* CALLBACKS */

