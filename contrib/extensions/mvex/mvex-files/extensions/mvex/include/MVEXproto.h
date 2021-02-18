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
static char *rcsid=  "$Header: /n/lacey/usr/local/src/video/MVEX/include/RCS/MVEXproto.h,v 2.4 1991/09/26 04:46:05 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#ifndef _MVEXPROTO_H_
#define _MVEXPROTO_H_

#include <X11/Xmd.h>
#include <X11/Xprotostr.h>

/*
 * Video protocol data structures
 */
typedef struct {
	INT32	numerator B32;
	INT32	denominator B32;
} xFraction;
#define sz_xFraction 8

typedef struct {
	INT32		numBase B32;
	INT32		numIncrement B32;
	INT32		numLimit B32;
	INT32		denomBase B32;
	INT32		denomIncrement B32;
	INT32		denomLimit B32;
	CARD8		numType;
	CARD8		denomType;
	CARD16 		pad1;
} xFractionRange;
#define sz_xFractionRange 28

/*
 * Since the Frame and Timecode are only transmitted in a value list,
 * meaning that it must be a list of 32-bit values, they are declared
 * here as such, even though the protocol says they are BOOL, CARD8, etc.
 */
typedef struct {
	CARD32		negative B32;
	CARD32		frame B32;
	CARD32		field B32;
} xFrame;
#define sz_xFrame 12

typedef struct {
	CARD32		negative B32;
	CARD32		hour B32;
	CARD32		minute B32;
	CARD32		second B32;
	CARD32		frame B32;
	CARD32		field B32;
} xTimecode;
#define sz_xTimecode 24

typedef struct {
	xRectangle	base;
	xRectangle	limit;
	INT16		xInc B16;
	INT16		yInc B16;
	CARD16		widthInc B16;
	CARD16		heightInc B16;
	CARD8		type;
	CARD8		pad1;
	CARD16 		pad2;
} xRectangleRange;
#define sz_xRectangleRange 28

typedef struct {
	xFraction	frameRate;
	xRectangleRange	src;
	xRectangleRange	dest;
	xFractionRange	xScale;
	xFractionRange	yScale;
	BOOL		identityAspect;
	CARD8		pad1;
	CARD16		pad2;
} xPlacement;
#define sz_xPlacement 124

typedef struct {
	xFraction	normalSaturation;
	xFraction	normalContrast;
	xFraction	normalHue;
	xFraction	normalBright;
	CARD16		nSaturation;
	CARD16		nContrast;
	CARD16		nHue;
	CARD16		nBright;
} xVideoAbility;
#define sz_xVideoAbility 40

typedef struct {
	xFraction	frameRate;
	xFraction	fieldRate;
	CARD16		width B16;
	CARD16		height B16;
	CARD16		concurrentUse B16;
	CARD16		prioritySteps B16;
	CARD32		referenceId B32;
	CARD32		nPlacement B32;
} xVideoGeometry;
#define sz_xVideoGeometry 32

typedef struct {
	CARD8		depth;
	CARD8 		opaque;
	CARD16 		pad2;
	CARD32		visualid B32;
	CARD32		redMask B32;
	CARD32		greenMask B32;
	CARD32		blueMask B32;
} xRenderModel;
#define sz_xRenderModel 20

typedef struct {
	CARD32	window;
	CARD32	vid;
} xOwner;
#define sz_xOwner	8

/*
 * Video requests and replies
 */
typedef struct _QueryVideo {
	CARD8	reqType;		/* always VideoReqCode */
	CARD8	videoReqType;		/* always X_QueryVideo */
	CARD16	length B16;
	CARD32	window B32;
} xQueryVideoReq;
#define sz_xQueryVideoReq	8

typedef struct {
	BYTE	type;			/* X_Reply */
	CARD8	unused;			/* not used */
	CARD16	sequenceNumber B16;
	CARD32	length B32;
	CARD8	screen;
	CARD8	inputOverlap;
	CARD8	captureOverlap;
	CARD8	ioOverlap;
	CARD32	time B32;
	CARD16	majorVersion;
	CARD16	minorVersion;
	CARD8	nDepths;
	CARD8	nAllowedDepths;
	CARD8	nVin;
	CARD8	nVout;
	CARD32	pad1;
	CARD32	pad2;
} xQueryVideoReply;
#define sz_xQueryVideoReply	32

typedef struct _RenderVideo {
	CARD8	reqType;		/* always VideoReqCode */
	CARD8	videoReqType;		/* always X_RenderVideo */
	CARD16	length B16;
	CARD32	src B32;
	CARD32	inport B32;
	CARD32	dest B32;
	INT16	srcX B16;
	INT16	srcY B16;
	INT16	dstX B16;
	INT16	dstY B16;
	CARD16	srcWidth B16;
	CARD16	srcHeight B16;
	CARD16	dstWidth B16;
	CARD16	dstHeight B16;
	CARD32	gc B32;
	CARD32	valueMask B32;
} xRenderVideoReq;
#define sz_xRenderVideoReq	40

typedef struct _CaptureGraphics {
	CARD8	reqType;		/* always VideoReqCode */
	CARD8	videoReqType;		/* always X_CaptureGraphics */
	CARD16	length B16;
	CARD32	src B32;
	CARD32	outport B32;
	CARD32	dest B32;
	CARD32	cmap B32;
	INT16	srcX B16;
	INT16	srcY B16;
	INT16	dstX B16;
	INT16	dstY B16;
	CARD16	srcWidth B16;
	CARD16	srcHeight B16;
	CARD16	dstWidth B16;
	CARD16	dstHeight B16;
	CARD8	subWindowMode;
	CARD8	pad1;
	CARD16	pad2 B16;
	CARD32	valueMask B32;
} xCaptureGraphicsReq;
#define sz_xCaptureGraphicsReq	44

typedef struct _StopVideo {
	CARD8	reqType;		/* always VideoReqCode */
	CARD8	videoReqType;		/* always X_StopVideo */
	CARD16	length B16;
	CARD8	action;
	CARD8	junk1;		/* not used */
	CARD16	junk2 B16;	/* not used */
} xStopVideoReq;
#define sz_xStopVideoReq	8

typedef struct _QuerySetup {
	CARD8	reqType;		/* always VideoReqCode */
	CARD8	videoReqType;		/* always X_QuerySetup */
	CARD16	length B16;
	CARD32	id B32;
	CARD32	atom B32;
} xQuerySetupReq;
#define sz_xQuerySetupReq	 12

typedef struct {
	BYTE	type;			/* X_Reply */
	CARD8	unused;			/* not used */
	CARD16	sequenceNumber B16;
	CARD32	length B32;
	CARD8	framesAvailable;
	CARD8	timecodesAvailable;
	CARD8	frameAccurate;
	CARD8	pad1;
	CARD32	pad2;
	CARD32	pad3;
	CARD32	pad4;
	CARD32	pad5;
	CARD32	pad6;
} xQuerySetupReply;
#define sz_xQuerySetupReply	32

typedef struct _CreateVideo {
	CARD8	reqType;		/* always VideoReqCode */
	CARD8	videoReqType;		/* always X_CreateVideo */
	CARD16	length B16;
	CARD32	id B32;
	CARD32	reference B32;
} xCreateVideoReq;
#define sz_xCreateVideoReq	 12

typedef struct _ChangeOwnership {
	CARD8	reqType;		/* always VideoReqCode */
	CARD8	videoReqType;		/* always X_ChangeOwnership */
	CARD16	length B16;
} xChangeOwnershipReq;
#define sz_xChangeOwnershipReq	 4

typedef struct _SelectVideoEvents {
	CARD8	reqType;		/* always VideoReqCode */
	CARD8	videoReqType;		/* always X_SelectVideoEvents */
	CARD16	length B16;
	CARD32	id B32;
	CARD32	mask B32;
} xSelectVideoEventsReq;
#define sz_xSelectVideoEventsReq	 12

/*
 * Video events
 */
typedef struct _VideoViolation {
	BYTE	type;			/* always eventBase + VideoViolation */
	CARD8	unused;			/* not used */
	CARD16	sequenceNumber B16;
	CARD32	id B32;
	CARD32	vid B32;
	CARD32	time B32;		/* server time */
	CARD16	actionMask B16;		/* Scale, Placement, Clip, Overlap */
	BYTE	state;			/* Success, Fail, Subset */
	CARD8	pad1;
	CARD32	pad2 B32;
	CARD32	pad3 B32;
	CARD32	pad4 B32;
} xVideoViolationEvent;
#define sz_xVideoViolationEvent	32

typedef struct _VideoSetup {
	BYTE	type;			/* always eventBase + VideoSetup */
	CARD8	unused;			/* not used */
	CARD16	sequenceNumber B16;
	CARD32	vid B32;
	CARD32	port B32;
	CARD32	time B32;		/* server time */
	CARD8	framesAvailable;
	CARD8	timecodesAvailable;
	CARD8	frameAccurate;
	CARD8	pad1;
	CARD32	pad2 B32;
	CARD32	pad3 B32;
	CARD32	pad4 B32;
} xVideoSetupEvent;
#define sz_xVideoSetupEvent	32

typedef struct _VideoSync {
	BYTE	type;			/* always eventBase + VideoSync */
	CARD8	unused;			/* not used */
	CARD16	sequenceNumber B16;
	CARD32	id B32;
	CARD32	vid B32;
	CARD32	time B32;		/* server time */
	BYTE	state;			/* Acquired or Lost */
	CARD8	pad1;
	CARD16	pad2 B16;
	CARD32	pad3 B32;
	CARD32	pad4 B32;
	CARD32	pad5 B32;
} xVideoSyncEvent;
#define sz_xVideoSyncEvent	32

typedef struct _VideoOverride {
	BYTE	type;			/* always eventBase + VideoOverride */
	CARD8	unused;			/* not used */
	CARD16	sequenceNumber B16;
	CARD32	id B32;
	CARD32	vid B32;
	CARD32	overrideId B32;
	CARD32	time B32;		/* server time */
	CARD8	state;
	CARD8	pad1;
	CARD16	pad2 B16;
	CARD32	pad3 B32;
	CARD32	pad4 B32;
} xVideoOverrideEvent;
#define sz_xVideoOverrideEvent	32

typedef struct _VideoRequest {
	BYTE	type;			/* always eventBase + VideoRequest */
	CARD8	unused;
	CARD16	sequenceNumber B16;
	BYTE	request;		/* Render or Capture */
	BYTE	redirect;
	CARD16	pad1 B16;
	CARD32	src B32;
	CARD32	dst B32;
	CARD32	time B32;
	CARD32	pad2 B32;
	CARD32	pad3 B32;
	CARD32	pad4 B32;
} xVideoRequestEvent;
#define sz_xVideoRequestEvent	32

/*
 * Video Requests
 */
#define X_CaptureGraphics		0
#define X_ChangeOwnership		1
#define X_CreateVideo			2
#define X_QueryVideo			3
#define X_QuerySetup			4
#define X_RenderVideo			5
#define X_SelectVideoEvents		6
#define X_StopVideo			7

#endif /* _MVEXPROTO_H_ */
