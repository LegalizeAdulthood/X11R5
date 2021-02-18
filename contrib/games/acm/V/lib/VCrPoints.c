#include "Vlib.h"

VPoint *VCreatePoints (numPts)
int	numPts; {

	return (VPoint *) Vmalloc (sizeof(VPoint) * numPts);
}
