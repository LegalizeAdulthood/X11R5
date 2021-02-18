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

/* PPCZEROARCS-
Tom Paquin 2/19/89

	The idea is to draw solid, zero-width circles and ellipses
using Bresenham or similar techniques, while preserving X semantics.
Zero widths can be drawn easily without regard to intersection points of
polyarcs.
	I use Art Kauffman's modified algorithm for quadrants 2 and 1 
(respectively) while reflecting in X and Y, but not the 45s like bresenham.  
I generate spans or runs for Quadrant 2 and points for Quadrant 1.  
	For bounding boxes of even width (EG (0,0) to (3,3)) the reflection
hasn't an integer center.  I choose floor(center) for left and upper
reflections and ceiling(center) for right and lower reflections. (Lower is
increasing Y.)  This can create 4 pseudo-center points.  I refer to this 
technique as the Jeff Shift (Jeff Weinstien).  Jeffshifted circles will not 
draw the top dead center and bottom dead center runs in a circle, so we 
predraw them for Jeffshifted circles.
	For potentially clipped circles, I generate Spans since the spans
routines clip.  For cases where the circle's box resides wholly within the
composite clip, I merely draw.  For cases I cannot easily handle, I crap
out to mi, which is HORRENDOUSLY slow, especially if you've no FP hardware.
*/

 
/* $Header: /andrew/X11/r3src/release/server/ddx/ibm/ppc/RCS/ppcArc.c,v 9.9 89/04/29 23:59:59 jeff Exp $ */
/* $Source: /andrew/X11/r3src/release/server/ddx/ibm/ppc/RCS/ppcArc.c,v $ */
 
#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/release/server/ddx/ibm/ppc/RCS/ppcArc.c,v 9.9 89/04/29 23:59:59 jeff Exp $";
#endif
 

#include "X.h"
#include "Xprotostr.h"
#include "gcstruct.h"
#include "regionstr.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "miscstruct.h"
#include "misc.h"

#include "OScompiler.h"

#include "ppc.h"

/* FULLCIRCLE is 360 * 64 (sixty-fourths of a degree) */
#define FULLCIRCLE ( 360 << 6 )

void ppcFillIntegerFullArc();
void ppcIntegerFullArc() ;
void DrawFullArcOnSpans();
void DrawFullArcOnHW();
void FillFullArcOnSpans();
void FillFullArcOnHW();
void ppcIntegerArc();
void GetPts();
void DrawArcOnSpans();
extern int mfbGCPrivateIndex;

typedef struct MyPointStruct {
	short x;
	short y;
	} MyPointRec, *MyPointPtr;

typedef struct MyBoxStruct {
    signed long int x1, y1, x2, y2;
} MyBoxRec, *MyBoxPtr;

void
ppcPolyZeroArc(pWindow, pGC, narcs, parcs)
WindowPtr	pWindow;
GCPtr		pGC;
int		narcs;
xArc		*parcs;
{ /* This routine should only be called when LineStyle==Solid and LineWidth=0 */
register xArc	*thisarc;
int		box_w, box_h, angle;
int		(* checkcursor)(), *semaphore, CursorIsSaved;

ppcScrnPrivPtr  pScrPriv = (ppcScrnPrivPtr) 
			pWindow->drawable.pScreen->devPrivate;

CursorIsSaved = 0;
semaphore = pScrPriv->CursorSemaphore;
checkcursor = pScrPriv->CheckCursor;

if ( ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.alu  == GXnoop ) return ;

while (narcs--)
	{
	thisarc = parcs++;
/*
	printf("w = %d, h = %d, a1 = %d, a2 = %d\n", thisarc->width,
			thisarc->height, thisarc->angle1, thisarc->angle2);
*/

	box_w = thisarc->width;
	box_h = thisarc->height;
	angle = thisarc->angle2;
    
    	CursorIsSaved |= !(*semaphore) && (* checkcursor)( 
					thisarc->x, thisarc->y,
					thisarc->width, thisarc->height ) ;
        (*semaphore)++ ;

	if ( (angle >= FULLCIRCLE) || (angle <= -FULLCIRCLE) )
		{
		if ( ( box_w == box_h) || 
				( ( box_w < 4096 ) && ( box_h < 4096 ) ) )
			ppcIntegerFullArc(pWindow, pGC, thisarc);
		else    /* algorithm doesn't work because we'll
			   flood a 32-bit int */
			miPolyArc(pWindow, pGC, 1, thisarc);
		}
	else
		{ 
		ppcIntegerArc(pWindow, pGC, thisarc);
		}
        (*semaphore)-- ;
	}
    if ( ( !(*semaphore)) && CursorIsSaved ) (* pScrPriv->ReplaceCursor)() ;
return ;
}

void
ppcPolyFillArc(pWindow, pGC, narcs, parcs)
WindowPtr	pWindow;
GCPtr		pGC;
int		narcs;
xArc		*parcs;
{ /* This routine should only be called when FillStyle==FillSolid */
register xArc	*thisarc;
int		box_w, box_h, angle;
int		(* checkcursor)(), *semaphore, CursorIsSaved;

ppcScrnPrivPtr  pScrPriv = (ppcScrnPrivPtr) 
			pWindow->drawable.pScreen->devPrivate;

CursorIsSaved = 0;
semaphore = pScrPriv->CursorSemaphore;
checkcursor = pScrPriv->CheckCursor;

if ( ( (ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr )->colorRrop.alu  == GXnoop ) return ;

while (narcs--)
	{
	thisarc = parcs++;

	box_w = thisarc->width;
	box_h = thisarc->height;
	angle = thisarc->angle2;
    
    	CursorIsSaved |= !(*semaphore) && (* checkcursor)( 
					thisarc->x, thisarc->y,
					thisarc->width, thisarc->height ) ;
        (*semaphore)++ ;

	if ( (angle >= FULLCIRCLE) || (angle <= -FULLCIRCLE) )
		{
		if ( ( box_w == box_h) || 
				( ( box_w < 4096 ) && ( box_h < 4096 ) ) )
			ppcFillIntegerFullArc(pWindow, pGC, thisarc);
		else
			miPolyFillArc(pWindow, pGC, 1, thisarc);
		}
	else 
		{ 
		miPolyFillArc(pWindow, pGC, 1, thisarc);
		}
        (*semaphore)-- ;
	}
    if ( ( !(*semaphore)) && CursorIsSaved ) (* pScrPriv->ReplaceCursor)() ;
return ;
}

static void
ppcFillIntegerFullArc(pWindow, pGC, pArc)
WindowPtr 	pWindow;
GCPtr		pGC;
xArc		*pArc;
{
BoxRec		bbox;
RegionPtr       pCC;
ScreenPtr	pScreen;
xRectangle	rect;


if (pArc->width == 0) return;
if (pArc->height == 0) return;

/* set up the box and convert to screen coordinates */
bbox.x2 = (rect.x = bbox.x1 = pWindow->drawable.x + pArc->x) 
			+ (rect.width = pArc->width ) ;
bbox.y2 = (rect.y = bbox.y1 = pWindow->drawable.y + pArc->y) 
			+ (rect.height = pArc->height );

if ( pGC->fillStyle != FillSolid )
	{
	FillFullArcOnSpans(pWindow, pGC, &rect);
	return;
	}

pCC = ((ppcPrivGC *)pGC->devPrivates[mfbGCPrivateIndex].ptr)->pCompositeClip;
pScreen = pWindow->drawable.pScreen;

switch ( (*pScreen->RectIn)(pCC, &bbox))
	{
	case rgnOUT:
		/* nothing to draw, abandon ship */
		return;
	case rgnIN:
		/* blast away, mateys! */
		FillFullArcOnHW( 
			((ppcScrnPriv *)pScreen->devPrivate)->solidFill,
			&rect, 
			((ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr)->colorRrop.fgPixel,
			((ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr)->colorRrop.alu,
			((ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr)->colorRrop.planemask );
		break;
	case rgnPART:
		/* avast! time to use finesse. */
		FillFullArcOnSpans(pWindow, pGC, &rect);
		break;
	}
return ;
}

#define PAINT(X,Y,WIDTH)			\
	{					\
	xR = cxR + (X);				\
	xL = cxL - (X) - (WIDTH) + 1;		\
	yU = cyU - (Y) ;			\
	yL = cyL + (Y) ;			\
	/* Quadrant 1 */ (*paint)(color, alu, pm, xR, yU, WIDTH, 1) ; \
	/* Quadrant 2 */ (*paint)(color, alu, pm, xL, yU, WIDTH, 1) ; \
	/* Quadrant 3 */ (*paint)(color, alu, pm, xL, yL, WIDTH, 1) ; \
	/* Quadrant 4 */ (*paint)(color, alu, pm, xR, yL, WIDTH, 1) ; \
	}

#define SPAN(X,Y,W)	\
	{		\
	ppt->x = X; ppt->y = Y; *pwidth = W;	\
	ppt++; pwidth++; nspans++;		\
	}

static void
ppcIntegerFullArc(pWindow, pGC, pArc)
WindowPtr 	pWindow;
GCPtr		pGC;
xArc		*pArc;
{
BoxRec		bbox;
RegionPtr       pCC;
ScreenPtr	pScreen;
xRectangle	rect;

if (pArc->width == 0) return;
if (pArc->height == 0) return;

/* set up the box and convert to screen coordinates */
bbox.x2 = (rect.x = bbox.x1 = pWindow->drawable.x + pArc->x) 
			+ (rect.width = pArc->width ) ;
bbox.y2 = (rect.y = bbox.y1 = pWindow->drawable.y + pArc->y) 
			+ (rect.height = pArc->height );

if ( pGC->fillStyle != FillSolid )
	{
	DrawFullArcOnSpans(pWindow, pGC, &rect);
	return;
	}

pCC = ((ppcPrivGC *)pGC->devPrivates[mfbGCPrivateIndex].ptr)->pCompositeClip;
pScreen = pWindow->drawable.pScreen;

switch ( (*pScreen->RectIn)(pCC, &bbox))
	{
	case rgnOUT:
		/* nothing to draw, abandon ship */
		return;
	case rgnIN:
		/* blast away, mateys! */
		DrawFullArcOnHW( 
			((ppcScrnPriv *)pScreen->devPrivate)->solidFill,
			&rect, 
			((ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr)->colorRrop.fgPixel,
			((ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr)->colorRrop.alu,
			((ppcPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr)->colorRrop.planemask );
		break;
	case rgnPART:
		/* avast! time to use finesse. */
		/**** WHICH ONE IS FASTER???    ****/
		DrawFullArcOnSpans(pWindow, pGC, &rect);
		break;
	}
return ;
}

static void
ppcIntegerArc(pWindow, pGC, pArc)
	WindowPtr 	pWindow;
	GCPtr		pGC;
	xArc		*pArc;
	{

	if (pArc->width == 0) return;
	if (pArc->height == 0) return;

	if ( (pArc->width != pArc->height) && 
			( ( pArc->width > 2048 ) || ( pArc->height > 2048 ) ) )
		{
		miPolyArc(pWindow, pGC, 1, pArc);
		}
	else
		{
		DrawArcOnSpans(pWindow, pGC, pArc);
		}
	return ;
	}

#define HALFCIRCLE ( FULLCIRCLE >> 1 )
#define QUARTERCIRCLE ( FULLCIRCLE >> 2 )

extern unsigned short ppcSinTab[];

static int 
isin(theta)
	int theta;
	{
	int ret, tmptheta;
	int savtheta;

	savtheta = theta;
	/* Compilers can't seem to handle modulo of a negative number */
	theta %= FULLCIRCLE;
	if ( theta < 0 )
		theta += FULLCIRCLE;

/*
	printf("theta = %d\n", theta);
*/

	if ( ( tmptheta = theta % HALFCIRCLE ) > QUARTERCIRCLE )
		ret = ppcSinTab[(HALFCIRCLE - tmptheta) >> 1];
	else
		ret = ppcSinTab[(tmptheta) >> 1];

/*
	printf("tmptheta = %d, ret = %d\n", tmptheta, ret);
*/
		
	if ( theta > HALFCIRCLE )
		{
/*
		printf("sin(%d) is %d\n", savtheta, -ret);
*/
		return ( -ret );
		}
	else
		{
/*
		printf("sin(%d) is %d\n", savtheta, ret);
*/
		return ( ret );
		}

	}

#define icos(x) isin((x)+QUARTERCIRCLE)

static void
GetPts(pArc, a, b)
	xArc *pArc;
	MyPointRec *a, *b;
	{
	long int	rx, ry, a1, a2;

	if (pArc->angle2 < 0)
		{
		a1 = pArc->angle1 + pArc->angle2;
		a2 = pArc->angle1;
		}
	else 
		{
		a1 = pArc->angle1;
		a2 = pArc->angle1 + pArc->angle2;
		}

	rx = pArc->width >> 1;
	ry = pArc->height >> 1;

/*
	printf("rx = %d, ry = %d, a1 = %d, a2 = %d\n", rx, ry, a1, a2);
*/
	a->x = ( ( rx * icos(a1) ) + ( 1 << 14 ) ) >> 15;
	a->y = - ( ( ( ry * isin(a1) ) + ( 1 << 14 ) ) >> 15 );
	b->x = ( ( rx * icos(a2) ) + ( 1 << 14 ) ) >> 15;
	b->y = - ( ( ( ry * isin(a2) ) + ( 1 << 14 ) ) >> 15 );
/*
	printf("a = (%d, %d), b = (%d, %d)\n", a->x, a->y, b->x, b->y);
*/

	return ;
	}

static int lastradius = -1;
static MyPointPtr circlePts = 0;
static int *circleWidths = 0;
static int ncirclespans;

static MyPointRec	*ellipsePts = 0;
static int		*ellipseWidths = 0;
static int		nellipsespans = 0;
static int		lasta = -1, lastb = -1;

static MyPointRec	*quadPts = 0;
static int		*quadWidths = 0;
static int		nquadspans = 0;

static void
ComputeCQuad( r )
	int r;
{
register int 	dv;
short		moves, x, y;
int		startx, lasty, width;
MyPointPtr	ppt;
int		nspans;
int		*pwidth;

if ( r == lastradius )
	return;

Xfree(circlePts);
Xfree(circleWidths);

moves = -( y = r );
x = 0;
dv = 1-r;
width = 0;
startx = x+1;
lasty = y;


/* allocate space for the spans */
ppt = (MyPointPtr) Xalloc( ( ( 2 + 2 * r ) * sizeof(MyPointRec)));
if (ppt == 0)
	{
	ErrorF("ppcZeroCircle: ComputeCQuad: cannot malloc ppt");
	return ;
	}

pwidth = (int *) Xalloc( ( 2 + 2 * r ) * sizeof(int) );
if (pwidth == 0)
	{
	Xfree(ppt);
	ErrorF("ppcZeroCircle: ComputeCQuad: cannot malloc pwidth");
	return ;
	}

circlePts = ppt;
circleWidths = pwidth;
lastradius = r;

nspans = 0;

/* Draw 2 octants and reflect them.  Do not draw first Pel.  Draw Last Pel.  */

/* OCTANT 2 */
while(moves<0)
	{
	if (dv >= 0)
		{ /* go diagonal */
		x++; y--;
		moves += 2;
		dv += ( ((x-y) << 1) + 1 );
		SPAN(startx, lasty, width);
		startx = x;	
		lasty = y;	
		width = 1;
		}
	   else { /* go axial */
		x++;
		moves++;
		dv += ( (x<<1) + 1 );
		width++;	
		}
	}

if (width) SPAN(startx, lasty, width);

/* OCTANT 1 */
dv -= (x + y);
moves -= r ;

while (moves < 0)
	{
	if (dv<0)
		{ /* go diagonal */
		x++; y--;
		moves += 2;
		dv += ( ((x-y) << 1) + 1 );
		SPAN(x, y, 1);
		lasty = y;	
		}
	   else { /* go axial */
		y--;
		moves++;
		dv += ( 1 - (y<<1));
		SPAN(x, y, 1);
		}
	}

ncirclespans = nspans - 1;
return ;
}

static void
ComputeEQuad(a, b)
	int a, b;
	{
	long int 	TwoBsq, TwoAsq, dv1, dv2, TwoBsqX, TwoAsqY;
	int		x, y, startx;
	int		r, *pwidth, *pwidthInit, nspans;
	MyPointRec	*ppt, *pptInit;

	if ( a == lasta && b == lastb )
		return;

	x = 0; y = b;
	startx = 1;
	r = ( a>b ? a : b );

	pptInit = ppt = 
		(MyPointPtr) Xalloc( ( ( 14 + 8 * r ) * sizeof(MyPointRec)));
	if (ppt == 0)
		{
		ErrorF("ComputeEQuad: cannot malloc ppt");
		return ;
		}

	pwidthInit = pwidth = 
			(int *) Xalloc( ( ( 14 + 8 * r ) * sizeof(int)) );
	if (pwidth == 0)
		{
		Xfree(ppt);
		ErrorF("ComputeEQuad: cannot malloc pwidth");
		return ;
		}

	Xfree(ellipsePts);
	Xfree(ellipseWidths);

	nspans = 0;

	TwoBsq = (b * b) << 1 ;
	TwoAsq = (a * a) << 1 ;
	TwoBsqX = 0; /* x is still zero */
	TwoAsqY = TwoAsq * y;

	dv2 = - (b * TwoAsq);	 /* use dv2 as a temp to hold -2b(aa) */
	dv1 = (TwoBsq >> 1) + (dv2 >> 1) + (TwoAsq >> 3);
	dv2 += (TwoBsq >> 3) + (TwoAsq >> 1) ;

	/*START LOOP */
	while (dv2 < 0)
		{
		/* REGION 1 */
		x++;
		TwoBsqX += TwoBsq;
		if (dv1 < 0)
			{ /* MOVE AXIAL */
			dv1 += TwoBsqX + (TwoBsq >> 1);
			dv2 += TwoBsqX;
			}
		else 
			{ /* MOVE DIAGONAL */
			ppt->x = startx;
			ppt->y = y;
			*pwidth = x - startx;
			ppt++; pwidth++; nspans++;
			startx = x;
			y--;
			TwoAsqY -= TwoAsq;
			dv1 += TwoBsqX - TwoAsqY + (TwoBsq >> 1);
			dv2 += (TwoAsq >> 1) - TwoAsqY + TwoBsqX ;
			}
		}

		ppt->x = startx;
		ppt->y = y;
		*pwidth = x - startx + 1;
		ppt++; pwidth++; nspans++;

	while (y>1)
		{
		/* REGION 2 */
		y--;
		TwoAsqY -= TwoAsq;
		if (dv2 < 0)
			{  /* diagonal move */
			x++;
			TwoBsqX += TwoBsq;
			dv2 += (TwoAsq>>1) - TwoAsqY + TwoBsqX;
			}
		else 
			{  /* axial move */
			dv2 += (TwoAsq >> 1) - TwoAsqY;
			}
		ppt->x = x;
		ppt->y = y;
		*pwidth = 1;
		ppt++; pwidth++; nspans++;
		}

	nellipsespans = nspans;
	ellipsePts = pptInit;
	ellipseWidths = pwidthInit;
	return ;
	}

static void
ComputeQuad( a, b )
	int a, b;
	{
	if ( a == b )
		{
		ComputeCQuad( a );
		quadPts = circlePts;
		quadWidths = circleWidths;
		nquadspans = ncirclespans;
		}
	else
		{
		ComputeEQuad( a, b );
		quadPts = ellipsePts;
		quadWidths = ellipseWidths;
		nquadspans = nellipsespans;
		}
	}

static void
DrawFullArcOnHW(paint, pRect, color, alu, pm)
	register void 	(*paint)();
	xRectangle	*pRect;
	int		color;
	int 		alu;
	int		pm;
	{
	register int 	dv;
	short		x, y;
	int		diam;
	int		cxL, cxR, cyL, cyU;
	int		startx, lasty, width, xR, xL, yU, yL;
	int		nspans, *pwidth;
	MyPointPtr	ppt;
	int		a, b;

	a = ( pRect->width - 1 ) >> 1;
	b = ( pRect->height - 1 ) >> 1;

	if ( pRect->width & 1)
		{
		cxL = cxR = pRect->x + a;
		(*paint)(color, alu, pm, cxL, pRect->y, 1, 1) ;
		(*paint)(color, alu, pm, cxL, pRect->y + pRect->height - 1, 1, 1) ;
		}
	else
		{
		cxL = pRect->x + a;
		cxR = pRect->x + a + 1;
		(*paint)(color, alu, pm, cxL, pRect->y, 2, 1) ;
		(*paint)(color, alu, pm, cxL, pRect->y + pRect->height - 1, 2, 1) ;
		}

	if ( pRect->height & 1)
		{
		cyL = cyU = pRect->y + b;
		(*paint)(color, alu, pm, pRect->x, cyU, 1, 1) ;
		(*paint)(color, alu, pm, pRect->x + pRect->width - 1, cyU, 1, 1) ;
		}
	else
		{
		cyU = pRect->y + b;
		cyL = pRect->y + b + 1;
		(*paint)(color, alu, pm, pRect->x, cyU, 1, 2) ;
		(*paint)(color, alu, pm, pRect->x + pRect->width - 1, cyU, 1, 2) ;
		}

	ComputeQuad(a,b);

	for( nspans = nquadspans, ppt = quadPts, pwidth = quadWidths;
			nspans > 0; nspans--, ppt++, pwidth++ )
		{
		PAINT(ppt->x, ppt->y, *pwidth);
		}

	return ;
	}

static void
FillFullArcOnHW(paint, pRect, color, alu, pm)
	register void 	(*paint)();
	xRectangle	*pRect;
	int		color;
	int 		alu;
	int		pm;
	{
	register int 	dv;
	short		x, y;
	int		diam;
	int		cxL, cxR, cyL, cyU;
	int		startx, lasty, width, xR, xL, yU, yL;
	int		nspans, *pwidth;
	MyPointPtr	ppt;
	int		a, b, w;

	a = ( pRect->width - 1 ) >> 1;
	b = ( pRect->height - 1 ) >> 1;

	if ( pRect->width & 1 )
		{
		cxL = cxR = pRect->x + a;
		}
	else
		{
		cxL = pRect->x + a;
		cxR = pRect->x + a + 1;
		}
		
	if ( pRect->height & 1)
		{
		cyL = cyU = pRect->y + b;
		(*paint)(color, alu, pm, pRect->x, cyU, pRect->width, 1) ;
		}
	else
		{
		cyU = pRect->y + b;
		cyL = pRect->y + b + 1;
		(*paint)(color, alu, pm, pRect->x, cyU, pRect->width, 2) ;
		}

	ComputeQuad(a,b);

	for( nspans = nquadspans, ppt = quadPts, pwidth = quadWidths;
			nspans > 0; nspans--, ppt++, pwidth++ )
		{
		xL = cxL - ppt->x - *pwidth + 1;
		yU = cyU - ppt->y;
		yL = cyL + ppt->y;
		if ( cxL == cxR )
			w = ( ( ppt->x + *pwidth - 1 ) << 1 ) + 1;
		else
			w = ( ( ppt->x + *pwidth - 1 ) << 1 ) + 2;

		(*paint)(color, alu, pm, xL, yU, w, 1);
		(*paint)(color, alu, pm, xL, yL, w, 1);
		}

	return ;
	}

static void
DrawFullArcOnSpans(pWindow, pGC, pRect)
	WindowPtr	pWindow;
	GCPtr		pGC;
	xRectangle	*pRect;
	{
	register int 	dv;
	short		r, moves, x, y;
	int		diam;
	int		cxL, cxR, cyL, cyU;
	int		startx, lasty, width, xR, xL, yU, yL;
	MyPointPtr	pptInit, ppt, pptsrc, p1, p2, p3, p4;
	int		nspans, *pwidthInit, *pwidth, *pwidthsrc;
	int		i;
	int		a, b;

	a = ( pRect->width - 1 ) >> 1;
	b = ( pRect->height - 1 ) >> 1;
	nspans = 0;

	ComputeQuad(a,b);

	/* allocate space for the spans */
	pptInit = ppt = (MyPointPtr) Xalloc( ( ( 8 + nquadspans * 4 ) * sizeof(MyPointRec)));
	if (ppt == 0)
		{
		ErrorF("ppcZeroCircle: DrawCircleOnSpans: cannot malloc ppt");
		return ;
		}

	pwidthInit = pwidth = (int *) Xalloc( ( ( 8 + nquadspans * 4 ) * sizeof(int)) );
	if (pwidth == 0)
		{
		Xfree(ppt);
		ErrorF("ppcZeroCircle: DrawCircleOnSpans: cannot malloc pwidth");
		return ;
		}

	/* take care of joining point of reflection; worry about Jeffshift */

	if ( pRect->width & 1)
		{
		cxL = cxR = pRect->x + a;

		ppt->x = cxL; ppt->y = pRect->y;
		*pwidth = 1;
		ppt++; pwidth++; nspans++;

		ppt->x = cxL; ppt->y = pRect->y + pRect->height - 1;
		*pwidth = 1;
		ppt++; pwidth++; nspans++;
		}
	else
		{
		cxL = pRect->x + a;
		cxR = pRect->x + a + 1;
		ppt->x = cxL; ppt->y = pRect->y;
		*pwidth = 2;
		ppt++; pwidth++; nspans++;

		ppt->x = cxL; ppt->y = pRect->y + pRect->height - 1;
		*pwidth = 2;
		ppt++; pwidth++; nspans++;
		}

	if ( pRect->height & 1)
		{
		cyL = cyU = pRect->y + b;

		ppt->x = pRect->x; ppt->y = cyU;
		*pwidth = 1;
		ppt++; pwidth++; nspans++;

		ppt->x = pRect->x + pRect->width - 1; ppt->y = cyU;
		*pwidth = 1;
		ppt++; pwidth++; nspans++;
		}
	else
		{
		cyU = pRect->y + b;
		cyL = pRect->y + b + 1;

		ppt->x = pRect->x; ppt->y = cyU;
		*pwidth = 1;
		ppt++; pwidth++; nspans++;
		ppt->x = pRect->x; ppt->y = cyL;
		*pwidth = 1;
		ppt++; pwidth++; nspans++;

		ppt->x = pRect->x + pRect->width - 1; ppt->y = cyU;
		*pwidth = 1;
		ppt++; pwidth++; nspans++;
		ppt->x = pRect->x + pRect->width - 1; ppt->y = cyL;
		*pwidth = 1;
		ppt++; pwidth++; nspans++;
		}

	MOVE( quadWidths, pwidth, nquadspans * sizeof(int) );
	pwidth += nquadspans;
	MOVE( quadWidths, pwidth, nquadspans * sizeof(int) );
	pwidth += nquadspans;
	MOVE( quadWidths, pwidth, nquadspans * sizeof(int) );
	pwidth += nquadspans;
	MOVE( quadWidths, pwidth, nquadspans * sizeof(int) );

	for( i = nquadspans, p1 = ppt, p2 = p1 + nquadspans, 
			p3 = p2 + nquadspans, p4 = p3 + nquadspans, 
			pptsrc = quadPts, pwidthsrc = quadWidths; 
		i > 0; 
		i--, p1++, p2++, p3++, p4++, pptsrc++, pwidthsrc++ )
		{

		xR = cxR + pptsrc->x;
		xL = cxL - pptsrc->x - *pwidthsrc + 1;
		yU = cyU - pptsrc->y ;
		yL = cyL + pptsrc->y ;
		/* Quadrant 1 */ p1->x = xR; p1->y = yU;
		/* Quadrant 2 */ p2->x = xL; p2->y = yU;
		/* Quadrant 3 */ p3->x = xL; p3->y = yL;
		/* Quadrant 4 */ p4->x = xR; p4->y = yL;
	
		}

	(* pGC->ops->FillSpans)( pWindow, pGC, nspans + (nquadspans << 2), 
					pptInit, pwidthInit, FALSE );

	Xfree(pptInit);
	Xfree(pwidthInit);

	return ;
	}

static void
FillFullArcOnSpans(pWindow, pGC, pRect)
	WindowPtr	pWindow;
	GCPtr		pGC;
	xRectangle	*pRect;
	{
	register int 	dv;
	short		r, moves, x, y;
	int		diam;
	int		cxL, cxR, cyL, cyU;
	int		startx, lasty, width, xR, xL, yU, yL;
	MyPointPtr	pptInit, ppt, pptsrc, p1, p2, p3, p4;
	int		nspans, *pwidthInit, *pwidth, *pwidthsrc;
	int		i;
	int		a, b, w;

	a = ( pRect->width - 1 ) >> 1;
	b = ( pRect->height - 1 ) >> 1;
	nspans = 0;

	ComputeQuad(a,b);

	/* allocate space for the spans */
	pptInit = ppt = (MyPointPtr) Xalloc( ( ( 8 + nquadspans * 2 ) * sizeof(MyPointRec)));
	if (ppt == 0)
		{
		ErrorF("ppcZeroCircle: DrawCircleOnSpans: cannot malloc ppt");
		return ;
		}

	pwidthInit = pwidth = (int *) Xalloc( ( ( 8 + nquadspans * 2 ) * sizeof(int)) );
	if (pwidth == 0)
		{
		Xfree(ppt);
		ErrorF("ppcZeroCircle: DrawCircleOnSpans: cannot malloc pwidth");
		return ;
		}

	/* take care of joining point of reflection; worry about Jeffshift */

	if ( pRect->width & 1)
		{
		cxL = cxR = pRect->x + a;
		}
	else
		{
		cxL = pRect->x + a;
		cxR = pRect->x + a + 1;
		}

	if ( pRect->height & 1)
		{
		cyL = cyU = pRect->y + b;

		ppt->x = pRect->x; ppt->y = cyU;
		*pwidth = pRect->width;
		ppt++; pwidth++; nspans++;
		}
	else
		{
		cyU = pRect->y + b;
		cyL = pRect->y + b + 1;

		ppt->x = pRect->x; ppt->y = cyU;
		*pwidth = pRect->width;
		ppt++; pwidth++; nspans++;
		ppt->x = pRect->x; ppt->y = cyL;
		*pwidth = pRect->width;
		ppt++; pwidth++; nspans++;

		}

	for( i = nquadspans, pptsrc = quadPts, pwidthsrc = quadWidths; 
		i > 0; 
		i--, pptsrc++, pwidthsrc++ )
		{

		xL = cxL - pptsrc->x - *pwidthsrc + 1;
		yU = cyU - pptsrc->y ;
		yL = cyL + pptsrc->y ;

		if ( cxL == cxR )
			w = ( ( pptsrc->x + *pwidthsrc - 1 ) << 1 ) + 1;
		else
			w = ( ( pptsrc->x + *pwidthsrc - 1 ) << 1 ) + 2;

		ppt->x = xL; ppt->y = yU;
		*pwidth = w;
		ppt++; pwidth++;

		ppt->x = xL; ppt->y = yL;
		*pwidth = w;
		ppt++; pwidth++;
		}

	(* pGC->ops->FillSpans)( pWindow, pGC, nspans + (nquadspans << 1), 
					pptInit, pwidthInit, FALSE );

	Xfree(pptInit);
	Xfree(pwidthInit);

	return ;
	}

static void
DrawArcOnSpans(pWindow, pGC, pArc)
WindowPtr	pWindow;
GCPtr		pGC;
xArc		*pArc;
{
int		cxL, cxR, cyL, cyU;
int		xR, xL, yU, yL;
MyPointPtr	pptInit, ppt, pptsrc, p1, p2, p3, p4;
int		nspans, *pwidthInit, *pwidth, *pwidthsrc;
int		i;
int		aQuad, bQuad;
MyPointRec	pta, ptb;
int		shortArc = 0, longArc = 0;
int		q, tmpspans, *tmppwidth;
int		topPt = 0, botPt = 0;
int		leftPt = 0, rightPt = 0;
int		x1, x2, y1, y2;
int		a, b;

x2 = (x1 = pWindow->drawable.x + pArc->x) + pArc->width -1;
y2 = (y1 = pWindow->drawable.y + pArc->y) + pArc->height -1;

a = ( pArc->width - 1 ) >> 1;
b = ( pArc->height - 1 ) >> 1;

if ( pArc->width & 1)
	{
	cxL = cxR = pArc->x + a;
	}
else
	{
	cxL = pArc->x + a;
	cxR = pArc->x + a + 1;
	}

if ( pArc->height & 1)
	{
	cyL = cyU = pArc->y + b;
	}
else
	{
	cyU = pArc->y + b;
	cyL = pArc->y + b + 1;
	}

ComputeQuad( a, b );

/* allocate space for the spans */
pptInit = ppt = (MyPointPtr) Xalloc( ( ( 6 + nquadspans * 4 ) * sizeof(MyPointRec)));
if (ppt == 0)
	{
	ErrorF("ppcZeroCircle: DrawCArcOnSpans: cannot malloc ppt");
	return ;
	}

pwidthInit = pwidth = (int *) Xalloc( ( ( 6 + nquadspans * 4 ) * sizeof(int)) );
if (pwidth == 0)
	{
	Xfree(ppt);
	ErrorF("ppcZeroCircle: DrawCArcOnSpans: cannot malloc pwidth");
	return ;
	}

nspans = 0;

GetPts(pArc, &pta, &ptb);

if ( pta.x > a )
	pta.x = a;
if ( pta.y > b )
	pta.y = b;

if ( ptb.x > a )
	ptb.x = a;
if ( ptb.y > b )
	ptb.y = b;

#define QUAD(X,Y) (((X)>=0) ? ((Y)>0 ? 4 : 1) : ((Y)>0 ? 3:2))

aQuad = QUAD(pta.x, pta.y);
bQuad = QUAD(ptb.x, ptb.y);

/*
printf("a = (%d,%d), b = (%d,%d)\n", pta.x, pta.y, ptb.x, ptb.y );
printf("aQuad = %d, bQuad = %d\n", aQuad, bQuad);
*/

if ( bQuad == aQuad )
	{
	if ( pta.y > 0 )
		{
		if( pta.x > ptb.x )
			{
			longArc = 1;
			}
		else
			shortArc = 1;
		}
	else
		{
		if( pta.x < ptb.x )
			{
			longArc = 1;
			}
		else
			shortArc = 1;
		}
	}

if( shortArc ) /* arc is within one quadrant */
	{
/*
printf("shortArc\n");
*/
	switch( aQuad )
		{
		case 1:
			pptsrc = quadPts + b - MAX(-pta.y, -ptb.y);
			pwidthsrc = quadWidths + b - MAX(-pta.y, -ptb.y);
			nspans = ABS( pta.y - ptb.y ) + 1;
			for ( i = 0; i < nspans; i++, pptsrc++, ppt++ )
				{
				ppt->x = cxR + pptsrc->x;
				ppt->y = cyU - pptsrc->y;
				}
			break;
		case 2:
			pptsrc = quadPts + b - MAX(-pta.y, -ptb.y);
			pwidth = pwidthsrc = quadWidths + b - MAX(-pta.y, -ptb.y);
			nspans = ABS( pta.y - ptb.y ) + 1;
			for ( i = 0; i < nspans; i++, pptsrc++, ppt++, pwidth++ )
				{
				ppt->x = cxL - pptsrc->x - *pwidth + 1;
				ppt->y = cyU - pptsrc->y;
				}
			break;
		case 3:
			pptsrc = quadPts + b - MAX(pta.y, ptb.y);
			pwidth = pwidthsrc = quadWidths + b - MAX(pta.y, ptb.y);
			nspans = ABS( pta.y - ptb.y ) + 1;
			for ( i = 0; i < nspans; i++, pptsrc++, ppt++, pwidth++ )
				{
				ppt->x = cxL - pptsrc->x - *pwidth + 1;
				ppt->y = cyL + pptsrc->y;
				}
			break;
		case 4:
			pptsrc = quadPts + b - MAX(pta.y, ptb.y);
			pwidthsrc = quadWidths + b - MAX(pta.y, ptb.y);
			nspans = ABS( pta.y - ptb.y ) + 1;
			for ( i = 0; i < nspans; i++, pptsrc++, ppt++, pwidth++ )
				{
				ppt->x = cxR + pptsrc->x;
				ppt->y = cyL + pptsrc->y;
				}
			break;
		}
	(* pGC->ops->FillSpans)( pWindow, pGC, nspans, pptInit, pwidthsrc, FALSE );
	
	}
else if ( longArc )
	{
/*
printf("longArc\n");
*/

	/* take care of joining point of reflection; worry about Jeffshift */
	ppt->x = x1; ppt->y = cyU; *pwidth = 1;
	ppt++; pwidth++;
	ppt->x = x2; ppt->y = cyU; *pwidth = 1;
	ppt++; pwidth++;

	if (cxL == cxR)
		{
		ppt->x = cxL; ppt->y = y1; *pwidth = 1;
		ppt++; pwidth++;
		ppt->x = cxL; ppt->y = y2; *pwidth = 1;
		ppt++; pwidth++;
		nspans = 4;
		}
	else 
		{  /* Jeffshift */
		ppt->x = cxL; ppt->y = y1; *pwidth = 2;
		ppt++; pwidth++;
		ppt->x = cxL; ppt->y = y2; *pwidth = 2;
		ppt++; pwidth++;
		ppt->x = x1; ppt->y = cyL; *pwidth = 1;
		ppt++; pwidth++;
		ppt->x = x2; ppt->y = cyL; *pwidth = 1;
		ppt++; pwidth++;
		nspans = 6;
		}

	MOVE( quadWidths, pwidth, nquadspans * sizeof(int) );
	pwidth += nquadspans;
	MOVE( quadWidths, pwidth, nquadspans * sizeof(int) );
	pwidth += nquadspans;
	MOVE( quadWidths, pwidth, nquadspans * sizeof(int) );
	pwidth += nquadspans;

	nspans = nquadspans * 3 + nspans;

	switch ( aQuad )
		{
		case 1:
			for( i = nquadspans, p2 = ppt,
					p3 = p2 + nquadspans,
					p4 = p3 + nquadspans,
					p1 = p4 + nquadspans,
					pptsrc = quadPts,
					pwidthsrc = quadWidths;
				i > 0;
				i--, p2++, p3++, p4++, pptsrc++,
					pwidthsrc++ )
				{

				xR = cxR + pptsrc->x;
				xL = cxL - pptsrc->x - *pwidthsrc + 1;
				yU = cyU - pptsrc->y ;
				yL = cyL + pptsrc->y ;
				/* Quadrant 2 */ p2->x = xL; p2->y = yU;
				/* Quadrant 3 */ p3->x = xL; p3->y = yL;
				/* Quadrant 4 */ p4->x = xR; p4->y = yL;
				if ( ( (-(pptsrc->y)) < MIN(pta.y, ptb.y) ) 
					|| ( (-(pptsrc->y)) > MAX(pta.y, ptb.y) ) )
					{
					/* Quadrant 1 */ 
					p1->x = xR; p1->y = yU;
					p1++;
					*pwidth = *pwidthsrc;
					pwidth++;
					nspans++;
					}
				}
			break;

		case 2:
			for( i = nquadspans, p1 = ppt,
					p3 = p1 + nquadspans,
					p4 = p3 + nquadspans,
					p2 = p4 + nquadspans,
					pptsrc = quadPts,
					pwidthsrc = quadWidths;
				i > 0;
				i--, p1++, p3++, p4++, pptsrc++,
					pwidthsrc++ )
				{

				xR = cxR + pptsrc->x;
				xL = cxL - pptsrc->x - *pwidthsrc + 1;
				yU = cyU - pptsrc->y ;
				yL = cyL + pptsrc->y ;
				/* Quadrant 1 */ p1->x = xR; p1->y = yU;
				/* Quadrant 3 */ p3->x = xL; p3->y = yL;
				/* Quadrant 4 */ p4->x = xR; p4->y = yL;
				if ( ( (-(pptsrc->y)) < MIN(pta.y, ptb.y) ) 
					|| ( (-(pptsrc->y)) > MAX(pta.y, ptb.y) ) )
					{
					/* Quadrant 2 */ 
					p2->x = xL; p2->y = yU;
					p2++;
					*pwidth = *pwidthsrc;
					pwidth++;
					nspans++;
					}
				}
			break;

		case 3:
			for( i = nquadspans, p1 = ppt,
					p2 = p1 + nquadspans,
					p4 = p2 + nquadspans,
					p3 = p4 + nquadspans,
					pptsrc = quadPts,
					pwidthsrc = quadWidths;
				i > 0;
				i--, p1++, p2++, p4++, pptsrc++,
					pwidthsrc++ )
				{

				xR = cxR + pptsrc->x;
				xL = cxL - pptsrc->x - *pwidthsrc + 1;
				yU = cyU - pptsrc->y ;
				yL = cyL + pptsrc->y ;
				/* Quadrant 1 */ p1->x = xR; p1->y = yU;
				/* Quadrant 2 */ p2->x = xL; p2->y = yU;
				/* Quadrant 4 */ p4->x = xR; p4->y = yL;
				if ( ( pptsrc->y < MIN(pta.y, ptb.y) ) 
					|| ( pptsrc->y > MAX(pta.y, ptb.y) ) )
					{
					/* Quadrant 3 */ 
					p3->x = xL; p3->y = yL;
					p3++;
					*pwidth = *pwidthsrc;
					pwidth++;
					nspans++;
					}
				}
			break;
			
		case 4:
			for( i = nquadspans, p1 = ppt,
					p2 = p1 + nquadspans,
					p3 = p2 + nquadspans,
					p4 = p3 + nquadspans,
					pptsrc = quadPts,
					pwidthsrc = quadWidths;
				i > 0;
				i--, p1++, p2++, p3++, pptsrc++,
					pwidthsrc++ )
				{

				xR = cxR + pptsrc->x;
				xL = cxL - pptsrc->x - *pwidthsrc + 1;
				yU = cyU - pptsrc->y ;
				yL = cyL + pptsrc->y ;
				/* Quadrant 1 */ p1->x = xR; p1->y = yU;
				/* Quadrant 2 */ p2->x = xL; p2->y = yU;
				/* Quadrant 3 */ p3->x = xL; p3->y = yL;
				if ( ( pptsrc->y < MIN(pta.y, ptb.y) ) 
					|| ( pptsrc->y > MAX(pta.y, ptb.y) ) )
					{
					/* Quadrant 4 */ 
					p4->x = xR; p4->y = yL;
					p4++;
					*pwidth = *pwidthsrc;
					pwidth++;
					nspans++;
					}
				}
			break;

		}
	(* pGC->ops->FillSpans)( pWindow, pGC, nspans, pptInit, pwidthInit, FALSE );
	}
else /* start and end of arc are not in same quadrant */
	{
/*
printf("mediumArc\n");
*/
	/* first we draw the starting part of the arc */
	switch ( aQuad )
		{
		case 1:
			pptsrc = quadPts;
			pwidthsrc = quadWidths;
			nspans = MIN(pta.y + b + 1, b);
			for ( i = 0; i < nspans; i++, pptsrc++, ppt++ )
				{
				ppt->x = cxR + pptsrc->x;
				ppt->y = cyU - pptsrc->y;
				}
			topPt = 1;
			break;
		case 2:
			pptsrc = quadPts + b + pta.y;
			tmppwidth = pwidthsrc = quadWidths + b + pta.y;
			nspans = -pta.y;
			for ( i = 0; i < nspans; i++, pptsrc++, ppt++, tmppwidth++ )
				{
				ppt->x = cxL - pptsrc->x - *tmppwidth + 1;
				ppt->y = cyU - pptsrc->y;
				}
			leftPt = 1;
			break;
		case 3:
			pptsrc = quadPts;
			tmppwidth = pwidthsrc = quadWidths;
			nspans = MIN(b - pta.y + 1, b);
			for ( i = 0; i < nspans; i++, pptsrc++, ppt++, tmppwidth++ )
				{
				ppt->x = cxL - pptsrc->x - *tmppwidth + 1;
				ppt->y = cyL + pptsrc->y;
				}
			botPt = 1;
			break;
		case 4:
			pptsrc = quadPts + b - pta.y;
			pwidthsrc = quadWidths + b - pta.y;
			nspans = pta.y;
			for ( i = 0; i < nspans; i++, pptsrc++, ppt++ )
				{
				ppt->x = cxR + pptsrc->x;
				ppt->y = cyL + pptsrc->y;
				}
			rightPt = 1;
			break;
		}
/*
	printf("medium: first move\n");
*/

	MOVE( pwidthsrc, pwidth, nspans * sizeof(int) );
	pwidth += nspans;

#ifdef notdef
	if ( ( *( pwidth - 1 ) ) > 5 )
		{
	printf("ppt->x = %d, ppt->y = %d, *pwidth = %d\n", 
		(ppt-1)->x, (ppt-1)->y, *(pwidth-1) );
	printf("medium: first move\n");
	printf("aQuad = %d\n", aQuad);
	printf("pwidthsrc = 0x%x, pwidth = 0x%x, nspans = %d\n",
		pwidthsrc, pwidth, nspans );
	printf("a = %d, b = %d, pta = (%d,%d)\n", a, b, pta.x, pta.y);
		}
#endif

	/* next we draw the end of the arc */
	switch ( bQuad)
		{
		case 1:
			pptsrc = quadPts + b + ptb.y;
			pwidthsrc = quadWidths + b + ptb.y;
			tmpspans = -ptb.y;
			for ( i = 0; i < tmpspans; i++, pptsrc++, ppt++ )
				{
				ppt->x = cxR + pptsrc->x;
				ppt->y = cyU - pptsrc->y;
				}
			rightPt = 1;
			break;
		case 2:
			pptsrc = quadPts;
			tmppwidth = pwidthsrc = quadWidths;
			tmpspans = MIN(b + ptb.y + 1, b);
			for ( i = 0; i < tmpspans; i++, pptsrc++, ppt++, tmppwidth++ )
				{
				ppt->x = cxL - pptsrc->x - *tmppwidth + 1;
				ppt->y = cyU - pptsrc->y;
				}
			topPt = 1;
			break;
		case 3:
			pptsrc = quadPts + b - ptb.y;
			tmppwidth = pwidthsrc = quadWidths + b - ptb.y;
			tmpspans = ptb.y;
			for ( i = 0; i < tmpspans; i++, pptsrc++, ppt++, tmppwidth++ )
				{
				ppt->x = cxL - pptsrc->x - *tmppwidth + 1;
				ppt->y = cyL + pptsrc->y;
				}
			leftPt = 1;
			break;
		case 4:
			pptsrc = quadPts;
			pwidthsrc = quadWidths;
			tmpspans = MIN(b - ptb.y + 1, b);
			for ( i = 0; i < tmpspans; i++, pptsrc++, ppt++ )
				{
				ppt->x = cxR + pptsrc->x;
				ppt->y = cyL + pptsrc->y;
				}
			botPt = 1;
			break;
		}



	MOVE( pwidthsrc, pwidth, tmpspans * sizeof(int) );
	pwidth += tmpspans;
	nspans += tmpspans;

#ifdef notdef
	if ( ( *( pwidth - 1 ) ) > 5 )
		{
	printf("ppt->x = %d, ppt->y = %d, *pwidth = %d\n", 
		(ppt-1)->x, (ppt-1)->y, *(pwidth-1) );
	printf("medium: second move\n");
	printf("bQuad = %d\n", bQuad);
	printf("pwidthsrc = 0x%x, pwidth = 0x%x, tmpspans = %d\n",
		pwidthsrc, pwidth, tmpspans );
	printf("a = %d, b = %d, ptb = (%d,%d)\n", a, b, ptb.x, ptb.y);
		}
#endif

	
	/*
	 * Fill the whole quads.
	 */
	q = aQuad + 1;
	if ( q == 5 ) q = 1;
	while ( q != bQuad )
		{
		switch ( q )
			{
			case 1:
				pptsrc = quadPts;
				pwidthsrc = quadWidths;
				for ( i = 0; i < nquadspans; i++, pptsrc++, ppt++ )
					{
					ppt->x = cxR + pptsrc->x;
					ppt->y = cyU - pptsrc->y;
					}
				topPt = 1;
				rightPt = 1;
				break;
			case 2:
				pptsrc = quadPts;
				tmppwidth = pwidthsrc = quadWidths;
				for ( i = 0; i < nquadspans; i++, pptsrc++, ppt++, tmppwidth++ )
					{
					ppt->x = cxL - pptsrc->x - *tmppwidth + 1;
					ppt->y = cyU - pptsrc->y;
					}
				topPt = 1;
				leftPt = 1;
				break;
			case 3:
				pptsrc = quadPts;
				tmppwidth = pwidthsrc = quadWidths;
				for ( i = 0; i < nquadspans; i++, pptsrc++, ppt++, tmppwidth++ )
					{
					ppt->x = cxL - pptsrc->x - *tmppwidth + 1;
					ppt->y = cyL + pptsrc->y;
					}
				botPt = 1;
				leftPt = 1;
				break;
			case 4:
				pptsrc = quadPts;
				pwidthsrc = quadWidths;
				for ( i = 0; i < nquadspans; i++, pptsrc++, ppt++ )
					{
					ppt->x = cxR + pptsrc->x;
					ppt->y = cyL + pptsrc->y;
					}
				botPt = 1;
				rightPt = 1;
				break;
			}
/*
		printf("medium: third move\n");
*/
		MOVE( pwidthsrc, pwidth, nquadspans * sizeof(int) );
		pwidth += nquadspans;
		nspans += nquadspans;

		q++;
		if ( q == 5 ) q = 1;
		}

	/* take care of joining point of reflection; worry about Jeffshift */


	if (cxL == cxR)
		{
		if ( topPt )
			{
			ppt->x = cxL; ppt->y = y1; *pwidth = 1;
			ppt++; pwidth++;
			nspans++;
			}
		if ( botPt )
			{
			ppt->x = cxL; ppt->y = y2; *pwidth = 1;
			ppt++; pwidth++;
			nspans++;
			}
		}
	else 
		{  /* Jeffshift */
		if ( topPt )
			{
			ppt->x = cxL; ppt->y = y1; *pwidth = 2;
			ppt++; pwidth++;
			nspans++;
			}
		if ( botPt )
			{
			ppt->x = cxL; ppt->y = y2; *pwidth = 2;
			ppt++; pwidth++;
			nspans++;
			}
		}
	if ( cyU == cyL )
		{  /* no jeffshift */
		if ( leftPt )
			{
			ppt->x = x1; ppt->y = cyL; *pwidth = 1;
			ppt++; pwidth++;
			nspans++;
			}
		if ( rightPt )
			{
			ppt->x = x2; ppt->y = cyL; *pwidth = 1;
			ppt++; pwidth++;
			nspans++;
			}
		}
	else
		{  /* jeffshift */
		if ( leftPt )
			{
			ppt->x = x1; ppt->y = cyL; *pwidth = 1;
			ppt++; pwidth++;
			nspans++;
			ppt->x = x1; ppt->y = cyU; *pwidth = 1;
			ppt++; pwidth++;
			nspans++;
			}
		if ( rightPt )
			{
			ppt->x = x2; ppt->y = cyL; *pwidth = 1;
			ppt++; pwidth++;
			nspans++;
			ppt->x = x2; ppt->y = cyU; *pwidth = 1;
			ppt++; pwidth++;
			nspans++;
			}

		}
	(* pGC->ops->FillSpans)( pWindow, pGC, nspans, pptInit, pwidthInit, FALSE );
	}

Xfree(pptInit);
Xfree(pwidthInit);

return ;
}
