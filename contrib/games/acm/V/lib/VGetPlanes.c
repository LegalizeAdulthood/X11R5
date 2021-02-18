#include "Vlib.h"

VPolygon *VGetPlanes (poly)
VPolygon *poly; {

	VPoint	tmp[64], *p;
	int	i, lasti;

	lasti = poly->numVtces - 1;
	p = poly->vertex;

	for (i=0; i<poly->numVtces; ++i) {
	    tmp[i].x = p->y * poly->vertex[lasti].z - p->z *
		poly->vertex[lasti].y;
	    tmp[i].y = p->z * poly->vertex[lasti].x - p->x *
		poly->vertex[lasti].z;
	    tmp[i].z = p->x * poly->vertex[lasti].y - p->y *
		poly->vertex[lasti].x;
	    lasti = i;
	    p++;
	}

	for (i=0; i<poly->numVtces; ++i)
	    poly->vertex[i] = tmp[i];

	return poly;
}
