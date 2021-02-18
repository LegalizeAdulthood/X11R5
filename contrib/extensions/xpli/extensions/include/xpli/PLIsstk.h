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
#ifndef PLISSTK_H
#define PLISSTK_H

#include "plicoord.h"
#include "Xmd.h"

/* this is part of the so-called "wire" format, see PLIproto.h and
   the xPLIManageStrokeXYReply typedef. NOTE: because many pens
   use a mechanical contact mechanism, contact bounce and unintended
   pen lift are a significant problem. The time information in
   this structure can help software detect and deal with this
   phenomenon (i.e. if the start -end time is < about 100 msec
   then one strongly suspects a pen skip. Implementors are
   urged to provide this information with as much timer resolution
   as possible. The time representation here presumes POSIX
   time formats. */

typedef struct {
        CARD8  strktype; /* same as xStrokeEvent type detail */
        BYTE   pad1_2;
        CARD16 pad1_34;
        INT16  startX;     /* all xys are scaled pixel coordinates */
        INT16  startY;
        INT16  endX;
        INT16  endY;
        INT16  minX;
        INT16  minY;
        INT16  maxX;
        INT16  maxY;
        CARD32 prevsecs; /* end time of previous stroke */
	CARD32 prevnsec;
        CARD32 timesecs; /* time of start of stroke */
        CARD32 timensec;
	CARD32 numXYs;
      } xPLIStroke;
 
/* 
 * this is an alternate typedef for the preceeding stroke structure which
 * implements a memory model in which the xys are represented as an array of
 * xy pairs at the end of the header. This is the form which Xlib and the
 * plilib interface report to the client.  It will be nice if this structure
 * and the one preceeding it are memory mapped identically - otherwise
 * conversion routines will have to be written.  
 */

typedef struct 
{
  INT16 x;
  INT16 y;
} t_pliPoint, *t_pliPointP;
 
 
typedef struct {
        CARD8  strktype; /* same as xStrokeEvent.detail */
        BYTE   pad1;
        CARD16 pad2;
        INT16  minX;     /* all xys are scaled pixel coordinates */
        INT16  maxX;
        INT16  minY;
        INT16  maxY;
        CARD32 prevsecs; /* end time of previous stroke */
        CARD32 prevnsec;
        CARD32 timesecs; /* start time of this stroke */
        CARD32 timensec;
	CARD32 numXYs;
	t_pliPoint xys[1];
      } PLIStroke;


/* 
 * this macro gets storage for the correct sized PLIstroke based on the 
 * number of xy pairs; eventp could be a pointer to an xStrokeEvent, an 
 * xPLIStroke, or a PLIStroke.
 */
#define MALLOCPLISTROKE(eventp)  \
  (PLIStroke *) malloc(sizeof(PLIStroke) + (eventp->numXYs - 1)*sizeof(t_pliPoint))
#endif  /* ifndef PLISSTK_H */
