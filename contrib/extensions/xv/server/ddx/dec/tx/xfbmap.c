
#include "servermd.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "dix.h"
#include "cfb.h"

int cfb32ScreenPrivateIndex;
static unsigned long cfb32DrawGeneration = 0;


void
cfbDrawClose(pScreen)
    ScreenPtr pScreen;
{
    xfree(pScreen->devPrivate);
}

/*
Initialize cfb drawing code.  Xfb cannot call cfbScreenInit() directly
because it does too much.
Inputs:
    cfbWindowPrivateIndex = window index to be used by cfb code, this
        is ignored because dec/cfb does not use window privates
    cfbGCPrivateIndex = GC index to be used by cfb code
*/
Bool
cfbDrawInit(pScreen, pbits, xsize, ysize, width, WindowPrivateIndex,
    GCPrivateIndex)
    register ScreenPtr pScreen;
    pointer pbits;              /* pointer to screen bitmap */
    int xsize, ysize;           /* in pixels */
    int width;                  /* pixel width of frame buffer */
    int WindowPrivateIndex;
    int GCPrivateIndex;
{
    PixmapPtr pPixmap;

    if (!AllocateGCPrivate(pScreen, GCPrivateIndex, sizeof(cfbPrivGC)))
        return FALSE;

    pPixmap = (PixmapPtr ) xalloc(sizeof(PixmapRec));
    if (!pPixmap)
        return FALSE;
    pPixmap->drawable.type = DRAWABLE_PIXMAP;
    pPixmap->drawable.depth = 8;
    pPixmap->drawable.pScreen = pScreen;
    pPixmap->drawable.serialNumber = 0;
    pPixmap->drawable.x = 0;
    pPixmap->drawable.y = 0;
    pPixmap->drawable.width = xsize;
    pPixmap->drawable.height = ysize;
    pPixmap->refcnt = 1;
    pPixmap->devPrivate.ptr = pbits;
    pPixmap->devKind = PixmapBytePad(width, 8);
    pScreen->devPrivate = (pointer)pPixmap;

    return (TRUE);
}


/*
This allows the xfbBankSwitch module to change the address of the screen
pixmap
*/
void
xfbDrawSetFb24(pScreen, fb24)
    ScreenPtr pScreen;
    pointer fb24;
{
    ((PixmapPtr) pScreen->devPrivates[cfb32ScreenPrivateIndex].ptr)
	->devPrivate.ptr = fb24;
}

void
cfb32DrawClose(pScreen)
    ScreenPtr pScreen;
{
    xfree(pScreen->devPrivates[cfb32ScreenPrivateIndex].ptr);
}

/*
Initialize cfb32 drawing code.  Xfb cannot call cfb32ScreenInit() directly
because it does too much.  

Inputs:
    cfbWindowPrivateIndex = window index to be used by cfb32 code
    cfbGCPrivateIndex = GC index to be used by cfb32 code

Bugs: Need to get rid of window private someday.  Rotated pixmaps don't
    make sense for 1 PPW.
*/
Bool
cfb32DrawInit(pScreen, fb24, xsize, ysize, width, WindowPrivateIndex,
    GCPrivateIndex)
    ScreenPtr pScreen;
    pointer fb24;		/* address of 24-bit framebuffer */
    int xsize, ysize;		/* in pixels */
    int width;			/* pixel width of frame buffer */
    int WindowPrivateIndex;
    int GCPrivateIndex;
{
    PixmapPtr pPixmap;

    if (cfb32DrawGeneration != serverGeneration) {
	if ((cfb32ScreenPrivateIndex = AllocateScreenPrivateIndex()) < 0) {
	    return (FALSE);
	}
	cfb32DrawGeneration = serverGeneration;
    }

    if (!AllocateWindowPrivate(pScreen, WindowPrivateIndex,
			       sizeof(cfbPrivWin)) ||
	!AllocateGCPrivate(pScreen, GCPrivateIndex, sizeof(cfbPrivGC))) {
	return FALSE;
    }

    /* store 24-bit frame buffer pixmap in screen private: */
    pPixmap = (PixmapPtr) xalloc(sizeof(PixmapRec));
    if (!pPixmap)
	return FALSE;
    pPixmap->drawable.type = DRAWABLE_PIXMAP;
    pPixmap->drawable.class = 0;
    pPixmap->drawable.pScreen = pScreen;
    pPixmap->drawable.depth = 24;
    pPixmap->drawable.bitsPerPixel = 32;
    pPixmap->drawable.id = 0;
    pPixmap->drawable.serialNumber = NEXT_SERIAL_NUMBER;
    pPixmap->drawable.x = 0;
    pPixmap->drawable.y = 0;
    pPixmap->drawable.width = xsize;
    pPixmap->drawable.height = ysize;
    pPixmap->devKind = PixmapBytePad(width, 24);
    pPixmap->refcnt = 1;
    pPixmap->devPrivate.ptr = fb24;
    pScreen->devPrivates[cfb32ScreenPrivateIndex].ptr = (pointer) pPixmap;

    return (TRUE);
}
