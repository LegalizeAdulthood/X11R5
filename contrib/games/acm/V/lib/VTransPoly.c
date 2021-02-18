#include "Vlib.h"

VPolygon *VTransformPolygon (poly, m)
VPolygon *poly;
VMatrix *m; {

	int	i;
	VPoint	tmp;

	for (i=0; i<poly->numVtces; ++i) {
		VTransform (&(poly->vertex[i]), m, &tmp);
		poly->vertex[i] = tmp;
	}

	return poly;
}
