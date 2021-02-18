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

/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcImg.c,v 9.2 89/05/07 15:27:33 paul Exp $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcImg.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcImg.c,v 9.2 89/05/07 15:27:33 paul Exp $";
#endif

#include "X.h"
#include "misc.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "windowstr.h"
#include "scrnintstr.h"
#include "regionstr.h"
#include "servermd.h"

#include "OScompiler.h"

#include "ppc.h"
#include "ibmTrace.h"

/* GETBITSPERPIXEL -- Find out how many bits per pixel are supported at
 * this depth -- another helper function
 */
static
int
GetBitsPerPixel( depth ) 
    int	depth ;
{
    register int     i ;

    for ( i = screenInfo.numPixmapFormats ; i-- ; ) 
	if ( screenInfo.formats[i].depth == depth ) 
		return screenInfo.formats[i].bitsPerPixel ;
    return 1 ;
}

/* Was MIGETIMAGE -- public entry for the GetImage Request
 * We're getting the image into a memory buffer. While we have to use GetSpans
 * to read a line from the device ( since we don't know what that looks like ) ,
 * we can just write into the destination buffer
 *
 * two different strategies are used, depending on whether we're getting the
 * image in Z format or XY format
 * Z format:
 * Line at a time, GetSpans a line and bcopy it to the destination
 * buffer, except that if the planemask is not all ones, we create a
 * temporary pixmap and do a SetSpans into it ( to get bits turned off ) 
 * and then another GetSpans to get stuff back ( because pixmaps are
 * opaque, and we are passed in the memory to write into ) .  This is
 * completely ugly and slow but works, but the interfaces just aren't
 * designed for this case.  Life is hard.
 * XY format:
 * get the single plane specified in planemask
 */
void
ppcGetImage( pDraw, sx, sy, w, h, format, planeMask, pdstLine ) 
    DrawablePtr pDraw ;
    int	sx, sy, w, h ;
    unsigned int format ;
    unsigned long int planeMask ;
    pointer	     pdstLine ;
{
    int	    depth, i, linelength, width ;
    DDXPointRec	pt ;
    unsigned char *pbits ;
    unsigned long int	gcv[2] ;
    PixmapPtr	pPixmap = (PixmapPtr) NULL ;
    GCPtr	pGC ;
    unsigned char *pDst = (unsigned char *) pdstLine ;

    depth = pDraw->depth ;
    if ( format == ZPixmap ) {
	linelength = PixmapBytePad( w, depth ) ;
	if ( pDraw->type == DRAWABLE_WINDOW ) {
	    sx += pDraw->x ;
	    sy += pDraw->y ;
	}
	if ( ( ( ( 1 << pDraw->depth ) - 1 ) & planeMask )
		 != ( 1 << pDraw->depth ) - 1 ) {
	    pGC = GetScratchGC( depth, pDraw->pScreen ) ;
	    pPixmap = (PixmapPtr)
	      (* pDraw->pScreen->CreatePixmap)( pDraw->pScreen, w, h, depth ) ;
	    gcv[0] = GXcopy ;
	    gcv[1] = planeMask ;
	    DoChangeGC( pGC, GCPlaneMask | GCFunction, gcv, 0 ) ;
	    ValidateGC( pPixmap, pGC ) ;

	    pbits = (unsigned char *)ALLOCATE_LOCAL(w);

	    for ( i = 0 ; i < h ; i++ ) {
		pt.x = sx ;
		pt.y = sy + i ;
		width = w ;
		(* pDraw->pScreen->GetSpans)( pDraw, w, &pt, &width, 1, pbits ) ;
		pt.x = 0 ;
		pt.y = 0 ;
		width = w ;
		(* pGC->ops->SetSpans)( pPixmap, pGC, pbits, &pt, &width, 1, TRUE ) ;
		(* pDraw->pScreen->GetSpans)( pPixmap, w, &pt, &width, 1, pDst ) ;
		pDst += linelength ;
	    }

	    DEALLOCATE_LOCAL(pbits) ;
	    (* pGC->pScreen->DestroyPixmap)( pPixmap ) ;
	    FreeScratchGC( pGC ) ;
	    return ;
	}

	for ( i = 0 ; i < h ; i++ ) {
	    pt.x = sx ;
	    pt.y = sy + i ;
	    width = w ;
	    (* pDraw->pScreen->GetSpans)( pDraw, w, &pt, &width, 1, pDst ) ;
	    pDst += linelength ;
	}
    }
    else if ( ( pDraw->type == DRAWABLE_PIXMAP )
	  || ( GetBitsPerPixel( depth ) == 1 ) ) {
	miGetImage( pDraw, sx, sy, w, h, format, planeMask, pdstLine ) ;
    }
    else {
	sx += pDraw->x ;
	sy += pDraw->y ;
	(* ( (ppcScrnPriv *) pDraw->pScreen->devPrivate )->planeRead )( 
				ffs( planeMask ) - 1, sx, sy, w, h, pDst ) ;
    }
    return ;
}
