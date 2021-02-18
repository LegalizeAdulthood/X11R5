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

/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcLine.c,v 30.2 89/05/07 15:26:53 paul Exp $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcLine.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcLine.c,v 30.2 89/05/07 15:26:53 paul Exp $" ;
#endif

#include "X.h"

#include "misc.h"
#include "gcstruct.h"
#include "gc.h"
#include "pixmap.h"
#include "window.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "scrnintstr.h"
#include "mistruct.h"

#include "mfb.h"
#include "ppc.h"

#include "OScompiler.h"
extern int mfbGCPrivateIndex;

/* ppc Line-
 *
 * This routine is basically the mfb line, except it calls functions it
 * finds in the ScreenPriv field defined as the ppcScreenPriv field.
 *
 * THE KEY IS YOU CANNOT USE THIS UNTIL YOU WRITE YOUR OWN ROUTINES FOR
 * LINE, HLINE, and VLINE.
 *
 * Until you do, most ppc users should use milines.  When you do, put
 * the function pointers into the ScreenPriv and then you can use ppcLine.
 * As long as your line routine is mi, you can NoopDDA the aforementioned
 * three line routines.
 *
 *
 * PLEASE READ COMMENT ON CODE AT TOP OF MFBLINE
 *
 * T. Paquin   9/87
 *
 */

/* NOTE
   maybe OUTCODES should take box (the one that includes all
edges) instead of pbox (the standard no-right-or-lower-edge one)?
*/
#define OUTCODES(result, x, y, pbox) \
    if (x < pbox->x1) \
	result |= OUT_LEFT ; \
    else if (x >= pbox->x2) \
	result |= OUT_RIGHT ; \
    if (y < pbox->y1) \
	result |= OUT_ABOVE ; \
    else if (y >= pbox->y2) \
	result |= OUT_BELOW ;

/* This Function is ONLY used when the linestyle in the GC is LineSolid.
 * This is assumed below !!
 */

void
ppcScrnZeroLine(pDrawable, pGC, mode, npt, pptInit)
    DrawablePtr pDrawable ;
    GCPtr pGC ;
    int mode ;		/* Origin or Previous */
    int npt ;		/* number of points */
    DDXPointPtr pptInit ;
{
    int nboxInit ;
    register int nbox ;
    BoxPtr pboxInit ;
    BoxRec box ;
    register BoxPtr pbox ;
    int nptTmp ;
    DDXPointPtr ppt ;		/* pointer to list of translated points */

    DDXPointRec pt1 ;
    DDXPointRec pt2 ;

    unsigned int oc1 ;		/* outcode of point 1 */
    unsigned int oc2 ;		/* outcode of point 2 */

    int xorg, yorg ;		/* origin of window */

    int adx ;		/* abs values of dx and dy */
    int ady ;
    int signdx ;		/* sign of dx and dy */
    int signdy ;
    int e, e1, e2 ;		/* bresenham error and increments */
    int len ;			/* length of segment */

    int clipDone ;		/* flag for clipping loop */
    DDXPointRec pt1Orig ;	/* unclipped start point */
    DDXPointRec pt2Orig ;	/* unclipped end point */
    int err ;			/* modified bresenham error term */
    int clip1, clip2 ;		/* clippedness of the endpoints */

    int alu ;			/* Screen Raster-Op value */
    long fg ;			/* foreground Pixel value */
    long pm ;			/* plane mask */
    register int tmp ;
    int x1, x2, y1, y2 ;		/* a bunch of temporaries */
    ppcPrivGC *gcPriv ;
    ppcScrnPriv *scrnPriv ;
    void (* scrnFunc)() ;

    if ( pDrawable->type != DRAWABLE_WINDOW ) {
	ErrorF("ppcScrnZeroLine: not a window\n") ;
	return ;
    }

    gcPriv = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr ) ;
    switch ( gcPriv->colorRrop.fillStyle ) {
	case FillStippled :
	case FillSolid : /* Can Use Optimized Color Raster-Op */
	    alu = gcPriv->colorRrop.alu ;
	    pm = gcPriv->colorRrop.planemask ;
	    fg = gcPriv->colorRrop.fgPixel ;
	    break ;
	case FillOpaqueStippled :
	case FillTiled : /* Can't Use Optimized Color Raster-Op */
	    alu = pGC->alu ;
	    pm = pGC->planemask ;
	    fg = pGC->fgPixel ;
	    break ;
    }
    if ( alu == GXnoop )
	return ;

    pboxInit = REGION_RECTS(gcPriv->pCompositeClip) ;
    nboxInit = REGION_NUM_RECTS(gcPriv->pCompositeClip) ;

    xorg = pDrawable->x ;
    yorg = pDrawable->y ;

    /* translate the point list */
    ppt = pptInit ;
    nptTmp = npt ;
    if (mode == CoordModeOrigin) {
	while ( nptTmp-- ) {
	    ppt->x += xorg ;
	    ppt->y += yorg ;
	    ppt++ ;
	}
    }
    else {
	ppt->x += xorg ;
	ppt->y += yorg ;
	nptTmp-- ;
	while ( nptTmp-- ) {
	    ppt++ ;
	    ppt->x += (ppt-1)->x ;
	    ppt->y += (ppt-1)->y ;
	}
    }

    scrnPriv = (ppcScrnPriv *) ( pDrawable->pScreen->devPrivate ) ;
    ppt = pptInit ;
    while ( --npt ) {
	nbox = nboxInit ;
	pbox = pboxInit ;

	pt1 = *ppt++ ;
	pt2 = *ppt ;

	if ( !( adx = ( pt2.x - pt1.x ) ) ) {
	    /* make the line go top to bottom of screen, keeping
	       endpoint semantics
	    */
	    if ( pt1.y > pt2.y ) {
		tmp = pt2.y ;
		pt2.y = pt1.y + 1 ;
		pt1.y = tmp + 1 ;
	    }

	    /* get to first band that might contain part of line */
	    while ((nbox) && (pbox->y2 <= pt1.y)) {
		pbox++ ;
		nbox-- ;
	    }

	    if ( nbox ) {
		scrnFunc = scrnPriv->lineVert ;
		/* stop when lower edge of box is beyond end of line */
		while ( ( nbox ) && ( pt2.y >= pbox->y1 ) ) {
		    if ( ( pt1.x >= pbox->x1 ) && ( pt1.x < pbox->x2 ) ) {
			/* this box has part of the line in it */
			if ( ( y1 = MAX( pt1.y, pbox->y1 ) )
			  != ( y2 = MIN( pt2.y, pbox->y2 ) ) ) {
			    (* scrnFunc)( fg, alu, pm, pt1.x, y1, y2 - y1 ) ;
			}
		    }
		    nbox-- ;
		    pbox++ ;
		}
	    }

	}
	else if ( !( ady = ( pt2.y - pt1.y ) ) ) {
	    /* force line from left to right, keeping
	       endpoint semantics
	    */
	    if (pt1.x > pt2.x) {
		tmp = pt2.x ;
		pt2.x = pt1.x + 1 ;
		pt1.x = tmp + 1 ;
	    }

	    /* find the correct band */
	    while ( nbox && (pbox->y2 <= pt1.y)) {
		pbox++ ;
		nbox-- ;
	    }

	    /* try to draw the line, if we haven't gone beyond it */
	    if ( nbox && ( pbox->y1 <= pt1.y ) ) {
		/* when we leave this band, we're done */
		tmp = pbox->y1 ;
		scrnFunc = scrnPriv->lineHorz ;
		while ( nbox && ( pbox->y1 == tmp ) ) {
		    if ( pbox->x2 <= pt1.x ) {
			/* skip boxes until one might contain start point */
			nbox-- ;
			pbox++ ;
			continue ;
		    }

		    /* stop if left of box is beyond right of line */
		    if (pbox->x1 >= pt2.x)
		    {
			nbox = 0 ;
			continue ;
		    }
		    if ( ( x1 = MAX( pt1.x, pbox->x1 ) )
		      != ( x2 = MIN( pt2.x, pbox->x2 ) ) ) {
			    (* scrnFunc)( fg, alu, pm, x1, pt1.y, x2 - x1 ) ;
		    }
		    nbox-- ;
		    pbox++ ;
		}
	    }
	}
	else	/* sloped line */
	{
	    signdx = sign(adx) ;
	    signdy = sign(ady) ;
	    adx = ABS(adx) ;
	    ady = ABS(ady) ;

	    if ( adx > ady ) {
		e1 = ady << 1 ;
		e = e1 - adx ;
		e2 = e - adx ;

	    }
	    else {
		e1 = adx << 1 ;
		e = e1 - ady ;
		e2 = e - ady ;
	    }

	    /* we have bresenham parameters and two points.
	       all we have to do now is clip and draw.
	    */

	    pt1Orig = pt1 ;
	    pt2Orig = pt2 ;

	    while ( nbox-- )
	    {

		pt1 = pt1Orig ;
		pt2 = pt2Orig ;
		clipDone = 0 ;
		box.x1 = pbox->x1 ;
		box.y1 = pbox->y1 ;
		box.x2 = pbox->x2-1 ;
		box.y2 = pbox->y2-1 ;
		clip1 = 0 ;
		clip2 = 0 ;

		oc1 = 0 ;
		oc2 = 0 ;
		OUTCODES(oc1, pt1.x, pt1.y, pbox) ;
		OUTCODES(oc2, pt2.x, pt2.y, pbox) ;

		if (oc1 & oc2)
		    clipDone = -1 ;
		else if ((oc1 | oc2) == 0)
		    clipDone = 1 ;
		else /* have to clip */
		    clipDone = mfbClipLine(pbox, box,
					   &pt1Orig, &pt1, &pt2,
					   adx, ady, signdx, signdy,
					   ( ( adx > ady ) ? X_AXIS : Y_AXIS ),
					   &clip1, &clip2) ;

		if (clipDone == -1)
		    pbox++ ;
		else {
			scrnFunc = scrnPriv->lineBres ;

		    if ( adx > ady )
			len = ABS(pt2.x - pt1.x) ;
		    else
			len = ABS(pt2.y - pt1.y) ;

		    len += (clip2 != 0) ;
		    if (len) {
			/* unwind bresenham error term to first point */
			if (clip1)
			{
			    if ( adx > ady )
				err = e
				    + ( ABS( pt1.y - pt1Orig.y ) * ( e2 - e1 ) )
				    + ( ABS( pt1.x - pt1Orig.x ) * e1 ) ;
			    else
				err = e
				    + ( ABS( pt1.x - pt1Orig.x ) * ( e2 - e1 ) )
				    + ( ABS( pt1.y - pt1Orig.y ) * e1 ) ;
			}
			else
			    err = e ;
			(* scrnFunc)( fg, alu, pm,
				      signdx, signdy,
				      ( ( adx > ady ) ? X_AXIS : Y_AXIS ),
				      pt1.x, pt1.y, err, e1, e2, len ) ;
		    }
		    /* if segment is unclipped, skip remaining rectangles */
		    if (!(clip1 || clip2))
			break ;
		    else
			pbox++ ;
		}
	    } /* while (nbox--) */
	} /* sloped line */
    } /* while (nline--) */

    /* paint the last point if the end style isn't CapNotLast.
       (Assume that a projecting, butt, or round cap that is one
	pixel wide is the same as the single pixel of the endpoint.)
    */

    if ( ( pGC->capStyle != CapNotLast )
      && ( ( ppt->x != pptInit->x ) || ( ppt->y != pptInit->y )
		|| ( ppt == pptInit + 1 ) )
      && ( (* pDrawable->pScreen->PointInRegion)(
		gcPriv->pCompositeClip,
		ppt->x, ppt->y, &box ) ) )
	(* scrnPriv->solidFill)( fg, alu, pm, ppt->x, ppt->y, 1, 1 ) ;

    return ;
}

void
ppcScrnZeroDash(pDrawable, pGC, mode, npt, pptInit)
    DrawablePtr pDrawable ;
    GCPtr pGC ;
    int mode ;		/* Origin or Previous */
    int npt ;		/* number of points */
    DDXPointPtr pptInit ;
{
    int nseg ;			/* number of dashed segments */
    miDashPtr pdash ;		/* list of dashes */
    miDashPtr pdashInit ;
    int alu ;
    unsigned long int color ;
    unsigned long int fg, bg, pm ;

    int nboxInit ;
    int nbox ;
    BoxPtr pboxInit ;
    BoxPtr pbox ;
    int nptTmp ;
    DDXPointPtr ppt ;		/* pointer to list of translated points */

    DDXPointRec pt1 ;
    DDXPointRec pt2 ;

    unsigned int oc1 ;		/* outcode of point 1 */
    unsigned int oc2 ;		/* outcode of point 2 */

    int xorg, yorg ;		/* origin of window */

				/* these are all per original line */
    int adx ;			/* abs values of dx and dy */
    int ady ;
    int signdx ;			/* sign of dx and dy */
    int signdy ;
    int e ;			/* error term for first point of
				   original line */
    int e1, e2 ;			/* unwinding terms */
				/* these are all per dash */
    int err ;			/* bres error term for first drawn point */
    int len ;			/* length of segment */

    int clipDone ;		/* flag for clipping loop */
    DDXPointRec pt1Orig ;	/* unclipped start point */
    int clip1, clip2 ;		/* clippedness of the endpoints */

    void (* scrnFunc)() ;
    register int tmp1 ;
    register int tmp2 ;

    if (pDrawable->type != DRAWABLE_WINDOW) {
	ErrorF( "ppcScrnZeroDash: not a window\n" ) ;
	return ;
    }

    if ( !( nboxInit = REGION_NUM_RECTS(((ppcPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip)))
	return ; /* No Work To Do !! */

    pboxInit = REGION_RECTS(((ppcPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip) ;

    /* Get The Function, Planemask and Colors */

    if ( pGC->lineStyle == LineOnOffDash ) {
	switch ( ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.fillStyle ) {
	    case FillStippled :
	    case FillSolid : /* Can Use Optimized Color Raster-Op */
		alu = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.alu ;
		pm = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.planemask ;
		color = fg = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.fgPixel ;
		break ;
	    case FillOpaqueStippled :
	    case FillTiled : /* Can't Use Optimized Color Raster-Op */
		alu = pGC->alu ;
		pm = pGC->planemask ;
		color = fg = pGC->fgPixel ;
		break ;
	}
    }
    else {
	switch ( ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.fillStyle ) {
	    case FillStippled :
	    case FillSolid :
	    case FillTiled : /* Can't Use Optimized Color Raster-Op */
		alu = pGC->alu ;
		pm = pGC->planemask ;
		fg = pGC->fgPixel ;
		bg = pGC->bgPixel ;
		break ;
	    case FillOpaqueStippled : /* Can Use Optimized Color Raster-Op */
		alu = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.alu ;
		pm = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.planemask ;
		fg = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.fgPixel ;
		bg = ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.bgPixel ;
		break ;
	}
    }
    if ( alu == GXnoop )
	return ;

    xorg = pDrawable->x ;
    yorg = pDrawable->y ;

    /* translate the point list */
    ppt = pptInit ;
    nptTmp = npt ;
    if (mode == CoordModeOrigin) {
	while ( nptTmp-- ) {
	    ppt->x += xorg ;
	    ppt->y += yorg ;
	    ppt++ ;
	}
    }
    else {
	ppt->x += xorg ;
	ppt->y += yorg ;
	while ( --nptTmp ) {
	    ppt++ ;
	    ppt->x += (ppt-1)->x ;
	    ppt->y += (ppt-1)->y ;
	}
    }

    pdashInit = pdash = miDashLine( npt, pptInit, pGC->numInDashList,
				    pGC->dash, pGC->dashOffset, &nseg ) ;

    for ( ; nseg-- ; pdash++ ) {
	if (pGC->lineStyle == LineOnOffDash) {
	    for ( ; nseg && (pdash->which == ODD_DASH) ; nseg--, pdash++ ) {
		if (pdash->newLine) {
		    pt1Orig = pt1 = *pptInit++ ;
		    pt2 = *pptInit ;
		    if ( ( adx = pt2.x - pt1.x )
		      && ( ady = pt2.y - pt1.y ) ) {
			signdx = sign(adx) ;
			signdy = sign(ady) ;
			adx = ABS(adx) ;
			ady = ABS(ady) ;
			e = pdash->e ;
			e1 = pdash->e1 ;
			e2 = pdash->e2 ;
		    }
		}
	    }
	    /* ??? is this right ??? */
	    if (!nseg)
		break ;
	}
	else if (pGC->lineStyle == LineDoubleDash) {
	    /* use a different color for odd dashes */
	    color = (pdash->which == EVEN_DASH) ? fg : bg ;
	}

	if (pdash->newLine) {
	    pt1Orig = pt1 = *pptInit++ ;
	    pt2 = *pptInit ;
	    if ( ( adx = pt2.x - pt1.x )
	      && ( ady = pt2.y - pt1.y ) ) {
		signdx = sign(adx) ;
		signdy = sign(ady) ;
		adx = ABS(adx) ;
		ady = ABS(ady) ;
		e = pdash->e ;
		e1 = pdash->e1 ;
		e2 = pdash->e2 ;
	    }
	}

	nbox = nboxInit ;
	pbox = pboxInit ;

/* ************************************************************************** */
	if ( !adx ) {
	    pt1 = pdash->pt ;
	    pt2 = (pdash+1)->pt ;
	    /* make the line go top to bottom of screen, keeping
	       endpoint semantics
	    */
	    if ( pt1.y > pt2.y ) {
		tmp1 = pt2.y + 1 ;
		tmp2 = pt1.y + 1 ;
	    }
	    else {
		tmp1 = pt1.y ;
		tmp2 = pt2.y ;
	    }

	    /* get to first band that might contain part of line */
	    for ( ; nbox && ( pbox->y2 <= tmp1 ) ; pbox++, nbox-- ) ;

	    if ( nbox ) {
		scrnFunc =
		 ( (ppcScrnPriv *)(pDrawable->pScreen->devPrivate))->lineVert ;
		/* stop when lower edge of box is beyond end of line */
		for ( ; nbox && ( pbox->y1 < tmp2 ) ; nbox--, pbox++ ) {
		    if ( ( pt1.x >= pbox->x1 ) && ( pt1.x < pbox->x2 ) ) {
			register int y1, y2 ;
			/* this box has part of the line in it */
			if ( ( y1 = MAX( tmp1, pbox->y1 ) )
			  != ( y2 = MIN( tmp2, pbox->y2 ) ) ) {
			    (* scrnFunc)( color, alu, pm, pt1.x, y1, y2 - y1 ) ;
			}
		    }
		}
	    }
	}
	else if ( !ady ) {
	    pt1 = pdash->pt ;
	    pt2 = (pdash+1)->pt ;
	    /* force line from left to right, keeping
	       endpoint semantics
	    */
	    if (pt1.x > pt2.x) {
		tmp1 = pt2.x + 1 ;
		tmp2 = pt1.x + 1 ;
	    }
	    else {
		tmp1 = pt1.x ;
		tmp2 = pt2.x ;
	    }

	    /* find the correct band */
	    for ( ; nbox && (pbox->y2 <= pt1.y ) ; pbox++, nbox-- ) ;

	    /* try to draw the line, if we haven't gone beyond it */
	    if ( nbox && ( pbox->y1 <= pt1.y ) ) {
		/* when we leave this band, we're done */
		int tmp3 = pbox->y1 ;
		scrnFunc =
		 ( (ppcScrnPriv *)(pDrawable->pScreen->devPrivate))->lineHorz ;
		for ( ; nbox && ( pbox->y1 == tmp3 ) ; nbox--, pbox++ )
		    if ( pbox->x2 <= tmp1 )
			/* skip boxes until one might contain start point */
			continue ;
		    /* stop if left of box is beyond right of line */
		    else if (pbox->x1 >= tmp2 )
			break ;
		    else {
			register int x1, x2 ;
			if ( ( x1 = MAX( tmp1, pbox->x1 ) )
			  != ( x2 = MIN( tmp2, pbox->x2 ) ) )
			    (* scrnFunc)( color, alu, pm, x1, pt1.y, x2 - x1 ) ;
		    }
	    }
	}
	else {	/* sloped line */
	    /* we have bresenham parameters and two points.
	       all we have to do now is clip and draw.
	    */
	    scrnFunc =
		( (ppcScrnPriv *)(pDrawable->pScreen->devPrivate))->lineBres ;

	    for ( ; nbox-- ; pbox++ ) {
		BoxRec box ;

		clipDone = 0 ;
		box.x1 = pbox->x1 ;
		box.y1 = pbox->y1 ;
		box.x2 = pbox->x2-1 ;
		box.y2 = pbox->y2-1 ;
		clip1 = 0 ;
		clip2 = 0 ;
		pt1 = pdash->pt ;
		pt2 = (pdash+1)->pt ;

		oc1 = 0 ;
		oc2 = 0 ;
		OUTCODES(oc1, pt1.x, pt1.y, pbox) ;
		OUTCODES(oc2, pt2.x, pt2.y, pbox) ;

		if (oc1 & oc2)
		    clipDone = -1 ;
		else if ((oc1 | oc2) == 0)
		    clipDone = 1 ;
		else /* have to clip */
		    clipDone = mfbClipLine(pbox, box,
					   &pt1Orig, &pt1, &pt2,
					   adx, ady, signdx, signdy,
					   ( ( adx > ady ) ? X_AXIS : Y_AXIS ),
					   &clip1, &clip2) ;

		if ( clipDone != -1 ) {
		    if ( adx > ady )
			len = ABS(pt2.x - pt1.x) ;
		    else
			len = ABS(pt2.y - pt1.y) ;

		    len += (clip2 != 0) ;
		    if (len) {
			if (clip1) {
			    /* unwind bres error term to first visible point */
			    if ( adx > ady )
				err = e
				    + ( ABS( pt1.y - pt1Orig.y ) * ( e2 - e1 ) )
				    + ( ABS( pt1.x - pt1Orig.x ) * e1 ) ;
			    else
				err = e
				    + ( ABS( pt1.x - pt1Orig.x ) * ( e2 - e1 ) )
				    + ( ABS( pt1.y - pt1Orig.y ) * e1 ) ;
			}
			else {
			    /* use error term calculated with the dash */
			    err = pdash->e ;
			}
			(* scrnFunc)( color, alu, pm, signdx, signdy,
				      ( ( adx > ady ) ? X_AXIS : Y_AXIS ),
				      pt1.x, pt1.y, err, e1, e2, len ) ;
		    }
		    /* if segment is unclipped, skip remaining rectangles */
		    if (!(clip1 || clip2))
			    break ;
		}
	    } /* while (nbox--) */
	} /* sloped line */
/* ************************************************************************** */
    } /* for ( ; --nseg ; pdash++ ) */

    Xfree( pdashInit ) ;
    return ;
}

/* This Function is ONLY used when the linestyle in the GC is LineSolid.
 * This is assumed below !!
 */

void
ppcScrnZeroSegs( pDrawable, pGC, nseg, psgInit )
DrawablePtr	pDrawable ;
GCPtr		pGC ;
int		nseg ;
xSegment	*psgInit ;
{
register int	nbox ;
register BoxPtr pbox ;
register int	tmp ;

BoxPtr		pboxInit ;
BoxRec		box ;
DDXPointRec	pt1, pt2,
		pt1Orig, pt2Orig ;	/* unclipped points */
xSegment	*psg ;

unsigned int	oc1, oc2 ;		/* outcodes */

int		xorg, yorg,		/* origin of window */
		adx, ady,		 /* abs values of dx and dy */
		signdx, signdy,		/* sign of dx and dy */
		e, e1, e2,		/* bresenham error and increments */
		len,			/* length of segment */
		clipDone,		/* flag for clipping loop */
		err,			/* modified bresenham error term */
		clip1, clip2,		/* clippedness of the endpoints */
		alu ,			/* Screen Raster-Op value */
		x1, x2, y1, y2,		/* a bunch of temporaries */
		nboxInit, nsgTmp ;

unsigned long int fg ;			 /* foreground Pixel value */
unsigned long int pm ;			/* plane mask */
ppcPrivGC	 *gcPriv ;
ppcScrnPriv	 *scrnPriv ;
void		 (* scrnFunc)() ;	/* function that will do the draw */

if ( pDrawable->type != DRAWABLE_WINDOW ) {
	ErrorF( "ppcScrnZeroSegs: not a window\n" ) ;
	return ;
}

gcPriv = (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr ;
switch ( gcPriv->colorRrop.fillStyle ) {
	case FillStippled :
	case FillSolid : /* Can Use Optimized Color Raster-Op */
		alu = gcPriv->colorRrop.alu ;
		pm = gcPriv->colorRrop.planemask ;
		fg = gcPriv->colorRrop.fgPixel ;
		break ;
	case FillOpaqueStippled :
	case FillTiled : /* Can't Use Optimized Color Raster-Op */
		alu = pGC->alu ;
		pm = pGC->planemask ;
		fg = pGC->fgPixel ;
		break ;
}
if ( alu == GXnoop )
	return ;

pboxInit = REGION_RECTS(gcPriv->pCompositeClip);
nboxInit = REGION_NUM_RECTS(gcPriv->pCompositeClip) ;

xorg = pDrawable->x ;
yorg = pDrawable->y ;

/* translate the point list */
for ( psg = psgInit, nsgTmp = nseg ; nsgTmp-- ; psg++) {
	psg->x1 += xorg ;
	psg->y1 += yorg ;
	psg->x2 += xorg ;
	psg->y2 += yorg ;
}

scrnPriv = (ppcScrnPriv *) ( pDrawable->pScreen->devPrivate ) ;
psg = psgInit ;
while ( nseg-- ) {
	nbox = nboxInit ;
	pbox = pboxInit ;

	pt1.x = psg->x1 ;
	pt1.y = psg->y1 ;
	pt2.x = psg->x2 ;
	pt2.y = psg->y2 ;
	psg++ ;

	if ( !( adx = ( pt2.x - pt1.x ) ) ) {
		/* WE'RE GOING TO DO A VERTICAL LINE */
		/* make the line go top to bottom of screen, keeping
		   endpoint semantics
		*/
		if ( pt1.y > pt2.y ) {
			tmp = pt2.y ;
			pt2.y = pt1.y + 1 ;
			pt1.y = tmp + 1 ;
		}
		/* get to first band that might contain part of line */
		while ((nbox) && (pbox->y2 <= pt1.y)) {
			pbox++ ;
			nbox-- ;
		}
		if ( nbox ) {
			scrnFunc = scrnPriv->lineVert ;
			/* stop when lower edge of box is beyond end of line */
			while ( ( nbox ) && ( pt2.y >= pbox->y1 ) ) {
				if ( ( pt1.x >= pbox->x1 )
				  && ( pt1.x < pbox->x2 ) )
					/* box has part of the line in it */
					if ( ( y1 = MAX( pt1.y, pbox->y1 ) )
					  != ( y2 = MIN( pt2.y, pbox->y2 ) ) )
						(* scrnFunc)( fg, alu, pm,
							      pt1.x, y1,
							      y2 - y1 ) ;
				nbox-- ;
				pbox++ ;
			}
		}
	}/* END OF VERTICAL LINE SECTION */
	else if ( !( ady = ( pt2.y - pt1.y ) ) ) {
		/* WE'RE GOING TO DO A HORIZONTAL LINE */
		/* force line from left to right, keeping
		   endpoint semantics
		*/
		if (pt1.x > pt2.x) {
			tmp = pt2.x ;
			pt2.x = pt1.x + 1 ;
			pt1.x = tmp + 1 ;
		}

		/* find the correct band */
		while ( nbox && ( pbox->y2 <= pt1.y ) ) {
			pbox++ ;
			nbox-- ;
		}
		/* try to draw the line, if we haven't gone beyond it */
		if ( nbox && ( pbox->y1 <= pt1.y ) ) {
			/* when we leave this band, we're done */
			tmp = pbox->y1 ;
			scrnFunc = scrnPriv->lineHorz ;
			while ( nbox && ( pbox->y1 == tmp ) ) {
				/* skip boxes until one may hold start point */
				if ( pbox->x2 <= pt1.x ) {
					nbox-- ;
					pbox++ ;
					continue ;
				}
				/* stop if left of box is past right of line */
				if ( pbox->x1 >= pt2.x ) {
					nbox = 0 ;
					continue ;
				}
				if ( ( x1 = MAX( pt1.x, pbox->x1 ) )
				  != ( x2 = MIN( pt2.x, pbox->x2 ) ) ) {
					(* scrnFunc)( fg, alu, pm, x1,
						      pt1.y, x2 - x1 ) ;
				}
				nbox-- ;
				pbox++ ;
			}
		}
	}/* END OF HORIZONTAL LINE SECTION */
	else {	/* sloped line -- BRESENHAM TIME!! */
		signdx = sign(adx) ;
		adx = ABS(adx) ;
		signdy = sign(ady) ;
		ady = ABS(ady) ;

		if (adx > ady) {
			e1 = ady << 1 ;
			e = e1 - adx ;
			e2 = e - adx ;
		}
		else {
			e1 = adx << 1 ;
			e = e1 - ady ;
			e2 = e - ady ;
		}
		/* we have bresenham parameters and two points.
		 * all we have to do now is clip and draw.
		 */
		pt1Orig = pt1 ;
		pt2Orig = pt2 ;
		scrnFunc = scrnPriv->lineBres ;
		while ( nbox-- ) {
			pt1 = pt1Orig ;
			pt2 = pt2Orig ;
			clipDone = 0 ;
			box.x1 = pbox->x1 ;
			box.y1 = pbox->y1 ;
			box.x2 = pbox->x2-1 ;
			box.y2 = pbox->y2-1 ;
			clip1 = 0 ;
			clip2 = 0 ;

			oc1 = 0 ;
			oc2 = 0 ;
			OUTCODES(oc1, pt1.x, pt1.y, pbox) ;
			OUTCODES(oc2, pt2.x, pt2.y, pbox) ;

			if ( oc1 & oc2 )
				clipDone = -1 ;
			else if ( !( oc1 | oc2 ) )
				clipDone = 1 ;
			else /* have to clip */
				clipDone = mfbClipLine( pbox, box,
							&pt1Orig, &pt1, &pt2,
							adx, ady,
						        signdx, signdy,
					( ( adx > ady ) ? X_AXIS : Y_AXIS ),
							&clip1, &clip2 ) ;
			if (clipDone == -1)
				pbox++ ;
			else {
				if ( adx > ady )
					len = ABS(pt2.x - pt1.x) ;
				else
					len = ABS(pt2.y - pt1.y) ;

				len += (clip2 != 0) ;
				if (len) {
				/* unwind bresenham error term to first point */
					if (clip1) {
						if ( adx > ady )
						    err = e
						+ ( ABS( pt1.y - pt1Orig.y )
						 * ( e2 - e1 ) )
						+ ( ABS( pt1.x - pt1Orig.x )
						 * e1 ) ;
						else
						    err = e
						+ ( ABS( pt1.x - pt1Orig.x )
						 * ( e2 - e1 ) )
						+ ( ABS( pt1.y - pt1Orig.y )
						 * e1 ) ;
					}
					else
					    err = e ;
					(* scrnFunc)( fg, alu, pm,
						      signdx, signdy,
					  ( ( adx > ady ) ? X_AXIS : Y_AXIS ),
						      pt1.x, pt1.y,
						      err, e1, e2, len ) ;
				}
				/* if segment is unclipped,
				   skip remaining rectangles */
				if ( !( clip1 || clip2 ) )
					break ;
				else
					pbox++ ;
			}
		} /* while (nbox--) */
	} /* sloped line */
} /* while (nline--) */

return ;
}
