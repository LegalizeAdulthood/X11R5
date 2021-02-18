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
 
#include <Vlib.h>

int	transpose (m, r)
VMatrix *m, *r; {

	int	i, j;

	for (i=0; i<4; ++i)
		for (j=0; j<4; ++j)
			r->m[i][j] = m->m[j][i];
	return 0;
}
