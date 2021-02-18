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

#include <sys/time.h>
#include "X.h"
#define NEED_EVENTS
#define NEED_REPLIES
#include "Xproto.h"
#include "xpliDevStr.h"
#include "xpliStrkQHdr.h"
#include "PLIsstk.h"

static t_xpliDevStrP xpliDevStrP;
static float fdx[4], fdy[4]; /* three alignment points, scaled disp coords */
static float ftx[4], fty[4]; /* three tablet coords */
static long ax, ay, npts; /* for accumulation of alignment strokes */
static int prevpenup;
static int penupCtr, penupMotionCnt;
static int pendownCtr, pendownMotionCnt;
static int alignState;
static struct timestruc_t resol, curtime;
static t_plicoeff coeff;

/**************************************************************************/
/*                       Alignment Support                                */ 
/**************************************************************************/
/* 
  Alignment without a device driver makes use of a 4 state machine - 
  driven by the alignState variable. State 1 draws the initial cross
  and waits for a stroke. States 2 and 3 collect a stroke and average
  its points, associate the average with the current cross, and
  erase the current cross and display the next one. State 4 collects
  a stroke, averages its points, associates the average with the last
  alignment point, computes the alignment coefficients, erases the
  last alignment cross and returns to normal mode (alignState == 0).
 */
static void
NodevAlignInit()
{ int dx, dy;
  alignState = 1;
  (xpliDevStrP->drawCross)(1, alignState, &dx, &dy);
  fdx[alignState] = dx; fdy[alignState] = dy;
}

static void
NodevAlignNext(tx, ty)
     int tx, ty;
{ int i, dx, dy;
  ftx[alignState] = tx; fty[alignState] = ty;
  (xpliDevStrP->drawCross)(0, alignState, &dx, &dy);
  alignState++;
  (xpliDevStrP->drawCross)(1, alignState, &dx, &dy);
  fdx[alignState] = dx; fdy[alignState] = dy;
}

static int
NodevAlignFin(tx, ty, coeffP)
     int tx, ty; t_plicoeff *coeffP;
{ int rc, dx, dy;   double a, b, c, d, e, f, dl, fshift, fround;
  ftx[alignState] = tx; fty[alignState] = ty;
  (xpliDevStrP->drawCross)(0, alignState, &dx, &dy);
  dl  = ftx[1]*(fty[2]-fty[3]) + ftx[2]*(fty[3]-fty[1]) + 
    ftx[3]*(fty[1]-fty[2]);
  a  = (fdx[1]*(fty[2]-fty[3]) + fdx[2]*(fty[3]-fty[1]) + 
	fdx[3]*(fty[1]-fty[2])) / dl;
  b  = (fdx[1]*(ftx[3]-ftx[2]) + fdx[2]*(ftx[1]-ftx[3]) + 
	fdx[3]*(ftx[2]-ftx[1])) / dl;
  d  = (fdy[1]*(fty[2]-fty[3]) + fdy[2]*(fty[3]-fty[1]) + 
	fdy[3]*(fty[1]-fty[2])) / dl;
  e  = (fdy[1]*(ftx[3]-ftx[2]) + fdy[2]*(ftx[1]-ftx[3]) + 
	fdy[3]*(ftx[2]-ftx[1])) / dl;
  c  = (fdx[1]*(ftx[2]*fty[3] - ftx[3]*fty[2]) + 
	fdx[2]*(ftx[3]*fty[1] - ftx[1]*fty[3]) +
	fdx[3]*(ftx[1]*fty[2] - ftx[2]*fty[1])) / dl;
  f  = (fdy[1]*(ftx[2]*fty[3] - ftx[3]*fty[2]) + 
	fdy[2]*(ftx[3]*fty[1] - ftx[1]*fty[3]) + 
	fdy[3]*(ftx[1]*fty[2] - ftx[2]*fty[1])) / dl;
  fshift = 1<<COEFFSCALE;
  coeffP->a = 
    (a>0) ? a*fshift + 0.5 : a*fshift - 0.5;
  coeffP->b = 
    (b>0) ? b*fshift + 0.5 : b*fshift - 0.5;
  coeffP->c = 
    (c>0) ? c*fshift + 0.5 : c*fshift - 0.5;
  coeffP->d = 
    (d>0) ? d*fshift + 0.5 : d*fshift - 0.5;
  coeffP->e = 
    (e>0) ? e*fshift + 0.5 : e*fshift - 0.5;
  coeffP->f = 
    (f>0) ? f*fshift + 0.5 : f*fshift - 0.5;
  alignState = 0;
}

static void
NodevAlignAbort()
{ int dx, dy;
  if (alignState) { 
    (xpliDevStrP->drawCross)(0, alignState, &dx, &dy); 
    alignState = 0; }
} 

/**************************************************************************/
/*                       void ProcessSample(flags,x,y)                    */ 
/**************************************************************************/
/* 
 * This routine is called with a raw sample from the digitizer.
 * The sample is converted to scaled display coordinates, inked
 * and converted into a stroke event when appropriate. It provides
 * functions normally found in the xpli device driver when there
 * is no such driver in the system (e.g. when the digitizer is
 * accessed via /dev/tty<n>).
 */
static void
NodevProcessXY(xpliDevP, flags, x, y)
     t_xpliDevStrP xpliDevP;
     unsigned int flags; int x, y;
{ t_pliPoint m; 
  static int ax=0, ay=0, npts=0; 
  static int prevpendown = 0;
  static t_strokeQHdrP QHdrP = (t_strokeQHdrP) 0;
  static t_XYsP XYsP = (t_XYsP) 0;
  int i;

  if (alignState == 0) {
    /* this computation produces scaled display coordinates
       mdx, mdy, given coefficients scaled by COEFFSCALE */
    m.x = x*coeff.a + y*coeff.b + coeff.c; 
    m.x = ((m.x < 0) ? (m.x - COEFFROUND) : (m.x + COEFFROUND))
      >> COEFFSCALE-COORDSCALE;
    m.y = x*coeff.d + y*coeff.e + coeff.f; 
    m.y = ((m.y < 0) ? (m.y - COEFFROUND) : (m.y + COEFFROUND))
      >> COEFFSCALE-COORDSCALE; }
  /* possible states are: penup, pendown, penup to pendown
     transition (start of stroke), and pendown to penup
     transition (end of stroke) */
  if (prevpendown) { /* previous sample contained pendown flag */
    if (flags & 0x01) { /* current pendown */
      if (alignState) { /* in stroke in alignment, add in XY values */
	ax += x; ay += y; npts++; }
      else { /* in stroke, not in alignment, maybe notify client */
	if ((i=((++QHdrP->stroke.numXYs) - XYsP->XYIndx)) >= 128) {
	  XYsP = QHdrP->lastXYsP = XYsP->nextP = (xpliDevP->newXYs)();
	  XYsP->nextP = (t_XYsP) 0;
	  XYsP->XYIndx = QHdrP->stroke.numXYs-1; i -= 128; }
	XYsP->xy[i] = m;
	QHdrP->stroke.endX = m.x; QHdrP->stroke.endY = m.y;
	if (m.x > QHdrP->stroke.maxX) QHdrP->stroke.maxX = m.x;
	if (m.y > QHdrP->stroke.maxY) QHdrP->stroke.maxY = m.y;
	if (m.x < QHdrP->stroke.minX) QHdrP->stroke.minX = m.x;
	if (m.y < QHdrP->stroke.minY) QHdrP->stroke.minY = m.y;
	(xpliDevP->ink)(m.x, m.y);
	if ((--pendownCtr) == 0) {
	  pendownCtr = pendownMotionCnt;
	  (xpliDevP->inStroke)(xpliDevP, QHdrP); } } }
    else { /* pendown to penup transition */
      if (alignState == 3) {
	ax /= npts; ay /= npts;
	NodevAlignFin(ax, ay, &coeff); }
      else if (alignState) {
	ax /= npts; ay /= npts;
	NodevAlignNext(ax, ay); }
      else { 
	gettimer(TIMEOFDAY, &curtime); /* get end time for next stroke */
	(xpliDevP->endStroke)(xpliDevP, QHdrP);
	penupCtr = penupMotionCnt; } 
      prevpendown = 0; } }
  else { /* previous sample had pen up */
    if (flags & 0x01) { /* this is a penup to pendown transition */
      if (alignState) { /* in alignment state */
	ax = x;	ay = y;	npts = 1; }
      else { /* penup to pendown, not in alignment -> start of stroke */
	QHdrP = (xpliDevP->newQHdr)();
	XYsP = (xpliDevP->newXYs)();
	QHdrP->lastXYsP = QHdrP->XYsP = XYsP;
	XYsP->nextP = (t_XYsP) 0;
	XYsP->XYIndx = 0;
	XYsP->xy[0].x = m.x;
	XYsP->xy[0].y = m.y;
	QHdrP->stroke.numXYs = 1;
	QHdrP->stroke.startX = QHdrP->stroke.endX = QHdrP->stroke.minX = 
	  QHdrP->stroke.maxX = m.x;
	QHdrP->stroke.startY = QHdrP->stroke.endY = 
	  QHdrP->stroke.minY = QHdrP->stroke.maxY = m.y;
	QHdrP->stroke.prevsecs = curtime.tv_sec;
	QHdrP->stroke.prevnsec = curtime.tv_nsec;
	gettimer(TIMEOFDAY, &curtime);
	QHdrP->stroke.timesecs = curtime.tv_sec;
	QHdrP->stroke.timensec = curtime.tv_nsec;
	(xpliDevP->initInkPoint)(1, m.x, m.y);
	pendownCtr = pendownMotionCnt;
	(xpliDevP->startStroke)(xpliDevP, QHdrP); }
      prevpendown = 1; }
    else { /* this is a proximity sample */
      if (alignState) ; /* ignore prox samples in alignment mode */
      else {
	if ((--penupCtr) == 0)
	  {
	    penupCtr = penupMotionCnt;
	    (xpliDevP->proxReport)(xpliDevP, m.x, m.y); } } } }
}

int	
InitNodev(INxpliDevStrP)
     t_xpliDevStrP INxpliDevStrP;
{
  xpliDevStrP = INxpliDevStrP;
  restimer(TIMEOFDAY, &resol, &curtime);
  xpliDevStrP->config.timeResolSec = resol.tv_sec;
  xpliDevStrP->config.timeResolNsec = resol.tv_nsec;
  xpliDevStrP->alignInit = NodevAlignInit;
  xpliDevStrP->alignAbort = NodevAlignAbort;
  xpliDevStrP->processXY = NodevProcessXY;
  /* generate proximity and stroke motion reports about 10 times per sec */
  penupMotionCnt = xpliDevStrP->config.samplesSec / 10;
  pendownMotionCnt = xpliDevStrP->config.samplesSec / 10;
  return 0;
}

