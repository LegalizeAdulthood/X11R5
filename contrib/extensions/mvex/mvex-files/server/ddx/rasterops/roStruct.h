/* $Header: /usr/local/src/x11r5.mvex/RCS/roStruct.h,v 1.2 1991/09/12 23:24:35 toddb Exp $ */
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
#ifndef ROSTRUCT_H
#define ROSTRUCT_H

#include    <errno.h>
extern int  errno;
#include    <sys/param.h>
#include    <sys/types.h>
#include    <sys/time.h>
#include    <sys/file.h>
#include    <sys/fcntl.h>
#include    <sys/signal.h>
#include    <sun/fbio.h>

#ifdef VIDEOX
#include    <sunwindow/rect.h>
#include    <pixrect/cg8var.h>
#endif

typedef struct _roframebuffer {
    union {
	u_char *byteFb;
	u_long *longFb;
    } u;
    short width;	    /* from Fb_Description */
    short height;	    /* from Fb_Description */
    u_int linebytes;	    /* from Fb_Description */
} RoFrameBuffer;
    
typedef struct _roscreenprivate {
    int fd;		    /* from device open */
    RoFrameBuffer overlay;  /* bit-per-pixel memory */
    RoFrameBuffer enable;   /* enable plane */
    RoFrameBuffer cg8;      /* word-per-pixel memory */
    RoFrameBuffer vidEnable;/* video enable plane */
    RoFrameBuffer cg4;      /* 8-plane fb access */
    PixmapPtr enablePix;
    PixmapPtr overlayPix;
    RegionPtr disabled;	    /* region of overlay that has been disabled */

    /*
     * These vectors assume the usage
     * MFBSOLIDFILLAREA(pDraw, nbox, pbox, alu, nop)
     *      DrawablePtr pDraw;
     *      int nbox;
     *      BoxPtr pbox;
     *      int alu;            (ignored)
     *      PixmapPtr nop;      (ignored)
     *
     * Hence: (*enableArea)(pDraw, nbox, pbox);
     */

    void (*enableArea)();    /* vector to enable an enable plane */
    void (*disableArea)();   /* vector to ... well, you know */
#ifdef VIDEOX
    PixmapPtr vidEnablePix;
    RegionPtr vidEnabled;  /* region of video that has been enabled */
    struct rect srcBox;
    struct rect dstBox;
    int saturation;
    int contrast;
    int hue;
    int bright;
    int port;
#endif
} RoScreenPrivate, *RoScreenPrivatePtr;

extern int RasterOpsScreenIndex;	/* for screen privates */
extern int RasterOpsScreenMask;		/* one bit per rasterops screen */

#define RoGetScreenPrivate(pscr) \
	((RoScreenPrivatePtr)((pscr)->devPrivates[ RasterOpsScreenIndex ].ptr))

/*
 * XXX this macro gets the composite clip for the validated gc... which
 * just so happens to be the cfbPrivGC.  This is ok, but in a more pure
 * world, this ddx implementation would have its own validataGC vector
 * that would put together its own composite clip.
 */
#define GetCompositClip(pGC)	\
  ((cfbPrivGCPtr) ((pGC)->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip

extern int cfbGCPrivateIndex;

extern Bool roProbe ();
extern void roCreate();
extern PixmapPtr roCreateBitmap();

/*
 * Device dependent routines
 */
extern Bool roWaitActive();
extern Bool roIoctl();
extern Bool roSetSaturationDac();
extern Bool roSetContrastDac();
extern Bool roSetHueDac();
extern Bool roSetBrightnessDac();
extern Bool roSetInputRectangle();
extern Bool roSetOutputRectangle();
extern Bool roSetPort();
extern Bool roStartPip();
extern Bool roPipRunning();
extern Bool roStopPip();

#endif /* ROSTRUCT_H */
