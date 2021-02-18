#include "Vlib.h"

void VResizeViewport (v, unit, dist, scale, width, height)
Viewport 	*v;
double  unit;
double  dist;
double  scale;
int	width;
int	height; {

	VPoint	 clip[4];

	v->width = width;
	v->height = height;

/*
 *  Allocate a new monochrome bitmap to buffer the displayed image.
 */

	if (v->flags & VPPixmap) {
		XFreePixmap (v->dpy, v->monoPixmap);
		v->monoPixmap = XCreatePixmap (v->dpy,
			RootWindow(v->dpy, v->screen), v->width, v->height,
			DisplayPlanes (v->dpy, v->screen));
	}

/*
 * Calculate screen resolution in pixels per unit.
 */

	v->dist = dist;
	v->units = unit;

/*
 *  Use that info to set scaling factors.
 */

        v->Scale.x = v->xres * dist * scale;
        v->Scale.y = v->yres * dist * scale;
        v->Scale.z = 1.0;

        v->Middl.x = (double) width / 2.0;
        v->Middl.y = (double) height / 2.0;
        v->Middl.z = 0.0;

/*
 *  Build the clipping planes for our view into the eye space.
 */

        clip[0].x = - width / v->xres / 2.0 / scale;
        clip[0].y = - height / v->yres / 2.0 / scale;
        clip[0].z = dist;
        clip[1].x = - width / v->xres / 2.0 / scale;
        clip[1].y = height / v->yres / 2.0 / scale;
        clip[1].z = dist;
        clip[2].x = width / v->xres / 2.0 / scale;
        clip[2].y = height / v->yres / 2.0 / scale;
        clip[2].z = dist;
        clip[3].x = width / v->xres / 2.0 / scale;
        clip[3].y = - height / v->yres / 2.0 / scale;
        clip[3].z = dist;

	VDestroyPolygon (v->clipPoly);
        v->clipPoly = VCreatePolygon (4, clip, (VColor *) 0);
        VGetPlanes (v->clipPoly);
}
