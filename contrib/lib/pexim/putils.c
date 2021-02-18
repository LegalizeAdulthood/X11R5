#ifdef SCCS
static char sccsid[]="@(#)putils.c	1.6 Stardent 91/07/24";
#endif
/*
 *			Copyright (c) 1991 by
 *			Stardent Computer Inc.
 *			All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Stardent Computer not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission. Stardent Computer
 * makes no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */
/*************************************************************************
 * putils.c - PEXIM utility routines.
 *
 * These routines are declared in peximint.h
 *
 * GetROC() - get a Render Output Commands request
 */
#include "pexim.h"
#include "PEX.h"
#include "PEXproto.h"
/*			define PUTILS_C so globals will "live" in putils */
#define PUTILS_C
#include "peximint.h"

/*************************************************************************
 * GetROC - get a Render Output Commands request
 *
 * This code was modelled after macros which appear in $x11/lib/X/Xlibint.h
 */
char *GetROC(Display *dpy, int size)
{
  pexRenderOutputCommandsReq *req;

  /* we either got here because the last request was not a ROC or ... */
  if ((dpy->bufptr + size+sizeof(pexRenderOutputCommandsReq)) > dpy->bufmax){
    /* we can't fit, flush existing buffer */
    XFlush(dpy);
    if ((dpy->bufptr + size+sizeof(pexRenderOutputCommandsReq)) > dpy->bufmax)
    	return (0); /* things are totally wacky - this will kill us */
  }

  /*
   * typedef struct {
   *     CARD8 		reqType;
   *     CARD8 		opcode;
   *     CARD16 		length B16;
   *     pexEnumTypeIndex	fpFormat B16;
   *     CARD16		unused B16;
   *     pexStructure	sid B32;
   *     CARD32		numCommands B32;
   *     * LISTof OutputCommand( numCommands ) *
   * } pexStoreElementsReq;
   *
   * typedef struct {
   *     CARD8		reqType;
   *     CARD8		opcode;
   *     CARD16		length B16;
   *     pexEnumTypeIndex	fpFormat B16;
   *     CARD16		unused B16;
   *     pexRenderer		rdr B32;
   *     CARD32		numCommands B32;
   *     * LISTof OutputCommand( numCommands ) *
   * } pexRenderOutputCommandsReq;
   */

  if (sizeof(pexStoreElementsReq) != sizeof(pexRenderOutputCommandsReq)) {
    /* this might be a problem needing more code some day */
    return (0);
  }
  /*
   * we 
   */
  req = (pexRenderOutputCommandsReq *)(dpy->last_req = dpy->bufptr);
  req->reqType = pPEXIMOpCode;
  req->length = (size+sizeof(pexRenderOutputCommandsReq))>>2;
  req->fpFormat = PEXIeee_754_32;
  if (pPEXIMState == PEXIM_RENDERING) {
    req->opcode = PEX_RenderOutputCommands;
    req->rdr = pPEXIMCurrRdr;
  } else if (pPEXIMState == PEXIM_STRUCTURE) {
    /* this is a little convoluted, but, get the resourceID of struct */
    /* also, we are cheating by stuffing the struct resource into the rdr */
    req->opcode = PEX_StoreElements;
    req->rdr = pPEXIMStructTable[
		 (pPEXIMRendererTable[pPEXIMCurrWKIdx])->structIndex
				 ].resourceID;
  }
  req->numCommands = 1;
  dpy->bufptr += size+sizeof(pexRenderOutputCommandsReq);
  dpy->request++;

  return ((char *)((char *)req) +  sizeof(pexRenderOutputCommandsReq));
}

/*************************************************************************
 * I know this is lame. This and the GDP/Foreign server need work
 * before I put this stuff on contrib.
 *
 * I am planning to use the new entry points for setting the error function
 * in the "C" binding.
 */
void PEXIMError()
{
  printf("error\n");
}

/*************************************************************************
 * CopyDown - compresses coordinate data that has indexed colors in it
 */
void CopyDown(int count, long *fromAddr, int fromStep,long *toAddr,int toStep)
{
  int i, j, fs, ts;
  long *f, *t;

  ts = toStep>>2;
  f = fromAddr;
  t = toAddr;

  for ( i = 0; i < count; i++ ) {
    for ( j = 0; j < ts; j++ ) {
      *t++ = *f++;
    }
    f = (long *)(((char *)f) + (fromStep-toStep));
  }
}

/*************************************************************************
 * CopyMerge2 - copy coords & normals or colors into one stream
 */
void CopyMerge2(int count, long *coords, long *other, long *toAddr)
{
  int i, j, fs, ts;
  long *c, *o, *t;

  c = coords;
  o = other;
  t = toAddr;

  for ( i = 0; i < count; i++ ) {
    for ( j = 0; j < 3; j++ ) {
      *t++ = *c++;
    }
    for ( j = 0; j < 3; j++ ) {
      *t++ = *o++;
    }
  }
}

/*************************************************************************
 * CopyMerge3 - copy coords & normals or colors into one stream
 */
void CopyMerge3(int count, long *coords, long *other, long *self, long *toAddr)
{
  int i, j;
  long *c, *o, *s, *t;

  c = coords;
  o = other;
  s = self;
  t = toAddr;

  for ( i = 0; i < count; i++ ) {
    for ( j = 0; j < 3; j++ ) {
      *t++ = *c++;
    }
    for ( j = 0; j < 3; j++ ) {
      *t++ = *o++;
    }
    for ( j = 0; j < 3; j++ ) {
      *t++ = *s++;
    }
  }
}

/*************************************************************************
 * CopyPintToCARD16 - 
 */
void
CopyPintToCARD16( Pint *fromAddr, CARD16 *toAddr, int count )
{
  while (count--) *toAddr++ = *fromAddr++;
}
