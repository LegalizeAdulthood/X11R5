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
static char *rcsid=  "$Header: /usr/local/src/x11r5/mit/extensions/RCS/Util.c,v 2.2 1991/09/19 22:26:28 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#define NEED_EVENTS
#define NEED_REPLIES
#include "Xlibint.h"
#include "MVEXlibint.h"
#include "Xutil.h"
#include "MVEXlib.h"
#include "MVEXproto.h"
#include <stdio.h>

struct DpyHasVideo {
    struct DpyHasVideo  *next;
    Display             *dpy;
    Bool                gotit;
    XExtCodes           codes;
};

static struct DpyHasVideo  *lastChecked, *dpysHaveVideo;

static XExtCodes    *queryExtension ();
static freeXExtData();

/*
 *    NAME
 *        FindVideoDpy - find the right structure for the display
 *
 *    SYNOPSIS
 */
static struct DpyHasVideo *
FindVideoDpy(dpy)
    Display     *dpy;		/* in: display pointer */
/*
 *    DESCRIPTION
 *        Scan the list of display/video structures.
 *
 *    RETURNS
 *        The display/video structure for the display.
 */
{
    struct DpyHasVideo	*dpyHas;

    for (dpyHas = dpysHaveVideo; dpyHas; dpyHas = dpyHas->next) {
	if (dpyHas->dpy == dpy)
	    return(dpyHas);
    }
    return (NULL);
}

/*
 *    NAME
 *        videoCloseDisplay - clean up MVEX when display is closed
 *
 *    SYNOPSIS
 */
static int
videoCloseDisplay (dpy, codes)
    Display	*dpy;		/* in: display pointer */
    XExtCodes	*codes;		/* in: extension codes */
/*
 *    DESCRIPTION
 *        Remove the display/video structure for the display.
 *
 *    RETURNS
 *        0.
 */
{
    struct DpyHasVideo	*dpyHas, *prev;

    prev = 0;
    for (dpyHas = dpysHaveVideo; dpyHas; dpyHas = dpyHas->next) {
	if (dpyHas->dpy == dpy)
	    break;
	prev = dpyHas;
    }
    if (!dpyHas)
	return 0;
    if (prev)
	prev->next = dpyHas->next;
    else
	dpysHaveVideo = dpyHas->next;
    if (dpyHas == lastChecked)
	lastChecked = (struct DpyHasVideo *) NULL;
    XFree (dpyHas);

    return 0;
}

/*
 *    NAME
 *        _MVEXCheckExtension - return the extension codes for the display
 *
 *    SYNOPSIS
 */
XExtCodes *
_MVEXCheckExtension (dpy)
    Display *dpy;		/* in: display pointer */
/*
 *    DESCRIPTION
 *        Return most recent data if it's appropriate, else get new data.
 *
 *    RETURNS
 *        The display's extension codes.
 */
{
    if (lastChecked && lastChecked->dpy == dpy) {
	if (lastChecked->gotit == 0)
	    return 0;
	return &lastChecked->codes;
    }
    return queryExtension (dpy);
}

				    
/*
 *    NAME
 *        videoError - MVEX error handler
 *
 *    SYNOPSIS
 */
static int
videoError (dpy, err, codes, ret_code)
    Display	*dpy;		/* in: display pointer */
    xError 	*err;		/* in: X error */
    XExtCodes	*codes;		/* in: extension codes */
    int 	*ret_code;	/* out: return code */
/*
 *    DESCRIPTION
 *        See if the error is a BadMatch for QueryVideo.
 *
 *    RETURNS
 *        1 if it is that error, 0 if not.
 */
{
    if (err->majorCode == codes->major_opcode && 
	    err->minorCode == X_QueryVideo && 
	    (int)err->errorCode == BadMatch) {
	*ret_code = 0;
	return 1;
    }

    return 0;
}

/*
 *    NAME
 *        videoErrorString - get the error string for BadVideo error
 *
 *    SYNOPSIS
 */
static char *
videoErrorString (dpy, code, codes, buffer, nbytes)
    Display	*dpy;		/* in: display pointer */
    int 	code;		/* in: error code */
    XExtCodes	*codes;		/* in: extension codes */
    char 	*buffer;	/* out: returned error string */
    int		nbytes;		/* in: n bytes to copy to buffer */
/*
 *    DESCRIPTION
 *        For BadVideo, get the string from the X list.
 *
 *    RETURNS
 *        The error string for BadVideo, NULL for all others.
 */
{
    char *defaultp = NULL;
    char buf[32];
    static char *VideoErrorStr = "BadVideo";

    if (code - codes->first_error == BadVideo) {
	sprintf(buf, "%d", code);
	defaultp = VideoErrorStr;
	XGetErrorDatabaseText(dpy, "MVEXError", buf, defaultp, buffer, nbytes);
	return buffer;
    }
    
    return (char *)NULL;
}

/*
 *    NAME
 *        videoWireToEvent - reformat MVEX wire event to XEvent
 *
 *    SYNOPSIS
 */
static int
videoWireToEvent (dpy, re, event)
    Display *dpy;	/* in: display pointer */
    XEvent  *re;	/* out: pointer to where event should be reformatted */
    xEvent  *event;	/* in: wire protocol event */
/*
 *    DESCRIPTION
 *        Reformat the event according to which type it is.
 *
 *    RETURNS
 *        0 if failure, not 0 if success
 */
{
    XExtCodes		*codes;

    codes = _MVEXCheckExtension (dpy);
    if (!codes)
	return False;

    re->type = event->u.u.type & 0x7f;
#ifdef R3
    ((XAnyEvent *)re)->serial = event->u.u.sequenceNumber;
#else
    ((XAnyEvent *)re)->serial = _XSetLastRequestRead(dpy,
        (xGenericReply *)event);
#endif
    ((XAnyEvent *)re)->send_event = ((event->u.u.type & 0x80) != 0);
    ((XAnyEvent *)re)->display = dpy;

   /*
    *  Ignore the leading bit of the event type since it is set when a
    *  client sends an event rather than the server. 
    */
    switch ((event->u.u.type & 0x7f) - codes->first_event) {
	case MVEXViolation:
	{
	    MVEXViolationEvent *hostEvent = (MVEXViolationEvent *) re;
	    xVideoViolationEvent *wireEvent = (xVideoViolationEvent *) event;

	    hostEvent->drawable = wireEvent->id;
	    hostEvent->vid = wireEvent->vid;
	    hostEvent->time = wireEvent->time;
	    hostEvent->action_mask = wireEvent->actionMask;
	    hostEvent->state = wireEvent->state;
	    return True;
	}
	case MVEXSetup:
	{
	    MVEXSetupEvent *hostEvent = (MVEXSetupEvent *) re;
	    xVideoSetupEvent *wireEvent = (xVideoSetupEvent *) event;

	    hostEvent->vid = wireEvent->vid;
	    hostEvent->port = wireEvent->port;
	    hostEvent->time = wireEvent->time;
	    hostEvent->frames_available = wireEvent->framesAvailable;
	    hostEvent->timecodes_available = wireEvent->timecodesAvailable;
	    hostEvent->frame_accurate = wireEvent->frameAccurate;
	    return True;
	}
	case MVEXSync:
	{
	    MVEXSyncEvent *hostEvent = (MVEXSyncEvent *) re;
	    xVideoSyncEvent *wireEvent = (xVideoSyncEvent *) event;

	    hostEvent->drawable = wireEvent->id;
	    hostEvent->vid = wireEvent->vid;
	    hostEvent->time = wireEvent->time;
	    hostEvent->state = wireEvent->state;
	    return True;
	}
	case MVEXOverride:
	{
	    MVEXOverrideEvent *hostEvent = (MVEXOverrideEvent *) re;
	    xVideoOverrideEvent *wireEvent = (xVideoOverrideEvent *) event;

	    hostEvent->drawable = wireEvent->id;
	    hostEvent->vid = wireEvent->vid;
	    hostEvent->override_id = wireEvent->overrideId;
	    hostEvent->time = wireEvent->time;
	    hostEvent->state = wireEvent->state;
	    return True;
	}
	case MVEXRequest:
	{
	    MVEXRequestEvent *hostEvent = (MVEXRequestEvent *) re;
	    xVideoRequestEvent *wireEvent = (xVideoRequestEvent *) event;

	    hostEvent->request = wireEvent->request;
	    hostEvent->redirect = wireEvent->redirect;
	    hostEvent->src = wireEvent->src;
	    hostEvent->dst = wireEvent->dst;
	    hostEvent->time = wireEvent->time;

	    return False; /* wait for the second half */
	}
    }
    return False;
}

/*
 *    NAME
 *        videoEventToWire - reformat MVEX XEvent to wire event
 *
 *    SYNOPSIS
 */
static int
videoEventToWire (dpy, re, event)
    Display *dpy;	/* in: display pointer */
    XEvent  *re;	/* in: pointer to host event structure */
    xEvent  *event;	/* out: wire protocol event */
/*
 *    DESCRIPTION
 *        Reformat the event according to type.
 *
 *    RETURNS
 *        0 if failure, not 0 if success
 */
{
    XExtCodes		*codes;

    codes = _MVEXCheckExtension (dpy);
    if (!codes)
	return False;

    event->u.u.type = ((XAnyEvent *)re)->type | 
	    (((XAnyEvent *)re)->send_event ? 0x80 : 0);
    event->u.u.sequenceNumber = ((XAnyEvent *)re)->serial & 0xffff;

   /*
    *  Ignore the leading bit of the event type since it is set when a
    *  client sends an event rather than the server. 
    */
    switch ((re->type & 0x7f) - codes->first_event) {
	case MVEXViolation:
	{
	    MVEXViolationEvent *hostEvent = (MVEXViolationEvent *) re;
	    xVideoViolationEvent *wireEvent = 
		    (xVideoViolationEvent *) event;

	    wireEvent->id = hostEvent->drawable;
	    wireEvent->vid = hostEvent->vid;
	    wireEvent->time = hostEvent->time;
	    wireEvent->actionMask = hostEvent->action_mask;
	    wireEvent->state = hostEvent->state;
	    return True;
	}
	case MVEXSync:
	{
	    MVEXSyncEvent *hostEvent = (MVEXSyncEvent *) re;
	    xVideoSyncEvent *wireEvent = 
		    (xVideoSyncEvent *) event;

	    wireEvent->id = hostEvent->drawable;
	    wireEvent->vid = hostEvent->vid;
	    wireEvent->time = hostEvent->time;
	    wireEvent->state = hostEvent->state;
	    return True;
	}
	case MVEXOverride:
	{
	    MVEXOverrideEvent *hostEvent = (MVEXOverrideEvent *) re;
	    xVideoOverrideEvent *wireEvent = 
		    (xVideoOverrideEvent *) event;

	    wireEvent->id = hostEvent->drawable;
	    wireEvent->vid = hostEvent->vid;
	    wireEvent->overrideId = hostEvent->override_id;
	    wireEvent->time = hostEvent->time;
	    return True;
	}
	case MVEXRequest:
	{
	    /*
	     * for this to happen, the way that XSendEvent calls
	     * (*dpy->wire_vec)() must be changed to allow multiple
	     * events to be returned.
	     */
	    (void) fprintf(stderr, "can't format VideoRequest to wire\n");
	    return False;
	}
    }
    return False;
}

/*
 *    NAME
 *        freeXExtData - interface to _MVEXFreeVideoInfo
 *
 *    SYNOPSIS
 */
static
freeXExtData(extension)
    XExtData *extension;	/* in: extension data */
/*
 *    DESCRIPTION
 *        Just call _MVEXFreeVideoInfo.
 *
 *    RETURNS
 *        void
 */
{
    _MVEXFreeVideoInfo((VideoInfo *)extension->private_data);
}

/*
 *    NAME
 *        _MVEXFreeVideoInfo - free all allocated memory in the video info
 *
 *    SYNOPSIS
 */
void
_MVEXFreeVideoInfo(pVideoInfo)
    VideoInfo *pVideoInfo;	/* in: pointer to video info */
/*
 *    DESCRIPTION
 *        Go through the structure, freeing everything that's there.
 *
 *    RETURNS
 *        void
 */
{
    int i,j;

    if (!pVideoInfo)
	return;

    if (pVideoInfo->ndepths > 0)
	XFree ((char *) pVideoInfo->depths);

    if (pVideoInfo->nvin > 0) {
	for (j = 0; j < pVideoInfo->nvin; j++) {
	    XFree ((char *) pVideoInfo->vin[j].model_list);
	    XFree ((char *) pVideoInfo->vin[j].geometry.placement);
	}
	XFree ((char *) pVideoInfo->vin);
    }

    if (pVideoInfo->nvout > 0) {
	for (j = 0; j < pVideoInfo->nvout; j++) {
	    XFree ((char *) pVideoInfo->vout[j].model_list);
	    XFree ((char *) pVideoInfo->vout[j].geometry.placement);
	}
	XFree ((char *) pVideoInfo->vout);
    }

    if (pVideoInfo->vin_ids.num > 0)
	XFree(pVideoInfo->vin_ids.map);
    if (pVideoInfo->vout_ids.num > 0)
	XFree(pVideoInfo->vout_ids.map);

    XFree ((char *) pVideoInfo);
}

/*
 *    NAME
 *        queryExtension - set up display/video structure if MVEX is there
 *
 *    SYNOPSIS
 */
static XExtCodes *
queryExtension (dpy)
    register Display *dpy;		/* in: display pointer */
/*
 *    DESCRIPTION
 *        If the display has no display/video structure, build one for it.
 *
 *    RETURNS
 *        The display's extension codes, or NULL if MVEX cannot be init'd.
 */
{
    struct DpyHasVideo	*dpyHas;
    XExtCodes		*codes;
    int			i;
    XExtData		**pScreenXExtData;
    XEDataObject	object;
    XExtData		*pXExtData;

    dpyHas = FindVideoDpy(dpy);
    if (!dpyHas) {
	dpyHas = (struct DpyHasVideo *) Xmalloc (sizeof (struct DpyHasVideo));
	if (!dpyHas)
	    return (XExtCodes *) 0;
	dpyHas->next = dpysHaveVideo;
	dpysHaveVideo = dpyHas;

	dpyHas->dpy = dpy;
	if (dpyHas->gotit = (codes = XInitExtension (dpy, MVEX_NAME)) != 0) {
	    dpyHas->codes = *codes;
	    /*
	     * initialize the various Xlib function vectors
	     */
	    XESetCloseDisplay (dpy, codes->extension, videoCloseDisplay);
	    XESetError(dpy, codes->extension, videoError);
	    XESetErrorString(dpy, codes->extension, videoErrorString);
	    for (i = 0; i < VideoNumberEvents; i++) {
		XESetWireToEvent (dpy, codes->first_event + i, videoWireToEvent);
		XESetEventToWire (dpy, codes->first_event + i, videoEventToWire);
	    }
	    /*
	     * Allocate a XExtData structure for each screen of this display
	     * and attach them to screen structures
	     *
	     * Call QueryVideo on each screen and attach info structures
	     * to XExtData structures for screens which have video hdwr.
	     */
	    for (i=0; i<ScreenCount(dpy); i++) {
		object.screen = ScreenOfDisplay(dpy, i);
		pScreenXExtData = XEHeadOfExtensionList(object);
		pXExtData = (XExtData *) Xmalloc(sizeof(XExtData));
		pXExtData->number = codes->extension;
		pXExtData->free_private = freeXExtData;
		(void) _MVEXQueryVideo(dpy, RootWindow(dpy, i),
			(VideoInfo **)&pXExtData->private_data, codes);
		if (pXExtData->private_data)
		  ((VideoInfo *)pXExtData->private_data)->video_connect_time=
			((VideoInfo *)pXExtData->private_data)->query_video_time;
		XAddToExtensionList(pScreenXExtData, pXExtData);

	    }
	    
	}
    }
    lastChecked = dpyHas;
    if (!dpyHas->gotit)
	return (XExtCodes *) 0;
    return &dpyHas->codes;
}
