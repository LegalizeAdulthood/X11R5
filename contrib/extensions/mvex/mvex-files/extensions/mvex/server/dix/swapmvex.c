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
static char *rcsid=  "$Header: /n/lacey/usr/local/src/video/MVEX/server/dix/RCS/swapmvex.c,v 2.2 1991/09/26 04:58:04 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#include "X.h"
#define NEED_EVENTS
#include "Xproto.h"
#include "misc.h"
#include "dixstruct.h"
#include "MVEX.h"
#include "MVEXproto.h"
#include "videostr.h"

void
SVideoViolationEvent(from, to)
    xVideoViolationEvent *from, *to;
{
    to->type = from->type;
    cpswaps (from->sequenceNumber, to->sequenceNumber);
    cpswapl (from->id, to->id);
    cpswapl (from->vid, to->vid);
    cpswapl (from->time, to->time);
    cpswaps (from->actionMask, to->actionMask);
    to->state = from->state;
}

void
SVideoSetupEvent(from, to)
    xVideoSetupEvent *from, *to;
{
    to->type = from->type;
    cpswaps (from->sequenceNumber, to->sequenceNumber);
    cpswapl (from->vid, to->vid);
    cpswapl (from->port, to->port);
    cpswapl (from->time, to->time);
    to->framesAvailable = from->framesAvailable;
    to->timecodesAvailable = from->timecodesAvailable;
    to->frameAccurate = from->frameAccurate;
}

void
SVideoSyncEvent(from, to)
    xVideoSyncEvent *from, *to;
{
    to->type = from->type;
    cpswaps (from->sequenceNumber, to->sequenceNumber);
    cpswapl (from->id, to->id);
    cpswapl (from->vid, to->vid);
    cpswapl (from->time, to->time);
    to->state = from->state;
}

void
SVideoOverrideEvent(from, to)
    xVideoOverrideEvent *from, *to;
{
    to->type = from->type;
    cpswaps (from->sequenceNumber, to->sequenceNumber);
    cpswapl (from->id, to->id);
    cpswapl (from->vid, to->vid);
    cpswapl (from->time, to->time);
    to->state = from->state;
}

void
SVideoRequestEvent(from, to)
    xVideoRequestEvent *from, *to;
{
    to->type = from->type;
    cpswaps (from->sequenceNumber, to->sequenceNumber);
    cpswapl (from->src, to->src);
    cpswapl (from->dst, to->dst);
    cpswapl (from->time, to->time);
    to->request = from->request;
    to->redirect = from->redirect;
}

#ifdef R3
/* Macros needed for byte-swapping, copied from swapreq.c.  Really
   should be in a header file somewhere. */

#define LengthRestB(stuff) \
    (((unsigned long)stuff->length << 2) - sizeof(*stuff))

#define LengthRestL(stuff) \
    ((unsigned long)stuff->length - (sizeof(*stuff) >> 2))

#define SwapRestL(stuff) \
    SwapLongs((long *)(stuff + 1), LengthRestL(stuff))
#endif R3


int
SProcQuerySetup (client)
    register ClientPtr	client;
{
    register char   n;
    REQUEST (xQuerySetupReq);

    swaps (&stuff->length, n);
    swapl (&stuff->id, n);
    swapl (&stuff->atom, n);
    return ProcQuerySetup (client);
}

int
SProcQueryVideo (client)
    register ClientPtr	client;
{
    register char   n;
    REQUEST (xQueryVideoReq);

    swaps (&stuff->length, n);
    swapl (&stuff->window, n);
    return ProcQueryVideo (client);
}

int
SProcCreateVideo (client)
    register ClientPtr	client;
{
    register char   n;
    REQUEST (xCreateVideoReq);

    swaps (&stuff->length, n);
    swapl (&stuff->id, n);
    swapl (&stuff->reference, n);
    return ProcCreateVideo (client);
}

int
SProcRenderVideo (client)
    register ClientPtr	client;
{
    register char   n;
    REQUEST (xRenderVideoReq);

    swaps (&stuff->length, n);
    swapl (&stuff->src, n);
    swapl (&stuff->inport, n);
    swapl (&stuff->dest, n);
    swaps (&stuff->srcX, n);
    swaps (&stuff->srcY, n);
    swaps (&stuff->dstX, n);
    swaps (&stuff->dstY, n);
    swaps (&stuff->srcWidth, n);
    swaps (&stuff->srcHeight, n);
    swaps (&stuff->dstWidth, n);
    swaps (&stuff->dstHeight, n);
    swapl (&stuff->gc, n);
    swapl (&stuff->valueMask, n);
    return ProcRenderVideo (client);
}

int
SProcCaptureGraphics (client)
    register ClientPtr	client;
{
    register char   n;
    REQUEST (xCaptureGraphicsReq);

    swaps (&stuff->length, n);
    swapl (&stuff->src, n);
    swapl (&stuff->outport, n);
    swapl (&stuff->dest, n);
    swapl (&stuff->cmap, n);
    swaps (&stuff->srcX, n);
    swaps (&stuff->srcY, n);
    swaps (&stuff->dstX, n);
    swaps (&stuff->dstY, n);
    swaps (&stuff->srcWidth, n);
    swaps (&stuff->srcHeight, n);
    swaps (&stuff->dstWidth, n);
    swaps (&stuff->dstHeight, n);
    swapl (&stuff->valueMask, n);
    return ProcCaptureGraphics (client);
}

int
SProcStopVideo (client)
    register ClientPtr	client;
{
    register char   n;
    REQUEST (xStopVideoReq);

    swaps (&stuff->length, n);
    SwapRestL(stuff);
    return ProcStopVideo (client);
}

int
SProcChangeOwnership (client)
    register ClientPtr	client;
{
    register char   n;
    REQUEST (xChangeOwnershipReq);

    swaps (&stuff->length, n);
    SwapRestL(stuff);
    return ProcChangeOwnership (client);
}

int
SProcSelectVideoEvents (client)
    register ClientPtr	client;
{
    register char   n;
    REQUEST (xSelectVideoEventsReq);

    swaps (&stuff->length, n);
    swapl (&stuff->id, n);
    swapl (&stuff->mask, n);
    return ProcSelectVideoEvents (client);
}

void
SwapFraction(pFraction)
    xFraction  *pFraction;
{
    SwapLongs((long *)(pFraction), sizeof(xFraction) >> 2);
}

void
SwapFractionRange(pFractionRange)
    xFractionRange  *pFractionRange;
{
    /*
     * This is kinda gross.  An xFractionRange is a bunch of longs
     * followed by 2 CARD8 type fields.
     */
    SwapLongs((long *)(pFractionRange), (sz_xFractionRange - 4) >> 2);
}

void
SwapRectangleRange(pRectangleRange)
    xRectangleRange  *pRectangleRange;
{
    /*
     * This is kinda gross.  An xRectangleRange is a bunch of shorts
     * followed by a CARD8 type field.
     */
    SwapShorts((short *)pRectangleRange, (sz_xRectangleRange - 4) >> 1);
}

void
SwapDepths(ppDepth, nDepths)
    xDepth **ppDepth;
    int nDepths;
{
    register int n;
    int i, k;
    xVisualType *pVis;

    for (i=0; i<nDepths; i++) {
	pVis = (xVisualType *) (*ppDepth + 1);
	for(k = 0; k < (*ppDepth)->nVisuals; k++){
	    swapl(pVis->visualID, n);
	    swaps(pVis->colormapEntries, n);
	    swapl(pVis->redMask, n);
	    swapl(pVis->greenMask, n);
	    swapl(pVis->blueMask, n);
	    pVis++;
	}
	swapl((*ppDepth)->nVisuals, n)
	*ppDepth = (xDepth *)pVis;
    }
}

void
SwapRenderModels(ppRenderModel, nAllowedDepths)
    xRenderModel **ppRenderModel;
    int nAllowedDepths;
{
    register int n;
    int i;

    for (i=0; i<nAllowedDepths; i++) {
	swapl((*ppRenderModel)->visualid, n);
	swapl((*ppRenderModel)->redMask, n);
	swapl((*ppRenderModel)->greenMask, n);
	swapl((*ppRenderModel)->blueMask, n);
	(*ppRenderModel)++;
    }
}

void
SwapVideoAbilities(ppVideoAbility, nAble)
    xVideoAbility **ppVideoAbility;
    int nAble;
{
    xVideoAbility *pVideoAbility = *ppVideoAbility;
    xFractionRange *pFRange;
    register int n;
    int i, k;

    for (i=0; i < nAble; i++) {
	pFRange = (xFractionRange *) (pVideoAbility + 1);

	k =   pVideoAbility->nSaturation
	    + pVideoAbility->nContrast
	    + pVideoAbility->nHue
	    + pVideoAbility->nBright;
	while (--k >= 0)
	    SwapFractionRange(pFRange++);
	SwapFraction(&(pVideoAbility->normalSaturation));
	SwapFraction(&(pVideoAbility->normalContrast));
	SwapFraction(&(pVideoAbility->normalHue));
	SwapFraction(&(pVideoAbility->normalBright));
	swaps(&pVideoAbility->nSaturation, n);
	swaps(&pVideoAbility->nContrast, n);
	swaps(&pVideoAbility->nHue, n);
	swaps(&pVideoAbility->nBright, n);
	pVideoAbility = (xVideoAbility *)pFRange;
    }
    *ppVideoAbility = pVideoAbility;
}

void
SwapVideoGeometries(ppVideoGeometry, nGeom)
    xVideoGeometry **ppVideoGeometry;
    int nGeom;
{
    xVideoGeometry *pVideoGeometry = *ppVideoGeometry;
    xPlacement *pPlacement;
    register int n;
    int i, k;

    for (i=0; i < nGeom; i++) {
	pPlacement = (xPlacement *) (pVideoGeometry + 1);
	for (k=0; k < pVideoGeometry->nPlacement; k++) {
	    SwapFraction(&(pPlacement->frameRate));
	    SwapRectangleRange(&(pPlacement->src));
	    SwapRectangleRange(&(pPlacement->dest));
	    SwapFractionRange(&(pPlacement->xScale));
	    SwapFractionRange(&(pPlacement->yScale));
	    pPlacement++;
	}
	SwapFraction(&(pVideoGeometry->frameRate));
	SwapFraction(&(pVideoGeometry->fieldRate));
	swaps(&pVideoGeometry->width, n);
	swaps(&pVideoGeometry->height, n);
	swaps(&pVideoGeometry->concurrentUse, n);
	swaps(&pVideoGeometry->prioritySteps, n);
	swapl(pVideoGeometry->referenceId, n);
	swapl(pVideoGeometry->nPlacement, n);
	pVideoGeometry = (xVideoGeometry *)pPlacement;
    }
    *ppVideoGeometry = pVideoGeometry;
}

void
SQueryVideoReply(pClient, nBytesData, pRep, preplyData, nPorts)
    ClientPtr pClient;
    int nBytesData;			/* Size of Reply data */
    xQueryVideoReply *pRep;		/* Reply structure */
    pointer preplyData;			/* Reply data */
    int nPorts;				/* as a helper, total ports */
{
    register int n;
    pointer pRepT;
    int nDepths, nAllowedDepths, nVin, nVout;

    nDepths = pRep->nDepths;
    nAllowedDepths = pRep->nAllowedDepths;
    nVin = pRep->nVin;
    nVout = pRep->nVout;

    /*
     * Swap the reply data structure in place and then write it out.
     * Routines which swap data structures also move the pRepT pointer.
     */

    pRepT = preplyData;

    SwapDepths((xDepth **)&pRepT, nDepths);
    SwapRenderModels((xRenderModel **)&pRepT, nAllowedDepths);
    SwapVideoGeometries((xVideoGeometry **)&pRepT, nVin);
    SwapVideoGeometries((xVideoGeometry **)&pRepT, nVout);
    /*
     * Skip over VideoInput Models, VideoOutput Models
     */
    pRepT += (nVin + nVout) * nAllowedDepths;

    /*
     * Skip over in- and out- port lists, then the padding
     */
    pRepT += (nVin + nVout);
    pRepT += ((4 - ((pRepT - preplyData) & 0x3)) & 0x3);

    /*
     * swap the in- and out-ports
     */
    SwapLongs((long *)pRepT, nPorts);
    pRepT += nPorts << 2;

    /*
     * swap the clip size list
     */
    SwapShorts((short *)pRepT, nVin * (sizeof(xRectangle) >> 1));
    pRepT += nVin * sizeof(xRectangle);

    /*
     * ... and swap the ability lists.
     */
    SwapVideoAbilities((xVideoAbility **)&pRepT, nVin + nVout);

    swaps(&pRep->sequenceNumber, n);
    swapl(&pRep->length, n);
    swapl(&pRep->time, n);

    (void)WriteToClient(pClient, sizeof (xQueryVideoReply), (char *) pRep);
    (void) WriteToClient(pClient, nBytesData, (char *)preplyData);
}

void
SQuerySetupReply(pClient, pRep)
    ClientPtr pClient;
    xQuerySetupReply *pRep;		/* Reply structure */
{
    int n;

    swaps(&pRep->sequenceNumber, n);
    swapl(&pRep->length, n);

    (void)WriteToClient(pClient, sizeof (xQuerySetupReply), (char *) pRep);
}
