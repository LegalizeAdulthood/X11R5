#include "Vlib.h"

double VDotProd (a, b)
VPoint	*a, *b; {

	return a->x * b->x + a->y * b->y + a->z * b->z;
}
