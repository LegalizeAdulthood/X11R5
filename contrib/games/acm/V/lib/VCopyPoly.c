#include "Vlib.h"

VPolygon *VCopyPolygon (poly)
VPolygon *poly; {

	return VCreatePolygon (poly->numVtces, poly->vertex, poly->color);
}
