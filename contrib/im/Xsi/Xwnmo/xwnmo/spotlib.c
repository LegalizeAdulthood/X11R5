/*
 * $Id: spotlib.c,v 1.2 1991/09/16 21:37:21 ohm Exp $
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

#include <X11/Xlib.h>
#include "Xi18nint.h"
#include "XIMlibint.h"
#include "SpotProto.h"

void
_XipChangeSpot(ic, x, y)
    XIC ic;
    short x, y;
{
    XipIM im = ipIMofIC((XipIC)ic);
    ximChangeSpotReq	req;
    ximEventReply	reply;

    if (im->fd < 0) {
	return;
    }
    req.reqType = XIM_ChangeSpot;
    req.length = sz_ximChangeSpotReq;
    req.xic = ((XipIC)ic)->icid;
    req.spot_x = x;
    req.spot_y = y;
    if ((_XipWriteToIM(im, (char *)&req, sz_ximChangeSpotReq) >= 0) &&
	(_XipFlushToIM(im) >= 0)) {
	reply.state = 0;
	_XipReadFromIM(im, (char *)&reply, sz_ximEventReply);
    }
    return;
}
