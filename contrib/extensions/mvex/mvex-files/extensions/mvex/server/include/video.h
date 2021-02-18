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
static char *rcsid=  "$Header: /n/lacey/usr/local/src/video/MVEX/server/include/RCS/video.h,v 2.2 1991/09/19 22:33:15 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#ifndef _VIDEO_H_
#define _VIDEO_H_

/*
 * Bits for constraints given to vmiClipAndScale().
 */
#define ConstrainSrcX		0x01
#define ConstrainSrcY		0x02
#define ConstrainSrcW		0x04
#define ConstrainSrcH		0x08
#define ConstrainDstX		0x10
#define ConstrainDstY		0x20
#define ConstrainDstW		0x40
#define ConstrainDstH		0x80
#define ConstrainSrcVertical	(ConstrainSrcY | ConstrainSrcH)
#define ConstrainDstVertical	(ConstrainDstY | ConstrainDstH)
#define ConstrainSrcHorizontal	(ConstrainSrcX | ConstrainSrcW)
#define ConstrainDstHorizontal	(ConstrainDstX | ConstrainDstW)
#define ConstrainHorizontal	(ConstrainSrcHorizontal \
			       | ConstrainDstHorizontal)
#define ConstrainVertical	(ConstrainSrcVertical \
			       | ConstrainDstVertical)

/*
 * Constants for VideoDrawRec (per drawable resource)
 * These work for either CaptureGraphics or RenderVideo
 */
#define	NoVideo			0	/* vanilla window */
#define	StillVideo		1	/* if the hardware makes distinction */
#define	MotionVideo		2	/* motion */
#define	SuspendedStillVideo	-1	/* -StillVideo */
#define	SuspendedMotionVideo	-2	/* -MotionVideo */

/*
 * Values to mark specified markers
 */
#define FrameInMarker		0x1
#define FrameOutMarker		0x2
#define TimecodeInMarker	0x4
#define TimecodeOutMarker	0x8

/*
 * values for VideoElement->type
 */
#define Input                           0
#define Output                          1

#define AllMVEXEventMask	\
	(MVEXRedirectMask|MVEXSyncMask|MVEXOverrideMask|MVEXViolationMask)


#define VideoNumberErrors		1
#define VideoNumberEvents		5

#define BAD_VIDEO	(VideoErrorBase + BadVideo)

#define SCREEN_HAS_VIDEO(pvs) \
    (pvs->vElements)

#define USE_SATURATED(vRes) \
    (vRes->useCount >= vRes->maxUse)
#define VDRAW_INACTIVE(vDraw)					\
    (! vDraw->captureActive && ! vDraw->renderActive)
#define VIO_ACTIVE(vDraw, vio)					\
    ((vDraw->captureActive && vio == vDraw->captureAction.videoIO)	\
  || (vDraw->renderActive && vio == vDraw->renderAction.videoIO))
#define VIDEO_RUNNING(vRes)					\
    ((vRes->vElement->type == Input && vDraw->renderAction.running) \
  || (vRes->vElement->type == Output && vDraw->captureAction.running))

extern void SVideoViolationEvent();
extern void SVideoSyncEvent();
extern void SVideoSetupEvent();
extern void SVideoOverrideEvent();
extern void SVideoRequestEvent();
extern int SProcQueryVideo ();
extern int SProcQuerySetup ();
extern int SProcCreateVideo ();
extern int SProcRenderVideo ();
extern int SProcCaptureGraphics ();
extern int SProcStopVideo ();
extern int SProcChangeOwnership ();
extern int SProcSelectVideoEvents ();
extern int SProcVideoDispatch ();
extern void SwapSettingList();
extern void SwapFraction();
extern void SwapDepths();
extern void SwapRenderModels();
extern void SwapVideoGeometries();
extern void SQueryVideoReply();
extern void SQuerySetupReply();

extern void SendVideoOverrideEvent();
extern void SendVideoViolationEvent();
extern void SendVideoRequestEvent();
extern void SendVideoSyncEvent();
extern void SendVideoSetupEvent();

#endif
