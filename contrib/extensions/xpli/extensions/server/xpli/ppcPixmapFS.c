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

/******************************************************************
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

/* $Header: /andrew/X11/r3src/release/server/ddx/ibm/ppc/RCS/ppcPixmapFS.c,v 1.2 89/04/29 22:13:30 jeff Exp $ */
/* $Source: /andrew/X11/r3src/release/server/ddx/ibm/ppc/RCS/ppcPixmapFS.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/release/server/ddx/ibm/ppc/RCS/ppcPixmapFS.c,v 1.2 89/04/29 22:13:30 jeff Exp $" ;
#endif

#include "X.h"
#include "misc.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "mfb.h"
#include "maskbits.h"

#include "OScompiler.h"

#include "ppc.h"
#include "ppcSpMcro.h"

#include "ibmTrace.h"
extern int mfbGCPrivateIndex;

#define LeftMostBitInScreenLongWord SCRLEFT( 0xFFFFFFFF, 31 )
/*
********** ********** ********** ********** ********** ********** **********
   these routines all clip.  they assume that anything that has called
them has already translated the points (i.e. pGC->miTranslate is
non-zero, which is howit gets set in mfbCreateGC().)

   the number of new scnalines created by clipping ==
MaxRectsPerBand * nSpans.
********** ********** ********** ********** ********** ********** **********
*/

void
ppcSolidPixmapFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
    DrawablePtr pDrawable ;
    GCPtr	pGC ;
    int		nInit ;			/* number of spans to fill */
    DDXPointPtr pptInit ;		/* pointer to list of start points */
    int		*pwidthInit ;		/* pointer to list of n widths */
    int 	fSorted ;
{
    register unsigned long int pm, npm ;
    register unsigned long int fg ;
    register int alu ;
				/* next three parameters are post-clip */
    int n ;			/* number of spans to fill */
    register DDXPointPtr ppt ;	/* pointer to list of start points */
    register int *pwidth ;	/* pointer to list of n widths */
    register unsigned char *addrl ;	/* pointer to current longword in bitmap */
    int i ;
    int *pwidthFree ;		/* copies of the pointers to free */
    DDXPointPtr pptFree ;

    TRACE(("ppcSolidPixmapFS(pDrawable=0x%x, pGC=0x%x, nInit=%d, pptInit=0x%x, pwidthInit=0x%x, fSorted=%d)\n", pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)) ;

    if ( pDrawable->type != DRAWABLE_PIXMAP ) {
	ErrorF("ppcSolidPixmapFS: drawable is not a pixmap\n") ;
	return ;
    }

    if ( ( alu = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.alu ) == GXnoop )
	return ;

    n = nInit * miFindMaxBand( ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->pCompositeClip) ;
    if ( !( pwidth = (int *) ALLOCATE_LOCAL( n * sizeof( int ) ) ) )
	return ;
    pwidthFree = pwidth ;

    if ( !( ppt = (DDXPointRec *)
	    ALLOCATE_LOCAL( n * sizeof( DDXPointRec ) ) ) ) {
	DEALLOCATE_LOCAL( pwidth ) ;
	return ;
    }
    pptFree = ppt ;

    n = miClipSpans( ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->pCompositeClip,
		     pptInit, pwidthInit, nInit, ppt, pwidth, fSorted ) ;

    pm = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.planemask ;
    fg = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.fgPixel ;
    npm = ( ~ pm ) & ( ( 1 << pDrawable->depth ) - 1 ) ;

    for ( ; n-- ; ppt++, pwidth++ ) {
        addrl = ( (unsigned char *) ( ( (PixmapPtr) pDrawable )->devPrivate.ptr ) )
	      + ( ppt->y * ( (int) ( ( (PixmapPtr) pDrawable )->devKind ) ) )
	      + ppt->x ;
	for ( i = *pwidth ; i-- ; addrl++ )
	    {
	    unsigned _p;
	    DoRop( _p, alu, fg, *addrl );
	    *addrl = ( *addrl & npm ) | ( pm & _p ) ;
	    }
#ifdef notdef /* PURDUE */
	    *addrl = ( *addrl & npm ) | ( pm & DoRop( alu, fg, *addrl ) ) ;
#endif /* PURDUE */
    }
    DEALLOCATE_LOCAL( pptFree ) ;
    DEALLOCATE_LOCAL( pwidthFree ) ;
    return ;
}

void
ppcStipplePixmapFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
register DrawablePtr pDrawable ;
GC *pGC ;
int nInit ;			/* number of spans to fill */
DDXPointPtr pptInit ;		/* pointer to list of start points */
int *pwidthInit ;		/* pointer to list of n widths */
int fSorted ;
{
    register unsigned char *pdst ; /* pointer to current word in bitmap */
    register int *psrc ;		/* pointer to current word in tile */
    register unsigned long int pm, npm ;
    register unsigned long int fg ;
    register int alu ;
				/* next three parameters are post-clip */
    int n ;			/* number of spans to fill */
    register DDXPointPtr ppt ;	/* pointer to list of start points */
    register int *pwidth ;	/* pointer to list of n widths */
    PixmapPtr	pTile ;		/* pointer to tile we want to fill with */
    int		width,  x, xSrc, ySrc ;
    int 	tlwidth, tileWidth, *psrcT ;
    int *pwidthFree ;		/* copies of the pointers to free */
    DDXPointPtr pptFree ;
    int xoff, count, stip, i ;

    TRACE(("ppcStipplePixmapFS(pDrawable=0x%x, pGC=0x%x, nInit=%d, pptInit=0x%x, pwidthInit=0x%x, fSorted=%d)\n",
		 pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)) ;

    if ( pDrawable->type != DRAWABLE_PIXMAP ) {
	ErrorF( "ppcStippleWindowFS: drawable is not a pixmap\n") ;
	return ;
    }
    if ( pGC->stipple->drawable.depth != 1 ) {
	ErrorF( "ppcStippleFS: bad depth\ntype = %d, depth = %d\n",
		pDrawable->type, pGC->stipple->drawable.depth ) ;
	return ;
    }

    if ( ( alu = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.alu ) == GXnoop )
	return ;

    SETSPANPTRS( nInit, n, pwidthInit, pwidth, pptInit,
		ppt, pwidthFree, pptFree, fSorted ) ;

    pm = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.planemask ;
    fg = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.fgPixel ;

    pTile = pGC->stipple ;
    tlwidth = pTile->devKind >> 2 ;

    tileWidth = pTile->drawable.width ;

    npm = ( ~ pm ) & ( ( 1 << pDrawable->depth ) - 1 ) ;

    /* this replaces rotating the stipple.  Instead, we just adjust the offset
     * at which we start grabbing bits from the stipple */
    xSrc = pGC->patOrg.x ;
    ySrc = pGC->patOrg.y ;
    while ( n-- ) {
        pdst = ( (unsigned char *) ( (PixmapPtr) pDrawable )->devPrivate.ptr )
	     + ( ppt->y * ( (int) ( ( (PixmapPtr) pDrawable )->devKind ) ) )
	     + ppt->x ;
        psrcT = (int *) pTile->devPrivate.ptr
	      + ( ( ( ppt->y - ySrc ) % pTile->drawable.height ) * tlwidth ) ;
	x = ppt->x ;

	xoff = ( x + xSrc ) % tileWidth ;
	psrc = psrcT + ( xoff >> 5 ) ;
	for ( width = *pwidth ; width ; psrc++, width -= count ) {
	    if ( psrc >= ( psrcT + tlwidth ) )
		psrc = psrcT ;
	    if ( ( psrc != ( psrcT + tlwidth - 1 ) )
	      || !( count = ( tileWidth & 0x1F ) ) )
		count = 32 ;
	    if ( count > width )
		count = width ;
	    getbits( psrc, xoff & 0x1F, count, stip ) ;
	    for ( i = count ; i-- ; ) {
		if ( stip & LeftMostBitInScreenLongWord )
		    {
		    unsigned _p;
		    DoRop( _p, alu, fg, *pdst ) ;
		    *pdst = ( *pdst & npm ) | ( pm & _p ) ;
		    }
#ifdef notdef /* PURDUE */
		    *pdst = ( *pdst & npm ) | ( pm & DoRop( alu, fg, *pdst ) ) ;
#endif /* PURDUE */
		pdst++ ;
		stip = SCRLEFT( stip, 1 ) ;
	    }
	}
	ppt++ ;
	pwidth++ ;
    }
    DEALLOCATE_LOCAL( pptFree ) ;
    DEALLOCATE_LOCAL( pwidthFree ) ;
    return ;
}

void
ppcOpStipplePixmapFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
DrawablePtr pDrawable ;
GC *pGC ;
int nInit ;			/* number of spans to fill */
DDXPointPtr pptInit ;		/* pointer to list of start points */
int *pwidthInit ;		/* pointer to list of n widths */
int fSorted ;
{
    register unsigned char *pdst ;	/* pointer to current word in bitmap */
    register unsigned long int pm, npm ;
    register unsigned long int fg, bg ;
    register int alu ;
				/* next three parameters are post-clip */
    int n ;			/* number of spans to fill */
    register DDXPointPtr ppt ;	/* pointer to list of start points */
    register int *pwidth ;	/* pointer to list of n widths */
    register int *psrc ;	/* pointer to current word in tile */
    PixmapPtr pTile ;		/* pointer to tile we want to fill with */
    int	width ;
    int tlwidth, tileWidth, *psrcT ;
    int *pwidthFree ;		/* copies of the pointers to free */
    DDXPointPtr pptFree ;
    int xoff, count, stip, i ;

    TRACE( ( "ppcOpStipplePixmapFS(pDrawable=0x%x,pGC=0x%x,nInit=%d,pptInit=0x%x,pwidthInit=0x%x,fSorted=%d)\n",
	   pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted ) ) ;

    if ( pGC->stipple->drawable.depth != 1 ) {
	ErrorF( "ppcOpStipplePixmapFS: bad depth\ntype = %d, depth = %d\n",
		pDrawable->type, pGC->stipple->drawable.depth ) ;
	return ;
    }

    if ( ( alu = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.alu ) == GXnoop )
	return ;

    SETSPANPTRS( nInit, n, pwidthInit, pwidth, pptInit,
		 ppt, pwidthFree, pptFree, fSorted ) ;

    fg = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.fgPixel ;
    bg = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.bgPixel ;
    pm = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.planemask ;
    npm = ( ~ pm ) & ( ( 1 << pDrawable->depth ) - 1 ) ;

    pTile = pGC->stipple ;
    tlwidth = pTile->devKind >> 2 ;
    tileWidth = pTile->drawable.width ;

    /* this replaces rotating the stipple.  Instead, we just adjust the offset
     * at which we start grabbing bits from the stipple */
    for ( ; n-- ; ppt++, pwidth++ ) {
        pdst = ( (unsigned char *) ( (PixmapPtr) pDrawable )->devPrivate.ptr )
	     + ( ppt->y * ( (int) ( (PixmapPtr) pDrawable )->devKind ) )
	     + ppt->x ;
        psrcT = (int *) pTile->devPrivate.ptr
	      + ( ( ( ppt->y - pGC->patOrg.y ) % pTile->drawable.height ) * tlwidth ) ;

	xoff = ( ppt->x + pGC->patOrg.x ) % tileWidth ;
	psrc = psrcT + ( xoff >> 5 ) ;
	for ( width = *pwidth ; width ; psrc++, width -= count ) {
	    if ( psrc >= ( psrcT + tlwidth ) )
		psrc = psrcT ;
	    if ( ( psrc != ( psrcT + tlwidth - 1 ) )
	     || !( count = tileWidth & 0x1F ) )
		    count = 32 ;
	    if ( count > width )
		count = width ;
	    getbits( psrc, xoff & 0x1F, count, stip ) ;
	    for ( i = count ; i-- ; pdst++, stip = SCRLEFT( stip, 1 ) )
		if ( stip & LeftMostBitInScreenLongWord )
		    {
		    unsigned _p;
		    DoRop( _p, alu, fg, *pdst ) ;
		    *pdst = ( *pdst & npm ) | ( pm & _p ) ;
		    }
#ifdef notdef /* PURDUE */
		    *pdst = ( *pdst & npm ) | ( pm & DoRop( alu, fg, *pdst ) ) ;
#endif /* PURDUE */
		else
		    {
		    unsigned _p;
		    DoRop( _p, alu, bg, *pdst ) ;
		    *pdst = ( *pdst & npm ) | ( pm & _p ) ;
		    }
#ifdef notdef /* PURDUE */
		    *pdst = ( *pdst & npm ) | ( pm & DoRop( alu, bg, *pdst ) ) ;
#endif /* PURDUE */
	}
    }
    DEALLOCATE_LOCAL( pptFree ) ;
    DEALLOCATE_LOCAL( pwidthFree ) ;
    return ;
}

void
ppcTilePixmapFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
register DrawablePtr pDrawable ;
GC *pGC ;
int nInit ;			/* number of spans to fill */
DDXPointPtr pptInit ;		/* pointer to list of start points */
int *pwidthInit ;		/* pointer to list of n widths */
int fSorted ;
{
    register DDXPointPtr ppt ;	/* pointer to list of start points */
    register int *pwidth ;	/* pointer to list of n widths */
    register unsigned char *pdst ;	/* pointer to current word in bitmap */
    register unsigned char *psrc ;	/* pointer to current word in tile */
    register PixmapPtr pTile ;	/* pointer to tile we want to fill with */
    int i ;
    int alu ;
    unsigned char pm, npm ;
				/* next three parameters are post-clip */
    int n ;			/* number of spans to fill */
    int tileWidth ;
    unsigned char *psrcT ;
    int *pwidthFree ;		/* copies of the pointers to free */
    DDXPointPtr pptFree ;

    TRACE( ( "ppcTileFS(pDrawable=0x%x,pGC=0x%x,nInit=%d,pptInit=0x%x,pwidthInit=0x%x,fSorted=%d)\n",
	    pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted ) ) ;

    if ( ( pDrawable->depth == 1 ) && ( pDrawable->type == DRAWABLE_PIXMAP ) ) {
	mfbTileFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted ) ;
	return ;
    }
    if ( !ppcDepthOK( pDrawable, pGC->tile.pixmap->drawable.depth ) ) {
	ErrorF( "ppcTileFS: bad depth\ntype = %d, depth = %d\n",
		pDrawable->type, pDrawable->depth) ;
	return ;
    }

    if ( ( alu = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.alu ) == GXnoop )
	return ;

    SETSPANPTRS( nInit, n, pwidthInit, pwidth, pptInit,
		 ppt, pwidthFree, pptFree, fSorted ) ;

    /* the following code is for 8 bits per pixel addressable memory only */
    pm = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.planemask ;
    npm = ( ~ pm ) & ( ( 1 << pDrawable->depth ) - 1 ) ;
    pTile = pGC->tile.pixmap ;
    tileWidth = pTile->drawable.width ;

    /* this replaces rotating the tile. Instead we just adjust the offset
     * at which we start grabbing bits from the tile */
    for ( ; n-- ; ppt++, pwidth++ ) {
        pdst = ( (unsigned char *) ( (PixmapPtr) pDrawable )->devPrivate.ptr )
	     + ( ppt->y * ( (int) ( (PixmapPtr) pDrawable )->devKind ) )
	     + ppt->x ;
        psrcT = (unsigned char *) pTile->devPrivate.ptr
      + ( ( ( ppt->y - pGC->patOrg.y ) % pTile->drawable.height ) * pTile->devKind ) ;

	psrc = psrcT + ( ( pGC->patOrg.x + ppt->x ) % tileWidth ) ;
	for ( i = *pwidth ; i-- ; pdst++, psrc++ ) {
	    if ( psrc >= ( psrcT + tileWidth ) )
		psrc = psrcT ;
	    {
	    unsigned _p;
	    DoRop( _p, alu, *psrc, *pdst ) ;
	    *pdst = ( *pdst & npm ) | ( pm & _p ) ;
	    }
#ifdef notdef /* PURDUE */
	    *pdst = ( *pdst & npm ) | ( pm & DoRop( alu, *psrc, *pdst ) ) ;
#endif /* PURDUE */
	}
    }
    DEALLOCATE_LOCAL( pptFree ) ;
    DEALLOCATE_LOCAL( pwidthFree ) ;
    return ;
}
