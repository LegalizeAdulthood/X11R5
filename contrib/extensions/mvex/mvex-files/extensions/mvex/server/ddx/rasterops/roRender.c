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
static char *rcsid=  "$Header: /n/lacey/usr/local/src/video/MVEX/server/ddx/rasterops/RCS/roRender.c,v 1.11 1991/09/26 21:23:08 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#include "X.h"
#include "scrnintstr.h"
#include "regionstr.h"
#include "misc.h"
#include "MVEX.h"
#include "MVEXproto.h"
#include "videostr.h"
#include "vmi.h"
#include "roStruct.h"
#include "roMvex.h"

#include <stdio.h>

/*
 *    NAME
 *        roConstrainer - constrain source y and height
 *
 *    SYNOPSIS
 */
/*ARGSUSED*/
static int
roConstrainer(constraint, value)
    int constraint;
    int value;
/*
 *    DESCRIPTION
 *        This routine is called by vmiClipAndScale whenever the source
 *        y or source height are looked at.  Our response is to make sure
 *	  that both are even.
 *
 *    RETURNS
 *        the amout to add for y, the amount to subtract for the width,
 *	  0 otherwise.
 */
{
    switch (constraint) {
    case ConstrainSrcY:	return(value & 1);
    case ConstrainSrcH:	return(value & 1);
    default:		return(0);
    }
}

#define FracToFloat(frac) (((float)(frac)->numerator)			    \
                         / ((float)(frac)->denominator))

#define FixAbilityFraction(fraction, violationMask)			    \
{									    \
    float specified;							    \
    Bool checkAgain;							    \
    if (pAct->fraction.denominator == 0) {				    \
        failViolations |= violationMask;				    \
	checkAgain = FALSE;						    \
    } else if (pAct->fraction.denominator != 63) {			    \
	specified = FracToFloat(&pAct->fraction);			    \
	pAct->fraction.numerator = specified * 63. + .5;		    \
	pAct->fraction.denominator = 63;				    \
	checkAgain = TRUE;						    \
    } else								    \
	checkAgain = FALSE;						    \
    if (pAct->fraction.numerator < 0 || pAct->fraction.numerator > 63) {    \
	pAct->fraction.numerator = 32; /* normal value */		    \
	failViolations |= violationMask;				    \
    } else if (checkAgain && FracToFloat(&pAct->fraction) != specified) {   \
	subsetViolations |= violationMask;				    \
    }									    \
}

/*
 *    NAME
 *        roValidateRequest - validate request parameters
 *
 *    SYNOPSIS
 */
/*ARGSUSED*/
Bool
roValidateRequest(vp, pAct)
    VideoScreenPtr vp;			/* in/out: MVEX video screen */
    VideoActionPtr pAct;		/* in: RenderVideo parameters */
/*
 *    DESCRIPTION
 *        This routine is called before roRenderVideo (the StartVideo vector)
 *        and takes care of massaging the request coordinates to be something
 *        that can be handed directly to the hardware.  A request may be
 *        started and stopped many times by the server, but this function
 *        is called only once: when the request is first issued.
 *
 *    RETURNS
 *        True if there is something left to render/capture.
 */
{
    ScreenPtr pScr = vp->pScreen;
    RoScreenPrivatePtr priv = RoGetScreenPrivate(pScr);
    Bool workToDo;
    int subsetViolations = 0;
    int failViolations = 0;
    int numerator;

    /*
     * Translate the x and y source coordinates to hardware 0,0.
     */
    switch (pAct->drawable->type) {
    case DRAWABLE_WINDOW:
	pAct->ddx.dstX = pAct->dix.dstX + pAct->drawable->x;
	pAct->ddx.dstY = pAct->dix.dstY + pAct->drawable->y;
	break;
    case DRAWABLE_PIXMAP:
	pAct->ddx.dstX = pAct->dix.dstX;
	pAct->ddx.dstY = pAct->dix.dstY;
	break;
    }

    pAct->ddx.srcX = pAct->dix.srcX + RO_PICT_X;
    pAct->ddx.srcY = pAct->dix.srcY + RO_PICT_Y;

    /*
     * If the scaling appears to increase the size of the picture,
     * reduce it to 1:1 and send a violation
     */
    if (pAct->dix.srcHeight < pAct->dix.dstHeight) {
	pAct->ddx.dstHeight = pAct->dix.srcHeight;
	subsetViolations |= MVEXScaleViolation;
    } else
	pAct->ddx.dstHeight = pAct->dix.dstHeight;
    pAct->ddx.srcHeight = pAct->dix.srcHeight;

    if (pAct->dix.srcWidth < pAct->dix.dstWidth) {
	pAct->ddx.dstWidth = pAct->dix.srcWidth;
	subsetViolations |= MVEXScaleViolation;
    } else
	pAct->ddx.dstWidth = pAct->dix.dstWidth;
    pAct->ddx.srcWidth = pAct->dix.srcWidth;

    /*
     * trim the request back to a sane subset
     */
    if (vmiClipAndScale(pAct,
			RO_SIG_W, RO_SIG_H,
			priv->vidEnable.width, priv->vidEnable.height,
			ConstrainSrcY|ConstrainSrcH, roConstrainer,
			&workToDo))
	subsetViolations |= MVEXPlacementViolation;

    /*
     * Yuck!  Protocol says that we must match the fractions as close
     * as we can or settle for the normal setting.  This may mean some
     * floating point arithmetic.  See the FixAbilityFraction macro above.
     */
    FixAbilityFraction(saturation, MVEXSaturationViolation);
    FixAbilityFraction(contrast,   MVEXContrastViolation);
    FixAbilityFraction(hue,        MVEXHueViolation);
    FixAbilityFraction(bright,     MVEXBrightnessViolation);

    if (subsetViolations)
	SendVideoViolationEvent(pAct->drawable,
				pAct->videoIO,
				subsetViolations,
				(unsigned long) MVEXViolationSubset);

    if (failViolations)
	SendVideoViolationEvent(pAct->drawable,
				pAct->videoIO,
				failViolations,
				(unsigned long) MVEXViolationFail);

    return (workToDo);
}

#define CheckPort(priv, reqPort, errorReturn)				    \
{									    \
    if (priv->port != reqPort) {					    \
	if (! roSetPort(priv->fd, reqPort, errorReturn))		    \
	    return FALSE;						    \
	priv->port = reqPort;	    					    \
    }									    \
}

#define CheckAbilityFraction(fd, func, current, req, errorReturn)	    \
{									    \
    if (current != req.numerator) {					    \
	current = req.numerator;					    \
	if (! func(fd, current, errorReturn))	    			    \
	    return FALSE;						    \
    }									    \
}

#define CheckBox(priv, func, pact, xx, errorReturn)			    \
    if (priv->xx/**/Box.r_left != pact->ddx.xx/**/X			    \
     || priv->xx/**/Box.r_top != pact->ddx.xx/**/Y			    \
     || priv->xx/**/Box.r_width != pact->ddx.xx/**/Width		    \
     || priv->xx/**/Box.r_height != pact->ddx.xx/**/Height) {		    \
	    priv->xx/**/Box.r_left = pact->ddx.xx/**/X;			    \
	    priv->xx/**/Box.r_top = pact->ddx.xx/**/Y;			    \
	    priv->xx/**/Box.r_width = pact->ddx.xx/**/Width;		    \
	    priv->xx/**/Box.r_height = pact->ddx.xx/**/Height;		    \
									    \
	    if (! func(priv->fd, (pointer)&priv->xx/**/Box, errorReturn))   \
		return FALSE;						    \
    }

/*
 *    NAME
 *        roSetupVideo - Setup tc/pip video
 *
 *    SYNOPSIS
 */
/*ARGSUSED*/
Bool
roSetupVideo(vp, pAct, okToStart, errorReturn)
    VideoScreenPtr vp;		/* in: MVEX video screen */
    VideoActionPtr pAct;	/* in: RenderVideo parameters */
    Bool okToStart;		/* in: true if we should start the video */
    int *errorReturn;		/* out: Protocol error return for dix */
/*
 *    DESCRIPTION
 *        This routine take the (digested) arguments from a RenderVideo
 *        request and attempts to produce a video picture in the specified
 *        drawable.  If the hardware cannot do precisely what the request
 *        asks for, then it should do something close, and generate a
 *        VideoViolation event with state=Subset.  If it cannot do the
 *        request at all, then the state=Fail.  The reason for failure
 *        may be that the client is too stupid to do the right thing,
 *        or that it is occluded by a window and cannot produce a clipped
 *        picture.
 *
 *	  The ddx layer is responsible for sending the Sync event.
 *
 *    RETURNS
 *        - If the routine cannot start a RenderVideo because
 *          of an error it should return FALSE with errorReturn set
 *	    to BadAlloc.
 *	  - If the RenderVideo generates a VideoViolation with
 *	    MVEXViolationFail, it should return FALSE (but BadAlloc is
 *	    set to Success)
 *	  - If the RenderVideo is successful, it should return TRUE
 *	    depending on whether the hardware actually started
 *	    digitizing.
 */
{
    ScreenPtr pScr = vp->pScreen;
    RoScreenPrivatePtr priv = RoGetScreenPrivate(pScr);
    Bool somethingToRender;

    *errorReturn = Success;

    /*
     * We can assume that the drawable is a window (dix checks that)
     */

    /*
     * Wait for the device to stop before we start twiddling registers.
     */
    roWaitNotRunning(priv->fd);

    /*
     * Fix up the enable plane
     */
    somethingToRender = vmiMaintainMinEnable(pScr, pAct,
					     priv->vidEnabled,
					     priv->enableArea,
					     priv->disableArea,
					     priv->vidEnablePix);
    if (! somethingToRender) {
	if (! pAct->hasSync)
	    SendVideoSyncEvent(pAct->drawable, pAct->videoIO, MVEXSyncAcquired);
	pAct->hasSync = TRUE;
	return (TRUE);
    }

    /*
     * Check the input port.
     */
    CheckPort(priv, RoPortDevices[ pAct->port ], errorReturn);

    /*
     * vmiClipAndScale() scales, clips and translates for us.
     * Set the source and destination rectangles.
     */
    CheckBox(priv, roSetInputRectangle, pAct, src, errorReturn);
    CheckBox(priv, roSetOutputRectangle, pAct, dst, errorReturn);

    /*
     * set the hue, saturation, contrast and brightness.
     * Since the priv-> copies will be initialized to zero, and since the dix
     * code initializes the pAct-> values to something (either the values
     * specified in the request or the normal values provided at init
     * time, this will guarantee the first access to the board will
     * initialize them to known values.
     */
    CheckAbilityFraction(priv->fd, roSetSaturationDac,
			 priv->saturation, pAct->saturation, errorReturn);
    CheckAbilityFraction(priv->fd, roSetContrastDac,
			 priv->contrast, pAct->contrast, errorReturn);
    CheckAbilityFraction(priv->fd, roSetHueDac,
			 priv->hue, pAct->hue, errorReturn);
    CheckAbilityFraction(priv->fd, roSetBrightnessDac,
			 priv->bright, pAct->bright, errorReturn);

    /*
     * Turn on the video, but we only do it here if okToStart.
     * The reason for the test is this:  ClipNotify is called for everyone
     * that wants it, and **then** the server will cause the windows to
     * move around.  If we were to start the video now, we may scribble on
     * some newly vacated real estate that the server plans to copy
     * to another part of the screen.  As a benefit, this also makes the
     * screen update a little faster since the server doesn't have to
     * share the framebuffer bandwidth with the video digitizer.
     */
    if (okToStart) {
	if (! roStartPip(priv->fd, errorReturn))
	    return FALSE;

	if (! pAct->fullMotion) {
	    usleep(100);    /* XXX ~ .1 millisecond */
	    if (roConfirmActive(priv->fd, errorReturn)) {
		pAct->hasSync = TRUE;
		SendVideoSyncEvent(pAct->drawable, pAct->videoIO,
				   MVEXSyncAcquired);
	    } else
	        return FALSE;
	    /*
	     * since it is a single frame, stop it... XXX should be done with
	     * the one-shot bit in the hardware
	     */
	    (void) roStopPip(priv->fd, errorReturn);
	} else
	    pAct->vRes->vElement->pvio->timeout = 17;	/* ~1/60 second */
    }

    return (okToStart);
}
#undef CheckAbilityFraction
#undef CheckBox
#undef CheckPort

/*
 *    NAME
 *        roStartVideo - Start a live video that was already setup
 *
 *    SYNOPSIS
 */
/*ARGSUSED*/
Bool
roStartVideo(vp, pAct)
    VideoScreenPtr vp;		/* in: MVEX video screen */
    VideoActionPtr pAct;	/* in: RenderVideo parameters */
/*
 *    DESCRIPTION
 *        If the SetupVideo vector returned false (latter put into
 *	  pAct->running), then mvex will arrange to call this routine just
 *        before the server blocks waiting for input.  At this point, it
 *	  is too late to return an Alloc error, so if we fail, we must
 *	  issue an Override event with state HardwareError.
 *
 *    RETURNS
 *        void
 *
 */
{
    ScreenPtr pScr = vp->pScreen;
    RoScreenPrivatePtr priv = RoGetScreenPrivate(pScr);
    int errorReturn;

    if (! roStartPip(priv->fd, &errorReturn)) {
	StopVideo(vp, pAct->vDraw, (VideoIO) 0,
		  MVEXStopRenderVideo, MVEXHardwareError);
	return FALSE;
    }

    /*
     * if we already have sync, then we were just stopped to allow graphics
     * processing.
     */
    if (pAct->hasSync)
	pAct->vRes->vElement->pvio->timeout = 2000;	/* two seconds */
    else
	pAct->vRes->vElement->pvio->timeout = 17;	/* ~1/60 second */
    return TRUE;
}

/*
 *    NAME
 *        roPollVideo - check a live video for sync status
 *
 *    SYNOPSIS
 */
/*ARGSUSED*/
void
roPollVideo(vp, pAct, result, pReadmask)
    VideoScreenPtr vp;		/* in: MVEX video screen */
    VideoActionPtr pAct;	/* in: RenderVideo parameters */
    int result;			/* in: return value from select */
    pointer pReadmask;          /* the resulting descriptor mask */
/*
 *    DESCRIPTION
 *        Ask the device driver if it is active.  If the state has changed
 *	  issue a sync event.
 *
 *    RETURNS
 *        void
 *
 */
{
    ScreenPtr pScr = vp->pScreen;
    RoScreenPrivatePtr priv = RoGetScreenPrivate(pScr);
    int errorReturn;
    unsigned long now;
    VioDataPtr vio;

    if (result > 0) {
	/*
	 * There are file descriptors to read, so we won't get in the way by
	 * polling for sync status.  However, we do
	 * try to acknowledge the priority.  The theory goes
	 * that if there is input to be processed (result > 0), then
	 * we can speed the throughput by stopping the video.  We mark
	 * things up such that we will be started before the server
	 * goes back to sleep.  Protocol says that for a priority step
	 * of 2, priorities of 50 and above are "high".
	 */
	if (pAct->priority >= 50)
	    return;
	(void) roStopPip(priv->fd, &errorReturn);
	pAct->running = FALSE;
	return;
    } else {
	/*
	 * Don't poll too soon
	 */
	VioDataPtr vio = pAct->vRes->vElement->pvio;
	if ((now = GetTimeInMillis()) < vio->lastCall + vio->timeout)
	    return;
	vio->lastCall = now;

	/*
	 * When polling, we are slower to recognize nosync --> sync than
	 * sync --> nosync because if it looks like we lost sync, then we
	 * sleep waiting for it to come back.  This is because we may just be
	 * in vertical retrace.  If we had an interrupt
	 * for state change this would be alot easier.
	 */
	if (pAct->hasSync) {
	    if (roConfirmActive(priv->fd, &errorReturn))
		return;

	    SendVideoSyncEvent(pAct->drawable, pAct->videoIO, MVEXSyncLost);
	    pAct->hasSync = FALSE;
	    /*
	     * if we had it but lost it, we start out checking fairly often
	     * and back off as below
	     */
	    pAct->vRes->vElement->pvio->timeout = 34;	/* ~ 1/30 second */
	} else {
	    /*
	     * If we don't get sync right away, keep backing the timeout off
	     * to about .75 second.
	     */
	    if (! roIsActive(priv->fd)) {
		if (++pAct->vRes->vElement->pvio->timeout > 750)
		    pAct->vRes->vElement->pvio->timeout = 750;
		return;
	    }
	    SendVideoSyncEvent(pAct->drawable, pAct->videoIO, MVEXSyncAcquired);
	    pAct->hasSync = TRUE;
	    pAct->vRes->vElement->pvio->timeout = 2000;	/* two seconds */
	}
    }
}

/*
 *    NAME
 *        roStopVideo - Toucan StopVideo
 *
 *    SYNOPSIS
 */
/*ARGSUSED*/
void
roStopVideo(vp, pDrawable, vDraw)
    VideoScreenPtr vp;		/* in: MVEX video screen */
    DrawablePtr pDrawable;
    VideoDrawPtr vDraw;
/*
 *    DESCRIPTION
 *        This routine should implement the protocol semantics for StopVideo.
 *        Namely, if the drawable is of a core visual type, then leave the
 *        last frame in the drawable.  If the visual class is VideoGray or
 *        VideoColor, the result is undefined, but either the picture ought
 *        to stay on the screen or it should be completely removed and an area
 *        filled with a solid color take its place.  The former is preferred.
 *
 *    RETURNS
 *        void
 *
 */
{
    RoScreenPrivatePtr priv = RoGetScreenPrivate(vp->pScreen);
    int errorReturn;

    (void) roStopPip(priv->fd, &errorReturn);
}
