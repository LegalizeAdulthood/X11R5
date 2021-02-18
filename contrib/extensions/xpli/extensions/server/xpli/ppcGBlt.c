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

/* Image Glyph Blt in terms of screen->rectFill and screen->glyphFill
 * for ppc
 *
 *  Tom Paquin 9/87
 *
 */

/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcGBlt.c,v 9.2 89/06/21 22:28:17 jeff Exp $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcGBlt.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcGBlt.c,v 9.2 89/06/21 22:28:17 jeff Exp $";
#endif


#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include        "misc.h"
#include	"font.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"

#include	"OScompiler.h"

#include	"ppc.h"
#include	"ibmTrace.h"

extern PixmapPtr ppcCreatePixmap() ;
extern int mfbGCPrivateIndex;

void
filltempPixmap( pPixmap, pglyph, w, h )
	 PixmapPtr pPixmap ;
register unsigned char *pglyph ;
register const int w ;
register int h ;
{
register unsigned char *dest = pPixmap->devPrivate.ptr;
register int		dstWidth= pPixmap->devKind;

while ( h-- ) {
	MOVE( pglyph, dest, w ) ;
	pglyph += w ;
	dest += dstWidth;
}

return ;
}

void
ppcImageGlyphBlt( pDrawable, pGC, x, y, nglyph, ppci, pglyphBase )
    DrawablePtr pDrawable ;
    GC 		*pGC ;
    int 	x, y ;
    unsigned int nglyph ;
    CharInfoPtr *ppci ;		/* array of character info */
    unsigned char *pglyphBase ;	/* start of array of glyphs */
{
    register unsigned char	*pglyph ;
    register BoxPtr		pbox ;
    register int		nbox ;
    ExtentInfoRec 		info ;	
    BoxRec 			bbox ;	
    xRectangle 			backrect ;
    CharInfoPtr  		pci ;
    FontInfoPtr			pfi ;
    int 			fg, bg, pm, ax,ay,zx,zy,zw,zh,
 		   		w, h, widthGlyph ;

    RegionPtr 			pRegion ;

    PixmapPtr			pTempPixmap = NULL ;
    ScreenPtr			pScreen ;
    ppcScrnPriv			*pScrPriv ;
    int				been_here = FALSE ;
    void			(*rectFunc)(), (*glyphFunc)(), (*stipFunc)() ;
    int				CursorIsSaved ;

    if ( pDrawable->type == DRAWABLE_PIXMAP ) {
	miImageGlyphBlt( pDrawable, pGC, x, y, nglyph, ppci, pglyphBase ) ;
	return ;
    }

    pRegion = ((ppcPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip ; 
    if ( !( nbox = REGION_NUM_RECTS(pRegion) ) )
	return ;

    pScreen = pGC->pScreen ;
    pScrPriv = (ppcScrnPriv *) pScreen->devPrivate ;
	
    pfi = ( FontInfoPtr ) &(pGC->font->info);

    rectFunc = pScrPriv->solidFill ;
    stipFunc = pScrPriv->stipFill ;
    glyphFunc = pScrPriv->glyphFill ;

    QueryGlyphExtents(pGC->font, ppci, nglyph, &info) ;

    x += pDrawable->x ;
    y += pDrawable->y ;

    backrect.y = y - pfi->fontAscent ;
    backrect.height = pfi->fontAscent + pfi->fontDescent ;
    backrect.width =
	 MAX( ( info.overallRight - info.overallLeft ), info.overallWidth ) ;
    backrect.x = x + info.overallLeft ;

    /* High Level Software Cursor Support */
    CursorIsSaved = !(*(pScrPriv->CursorSemaphore))
		 && (* pScrPriv->CheckCursor)( backrect.x, backrect.y,
					       backrect.width,
					       backrect.height ) ;
    (*(pScrPriv->CursorSemaphore))++ ;

    bbox.x1 = x + info.overallLeft ;
    bbox.x2 = x + info.overallRight ;
    bbox.y1 = y - info.overallAscent ;
    bbox.y2 = y + info.overallDescent ;

    pm =  pGC->planemask ;
    bg =  pGC->bgPixel ;
    for ( pbox = REGION_RECTS(pRegion) ; nbox-- ; pbox++ ) {
	ax = MAX( pbox->x1, backrect.x ) ;
	ay = MAX( pbox->y1, backrect.y ) ;
	zx = MIN( pbox->x2, backrect.x + backrect.width ) ;
	zy = MIN( pbox->y2, backrect.y + backrect.height ) ;
	if ( ( ( zw = zx - ax ) > 0 ) && ( ( zh = zy - ay ) > 0 ) )
	    (* rectFunc)( bg, GXcopy, pm, ax, ay, zw, zh ) ;
    }

    fg =  pGC->fgPixel ;

    switch ( (* pScreen->RectIn)( pRegion, &bbox ) ) {
      case rgnIN:
        while ( nglyph-- ) {
	    pci = *ppci++ ;
	    pglyph = (unsigned char *  ) pci->bits ;
	    if ( ( w = GLYPHWIDTHPIXELS(pci) )
	      && ( h = GLYPHHEIGHTPIXELS(pci) ) )
		    (*glyphFunc)( pglyph,
				  x + pci->metrics.leftSideBearing,
				  y - pci->metrics.ascent,
				  w, h, fg, GXcopy, pm ) ;
	    x += pci->metrics.characterWidth ;	/* update character origin */
        } /* while nglyph-- */
      case rgnOUT:
	break ;
      case rgnPART:
      {
	RegionPtr prgnClip ;

	BoxRec cbox ;
	int glx, gly ;

        while ( nglyph-- )
        {
	    pci = *ppci++ ;
	    pglyph = (unsigned char * ) pci->bits ;
	    w = GLYPHWIDTHPIXELS(pci) ;
	    h = GLYPHHEIGHTPIXELS(pci) ;
	    cbox.x1 = glx = x + pci->metrics.leftSideBearing ;
	    cbox.x2 = cbox.x1 + w ;
	    cbox.y1 = gly = y - pci->metrics.ascent ;
	    cbox.y2 = cbox.y1 + h ;

	    switch ( (* pScreen->RectIn)( pRegion,  &cbox ) ) {
	      case rgnIN:
	    	(* glyphFunc)( pglyph, glx, gly, w, h, fg, GXcopy, pm ) ;
	      case rgnOUT:
		break ;
	      case rgnPART:
		if ( !been_here )
			{
			widthGlyph = pfi->maxbounds.rightSideBearing-
				        pfi->minbounds.leftSideBearing;
			widthGlyph = PADGLYPHWIDTHBYTES(widthGlyph);
    			pTempPixmap = ppcCreatePixmap( pScreen, 
				widthGlyph * 8,
				pfi->maxbounds.ascent +
					pfi->maxbounds.descent ,
				1 ) ;
			been_here = TRUE ;
			}
		prgnClip = (* pScreen->RegionCreate)( &cbox,
						      REGION_NUM_RECTS(pRegion)) ;
		(* pScreen->Intersect)( prgnClip, prgnClip, pRegion ) ;
		if ( !( nbox = REGION_NUM_RECTS(prgnClip) ) ) {
		    (* pScreen->RegionDestroy)( prgnClip ) ;
		    break ;
		}
		widthGlyph = GLYPHWIDTHBYTESPADDED(pci) ;
	    	filltempPixmap(pTempPixmap, pglyph, widthGlyph, h) ;
		for ( pbox = REGION_RECTS(prgnClip) ; nbox-- ; pbox++ )
			{
			ax = MAX(pbox->x1, cbox.x1) ;
			ay = MAX(pbox->y1, cbox.y1) ;
			zx = MIN(pbox->x2, cbox.x2) ;
			zy = MIN(pbox->y2, cbox.y2) ;
	    		if ( ( ( zw = zx - ax ) > 0 )
	    		  && ( ( zh = zy - ay ) > 0 ) )
				(*stipFunc)( pTempPixmap, 
					     fg, GXcopy, pm, ax, ay,
					     zw, zh, glx, gly ) ;
			}
		(* pScreen->RegionDestroy)( prgnClip ) ;
		break ;
	    }
	    /* update character origin */
	    x += pci->metrics.characterWidth ;
        } /* while nglyph-- */

	if ( pTempPixmap )
	    (* pScreen->DestroyPixmap)( pTempPixmap ) ;

      }
      default:
	break ;
    }
    if ( !--(*(pScrPriv->CursorSemaphore)) && CursorIsSaved )
	(* pScrPriv->ReplaceCursor)() ;

    return ;
}

void
ppcPolyGlyphBlt( pDrawable, pGC, x, y, nglyph, ppci, pglyphBase )
    DrawablePtr pDrawable ;
    GC 		*pGC ;
    int 	x, y ;
    unsigned int nglyph ;
    CharInfoPtr *ppci ;		/* array of character info */
    unsigned char *pglyphBase ;	/* start of array of glyphs */
{
    register unsigned char	*pglyph ;
    register BoxPtr		pbox ;
    register int		nbox ;
    ExtentInfoRec 		info ;	
    BoxRec 			bbox ;	

    CharInfoPtr  		pci ;
    FontInfoPtr			pfi ;
    int 			fg, alu, pm, ax,ay,zx,zy,zw,zh,
 		   		w, h, widthGlyph ;

    RegionPtr 			pRegion ;

    PixmapPtr			pTempPixmap = NULL ;
    ScreenPtr			pScreen ;
    ppcScrnPriv			*pScrPriv ;
    int				been_here = FALSE ;
    void			(*glyphFunc)(), (*stipFunc)() ;
    int				CursorIsSaved ;

    if ( pDrawable->type == DRAWABLE_PIXMAP ) {
	miPolyGlyphBlt( pDrawable, pGC, x, y, nglyph, ppci, pglyphBase ) ;
	return ;
    }

    if ( ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr)->colorRrop.fillStyle != FillSolid ) {
	miPolyGlyphBlt(pDrawable, pGC, x, y, nglyph, ppci, pglyphBase) ;
	return ;
    }

    pRegion = ((ppcPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip ; 
    if ( !( nbox = REGION_NUM_RECTS(pRegion)) )
	return ;

    pScreen = pGC->pScreen ;
    pScrPriv = (ppcScrnPriv *) pScreen->devPrivate ;
    pfi = (FontInfoPtr) & (pGC->font->info);

    stipFunc = pScrPriv->stipFill ;
    glyphFunc = pScrPriv->glyphFill ;

    QueryGlyphExtents(pGC->font, ppci, nglyph, &info) ;

    x += pDrawable->x ;
    y += pDrawable->y ;

    bbox.x1 = x + info.overallLeft ;
    bbox.x2 = x + info.overallRight ;
    bbox.y1 = y - info.overallAscent ;
    bbox.y2 = y + info.overallDescent ;

    /* High Level Software Cursor Support */
    CursorIsSaved = !(*(pScrPriv->CursorSemaphore))
		 && (* pScrPriv->CheckCursor)( bbox.x1, bbox.y1,
					       info.overallRight
					     - info.overallLeft,
					       info.overallAscent
					     + info.overallDescent ) ;
    (*(pScrPriv->CursorSemaphore))++ ;

    pbox = REGION_RECTS(pRegion) ;

    /* Use Reduced Raster-Op */
    alu = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr)->colorRrop.alu ;
    pm =  ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr)->colorRrop.planemask ;
    fg =  ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr)->colorRrop.fgPixel ;

    switch ( (* pScreen->RectIn)( pRegion, &bbox ) ) {
      case rgnOUT:
	break ;
      case rgnIN:
        while (nglyph-- )
        {
	    pci = *ppci++ ;
	    pglyph = (unsigned char * ) pci->bits ;
	    if ( ( w = GLYPHWIDTHPIXELS(pci) )
	      && ( h = GLYPHHEIGHTPIXELS(pci) ) )
		    (* glyphFunc)( pglyph,
				   x + pci->metrics.leftSideBearing,
				   y - pci->metrics.ascent,
				   w, h, fg, alu, pm ) ;
	    x += pci->metrics.characterWidth ;	/* update character origin */
        } /* while nglyph-- */
	break ;
      case rgnPART:
      {
	RegionPtr prgnClip ;

	BoxRec cbox ;
	int glx, gly ;

        while ( nglyph-- ) {
	    pci = *ppci++ ;
	    pglyph = (unsigned char * ) pci->bits ;
	    w = GLYPHWIDTHPIXELS(pci) ;
	    h = GLYPHHEIGHTPIXELS(pci) ;
	    cbox.x1 = glx = x + pci->metrics.leftSideBearing ;
	    cbox.y1 = gly = y - pci->metrics.ascent ;
	    cbox.x2 = cbox.x1 + w ;
	    cbox.y2 = cbox.y1 + h ;

	    switch ( (* pScreen->RectIn)( pRegion,  &cbox ) ) {
	      case rgnIN:
	    	(* glyphFunc)( pglyph, glx, gly, w, h, fg, alu, pm ) ;
	      case rgnOUT:
		break ;
	      case rgnPART:
		if ( !been_here ) {
			widthGlyph= pfi->maxbounds.rightSideBearing-
					pfi->maxbounds.leftSideBearing;
			widthGlyph= PADGLYPHWIDTHBYTES(widthGlyph);
    			pTempPixmap = ppcCreatePixmap(pScreen, 
				widthGlyph*8,
				pfi->maxbounds.ascent 
					+ pfi->maxbounds.descent, 
				1) ;
			been_here = TRUE ;
		}
		prgnClip = (*pScreen->RegionCreate)(&cbox,REGION_NUM_RECTS(pRegion)) ;
		(*pScreen->Intersect) (prgnClip, prgnClip, pRegion) ;
		if ( !( nbox = REGION_NUM_RECTS(prgnClip) ) ) {
		    (* pScreen->RegionDestroy)( prgnClip ) ;
		    break ;
		}
		widthGlyph = GLYPHWIDTHBYTESPADDED(pci) ;
	    	filltempPixmap( pTempPixmap, pglyph, widthGlyph, h ) ;
		for ( pbox = REGION_RECTS(prgnClip) ; nbox-- ; pbox++ ) {
			ax = MAX(pbox->x1, cbox.x1) ;
			ay = MAX(pbox->y1, cbox.y1) ;
			zx = MIN(pbox->x2, cbox.x2) ;
			zy = MIN(pbox->y2, cbox.y2) ;
			if ( ( ( zw = zx - ax ) > 0 )
			  && ( ( zh = zy - ay ) > 0 ) )
				(*stipFunc)(pTempPixmap, 
					fg, alu, pm, ax, ay, zw, zh,
					glx, gly) ;
		}
		(* pScreen->RegionDestroy)( prgnClip ) ;
		break ;
	    }
	    /* update character origin */
	    x += pci->metrics.characterWidth ;
        } /* while nglyph-- */
	if ( pTempPixmap )
	    (* pScreen->DestroyPixmap)( pTempPixmap ) ;
      }
      default:
	break ;
    }
    if ( !--(*(pScrPriv->CursorSemaphore)) && CursorIsSaved )
	(* pScrPriv->ReplaceCursor)() ;

    return ;
}
