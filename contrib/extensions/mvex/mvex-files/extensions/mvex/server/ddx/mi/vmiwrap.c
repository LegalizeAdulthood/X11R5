/*
 * Copyright 1990 by Tektronix, Inc., Beaverton, Oregon, and
 * the Massachusetts Institute of Technology, Cambridge, Massachusetts.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Tektronix or M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Tektronix and M.I.T. make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * TEKTRONIX AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL TEKTRONIX OR M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: /n/lacey/usr/local/src/video/MVEX/server/ddx/mi/RCS/vmiwrap.c,v 1.1 1991/09/26 21:20:06 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#include <sys/time.h>
#include "X.h"
#include "scrnintstr.h"
#include "misc.h"
#include "MVEX.h"
#include "MVEXproto.h"
#include "videostr.h"
#include "vmi.h"
#include "dixfontstr.h"

/*
 * vmiGetImage - handle GetImage in a simple manner
 */
void
vmiGetImage( pDrawable, sx, sy, w, h, format, planeMask, pdstLine)
    DrawablePtr pDrawable;
    int		sx, sy, w, h;
    unsigned int format;
    unsigned long planeMask;
    pointer	pdstLine;
/*
 *	DESCRIPTION
 *		If your hardware cannot guarantee that it can get all the
 *		bits in one frame time, then you can use this routine to
 *		shut it down and start it up again.
 *
 *	RETURNS
 *		void
 *
 */
{
    VideoScreenPtr vp = ScreenVideoInfo[ pDrawable->pScreen->myNum ];
    int i, nEntries;
    ActiveVideoPtr activeVDraws = &vp->activeVDraws;
    Bool suspended = FALSE;

    if (nEntries = activeVDraws->nEntries) {
	for (i = nEntries - 1; i >= 0; i--) {
	    if (activeVDraws->vDrawArray[ i ]->pDraw == pDrawable) {
		suspended = vmiSuspendVideo(pDrawable);
		break;
	    }
	}
    }

    vp->pScreen->GetImage = vp->GetImage;
    (*vp->GetImage)(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
    vp->pScreen->GetImage = vmiGetImage;

    if (suspended)
	vmiResumeVideo(pDrawable);
}

static void vmiValidateGC(), vmiCopyGC(), vmiDestroyGC(), vmiChangeGC ();
static void vmiChangeClip(), vmiDestroyClip(), vmiCopyClip();

static GCFuncs vmiGCFuncs = {
    vmiValidateGC,
    vmiChangeGC,
    vmiCopyGC,
    vmiDestroyGC,
    vmiChangeClip,
    vmiDestroyClip,
    vmiCopyClip,
};

/* vmiCopyArea(), vmiCopyPlane() declared in vmi.h */
static void	  vmiFillSpans(),	vmiSetSpans(),	    vmiPutImage();
static void	  vmiPolyPoint(),	vmiPolylines(),     vmiPolySegment();
static void	  vmiPolyRectangle(),	vmiPolyArc(),	    vmiFillPolygon();
static void	  vmiPolyFillRect(),	vmiPolyFillArc();
static int	  vmiPolyText8(),	vmiPolyText16();
static void	  vmiImageText8(),	vmiImageText16();
static void	  vmiImageGlyphBlt(),	vmiPolyGlyphBlt();
static void	  vmiPushPixels(),	vmiLineHelper();

static GCOps vmiGCOps = {
    vmiFillSpans,    vmiSetSpans,     vmiPutImage,	
    vmiCopyArea,     vmiCopyPlane,    vmiPolyPoint,
    vmiPolylines,    vmiPolySegment,  vmiPolyRectangle,
    vmiPolyArc,	     vmiFillPolygon,  vmiPolyFillRect,
    vmiPolyFillArc,  vmiPolyText8,    vmiPolyText16,
    vmiImageText8,   vmiImageText16,  vmiImageGlyphBlt,
    vmiPolyGlyphBlt, vmiPushPixels,   vmiLineHelper,
};

#define CALL_GC_OP_AND_RETURN(pGC, func, result, args)			\
    VmiPrivGCPtr privGC = VmiGetGCPrivate(pGC);				\
									\
    (pGC)->ops = privGC->ops;						\
    result = (*(pGC)->ops->func) args;					\
    (pGC)->ops = &vmiGCOps;						\
    return (result);

#define CALL_GC_OP(pGC, func, args)					\
    VmiPrivGCPtr privGC = VmiGetGCPrivate(pGC);				\
									\
    (pGC)->ops = privGC->ops;						\
    (*(pGC)->ops->func) args;						\
    (pGC)->ops = &vmiGCOps;

#define CALL_GC_FUNC(pGC, func, args)					\
    VmiPrivGCPtr privGC = VmiGetGCPrivate(pGC);				\
									\
    (pGC)->funcs = privGC->funcs;					\
    (*(pGC)->funcs->func) args;						\
    (pGC)->funcs = &vmiGCFuncs;

static void
vmiValidateGC (pGC, stateChanges, pDrawable)
    GCPtr	    pGC;
    unsigned long   stateChanges;
    DrawablePtr	    pDrawable;
{
    CALL_GC_FUNC(pGC, ValidateGC,
	/* (*ValidateGC) */ (pGC, stateChanges, pDrawable));
}

static void
vmiChangeGC (pGC, mask)
    GCPtr   pGC;
    unsigned long   mask;
{
    CALL_GC_FUNC(pGC, ChangeGC,
	/* (*ChangeGC) */ (pGC, mask));
}

static void
vmiCopyGC (pGCSrc, mask, pGCDst)
    GCPtr   pGCSrc, pGCDst;
    unsigned long   mask;
{
    CALL_GC_FUNC(pGCDst, CopyGC,
	/* (*CopyGC) */ (pGCSrc, mask, pGCDst));
}

static void
vmiDestroyGC (pGC)
    GCPtr   pGC;
{
    VmiPrivGCPtr privGC = VmiGetGCPrivate(pGC);

    pGC->funcs = privGC->funcs;
    (*pGC->funcs->DestroyGC) (pGC);

    /* leave it unwrapped */
}

static void
vmiChangeClip (pGC, type, pvalue, nrects)
    GCPtr   pGC;
    int		type;
    pointer	pvalue;
    int		nrects;
{
    CALL_GC_FUNC(pGC, ChangeClip,
	/* (*ChangeClip) */ (pGC, type, pvalue, nrects));
}

static void
vmiDestroyClip (pGC)
    GCPtr   pGC;
{
    CALL_GC_FUNC(pGC, DestroyClip,
	/* (*DestroyClip) */ (pGC));
}

static void
vmiCopyClip(pgcDst, pgcSrc)
    GCPtr pgcDst, pgcSrc;
{
    CALL_GC_FUNC(pgcDst, CopyClip,
	/* (*CopyClip) */ (pgcDst, pgcSrc));
}

/*
 * vmiCreateGC - handle CreateGC in a simple manner
 */
Bool vmiCreateGC(pGC)
    register GCPtr pGC;
/*
 *	DESCRIPTION
 *		If your hardware cannot guarantee that it can get all the
 *		bits in one frame time, then you can use this routine to
 *		grab the CopyArea and CopyPlane GC vectors: these should
 *		shut it down and start it up again.
 *
 *	RETURNS
 *		Bool
 *
 */
{
    VmiPrivGCPtr privGC = VmiGetGCPrivate(pGC);
    VideoScreenPtr vp = ScreenVideoInfo[ pGC->pScreen->myNum ];
    Bool result;

    pGC->pScreen->CreateGC = vp->CreateGC;
    result = (*vp->CreateGC)(pGC);
    pGC->pScreen->CreateGC = vmiCreateGC;

    privGC->ops = pGC->ops;
    privGC->funcs = pGC->funcs;
    pGC->ops = &vmiGCOps;
    pGC->funcs = &vmiGCFuncs;

    return (result);
}

static void
vmiFillSpans(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		nInit;			/* number of spans to fill */
    DDXPointPtr pptInit;		/* pointer to list of start points */
    int		*pwidthInit;		/* pointer to list of n widths */
    int 	fSorted;
{
    CALL_GC_OP(pGC, FillSpans,
	/* (*FillSpans) */ (pDrawable, pGC, nInit,
			    pptInit, pwidthInit, fSorted));
}

static void
vmiSetSpans(pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted)
    DrawablePtr		pDrawable;
    GCPtr		pGC;
    int			*psrc;
    register DDXPointPtr ppt;
    int			*pwidth;
    int			nspans;
    int			fSorted;
{
    CALL_GC_OP(pGC, SetSpans,
	/* (*SetSpans) */ (pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted));
}

static void
vmiPutImage(pDrawable, pGC, depth, x, y, w, h, leftPad, format, pBits)
    DrawablePtr	  pDrawable;
    GCPtr   	  pGC;
    int		  depth;
    int	    	  x;
    int	    	  y;
    int	    	  w;
    int	    	  h;
    int	    	  format;
    char    	  *pBits;
{
    CALL_GC_OP(pGC, PutImage,
	/* (*PutImage) */ (pDrawable, pGC, depth, x, y, w, h,
			   leftPad, format, pBits));
}

/*
 * vmiCopy - common function for pGC->ops->CopyArea and pGC->ops->CopyPlane
 */
static RegionPtr
vmiCopy (pSrc, pDst, pGC, srcx, srcy, w, h, dstx, dsty, plane, copyFunc)
    DrawablePtr	  pSrc;
    DrawablePtr	  pDst;
    GCPtr   	  pGC;
    int	    	  srcx;
    int	    	  srcy;
    int	    	  w;
    int	    	  h;
    int	    	  dstx;
    int	    	  dsty;
    unsigned long plane;
    RegionPtr     (*copyFunc)();
/*
 *	DESCRIPTION
 *		If your hardware cannot guarantee that it can get all the
 *		bits in one frame time, then you can use this routine to
 *		shut it down and start it up again.
 *
 *	RETURNS
 *		void
 *
 */
{
    VmiPrivGCPtr privGC = VmiGetGCPrivate(pGC);
    VideoScreenPtr vp = ScreenVideoInfo[ pGC->pScreen->myNum ];
    int i, nEntries;
    ActiveVideoPtr activeVDraws = &vp->activeVDraws;
    Bool srcSuspended = FALSE;
    Bool dstSuspended = FALSE;
    RegionPtr result;
    VideoDrawPtr vDraw;

    if (nEntries = activeVDraws->nEntries) {
	for (i = nEntries - 1; i >= 0; i--) {
	    vDraw = activeVDraws->vDrawArray[ i ];
	    if (vDraw->pDraw == pSrc)
		srcSuspended = vmiSuspendVideo(pSrc);
	    if (pDst != pSrc && vDraw->pDraw == pDst)
		dstSuspended = vmiSuspendVideo(pDst);
	}
    }

    (pGC)->ops = privGC->ops;
    result = (*copyFunc) (pSrc, pDst, pGC, srcx, srcy, w, h, dstx, dsty, plane);
    (pGC)->ops = &vmiGCOps;

    if (srcSuspended)
	vmiResumeVideo(pSrc);
    if (dstSuspended)
	vmiResumeVideo(pDst);

    return (result);
}

RegionPtr
vmiCopyArea (pSrc, pDst, pGC, srcx, srcy, w, h, dstx, dsty)
    DrawablePtr	  pSrc;
    DrawablePtr	  pDst;
    GCPtr   	  pGC;
    int	    	  srcx;
    int	    	  srcy;
    int	    	  w;
    int	    	  h;
    int	    	  dstx;
    int	    	  dsty;
{
    VmiPrivGCPtr privGC = VmiGetGCPrivate(pGC);

    return vmiCopy (pSrc, pDst, pGC,
		    srcx, srcy, w, h,
		    dstx, dsty,
		    0, privGC->ops->CopyArea);
}

RegionPtr
vmiCopyPlane (pSrc, pDst, pGC, srcx, srcy, w, h, dstx, dsty, plane)
    DrawablePtr	  pSrc;
    DrawablePtr	  pDst;
    register GC   *pGC;
    int     	  srcx,
		  srcy;
    int     	  w,
		  h;
    int     	  dstx,
		  dsty;
    unsigned long  plane;
{
    VmiPrivGCPtr privGC = VmiGetGCPrivate(pGC);

    return vmiCopy (pSrc, pDst, pGC,
		    srcx, srcy, w, h,
		    dstx, dsty,
		    plane, privGC->ops->CopyPlane);
}

static void
vmiPolyPoint (pDrawable, pGC, mode, npt, pptInit)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		mode;		/* Origin or Previous */
    int		npt;
    xPoint 	*pptInit;
{
    CALL_GC_OP(pGC, PolyPoint,
	/* (*PolyPoint) */ (pDrawable, pGC, mode, npt, pptInit));
}

static void
vmiPolylines (pDrawable, pGC, mode, npt, pptInit)
    DrawablePtr	  pDrawable;
    GCPtr   	  pGC;
    int	    	  mode;
    int	    	  npt;
    DDXPointPtr	  pptInit;
{
    CALL_GC_OP(pGC, Polylines,
	/* (*Polylines) */ (pDrawable, pGC, mode, npt, pptInit));
}

static void
vmiPolySegment(pDrawable, pGC, nseg, pSegs)
    DrawablePtr pDrawable;
    GCPtr 	pGC;
    int		nseg;
    xSegment	*pSegs;
{
    CALL_GC_OP(pGC, PolySegment,
	/* (*PolySegment) */ (pDrawable, pGC, nseg, pSegs));
}

static void
vmiPolyRectangle(pDrawable, pGC, nrects, pRects)
    DrawablePtr	pDrawable;
    GCPtr	pGC;
    int		nrects;
    xRectangle	*pRects;
{
    CALL_GC_OP(pGC, PolyRectangle,
	/* (*PolyRectangle) */ (pDrawable, pGC, nrects, pRects));
}

static void
vmiPolyArc(pDrawable, pGC, narcs, parcs)
    DrawablePtr	pDrawable;
    GCPtr	pGC;
    int		narcs;
    xArc	*parcs;
{
    CALL_GC_OP(pGC, PolyArc,
	/* (*PolyArc) */ (pDrawable, pGC, narcs, parcs));
}

static void
vmiFillPolygon(pDrawable, pGC, shape, mode, count, pPts)
    DrawablePtr		pDrawable;
    register GCPtr	pGC;
    int			shape, mode;
    register int	count;
    DDXPointPtr		pPts;
{
    CALL_GC_OP(pGC, FillPolygon,
	/* (*FillPolygon) */ (pDrawable, pGC, shape, mode, count, pPts));
}

static void
vmiPolyFillRect(pDrawable, pGC, nrectFill, prectInit)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		nrectFill; 	/* number of rectangles to fill */
    xRectangle	*prectInit;  	/* Pointer to first rectangle to fill */
{
    CALL_GC_OP(pGC, PolyFillRect,
	/* (*PolyFillRect) */ (pDrawable, pGC, nrectFill, prectInit));
}

static void
vmiPolyFillArc(pDrawable, pGC, narcs, parcs)
    DrawablePtr	pDrawable;
    GCPtr	pGC;
    int		narcs;
    xArc	*parcs;
{
    CALL_GC_OP(pGC, PolyFillArc,
	/* (*PolyFillArc) */ (pDrawable, pGC, narcs, parcs));
}

static int
vmiPolyText8(pDrawable, pGC, x, y, count, chars)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		x, y;
    int 	count;
    char	*chars;
{
    int result;
    CALL_GC_OP_AND_RETURN(pGC, PolyText8, result,
	/* (*PolyText8) */ (pDrawable, pGC, x, y, count, chars));
}

static int
vmiPolyText16(pDrawable, pGC, x, y, count, chars)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		x, y;
    int		count;
    unsigned short *chars;
{
    int result;
    CALL_GC_OP_AND_RETURN(pGC, PolyText16, result,
	/* (*PolyText16) */ (pDrawable, pGC, x, y, count, chars));
}

static void
vmiImageText8(pDrawable, pGC, x, y, count, chars)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		x, y;
    int		count;
    char	*chars;
{
    CALL_GC_OP(pGC, ImageText8,
	/* (*ImageText8) */ (pDrawable, pGC, x, y, count, chars));
}

static void
vmiImageText16(pDrawable, pGC, x, y, count, chars)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		x, y;
    int		count;
    unsigned short *chars;
{
    CALL_GC_OP(pGC, ImageText16,
	/* (*ImageText16) */ (pDrawable, pGC, x, y, count, chars));
}

static void
vmiImageGlyphBlt(pDrawable, pGC, x, y, nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC 		*pGC;
    int 	x, y;
    unsigned int nglyph;
    CharInfoPtr *ppci;		/* array of character info */
    pointer 	pglyphBase;	/* start of array of glyphs */
{
    CALL_GC_OP(pGC, ImageGlyphBlt,
	/* (*ImageGlyphBlt) */ (pDrawable, pGC, x, y,
				nglyph, ppci, pglyphBase));
}

static void
vmiPolyGlyphBlt(pDrawable, pGC, x, y, nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int 	x, y;
    unsigned int nglyph;
    CharInfoPtr *ppci;		/* array of character info */
    char 	*pglyphBase;	/* start of array of glyphs */
{
    CALL_GC_OP(pGC, PolyGlyphBlt,
	/* (*PolyGlyphBlt) */ (pDrawable, pGC, x, y, nglyph, ppci, pglyphBase));
}

static void
vmiPushPixels(pGC, pBitMap, pDst, w, h, x, y)
    GCPtr	pGC;
    PixmapPtr	pBitMap;
    DrawablePtr pDst;
    int		w, h, x, y;
{
    CALL_GC_OP(pGC, PushPixels,
	/* (*PushPixels) */ (pGC, pBitMap, pDst, w, h, x, y));
}

/*
 * I don't expect this routine will ever be called, as the GC
 * will have been unwrapped for the line drawing
 */
static void
vmiLineHelper()
{
    FatalError("miSpriteLineHelper called\n");
}
