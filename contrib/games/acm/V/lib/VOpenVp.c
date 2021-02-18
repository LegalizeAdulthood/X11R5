#include "Vlib.h"

Viewport *VOpenViewport (dpy, screen, win, cmap, unit, dist, scale, width, height)
Display	*dpy;
int	screen;
Window	win;
Colormap cmap;
double  unit;
double  dist;
double  scale;
int	width;
int	height; {

	Viewport *v;
	VPoint	 clip[4];
	int	 planes;

/*
 *  Allocate space for the Viewport structure
 */

	v = (Viewport *) Vmalloc (sizeof(Viewport));

/*
 * Calculate screen resolution in pixels per unit.
 */

	v->dist = dist;
	v->units = unit;

        v->xres = ((((double) DisplayWidth(dpy,screen)) * unit * 1000.0) /
            ((double) DisplayWidthMM(dpy,screen)));
        v->yres = ((((double) DisplayHeight(dpy,screen)) * unit * 1000.0) /
            ((double) DisplayHeightMM(dpy,screen)));
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

        v->clipPoly = VCreatePolygon (4, clip, (VColor *) 0);
        VGetPlanes (v->clipPoly);

/*
 *  Fill out the rest of the structure.
 */

	v->flags = VPPerspective | VPClip;
	v->dpy = dpy;
	v->cmap = cmap;
	v->screen = screen;
	v->win = win;
	VIdentMatrix (&v->eyeSpace);
	v->set = 0;
	v->width = width;
	v->height = height;

/*
 *  Is this a monochrome situation?  We also resort to monochrome on
 *  color systems that probably won't have enough color cell space
 *  to support double buffering.  We'll also use pixmaps if the
 *  global variable "usePixmaps" is non-zero.
 */

	planes = DisplayPlanes (dpy, screen);
	if (planes < PLANES*2+2 || usePixmaps != 0) {
		v->monoPixmap = XCreatePixmap (dpy, RootWindow(dpy, screen), width, height, planes);
		v->flags |= VPPixmap;
		if (planes == 1)
			v->flags |= VPMono;
		v->mask = AllPlanes;
	}

	return v;
}
