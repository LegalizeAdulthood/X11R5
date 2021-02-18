#include "../V/lib/Vlib.h"

main () {

	int	i, k;
	VObject *obj;
	VPolygon **poly;
	VPoint	*q, tmp;
	VMatrix mtx;

        if ((obj = VReadObject(stdin)) == (VObject *) NULL) {
                fprintf (stderr, "Error reading the object definition.\n");
                exit (1);
        }

/*
 *  Prepare a transformation matrix that:
 *	(a) swaps X and Y coordinates.
 *	(b) negates Z coordinate.
 */

	VIdentMatrix (&mtx);
	mtx.m[0][0] = 0.0;
	mtx.m[0][1] = 1.0;
	mtx.m[1][0] = 1.0;
	mtx.m[1][1] = 0.0;
	mtx.m[2][2] = -1.0;

	poly = obj->polygon;

        for (i=0; i<obj->numPolys; ++i) {
            for ((k=0, q=poly[i]->vertex); k<poly[i]->numVtces; (++k, ++q)) {
                    VTransform(q, &mtx, &tmp);
                    *q = tmp;
            }
        }

	VWriteObject (stdout, obj);
}
