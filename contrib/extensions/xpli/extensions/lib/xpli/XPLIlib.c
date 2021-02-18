/*
 * Copyright IBM Corporation 1991
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/*
  Todo
  
  1. Determine proper names and place for defines for manage stroke bits
  
  
  */


#define NEED_REPLIES

#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/Xproto.h>
#include <XPLIlib.h>
#include <stdio.h>

/*********************************
  /*********************************
    
    PLIRegisterExtension
    
    
    Initilialize pli extension
    allocate memory for pliextdata
    allocate memory for xextdata
    chain pliextdata off of data control block
    issue getconfig request
    update pliextdata with mask and configuration data
    (for r5:  machine id replaced by time 
    
    return pliextdata
    
    
    Thoughts
    
    
    
    *********************************/
  
extern XExtCodes *XInitExtension();
extern XExtCodes *XAddExtension();
extern XExtData  **XEHeadOfExtensionList();

extern Bool XCheckTypedEvent();
extern Bool XCheckTypedWindowEvent();

extern Status _XReply();
extern void   _XRead();
extern void   _XReadPad();

/* for single server connection or consistent event codes*/
static int PLIBaseReqCode = 0;
static int PLIBaseEvent   = 0;
static int PLIBaseError   = 0;

extern Bool PLISetWireToHost();



PLIExtData  *PLIRegisterExtension(dpy)
     Display *dpy;
{ /* PLIRegisterExtension */
  
  XExtCodes  *xextcodes;
  PLIExtData *pliextdata;
  XExtData   *xextdata;
  XExtData   **xextdatalist;
  
  xPLIConfigReq         *pliconfigreq;
  xPLIConfigReply       pliconfigreply;
  
  pliextdata = NULL;
  
  xextcodes = XInitExtension(dpy, XPLI_NAME);
  
  if (xextcodes == NULL)
    { /* extension does not exist */
      fprintf(stderr,"PLIRegisterExtension: Could not initialize extension\n");
      return(pliextdata);
    } /* extension does not exist */
  
  
  XESetWireToEvent(dpy,xextcodes->first_event,PLISetWireToHost);
  
  
  pliextdata = (PLIExtData *) Xmalloc(sizeof(*pliextdata));
  
  if (pliextdata == NULL)
    { /* Xmalloc failed */
      fprintf(stderr,"PLIRegisterExtension: Xmalloc failed for PLIExtData\n");
      return(pliextdata);
    } /* Xmalloc failed */
  
  
  PLIBaseReqCode = xextcodes->major_opcode;
  PLIBaseEvent   = xextcodes->first_event;
  PLIBaseError   = xextcodes->first_error;
  
  
  xextdata = (XExtData *) Xmalloc(sizeof(*xextdata));
  
  if (pliextdata == NULL)
    { /* Xmalloc failed */
      fprintf(stderr,"PLIRegisterExtension: Xmalloc failed for XExtData\n");
      Xfree(pliextdata);
      pliextdata = NULL;
      return(pliextdata);
    } /* Xmalloc failed */
  
  xextdata->number = xextcodes->extension;
  xextdata->next = NULL;
  
  /* By not setting a free routine the storage for XExtData and the private */
  /* PLIExtData will be freed automatically at close                        */
  xextdata->free_private = NULL;
  xextdata->private_data = (char *) pliextdata;
  
  xextdatalist = XEHeadOfExtensionList(dpy);
  
  if (xextdatalist == NULL)
    { /*  failed */
      fprintf(stderr,"PLIRegisterExtension: XEHeadOfExtensionList failed\n");
      Xfree(pliextdata);
      Xfree(xextdata);
      pliextdata = NULL;
      xextdata = NULL;
      return(pliextdata);
    } /*  failed */
  
  XAddToExtensionList(xextdatalist,xextdata);
  
  
  /* Preamble */
  LockDisplay(dpy);
  /* Preamble */
  
  GetReq(PLIConfig,pliconfigreq);
  
  pliconfigreq->reqType = PLIBaseReqCode;
  pliconfigreq->extReqType = X_PLIConfig;
  
  
  (void) _XReply(dpy,(xReply *) &pliconfigreply,
		 ( (sizeof(xPLIConfigReply) - sizeof(xReply) >> 2)), xTrue);

  pliextdata->eventCode = PLIBaseEvent;
  pliextdata->eventMask = pliconfigreply.eventMask;
  pliextdata->digtype  = pliconfigreply.digtype;
  pliextdata->scalefac = pliconfigreply.scalefac;
  pliextdata->sampsec  = pliconfigreply.sampsec;
  pliextdata->resolX   = pliconfigreply.resolX;   
  pliextdata->resolY   = pliconfigreply.resolY;
  pliextdata->resolTimeSec    = pliconfigreply.resolTimeSec;
  pliextdata->resolTimeNsec   = pliconfigreply.resolTimeNsec;
  
  /* Postamble */
  UnlockDisplay(dpy);
  SyncHandle();
  
  /* Postamble */
  
  
  return(pliextdata);
  
} /* PLIRegisterExtension */





/*********************************
  /*********************************
    
    PLICopyStrokeToClient
    
    
    if extension is not registered
    register extension
    request stroke structure for event
    put reply structure into stroke event
    
    
    *********************************/
  
BOOL PLICopyStrokeToClient(dpy,w,strokeID,strokeP,nxys)
Display *dpy;
Window w;
CARD16 strokeID;
xPLIStroke *strokeP;
int nxys;               /* number of xy pairs expected in reply */ 

{ /* PLICopyStrokeToClient */
  
  xPLIManageStrokeXYReq    *plimanagestrokexyreq;
  xPLIManageStrokeXYReply  plimanagestrokexyreply;
  
#ifdef VERBOSE
  printf("PLICopyStrokeToClient: display = %lx\n",(long int)dpy );
#endif
  
  if (PLIBaseReqCode == 0)
    { /* this process has not initialized */
      if (PLIRegisterExtension(dpy) == NULL)
	return(False);
    } /* this process has not initialized */
  
  /* Preamble */
  LockDisplay(dpy);
  /* Preamble */
  
  GetReq(PLIManageStrokeXY,plimanagestrokexyreq);
  
  plimanagestrokexyreq->reqType = PLIBaseReqCode;
  plimanagestrokexyreq->extReqType = X_PLIManageStrokeXY;
  plimanagestrokexyreq->window  = w;
  plimanagestrokexyreq->strokeID = strokeID;
  plimanagestrokexyreq->subReq = COPY_STROKE;
  plimanagestrokexyreq->startIndex = 0;
  plimanagestrokexyreq->endIndex = 0;
  
  /* 
   * First tell _XReply to read the complete stroke reply; this number is the 
   * number of 32-bit words above the 8 words in a standard xReply.
   */
    if (_XReply(dpy, (xReply *)&plimanagestrokexyreply,
	       ((sizeof(xPLIManageStrokeXYReply) - 32) >> 2), xFalse ) == 0)
	{
	UnlockDisplay(dpy);
	SyncHandle();
	return(NULL);
	}
  
  *strokeP = plimanagestrokexyreply.stroke;

  /* 
   * Now tell _XReadPad to read the  variable length data; this is the data 
   * beyond the fixed xPLIManageStrokeXYReply; the length of a 
   * xPLIManageStrokeXYReply in bytes is the length of an xPLIStroke plus the
   * number of xy pairs times 4.  But _XReply has already read the complete 
   * fixed part of the reply, so simply read nxys << 2.
   */
_XReadPad(dpy,(char *)&strokeP[1], ( nxys << 2 ) );
  
  /* Postamble */
  UnlockDisplay(dpy);
  SyncHandle();
  /* Postamble */
  
  return(True);
  
} /* PLICopyStrokeToClient */



/*********************************
  /*********************************
    
    PLIDeleteStrokes
    
    if extension is not registered
    register extension
    for each stroke event
    request stroke to be removed
    Flush the output buffer
    
    
    Note   The output buffer is guaranteed to hold at least 4096 bytes.
    That is an awful lot of 8 byte requests :-)
    
    *********************************/
  
  BOOL PLIDeleteStrokes(dpy,strokeid)
Display *dpy;
CARD16 strokeid;
{ /* PLIDeleteStrokes */
  
  xPLIManageStrokeXYReq    *plimanagestrokexyreq;
  
  int i;
  
#ifdef VERBOSE
  printf("PLIDeleteStrokes: display = %lx\n",(long int)dpy );
#endif
  
  if (PLIBaseReqCode == 0)
    { /* this process has not initialized */
      if (PLIRegisterExtension(dpy) == NULL)
	return(False);
    } /* this process has not initialized */
  
  /* Preamble */
  LockDisplay(dpy);
  /* Preamble */
  
  GetReq(PLIManageStrokeXY,plimanagestrokexyreq);
  plimanagestrokexyreq->reqType = PLIBaseReqCode;
  plimanagestrokexyreq->extReqType = X_PLIManageStrokeXY;
  plimanagestrokexyreq->strokeID = strokeid;
  plimanagestrokexyreq->subReq = ACCEPT_STROKE;
  
  XFlush(dpy);
  
  /* Postamble */
  UnlockDisplay(dpy);
  SyncHandle();
  /* Postamble */
  
  return(True);
  
  
} /* PLIDeleteStrokes */

/*********************************
  /*********************************
    
    PLIAlign
    
    if extension is not registered
    register extension
    request alignment
    Flush the output buffer
    
    
    Note   The output buffer is guaranteed to hold at least 4096 bytes.
    That is an awful lot of 8 byte requests :-)
    
    *********************************/
  
  BOOL PLIAlign(dpy)
Display *dpy;

{ /* PLIAlign */
  
  xPLIAlignReq    *plialignreq;
  
#ifdef VERBOSE
  printf("PLIAlign: display = %lx\n",(long int)dpy );
#endif
  
  if (PLIBaseReqCode == 0)
    { /* this process has not initialized */
      if (PLIRegisterExtension(dpy) == NULL)
	return(False);
    } /* this process has not initialized */
  
  /* Preamble */
  LockDisplay(dpy);
  /* Preamble */
  
  GetReq(PLIAlign,plialignreq);
  
  plialignreq->reqType = PLIBaseReqCode;
  plialignreq->extReqType = X_PLIAlign;
  
  XFlush(dpy);
  
  /* Postamble */
  UnlockDisplay(dpy);
  SyncHandle();
  /* Postamble */
  
  return(True);
  
  
} /* PLIAlign */


/*********************************
  /*********************************
    
    PLISetPointerButton
    
    if extension is not registered
    register extension
    set current active pointer button
    Flush the output buffer
    
    
    Note   The output buffer is guaranteed to hold at least 4096 bytes.
    That is an awful lot of 8 byte requests :-)
    
    *********************************/
  
  BOOL PLISetPointerButton(dpy,button)
Display *dpy;
int button;

{ /* PLIpm */
  
  xPLISetStrokeAttrReq    *plisetstrokeattrreq;
  
#ifdef VERBOSE
  printf("PLISetPointerButton: display = %lx, button = %d\n",(long int)dpy,button );
#endif
  
  if (PLIBaseReqCode == 0)
    { /* this process has not initialized */
      if (PLIRegisterExtension(dpy) == NULL)
	return(False);
    } /* this process has not initialized */
  
  /* Preamble */
  LockDisplay(dpy);
  /* Preamble */
  
  GetReq(PLISetStrokeAttr,plisetstrokeattrreq);
  
  plisetstrokeattrreq->reqType = PLIBaseReqCode;
  plisetstrokeattrreq->extReqType = X_PLISetStrokeAttr;
  plisetstrokeattrreq->valueMask = STROKE_ATTR_BUTTON;

  /* set the button here */
    
  XFlush(dpy);
  
  /* Postamble */
  UnlockDisplay(dpy);
  SyncHandle();
  /* Postamble */
  
  return(True);
  
  
} /* PLISetPointerButton */


/*********************************
  /*********************************
    
    PLIEnqueueStrokes
    
    if extension is not registered
    register extension
    for each stroke event
    request stroke to be sent to server
    Flush the output buffer
    
    Note   The packdata macro will worry about over stuffing the buffer and
    will flush if it becomes full.
    
    *********************************/
  
  BOOL PLIEnqueueStrokes(dpy,strokePP,n)
Display *dpy;
PLIStroke **strokePP;
int n;
{ /* PLIEnqueueStrokes */
  
  xPLIEnqueueStrokeReq    *plienqueuestrokereq;
  
  int i;
  
#ifdef VERBOSE
  printf("PLIEnqueueStrokes: display = %lx\n",(long int)dpy );
#endif
  
  if (PLIBaseReqCode == 0)
    { /* this process has not initialized */
      if (PLIRegisterExtension(dpy) == NULL)
	return(False);
    } /* this process has not initialized */
  
  /* Preamble */
  LockDisplay(dpy);
  /* Preamble */
  
  for (i=0; i < n; i++) 
    { /* for each stroke structure */
      GetReq(PLIEnqueueStroke,plienqueuestrokereq);
      
      plienqueuestrokereq->reqType = PLIBaseReqCode;
      plienqueuestrokereq->extReqType = X_PLIEnqueueStroke;
      plienqueuestrokereq->length += strokePP[i]->numXYs;
      
      
      /* TODO fill in static part of xPLIstroke for enqueue */
      
      plienqueuestrokereq->stroke.strktype = strokePP[i]->strktype;
      plienqueuestrokereq->stroke.minX     = strokePP[i]->minX;
      plienqueuestrokereq->stroke.maxX     = strokePP[i]->maxX;
      plienqueuestrokereq->stroke.minY     = strokePP[i]->minY;
      plienqueuestrokereq->stroke.maxY     = strokePP[i]->maxY;
      plienqueuestrokereq->stroke.maxY     = strokePP[i]->maxY;
      plienqueuestrokereq->stroke.prevsecs = strokePP[i]->prevsecs;
      plienqueuestrokereq->stroke.prevnsec = strokePP[i]->prevnsec;
      plienqueuestrokereq->stroke.timesecs = strokePP[i]->timesecs;
      plienqueuestrokereq->stroke.timensec= strokePP[i]->timensec;
      plienqueuestrokereq->stroke.numXYs   = strokePP[i]->numXYs;
      
      
      Data(dpy,(char *) &(strokePP[i]->xys[0]),strokePP[i]->numXYs*sizeof(t_pliPoint) );
      
    } /* for each stroke event */
  
  XFlush(dpy);
  
  /* Postamble */
  UnlockDisplay(dpy);
  SyncHandle();
  /* Postamble */
  
  return(True);
  
} /* PLIEnqueueStrokes */





/*********************************
  /*********************************
    
    PLIMoveStrokeToClient
    
    
    if extension is not registered
    register extension
    request stroke structure for event with remove
    put reply structure into stroke event
    
    
    *********************************/
  
  BOOL PLIMoveStrokeToClient(dpy,w,strokeID,strokeP, nxys)
Display *dpy;
Window w;
CARD16 strokeID;
xPLIStroke *strokeP;
int nxys;               /* number of xy pairs expected in reply */ 

{ /* PLIMoveStrokeToClient */
  
  xPLIManageStrokeXYReq    *plimanagestrokexyreq;
  xPLIManageStrokeXYReply  plimanagestrokexyreply;
  
#ifdef VERBOSE
  printf("PLIMoveStrokeToClient: display = %lx\n",(long int)dpy );
#endif
  
  if (PLIBaseReqCode == 0)
    { /* this process has not initialized */
      if (PLIRegisterExtension(dpy) == NULL)
	return(False);
    } /* this process has not initialized */
  
  /* Preamble */
  LockDisplay(dpy);
  /* Preamble */
  
  GetReq(PLIManageStrokeXY,plimanagestrokexyreq);
  
  plimanagestrokexyreq->reqType = PLIBaseReqCode;
  plimanagestrokexyreq->extReqType = X_PLIManageStrokeXY;
  plimanagestrokexyreq->window  = w;
  plimanagestrokexyreq->strokeID = strokeID;
  plimanagestrokexyreq->subReq = ACCEPT_AND_COPY_STROKE;
  plimanagestrokexyreq->startIndex = 0;
  plimanagestrokexyreq->endIndex = 0;

  /* 
   * First tell _XReply to read the complete stroke reply; this number is the 
   * number of 32-bit words above the 8 words in a standard xReply.
   */
  if (_XReply(dpy, (xReply *)&plimanagestrokexyreply,
	       ((sizeof(xPLIManageStrokeXYReply) - 32) >> 2), xFalse ) == 0)
  {
    UnlockDisplay(dpy);
    SyncHandle();
    return(NULL);
  }
  
  *strokeP = plimanagestrokexyreply.stroke;

  /* 
   * Now tell _XReadPad to read the  variable length data; this is the data 
   * beyond the fixed xPLIManageStrokeXYReply; the length of a 
   * xPLIManageStrokeXYReply in bytes is the length of an xPLIStroke plus the
   * number of xy pairs times 4.  But _XReply has already read the complete 
   * fixed part of the reply, so simply read nxys << 2.
   */
_XReadPad(dpy,(char *)&strokeP[1], ( nxys << 2 ) );

  /* Postamble */
  UnlockDisplay(dpy);
  SyncHandle();
  /* Postamble */
  
  return(True);
  
} /* PLIMoveStrokeToClient */





/*********************************
  /*********************************
    
    PLIRejectStroke
    
    
    if extension is not registered
    register extension
    
    Empty clients queue of all stroke data
    request reject of the indicated stroke event
    
    
    note  The server upon receipt will automatically remove all prior stroke
    events and reroute the indicated event and all following to the
    next client
    
    *********************************/
  
BOOL PLIRejectStroke(dpy,strokeID)
Display *dpy;
CARD16 strokeID;
{ /* PLIRejectStroke */
  
  xPLIManageStrokeXYReq    *plimanagestrokexyreq;
  XEvent strokeevent, *tmp;
  
#ifdef VERBOSE
  printf("PLIRejectStroke: display = %lx\n",(long int)dpy );
#endif
  
  if (PLIBaseReqCode == 0)
    { /* this process has not initialized */
      if (PLIRegisterExtension(dpy) == NULL)
	return(False);
    } /* this process has not initialized */

  XSync(dpy,FALSE);

  /* SEARCH QUEUE AND DELETE ALL EVENTS FOUND */
  while( XCheckTypedEvent(dpy,PLIBaseEvent, &strokeevent) )
    ;
  
  /* Preamble */
  LockDisplay(dpy);
  /* Preamble */
  
  GetReq(PLIManageStrokeXY,plimanagestrokexyreq);
  
  plimanagestrokexyreq->reqType = PLIBaseReqCode;
  plimanagestrokexyreq->extReqType = X_PLIManageStrokeXY;
  plimanagestrokexyreq->strokeID = strokeID;
  plimanagestrokexyreq->subReq = REJECT_STROKE;

  XFlush(dpy);

  /* Postamble */
  UnlockDisplay(dpy);
  SyncHandle();
  /* Postamble */
  
  return(True);
  
} /* PLIRejectStroke */


Bool PLISetWireToHost(dpy,re,event)
     Display *dpy;
     XStrokeEvent *re;
     xStrokeEvent *event;
{
  
  
  re->type = (int) event->type;
  re->serial = (long) event->sequenceNumber;
  re->send_event = False;
  re->display = dpy;
  re->window = event->eventWin;
  re->detail = event->detail;
  re->time   = event->time;
  re->strokeID = event->strokeID;
  re->numXYs   = event->numXYs;
  re->startX   = event->startX;
  re->startY   = event->startY;
  re->endX     = event->endX;
  re->endY     = event->endY;
  re->minX     = event->minX;
  re->minY     = event->minY;
  re->maxX     = event->maxX;
  re->maxY     = event->maxY;
  
  
  return(True);
  
  
}     
