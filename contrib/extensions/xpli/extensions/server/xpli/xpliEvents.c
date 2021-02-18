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
  xpliEvents.c - this module handles the receipt of strokes from the
  pli device, and their dispatch to windows. It also determines whether
  mouse emulation is to take place.
*/

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>

#include "X.h"
#define NEED_REPLIES
#define NEED_EVENTS
#include "Xproto.h"
#include "servermd.h"
#include "misc.h"
#include "dixstruct.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "inputstr.h"

#include "PLIproto.h"
#include "xpliDevStr.h"
#include "xpliStrkQHdr.h"

/*----- X EXTERNAL DATA AND PROCEDURES -----*/
extern GetSpriteInfo(); /* xpli addition to events.c */
extern InputInfo inputInfo;

/*----- LOCAL TYPEDEFS & DEFINES -----*/

/* stolen from events.c */
#define rClient(obj) (clients[CLIENT_ID((obj)->resource)])

/*----- XPLI EXTERNAL DATA AND PROCEDURES -----*/

extern int xpliStrokeEvent, xpliErrorCode;
extern unsigned long xpliStrokeMask;
extern unsigned char xpliPointerButton;
extern void SwapXpliStroke();

/*----- STATIC DATA AND PROCEDURES -----*/

/* The stroke headers (defined in xpliStrkQHdr.h as t_strokeQHdr)
   carry five sets of flags that show what disposition has been
   made of the stroke. The flags correspond to slots in the
   window cache (windowPA in the struct below) using the formula
   1<<k, where k is the index in the window cache. Now, because
   of the percolation of events up the window tree and because
   clients can register interest in windows they do not own, it
   is not necessarily the case that the stroke will be dispatched
   to the window it enters. The IsWindowPLIAware function (q.v) finds
   the dispatch target window and client and returns whether a
   stroke or pointer event is wanted. If a stroke event is wanted
   the entered window is added to the winDispFlags in the stroke
   header, but the event is dispatched to the target window and
   client. 

   This design is based on the assumption that an individual stroke
   will never enter more than a few windows. We have not observed this
   happen except when people pick up the pen and randomly scribble
   on the drawing surface. Windows go in the cache only if a stroke
   has entered them, and they disappear from the cache when the
   client accepts or rejects the stroke. If a client is balky,
   the stroke may be forced from the queue. */

static unsigned short   strokeID; /* used to assign strokeIDs */
static t_strokeQHdrP    lastQHdrP; /* most recent enqueued stroke */
static t_strokeQHdrP    firstQHdrP; /* oldest stroke we know about */
static t_strokeQHdrP    prevQHdrP; /* used by Find and Delete */
static WindowPtr        windowPA[32]; /* window cache */
static unsigned short   wRefcntA[32]; /* refcnt for cache entries */
/*? probably should add this to window privates */
static unsigned char    wGrabFlagA[32]; /* Grab set for window */
static t_XYsP           freeXYsP;  /* free queue of XYsStruct's */
static t_strokeQHdrP    freeQHdrP; /* free queue of pliStrokeQHdr's */
static WindowPtr        *PLISpriteTrace;
static int              PLISpriteTraceGood;
static int              PLISpriteTraceSize = 0;
static int              PLISpriteTraceWinAware;
static unsigned long    StrokeGrabMask;
static t_xpliDevStrP    xpliDevStrP;
static char             currentButtonDown;

/* client has requested a passive grab placed on this window. Window must
   be stroke aware and not previously grabbed. */
int
DoGrabOnWindow(wP, onoff)
     WindowPtr wP; int onoff;
{ WindowPtr twP; ClientPtr cP; int k;
  if (IsWindowPLIAware(wP, twP, cP, DET_START) <= 0) return BadAccess;
  if ((k = mustFindWindowInCache(wP)) < 0) return BadAlloc;
  if (onoff) {
    if (wGrabFlagA[k] & 0x01) return BadAccess;
    wGrabFlagA[k] |= 0x01;
    wRefcntA[k]++; }
  else {
    if (!(wGrabFlagA[k] & 0x01)) return BadAccess;
    wGrabFlagA[k] &= ~0x01;
    wRefcntA[k]--; }
  return Success;
}

/* It is not aesthetically pleasing to duplicate here the essential
   window tracking provided by the DIX sprite. However, we don't
   want the side effects of bashing around the DIX sprite while
   we are trying to decide whether to do mouse emulation. For example,
   if a stroke starts in a stroke unaware window that wants pointer
   events, we need to hold off updating the DIX sprite until we decide
   whether to do mouse emulation or not. But, we do need to find the
   window containing the start of the stroke so we can test whether
   it is stroke aware. Hence, we have this separate sprite to track
   the stroke. */
static int
XYToWindow(x,y)
     int x, y;
{ register WindowPtr  pWin, pRootWin; int BW, winX, winY;
  extern WindowPtr WindowTable[];
  if (PLISpriteTraceSize == 0) {
    PLISpriteTraceSize = 10;
    PLISpriteTrace = (WindowPtr *)
      xrealloc(PLISpriteTrace, PLISpriteTraceSize*sizeof(WindowPtr)); 
    *PLISpriteTrace = WindowTable[xpliDevStrP->screenP->myNum]; 
    PLISpriteTraceGood = 0; }
  pWin = *(PLISpriteTrace+PLISpriteTraceGood);
  if (pWin->mapped) {
    BW = pWin->borderWidth;
    winX = pWin->drawable.x;
    winY = pWin->drawable.y;
    if ((x >= (winX - BW)) &&
	(x < (winX + (int)pWin->drawable.width + BW)) &&
	(y >= (winY - BW)) &&
	(y < (winY + (int)pWin->drawable.height + BW)))
      return 0; }
  PLISpriteTraceGood = 0;	/* root window still there */
  pWin = (*PLISpriteTrace)->firstChild;
  while (pWin) {
    if (pWin->mapped) {
      BW = pWin->borderWidth;
      winX = pWin->drawable.x;
      winY = pWin->drawable.y;
      if ((x >= (winX - BW)) &&
	  (x < (winX + (int)pWin->drawable.width + BW)) &&
	  (y >= (winY - BW)) &&
	  (y < (winY + (int)pWin->drawable.height + BW))) {
	if (PLISpriteTraceGood >= (PLISpriteTraceSize-1)) {
	  PLISpriteTraceSize += 10;
	  PLISpriteTrace = (WindowPtr *)
	    xrealloc(PLISpriteTrace, PLISpriteTraceSize*sizeof(WindowPtr)); }
	*(PLISpriteTrace+(++PLISpriteTraceGood)) = pWin;
	pWin = pWin->firstChild; }
      else pWin = pWin->nextSib; } }
}

/* 
 * This routine gets passed the lowest-level window that the x,y falls 
 * inside of and begins its search there for a window that is 
 * interested in either a stroke event or a pointer event.  The routine also 
 * gets passed what type of stroke event it is, and uses this to get the 
 * correct detail for the pointer event.  Each window has an owner client and 
 * then a list of clients other than the owner who have registered an 
 * interest in events occurring inside that window.  The rules for delivery 
 * are as follows:  if the owner wants a stroke event, then it gets a stroke 
 * event.  If the owner doesn't want a stroke event, but an "other" client 
 * does, then the "other" client gets the stroke event.  If nobody wants a 
 * stroke event from this window, then we go through the same process for 
 * pointer events.  If nobody wants pointer events then we try the same 
 * process for the parent of this window. 
 * 
 */
static int
IsWindowPLIAware(pWin, retWin, retCli, detail_flag)
     register WindowPtr pWin;
     WindowPtr *retWin;
     ClientPtr *retCli;
     char detail_flag;
{ Mask PointerFilter;
  extern Mask GetMaskForEvent();
  static Mask MotionNotifyFilter, ButtonPressFilter, ButtonReleaseFilter,
  StrokeFilter;
  static int firstTime = 1;
  int     deliveries;
  struct _OtherClients *other;
  
  if (firstTime) {
    StrokeFilter = GetMaskForEvent(xpliStrokeEvent);
    MotionNotifyFilter = GetMaskForEvent(MotionNotify);
    ButtonPressFilter = GetMaskForEvent(ButtonPress);
    ButtonReleaseFilter = GetMaskForEvent(ButtonRelease);
    firstTime = 0; }
  switch ( detail_flag & 0x3 )    {
    case DET_MOTION:
    case DET_STROKE:
      PointerFilter = MotionNotifyFilter;
      break;
    case DET_START:
      PointerFilter = ButtonPressFilter;
      break;
    case DET_END:
      PointerFilter = ButtonReleaseFilter;
      break;
    case DET_WITHDRAWN:
      PointerFilter = 0; /* not appropriate for stroke unaware windows */
      break;
    }
  /* 
   * now we have the correct pointer mask.  Climb the window tree until we 
   * find a window that wants a stroke event or a pointer event.  Always try 
   * the stroke event first.
   */
  while (pWin) {
    /* see if the window is valid */
    if ((wClient(pWin)) && ( wClient(pWin) != serverClient ) &&
	(wClient(pWin)->clientGone == 0 )) {
      /* see if window wants stroke */
      if (pWin->eventMask & StrokeFilter) {
	*retWin = pWin;
	*retCli = wClient(pWin);
	return 1; }
      /* see if other clients want stroke */
      for (other = wOtherClients(pWin); other; other = other->next)
	if ((rClient(other) != serverClient ) && 
	    ( other->mask & StrokeFilter )) {
	  *retWin = pWin;
	  *retCli = rClient(other);
	  return 1; }
      /* see if window wants corresponding mouse event */
      if (pWin->eventMask & PointerFilter ) {
	return -1; }
      /* see if other clients want corresponding mouse event */
      for ( other = wOtherClients(pWin); other; other = other->next )
	if (( rClient(other) != serverClient ) && 
	    ( rClient(other)->clientGone == 0 ) && 
	    ( other->mask & PointerFilter )) {
	  return -1; }
      if ( StrokeFilter & wDontPropagateMask(pWin))
	return 0; /* nobody wants strokes or mouse events */
      pWin = pWin->parent; } }
  return 0; /* highly unlikely to get here */
}

/* derive a stroke event from the QHdr
   that is relative to a specific window. The sequenceNumber
   is left unfilled. Only the event type is taken from
   the detail parameter. The allin is a boolean that is true
   if the entire stroke falls within the window. In many
   cases, there is a simple test for this, which is to see
   if the winEnterMask in the QHdr is equal to the winMask
   of the wP window. This equality implies that the stroke
   has entered only the window to which the event is being
   dispatched. */
static void
FixupStrokeEventToClient(xEP, wP, QHdrP, detail, allin)
     xStrokeEvent *xEP; WindowPtr wP; t_strokeQHdrP QHdrP;
     unsigned char detail; int allin;
{ int winX, winY, winB;
  xEP->type = xpliStrokeEvent; /* event code assigned by AddExtension */
  xEP->eventWin = wP->drawable.id;
  xEP->strokeID = QHdrP->strokeID;
  xEP->numXYs = QHdrP->stroke.numXYs;
  winX = wP->drawable.x;
  winY = wP->drawable.y;
  winB = wBorderWidth(wP);
  xEP->startX = QHdrP->stroke.startX - winX;
  xEP->endX = QHdrP->stroke.endX - winX;
  xEP->minX = QHdrP->stroke.minX - winX;
  xEP->maxX = QHdrP->stroke.maxX - winX;
  xEP->startY = QHdrP->stroke.startY - winY;
  xEP->endY = QHdrP->stroke.endY - winY;
  xEP->minY = QHdrP->stroke.minY - winY;
  xEP->maxY = QHdrP->stroke.maxY - winY;
  xEP->detail = detail & DET_TYPE_MASK;
  if (allin) xEP->detail |= DET_ALL_IN;
  else {
    xEP->detail |= DET_INOUT_SEOUT;
    if ((xEP->startX >= winB) &&
	(xEP->startX < (int)wP->drawable.width + winB) &&
	(xEP->startY >= winB) &&
	(xEP->startY < (int)wP->drawable.height + winB))
      xEP->detail |= DET_STRT_IN;
    if ((xEP->endX >= winB) &&
	(xEP->endX < (int)wP->drawable.width + winB) &&
	(xEP->endY >= winB) &&
	(xEP->endY < (int)wP->drawable.height + winB))
      xEP->detail |= DET_END_IN; }
}
	     
static int
DelStroke(QHdrP)
     t_strokeQHdrP QHdrP;
{ t_XYsP XYsP, tmpXYsP;
  unsigned int k, i, j; unsigned long winMask;
  /* NOTE: assumes findStrokeInQueue has set prevQHdrP */
  if (prevQHdrP) {
    prevQHdrP->nextP = QHdrP->nextP; }
  else {
    if (!(firstQHdrP = QHdrP->nextP))
      lastQHdrP = (t_strokeQHdrP)0; } 
  XYsP = QHdrP->XYsP;
  (xpliDevStrP->initInkPoint)(0, XYsP->xy[0].x, XYsP->xy[0].y);
  j=1;
  for (i=QHdrP->stroke.numXYs-1; i; i--) {
    (xpliDevStrP->ink)(XYsP->xy[j].x, XYsP->xy[j].y);
    if ((++j) >= 128) {
      j = 0; 
      tmpXYsP = XYsP->nextP; 
      XYsP->nextP = freeXYsP;
      freeXYsP = XYsP;
      XYsP = tmpXYsP; }
    QHdrP->nextP = freeQHdrP;
    freeQHdrP = QHdrP; }
  (xpliDevStrP->endInk)();
  for (k=0, winMask=1; k<32; k++, winMask <<=1) {
    if (winMask & QHdrP->winEnterFlags)
      if (--wRefcntA[k])
	windowPA[k] = (WindowPtr) 0; }
  return Success;
}

static void
AddQHdrToQueue(QHdrP)
     t_strokeQHdrP QHdrP;
{
  QHdrP->nextP = (t_strokeQHdrP) 0;
  if (lastQHdrP) {
    lastQHdrP->nextP = QHdrP; }
  else firstQHdrP = QHdrP;
  lastQHdrP = QHdrP;
  QHdrP->strokeID = strokeID++;
}

/* let the client know that server has deleted stroke - probably
   because client waited too long to accept or reject stroke */
static void
ServerDeleteEvent(QHdrP, windowP)
     t_strokeQHdrP QHdrP; WindowPtr windowP;
{ xStrokeEvent xSE; ClientPtr clientP; WindowPtr cliWindowP;
  IsWindowPLIAware(windowP, &cliWindowP, &clientP, DET_WITHDRAWN);
  xSE.type = xpliStrokeEvent;
  xSE.detail = DET_WITHDRAWN; /* server deleted stroke */
  xSE.sequenceNumber = clientP->sequence;
  xSE.time = GetTimeInMillis();
  xSE.eventWin = cliWindowP->drawable.id;
  xSE.strokeID = QHdrP->strokeID;
  xSE.numXYs = 0;
  xSE.startX = xSE.startY = xSE.endX = xSE.endY = xSE.minX = xSE.minY =
    xSE.maxX = xSE.maxY = 0;
  WriteEventsToClient(clientP, 1, &xSE);
}

/* the client has deleted this window, drop it from the table
   and scan the stroke queue to delete the dispatch flag bits.
   If a stroke has entered no windows after this one is
   deleted, send a server delete event to the client */
Bool
pliDestroyWindow(windowP)
     WindowPtr windowP;
{ unsigned int k, i, j; unsigned long mask; WindowPtr tmpWindowP;
  t_strokeQHdrP tmpQHdrP; t_XYsP XYsP, tmpXYsP;
  extern Bool ppcDestroyWindow();

  k = (((unsigned int) windowP) >> 3) & 0x3F;
  for (i=0; i<32; i++) {
    if (windowPA[k] == windowP)
      break;
    k = (k+1)&0x3F; }
  if (i < 32) {
    mask = 1<<k;
    wGrabFlagA[k] = 0; /* turn off the grab flag */
    StrokeGrabMask &= ~mask;
    for (tmpQHdrP=firstQHdrP, prevQHdrP=(t_strokeQHdrP)0; 
	 tmpQHdrP; prevQHdrP=tmpQHdrP, tmpQHdrP=tmpQHdrP->nextP) {
      tmpQHdrP->winEnterFlags &= ~mask;
      if (tmpQHdrP->winDispFlags & mask) 
	ServerDeleteEvent(tmpQHdrP, windowP); /* notify client of deletion */
      tmpQHdrP->winDispFlags &= ~mask; 
      tmpQHdrP->winPtrFlags &= ~mask; 
      tmpQHdrP->winAcceptFlags &= ~mask; 
      tmpQHdrP->winRejectFlags &= ~mask;
      if (!tmpQHdrP->winDispFlags) {
	if (prevQHdrP) {
	  prevQHdrP->nextP = tmpQHdrP->nextP; }
	else {
	  if (!(firstQHdrP = tmpQHdrP->nextP))
	    lastQHdrP = (t_strokeQHdrP)0; } 
	XYsP = tmpQHdrP->XYsP;
	(xpliDevStrP->initInkPoint)(0, XYsP->xy[0].x, XYsP->xy[0].y);
	j=1;
	for (i=tmpQHdrP->stroke.numXYs-1; i; i--) {
	  (xpliDevStrP->ink)(XYsP->xy[j].x, XYsP->xy[j].y);
	  if ((++j) >= 128) {
	    j = 0; 
	    tmpXYsP = XYsP->nextP; 
	    XYsP->nextP = freeXYsP;
	    freeXYsP = XYsP;
	    XYsP = tmpXYsP; }
	  tmpQHdrP->nextP = freeQHdrP;
	  freeQHdrP = tmpQHdrP; }
	windowPA[k] = (WindowPtr) 0;
	wRefcntA[k] = 0; } } }
  return ppcDestroyWindow(windowP);
}

/* when a stroke enters a window different from the previous one, this
   function determines if the window has been entered before by the
   stroke. If not, and the window is accepting strokes, it is sent
   a start event for the stroke. and is added to the list of windows
   to which the stroke has been dispatched. This function is nonzero if
   a start event was dispatched (i.e. the window was accepting strokes)
   and zero otherwise. */
static int
AddWindowToStroke(QHdrP, windowP)
     t_strokeQHdrP QHdrP; WindowPtr windowP;
{ unsigned int k, fk; int i, winAware; xEvent xE;
  WindowPtr tmpWindowP, cliWindowP;
  unsigned long winMask; ClientPtr clientP;

  if ((k = mustFindWindowInCache(windowP)) < 0) return -1;
  winMask = 1<<k;
  if (QHdrP->winEnterFlags & winMask) return 0;
  /* stroke has not entered this window before */
  QHdrP->winEnterFlags |= winMask;
  wRefcntA[k]++;
  /* if client not accepting stroke events, we're done */
  winAware = 
    IsWindowPLIAware(tmpWindowP, &cliWindowP, &clientP, DET_START);
  if (winAware == 1) {
    QHdrP->winDispFlags |= winMask;
    if (wGrabFlagA[k] & 0x01) StrokeGrabMask |= winMask;
    FixupStrokeEventToClient(&xE, cliWindowP, QHdrP, DET_START,
			     QHdrP->winEnterFlags == winMask);
    xE.u.u.sequenceNumber = clientP->sequence;
    WriteEventsToClient(clientP, 1, &xE); }
  return winAware;
}

/* if the digitizer reports proximity, this causes a motion event
   to be sent to the client in whose window the pen currently resides.
   Proximity events consist only of a single point so they cannot
   be routed to multiple windows. The strokeID in a prox event will
   always be 0, and there is no stroke to be retrieved or managed. */
static void
proxReport(xpliDevP, x, y)
     t_xpliDevStrP xpliDevP;
     int x, y; /* NOTE x,y are scaled display coords */
{ xStrokeEvent xE; xEvent xEPointer; int tmpx, tmpy, winAware;
  WindowPtr wP; ClientPtr cP; Time time;
  
  time = GetTimeInMillis();
  if (XYToWindow(x, y)) {
    /* only recompute window awareness if XYToWindow says window changed */
    PLISpriteTraceWinAware = 
      IsWindowPLIAware(*(PLISpriteTrace+PLISpriteTraceGood), 
		       &wP, &cP, DET_START); }
  if (PLISpriteTraceWinAware == 1) {
    xE.type = xpliStrokeEvent; /* event code assigned by AddExtension */
    xE.detail = DET_MOTION | DET_ALL_IN;
    xE.eventWin = wP->drawable.id;
    xE.time = time;
    xE.sequenceNumber = cP->sequence;
    xE.strokeID = 0; /* prox has no stroke ID */
    xE.numXYs = 0; /* no stroke => no length */
    xE.startX = xE.endX = xE.minX = xE.maxX = 
      ntc2dc(x) - wP->drawable.x;
    xE.startY = xE.endY = xE.minY = xE.maxY =
      ntc2dc(y) - wP->drawable.y;
    WriteEventsToClient(cP, 1, &xE); }
  else {
    xEPointer.u.u.type = MotionNotify;
    xEPointer.u.keyButtonPointer.time = time;
    xEPointer.u.keyButtonPointer.rootX = ntc2dc(x); 
    xEPointer.u.keyButtonPointer.rootY = ntc2dc(y);
    /* ship out a motion event to get DIX sprite up to date */
    (*inputInfo.pointer->public.processInputProc)
      (&xEPointer, inputInfo.pointer, 1); }
}

static void
startStroke(xpliDevP, QHdrP)
     t_xpliDevStrP xpliDevP; t_strokeQHdrP QHdrP;
{ xEvent xEPointer; WindowPtr wP; ClientPtr cP; int i;
  unsigned long mask;

  QHdrP->startTime = GetTimeInMillis();
  if (XYToWindow(QHdrP->stroke.startX, QHdrP->stroke.startY)) {
    PLISpriteTraceWinAware = 
      IsWindowPLIAware(*(PLISpriteTrace+PLISpriteTraceGood), 
		       &wP, &cP, DET_START); }
  if (PLISpriteTraceWinAware <= 0) { QHdrP->mouseEm = 1; return; }
  /* going to do stroke routing */
  AddQHdrToQueue(QHdrP); /* add the QHdr and assign it a unique ID */
  xEPointer.u.u.type = MotionNotify;
  xEPointer.u.keyButtonPointer.time = QHdrP->startTime;
  xEPointer.u.keyButtonPointer.rootX = ntc2dc(QHdrP->stroke.startX); 
  xEPointer.u.keyButtonPointer.rootY = ntc2dc(QHdrP->stroke.startY);
  /* ship out a motion event to get DIX sprite up to date */
  (*inputInfo.pointer->public.processInputProc)(&xEPointer, 
						inputInfo.pointer, 1);
  AddWindowToStroke(QHdrP, *(PLISpriteTrace+PLISpriteTraceGood));
  if (StrokeGrabMask)
    for (i=0, mask = 1; i<32; i++, mask<<=1)
      if (StrokeGrabMask & mask)
	AddWindowToStroke(QHdrP, windowPA[i]);
}

static void
inStroke(xpliDevP, QHdrP)
     t_xpliDevStrP xpliDevP; t_strokeQHdrP QHdrP;
{ xEvent xEPP[2]; WindowPtr prevWindow, wP; ClientPtr cP; Time time;
  int i; unsigned long mask; xStrokeEvent xE;

  QHdrP->endTime = GetTimeInMillis();
  if (XYToWindow(QHdrP->stroke.endX, QHdrP->stroke.endY)) {
    PLISpriteTraceWinAware = 
      IsWindowPLIAware(*(PLISpriteTrace+PLISpriteTraceGood), 
		       &wP, &cP, DET_START); }
  if (QHdrP->mouseEm == 1) { /* drag drop candidate */
    if (PLISpriteTraceWinAware <= 0) {
      if (QHdrP->stroke.numXYs <= xpliDevStrP->dragThresh) return;
      else {
	if (((QHdrP->stroke.maxX - QHdrP->stroke.minX) 
	     < xpliDevStrP->dragX) &&
	    ((QHdrP->stroke.maxY - QHdrP->stroke.minY) 
	     < xpliDevStrP->dragY)) {
	  QHdrP->mouseEm = 2; /* force mousing */
	  xEPP[1].u.u.type = MotionNotify;
	  xEPP[1].u.keyButtonPointer.time = QHdrP->startTime;
	  xEPP[1].u.keyButtonPointer.rootX = ntc2dc(QHdrP->stroke.startX); 
	  xEPP[1].u.keyButtonPointer.rootY = ntc2dc(QHdrP->stroke.startY);
	  /* ship out a motion event to get DIX sprite up to date */
	  (*inputInfo.pointer->public.processInputProc)
	    (&xEPP[1], inputInfo.pointer, 1);
	  xEPP[1].u.u.type = ButtonPress;
	  xEPP[1].u.u.detail = currentButtonDown = xpliPointerButton;
	  /* ship out a button down event */
	  (*inputInfo.pointer->public.processInputProc)
	    (&xEPP[1], inputInfo.pointer, 1);
	  return; } } }
    AddQHdrToQueue(QHdrP); QHdrP->mouseEm = 0; 
    if (StrokeGrabMask)
      for (i=0, mask=1; i<32; i++, mask<<=1)
        if (StrokeGrabMask & mask)
	  AddWindowToStroke(QHdrP, windowPA[i]); }
  xEPP[0].u.u.type = MotionNotify;
  xEPP[0].u.keyButtonPointer.time = QHdrP->endTime;
  xEPP[0].u.keyButtonPointer.rootX = ntc2dc(QHdrP->stroke.endX); 
  xEPP[0].u.keyButtonPointer.rootY = ntc2dc(QHdrP->stroke.endY);
  /* ship out a motion event to get DIX sprite up to date */
  (*inputInfo.pointer->public.processInputProc)
    (&xEPP[0], inputInfo.pointer, 1); 
  if (QHdrP->mouseEm == 2) return;
  AddWindowToStroke(QHdrP, *(PLISpriteTrace+PLISpriteTraceGood));
  for (i=0, mask=1; i<32; i++, mask<<=1) {
    if (QHdrP->winDispFlags & mask) {
      IsWindowPLIAware(windowPA[i], wP, cP, DET_STROKE);
      FixupStrokeEventToClient(&xE, wP, QHdrP, DET_STROKE, 
			       (QHdrP->winEnterFlags == mask));
      xE.time = QHdrP->endTime;
      xE.sequenceNumber = cP->sequence;
      WriteEventsToClient(cP, 1, &xE); }}
}

static void
endStroke(xpliDevP, QHdrP)
     t_xpliDevStrP xpliDevP; t_strokeQHdrP QHdrP;
{ xEvent xEPP[2]; WindowPtr prevWindow, wP; ClientPtr cP;
  t_pliPoint m; t_XYsP XYsP; xStrokeEvent xE;
  int i; unsigned long mask;

  QHdrP->endTime = GetTimeInMillis();
  if (XYToWindow(QHdrP->stroke.endX, QHdrP->stroke.endY)) {
    PLISpriteTraceWinAware = 
      IsWindowPLIAware(*(PLISpriteTrace+PLISpriteTraceGood), 
		       &wP, &cP, DET_START); }
  if (QHdrP->mouseEm == 1) {
    if (PLISpriteTraceWinAware <= 0) {
      if (QHdrP->stroke.numXYs <= xpliDevStrP->dragThresh) return;
      else {
	if (((QHdrP->stroke.maxX - QHdrP->stroke.minX) 
	     < xpliDevStrP->dragX) &&
	    ((QHdrP->stroke.maxY - QHdrP->stroke.minY) 
	     < xpliDevStrP->dragY)) {
	  QHdrP->mouseEm = 2; /* force mousing */
	  xEPP[1].u.u.type = MotionNotify;
	  xEPP[1].u.keyButtonPointer.time = QHdrP->startTime;
	  xEPP[1].u.keyButtonPointer.rootX = ntc2dc(QHdrP->stroke.startX); 
	  xEPP[1].u.keyButtonPointer.rootY = ntc2dc(QHdrP->stroke.startY);
	  /* ship out a motion event to get DIX sprite up to date */
	  (*inputInfo.pointer->public.processInputProc)
	    (&xEPP[1], inputInfo.pointer, 1);
	  xEPP[1].u.u.type = ButtonPress;
	  xEPP[1].u.u.detail = currentButtonDown = xpliPointerButton;
	  xEPP[1].u.keyButtonPointer.rootX = ntc2dc(QHdrP->stroke.startX);
	  xEPP[1].u.keyButtonPointer.rootY = ntc2dc(QHdrP->stroke.startY);
	  /* ship out a button down event */
	  (*inputInfo.pointer->public.processInputProc)
	    (&xEPP[1], inputInfo.pointer, 1); } } }
    QHdrP->mouseEm = 0; AddQHdrToQueue(QHdrP);
    if (StrokeGrabMask)
      for (i=0, mask=1; i<32; i++, mask<<=1)
        if (StrokeGrabMask & mask)
	  AddWindowToStroke(QHdrP, windowPA[i]); }
  xEPP[0].u.u.type = MotionNotify;
  xEPP[0].u.keyButtonPointer.time = QHdrP->endTime;
  xEPP[0].u.keyButtonPointer.rootX = ntc2dc(QHdrP->stroke.endX); 
  xEPP[0].u.keyButtonPointer.rootY = ntc2dc(QHdrP->stroke.endY);
  /* ship out a motion event to get DIX sprite up to date */
  (*inputInfo.pointer->public.processInputProc)
    (&xEPP[0], inputInfo.pointer, 1);
  if (QHdrP->mouseEm == 2) {
    xEPP[0].u.u.type = ButtonRelease;
    xEPP[0].u.u.detail = currentButtonDown;
    xEPP[0].u.keyButtonPointer.rootX = ntc2dc(QHdrP->stroke.endX); 
    xEPP[0].u.keyButtonPointer.rootY = ntc2dc(QHdrP->stroke.endY);
    /* ship out a motion event to get DIX sprite up to date */
    (*inputInfo.pointer->public.processInputProc)
      (&xEPP[0], inputInfo.pointer, 1);
    DelStroke(QHdrP);
    return; }
  for (i=0, mask=1; i<32; i++, mask<<=1)
    if (QHdrP->winDispFlags & mask) {
      IsWindowPLIAware(windowPA[i], &wP, &cP, DET_END);
      FixupStrokeEventToClient(&xE, wP, QHdrP, DET_END, 
			       (QHdrP->winEnterFlags == mask));
      xE.sequenceNumber = cP->sequence;
      WriteEventsToClient(cP, 1, &xE); }
}

int
findWindowInCache(windowP)
     WindowPtr windowP;
{ unsigned int k, i; WindowPtr tmpWindowP;
  k = (((unsigned int) windowP) >> 3) & 0x3F;
  for (i=0; i<32; i++) {
    if (windowPA[k] == windowP) return k;
    k = (k+1)&0x3F; }
  return -1; /* window not in cache */
}

int
mustFindWindowInCache(windowP)
     WindowPtr windowP;
{ unsigned int k, i, fk, j; t_strokeQHdrP tmpQHdrP;
  unsigned long mask;
  for (;;) { /* loop until window is found */
    fk = -1;
    k = (((unsigned int) windowP) >> 3) & 0x3F;
    for (i=0; i<32; i++) {
      if (windowPA[k] == windowP)
	break;
      if ((fk < 0) && !wRefcntA[k]) fk = k;
      k = (k+1)&0x3F; }
    if (i < 32) break; /* found the window */
    if (fk >= 0) { /* window cache is not full */
      k = fk;
      windowPA[k] = windowP;
      wRefcntA[k] = 0;
      wGrabFlagA[k] = 0;
      break; }
    /* window table is full, throw out strokes until we get an empty slot */
    if (firstQHdrP == lastQHdrP) return -1;
    tmpQHdrP = firstQHdrP; 
    prevQHdrP = (t_strokeQHdrP) 0;
    for (j=0, mask=1; j<32; j++, mask<<=1) {
      if (mask & tmpQHdrP->winDispFlags)	  
	ServerDeleteEvent(tmpQHdrP, windowPA[k]); }
    DelStroke(tmpQHdrP); }
}

static t_strokeQHdrP prevQHdrP;

t_strokeQHdrP
findStrokeInQueue(ID)
     unsigned short ID;
{ t_strokeQHdrP tmpQHdrP;
  for (tmpQHdrP=firstQHdrP, prevQHdrP=(t_strokeQHdrP)0; 
       tmpQHdrP; prevQHdrP=tmpQHdrP, tmpQHdrP=tmpQHdrP->nextP) {
    if (tmpQHdrP->strokeID == ID) return tmpQHdrP; }
  return (t_strokeQHdrP) 0; /* stroke not here */
}

/* NOTE: this code must work for any window, not just windows to
   which the stroke has been routed */
int
CopyStrokeToClient(QHdrP, windowP, startIndex, endIndex)
     t_strokeQHdrP QHdrP; WindowPtr windowP;
{ xPLIManageStrokeXYReply reply; t_XYs XYs; t_XYsP XYsP;
  extern void SwapXpliStroke();
  ClientPtr clientP = wClient(windowP);
  int numXYs, winX, winY, i, j, jj, l; char n;
  t_pliPoint *XYiP, *XYoP, *XYP;
  if (endIndex) {
    if (((numXYs=endIndex-startIndex) > (QHdrP->stroke.numXYs-startIndex)) ||
	(numXYs < 0))
      return(BadValue); /* bad start or end index */ }
  else numXYs = QHdrP->stroke.numXYs;
  reply.type = X_Reply; reply.pad0 = 0;
  reply.sequenceNumber = clientP->sequence;
  reply.length = sizeof(xPLIManageStrokeXYReply)/4 + numXYs;
  reply.window = windowP->drawable.id;
  reply.startIndex = startIndex;
  reply.endIndex = startIndex + numXYs;
  reply.stroke = QHdrP->stroke;
  winX = ndc2tc(windowP->drawable.x);
  winY = ndc2tc(windowP->drawable.y);
  reply.stroke.minX -= winX;
  reply.stroke.maxX -= winX;
  reply.stroke.minY -= winY;
  reply.stroke.maxY -= winY;
  if (clientP->swapped) {
    long n;
    swaps(reply.sequenceNumber, n);
    swapl(reply.length, n);
    swapl(reply.window, n);
    SwapXpliStroke(&reply.stroke); }
  WriteToClient(clientP, sizeof(xPLIManageStrokeXYReply), &reply);
  for (XYsP=QHdrP->XYsP; XYsP && (XYsP->XYIndx+SIZE_XYS) < startIndex;
       XYsP = XYsP->nextP);
  j = startIndex - XYsP->XYIndx;
  for (;numXYs > 0;XYsP++) {
    l = SIZE_XYS - j; 
    if (l > numXYs) l = numXYs;
    XYP = XYiP = &XYs.xy[j]; XYoP = &XYsP->xy[j];
    for (jj=j; jj<l; jj++) {
      XYiP->x = XYoP->x - winX;
      XYiP->y = XYoP->y - winY;
      if (clientP->swapped) {
	swaps(XYiP->x, n); swaps(XYiP->y, n); }
      XYiP++; XYoP++; }
    WriteToClient(clientP, l<<2, XYP); 
    numXYs -= l; XYsP = XYsP->nextP; j=0; }
  return Success;
}

int
acceptStroke(QHdrP, windowP)
     t_strokeQHdrP QHdrP; WindowPtr windowP;
{ unsigned int k; unsigned long mask;
  if ((k = findWindowInCache(windowP)) < 0) return (BadAccess);
  mask = 1<<k;
  if (!(QHdrP->winDispFlags & mask)) return (BadAccess);
  QHdrP->winAcceptFlags |= mask; /* turn on accept flag */
  if (!(QHdrP->winDispFlags &= ~mask))
    DelStroke(QHdrP);
}
  
int
rejectStroke(QHdrP, windowP)
     t_strokeQHdrP QHdrP; WindowPtr windowP;
{ unsigned int k; unsigned long mask;
  if ((k = findWindowInCache(windowP)) < 0) return (BadAccess);
  mask = 1<<k;
  if (!(QHdrP->winDispFlags & mask)) return (BadAccess);
  QHdrP->winRejectFlags |= mask;
  if (!(QHdrP->winDispFlags &= ~mask))
    DelStroke(QHdrP);
  StrokeGrabMask &= ~mask;
}

/* like reject stroke, but send notification to window */  
int
withdrawStroke(QHdrP, windowP)
     t_strokeQHdrP QHdrP; WindowPtr windowP;
{ unsigned int k; unsigned long mask;
  if ((k = findWindowInCache(windowP)) < 0) return (BadAccess);
  mask = 1<<k;
  if (!(QHdrP->winDispFlags & mask)) return (BadAccess);
  QHdrP->winRejectFlags |= mask;
  ServerDeleteEvent(QHdrP, windowP);
  if (!(QHdrP->winDispFlags &= ~mask))
    DelStroke(QHdrP);
  StrokeGrabMask &= ~mask;
}

int
SendPendingStrokes(cP)
     ClientPtr cP;
{ xPLIPendingStrokesReply reply; 
  xPLIPendingStroke *replyExt, *replyExtTmp;
  struct timestruc_t curtime;
  int i, j, k, cursize; unsigned long mask;
  t_strokeQHdrP QHdrP;

  gettimer(TIMEOFDAY, &curtime);
  reply.type = X_Reply;
  reply.sequenceNumber = cP->sequence;
  cursize = 128;
  replyExt = replyExtTmp = 
    (xPLIPendingStroke *) xalloc(cursize*sizeof(xPLIPendingStroke));
  if (!replyExt) return BadAlloc;
  j = 0;
  for(QHdrP=firstQHdrP; QHdrP; QHdrP=QHdrP->nextP) {
    for (i=0, mask=1; i<32; i++, mask <<= 1) {
      if (QHdrP->winDispFlags & mask) {
	replyExtTmp->strokeID = QHdrP->strokeID;
	replyExtTmp->window = windowPA[i]->drawable.id;
	replyExtTmp->age = curtime.tv_sec - QHdrP->stroke.timesecs;
	if ((++j) > cursize) {
	  cursize += 128;
	  replyExt = 
	    (xPLIPendingStroke *) xrealloc(replyExt, 
					   cursize*sizeof(xPLIPendingStroke));
	  if (!replyExt) return BadAlloc;
	  replyExtTmp = replyExt+j; }
	else replyExtTmp++; } } }
  reply.length = j<<1;
  if (cP->swapped) { 
    register char n;
    swaps(&reply.sequenceNumber, n);
    swapl(&reply.length, n);
    replyExtTmp = replyExt;
    for (i=0; i<j; i++) {
      swaps(&replyExtTmp->strokeID, n);
      swaps(&replyExtTmp->age, n);
      swapl(&(replyExtTmp++)->window, n); } }
  WriteToClient(cP, sizeof(reply), &reply);
  WriteToClient(cP, j<<3, replyExt);
  return Success;
}

static t_strokeQHdrP
newQHdr()
{ t_strokeQHdrP QHdrP;
  if (!(QHdrP = freeQHdrP)) {
    if (!(QHdrP = (t_strokeQHdrP) xalloc(sizeof(t_strokeQHdr))))
      FatalError("no storage for strokeQHdr"); }
  else freeQHdrP = QHdrP->nextP; 
  return QHdrP;
}
  
static t_XYsP
newXYs()
{ t_XYsP XYsP;
  if (!(XYsP = freeXYsP)) {
    if (!(XYsP = (t_XYsP) xalloc(sizeof(t_XYs))))
      FatalError("no storage for XYs"); }
  else freeXYsP = XYsP->nextP; 
  return XYsP;
}
  
void
InitXpliEvents(xpliDevStrP)
     t_xpliDevStrP xpliDevStrP;
{
  xpliDevStrP->newQHdr = newQHdr;
  xpliDevStrP->newXYs  = newXYs;
  xpliDevStrP->proxReport = proxReport;
  xpliDevStrP->startStroke = startStroke;
  xpliDevStrP->inStroke = inStroke;
  xpliDevStrP->endStroke = endStroke;
  /* default drag time is 0.25 secs, expressed as number of samples */
  xpliDevStrP->dragThresh = xpliDevStrP->config.samplesSec / 4;
  /* default drag box is 4 mm */
  xpliDevStrP->dragX = (xpliDevStrP->config.xResol * 4) / 10;
  xpliDevStrP->dragY = (xpliDevStrP->config.yResol * 4) / 10;
}


