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

/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcFillSp.c,v 9.4 89/05/07 15:22:52 paul Exp $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcFillSp.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcFillSp.c,v 9.4 89/05/07 15:22:52 paul Exp $" ;
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

void ppcSolidPixmapFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
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
    unsigned char *addrlBase ;		/* pointer to start of bitmap */
    int width ;			/* width in bytes of bitmap */
    register unsigned char *addrl ;	/* pointer to current longword in bitmap */
    int i ;
    int *pwidthFree ;		/* copies of the pointers to free */
    DDXPointPtr pptFree ;

    TRACE(("ppcSolidPixmapFS(pDrawable=0x%x, pGC=0x%x, nInit=%d, pptInit=0x%x, pwidthInit=0x%x, fSorted=%d)\n", pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)) ;

    if ( pDrawable->type != DRAWABLE_PIXMAP ) {
	ErrorF("ppcSolidPixmapFS: drawable is not a pixmap\n") ;
	return ;
    }

    if ( ( alu = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.alu ) == GXnoop )
	return ;

    pm = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.planemask ;
    fg = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.fgPixel ;

	{
	n = nInit * miFindMaxBand( ( (ppcPrivGC *) pGC->devPriv )->pCompositeClip ) ;
	if ( !( pwidth = (int *) ALLOCATE_LOCAL( n * sizeof( int ) ) ) )
		return ;
	if ( !( ppt = (DDXPointRec *)
			ALLOCATE_LOCAL( n * sizeof( DDXPointRec ) ) ) ) {
		DEALLOCATE_LOCAL( pwidth ) ;
		return ;
    		}
	pwidthFree = pwidth ;
	pptFree = ppt ;
	n = miClipSpans( ( (ppcPrivGC *) pGC->devPriv )->pCompositeClip,
		pptInit, pwidthInit, nInit,
		ppt, pwidth, fSorted ) ;
	}


    addrlBase = (unsigned char *) ( ( (PixmapPtr) pDrawable )->devPrivate ) ;
    width = (int) ( ( (PixmapPtr) pDrawable )->devKind ) ;
    npm = ( ~ pm ) & ( ( 1 << pDrawable->depth ) - 1 ) ;

    while ( n-- ) {
        addrl = addrlBase + (ppt->y * width) + ppt->x ;

	for ( i = *pwidth ; i-- ; addrl++ )
	    *addrl = ( *addrl & npm ) | ( pm & DoRop( alu, fg, *addrl ) ) ;
	pwidth++ ;
	ppt++ ;
    }
    DEALLOCATE_LOCAL(pptFree) ;
    DEALLOCATE_LOCAL(pwidthFree) ;
    return ;
}

void 
ppcSolidWindowFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
    DrawablePtr pDrawable ;
    GCPtr	pGC ;
    int		nInit ;			/* number of spans to fill */
    DDXPointPtr pptInit ;		/* pointer to list of start points */
    int		*pwidthInit ;		/* pointer to list of n widths */
    int 	fSorted ;
{
    register void (*fnp)() ;
    register unsigned long int pm ;
    register unsigned long int fg ;
    register int alu ;
				/* next three parameters are post-clip */
    int n ;			/* number of spans to fill */
    register DDXPointPtr ppt ;	/* pointer to list of start points */
    register int *pwidth ;	/* pointer to list of n widths */
    int *pwidthFree ;		/* copies of the pointers to free */
    DDXPointPtr pptFree ;

    TRACE(("ppcSolidWindowFS(pDrawable=0x%x, pGC=0x%x, nInit=%d, pptInit=0x%x, pwidthInit=0x%x, fSorted=%d)\n", pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)) ;

    if ( pDrawable->type != DRAWABLE_WINDOW ) {
	ErrorF( "ppcSolidWindowFS: drawable is not a window\n") ;
	return ;
    }

    if ( ( alu = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.alu ) == GXnoop )
	return ;

	n = nInit * miFindMaxBand( ( (ppcPrivGC *) pGC->devPriv )->pCompositeClip ) ;
	if ( !( pwidth = (int *) ALLOCATE_LOCAL( n * sizeof( int ) ) ) )
		return ;
	if ( !( ppt = (DDXPointRec *)
			ALLOCATE_LOCAL( n * sizeof( DDXPointRec ) ) ) ) {
		DEALLOCATE_LOCAL( pwidth ) ;
		return ;
    		}
	pwidthFree = pwidth ;
	pptFree = ppt ;
	n = miClipSpans( ( (ppcPrivGC *) pGC->devPriv )->pCompositeClip,
		pptInit, pwidthInit, nInit,
		ppt, pwidth, fSorted ) ;

    fnp = ((ppcScrnPriv *)(pDrawable->pScreen->devPrivate))->solidFill ;
    pm = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.planemask ;
    fg = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.fgPixel ;

    while ( n-- ) {
	if ( *pwidth )
	    (*fnp)( fg,alu,pm,ppt->x,ppt->y,*pwidth,1) ;
	ppt++ ; pwidth++ ;
    }

    DEALLOCATE_LOCAL(pptFree) ;
    DEALLOCATE_LOCAL(pwidthFree) ;
    return ;
}

void
ppcStipplePixmapFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
DrawablePtr pDrawable ;
GC		*pGC ;
int		nInit ;		/* number of spans to fill */
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
    int		iline ;		/* first line of tile to use */
    unsigned char *addrlBase ;	/* pointer to start of bitmap */
    int		 nlwidth ;	/* width in longwords of bitmap */
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

    if ( ( alu = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.alu ) == GXnoop )
	return ;

    SETSPANPTRS( nInit, n, pwidthInit, pwidth, pptInit,
		ppt, pwidthFree, pptFree, fSorted ) ;

    pm = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.planemask ;
    fg = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.fgPixel ;

    addrlBase = (unsigned char *) ((PixmapPtr)pDrawable)->devPrivate ;
    nlwidth = (int) ( ( (PixmapPtr) pDrawable )->devKind ) ;

    pTile = pGC->stipple ;
    tlwidth = pTile->devKind >> 2 ;

    tileWidth = pTile->width ;

    npm = ( ~ pm ) & ( ( 1 << pDrawable->depth ) - 1 ) ;

    /* this replaces rotating the stipple.  Instead, we just adjust the offset
     * at which we start grabbing bits from the stipple */
    xSrc = pGC->patOrg.x ;
    ySrc = pGC->patOrg.y ;
    while ( n-- ) {
	iline = (ppt->y - ySrc) % pTile->height ;
        pdst = addrlBase + (ppt->y * nlwidth) + (ppt->x) ;
        psrcT = (int *) pTile->devPrivate + (iline * tlwidth) ;
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
	    getbits(psrc, xoff & 0x1F, count, stip) ;
	    for ( i = count ; i-- ; ) {
		if ( stip & LeftMostBitInScreenLongWord )
		    *pdst = ( *pdst & npm ) | ( pm & DoRop(alu, fg, *pdst) ) ;
		pdst++ ;
		stip = SCRLEFT( stip, 1 ) ;
	    }
	}
	ppt++ ;
	pwidth++ ;
    }
    DEALLOCATE_LOCAL(pptFree) ;
    DEALLOCATE_LOCAL(pwidthFree) ;
    return ;
}

void
ppcStippleWindowFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
DrawablePtr pDrawable ;
GC		*pGC ;
int		nInit ;		/* number of spans to fill */
DDXPointPtr pptInit ;		/* pointer to list of start points */
int *pwidthInit ;		/* pointer to list of n widths */
int fSorted ;
{
    register void (*fnp)() ;
    register unsigned long int pm ;
    register unsigned long int fg ;
    register int alu ;
				/* next three parameters are post-clip */
    int n ;			/* number of spans to fill */
    register DDXPointPtr ppt ;	/* pointer to list of start points */
    register int *pwidth ;	/* pointer to list of n widths */
    PixmapPtr	pTile ;		/* pointer to tile we want to fill with */
    int		xSrc, ySrc ;
    int *pwidthFree ;		/* copies of the pointers to free */
    DDXPointPtr pptFree ;

    TRACE(("ppcStippleWindowFS(pDrawable=0x%x, pGC=0x%x, nInit=%d, pptInit=0x%x, pwidthInit=0x%x, fSorted=%d)\n",
		 pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)) ;

    if ( pDrawable->type != DRAWABLE_WINDOW ) {
	ErrorF( "ppcStippleWindowFS: drawable is not a window\n" ) ;
	return ;
    }

    if ( pGC->stipple->drawable.depth != 1 ) {
	ErrorF("ppcStippleFS: bad depth\ntype = %d, depth = %d\n",
		pDrawable->type, pGC->stipple->drawable.depth) ;
	return ;
    }

    if ( ( alu = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.alu ) == GXnoop )
	return ;

    SETSPANPTRS(nInit, n, pwidthInit, pwidth, pptInit,
		ppt, pwidthFree, pptFree, fSorted) ;

    pm = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.planemask ;
    fg = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.fgPixel ;

    xSrc = pGC->patOrg.x + ( (WindowPtr) pDrawable )->absCorner.x ;
    ySrc = pGC->patOrg.y + ( (WindowPtr) pDrawable )->absCorner.y ;
    pTile = pGC->stipple ;
    fnp = ( (ppcScrnPriv *)(pDrawable->pScreen->devPrivate))->stipFill ;

    while ( n-- ) {
	(*fnp)(pTile,fg,alu,pm,ppt->x,ppt->y,*pwidth,1,xSrc,ySrc) ;
	ppt++ ; pwidth++ ;
    }

    DEALLOCATE_LOCAL(pptFree) ;
    DEALLOCATE_LOCAL(pwidthFree) ;

    return ;
}

void
ppcOpStipplePixmapFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
DrawablePtr pDrawable ;
GC		*pGC ;
int		nInit ;		/* number of spans to fill */
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
    int		iline ;		/* first line of tile to use */
    unsigned char		*addrlBase ;	/* pointer to start of bitmap */
    int		 nlwidth ;	/* width in longwords of bitmap */
    register int *psrc ;		/* pointer to current word in tile */
    PixmapPtr	pTile ;		/* pointer to tile we want to fill with */
    int		width,  x, xSrc, ySrc ;
    int 	tlwidth, tileWidth, *psrcT ;
    int *pwidthFree ;		/* copies of the pointers to free */
    DDXPointPtr pptFree ;
    int xoff, count, stip, i ;

    TRACE(("ppcOpStipplePixmapFS(pDrawable=0x%x, pGC=0x%x, nInit=%d, pptInit=0x%x, pwidthInit=0x%x, fSorted=%d)\n", pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)) ;

    if ( pGC->stipple->drawable.depth != 1 ) {
	ErrorF("ppcOpStipplePixmapFS: bad depth\ntype = %d, depth = %d\n",
		pDrawable->type, pGC->stipple->drawable.depth ) ;
	return ;
    }

    if ( ( alu = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.alu ) == GXnoop )
	return ;

    SETSPANPTRS(nInit, n, pwidthInit, pwidth, pptInit,
		ppt, pwidthFree, pptFree, fSorted) ;

    pm = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.planemask ;
    fg = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.fgPixel ;
    bg = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.bgPixel ;

    addrlBase = (unsigned char *) ((PixmapPtr)pDrawable)->devPrivate ;
    nlwidth = (int)(((PixmapPtr)pDrawable)->devKind) ;

    npm = ( ~ pm ) & ( ( 1 << pDrawable->depth ) - 1 ) ;

    pTile = pGC->stipple ;
    tlwidth = pTile->devKind >> 2 ;

    tileWidth = pTile->width ;

    /* this replaces rotating the stipple.  Instead, we just adjust the offset
     * at which we start grabbing bits from the stipple */
    xSrc = pGC->patOrg.x ;
    ySrc = pGC->patOrg.y ;
    while ( n-- ) {
	iline = (ppt->y - ySrc) % pTile->height ;
        pdst = addrlBase + (ppt->y * nlwidth) + (ppt->x) ;
        psrcT = (int *) pTile->devPrivate + (iline * tlwidth) ;
	x = ppt->x ;

	xoff = ( x + xSrc ) % tileWidth ;
	psrc = psrcT + ( xoff >> 5 ) ;
	for ( width = *pwidth ; width ; psrc++, width -= count ) {
	    if ( psrc >= ( psrcT + tlwidth ) )
		psrc = psrcT ;
	    if ( ( psrc != ( psrcT + tlwidth - 1 ) )
	     || !( count = tileWidth & 0x1F ) )
		    count = 32 ;
	    if ( count > width )
		count = width ;
	    getbits( psrc, xoff & 0x1F, count, stip) ;
	    for ( i = count ; i-- ; ) {
		if ( stip & LeftMostBitInScreenLongWord )
		    *pdst = ( *pdst & npm ) | ( pm & DoRop( alu, fg, *pdst) ) ;
		else
		    *pdst = ( *pdst & npm ) | ( pm & DoRop( alu, bg, *pdst) ) ;
		pdst++ ;
		stip = SCRLEFT( stip, 1 ) ;
	    }
	}
	ppt++ ;
	pwidth++ ;
    }
    DEALLOCATE_LOCAL(pptFree) ;
    DEALLOCATE_LOCAL(pwidthFree) ;
    return ;
}

void
ppcOpStippleWindowFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
DrawablePtr pDrawable ;
GC		*pGC ;
int		nInit ;		/* number of spans to fill */
DDXPointPtr pptInit ;		/* pointer to list of start points */
int *pwidthInit ;		/* pointer to list of n widths */
int fSorted ;
{
    register void (*fnp)() ;
    unsigned long int pm ;
    unsigned long int fg, bg ;
    int alu ;
				/* next three parameters are post-clip */
    int n ;			/* number of spans to fill */
    register DDXPointPtr ppt ;	/* pointer to list of start points */
    register int *pwidth ;	/* pointer to list of n widths */
    PixmapPtr	pTile ;		/* pointer to tile we want to fill with */
    int		xSrc, ySrc ;
    int *pwidthFree ;		/* copies of the pointers to free */
    DDXPointPtr pptFree ;

    TRACE(("ppcOpStippleWindowFS(pDrawable=0x%x, pGC=0x%x, nInit=%d, pptInit=0x%x, pwidthInit=0x%x, fSorted=%d)\n", pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)) ;

    if ( pGC->stipple->drawable.depth != 1 ) {
	ErrorF( "ppcOpStippleWindowFS: bad depth\ntype = %d, depth = %d\n",
		pDrawable->type, pGC->stipple->drawable.depth) ;
	return ;
    }

    if ( ( alu = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.alu ) == GXnoop )
	return ;

    SETSPANPTRS(nInit, n, pwidthInit, pwidth, pptInit,
		ppt, pwidthFree, pptFree, fSorted) ;

    pm = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.planemask ;
    fg = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.fgPixel ;
    bg = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.bgPixel ;

    xSrc = pGC->patOrg.x ;
    ySrc = pGC->patOrg.y ;
    pTile =pGC->stipple ;
    fnp = ((ppcScrnPriv *)(pDrawable->pScreen->devPrivate))->opstipFill ;

    while ( n-- ) {
	(*fnp)( pTile, fg, bg, alu, pm,
		ppt->x, ppt->y, *pwidth, 1, xSrc, ySrc ) ;
	ppt++ ; pwidth++ ;
    }

    DEALLOCATE_LOCAL(pptFree) ;
    DEALLOCATE_LOCAL(pwidthFree) ;
    return ;
}

void
ppcTileFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
DrawablePtr pDrawable ;
GC		*pGC ;
int		nInit ;		/* number of spans to fill */
DDXPointPtr pptInit ;		/* pointer to list of start points */
int *pwidthInit ;		/* pointer to list of n widths */
int fSorted ;
{
    int		iline ;		/* first line of tile to use */
				/* next three parameters are post-clip */
    int n ;			/* number of spans to fill */
    register DDXPointPtr ppt ;	/* pointer to list of start points */
    register int *pwidth ;	/* pointer to list of n widths */
    unsigned char	*addrlBase ;	/* pointer to start of bitmap */
    int		 nlwidth ;	/* width in longwords of bitmap */
    register unsigned char *pdst ;	/* pointer to current word in bitmap */
    register unsigned char *psrc ;	/* pointer to current word in tile */
    PixmapPtr	pTile ;		/* pointer to tile we want to fill with */
    int		xSrc, ySrc ;
    int 	tlwidth, tileWidth ;
    unsigned char *psrcT ;
    int *pwidthFree ;		/* copies of the pointers to free */
    DDXPointPtr pptFree ;
    unsigned char pm, npm ;
    int i, alu ;

    TRACE(("ppcTileFS(pDrawable=0x%x, pGC=0x%x, nInit=%d, pptInit=0x%x, pwidthInit=0x%x, fSorted=%d)\n", pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)) ;

    if ( ( pDrawable->depth == 1 ) && ( pDrawable->type == DRAWABLE_PIXMAP ) ) {
	mfbTileFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted ) ;
	return ;
    }
    if ( !ppcDepthOK( pDrawable, pGC->tile->drawable.depth ) ) {
	ErrorF( "ppcTileFS: bad depth\ntype = %d, depth = %d\n",
		pDrawable->type, pDrawable->depth) ;
	return ;
    }

    if ( ( alu = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.alu ) == GXnoop )
	return ;
    pm = ( (ppcPrivGC *) pGC->devPriv )->colorRrop.planemask ;
    pTile= pGC->tile ;

    SETSPANPTRS(nInit, n, pwidthInit, pwidth, pptInit,
		ppt, pwidthFree, pptFree, fSorted) ;

    if ( pDrawable->type == DRAWABLE_WINDOW ) {
	void (* fnp)() ;

    	xSrc = pGC->patOrg.x + (((WindowPtr)pDrawable)->absCorner.x) ;
    	ySrc = pGC->patOrg.y + (((WindowPtr)pDrawable)->absCorner.y) ;
	fnp = (((ppcScrnPriv *)(pDrawable->pScreen->devPrivate))->tileFill) ;
    	while (n--) {
		(* fnp)(pTile,alu,pm,ppt->x,ppt->y,*pwidth,1,xSrc,ySrc) ;
		ppt++ ;
		pwidth++ ;
	}
    	DEALLOCATE_LOCAL(pptFree) ;
    	DEALLOCATE_LOCAL(pwidthFree) ;
	return ;
    }

    /* the following code is for 8 bits per pixel addressable memory only */
    addrlBase = (unsigned char *) ((PixmapPtr)pDrawable)->devPrivate ;
    nlwidth = (int)(((PixmapPtr)pDrawable)->devKind) ;
    npm = ( ~ pm ) & ( ( 1 << pDrawable->depth ) - 1 ) ;
    tlwidth = pTile->devKind ;
    tileWidth = pTile->width ;

    /* this replaces rotating the tile. Instead we just adjust the offset
     * at which we start grabbing bits from the tile */
    xSrc = pGC->patOrg.x ;
    ySrc = pGC->patOrg.y ;

    while (n--)
    {
	iline = (ppt->y - ySrc) % pTile->height ;
        pdst = addrlBase + (ppt->y * nlwidth) + (ppt->x) ;
        psrcT = (unsigned char *) pTile->devPrivate + (iline * tlwidth) ;

	psrc = psrcT + ( ( xSrc + ppt->x ) % tileWidth ) ;
	for ( i = *pwidth ; i-- ; ) {
	    if ( psrc >= ( psrcT + tileWidth ) )
		psrc = psrcT ;
	    *pdst = ( *pdst & npm ) | ( pm & DoRop( alu, *psrc, *pdst) ) ;
	    pdst++ ;
	    psrc++ ;
	}
	ppt++ ;
	pwidth++ ;
    }
    DEALLOCATE_LOCAL(pptFree) ;
    DEALLOCATE_LOCAL(pwidthFree) ;
    return ;
}

void ppcOpStippleFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable ;
    GCPtr	pGC ;
    int		nInit ;			/* number of spans to fill */
    DDXPointPtr pptInit ;		/* pointer to list of start points */
    int		*pwidthInit ;		/* pointer to list of n widths */
    int 	fSorted ;
{
    TRACE(("ppcOpStippleFS(pDrawable=0x%x, pGC=0x%x, nInit=%d, pptInit=0x%x, pwidthInit=0x%x, fSorted=%d)\n", pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)) ;

    (* ( ( pDrawable->type == DRAWABLE_PIXMAP )
	 ? ppcOpStipplePixmapFS
	 : ppcOpStippleWindowFS ) )( pDrawable, pGC, nInit, pptInit,
				     pwidthInit, fSorted ) ;

    return ;
}

void ppcStippleFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable ;
    GCPtr	pGC ;
    int		nInit ;			/* number of spans to fill */
    DDXPointPtr pptInit ;		/* pointer to list of start points */
    int		*pwidthInit ;		/* pointer to list of n widths */
    int 	fSorted ;
{
    TRACE(("ppcStippleFS(pDrawable=0x%x, pGC=0x%x, nInit=%d, pptInit=0x%x, pwidthInit=0x%x, fSorted=%d)\n", pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)) ;

    (* ( ( pDrawable->type == DRAWABLE_PIXMAP )
	 ? ppcStipplePixmapFS 
	 : ppcStippleWindowFS ) )( pDrawable, pGC, nInit, pptInit,
				   pwidthInit, fSorted ) ;

    return ;
}

void ppcSolidFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable ;
    GCPtr	pGC ;
    int		nInit ;			/* number of spans to fill */
    DDXPointPtr pptInit ;		/* pointer to list of start points */
    int		*pwidthInit ;		/* pointer to list of n widths */
    int 	fSorted ;
{
register void (*spanFunc)() ;

    TRACE(("ppcSolidFS(pDrawable=0x%x, pGC=0x%x, nInit=%d, pptInit=0x%x, pwidthInit=0x%x, fSorted=%d)\n", pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)) ;

if ( pDrawable->type == DRAWABLE_WINDOW ) {
#ifdef PAQUINDOESNTLIKETHIS
    if ( pDrawable->depth == 1 )
    {
	ErrorF("ppcSolidFS: window depth == 1\n") ;
	return ;
    }
    else
#endif PAQUINDOESNTLIKETHIS
	spanFunc = ppcSolidWindowFS ;
}
else { /* pDrawable->type == DRAWABLE_PIXMAP */
    if ( pDrawable->depth == 1 ) {
	switch ( ( (ppcPrivGC *) pGC->devPriv )->colorRrop.alu )
	    {
	    case GXclear :
		spanFunc = mfbBlackSolidFS ;
		break ;
	    case GXset :
		spanFunc = mfbWhiteSolidFS ;
		break ;
	    case GXinvert:
		spanFunc = mfbInvertSolidFS ;
		break ;
	     default:
		ErrorF( "ppcFillSpans: unexpected Raster-Op" ) ;
		break ;
	    }
    }
    else /* pDrawable->depth > 1 */
	spanFunc = ppcSolidPixmapFS ;
}

(*spanFunc)( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted ) ;

return ;
}

void 
ppcFillSpans(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable ;
    GCPtr	pGC ;
    int		nInit ;			/* number of spans to fill */
    DDXPointPtr pptInit ;		/* pointer to list of start points */
    int		*pwidthInit ;		/* pointer to list of n widths */
    int 	fSorted ;
{
register void (*spanFunc)() ;

if ( pDrawable->type == DRAWABLE_WINDOW )
	switch ( ( (ppcPrivGC *) pGC->devPriv )->colorRrop.fillStyle ) {
		case FillSolid :
		    spanFunc = ppcSolidWindowFS ;
		    break ;
		case FillStippled :
		    spanFunc = ppcStippleWindowFS ;
		    break ;
		case FillOpaqueStippled :
		   spanFunc = ppcOpStippleWindowFS ;
		   break ;
		case FillTiled :
		    spanFunc = ppcTileFS ;
		    break ;
		default :
		    ErrorF( "ppcFillSpans: unknown Fill Style" ) ;
		    break ;
	}
else /* pDrawable->type == DRAWABLE_PIXMAP */
	switch ( ( (ppcPrivGC *) pGC->devPriv )->colorRrop.fillStyle ) {
		case FillSolid :
		    if ( pDrawable->depth == 1 ) {
			switch ( ( (ppcPrivGC *) pGC->devPriv )->colorRrop.alu )
			    {
			    case GXclear :
				spanFunc = mfbBlackSolidFS ;
				break ;
			    case GXset :
				spanFunc = mfbWhiteSolidFS ;
				break ;
			    case GXinvert:
				spanFunc = mfbInvertSolidFS ;
				break ;
			     default:
				ErrorF( "ppcFillSpans: unexpected Raster-Op" ) ;
				break ;
			    }
		    }
		    else /* pDrawable->depth > 1 */
			spanFunc = ppcSolidPixmapFS ;
		    break ;
		case FillStippled :
		    spanFunc = ppcStipplePixmapFS ;
		    break ;
		case FillOpaqueStippled :
		    spanFunc = ppcOpStipplePixmapFS ;
		    break ;
		case FillTiled :
		    spanFunc = ppcTileFS ;
		    break ;
		default :
		    ErrorF( "ppcFillSpans: unknown Fill Style" ) ;
		    break ;
	}

(*spanFunc)( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted ) ;

return ;
}
