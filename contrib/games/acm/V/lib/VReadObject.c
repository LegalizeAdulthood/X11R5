#include "Vlib.h"
#include <string.h>

VObject *VReadObject(f)
FILE *f; {

	short	I, J, vertex, vertices, NumPts, NumPolys;
	float	x, y, z;
	char	str[64], name[128];
	VPoint	*tmpPts, pts[VmaxVP];
	VPolygon **polygons;
	VObject	*object;

	fscanf(f, "%s\n", name);
	fscanf(f, "%hd", &NumPts);
	fscanf(f, "%hd", &NumPolys);

	tmpPts = (VPoint *) Vmalloc (NumPts * sizeof(VPoint));
	polygons = (VPolygon **) Vmalloc (NumPolys * sizeof(VPolygon *));

	for (I=0; I<NumPts; ++I) {
		fscanf(f, "%hd", &J);
		fscanf(f, "%f", &x);
		fscanf(f, "%f", &y);
		fscanf(f, "%f", &z);
		tmpPts[I].x = x;
		tmpPts[I].y = y;
		tmpPts[I].z = z;
	}

	for (I=0; I<NumPolys; ++I) {
		fscanf(f, "%s %hd", str, &(vertices));
		for (J=0; J<vertices; ++J) {
			fscanf(f, "%hd", &(vertex));
			pts[J] = tmpPts[vertex - 1];
		}
		polygons[I] = VCreatePolygon (vertices, pts, VAllocColor(str));
	}

	object = (VObject *) Vmalloc (sizeof(VObject));
	object->name = strdup (name);
	object->numPolys = NumPolys;
	object->polygon = polygons;

	free ((char *) tmpPts);
	return ferror(f) ? (VObject *) 0 : object;
}
