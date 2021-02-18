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

/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcGetSp.c,v 9.2 89/05/07 15:27:55 paul Exp $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcGetSp.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcGetSp.c,v 9.2 89/05/07 15:27:55 paul Exp $" ;
#endif

#include "X.h"
#include "servermd.h"
#include "scrnintstr.h"
#include "region.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"

#include "OScompiler.h"

#include "ppcGCstr.h"
#include "ppcScrn.h"

#include "ibmTrace.h"

/* GetSpans -- for each span, gets bits from drawable starting at ppt[i]
 * and continuing for pwidth[i] bits
 * Each scanline returned will be server scanline padded, i.e., it will come
 * out to an integral number of words.
 */
void
ppcGetSpans( pDrawable, wMax, ppt, pwidth, nspans, pdstStart )
    DrawablePtr		pDrawable ;	/* drawable from which to get bits */
    int			wMax ;		/* largest value of all *pwidths */
    register DDXPointPtr ppt ;		/* points to start copying from */
    int			*pwidth ;	/* list of number of bits to copy */
    register int	nspans ;	/* number of scanlines to copy */
    unsigned int 	*pdstStart ;
{
    register int		j ;
    register unsigned char	*pdst ;	/* where to put the bits */
    register unsigned char	*psrc ;	/* where to get the bits */
    register int		pixmapStride ;


    TRACE( ( "ppcGetSpans(pDrawable=0x%x,wMax=%d,ppt=0x%x,pwidth=0x%x,nspans=%d)\n", 
	pDrawable, wMax, ppt, pwidth, nspans ) ) ;

    if ( ( pDrawable->depth == 1 ) && ( pDrawable->type == DRAWABLE_PIXMAP ) )
	{
	mfbGetSpans( pDrawable, wMax, ppt, pwidth, nspans, pdstStart ) ;
	return;
	}

    pixmapStride = PixmapBytePad( wMax, pDrawable->depth ) ;
    pdst = pdstStart ;

    if ( pDrawable->type == DRAWABLE_WINDOW ) {
     	register void (* fnp)()
	   = ((ppcScrnPriv *)(pDrawable->pScreen->devPrivate))->imageRead ;
	for ( ; nspans-- ; ppt++, pwidth++ ) {
		(* fnp)( ppt->x, ppt->y, j = *pwidth, 1, pdst, pixmapStride ) ;
		pdst += j ; /* width is in 32 bit words */
		j = ( -j ) & 3 ;
		while ( j-- ) /* Pad out to 32-bit boundary */
		    *pdst++ = 0 ;
	}
    }
    else {  /* OK, if we are here, we had better be a DRAWABLE PIXMAP */
	register int widthSrc =  /* width of pixmap in bytes */
	 (int) ( (PixmapPtr) pDrawable )->devKind ;

	psrc = (unsigned char *) ( (PixmapPtr) pDrawable )->devPrivate.ptr ;
	for ( ; nspans-- ; ppt++, pwidth++ ) {
	    MOVE( psrc + ( ppt->y * widthSrc ) + ppt->x,
		  pdst, j = *pwidth ) ;
	    pdst += j ;
	    j = ( -j ) & 3 ;
	    while ( j-- ) /* Pad out to 32-bit boundary */
	    	*pdst++ = 0 ;
	}
    }
    return ;
}
