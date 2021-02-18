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
static char *rcsid=  "$Header: /n/lacey/usr/local/src/video/MVEX/include/RCS/MVEX.h,v 2.3 1991/09/26 03:59:20 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#ifndef _MVEX_H_
#define _MVEX_H_

/*
 * Protocol requests constants and alignment values
 */

#define MVEX_NAME "MVEX"

#define MVEX_MAJOR_VERSION	1	/* current version numbers */
#define MVEX_MINOR_VERSION	0

/*
 * Resources
 */
typedef XID VideoIn;
typedef XID VideoOut;
typedef XID VideoIO;
typedef XID VideoReference;

/*
 * RANGE types
 */
#define MVEXGeometricRange	0
#define MVEXLinearRange		1

/*
 * Marker types
 */
#define MVEXFrameMarker		1
#define MVEXTimecodeMarker	2

/*
 * StopVideo Actions
 */
#define MVEXStopRenderVideo	(1 << 0)
#define MVEXStopCaptureGraphics	(1 << 1)

/*
 * MVEXRequest Requests
 */
#define MVEXRender		0
#define MVEXCapture		1

/*
 * Video Request (RenderVideo and CaptureGraphics) value-mask bits
 */
#define VRFullMotion	0x00000001
#define VRPriority	0x00000002
#define VRMarkerType	0x00000004
#define VRInFrame	0x00000008
#define VROutFrame	0x00000010
#define VRInTimecode	0x00000020
#define VROutTimecode	0x00000040
#define VRHue		0x00000080
#define VRSaturation	0x00000100
#define VRBrightness	0x00000200
#define VRContrast	0x00000400

/*
 * MVEXViolation actionMask values
 */
#define MVEXScaleViolation	(1L << 0)
#define MVEXPlacementViolation	(1L << 1)
#define MVEXClipViolation	(1L << 2)
#define MVEXOverlapViolation	(1L << 3)
#define MVEXHueViolation	(1L << 4)
#define MVEXSaturationViolation	(1L << 5)
#define MVEXContrastViolation	(1L << 6)
#define MVEXBrightnessViolation	(1L << 7)

/*
 * MVEXViolation event states
 */
#define MVEXViolationSuccess	0
#define MVEXViolationFail	1
#define MVEXViolationSubset	2

/*
 *  Values for digitization controls
 */
#define MVEXFieldOne		1
#define MVEXFieldTwo		2
#define MVEXFieldBoth		3

/*
 * Values for video adjustments
 */
#define MVEXSaturation		0x01
#define MVEXContrast		0x02
#define MVEXHue			0x04
#define MVEXBright		0x08

/*
 * IO Model Bitmasks
 */
#define MVEXWindowModelMask     (1 << 0)
#define MVEXPixmapModelMask     (1 << 1)
#define MVEXCompositeModelMask  (1 << 2)

/*
 * MVEXSync event states
 */
#define MVEXSyncAcquired	0
#define MVEXSyncLost		1

/*
 * MVEXOverride event states
 */
#define MVEXStopped		0
#define MVEXDrawableChange	1
#define MVEXMarkerReached	2
#define MVEXRequestOverride	3
#define MVEXHardwareError	4
#define MVEXReused		5

/*
 * MVEX event masks
 */
#define MVEXRedirectMask	(1L << 0)
#define MVEXSetupMask		(1L << 1)
#define MVEXSyncMask		(1L << 2)
#define MVEXOverrideMask	(1L << 3)
#define MVEXViolationMask	(1L << 4)

/*
 * Video events
 */
#define MVEXViolation		0
#define MVEXSetup		1
#define MVEXSync		2
#define MVEXOverride		3
#define MVEXRequest		4

/*
 * MVEX visual classes.
 */
#define MVEXVideoColor		6
#define MVEXVideoGray		8

#define BadVideo		0


#endif /* _MVEX_H_ */
