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

/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcGC.c,v 9.7 89/06/16 16:37:21 jeff Exp $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcGC.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcGC.c,v 9.7 89/06/16 16:37:21 jeff Exp $" ;
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

#include "ibmTrace.h"

extern int ibmAllowBackingStore ;
extern int mfbGCPrivateInterest;
#define ppcGCInterestValidateMask \
( GCLineStyle | GCLineWidth | GCJoinStyle | GCBackground | GCForeground \
| GCFunction | GCPlaneMask | GCFillStyle | GC_CALL_VALIDATE_BIT         \
| GCClipXOrigin | GCClipYOrigin | GCClipMask | GCSubwindowMode )

Bool
ppcCreateGC( pGC )
register GCPtr pGC ;
{
	ppcScrnPriv *devScrnPriv = (ppcScrnPriv * ) pGC->pScreen->devPrivate ;
	ppcPrivGC *pPriv ;
	GCOps *pOps ;

	if ( pGC->depth == 1 )
		{
		return (mfbCreateGC(pGC));
		}

	if ( !( pPriv = (ppcPrivGC *) Xalloc( sizeof( ppcPrivGC ) ) ) )
		return FALSE ;

	if ( !( pOps = (GCOps *) Xalloc( sizeof( GCOps ) ) ) ) {
		xfree(pPriv);
		return FALSE;
	}
	
	{ /* Save & Restore any passed-in variables !! */
		int		orig_depth	= pGC->depth ;
		PixmapPtr	orig_tile	= pGC->tile.pixmap ;
		PixmapPtr	orig_stipple	= pGC->stipple ;
		FontPtr		orig_font	= pGC->font ;
		int		orig_numInDashList	= pGC->numInDashList ;
		unsigned char	*orig_dash	= pGC->dash ;
		unsigned long	orig_serialNumber	= pGC->serialNumber ;
		DevUnion	*orig_devPrivates	= pGC->devPrivates ;

		/* Copy The Prototype GC */
		*pGC = *( devScrnPriv->devPrototypeGC ) ;

		/* Now restore the pointers ! */
		pGC->depth	= orig_depth ;
		pGC->tile.pixmap	= orig_tile ;
		pGC->stipple	= orig_stipple ;
		pGC->font	= orig_font ;
		pGC->numInDashList = orig_numInDashList ;
		pGC->dash	= orig_dash ;
		pGC->serialNumber  = orig_serialNumber ;
		pGC->devPrivates = orig_devPrivates;
	}

	/* Copy The Prototype devPriv */
	pGC->devPrivates[mfbGCPrivateIndex].ptr = (pointer) pPriv ;
	*pPriv = *( devScrnPriv->devPrototypeGCPriv ) ;

	*pOps = *( pGC->ops );
	pOps->devPrivate.val = 1;
	pGC->ops = pOps;

	return TRUE ;
}

void
ppcDestroyGC( pGC )
    register GC	*pGC ;

{
    register ppcPrivGC *pPriv ;

    TRACE( ( "ppcDestroyGC(pGC=0x%x)\n", pGC ) ) ;

    pPriv = (ppcPrivGC *) ( pGC->devPrivates[mfbGCPrivateIndex].ptr ) ;

    /* (ef) 11/9/87 -- ppc doesn't use rotated tile or stipple, but */
    /*		*does* call mfbValidateGC under some conditions.    */
    /*		mfbValidateGC *does* use rotated tile and stipple   */
    if ( pPriv->pRotatedPixmap )
	mfbDestroyPixmap( pPriv->pRotatedPixmap ) ;

    if ( pPriv->freeCompClip && pPriv->pCompositeClip )
	(* pGC->pScreen->RegionDestroy)( pPriv->pCompositeClip ) ;
    if ( pPriv->devPriv )
	 (* ( (ppcScrnPrivPtr) (pGC->pScreen->devPrivate) )->DestroyGCPriv)(
					pPriv->devPriv ) ;
    if(pGC->ops->devPrivate.val) Xfree( pGC->ops );
    Xfree( pGC->devPrivates[mfbGCPrivateIndex].ptr ) ;
    return ;
}

Mask
ppcChangePixmapGC( pGC, changes )
register GC *pGC ;
register Mask changes ;
{
register ppcPrivGCPtr devPriv = (ppcPrivGCPtr) (pGC->devPrivates[mfbGCPrivateIndex].ptr ) ;
register unsigned long int index ; /* used for stepping through bitfields */
register Mask bsChanges = 0 ;

#define LOWBIT( x ) ( x & - x ) /* Two's complement */
while ( index = LOWBIT( changes ) ) {
    switch ( index ) {

	case GCLineStyle:
	case GCLineWidth:
	    pGC->ops->Polylines = ( ! pGC->lineWidth )
		 ? miZeroLine
		 : ( ( pGC->lineStyle == LineSolid )
			 ? miWideLine : miWideDash ) ;
	    changes &= ~( GCLineStyle | GCLineWidth ) ;
	    break ;

	case GCJoinStyle:
	    pGC->ops->LineHelper = ( pGC->joinStyle == JoinMiter )
			    ? miMiter : miNotMiter ;
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
		    pGC->ops->FillSpans = ppcSolidPixmapFS ;
		else if ( fillStyle == FillStippled )
		    pGC->ops->FillSpans = ppcStipplePixmapFS ;
		else if ( fillStyle == FillOpaqueStippled )
		    pGC->ops->FillSpans = ppcOpStipplePixmapFS ;
		else /*  fillStyle == FillTiled */
		    pGC->ops->FillSpans = ppcTilePixmapFS ;
		changes &= ~( GCBackground | GCForeground
			    | GCFunction | GCPlaneMask | GCFillStyle ) ;
		break ;
	    } /* end of new_fill */

	default:
	    ErrorF( "ppcChangePixmapGC: Unexpected GC Change\n" ) ;
	    changes &= ~ index ; /* Remove it anyway */
	    break ;
	}
}

return bsChanges ;
}

/* Clipping conventions
	if the drawable is a window
	    CT_REGION ==> pCompositeClip really is the composite
	    CT_other ==> pCompositeClip is the window clip region
	if the drawable is a pixmap
	    CT_REGION ==> pCompositeClip is the translated client region
		clipped to the pixmap boundary
	    CT_other ==> pCompositeClip is the pixmap bounding box
*/

void
ppcValidateGC( pGC, changes, pDrawable )
    register GCPtr	pGC ;
    register Mask	changes ;
    DrawablePtr		pDrawable ;
{
    register ppcPrivGCPtr devPriv ;
    ppcScrnPriv *devScrnPriv = (ppcScrnPriv * )pGC->pScreen->devPrivate ;
    WindowPtr pWin ;
    Mask bsChanges = 0 ;

    devPriv = (ppcPrivGCPtr) (pGC->devPrivates[mfbGCPrivateIndex].ptr ) ;

    if ( pDrawable->type != devPriv->lastDrawableType ) {
	devPriv->lastDrawableType = pDrawable->type ;
	bsChanges |= (* devScrnPriv->changeGCtype)( pGC, devPriv ) ;
	changes = ~0 ;
    }

    if ( pDrawable->depth == 1 ) {
	ibmAbort();
    }

    if ( pDrawable->type == DRAWABLE_WINDOW ) {
	pWin = (WindowPtr) pDrawable ;
	pGC->lastWinOrg.x = pWin->drawable.x ;
	pGC->lastWinOrg.y = pWin->drawable.y ;
    }
    else {
	pWin = (WindowPtr) NULL ;
	pGC->lastWinOrg.x = 0 ;
	pGC->lastWinOrg.y = 0 ;
    }

    changes &= ppcGCInterestValidateMask ;
    /* If Nothing REALLY Changed, Just Return */
    if ( pDrawable->serialNumber == (pGC->serialNumber & DRAWABLE_SERIAL_BITS) )
	if ( !( changes &= ~ GC_CALL_VALIDATE_BIT ) )
	    return ;

    /*
	if the client clip is different or moved OR
	the subwindowMode has changed OR
	the window's clip has changed since the last validation
	we need to recompute the composite clip
    */
    if ( ( changes & (GCClipXOrigin|GCClipYOrigin|GCClipMask|GCSubwindowMode) )
      || ( pDrawable->serialNumber !=
		( pGC->serialNumber & DRAWABLE_SERIAL_BITS ) ) ) {

	/* if there is a client clip (always a region, for us) AND
		it has moved or is different OR
		the window has moved
	   we need to (re)translate it.
	*/
	if ( ( pGC->clientClipType == CT_REGION )
	  && ( ( changes & ( GCClipXOrigin | GCClipYOrigin | GCClipMask ) )
	    || ( ( pGC->lastWinOrg.x != pGC->lastWinOrg.x )
	      || ( pGC->lastWinOrg.y != pGC->lastWinOrg.y ) ) ) ) {
	    /* retranslate client clip */

	    (* pGC->pScreen->TranslateRegion)(
			   pGC->clientClip,
			   pGC->lastWinOrg.x + pGC->clipOrg.x,
			   pGC->lastWinOrg.y + pGC->clipOrg.y ) ;
	}

	if ( pWin ) {
	    int freeTmpClip, freeCompClip ;
	    RegionPtr pregWin ; /* clip for this window, without
				   client clip */

	    if ( pGC->subWindowMode == IncludeInferiors ) {
		pregWin = NotClippedByChildren(pWin ) ;
		freeTmpClip = TRUE ;
	    }
	    else {
		pregWin = &pWin->clipList ;
		freeTmpClip = FALSE ;
	    }
	    freeCompClip = devPriv->freeCompClip ;

	    /* if there is no client clip, we can get by with
	       just keeping the pointer we got, and remembering
	       whether or not should destroy (or maybe re-use)
	       it later.  this way, we avoid unnecessary copying
	       of regions.  (this wins especially if many clients clip
	       by children and have no client clip.)
	    */
	    if ( pGC->clientClipType == CT_NONE ) {
		if ( freeCompClip == TRUE ) {
		    (* pGC->pScreen->RegionDestroy) (devPriv->pCompositeClip ) ;
		}
		devPriv->pCompositeClip = pregWin ;
		devPriv->freeCompClip = freeTmpClip ;
	    }
	    else {
		/* we need one 'real' region to put into the composite
		   clip.
			if pregWin and the current composite clip
		   are real, we can get rid of one.
			if the current composite clip is real and
		   pregWin isn't, intersect the client clip and
		   pregWin into the existing composite clip.
			if pregWin is real and the current composite
		   clip isn't, intersect pregWin with the client clip
		   and replace the composite clip with it.
			if neither is real, create a new region and
		   do the intersection into it.
		*/

		if ( freeTmpClip == TRUE ) {
		    if ( freeCompClip == TRUE ) {
			(* pGC->pScreen->Intersect)(
			    devPriv->pCompositeClip,
			    pregWin,
			    pGC->clientClip ) ;
			(* pGC->pScreen->RegionDestroy)( pregWin ) ;
		    }
		    else if ( freeCompClip == FALSE ) {
			(* pGC->pScreen->Intersect)(
			   pregWin,
			   pregWin,
			   pGC->clientClip ) ;
			devPriv->pCompositeClip = pregWin ;
		    }
		}
		else if ( freeTmpClip == FALSE) {
		    if ( freeCompClip == TRUE ) {
		    (* pGC->pScreen->Intersect)(
			devPriv->pCompositeClip,
			pregWin,
			pGC->clientClip ) ;
		    }
		    else if ( freeCompClip == FALSE ) {
			devPriv->pCompositeClip =
			    (* pGC->pScreen->RegionCreate)( NULL, 1 ) ;
			(* pGC->pScreen->Intersect)(
			    devPriv->pCompositeClip,
			    pregWin,
			    pGC->clientClip ) ;
		    }
		}
		devPriv->freeCompClip = TRUE ;
		if ((pGC->clientClipType == CT_REGION)
		   &&(changes & (GCClipXOrigin | GCClipYOrigin | GCClipMask))) {
		/* retranslate client clip */
			(* pGC->pScreen->TranslateRegion)(pGC->clientClip,
				-(pDrawable->x + pGC->clipOrg.x),
				-(pDrawable->y + pGC->clipOrg.y));
	         }
	    }
	} /* end of composite clip for a window */
	else {
	    BoxRec pixbounds ;

	    pixbounds.x1 = 0 ;
	    pixbounds.y1 = 0 ;
	    pixbounds.x2 = pDrawable->width ;
	    pixbounds.y2 = pDrawable->height ;

	    if ( devPriv->freeCompClip == TRUE)
		(* pGC->pScreen->RegionReset)(
		    devPriv->pCompositeClip, &pixbounds ) ;
	    else {
		devPriv->freeCompClip = TRUE ;
		devPriv->pCompositeClip =
			(* pGC->pScreen->RegionCreate)( &pixbounds, 1 ) ;
	    }

	    if ( pGC->clientClipType == CT_REGION)
		(* pGC->pScreen->Intersect)(
		   devPriv->pCompositeClip,
		   devPriv->pCompositeClip,
		   pGC->clientClip ) ;
	} /* end of composite clip for pixmap */
    }

    changes &= ~ ( GCClipXOrigin | GCClipYOrigin | GCClipMask | GCSubwindowMode
		| GC_CALL_VALIDATE_BIT ) ;

    /* If needed, Calculate the Color Reduced Raster-Op */
    if ( changes & ( GCFillStyle | GCBackground | GCForeground
		   | GCPlaneMask | GCFunction ) )
		ppcGetReducedColorRrop( pGC, pDrawable->depth,
					&devPriv->colorRrop ) ;

	(* ( ( pDrawable->type == DRAWABLE_WINDOW )
	     ? devScrnPriv->changeWindowGC
	     : ppcChangePixmapGC ) )( pGC, changes ) ;

    return ;
}
