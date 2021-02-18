/*
 * XDPS.c -- implementation of low-level Xlib routines for XDPS extension
 *
 * Copyright (C) 1989-1991 by Adobe Systems Incorporated.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notices appear in all copies and that
 * both those copyright notices and this permission notice appear in
 * supporting documentation and that the name of Adobe Systems
 * Incorporated not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  If any portion of this software is changed, it cannot be
 * marketed under Adobe's trademarks and/or copyrights unless Adobe, in
 * its sole discretion, approves by a prior writing the quality of the
 * resulting implementation.
 * 
 * ADOBE MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THE SOFTWARE FOR
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
 * ADOBE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO EVENT SHALL ADOBE BE LIABLE
 * TO YOU OR ANY OTHER PARTY FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE, STRICT LIABILITY OR ANY OTHER ACTION ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  ADOBE WILL NOT
 * PROVIDE ANY TRAINING OR OTHER SUPPORT FOR THE SOFTWARE.
 * 
 * PostScript, Display PostScript, and Adobe are trademarks of Adobe Systems
 * Incorporated registered in the U.S.A. and other countries.
 *
 * Author: Adobe Systems Incorporated
 */

#define NEED_EVENTS
#define NEED_REPLIES

#include <stdio.h>
#include <X11/Xlibint.h>
#include "DPS/XDPS.h"
#include "DPS/XDPSproto.h"
#include "DPS/XDPSlib.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#ifndef _NFILE
#define _NFILE  128
#endif /* _NFILE */

static XExtCodes *Codes[_NFILE] = {NULL};

static Display *dpys[_NFILE];
static int version[_NFILE];
static XDPSLEventHandler StatusProc[_NFILE] = {NULL};
static nextDpy = 0;

typedef struct {
    char passEvents;
    char wrapWaiting;
} DPSDisplayFlags;

/* For now the structure is no larger than a pointer.  Revisit this if the
   structure grows. */

static DPSDisplayFlags displayFlags[_NFILE];

static int Punt()
{
    fprintf(stderr,
	    "DPS/X Error:  Extension has not been initialized.\n");
    exit(1);
}

#ifdef VMS
/* This is a terribly inefficient way to find a per-display index, but we
   need it till we get dpy->fd fixed in VMS%%%%%*/
static int FindDpyNum(dpy)
{
int i;
for (i=0; dpys[i] != dpy ; i++)
    {
    if (i == nextDpy)
        {
        dpys[nextDpy++]=dpy;
        break;
        }
    }
return i;
}
#define DPY_NUMBER(dpy)         FindDpyNum(dpy)
#else /* VMS */
#define DPY_NUMBER(dpy)		((dpy)->fd)
#endif /* VMS */

#define MajorOpCode(dpy)	(Codes[DPY_NUMBER(dpy)] ?		\
				 Codes[DPY_NUMBER(dpy)]->major_opcode : \
				 Punt())


static XDPSLEventHandler TextProc= NULL;

/* ARGSUSED */
void
XDPSLSetTextEventHandler(dpy, proc)
    Display *dpy;
    XDPSLEventHandler proc;
{
    TextProc = proc;
}

/* ARGSUSED */
void XDPSLCallOutputEventHandler(dpy, event)
    Display *dpy;
    XEvent *event;
{
    (*TextProc)(event);
}

void
XDPSLSetStatusEventHandler(dpy, proc)
    Display *dpy;
    XDPSLEventHandler proc;
{
    StatusProc[DPY_NUMBER(dpy)] = proc;
}

void XDPSLCallStatusEventHandler(dpy, event)
    Display *dpy;
    XEvent *event;
{
    (*(StatusProc[DPY_NUMBER(dpy)]))(event);
}

void XDPSLInitDisplayFlags(dpy)
    Display *dpy;
{
    int d = DPY_NUMBER(dpy);
    displayFlags[d].wrapWaiting = False;

    /* Instead of explicitly setting the pass-event flag, rely upon the fact
       that it gets initialized to 0 by the compiler.  This means that you
       can set the event delivery mode on a display before creating any
       contexts, which is a Good Thing */
}

XExtCodes *XDPSLGetCodes(dpy)
    Display *dpy;
{
    return Codes[DPY_NUMBER(dpy)];
}

/* ARGSUSED */
static int
CloseDisplayProc(dpy, codes)
Display *dpy;
XExtCodes *codes;
{
    Codes[DPY_NUMBER(dpy)] = NULL;
#ifdef VMS
    dpys[DPY_NUMBER(dpy)] = NULL;
 /*%%%%Temp till we fix dpy->fd*/
#endif /* VMS */
}

Bool XDPSLGetPassEventsFlag(dpy)
    Display *dpy;
{
    return displayFlags[DPY_NUMBER(dpy)].passEvents;
}

void XDPSLSetPassEventsFlag(dpy, flag)
    Display *dpy;
    Bool flag;
{
    displayFlags[DPY_NUMBER(dpy)].passEvents = flag;
}

Bool XDPSLGetWrapWaitingFlag(dpy)
    Display *dpy;
{
    return displayFlags[DPY_NUMBER(dpy)].wrapWaiting;
}

void XDPSLSetWrapWaitingFlag(dpy, flag)
    Display *dpy;
    Bool flag;
{
    displayFlags[DPY_NUMBER(dpy)].wrapWaiting = flag;
}

static Status 
ConvertOutputEvent(dpy, ce, we)
    Display *dpy;
    XEvent *ce;
    xEvent *we;
{
    register PSOutputEvent *wireevent = (PSOutputEvent *) we;
    register XDPSLOutputEvent *clientevent = (XDPSLOutputEvent *) ce;
    
    clientevent->type = wireevent->type & 0x7f;
    clientevent->serial = _XSetLastRequestRead(dpy,
					       (xGenericReply *)wireevent);
    clientevent->send_event = (wireevent->type & 0x80) != 0;
    clientevent->display = dpy;
    clientevent->cxid = wireevent->cxid;
    clientevent->length = wireevent->length;
    bcopy(wireevent->data, clientevent->data, wireevent->length);
    if (TextProc && !XDPSLGetPassEventsFlag(dpy)) {
	(*TextProc)((XEvent *) clientevent);
	return False;
    }
    return True;
}

static Status 
ConvertStatusEvent(dpy, ce, we)
    Display *dpy;
    XEvent *ce;
    xEvent *we;
{
    register PSStatusEvent *wireevent = (PSStatusEvent *) we;
    register XDPSLStatusEvent *clientevent = (XDPSLStatusEvent *) ce;
    
    clientevent->type = wireevent->type & 0x7f;
    clientevent->serial = _XSetLastRequestRead(dpy,
					       (xGenericReply *)wireevent);
    clientevent->send_event = (wireevent->type & 0x80) != 0;
    clientevent->display = dpy;
    clientevent->cxid = wireevent->cxid;
    clientevent->status = wireevent->status;
    if (StatusProc[DPY_NUMBER(dpy)] && !XDPSLGetPassEventsFlag(dpy)) {
	(*(StatusProc[DPY_NUMBER(dpy)]))((XEvent *) clientevent);
	return False;
    }
    return True;
}

int
XDPSLInit(dpy, numberType, floatingName)
    Display *dpy;
    int *numberType;		/* RETURN */
    char **floatingName;	/* RETURN */
{
    XExtCodes *codes;
    register xPSInitReq *req;
    xPSInitReply rep;
    char *ptr;
    int first_event;
    
    if (Codes[DPY_NUMBER(dpy)] != NULL) {
	first_event = Codes[DPY_NUMBER(dpy)]->first_event;
    } else {
	codes = XInitExtension(dpy, DPSNAME);
	if (codes == NULL) {
	    /* try DEC UWS 2.2 server */
	    codes = XInitExtension(dpy, DECDPSNAME);
	    if (codes == NULL) return -1;
	}
	Codes[DPY_NUMBER(dpy)] = codes;
	XESetCloseDisplay(dpy, codes->extension, CloseDisplayProc);
	XESetWireToEvent(dpy, codes->first_event + PSEVENTOUTPUT,
			 ConvertOutputEvent);
	XESetWireToEvent(dpy, codes->first_event + PSEVENTSTATUS,
			 ConvertStatusEvent);
	first_event = codes->first_event;
    }

    LockDisplay(dpy);

    GetReq(PSInit, req);
    req->reqType = MajorOpCode(dpy);
    req->dpsReqType = X_PSInit;
    req->libraryversion = DPSPROTOCOLVERSION;

    (void) _XReply(dpy, (xReply *) &rep, 0, xFalse);

    if (rep.serverversion != DPSPROTOCOLVERSION) {
	fprintf(stderr, "XDPS Error:  Incompatible protocol versions.\n");
	exit(1);
    }


    version[DPY_NUMBER(dpy)] = rep.serverversion;
    if (numberType)
	*numberType = rep.preferredNumberFormat;

    ptr = Xmalloc(rep.floatingNameLength + 1);
    _XReadPad(dpy, ptr, rep.floatingNameLength);
    ptr[rep.floatingNameLength] = 0;
    if (floatingName)
	*floatingName = ptr;
    else
	Xfree(ptr);

    UnlockDisplay(dpy);
    SyncHandle();
    return first_event;
}




static void CopyColorMapsIntoCreateContextReq(req, colorcube, grayramp)
    xPSCreateContextReq *req;
    XStandardColormap *colorcube, *grayramp;
{
    req->cmap = 0;
    if (colorcube != NULL) {
        req->cmap = colorcube->colormap;
	req->redmax = colorcube->red_max;
	req->redmult = colorcube->red_mult;
	req->greenmax = colorcube->green_max;
	req->greenmult = colorcube->green_mult;
	req->bluemax = colorcube->blue_max;
	req->bluemult = colorcube->blue_mult;
	req->colorbase = colorcube->base_pixel;
    }
    else req->redmult = 0;

    if (grayramp != NULL) {
	req->cmap = grayramp->colormap;
	req->graymax = grayramp->red_max;
	req->graymult = grayramp->red_mult;
	req->graybase = grayramp->base_pixel;
    }
    else req->graymult = 0;
}





/* ARGSUSED */
ContextXID
XDPSLCreateContextAndSpace(dpy, draw, gc, x, y, eventMask, grayRamp,
			   colorCube, actual, cpsid, sxid)
    register Display *dpy;
    Drawable draw;
    GC gc;
    int x, y;
    unsigned int eventMask;
    XStandardColormap *grayRamp,*colorCube;
    unsigned int actual;
    ContextPSID *cpsid;		/* RETURN */
    SpaceXID *sxid;		/* RETURN */
{
    ContextXID cxid;
    register xPSCreateContextReq *req;
    xPSCreateContextReply rep;
    XStandardColormap defColorcube, defGrayramp;
    XStandardColormap *requestCube, *requestRamp;
    int index;
    
    if (grayRamp == NULL && colorCube == NULL) return(None);

    /* Index gets encoded as follows:
     *
     *  0  grayRamp = Default,	   colorCube = Default
     *  1  grayRamp = non-Default, colorcube = Default
     *  2  grayRamp = Default,	   colorcube = non-Default
     *  3  grayRamp = non-Default, colorcube = non-Default
     *
     */
    index = ((grayRamp == DefaultStdCMap || grayRamp == NULL) ? 0 : 1) +
	((colorCube == DefaultStdCMap || colorCube == NULL) ? 0 : 2);

    switch (index)
	{
	case 0: /* Both are default */
	    XDPSGetDefaultColorMaps(dpy, (Screen *) NULL, draw,
				    &defColorcube, &defGrayramp);
	    requestCube = &defColorcube;
	    requestRamp = &defGrayramp;
	    break;

	case 1: /* gray specified, Color default */
	    XDPSGetDefaultColorMaps(dpy, (Screen *) NULL, draw,
				    &defColorcube, (XStandardColormap *) NULL);
	    requestCube = &defColorcube;
	    requestRamp = grayRamp;
	    break;

	case 2: /* gray default, Color specified */
	    XDPSGetDefaultColorMaps(dpy, (Screen *) NULL, draw,
				    (XStandardColormap *) NULL, &defGrayramp);
	    requestCube = colorCube;
	    requestRamp = &defGrayramp;
	    break;

	case 3: /* Both specified */
	    requestCube = colorCube;
	    requestRamp = grayRamp;
	    break;
	}

    LockDisplay(dpy);
    if (gc != NULL) FlushGC(dpy, gc);

    GetReq(PSCreateContext, req);
    CopyColorMapsIntoCreateContextReq(req, requestCube, requestRamp);

    req->reqType = MajorOpCode(dpy);
    req->dpsReqType = X_PSCreateContext;
    req->x = x;
    req->y = y;
    req->drawable = draw;
    req->gc = (gc != NULL) ? gc->gid : None;
    cxid = req->cxid = XAllocID(dpy);
    req->sxid = XAllocID(dpy);	  
    if (sxid)
	*sxid = req->sxid;
    req->eventmask = 0;		/* %%% */
    req->actual = actual;
    (void) _XReply (dpy, (xReply *)&rep, 0, xTrue);

    if (cpsid)
	*cpsid = (int)rep.cpsid;

    UnlockDisplay(dpy);
    SyncHandle();
    return (cxid);
}


/* ARGSUSED */
ContextXID
XDPSLCreateContext(dpy, sxid, draw, gc, x, y, eventMask, grayRamp,
		   colorCube, actual, cpsid)
    register Display *dpy;
    SpaceXID sxid;
    Drawable draw;
    GC gc;
    int x, y;
    unsigned int eventMask;
    XStandardColormap *grayRamp,*colorCube;
    unsigned int actual;
    ContextPSID *cpsid;		/* RETURN */
{
    register xPSCreateContextReq *req;
    xPSCreateContextReply rep;
    ContextXID cxid;			/* RETURN */
    XStandardColormap defColorcube, defGrayramp;
    XStandardColormap *requestCube, *requestRamp;
    int index;

    /* Index gets encoded as follows:
     *
     *  0  grayRamp = Default,	   colorCube = Default
     *  1  grayRamp = non-Default, colorcube = Default
     *  2  grayRamp = Default,	   colorcube = non-Default
     *  3  grayRamp = non-Default, colorcube = non-Default
     *
     */
    index = ((grayRamp == DefaultStdCMap) ? 0 : 1) +
	((colorCube == DefaultStdCMap) ? 0 : 2);

    switch (index)
	{
	case 0: /* Both are default */
	    XDPSGetDefaultColorMaps(dpy, (Screen *) NULL, draw,
				    &defColorcube, &defGrayramp);
	    requestCube = &defColorcube;
	    requestRamp = &defGrayramp;
	    break;

	case 1: /* gray specified, Color default */
	    XDPSGetDefaultColorMaps(dpy, (Screen *) NULL, draw,
				    &defColorcube, (XStandardColormap *) NULL);
	    requestCube = &defColorcube;
	    requestRamp = grayRamp;
	    break;

	case 2: /* gray default, Color specified */
	    XDPSGetDefaultColorMaps(dpy, (Screen *) NULL, draw,
				    (XStandardColormap *) NULL, &defGrayramp);
	    requestCube = colorCube;
	    requestRamp = &defGrayramp;
	    break;

	case 3: /* Both specified */
	    requestCube = colorCube;
	    requestRamp = grayRamp;
	    break;
	}


    LockDisplay(dpy);
    if (gc != NULL) FlushGC(dpy, gc);

    GetReq(PSCreateContext, req);
    CopyColorMapsIntoCreateContextReq(req, requestCube, requestRamp);

    req->reqType = MajorOpCode(dpy);
    req->dpsReqType = X_PSCreateContext;
    req->x = x;
    req->y = y;
    req->drawable = draw;
    req->gc = (gc != NULL) ? gc->gid : None;
    cxid = req->cxid = XAllocID(dpy);
    req->sxid = sxid;
    req->actual = actual;

    (void) _XReply (dpy, (xReply *)&rep, 0, xTrue);
    if (cpsid)
	*cpsid = (int)rep.cpsid;
    UnlockDisplay(dpy);
    SyncHandle();

    return cxid;
    
}


SpaceXID
XDPSLCreateSpace(dpy)
    register Display *dpy;
{
    register xPSCreateSpaceReq *req;
    SpaceXID sxid;

    LockDisplay(dpy);

    GetReq(PSCreateSpace, req);
    req->reqType = MajorOpCode(dpy);
    req->dpsReqType = X_PSCreateSpace;
    sxid = req->sxid = XAllocID(dpy);

    UnlockDisplay(dpy);
    SyncHandle();
    return sxid;
    
}



/*
 * I'm not sure how portable my coalescing code is, so I've provided the
 * below define.  If it turns out this code just breaks somewhere, you
 * can simply undefine COALESCEGIVEINPUT, and then everything will work
 * (but slower).  6/16/89 (tw)
 */

#define COALESCEGIVEINPUT

void
XDPSLGiveInput(dpy, cxid, data, length)
register Display *dpy;
ContextXID cxid;
char *data;
int length;
{
    register xPSGiveInputReq *req;
    int sendlen;

    LockDisplay(dpy);
#ifdef COALESCEGIVEINPUT
    req = (xPSGiveInputReq *) dpy->last_req;
    if ((req->reqType == MajorOpCode(dpy)
	 && req->dpsReqType == X_PSGiveInput
	 && req->cxid == cxid
	 && dpy->bufptr + length + 3 < dpy->bufmax)) {
	bcopy(data, ((char *) req) + sizeof(xPSGiveInputReq) + req->nunits,
	      length);
	req->nunits += length;
	req->length = (sizeof(xPSGiveInputReq) + req->nunits + 3) >> 2;
	dpy->bufptr = dpy->last_req + sizeof(xPSGiveInputReq) +
	    ((req->nunits + 3) & ~3);
    } else
#endif /* COALESCEGIVEINPUT */
    {
	do {
	    if (dpy->bufptr + sizeof(xPSGiveInputReq) + length + 3 >=
		    dpy->bufmax)
		_XFlush(dpy);
	    sendlen = dpy->bufmax - dpy->bufptr - sizeof(xPSGiveInputReq) - 4;
	    if (sendlen > length) sendlen = length;
	    GetReq(PSGiveInput, req);
	    req->reqType = MajorOpCode(dpy);
	    req->dpsReqType = X_PSGiveInput;
	    req->cxid = cxid;
	    req->nunits = sendlen;
	    req->length += ((sendlen + 3) >> 2);
	    Data(dpy, (char *) data, sendlen);
	    length -= sendlen;
	    data += sendlen;
	} while (length > 0);
    }
    UnlockDisplay(dpy);
    SyncHandle();
}


int
XDPSLGetStatus(dpy, cxid)
    register Display *dpy;
    ContextXID cxid;
{
    register xPSGetStatusReq *req;
    xPSGetStatusReply rep;
    
    LockDisplay(dpy);

    GetReq(PSGetStatus, req);
    req->reqType = MajorOpCode(dpy);
    req->dpsReqType = X_PSGetStatus;
    req->cxid = cxid;
    req->notifyIfChange = 0;

    if (!_XReply(dpy, (xReply *)&rep, 0, xTrue))
	rep.status = PSSTATUSERROR;
    UnlockDisplay(dpy);
    SyncHandle();
    return (int) rep.status;
}

void
XDPSLDestroySpace( dpy, sxid )
    Display *dpy;
    SpaceXID sxid;
{
    register xPSDestroySpaceReq *req;

    LockDisplay(dpy);

    GetReq(PSDestroySpace, req);
    req->reqType = MajorOpCode(dpy);
    req->dpsReqType = X_PSDestroySpace;
    req->sxid = sxid;

    UnlockDisplay(dpy);
    SyncHandle();
}


void
XDPSLReset( dpy, cxid )
    Display *dpy; 
    ContextXID cxid; 
{
    register xPSResetReq *req;

    LockDisplay(dpy);

    GetReq(PSReset, req);
    req->reqType = MajorOpCode(dpy);
    req->dpsReqType = X_PSReset;
    req->cxid = cxid;

    UnlockDisplay(dpy);
    SyncHandle();
}

void
XDPSLNotifyContext( dpy, cxid, ntype )
    Display *dpy; 
    ContextXID cxid; 
    int ntype;		  /* should this be an enum?? %%% */
{
    register xPSNotifyContextReq *req;

    LockDisplay(dpy);

    GetReq(PSNotifyContext, req);
    req->reqType = MajorOpCode(dpy);
    req->dpsReqType = X_PSNotifyContext;
    req->cxid = cxid;
    req->notifyType = ntype;

    UnlockDisplay(dpy);
    SyncHandle();

}


ContextXID
XDPSLCreateContextFromID( dpy, cpsid, sxid )
    Display *dpy; 
    ContextPSID cpsid; 
    SpaceXID *sxid;		/* RETURN */
{
    register xPSCreateContextFromIDReq *req;
    xPSCreateContextFromIDReply rep;
    ContextXID cxid;

    LockDisplay(dpy);

    GetReq(PSCreateContextFromID, req);
    req->reqType = MajorOpCode(dpy);
    req->dpsReqType = X_PSCreateContextFromID;
    req->cpsid = cpsid;
    cxid = req->cxid = XAllocID(dpy);
    
    (void) _XReply (dpy, (xReply *)&rep, 0, xTrue);
    if (sxid)
	*sxid = (int)rep.sxid;

    UnlockDisplay(dpy);
    SyncHandle();
    return(cxid);
}


/* Returns 1 on success, 0 on failure (cpsid not a valid context). */

Status
XDPSLIDFromContext( dpy, cpsid, cxid, sxid )
    Display *dpy; 
    ContextPSID cpsid; 
    ContextXID *cxid;			/* RETURN */
    SpaceXID *sxid;			/* RETURN */
{
    register xPSXIDFromContextReq *req;
    xPSXIDFromContextReply rep;

    LockDisplay(dpy);

    GetReq(PSXIDFromContext, req);
    req->reqType = MajorOpCode(dpy);
    req->dpsReqType = X_PSXIDFromContext;
    req->cpsid = cpsid;
    
    (void) _XReply (dpy, (xReply *)&rep, 0, xTrue);
    *sxid = (int)rep.sxid;
    *cxid = (int)rep.cxid;

    UnlockDisplay(dpy);
    SyncHandle();

    return((Status)(*sxid != None && *cxid != None));
	
}


ContextPSID
XDPSLContextFromXID( dpy, cxid )
    Display *dpy; 
    ContextXID cxid; 
{
    register xPSContextFromXIDReq *req;
    xPSContextFromXIDReply rep;

    LockDisplay(dpy);

    GetReq(PSContextFromXID, req);
    req->reqType = MajorOpCode(dpy);
    req->dpsReqType = X_PSContextFromXID;
    req->cxid = cxid;
    
    (void) _XReply (dpy, (xReply *)&rep, 0, xTrue);

    UnlockDisplay(dpy);
    SyncHandle();

    return (int)rep.cpsid;
}


void
XDPSLSetStatusMask( dpy, cxid, enableMask, disableMask, nextMask )
    Display *dpy;
    ContextXID cxid;
    unsigned long enableMask, disableMask, nextMask;
{
    register xPSSetStatusMaskReq *req;

    LockDisplay(dpy);

    GetReq(PSSetStatusMask, req);
    req->reqType = MajorOpCode(dpy);
    req->dpsReqType = X_PSSetStatusMask;
    req->cxid = cxid;
    req->enableMask = enableMask;
    req->disableMask = disableMask;
    req->nextMask = nextMask;

    UnlockDisplay(dpy);
    SyncHandle();
}


#ifdef VMS
/*
 * _XReadPad - Read bytes from the socket taking into account incomplete
 * reads.  If the number of bytes is not 0 mod 32, read additional pad
 * bytes. This routine may have to be reworked if int < long.
 */
 
/*
This is really in xlib, but is not in the sharable image transfer vector
so it's here for now.  The following notice applies only to the function
_XReadPad */

Copyright 1985, 1986, 1987, 1988, 1989 by the
Massachusetts Institute of Technology

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of M.I.T. not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission.  M.I.T. makes no representations about the
suitability of this software for any purpose.  It is provided "as is"
without express or implied warranty.

*/  

_XReadPad (dpy, data, size)
	register Display *dpy;
	register char *data;
	register long size;
{
	static int padlength[4] = {0,3,2,1};
	register long bytes_read;
	char pad[3];
 
	CheckLock(dpy);
	if (size == 0) return;
	_XRead( dpy, data, size );
	if ( padlength[size & 3] ) {
	    _XRead( dpy, pad, padlength[size & 3] );
	}

}
#endif /* VMS */
