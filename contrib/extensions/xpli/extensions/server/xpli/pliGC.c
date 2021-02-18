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
		Copyright IBM Corporation 1987,1988

		      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
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

/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliGC.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliGC.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */

#ifndef lint
static char *rcsid = "$Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliGC.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $" ;
#endif

#include "X.h"
#include "Xproto.h"
#include "windowstr.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "misc.h"
#include "font.h"
#include "gcstruct.h"
#include "cursorstr.h"
#include "region.h"

#include "mistruct.h"

#include "../../mfb/mfb.h"

#include "OScompiler.h"

#include "ppc.h"
#include "ppcProcs.h"

#include "pliProcs.h"
extern int mfbGCPrivateIndex;

Mask
pliChangeGCtype( pGC, devPriv )
register GC *pGC ;
register ppcPrivGCPtr devPriv ;
{
	if ( devPriv->lastDrawableType == DRAWABLE_PIXMAP ) {
	    devPriv->FillArea	= mfbSolidInvertArea ;
	    pGC->ops->CopyArea	= miCopyArea ;
	    pGC->ops->PolyFillRect	= miPolyFillRect ;
	    pGC->ops->PushPixels	= miPushPixels ;
	    pGC->ops->PolyArc	= miPolyArc ;
	    pGC->ops->PolyFillArc	= miPolyFillArc ;
	    pGC->ops->PolySegment	= miPolySegment ;
	}
	else {
	    devPriv->FillArea	= ppcAreaFill ;
	    pGC->ops->CopyArea	= ppcCopyArea ;
	    pGC->ops->PolyFillRect	= ppcPolyFillRect ;
	    pGC->ops->PushPixels	= ppcPushPixels ;
	    pGC->ops->PolyArc	= ppcPolyZeroArc ;
	    pGC->ops->PolyFillArc	= ppcPolyFillArc ;
	    pGC->ops->PolySegment	= ppcScrnZeroSegs ;
	}
	return;
}

Mask
pliChangeWindowGC( pGC, changes )
register GC *pGC ;
register Mask changes ;
{
register ppcPrivGCPtr devPriv = (ppcPrivGCPtr) (pGC->devPrivates[mfbGCPrivateIndex].ptr) ;
register unsigned long int index ; /* used for stepping through bitfields */
register Mask bsChanges = 0 ;

#define LOWBIT( x ) ( x & - x ) /* Two's complement */
    while ( index = LOWBIT( changes ) ) {
	switch ( index ) {

	  case GCLineStyle:
	  case GCLineWidth:
	    pGC->ops->PolyArc = ( ( pGC->lineStyle == LineSolid )
		? ( ( pGC->lineWidth == 0 ) ? ppcPolyZeroArc
					    : miPolyArc )
		: miPolyArc ) ;
	    pGC->ops->PolySegment = ( ( pGC->lineStyle == LineSolid )
		? ( ( pGC->lineWidth == 0 ) ? ppcScrnZeroSegs
					    : miPolySegment )
		: miPolySegment ) ;
	    pGC->ops->Polylines = ( ( pGC->lineStyle == LineSolid )
		? ( ( pGC->lineWidth == 0 ) ? ppcScrnZeroLine
					    : miWideLine )
		: ( ( pGC->lineWidth == 0 ) ? ppcScrnZeroDash
					    : miWideDash ) ) ;
	    /*
	     * If these are just square boxes with no funny business
	     * going on we can call the fast routine that draws
	     * rectangles without floating point.
	     */
	    if ( ( pGC->lineStyle == LineSolid ) 
			&& ( pGC->joinStyle == JoinMiter ) 
			&& ( pGC->lineWidth != 0 ) )
		pGC->ops->PolyRectangle = ppcPolyRectangle;
	    else
		pGC->ops->PolyRectangle = miPolyRectangle;

	    changes &= ~( GCLineStyle | GCLineWidth ) ;
	    break ;
	  case GCJoinStyle:
	    pGC->ops->LineHelper =
		( pGC->joinStyle == JoinMiter ) ? miMiter : miNotMiter ;
	    /*
	     * If these are just square boxes with no funny business
	     * going on we can call the fast routine that draws
	     * rectangles without floating point.
	     */
	    if ( ( pGC->lineStyle == LineSolid ) 
			&& ( pGC->joinStyle == JoinMiter ) 
			&& ( pGC->lineWidth != 0 ) )
		pGC->ops->PolyRectangle = ppcPolyRectangle;
	    else
		pGC->ops->PolyRectangle = miPolyRectangle;
	    changes &= ~ index ; /* i.e. changes &= ~ GCJoinStyle */
	    break ;

	  case GCBackground:
	    if ( pGC->fillStyle != FillOpaqueStippled ) {
		changes &= ~ index ; /* i.e. changes &= ~GCBackground */
		break ;
	    } /* else Fall Through */
	  case GCForeground:
	    if ( pGC->fillStyle == FillTiled ) {
		changes &= ~ index ; /* i.e. changes &= ~GCForeground */
		break ;
	    } /* else Fall Through */
	  case GCFunction:
	  case GCPlaneMask:
	  case GCFillStyle:
	    { /* new_fill */
		int fillStyle = devPriv->colorRrop.fillStyle ;
		/* install a suitable fillspans */
		if ( fillStyle == FillSolid )
		    pGC->ops->FillSpans = ppcSolidWindowFS ;
		else if ( fillStyle == FillStippled )
		    pGC->ops->FillSpans = ppcStippleWindowFS ;
		else if ( fillStyle == FillOpaqueStippled )
		    pGC->ops->FillSpans = ppcOpStippleWindowFS ;
		else /*  fillStyle == FillTiled */
		    pGC->ops->FillSpans = ppcTileWindowFS ;
	    } /* end of new_fill */
	    changes &= ~( GCBackground | GCForeground
		     | GCFunction
		     | GCPlaneMask | GCFillStyle ) ;
	    break ;

	default:
	    ErrorF("pliChangeWindowGC: Unexpected GC Change\n") ;
	    changes &= ~ index ; /* Remove it anyway */
	    break ;
	}
    }
    return;
}
