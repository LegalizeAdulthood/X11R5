#include "Vlib.h"

void VDrawPolygon(v, win, gc, poly)
Viewport *v;
Window	 win;
GC	 gc;
VPolygon *poly; {

	VPoint 	TmpPt, *p;
	XPoint	xpt[VmaxVP];
	int	i;
	Drawable d;

	d = (v->flags & VPPixmap) ? (Drawable) v->monoPixmap : (Drawable) win;

	if (poly == (VPolygon *) NULL)
		return;

	for ((i=0, p=poly->vertex); i<poly->numVtces; (++i, ++p)) {
	    if (v->flags & VPPerspective) {
		TmpPt.x = v->Middl.x + v->Scale.x * p->x / p->z;
		TmpPt.y = v->Middl.y - v->Scale.y * p->y / p->z;
	    }
	    else {
		TmpPt.x = v->Middl.x + v->Scale.x * p->x;
		TmpPt.y = v->Middl.y - v->Scale.y * p->y;
	    }
	    xpt[i].x = TmpPt.x + 0.5;
	    xpt[i].y = TmpPt.y + 0.5;
	}

	if (i > 0) {
		xpt[i] = xpt[0];
		XDrawLines (v->dpy, d, gc, xpt, i+1, CoordModeOrigin);
	}

}
