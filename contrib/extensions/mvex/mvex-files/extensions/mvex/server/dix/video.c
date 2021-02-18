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
static char *rcsid=  "$Header: /n/lacey/usr/local/src/video/MVEX/server/dix/RCS/video.c,v 2.5 1991/09/26 21:25:19 toddb Exp $";
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

#define NextSetting(p)	((xSetting *)((int)((p)+1) + (((p)->length +3) & (~3))))
#define NBYTES_BASIC_REPLY	32

/*
 * To block a client but not locking up the server. This is best to be done by
 * multi-threading the server. With server as a single thread, it can be kludged
 * around.
 */
extern void vmiBlockCurrentRequest();
/*
 * Flag to indicate that client blocking is permitted. (set by dix layer)
 * Currently, if clients issues mutiple control settings in a single request
 * blocking is not permitted.
 */
Bool can_block;
ClientPtr current_client;
/*
 * Flag for blocking client. Set by the ddx device layer if the client
 * to be blocked because device is currently busy.
 */
Bool block_client;

extern int Ones();
XID clientErrorValue;   /* XXX this is a kludge */

static unsigned char VideoReqCode = 0;

#define LOOKUP_DRAWABLE(did, client)\
    ((client->lastDrawableID == did) ? \
     client->lastDrawable : (DrawablePtr)LookupDrawable(did, client))

/*ARGSUSED*/
static DeleteVideoDrawable(vDraw, id)
    VideoDrawPtr vDraw;
    XID id;
{
    VideoScreenPtr	vp;

    if (vDraw->renderActive || vDraw->captureActive) {
	vp = ScreenVideoInfo[ vDraw->pDraw->pScreen->myNum ];
	StopVideo (vp,
		   vDraw,
		   (VideoIO) 0, /* overriding video id (i.e. none) */
		   (MVEXStopRenderVideo|MVEXStopCaptureGraphics),
		   MVEXDrawableChange);
    }
    xfree(vDraw);
}

/*
 * StopVideo on all active render or captures by this video Resource
 */
static StopVideoOnVResource(vRes, fAll, vio, state)
    VideoResourcePtr vRes;
    Bool fAll;		/* If TRUE, then stop all active render/capture */
    VideoIO vio;	/* Stop all active render/capture on this VideoIO.
			 * can be invalid if fAll is TRUE */
    int state;		/* reason for the stop */
{
    DrawablePtr		pDraw;
    VideoScreenPtr	vp;
    int			i;
    VideoDrawPtr	vDraw;

    for (i=0; i < vRes->maxUse; i++) {
	if (pDraw = vRes->usage[i].pDraw) {
	    vp = ScreenVideoInfo[ pDraw->pScreen->myNum ];
	    if (vDraw = (VideoDrawPtr) LookupIDByType(pDraw->id, vp->vDrawType))
		if (fAll || (vDraw->videoIn == vio || vDraw->videoOut == vio))
		    StopRenderOrCapture(vp, vDraw, vRes, vio, 0, state);
	}
    }
}


/*ARGSUSED*/
static DeleteVideoResource(vRes, id)
    VideoResourcePtr vRes;
    XID id;
{
    /*
     * If there is an active Render or Capture using this VideoResource, Stop
     * the video
     */
    StopVideoOnVResource(vRes, TRUE, (VideoIO) 0, MVEXStopped);

    xfree(vRes->owners);
    xfree(vRes->usage);
    xfree(vRes);
}

/*ARGSUSED*/
static DeleteVideoId(vRes, id)
    VideoResourcePtr vRes;
    XID id;
{
    /*
     * If there is an active Render or Capture using this VideoIO, Stop
     * the video
     */
    StopVideoOnVResource(vRes, FALSE, id, MVEXStopped);
}

#define WIN_OWNER	0
#define CLIENT_OWNER	1
#define RES_OWNER(type,owner,id) \
	((type == WIN_OWNER && owner.window == id) || \
	(type == CLIENT_OWNER && owner.clientID == id))

/*
 * "value" is a pointer to an XID.  "id" is a window ID.
 * The "value" XID is used to find the VideoResource.
 */
static DeleteWinOwner(value, id)
    pointer value;
    XID id;
{
    DeleteWinOrClientOwner(value, id, WIN_OWNER);
}
/*
 * "value" is a pointer to an XID.  "id" is a Fake client ID.
 * The XID is used to find the VideoResource.
 */
static DeleteClientOwner(value, id)
    pointer value;
    XID id;
{
    DeleteWinOrClientOwner(value, id, CLIENT_OWNER);
}
/*
 * "value" is a pointer to an XID.  "id" is a window ID or a Fake client ID.
 * The "value" XID is used to find the VideoResource.
 * Delete all ownerships for this id for the VideoResource.
 * "DisOwn" any current uses which are owned by this "window/client"
 */
static DeleteWinOrClientOwner(value, id, resType)
    pointer value;
    XID id;
    int resType;
{
    VideoResourcePtr vRes;
    int i,k;

    vRes = (VideoResourcePtr) LookupIDByType((XID) *value,
					     ScreenVideoInfo[0]->vIdType);
    if (vRes) {
	for (i=0; i < vRes->maxUse; i++) {
	    if (RES_OWNER(resType,vRes->owners[i],id)) {
		for (k=0; k < vRes->maxUse; k++) {
		    if (vRes->usage[k].pClient == vRes->owners[i].pClient &&
			    vRes->usage[k].owned) {
			vRes->usage[i].owned = FALSE;
			break;
		    }
		}
		vRes->owners[i].pClient = (ClientPtr) NULL;
		vRes->owners[i].clientID = INVALID;
		vRes->owners[i].window = INVALID;
		vRes->owners[i].inUse = FALSE;
		vRes->ownerCount--;
	    }
	}
    }
    xfree(value);
}
#undef WIN_OWNER
#undef CLIENT_OWNER
#undef RES_OWNER

/****************
 *
 * MvexExtensionInit()
 *
 * Called from InitExtensions in main() or from QueryExtension() if the
 * extension is dynamically loaded.
 *
 ****************/
void
MvexExtensionInit()
{
    ExtensionEntry *extEntry, *AddExtension();
    int ProcVideoDispatch(), SProcVideoDispatch();
    void  VideoResetProc();
    int i;
    XID id;
    RESTYPE vDrawType;
    RESTYPE vResType;
    RESTYPE vIdType;
    RESTYPE vClientOwnerType;
    RESTYPE vWinOwnerType;
    RESTYPE resType;
    int nUse;
    VideoResourcePtr	vRes;
    screenIndexRec *vscreens;
    VideoElementPtr     pvElement;

    ScreenVideoInfo = (VideoScreenPtr *) 
	    xalloc (screenInfo.numScreens * sizeof (VideoScreenPtr));
    if (!ScreenVideoInfo) {
	return;
    }

    vDrawType = CreateNewResourceType(DeleteVideoDrawable);
    vResType = CreateNewResourceType(DeleteVideoResource);
    vIdType = CreateNewResourceType(DeleteVideoId);
    vClientOwnerType = CreateNewResourceType(DeleteClientOwner);
    vWinOwnerType = CreateNewResourceType(DeleteWinOwner);

    for (i=0; i<screenInfo.numScreens; i++) {
	if (!(ScreenVideoInfo[i] = CreateVideoScreenRec(screenInfo.screens[i])))
	    /* XXX Do something here someday */
	    return;

	if (ddxMvexInit(screenInfo.screens[i], ScreenVideoInfo[i]) == FALSE) {
	    /* XXX Do something here someday */
	}

	if (!ScreenVideoInfo[i])
	    continue;
	/*
	 * initialize
	 */
	ScreenVideoInfo[i]->activeVDraws.nEntries = 0;
	ScreenVideoInfo[i]->activeVDraws.maxEntries = 0;
	ScreenVideoInfo[i]->activeVDraws.vDrawArray = (VideoDrawPtr *) NULL;
	ScreenVideoInfo[i]->id = FakeClientID(0);
	ScreenVideoInfo[i]->vDrawType = vDrawType;
	ScreenVideoInfo[i]->vResType = vResType;
	ScreenVideoInfo[i]->vIdType = vIdType;
	ScreenVideoInfo[i]->vClientOwnerType = vClientOwnerType;
	ScreenVideoInfo[i]->vWinOwnerType = vWinOwnerType;
	ScreenVideoInfo[i]->lastVideoChangeTime = currentTime;

	ScreenVideoInfo[i]->pScreen = screenInfo.screens[i];

	/*
	 * all vectors in the VideoScreenPtr should have been set
	 * by mvex ddx (usually mi)
	 */
	ScreenVideoInfo[i]->CreateWindow = screenInfo.screens[i]->CreateWindow;
	ScreenVideoInfo[i]->CopyWindow = screenInfo.screens[i]->CopyWindow;
	ScreenVideoInfo[i]->RealizeWindow =
	    screenInfo.screens[i]->RealizeWindow;
	ScreenVideoInfo[i]->UnrealizeWindow =
	    screenInfo.screens[i]->UnrealizeWindow;
	ScreenVideoInfo[i]->ClipNotify = screenInfo.screens[i]->ClipNotify;

     /* n/a: screenInfo.screens[i]->CreateWindow = mvexCreateWindow; */
     /* n/a: screenInfo.screens[i]->CopyWindow = mvexCopyWindow; */
	screenInfo.screens[i]->RealizeWindow = mvexRealizeWindow;
	screenInfo.screens[i]->UnrealizeWindow = mvexUnrealizeWindow;
	screenInfo.screens[i]->ClipNotify = mvexClipNotify;

	/*
	 * Add reference resources to each of the video inputs, outputs
	 * and devices.
	 * There should be 1 and only 1 reference resource per ID.
	 * There are some complications:
	 *	A single VReference ID may be used on multiple screens, but
	 *		refers to one VideoInput or VideoOutput and should
	 *		have only one reference resource for the ID.
	 */
	for (pvElement = ScreenVideoInfo[i]->vElements;
	     pvElement;
	     pvElement = pvElement->next) {
	    resType = ScreenVideoInfo[0]->vResType;
	    nUse = pvElement->pvio->pattr->xvg.concurrentUse;
	    assert (nUse >= 1);

	    id = pvElement->id;
	    if (!(vRes = (VideoResourcePtr) LookupIDByType(id, resType))) {
		vRes = (VideoResourcePtr) xalloc(sizeof(VideoResourceRec));
		vRes->owners = (Ownership *) 
			xalloc(nUse * sizeof(Ownership));
		vRes->maxUse = nUse;
		vRes->usage = (ResourceUsage *) 
			xalloc(nUse *sizeof(ResourceUsage));
		vRes->vp = ScreenVideoInfo[i];
		InitUsage(vRes);
		InitOwnership(vRes);
		vRes->vElement = pvElement;
		vRes->screens = (screenIndexRec *) NULL;
		if (! AddResource(id, resType, (pointer)vRes)) {
		    /* XXX Do something here someday */
		    return;
		}
	    }
	    vscreens = (screenIndexRec *)xalloc(sizeof(screenIndexRec));
	    if (!vscreens)
		/* XXX Do something here someday */
		return;
	    vscreens->pScreen = screenInfo.screens[i];
	    vscreens->next = vRes->screens;
	    vRes->screens = vscreens;
	}

    }
    extEntry = AddExtension(MVEX_NAME,
			    VideoNumberEvents,
			    VideoNumberErrors,
			    ProcVideoDispatch,
			    SProcVideoDispatch,
			    VideoResetProc,
			    StandardMinorOpcode);
    if (extEntry)
    {
	VideoReqCode = (unsigned char)extEntry->base;
	VideoErrorBase = extEntry->errorBase;
	(void) InitVideoEvents(extEntry);
    }
}

/*ARGSUSED*/
void
VideoResetProc (extEntry)
ExtensionEntry	*extEntry;
{

    int i;
    VideoScreenRec *pVideoScreen;

    /*
     * XXX
     * Is screenInfo still valid at this point?
     */
    VideoReqCode = 0;
    for (i=0; i<screenInfo.numScreens; i++) {
	pVideoScreen = ScreenVideoInfo[i];
	(*pVideoScreen->Reset)(pVideoScreen);
	if (pVideoScreen->activeVDraws.vDrawArray)
	    xfree(pVideoScreen->activeVDraws.vDrawArray);
	xfree(pVideoScreen);
    }
    xfree(ScreenVideoInfo);
}


ProcQuerySetup (client)
    register ClientPtr	client;
{
    REQUEST(xQuerySetupReq);
    xQuerySetupReply rep;
    int i;
    VideoResourcePtr	vRes;
    VioPortList *pList;

    REQUEST_SIZE_MATCH (xQuerySetupReq);

    vRes = (VideoResourcePtr) LookupIDByType(stuff->id,
			ScreenVideoInfo[0]->vIdType);
    if (vRes == NULL)
	return (BAD_VIDEO);

    pList = vRes->vElement->pvio->pPortList;
    for (i = pList->nPorts - 1; i >= 0; i--)
	if (pList->ports[ i ] == stuff->atom)
	    break;
    if (i < 0)
	return (BadMatch);

    /*
     * ask the ddx layer to update the Given video setup.
     */
    (*vRes->vElement->pvio->QuerySetup)(vRes, i);

    rep.type = X_Reply;
    rep.sequenceNumber = client->sequence;
    if (sz_xQuerySetupReply > 32)
        rep.length = (sz_xQuerySetupReply - 32) >> 2;
    else
        rep.length = 0;
    rep.framesAvailable = pList->setups[ i ].framesAvailable;
    rep.timecodesAvailable = pList->setups[ i ].timecodesAvailable;
    rep.frameAccurate = pList->setups[ i ].frameAccurate;

    if (client->swapped)
        SQuerySetupReply(client, &rep);
    else
        (void) WriteToClient(client, sz_xQuerySetupReply, (char *)&rep);

    return (client->noClientException);
}

ProcQueryVideo (client)
    register ClientPtr	client;
{
    REQUEST(xQueryVideoReq);
    WindowPtr	pWin;
    int retval;
    VideoScreenRec *pVideoScreen;

    REQUEST_SIZE_MATCH (xQueryVideoReq);
    pWin = LookupWindow (stuff->window, client);
    if (!pWin) {
	client->errorValue = stuff->window;
	return BadWindow;
    }
    /*
     * No video hardware for this screen
     */
    pVideoScreen = ScreenVideoInfo[pWin->drawable.pScreen->myNum];
    if (!SCREEN_HAS_VIDEO(pVideoScreen)) {
	client->errorValue = stuff->window;
	return BadMatch;
    }

    /*
     * QueryVideo will output the reply
     */

    if (retval = QueryVideo(pWin, client)) {
	if (client->noClientException != Success) {
	    return client->noClientException;
	} else {
	    client->errorValue = clientErrorValue;
	    return retval;
	}
    }

    return (client->noClientException);
}

int
ProcRenderVideo (client)
    register ClientPtr client;
{
    VideoActionRec	render;
    int			result;
    DrawablePtr		pDest;
    GC			*pGC;
    REQUEST(xRenderVideoReq);

    REQUEST_AT_LEAST_SIZE (xRenderVideoReq);

    VALIDATE_DRAWABLE_AND_GC(stuff->dest, pDest, pGC, client);
    if (stuff->srcWidth == 0 || stuff->srcHeight == 0
     || stuff->dstWidth == 0 || stuff->dstHeight == 0) {
	    client->errorValue = 0;
	    return BadValue;
    }

    render.request = MVEXRender;
    render.videoIO = stuff->src;
    render.dix.srcX = stuff->srcX;
    render.dix.srcY = stuff->srcY;
    render.dix.dstX = stuff->dstX;
    render.dix.dstY = stuff->dstY;
    render.dix.srcWidth = stuff->srcWidth;
    render.dix.srcHeight = stuff->srcHeight;
    render.dix.dstWidth = stuff->dstWidth;
    render.dix.dstHeight = stuff->dstHeight;
    render.drawable = pDest;
    render.port = stuff->inport;	/* XXX not an index yet... */
    render.running = FALSE;

    result = RenderOrCapture(client,
		 &render,
		 pGC,
		 stuff->valueMask,
		 (XID *) &stuff[1],
		 (int)stuff->length - (sizeof(xRenderVideoReq)>>2));

    if (client->noClientException != Success)
        return(client->noClientException);
    else
        return(result);
}


int
ProcCaptureGraphics (client)
    register ClientPtr client;
{
    VideoActionRec	capture;
    int			result;
    DrawablePtr		pSrc;
    Colormap		cmap;
    ColormapPtr		pcmp;
    REQUEST(xCaptureGraphicsReq);

    REQUEST_AT_LEAST_SIZE (xCaptureGraphicsReq);

    if (!(pSrc = LOOKUP_DRAWABLE(stuff->src, client)))
    {
        client->errorValue = stuff->src;
        return BadDrawable;
    }
    if (pSrc->type == DRAWABLE_WINDOW) {
	if (stuff->cmap != None) {
	    client->errorValue = stuff->cmap;
	    return BadMatch;
	}
	cmap = wColormap(((WindowPtr)pSrc));
	pcmp = (ColormapPtr  )LookupIDByType(cmap, RT_COLORMAP);
	if (!pcmp) {
	    client->errorValue = stuff->cmap;
	    return BadColor;
	}
    } else {
	if (stuff->cmap != None) {
	    pcmp = (ColormapPtr  )LookupIDByType(stuff->cmap, RT_COLORMAP);
	    if (!pcmp) {
		client->errorValue = stuff->cmap;
		return BadColor;
	    }
	} else {
	    pcmp = (ColormapPtr  ) NULL;
	}
    }

    if (stuff->subWindowMode != ClipByChildren &&
	stuff->subWindowMode != IncludeInferiors) {
	    client->errorValue = stuff->subWindowMode;
	    return BadValue;
    }
    if (stuff->srcWidth == 0 || stuff->srcHeight == 0
     || stuff->dstWidth == 0 || stuff->dstHeight == 0) {
	    client->errorValue = 0;
	    return BadValue;
    }

    capture.request = MVEXCapture;
    capture.videoIO = stuff->dest;
    capture.dix.srcX = stuff->srcX;
    capture.dix.srcY = stuff->srcY;
    capture.dix.dstX = stuff->dstX;
    capture.dix.dstY = stuff->dstY;
    capture.dix.srcWidth = stuff->srcWidth;
    capture.dix.srcHeight = stuff->srcHeight;
    capture.dix.dstWidth = stuff->dstWidth;
    capture.dix.dstHeight = stuff->dstHeight;
    capture.subWindowMode = stuff->subWindowMode;
    capture.drawable = pSrc;
    capture.pcmap = pcmp;
    capture.port = stuff->outport;	/* XXX not an index yet... */
    capture.running = FALSE;

    result = RenderOrCapture(client,
		 &capture,
		 (GC *) NULL,
		 stuff->valueMask,
		 (XID *) &stuff[1],
		 (int)stuff->length - (sizeof(xCaptureGraphicsReq) >> 2));

    if (client->noClientException != Success)
        return(client->noClientException);
    else
        return(result);

}


int
ProcStopVideo (client)
    register ClientPtr client;
{
    REQUEST(xStopVideoReq);
    int			nDrawables;
    DrawablePtr		pDraw;
    Drawable		*drawables;
    VideoScreenPtr	vp;
    int			error = Success;
    int			errorValue;
    int			i;
    VideoDrawPtr	vDraw;
    unsigned char	actionMask;

    REQUEST_AT_LEAST_SIZE (xStopVideoReq);

    actionMask = MVEXStopRenderVideo | MVEXStopCaptureGraphics;
    if ((stuff->action & ~actionMask) || stuff->action == 0) {
	    client->errorValue = stuff->action;
	    return BadValue;
    }

    nDrawables = stuff->length -  (sizeof(xStopVideoReq) >> 2);
    drawables = (Drawable *) (stuff + 1);
    for (i=0; i<nDrawables; i++) {
	if (!(pDraw = LOOKUP_DRAWABLE(drawables[i], client))) {
	    error = BadDrawable;
	    errorValue = drawables[i];
	}
	else {
	    vp = ScreenVideoInfo[ pDraw->pScreen->myNum ];
	    if (vDraw = GetVideoDrawable(vp, pDraw))
		StopVideo(vp, vDraw,
		    (VideoIO) 0, /* overriding video id (i.e. none) */
		    (int) stuff->action, MVEXStopped);
	}
    }
    if (error != Success) {
	client->errorValue = errorValue;
	return error;
    }

    return (client->noClientException);
}

int
ProcCreateVideo (client)
    register ClientPtr client;
{
    VideoResourcePtr	vRes;
    REQUEST(xCreateVideoReq);

    REQUEST_SIZE_MATCH(xCreateVideoReq);
    /* XXX
     * Is LEGAL_NEW_RESOURCE the proper macro to call for a MVEX resource ID??
     */
    LEGAL_NEW_RESOURCE(stuff->id, client);

    /*
     * This is a bit of a hack, because we need the resource type,
     * but they are kept with the video info which is stored by screen,
     * but we don't yet know what screen we are talking about, so
     * we use the 0th (all the resource types are the same for all screens).
     */
    vRes = (VideoResourcePtr) LookupIDByType(stuff->reference,
			ScreenVideoInfo[0]->vResType);
    if (vRes == NULL) {
        client->errorValue = stuff->reference;
        return(BAD_VIDEO);
    }

    AddResource(stuff->id, ScreenVideoInfo[0]->vIdType, (pointer)vRes);

    /*
     * CreateVideo
     * return result
     */
    return (client->noClientException);
}

int
ProcChangeOwnership (client)
    register ClientPtr client;
{
    REQUEST(xChangeOwnershipReq);
    int			nOwners;
    VideoResourcePtr	vRes;
    int			i;
    xOwner		*pOwners;
    int			error = Success;

    REQUEST_AT_LEAST_SIZE (xChangeOwnershipReq);

    nOwners = (stuff->length - (sizeof(xChangeOwnershipReq) >> 2)) >> 1;

    pOwners = (xOwner *)(stuff + 1);
    for (i=0; i < nOwners; i++) {
	if (!LookupWindow(pOwners[i].window, client)) {
	    /*
	     * If we find errors, just remember the last one and process
	     * the rest of the owner list.
	     */
	    error = BadWindow;
	    break;
	}
#ifdef XXXfinished
	XXX If client no longer exists
	    break;
#endif /* XXXfinished */
	vRes = (VideoResourcePtr) LookupIDByType(pOwners[i].vid,
			    ScreenVideoInfo[0]->vIdType);
	if (vRes == NULL) {
	    error = BAD_VIDEO;
	    break;
	}
    }

    return (error);
}

int
ProcSelectVideoEvents (client)
    register ClientPtr client;
{
    int retval;
    REQUEST(xSelectVideoEventsReq);

    REQUEST_SIZE_MATCH(xSelectVideoEventsReq);


    if (stuff->mask & ~AllMVEXEventMask) {
	client->errorValue = stuff->mask;
	return BadValue;
    }

    if ((retval = VideoSelectInput( stuff->id, client, stuff->mask)) != Success)
	return retval;
    else
	return (client->noClientException);
}

InitOwnership(vRes)
    VideoResourcePtr vRes;
{
    int i;

    vRes->ownerCount = 0;
    for (i=0; i < vRes->maxUse; i++) {
	vRes->owners[i].pClient = (ClientPtr) NULL;
	vRes->owners[i].clientID = INVALID;
	vRes->owners[i].window = INVALID;
	vRes->owners[i].inUse = FALSE;
    }
}

InitUsage(vRes)
    VideoResourcePtr vRes;
{
    int i;

    vRes->useCount = 0;
    for (i=0; i < vRes->maxUse; i++) {
	vRes->usage[i].pClient = (ClientPtr) NULL;
	vRes->usage[i].pDraw = (DrawablePtr) NULL;
	vRes->usage[i].owned = FALSE;
    }
}


int
ProcVideoDispatch (client)
    register ClientPtr	client;
{
    REQUEST(xReq);
    switch (stuff->data) {
    case X_QueryVideo:        return ProcQueryVideo (client);
    case X_RenderVideo:       return ProcRenderVideo (client);
    case X_CaptureGraphics:   return ProcCaptureGraphics (client);
    case X_StopVideo:         return ProcStopVideo (client);
    case X_CreateVideo:       return ProcCreateVideo (client);
    case X_ChangeOwnership:   return ProcChangeOwnership (client);
    case X_SelectVideoEvents: return ProcSelectVideoEvents (client);
    case X_QuerySetup:        return ProcQuerySetup (client);
    default:
	SendErrorToClient (client, VideoReqCode, stuff->data, (XID)0,
			   BadRequest);
	return BadRequest;
    }
}

int
SProcVideoDispatch (client)
    register ClientPtr	client;
{
    REQUEST(xReq);
    switch (stuff->data) {
    case X_QueryVideo:        return SProcQueryVideo (client);
    case X_CreateVideo:       return SProcCreateVideo (client);
    case X_RenderVideo:       return SProcRenderVideo (client);
    case X_CaptureGraphics:   return SProcCaptureGraphics (client);
    case X_StopVideo:         return SProcStopVideo (client);
    case X_ChangeOwnership:   return SProcChangeOwnership (client);
    case X_SelectVideoEvents: return SProcSelectVideoEvents (client);
    case X_QuerySetup:        return SProcQuerySetup (client);
    default:
	SendErrorToClient (client, VideoReqCode, stuff->data, (XID)0,
			   BadRequest);
	return BadRequest;
    }
}
