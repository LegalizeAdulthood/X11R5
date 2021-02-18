/*
 * Copyright 1990 by Tektronix, Inc., Beaverton, Oregon, and
 * the Massachusetts Institute of Technology, Cambridge, Massachusetts.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Tektronix or M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Tektronix and M.I.T. make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * TEKTRONIX AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL TEKTRONIX OR M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: /usr/local/src/x11r5.mvex/RCS/MVEXCaptGr.c,v 2.1 1991/08/13 19:46:19 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#include "Xlibint.h"
#include "MVEXlibint.h"
#include "Xutil.h"
#include "MVEXlib.h"
#include "MVEXproto.h"

/*
 *    NAME
 *        MVEXCaptureGraphics - capture graphics, send it out as video
 *
 *    SYNOPSIS
 */
MVEXCaptureGraphics(dpy, src, dest, cmap, port,
		    src_x, src_y, srcWidth, srcHeight,
		    dst_x, dst_y, dstWidth, dstHeight,
		    subWindowMode, valueMask, values)
    Display *dpy;		/* in: display pointer */
    Drawable src;		/* in: window or pixmap */
    VideoOut dest;		/* in: video out */
    Colormap cmap;		/* in: color map */
    Atom port;			/* in: video out port id */
    int src_x, src_y;		/* in: source location */
    unsigned int srcWidth;	/* in: source width */
    unsigned int srcHeight;	/* in: source height */
    int dst_x, dst_y;		/* in: destination location */
    unsigned int dstWidth;	/* in: destination width */
    unsigned int dstHeight;	/* in: destination height */
    int subWindowMode;		/* in: ClipByChildren or IncludeInferiors */
    unsigned long valueMask;    /* in: bits designating values */
    MVEXVideoValues *values;    /* in: list of values */
/*
 *    DESCRIPTION
 *        Format and send the CaptureGraphics request.
 *
 *    RETURNS
 *        void
 */
{
    register xCaptureGraphicsReq   *req;
    XExtCodes			    *codes;

    if (!(codes = _MVEXCheckExtension (dpy)))
	return;
    LockDisplay (dpy);
    GetReq (CaptureGraphics, req);
    req->reqType = codes->major_opcode;
    req->videoReqType = X_CaptureGraphics;
    req->src = src;
    req->dest = dest;
    req->cmap = cmap;
    req->outport = port;
    req->srcX = src_x;
    req->srcY = src_y;
    req->srcWidth = srcWidth;
    req->srcHeight = srcHeight;
    req->dstX = dst_x;
    req->dstY = dst_y;
    req->dstWidth = dstWidth;
    req->dstHeight = dstHeight;
    req->subWindowMode = subWindowMode;

    if (req->valueMask = valueMask)
        _XProcessVideoValues (dpy, (xRenderVideoReq *)req,
                        valueMask, values);
    UnlockDisplay (dpy);
    SyncHandle ();
}
