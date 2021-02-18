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
#ifndef XPLISTRKQHDR_H
#define XPLISTRKQHDR_H

#include "X.h"
#include "PLIsstk.h"

#define SIZE_XYS 128

/*
  XysStruct - this holds 128 of the XY values and a pointer to the next
  group of 128.
*/

typedef struct s_XYs {
  struct s_XYs    *nextP;
  unsigned int    XYIndx; /* what is the numXYs preceeding xy[0] */
  t_pliPoint      xy[SIZE_XYS];
} t_XYs, *t_XYsP;

/*
  Stroke queue header - there is one of these for each enqueued stroke.
*/

typedef struct s_strokeQHdr {
  struct s_strokeQHdr *nextP;
  unsigned short strokeID;
  unsigned short mouseEm; /* 1 - drag candidate, 2 - mouse emulation */
  unsigned long winEnterFlags; /* which windows have this stroke entered */
  unsigned long winPtrFlags;   /* which entered windows want ptr events */
  unsigned long winDispFlags;  /* which entered windows want strokes */
  unsigned long winAcceptFlags; /* which stroke aware windows accepted it */
  unsigned long winRejectFlags; /* which stroke aware windows rejected it */
  Time          startTime, endTime; /* X time, for xEvents  */
  t_XYsP  XYsP, lastXYsP; /* chain of XYs */
  xPLIStroke    stroke; /* proto stroke header needed for replies */
} t_strokeQHdr, *t_strokeQHdrP;

#endif XPLISTRKQHDR_H
