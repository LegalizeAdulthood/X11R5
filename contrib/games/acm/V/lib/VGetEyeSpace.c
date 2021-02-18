#include "Vlib.h"
#include <math.h>

void VGetEyeSpace (v, EyePt, CntrInt, up)
Viewport *v;
VPoint	EyePt, CntrInt, up; {

	VMatrix	Mtx, es;
	VPoint	C1, C2;
	double	Hypotenuse, h1, CosA, SinA;

/*
 *  Calculate the eye space transformation matrix
 *
 *  First, orient the Z axis towards the center of interest.
 */

	VIdentMatrix (&(v->eyeSpace));
	v->eyeSpace.m[0][3] = -EyePt.x;
	v->eyeSpace.m[1][3] = -EyePt.y;
	v->eyeSpace.m[2][3] = -EyePt.z;
	VTransform(&CntrInt, &(v->eyeSpace), &C1);

	VIdentMatrix (&Mtx);
	Hypotenuse = sqrt(C1.x * C1.x + C1.y * C1.y);
	if (Hypotenuse > 0.0) {
		CosA = C1.y / Hypotenuse;
		SinA = C1.x / Hypotenuse;
		Mtx.m[0][0] = CosA;
		Mtx.m[1][0] = SinA;
		Mtx.m[0][1] = -SinA;
		Mtx.m[1][1] = CosA;
		es = v->eyeSpace;
		VMatrixMult(&es, &Mtx, &(v->eyeSpace));
	}

	VTransform(&CntrInt, &(v->eyeSpace), &C2);
	VIdentMatrix (&Mtx);
	Hypotenuse = sqrt(C2.y * C2.y + C2.z * C2.z);
	if (Hypotenuse > 0.0) {
		CosA = C2.y / Hypotenuse;
		SinA = -C2.z / Hypotenuse;
		Mtx.m[1][1] = CosA;
		Mtx.m[2][1] = SinA;
		Mtx.m[1][2] = -SinA;
		Mtx.m[2][2] = CosA;
		es = v->eyeSpace;
		VMatrixMult(&es, &Mtx, &(v->eyeSpace));
	}

/*
 *  Orient the y axis towards "up". Swap y and z axes.
 */

	VTransform (&up, &(v->eyeSpace), &C2);
	VIdentMatrix (&Mtx);
	h1 = sqrt (C2.y * C2.y + C2.z * C2.z);
	Hypotenuse = sqrt(C2.x * C2.x + h1 * h1);
	if (Hypotenuse > 0.0) {
		CosA = h1 / Hypotenuse;
		SinA = C2.x / Hypotenuse;
		if (C2.z < 0.0) {
			CosA = -CosA;
		}
		Mtx.m[0][0] = CosA;
		Mtx.m[2][0] = SinA;
		Mtx.m[0][2] = -SinA;
		Mtx.m[2][2] = CosA;
		es = v->eyeSpace;
		VMatrixMult(&es, &Mtx, &(v->eyeSpace));
	}

	VIdentMatrix (&Mtx);
	Mtx.m[1][1] = 0.0;
	Mtx.m[2][1] = 1.0;
	Mtx.m[1][2] = 1.0;
	Mtx.m[2][2] = 0.0;
	es = v->eyeSpace;
	VMatrixMult(&es, &Mtx, &(v->eyeSpace));

}
