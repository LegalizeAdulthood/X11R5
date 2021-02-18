#include "Vlib.h"

void VMatrixMult (Mt1, Mt2, R)
VMatrix *Mt1, *Mt2, *R; {

	register int I, J, K;
	register double x;

	for (I=0; I<4; ++I)
		for (J=0; J<4; ++J) {
			x = 0.0;
			for (K=0; K<4; ++K)
				x += Mt1->m[K][J] * Mt2->m[I][K];
			R->m[I][J] = x;
		}
}

#define ZEROFOURTH

void VMatrixMultByRank (Mt1, Mt2, R, rank)
VMatrix *Mt1, *Mt2, *R;
int rank; {

	register int I, J, K, r = rank;
	register double x;

	for (I=0; I<r; ++I)
		for (J=0; J<r; ++J) {
			x = 0.0;
			for (K=0; K<r; ++K)
				x += Mt1->m[K][J] * Mt2->m[I][K];
			R->m[I][J] = x;
		}

#ifdef ZEROFOURTH
	R->m[0][3] = R->m[1][3] = R->m[2][3] = 0.0;
	R->m[3][0] = R->m[3][1] = R->m[3][2] = 0.0;
	R->m[3][3] = 1.0;
#endif
}
