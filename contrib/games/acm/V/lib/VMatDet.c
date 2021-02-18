#include "Vlib.h"

double VMatrixDeterminant(m)
VMatrix *m; {

	double	det;

	det = m->m[0][0] * m->m[1][1] * m->m[2][2] * m->m[3][3] +
	      m->m[1][0] * m->m[2][1] * m->m[3][2] * m->m[0][3] +
	      m->m[2][0] * m->m[3][1] * m->m[0][2] * m->m[1][3] +
	      m->m[3][0] * m->m[0][1] * m->m[1][2] * m->m[2][3] -
	      m->m[0][3] * m->m[1][2] * m->m[2][1] * m->m[3][0] -
	      m->m[1][3] * m->m[2][2] * m->m[3][1] * m->m[0][0] -
	      m->m[2][3] * m->m[3][2] * m->m[0][1] * m->m[1][0] -
	      m->m[3][3] * m->m[0][2] * m->m[1][1] * m->m[2][0];

	return det;
}
