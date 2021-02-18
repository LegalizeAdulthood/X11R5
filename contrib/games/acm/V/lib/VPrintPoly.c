#include "Vlib.h"

void	VPrintPolygon (file, p)
FILE	*file;
VPolygon *p; {

	int	i;
	char	*nullPoly = "*** Null Polygon ***\n";

	if (p == (VPolygon *) NULL)
		fprintf (file, nullPoly);
	else {
		if (p->numVtces == 0) {
			fprintf (file, nullPoly);
			return;
		}

		fprintf (file, "%d vertices:\n", p->numVtces);

		for (i=0; i<p->numVtces; ++i)
			fprintf(file, "%9.6g %9.6g %9.6g\n", p->vertex[i].x,
				p->vertex[i].y, p->vertex[i].z);
	}

	return;
}
