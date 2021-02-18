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

#include "X.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "mfb.h"
#include "mistruct.h"
#include "regionstr.h"

#include "ppc.h"
#include "ibmTrace.h"

extern	int		ibmAllowBackingStore ;

/*
   ppcCopyWindow copies only the parts of the destination that are
visible in the source.
*/

void 
ppcCopyWindow(pWin, ptOldOrg, prgnSrc)
    register WindowPtr pWin ;
    DDXPointRec ptOldOrg ;
    RegionPtr prgnSrc ;
{
    RegionPtr prgnDst ;
    register BoxPtr pbox ;
    register int dx, dy ;
    register int nbox ;
    register int pm ;

    register void (*fnp)() ;
    BoxPtr pboxTmp, pboxNext, pboxBase, pboxNew ;
				/* temporaries for shuffling rectangles */

    TRACE(("ppcCopyWindow(pWin= 0x%x, ptOldOrg= 0x%x, prgnSrc= 0x%x)\n", pWin, ptOldOrg, prgnSrc)) ;


    dx = ptOldOrg.x - pWin->drawable.x ;
    dy = ptOldOrg.y - pWin->drawable.y ;
    (* pWin->drawable.pScreen->TranslateRegion)(prgnSrc, -dx, -dy) ;

    prgnDst = (* pWin->drawable.pScreen->RegionCreate)(NULL, 1);
    (* pWin->drawable.pScreen->Intersect)(prgnDst, &pWin->borderClip, prgnSrc) ;

    if ( !( nbox = REGION_NUM_RECTS(prgnDst) ) )
	return;

    pbox = REGION_RECTS(prgnDst);

    pboxNew = 0 ;
    if ( nbox > 1 ) {
	if ( dy < 0 ) {
	    if ( dx > 0 ) {
		/* walk source bottom to top */
		/* keep ordering in each band, reverse order of bands */
		if ( !( pboxNew =
			(BoxPtr) ALLOCATE_LOCAL( sizeof( BoxRec ) * nbox ) ) )
			return ;
		pboxBase = pboxNext = pbox+nbox - 1 ;
		while ( pboxBase >= pbox ) {
			while ( ( pboxNext >= pbox )
			     && ( pboxBase->y1 == pboxNext->y1 ) )
				pboxNext-- ;
			pboxTmp = pboxNext + 1 ;
			while ( pboxTmp <= pboxBase )
				*pboxNew++ = *pboxTmp++ ;
			pboxBase = pboxNext ;
		}
		pboxNew -= nbox ;
		pbox = pboxNew ;
	    }
	    else { /* dx <= 0 */
		/* we can just reverse the entire list in place */
		/* Do three-position swaps */
		BoxRec tmpBox ;

		pboxBase = pbox ;
		pboxNext = pbox + nbox - 1 ;
		while ( pboxBase < pboxNext ) {
			/* ****** Warning Structure Assignment !! ****** */
			tmpBox    = *pboxBase ;
			*pboxBase = *pboxNext ;
			*pboxNext = tmpBox ;
			pboxBase++ ;
			pboxNext-- ;
		}
	    }
	}
        else if ( dx < 0 ) {
	/* walk source right to left */
	    /* reverse order of rects in each band */
	    if ( !( pboxNew = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox) ) )
		return ;
	    pboxBase = pboxNext = pbox ;
	    while (pboxBase < pbox+nbox)
	    {
		while ((pboxNext < pbox+nbox) &&
		       (pboxNext->y1 == pboxBase->y1))
		    pboxNext++ ;
		pboxTmp = pboxNext ;
		while (pboxTmp != pboxBase)
		    *pboxNew++ = *--pboxTmp ;
		pboxBase = pboxNext ;
	    }
	    pboxNew -= nbox ;
	    pbox = pboxNew ;
	}
    } /* END if nbox > 1 */

    /*
     * call blit several times, the parms are:
     *   blit( alu,rplanes, wplanes, srcx, srcy, destx, desty, width, height ) ;
     */

    pm = ( 1 << pWin->drawable.depth ) - 1 ;
    fnp = ((ppcScrnPriv *)(pWin->drawable.pScreen->devPrivate))->blit ;
    for ( ; nbox-- ; pbox++ )
	(* fnp)( GXcopy, pm, pm, 
		 pbox->x1 + dx, pbox->y1 + dy, 
		 pbox->x1, pbox->y1, 
		 pbox->x2 - pbox->x1, pbox->y2 - pbox->y1) ;

    /* free up stuff */
    if ( pboxNew )
	DEALLOCATE_LOCAL( pboxNew ) ;

    (* pWin->drawable.pScreen->RegionDestroy)(prgnDst) ;
    return ;
}

/* As The Name Says -- Used For ega, vga and apa8c */
void 
ppcCopyWindowForXYhardware(pWin, ptOldOrg, prgnSrc)
    register WindowPtr pWin ;
    DDXPointRec ptOldOrg ;
    RegionPtr prgnSrc ;
{
    RegionPtr prgnDst ;
    register BoxPtr pbox ;
    register int dx, dy ;
    register int nbox ;
    register int pm ;
    int box_direction = 1 ; /* go forward */

    register void (*fnp)() ;
    BoxPtr pboxTmp, pboxNext, pboxBase, pboxNew ;
				/* temporaries for shuffling rectangles */

    TRACE(("ppcCopyWindow(pWin= 0x%x, ptOldOrg= 0x%x, prgnSrc= 0x%x)\n", pWin, ptOldOrg, prgnSrc)) ;


    dx = ptOldOrg.x - pWin->drawable.x ;
    dy = ptOldOrg.y - pWin->drawable.y ;

    prgnDst = (* pWin->drawable.pScreen->RegionCreate)(NULL, 1);
    (* pWin->drawable.pScreen->RegionCopy)( prgnDst, prgnSrc ) ;
    (* pWin->drawable.pScreen->TranslateRegion)(prgnDst, -dx, -dy) ;
    (* pWin->drawable.pScreen->Intersect)(prgnDst, &pWin->borderClip, prgnDst) ;

    if ( !( nbox = REGION_NUM_RECTS(prgnDst)))
	return ;

    pbox = REGION_RECTS(prgnDst);

    pboxNew = 0 ;
    if ( nbox > 1 ) {
	if ( dy < 0 ) {
	    if ( dx > 0 ) {
		/* walk source bottom to top */
		/* keep ordering in each band, reverse order of bands */
		if ( !( pboxNew =
			(BoxPtr) ALLOCATE_LOCAL( sizeof( BoxRec ) * nbox ) ) )
			return ;
		pboxBase = pboxNext = pbox+nbox - 1 ;
		while ( pboxBase >= pbox ) {
			while ( ( pboxNext >= pbox )
			     && ( pboxBase->y1 == pboxNext->y1 ) )
				pboxNext-- ;
			pboxTmp = pboxNext + 1 ;
			while ( pboxTmp <= pboxBase )
				*pboxNew++ = *pboxTmp++ ;
			pboxBase = pboxNext ;
		}
		pboxNew -= nbox ;
		pbox = pboxNew ;
	    }
	    else { /* dx <= 0 */
		/* we can just reverse the entire list in place */
		/* Do three-position swaps */
		/* ****** Warning Structure Assignment !! ****** */
		box_direction = -1 ;
	    }
	}
        else if ( dx < 0 ) {
	/* walk source right to left */
	    /* reverse order of rects in each band */
	    if ( !( pboxNew = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox) ) )
		return ;
	    pboxBase = pboxNext = pbox ;
	    while ( pboxBase < pbox + nbox ) {
		while ( ( pboxNext < pbox + nbox )
		     && ( pboxNext->y1 == pboxBase->y1 ) )
		    pboxNext++ ;
		pboxTmp = pboxNext ;
		while ( pboxTmp != pboxBase )
		    *pboxNew++ = *--pboxTmp ;
		pboxBase = pboxNext ;
	    }
	    pboxNew -= nbox ;
	    pbox = pboxNew ;
	}
    } /* END if nbox > 1 */

    /*
     *  OPTIMIZATION FOR XY-Oriented Hardware:
     * Only Move Planes That The Client Has Declared An Interest In !!
     *
     * call blit several times, the parms are:
     *   blit( alu,rplanes, wplanes, srcx, srcy, destx, desty, width, height ) ;
     */

    pm = (~( ~0 << pWin->drawable.depth ) ) & pWin->optional->backingBitPlanes ;
    fnp = ((ppcScrnPriv *)(pWin->drawable.pScreen->devPrivate))->blit ;
    if ( box_direction < 0 )
	pbox += nbox - 1 ;
    for ( ; nbox-- ; pbox += box_direction )
	(* fnp)( GXcopy, pm, pm, 
		 pbox->x1 + dx, pbox->y1 + dy, 
		 pbox->x1, pbox->y1, 
		 pbox->x2 - pbox->x1, pbox->y2 - pbox->y1) ;
    /*
     * Fill In the non-overlapping regions with backing pixel !!
     * (* ... ->solidFill)( color, alu, planes, x0, y0, lx, ly )
     */
    if ( pm = ( ~( ~0 << pWin->drawable.depth ) ) & ~ pWin->optional->backingBitPlanes ) {
	(* pWin->drawable.pScreen->Subtract)( prgnDst, prgnDst, prgnSrc ) ;
	if ( nbox = REGION_NUM_RECTS(prgnDst)) {
	    unsigned long int fg = pWin->optional->backingPixel ;

	    fnp =
	      ((ppcScrnPriv *)(pWin->drawable.pScreen->devPrivate))->solidFill ;
	    for ( pbox = REGION_RECTS(prgnDst) ; nbox-- ; pbox++ )
		(* fnp)( fg, GXcopy, pm,
			 pbox->x1, pbox->y1,
			 pbox->x2 - pbox->x1, pbox->y2 - pbox->y1 ) ;
	    }
	}

    /* free up stuff */
    if ( pboxNew )
	DEALLOCATE_LOCAL( pboxNew ) ;

    (* pWin->drawable.pScreen->RegionDestroy)(prgnDst) ;
    return ;
}

Bool ppcPositionWindow(pWin, x, y)
register WindowPtr pWin ;
register int x, y ;
{
    return TRUE ;
}

Bool
ppcUnrealizeWindow( pWindow, x, y)
register WindowPtr pWindow ;
register int x,y ;
{
return pWindow ? TRUE : FALSE ;
}

Bool
ppcRealizeWindow( pWindow )
register WindowPtr pWindow ;
{
return pWindow ? TRUE : FALSE ;
}

Bool 
ppcDestroyWindow(pWin)
register WindowPtr pWin ;
{
return pWin ? TRUE : FALSE ;
}

Bool
ppcCreateWindow(pWin)
register WindowPtr pWin ;
{
    register mfbPrivWin *pPrivWin;

    TRACE(("ppcCreateWindow(pWin= 0x%x)\n", pWin));

    pPrivWin = pWin->devPrivates[mfbWindowPrivateIndex].ptr;
    pPrivWin->pRotatedBorder = NullPixmap;
    pPrivWin->pRotatedBackground = NullPixmap;
    pPrivWin->fastBackground = 0;
    pPrivWin->fastBorder = 0;

    return TRUE;
}

/* As The Name Says -- Used For ega, vga and apa8c */
Bool
ppcCreateWindowForXYhardware(pWin)
register WindowPtr pWin ;
{
    register mfbPrivWin *pPrivWin;

    TRACE(("ppcCreateWindowForXYhardware (pWin= 0x%x)\n", pWin));

    pPrivWin = pWin->devPrivates[mfbWindowPrivateIndex].ptr;
    pPrivWin->pRotatedBorder = NullPixmap;
    pPrivWin->pRotatedBackground = NullPixmap;
    pPrivWin->fastBackground = 0;
    pPrivWin->fastBorder = 0;

    pWin->drawable.pScreen->CopyWindow = ppcCopyWindowForXYhardware ;

    return TRUE;
}
