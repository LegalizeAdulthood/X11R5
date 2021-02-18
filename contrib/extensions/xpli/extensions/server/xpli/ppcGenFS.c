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

/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcGenFS.c,v 1.1 89/05/07 15:29:07 paul Exp $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcGenFS.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcGenFS.c,v 1.1 89/05/07 15:29:07 paul Exp $" ;
#endif

#include "X.h"
#include "misc.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "OScompiler.h"

#include "ppc.h"

#include "ibmTrace.h"
extern int mfbGCPrivateIndex;

/* Declared in ppcPixmapFS.o */
void ppcSolidWindowFS() ;
void ppcStippleWindowFS() ;
void ppcOpStippleWindowFS() ;
void ppcTileWindowFS() ;
 
/* Declared in ppcPixmapFS.o */
void ppcSolidPixmapFS() ;
void ppcStipplePixmapFS() ;
void ppcOpStipplePixmapFS() ;
void ppcTilePixmapFS() ;
 
void
ppcTileGenFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
DrawablePtr pDrawable ;
GC		*pGC ;
int		nInit ;		/* number of spans to fill */
DDXPointPtr pptInit ;		/* pointer to list of start points */
int *pwidthInit ;		/* pointer to list of n widths */
int fSorted ;
{
    TRACE( ( "ppcTileFS(pDrawable=0x%x,pGC=0x%x,nInit=%d,pptInit=0x%x,pwidthInit=0x%x,fSorted=%d)\n",
	    pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted ) ) ;

    (* ( ( pDrawable->type == DRAWABLE_PIXMAP )
	 ? ppcTilePixmapFS
	 : ppcTileWindowFS ) )( pDrawable, pGC, nInit, pptInit,
				pwidthInit, fSorted ) ;

    return ;
}

void
ppcOpStippleGenFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
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

void
ppcStippleGenFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
    DrawablePtr pDrawable ;
    GCPtr	pGC ;
    int		nInit ;			/* number of spans to fill */
    DDXPointPtr pptInit ;		/* pointer to list of start points */
    int		*pwidthInit ;		/* pointer to list of n widths */
    int 	fSorted ;
{
    TRACE( ( "ppcStippleFS(pDrawable=0x%x,pGC=0x%x,nInit=%d,pptInit=0x%x,pwidthInit=0x%x,fSorted=%d)\n",
	    pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted ) ) ;

    (* ( ( pDrawable->type == DRAWABLE_PIXMAP )
	 ? ppcStipplePixmapFS
	 : ppcStippleWindowFS ) )( pDrawable, pGC, nInit, pptInit,
				   pwidthInit, fSorted ) ;

    return ;
}

void
ppcSolidGenFS( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
    DrawablePtr pDrawable ;
    GCPtr	pGC ;
    int		nInit ;			/* number of spans to fill */
    DDXPointPtr pptInit ;		/* pointer to list of start points */
    int		*pwidthInit ;		/* pointer to list of n widths */
    int 	fSorted ;
{
    TRACE( ( "ppcSolidFS(pDrawable=0x%x,pGC=0x%x,nInit=%d,pptInit=0x%x,pwidthInit=0x%x,fSorted=%d)\n",
	   pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted ) ) ;

    (* ( ( pDrawable->type == DRAWABLE_PIXMAP )
	 ? ppcSolidPixmapFS
	 : ppcSolidWindowFS ) )( pDrawable, pGC, nInit, pptInit,
				 pwidthInit, fSorted ) ;

    return ;
}

void
ppcFillSpans( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted )
    DrawablePtr pDrawable ;
    GCPtr	pGC ;
    int		nInit ;			/* number of spans to fill */
    DDXPointPtr pptInit ;		/* pointer to list of start points */
    int		*pwidthInit ;		/* pointer to list of n widths */
    int 	fSorted ;
{
register void (*spanFunc)() ;

if ( pDrawable->type == DRAWABLE_WINDOW )
	switch ( ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.fillStyle ) {
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
		    spanFunc = ppcTileWindowFS ;
		    break ;
		default :
		    ErrorF( "ppcFillSpans: unknown Fill Style" ) ;
		    break ;
	}
else /* pDrawable->type == DRAWABLE_PIXMAP */
	switch ( ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.fillStyle ) {
		case FillSolid :
		    spanFunc = ppcSolidPixmapFS ;
		    break ;
		case FillStippled :
		    spanFunc = ppcStipplePixmapFS ;
		    break ;
		case FillOpaqueStippled :
		    spanFunc = ppcOpStipplePixmapFS ;
		    break ;
		case FillTiled :
		    spanFunc = ppcTileWindowFS ;
		    break ;
		default :
		    ErrorF( "ppcFillSpans: unknown Fill Style" ) ;
		    break ;
	}

(* spanFunc)( pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted ) ;

return ;
}
