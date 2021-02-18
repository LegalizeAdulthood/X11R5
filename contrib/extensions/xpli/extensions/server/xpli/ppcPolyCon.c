/*
 * Copyright IBM Corporation 1987,1988,1989
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

/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $Header: /andrew/X11/R3src/tape/server/ddx/ibm/ppc/RCS/ppcPolyCon.c,v 9.1 88/10/24 04:00:56 paul Exp $ */
/* $Source: /andrew/X11/R3src/tape/server/ddx/ibm/ppc/RCS/ppcPolyCon.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/R3src/tape/server/ddx/ibm/ppc/RCS/ppcPolyCon.c,v 9.1 88/10/24 04:00:56 paul Exp $";
#endif

#include "X.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "mifpoly.h"

static int GetFPolyYBounds();

/*
 *	Written by Todd Newman; April. 1987.
 *
 *	Fill a convex polygon.  If the given polygon
 *	is not convex, then the result is undefined.
 *	The algorithm is to order the edges from smallest
 *	y to largest by partitioning the array into a left
 *	edge list and a right edge list.  The algorithm used
 *	to traverse each edge is digital differencing analyzer
 *	line algorithm with y as the major axis. There's some funny linear
 *	interpolation involved because of the subpixel postioning.
 *
 *	Given the opportunity to write non-portable code, I would replace
 * 	this with a fixed point extended precision Bresenham line algorithm.
 *	Instead of doubles in the SppPointRecs, I'd use 64 bit fixed point
 *	values.  I'd have to write a fixed point math pack in assembler,
 *	but these things are relatively straight forward. (Alas, not portable.)
 *	
 *	It can be shown that given 16 bit values for x and y, there's no
 *	room to extend the precision and still fit intermediate results in
 *	a 32 bit integer.  It should also be clear that trying to write a
 *	extended precision math pack in C would not produce code that runs
 * 	much faster than your average floating point coprocessor.  Even
 *	floating point emulation should be able to run faster if the assembly
 *	language coding is handled at all well.
 *
 *	"So why not at least use float, instead of double," my friends ask
 *	me.  Well, C is going to cast everything to double as soon as you
 * 	try to do anything with it anyway.  (Check out K&R.)  I'd rather
 *	trade a little space and skip all the superflous type conversions.
 *	(This decision could be revisited if you have an ANSI C compiler.)
 *
 *	So, in sum, I apologize for doing all the wide lines with doubles,
 *	but it seems the best trade off  of complexity, performance, and
 *	time pressure.
 */
void
ppcFillSppPoly(dst, pgc, count, ptsIn, xTrans, yTrans)
    DrawablePtr 	dst;
    GCPtr		pgc;
    int			count;          /* number of points */
    SppPointPtr 	ptsIn;          /* the points */
    int			xTrans, yTrans;	/* Translate each point by this */
{
    int			i,nOut= 0;
    DDXPointPtr 	ptsOut;

    ptsOut = (DDXPointPtr)ALLOCATE_LOCAL(sizeof(DDXPointRec) * count);

    if(!ptsOut || (count < 3)) {
	DEALLOCATE_LOCAL(ptsOut);
	return;
    }

    for (i=0;i<count;i++) {
	ptsOut[i].x= (ptsIn[i].x+xTrans);
	ptsOut[i].y= (ptsIn[i].y+yTrans);
    }

    (*pgc->ops->FillPolygon)(dst, pgc, Convex, CoordModeOrigin, count, ptsOut);
    DEALLOCATE_LOCAL(ptsOut);
}
