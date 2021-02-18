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

/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcWLine.c,v 9.2 89/05/07 15:33:16 jeff Exp $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcWLine.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcWLine.c,v 9.2 89/05/07 15:33:16 jeff Exp $" ;
#endif

#include "X.h"
#include "misc.h"
#include "windowstr.h"
#include "Xprotostr.h"
#include "gcstruct.h"
#include "scrnintstr.h"
#include "miscstruct.h"
#include "pixmap.h"
#include "pixmapstr.h"
#include "mifpoly.h"
#include "mi.h"

#include "OScompiler.h"

static DDXPointPtr ppcRoundCap() ;

#define IS_VERT(pp) (pp[0].x == pp[1].x)
#define IS_HORZ(pp) (pp[0].y == pp[1].y)

void
ppcWideLine(pDrawable, pGC, mode, npt, pPts)
    DrawablePtr pDrawable ;
    GCPtr	pGC ;
    int		mode ;
    int		npt ;
    DDXPointRec pPts[] ;
{
    miWideLine(pDrawable, pGC, mode, npt, nPts);
}

#ifdef notdef
/* PPCWIDELINE - Public entry for PolyLine call
 * handles 1 segment wide lines specially.  Calls miWideLine
 * for all other cases.  Code taken from miOneSegment.
 */
void
ppcWideLine(pDrawable, pGC, mode, npt, pPts)
    DrawablePtr pDrawable ;
    GCPtr	pGC ;
    int		mode ;
    int		npt ;
    DDXPointRec pPts[] ;
{
    int		width = (pGC->lineWidth ? pGC->lineWidth : 1) ;
    DDXPointRec	tmpPts[4] ;
    DDXPointRec	otmpPts[2] ;
    SppPointRec SppPts[2] ;
    SppPointRec PolyPoints[4] ;
    xRectangle	tmpRect ;

    if ( pDrawable->type != DRAWABLE_WINDOW || npt > 2 ) {
	miWideLine( pDrawable, pGC, mode, npt, pPts ) ;
	return ;
    }

    if ( npt == 1 )
	tmpPts[0] = tmpPts[1] = pPts[0] ;
    if ( npt == 2 ) {
	tmpPts[0] = pPts[0] ;
	tmpPts[1] = pPts[1] ;
    }

    /* make everything absolute */
    if ( mode == CoordModePrevious ) {
	tmpPts[1].x += tmpPts[0].x ;
    	tmpPts[1].y += tmpPts[0].y ;
    }
    otmpPts[0] = tmpPts[0] ;
    otmpPts[1] = tmpPts[1] ;

    if ( tmpPts[0].x > tmpPts[1].x ) {
	DDXPointRec tp ;

	tp = tmpPts[0] ;
	tmpPts[0] = tmpPts[1] ;
	tmpPts[1] = tp ;
    }

    if ( pGC->capStyle == CapProjecting ) {
	if ( PtEqual(pPts[0], pPts[1] ) ) {
	    tmpPts[0].y -= width / 2.0 ;
	    tmpPts[1].y += width / 2.0 ;
	}
	else {
	    if ( IS_VERT( tmpPts ) ) {
		tmpPts[0].y -= width / 2 ;
		tmpPts[1].y += width / 2 ;
	    }
	    else {
	    	if ( IS_HORZ( tmpPts ) ) {
			tmpPts[0].x -= width / 2 ;
			tmpPts[1].x += width / 2 ;
	    	}
	    	else {
			SppPts[0].x = (double)tmpPts[0].x ;
			SppPts[0].y = (double)tmpPts[0].y ;
			SppPts[1].x = (double)tmpPts[1].x ;
			SppPts[1].y = (double)tmpPts[1].y ;
			if ( PtEqual( SppPts[0], SppPts[1] ) ) {
	    			SppPts[0].y -= width / 2.0 ;
	    			SppPts[1].y += width / 2.0 ;
			}
	    		SppPts[0] = 
				miExtendSegment( SppPts[0], SppPts[1],
						 width / 2 ) ;
	    		SppPts[1] =
				miExtendSegment( SppPts[1], SppPts[0],
						 width / 2 ) ;
		}
	    }
	}
    }

    /* get points for rect */

    if ( IS_VERT( tmpPts ) ) {
	tmpRect.x = tmpPts[0].x - width / 2 ;
	tmpRect.y = MIN(tmpPts[0].y, tmpPts[1].y) ;
	tmpRect.width = width ;
	tmpRect.height = ABS(tmpPts[1].y - tmpPts[0].y) ;
    }
    else {
    	if ( IS_HORZ( tmpPts ) ) {
		tmpRect.y = tmpPts[0].y - width / 2 ;
		tmpRect.x = tmpPts[0].x ;
		tmpRect.height = width ;
		tmpRect.width = tmpPts[1].x - tmpPts[0].x ;
	}
	else {
		if ( pGC->capStyle != CapProjecting ) {
			SppPts[0].x = (double)tmpPts[0].x ;
			SppPts[0].y = (double)tmpPts[0].y ;
			SppPts[1].x = (double)tmpPts[1].x ;
			SppPts[1].y = (double)tmpPts[1].y ;
		}
    		miGetPts( SppPts[0], SppPts[1], &PolyPoints[0], 
	  		  &PolyPoints[1], &PolyPoints[2], &PolyPoints[3],
			  width ) ;

    		if ( pGC->capStyle == CapRound ) {
			register n ;
			int n1, n2 ;
			DDXPointPtr pt ;
			DDXPointPtr pt1 ;
			DDXPointPtr pt2 ;
			DDXPointPtr ptOrig ;
			DDXPointPtr pt1Orig ;
			DDXPointPtr pt2Orig ;

			pt1Orig = pt1 = ppcRoundCap(pDrawable, pGC,
				SppPts[0], SppPts[1], PolyPoints[0],
	  			PolyPoints[3], FirstEnd, 0, 0, &n1) ;
			pt2Orig = pt2 = ppcRoundCap(pDrawable, pGC,
				SppPts[1], SppPts[0], PolyPoints[2],
	  			PolyPoints[1], SecondEnd, 0, 0, &n2) ;
			ptOrig = pt = ALLOCATE_LOCAL((n1 + n2) * sizeof(DDXPointRec)) ;
			for ( n = n1 ; n-- ; pt++, pt1++ ) {
				pt->x = pt1->x ;
				pt->y = pt1->y ;
			}
			Xfree(pt1Orig) ;
			for(n=n2 ;n-- ;pt++,pt2++) {
				pt->x = pt2->x ;
				pt->y = pt2->y ;
			}
			Xfree(pt2Orig) ;
			(*pGC->ops->FillPolygon)(pDrawable, pGC,
				Convex, CoordModeOrigin, n1 + n2, ptOrig) ;
			DEALLOCATE_LOCAL(ptOrig) ;
    		} else {
			tmpPts[0].x = ROUNDTOINT(PolyPoints[0].x) ;
			tmpPts[0].y = ROUNDTOINT(PolyPoints[0].y) ;
			tmpPts[1].x = ROUNDTOINT(PolyPoints[1].x) ;
			tmpPts[1].y = ROUNDTOINT(PolyPoints[1].y) ;
			tmpPts[2].x = ROUNDTOINT(PolyPoints[2].x) ;
			tmpPts[2].y = ROUNDTOINT(PolyPoints[2].y) ;
			tmpPts[3].x = ROUNDTOINT(PolyPoints[3].x) ;
			tmpPts[3].y = ROUNDTOINT(PolyPoints[3].y) ;
			if(tmpPts[0].x == tmpPts[3].x)
				tmpPts[3].x++;
			if(tmpPts[0].y == tmpPts[3].y)
				tmpPts[3].y++;
			if(tmpPts[1].x == tmpPts[2].x)
				tmpPts[2].x++;
			if(tmpPts[1].y == tmpPts[2].y)
				tmpPts[2].y++;
			(*pGC->ops->FillPolygon)(pDrawable, pGC,
				Convex, CoordModeOrigin, 4, tmpPts) ;
		}
		return ;
	}
    }

    (*pGC->ops->PolyFillRect)(pDrawable, pGC, 1, &tmpRect) ;

    if ( pGC->capStyle == CapRound ) {
	register int w2 = width / 2 ;
	SppPointRec pCenter, pEnd ;
	SppPointRec pCorner, pOtherCorner ;

	pCenter.x = (double)otmpPts[0].x ;
	pCenter.y = (double)otmpPts[0].y ;
	pEnd.x = (double)otmpPts[1].x ;
	pEnd.y = (double)otmpPts[1].y ;

	if (IS_VERT(otmpPts)) {
		pCorner.x = pCenter.x + w2 ;
		pCorner.y = pCenter.y ;
		pOtherCorner.x = pCenter.x - w2 ;
		pOtherCorner.y = pCenter.y ;
	} else
	if (IS_HORZ(otmpPts)) {
		pCorner.x = pCenter.x ;
		pCorner.y = pCenter.y + w2 ;
		pOtherCorner.x = pCenter.x ;
		pOtherCorner.y = pCenter.y - w2 ;
	}
	miRoundCap(pDrawable, pGC, pCenter, pEnd, pCorner,
	  pOtherCorner, FirstEnd, 0, 0, 0.0, 0.0) ;

	pCenter.x = (double)otmpPts[1].x ;
	pCenter.y = (double)otmpPts[1].y ;
	pEnd.x = (double)otmpPts[0].x ;
	pEnd.y = (double)otmpPts[0].y ;

	if (IS_VERT(otmpPts)) {
		pCorner.x = pCenter.x + w2 ;
		pCorner.y = pCenter.y ;
		pOtherCorner.x = pCenter.x - w2 ;
		pOtherCorner.y = pCenter.y ;
	} else {
		pCorner.x = pCenter.x ;
		pCorner.y = pCenter.y + w2 ;
		pOtherCorner.x = pCenter.x ;
		pOtherCorner.y = pCenter.y - w2 ;
	}
	miRoundCap(pDrawable, pGC, pCenter, pEnd, pCorner,
	  pOtherCorner, SecondEnd, 0, 0, 0.0, 0.0) ;
    }
    return ;
}

/* ppcROUNDCAP -- a private helper function
 * Put Rounded cap on end. pCenter is the center of this end of the line
 * pEnd is the center of the other end of the line. pCorner is one of the
 * two corners at this end of the line.  It doesn't matter which of the two
 * corners you give it. pOtherCorner is the other one.
 */
static
DDXPointPtr
ppcRoundCap(pDraw, pGC, pCenter, pEnd, pCorner, pOtherCorner, fLineEnd,
     xOrg, yOrg, cpt)
    DrawablePtr	pDraw ;
    GCPtr	pGC ;
    SppPointRec	pCenter, pEnd ;
    SppPointRec	pCorner, pOtherCorner ;
    int		fLineEnd, xOrg, yOrg ;
    int *cpt ;
{
    int		c, signc ;
    double	width ;
    double	dx, dy, hypot() ;
    xArc	arc ;
    DDXPointPtr pArcPts ;

    if (fLineEnd == NotEnd)
    {
	dx = pCenter.x - pCorner.x ;
	dy = pCenter.y - pCorner.y ;
	width = hypot(dx, dy) ;
    }
    else
	width = (pGC->lineWidth ? pGC->lineWidth : 1) ;
    if (PtEqual(pCenter, pEnd))
    {
	signc = 1 ;
    }
    else
    {
	c = (pCenter.x - pEnd.x) * (pCorner.y - pCenter.y) -
	     (pCorner.x - pCenter.x) * (pCenter.y - pEnd.y) ;
	signc = -sign(c) ;
    }

    /* These come back scaled by 64 and all ready to use */
    arc.x = ROUNDTOINT(pCenter.x - width / 2) ;
    arc.y = ROUNDTOINT(pCenter.y - width / 2) ;
    arc.width = ROUNDTOINT(width) ;
    arc.height = arc.width ;
    arc.angle1 = -PtToAngle(pCenter, pCorner) ;
    arc.angle2 = -signc * 180 * 64 ;
    pArcPts = (DDXPointPtr)NULL ;
    *cpt = miGetArcPts(&arc, 0, &pArcPts) ;
    return(pArcPts) ;
}

/* these are from <math.h>, but I'm told some systems don't have math.h Psi! */
extern double sqrt(), cos(), sin(), atan() ;
#define M_PI	3.14159265358979323846
#define M_PI_2	1.57079632679489661923

/* PTTOANGLE -- a private helper function
 * Given two points, compute the slope of the line segment
 * Result is in radians * 64
 */
static
int
PtToAngle(c, p)
    SppPointRec	c, p ;
{
    double	dx, dy, theta ;

    dx = p.x - c.x ;
    dy  = p.y - c.y ;
    if (dx == 0)
    {
	theta = (dy > 0) ? M_PI_2 : -M_PI_2 ;
    }
    else
        theta =(atan(dy/dx)) ;
    if (dx < 0)
	theta += M_PI ;
    theta = (theta * 64 * 180/M_PI) ;
    return ROUNDTOINT(theta) ;
}
#endif /* notdef */
