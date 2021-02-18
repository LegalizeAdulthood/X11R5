#include "Vlib.h"

/*
 *  Transform a 3-D point in the world system into viewable
 *  coordinates.  The function returns 1 if the x,y information is
 *  displayable (probably displayable, that is).
 */

int VWorldToScreen (v, p, x, y)
Viewport	*v;
VPoint		*p;
int		*x, *y; {

	VPoint	eyept;

	VTransform (p, &(v->eyeSpace), &eyept);
	return VEyeToScreen (v, &eyept, x, y);
}
