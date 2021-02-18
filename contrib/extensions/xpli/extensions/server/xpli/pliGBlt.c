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

/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliGBlt.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliGBlt.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */

#ifndef lint
static char *rcsid = "$Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliGBlt.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $" ;
#endif

/* Image Glyph Blt in terms of screen->rectFill and screen->glyphFill
 * for ppc
 *
 *  Tom Paquin 9/87
 *
 */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include        "misc.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"
#include	"mfb.h"
#include	"ppc.h"
#include	"maskbits.h"
#include	"ppcUtils.h"

extern PixmapPtr ppcCreatePixmap() ;

static
void filltempPixmap( pPixmap, pglyph, w, h )
register PixmapPtr pPixmap ;
register unsigned char *pglyph ;
register int w, h ;
{
register unsigned char *dest ;
register int djmp = (int) pPixmap->devKind ;

dest = pPixmap->devPrivate ;

while ( h-- ) {
	bcopy( pglyph, dest, w ) ;
	pglyph += w ;
	dest += djmp ;
}

return ;
}

void
ppcImageGlyphBlt( pDrawable, pGC, x, y, nglyph, ppci, pglyphBase )
DrawablePtr pDrawable ;
GC *pGC ;
register int x ;
int y ;
unsigned int nglyph ;
CharInfoPtr *ppci ;		/* array of character info */
unsigned char *pglyphBase ;	/* start of array of glyphs */
{
register unsigned char	*pglyph ;
ExtentInfoRec		info ;
BoxRec			bbox ;
xRectangle		backrect ;
CharInfoPtr		bigchar, pci ;
int			fg, bg, alu, pm ;
int			ax, ay, zx, zy, zw, zh ;
int			w, h, nbox, i ;
BoxPtr			pbox ;
short			merge ;
RegionPtr		pRegion ;
PixmapPtr		pTempPixmap = NULL ;
ScreenPtr		pScreen ;
int			been_here = FALSE ;
void			(*rectFunc)(), (*glyphFunc)(), (*stipFunc)() ;

if ( pDrawable->type == DRAWABLE_PIXMAP ) {
	miImageGlyphBlt( pDrawable, pGC, x, y, nglyph, ppci, pglyphBase ) ;
	return ;
}

pScreen = pGC->pScreen ;

rectFunc =  ( (ppcScrnPriv *) ( pScreen->devPrivate ) )->solidFill ;
stipFunc =  ( (ppcScrnPriv *) ( pScreen->devPrivate ) )->stipFill ;
glyphFunc = ( (ppcScrnPriv *) ( pScreen->devPrivate ) )->glyphFill ;

QueryGlyphExtents( pGC->font, ppci, nglyph, &info ) ;

x += ( (WindowPtr) pDrawable )->absCorner.x ;
y += ( (WindowPtr) pDrawable )->absCorner.y ;

backrect.x = x + info.overallLeft ;
backrect.y = y - pGC->font->pFI->fontAscent ;
backrect.width = info.overallRight - info.overallLeft ;
backrect.width = max( backrect.width, info.overallWidth ) ;
backrect.height = pGC->font->pFI->fontAscent
		+ pGC->font->pFI->fontDescent ;

bbox.x1 = x + info.overallLeft ;
bbox.x2 = x + info.overallRight ;
bbox.y1 = y - info.overallAscent ;
bbox.y2 = y + info.overallDescent ;

pRegion = ( (mfbPrivGC *) ( pGC->devPriv ) )->pCompositeClip ;
if ( !( nbox = pRegion->numRects ) )
	return ;
pbox = pRegion->rects ;

alu = pGC->alu ;
pm =  pGC->planemask ;
fg =  pGC->fgPixel ;
bg =  pGC->bgPixel ;
for ( i = 0 ; i < nbox ; i++, pbox++ ) {
	ax = MAX( pbox->x1, backrect.x ) ;
	ay = MAX( pbox->y1, backrect.y ) ;
	zx = MIN( pbox->x2, backrect.x + backrect.width ) ;
	zy = MIN( pbox->y2, backrect.y + backrect.height ) ;
	zw = zx - ax ;
	zh = zy - ay ;
	if ( ( zw > 0 ) && ( zh > 0 ) )
		(*rectFunc)( bg, alu, pm, ax, ay, zw, zh ) ;
}
switch ( (*pScreen->RectIn)( pRegion, &bbox ) ) {
	case rgnIN:
	while ( nglyph-- ) {
		pci = *ppci++ ;
		pglyph = pglyphBase + pci->byteOffset ;
		if ( ( w = GLYPHWIDTHPIXELS( pci ) )
		  && ( h = GLYPHHEIGHTPIXELS( pci ) ) )
			(*glyphFunc)( pglyph,
				      x + pci->metrics.leftSideBearing,
				      y - pci->metrics.ascent,
				      w, h, fg, alu, pm ) ;
		/* update character origin */
		x += pci->metrics.characterWidth ;
	} /* while nglyph-- */
	break ;
	case rgnPART: {
	int nbox ;
	BoxPtr pbox ;
	int i ;
	RegionPtr prgnClip ;

	BoxRec cbox ;
	int glx, gly ;

	while ( nglyph-- ) {
		pci = *ppci++ ;
		pglyph = pglyphBase + pci->byteOffset ;
		w = GLYPHWIDTHPIXELS( pci ) ;
		h = GLYPHHEIGHTPIXELS( pci ) ;
		cbox.x1 = glx = x + pci->metrics.leftSideBearing ;
		cbox.y1 = gly = y - pci->metrics.ascent ;
		cbox.x2 = cbox.x1 + w ;
		cbox.y2 = cbox.y1 + h ;

		switch ( (*pScreen->RectIn)( pRegion, &cbox ) ) {
			case rgnOUT:
				break ;
			case rgnIN:
				(*glyphFunc)( pglyph, glx, gly,
					      w, h, fg, alu, pm ) ;
				break ;
			case rgnPART:
				if ( !been_here ) {
					bigchar =
					&( pGC->font->pFI->maxbounds ) ;
					pTempPixmap =
					  ppcCreatePixmap( pScreen,
				   bigchar->metrics.rightSideBearing
				 - bigchar->metrics.leftSideBearing,
				   bigchar->metrics.ascent
				 + bigchar->metrics.descent, 1 ) ;
					been_here = TRUE ;
				}
				prgnClip = (*pScreen->RegionCreate)
					( &cbox, pRegion->numRects ) ;
				(*pScreen->Intersect)( prgnClip,
						       prgnClip,
						       pRegion ) ;
				pbox = prgnClip->rects ;
				if ( !( nbox = prgnClip->numRects ) )
					break ;
				filltempPixmap( pTempPixmap, pglyph,
						GLYPHWIDTHBYTESPADDED( pci ),
						h ) ;
				for ( i = 0 ; i < nbox ; i++, pbox++ ) {
					ax = MAX( pbox->x1, cbox.x1 ) ;
					ay = MAX( pbox->y1, cbox.y1 ) ;
					zx = MIN( pbox->x2, cbox.x2 ) ;
					zy = MIN( pbox->y2, cbox.y2 ) ;
					zw = zx - ax ;
					zh = zy - ay ;
					if ( ( zw > 0 ) && ( zh > 0 ) )
						(*stipFunc)( pTempPixmap,
							     fg, alu, pm,
							     ax, ay, zw, zh,
							     glx, gly ) ;
				}
				break ;
		}
		/* update character origin */
		x += pci->metrics.characterWidth ;
	} /* while nglyph-- */
	if ( pTempPixmap )
		mfbDestroyPixmap( pTempPixmap ) ;
	} /* End "case rgnPART" */
	case rgnOUT:
	default:
		break ;
}

return ;
}

void
ppcPolyGlyphBlt ( pDrawable, pGC, x, y, nglyph, ppci, pglyphBase )
DrawablePtr pDrawable ;
GC	*pGC ;
int	x, y ;
unsigned int nglyph ;
CharInfoPtr *ppci ;		/* array of character info */
unsigned char *pglyphBase ;	/* start of array of glyphs */
{
register unsigned char	*pglyph ;
ExtentInfoRec		info ;
BoxRec			bbox ;
xRectangle		backrect ;
CharInfoPtr		bigchar, pci ;
int			fg, bg, alu, pm ;
int			ax, ay, zx, zy, zw, zh,
			w, h, nbox, i ;
BoxPtr			pbox ;
short			merge ;
RegionPtr		pRegion ;
PixmapPtr		pTempPixmap = NULL ;
ScreenPtr		pScreen ;
int			been_here = FALSE ;
void			(*glyphFunc)(), (*stipFunc)() ;

if ( ( pDrawable->type == DRAWABLE_PIXMAP )
  || ( pGC->fillStyle != FillSolid ) ) {
	miPolyGlyphBlt( pDrawable, pGC, x, y, nglyph, ppci, pglyphBase ) ;
	return ;
}

pScreen = pGC->pScreen ;

stipFunc =  ( (ppcScrnPriv *) ( pScreen->devPrivate ) )->stipFill ;
glyphFunc = ( (ppcScrnPriv *) ( pScreen->devPrivate ) )->glyphFill ;

QueryGlyphExtents( pGC->font, ppci, nglyph, &info ) ;

x += ( (WindowPtr) pDrawable )->absCorner.x ;
y += ( (WindowPtr) pDrawable )->absCorner.y ;

bbox.x1 = x + info.overallLeft ;
bbox.x2 = x + info.overallRight ;
bbox.y1 = y - info.overallAscent ;
bbox.y2 = y + info.overallDescent ;

pRegion = ( (mfbPrivGC *) ( pGC->devPriv ) )->pCompositeClip ;
pbox = pRegion->rects ;
if ( !( nbox = pRegion->numRects ) )
	return ;

alu = pGC->alu ;
pm =  pGC->planemask ;
fg =  pGC->fgPixel ;
bg =  pGC->bgPixel ;

switch ( ( *pScreen->RectIn ) ( pRegion, &bbox ) ) {
	case rgnOUT:
		break ;
	case rgnIN:
		while ( nglyph-- ) {
			pci = *ppci++ ;
			pglyph = pglyphBase + pci->byteOffset ;
			if ( ( w = GLYPHWIDTHPIXELS( pci ) )
			  && ( h = GLYPHHEIGHTPIXELS( pci ) ) )
				(*glyphFunc)
				  ( pglyph,
				     x + pci->metrics.leftSideBearing,
				     y - pci->metrics.ascent,
				     w, h, fg, alu, pm ) ;
			/* update character origin */
			x += pci->metrics.characterWidth ;
		} /* while nglyph-- */
		break ;
	case rgnPART: {
		int nbox ;
		BoxPtr pbox ;
		int i ;
		RegionPtr prgnClip ;

		BoxRec cbox ;
		int glx, gly ;

		while ( nglyph-- ) {
			pci = *ppci++ ;
			pglyph = pglyphBase + pci->byteOffset ;
			w = GLYPHWIDTHPIXELS( pci ) ;
			h = GLYPHHEIGHTPIXELS( pci ) ;
			cbox.x1 = glx = x + pci->metrics.leftSideBearing ;
			cbox.y1 = gly = y - pci->metrics.ascent ;
			cbox.x2 = cbox.x1 + w ;
			cbox.y2 = cbox.y1 + h ;

			switch ( (*pScreen->RectIn)( pRegion, &cbox ) ) {
				case rgnOUT:
					break ;
				case rgnIN:
					(*glyphFunc)( pglyph, glx, gly,
						      w, h, fg, alu,
						      pm ) ;
					break ;
				case rgnPART:
					if ( !been_here ) {
						bigchar =
					&( pGC->font->pFI->maxbounds ) ;
						pTempPixmap =
					ppcCreatePixmap ( pScreen,
				bigchar->metrics.rightSideBearing
					- bigchar->metrics.leftSideBearing,
				bigchar->metrics.ascent
					+ bigchar->metrics.descent,
				1 ) ;
						been_here = TRUE ;
					}
					prgnClip =
		(*pScreen->RegionCreate)( &cbox, pRegion->numRects ) ;
					(*pScreen->Intersect)( prgnClip,
							       prgnClip,
							       pRegion ) ;
					pbox = prgnClip->rects ;
					if ( !( nbox = prgnClip->numRects ) )
						break ;
					filltempPixmap( pTempPixmap, pglyph,
						GLYPHWIDTHBYTESPADDED( pci ),
							 h ) ;
					for ( i = 0 ; i < nbox ; i++, pbox++ ) {
						ax = MAX( pbox->x1, cbox.x1 ) ;
						ay = MAX( pbox->y1, cbox.y1 ) ;
						zx = MIN( pbox->x2, cbox.x2 ) ;
						zy = MIN( pbox->y2, cbox.y2 ) ;
						zw = zx - ax ;
						zh = zy - ay ;
						if ( ( zw > 0 ) && ( zh > 0 ) )
							(*stipFunc)(
								pTempPixmap,
								fg, alu, pm,
								ax, ay, zw, zh,
								glx, gly ) ;
					}
					break ;
			}
			/* update character origin */
			x += pci->metrics.characterWidth ;
		} /* while nglyph-- */

		if ( pTempPixmap )
			mfbDestroyPixmap( pTempPixmap ) ;

	}
	default:
		break ;
}

return ;
}
