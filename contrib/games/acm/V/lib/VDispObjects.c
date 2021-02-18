#include "Vlib.h"

void VDisplayObjects() {

	Point3 	TmpPt;
	int	I, J;
	XPoint	xpt[MaxPts];

	for (I=0; I<NumPolys; ++I) {

		for (J=0; J<Polygons[I].PolyVtces; ++J) {
			Transform(&Points[Vertices[Polygons[I].Start + J]], &EyeSpace, &TmpPt);
			MakeDisplayable(&TmpPt);
			xpt[J].x = (int) TmpPt.X;
			xpt[J].y = (int) TmpPt.Y;
		}
/*		xpt[J] = xpt[0];
		XDrawLines (dpy, win, gc, &xpt, J+1, CoordModeOrigin); */
		XFillPolygon (dpy, win, curGC, &xpt, J, Nonconvex, CoordModeOrigin);
	}

}
