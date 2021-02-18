#include "Vlib.h"

void VCrossProd (a, b, r)
VPoint	*a, *b, *r; {

	r->x = a->y * b->z - a->z * b->y;
	r->y = a->z * b->x - a->x * b->z;
	r->z = a->x * b->y - a->y * b->x;
}
