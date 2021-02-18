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
static char *rcsid=  "$Header: /usr/local/src/x11r5.mvex/RCS/query.c,v 2.1 1991/09/12 23:21:23 toddb Exp $";
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

#include "MVEXproto.h"
#include "MVEX.h"
#include "videostr.h"

#define sz_CARD32       4
/*
 *    NAME
 *        QueryVideo - format and send the QueryVideo reply
 *
 *    SYNOPSIS
 */
int
QueryVideo(pWin, client)
    WindowPtr pWin;		/* in: window pointer */
    register ClientPtr	client;	/* in: client pointer */
/*
 *    DESCRIPTION
 *        Assemble and send the data.
 *
 *    RETURNS
 *        BadAlloc if memory won't allocate, else 0.
 */
{
    xQueryVideoReply    rep;
    VideoScreenRec *pVideoInfo;	/* Video Info struct for screen*/
    int nBytesData;			/* Size of Reply data */
    int nBytesAllowedDepths;		/* Size of AllowedDepths data */
    int nBytesVinModels;		/* Size of Input Models data */
    int nBytesVoutModels;		/* Size of Output Models data */
    int nBytesClipSize;			/* Size of ClipSize data */
    int nBytesPlacement;		/* Size of Placements data */
    int nBytesPorts;			/* Size of Ports data */
    int nBytesAdjustments;		/* Size of Adjustment data */
    pointer preplyData;			/* Reply data */
    int i,j,k;
    unsigned long vid;			/* Visual ID */
    DepthPtr        pDepth;
    VisualPtr       pVisual;
    VideoElementPtr pvElements, pvElement;
    int nDepths, nRenderModels, nVin, nVout, nPorts;
    xRenderModel *pRenderModels;
    VioDataPtr pvio;
    VioPortList *ppl;
    VideoAbility *pAbility;
    VideoGeometry *pattr;
    int type;
    union {
            xDepth *dp;
            xVisualType *vp;
            xVideoGeometry *vgp;
            xPlacement *pp;
            xVideoAbility *vap;
            CARD8 *card8p;
            CARD32 *card32p;
    } u;
    extern xRenderModel *GetRenderModels();

    /*
     * Give the ddx QueryVideo first shot...
     */
    pVideoInfo = ScreenVideoInfo[pWin->drawable.pScreen->myNum];
    if (!(*pVideoInfo->QueryVideo)(pWin->drawable.pScreen))
	return BadAlloc;

    /*
     * Fill in the basic reply (xQueryVideoReply)
     */
    rep.type = X_Reply;
    rep.sequenceNumber = client->sequence;
    if (sz_xQueryVideoReply > 32)
	rep.length = (sz_xQueryVideoReply - 32) >> 2;
    else 
	rep.length = 0;
    rep.screen = pWin->drawable.pScreen->myNum;
    rep.inputOverlap = pVideoInfo->inputOverlap;
    rep.captureOverlap = pVideoInfo->captureOverlap;
    rep.ioOverlap = pVideoInfo->ioOverlap;
    rep.time = pVideoInfo->lastVideoChangeTime.milliseconds;
    rep.majorVersion = MVEX_MAJOR_VERSION;
    rep.minorVersion = MVEX_MINOR_VERSION;
    nDepths = rep.nDepths = pVideoInfo->nDepths;
    /* rep.nAllowedDepths filled in below */
    /* rep.nVin filled in below */
    /* rep.nVout filled in below */

    /*
     * Now start counting up all the bytes needed to store the
     * rest of the QueryVideo reply, the LISTofFOO.
     */
    nBytesData = nDepths*sz_xDepth;
    for (i=0; i<nDepths; i++) {
	nBytesData += pVideoInfo->pDepths[i].numVids * sz_xVisualType;
    }

    /*
     * Get number of Vins and Vouts
     * Traverse the list of VideoElements to determine how many video
     * ins and outs are out there.
     */
    pvElements = pVideoInfo->vElements;
    nVin = nVout = 0;
    for (pvElement = pvElements; pvElement; pvElement = pvElement->next) {
	VioDataPtr pvio = pvElement->pvio;
	if (pvElement->type == Input)
	    nVin++;
	else
	    nVout++;
	nBytesData += pvio->pattr->xvg.nPlacement * sz_xPlacement;
	nBytesData += pvio->pAbility->nAdjustments * sz_xFractionRange;
	nBytesData += pvio->pPortList->nPorts * 4; /* what's sz_* for atom? */
    }
    rep.nVin = nVin;
    rep.nVout = nVout;
    nBytesData += (nVin + nVout) * (sz_xVideoGeometry+sz_xVideoAbility);

    /*
     * Count the bytes for RenderModels and port list lengths
     */
    pRenderModels = GetRenderModels(pVideoInfo, &nRenderModels);
    rep.nAllowedDepths = nRenderModels;

    nBytesAllowedDepths = nRenderModels*sz_xRenderModel;
    nBytesData += nBytesAllowedDepths;

    nBytesClipSize = nVin * sz_xRectangle;
    nBytesData += nBytesClipSize;

    nBytesVinModels = nVin * nRenderModels;
    nBytesVoutModels = nVout * nRenderModels;
    nBytesData += (nVout		/* bytes in out-port list lengths */
    		 + nVin			/* bytes in in-port list lengths */
		 + nBytesVinModels	/* bytes in vin models */
		 + nBytesVoutModels	/* bytes in vout models */
		 + 3) & ~ 0x3;		/* pad */

    if (!(preplyData = (pointer)ALLOCATE_LOCAL(nBytesData))) {
	xfree(pRenderModels);
	return(BadAlloc);
    }

    /*
     * Now start copying the data into place in the follow-on reply structure.
     * Start with the allowed depths.
     */
    u.dp = (xDepth *)preplyData;
    pDepth = pVideoInfo->pDepths;
    for (i = 0; i < nDepths; i++, pDepth++) {
	u.dp->depth = pDepth->depth;
	u.dp->nVisuals = pDepth->numVids;
	u.dp++;
	for (j = 0; j < pDepth->numVids; j++) {
	    vid = pDepth->vids[j];
	    /*
	     * We know this is a mvex visual (otherwise we wouldn't be sending
	     * it!).  Find it on MVEX visuals list.
	     */
	    for (pVisual = pVideoInfo->visuals;
		 pVisual->vid != vid;
		 pVisual++)
		;
	    assert(pVisual->vid == vid);
	    u.vp->visualID = pVisual->vid;
	    u.vp->class = pVisual->class;
	    u.vp->bitsPerRGB = pVisual->bitsPerRGBValue;
	    u.vp->colormapEntries = pVisual->ColormapEntries;
	    u.vp->redMask = pVisual->redMask;
	    u.vp->greenMask = pVisual->greenMask;
	    u.vp->blueMask = pVisual->blueMask;
	    u.vp++;
	}
    }
    bcopy((char *)pRenderModels, u.card8p, nBytesAllowedDepths);
    u.card8p += nBytesAllowedDepths;

    /*
     * Copy the VideoGeometry (including the placements) for each vin and vout.
     */
    for (k=0; k<2; k++) {
	type = (k==0) ? Input: Output;
	for (pvElement = pvElements; pvElement; pvElement = pvElement->next) {
	    if (pvElement->type == type) {
		pattr = pvElement->pvio->pattr;
		bcopy((char *)&pattr->xvg, u.card8p, sz_xVideoGeometry);
		u.vgp++;
		nBytesPlacement = pattr->xvg.nPlacement * sz_xPlacement;
		bcopy((char *)pattr->pPlacements, u.card8p, nBytesPlacement);
		u.card8p += nBytesPlacement;
	    }
	}
    }

    /*
     * For each vin and vout, the Model row is the same length: nRenderModels.
     * Produce Model Rows one at a time by examining the pAllowedDepths list
     * of each vin/vout and the rendermodel list: pRenderModels.
     */
    for (k=0; k<2; k++) {
	type = (k==0) ? Input: Output;
	for (pvElement = pvElements; pvElement; pvElement = pvElement->next) {
	    if (pvElement->type == type) {
		pvio = pvElement->pvio;
		MakeModelRow(pvio, (char *)u.card8p,
			     pRenderModels, nRenderModels);
		u.card8p += nRenderModels;
	    }
	}
    }

    /*
     * Copy the in- and out- port list lengths
     */
    nPorts = 0;	/* count these for SQueryVideoReply() */
    for (k=0; k<2; k++) {
	type = (k==0) ? Input: Output;
	for (pvElement = pvElements; pvElement; pvElement = pvElement->next) {
	    if (pvElement->type == type)
		nPorts += (*u.card8p++ = pvElement->pvio->pPortList->nPorts);
	}
    }

    /*
     * provide the padding... this will long-word align it.
     */
    u.card8p += sz_CARD32 - ((u.card8p - (CARD8 *)preplyData) & 0x3);

    /*
     * Copy the in- and out- ports
     */
    for (k=0; k<2; k++) {
	type = (k==0) ? Input: Output;
	for (pvElement = pvElements; pvElement; pvElement = pvElement->next) {
	    if (pvElement->type == type) {
		ppl = pvElement->pvio->pPortList;
		nBytesPorts = ppl->nPorts << 2;
		bcopy((char *)ppl->ports, u.card8p, nBytesPorts);
		u.card8p += nBytesPorts;
	    }
	}
    }

    /*
     * copy the clip size for each video input
     */
    for (pvElement = pvElements; pvElement; pvElement = pvElement->next) {
	if (pvElement->type == Input) {
	    bcopy((char *)pvElement->pvio->pClipSize, u.card8p, sz_xRectangle);
	    u.card8p += sz_xRectangle;
	}
    }

    /*
     * Copy the in- and out- ability
     */
    for (k=0; k<2; k++) {
	type = (k==0) ? Input: Output;
	for (pvElement = pvElements; pvElement; pvElement = pvElement->next) {
	    if (pvElement->type == type) {
		pAbility = pvElement->pvio->pAbility;
		bcopy((char *)&pAbility->xva, u.card8p, sz_xVideoAbility);
		u.vap++;
		nBytesAdjustments = pAbility->nAdjustments * sz_xFractionRange;
		bcopy((char *)pAbility->pAdjustments,
		      u.card8p, nBytesAdjustments);
		u.card8p += nBytesAdjustments;
	    }
	}
    }

    /*
     * Done.  Send the reply.
     */
    rep.length += nBytesData >> 2;
    if (client->swapped) {
        SQueryVideoReply(client, nBytesData, &rep, preplyData, nPorts);
    } else {
	(void) WriteToClient(client, sizeof (xQueryVideoReply), (char *)&rep);
	(void) WriteToClient(client, nBytesData, (char *)preplyData);
    }

    xfree(pRenderModels);
    DEALLOCATE_LOCAL(preplyData);

    return 0;
}

/*
 * See if fields of 2 rendermodels are all equal
 */
#define RENDER_MATCH(pxrm1, pxrm2) \
	(pxrm1->depth == pxrm2->depth &&	\
	 pxrm1->opaque == pxrm2->opaque &&	\
	 pxrm1->visualid == pxrm2->visualid &&	\
	 pxrm1->redMask == pxrm2->redMask &&	\
	 pxrm1->greenMask == pxrm2->greenMask && \
	 pxrm1->blueMask == pxrm2->blueMask)
/*
 * Choose a power of 2 for ALLOC_INCREMENT and set ALLOC_MOD_MSK to 
 * ALLOC_INCREMENT - 1
 */
#define ALLOC_INCREMENT	32
#define ALLOC_MOD_MSK	31
/*
 *    NAME
 *        GetRenderModels - return all video in and out unique render models
 *
 *    SYNOPSIS
 */
static xRenderModel *
GetRenderModels(pVideoInfo, nRenderModels)
    VideoScreenRec *pVideoInfo;		/* in: screen's video data */
    int *nRenderModels;			/* out: number of models returned */
/*
 *    DESCRIPTION
 *        Format and send the request.
 *
 *    RETURNS
 *        An array of all the unique RenderModels needed for all the VideoOuts
 *        and VideoIns on a VideoScreenRec.  This array can be copied directly
 *        onto the protocol bytestream.  The protocol says this list is in no
 *        particular order.  It is legal for this list to be empty (no video
 *        input or output).
 */
{
    xRenderModel *pRenderModels = (xRenderModel *) NULL;
    xRenderModel *pRenderModel;
    unsigned int modelIndex = 0;
    xRenderModel *pAllowedDepth;
    int found;
    int i,j;
    int size;
    VioDataPtr pvio;
    VideoElementPtr pvElement;

    /*
     * Make sure that every VideoVisual for every vin and every vout is
     * represented on the RenderModel list
     */
    for (pvElement = pVideoInfo->vElements;
	 pvElement;
	 pvElement = pvElement->next) {
	if (pvElement->type == Input || pvElement->type == Output) {
	    pvio = pvElement->pvio;
	    for (i=0; i < pvio->nAllowedDepths; i++) {
		/*
		 * See if this VideoVisual is already on the RenderModel list..
		 */
		pAllowedDepth = &pvio->pAllowedDepths[i].xrm;
		for (j=0, found=FALSE; j < modelIndex; j++) {
		    pRenderModel = &pRenderModels[j];
		    if (RENDER_MATCH(pAllowedDepth,pRenderModel)) {
			found = TRUE;
			break;
		    }
		}
		/*
		 * If this VideoVisual is not already on the RenderModel list,
		 * add it.
		 */
		if (!found) {
		    if (!(modelIndex & ALLOC_MOD_MSK)) {
			size = (ALLOC_INCREMENT + modelIndex) *
				sizeof(xRenderModel);
			pRenderModels = (xRenderModel *)
					 xrealloc(pRenderModels,size);
			if (!pRenderModels) {
			    *nRenderModels = 0;
			    return (xRenderModel *)NULL;
			}
		    }
		    pRenderModel = &pRenderModels[modelIndex++];
		    *pRenderModel = *pAllowedDepth;
		}
	    }
	}
    }
    *nRenderModels = modelIndex;
    return pRenderModels;
}

/*
 *    NAME
 *        MakeModelRow - construct a row for models
 *
 *    SYNOPSIS
 */
MakeModelRow(pvio, pModelRow, pRenderModels, nRenderModels)
    VioDataPtr pvio;		/* in: the vio structure */
    char *pModelRow;		/* out: one row of protocol data */
    xRenderModel *pRenderModels;/* in: list of xRenderModel's for the screen */
    int nRenderModels;		/* in: length of list */
/*
 *    DESCRIPTION
 *        For each element in the pRenderModels array, try to find an exact
 *        match on the pAllowedDepths array of the 'pvio' structure.  If a
 *        match is found copy the model field to the corresponding entry in the
 *        pModelRow array.  Otherwise set the entry in the pModelRow array to 0
 *        (meaning this vin/vout is not interested in the RenderModel entry).
 *
 *    RETURNS
 *        void
 */
{
    xRenderModel *pxrm;
    xRenderModel *pAllowedDepth;
    int found;
    int i,j;

    for (i=0; i<nRenderModels; i++) {
	pxrm = &pRenderModels[i];
	for (j=0, found=FALSE; j < pvio->nAllowedDepths; j++) {
	    pAllowedDepth = &pvio->pAllowedDepths[j].xrm;
	    if (RENDER_MATCH(pAllowedDepth,pxrm)) {
		found = TRUE;
		break;
	    }
	}
	if (found)
	    pModelRow[i] = pvio->pAllowedDepths[j].model;
	else
	    pModelRow[i] = 0;
    }
}
