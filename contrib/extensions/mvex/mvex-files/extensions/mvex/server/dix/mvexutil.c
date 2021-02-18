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
static char *rcsid=  "$Header: /usr/local/src/x11r5/mit/extensions/RCS/mvexutil.c,v 2.2 1991/09/19 22:32:52 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#include <stdio.h>
#include "X.h"
#include "Xproto.h"
#include "misc.h"
#include "os.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "extnsionst.h"
#include "dixstruct.h"
#include "resource.h"
#include "opaque.h"
#include "regionstr.h"

#include "MVEX.h"
#include "MVEXproto.h"
#include "videostr.h"

/*
 * Create a VideoScreenRec structure and initialize the ddx fields.
 * Return NULL if malloc fails.
 */
VideoScreenRec *CreateVideoScreenRec(pScreen)
    ScreenPtr pScreen;			/* Video Screen */
{
    VideoScreenRec *pInitVideoInfo;

    if (!(pInitVideoInfo =
            (VideoScreenRec *) xalloc(sizeof(VideoScreenRec)))) {
        return((VideoScreenRec *) NULL);
    }

    pInitVideoInfo->pScreen = pScreen;
    pInitVideoInfo->numVisuals = 0;
    pInitVideoInfo->visuals = (VisualRec *)NULL;
    pInitVideoInfo->nDepths = 0;
    pInitVideoInfo->pDepths = (DepthPtr) NULL;

    pInitVideoInfo->inputOverlap = 0;
    pInitVideoInfo->captureOverlap = 0;
    pInitVideoInfo->ioOverlap = 0;

    pInitVideoInfo->vElements = (VideoElementPtr) NULL;

    return pInitVideoInfo;
}
/*
 * Get a VideoDrawRec structure for a given drawable.  Create
 * one if one does not already exist.
 *
 * Returns NULL if alloc error, else returns a VideoDrawPtr.
 */
VideoDrawPtr GetVideoDrawable(vp, pDraw)
    VideoScreenPtr vp;		/* Video screen */
    DrawablePtr pDraw;		/* Drawable */
{
    VideoDrawPtr vDraw;

    if (vDraw = (VideoDrawPtr) LookupIDByType(pDraw->id, vp->vDrawType))
	return (vDraw);

    vDraw = (VideoDrawPtr) xalloc(sizeof(VideoDrawRec));
    if (vDraw == NULL)
	return (vDraw);
    if (!AddResource(pDraw->id, vp->vDrawType, (pointer)vDraw)) {
	xfree(vDraw);
	return(NULL);
    }
    vDraw->renderActive = NoVideo;
    vDraw->captureActive = NoVideo;
    vDraw->videoIn = None;
    vDraw->videoOut = None;
    vDraw->pDraw = pDraw;
    vDraw->captureAction.pGcClip = (RegionPtr)NULL;
    vDraw->captureAction.freeCompClip = FALSE;
    vDraw->captureAction.request = MVEXCapture;
    vDraw->renderAction.pGcClip = (RegionPtr)NULL;
    vDraw->renderAction.freeCompClip = FALSE;
    vDraw->renderAction.request = MVEXRender;
    return(vDraw);
}
/*
 * Determine if a Visual ID is a MVEX visual ID for the given screen.
 * (If it is included on the list of visual IDs which are sent to the client
 * in QueryVideo).
 *
 * Returns a MVEX visual if vid is a MVEX visual ID
 *         NULL otherwise.
 */
VisualPtr GetVexVisual(pScreen, vid)
    ScreenPtr pScreen;			/* Video Screen */
    unsigned long vid;                  /* Visual ID */
{
    int		    i;
    VisualPtr       pVisual;
    VideoScreenRec *pVideoInfo;	/* Video Info struct for screen*/

    pVideoInfo = ScreenVideoInfo[pScreen->myNum];
    for (i=0, pVisual = pVideoInfo->visuals;
	 i < pVideoInfo->numVisuals;
	 i++, pVisual++) {

	if (vid == pVisual->vid)
	    return pVisual;
    }
    return (VisualPtr) NULL;
}
/*
 * Determine if a Visual ID is a MVEX visual ID for the given screen and
 * depth (depth 0 means the MVEX visual may be matched with any depth.)
 * (If it is included on the list of visual IDs which are sent to the client
 * in QueryVideo).
 *
 * Returns TRUE if vid is a MVEX visual ID for the given depth
 *         FALSE otherwise.
 */
Bool IsVexVidAndDepth(pScreen, depth, vid)
    ScreenPtr pScreen;			/* Video Screen */
    int depth;				/* depth for visual */
    unsigned long vid;                  /* Visual ID */
{
    VideoScreenRec *pVideoInfo;	/* Video Info struct for screen*/
    int idepth, ivisual;
    DepthPtr pDepth;

    pVideoInfo = ScreenVideoInfo[pScreen->myNum];
    for(idepth = 0; idepth < pVideoInfo->nDepths; idepth++) {
	pDepth = &pVideoInfo->pDepths[idepth];
	if ((depth == pDepth->depth) || (depth == 0)) {
	    for (ivisual = 0; ivisual < pDepth->numVids; ivisual++) {
		if (vid == pDepth->vids[ivisual]) {
		    return TRUE;
		}
	    }
	}
    }

    return FALSE;
}
/*
 * Determine if a depth is a MVEX depth for the given screen.
 * (If it is included on the list of depths which are sent to the client
 * in QueryVideo).
 *
 * Returns TRUE if vid is a MVEX  depth
 *         FALSE otherwise.
 */
Bool IsVexDepth(pScreen, depth)
    ScreenPtr pScreen;			/* Video Screen */
    int depth;				/* depth for visual */
{
    VideoScreenRec *pVideoInfo;	/* Video Info struct for screen*/
    int idepth;

    pVideoInfo = ScreenVideoInfo[pScreen->myNum];
    for(idepth = 0; idepth < pVideoInfo->nDepths; idepth++) {
	if (depth == pVideoInfo->pDepths[idepth].depth) {
	    return TRUE;
	}
    }

    return FALSE;
}
