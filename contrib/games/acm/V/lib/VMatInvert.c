#include "Vlib.h"

#define mod(a)	(a < 0 ? a + 3 : a % 3)

VMatrix *VMatrixInvert(s, d)
VMatrix *s;
VMatrix *d; {

	register int	i, j;
	register double det;

	det = VMatrixDeterminant (s);

	for (i=0; i<4; ++i)
		for (j=0; j<4; ++j)
			d->m[j][i] =(s->m[mod(j+1)][mod(i+1)] *
				     s->m[mod(j+2)][mod(i+2)] -
				     s->m[mod(j-1)][mod(i+1)] *
				     s->m[mod(j-2)][mod(i+2)] ) / det;

	return d;
}
