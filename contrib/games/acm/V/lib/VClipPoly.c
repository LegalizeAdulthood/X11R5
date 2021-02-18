#include "Vlib.h"

VPolygon * _VClipPolygon (poly, clipPlane)
VPolygon *poly;
VPoint	 *clipPlane; {

    register	int j, lastj, numPts = 0, clipped = 0;
    double	d1, d2, a;
    VPoint	tmpPoint[64];
    VPolygon	*p;

	if (poly->numVtces > 0) {

	    lastj = poly->numVtces-1;
	    d1 = VDotProd(&(poly->vertex[poly->numVtces-1]), clipPlane);
	    numPts = 0;

/*
 *  Examine each vertex and determine if it is inside or outside of the
 *  specified clipping plane.
 */

	    for (j=0; j<poly->numVtces; ++j) {

/* Leading vertex inside? */

		if (d1 > 0.0)
		    tmpPoint[numPts++] = poly->vertex[lastj];

		d2 = VDotProd(&(poly->vertex[j]), clipPlane);

/* Does the edge straddle the window? If so, add a vertex on the window */

		if (d1 * d2 < 0.0) {
			clipped = 1;
			a = d1 / (d1 - d2);
			tmpPoint[numPts].x = a * poly->vertex[j].x +
				(1.0 - a) * poly->vertex[lastj].x;
			tmpPoint[numPts].y = a * poly->vertex[j].y +
				(1.0 - a) * poly->vertex[lastj].y;
			tmpPoint[numPts++].z = a * poly->vertex[j].z +
				(1.0 - a) * poly->vertex[lastj].z;
		}

		lastj = j;
		d1 = d2;
	    }
	}

/*
 *  If the polygon was completely out of bounds, delete this polygon.
 */

    if (numPts == 0) {
	p = (VPolygon *) NULL;
	VDestroyPolygon (poly);
#ifdef DEBUG
	fprintf (stderr, "VClipPolygon: polygon outside area of interest\n");
#endif
    }

/*
 *  If we did any clipping, return the clipped polygon.
 */

    else if (clipped != 0) {
	p = VCreatePolygon(numPts, &tmpPoint[0], poly->color);
#ifdef DEBUG
	fprintf (stderr, "VClipPolygon: Polygon has been clipped:\n");
	fprintf (stderr, "Before Clipping:\n");
	VPrintPolygon (stderr, poly);
	fprintf (stderr, "\nAfter Clipping:\n\n");
	VPrintPolygon (stderr, p);
#endif
	VDestroyPolygon (poly);
    }
    else
	p = poly;

    return p;
}

VPolygon * VClipPolygon (poly, clipPoly)
VPolygon *poly, *clipPoly; {

    int 	i;
    VPolygon 	*p = poly;

/*
 *  Clip against each clipping plane supplied, one at a time.
 */

    for (i=0; i<clipPoly->numVtces; ++i) {

	if (p == (VPolygon *) NULL)
	    break;

	p = _VClipPolygon (p, &(clipPoly->vertex[i]));

    }

    return p;
}
