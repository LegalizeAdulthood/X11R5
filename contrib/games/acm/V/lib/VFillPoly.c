#include "Vlib.h"

#define COLLAPSEUNUSEDPOINTS

void VFillPolygon(v, win, gc, poly)
Viewport *v;
Window	 win;
GC	 gc;
VPolygon *poly; {

	VPoint 	TmpPt, *p;
	XPoint	xpt[VmaxVP], *lastpt;
	int	i, k;
	Drawable d;

	d = (v->flags & VPPixmap) ? (Drawable) v->monoPixmap : (Drawable) win;

	if (poly == (VPolygon *) NULL)
		return;

	k = 0;
	lastpt = &xpt[0];
	for ((i=0, p=poly->vertex); i<poly->numVtces; (++i, ++p)) {
	    if (v->flags & VPPerspective) {
		TmpPt.x = v->Middl.x + v->Scale.x * p->x / p->z;
		TmpPt.y = v->Middl.y - v->Scale.y * p->y / p->z;
	    }
	    else {
		TmpPt.x = v->Middl.x + v->Scale.x * p->x;
		TmpPt.y = v->Middl.y - v->Scale.y * p->y;
	    }

#ifdef COLLAPSEUNUSEDPOINTS
	    xpt[k].x = TmpPt.x + 0.5;
	    xpt[k].y = TmpPt.y + 0.5;

	    if (k == 0 || !(xpt[k].x == lastpt->x && xpt[k].y == lastpt->y))
		lastpt = &xpt[k++];

#else
	    xpt[k].x = TmpPt.x + 0.5;
	    xpt[k].y = TmpPt.y + 0.5;
	    ++k;
#endif

	}

	if (k > 0) {
#ifdef COLLAPSEUNUSEDPOINTS
		if (k == 1)
			XDrawPoint (v->dpy, d, gc, xpt[0].x, xpt[0].y);
		else if (k == 2)
			XDrawLines (v->dpy, d, gc, xpt, 2, CoordModeOrigin);
		else
#endif
		XFillPolygon (v->dpy, d, gc, xpt, k, Nonconvex, CoordModeOrigin);
	}

}
