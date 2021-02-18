#include "Vlib.h"
#include <string.h>

int VWriteObject(f, obj)
FILE    *f;
VObject *obj; {

	int	 i, j, k, points;
	VPolygon **q;
	VPoint	 *p;

/*
 *  Total the number of vertices in all of the object's polygons
 */

	points = 0;
	q = obj->polygon;
	for (i=0; i<obj->numPolys; ++i) {
		points += q[i]->numVtces;
	}

/*
 *  Print the header
 */

	fprintf (f, "%s\n%d %d\n", obj->name, points, obj->numPolys);

/*
 *  Print the point list
 */

	k = 1;
	q = obj->polygon;
	for (i=0; i<obj->numPolys; ++i) {
		for ((j=0, p=q[i]->vertex); j<q[i]->numVtces; (++p, ++j)) {
			fprintf(f, "%d %g %g %g\n", k, p->x, p->y, p->z);
			++k;
		}
	}

/*
 *  Print the polygon list
 */

	k = 1;
	q = obj->polygon;
	for (i=0; i<obj->numPolys; ++i) {
		fprintf(f, "%s %d", q[i]->color->color_name, q[i]->numVtces);
		for (j=0; j<q[i]->numVtces; ++j)
			fprintf(f, " %d", k++);
		fprintf (f, "\n");
	}

	return ferror(f) ? -1 : 0;
}
