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

/* $Header: /andrew/X11/r3src/release/server/ddx/ibm/ppc/RCS/ppcFillRct.c,v 30.0 89/01/23 19:29:05 paul Exp $ */
/* $Source: /andrew/X11/r3src/release/server/ddx/ibm/ppc/RCS/ppcFillRct.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/release/server/ddx/ibm/ppc/RCS/ppcFillRct.c,v 30.0 89/01/23 19:29:05 paul Exp $" ;
#endif

#include "X.h"
#include "pixmapstr.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "miscstruct.h"
#include "regionstr.h"
#include "scrnintstr.h"

#include "OScompiler.h"
#include "ibmTrace.h"

#include "ppc.h"
extern int mfbGCPrivateIndex;

/*
	filled rectangles.
	translate the rectangles, clip them, and call the
	helper function in the GC.
*/

void
ppcPolyFillRect( pDrawable, pGC, nrectFill, prectInit )
    DrawablePtr pDrawable ;
    GCPtr	pGC ;
    int		nrectFill ; 	/* number of rectangles to fill */
    xRectangle	*prectInit ;  	/* Pointer to first rectangle to fill */
{
    register int n ;		/* spare counter */
    register xRectangle *prect ;	/* temporary */
    RegionPtr prgnClip ;
    register BoxPtr pbox ;	/* used to clip with */
    register BoxPtr pboxClipped ;
    BoxPtr pboxTempBase ;
    BoxPtr pboxTempEnd ;
    BoxPtr pboxClippedBase ;
    void (* fillArea)() ;

    TRACE(("ppcPolyFillRect(0x%x,0x%x,%d,0x%x)\n",pDrawable,pGC,
						  nrectFill,prectInit)) ;

    if ( pDrawable->type != DRAWABLE_WINDOW ) {
	ErrorF("ppcPolyFillRect: Not a window\n") ;
	return ;
    }

    if ( ( pGC->alu == GXnoop ) || !pGC->planemask )
	return ;

    prgnClip = ( (ppcPrivGC *) (pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip ;

    pboxTempBase = (BoxPtr)
      ALLOCATE_LOCAL( ( REGION_NUM_RECTS(prgnClip) + nrectFill ) * sizeof( BoxRec ) ) ;
    if ( !pboxTempBase )
	return ;

    {
	register const int xorg = pDrawable->x ;
	register const int yorg = pDrawable->y ;

	for ( prect = prectInit, n = nrectFill ; n-- ; prect++ ) {
	    prect->x += xorg ;
	    prect->y += yorg ;
	}
    }

    fillArea = ( (ppcPrivGC *) ( pGC->devPrivates[mfbGCPrivateIndex].ptr ) )->FillArea ;
    pboxClippedBase = ( pboxTempEnd = pboxTempBase ) + nrectFill ;
/* NOTE:
 *	Some VERY stupid programs may call the server with
 *	very large width or height values.
 *	These programs think they are using negative numbers!
 *	Therefore, we must do "saturation" arithmetic for the rect's .
 */
#define MAXIMUM_X11_POS	(32767)
#define MINIMUM_X11_POS	(-32768)
    for ( prect = prectInit ; nrectFill-- ; prect++ ) {
	pboxTempEnd->x1 = prect->x ;
	pboxTempEnd->x2 = MIN( prect->x + prect->width, MAXIMUM_X11_POS ) ;
	pboxTempEnd->y1 = prect->y ;
	pboxTempEnd->y2 = MIN( prect->y + prect->height, MAXIMUM_X11_POS ) ;

	switch ( (* pGC->pScreen->RectIn)( prgnClip, pboxTempEnd ) ) {
	  case rgnIN:
	    pboxTempEnd++ ; /* Save for later ! */
	  case rgnOUT:
	    break ;
	  case rgnPART:
	    /* clip the rectangle to each box in the clip region
	       this is logically equivalent to calling Intersect()
	    */
	    for ( pboxClipped = pboxClippedBase,
		  pbox = REGION_RECTS(prgnClip),
		  n = REGION_NUM_RECTS(prgnClip) ;
		  n-- ; pbox++ ) {
		pboxClipped->x1 = MAX( pboxTempEnd->x1, pbox->x1 ) ;
		pboxClipped->x2 = MIN( pboxTempEnd->x2, pbox->x2 ) ;
		if ( pboxClipped->x1 < pboxClipped->x2 ) {
		    pboxClipped->y1 = MAX( pboxTempEnd->y1, pbox->y1 ) ;
		    pboxClipped->y2 = MIN( pboxTempEnd->y2, pbox->y2 ) ;
		    /* see if clipping left anything */
		    if ( pboxClipped->y1 < pboxClipped->y2 )
			pboxClipped++ ;
		}
	    }
	    if ( n = pboxClipped - pboxClippedBase )
		(* fillArea)( pDrawable, n, pboxClippedBase, pGC ) ;
	    break ;
	}
    }
    if ( n = pboxTempEnd - pboxTempBase )
	(* fillArea)( pDrawable, n, pboxTempBase, pGC ) ;

    DEALLOCATE_LOCAL( pboxTempBase ) ;
    return ;
}
