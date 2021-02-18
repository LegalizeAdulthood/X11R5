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
static char *rcsid=  "$Header: /usr/local/src/x11r5/mit/extensions/RCS/MVEXRndVid.c,v 2.2 1991/09/19 22:26:28 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#include "Xlibint.h"
#include "MVEXlibint.h"
#include "Xutil.h"
#include "MVEXlib.h"
#include "MVEXproto.h"

/*
 *    NAME
 *        MVEXRenderVideo - render video to a window or pixmap
 *
 *    SYNOPSIS
 */
MVEXRenderVideo(dpy, src, dest, gc, port,
		src_x, src_y, srcWidth, srcHeight,
		dst_x, dst_y, dstWidth, dstHeight,
		valueMask, values)
    Display *dpy;		/* in: display pointer */
    VideoIn src;		/* in: video in */
    Drawable dest;		/* in: window or pixmap */
    GC gc;			/* in: gc for clip and subwindow mode */
    Atom port;			/* in: video in port id */
    int src_x, src_y;		/* in: source location */
    unsigned int srcWidth;	/* in: source width */
    unsigned int srcHeight;	/* in: source height */
    int dst_x, dst_y;		/* in: destination location */
    unsigned int dstWidth;	/* in: destination width */
    unsigned int dstHeight;	/* in: destination height */
    unsigned long valueMask;	/* in: bits designating values */
    MVEXVideoValues *values;	/* in: list of values */
/*
 *    DESCRIPTION
 *        Format and send the request.
 *
 *    RETURNS
 *        void
 */
{
    register xRenderVideoReq   *req;
    XExtCodes			    *codes;

    if (!(codes = _MVEXCheckExtension (dpy)))
	return;
    LockDisplay (dpy);
    GetReq (RenderVideo, req);
    req->reqType = codes->major_opcode;
    req->videoReqType = X_RenderVideo;
    req->src = src;
    req->dest = dest;
    req->gc = gc->gid;
    req->inport = port;
    req->srcX = src_x;
    req->srcY = src_y;
    req->srcWidth = srcWidth;
    req->srcHeight = srcHeight;
    req->dstX = dst_x;
    req->dstY = dst_y;
    req->dstWidth = dstWidth;
    req->dstHeight = dstHeight;
    if (req->valueMask = valueMask)
        _XProcessVideoValues (dpy, req, valueMask, values);

    UnlockDisplay (dpy);
    SyncHandle ();
}

_XProcessVideoValues (dpy, req, valueMask, values)
    register Display *dpy;
    xRenderVideoReq *req;
    register unsigned long valueMask;
    register MVEXVideoValues *values;
{

    /* Warning!  This code assumes that "unsigned long" is 32-bits wide */

    unsigned long valuesTmp[ 1 /* VRFullMotion */
			   + 1 /* VRPriority */
			   + 1 /* VRMarkerType */
			   + (sz_xFrame / 4) /* VRInFrame */
			   + (sz_xFrame / 4) /* VROutFrame */
			   + (sz_xTimecode / 4) /* VRInTimecode */
			   + (sz_xTimecode / 4) /* VROutTimecode */
			   + (sz_xFraction / 4) /* VRHue */
			   + (sz_xFraction / 4) /* VRSaturation */
			   + (sz_xFraction / 4) /* VRBrightness */
			   + (sz_xFraction / 4) /* VRContrast */
			   ];

    unsigned int nvalues;
    union {
	unsigned long *card32p;
	xFrame *framep;
	xTimecode *timep;
	xFraction *fracp;
    } u;

    u.card32p = valuesTmp;

    if (valueMask & VRFullMotion)
        *u.card32p++ = values->full_motion;

    if (valueMask & VRPriority)
        *u.card32p++ = values->priority;

    if (valueMask & VRMarkerType)
        *u.card32p++ = values->marker_type;

    if (valueMask & VRInFrame) {
        u.framep->negative = values->in_frame.negative;
        u.framep->frame = values->in_frame.frame;
        u.framep->field = values->in_frame.field;
	u.framep++;
    }

    if (valueMask & VROutFrame) {
        u.framep->negative = values->out_frame.negative;
        u.framep->frame = values->out_frame.frame;
        u.framep->field = values->out_frame.field;
	u.framep++;
    }

    if (valueMask & VRInTimecode) {
        u.timep->negative = values->in_timecode.negative;
        u.timep->hour = values->in_timecode.hour;
        u.timep->minute = values->in_timecode.minute;
        u.timep->second = values->in_timecode.second;
        u.timep->frame = values->in_timecode.frame;
        u.timep->field = values->in_timecode.field;
	u.timep++;
    }

    if (valueMask & VROutTimecode) {
        u.timep->negative = values->out_timecode.negative;
        u.timep->hour = values->out_timecode.hour;
        u.timep->minute = values->out_timecode.minute;
        u.timep->second = values->out_timecode.second;
        u.timep->frame = values->out_timecode.frame;
        u.timep->field = values->out_timecode.field;
	u.timep++;
    }

    if (valueMask & VRHue) {
        u.fracp->numerator = values->hue.numerator;
        u.fracp->denominator = values->hue.denominator;
	u.fracp++;
    }

    if (valueMask & VRSaturation) {
        u.fracp->numerator = values->saturation.numerator;
        u.fracp->denominator = values->saturation.denominator;
	u.fracp++;
    }

    if (valueMask & VRBrightness) {
        u.fracp->numerator = values->bright.numerator;
        u.fracp->denominator = values->bright.denominator;
	u.fracp++;
    }

    if (valueMask & VRContrast) {
        u.fracp->numerator = values->contrast.numerator;
        u.fracp->denominator = values->contrast.denominator;
	u.fracp++;
    }

    req->length += (nvalues = u.card32p - valuesTmp);

    nvalues <<= 2;                          /* watch out for macros... */
    Data32 (dpy, (long *) valuesTmp, (long)nvalues);
}
