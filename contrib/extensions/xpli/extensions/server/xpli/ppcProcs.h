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
		Copyright IBM Corporation 1988

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

/* "@(#)ppcprocs.h	3.1 88/09/22 09:36:07" */
/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcProcs.h,v 9.9 89/06/08 20:21:20 jeff Exp Locker: jeff $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcProcs.h,v $ */

#if !defined(NO_FUNCTION_PROTOTYPES)

/* ppcClip.o */
extern void ppcDestroyClip(
	GCPtr pGC );

extern void ppcChangeClip(
	GCPtr pGC,
	unsigned int type,
	pointer pvalue,
	int nrects );

extern void ppcCopyClip(
	GCPtr pgcDst, 
	GCPtr pgcSrc );

/* ppcPolyRec.o */
extern void ppcPolyRectangle(
	DrawablePtr pDraw,
	GCPtr pGC,
	int nrects,
	xRectangle pRects );

/* ppcFont.o */
extern Bool ppcRealizeFont(
	ScreenPtr pScreen,
	FontPtr pFont );

extern Bool ppcUnrealizeFont(
	ScreenPtr pScreen,
	FontPtr pFont );

/* ppcCacheTx.o */
extern int ppcCachedImageText8(
	DrawablePtr pDraw,
	GCPtr pGC,
	int x,
	int y,
	int count,
	char *chars );

extern int ppcCachedImageText16(
	DrawablePtr pDraw,
	GCPtr pGC,
	int x,
	int y,
	int count,
	unsigned short *chars );

extern int ppcCachedPolyText8(
	DrawablePtr pDraw,
	GCPtr pGC,
	int x,
	int y,
	int count,
	char *chars );

extern int ppcCachedPolyText16(
	DrawablePtr pDraw,
	GCPtr pGC,
	int x,
	int y,
	int count,
	unsigned short *chars );

extern void ppcCachedImageGlyphBlt(
	DrawablePtr pDrawable,
	GCPtr pGC,
	int x,
	int y,
	unsigned int nglyph,
	CharInfoPtr *ppci,
	ppcBMInfoPtr *bms,
	unsigned char *pglyphBase );

extern void ppcCachedPolyGlyphBlt(
	DrawablePtr pDrawable,
	GCPtr pGC,
	int x,
	int y,
	unsigned int nglyph,
	CharInfoPtr *ppci,
	ppcBMInfoPtr *bms,
	unsigned char *pglyphBase );

/* ppcArc.o */
extern void ppcPolyZeroArc(
	WindowPtr	pWindow,
	GCPtr		pGC,
	int		narcs,
	xArc		*parcs );

extern void ppcPolyFillArc(
	WindowPtr	pWindow,
	GCPtr		pGC,
	int		narcs,
	xArc		*parcs );

/* ppcArea.o */
extern void ppcAreaFill(
	WindowPtr pWin,
	int nboxes,
	BoxPtr pBox,
	GCPtr pGC ) ;

/* ppcBitMap.o */
extern void ppcQuickBlt(
	int *psrcBase,
	int *pdstBase,
	int xSrc,
	int ySrc,
	int xDst,
	int yDst,
	int w,
	int h,
	int wSrc,
	int wDst ) ;

extern void ppcRotBitmapRight(
	PixmapPtr pPix,
	int rw ) ;

extern void ppcRotBitmapDown(
	PixmapPtr pPix,
	int rh ) ;

/* ppcBStore.o */
extern void ppcSaveAreas(
	PixmapPtr pPixmap,
	RegionPtr prgnSave,
	int xorg,
	int yorg ) ;

extern void ppcRestoreAreas(
	PixmapPtr pPixmap,
	RegionPtr prgnRestore,
	int xorg,
	int yorg ) ;

/* ppcCmap.o */
/* ppc Color Routines */

extern Bool ppcCreateColormapNoop(
	ColormapPtr pmap );

extern void ppcStoreColors(
	ColormapPtr pmap,
	int ndef,
	xColorItem *pdefs ) ;

extern int ppcListInstalledColormaps(
	ScreenPtr pScreen,
	Colormap *pCmapList ) ;

extern void ppcInstallColormap(
	ColormapPtr pColormap ) ;

extern void ppcUninstallColormap(
	ColormapPtr pColormap ) ;

extern void ppcUninstallDefaultColormap(
	ScreenPtr pScreen ) ;

/* ppcCpArea.o */
extern RegionPtr ppcCopyArea(
	DrawablePtr pSrcDrawable,
	DrawablePtr pDstDrawable,
	GC *pGC,
	int srcx,
	int srcy,
	int width,
	int height,
	int dstx,
	int dsty ) ;

/* ppcCReduce.o */
extern void ppcReduceGeneral(
	int alu,
	unsigned long int pm,
	unsigned long int fg,
	unsigned long int bg,
	int fillStyle,
	int drawableDepth,
	ppcReducedRrop *returnLoc ) ;

extern void ppcReduceColorRrop(
	ppcReducedRrop *initialLoc,
	int drawableDepth,
	ppcReducedRrop *returnLoc ) ;

extern void ppcGetReducedColorRrop(
	GC *pGC,
	int drawableDepth,
	ppcReducedRrop *returnLoc ) ;

/* ppc Cursor Routines */
/* ppcCurs.o */
extern Bool ppcRealizeCursor(
	ScreenPtr pScr,
	CursorPtr pCurs ) ;

extern Bool ppcUnrealizeCursor(
	ScreenPtr pScr,
	CursorPtr pCurs ) ;

/* ppcDepth.o */
extern Bool ppcDepthOK(
	DrawablePtr pDraw,
	int depth ) ;

/* ppcFillRct.o */
extern void ppcPolyFillRect(
	DrawablePtr pDrawable,
	GCPtr pGC,
	int nrectFill,
	xRectangle *prectInit ) ;

/* Various Fill-Span Routines */
/* ppcPixmapFS.o */
void ppcSolidWindowFS(
	DrawablePtr pDrawable,
	GCPtr pGC,
	int nInit,
	DDXPointPtr pptInit,
	int *pwidthInit,
	int fSorted ) ;
void ppcStippleWindowFS(
	DrawablePtr pDrawable,
	GC *pGC,
	int nInit,
	DDXPointPtr pptInit,
	int *pwidthInit,
	int fSorted ) ;
void ppcOpStippleWindowFS(
	DrawablePtr pDrawable,
	GC *pGC,
	int nInit,
	DDXPointPtr pptInit,
	int *pwidthInit,
	int fSorted ) ;
void ppcTileWindowFS(
	DrawablePtr pDrawable,
	GC *pGC,
	int nInit,
	DDXPointPtr pptInit,
	int *pwidthInit,
	int fSorted ) ;
 
/* ppcPixmapFS.o */
void ppcSolidPixmapFS(
	DrawablePtr pDrawable,
	GCPtr pGC,
	int nInit,
	DDXPointPtr pptInit,
	int *pwidthInit,
	int fSorted ) ;
void ppcStipplePixmapFS(
	DrawablePtr pDrawable,
	GC *pGC,
	int nInit,
	DDXPointPtr pptInit,
	int *pwidthInit,
	int fSorted ) ;
void ppcOpStipplePixmapFS(
	DrawablePtr pDrawable,
	GC *pGC,
	int nInit,
	DDXPointPtr pptInit,
	int *pwidthInit,
	int fSorted ) ;
void ppcTilePixmapFS(
	DrawablePtr pDrawable,
	GC *pGC,
	int nInit,
	DDXPointPtr pptInit,
	int *pwidthInit,
	int fSorted ) ;

/* ppcGenFS.o */
void ppcSolidGenFS(
	DrawablePtr pDrawable,
	GCPtr pGC,
	int nInit,
	DDXPointPtr pptInit,
	int *pwidthInit,
	int fSorted ) ;
void ppcStippleGenFS(
	DrawablePtr pDrawable,
	GC *pGC,
	int nInit,
	DDXPointPtr pptInit,
	int *pwidthInit,
	int fSorted ) ;
void ppcOpStippleGenFS(
	DrawablePtr pDrawable,
	GC *pGC,
	int nInit,
	DDXPointPtr pptInit,
	int *pwidthInit,
	int fSorted ) ;
void ppcTileGenFS(
	DrawablePtr pDrawable,
	GC *pGC,
	int nInit,
	DDXPointPtr pptInit,
	int *pwidthInit,
	int fSorted ) ;

void ppcFillSpans(
	DrawablePtr pDrawable,
	GC *pGC,
	int nInit,
	DDXPointPtr pptInit,
	int *pwidthInit,
	int fSorted ) ;

/* ppcGBlt.o */
extern void ppcImageGlyphBlt(
	DrawablePtr pDrawable,
	GC *pGC,
	int x,
	int y,
	unsigned int nglyph,
	CharInfoPtr *ppci,
	unsigned char *pglyphBase ) ;

extern void ppcPolyGlyphBlt(
	DrawablePtr pDrawable,
	GC *pGC,
	int x,
	int y,
	unsigned int nglyph,
	CharInfoPtr *ppci,
	unsigned char *pglyphBase ) ;

/* ppcGC.o */
extern Bool ppcCreateGC(
	GCPtr pGC ) ;

extern void ppcDestroyGC(
	GC *pGC ) ;

extern void ppcValidateGC(
	GCPtr pGC,
	Mask changes,
	DrawablePtr pDrawable ) ;

/* ppcGetSp.o */
extern void ppcGetSpans(
	DrawablePtr pDrawable,
	int wMax,
	DDXPointPtr ppt,
	int *pwidth,
	int nspans,
	unsigned int *pdstStart ) ;

/* ppcImg.o */
extern void ppcGetImage(
	DrawablePtr pDraw,
	int sx,
	int sy,
	int w,
	int h,
	unsigned int format,
	unsigned long int planeMask,
	pointer pdstLine ) ;

/* ppcLine.o */
extern void ppcScrnZeroLine(
	DrawablePtr pDrawable,
	GCPtr pGC,
	int mode,
	int npt,
	DDXPointPtr pptInit ) ;

extern void ppcScrnZeroDash(
	DrawablePtr pDrawable,
	GCPtr pGC,
	int mode,
	int npt,
	DDXPointPtr pptInit ) ;

extern void ppcScrnZeroSegs(
	DrawablePtr	 pDrawable,
	GCPtr		 pGC,
	int		 nseg,
	xSegment	*psgInit ) ;

/* Low Level Emulation Routines -- ppcStipple() & ppcOpaqueStipple() */
/* ppcStip.o */
extern void ppcStipple(
	PixmapPtr pStipple,
	unsigned long int fg,
	int merge,
	unsigned long int planes,
	int x,
	int y,
	int w,
	int h,
	int xSrc,
	int ySrc ) ;

/* ppcOpStip.o */
extern void ppcOpaqueStipple(
	PixmapPtr pStipple,
	unsigned long int fg,
	unsigned long int bg,
	int alu,
	unsigned long int planes,
	int x,
	int y,
	int w,
	int h,
	int xSrc,
	int ySrc ) ;

/* ppcPixmap.o */
extern PixmapPtr ppcCreatePixmap(
	ScreenPtr pScreen,
	int width,
	int height,
	int depth ) ;

extern PixmapPtr ppcCopyPixmap(
	PixmapPtr pSrc ) ;

extern void ppcPadPixmap(
	PixmapPtr pPixmap ) ;

extern void ppcRotatePixmap(
	PixmapPtr pPix,
	int rw ) ;

/* ppcPntWin.o */
extern void ppcPaintWindow(
	WindowPtr pWin,
	RegionPtr pRegion,
	int what ) ;
extern void ppcPaintWindowSolid(
	WindowPtr pWin,
	RegionPtr pRegion,
	int what ) ;

extern void ppcPaintWindowTile(
	WindowPtr pWin,
	RegionPtr pRegion,
	int what ) ;

/* ppcPolyPnt.o */
extern void ppcPolyPoint(
	DrawablePtr pDrawable,
	GCPtr pGC,
	int mode,
	int npt,
	xPoint *pptInit ) ;

/* ppcPushPxl.o */
extern void ppcPushPixels(
	GCPtr pGC,
	PixmapPtr pBitMap,
	DrawablePtr pDrawable,
	int dx,
	int dy,
	int xOrg,
	int yOrg ) ;

/* ppcQuery.o */
extern void ppcQueryBestSize(
	int class,
	short *pwidth,
	short *pheight ) ;

/* ppcRepArea.o */
extern void ppcReplicateArea(
	int x,
	int y,
	int planeMask,
	int goalWidth,
	int goalHeight,
	int currentHoriz,
	int currentVert,
	ScreenPtr pScrn ) ;

/* ppcRot.o */
extern void ppcRotZ8mapUp(
	PixmapPtr pSource,
	PixmapPtr pDest,
	int shift ) ;

extern void ppcRotZ8mapLeft(
	PixmapPtr pSource,
	PixmapPtr pDest,
	int shift ) ;

extern void ppcClipZ8Pixmap(
	PixmapPtr pSource,
	PixmapPtr pDest ) ;

extern PixmapPtr ppcClipBitmap(
	PixmapPtr pStipple,
	int w,
	int h ) ;

extern void ppcRotBitmapUp(
	PixmapPtr pSource,
	PixmapPtr pDest,
	int shift ) ;

extern void ppcRotBitmapLeft(
	PixmapPtr pSource,
	PixmapPtr pDest,
	int shift ) ;

extern void ppcClipZ1Pixmap(
	PixmapPtr pSource,
	PixmapPtr pDest ) ;

/* ppcRslvC.o */
extern void ppcResolveColor(
	unsigned short (const *pRed),
	unsigned short (const *pGreen),
	unsigned short (const *pBlue),
	const VisualPtr pVisual ) ;

/* ppcSetSp.o */
extern void ppcSetSpans(
	const DrawablePtr pDrawable,
	const GCPtr pGC,
	unsigned char *psrc,
	DDXPointPtr ppt,
	int *pwidth,
	int nspans,
	int fSorted ) ;

/* ppcTile.o */
extern void ppcTileRect(
	PixmapPtr pTile,
	const int alu,
	const unsigned long int planes,
	int x,
	int y,
	int w,
	int h,
	int xSrc,
	int ySrc ) ;

/* ppcWindow.o */
extern void ppcCopyWindow(
	WindowPtr pWin,
	DDXPointRec ptOldOrg,
	RegionPtr prgnSrc ) ;

extern void ppcCopyWindowForXYhardware(
	WindowPtr pWin,
	DDXPointRec ptOldOrg,
	RegionPtr prgnSrc ) ;

extern Bool ppcChangeWindowAttributes(
	WindowPtr pWin,
	int mask ) ;

extern Bool ppcPositionWindow(
	WindowPtr pWin,
	int x,
	int y ) ;

extern Bool ppcUnrealizeWindow(
	WindowPtr pWindow,
	int x,
	int y ) ;

extern Bool ppcRealizeWindow(
	WindowPtr pWindow ) ;

extern Bool ppcDestroyWindow(
	WindowPtr pWin ) ;

extern Bool ppcCreateWindow(
	WindowPtr pWin ) ;

extern Bool ppcCreateWindowForXYhardware(
	WindowPtr pWin ) ;

#else /* NO_FUNCTION_PROTOTYPES */

/* ppcClip.o */
extern void ppcDestroyClip() ;
extern void ppcChangeClip() ;
extern void ppcCopyClip() ;
/* ppcPolyRec.o */
extern void ppcPolyRectangle() ;
/* ppcFont.o */
extern Bool ppcRealizeFont() ;
extern Bool ppcUnrealizeFont() ;
/* ppcCacheTx.o */
extern int ppcCachedImageText8() ;
extern int ppcCachedImageText16() ;
extern int ppcCachedPolyText8() ;
extern int ppcCachedPolyText16() ;
extern void ppcCachedImageGlyphBlt() ;
extern void ppcCachedPolyGlyphBlt() ;
/* ppcArc.o */
extern void ppcPolyZeroArc() ;
extern void ppcPolyFillArc() ;
/* ppcArea.o */
extern void ppcAreaFill() ;
/* ppcBitMap.o */
extern void ppcQuickBlt() ;
extern void ppcRotBitmapRight() ;
extern void ppcRotBitmapDown() ;
/* ppcBStore.o */
extern void ppcSaveAreas() ;
extern void ppcRestoreAreas() ;
/* ppcCmap.o */
extern Bool ppcCreateColormapNoop() ;
extern ColormapPtr ppcGetStaticColormap() ;
extern void ppcStoreColors() ;
extern void ppcDefineDefaultColormapColors() ;
extern int ppcListInstalledColormaps() ;
extern void ppcInstallColormap() ;
extern void ppcUninstallColormap() ;
extern void ppcAllocBlackAndWhitePixels() ;
extern unsigned long int ppcFindColor() ;
extern void ppcUninstallDefaultColormap() ;
/* ppcCpArea.o */
extern RegionPtr ppcCopyArea() ;
/* ppcCReduce.o */
extern void ppcReduceGeneral() ;
extern void ppcReduceColorRrop() ;
extern void ppcGetReducedColorRrop() ;
/* ppcCurs.o */
extern Bool ppcRealizeCursor() ;
extern Bool ppcUnrealizeCursor() ;
/* ppcDepth.o */
extern Bool ppcDepthOK() ;
/* ppcFillRct.o */
extern void ppcPolyFillRect() ;

/* ppcPixmapFS.o */
void ppcSolidWindowFS() ;
void ppcStippleWindowFS() ;
void ppcOpStippleWindowFS() ;
void ppcTileWindowFS() ;
 
/* ppcPixmapFS.o */
void ppcSolidPixmapFS() ;
void ppcStipplePixmapFS() ;
void ppcOpStipplePixmapFS() ;
void ppcTilePixmapFS() ;
 
/* ppcGenFS.o */
void ppcSolidGenFS() ;
void ppcStippleGenFS() ;
void ppcOpStippleGenFS() ;
void ppcTileGenFS() ;
void ppcFillSpans() ;

/* ppcGBlt.o */
extern void ppcImageGlyphBlt() ;
extern void ppcPolyGlyphBlt() ;
/* ppcGC.o */
extern Bool ppcCreateGC() ;
extern void ppcDestroyGC() ;
extern void ppcValidateGC() ;
/* ppcGetSp.o */
extern void ppcGetSpans() ;
/* ppcImg.o */
extern void ppcGetImage() ;
/* ppcLine.o */
extern void ppcScrnZeroLine() ;
extern void ppcScrnZeroDash() ;
extern void ppcScrnZeroSegs() ;
/* ppcStip.o */
extern void ppcStipple() ;
/* ppcOpStip.o */
extern void ppcOpaqueStipple() ;
/* ppcPixmap.o */
extern PixmapPtr ppcCreatePixmap() ;
extern PixmapPtr ppcCopyPixmap() ;
extern void ppcPadPixmap() ;
extern void ppcRotatePixmap() ;
/* ppcPntWin.o */
extern void ppcPaintWindow() ;
extern void ppcPaintWindowSolid() ;
extern void ppcPaintWindowTile() ;
/* ppcPolyPnt.o */
extern void ppcPolyPoint() ;
/* ppcPushPxl.o */
extern void ppcPushPixels() ;
/* ppcQuery.o */
extern void ppcQueryBestSize() ;
/* ppcRepArea.o */
extern void ppcReplicateArea() ;
/* ppcRot.o */
extern void ppcRotZ8mapUp() ;
extern void ppcRotZ8mapLeft() ;
extern void ppcClipZ8Pixmap() ;
extern PixmapPtr ppcClipBitmap() ;
extern void ppcRotBitmapUp() ;
extern void ppcRotBitmapLeft() ;
extern void ppcClipZ1Pixmap() ;
/* ppcRslvC.o */
extern void ppcResolveColor() ;
/* ppcSetSp.o */
extern void ppcSetSpans() ;
/* ppcTile.o */
extern void ppcTileRect() ;
/* ppcWindow.o */
extern void ppcCopyWindow() ;
extern void ppcCopyWindowForXYhardware() ;
extern Bool ppcChangeWindowAttributes() ;
extern Bool ppcPositionWindow() ;
extern Bool ppcUnrealizeWindow() ;
extern Bool ppcRealizeWindow() ;
extern Bool ppcDestroyWindow() ;
extern Bool ppcCreateWindow() ;
extern Bool ppcCreateWindowForXYhardware() ;

#endif /* NO_FUNCTION_PROTOTYPES */
