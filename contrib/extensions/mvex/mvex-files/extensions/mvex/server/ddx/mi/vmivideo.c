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
static char *rcsid=  "$Header: /n/lacey/usr/local/src/video/MVEX/server/ddx/mi/RCS/vmivideo.c,v 1.8 1991/09/26 21:22:03 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#include "X.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "misc.h"
#include "MVEX.h"
#include "MVEXproto.h"
#include "videostr.h"
#include "vmi.h"

/*
 *    NAME
 *        vmiUnrealizeWindow - stop when a window goes down
 *
 *    SYNOPSIS
 */
void
vmiUnrealizeWindow(vDraw, vRes)
    VideoDrawPtr vDraw;
    VideoResourcePtr vRes;
/*
 *    DESCRIPTION
 *        If device is active, stop it.
 *
 *    RETURNS
 *        void
 */
{
    VideoScreenPtr	vp;
    int			stopAction;

    if (vRes->vElement->type == Input && vDraw->renderActive)
	stopAction = MVEXStopRenderVideo;
    else if (vRes->vElement->type == Output && vDraw->captureActive)
	stopAction = MVEXStopRenderVideo;
    else
	return;
     /*
      * Tell dix to StopVideo
      */
    vp = ScreenVideoInfo[ vDraw->pDraw->pScreen->myNum ];
    StopVideo(vp, vDraw, (VideoIO) 0, stopAction, MVEXDrawableChange);
}

/*
 *    NAME
 *        vmiClipNotify - stop and restart an active render or capture
 *
 *    SYNOPSIS
 */
/*ARGSUSED*/
void
vmiClipNotify(vDraw, vRes, dx, dy)
    VideoDrawPtr vDraw;
    VideoResourcePtr vRes;
    int    dx, dy;
/*
 *    DESCRIPTION
 *        If device is active, stop it and try to restart it.
 *
 *    RETURNS
 *        void
 */
{
    VideoScreenPtr	vp = ScreenVideoInfo[ vDraw->pDraw->pScreen->myNum ];
    int			errorReturn;
    short		*active;
    VideoActionPtr	pVideoAction;
    VioDataPtr		pvio = vRes->vElement->pvio;

    /*
     * most of this code was stolen directly from cfbValidateGC.
     */
    if (vRes->vElement->type == Input && vDraw->renderActive) {
	ScreenPtr   pScreen = vp->pScreen;
	WindowPtr   pWin = (WindowPtr) vDraw->pDraw;
	RegionPtr   pregWin;
	Bool        freeTmpClip, freeCompClip;

	pVideoAction = &vDraw->renderAction;
	active = &vDraw->renderActive;

	if (pVideoAction->subWindowMode == IncludeInferiors) {
	    pregWin = NotClippedByChildren(pWin);
	    freeTmpClip = TRUE;
	}
	else {
	    pregWin = &pWin->clipList;
	    freeTmpClip = FALSE;
	}
	freeCompClip = pVideoAction->freeCompClip;

	/*
	 * if there is no client clip, we can get by with just keeping
	 * the pointer we got, and remembering whether or not should
	 * destroy (or maybe re-use) it later.  this way, we avoid
	 * unnecessary copying of regions.  (this wins especially if
	 * many clients clip by children and have no client clip.) 
	 */
	if (pVideoAction->pGcClip == (RegionPtr) NULL) {
	    if (freeCompClip)
		(*pScreen->RegionDestroy) (pVideoAction->pClip);
	    pVideoAction->pClip = pregWin;
	    pVideoAction->freeCompClip = freeTmpClip;
	}
	else {
	    /*
	     * we need one 'real' region to put into the composite
	     * clip. if pregWin the current composite clip are real,
	     * we can get rid of one. if pregWin is real and the
	     * current composite clip isn't, use pregWin for the
	     * composite clip. if the current composite clip is real
	     * and pregWin isn't, use the current composite clip. if
	     * neither is real, create a new region. 
	     */

	    if (dx || dy)
		(*pScreen->TranslateRegion)(pVideoAction->pGcClip, dx, dy);
						  

	    if (freeCompClip)
	    {
		(*pScreen->Intersect)(pVideoAction->pClip,
					   pregWin, pVideoAction->pGcClip);
		if (freeTmpClip)
		    (*pScreen->RegionDestroy)(pregWin);
	    }
	    else if (freeTmpClip)
	    {
		(*pScreen->Intersect)(pregWin, pregWin, pVideoAction->pGcClip);
		pVideoAction->pClip = pregWin;
	    }
	    else
	    {
		pVideoAction->pClip = (*pScreen->RegionCreate)(NullBox, 0);
		(*pScreen->Intersect)(pVideoAction->pClip,
				      pregWin, pVideoAction->pGcClip);
	    }
	    pVideoAction->freeCompClip = TRUE;
	}
    } else if (vRes->vElement->type == Output && vDraw->captureActive) {
	pVideoAction = &vDraw->captureAction;
	active = &vDraw->captureActive;
    } else
	return;

    (*pvio->StopVideo)(vp, vDraw->pDraw, vDraw);

    /*
     * if the x and y coordinate changed, then we have to re-validate
     * the request.
     */
    if (dx || dy) {
	if (! (*pvio->ValidateRequest)(vRes->vp, pVideoAction)) {
	    pVideoAction->running = TRUE;
	    *active = NoVideo;
	    return;
	}
    }

    /*
     * if we told it not to start, but it did anyway, then it
     * must have been fully clipped.
     */
    if ((*pvio->SetupVideo)(vp, pVideoAction, FALSE, &errorReturn))
	*active = NoVideo;
    else
	*active = (pVideoAction->fullMotion) ? MotionVideo : StillVideo;

    /*
     * The block handler will start it up again.
     */
    pVideoAction->running = FALSE;
}

/*
 *    NAME
 *        vmiMaintainMinEnable - maintain a minimum enable region
 *
 *    SYNOPSIS
 */
/*ARGSUSED*/
Bool
vmiMaintainMinEnable(pScr, pAct, pScrEnable, enableFunc, disableFunc, fb)
    ScreenPtr pScr;		/* in: screen pointer */
    VideoActionPtr pAct;	/* in: RenderVideo parameters */
    RegionPtr pScrEnable;	/* in: the region currently enabled, and... */
				/* out: the updated version */
    void (*enableFunc)();	/* in: function for enabling a region */
    void (*disableFunc)();	/* in: function for disabling a region */
    PixmapPtr fb;		/* in: enable frame buffer */
/*
 *    DESCRIPTION
 *        Some devices *don't* work well by disabling the minimum rectangle.
 *	  and force us to disable everything but what should be rendered.
 *
 *	  XXX this assumes only one video active at a time.
 *
 *    RETURNS
 *        true if there is anything left to render, false otherwise.
 */
{
    RegionRec newEnable;
    RegionRec unchanged;
    BoxRec vbox;
    register BoxPtr pCurExtent, pNewExtent;

    /*
     * By default the entire screen is disabled for video, and we enable
     * only the parts of the destination rectangle that are
     * allowed by window clip or gc clip.
     */
    vbox.x1 = pAct->ddx.dstX;
    vbox.y1 = pAct->ddx.dstY;
    vbox.x2 = pAct->ddx.dstX + pAct->ddx.dstWidth;
    vbox.y2 = pAct->ddx.dstY + pAct->ddx.dstHeight;

    /*
     * Intersect the composite clip with the video request rectangle.
     * This will define newEnable to be precisely that.
     */
    (*pScr->RegionInit)(&newEnable, &vbox, 1);
    (*pScr->Intersect)(&newEnable, &newEnable, pAct->pClip);
    if (REGION_NIL(&newEnable)) {
	(*pScr->RegionUninit)(&newEnable);
	return FALSE;	/* nothing left to render */
    }

    if (REGION_NUM_RECTS(&newEnable) == 1
     && REGION_NUM_RECTS(pScrEnable) == 1) {
	pNewExtent = REGION_RECTS(&newEnable);
	pCurExtent = REGION_RECTS(pScrEnable);
	if (pNewExtent->x1 == pCurExtent->x1
	 && pNewExtent->y1 == pCurExtent->y1
	 && pNewExtent->x2 == pCurExtent->x2
	 && pNewExtent->y2 == pCurExtent->y2) {
	    (*pScr->RegionUninit)(&newEnable);
	    return (TRUE);	/* nothing has changed */
	}
    } 

    /*
     * Intersect the new enable with the current enable.
     * This will define the region that remains enabled (i.e., unchanged).
     */
    (*pScr->RegionInit)(&unchanged, (BoxPtr) NULL, 0);
    (*pScr->Intersect)(&unchanged, pScrEnable, &newEnable);
    if (REGION_NIL(&unchanged)) {
	(*disableFunc)(fb,
		       REGION_NUM_RECTS(pScrEnable),
		       REGION_RECTS(pScrEnable), 0, 0);
	(*enableFunc)(fb,
		       REGION_NUM_RECTS(&newEnable),
		       REGION_RECTS(&newEnable), 0, 0);
	(*pScr->RegionUninit)(&unchanged);
	(*pScr->RegionUninit)(pScrEnable);
	*pScrEnable = newEnable;
	return TRUE;
    }

    /*
     * the current screen enable minus the unchanged is what must
     * be disabled right now.
     */
    (*pScr->Subtract)(pScrEnable, pScrEnable, &unchanged);
    if (! REGION_NIL(pScrEnable))
	(*disableFunc)(fb,
		       REGION_NUM_RECTS(pScrEnable),
		       REGION_RECTS(pScrEnable), 0, 0);
    (*pScr->RegionUninit)(pScrEnable);

    /*
     * the new screen enable minus the unchanged is what must
     * be enabled right now.
     */
    (*pScr->Subtract)(&unchanged, &newEnable, &unchanged);
    if (! REGION_NIL(&unchanged))
	(*enableFunc)(fb,
		       REGION_NUM_RECTS(&unchanged),
		       REGION_RECTS(&unchanged), 0, 0);
    (*pScr->RegionUninit)(&unchanged);
    
    *pScrEnable = newEnable;
    return TRUE;
}

/*
 *    NAME
 *        vmiMaintainMinDisable - maintain a minimum disable region
 *
 *    SYNOPSIS
 */
/*ARGSUSED*/
Bool
vmiMaintainMinDisable(pScr, pAct, pScrDisable, enableFunc, disableFunc, fb)
    ScreenPtr pScr;		/* in: screen pointer */
    VideoActionPtr pAct;	/* in: RenderVideo parameters */
    RegionPtr pScrDisable;	/* in: the region currently disabled, and... */
				/* out: the updated version */
    void (*enableFunc)();	/* in: function for enabling a region */
    void (*disableFunc)();	/* in: function for disabling a region */
    PixmapPtr fb;		/* in: enable frame buffer */
/*
 *    DESCRIPTION
 *        Some devices work well with maintaining the enable plane such
 *	  that only the intersection between the video rectangle
 *	  and the window clip list.
 *
 *	  XXX this assumes only one video active at a time.
 *
 *    RETURNS
 *        true if there is anything left to render, false otherwise.
 */
{
    RegionRec newEnable;
    RegionRec newDisable;
    RegionRec disableNow;
    RegionPtr pDisableNow;
    BoxRec vbox;
    register BoxPtr pextent;

    /*
     * By default the entire screen is enabled for video, and we only
     * trim back the parts of the destination rectangle that are
     * excluded by window clip or gc clip.  This way, we have no
     * extra work to do if nothing special needs to be done.
     */
    vbox.x1 = pAct->ddx.dstX;
    vbox.y1 = pAct->ddx.dstY;
    vbox.x2 = pAct->ddx.dstX + pAct->ddx.dstWidth;
    vbox.y2 = pAct->ddx.dstY + pAct->ddx.dstHeight;

    if (REGION_NUM_RECTS(pAct->pClip) == 1) {
	pextent = REGION_RECTS(pAct->pClip);
	if (vbox.x1 >= pextent->x1
	 && vbox.y1 >= pextent->y1
	 && vbox.x2 <= pextent->x2
	 && vbox.y2 <= pextent->y2) {
	    if (! REGION_NIL(pScrDisable)) {
		(*enableFunc)(fb,
			      REGION_NUM_RECTS(pScrDisable),
			      REGION_RECTS(pScrDisable), 0, 0);
		(*pScr->RegionEmpty)(pScrDisable);
	    }
	    return (TRUE);	/* the request rect is contained in clip box */
	}
    } 

    /*
     * Intersect the composite clip with the video request rectangle.
     * This will define newEnable to be precisely that.
     */
    (*pScr->RegionInit)(&newEnable, &vbox, 1);
    (*pScr->Intersect)(&newEnable, &newEnable, pAct->pClip);
    if (REGION_NIL(&newEnable)) {
	(*pScr->RegionUninit)(&newEnable);
	return FALSE;	/* nothing left to render */
    }

    /*
     * the inverse of newEnable with respect to the video
     * request rectangle is the newDisable.  This latter is the total
     * of what should be disabled and will be recorded.
     */
    (*pScr->RegionInit)(&newDisable, (RegionPtr)NULL, 0);
    (*pScr->Inverse)(&newDisable, &newEnable, &vbox);
    (*pScr->RegionUninit)(&newEnable);

    /*
     * If we have disabled some other region before, we don't need
     * to disable it again.  Further, we must re-enable parts
     * that should no longer be disabled.
     */
    if (! REGION_NIL(pScrDisable)) {
	(*pScr->RegionInit)(&disableNow, (RegionPtr)NULL, 0);
	(*pScr->Subtract)(&disableNow, &newDisable, pScrDisable);
	pDisableNow = &disableNow;

	/*
	 * if we subtract the newDisable from what was already disabled
	 * this will yield what needs to be reenabled.
	 */
	if (! REGION_NIL(&newDisable))
	    (*pScr->Subtract)(pScrDisable,
			      pScrDisable,
			      &newDisable);
	if (! REGION_NIL(pScrDisable))
	    (*enableFunc)(fb,
			  REGION_NUM_RECTS(pScrDisable),
			  REGION_RECTS(pScrDisable), 0, 0);
    } else
	pDisableNow = &newDisable;

    /*
     * disable all the areas we don't want to render to.
     */
    if (! REGION_NIL(pDisableNow))
	(*disableFunc)(fb,
		       REGION_NUM_RECTS(pDisableNow),
		       REGION_RECTS(pDisableNow), 0, 0);
    if (pDisableNow == &disableNow)
	(*pScr->RegionUninit)(pDisableNow);
    (*pScr->RegionEmpty)(pScrDisable);
    if (! REGION_NIL(&newDisable))
	*pScrDisable = newDisable;
    return TRUE;
}

/*
 *    NAME
 *        vmiSuspendVideo - suspend an active render
 *
 *    SYNOPSIS
 */
Bool
vmiSuspendVideo(pDraw)
    DrawablePtr pDraw;
/*
 *    DESCRIPTION
 *        If device is active rendering, stop it without "telling dix"
 *
 *    RETURNS
 *        TRUE if the device is stopped, else FALSE.
 */
{
    VideoDrawPtr	vDraw;
    VideoResourcePtr	vRes;

    if ( (vDraw = VDRAW_OF_PDRAW(pDraw))
     && vDraw->renderActive == MotionVideo) {
	vRes = VRES_OF_VID(vDraw->videoIn);
	(*vRes->vElement->pvio->StopVideo)(VSCREEN_OF_PDRAW(pDraw),
					   pDraw,
					   vDraw);
	return TRUE;
    }

    return FALSE;
}

/*
 *    NAME
 *        vmiResumeVideo - resume a suspended render
 *
 *    SYNOPSIS
 */
void
vmiResumeVideo(pDraw)
    DrawablePtr pDraw;
/*
 *    DESCRIPTION
 *        If device is not rendering, start it up.
 *
 *        Note this does not check to see if video is really suspended -- it
 *        just looks to see that it is not active and that a RenderVideo was
 *        really requested at some time in the past.
 *
 *    RETURNS
 *        void
 */
{
    int			errorReturn;
    VideoDrawPtr	vDraw;
    Bool		(* StartVideo)();

    if ( (vDraw = VDRAW_OF_PDRAW(pDraw))
     && vDraw->renderActive == MotionVideo) {
	StartVideo = VRES_OF_VID(vDraw->videoIn)->vElement->pvio->StartVideo;
	vDraw->renderAction.running =
	    (*StartVideo)(VSCREEN_OF_PDRAW(pDraw),
			  &vDraw->renderAction,
			  FALSE,
			  &errorReturn);
    }
}

/*
 *    NAME
 *        vmiClipAndScale - clip and scale a src and dest rectangle
 *	  in a VideoActionPtr
 *
 *    SYNOPSIS
 */
Bool
vmiClipAndScale(pAct, maxSrcW, maxSrcH, maxDstW, maxDstH,
		constraints, constrainer, workToDo)
    VideoActionPtr pAct;	/* in/out: request */
    int maxSrcW, maxSrcH;	/* in: Maximum source width and height */
    int maxDstW, maxDstH;	/* in: Maximum destination width and height */
    int constraints;		/* in: further constraints on src and dest */
    Bool (*constrainer)();	/* in: function to perform constraints */
    Bool *workToDo;		/* out: true if there's something left */
/*
 *    DESCRIPTION
 *	  This clips the source and destination rectangles in the
 *	  VideoActionPtr->ddx so that the {src,dest} {x,y} are all >= 0
 *	  and the {src,dest} {width,height} are all smaller than
 *	  the specified widths and heights.
 *
 *	  If the source needs to change, then adjust the destination 
 *	  to be appropriately smaller so that the relative scaling
 *	  doesn't change.  Same is true if the destination must change.
 *	  Note that scaling accuracy is sometimes poor because the result
 *	  cannot be a fractional pixel (but may need to be).
 *
 *	  The constraints bits and the constrainer allow the ddx layer to
 *	  place other, more complex constraints on the values.  If one of
 *	  the constraint bits is set, the constrainer function will be
 *	  called with the corresponding value (see server/include/video.h
 *	  for a definition of the bits.
 *
 *		int (*constrainer)(constraint, value)
 *		    int constraint;
 *		    int value;
 *	  The constrainer should always return a positive value... for
 *	  SrcX, SrcY, DstX, DstY, it returns the amount to increase the
 *	  value to make it valid; for SrcW, SrcH, DstW, DstH it returns the
 *	  amount to decrease the value to make it valid.
 *
 *    RETURNS
 *	  True if the request {W,H} exceeded max{Src,Dst}{W,H}, or the
 *	  {X,Y} was less than 0 (it was clipped).
 *	  Also, the src (or dest) x and y are translated to the window
 *	  coordinates based on whether the request is MVEXRender or MVEXCapture.
 *        Finally, workToDo is set to true if there is something left to
 *	  render/capture; FALSE otherwise.
 */
{
    int dstx, dsty, srcx, srcy, srcw, srch, dstw, dsth;
    int change;
    DrawablePtr pDrawable = pAct->drawable;
    Bool fixSrcX, fixDstX, fixSrcY, fixDstY, doScale, clipped, clippedThisPass;

    /*
     * the source and destination width and height are all guaranteed by
     * dix to be > 0 so we don't have to check here.
     */
    srcw = pAct->ddx.srcWidth;
    srch = pAct->ddx.srcHeight;
    dstw = pAct->ddx.dstWidth;
    dsth = pAct->ddx.dstHeight;
    srcx = pAct->ddx.srcX;
    srcy = pAct->ddx.srcY;
    dstx = pAct->ddx.dstX;
    dsty = pAct->ddx.dstY;

    fixSrcX = (srcx < 0 || srcw+srcx > maxSrcW);
    fixDstX = (dstx < 0 || dstw+dstx > maxDstW);
    fixSrcY = (srcy < 0 || srch+srcy > maxSrcH);
    fixDstY = (dsty < 0 || dsth+dsty > maxDstH);
    *workToDo = TRUE;
    if (!fixSrcX && !fixDstX && !fixSrcY && !fixDstY && !constraints)
	return FALSE; /* nothing to fix, but there is something to do */

    /*
     * The scaling is based on:
     *   src   nsrc
     *   --- = ----
     *   dst   ndst
     *
     * On entry we know the src and dst (be they width or height), and
     * we will soon know the nsrc (new source) or ndst (new destination).
     * From these we can compute the missing piece with on of
     *          src                 dst
     *   nsrc = ---*ndst     ndst = ---*nsrc
     *          dst                 src
     *
     */
#define vmiScale(new,numerator,denominator)				\
	   (doScale ? ((new * numerator) / denominator) : change)

    clipped = FALSE;
    *workToDo = FALSE;
    for (;;) {
	clippedThisPass = FALSE;
	if (fixSrcX || fixDstX || (constraints & ConstrainHorizontal)) {
	    doScale = (dstw != srcw);

	    if (fixSrcX || (constraints & ConstrainSrcHorizontal)) {
		/* we need ndst = (dst*nsrc)/src */

		if (srcx < 0)
		    change = -srcx;
		else if (constraints & ConstrainSrcX)
		    change = (*constrainer)(ConstrainSrcX, srcx);
		else
		    change = 0;
		if (change) {
		    clippedThisPass = TRUE;
		    srcx += change;
		    if ((srcw -= change) <= 0)
			return (clipped || clippedThisPass);
		    change = vmiScale(change,
				      pAct->dix.dstWidth, pAct->dix.srcWidth);
		    dstx += change;
		    dstw -= change;
		}

		if ((change = srcx + srcw - maxSrcW) < 0)
		    change = 0;
		if (constraints & ConstrainSrcW)
		    change += (*constrainer)(ConstrainSrcW, srcw - change);
		if (change) {
		    clippedThisPass = TRUE;
		    if ((srcw -= change) <= 0)
			return (clipped || clippedThisPass);
		    change = vmiScale(change,
				      pAct->dix.dstWidth, pAct->dix.srcWidth);
		    dstw -= change;
		}
	    }

	    if (fixDstX || (constraints & ConstrainDstHorizontal)) {
		/* we need nsrc = (src*ndst)/dst */
		if (dstx < 0)
		    change = -dstx;
		else if (constraints & ConstrainSrcX)
		    change = (*constrainer)(ConstrainDstX, dstx);
		else
		    change = 0;
		if (change) {
		    clippedThisPass = TRUE;
		    dstx += change;
		    if ((dstw -= change) <= 0)
			return (clipped || clippedThisPass);
		    change = vmiScale(change,
				      pAct->dix.srcWidth, pAct->dix.dstWidth);
		    srcx += change;
		    srcw -= change;
		}

		if ((change = dstx + dstw - maxDstW) < 0)
		    change = 0;
		if (constraints & ConstrainDstW)
		    change += (*constrainer)(ConstrainDstW, dstw - change);
		if (change) {
		    clippedThisPass = TRUE;
		    if ((dstw -= change) <= 0)
			return (clipped || clippedThisPass);
		    change = vmiScale(change,
				      pAct->dix.srcWidth, pAct->dix.dstWidth);
		    srcw -= change;
		}
	    }
	}

	if (fixSrcY || fixDstY || (constraints & ConstrainVertical)) {
	    doScale = (dsth != srch);

	    if (fixSrcY || (constraints & ConstrainSrcVertical)) {
		/* we need ndst = (dst*nsrc)/src */

		if (srcy < 0)
		    change = -srcy;
		else if (constraints & ConstrainSrcY)
		    change = (*constrainer)(ConstrainSrcY, srcy);
		else
		    change = 0;
		if (change) {
		    clippedThisPass = TRUE;
		    srcy += change;
		    if ((srch -= change) <= 0)
			return (clipped || clippedThisPass);
		    change = vmiScale(change,
				      pAct->dix.dstHeight, pAct->dix.srcHeight);
		    dsty += change;
		    dsth -= change;
		}

		if ((change = srcy + srch - maxSrcH) < 0)
		    change = 0;
		if (constraints & ConstrainSrcH)
		    change += (*constrainer)(ConstrainSrcH, srch - change);
		if (change) {
		    clippedThisPass = TRUE;
		    if ((srch -= change) <= 0)
			return (clipped || clippedThisPass);
		    change = vmiScale(change,
				      pAct->dix.dstHeight, pAct->dix.srcHeight);
		    dsth -= change;
		}
	    }

	    if (fixDstY || (constraints & ConstrainDstVertical)) {
		/* we need nsrc = (src*ndst)/dst */
		if (dsty < 0)
		    change = -dsty;
		else if (constraints & ConstrainDstY)
		    change = (*constrainer)(ConstrainDstY, dsty);
		else
		    change = 0;
		if (change) {
		    clippedThisPass = TRUE;
		    dsty += change;
		    if ((dsth -= change) <= 0)
			return (clipped || clippedThisPass);
		    change = vmiScale(change,
				      pAct->dix.srcHeight, pAct->dix.dstHeight);
		    srcy += change;
		    srch -= change;
		}

		if ((change = dsty + dsth - maxDstH) < 0)
		    change = 0;
		if (constraints & ConstrainDstH)
		    change += (*constrainer)(ConstrainDstH, dsth - change);
		if (change) {
		    clippedThisPass = TRUE;
		    if ((dsth -= change) <= 0)
			return (clipped || clippedThisPass);
		    change = vmiScale(change,
				      pAct->dix.srcHeight, pAct->dix.dstHeight);
		    srch -= change;
		}
	    }
	}

	if (clippedThisPass) {
	    clipped = TRUE;
	    if (! constraints)
		break;
	    clippedThisPass = FALSE;
	} else
	    break;
    }

    *workToDo = TRUE;
    pAct->ddx.srcX = srcx;
    pAct->ddx.srcY = srcy;
    pAct->ddx.dstX = dstx;
    pAct->ddx.dstY = dsty;
    pAct->ddx.srcWidth = srcw;
    pAct->ddx.srcHeight = srch;
    pAct->ddx.dstWidth = dstw;
    pAct->ddx.dstHeight = dsth;

    return (clipped);
}

#if CompiledWithCfb

#include "cfb.h"

/*
 *    NAME
 *        vmiGetCfbCompositeClip - get cfb's idea of the gc's composite clip.
 *
 *    SYNOPSIS
 */
void
vmiGetCfbCompositeClip(pGC, pAct)
    GC *pGC;
    VideoActionPtr pAct;
/*
 *    DESCRIPTION
 *	  Get the composite clip for the specified gc and drawable.
 *	  If you compile with cfb, then this will get cfb's composite
 *	  clip and the gc clip, if any.  If you don't, you have to
 *	  write something like cfb's ValidateGC that makes up the
 *	  composite clip.
 *
 *    RETURNS
 *        RegionPtr
 */
{
    cfbPrivGCPtr ptr = (cfbPrivGCPtr) pGC->devPrivates[cfbGCPrivateIndex].ptr;
    extern cfbGCPrivateIndex;

    pAct->pClip = ptr->pCompositeClip;
    pAct->freeCompClip = FALSE;

    /*
     * Save the GC clip for ClipNotifies.
     */
    if (pAct->request == MVEXCapture || pGC->clientClipType == CT_NONE) {
	pAct->pGcClip = (RegionPtr) NULL;
	return;
    }
    pAct->pGcClip = (*pGC->pScreen->RegionCreate)((BoxPtr) NULL, 0);
    if (! (*pGC->pScreen->RegionCopy) (pAct->pGcClip, pGC->clientClip)) {
	(*pGC->pScreen->RegionDestroy) (pAct->pGcClip);
	pAct->pGcClip = (RegionPtr) NULL;
	return;
    }

    /*
     * translate the clip because it will never be used for anything
     * else (unlike the gc clip)
     */
    (*pGC->pScreen->TranslateRegion)(pAct->pGcClip,
				     pAct->drawable->x + pGC->clipOrg.x,
				     pAct->drawable->y + pGC->clipOrg.y);
}

#endif /* CompiledWithCfb */

int
vmiPrintRegion(rgn)
    RegionPtr rgn;
{
    int num, size;
    register int i;
    BoxPtr rects;

    num = REGION_NUM_RECTS(rgn);
    size = REGION_SIZE(rgn);
    rects = REGION_RECTS(rgn);
    ErrorF("num: %d size: %d\n", num, size);
    ErrorF("extents: %d %d %d %d\n",
	   rgn->extents.x1, rgn->extents.y1, rgn->extents.x2, rgn->extents.y2);
    for (i = 0; i < num; i++)
      ErrorF("%d %d %d %d \n",
	     rects[i].x1, rects[i].y1, rects[i].x2, rects[i].y2);
    ErrorF("\n");
    return(num);
}
