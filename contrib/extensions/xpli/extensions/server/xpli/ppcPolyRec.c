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
/* $Header: mipolyrect.c,v 1.12 88/09/06 14:50:28 jim Exp $ */
#include "X.h"
#include "Xprotostr.h"
#include "miscstruct.h"
#include "gcstruct.h"
#include "pixmap.h"

void
ppcPolyRectangle(pDraw, pGC, nrects, pRects)
    DrawablePtr	pDraw;
    GCPtr	pGC;
    int		nrects;
    xRectangle	*pRects;
{
    int i;
    xRectangle *pR = pRects;
    xRectangle *tmprects, *tmprectsinit;
    int lw, fs, ss;

    if ( ! ( tmprectsinit = tmprects = (xRectangle *)ALLOCATE_LOCAL( ( sizeof ( xRectangle ) * nrects ) << 2 ) ) )
	return;

    lw = pGC->lineWidth;
    ss = lw >> 1;		/* skinny side of line */
    fs = ( lw + 1 ) >> 1;	/* fat side of line */

    for (i=0; i<nrects; i++)
    {
	tmprects->x = pR->x - ss;
	tmprects->y = pR->y - ss;
	tmprects->width = pR->width + lw;
	tmprects->height = lw;
	tmprects++;

	tmprects->x = pR->x - ss;
	tmprects->y = pR->y + fs;
	tmprects->width = lw;
	tmprects->height = pR->height - lw;
	tmprects++;

	tmprects->x = pR->x + pR->width - ss;
	tmprects->y = pR->y + fs;
	tmprects->width = lw;
	tmprects->height = pR->height - lw;
	tmprects++;

	tmprects->x = pR->x - ss;
	tmprects->y = pR->y + pR->height - ss;
	tmprects->width = pR->width + lw;
	tmprects->height = lw;
	tmprects++;

	pR++;
    }

    (* pGC->ops->PolyFillRect)( pDraw, pGC, nrects << 2, tmprectsinit );

    DEALLOCATE_LOCAL( tmprectsinit );
    return ;
}
