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
static char *rcsid=  "$Header: /usr/local/src/x11r5.mvex/RCS/mvexevents.c,v 2.1 1991/09/12 23:21:23 toddb Exp $";
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
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "colormapst.h"
#include "extnsionst.h"
#include "dixstruct.h"
#include "resource.h"
#include "opaque.h"
#define _MVEX_SERVER_	/* don't want Xlib structures */ /* XXX */
#include "regionstr.h"
#include "gcstruct.h"

#include "MVEX.h"
#include "MVEXproto.h"
#include "videostr.h"

static int VideoFreeClient(), VideoFreeEvents();
static void SendVideoNotify();

static RESTYPE ClientType, EventType; /* resource types for event masks */
static int VideoEventBase = 0;

/*
 * each resource has a list of clients requesting
 * MVEX events.  Each client has a resource
 * for each resource it selects MVEX events for:
 * this resource is used to delete the VideoNotifyRec
 * entry from the per-resource queue.
 */

typedef struct _VideoEvent *VideoEventPtr;

typedef struct _VideoEvent {
    VideoEventPtr   next;
    ClientPtr	    client;
    XID		    resourceID;
    XID		    clientResource;
    unsigned long   events;
} VideoEventRec;


int
InitVideoEvents(extEntry)
    ExtensionEntry *extEntry;
{
    VideoEventBase = extEntry->eventBase;
    ClientType = CreateNewResourceType(VideoFreeClient);
    EventType = CreateNewResourceType(VideoFreeEvents);
    EventSwapVector[VideoEventBase + MVEXViolation] = SVideoViolationEvent;
    EventSwapVector[VideoEventBase + MVEXSync] = SVideoSyncEvent;
    EventSwapVector[VideoEventBase + MVEXSetup] = SVideoSetupEvent;
    EventSwapVector[VideoEventBase + MVEXOverride] = SVideoOverrideEvent;
    EventSwapVector[VideoEventBase + MVEXRequest] = SVideoRequestEvent;

    if (!ClientType || !EventType)
	return FALSE;
    else
	return TRUE;
}

/*
 * Called when a client dies.  This routine frees "data" (a VideoEvent struct
 * in the event list for the resource "id").
 */
/*ARGSUSED*/
static int
VideoFreeClient (data, id)
    pointer	    data;
    XID		    id;
{
    VideoEventPtr   pVideoEvent;
    VideoEventPtr   *pHead, pCur, pPrev;

    pVideoEvent = (VideoEventPtr) data;
    pHead = (VideoEventPtr *)LookupIDByType(pVideoEvent->resourceID, EventType);
    if (pHead) {
	pPrev = 0;
	for (pCur = *pHead; pCur && pCur != pVideoEvent; pCur=pCur->next)
	    pPrev = pCur;
	if (pPrev)
	    pPrev->next = pVideoEvent->next;
	else
	    *pHead = pVideoEvent->next;
    }
    xfree ((pointer) pVideoEvent);
}

/*
 * Called when resource, "id", goes away.  This routine frees each of the
 * VideoEvent structures which are attached to "id".
 */
/*ARGSUSED*/
static int
VideoFreeEvents (data, id)
    pointer	    data;
    XID		    id;
{
    VideoEventPtr   *pHead, pCur, pNext;

    pHead = (VideoEventPtr *) data;
    for (pCur = *pHead; pCur; pCur = pNext) {
	pNext = pCur->next;
	FreeResource (pCur->clientResource, ClientType);
	xfree ((pointer) pCur);
    }
    xfree ((pointer) pHead);
}

#define SCREEN_EVENTS	(MVEXRedirectMask)
#define DRAWABLE_EVENTS	(SCREEN_EVENTS|MVEXOverrideMask|MVEXViolationMask)
#define VID_EVENTS	(MVEXSyncMask)
int
VideoSelectInput (id, client, events)
    XID		    	id;
    ClientPtr		client;
    unsigned long  	events;
{
    DrawablePtr			pDraw;
    int				ret;
    XID				screenID;

    /*
     * There are 4 Event types.  Each has its own idiosyncracies.
     * MVEXRedirectMask:
     *		id is a drawable.  
     *		It selects a screen to receive VideoRequest events from.
     *		Only 1 client may select this event at a time, 
     *		else Access Error
     * MVEXSetupMask:
     *		id is a VideoIO.
     * MVEXSyncMask:
     *		id is a VideoIO.
     * MVEXOverrideMask:
     *		id is a drawable.
     * MVEXViolationMask:
     *		id is a drawable.
     */

    if (events & ~AllMVEXEventMask ||
	((events & DRAWABLE_EVENTS) && (events & VID_EVENTS))) {
	client->errorValue = events;
	return BadValue;
    }
    /*
     * First determine the type of the id.  Unfortunately, we can only do
     * this by doing a Lookup on each of the possible types.
     */
    if ( pDraw = (DrawablePtr) LookupDrawable(id, client)) {
	if (events & VID_EVENTS) {
	    client->errorValue = id;
	    return BAD_VIDEO;
	}
	screenID = ScreenVideoInfo[pDraw->pScreen->myNum]->id;
	/*
	 * SelectEvents for MVEXRedirectMask using 
	 * screen ID, later SelectEvents for other events (e.g. Override).
	 */
	ret = SelectEvents (screenID, client, (events & SCREEN_EVENTS));
	if (ret != Success)
	    return ret;
    }
    else if (LookupIDByType(id, ScreenVideoInfo[0]->vIdType)){
	if (events & DRAWABLE_EVENTS) {
		client->errorValue = id;
		return BadDrawable;
	}
    } else {
	client->errorValue = id;
	if (events & DRAWABLE_EVENTS)
	    return BadDrawable;
	else
	    return BAD_VIDEO;
    }

    ret = SelectEvents (id, client, (events & ~SCREEN_EVENTS));
    return ret;
} 

int
SelectEvents (id, client, events)
    XID		    	id;
    ClientPtr		client;
    unsigned long  	events;
{
    VideoEventPtr		pVideoEvent, pNewVideoEvent, *pHead;
    XID				clientResource;
    unsigned long  	eventMask;

    eventMask = AllMVEXEventMask;
    /*
     * There is a list of VideoEvents for each id.  There is one entry for
     * each client that has selected events for this id.
     */

    pHead = (VideoEventPtr *) LookupIDByType(id, EventType);
    /*
     * If this client is trying to set MVEXRedirectMask, see if another
     * client has already selected this.  If it has, report Access error.
     */
    if ((events & MVEXRedirectMask) && pHead) {
	for (pVideoEvent = *pHead; pVideoEvent; pVideoEvent = pVideoEvent->next)
	{
	    if ((pVideoEvent->events & MVEXRedirectMask) && 
		    (pVideoEvent->client != client)) {
		client->errorValue = id;
		return BadAccess;
	    }
	}
    }

    if (pHead) {

	/* check for existing entry. */
	pNewVideoEvent = 0;
	for (pVideoEvent = *pHead; pVideoEvent; pVideoEvent = pVideoEvent->next)
	{
	    if (pVideoEvent->client == client) {
		pVideoEvent->events = 
		      (pVideoEvent->events & ~eventMask) | (events & eventMask);
		/*
		 * If there are no longer any events selected, we can delete
		 * this entry
		 */
		if (!pVideoEvent->events) {
		    FreeResource (pVideoEvent->clientResource, EventType);
		    if (pNewVideoEvent)
			pNewVideoEvent->next = pVideoEvent->next;
		    else
			*pHead = pVideoEvent->next;
		    xfree (pVideoEvent);
		}
		return Success;
	    } else
		pNewVideoEvent = pVideoEvent;
	}
    }

    /*
     * If there was no existing event structure and events == 0, just
     * return now.
     */
    if (!events)
	return Success;

    /* build the entry */
    pNewVideoEvent = (VideoEventPtr)
			xalloc (sizeof (VideoEventRec));
    if (!pNewVideoEvent)
	return BadAlloc;
    pNewVideoEvent->next = 0;
    pNewVideoEvent->client = client;
    pNewVideoEvent->resourceID = id;
    pNewVideoEvent->events = (events & eventMask);
    /*
     * add a resource that will be deleted when
     * the client goes away
     */
    clientResource = FakeClientID (client->index);
    pNewVideoEvent->clientResource = clientResource;
    if (!AddResource (clientResource, ClientType, (pointer)pNewVideoEvent))
    {
	xfree (pNewVideoEvent);
	return BadAlloc;
    }
    /*
     * create a resource to contain a pointer to the list
     * of clients selecting input.  This must be indirect as
     * the list may be arbitrarily rearranged which cannot be
     * done through the resource database.
     */
    if (!pHead)
    {
	pHead = (VideoEventPtr *) xalloc (sizeof (VideoEventPtr));
	if (!pHead ||
	    !AddResource (id, EventType, (pointer)pHead))
	{
	    FreeResource (clientResource, ClientType);
	    xfree (pHead);
	    xfree (pNewVideoEvent);
	    return BadAlloc;
	}
	*pHead = 0;
    }
    pNewVideoEvent->next = *pHead;
    *pHead = pNewVideoEvent;

    return Success;
}

/*
 * deliver an event.  type can be any MVEX event.
 * XXX sequence number accuracy???
 */

static void
SendVideoNotify (id, pevent, type)
    XID		    	id;		/* Resource ID to report event on */
    xEvent		*pevent;	/* event to send */
    int			type;		/* event type */
{
    VideoEventPtr	*pHead, pVideoEvent;
    ClientPtr		client;
    unsigned long	mask;

    pHead = (VideoEventPtr *) LookupIDByType(id, EventType);
    if (!pHead)
	return;
    pevent->u.u.type = type + VideoEventBase;

    switch (type) {
    case MVEXRequest:	mask = MVEXRedirectMask;	break;
    case MVEXSetup:	mask = MVEXSetupMask;		break;
    case MVEXSync:	mask = MVEXSyncMask;		break;
    case MVEXOverride:	mask = MVEXOverrideMask;	break;
    case MVEXViolation:	mask = MVEXViolationMask;	break;
    default:		assert(FALSE);
    }
    for (pVideoEvent = *pHead; pVideoEvent; pVideoEvent = pVideoEvent->next) {
	client = pVideoEvent->client;
	if (client != serverClient && !client->clientGone && 
		(pVideoEvent->events & mask)) {
	    pevent->u.u.sequenceNumber = client->sequence;
	    WriteEventsToClient (client, 1, pevent);
	}
    }
}

void
SendVideoViolationEvent(pDraw, vid, actionMask, state)
    DrawablePtr	pDraw;		/* Drawable on which to report the event */
    VideoIO	vid;		/* VideoIO whose constraints were violated */
    unsigned long actionMask;	/* MVEXScale|MVEXPlacement|MVEXClip|MVEXOverlap*/
    int		state;		/* MVEXViolationSuccess, MVEXViolationFail,
				 * or MVEXViolationSubset */
{
    xVideoViolationEvent event;

    event.id = pDraw->id;
    event.vid = vid;
    event.time = currentTime.milliseconds;
    event.actionMask = actionMask;
    event.state = state;

    SendVideoNotify(pDraw->id, (xEvent *) &event, MVEXViolation);
}

void
SendVideoRequestEvent(request, pVideoAction, redirect)
    int			request;
    VideoActionPtr	pVideoAction;
    int			redirect;
{
    xVideoRequestEvent event;

    event.type = MVEXRequest;
    event.request = request;
    event.redirect = redirect;
    event.src = pVideoAction->videoIO;
    event.dst = pVideoAction->drawable->id;
    event.time = currentTime.milliseconds;

    SendVideoNotify(ScreenVideoInfo[pVideoAction->drawable->pScreen->myNum]->id,
	    (xEvent *) &event, MVEXRequest);
}

void
SendVideoSetupEvent(vid, port, frames, timecodes, frameAccurate)
    VideoIO	vid;
    Atom	port;
    Bool	frames;
    Bool	timecodes;
    Bool	frameAccurate;
{
    xVideoSetupEvent event;

    event.vid = vid;
    event.port = port;
    event.framesAvailable = frames;
    event.timecodesAvailable = timecodes;
    event.frameAccurate = frameAccurate;
    event.time = currentTime.milliseconds;
    SendVideoNotify (vid, (xEvent *) &event, MVEXSetup);
}

void
SendVideoSyncEvent(pDraw, vid, state)
    DrawablePtr	pDraw;
    VideoIO	vid;
    int		state;
{
    xVideoSyncEvent event;

    event.id = pDraw->id;
    event.vid = vid;
    event.time = currentTime.milliseconds;
    event.state = state;
    SendVideoNotify (vid, (xEvent *) &event, MVEXSync);
}

void
SendVideoOverrideEvent(pDraw, vid, overrideId, state)
    DrawablePtr	pDraw;
    VideoIO	vid;
    VideoIO	overrideId;
    int		state;
{
    xVideoOverrideEvent event;

    event.id = pDraw->id;
    event.vid = vid;
    event.overrideId = overrideId;
    event.time = currentTime.milliseconds;
    event.state = state;
    SendVideoNotify (pDraw->id, (xEvent *) &event, MVEXOverride);
}
