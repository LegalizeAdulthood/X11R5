#include "Vlib.h"

void VIdentMatrix (Mtx)
VMatrix *Mtx;
{
	short I, J;

	for (I=0; I<4; ++I)
		for (J=0; J<4; ++J)
			if (I == J)
				(*Mtx).m[I][J] = 1.0;
			else
				(*Mtx).m[I][J] = 0.0;

}
