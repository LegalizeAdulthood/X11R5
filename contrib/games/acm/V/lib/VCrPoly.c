#include "Vlib.h"

VPolygon *VCreatePolygon (numVtces, vert, color)
int	numVtces;
VPoint	*vert;
VColor	*color; {

	VPolygon *p;

	p = (VPolygon *) Vmalloc (sizeof (VPolygon));

	p->numVtces = numVtces;
	p->vertex = (VPoint *) Vmalloc (sizeof (VPoint) * numVtces);
	memcpy ((char *) p->vertex, (char *) vert, sizeof(VPoint) * numVtces);
	p->color = color;

	return p;
}
