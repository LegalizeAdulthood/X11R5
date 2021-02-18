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

/* AIX includes */
#include <memory.h>      /* for memcpy  */
#include <errno.h>       /* for kernel errors */
/* X11 includes */
#include "X.h"
#define NEED_REPLIES
#define NEED_EVENTS
#include "Xproto.h"
#include "servermd.h"
#include "windowstr.h"    /* for window stuff */
#include "os.h"           /* alloc macros */
#include "gcstruct.h"
#include "input.h"
#include "inputstr.h"
#include "dix.h"
#include "dixstruct.h"
#include "scrnintstr.h"
#include "extnsionst.h"
#include "PLIproto.h"
#include "xpliDevStr.h"

/* exported stuff for xpliEvents.c */
int xpliStrokeReq; /* major request code for extension */
int xpliStrokeEvent; /* extension event number for xpli */
int xpliErrorCode;   /* extension error code for xpli */
int unsigned long xpliStrokeMask;
unsigned char xpliPointerButton = Button1;

/* imported stuff from xpliEvents.c */
extern int DoGrabOnWindow();
extern int CopyStrokeToClient();

/* static variables */
static t_xpliDevStrP xpliDevStrP;
static int  PLIStrokeReq;
static int  PLIStrokeError;
static Mask PLIStrokeEventMask;
static ExtensionEntry *xpliExtension; /* filled in by AddExtension */
static unsigned short grabs[32], numGrabs=0;
static int wantXpli = 0;

#define AllModifiersMask ( ShiftMask | LockMask | ControlMask | \
			  Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask \
			  | Mod5Mask )

#define rClient(obj) (clients[CLIENT_ID((obj)->resource)])

static int 
xpliGrabStrokeProc(client)
     ClientPtr client;
{ WindowPtr wP;
  REQUEST(xPLIGrabStrokeReq);
  REQUEST_AT_LEAST_SIZE(xPLIGrabStrokeReq);
  client->errorValue = stuff->window;
  if (!(wP = LookupWindow(stuff->window, client))) return (BadWindow);
  return DoGrabOnWindow(wP, 1);
}

static int 
xpliUngrabStrokeProc(client)
     ClientPtr client;
{ WindowPtr wP;
  REQUEST(xPLIGrabStrokeReq);
  REQUEST_AT_LEAST_SIZE(xPLIGrabStrokeReq);
  client->errorValue = stuff->window;
  if (!(wP = LookupWindow(stuff->window, client))) return (BadWindow);
  return DoGrabOnWindow(wP, 0);
}

static int 
xpliAlignProc(client)
     ClientPtr client;
{   
  REQUEST(xPLIAlignReq);
  (xpliDevStrP->alignInit)();
  return(Success);
}

static int 
xpliAlignAbortProc(client)
     ClientPtr client;
{   
  REQUEST(xPLIAlignReq);
  (xpliDevStrP->alignAbort)();
  return(Success);
}

static int 
xpliEnqueueStrokeProc(client)
     ClientPtr client;
{ int mysize;
  xStrokeEvent *MyEventAndStrokeData, *startStroke;
  xPLIStroke *mystroke;
  register t_pliPointP xys;
  
  REQUEST(xPLIEnqueueStrokeReq);
  REQUEST_AT_LEAST_SIZE(xPLIEnqueueStrokeReq);
  client->errorValue = 0;
  /* function not yet implemented */
  return(BadRequest);
}

static int 
xpliConfigProc(client)
     ClientPtr client;
{ xPLIConfigReply reply;
  REQUEST(xPLIConfigReq);
  REQUEST_SIZE_MATCH(xPLIConfigReq);
  reply.type = X_Reply;
  reply.sequenceNumber = client->sequence;
  reply.length = (sizeof(reply) - 32) >> 2;
  reply.eventMask = PLIStrokeEventMask;
  reply.digtype = xpliDevStrP->config.digType;
  reply.scalefac = xpliDevStrP->config.coordScale;
  reply.sampsec = xpliDevStrP->config.samplesSec;
  reply.resolX = xpliDevStrP->config.xResol;
  reply.resolY = xpliDevStrP->config.yResol;
  reply.resolTimeSec = xpliDevStrP->config.timeResolSec;
  reply.resolTimeNsec = xpliDevStrP->config.timeResolNsec;
  if (client->swapped)
    { register int n;
      swaps(&reply.sequenceNumber, n);
      swapl(&reply.length, n);
      swapl(&reply.eventMask, n);
      swapl(&reply.digtype, n);
      swaps(&reply.scalefac, n);
      swaps(&reply.sampsec, n);
      swaps(&reply.resolX, n);
      swaps(&reply.resolY, n);
      swapl(&reply.resolTimeSec, n);
      swapl(&reply.resolTimeNsec, n);
    }
  WriteToClient(client, sizeof(reply), &reply);
  return Success;
}

static int 
xpliManageStrokeProc(client) 
     ClientPtr client; 
{ xPLIManageStrokeXYReply *StrokeReply; 
  WindowPtr wP;  t_strokeQHdrP QHdrP;  int rc;
  extern int acceptStroke(), rejectStroke(), withdrawStroke();
  
  REQUEST(xPLIManageStrokeXYReq);
  REQUEST_SIZE_MATCH(xPLIManageStrokeXYReq);
  if (!(QHdrP = findStrokeInQueue(stuff->strokeID))) {
    client->errorValue = stuff->strokeID;
    return BadIDChoice; }
  if (!(wP = LookupWindow(stuff->window, client))) {
    client->errorValue = stuff->window;
    return BadWindow; }
  if (stuff->subReq & COPY_STROKE) {
    /* copyStrokeToClient may return BadValue error on indices */
    client->errorValue = ((stuff->startIndex)<<16) || stuff->endIndex;
    if ((rc = CopyStrokeToClient(QHdrP, wP, stuff->startIndex,
				 stuff->endIndex)) != Success)
      return rc; }
  /* accept and reject routines may return BadAccess on winID */
  client->errorValue = stuff->window;
  switch (stuff->subReq & ACK_MASK_STROKE) {
  case 0:
    break;
  case 1:
    return acceptStroke(QHdrP, wP);
    break;
  case 2:
    return rejectStroke(QHdrP, wP);
    break;
  case 3:
    return withdrawStroke(QHdrP, wP);
    break; }
}

static int 
xpliSetStrokeAttrProc(client)
     ClientPtr client;
{ long *valueP; int i;
  REQUEST(xPLISetStrokeAttrReq);
  REQUEST_SIZE_MATCH(xPLISetStrokeAttrReq);
  valueP = (long *)(stuff + 1);
  i=0;
  if (stuff->valueMask & STROKE_ATTR_DRAGTIME) {
    if (i >= stuff->length) return BadLength;
    /* convert msec to samples */
    xpliDevStrP->dragThresh = 
      (*valueP++) * xpliDevStrP->config.samplesSec / 1000;
    i++; }
  if (stuff->valueMask & STROKE_ATTR_DRAGX) {
    if (i >= stuff->length) return BadLength;
    xpliDevStrP->dragX = 
      (*valueP++) * xpliDevStrP->config.xResol / 10;
    i++; }
  if (stuff->valueMask & STROKE_ATTR_DRAGY) {
    if (i >= stuff->length) return BadLength;
    xpliDevStrP->dragY = 
      (*valueP++) * xpliDevStrP->config.yResol / 10;
    i++; }
  if (stuff->valueMask & STROKE_ATTR_BUTTON) {
    if (i >= stuff->length) return BadLength;
    if (*valueP < Button1 || *valueP > Button5) { 
      client->errorValue = -(*valueP);
      return BadValue; }
    xpliPointerButton = *valueP++;
    i++; }
  return(Success);
}

static int
xpliGetStrokeAttrProc(client)
     ClientPtr client;
{ xPLIGetStrokeAttrReply reply; register char n; long *valueP;
  REQUEST(xPLIGetStrokeAttrReq);
  reply.type = X_Reply;
  reply.sequenceNumber = client->sequence;
  reply.valueMask = stuff->valueMask;
  reply.length = 0;
  valueP = (long *) &reply.pad4;
  if (stuff->valueMask & STROKE_ATTR_DRAGTIME) {
    *valueP++ = 
      xpliDevStrP->dragThresh * 1000 / xpliDevStrP->config.samplesSec;}
  if (stuff->valueMask & STROKE_ATTR_DRAGX) {
    *valueP++ = xpliDevStrP->dragX * 10 / xpliDevStrP->config.xResol; }
  if (stuff->valueMask & STROKE_ATTR_DRAGY) {
    *valueP++ = xpliDevStrP->dragY * 10 / xpliDevStrP->config.yResol; }
  if (stuff->valueMask & STROKE_ATTR_BUTTON) {
    *valueP++ = xpliPointerButton; }
  if (client->swapped) {
    swaps(&reply.sequenceNumber, n);
    swapl(&reply.length, n);
    swapl(&reply.valueMask, n);
    swapl(&reply.pad4, n);
    swapl(&reply.pad5, n);
    swapl(&reply.pad6, n);
    swapl(&reply.pad7, n);
    swapl(&reply.pad8, n); }
  WriteToClient(client, sizeof(reply), &reply);
  return Success;
}

static int
xpliPendingStrokesProc(clientP)
     ClientPtr clientP;
{
  return SendPendingStrokes(clientP);
}

static int 
xpliDrawStrokeProc(client)
     ClientPtr client;
{ register t_pliPointP xys;
  register int inx;	
  register GC *pGC;
  register DrawablePtr pDraw;
  int nxy;

  REQUEST(xPLIDrawStrokeReq);
  REQUEST_AT_LEAST_SIZE(xPLIDrawStrokeReq);
  if  (stuff->extReqType != X_PLIDrawStroke) {
    client->errorValue = stuff->extReqType;
    return(BadRequest); }
  if ((stuff->coordMode != CoordModeOrigin) &&
      (stuff->coordMode != CoordModePrevious)) {
    client->errorValue = stuff->coordMode;
    return BadValue; }
  VALIDATE_DRAWABLE_AND_GC(stuff->drawable, pDraw, pGC, client);
  nxy = ((stuff->length << 2) - sizeof(xPLIDrawStrokeReq)) >> 2;
  if (nxy) {
    xys = (t_pliPointP)&stuff[1];
    for (inx=0;inx<nxy;inx++ ){
      xys[inx].x = (t_tCoord) ntc2dc(xys[inx].x);
      xys[inx].y = (t_tCoord) ntc2dc(xys[inx].y); }      
    (*pGC->ops->Polylines)(pDraw, pGC, stuff->coordMode, nxy,
			   (xPoint *) &stuff[1]); }
  return (client->noClientException);
}

void SwapXpliStroke(stroke)
xPLIStroke *stroke;
{ register char n; register int i;
  register unsigned short *p, *q;
  swaps (&stroke->startX, n);
  swaps (&stroke->startY, n);
  swaps (&stroke->endX, n);
  swaps (&stroke->endY, n);
  swaps (&stroke->minX, n);
  swaps (&stroke->minY, n);
  swaps (&stroke->maxX, n);
  swaps (&stroke->maxY, n);
  swapl (&stroke->prevsecs, n);
  swapl (&stroke->prevnsec, n);
  swapl (&stroke->timesecs, n);
  swapl (&stroke->timensec, n);
  swapl (&stroke->numXYs, n);
  p = (unsigned short *)(stroke+1);
  q = p + (stroke->numXYs << 1);
  for (; p < q; p++)
	swaps(p,n);
}

static int 
SxpliGrabStrokeProc(client)
     ClientPtr client;
{ register char n;
  REQUEST(xPLIGrabStrokeReq);
  swaps (&stuff->length, n);
  swapl (&stuff->window, n);
  return (xpliGrabStrokeProc(client));
}


static int 
SxpliUngrabStrokeProc(client)
     ClientPtr client;
{ register char n;
  REQUEST(xPLIGrabStrokeReq);
  swaps (&stuff->length, n);
  swapl (&stuff->window, n);
  return (xpliUngrabStrokeProc(client));
}

static int 
SxpliAlignProc(client)
     ClientPtr client;
{ register char n;
  REQUEST(xPLIAlignReq);
  swaps (&stuff->length, n);
  return xpliAlignProc(client);
}

static int 
SxpliAlignAbortProc(client)
     ClientPtr client;
{ register char n;
  REQUEST(xPLIAlignReq);
  swaps (&stuff->length, n);
  return xpliAlignProc(client);
}


static int 
SxpliEnqueueStrokeProc(client)
     ClientPtr client;
{ register char n;
  void SwapXpliStroke();
  REQUEST(xPLIEnqueueStrokeReq);
  swaps (&stuff->length, n);
  SwapXpliStroke(&stuff->stroke);
  SwapRestS(stuff);
  return xpliEnqueueStrokeProc(client);
}


static int 
SxpliManageStrokeProc(client)
     ClientPtr client; 
{ register char n;
  REQUEST(xPLIManageStrokeXYReq);
  swaps (&stuff->length, n);
  swapl (&stuff->window, n);
  swaps (&stuff->strokeID, n);
  swaps (&stuff->startIndex, n);
  swaps (&stuff->endIndex, n);
  return xpliManageStrokeProc(client);
}


static int 
SxpliConfigProc(client)
     ClientPtr client;
{ register char n;
  REQUEST(xPLIConfigReq);
  swaps (&stuff->length, n);
  return xpliConfigProc(client);
}

static int 
SxpliDrawStrokeProc(client)
     ClientPtr client;
{ register char n;
  REQUEST(xPLIDrawStrokeReq);
  swaps(&stuff->length, n);
  swapl(&stuff->drawable, n);
  swapl(&stuff->gc, n);
  SwapRestS(stuff);
  return(xpliDrawStrokeProc(client));
}

static int
SxpliPendingStrokesProc(client)
     ClientPtr client;
{ register char n;
  REQUEST(xPLIPendingStrokesReq);
  return (xpliPendingStrokesProc(client));
}

static int
SxpliSetStrokeAttrProc(client)
     ClientPtr client;
{ register char n;
  REQUEST(xPLISetStrokeAttrReq);
  swaps(&stuff->length, n);
  swapl(&stuff->valueMask, n);
  SwapRestL(stuff);
  return (xpliSetStrokeAttrProc(client));
}

static int
SxpliGetStrokeAttrProc(client)
     ClientPtr client;
{ register char n;
  REQUEST(xPLIGetStrokeAttrReq);
  swaps(&stuff->length, n);
  swapl(&stuff->valueMask, n);
  return (xpliGetStrokeAttrProc(client));
}

static int 
xpliNoop(client)
     ClientPtr client;
{
  client->errorValue = 0;
  return BadRequest;
}

static void
SxpliStrokeEvent(xEP, SxEP)
     xStrokeEvent *xEP, *SxEP;
{ register char n;
  *SxEP = *xEP;
  swaps(SxEP->sequenceNumber, n);
  swapl(SxEP->time, n);
  swapl(SxEP->eventWin, n);
  swaps(SxEP->strokeID, n);
  swaps(SxEP->numXYs, n);
  swaps(SxEP->startX, n);
  swaps(SxEP->startY, n);
  swaps(SxEP->endX, n);
  swaps(SxEP->endY, n);
  swaps(SxEP->minX, n);
  swaps(SxEP->minY, n);
  swaps(SxEP->maxX, n);
  swaps(SxEP->maxY, n);
}

static int (*xpliProcVector[])() = 
{
  xpliNoop,                /* don't use request minor of 0 */
  xpliGrabStrokeProc,      /* 1 */
  xpliUngrabStrokeProc,    /* 2 */
  xpliAlignProc,           /* 3 */
  xpliAlignAbortProc,      /* 4 */
  xpliEnqueueStrokeProc,   /* 5 */
  xpliConfigProc,          /* 6 */
  xpliManageStrokeProc,    /* 7 */
  xpliDrawStrokeProc,       /* 8 */
  xpliPendingStrokesProc,   /* 9 */
  xpliSetStrokeAttrProc,    /* 10 */
  xpliGetStrokeAttrProc     /* 11 */
  };

static int (*SxpliProcVector[])() = 
{
  xpliNoop,                   /* don't use request minor of 0 */
  SxpliGrabStrokeProc,        /* 1 */
  SxpliUngrabStrokeProc,      /* 2 */
  SxpliAlignProc,	      /* 3 */
  SxpliAlignAbortProc,	      /* 4 */
  SxpliEnqueueStrokeProc,     /* 5 */
  SxpliConfigProc,	      /* 6 */
  SxpliManageStrokeProc,      /* 7 */
  SxpliDrawStrokeProc,	      /* 8 */
  SxpliPendingStrokesProc,    /* 9 */
  SxpliSetStrokeAttrProc,     /* 10 */
  SxpliGetStrokeAttrProc      /* 11 */
  };

static int
xpliDispatch(client)
     register ClientPtr client;
{ int errorStatus;
  REQUEST(xReq);
  errorStatus = Success;
  if (stuff->data >= X_PLILastMinorReq) {
    client->errorValue = 0;
    return BadRequest; }
  errorStatus = (*xpliProcVector[stuff->data])(client);
  return errorStatus;
}

static int
xpliSwappedMainProc(client)
register ClientPtr client;
{ int errorStatus;
  REQUEST(xReq);
  errorStatus = Success;
  if (stuff->data >= X_PLILastMinorReq) {
    client->errorValue = 0;
    return BadRequest; }
  errorStatus = (*SxpliProcVector[stuff->data])(client);
  return errorStatus;
}

static int
xpliCloseDownProc()
{
  return(0);
}

void
PLIExtensionInit(screenInfo, argc, argv)
     ScreenInfo *screenInfo;
     int argc;
     char **argv;
{ int i; extern t_xpliDevStrP InitBSTtyDev();
  extern void InitXpliEvents();

  if (!wantXpli) return; /* -pli flag not used on server startup */

  /* setup screens and input devices based on hardware config */
  xpliDevStrP = InitBSTtyDev(argc, argv);
  if (!xpliDevStrP) return;
  InitXpliEvents(xpliDevStrP);

  /* add the XPLI (X11 Paper-Like Interface) extension */
  xpliExtension = AddExtension("xpli", 1 /* events */, 1 /* errors */,
			      xpliDispatch, xpliSwappedMainProc,
			      xpliCloseDownProc, StandardMinorOpcode);
  
  /* save the major request code, event code and error code */
  xpliStrokeReq = xpliExtension->base;
  xpliStrokeEvent = xpliExtension->eventBase;
  xpliErrorCode = xpliExtension->errorBase;
  EventSwapVector[xpliStrokeEvent] = SxpliStrokeEvent;

  /* get an event mask for the stroke event */		 
  xpliStrokeMask = GetNextEventMask();
  SetMaskForEvent(xpliStrokeMask, xpliStrokeEvent);

  return;
  
}

/* This is a new inclusion for X11R5. */
/* the function PLIProcessCommandArgument is called by
 * extProcessCommandArgs() which in turn is called by
 * main() */
int PLIProcessCommandArgument(argc,argv,i)
     int argc ; char **argv; int i;
{
	int skip = 0;
	if (! strcmp(argv[i], "-pli"))
	{
	  	wantXpli = xTrue;
		skip =1;
	}
	return(skip);
}

