#include "Vlib.h"

void VExposeBuffer (v, gc)
Viewport *v;
GC	gc; {

/*
 * this is one area where more-than-one window per display gets hairy
 * -- this call really exposes the drawings buffered in all windows
 * associated with a given display.  the calling sequence needs to be
 * changed to reflect this.
 */

    if (v->flags & VPMono) {
	XCopyArea (v->dpy, v->monoPixmap, v->win, gc,
		0, 0, v->width, v->height, 0, 0);
	XSetForeground (v->dpy, gc, WhitePixel(v->dpy, v->screen));
	XFillRectangle (v->dpy, v->monoPixmap, gc, 0, 0, v->width, v->height);
	XSetForeground (v->dpy, gc, BlackPixel(v->dpy, v->screen));
    }
    else if (v->flags & VPPixmap) {
	XCopyArea (v->dpy, v->monoPixmap, v->win, gc,
		0, 0, v->width, v->height, 0, 0);
    }
    else if (v->set == 0) {
	v->set = 1;
	XStoreColors (v->dpy, v->cmap, v->aColor, v->colors*v->colors);
	v->pixel = &(v->bPixel[0]);
	XSetPlaneMask (v->dpy, gc, v->bMask);
	v->mask = v->bMask;
	/*
	for ((p=VColorList, q = &(v->bPixel[1])); p != (VColor *) 0; ++q) {
	    p->xcolor.pixel = *q;
	    p = p->next;
	}
	*/
    }
    else {
	v->set = 0;
	XStoreColors (v->dpy, v->cmap, v->bColor, v->colors*v->colors);
	v->pixel = &(v->aPixel[0]);
	XSetPlaneMask (v->dpy, gc, v->aMask);
	v->mask = v->aMask;
	/*
	for ((p=VColorList, q = &(v->aPixel[1])); p != (VColor *) 0; ++q) {
	    p->xcolor.pixel = *q;
	    p = p->next;
	}
	*/
    }

    XFlush (v->dpy);
}
