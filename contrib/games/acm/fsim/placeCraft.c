/*
 *	acm : an aerial combat simulator for X
 *
 *	Written by Riley Rainey,  riley@mips.com
 *
 *	Permission to use, copy, modify and distribute (without charge) this
 *	software, documentation, images, etc. is granted, provided that this 
 *	comment and the author's name is retained.
 *
 */
 
#include "pm.h"

void placeCraft (obj, poly, cnt)
craft	 *obj;
VPolygon **poly;
int	 *cnt; {

	int	 i, j, k, n;
	VPoint	 *q, tmp;
	VMatrix	 mtx;
	VPolygon **p;

	j = *cnt;

	VIdentMatrix (&mtx);
	if (obj->curRoll != 0.0)
		VRotate (&mtx, XRotation, obj->curRoll);
	if (obj->curPitch != 0.0)
		VRotate (&mtx, YRotation, - obj->curPitch);
	if (obj->curHeading != 0.0)
		VRotate (&mtx, ZRotation, obj->curHeading);
	VTranslatePoint (&mtx, obj->Sg);

	if (obj->cinfo->placeProc != NULL) {
		(*obj->cinfo->placeProc)(obj, &mtx, poly, cnt);
		return;
	}

	n = obj->cinfo->object->numPolys;
	p = obj->cinfo->object->polygon;
	for (i=0; i<n; ++i) {

		poly[j] = VCopyPolygon(p[i]);
		for ((k=0, q=poly[j]->vertex); k<poly[j]->numVtces; (++k, ++q)) {
			VTransform(q, &mtx, &tmp);
			*q = tmp;
		}
		++j;
	}

	*cnt = j;
}
