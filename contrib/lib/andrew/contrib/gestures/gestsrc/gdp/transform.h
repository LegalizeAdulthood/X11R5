/***********************************************************************

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/

typedef Matrix Transformation;

Transformation	AllocTran();	/* Only call which allocs mem */
Transformation	IdentityTran();	/* Transformation t;  sets t to identity */
Transformation	TranslateTran();/* Transformation t; int x, y; */
Transformation	RotateTran();	/* Transformation t; int degrees; */
Transformation	RotateCosSinTran(); /* Transformation t; double a*cosine,a*sine;*/
Transformation	ScaleTran(); 	/* Transformation t; double dilation; */

void	ApplyTran(); 		/* int x, y; Transformation t; int *xp, *yp; */
#define	ComposeTran(r, a, b) 	/* Transformation r, a, b;  r = a o b */ \
			(MatrixMultiply((a), (b), (r)), (r))
double	TransScale();		/* Transformation t; */

