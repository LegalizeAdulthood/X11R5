/* $Header: /users/x11devel/x11r5/mit/server/ddx/rasterops/RCS/roInit.c,v 1.4 1991/09/26 18:14:20 toddb Exp $ */
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
#include    "sun.h"
#include    "roStruct.h"
#include    "servermd.h"
#include    "pixmapstr.h"

/*
 * R5 needs pixmapstr.h and mfb.h, but R4 does not.
 * However, R4 mfb.h does not have protection against multiple includes.
 */
#ifndef DoRop
#include    "mfb.h"
#endif

#include    <pixrect/cg8var.h>
#include    <pixrect/pr_planegroups.h>

extern caddr_t sunMapFb();

static RoScreenPrivatePtr
	    *PrivatePtrs;
int	    RasterOpsScreenIndex = -1;	/* for screen privates */
static int  RasterOpsGeneration = -1;	/* to keep track of server resets */
int	    RasterOpsScreenMask = 0;	/* one bit per rasterops screen */

/*
 * roProbe -- see if the rasterOps sparcCard is present
 */
Bool
roProbe (pScreenInfo, scrIndex, fbNum, fd, fbType, pOverlay, pEnable, pCg8)
    ScreenInfo	  *pScreenInfo;	/* The screenInfo struct */
    int	    	  scrIndex;    	/* The index of pScreen in the ScreenInfo */
    int	    	  fbNum;    	/* Index into the sunFbData array */
    int		  fd;
    struct fbtype *fbType;
    u_char        **pOverlay;
    u_char        **pEnable;
    u_long        **pCg8;
/*
 * DESCRIPTION
 *	Map in all the framebuffers available and allocate some private space
 *	to record them.  This is kind of a hack, because we really can't
 *	store the data in the screen structure yet, because it isn't allocated.
 *	So we just hang onto the data in the 'PrivatePtrs' structure and copy
 *	those pointers to the screen devPrivates in roCreate().
 *
 * RETURNS
 *	FALSE if allocation failure, else TRUE.
 */
{
    size_t      length;
    RoScreenPrivatePtr pPriv;
    RoFrameBuffer *pFb;
    Pipio_Fb_Info roFbInfo;
    Pipio_Emulation emulation;
    int i, errorReturn;
    size_t offset, total;
    caddr_t base;

    /*
     * Set the emulation to enable all possible frame buffers
     * We do this by getting the current values and change all
     * '2' values (meaning available) to '1' values (meaning enabled).
     */
    if (ioctl(fd, PIPIO_G_EMULATION_MODE, &emulation) < 0) {
	Error("can't get rasterops emulation mode\n");
	return FALSE;
    }
    /*
     * XXX Hack!  Apparently, the 8-bit plane group causes the ioctls
     * that manipulate the colormap to work incorrectly
     */
    emulation.plane_groups[ PIXPG_8BIT_COLOR ] = 0;

    for (i = 0; i < FB_NPGS; i++)
	if (emulation.plane_groups[ i ])
	    emulation.plane_groups[ i ] = 1;
    if (ioctl(fd, PIPIO_S_EMULATION_MODE, &emulation) < 0) {
	Error("can't set rasterops emulation mode\n");
	return FALSE;
    }

    /*
     * Turn off the pip if it is still running
     */
    (void) roStopPip(fd, &errorReturn);
    roWaitNotRunning(fd);

    /*
     * Now get the frame buffer info
     */
    if (ioctl(fd, PIPIO_G_FB_INFO, &roFbInfo) < 0) {
	Error("can't get rasterops fb info\n");
	return FALSE;
    }

    base = sunMapFb(fd, roFbInfo.total_mmap_size, "cg8c");
    if (base == (caddr_t)NULL)
	return FALSE;
    RasterOpsScreenMask |= 1<<scrIndex;	/* mark this screen valid */

    /*
     * the sun init code calls the probe routines once for each screen...
     * but not on server reset
     */
    PrivatePtrs = (RoScreenPrivatePtr *)
	xrealloc(PrivatePtrs, sizeof(RoScreenPrivatePtr) * (scrIndex + 1));

    pPriv = (RoScreenPrivatePtr) xalloc(sizeof(RoScreenPrivate));
    bzero(pPriv, sizeof(RoScreenPrivate));
    PrivatePtrs[ scrIndex ] = pPriv;

    pPriv->fd = fd;
    for (i = 0; i < roFbInfo.frame_buffer_count; i++) {
	offset = roFbInfo.fb_descriptions[ i ].mmap_offset;
	switch (roFbInfo.fb_descriptions[ i ].group) {
	case PIXPG_8BIT_COLOR:     pFb = &pPriv->cg4;       break;
	case PIXPG_OVERLAY_ENABLE: pFb = &pPriv->enable;    break;
	case PIXPG_OVERLAY:        pFb = &pPriv->overlay;   break;
	case PIXPG_24BIT_COLOR:    pFb = &pPriv->cg8;       break;
	case PIXPG_VIDEO_ENABLE:   pFb = &pPriv->vidEnable; break;
	}
	pFb->u.byteFb = (u_char *)(base + offset);
	pFb->width = roFbInfo.fb_descriptions[ i ].width;
	pFb->height = roFbInfo.fb_descriptions[ i ].height;
	pFb->linebytes = roFbInfo.fb_descriptions[ i ].linebytes;

	if (pFb == &pPriv->cg8) {
	    /*
	     * Set the fb type to be correct.
	     */
	    fbType->fb_type = PIXPG_24BIT_COLOR;
	    fbType->fb_height = pFb->height;
	    fbType->fb_width = pFb->width;
	    fbType->fb_depth = roFbInfo.fb_descriptions[ i ].depth;
	    fbType->fb_cmsize = 24;	/* XXX this isn't available! */
	    fbType->fb_size = roFbInfo.fb_descriptions[ i ].mmap_size;
	}
    }

    *pOverlay = pPriv->overlay.u.byteFb;
    *pEnable = pPriv->enable.u.byteFb;
    *pCg8 = pPriv->cg8.u.longFb;

    /*
     * The intent is for these vectors to enable and disable the enable planes,
     * and is based on the current usage in mfb:
     *	MFBSOLIDFILLAREA(pDraw, nbox, pbox, alu, nop)
     *      DrawablePtr pDraw;
     *      int nbox;
     *      BoxPtr pbox;
     *      int alu;		(ignored)
     *      PixmapPtr nop;	(ignored)
     *
     * Hence the usage is always
     *		(*func)(pDraw, nbox, pbox);
     * If this changes, you will have to write a translate routine or
     * change all the code that calls these vectors.  At least the
     * former will be easy to implement.
     */
    pPriv->enableArea = mfbSolidWhiteArea;
    pPriv->disableArea = mfbSolidBlackArea;

    return TRUE;
}

/*
 * roCreate -- allocate screen privates and store data there
 */
void
roCreate(pScreenInfo, scrIndex)
    ScreenInfo	  *pScreenInfo;	/* The screenInfo struct */
    int		  scrIndex;
/*
 * DESCRIPTION
 *	Initialize the RasterOpsScreenIndex.
 *      Copy the data created in roProbe() into the devPrivates array.
 *
 * RETURNS
 *      Void
 *
 */
{
    int privi, errorReturn;
    RoScreenPrivatePtr pPriv;
    ScreenPtr pScr = pScreenInfo->screens[ scrIndex ];

    if (serverGeneration != RasterOpsGeneration) {
	RasterOpsScreenIndex = AllocateScreenPrivateIndex();
	RasterOpsGeneration = serverGeneration;
    }

    privi = RasterOpsScreenIndex;
    pPriv = PrivatePtrs[ scrIndex ];
    pScr->devPrivates[ privi ].ptr = (pointer)pPriv;

#ifdef VIDEOX
    if (pPriv->vidEnable.u.byteFb) {
	pPriv->vidEnablePix =
	    roCreateBitmap(pScr,
			   pPriv->vidEnable.width,
			   pPriv->vidEnable.height,
			   pPriv->vidEnable.linebytes,
			   (pointer)pPriv->vidEnable.u.byteFb);
	pPriv->vidEnabled = (*pScr->RegionCreate)(NULL, 0);

	/*
	 * Disable all of the video
	 */
	if (pPriv->vidEnablePix) {
	    BoxRec screen;

	    screen.x1 = pPriv->vidEnablePix->drawable.x;
	    screen.y1 = pPriv->vidEnablePix->drawable.y;
	    screen.x2 = screen.x1 + pPriv->vidEnable.width;
	    screen.y2 = screen.y1 + pPriv->vidEnable.height;
	    (*pPriv->disableArea)(pPriv->vidEnablePix, 1, &screen, 0, 0);
	}
    }

    pPriv->port = -1; /* this will force it to be set */
#endif

    /*
     * Currently the overlay is unused in dual display systems and
     * is used as a second screen on single display systems.  We allocate
     * a pixmap here only to make sure the overlay has been cleared from
     * garbage scribbled by the console.  Later we will offer it as a
     * second visual depth.
     */
    if (pPriv->overlay.u.byteFb) {
	pPriv->overlayPix =
	    roCreateBitmap(pScr,
			   pPriv->overlay.width,
			   pPriv->overlay.height,
			   pPriv->overlay.linebytes,
			   (pointer)pPriv->overlay.u.byteFb);
	pPriv->disabled = (*pScr->RegionCreate)(NULL, 0);

	/*
	 * Disable the overlay.
	 */
	if (pPriv->overlayPix) {
	    BoxRec screen;

	    screen.x1 = pPriv->overlayPix->drawable.x;
	    screen.y1 = pPriv->overlayPix->drawable.y;
	    screen.x2 = screen.x1 + pPriv->overlay.width;
	    screen.y2 = screen.y1 + pPriv->overlay.height;
	    (*pPriv->disableArea)(pPriv->overlayPix, 1, &screen, 0, 0);
	}
    }

    if (pPriv->enable.u.byteFb) {
	pPriv->enablePix =
	    roCreateBitmap(pScr,
			   pPriv->enable.width,
			   pPriv->enable.height,
			   pPriv->enable.linebytes,
			   (pointer)pPriv->enable.u.byteFb);
	pPriv->disabled = (*pScr->RegionCreate)(NULL, 0);

	/*
	 * Disable the overlay.
	 */
	if (pPriv->enablePix) {
	    BoxRec screen;

	    screen.x1 = pPriv->enablePix->drawable.x;
	    screen.y1 = pPriv->enablePix->drawable.y;
	    screen.x2 = screen.x1 + pPriv->enable.width;
	    screen.y2 = screen.y1 + pPriv->enable.height;
	    (*pPriv->disableArea)(pPriv->enablePix, 1, &screen, 0, 0);
	}
    }
}

/*
 * roCreateBitmap -- create a bitmap (pixmap) given a pointer to a framebuffer
 */
PixmapPtr
roCreateBitmap(pScreen, width, height, lineBytes, fb)
    ScreenPtr pScreen;
    int width, height;
    int lineBytes;
    pointer fb;
/*
 * DESCRIPTION
 *      Create mfb pixmaps for a framebuffer that can be twiddled by the
 *	mfb routines.
 *
 * RETURNS
 *      PixmapPtr
 *
 */
{
    PixmapPtr pPixmap;

    /*
     * Start with a 1x1 pixmap and ignore the bitmap storage, assigning
     * the devPrivate.ptr to the framebuffer.
     */
    if ((pPixmap = mfbCreatePixmap (pScreen, 1, 1, 1)) == NULL)
	return (NULL);
    pPixmap->devPrivate.ptr = fb;
    pPixmap->drawable.width = width;
    pPixmap->drawable.height = height;
    pPixmap->devKind = PixmapBytePad(width, 1);	/* XXX debug */
    pPixmap->devKind = lineBytes;
    return (pPixmap);
}
