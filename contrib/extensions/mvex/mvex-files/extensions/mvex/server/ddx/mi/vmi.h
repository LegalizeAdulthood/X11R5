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
static char *rcsid=  "$Header: /n/lacey/usr/local/src/video/MVEX/server/ddx/mi/RCS/vmi.h,v 1.8 1991/09/26 21:28:06 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#ifndef _VMI_H_
#define _VMI_H_

#include "videostr.h"
#include "MVEXsite.h"

#define VisualDepthMask		(1L << 0)
#define VisualClassMask		(1L << 1)
#define VisualBitsPerRGBMask	(1L << 2)
#define VisualColormapSizeMask	(1L << 3)
#define VisualRedMask		(1L << 4)
#define VisualGreenMask		(1L << 5)
#define VisualBlueMask		(1L << 6)

#define ArrayLength(list)       ((sizeof list)/(sizeof list[0]))

extern VideoScreenRec *CreateVideoScreenRec();
extern VideoVisual *GetAllDepths();
extern void vmiScreenReset();
extern void vmiNoOp();
extern void vmiUnrealizeWindow();
extern void vmiClipNotify();
extern void vmiGetImage();
extern Bool vmiCreateGC();
extern RegionPtr vmiCopyArea();
extern RegionPtr vmiCopyPlane();
extern FID vmiOpenForRead();
Bool vmiClipAndScale();

typedef struct _cfbprivgc {
    /*
     * Saved vectors from the gc ops and funcs
     */
    GCOps       *ops;
    GCFuncs     *funcs;
} VmiPrivGC, *VmiPrivGCPtr;

extern int vmiGCPrivateIndex[];

#define VmiGetGCPrivate(pGC) \
        ((VmiPrivGCPtr) \
	  ((pGC)->devPrivates[ vmiGCPrivateIndex[(pGC)->pScreen->myNum] ].ptr))

#if CompiledWithCfb
void vmiGetCfbCompositeClip();
#endif

#endif /* _VMI_H_ */

