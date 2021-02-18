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
static char *rcsid=  "$Header: /usr/local/src/x11r5/mit/extensions/RCS/render.c,v 2.2 1991/09/19 22:32:52 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#define NEED_REPLIES
#define NEED_EVENTS
#include <stdio.h>
#include "X.h"
#include "Xproto.h"
#include "misc.h"
#include "os.h"
#include "windowstr.h"
#include "colormapst.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "extnsionst.h"
#include "gcstruct.h"
#include "dixstruct.h"
#include "resource.h"
#include "opaque.h"
#include "regionstr.h"

#include "MVEX.h"
#include "MVEXproto.h"
#include "videostr.h"
#include "mvexdix.h"

/*
 *    NAME
 *        GetUseWithoutOwner - find a current non-owned render
 *
 *    SYNOPSIS
 */
static DrawablePtr
GetUseWithoutOwner(vRes)
    VideoResourcePtr vRes;	/* in: the video resource structure */
/*
 *    DESCRIPTION
 *        Scan the resource for uses, see if they are owned.
 *
 *    RETURNS
 *        The Drawable pointer for an non-owned use if there is one, else NULL.
 */
{
    int i;

    for (i=0; i < vRes->maxUse; i++)
	if (!vRes->usage[i].owned) 
	    return (vRes->usage[i].pDraw);

    return (DrawablePtr) NULL;
}


/*
 *    NAME
 *        FindActiveVideoIO - look for a VideoDrawPtr using a videoIO
 *
 *    SYNOPSIS
 */
static VideoDrawPtr
FindActiveVideoIO(vRes, videoIO)
    VideoResourcePtr vRes;	/* in: the video resource structure */
    VideoIO videoIO;
/*
 *    DESCRIPTION
 *        Scan the interla list of active video drawable info looking
 *	  for an active render/capture using the specified videoIO id.
 *
 *    RETURNS
 *        The VideoDrawPtr pointer using the videoIO, else NULL.
 */
{
    int i;
    ActiveVideoPtr activeVDraws = &vRes->vp->activeVDraws;
    VideoDrawPtr vDraw;

    for (i = activeVDraws->nEntries - 1; i >= 0; i--) {
	vDraw = activeVDraws->vDrawArray[i];
	if (VIO_ACTIVE(vDraw, videoIO))
	    return (vDraw);
    }
    return ((VideoDrawPtr) NULL);
}

/*
 *    NAME
 *        DeleteVideoUse - remove a use from the structure
 *
 *    SYNOPSIS
 */
/*ARGSUSED*/
static void 
DeleteVideoUse(vRes, vDraw, pDraw)
    VideoResourcePtr	vRes;	/* in: VideoOut or VideoIn resource */
    DrawablePtr         pDraw;	/* in: drawable pointer */
    VideoDrawPtr	vDraw;	/* in: Drawable whose use is to be deleted */
/*
 *    DESCRIPTION
 *        Find the use and remove it.
 *
 *    RETURNS
 *        void
 */
{
    int			i, k;
    ActiveVideoPtr	activeVDraws = &vRes->vp->activeVDraws;

    for (i=0; i < vRes->maxUse; i++)
	if (vRes->usage[i].pDraw == pDraw) {
	    if (vRes->usage[i].owned) {
		for (k=0; k < vRes->maxUse; k++) {
		    if (vRes->owners[k].pClient == vRes->usage[i].pClient && 
			vRes->owners[k].inUse) {
			vRes->owners[k].inUse = FALSE;
			break;
		    }
		}
	    }
	    vRes->useCount--;
	    vRes->usage[i].pClient = (ClientPtr) NULL;
	    vRes->usage[i].pDraw = (DrawablePtr) NULL;
	    vRes->usage[i].owned = FALSE;
	    break;
	}

    /*
     * Find this vDraw in the static global array of active video
     * drawable entries.  If the entry is no longer active (no capture OR
     * render), remove it from the array, repack the array and decrement
     * the "nEntries" field.
     * Don't depend on the caller having unset the "active" field of
     * the vDraw before we get here.
     */
    if (VDRAW_INACTIVE(vDraw)) {
	for (i = 0; i < activeVDraws->nEntries; i++) {
	    if (vDraw == activeVDraws->vDrawArray[i]) {
		if ( --activeVDraws->nEntries > i) {
		    activeVDraws->vDrawArray[i] =
			activeVDraws->vDrawArray[activeVDraws->nEntries];
		}
		break;
	    }
	}
    }
}

/*
 *    NAME
 *        AddVideoUse - add a use to a resource structure
 *
 *    SYNOPSIS
 */
/*ARGSUSED*/
static void 
AddVideoUse(vDraw, pDraw, vRes, client)
    VideoDrawPtr	vDraw;	/* in: mvex info about drawable being used */
    DrawablePtr         pDraw;	/* in: window or pixmap */
    ClientPtr		client;	/* in: using client */
    VideoResourcePtr	vRes;	/* out: resource to add to */
/*
 *    DESCRIPTION
 *        Find the place to add the use, add it.  Assume that it will
 *	  work; i.e. the caller has confirmed that USE_SATURATED(vRes)
 *	  is false.
 *
 *    RETURNS
 *        void
 */
{
#define VDRAW_ARRAY_ALLOC_INC	1
    int			i,k;
    ActiveVideoPtr	activeVDraws = &vRes->vp->activeVDraws;

    assert (USE_SATURATED(vRes));

    /*
     * Find an empty usage slot and claim it for this client/drawable
     */
    vRes->useCount++;
    for (i=0; i < vRes->maxUse; i++) 
	if (!vRes->usage[i].pDraw) {
	    vRes->usage[i].pClient = client;
	    vRes->usage[i].pDraw = pDraw;
	    break;
	}
    assert(i < vRes->maxUse);

    /*
     * Find an unused ownership, if any, and claim it for this use
     */
    vRes->usage[i].owned = FALSE;
    for (k=0; k < vRes->maxUse; k++)
	if (vRes->owners[k].pClient == client && !vRes->owners[k].inUse) {
	    vRes->owners[k].inUse = TRUE;
	    vRes->usage[i].owned = TRUE;
	    break;
	}

    /*
     * Keep a static global list of all vDraws with an active Capture or
     * Render.  If a vDraw is already on the list (because it has one
     * type of video activity and we are adding the second type), don't
     * add anything.
     * The list is an array.  Add to the back of the array.
     * Increment the count if you add an entry.
     */
    for (i = activeVDraws->nEntries - 1; i >= 0; i--)
	if (activeVDraws->vDrawArray[ i ] == vDraw)
	    return;
    if (activeVDraws->maxEntries == 0) {
	activeVDraws->vDrawArray = (VideoDrawPtr *)
	    Xalloc(sizeof(VideoDrawPtr) * VDRAW_ARRAY_ALLOC_INC);
	activeVDraws->maxEntries = VDRAW_ARRAY_ALLOC_INC;
    } else if (activeVDraws->maxEntries == activeVDraws->nEntries) {
	activeVDraws->maxEntries += VDRAW_ARRAY_ALLOC_INC;
	activeVDraws->vDrawArray = (VideoDrawPtr *)
	    xrealloc(activeVDraws->vDrawArray,
		     sizeof(VideoDrawPtr) * activeVDraws->maxEntries);
    }
    if (!activeVDraws->vDrawArray) {
	activeVDraws->maxEntries = 0;
	activeVDraws->nEntries = 0;
	return;
    }
    activeVDraws->vDrawArray[activeVDraws->nEntries++] = vDraw;

#undef VDRAW_ARRAY_ALLOC_INC
}

/*
 *    NAME
 *        StopRenderOrCapture - stop full motion video (internal)
 *
 *    SYNOPSIS
 */
StopRenderOrCapture(vp, vDraw, vRes, vio, overrideId, state)
    VideoScreenPtr	vp;	/* in: Video screen of drawable */
    VideoDrawPtr	vDraw;	/* in: Drawable whose video is to be stopped */
    VideoResourcePtr	vRes;	/* in: VideoOut or VideoIn resource */
    VideoIO		vio;	/* in: VideoOut or VideoIn id */
    VideoIO		overrideId;/* in: id in overriding request */
    int			state;	/* in: reason for stop */
/*
 *    DESCRIPTION
 *        Delete the use, send an event, stop the video.  This routine calls
 *        ddx routines to actually affect the video in/out.
 *
 *    RETURNS
 *        void
 */
{
    DrawablePtr		pDraw = vDraw->pDraw;

    /*
     * The video may never have gotten the chance to get started.
     * for example, the server receives two requests back to back,
     * calls SetupVideo for the first, and on the second, discovers
     * that it must stop the first.
     *
     * Tough luck.
     */

    SendVideoOverrideEvent(pDraw, vio, overrideId, state);
    (*vRes->vElement->pvio->StopVideo)(vp, pDraw, vDraw);
    if (vRes->vElement->type == Input) {
	vDraw->renderActive = NoVideo;
	if (vDraw->renderAction.pGcClip) {
	    (*vp->pScreen->RegionDestroy)(vDraw->renderAction.pGcClip);
	    vDraw->renderAction.pGcClip = (RegionPtr) NULL;
	}
	if (vDraw->renderAction.freeCompClip) {
	    (*vp->pScreen->RegionDestroy)(vDraw->renderAction.pClip);
	    vDraw->renderAction.freeCompClip = FALSE;
	}
    } else
	vDraw->captureActive = NoVideo;
    DeleteVideoUse(vRes, vDraw, pDraw);
}

/*
 *    NAME
 *        StopVideo - stop a full motion render or capture
 *
 *    SYNOPSIS
 */
StopVideo(vp, vDraw, overrideId, action, state)
    VideoScreenPtr vp;	/* in: Video screen of drawable */
    VideoDrawPtr vDraw;	/* in: Drawable whose video is to be stopped */
    VideoIO overrideId;	/* in: id in overriding request */
    int	action;		/* in: (MVEXStopRenderVideo|MVEXStopCaptureGraphics) */
    int state;		/* in: reason for stop */
/*
 *    DESCRIPTION
 *        Find the resource, use StopRenderOrCapture to stop the video.
 *
 *    RETURNS
 *        void
 */
{
    VideoResourcePtr	vRes;

    if (vDraw->renderActive && (action & MVEXStopRenderVideo)) {
	vRes = (VideoResourcePtr) 
		LookupIDByType((XID)vDraw->videoIn, vp->vIdType);
	assert(vRes);
	StopRenderOrCapture(vp, vDraw, vRes,
			    vDraw->videoIn, overrideId, state);
    }
    if (vDraw->captureActive && (action & MVEXStopCaptureGraphics)) {
	vRes = (VideoResourcePtr) 
		LookupIDByType(vDraw->videoOut, vp->vIdType);
	assert(vRes);
	StopRenderOrCapture(vp, vDraw, vRes,
			    vDraw->videoOut, overrideId, state);
    }
    /*
    /* XXX
     * Can I delete vDraw now?  vDraw is a resource associated with a
     * pDraw.
     */
}

/*
 *    NAME
 *        GetVideoValues - parse the mask/valuelist in a RenderVideo
 *			   or CaptureGraphics request.
 *
 *    SYNOPSIS
 */
static
GetVideoValues(client, pAct, vMask, valueList, nValues)
    ClientPtr		client;
    VideoActionPtr	pAct;
    CARD32		vMask;
    XID			*valueList;
    int			nValues;
/*
 *    DESCRIPTION
 *        iterate through each of the masks, like that done in
 *	  ProcCreateWindow.
 *	  The value-mask specifies which attributes are to be changed; the
 *	  value-list contains one value for each one bit in the mask, from
 *	  least to most significant bit in the mask.
 *
 *    RETURNS
 *        Success if all is well, otherwise an appropriate protocol
 *	  error value.
 */
{
    int error = Success;
    int nValuesNeeded;
    register Mask tmask;
    union {
	XID *pXID;
	xFraction *pfract;
	xFrame *pframe;
	xTimecode *ptimec;
    } vList;
    register Mask index;
    VioDataPtr pvio = pAct->vRes->vElement->pvio;
    xVideoAbility *pva = &pvio->pAbility->xva;

    /*
     * Fill in defaults
     */
    pAct->fullMotion = TRUE;
    pAct->priority = 100;
    pAct->markerType = None;
    pAct->markerSpecified = 0;
    pAct->saturation = pva->normalSaturation;
    pAct->contrast = pva->normalContrast;
    pAct->hue = pva->normalHue;
    pAct->bright = pva->normalBright;

    vList.pXID = valueList;
    tmask = vMask;
    nValuesNeeded = 0;
    while (tmask) {
        index = (Mask) lowbit (tmask);
        tmask &= ~index;

        switch (index) {
	case VRFullMotion:	nValuesNeeded += 1; break;
	case VRPriority:	nValuesNeeded += 1; break;
	case VRMarkerType:	nValuesNeeded += 1; break;
	case VRInFrame:		nValuesNeeded += sz_xFrame >> 2; break;
	case VROutFrame:	nValuesNeeded += sz_xFrame >> 2; break;
	case VRInTimecode:	nValuesNeeded += sz_xTimecode >> 2; break;
	case VROutTimecode:	nValuesNeeded += sz_xTimecode >> 2; break;
	case VRHue:		nValuesNeeded += sz_xFraction >> 2; break;
	case VRSaturation:	nValuesNeeded += sz_xFraction >> 2; break;
	case VRBrightness:	nValuesNeeded += sz_xFraction >> 2; break;
	case VRContrast:	nValuesNeeded += sz_xFraction >> 2; break;
	}

	if (nValuesNeeded > nValues)
	     return BadLength;

        switch (index) {
	case VRFullMotion:
	    pAct->fullMotion = (Bool) *vList.pXID++;
	    break;

	case VRPriority:
	    if ((pAct->priority = (int) *vList.pXID++) > 100) {
		client->errorValue = pAct->priority;
		return BadValue;
	    }
	    break;

	case VRMarkerType:
	    pAct->markerType = (int) *vList.pXID++;
	    if (pAct->markerType == MVEXFrameMarker) {
		if ((vMask & (VRInFrame | VROutFrame)) == 0) {
		    client->errorValue = MVEXFrameMarker;
		    return BadMatch;
		}
	    } else if (pAct->markerType == MVEXTimecodeMarker) {
		if ((vMask & (VRInTimecode | VROutTimecode)) == 0) {
		    client->errorValue = MVEXFrameMarker;
		    return BadMatch;
		}
	    } else if (pAct->markerType == None) {
		;
	    } else {
		client->errorValue = pAct->markerType;
		return BadValue;
	    }
	    break;

	case VRInFrame:
	case VROutFrame:
	    if (vList.pframe->field != MVEXFieldOne
	     && vList.pframe->field != MVEXFieldTwo) {
		client->errorValue = vList.pframe->field;
		return BadValue;
	    }
	    if (index == VRInFrame)
		pAct->inFrame = *vList.pframe++;
	    else
		pAct->outFrame = *vList.pframe++;
	    break;

	case VRInTimecode:
	case VROutTimecode:
	    if (vList.ptimec->field != MVEXFieldOne
	     && vList.ptimec->field != MVEXFieldTwo) {
		client->errorValue = vList.ptimec->field;
		return BadValue;
	    }
	    if (vList.ptimec->minute > 59) {
		client->errorValue = vList.ptimec->minute;
		return BadValue;
	    }
	    if (vList.ptimec->second > 59) {
		client->errorValue = vList.ptimec->second;
		return BadValue;
	    }
	    if (index == VRInTimecode)
		pAct->inTimecode = *vList.ptimec++;
	    else
		pAct->outTimecode = *vList.ptimec++;
	    break;

	/*
	 * It is better for the ddx layer to decide wether these values
	 * are any good.  However, we still check the denominator.
	 */
	case VRHue:
	case VRSaturation:
	case VRBrightness:
	case VRContrast:
	    if (vList.pfract->denominator <= 0) {
		client->errorValue = vList.pfract->denominator;
		return BadValue;
	    }

	    switch (index) {
	    case VRHue: pAct->hue = *vList.pfract++; break;
	    case VRSaturation: pAct->saturation = *vList.pfract++; break;
	    case VRBrightness: pAct->bright = *vList.pfract++; break;
	    case VRContrast: pAct->contrast = *vList.pfract++; break;
	    }
	    break;
	}
    }

    return (error);
}

/*
 *    NAME
 *        RenderOrCapture - start a render or capture
 *
 *    SYNOPSIS
 */
int
RenderOrCapture(client, pNewAction, pGC, vMask, valueList, nValues)
    ClientPtr client;			/* in: client */
    VideoActionPtr pNewAction;	/* in: render/capture data */
    GC *pGC;
    CARD32 vMask;
    XID *valueList;
    int nValues;
/*
 *    DESCRIPTION
 *        Check a million things and then do it.
 *
 *	  Note that the hardware has the option to not start just yet.
 *	  In this case, the mvex block handler will tell it to start
 *	  just before the server goes to sleep.
 *
 *    RETURNS
 *        Any error found.
 */
{
    VideoScreenPtr	vp;
    VideoResourcePtr	vRes;
    VideoDrawPtr	vDraw, vDrawUsingVio;
    VioDataPtr		pvio;
    VioPortList		*pList;
    DrawablePtr		pDraw = pNewAction->drawable;
    DrawablePtr		pDrawWithoutOwner;
    screenIndexRec	*screen = (screenIndexRec *) NULL;
    RegionRec		noClip;
    short		*active;
    XID			*vio;
    int			stop;
    int			errorReturn;
    int			i;
    int			request = pNewAction->request;
    VideoActionPtr	pCurrentAction;
    ColormapPtr         pcmp;


    /*
     * check the Vin/Vout.
     */
    vp = ScreenVideoInfo[ pDraw->pScreen->myNum ];
    if (vp == NULL)
	vRes = (VideoResourcePtr) NULL;
    else
	vRes = (VideoResourcePtr) 
		LookupIDByType(pNewAction->videoIO, vp->vIdType);
    if (vRes) {
	for (screen = vRes->screens; screen; screen = screen->next) {
	    if (pDraw->pScreen == screen->pScreen) {
		break;
	    }
	}

	if (!screen) {
	    client->errorValue = pNewAction->videoIO;
	    return BadMatch;
	}
    }

    if (!vRes
     || (vRes->vElement->type != ((request == MVEXRender) ? Input: Output))) {
	client->errorValue = pNewAction->videoIO;
	return BAD_VIDEO;
    }
    pNewAction->vRes = vRes;
    pvio = vRes->vElement->pvio;

    /*
     * be sure the depth/visual of the vin/vout and the drawable match
     */
    pcmp = (request == MVEXRender) ? (ColormapPtr) NULL : pNewAction->pcmap;
    if (! ModelMatch(vRes, pDraw, pcmp)) {
	client->errorValue = pDraw->id;
	return BadMatch;
    }

    /*
     * Look up the port atom and change it into the port index supplied
     * by the ddx layer.  Note that we don't bother looking up the atom
     * because its either in the ddx list (a valid atom), or it is
     * something else.  In the latter case, call it a match error.
     */
    pList = pvio->pPortList;
    for (i = pList->nPorts-1; i >= 0; i--) {
	if (pList->ports[ i ] == pNewAction->port) {
	    pNewAction->port = i;	/* now its an index! */
	    break;
	}
    }
    if (i < 0) {
	client->errorValue = pNewAction->port;
	return BadMatch;
    }
    
    /*
     * Check out the values in the value list.
     */
    errorReturn = GetVideoValues(client, pNewAction, vMask, valueList, nValues);
    if (errorReturn != Success)
	return errorReturn;

    pNewAction->vDraw = vDraw = GetVideoDrawable(vp, pDraw);
    if (request == MVEXRender) {
	(*pvio->GetCompositeClip)(pGC, pNewAction);
	pNewAction->subWindowMode = pGC->subWindowMode;

	active = &vDraw->renderActive;
	stop = MVEXStopRenderVideo;
	vio = &vDraw->videoIn;
	pCurrentAction = &vDraw->renderAction;
    } else {
        /* subWindowMode already assigned in ProcCaptureGraphics */
	/*
	 * This may not be exactly right.
	 */
	if (pDraw->type == DRAWABLE_WINDOW)
	    pNewAction->pClip = &((WindowPtr)pDraw)->borderClip;
	else {
	    BoxRec pixmapExtents;
	    pixmapExtents.x1 = pDraw->x;
	    pixmapExtents.y1 = pDraw->y;
	    pixmapExtents.x2 = pDraw->x + pDraw->width;
	    pixmapExtents.y2 = pDraw->y + pDraw->height;
	    (*vp->pScreen->RegionInit)(&noClip, &pixmapExtents, 1);
	    pNewAction->pClip = &noClip;
	}

	active = &vDraw->captureActive;
	stop = MVEXStopCaptureGraphics;
	vio = &vDraw->videoOut;
	pCurrentAction = &vDraw->captureAction;
    }

    /*
     * Figure out if concurrent use prevents this request from happening
     * See if we need to StopVideo on some other drawable used by this
     * videoIO to allow this one to happen.
     *
     * The decision tree:
     *		If (current use < max use)
     *			Request succeeds
     *			No events generated
     *		else if (there is current use by a client having insufficient
     *			 ownerships)
     *			StopVideo on one of that client's uses
     *				VideoOverride event is generated
     *			Request succeeds
     *		else 
     *			Request fails
     *			VideoRequest event is generated
     */
    if (USE_SATURATED(vRes)) {
	pDrawWithoutOwner = GetUseWithoutOwner(vRes);
	if (!pDrawWithoutOwner) {
	    SendVideoRequestEvent(request, pNewAction, TRUE);
	    return Success;
	}
	else
	    StopVideo(ScreenVideoInfo[ pDrawWithoutOwner->pScreen->myNum ],
		      GetVideoDrawable(vp, pDrawWithoutOwner),
		      pNewAction->videoIO, /* overriding request */
		      stop,
		      MVEXRequestOverride);
    }

    /*
     * If there there is already a competing VideoAction
     * on the drawable or on the videoIO, then stop it.
     */
    if (*active && pNewAction->videoIO == pCurrentAction->videoIO)
	StopVideo(vp, vDraw, pNewAction->videoIO, stop, MVEXReused);
    if (vRes->useCount > 1
     && (vDrawUsingVio = FindActiveVideoIO(vRes, pNewAction->videoIO)))
	StopVideo(vp, vDrawUsingVio, pNewAction->videoIO, stop, MVEXReused);

    /*
     * After the sync value is set here, it is changed by whomever detects
     * a change in the state: ddx or dix.
     */
    pNewAction->hasSync = FALSE;

    if (! (*pvio->ValidateRequest)(vp, pNewAction)) {
	SendVideoSyncEvent(pNewAction->drawable,
			   pNewAction->videoIO,
			   MVEXSyncAcquired);
	pNewAction->hasSync = TRUE;
	pNewAction->running = TRUE;
	*active = NoVideo;
    } else {
	pNewAction->running =
	    (*pvio->SetupVideo)(vp, pNewAction, TRUE, &errorReturn);

	if (errorReturn != Success)
	    return errorReturn;
	*active = (pNewAction->fullMotion) ? MotionVideo : StillVideo;
	if (pvio->timeout)
	    pvio->lastCall = GetTimeInMillis();
    }

    *vio = pNewAction->videoIO;
    SendVideoRequestEvent(request, pNewAction, FALSE);

    /*
     * hang on to the original request.  Note that if there was an
     * active request for this vDraw, the StopVideo above will
     * have freed up any space in the pCurrentAction... we can
     * scribble over the top of it.
     */
    AddVideoUse(vDraw, pDraw, vRes, client);
    *pCurrentAction = *pNewAction;
    /*
     * This is only here so that we can tell when a drawable gets
     * resized 
     */
    vDraw->drawableWidth = pDraw->width;
    vDraw->drawableHeight = pDraw->height;

    return (errorReturn);
}


/*
 *    NAME
 *        ModelMatch - do the visuals/depths match?
 *
 *    SYNOPSIS
 */
Bool
ModelMatch(vRes, pDraw, pcmp)
    VideoResourcePtr	vRes;
    DrawablePtr		pDraw;
    ColormapPtr         pcmp;
/*
 *    DESCRIPTION
 *        Compare the Vin/Vout rendermodels with the drawables.
 *
 *    RETURNS
 *        TRUE if they match, FALSE if not.
 */
{
    unsigned char	modelMask;
    VioDataPtr pvio;
    WindowOptPtr ancwopt;
    VideoVisual *pAllowedDepths;
    unsigned long vid;
    int i;
    Bool fPixmapDrawable = FALSE;
    Bool fWindowDrawable = FALSE;

    switch (pDraw->type) {
    case DRAWABLE_PIXMAP:
	modelMask = MVEXPixmapModelMask;
	fPixmapDrawable = TRUE;
	break;
    case DRAWABLE_WINDOW:
	modelMask = MVEXWindowModelMask;
	fWindowDrawable = TRUE;
	ancwopt = ((WindowPtr)pDraw)->optional;
	if (!ancwopt)
	    ancwopt = FindWindowWithOptional((WindowPtr)pDraw)->optional;
	break;
    default:
        return (FALSE);
    }

    pvio = vRes->vElement->pvio;
    for (i=0; i < pvio->nAllowedDepths; i++) {
	pAllowedDepths = &pvio->pAllowedDepths[i];
	vid = pAllowedDepths->xrm.visualid;
	if ((modelMask & pAllowedDepths->model) &&
	     pDraw->depth == pAllowedDepths->xrm.depth) {
	    if (fPixmapDrawable && (!pcmp || pcmp->pVisual->vid == vid))
		return (TRUE);
	    if (fWindowDrawable && ancwopt->visual == vid)
		return (TRUE);
	}
    }
    return FALSE;
}

Bool mvexRealizeWindow(pWindow)
    WindowPtr pWindow;
{
    VideoDrawPtr	vDraw;
    ScreenPtr		pScr = pWindow->drawable.pScreen;
    VideoScreenPtr	vp = ScreenVideoInfo[ pScr->myNum ];
    VideoResourcePtr    vRes;
    Bool		ret;
    int			i;

    if ((vDraw = (VideoDrawPtr)
		 LookupIDByType(pWindow->drawable.id, vp->vDrawType))) {
	 /*
	 * We know that this drawable has had a Render and/or Capture requested
	 * on it.  It may or may not still be active.  Call ddx either way.
	 */
	for (i=0; i < 2; i++){
	    if (vRes = (VideoResourcePtr)
			    LookupIDByType((i == 0 ? vDraw->videoIn
						   : vDraw->videoOut),
					    vp->vIdType))
		(*vRes->vElement->pvio->RealizeWindow)(vDraw, vRes);
	}
    }

    pScr->RealizeWindow = ScreenVideoInfo[ pScr->myNum ]->RealizeWindow;
    ret = (*pScr->RealizeWindow)(pWindow);
    pScr->RealizeWindow = mvexRealizeWindow;

    return ret;

}

Bool mvexUnrealizeWindow(pWindow)
    WindowPtr pWindow;
{
    VideoDrawPtr	vDraw;
    ScreenPtr		pScr = pWindow->drawable.pScreen;
    VideoScreenPtr	vp = ScreenVideoInfo[ pScr->myNum ];
    VideoResourcePtr    vRes;
    Bool		ret;
    int			i;

    if ((vDraw = (VideoDrawPtr)
		 LookupIDByType(pWindow->drawable.id, vp->vDrawType))) {
	 /*
	 * We know that this drawable has had a Render and/or Capture requested
	 * on it.  It may or may not still be active.  Call ddx either way.
	 */
	for (i=0; i < 2; i++){
	    if (vRes = (VideoResourcePtr)
			    LookupIDByType((i == 0 ? vDraw->videoIn
						   : vDraw->videoOut),
					    vp->vIdType))
		(*vRes->vElement->pvio->UnrealizeWindow)(vDraw, vRes);
	}
    }

    pScr->UnrealizeWindow = ScreenVideoInfo[ pScr->myNum ]->UnrealizeWindow;
    ret = (*pScr->UnrealizeWindow)(pWindow);
    pScr->UnrealizeWindow = mvexUnrealizeWindow;

    return ret;
}

void mvexClipNotify(pWindow, dx, dy)
    WindowPtr pWindow;
    int    dx, dy;		/* offset with respect to the old window pos. */
{
    VideoDrawPtr	vDraw;
    ScreenPtr		pScr = pWindow->drawable.pScreen;
    VideoScreenPtr	vp = ScreenVideoInfo[ pScr->myNum ];
    Bool		fVexNotify = TRUE;
    Bool		fGravityTranslate = FALSE;
    int			dWidth, dHeight;
    int			*px, *py;
    int			i;
    VideoIO		vio;
    VideoResourcePtr	vRes;

    if ((vDraw = (VideoDrawPtr)
		 LookupIDByType(pWindow->drawable.id, vp->vDrawType))) {
	/*
	 * First, check to see if the window has been resized.
	 * If it has been resized,
	 *	if its bitGravity is ForgetGravity,
	 *		then StopVideo.
	 *	else
	 *		remember its new height and width.
	 *		adjust x and y of VideoActionRec(s)
	 */
	if (vDraw->drawableWidth != pWindow->drawable.width ||
	    vDraw->drawableHeight != pWindow->drawable.height) {

	    if (pWindow->bitGravity == ForgetGravity) {
		StopVideo(vp,
			  vDraw,
			  (VideoIO) 0, /* overriding video id (i.e. none) */
			  (MVEXStopRenderVideo|MVEXStopCaptureGraphics),
			  MVEXDrawableChange);
		fVexNotify = FALSE;
	    } else {
		dWidth = (int)pWindow->drawable.width
			 - (int)vDraw->drawableWidth;
		dHeight = (int)pWindow->drawable.height
			 - (int)vDraw->drawableHeight;
		vDraw->drawableWidth = pWindow->drawable.width;
		vDraw->drawableHeight = pWindow->drawable.height;
		fGravityTranslate = TRUE;
	    }
	}
	/*
	 * We know that this drawable has had a Render and/or Capture requested
	 * on it.  It may or may not still be active.  Call ddx either way.
	 */
	if (fVexNotify) {
	    for (i=0; i < 2; i++){
		if (i == 0) {
		    vio = vDraw->videoIn;
		    px = &vDraw->renderAction.dix.dstX;
		    py = &vDraw->renderAction.dix.dstY;
		} else {
		    vio = vDraw->videoOut;
		    px = &vDraw->captureAction.dix.srcX;
		    py = &vDraw->captureAction.dix.srcY;
		}
		if (vRes = (VideoResourcePtr) LookupIDByType(vio, vp->vIdType)){
		    if (fGravityTranslate) {
			GravityTranslate (*px, *py,
					  *px - dx, *py - dy,
					  dWidth, dHeight,
					  pWindow->bitGravity,
					  px, py);
		    }
		    (*vRes->vElement->pvio->ClipNotify)(vDraw, vRes, dx, dy);
		}
	    }
	}
    }

#if defined(UTEK) || defined(UTEKV)
    /* XXX
     * rws says that someday the pScr->ClipNotify vector will be a standard
     * part of the sample server and then we can remove the ifdef's
     */
    pScr->clipNotify = ScreenVideoInfo[ pScr->myNum ]->ClipNotify;
    if (pScr->clipNotify)
	(*pScr->clipNotify)(pWindow, dx, dy);
    pScr->clipNotify = mvexClipNotify;
#endif    /*UTEK  || UTEKV*/

}
