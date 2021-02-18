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
static char *rcsid=  "$Header: /n/lacey/usr/local/src/video/MVEX/server/ddx/mi/RCS/vmiinit.c,v 1.14 1991/09/26 21:20:45 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#include <sys/time.h>
#include "X.h"
#include "scrnintstr.h"
#include "misc.h"
#include "MVEX.h"
#include "MVEXproto.h"
#include "videostr.h"
#include "vmi.h"

static void vmiChangeConnectivity();
static Bool vmiQueryVideo();
static void vmiBlockHandler();
static void vmiWakeupHandler();
static void vmiFreeConnectivityInfo();

#define VMI_MAX_SCREENS	3
int vmiGCPrivateIndex[ 3 ];

/*
 * ddxMvexInit -- perform ddx initialization for MVEX
 *
 * Initialize all the elements of a VideoScreenRec for this screen.
 * ddx allocates and frees all space attached to VideoScreenRec
 * structures.  dix has set all elements to proper state for no video attached
 * to this screen.
 *
 * Return FALSE if allocation failure, else TRUE.
 */

Bool
ddxMvexInit(pScr, pInitVideoInfo)
    ScreenPtr pScr;			/* Screen to be initialized */
    VideoScreenPtr pInitVideoInfo;	/* Video data for the screen */
{
    /* inputs */
#ifdef MVEX_RASTEROPS_TCP
    Bool haveTcp;			/* Is tc/pip really here? */
#endif
#ifdef MVEX_SOFT_VIDEO
    Bool haveSoftVideo;			/* Is soft video really here? */
#endif
#ifdef MVEX_PARALLAX_1280
    Bool haveParallax1280;		/* Is parallax 1280 really here? */
#endif
    pInitVideoInfo->Reset = vmiScreenReset;
    pInitVideoInfo->QueryVideo = vmiQueryVideo;
    pInitVideoInfo->BlockHandler = vmiBlockHandler;
    pInitVideoInfo->WakeupHandler = vmiWakeupHandler;

    /*
     * Save the screen pointers and replace with vmi versions.  If the
     * ddx layer wants to replace them, it can.
     */
    pInitVideoInfo->GetImage = pScr->GetImage;
    pInitVideoInfo->CreateGC = pScr->CreateGC;

    pScr->GetImage = vmiGetImage;
    pScr->CreateGC = vmiCreateGC;

    /*
     * Do all input device initialization
     * Allow all input devices to add depths/visuals to the VideoScreenRec
     * (We split initialization of VideoIns into 2 steps:
     *	First allow all VideoIns to add DepthRec structures to the video
     *	screen information (pInitVideoInfo).  In a second step, allow
     *	all VideoIns to complete MVEX initialization.
     * This allows any VideoIn/Out to use any MVEX depths/visual in its
     * list of RenderModels.
     */
#ifdef MVEX_RASTEROPS_TCP
    if (!roTcpInit(pScr, pInitVideoInfo, &haveTcp)) {
	return(FALSE);
    }
#endif
#ifdef MVEX_SOFT_VIDEO
    if (!softvInit(pScr, pInitVideoInfo, &haveSoftVideo)) {
	return(FALSE);
    }
#endif
#ifdef MVEX_PARALLAX_1280
    if (!plx1280Init(pScr, pInitVideoInfo, &haveParallax1280)) {
	return(FALSE);
    }
#endif

    /*
     * Now do device and data initialization for all output devices
     */

    /*
     * Initialize VideoOuts
     */
#ifdef MVEX_RASTEROPS_TCS
    if (!roTcsInit(pScr, pInitVideoInfo, (Bool)NULL)) {
	return(FALSE);
    }
#endif
#ifdef MVEX_LYON_LAMB_RTC
    if (!rtcInit(pScr, pInitVideoInfo, (Bool)NULL)) {
	return(FALSE);
    }
#endif
#ifdef MVEX_TEK_OPT9
     /* Tektronix Option8/9 VideoOutput board. */
    if (!tekOpt9Init(pScr, pInitVideoInfo, (Bool)NULL)) {
	return(FALSE);
    }
#endif
#ifdef MVEX_A60
     /* Abekas A60 D1 recorder */
    if (!a60Init(pScr, pInitVideoInfo, (Bool)NULL)) {
	return(FALSE);
    }
#endif

    /*
     * Now do data initialization for all input devices
     */
#ifdef MVEX_RASTEROPS_TCP
    if (haveTcp)
	if (!(roTcpDataInit(pScr, pInitVideoInfo))) {
	return(FALSE);
    }
#endif
#ifdef MVEX_SOFT_VIDEO
    if (haveSoftVideo)
	if (!(softvDataInit(pScr, pInitVideoInfo))) {
	return(FALSE);
    }
#endif
#ifdef MVEX_PARALLAX_1280
    if (haveParallax1280)
	if (!(plx1280DataInit(pScr, pInitVideoInfo))) {
	return(FALSE);
    }
#endif

    /*
     * we only do this when we know that the ddx devices are present.
     * and that they are not going to do their own gc wrapping.
     */
    if (pInitVideoInfo->vElements == (VideoElementPtr) NULL) {
	if (pScr->CreateGC == vmiCreateGC)
	    pScr->CreateGC = pInitVideoInfo->CreateGC;
	if (pScr->GetImage == vmiGetImage)
	    pScr->GetImage = pInitVideoInfo->GetImage;
    } else {
	if (pScr->myNum >= VMI_MAX_SCREENS)
	    FatalError("vmi: out of screen storage!\n");
	vmiGCPrivateIndex[ pScr->myNum ] = AllocateGCPrivateIndex();
	if (!AllocateGCPrivate(pScr, vmiGCPrivateIndex[ pScr->myNum ],
			       sizeof(VmiPrivGC)))
	    return FALSE;
    }

    /*
     * only register the block handlers if this screen has video devices
     */
    if (pInitVideoInfo->vElements)
	(void) RegisterBlockAndWakeupHandlers (pInitVideoInfo->BlockHandler,
					       pInitVideoInfo->WakeupHandler,
					       (pointer) pInitVideoInfo);
    return (TRUE);
}

/*
 * vmiWakeupHandler - do mvex things just as server awakes
 */
/*ARGSUSED*/
static void
vmiWakeupHandler(vp, result, pReadmask)
    VideoScreenPtr vp;		/* video data */
    unsigned long result;	/* 32 bits of undefined result from the wait */
    pointer pReadmask;		/* the resulting descriptor mask */
/*
 *	DESCRIPTION
 *		When the server awakes (from a select call),
 *		we poll all the video devices that are active and have asked 
 *		to be polled.
 *
 *	RETURNS
 *		void
 *
 */
{
    int i, nEntries;
    ActiveVideoPtr activeVDraws = &vp->activeVDraws;
    VideoDrawPtr vDraw;
    VioDataPtr vio;
    long now;

    if ((nEntries = activeVDraws->nEntries) == 0)
	return;

    for (i = nEntries - 1; i >= 0; i--) {
	vDraw = activeVDraws->vDrawArray[ i ];
        if (VIO_ACTIVE(vDraw, vDraw->videoIn)) {
	    if (! vDraw->renderAction.running)
		continue;
	    vio = vDraw->renderAction.vRes->vElement->pvio;
	    if (! vio->timeout)
		continue;
	    (*vio->PollVideo)(vp, &vDraw->renderAction, result, pReadmask);
	}
        if (VIO_ACTIVE(vDraw, vDraw->videoOut)) {
	    if (! vDraw->captureAction.running)
		continue;
	    vio = vDraw->captureAction.vRes->vElement->pvio;
	    if (! vio->timeout)
		continue;
	    (*vio->PollVideo)(vp, &vDraw->captureAction, result, pReadmask);
	}
    }
}

/*
 * vmiBlockHandler - do mvex things before server sleeps
 */
/*ARGSUSED*/
static void
vmiBlockHandler(vp, pTimeout, pReadmask)
    VideoScreenPtr vp;		/* video data */
    pointer pTimeout;		/* OS version of time */
    pointer pReadmask;		/* the resulting descriptor mask */
/*
 *	DESCRIPTION
 *		Just before the server blocks waiting for more requests,
 *		we peruse the active video requests to make sure that
 *		all that are supposed to be started, are.
 *
 *		XXX this is OS dependent because it knows about OS time.
 *
 *	RETURNS
 *		void
 *
 */
{
    int i, nEntries;
    ActiveVideoPtr activeVDraws = &vp->activeVDraws;
    VideoDrawPtr vDraw;
    VioDataPtr vio;
    unsigned long timeout, sysTimeout;
    struct timeval *pWait;

    if ((nEntries = activeVDraws->nEntries) == 0)
	return;

    /*
     * figure out the current timeout in milliseconds
     */
    pWait = *((struct timeval **) pTimeout);
    timeout = sysTimeout = pWait->tv_sec * 1000 + pWait->tv_usec / 1000;

    for (i = nEntries - 1; i >= 0; i--) {
	vDraw = activeVDraws->vDrawArray[ i ];
        if (VIO_ACTIVE(vDraw, vDraw->videoIn)) {
	    vio = vDraw->renderAction.vRes->vElement->pvio;
	    if (vDraw->renderActive == MotionVideo
	     && ! vDraw->renderAction.running)
		vDraw->renderAction.running =
		    (*vio->StartVideo)(vp, &vDraw->renderAction);
	    if (vio->timeout && vio->timeout < timeout)
		timeout = vio->timeout;
	}
        if (VIO_ACTIVE(vDraw, vDraw->videoOut)) {
	    vio = vDraw->captureAction.vRes->vElement->pvio;
	    if (vDraw->captureActive == MotionVideo
	     && ! vDraw->captureAction.running)
		vDraw->captureAction.running =
		    (*vio->StartVideo)(vp, &vDraw->captureAction);
	    if (vio->timeout && vio->timeout < timeout)
		timeout = vio->timeout;
	}
    }

    if (timeout < sysTimeout) {
	pWait->tv_sec = timeout / 1000;
	pWait->tv_usec = (timeout % 1000) * 1000;
    }
}

/*
 * GetAllDepths - Get all core and mvex depths/visuals.
 */
VideoVisual *
GetAllDepths(pScreen, pVideoScreenRec, nDepths, fVisualsOnly)
    ScreenPtr pScreen;			/* Video Screen */
    VideoScreenPtr pVideoScreenRec;     /* video data */
    int *nDepths;			/* return the length of the array */
    int fVisualsOnly;			/* if TRUE, include only depths which
					 * have at least one visual */
/*
 *	DESCRIPTION
 *		Create an array of VideoVisual structures, one for each
 *		depth/visual in the core and video lists.
 *		Fill in depth and visual id fields.
 *		Each element of the array will have a valid VisualID 
 *		if fVisualsOnly is TRUE.
 *
 *	RETURNS
 *		The array of VideoVisual structures if nAllowedDepths > 0
 *		FALSE if nAllowedDepths == 0 or if malloc fails.
 *
 */
{
    int i,j,k,n;
    int nDepthsTmp = 0;
    VideoVisual *pAllowedDepths;
    DepthPtr pDepth;
    struct {
	int numDepths;
	DepthPtr allowedDepths;
    } DepthInfo[2];
	
    DepthInfo[0].numDepths = pScreen->numDepths;
    DepthInfo[0].allowedDepths = pScreen->allowedDepths;
    DepthInfo[1].numDepths = pVideoScreenRec->nDepths;
    DepthInfo[1].allowedDepths = pVideoScreenRec->pDepths;
    /*
     * count up the number of elements to put on list.
     * First check core screen, then MVEX VideoScreenRec.
     */
    for (n=0; n < 2; n++) {
	for (i = 0; i < DepthInfo[n].numDepths; i++) {
	    pDepth = &DepthInfo[n].allowedDepths[i];
	    nDepthsTmp += pDepth->numVids;
	    if (!fVisualsOnly && pDepth->numVids == 0)
		nDepthsTmp++;
	}
    }

    /*
     * Allocate list
     */
    pAllowedDepths = (VideoVisual *) xalloc(nDepthsTmp * sizeof(VideoVisual));
    if (!pAllowedDepths) {
	*nDepths = 0;
	return (VideoVisual *) NULL;
    }

    /*
     * Fill in list data
     */
    for (n=0, k=0; n < 2; n++) {
	for (i = 0; i < DepthInfo[n].numDepths; i++) {
	    pDepth = &DepthInfo[n].allowedDepths[i];
	    for (j = 0; j < pDepth->numVids; j++) {
		pAllowedDepths[k].xrm.depth = pDepth->depth;
		pAllowedDepths[k++].xrm.visualid = pDepth->vids[j];
	    }
	    if (!fVisualsOnly && pDepth->numVids == 0) {
		pAllowedDepths[k].xrm.depth = pDepth->depth;
		pAllowedDepths[k++].xrm.visualid = 0;
	    }
	}
    }

    *nDepths = nDepthsTmp;
    return pAllowedDepths;
}
/*
 * Free a list of depth structures created by CreateVideoDepths.
 */
void
FreeDepths(nDepths, pDepths)
    int nDepths;		/* number of depth structures */
    DepthPtr pDepths;		/* list of depth structures */
{
    int i;

    if (!pDepths)
	return;

    for(i=0; i < nDepths; i++) {
	if (pDepths[i].vids)
	    xfree(pDepths[i].vids);
    }
    xfree(pDepths);
}


/*
 * Add a unique MVEX depth/visual to pVideoScreenRec depths/visuals for 
 * this screen.
 * This routine does not check to see if the visual is different
 * than those already on the list (or different from the Core list).
 * It will avoid adding a new DepthRec structure to the VideoScreenRec
 * if the depth is already there.
 *
 * Modifies numVisuals, visuals, pDepths, and nDepths fields of pVideoScreenRec.
 *
 * Return FALSE if allocation fails, TRUE otherwise.
 */
Bool
AddVideoDepth(pVideoScreenRec, depth, pVis)
    VideoScreenPtr pVideoScreenRec;	/* video data */
    int depth;				/* video depth to add */
    VisualPtr pVis;			/* video visual to add (may be NULL) */
{
    VisualPtr pvisuals;

    /*
     * MVEX may have a list of depths/visuals which come with the video hardware
     * We need to put desired depths/visuals into pVideoScreenRec->pDepths.
     *
     * If there is a visual, add it to pVideoScreenRec->visuals.
     */

    if (!AddNewDepth(pVideoScreenRec, depth, pVis))
	return(FALSE);
    if (pVis) {
	pVideoScreenRec->numVisuals++;
	pvisuals = (VisualPtr) xrealloc(pVideoScreenRec->visuals, 
		sizeof(VisualRec) * pVideoScreenRec->numVisuals);
	if (!pvisuals) {
	    pVideoScreenRec->numVisuals--;
	    FreeDepths(pVideoScreenRec->nDepths, pVideoScreenRec->pDepths);
	    return(FALSE);
	}
	bcopy((char *)pVis, 
	      (char *)(pvisuals + pVideoScreenRec->numVisuals - 1),
	      sizeof(VisualRec));
	pVideoScreenRec->visuals =  pvisuals;
    }

    return (TRUE);
}

/*
 * Add a given depth and visual to the VideoScreenRec.  If there is already
 * a DepthRec for the given depth, use it.  Otherwise make a new DepthRec
 * structure and add it to pVideoScreenRec->pDepths.
 *
 * If a visual is given, add its VisualID to the list of VisualIDs in the
 * DepthRec.
 *
 * If alloc failure, delete list
 */
static Bool
AddNewDepth(pVideoScreenRec, newDepth, pVis)
    VideoScreenPtr pVideoScreenRec;     /* video data */
    int newDepth;		/* depth to add to list if not already present*/
    VisualRec *pVis;		/* if non-NULL, add vid to list for this depth*/
{
    int i, index;
    int fFound;
    unsigned long *pvids;
    DepthPtr pDepths;

    for(i=0, fFound=FALSE; i < pVideoScreenRec->nDepths; i++)
	if (pVideoScreenRec->pDepths[i].depth == newDepth) {
	    fFound = TRUE;
	    index = i;
	    break;
	}
    if (!fFound) {
	pVideoScreenRec->nDepths++;
	pDepths = (DepthPtr) xrealloc(pVideoScreenRec->pDepths, 
		sizeof(DepthRec) * pVideoScreenRec->nDepths);
	if (!pDepths) {
	    pVideoScreenRec->nDepths--;
	    FreeDepths(pVideoScreenRec->nDepths, pVideoScreenRec->pDepths);
	    return(FALSE);
	}
	index = pVideoScreenRec->nDepths - 1;
	pDepths[index].depth = newDepth;
	pDepths[index].numVids = 0;
	pDepths[index].vids = (unsigned long *)0;
	pVideoScreenRec->pDepths = pDepths;
    }

    if (pVis) {
	pVideoScreenRec->pDepths[index].numVids++;
	pvids = xrealloc(pVideoScreenRec->pDepths[index].vids,
		sizeof(unsigned long) *pVideoScreenRec->pDepths[index].numVids);
	if (!pvids) {
	    FreeDepths(pVideoScreenRec->nDepths, pVideoScreenRec->pDepths);
	    return(FALSE);
	}
	pvids[pVideoScreenRec->pDepths[index].numVids - 1] = pVis->vid;
	pVideoScreenRec->pDepths[index].vids = pvids;
    }

    return(TRUE);
}

/*
 * Return TRUE if depth is on the core list.
 * FALSE otherwise.
 */
Bool
IsCoreDepth(pScreen, depth)
    ScreenPtr pScreen;
    int depth;
{
    int i;
    for (i = 0; i < pScreen->numDepths; i++) {
	if (depth == pScreen->allowedDepths[i].depth)
	    return (TRUE);
    }
    return (FALSE);
}

/*
 * Find a core visual which matches "pVis".  If a match is found, return
 * the visual id.  Otherwise return None
 *
 * Parameters of the match are determined by bits set in 'mask'.
 * Possible values are bitwise OR of the following:
 *
 */
unsigned long
GetCoreVid(pScreen, mask, depth, pVis)
    ScreenPtr pScreen;
    unsigned long mask;
    int depth;
    VisualPtr pVis;
{
    int i, j;
    VisualPtr pVisCore;
    unsigned long vid;

    for (i = 0; i < pScreen->numDepths; i++) {
        if (!(mask & VisualDepthMask) ||
	      depth == pScreen->allowedDepths[i].depth) {
	    for (j = 0; j < pScreen->allowedDepths[i].numVids; j++) {
		vid = pScreen->allowedDepths[i].vids[j];
#ifdef R3
		pVisCore = (VisualPtr) LookupID(vid, RT_VISUALID, RC_CORE);
#else
		for (pVisCore = pScreen->visuals;
		     pVisCore->vid != vid;
		     pVisCore++)
		     ;
#endif
		if ((!(mask & VisualClassMask) || 
		       pVis->class == pVisCore->class) &&
		    (!(mask & VisualBitsPerRGBMask) ||
		       pVis->bitsPerRGBValue == pVisCore->bitsPerRGBValue) &&
		    (!(mask & VisualColormapSizeMask) ||
		       pVis->ColormapEntries == pVisCore->ColormapEntries) &&
		    ((pVis->class != DirectColor && 
			    pVis->class != TrueColor) ||
		        (!(mask & VisualRedMask) ||
			   pVis->redMask == pVisCore->redMask) &&
		        (!(mask & VisualGreenMask) ||
			   pVis->greenMask == pVisCore->greenMask) &&
		        (!(mask & VisualBlueMask) ||
			   pVis->blueMask == pVisCore->blueMask))) {

		    return(pVisCore->vid);
		}
	    }
	}
    }

    return (None);
}
/*
 * If we are going to add depths which are not listed in the core
 * connection, we must ensure that the core connection lists formats
 * for our new depths.
 *
 * Modifies info in dix global 'screenInfo'.
 *
 * If the format depth is already on the core list, we can't add it:
 * return TRUE and set 'oldFormat' to point to existing format data for
 * the given depth.
 *
 * Return FALSE if MAXFORMATS formats are already allocated (and format
 * will NOT be added).
 *
 * If format data can be added, return TRUE and set 'oldFormat' to NULL.
 */
Bool
mvexAddFormat(newFormat, oldFormat)
    PixmapFormatRec *newFormat;		/* in: format data to be added */
    PixmapFormatRec **oldFormat;	/* out: existing format data, if any */
{
    int j;
    int formatIndex;

    for (j = 0; j < screenInfo.numPixmapFormats; j++) {
	if (newFormat->depth == screenInfo.formats[j].depth) {
	    *oldFormat = &screenInfo.formats[j];
	    return TRUE;
	}
    }

    *oldFormat = (PixmapFormatRec *)NULL;
    formatIndex = screenInfo.numPixmapFormats;
    if (formatIndex == MAXFORMATS)
	return FALSE;
    screenInfo.formats[formatIndex].depth = newFormat->depth;
    screenInfo.formats[formatIndex].bitsPerPixel = newFormat->bitsPerPixel;
    screenInfo.formats[formatIndex].scanlinePad = newFormat->scanlinePad;

    screenInfo.numPixmapFormats++;

    return TRUE;
}

void
vmiScreenReset(pVideoScreen)
    VideoScreenRec *pVideoScreen;
{
    VideoElementPtr pvElement, nextVidEl;

    FreeDepths(pVideoScreen->nDepths, pVideoScreen->pDepths);
    /*
     * Free Devices and vin/vout structures
     */
    pvElement = pVideoScreen->vElements;
    while (pvElement) {
	(*pvElement->Reset)(pvElement);
	nextVidEl = pvElement->next;
	xfree(pvElement);
	pvElement = nextVidEl;
    }
}

void
vmiNoOp()
{}

/*
 * This routine allows ddx to ...
 * well, this routine may be useless.
 *
 * Return FALSE if allocation failure, else TRUE.
 */
/*ARGSUSED*/
static Bool
vmiQueryVideo(pScreen)
    ScreenPtr pScreen;
{
    /*
     * We don't have anything to do here.
     */
    return (TRUE);
}
/*
 *	NAME
 *		mvexProcessArgument - Process device-dependent command line args
 *
 *	SYNOPSIS
 */
/*ARGSUSED*/
int
mvexProcessArgument (argc, argv, i)
    int argc;
    char *argv[];	/* in: commandline */
    int i;		/* in: current index into argv */
/*
 *	DESCRIPTION
 *		Process command line.	Part of mvex/ddx interface.
 *
 *	RETURNS
 *		0 if argument is not device dependent, otherwise
 *		Count of number of elements of argv that are part of a 
 *		device dependent commandline option.
 *
 */
{
    return 0;
}

/*
 *	NAME
 *		mvexUseMsg - print use of device dependent commandline options
 *
 *	SYNOPSIS
 */
void
mvexUseMsg()
/*
 *	DESCRIPTION
 *		Print out correct use of device dependent commandline options.
 *		Part of mvex/ddx interface.
 *
 *	RETURNS
 *		None
 *
 */
{
}
