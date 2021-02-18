#include "Vlib.h"

void VCloseViewport (v)
Viewport *v; {

	VDestroyPolygon (v->clipPoly);
	free ((char *) v);
}
