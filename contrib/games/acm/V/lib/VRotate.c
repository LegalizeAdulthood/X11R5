#include "Vlib.h"
#include <math.h>

VMatrix *VRotate (Mt1, operation, angle)
VMatrix * Mt1;
int	operation;
double	angle; {

	VMatrix	m, s;

	VIdentMatrix (&m);

	switch (operation) {
	case XRotation:
		m.m[1][1] = m.m[2][2] = cos(angle);
		m.m[2][1] = sin(angle);
		m.m[1][2] = - m.m[2][1];
		break;
	case YRotation: 
		m.m[0][0] = m.m[2][2] = cos(angle);
		m.m[2][0] = sin(angle);
		m.m[0][2] = - m.m[2][0];
		break;
	case ZRotation:
		m.m[0][0] = m.m[1][1] = cos(angle);
		m.m[1][0] = sin(angle);
		m.m[0][1] = - m.m[1][0];
		break;
	}

	s = *Mt1;

	VMatrixMult (&s, &m, Mt1);
	return Mt1;
}

VMatrix *VTranslatePoint (Mt, loc)
VMatrix *Mt;
VPoint  loc; {

	Mt->m[0][3] = Mt->m[0][3] + loc.x;
	Mt->m[1][3] = Mt->m[1][3] + loc.y;
	Mt->m[2][3] = Mt->m[2][3] + loc.z;
	return Mt;

}

VMatrix *VTranslate (Mt, x, y ,z)
VMatrix *Mt;
double  x, y, z; {

	Mt->m[0][3] = Mt->m[0][3] + x;
	Mt->m[1][3] = Mt->m[1][3] + y;
	Mt->m[2][3] = Mt->m[2][3] + z;
	return Mt;
}
