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

/* $Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/ppc/RCS/ppcArea.c,v 9.4 88/11/21 14:58:39 paul Exp $ */
/* $Source: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/ppc/RCS/ppcArea.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/ppc/RCS/ppcArea.c,v 9.4 88/11/21 14:58:39 paul Exp $" ;
#endif

/* 
 * ppc solid area fill
 *
 * Tom Paquin 8/87 
 */

#include "X.h"
#include "pixmapstr.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "miscstruct.h"
#include "scrnintstr.h"

#include "ppc.h"

#include "ibmTrace.h"
extern int mfbGCPrivateIndex;

void
ppcAreaFill( pWin, nboxes, pBox, pGC )
    register WindowPtr pWin ;
    register int nboxes ;
    register BoxPtr pBox ;
    GCPtr	pGC ;
{
register void (*FillFunc)() ;
register int x, y, w, h ;
int alu ;
unsigned long int fg, bg, pm ;
int xSrc, ySrc ;
PixmapPtr pPixmap ;

TRACE( ( "ppcAreaFill(0x%x,%d,0x%x,0x%x)\n", pWin, nboxes, pBox, pGC ) ) ;

if ( ( alu = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.alu ) == GXnoop
  || !nboxes )
	return ;

xSrc = pGC->patOrg.x + pWin->drawable.x ;
ySrc = pGC->patOrg.y + pWin->drawable.y ;

pm = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.planemask ;
fg = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.fgPixel ;
bg = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.bgPixel ;

nboxes++ ;
switch (  ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.fillStyle ) {
	case FillTiled:
		FillFunc =
 ( (ppcScrnPriv *) ( ( (DrawablePtr) pWin )->pScreen->devPrivate ) )->tileFill ;
		for ( pPixmap = pGC->tile.pixmap ; --nboxes ; pBox++ )
			if ( ( w = pBox->x2 - ( x = pBox->x1 ) )
			  && ( h = pBox->y2 - ( y = pBox->y1 ) ) )
				(*FillFunc)( pPixmap, alu, pm,
					     x, y, w, h, xSrc, ySrc ) ;
		break ;
	case FillOpaqueStippled:
		FillFunc =
( (ppcScrnPriv *) ( ( (DrawablePtr) pWin )->pScreen->devPrivate ) )->opstipFill;
		for ( pPixmap = pGC->stipple ; --nboxes ; pBox++ )
			if ( ( w = pBox->x2 - ( x = pBox->x1 ) )
			  && ( h = pBox->y2 - ( y = pBox->y1 ) ) )
				(*FillFunc)( pPixmap, fg, bg, alu, pm,
					     x, y, w, h, xSrc, ySrc ) ;
		break ;
	case FillStippled:
		FillFunc =
( (ppcScrnPriv *) ( ( (DrawablePtr) pWin )->pScreen->devPrivate ) )->stipFill ;
		for ( pPixmap = pGC->stipple ; --nboxes ; pBox++ )
			if ( ( w = pBox->x2 - ( x = pBox->x1 ) )
			  && ( h = pBox->y2 - ( y = pBox->y1 ) ) )
				(*FillFunc)( pPixmap, fg, alu, pm,
					     x, y, w, h, xSrc, ySrc ) ;
		break ;
	case FillSolid:
		FillFunc =
( (ppcScrnPriv *) ( ( (DrawablePtr) pWin )->pScreen->devPrivate ) )->solidFill ;
		for ( ; --nboxes ; pBox++ )
			if ( ( w = pBox->x2 - ( x = pBox->x1 ) )
			  && ( h = pBox->y2 - ( y = pBox->y1 ) ) )
				(*FillFunc)( fg, alu, pm, x, y, w, h ) ;
		break ;
}

return ;
}
